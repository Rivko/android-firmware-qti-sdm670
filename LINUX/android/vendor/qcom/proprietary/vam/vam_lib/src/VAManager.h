/*
 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAMANAGER_H_
#define _VAMANAGER_H_

#include <vaapi.h>
#include <vaapi_errors.h>
#include <VAMUtilities.h>
#include "VAMEngineBase.h"

#include <string>
#include <map>
#include <deque>
#include <list>
#include <vector>

class VAManager {
 public:
    VAManager();
    ~VAManager();

    int init(const struct vaapi_source_info *pSourceInfo,
             const char *dyn_lib_path);
    int destroy();

    int setConfig(vaapi_configuration *config);
    int delConfig(vaapi_configuration *config);
    int enrollObj(vaapi_event_type type, vaapi_enrollment_info *enroll_info);
    int disenrollObj(vaapi_event_type type, const char *id);

    int putFrame(vaapi_frame_info *pFrameInfo);

    int run();
    int stop();

    bool isEventSupported(vaapi_event_type type);

    int convertMetadataToJSON(const struct vaapi_metadata_frame *frame,
                              std::string *pJSONStr);
    int convertEventToJSON(const struct vaapi_event *pEvent,
                           std::string *pJSONStr);
    int convertRuleToJSON(const struct vaapi_rule *pR,
                          std::string *pJSONStr);
    int convertEnrollInfoToJSON(const struct vaapi_enrollment_info *pEnroll,
                                std::string *pJSONStr);

    inline void registerEventCB(vaapi_event_cb_func func, void *pUsrData) {
        _pEventCBFunc = func;
        _pEventCBUsrData = pUsrData;
    }
    inline void registerMetadataFrameCB(vaapi_metadata_cb_func func,
                                        void *pUsrData) {
        _pMetadataCBFunc = func;
        _pMetadataCBUsrData = pUsrData;
    }
    inline void registerSnapshotCB(vaapi_snapshot_cb_func func,
                                   void *pUsrData) {
        _pSnapshotCBFunc = func;
        _pSnapshotCBUsrData = pUsrData;
    }

    inline void registerFrameProcessedCB(vaapi_frame_processed_cb_func func,
                                         void *pUsrData) {
        _pFrameProcessedCBFunc = func;
        _pFrameProcessedCBUsrData = pUsrData;
    }

 private:
    static int _eventCB(const VAMEngineBase *pEngine,
                        vaapi_event *pEventInfo,
                        vaapi_frame_info *pFrameInfo,
                        void *pVAManager);
    static int _metadataCB(const VAMEngineBase *pEngine,
                           uint64_t pts,
                           bool isUpdated,
                           void *pVAManager,
                           std::vector<vaapi_object> *objList);
    static int _frameCB(const VAMEngineBase *pEngine,
                        vaapi_frame_info *frameInfo,
                        void *pVAManager);
    static void _workerThread(VAManager *_this);

    struct VAMStage {
        std::list<VAMEngineBase *> engines;
        std::vector<vaapi_object> results;
    };

    struct VAMTask {
        std::list<VAMStage *> stages;
        std::list<VAMStage *>::iterator currentStage;
        size_t enginesReady;
        vaapi_frame_info frame;
        std::vector<vaapi_event> events;
        std::vector<vaapi_object> objects;
    };

    std::mutex _signalTaskMutex;
    std::condition_variable _signalTask;
    std::mutex _engineReadyMutex;
    std::condition_variable _engineReady;

    std::mutex _taskMutex;
    std::mutex _engineMutex;
    std::mutex _resultsMutex;


    std::list<VAMTask *> _tasks;
    std::list<VAMTask *>::iterator _currentTask;

    std::string _dynLibPath;
    std::string _dynDataPath;
    std::deque<VAMEngineBase*> _engineList;
    std::mutex _engineListMutex;


    std::thread _workerThreadId;

    bool _isInited;
    bool _workerThreadStop;
    bool _areEnginedRunning;
    struct vaapi_source_info _sourceInfo;

    struct VAMObjIDInfo {
        std::string uniqueObjID;
        int lastSeen;
        uint32_t VAMAssignedID;

        std::deque<vaapi_position> trail;

        // TODO(future): <Temp> for generating trail image
        vaapi_frame_info frameInfo;

        // TODO(future): make it a vector?
        std::string eventID;

        VAMObjIDInfo();
        ~VAMObjIDInfo();

        void clear();
    };

    std::mutex _frameObjListMutex;
    std::map<uint64_t, std::vector<vaapi_object>> _frameObjList;
    std::map<std::string, int> _objIDList;
    std::vector<VAMObjIDInfo> _objIDAssignment;
    uint32_t _lastAssignedID;

    vaapi_event_cb_func _pEventCBFunc;
    void *_pEventCBUsrData;
    vaapi_metadata_cb_func _pMetadataCBFunc;
    void *_pMetadataCBUsrData;
    vaapi_snapshot_cb_func _pSnapshotCBFunc;
    void *_pSnapshotCBUsrData;
    vaapi_frame_processed_cb_func _pFrameProcessedCBFunc;
    void *_pFrameProcessedCBUsrData;

    int sendEventsToClient(VAMTask *task);
    int sendMetadataToClient(VAMTask *task);
    int sendFrameToClient(const VAMTask *task);

    VAMEngineBase *_getEngineByEventType(vaapi_event_type type);
    int _initEngineLibs();

    std::string _getUniqueObjID(const VAMEngineBase *pEngine,
                                const vaapi_object &objInfo);
    int _resetObjIDList();
    uint32_t _pushObjAndGetID(std::string uniqueObjID,
                              const vaapi_object &objInfo);
    int _trimObjIDList(bool clearAll = false);
};

#endif  // _VAMANAGER_H_
