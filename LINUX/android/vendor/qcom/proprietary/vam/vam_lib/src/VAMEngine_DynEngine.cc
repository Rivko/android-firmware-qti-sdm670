/*
 * Copyright (c) 2016-2017, 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define DEBUG_LOG

#include <dlfcn.h>
#include "VAMEngine_DynEngine.h"
#include <string>
#include <vector>

static const int kMaxTargetFPS = 61;       // Ceiling for targetFPS

VAMEngine_DynEngine::VAMEngine_DynEngine(const std::string libName,
                                const struct vaapi_source_info &sourceInfo):
    VAMEngineBase(sourceInfo),
    _pEngineLibHandle(nullptr),
    _pEngineModule(nullptr),
    _pEngineHandle(nullptr) {
    VAM_KPI_GET_MASK();
    VAM_KPI_BASE("ConstructDynEngine");
    // open library
    _pEngineLibHandle = dlopen(libName.c_str(), RTLD_LAZY);
    if (_pEngineLibHandle == nullptr) {
        VAM_LOGE("VAMEngine_DynEngine::VAMEngine_DynEngine: "
                 "load (%s) failed error = %s\n", libName.c_str(), dlerror());
        throw VAM_ENGINE_DYLIB_NOT_EXIST;
    }

    // Clear any existing error
    dlerror();

    // link symbol
    _pEngineModule = reinterpret_cast<vaapi_alg_module_t *>(
                                                    dlsym(_pEngineLibHandle,
                                                          "VA_ALG_INFO_SYM"));
    if (_pEngineModule == nullptr) {
        VAM_LOGE("VAMEngine_DynEngine::VAMEngine_DynEngine: "
                 "Can't find symbol in (%s) failed error = %s \n",
                 libName.c_str(), dlerror());
        dlclose(_pEngineLibHandle);
        throw VAM_ENGINE_SYMBOL_ERROR;
    }

    auto ret = _pEngineModule->methods->init(&sourceInfo, &_pEngineHandle);
    if (ret != VAM_OK) {
        VAM_LOGE("VAMEngine_DynEngine: _pEngineModule init failed !!");
        throw VAM_FAIL;
    }

    uint8_t targetFPS = 0;
    _pEngineModule->methods->get_target_FPS(_pEngineHandle, &targetFPS);
    _targetFPS = static_cast<int>(targetFPS);
    if ((_targetFPS == 0) || (_targetFPS > kMaxTargetFPS)) {
        VAM_LOGW("VAMEngine_DynEngine error: found invalid target FPS %d"
                 ". set to default 30 FPS.\n", _targetFPS);
        _targetFPS = 30;
    }

    for (int eventType = 0; eventType < vaapi_event_size; eventType++) {
        uint8_t isDepend = 0;
        _pEngineModule->methods->is_depends_on_other_event(_pEngineHandle,
                                        (vaapi_event_type)eventType, &isDepend);
        _isDependsOnOtherEvent |= isDepend;
    }

    VAM_LOGD("===VAM Dynamic engine info=================begin");
    VAM_LOGD("library path %s", libName.c_str());
    VAM_LOGD("module api version: %s", _pEngineModule->module_api_version);
    VAM_LOGD("name: %s", _pEngineModule->name);
    VAM_LOGD("author: %s", _pEngineModule->author);
    VAM_LOGD("target FPS: %d", _targetFPS);
    VAM_LOGD("supported events: ");
    for (int eventType = 0; eventType < vaapi_event_size; eventType++) {
        uint8_t isSupported = 0;
        int32_t ret = _pEngineModule->methods->is_event_supported(
                _pEngineHandle, (vaapi_event_type)eventType, &isSupported);
        if (ret == VAM_OK && isSupported) {
            VAM_LOGD("%d, ", eventType);
        }
    }

    if (_isDependsOnOtherEvent == false) {
        VAM_LOGD("is depends on other event: no.");
    } else {
        VAM_LOGD("is depends on other event: %s",
                (_isDependsOnOtherEvent) ? "yes" : "no");
        for (int eventType = 0; eventType < vaapi_event_size; eventType++) {
            uint8_t isDepend = 0;
            int32_t ret = _pEngineModule->methods->is_depends_on_other_event(
                    _pEngineHandle, (vaapi_event_type)eventType, &isDepend);
            if (ret == VAM_OK && isDepend) {
                VAM_LOGD("%d, ", eventType);
            }
        }
    }
    VAM_LOGD("===VAM Dynamic engine info=================end");
    VAM_LOGD("VA Dynamic Engine sucessfully inited.");
}

VAMEngine_DynEngine::~VAMEngine_DynEngine() {
    VAM_KPI_BASE("DestroyDynEngine");
    if (_pEngineModule) {
        _pEngineModule->methods->deinit(_pEngineHandle);
    }
    _pEngineHandle = nullptr;

    if (_pEngineLibHandle) {
        dlclose(_pEngineLibHandle);
    }
    _pEngineLibHandle = nullptr;
}

int VAMEngine_DynEngine::_addRule(vaapi_rule *pRule) {
    VAM_KPI_BASE(__func__);
    if (_pEngineModule == nullptr) {
        return VAM_NULLPTR;
    }

    return _pEngineModule->methods->add_rule(_pEngineHandle, pRule);
}

int VAMEngine_DynEngine::_rmRule(std::string configID) {
    VAM_KPI_BASE(__func__);
    if (_pEngineModule == nullptr) {
        return VAM_NULLPTR;
    }

    return _pEngineModule->methods->del_rule(_pEngineHandle, configID.c_str());
}

int VAMEngine_DynEngine::enrollObj(vaapi_enrollment_info *enroll_info) {
    VAM_KPI_BASE("DynEngineEnrollObj");
    if (_pEngineModule == nullptr) {
        return VAM_NULLPTR;
    }

    return _pEngineModule->methods->enroll_obj(_pEngineHandle, enroll_info);
}

int VAMEngine_DynEngine::disenrollObj(const char *id) {
    if (_pEngineModule == nullptr) {
        return VAM_NULLPTR;
    }

    return _pEngineModule->methods->disenroll_obj(_pEngineHandle, id);
}

bool VAMEngine_DynEngine::isEventSupported(vaapi_event_type type) {
    if (_pEngineModule == nullptr) {
        return false;
    }

    uint8_t isSupported = 0;
    int32_t ret = _pEngineModule->methods->is_event_supported(_pEngineHandle,
                                                              type,
                                                              &isSupported);

    if (ret != VAM_OK) {
        return false;
    }

    return isSupported > 0;
}

int VAMEngine_DynEngine::_workerFunc(struct vaapi_frame_info *p) {
    VAM_KPI_DETAIL(_pEngineModule->name);
    if (_pEngineModule == nullptr || p == nullptr) {
        return VAM_NULLPTR;
    }

    _pEngineModule->methods->process(_pEngineHandle, p);

    uint32_t objCount = 0;
    int ret = _pEngineModule->methods->get_object_count(_pEngineHandle,
                                                        &objCount);
    if (ret == VAM_OK && objCount > 0) {
        std::vector<vaapi_object> objList;

        vaapi_object *pObjects = new vaapi_object[objCount];
        memset(pObjects, 0, sizeof(vaapi_object)*objCount);
        int error = _pEngineModule->methods->get_objects(_pEngineHandle,
                                                         pObjects,
                                                         objCount);

        if (error) {
            VAM_LOGE("VAMEngine_DynEngine error(%d): "
                     "get_meta failed\n", objCount);
        }

        for (size_t i = 0; i < objCount; i++) {
            _objList.push_back(pObjects[i]);
        }

        delete [] pObjects;
    }

    uint32_t eventCount = 0;
    ret = _pEngineModule->methods->get_event_count(_pEngineHandle, &eventCount);
    if (ret == VAM_OK && eventCount  > 0) {
        VAM_LOGD("VAMEngine_DynEngine: get event\n");
        vaapi_event *pEvents = new vaapi_event[eventCount];
        memset(pEvents, 0, sizeof(vaapi_event)*eventCount);
        int32_t error = _pEngineModule->methods->get_events(_pEngineHandle,
                                                            pEvents,
                                                            eventCount);

        if (error) {
            VAM_LOGE("VAMEngine_DynEngine error(%d): "
                     "get_event failed\n", error);
        }

        for (size_t i = 0; i < eventCount; i++) {
            _eventList.push_back(pEvents[i]);
        }

        delete [] pEvents;
    }

    return VAM_OK;
}
