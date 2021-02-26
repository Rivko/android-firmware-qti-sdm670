/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define DEBUG_LOG

#include "VAMEngineBase.h"
#include <string>

VAMEngineBase::VAMEngineBase(const vaapi_source_info &sourceInfo):
    _sourceInfo(sourceInfo),
    _lastPutFramePTS(0),
    _lastProcessedPTS(0),
    _targetFPS(15),
    _isDependsOnOtherEvent(false),
    _isStop(true),
    _isWorkerThreadRunning(false),
    MAX_FRAME_QUEUE_SIZE(5) {
   VAM_KPI_GET_MASK();
   VAM_KPI_BASE("ConstructVamEngineBase");
}

VAMEngineBase::~VAMEngineBase() {
    VAM_KPI_BASE("DestroyVamEngineBase");
}

int VAMEngineBase::addRule(vaapi_rule *pVAAPIRule) {
    if (pVAAPIRule == nullptr) {
        return VAM_NULLPTR;
    }

    std::lock_guard<std::mutex> m(_configMutex);
    return _addRule(pVAAPIRule);
}

int VAMEngineBase::rmRule(std::string configID) {
    std::lock_guard<std::mutex> m(_configMutex);
    return _rmRule(configID);
}

int VAMEngineBase::run() {
    VAM_KPI_BASE("VamEngineBaseRun");
    std::lock_guard<std::mutex> m(_frameDataListMutex);

    if (_isWorkerThreadRunning) {
        return VAM_OK;
    }

    _isStop = false;
    _workerThreadID = std::thread(VAMEngineBase::_workerThread, this);
    _isWorkerThreadRunning = true;

    return VAM_OK;
}

int VAMEngineBase::stop() {
    VAM_KPI_BASE("VamEngineBaseStop");
    if (_isWorkerThreadRunning == false) {
        return VAM_OK;
    }

    _isStop = true;
    _putFrameCond.notify_one();
    _workerThreadID.join();
    _isWorkerThreadRunning = false;

    return VAM_OK;
}

int VAMEngineBase::putFrame(vaapi_frame_info *pFrameInfo) {
    VAM_KPI_DETAIL("VamEngineBasePutFrame");
    if (pFrameInfo == nullptr) {
        return VAM_NULLPTR;
    }
    std::lock_guard<std::mutex> m(_frameDataListMutex);

    if (_isStop) {
        return VAM_ENGINE_IS_NOT_RUNNING;
    }

    if (_frameDataList.size() > MAX_FRAME_QUEUE_SIZE) {
        return VAM_FRAME_QUEUE_FULL;
    }

    _frameDataList.push_back(*pFrameInfo);
    _putFrameCond.notify_one();

    return VAM_OK;
}

void VAMEngineBase::RegisterCBs(void *pVAManager,
                                EventCBFunc pEventCBFunc,
                                MetadataCBFunc pMetadataCBFunc,
                                FrameDoneCBFunc pFrameDoneCBFunc) {
    _pVAManager = pVAManager;
    _pEventCBFunc = pEventCBFunc;
    _pMetadataCBFunc = pMetadataCBFunc;
    _pFrameDoneCBFunc = pFrameDoneCBFunc;
}

int VAMEngineBase::_updateRule(vaapi_rule *pVAAPIRule) {
    std::lock_guard<std::mutex> m(_frameDataListMutex);
    return _addRule(pVAAPIRule);
}

bool VAMEngineBase::_isTimeToPutFrame(uint64_t newPTS) {
    if (newPTS - _lastProcessedPTS < 1000000000 / _targetFPS) {
        return false;
    }

    _lastProcessedPTS = newPTS;
    return true;
}

void VAMEngineBase::_workerThread(VAMEngineBase *_this) {
    VAM_KPI_BASE("VamEngineBaseWorkerThread");
    uint64_t totalProcessTimeInMS = 0;
    int processFrame = 0, maxProcessTimeInMS = 0;
    while (_this->_isStop == false) {
        vaapi_frame_info frameInfo;
        memset(&frameInfo, 0, sizeof(vaapi_frame_info));
        {
            std::unique_lock<std::mutex> m(_this->_frameDataListMutex);
            if (_this->_isStop == false && _this->_frameDataList.size() == 0) {
                // timeout: 20 sec
                std::cv_status ret = _this->_putFrameCond.wait_for(m,
                                            std::chrono::milliseconds(20000));
                if (ret == std::cv_status::timeout) {
                    VAM_LOGE("VAMEngineBase::putFrame error: TimeOut (1 sec)");
                    continue;
                }
            }

            if (_this->_isStop) {
                break;
            }

            if (_this->_frameDataList.size() <= 0) {
                continue;
            }

            frameInfo = _this->_frameDataList[0];
            _this->_frameDataList.pop_front();
        }

        // the event gets clear for every frame, always sent only once.
        _this->_eventList.clear();

        bool isUpdated = false;
        if (_this->_isTimeToPutFrame(frameInfo.time_stamp)) {
            std::lock_guard<std::mutex> m(_this->_configMutex);
            isUpdated = true;
            uint64_t startTime = getTimeNow();

            // object (metadata) only gets updated when processed
            _this->_objList.clear();

            _this->_workerFunc(&frameInfo);

            int processTimeInMS = static_cast<int>(getTimeNow() - startTime);
            processTimeInMS /= 1000;
            totalProcessTimeInMS += processTimeInMS;
            if (maxProcessTimeInMS < processTimeInMS) {
                maxProcessTimeInMS = processTimeInMS;
            }
            processFrame++;
        }

        // this we announce to released control of the input frame
        _this->_processCallback(&frameInfo, isUpdated);
    }

    VAM_LOGE("VAMEngineBase::_workerThread ended. \n"
             "average processed time per frame:%d ms, max:%d ms\n",
             (processFrame == 0) ? 0 : (int)totalProcessTimeInMS / processFrame,
             maxProcessTimeInMS);
}

int VAMEngineBase::_processCallback(vaapi_frame_info *pFrameInfo,
                                    bool isUpdated) {
    if (_pMetadataCBFunc == nullptr || pFrameInfo == nullptr) {
        return VAM_NULLPTR;
    }

    for (size_t i = 0; i < _eventList.size(); i++) {
        _pEventCBFunc(this, &_eventList[i], pFrameInfo, _pVAManager);
    }

    _pMetadataCBFunc(this,
                     pFrameInfo->time_stamp,
                     isUpdated,
                     _pVAManager,
                     &_objList);

    _pFrameDoneCBFunc(this, pFrameInfo, _pVAManager);
    return VAM_OK;
}

VAMEngineBase::EventCBFunc VAMEngineBase::_pEventCBFunc = nullptr;
VAMEngineBase::MetadataCBFunc VAMEngineBase::_pMetadataCBFunc = nullptr;
VAMEngineBase::FrameDoneCBFunc VAMEngineBase::_pFrameDoneCBFunc = nullptr;
void *VAMEngineBase::_pVAManager = nullptr;

