/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_FlpClient"
#define LOG_NDEBUG 0

#include "LocHidlFlpClient.h"
#include "LocHidlUtils.h"
#include <LocationAPI.h>
#include <log_util.h>
#include <dlfcn.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

typedef void* (getLocationInterface)();
using ::android::hardware::hidl_vec;

LocHidlFlpClient::LocHidlFlpClient(
        const sp<ILocHidlFlpServiceCallback>& callback) :
                LocationAPIClientBase(), mGnssCbIface(callback) {

    ENTRY_LOG();

    mGnssInterface = nullptr;

    LocationCallbacks locationCallbacks;
    memset(&locationCallbacks, 0, sizeof(LocationCallbacks));
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.batchingCb = [this](size_t count, Location* location,
            BatchingOptions batchOptions) {
        onBatchingCb(count, location, batchOptions);
    };
    locationCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
    };
    locationCallbacks.batchingStatusCb = [this](BatchingStatusInfo batchStatusInfo,
            std::list<uint32_t> &listOfCompletedTrips) {
        onBatchingStatusCb(batchStatusInfo, listOfCompletedTrips);
    };

    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

GnssInterface* LocHidlFlpClient::getGnssInterface() {
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGD("%s]: loading libgnss.so::getGnssInterface ...", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle || (error = dlerror()) != NULL)  {
            LOC_LOGW("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGW("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            getGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

uint32_t LocHidlFlpClient::locAPIGnssDeleteAidingData(GnssAidingData& data)
{
    ENTRY_LOG();

    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    GnssInterface* gnssInterface = getGnssInterface();
    if (gnssInterface != nullptr) {
        gnssInterface->gnssDeleteAidingData(data);
        retVal = LOCATION_ERROR_SUCCESS;
    }

    return retVal;
}

void LocHidlFlpClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {

    LOC_LOGV("capabilities mask: 0x%x", capabilitiesMask);
    mCapabilitiesMask = capabilitiesMask;
}

void LocHidlFlpClient::onTrackingCb(Location location) {

    // Let's convert Location to LocHidlLocation
    LocHidlLocation gnssLocation;
    LocHidlUtils::locationToLocHidlLocation(location, gnssLocation);

    // Invoke the callback
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }
    TO_HIDL_CLIENT();
    auto r = mGnssCbIface->gnssLocationTrackingCb(gnssLocation);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlFlpClient::onBatchingCb(size_t count, Location* locations,
        BatchingOptions batchOptions) {

    // Sanity checks
    if ((int)count <= 0) {
        LOC_LOGE("Invalid count %zu", count);
        return;
    }
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    // Create a vector with all locations
    hidl_vec<LocHidlLocation> gnssLocationVec;
    gnssLocationVec.resize(count);
    for (int i = 0; i < (int)count; i++) {
        LocHidlUtils::locationToLocHidlLocation(
                locations[i], gnssLocationVec[i]);
    }
    TO_HIDL_CLIENT();
    LocHidlBatchOptions hidlbatchOptions;
    hidlbatchOptions.batchMode = (LocHidlBatchMode)batchOptions.batchingMode;

    auto r = mGnssCbIface->gnssLocationBatchingCb(hidlbatchOptions, gnssLocationVec);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlFlpClient::onBatchingStatusCb(BatchingStatusInfo batchStatusInfo,
        std::list<uint32_t> &listOfCompletedTrips)
{
    // Sanity checks
    if (mGnssCbIface == nullptr) {
        LOC_LOGE("mGnssCbIface NULL");
        return;
    }

    // Create a vector with all trip ids
    hidl_vec<uint32_t> completedTripsVec;
    completedTripsVec.resize(listOfCompletedTrips.size());
    auto it = listOfCompletedTrips.begin();
    for (int index = 0; it != listOfCompletedTrips.end(); it++, index++) {
        uint32_t tripClientId = *it;
        completedTripsVec[index] = tripClientId;
    }

    TO_HIDL_CLIENT();

    LocHidlBatchStatusInfo hidlBatchStatusInfo;
    hidlBatchStatusInfo.batchStatus = (LocHidlBatchStatus)batchStatusInfo.batchingStatus;

    auto r = mGnssCbIface->gnssBatchingStatusCb(hidlBatchStatusInfo, completedTripsVec);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}


}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
