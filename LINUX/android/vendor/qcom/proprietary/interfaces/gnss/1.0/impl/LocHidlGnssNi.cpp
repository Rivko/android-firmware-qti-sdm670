/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_GnssNi"
#define LOG_NDEBUG 0

#include "LocHidlGnssNi.h"
#include <log_util.h>
#include <LocHidlUtils.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

LocHidlGnssNi::LocHidlGnssNi(sp<IGnssNi> gnssNi) :
        mNiClient(nullptr), mGnssNi(gnssNi), mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocHidlGnssNi::~LocHidlGnssNi() {

    ENTRY_LOG();
    if (mNiClient != nullptr) {
        delete mNiClient;
        mNiClient = nullptr;
    }
}

// Method from base class ::android::hardware::gnss::V1_0::IGnssNi
Return<void> LocHidlGnssNi::setCallback(const sp<IGnssNiCallback>& /*callback*/) {

    FROM_HIDL_CLIENT();

    // We only expect setVendorCallback() invocation on LocHidlGnssNi
    LOC_LOGE("%s] Unexpected invocation !", __FUNCTION__);
    return Void();
}

// Method from base class ::android::hardware::gnss::V1_0::IGnssNi
Return<void> LocHidlGnssNi::respond(
        int32_t notifId, IGnssNiCallback::GnssUserResponseType userResponse) {

    FROM_HIDL_CLIENT();

    if (mNiClient == nullptr) {
        LOC_LOGE("IZAT API Client null !");
        return Void();
    }

    mNiClient->gnssNiRespond(notifId, userResponse);
    return Void();
}

// Methods from ::vendor::qti::gnss::V1_0::IGnssNiVendor follow.
Return<void> LocHidlGnssNi::setVendorCallback(
        const sp<ILocHidlGnssNiCallback>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    if (mNiClient != nullptr) {
        LOC_LOGE("mNiClient not null !");
        return Void();
    }
    mNiClient = new LocHidlNiClient(callback);

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([this] {
                LOC_LOGE("%s]: ILocHidlGnssNiCallback died.", __func__);
                if (mNiClient != nullptr) {
                    delete mNiClient;
                    mNiClient = nullptr;
                }
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    mCallbackIface = callback;

    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
