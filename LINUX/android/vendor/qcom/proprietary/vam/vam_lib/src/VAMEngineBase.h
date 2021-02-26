/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAENGINEBASE_H_
#define _VAENGINEBASE_H_

#include <vaapi.h>
#include <VAMUtilities.h>
#include <deque>
#include <string>
#include <vector>

class VAMEngineBase {
 public:
    explicit VAMEngineBase(const vaapi_source_info &sourceInfo);
    virtual ~VAMEngineBase();

    int init();
    int destroy();

    // used it for both new and update
    int addRule(vaapi_rule *pRule);
    int rmRule(std::string configID);

    int run();
    int stop();

    int putFrame(vaapi_frame_info *pFrameInfo);

    inline bool isDependsOnOtherEvent() {
        return _isDependsOnOtherEvent;
    }

    // NOTE: always call event callback BEFORE metadata callback
    typedef int (*EventCBFunc)(const VAMEngineBase *pEngine,
                               vaapi_event *pEventInfo,
                               vaapi_frame_info *pFrameInfo,
                               void *pVAManager);

    // NOTE: always call event callback AFTER metadata callback
    typedef int (*MetadataCBFunc)(const VAMEngineBase *pEngine,
                                  uint64_t pts,
                                  bool isUpdated,
                                  void *pVAManager,
                                  std::vector<vaapi_object> *objList);

    // NOTE: always call frame done callback last
    typedef int (*FrameDoneCBFunc)(const VAMEngineBase *pEngine,
                                   vaapi_frame_info *frameInfo,
                                   void *pVAManager);

    static void RegisterCBs(void *pVAManager,
                            EventCBFunc pEventCBFunc,
                            MetadataCBFunc pMetadataCBFunc,
                            FrameDoneCBFunc pFrameDoneCBFunc);

    virtual int enrollObj(vaapi_enrollment_info *) {
        return VAM_OK;
    }

    virtual int disenrollObj(const char *) {
        return VAM_OK;
    }

    virtual bool isEventSupported(vaapi_event_type type) = 0;

 protected:
    virtual int _addRule(vaapi_rule */*pRule*/) {
        return VAM_OK;
    }
    virtual int _rmRule(std::string /*configID*/) {
        return VAM_OK;
    }

    // the object information for current frame
    std::vector<vaapi_object> _objList;
    std::vector<vaapi_event> _eventList;

    std::deque<vaapi_frame_info> _frameDataList;
    std::mutex _frameDataListMutex;
    std::condition_variable _putFrameCond;
    const struct vaapi_source_info _sourceInfo;

    uint64_t _lastPutFramePTS;
    uint64_t _lastProcessedPTS;
    uint8_t _targetFPS;

    bool _isDependsOnOtherEvent;

 private:
    int _updateRule(vaapi_rule *pRule);
    bool _isTimeToPutFrame(uint64_t newPTS);

    static void _workerThread(VAMEngineBase *_this);
    virtual int _workerFunc(vaapi_frame_info *pFrameInfo) = 0;
    int _processCallback(vaapi_frame_info *pFrameInfo, bool isUpdated);

    bool _isStop;
    std::thread _workerThreadID;
    bool _isWorkerThreadRunning;

    std::mutex _configMutex;

    const size_t MAX_FRAME_QUEUE_SIZE;

    static EventCBFunc _pEventCBFunc;
    static MetadataCBFunc _pMetadataCBFunc;
    static FrameDoneCBFunc _pFrameDoneCBFunc;
    static void *_pVAManager;
};

#endif  // _VAENGINEBASE_H_
