/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef LOC_HIDL_GEOFENCE_CLIENT_H
#define LOC_HIDL_GEOFENCE_CLIENT_H

#include <vendor/qti/gnss/1.1/ILocHidlGeofenceService.h>
#include <vendor/qti/gnss/1.0/ILocHidlGeofenceServiceCallback.h>
#include <LocationAPI.h>
#include <LocationAPIClientBase.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::android::sp;
using V1_0::ILocHidlGeofenceServiceCallback;

class LocHidlGeofenceClient : public LocationAPIClientBase
{
public:
    LocHidlGeofenceClient(const sp<ILocHidlGeofenceServiceCallback>& callback);
    virtual ~LocHidlGeofenceClient() = default;

    void onGeofenceBreachCb(GeofenceBreachNotification geofenceBreachNotification) final;
    void onGeofenceStatusCb(GeofenceStatusNotification geofenceStatusNotification) final;
    void onAddGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onRemoveGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onPauseGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;
    void onResumeGeofencesCb(size_t count, LocationError* errors, uint32_t* ids) final;

private:
    sp<ILocHidlGeofenceServiceCallback> mGnssCbIface;
};

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // LOC_HIDL_GEOFENCE_CLIENT_H
