/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#ifndef __ULP_ENGINE_H__
#define __ULP_ENGINE_H__

#ifndef LOG_NDEBUG
#define LOG_NDEBUG 0
#endif
#ifndef LOG_TAG
#define LOG_TAG "LocSvc_ulp_log"
#endif
#include <new>
#include <log_util.h>
#include <loc_pla.h>
#include <ctype.h>
#include <stdbool.h>
#include <ulp_log.h>
#include <ulp_service.h>
#include <LocationAPI.h>
#include <inttypes.h>
//Message ID Definitions
enum ulp_eng_msg_ids_t {
    // Message is sent by GPS HAL layer to add/remove unique request criteria
    ULP_MSG_UPDATE_CRITERIA = 0x600,

    // Message is sent by Fused Location Provider to request ULP to start producing position fixes
    ULP_MSG_START_FIX,

    // Message is sent by Fused Location Provider
    // to request ULP to stop producing position fixes
    ULP_MSG_STOP_FLP_FIX,
    // Sent by GPS Location Provider to request ULP to stop
    ULP_MSG_STOP_GNSS_FIX,
    // Message is sent by Android framework(GpsLocationProvider)
    // to inject phone context setting  include initial phone context setting and subsequent changes
    ULP_MSG_INJECT_PHONE_CONTEXT_SETTINGS,

    // Message is sent by network provider to INJECT the position in UlpNetworkPositionReport format
    ULP_MSG_INJECT_NETWORK_POSITION,

    // Message is sent by QUIPC provider in order to report the position in GpsPosition format with QUIPC status
    ULP_MSG_REPORT_QUIPC_POSITION,

    // Message is sent by QUIPC module in order to request some info from ULP
    ULP_MSG_REQUEST_COARSE_POSITION,

    // Message is sent to ULP module to re-evaluate its subsystems
    ULP_MSG_MONITOR,

    // Message is sent by Android framework (GpsLocationProvider)
    // to inject the raw command
    ULP_MSG_INJECT_RAW_COMMAND,

    ULP_MSG_SET_POSITION_MODE,
    ULP_MSG_REPORT_POSITION,
    ULP_MSG_REPORT_SV,
    ULP_MSG_REPORT_GNSS_STATUS,
    ULP_MSG_SYSTEM_UPDATE,
    ULP_MSG_REPORT_BATCHING_SESSION,
    ULP_MSG_REPORT_POSITIONS,

    //Message used by LocAmtProxy to start DR Fix session
    ULP_MSG_START_DR_FIX,
    //Message used by LocAmtProxy to stop DR Fix session
    ULP_MSG_STOP_DR_FIX,
    //Message used by LocAmtProxy to update Fix criteria for DR session
    ULP_MSG_SET_DR_POSITION_MODE,
    //Message used by LocAmtProxy to report DR Fix
    ULP_MSG_SEND_POSITION_EXTERNAL_REQ,
    //Message used by LocAmtProxy to report DR Fix
    ULP_MSG_REPORT_DR_POSITION,
    ULP_MSG_REPORT_SV_MEASUREMENT,
    ULP_MSG_REPORT_SV_POLYNOMIAL,
    ULP_MSG_DELETE_AIDING_DATA,
    ULP_MSG_REPORT_NMEA,
    ULP_MSG_SEND_SV_EXTERNAL_REQ,
    ULP_MSG_REPORT_DR_SV,

    // Last ULP MSG
    ULP_MSG_LAST = 0x700
};

struct ulp_msg {
    const void* owner;
    const int msgid;
    inline ulp_msg(void* instance, int id) :
        owner(instance), msgid(id)
    {
        LOC_LOGV("creating msg %s", ulp_get_ulp_msg_name(msgid));
        LOC_LOGV("creating msg ox%x", msgid);
    }
    virtual ~ulp_msg()
    {
        LOC_LOGV("deleting msg %s (0x%x)", ulp_get_ulp_msg_name(msgid), msgid);
    }
};

struct ulp_msg_update_criteria : public ulp_msg {
    const UlpLocationCriteria locationCriteria;
    inline ulp_msg_update_criteria (void* instance, UlpLocationCriteria criteria) :
        ulp_msg(instance, ULP_MSG_UPDATE_CRITERIA),
        locationCriteria(criteria)
    {
        LOC_LOGV("location criteria: aciton %d\n  valid mask: %d\n"
             "provider source: %d\n accuracy %d\n recurrence type %d\n"
             "min interval %d\n power consumption %d\n intermediate pos %d\n "
             "emergency_request: %d\n",
             locationCriteria.action,
             locationCriteria.valid_mask,
             locationCriteria.provider_source,
             locationCriteria.preferred_horizontal_accuracy,
             locationCriteria.recurrence_type,
             locationCriteria.min_interval,
             locationCriteria.preferred_power_consumption,
             locationCriteria.intermediate_pos_report_enabled,
             locationCriteria.emergency_request);
    }
};

struct ulp_msg_inject_raw_command : public ulp_msg {
    const char* rawCommand;
    const int rawCommandLength;
    inline ulp_msg_inject_raw_command (void* instance, char* command, int length) :
        ulp_msg(instance, ULP_MSG_INJECT_RAW_COMMAND),
        rawCommand(new char[length]),
        rawCommandLength(length)
    {
      if (nullptr == rawCommand) {
          LOC_LOGE("new failed to allocate for rawCommand");
          return;
      }
      memcpy((void*)rawCommand, (void*)command, length);
      LOC_LOGV("inject raw command: command %s\n  command length: %d\n ",
             rawCommand,
             rawCommandLength);
    }

    inline ~ulp_msg_inject_raw_command()
    {
        delete[] rawCommand;
    }
};

struct ulp_msg_inject_phone_context_settings : public ulp_msg {
    const UlpPhoneContextSettings phoneSetting;
    inline ulp_msg_inject_phone_context_settings(void* instance, UlpPhoneContextSettings setting) :
        ulp_msg(instance, ULP_MSG_INJECT_PHONE_CONTEXT_SETTINGS),
        phoneSetting(setting)
    {
        LOC_LOGV("context type: %d\n  gps enabled: %d\n"
                 "network position available %d\n wifi setting enabled %d\n"
                 "battery charging %d is_agps_setting_enabled %d"
                 "is_enh_location_services_enabled %d\n",
             phoneSetting.context_type,
             phoneSetting.is_gps_enabled,
             phoneSetting.is_network_position_available,
             phoneSetting.is_wifi_setting_enabled,
             phoneSetting.is_battery_charging,
             phoneSetting.is_agps_enabled,
             phoneSetting.is_enh_location_services_enabled);
    }
};

struct ulp_msg_inject_network_position : public ulp_msg {
    const UlpNetworkPositionReport networkPosition;
    inline ulp_msg_inject_network_position(void* instance, UlpNetworkPositionReport networkPos) :
        ulp_msg(instance, ULP_MSG_INJECT_NETWORK_POSITION),
        networkPosition(networkPos)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n longitude: %f\n"
                 "accuracy %f",
             networkPosition.valid_flag,
             networkPosition.position.pos_source,
             networkPosition.position.latitude,
             networkPosition.position.longitude,
             networkPosition.position.HEPE);
    }
};

struct ulp_msg_report_quipc_position : public ulp_msg {
    const UlpLocation location;
    const int  quipc_error_code;
    inline ulp_msg_report_quipc_position(void* instance, UlpLocation &loc,
                                         int  quipc_err) :
        ulp_msg(instance, ULP_MSG_REPORT_QUIPC_POSITION),
        location(loc), quipc_error_code(quipc_err)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n"
                 "altitude: %f\n  speed: %f\n  bearing: %f\n  accuracy: %f\n"
                 "timestamp: %" PRIu64 "\n  Quipc error: %d",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, quipc_error_code);
    }
};

struct ulp_msg_position_mode : public ulp_msg {
    const LocPosMode pMode;
    inline ulp_msg_position_mode(void* instance,
                                     LocPosMode &mode) :
        ulp_msg(instance, ULP_MSG_SET_POSITION_MODE),
        pMode(mode)
    {
        pMode.logv();
    }
};

struct ulp_msg_dr_position_mode : public ulp_msg {
    const LocPosMode pMode;
    inline ulp_msg_dr_position_mode(void* instance,
                                     LocPosMode &mode) :
        ulp_msg(instance, ULP_MSG_SET_DR_POSITION_MODE),
        pMode(mode)
    {
        pMode.logv();
    }
};

struct ulp_msg_dr_delete_aiding_data : public ulp_msg {
    const LocGpsAidingData delete_aiding_data;
    inline ulp_msg_dr_delete_aiding_data(void* instance,
                                     LocGpsAidingData aidingData) :
        ulp_msg(instance, ULP_MSG_DELETE_AIDING_DATA),
        delete_aiding_data(aidingData)
    {
        LOC_LOGV("ULP_MSG: Delete Aiding Data 0x%x", delete_aiding_data);
    }
};

struct ulp_msg_report_position : public ulp_msg {
    const UlpLocation location;
    const GpsLocationExtended locationExtended;
    const enum loc_sess_status status;
    const LocPosTechMask technology_mask;

    inline ulp_msg_report_position(void* instance,
                                   const UlpLocation &loc,
                                   const GpsLocationExtended &locExtended,
                                   enum loc_sess_status st,
                                   LocPosTechMask technology = LOC_POS_TECH_MASK_DEFAULT) :
        ulp_msg(instance, ULP_MSG_REPORT_POSITION),
        location(loc),
        locationExtended(locExtended),
        status(st),
        technology_mask(technology)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n  altitude: %f\n"
                "  speed: %f\n  bearing: %f\n  accuracy: %f\n  timestamp: %" PRIu64 "\n "
                "  Session status: %d\n Technology mask: %u",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, status, technology_mask);
    }
};

struct ulp_msg_report_positions : public ulp_msg {
    UlpLocation* locations;
    GpsLocationExtended* extendedLocations;
    uint32_t* techMasks;
    const size_t num_locations;

    inline ulp_msg_report_positions(void * instance,
                                    const UlpLocation* locs,
                                    const GpsLocationExtended* locsExtended,
                                    const uint32_t* techs,
                                    const size_t number_of_locations) :
    ulp_msg(instance, ULP_MSG_REPORT_POSITIONS),
    locations(new (std::nothrow) UlpLocation[number_of_locations]),
    extendedLocations(new (std::nothrow) GpsLocationExtended[number_of_locations]),
    techMasks(new (std::nothrow) uint32_t[number_of_locations]),
    num_locations(number_of_locations) {

        if (locations != NULL && locs != NULL &&
            extendedLocations != NULL && locsExtended != NULL) {

            // Reset allocated memory buffers
            memset(locations, 0, sizeof(UlpLocation)*number_of_locations);
            memset(extendedLocations, 0, sizeof(GpsLocationExtended)*number_of_locations);

            for (size_t i = 0; i < num_locations; i++) {
                locations[i] = locs[i];
                extendedLocations[i] = locsExtended[i];
                // technology mask in LocPosTechMask format
                if (techMasks != NULL && techs != NULL) {
                    techMasks[i] = techs[i];
                }
            }
        }
        print_msg();
    }

    inline ~ulp_msg_report_positions() {
        delete [] locations;
        delete [] extendedLocations;
        delete [] techMasks;
    }

    inline void print_msg() {

        if(locations == NULL) {
            LOC_LOGV("ulp_msg_report_positions Locations is NULL");
            return;
        }

        LOC_LOGV("ulp_msg_report_positions message info: BEGIN");

        LOC_LOGV("Number of locations: %zu",num_locations);

        for(size_t i = 0; i < num_locations; i++) {
            LOC_LOGV("Location number %zu:",i+1);
            LOC_LOGV("Source[%d] : Flags[%d]",
                     locations[i].position_source,
                     locations[i].gpsLocation.flags);

            LOC_LOGV("Lat[%f] : Lon[%f] : Alt[%f] : Speed[%f]: Bearing[%f]:"
                     " Accuracy[%f]: Timestamp[%" PRIu64 "] : Vert_unc[%f]\n"
                     " : Speed_unc[%f] : Heading_unc[%f]"
                     " : Horizontal_reliability[%d] : Vertical_reliability[%d]"
                     " : TechMasks[%d]",
                      locations[i].gpsLocation.latitude,
                      locations[i].gpsLocation.longitude,
                      locations[i].gpsLocation.altitude,
                      locations[i].gpsLocation.speed,
                      locations[i].gpsLocation.bearing,
                      locations[i].gpsLocation.accuracy,
                      locations[i].gpsLocation.timestamp,
                      extendedLocations[i].vert_unc,
                      extendedLocations[i].speed_unc,
                      extendedLocations[i].bearing_unc,
                      extendedLocations[i].horizontal_reliability,
                      extendedLocations[i].vertical_reliability,
                      techMasks[i]);
        }

        LOC_LOGV("ulp_msg_report_positions message info: END");
    }
};

struct ulp_msg_report_batching_session : public ulp_msg {
    const LocationOptions options;
    const bool active;
    inline ulp_msg_report_batching_session(void * instance,
                                           const LocationOptions& opt,
                                           bool act):
        ulp_msg(instance, ULP_MSG_REPORT_BATCHING_SESSION),
        options(opt),
        active(act)
    {
        LOC_LOGV("%s:%d]",__func__, __LINE__);
        LOC_LOGV("minInterval: %u\n minDistance: %u\n mode: %u\n active: %u\n",
            options.minInterval,
            options.minDistance,
            options.mode,
            active);
    }
};

struct ulp_msg_send_position_external : public ulp_msg {
    const UlpLocation location;
    const GpsLocationExtended locationExtended;
    const enum loc_sess_status status;
    const LocPosTechMask technology_mask;
    inline ulp_msg_send_position_external(void* instance, const UlpLocation &loc,
                                          const GpsLocationExtended &locExtended,
                                          enum loc_sess_status st) :
        ulp_msg(instance, ULP_MSG_SEND_POSITION_EXTERNAL_REQ),
        location(loc), locationExtended(locExtended),
        status(st), technology_mask(LOC_POS_TECH_MASK_DEFAULT)
    {
        LOC_LOGV("PositionExternal latitude: %f  longitude: %f | altitude: %f | TechMask: %u",
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude,technology_mask);
    }
    inline ulp_msg_send_position_external(void* instance, const UlpLocation &loc,
                                          const GpsLocationExtended &locExtended,
                                       enum loc_sess_status st, LocPosTechMask technology) :
        ulp_msg(instance, ULP_MSG_SEND_POSITION_EXTERNAL_REQ),
        location(loc), locationExtended(locExtended),
        status(st), technology_mask(technology)
    {
        LOC_LOGV("PositionExternal latitude: %f  longitude: %f | altitude: %f | TechMask: %u",
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, technology_mask);
    }
};


struct ulp_msg_report_dr_position : public ulp_msg {
    const UlpLocation location;
    const GpsLocationExtended locationExtended;
    const enum loc_sess_status status;
    const LocPosTechMask technology_mask;
    inline ulp_msg_report_dr_position(void* instance, UlpLocation &loc,
                                      GpsLocationExtended &locExtended,
                                      enum loc_sess_status st) :
        ulp_msg(instance, ULP_MSG_REPORT_DR_POSITION),
        location(loc),locationExtended(locExtended), status(st), technology_mask(LOC_POS_TECH_MASK_DEFAULT)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n  altitude: %f\n "
                "speed: %f\n bearing: %f\n  accuracy: %f\n  timestamp: %" PRIu64 "\n  "
                " Session status: %d\n Technology mask: %u",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, status, technology_mask);
    }
    inline ulp_msg_report_dr_position(void* instance, UlpLocation &loc,GpsLocationExtended &locExtended,
                                       enum loc_sess_status st, LocPosTechMask technology) :
        ulp_msg(instance, ULP_MSG_REPORT_DR_POSITION),
        location(loc), locationExtended(locExtended),status(st), technology_mask(technology)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n  altitude: %f\n "
                "speed: %f\n bearing: %f\n  accuracy: %f\n  timestamp: %" PRIu64 "\n  "
                "  Session status: %d\n Technology mask: %u",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, status, technology_mask);
    }
};

struct ulp_msg_report_sv : public ulp_msg {
    const GnssSvNotification svNotify;
    const LocSvInfoSource svSource;
    inline ulp_msg_report_sv(void* instance,
                             const GnssSvNotification& sv,
                             const LocSvInfoSource sv_source) :
        ulp_msg(instance, ULP_MSG_REPORT_SV), svNotify(sv), svSource(sv_source)
    {
        LOC_LOGV("%s:%d] ulp_msg_report_sv",__func__, __LINE__);
    }
};

struct ulp_msg_send_sv_external : public ulp_msg {
    const GnssSvNotification svNotify;
    inline ulp_msg_send_sv_external(void* instance, const GnssSvNotification& sv) :
        ulp_msg(instance, ULP_MSG_SEND_SV_EXTERNAL_REQ), svNotify(sv)
    {
        LOC_LOGV("%s:%d] ulp_msg_send_sv_external",__func__, __LINE__);
    }
};

struct ulp_msg_report_sv_measurement : public ulp_msg {
    const GnssSvMeasurementSet svMeasurementSet;

    inline ulp_msg_report_sv_measurement(void* instance, GnssSvMeasurementSet &svMeasSet) :
        ulp_msg(instance, ULP_MSG_REPORT_SV_MEASUREMENT), svMeasurementSet(svMeasSet)
    {
        LOC_LOGV("Ulp Msg - Num of SVs: %d \n", svMeasurementSet.gnssMeas.numSvs);
    }
};

struct ulp_msg_report_sv_polynomial : public ulp_msg {
    const GnssSvPolynomial svPolynomial;

    inline ulp_msg_report_sv_polynomial(void* instance, GnssSvPolynomial &svPoly) :
        ulp_msg(instance, ULP_MSG_REPORT_SV_POLYNOMIAL), svPolynomial(svPoly)
    {
        LOC_LOGV("Ulp Msg - SV Poly: %d \n", svPoly.gnssSvId);
    }
};


struct ulp_msg_report_gnss_status : public ulp_msg {
    const LocGpsStatusValue engStatus;
    inline ulp_msg_report_gnss_status(void* instance, LocGpsStatusValue status) :
        ulp_msg(instance, ULP_MSG_REPORT_GNSS_STATUS), engStatus(status)
    {
        LOC_LOGV("%s:%d]:Engine status: %d\n",
                 __func__, __LINE__, (int)status);
    }
};
struct ulp_msg_report_nmea: public ulp_msg {
    UlpNmea nmeaStr;
    inline ulp_msg_report_nmea(void* instance, const char* nmea, int length) :
        ulp_msg(instance, ULP_MSG_REPORT_NMEA)
    {
        memset(&nmeaStr, 0, sizeof(nmeaStr));
        nmeaStr.len = length;
        strlcpy(nmeaStr.nmea_str, nmea, sizeof(nmeaStr.nmea_str));
    }
};

struct ulp_msg_system_update : public ulp_msg {
    const UlpSystemEvent systemEvent;
    inline ulp_msg_system_update (void* instance, UlpSystemEvent event) :
        ulp_msg(instance, ULP_MSG_SYSTEM_UPDATE),
        systemEvent(event)
    {
        LOC_LOGV("ulp_msg_system_update: event :%d ",event);
    }
};

/** Represents the standard ulp module interface. */
typedef struct {
    /** set to sizeof(ulpInterface) */
    size_t   size;

    /**
     * Starts the libulp module. 0: success
     */
    int   (*init)(UlpEngineCallbacks* pEngineCallbacks,
                  UlpNetworkLocationCallbacks* pNetworkLocationCallbacks,
                  UlpPhoneContextCallbacks* pPhoneContextCallbacks);
    /** Get a pointer to extension information. */
    const void* (*get_extension)(const char* name);

}ulpInterface;

typedef const ulpInterface* (get_ulp_interface) (void);


//int loc_update_criteria(UlpLocationCriteria criteria);
//const void* loc_get_extension(const char* name);


#endif // __ULP_ENGINE_H__
