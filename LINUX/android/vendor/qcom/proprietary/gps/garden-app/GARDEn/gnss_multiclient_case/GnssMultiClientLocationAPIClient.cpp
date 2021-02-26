/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_LocAPIClnt"

#include "GnssMultiClientLocationAPIClient.h"
#include <dlfcn.h>
#include <location_interface.h>
#include <loc_cfg.h>
#include "GardenUtil.h"
#include "GnssCase.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {

typedef void* (getLocationInterface)();

GnssMultiClientLocationAPIClient::GnssMultiClientLocationAPIClient():
        mControlClient(nullptr), mGnssInterface(nullptr),
        mLocationCapabilitiesMask(0), mGnssCallbacks(nullptr) {
}

GnssMultiClientLocationAPIClient::~GnssMultiClientLocationAPIClient() {
    lock_guard guard(mGnssLock);
    if (mControlClient) {
        delete mControlClient;
        mControlClient = nullptr;
    }
}

int GnssMultiClientLocationAPIClient::createControlClient()
{
    if (nullptr != mControlClient) {
        PRINTERROR("Control Client already exists !!");
        return -1;
    }

    mControlClient = new LocationAPIControlClient();
    if (NULL == mControlClient) {
        gardenPrint("Control Client creation error");
        return -1;
    }

    return 0;
}

int GnssMultiClientLocationAPIClient::setCallbacks(GnssCbBase* callbacks) {
    lock_guard guard(mGnssLock);
    mGnssCallbacks = callbacks;

    // set default LocationOptions.
    memset(&mTrackingOptions, 0, sizeof(LocationOptions));
    mTrackingOptions.size = sizeof(LocationOptions);
    mTrackingOptions.minInterval = 1000;
    mTrackingOptions.minDistance = 0;
    mTrackingOptions.mode = GNSS_SUPL_MODE_STANDALONE;

    //update Callbacks
    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);
    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    if (nullptr != callbacks) {
        locationCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
        };
        locationCallbacks.gnssNiCb = [this](uint32_t id, GnssNiNotification gnssNiNotification) {
            onGnssNiCb(id, gnssNiNotification);
        };
        locationCallbacks.gnssSvCb = [this](GnssSvNotification gnssSvNotification) {
            onGnssSvCb(gnssSvNotification);
        };

        locationCallbacks.gnssNmeaCb = [this](GnssNmeaNotification gnssNmeaNotification) {
            onGnssNmeaCb(gnssNmeaNotification);
        };
    }
    locAPISetCallbacks(locationCallbacks);
    return 0;
}

GnssInterface* GnssMultiClientLocationAPIClient::getGnssInterface() {
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGD("%s]: loading libgnss.so::getGnssInterface ...", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle) {
            gardenPrint("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror())) {
            gardenPrint("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if (NULL != (error = dlerror()))  {
                gardenPrint("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
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

int GnssMultiClientLocationAPIClient::gnssStart() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    retVal = locAPIStartTracking(mTrackingOptions);
    return retVal;
}

int GnssMultiClientLocationAPIClient::gnssInjectLocation(double latitude, double longitude,
        float accuracy) {
    ENTRY_LOG_CALLFLOW();
    GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr != gnssInterface) {
        gnssInterface->injectLocation(latitude, longitude, accuracy);
        return 0;
    } else {
        return -1;
    }
}

void GnssMultiClientLocationAPIClient::gnssDeleteAidingData(LocGpsAidingData flags) {
    LOC_LOGD("%s]: (%02x)", __func__, flags);
    if (mControlClient == nullptr) return;
    GnssAidingData data;
    memset(&data, 0, sizeof (GnssAidingData));
    data.sv.svTypeMask = GNSS_AIDING_DATA_SV_TYPE_GPS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT;

    if (flags == LOC_GPS_DELETE_ALL) {
        data.deleteAll = true;
    }
    else {
        if (flags & LOC_GPS_DELETE_EPHEMERIS)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
        if (flags & LOC_GPS_DELETE_ALMANAC) data.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC_BIT;
        if (flags & LOC_GPS_DELETE_POSITION)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION_BIT;
        if (flags & LOC_GPS_DELETE_TIME) data.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
        if (flags & LOC_GPS_DELETE_IONO) data.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE_BIT;
        if (flags & LOC_GPS_DELETE_UTC) data.common.mask |= GNSS_AIDING_DATA_COMMON_UTC_BIT;
        if (flags & LOC_GPS_DELETE_HEALTH) data.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH_BIT;
        if (flags & LOC_GPS_DELETE_SVDIR) data.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION_BIT;
        if (flags & LOC_GPS_DELETE_SVSTEER) data.sv.svMask |= GNSS_AIDING_DATA_SV_STEER_BIT;
        if (flags & LOC_GPS_DELETE_SADATA) data.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA_BIT;
        if (flags & LOC_GPS_DELETE_RTI) data.common.mask |= GNSS_AIDING_DATA_COMMON_RTI_BIT;
        if (flags & LOC_GPS_DELETE_CELLDB_INFO)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB_BIT;
    }
    mControlClient->locAPIGnssDeleteAidingData(data);
}

int GnssMultiClientLocationAPIClient::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time) {
    LOC_LOGD("%s]: (%d %d %d %d %d)", __func__,
            mode, recurrence, min_interval, preferred_accuracy, preferred_time);
    int retVal = 0;
    memset(&mTrackingOptions, 0, sizeof(LocationOptions));
    mTrackingOptions.size = sizeof(LocationOptions);
    mTrackingOptions.minInterval = min_interval;
    mTrackingOptions.minDistance = preferred_accuracy;
    if (mode == LOC_GPS_POSITION_MODE_STANDALONE) {
        mTrackingOptions.mode = GNSS_SUPL_MODE_STANDALONE;
    }
    else if (mode == LOC_GPS_POSITION_MODE_MS_BASED) {
        mTrackingOptions.mode = GNSS_SUPL_MODE_MSB;
    }
    else if (mode == LOC_GPS_POSITION_MODE_MS_ASSISTED) {
        mTrackingOptions.mode = GNSS_SUPL_MODE_MSA;
    }
    return retVal;
}

void GnssMultiClientLocationAPIClient::configurationUpdate(const char* config_data, int32_t length) {
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
    const int n = 10;
    uint8_t flags[n];
    memset(&flags, 0, sizeof(uint8_t) * n);
    GnssConfig data;
    memset(&data, 0, sizeof(GnssConfig));
    data.size = sizeof(GnssConfig);

    const loc_param_s_type gnssConfTable[] =
    {
        {"GPS_LOCK",                            &data.gpsLock,                      flags+0, 'n'},
        {"SUPL_VER",                            &data.suplVersion,                  flags+1, 'n'},
        {"LPP_PROFILE",                         &data.lppProfile,                   flags+3, 'n'},
        {"LPPE_CP_TECHNOLOGY",                  &data.lppeControlPlaneMask,         flags+4, 'n'},
        {"LPPE_UP_TECHNOLOGY",                  &data.lppeUserPlaneMask,            flags+5, 'n'},
        {"A_GLONASS_POS_PROTOCOL_SELECT",       &data.aGlonassPositionProtocolMask, flags+6, 'n'},
        {"USE_EMERGENCY_PDN_FOR_EMERGENCY_SUPL",&data.emergencyPdnForEmergencySupl, flags+7, 'n'},
        {"SUPL_ES",                             &data.suplEmergencyServices,        flags+8, 'n'},
        {"SUPL_MODE",                           &data.suplModeMask,                 flags+9, 'n'},
    };
    UTIL_UPDATE_CONF(config_data, length, gnssConfTable);

    for (int i = 0; i < n; i++) {
        if (flags[i] != 0)
            data.flags |= (0x1 << i);
    }
    mControlClient->locAPIGnssUpdateConfig(data);
}

void GnssMultiClientLocationAPIClient::updateNetworkAvailability(int /*available*/, const char* /*apn*/) {
}

void GnssMultiClientLocationAPIClient::onTrackingCb(Location location) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);
    lock_guard guard(mGnssLock);
    if (nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssLocationCb(location);
    }
}

void GnssMultiClientLocationAPIClient::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) {
    LOC_LOGD("%s]: (id: %d)", __FUNCTION__, id);
    lock_guard guard(mGnssLock);
    if (nullptr == mGnssCallbacks) {
        gardenPrint("%s]: mGnssNiCbIface is nullptr", __FUNCTION__);
        return;
    }
    mGnssCallbacks->gnssNiNotifyCb(id, gnssNiNotification);
}

void GnssMultiClientLocationAPIClient::onGnssSvCb(GnssSvNotification gnssSvNotification) {
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, gnssSvNotification.count);
    lock_guard guard(mGnssLock);
    if (nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssSvStatusCb(gnssSvNotification);
    }
}

void GnssMultiClientLocationAPIClient::onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification) {
    lock_guard guard(mGnssLock);
    if (nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssNmeaCb(
            static_cast<LocGpsUtcTime>(gnssNmeaNotification.timestamp),
            gnssNmeaNotification.nmea, gnssNmeaNotification.length);
    }
}

void GnssMultiClientLocationAPIClient::onStartTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mGnssLock);
    if (LOCATION_ERROR_SUCCESS == error && nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_ENGINE_ON);
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_SESSION_BEGIN);
    }
}

void GnssMultiClientLocationAPIClient::onStopTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mGnssLock);
    if (LOCATION_ERROR_SUCCESS == error && nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_SESSION_END);
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_ENGINE_OFF);
    }
}

void GnssMultiClientLocationAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
    lock_guard guard(mGnssLock);
    if (nullptr != mGnssCallbacks) {
        mGnssCallbacks->gnssSetCapabilitiesCb(capabilitiesMask);
    }
}
} //namespace garden
