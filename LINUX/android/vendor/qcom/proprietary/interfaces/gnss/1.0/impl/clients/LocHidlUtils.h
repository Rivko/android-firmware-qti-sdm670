/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_HIDL_UTILS_H
#define LOC_HIDL_UTILS_H

#include <vendor/qti/gnss/1.0/types.h>
#include <LocationAPI.h>
#include <IzatLocation.h>
#include <IzatRequest.h>
#include <DataItemId.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

#define TO_HIDL_CLIENT()   ALOGV("[%s][%d] [HS] =>>>> [HC]", __func__, __LINE__)
#define FROM_HIDL_CLIENT() ALOGV("[%s][%d] [HS] <<<<= [HC]", __func__, __LINE__)

using namespace izat_manager;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;
using ::android::wp;

struct LocHidlDeathRecipient : hidl_death_recipient {
    LocHidlDeathRecipient(std::function<void(void)> fun) : mFun(fun), mPeerDied(false) {
    }
    LocHidlDeathRecipient() = delete;
    ~LocHidlDeathRecipient() = default;
    void registerToPeer(const sp<::android::hidl::base::V1_0::IBase>& peer, uint64_t cookie);
    void unregisterFromPeer(const sp<::android::hidl::base::V1_0::IBase>& peer);
    inline bool peerDied() { return mPeerDied; }
    virtual void serviceDied(uint64_t cookie,
            const ::android::wp<::android::hidl::base::V1_0::IBase>& who) override;
    std::function<void(void)> mFun;
    bool mPeerDied;
};

class LocHidlUtils {

public:
    static void locationToLocHidlLocation(
            const Location& location, LocHidlLocation& gnssLocation);

    static void izatLocationToGnssIzatLocation(
            const IzatLocation& izatLocation,
            LocHidlIzatLocation& gnssIzatLocation);

    inline static LocHidlIzatStreamType izatStreamToGnssIzatStream(
            IzatStreamType ist) {
        return static_cast<LocHidlIzatStreamType>(ist);
    }

    inline static IzatStreamType gnssIzatStreamToIzatStreamType(
            LocHidlIzatStreamType gist) {
        return static_cast<IzatStreamType>(gist);
    }

    inline static LocHidlIzatProviderStatus izatStatusToGnssIzatStatus(
            izat_manager::IzatProviderStatus ips) {
        return static_cast<LocHidlIzatProviderStatus>(ips);
    }

    static DataItemId translateToDataItemId(LocHidlSubscriptionDataItemId id);

    static LocHidlSubscriptionDataItemId
    translateToSubscriptionDataItemId(DataItemId id);

    static void izatRequestToGnssIzatRequest(
            const IzatRequest& ir, LocHidlIzatRequest& gir);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // LOC_HIDL_UTILS_H
