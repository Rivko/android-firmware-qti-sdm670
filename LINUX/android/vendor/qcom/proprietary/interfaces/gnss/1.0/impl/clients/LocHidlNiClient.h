/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_HIDL_NI_CLIENT_H
#define LOC_HIDL_NI_CLIENT_H

#include <vendor/qti/gnss/1.0/ILocHidlGnssNi.h>
#include <vendor/qti/gnss/1.0/ILocHidlGnssNiCallback.h>
#include <LocationAPI.h>
#include <LocationAPIClientBase.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::sp;
using ::android::hardware::gnss::V1_0::IGnssNiCallback;

class LocHidlNiClient : public LocationAPIClientBase
{
public:
    LocHidlNiClient(const sp<ILocHidlGnssNiCallback>& callback);
    virtual ~LocHidlNiClient() = default;

    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) final;

    void gnssNiRespond(int32_t notifId,
            IGnssNiCallback::GnssUserResponseType userResponse);

private:
    sp<ILocHidlGnssNiCallback> mGnssCbIface;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // LOC_HIDL_NI_CLIENT_H
