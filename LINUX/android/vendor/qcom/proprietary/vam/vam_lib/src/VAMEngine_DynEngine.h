/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAMENGINE_DYNENGINE_H_
#define _VAMENGINE_DYNENGINE_H_

#include <vam_engine_api.h>
#include <VAMUtilities.h>
#include "VAMEngineBase.h"
#include <string>

class VAMEngine_DynEngine: public VAMEngineBase {
 public:
    VAMEngine_DynEngine(const std::string libName,
                        const struct vaapi_source_info &sourceInfo);
    ~VAMEngine_DynEngine();

    virtual int enrollObj(vaapi_enrollment_info *enroll_info);
    virtual int disenrollObj(const char *id);
    virtual bool isEventSupported(vaapi_event_type type);

 protected:
    virtual int _addRule(vaapi_rule *pRule);
    virtual int _rmRule(std::string configID);

 private:
    virtual int _workerFunc(struct vaapi_frame_info *p);

    void *_pEngineLibHandle;
    vaapi_alg_module_t *_pEngineModule;

    void *_pEngineHandle;
};

#endif  // _VAMENGINE_DYNENGINE_H_
