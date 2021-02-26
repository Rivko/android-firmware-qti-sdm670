/******************************************************************************
  @file     qcril_qmi_audio_service.c
  @brief    qcril qmi - qcril_audio_service

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <hidl/HidlSupport.h>
#include <string.h>
#include <qcril_qmi_audio_service.h>
#include <qcril_am.h>
#include <string>

extern "C" {
    #include "qcril_log.h"
}

using android::hardware::Return;
using android::hardware::Void;
using android::sp;
using android::hardware::hidl_string;

using namespace android;
using namespace vendor::qti::hardware::radio::am::V1_0;
using namespace vendor::qti::hardware::radio::am::V1_0::implementation;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace am {
namespace V1_0 {
namespace implementation {

static sp<qcril_audio_impl> audio_service;

Return<void> qcril_audio_impl::setCallback(const ::android::sp<IQcRilAudioCallback>& cb) {
    QCRIL_LOG_DEBUG("QcRilAudioImpl::setCallback");

    RWLock::AutoWLock lock(mCallbackLock);
    if (mQcRilAudioCallback != nullptr) {
        mQcRilAudioCallback->unlinkToDeath(this);
    }
    mQcRilAudioCallback = cb;
    if (mQcRilAudioCallback != nullptr) {
        mQcRilAudioCallback->linkToDeath(this, 0);
    }
    return Void();
}

sp<IQcRilAudioCallback> qcril_audio_impl::getQcRilAudioCallback() {
    RWLock::AutoRLock lock(mCallbackLock);
    return mQcRilAudioCallback;
}

void qcril_audio_impl::serviceDied(uint64_t,
        const ::android::wp<::android::hidl::base::V1_0::IBase>&) {
    QCRIL_LOG_DEBUG("qcril_audio_impl::serviceDied: Client died. Cleaning up callbacks");
    RWLock::AutoWLock lock(mCallbackLock);
    mQcRilAudioCallback = nullptr;
}

qcril_audio_impl::qcril_audio_impl() {}

qcril_audio_impl::~qcril_audio_impl() {}

Return<void> qcril_audio_impl::setError(AudioError errorCode) {
    QCRIL_LOG_INFO("QcRilAudioImpl::setError: %d", errorCode);
    if (errorCode == AudioError::AUDIO_STATUS_SERVER_DIED) {
        qcril_am_audio_system_error_callback();
    }
    return Void();
}

int qcril_audio_impl::setParameters(android::String8 param) {
    QCRIL_LOG_INFO("QcRilAudioImpl::setParameters");
    int result = -1;
    sp<IQcRilAudioCallback> callback = getQcRilAudioCallback();
    if (callback != nullptr) {
        hidl_string str = hidl_string(param.string());
        Return<int32_t> ret = callback->setParameters(str);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to setParameters. Exception : %s", ret.description().c_str());
        } else {
            result = ret;
        }
    } else {
        QCRIL_LOG_ERROR("mQcRilAudioCallback == NULL");
    }

    return result;
}

android::String8 qcril_audio_impl::getParameters(android::String8 param) {
    QCRIL_LOG_INFO("QcRilAudioImpl::getParameters");
    android::String8 keyValPairs;
    sp<IQcRilAudioCallback> callback = getQcRilAudioCallback();
    if (callback != nullptr) {
        hidl_string str = hidl_string(param.string());
        Return<void> ret = callback->getParameters(str, [&](const hidl_string &results) {
            keyValPairs = android::String8(results.c_str());
            });
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to getParameters. Exception : %s", ret.description().c_str());
        }
    }
    return keyValPairs;
}

void qcril_audio_register_service(uint32_t simSlot) {
    android::status_t ret = android::OK;
    audio_service = new qcril_audio_impl();
    if (audio_service == nullptr) {
        QCRIL_LOG_ERROR("Service is null");
        return;
    }
    const std::string name = std::string("slot") + std::to_string(simSlot + 1);
    ret = audio_service->registerAsService(name);
    QCRIL_LOG_INFO("registerService: starting QcRilAudioImpl as '%s'. Status: %d",
                     name.c_str(), ret);
}

int qcril_audio_setParameters(android::String8 params) {
    return audio_service->setParameters(params);
}

android::String8 qcril_audio_getParameters(android::String8 params) {
    return audio_service->getParameters(params);
}

} // vendor
} // qti
} // hardware
} // radio
} // am
} // V1_0
} // implementation
