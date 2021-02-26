/*
 * Copyright (c) 2016-2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define DEBUG_LOG

#include "VAManager.h"
#include "VAMEngine_DynEngine.h"

#ifdef ENABLE_OMRON_FD
#include "VAMEngine_Omron.h"
#endif

#ifdef ENABLE_INTELLIVISION
#include "VAMEngine_Intellivision.h"
#endif

#ifdef ENABLE_SCVE
#include "VAMEngine_SCVE.h"
#endif

#ifdef LOG_LEVEL_KPI
volatile uint32_t kpi_debug_level = BASE_KPI_FLAG;
#endif

#include <dirent.h>
#include <iostream>
#include <limits>
#include <string>
#include <map>
#include <deque>
#include <list>
#include <vector>

#define ABS_TO_REL_COORD_COEF 10000

VAManager::VAManager():
    _isInited(false),
    _workerThreadStop(false),
    _areEnginedRunning(false),
    _lastAssignedID(0),
    _pEventCBFunc(nullptr),
    _pEventCBUsrData(nullptr),
    _pMetadataCBFunc(nullptr),
    _pMetadataCBUsrData(nullptr),
    _pSnapshotCBFunc(nullptr),
    _pSnapshotCBUsrData(nullptr),
    _pFrameProcessedCBFunc(nullptr),
    _pFrameProcessedCBUsrData(nullptr) {
    VAM_KPI_GET_MASK();
    VAM_KPI_BASE("ConstructVAManager");
    _objIDAssignment.resize(32);
    VAMEngineBase::RegisterCBs(this, &_eventCB, &_metadataCB, &_frameCB);
}

VAManager::~VAManager() {
    VAM_KPI_BASE("DestroyVAManager");
    destroy();
}

int VAManager::init(const struct vaapi_source_info *pSourceInfo,
                    const char *dyn_lib_path) {
    VAM_KPI_BASE("VamInit");
    if (pSourceInfo == nullptr) {
        return VAM_NULLPTR;
    }

    if (dyn_lib_path == nullptr || strlen(dyn_lib_path) == 0) {
        VAM_LOGE("VAManager Error: dynamia loading library path or "
                 "data path is not set");
        return VAM_ENGINE_DYLIB_PATH_NOT_SET;
    }

    if (_isInited) {
        VAM_LOGE("VAManager Error: VAM is inited & can't be re-inited twice."
                 " Try destroy()");
        return VAM_FAIL;
    }

    switch (pSourceInfo->img_format) {
        case vaapi_format_nv12:
        case vaapi_format_nv21:
        case vaapi_format_yv12:
        // case vaapi_format_yuv420:
            break;
        default:
            VAM_LOGE("VAManager Error: unsupported img format (%d).",
                     pSourceInfo->img_format);
            return VAM_IMG_FORMAT_NOT_SUPPORTED;
    }

    _sourceInfo = *pSourceInfo;
    _dynLibPath = dyn_lib_path;
    _dynDataPath = dyn_lib_path;

    int ret = _initEngineLibs();
    if (ret != VAM_OK) {
        return ret;
    }

    _workerThreadStop = false;
    _workerThreadId = std::thread(VAManager::_workerThread, this);

    return VAM_OK;
}

// need to be protected when call
VAMEngineBase *VAManager::_getEngineByEventType(vaapi_event_type type) {
    for (size_t idx = 0; idx < _engineList.size(); idx++) {
        if (_engineList[idx] && _engineList[idx]->isEventSupported(type)) {
            return _engineList[idx];
        }
    }

    return nullptr;
}

int VAManager::_initEngineLibs() {
    std::lock_guard<std::mutex> m(_engineListMutex);
    VAMEngineBase *v = nullptr;

    std::vector<std::string> libList;
    {
        DIR *pDir = opendir(_dynLibPath.c_str());
        if (pDir != nullptr) {
            struct dirent *pDirent = nullptr;
            while ((pDirent = readdir(pDir)) != nullptr) {
                std::string libName = pDirent->d_name;

#ifdef ANDROID
                std::string ext =  ".so";
#else
                std::string ext =  ".so.0";
#endif
                size_t pos = libName.rfind(ext);
                if (libName.find("lib") == 0 &&
                    pos != std::string::npos && pos + ext.size() == libName.size()) {
                    libList.push_back(libName);
                }
            }
        }
        closedir(pDir);
    }

    if (libList.size() == 0) {
        VAM_LOGE("VAManager::_initEngineLibs: No engine libs found !!");
        return VAM_FAIL;
    }

    try {
        for (size_t idx = 0; idx < libList.size(); idx++) {
            VAM_LOGD("VAManager::_initEngineLibs: loading %s",
                     libList[idx].c_str());
            std::string dynLibName = _dynLibPath + "/" + libList[idx];
            v = new VAMEngine_DynEngine(dynLibName.c_str(), _sourceInfo);
            if (v) {
                _engineList.push_back(v);
            }
        }
    } catch(int e) {
        VAM_LOGE("VAManager error(%d): Exception: failed to open engine.", e);
        return VAM_FAIL;
    } catch(...) {
        VAM_LOGE("VAManager error: Exception: failed to open engine.");
        return VAM_FAIL;
    }

    return VAM_OK;
}

int VAManager::destroy() {
    VAM_KPI_BASE("VamDestroy");
    stop();

    _engineList.clear();

    _workerThreadStop = true;
    _signalTask.notify_one();
    if (_workerThreadId.joinable()) {
        _workerThreadId.join();
    }
    _isInited = false;
    _workerThreadStop = false;

    return VAM_OK;
}

int VAManager::run() {
    if (_areEnginedRunning) {
        return VAM_OK;
    }

    std::lock_guard<std::mutex> m(_engineListMutex);
    _areEnginedRunning = true;
    std::deque<VAMEngineBase *>::iterator it =  _engineList.begin();
    for ( ; it != _engineList.end(); it++) {
        int ret = (*it)->run();
        if (ret != VAM_OK) {
            VAM_LOGE("VAManager warning(%d): engine run() failed.", ret);
        }
    }

    return VAM_OK;
}

int VAManager::stop() {
    VAM_KPI_BASE("VamStop");
    {
        std::lock_guard<std::mutex> l(_frameObjListMutex);
        _trimObjIDList(true);
    }

    if (_areEnginedRunning == false) {
        return VAM_OK;
    }

    std::lock_guard<std::mutex> m(_engineListMutex);
    _areEnginedRunning = false;
    std::deque<VAMEngineBase *>::iterator it =  _engineList.begin();
    for ( ; it != _engineList.end(); it++) {
        int ret = (*it)->stop();
        if (ret != VAM_OK) {
            VAM_LOGE("VAManager Error(%d): stop() failed", ret);
        }

        delete (*it);
    }

    return VAM_OK;
}

int VAManager::setConfig(vaapi_configuration *pConfig) {
    VAM_KPI_BASE(__func__);
    if (pConfig == nullptr) {
        return VAM_NULLPTR;
    }

    std::lock_guard<std::mutex> lg(_engineListMutex);
    for (size_t i = 0; i < pConfig->rule_size; i++) {
        VAMEngineBase *v = _getEngineByEventType(pConfig->rules[i].type);
        if (v == nullptr) {
            VAM_LOGE("VAManager error: Can't find an engine to "
                     "support event type %d. Rule %s gets ignored.",
                     pConfig->rules[i].type, pConfig->rules[i].id);
            continue;
        }

        // convert from relative to absolute coordinates
        vaapi_rule r = pConfig->rules[i];
        vaapi_source_info &si = _sourceInfo;
        for (uint32_t zi = 0; zi < r.zone_size; zi++) {
            for (uint32_t pi = 0; pi < r.zones[zi].point_size; pi++) {
                vaapi_point &p = r.zones[zi].points[pi];
                p.x = p.x * si.frame_l_width[0] / ABS_TO_REL_COORD_COEF;
                p.y = p.y * si.frame_l_height[0] / ABS_TO_REL_COORD_COEF;
            }
        }
        v->addRule(&r);
        VAM_LOGD("VAManager::setConfig: add rule:%d.", pConfig->rules[i].type);
    }

    // TODO(future): add compound event to rule interpreter
    // int VAMEngine_RuleInterpreter::addCompoundRule(vaapi_compound_rule *rule)

    return VAM_OK;
}

int VAManager::delConfig(vaapi_configuration *pConfig) {
    VAM_KPI_BASE("VamDelConfig");
    if (pConfig == nullptr) {
        return VAM_NULLPTR;
    }

    std::lock_guard<std::mutex> lg(_engineListMutex);
    for (size_t i = 0; i < pConfig->rule_size; i++) {
        VAMEngineBase *v = _getEngineByEventType(pConfig->rules[i].type);
        if (v == nullptr) {
            VAM_LOGE("VAManager error: Can't find an engine to "
                     "support event type %d. Rule %s gets ignored.\n",
                     pConfig->rules[i].type, pConfig->rules[i].id);
            continue;
        }

        v->rmRule(pConfig->rules[i].id);
    }

    return VAM_OK;
}

int VAManager::enrollObj(vaapi_event_type type,
                         vaapi_enrollment_info *enroll_info) {
    VAM_KPI_BASE("VamEnrollObj");

    if (!IsValidUUID(enroll_info->id)) {
        CreateUUID(enroll_info->id, VAAPI_UUID_LEN);
    }
    CreateUUID(enroll_info->img_id, VAAPI_UUID_LEN);

    for (size_t idx = 0; idx < _engineList.size(); idx++) {
        if (_engineList[idx] && _engineList[idx]->isEventSupported(type)) {
            return _engineList[idx]->enrollObj(enroll_info);
        }
    }

    return VAM_OK;
}

int VAManager::disenrollObj(vaapi_event_type type, const char *id) {
    VAM_KPI_BASE("VamDisenrollObj");
    for (size_t idx = 0; idx < _engineList.size(); idx++) {
        if (_engineList[idx] && _engineList[idx]->isEventSupported(type)) {
            return _engineList[idx]->disenrollObj(id);
        }
    }

    return VAM_OK;
}

int VAManager::putFrame(vaapi_frame_info *pFrameInfo) {
    VAM_KPI_DETAIL("VamPutFrame");
    if (pFrameInfo == nullptr) {
        return VAM_NULLPTR;
    }

    if (_engineList.size() <= 0) {
        return VAM_BUSY;
    }

    {
        std::lock_guard<std::mutex>  lg(_taskMutex);
        if (_tasks.size() > 0) {
            return VAM_BUSY;
        }

        VAMTask  *newTask = new VAMTask;
        memset(&newTask->frame, 0, sizeof(vaapi_frame_info));
        newTask->frame = *pFrameInfo;
        newTask->frame.obj_count = 0;
        newTask->frame.objects = nullptr;

        VAMStage *stage = new VAMStage;
        // engines that run on their own are executed first
        for (size_t i = 0; i < _engineList.size(); i++) {
            if (!_engineList[i]->isDependsOnOtherEvent()) {
                stage->engines.push_back(_engineList[i]);
            }
        }
        if (stage->engines.size() > 0) {
            newTask->stages.push_back(stage);
        }

        stage = new VAMStage;
        // engines that require input from other engines are executed second
        for (size_t i = 0; i < _engineList.size(); i++) {
            if (_engineList[i]->isDependsOnOtherEvent()) {
                stage->engines.push_back(_engineList[i]);
            }
        }
        if (stage->engines.size() > 0) {
            newTask->stages.push_back(stage);
        }

        _tasks.push_back(newTask);
    }
    _signalTask.notify_one();

    return VAM_OK;
}

void VAManager::_workerThread(VAManager *_this) {
    VAM_KPI_BASE("VamWorkerThread");
    _this->_isInited = true;

    while (_this->_workerThreadStop == false) {
        {
            std::cv_status res = std::cv_status::no_timeout;

            if (_this->_tasks.size() == 0) {
                std::unique_lock<std::mutex> ul(_this->_signalTaskMutex);
                res = _this->_signalTask.wait_for(ul,
                        std::chrono::milliseconds(1000));
            }

            if (res == std::cv_status::timeout &&
                !_this->_workerThreadStop &&
                _this->_tasks.size() == 0) {
                VAM_KPI_DETAIL("taskWaitTimeout");
                VAM_LOGD("%s: task wait timeout", __func__);
                continue;
            }
            if (_this->_workerThreadStop) {
                break;
            }
        }

        {
            std::lock_guard<std::mutex> lg(_this->_taskMutex);
            _this->_currentTask = _this->_tasks.begin();
        }

        VAMTask *task = *(_this->_currentTask);
        task->currentStage = task->stages.begin();
        while (task->currentStage != task->stages.end()) {
            VAM_KPI_DETAIL("taskCurrentStage");
            VAMStage *stage = *(task->currentStage);

            task->enginesReady = 0;
            for (VAMEngineBase *engine : stage->engines) {
                engine->putFrame(&task->frame);
            }

            do {
                std::unique_lock<std::mutex> ul(_this->_engineReadyMutex);
                _this->_engineReady.wait_for(ul, std::chrono::milliseconds(10));
                if (_this->_workerThreadStop) {
                    goto finishTask;
                }
            } while (task->enginesReady < stage->engines.size());

            {
                std::lock_guard<std::mutex> lg(_this->_resultsMutex);
                task->frame.obj_count = stage->results.size();
                if (task->frame.obj_count > 0) {
                    task->frame.objects = stage->results.data();
                } else {
                    task->frame.objects = nullptr;
                }
            }

            task->currentStage++;
        }

        _this->sendEventsToClient(task);
        _this->sendMetadataToClient(task);
finishTask:
        _this->sendFrameToClient(task);

//        std::list<VAMStage *>::iterator it = task->stages.begin();
        for (VAMStage *stage : task->stages) {
            delete stage;
        }
        delete task;

        {
            std::lock_guard<std::mutex> lg(_this->_taskMutex);
            _this->_tasks.erase(_this->_currentTask);
        }

        if (_this->_workerThreadStop) {
            break;
        }
    }

    _this->_isInited = false;
    _this->_workerThreadStop = false;
}

bool VAManager::isEventSupported(vaapi_event_type type) {
    for (size_t idx = 0; idx < _engineList.size(); idx++) {
        if (_engineList[idx] && _engineList[idx]->isEventSupported(type)) {
            return true;
        }
    }

    return false;
}

int VAManager::_eventCB(const VAMEngineBase *pEngine,
                        vaapi_event *pEventInfo,
                        vaapi_frame_info */*pFrameInfo*/,
                        void *pVAManager) {
    VAM_KPI_DETAIL("VamEventCB");
    if (pVAManager == nullptr || pEventInfo == nullptr) {
        return VAM_NULLPTR;
    }

    VAManager *_this = reinterpret_cast<VAManager *>(pVAManager);

    // Store the event in the task's event cache
    {
        std::lock_guard<std::mutex> lg(_this->_resultsMutex);
        VAMTask *task = *(_this->_currentTask);
        CreateUUID(pEventInfo->id, VAAPI_UUID_LEN);
        task->events.push_back(*pEventInfo);
    }

    // Add/update the event's object to the global object trail
    {
        std::lock_guard<std::mutex> lg(_this->_frameObjListMutex);
        std::string objID = _this->_getUniqueObjID(pEngine, pEventInfo->obj);
        std::map<std::string, int>::iterator it = _this->_objIDList.find(objID);
        if (it == _this->_objIDList.end()) {
            _this->_pushObjAndGetID(objID, pEventInfo->obj);

            it = _this->_objIDList.find(objID);
            if (it == _this->_objIDList.end()) {
                VAM_LOGE("VAManager::_eventCB error: Failed to assign "
                        "new object ID %d", pEventInfo->obj.id);
                return VAM_FAIL;
            }
        }
    }

    return VAM_OK;
}

int VAManager::sendEventsToClient(VAMTask *task) {
    if (!_pEventCBFunc) {
        return VAM_NULLPTR;
    }

    vaapi_source_info &si = _sourceInfo;
    for (size_t i = 0; i < task->events.size(); i++) {
        vaapi_event &e = task->events[i];
        vaapi_position &p = e.obj.pos;
        p.x = p.x * ABS_TO_REL_COORD_COEF / si.frame_l_width[0];
        p.y = p.y * ABS_TO_REL_COORD_COEF / si.frame_l_height[0];
        p.width = p.width * ABS_TO_REL_COORD_COEF / si.frame_l_width[0];
        p.height = p.height * ABS_TO_REL_COORD_COEF / si.frame_l_height[0];
        _pEventCBFunc(&e, _pEventCBUsrData);
    }

    return VAM_OK;
}

int VAManager::_metadataCB(const VAMEngineBase *pEngine,
                           uint64_t pts,
                           bool /*isUpdated*/,
                           void *pVAManager,
                           std::vector<vaapi_object> *objList) {
    VAM_KPI_DETAIL("VamMetadataCB");
    if (pVAManager == nullptr || pEngine == nullptr) {
        return VAM_NULLPTR;
    }

    VAManager *_this = reinterpret_cast<VAManager *>(pVAManager);

    // Store objects for next engines stage
    {
        std::lock_guard<std::mutex> lg(_this->_resultsMutex);
        VAMTask *task = *(_this->_currentTask);
        VAMStage *stage = *(task->currentStage);
        stage->results.insert(stage->results.end(),
                              objList->begin(),
                              objList->end());
    }

    // Store objects in global object trail
    {
        std::lock_guard<std::mutex> l(_this->_frameObjListMutex);
        for (size_t i = 0; i < objList->size(); i++) {
            vaapi_object &o = objList->at(i);
            std::string uniqueObjID = _this->_getUniqueObjID(pEngine, o);
            o.id = _this->_pushObjAndGetID(uniqueObjID, o);
            _this->_frameObjList[pts].push_back(o);
        }
    }

    return VAM_OK;
}

int VAManager::sendMetadataToClient(VAMTask *task) {
    struct vaapi_metadata_frame m;
    memset(&m, 0, sizeof(vaapi_metadata_frame));
    m.pts = task->frame.time_stamp;
    m.object_num = _frameObjList[m.pts].size();
    if (m.object_num > 0) {
        vaapi_source_info &si = _sourceInfo;
        for (uint32_t i = 0; i < m.object_num; i++) {
            vaapi_object o = _frameObjList[m.pts][i];
            vaapi_position &p = o.pos;
            p.x = p.x * ABS_TO_REL_COORD_COEF / si.frame_l_width[0];
            p.y = p.y * ABS_TO_REL_COORD_COEF / si.frame_l_height[0];
            p.width = p.width * ABS_TO_REL_COORD_COEF / si.frame_l_width[0];
            p.height = p.height * ABS_TO_REL_COORD_COEF / si.frame_l_height[0];
            task->objects.push_back(o);
        }

        m.objects = (vaapi_object *)task->objects.data();
    }

    if (_pMetadataCBFunc) {
        _pMetadataCBFunc(&m, _pMetadataCBUsrData);
    }

    _frameObjList.erase(_frameObjList.find(m.pts));
    _trimObjIDList();

    return VAM_OK;
}

int VAManager::_frameCB(const VAMEngineBase */*pEngine*/,
                        vaapi_frame_info */*frameInfo*/,
                        void *pVAManager) {
    VAM_KPI_DETAIL("VamFrameCB");
    if (pVAManager == nullptr) {
        return VAM_NULLPTR;
    }

    VAManager *_this = reinterpret_cast<VAManager *>(pVAManager);

    std::lock_guard<std::mutex> lg(_this->_engineMutex);
    (*(_this->_currentTask))->enginesReady++;
    _this->_engineReady.notify_one();

    return VAM_OK;
}

int VAManager::sendFrameToClient(const VAMTask *task) {
    VAM_KPI_DETAIL("VamSendFrameToClient");
    if (_pSnapshotCBFunc && task->events.size() > 0) {
        struct vaapi_snapshot_info snapshotInfo;
        memset(&snapshotInfo, 0, sizeof(snapshotInfo));
        snapshotInfo.pts = task->frame.time_stamp;
        snapshotInfo.source_info = _sourceInfo;
        snapshotInfo.img_data[0] = task->frame.frame_l_data[0];
        snapshotInfo.img_data[1] = task->frame.frame_l_data[1];
        snapshotInfo.img_data[2] = task->frame.frame_l_data[2];
        snapshotInfo.events = (vaapi_event *)task->events.data();
        snapshotInfo.events_count = task->events.size();
        snapshotInfo.objects = (vaapi_object *)task->objects.data();
        snapshotInfo.objects_count = task->objects.size();
        CreateUUID(snapshotInfo.id, sizeof(snapshotInfo.id));

        _pSnapshotCBFunc(&snapshotInfo, _pSnapshotCBUsrData);
    }

    if (_pFrameProcessedCBFunc) {
        _pFrameProcessedCBFunc(task->frame.time_stamp,
                               _pFrameProcessedCBUsrData);
    }
    return VAM_OK;
}

std::string VAManager::_getUniqueObjID(const VAMEngineBase *pEngine,
                                       const vaapi_object &objInfo) {
    std::stringstream ss;
    ss << pEngine;
    ss << "-";
    ss << objInfo.id;

    return ss.str();
}

int VAManager::_resetObjIDList() {
    for (size_t i = 0; i < _objIDAssignment.size(); i++) {
        _objIDAssignment[i].lastSeen--;
    }

    return VAM_OK;
}

uint32_t VAManager::_pushObjAndGetID(std::string uniqueObjID,
                                     const vaapi_object &objInfo) {
    const int MAX_TRAIL_SIZE = 512;
    const vaapi_position &pos = objInfo.pos;
    size_t IDIdx = 0;
    std::map<std::string, int>::iterator it =  _objIDList.find(uniqueObjID);
    if (it != _objIDList.end()) {
        IDIdx = it->second;
        _objIDAssignment[IDIdx].lastSeen = -1;
        size_t trailSize = _objIDAssignment[IDIdx].trail.size();

        // only update when moves
        if (trailSize > 0 &&
            _objIDAssignment[IDIdx].trail[trailSize - 1].x != pos.x &&
            _objIDAssignment[IDIdx].trail[trailSize - 1].y != pos.y &&
            _objIDAssignment[IDIdx].trail[trailSize - 1].width != pos.width &&
            _objIDAssignment[IDIdx].trail[trailSize - 1].height != pos.height) {
            _objIDAssignment[IDIdx].trail.push_back(pos);
        }

        if (_objIDAssignment[IDIdx].trail.size() > MAX_TRAIL_SIZE) {
            _objIDAssignment[IDIdx].trail.pop_front();
        }

        return _objIDAssignment[IDIdx].VAMAssignedID;
    }

    // find next empty slot
    for (IDIdx = 0; IDIdx < _objIDAssignment.size(); IDIdx++) {
        if (_objIDAssignment[IDIdx].uniqueObjID.empty()) {
            break;
        }
    }

    const int MAX_OBJECT_QUEUED = 1024;
    // resize _objIDAssignment if full
    if (IDIdx == _objIDAssignment.size()) {
        if (_objIDList.size() > MAX_OBJECT_QUEUED) {
            VAM_LOGE("VAManager::_metadataCB error: failed to assign obj ID,"
                     " tool size overflow.");
            return -1;
        }

        _objIDAssignment.resize(_objIDAssignment.size() * 2);
    }

    _objIDAssignment[IDIdx].uniqueObjID = uniqueObjID;
    _objIDAssignment[IDIdx].lastSeen = -1;
    _objIDAssignment[IDIdx].trail.push_back(pos);

    _objIDAssignment[IDIdx].VAMAssignedID = ++_lastAssignedID;

    if (_lastAssignedID > std::numeric_limits<uint32_t>::max() - 3) {
        _lastAssignedID = 0;
    }

    _objIDList[uniqueObjID] = IDIdx;

    return _objIDAssignment[IDIdx].VAMAssignedID;
}

int VAManager::_trimObjIDList(bool clearAll) {
    const int REMOVE_UNSEEN_OBJ_AFTER_FRAME = -3000;
    for (size_t i = 0; i <_objIDAssignment.size(); i++) {
        VAMObjIDInfo &o = _objIDAssignment[i];

        if (o.uniqueObjID.empty()) {
            continue;
        }

        if (o.lastSeen >= REMOVE_UNSEEN_OBJ_AFTER_FRAME && clearAll == false) {
            continue;
        }

        std::map<std::string, int>::iterator it;
        it = _objIDList.find(o.uniqueObjID);
        if (it != _objIDList.end()) {
            _objIDList.erase(it);
        }

        o.clear();
    }

    return VAM_OK;
}

VAManager::VAMObjIDInfo::VAMObjIDInfo()
    : lastSeen(0) {
    memset(&frameInfo, 0, sizeof(vaapi_frame_info));
}

VAManager::VAMObjIDInfo::~VAMObjIDInfo() {
    clear();
}

void VAManager::VAMObjIDInfo::clear() {
    if (frameInfo.frame_l_data[0]) {
        delete [] frameInfo.frame_l_data[0];
    }
    frameInfo.frame_l_data[0] = nullptr;

    if (frameInfo.frame_l_data[1]) {
        delete [] frameInfo.frame_l_data[1];
    }
    frameInfo.frame_l_data[1] = nullptr;

    if (frameInfo.frame_l_data[2]) {
        delete [] frameInfo.frame_l_data[2];
    }
    frameInfo.frame_l_data[2] = nullptr;

    uniqueObjID.clear();
    lastSeen = 0;
    trail.clear();
    eventID.clear();
}

int VAManager::convertMetadataToJSON(const struct vaapi_metadata_frame *pFrame,
                                     std::string *pJSONStr) {
    if (pJSONStr == nullptr) {
        return VAM_NULLPTR;
    }

    *pJSONStr = getStrFromMetadataFrame(pFrame);

    return VAM_OK;
}

int VAManager::convertEventToJSON(const struct vaapi_event *pEvent,
                                  std::string *pJSONStr) {
    if (pJSONStr == nullptr) {
        return VAM_NULLPTR;
    }

    *pJSONStr = getStrFromEvent(pEvent);

    return VAM_OK;
}

int VAManager::convertRuleToJSON(const struct vaapi_rule *pR,
                                 std::string *pJSONStr) {
    if (pJSONStr == nullptr) {
        return VAM_NULLPTR;
    }

    *pJSONStr = getStrFromRule(pR);

    return VAM_OK;
}

int VAManager::convertEnrollInfoToJSON(const struct vaapi_enrollment_info *pEnroll,
                                       std::string *pJSONStr) {
    if (pJSONStr == nullptr) {
        return VAM_NULLPTR;
    }

    *pJSONStr = getStrFromEnrollInfo(pEnroll);

    return VAM_OK;
}
