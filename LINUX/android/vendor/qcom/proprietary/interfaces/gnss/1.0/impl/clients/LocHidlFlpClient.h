/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_HIDL_FLP_CLIENT_H
#define LOC_HIDL_FLP_CLIENT_H

#include <vendor/qti/gnss/1.0/ILocHidlFlpService.h>
#include <vendor/qti/gnss/1.0/ILocHidlFlpServiceCallback.h>
#include <LocationAPI.h>
#include <location_interface.h>
#include <LocationAPIClientBase.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::sp;

class LocHidlFlpClient : public LocationAPIClientBase
{
public:
    LocHidlFlpClient(const sp<ILocHidlFlpServiceCallback>& callback);
    virtual ~LocHidlFlpClient() = default;

    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onTrackingCb(Location location) final;
    void onBatchingCb(size_t count, Location* locations, BatchingOptions BatchingOptions) final;
    void onBatchingStatusCb(BatchingStatusInfo batchingStatus, std::list<uint32_t> &listOfCompletedTrips) final;

    uint32_t locAPIGnssDeleteAidingData(GnssAidingData& data);

private:
    GnssInterface* getGnssInterface();

public:
    LocationCapabilitiesMask mCapabilitiesMask;

private:
    sp<ILocHidlFlpServiceCallback> mGnssCbIface;
    GnssInterface* mGnssInterface;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // LOC_HIDL_FLP_CLIENT_H
