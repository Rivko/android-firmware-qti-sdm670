/******************************************************************************
  @file    IOP.cpp
  @brief   Android IOP HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <dlfcn.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <hidl/HidlTransportSupport.h>
#include "Iop.h"
#include "io-p.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace iop {
namespace V2_0 {
namespace implementation {

// Methods from ::vendor::qti::hardware::iop::V2_0::IIop follow.
Return<int32_t> Iop::iopStart(int32_t pid, const hidl_string& pkg_name, const hidl_string& code_path) {
    iop_msg_t pMsg;
    uint32_t retVal = -1;
    memset(&pMsg, 0, sizeof(iop_msg_t));

    pMsg.cmd = IOP_CMD_PERFLOCK_IOPREFETCH_START;
    pMsg.pid = pid;
    strlcpy(pMsg.pkg_name, pkg_name.c_str(), PKG_LEN);
    strlcpy(pMsg.code_path, code_path.c_str(), PKG_LEN);
    if (mHandle.dlhandle != NULL)
        retVal = (*(mHandle.iop_server_submit_request))(&pMsg);
    return retVal;
}

Return<void> Iop::iopStop() {
    iop_msg_t pMsg;
    memset(&pMsg, 0, sizeof(iop_msg_t));

    pMsg.cmd = IOP_CMD_PERFLOCK_IOPREFETCH_STOP;
    pMsg.pkg_name[0] = 0;
    pMsg.code_path[0] = 0;
    if (mHandle.dlhandle != NULL)
        (*(mHandle.iop_server_submit_request))(&pMsg);
    return Void();
}

Return<int32_t> Iop::uxEngine_events(int32_t opcode, int32_t pid, const hidl_string& pkg_name, int32_t lat) {
    iop_msg_t pMsg;
    uint32_t retVal = -1;
    memset(&pMsg, 0, sizeof(iop_msg_t));

    pMsg.cmd = UXENGINE_CMD_PERFLOCK_EVENTS;
    pMsg.opcode = opcode;
    pMsg.pid = pid;
    strlcpy(pMsg.pkg_name, pkg_name.c_str(), PKG_LEN);
    pMsg.lat = lat;

    if (mHandle.dlhandle != NULL)
        retVal = (*(mHandle.iop_server_submit_request))(&pMsg);
    return retVal;
}

Return<void> Iop::uxEngine_trigger(int32_t opcode, uxEngine_trigger_cb _hidl_cb) {
    iop_msg_t pMsg;
    char *retVal;
    memset(&pMsg, 0, sizeof(iop_msg_t));

    pMsg.cmd = UXENGINE_CMD_PERFLOCK_TRIGGER;
    pMsg.opcode = opcode;
    pMsg.pid = 0;
    strlcpy(pMsg.pkg_name, "\0", PKG_LEN);
    pMsg.lat = 0;

    if (mHandle.dlhandle != NULL)
        retVal = (*(mHandle.uxe_server_submit_request))(&pMsg);
    else
        return Void();

    if (retVal != NULL) {
        _hidl_cb(retVal);
    } else {
        _hidl_cb("");
    }

    return Void();
}

Iop::Iop() {
    mMutex = PTHREAD_MUTEX_INITIALIZER;
    mHandle.is_opened = false;
    mHandle.dlhandle = NULL;
    mHandle.iop_server_init = NULL;
    mHandle.iop_server_exit = NULL;
    mHandle.iop_server_submit_request = NULL;
    mHandle.uxe_server_submit_request = NULL;

}

Iop::~Iop() {
}

void Iop::LoadIopLib() {
    const char *rc = NULL;
    char buf[PROPERTY_VALUE_MAX];

    if (!mHandle.is_opened) {
         mHandle.dlhandle = dlopen("libqti-iopd.so", RTLD_NOW | RTLD_LOCAL);
         if (mHandle.dlhandle == NULL) {
             ALOGE("IOP-HAL %s Failed to (dl)open iopd\n", __func__);
             return;
         }

         dlerror();

         *(void **) (&mHandle.iop_server_init) = dlsym(mHandle.dlhandle, "iop_server_init");
         if ((rc = dlerror()) != NULL) {
             ALOGE("IOP-HAL %s Failed to get iop_server_init\n", rc);
             dlclose(mHandle.dlhandle);
             mHandle.dlhandle = NULL;
             return;
         }

         *(void **) (&mHandle.iop_server_exit) = dlsym(mHandle.dlhandle, "iop_server_exit");
         if ((rc = dlerror()) != NULL) {
             ALOGE("IOP-HAL %s Failed to get iop_server_exit\n", rc);
             dlclose(mHandle.dlhandle);
             mHandle.dlhandle = NULL;
             return;
         }

         *(void **) (&mHandle.iop_server_submit_request) = dlsym(mHandle.dlhandle, "iop_server_submit_request");
         if ((rc = dlerror()) != NULL) {
             ALOGE("IOP-HAL %s Failed to get iop_server_submit_request\n", rc);
             dlclose(mHandle.dlhandle);
             mHandle.dlhandle = NULL;
             return;
         }

         *(void **) (&mHandle.uxe_server_submit_request) = dlsym(mHandle.dlhandle, "uxe_server_submit_request");
         if ((rc = dlerror()) != NULL) {
             ALOGE("IOP-HAL %s Failed to get uxe_server_submit_request\n", __func__);
             dlclose(mHandle.dlhandle);
             mHandle.dlhandle = NULL;
             return;
         }
         mHandle.is_opened = true;
    }

    return;
}

// Methods from ::android::hidl::base::V2_0::IBase follow.

IIop* HIDL_FETCH_IIop(const char* /* name */) {
    ALOGE("IOP-HAL: inside HIDL_FETCH_IIop");
    Iop *iopObj = new (std::nothrow) Iop();
    ALOGE("IOP-HAL: boot Address of iop object");
    if (iopObj != NULL) {
        iopObj->LoadIopLib();
        ALOGE("IOP-HAL: loading library is done");
        if (iopObj->mHandle.iop_server_init != NULL ) {
            (*(iopObj->mHandle.iop_server_init))();
        }
    }
    return iopObj;
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace iop
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
