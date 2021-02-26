/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef FUSED_LOCATION_EXTENDED_H
#define FUSED_LOCATION_EXTENDED_H

/** Flp invalid hardware request id*/
#define FLP_INVALID_HW_ID                       0

/** Default ID for dummy DBT session*/
#define FLP_TRACKING_DEFAULT_ID                 1

/** Default ID for dummy DBT session*/
#define FLP_DBT_DEFAULT_ID                      1

/** Default time interval for flp request.
    Set it to the maximum of int64_t*/
#define FLP_REQUEST_TIME_PARAMETER_INVALID      9223372036854775807LL
/** Default distance interval for flp request.
    Set it to the maximum of uint32_t*/
#define FLP_REQUEST_DISTANCE_PARAMETER_INVALID  0xFFFFFFFF

/** This cap is used to decide the FLP session cache
size on AP. If the BATCH_SIZE in flp.conf is less than
FLP_AP_BATCHING_SIZE_CAP, FLP session cache size will
be twice the BATCH_SIZE defined in flp.conf. Otherwise,
FLP session cache size will be equal to the BATCH_SIZE.*/
#define FLP_AP_BATCHING_SIZE_CAP                40

#define FLP_OPERATION_SUCCEESS                  1
#define FLP_OPERATION_FAILURE                   0

/** flp extended batching flags*/
#define FLP_EXT_BATCHING_ON_FULL                0x0000001
#define FLP_EXT_BATCHING_ON_FIX                 0x0000002
#define FLP_EXT_BATCHING_ON_TRIP_COMPLETED      0x0000003

/** Location Batching Feature Supported Mask, indicating
    the various feature that is supported. */
typedef uint32_t LBFeatureSupportedMask;
#define FLP_TIME_BASED_BATCHING_IS_SUPPORTED      (1U<<0)
#define FLP_DISTANCE_BASED_TRACKING_IS_SUPPORTED  (1U<<1)
#define FLP_ADAPTIVE_BATCHING_IS_SUPPORTED        (1U<<2)
#define FLP_DISTANCE_BASED_BATCHING_IS_SUPPORTED  (1U<<3)
#define FLP_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED    (1U<<4)

/** FlpExtLocationFlags bits*/
/** FlpExtLocation has valid latitude and longitude. */
#define FLP_EXTENDED_LOCATION_HAS_LAT_LONG   (1U<<0)
/** FlpExtLocation has valid altitude. */
#define FLP_EXTENDED_LOCATION_HAS_ALTITUDE   (1U<<1)
/** FlpExtLocation has valid speed. */
#define FLP_EXTENDED_LOCATION_HAS_SPEED      (1U<<2)
/** FlpExtLocation has valid bearing. */
#define FLP_EXTENDED_LOCATION_HAS_BEARING    (1U<<4)
/** FlpExtLocation has valid accuracy. */
#define FLP_EXTENDED_LOCATION_HAS_ACCURACY   (1U<<8)

/** Flags to indicate which values are valid in a FlpLocationExtended. */
typedef uint16_t FlpLocationExtendedFlags;
/** FlpLocationExtended has valid vertical uncertainty */
#define FLP_LOCATION_EXTENDED_HAS_VERT_UNC    0x0010
/** FlpLocationExtended has valid speed uncertainty */
#define FLP_LOCATION_EXTENDED_HAS_SPEED_UNC   0x0020
/** FlpLocationExtended has valid heading uncertainty */
#define FLP_LOCATION_EXTENDED_HAS_BEARING_UNC 0x0040
/** FlpLocationExtended has valid horizontal reliability */
#define FLP_LOCATION_EXTENDED_HAS_HOR_RELIABILITY 0x0080
/** FlpLocationExtended has valid vertical reliability */
#define FLP_LOCATION_EXTENDED_HAS_VERT_RELIABILITY 0x0100

typedef uint32_t LocReportType;
#define LOCATION_REPORT_ON_FULL_INDICATION     0x0000001
#define LOCATION_REPORT_ON_FIX_INDICATION      0x0000002
#define LOCATION_REPORT_ON_QUERY               0x0000004
#define LOCATION_REPORT_ON_INDICATIONS         0x0000008

#define FLP_SESSION_POWER_LEVEL_HIGH           5000
#define FLP_SESSION_POWER_LEVEL_MIDDLE         2500
#define FLP_SESSION_POWER_LEVEL_LOW            1000

#define FLP_SESSION_TECH_SOURCE_BIT_GNSS       0x0000001
#define FLP_SESSION_TECH_SOURCE_BIT_WIFI       0x0000002
#define FLP_SESSION_TECH_SOURCE_BIT_SENSORS    0x0000004
#define FLP_SESSION_TECH_SOURCE_BIT_CELL       0x0000008
#define FLP_SESSION_TECH_SOURCE_BIT_BLUETOOTH  0x0000016

/** Represents FlpExtLocation extended. */
typedef struct {
    /** set to sizeof(FlpLocationExtended) */
    size_t          size;
    /** Contains FlpLocationExtendedFlags bits. */
    uint16_t        flags;
    /** vertical uncertainty in meters */
    float           vert_unc;
    /** speed uncertainty in m/s */
    float           speed_unc;
    /** heading uncertainty in degrees (0 to 359.999) */
    float           bearing_unc;
    /** horizontal reliability. */
    LocReliability  horizontal_reliability;
    /** vertical reliability. */
    LocReliability  vertical_reliability;
} FlpLocationExtended;

/** Locations Used for FLP Session.*/
typedef struct FlpExtLocation_s {
    size_t                  size;
    /** Contains FlpExtLocationFlags bits. */
    uint16_t                flags;
    double                  latitude;
    double                  longitude;
    double                  altitude;
    float                   speed;
    float                   bearing;
    float                   accuracy;
    int64_t                 timestamp;
    uint32_t                sources_used; //in LocPosTechMask format
    FlpLocationExtended     flpExtended;  //used by ULP only
} FlpExtLocation;

typedef enum {
    ASSOCIATE_JVM_EXT,
    DISASSOCIATE_JVM_EXT,
} FlpExtThreadEvent;

/** call backs for FLP session. */
typedef void (*flp_ext_location_callback)(int32_t num_locations,
                                          FlpExtLocation** location,
                                          LocReportType reportTrigger);
typedef void (*flp_ext_acquire_wakelock)();
typedef void (*flp_ext_release_wakelock)();
typedef int (*flp_ext_set_flp_session_thread)(FlpExtThreadEvent event);
typedef void(*flp_ext_capabilities_callback)(int capabilities);
typedef void(*flp_ext_status_callback)(int32_t status);
typedef void (*flp_ext_max_pw_allocated_callback)(double power_mW);


/** FlpExtCallbacks structure for FLP session. */
typedef struct {
    /** set to sizeof(FlpExtCallbacks) */
    size_t      size;
    flp_ext_location_callback location_cb;
    flp_ext_acquire_wakelock acquire_wakelock_cb;
    flp_ext_release_wakelock release_wakelock_cb;
    flp_ext_set_flp_session_thread set_thread_event_cb;
    flp_ext_capabilities_callback flp_capabilities_cb;
    flp_ext_status_callback flp_status_cb;
    flp_ext_max_pw_allocated_callback max_pw_allocated_cb;
} FlpExtCallbacks;

/** flp fixes return type*/
typedef enum flp_fixes_return_type {
    FLP_RETURN_BATCHED_LOCATIONS_TO_BATCHING_CLIENTS,
    FLP_RETURN_LIVE_LOCATIONS_TO_TRACKING_CLIENTS,
    FLP_RETURN_TO_ALL_CLIENTS
}FlpFixesReturnType;

/** API selection for flp session*/
typedef enum loc_api_selection_type {
    LOC_API_SELECTION_UN_SELECTED,
    LOC_API_SELECTION_BATCHING_API,
    LOC_API_SELECTION_GPS_API,
    LOC_API_SELECTION_DBT_API,
    LOC_API_SELECTION_DBT_AND_GPS_API
}LocApiSelectionType;

/** Reasons of GPS reports batched locations*/
typedef enum loc_batching_reported_type {
    LOC_BATCHING_ON_FULL_IND_REPORT,
    LOC_BATCHING_ON_FIX_IND_REPORT,
    LOC_BATCHING_ON_QUERY_REPORT
}LocBatchingReportedType;

/** FLP extended batch options */
typedef struct {
    double max_power_allocation_mW;
    uint32_t sources_to_use;
    uint32_t flags;
    int64_t period_ns;
    uint32_t distance_ms;
    LocApiSelectionType loc_api_selection;
} FlpExtBatchOptions;

/** Flags used to specify which aiding data to delete
    when calling delete_aiding_data() */
typedef uint64_t GnssAidingDataMask;

// IMPORTANT: Note that the following values must match
// constants in IZatFlpService.java
#define GNSS_AIDING_DATA_POSITION           (1U << 1)
#define GNSS_AIDING_DATA_TIME               (1U << 2)
#define GNSS_AIDING_DATA_IONO               (1U << 3)
#define GNSS_AIDING_DATA_UTC                (1U << 4)
#define GNSS_AIDING_DATA_HEALTH             (1U << 5)
#define GNSS_AIDING_DATA_SADATA             (1U << 6)
#define GNSS_AIDING_DATA_RTI                (1U << 7)
#define GNSS_AIDING_DATA_FREQ_BIAS_EST      (1U << 8)
#define GNSS_AIDING_DATA_CELLDB_INFO        (1U << 9)
// GPS
#define GNSS_AIDING_DATA_GPS_EPHEMERIS      (1U << 10)
#define GNSS_AIDING_DATA_GPS_ALMANAC        (1U << 11)
#define GNSS_AIDING_DATA_GPS_SVDIR          (1U << 12)
#define GNSS_AIDING_DATA_GPS_SVSTEER        (1U << 13)
#define GNSS_AIDING_DATA_GPS_TIME           (1U << 14)
#define GNSS_AIDING_DATA_GPS_ALMANAC_CORR   (1U << 15)
// GLONASS
#define GNSS_AIDING_DATA_GLO_EPHEMERIS      (1U << 16)
#define GNSS_AIDING_DATA_GLO_ALMANAC        (1U << 17)
#define GNSS_AIDING_DATA_GLO_SVDIR          (1U << 18)
#define GNSS_AIDING_DATA_GLO_SVSTEER        (1U << 19)
#define GNSS_AIDING_DATA_GLO_TIME           (1U << 20)
#define GNSS_AIDING_DATA_GLO_ALMANAC_CORR   (1U << 21)
// BeiDou
#define GNSS_AIDING_DATA_BDS_EPHEMERIS      (1U << 22)
#define GNSS_AIDING_DATA_BDS_ALMANAC        (1U << 23)
#define GNSS_AIDING_DATA_BDS_SVDIR          (1U << 24)
#define GNSS_AIDING_DATA_BDS_SVSTEER        (1U << 25)
#define GNSS_AIDING_DATA_BDS_TIME           (1U << 26)
#define GNSS_AIDING_DATA_BDS_ALMANAC_CORR   (1U << 27)

#define GNSS_AIDING_DATA_ALL                ~0L

#endif /* FUSED_LOCATION_EXTENDED_H */
