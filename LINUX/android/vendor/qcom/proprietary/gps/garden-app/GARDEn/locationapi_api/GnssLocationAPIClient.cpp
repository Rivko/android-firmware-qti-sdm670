/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GnssLocationAPIClient.h"
#include <loc_pla.h>
#include <log_util.h>
#include <dlfcn.h>
#include <location_interface.h>
#include <loc_cfg.h>
#include "GardenUtil.h"
#include "GnssCase.h"

namespace garden {

typedef void* (getLocationInterface)();
GnssCbBase* gGnssCbs = nullptr;

GnssLocationAPIClient::GnssLocationAPIClient():
        mControlClient(new LocationAPIControlClient()), mLocationCapabilitiesMask(0) {
}

GnssLocationAPIClient::~GnssLocationAPIClient() {
    lock_guard guard(mGnssLock);
    if (mControlClient) {
        delete mControlClient;
        mControlClient = nullptr;
    }
}

int GnssLocationAPIClient::setCallbacks(GnssCbBase* callbacks) {
    {
        lock_guard guard(mGnssLock);
        mGnssCallbacks = callbacks;
        gGnssCbs = callbacks;
    }
    // set default LocationOptions.
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = 1000;
    mLocationOptions.minDistance = 0;
    mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;

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
    locationCallbacks.gnssDataCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    if (callbacks != nullptr) {
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

        locationCallbacks.gnssDataCb = [this](GnssDataNotification gnssDataNotification) {
            onGnssDataCb(gnssDataNotification);
        };

        locationCallbacks.gnssLocationInfoCb = [this]
                (GnssLocationInfoNotification gnssLocInfoNotif) {
            onGnssLocationInfoCb(gnssLocInfoNotif);
        };
        locationCallbacks.gnssMeasurementsCb = [this]
                (GnssMeasurementsNotification gnssMeasurementsNotification) {
            onGnssMeasurementsCb(gnssMeasurementsNotification);
        };
    }
    locAPISetCallbacks(locationCallbacks);
    return 0;
}

GnssInterface* GnssLocationAPIClient::getGnssInterface() {
    static bool getGnssInterfaceFailed = false;
    if (nullptr == mGnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGD("%s]: loading libgnss.so::getGnssInterface ...", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle) {
            gardenPrint("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror()))  {
            gardenPrint("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
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

int GnssLocationAPIClient::gnssStart() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    if (nullptr != mControlClient) {
        mControlClient->locAPIEnable(LOCATION_TECHNOLOGY_TYPE_GNSS);
    }
    retVal = locAPIStartTracking(mLocationOptions);
    return retVal;
}

int GnssLocationAPIClient::gnssInjectLocation(double latitude, double longitude,
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

void GnssLocationAPIClient::gnssDeleteAidingData(LocGpsAidingData flags) {
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

int GnssLocationAPIClient::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time) {
    LOC_LOGD("%s]: (%d %d %d %d %d)", __func__,
            mode, recurrence, min_interval, preferred_accuracy, preferred_time);
    int retVal = 0;
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = min_interval;
    if (mode == LOC_GPS_POSITION_MODE_STANDALONE) {
        mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;
    }
    else if (mode == LOC_GPS_POSITION_MODE_MS_BASED) {
        mLocationOptions.mode = GNSS_SUPL_MODE_MSB;
    }
    else if (mode == LOC_GPS_POSITION_MODE_MS_ASSISTED) {
        mLocationOptions.mode = GNSS_SUPL_MODE_MSA;
    }
    return retVal;
}

void GnssLocationAPIClient::configurationUpdate(const char* config_data, int32_t length) {
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

void GnssLocationAPIClient::updateNetworkAvailability(int /*available*/, const char* /*apn*/) {
}

void GnssLocationAPIClient::onTrackingCb(Location location) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssLocationCb(location);
    }
}

void GnssLocationAPIClient::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) {
    LOC_LOGD("%s]: (id: %d)", __FUNCTION__, id);
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks == nullptr) {
        gardenPrint("%s]: mGnssNiCbIface is nullptr", __FUNCTION__);
        return;
    }
    mGnssCallbacks->gnssNiNotifyCb(id, gnssNiNotification);
}

void GnssLocationAPIClient::onGnssSvCb(GnssSvNotification gnssSvNotification) {
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, gnssSvNotification.count);
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssSvStatusCb(gnssSvNotification);
    }
}

void GnssLocationAPIClient::onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification) {
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssNmeaCb(
            static_cast<LocGpsUtcTime>(gnssNmeaNotification.timestamp),
            gnssNmeaNotification.nmea, gnssNmeaNotification.length);
    }
}

void GnssLocationAPIClient::onGnssDataCb(GnssDataNotification gnssDataNotification) {
    LOC_LOGD("%s]:", __FUNCTION__);
    for (int sig = GNSS_LOC_SIGNAL_TYPE_GPS_L1CA;
        sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
        if (GNSS_LOC_DATA_JAMMER_IND_BIT ==
            (gnssDataNotification.gnssDataMask[sig] & GNSS_LOC_DATA_JAMMER_IND_BIT)) {
            LOC_LOGV("jammer[%d]=%f", sig, gnssDataNotification.jammerInd[sig]);
        }
        if (GNSS_LOC_DATA_AGC_BIT ==
            (gnssDataNotification.gnssDataMask[sig] & GNSS_LOC_DATA_AGC_BIT)) {
            LOC_LOGV("agc[%d]=%f", sig, gnssDataNotification.agc[sig]);
        }
    }

    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssDataCb(gnssDataNotification);
    }
}

void GnssLocationAPIClient::onGnssLocationInfoCb(GnssLocationInfoNotification gnssLocInfoNotif) {
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, gnssLocInfoNotif.flags);
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssLocInfoNotifCb(gnssLocInfoNotif);
    }
}

void GnssLocationAPIClient::onGnssMeasurementsCb(
        GnssMeasurementsNotification gnssMeasurementsNotification)
{
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssMeasurementsCb(gnssMeasurementsNotification);
    }
}

void GnssLocationAPIClient::onStartTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mGnssLock);
    if (error == LOCATION_ERROR_SUCCESS && mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_ENGINE_ON);
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_SESSION_BEGIN);
    }
}

void GnssLocationAPIClient::onStopTrackingCb(LocationError error) {
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    lock_guard guard(mGnssLock);
    if (error == LOCATION_ERROR_SUCCESS && mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_SESSION_END);
        mGnssCallbacks->gnssStatusCb(LOC_GPS_STATUS_ENGINE_OFF);
    }
}

void GnssLocationAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
    lock_guard guard(mGnssLock);
    if (mGnssCallbacks != nullptr) {
        mGnssCallbacks->gnssSetCapabilitiesCb(capabilitiesMask);
    }
}
} //namespace garden
