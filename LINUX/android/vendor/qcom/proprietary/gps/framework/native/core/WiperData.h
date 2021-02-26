/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef WIPERDATA_H
#define WIPERDATA_H

#include "comdef.h"

namespace izat_manager
{

#ifdef __cplusplus
extern "C"
{
#endif


#define MAX_REPORTED_APS 50
#define MAC_ADDRESS_LENGTH 6
#define SSID_LENGTH 32
#define SRN_MAX_REPORTED_APS 64
#define SRN_MAC_ADDRESS_LENGTH 6

#define WIFI_APDATA_MASK_AP_RSSI         0x00000010
#define WIFI_APDATA_MASK_AP_CHANNEL      0x00000020
#define WIFI_APDATA_MASK_RSSI_TIMESTAMP  0x00000400
#define WIFI_APDATA_MASK_MEASUREMENT_AGE 0x00000800
#define WIFI_APDATA_MASK_SERVING_AP      0x00001000
#define WIFI_APDATA_MASK_FREQUENCY       0x00002000
#define WIFI_APDATA_MASK_SSID            0x00004000


typedef enum ReliabilityValue
{
    RELIABILITY_NOT_SET = 0,
    RELIABILITY_VERY_LOW = 1,
    RELIABILITY_LOW = 2,
    RELIABILITY_MEDIUM = 3,
    RELIABILITY_HIGH = 4
} ReliabilityValue;

typedef enum PositionSourceType
{
    GNSS = 0,
    CELLID = 1,
    ENH_CELLID = 2,
    WIFI = 3,
    TERRESTRIAL = 4,
    GNSS_TERRESTRIAL_HYBRID = 5,
    OTHER = 6
} PositionSourceType;

typedef enum PositionSourceProviderType
{
    EXTERNAL = 0,
    INTERNAL = 1
} PositionSourceProviderType;

enum WifiScanResponseType
{
    WIFI_SCAN_RESPONSE_SUCCESS = 0,
    WIFI_SCAN_RESPONSE_UNKNOWN,
    WIFI_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE,
    WIFI_SCAN_RESPONSE_NO_WIFI_AVAILABLE
};

enum WifiScanType
{
    FREE_WIFI_SCAN = 0,
    ON_DEMAND_SCAN
};

typedef struct CoarsePositionInfo {

    uint8 latitudeValid;  /* Must be set to true if latitude is being passed */
    double latitude;

    uint8 longitudeValid;  /* Must be set to true if longitude is being passed */
    double longitude;

    uint8 horUncCircularValid;  /* Must be set to true if horUncCircular is being passed */
    float horUncCircular;

    uint8 horConfidenceValid;  /* Must be set to true if horConfidence is being passed */
    uint8 horConfidence;

    uint8 horReliabilityValid;  /* Must be set to true if horReliability is being passed */
    ReliabilityValue horReliability;

    uint8 altitudeWrtEllipsoidValid;  /* Must be set to true if altitudeWrtEllipsoid is being passed */
    float altitudeWrtEllipsoid;

    uint8 altitudeWrtMeanSeaLevelValid;  /* Must be set to true if altitudeWrtMeanSeaLevel is being passed */
    float altitudeWrtMeanSeaLevel;

    uint8 vertUncValid;  /* Must be set to true if vertUnc is being passed */
    float vertUnc;

    uint8 vertConfidenceValid;  /* Must be set to true if vertConfidence is being passed */
    uint8 vertConfidence;

    uint8 vertReliabilityValid;  /* Must be set to true if vertReliability is being passed */
    ReliabilityValue vertReliability;

    uint8 timestampUtcValid;  /* Must be set to true if timestampUtc is being passed */
    uint64 timestampUtc;

    uint8 timestampAgeValid;  /* Must be set to true if timestampAge is being passed */
    int32 timestampAge;

    uint8 positionSrcValid;  /* Must be set to true if positionSrc is being passed */
    PositionSourceType positionSrc;

    uint8 positionSrcProviderValid;  /* Must be set to true if networkProviderSrc is being passed */
    PositionSourceProviderType positionSrcProvider;

    uint8 onDemandCpiValid; /* Must be set to true if position being injected is a result of ODCPI, and not CPI */
    uint8 onDemandCpi;

} CoarsePositionInfo;

typedef struct WifiApSsidInfo {

  /*  Wifi SSID string */
  char ssid[SSID_LENGTH+1];
} WifiApSsidInfo;

typedef struct WifiApInfo {

    WifiScanResponseType wifiScanRespType;
    /* Represents any error encountered during wifi scan*/
    int apLen;
    /* Represents whether the access point is a serving access point or not.*/
    uint8 onDemandScan;
    /*Represents whether the scan is a on-demand / free wifi scan*/
    int64 requestTimestamp;
    /*Represent the timestamp at which the scan was requested, valid only if onDemandScan is true*/
    int64 receiveTimestamp;
    /*Represent the timestamp at which the scan was received, valid only if onDemandScan is true*/
    /* Represents whether access point len*/
    uint8 mac_address[MAX_REPORTED_APS * MAC_ADDRESS_LENGTH];
    /*  Represents mac address of the wifi access point*/
    int32 measurement_age[MAX_REPORTED_APS];
    /* Represents measurement age*/
    int16 rssi[MAX_REPORTED_APS];
    /*  Represents received signal strength indicator of the wifi access point*/
    int32 apHighResolutionRssi[MAX_REPORTED_APS];
    /* Represents AP signal strength indicator in 0.1 dbm
     */
    int64 rssi_timestamp[MAX_REPORTED_APS];
    /* Represents timstamp at which the RSS was received*/
    uint16 channel[MAX_REPORTED_APS];
    /*  Represents  wiFi channel on which a beacon was received of the wifi access point */
    uint32 frequency[MAX_REPORTED_APS];
    /*  Represents  wiFi frequency on which a beacon was received of the wifi access point
            The channel field being set earlier was actually the frequency, frequency field was introduced for
            better readibility. Channel numbers could actually be repeated for different frequencies,
            but for backward compatibility we continue to input frequency in channel filed as well*/
    uint8 servingAccessPoint[MAX_REPORTED_APS];
    /* Represents info on whether ap SSID is valid */
    uint8  wifiApSsidValid;
    /* Represents info on whether ap SSID is valid */
    uint32 wifiApSsidInfoLen;
    /* Represents info on ap SSID length */
    WifiApSsidInfo wifiApSsidInfo[MAX_REPORTED_APS];
    /* Represent Wifi SSID string */
} WifiApInfo;

typedef struct WifiLocation {

    unsigned char positionValid;
    /* Represents info on whether position is valid */
    double latitude;
    /* Represents latitude in degrees */
    double longitude;
    /* Represents longitude in degrees */
    float accuracy;
    /* Represents expected accuracy in meters */
    uint8 horConfidence;
    /* Represents expected hor confidence in percentage */
    int fixError;
    /* Represents Wifi position error code */
    unsigned char numApsUsed;
    /* Represents  number of Access Points (AP) used to generate a fix */
    unsigned char apInfoValid;
    /* Represents whether access point info is valid*/
    WifiApInfo apInfo;
}WifiLocation;

typedef struct WifiSupplicantInfo {

    int attachState;
    /* Represents whether access point attach state*/
    unsigned char apMacAddressValid;
    /* Represents info on whether ap mac address is valid */
    uint8 apMacAddress[MAC_ADDRESS_LENGTH];
    /* Represents mac address of the wifi access point*/
    uint8 wifiApSsidValid;
    /* Represents info on whether ap SSID is valid */
    char ssid[SSID_LENGTH+1];
    /* Represents Wifi SSID string*/
} WifiSupplicantInfo;

/* Wifi request types from modem*/
enum WifiRequestType
{
    HIGH = 0,
    LOW = 1,
    STOP = 2,
    UNKNOWN
};

typedef struct OdcpiRequest {
    enum WifiRequestType requestType;
    uint8 emergencyRequest;
} OdcpiRequest;

typedef struct WifiApDataRequest {
    uint8 emergencyRequest;
} WifiApDataRequest;

/*Wifi attachment status */
enum WifiAttachmentStatus
{
    WIFI_ACCESS_POINT_ATTACHED = 0,
    WIFI_ACCESS_POINT_DETACHED = 1,
    WIFI_ACCESS_POINT_HANDOVER = 2
};

typedef enum SrnScanErrorCodes {
    SRN_SCAN_RESPONSE_SUCCESS = 0,
    SRN_SCAN_RESPONSE_GENERAL_FAILURE,
    SRN_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE,
    SRN_SCAN_RESPONSE_SRN_ADAPTER_OFF,
    SRN_SCAN_RESPONSE_UNKNOWN
}SrnScanErrorCodes;


// Store BT ap info
typedef struct BtAccessPointInfo {
    /* BT device RSSI reported */
    int32 mApBtDevRssi;
    /* MAC adress of BT device */
    uint8 mApBtMacAddress[SRN_MAC_ADDRESS_LENGTH];
    /* UTC timestamp at which the scan was requested for this device*/
    int64 mBtDevScanRequestTimestamp;
    /* UTC timestamp at which the scan was started. */
    int64 mScanStartTimestamp;
    /* UTC timestamp at which the scan was received.*/
    int64 mScanReceiveTimestamp;
} BtAccessPointInfo;

// Common structure to hold BT/BTLE device data to be
// passed to modem.
typedef struct BtDeviceInfo {
    /* Number of bt devices */
    int btAp_len;
    /* Represents info on whether Bt data is valid (no error)*/
    bool mValidDeviceData;
    /* BT device RSSI reported */
    int32 mApBtDevRssi[SRN_MAX_REPORTED_APS];
    /* MAC adress of BT device */
    uint8 mApBtMacAddress[SRN_MAX_REPORTED_APS * SRN_MAC_ADDRESS_LENGTH];
    /* UTC timestamp at which the scan was requested for this device*/
    int64 mBtDevScanRequestTimestamp[SRN_MAX_REPORTED_APS];
    /* UTC timestamp at which the scan was started. */
    int64 mScanStartTimestamp;
    /* UTC timestamp at which the scan was received.*/
    int64 mScanReceiveTimestamp;
    /* Reason for the error/failure if details are not valid */
    int32 mErrorCause;
} BtDeviceInfo;

#ifdef __cplusplus
}
#endif
}

#endif /* WIPERDATA_H */
