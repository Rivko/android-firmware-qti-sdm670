#ifndef LOWI_SERVICE_01_H
#define LOWI_SERVICE_01_H
/**
  @file lowi_service_v01.h

  @brief This is the public header file which defines the lowi service Data structures.

  This header file defines the types and structures that were defined in
  lowi. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/lowi/main/latest/api/lowi_service_v01.h#7 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Fri Feb 26 2016 (Spin 0)
   From IDL File: lowi_service_v01.idl */

/** @defgroup lowi_qmi_consts Constant values defined in the IDL */
/** @defgroup lowi_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup lowi_qmi_enums Enumerated types used in QMI messages */
/** @defgroup lowi_qmi_messages Structures sent as QMI messages */
/** @defgroup lowi_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup lowi_qmi_accessor Accessor for QMI service object */
/** @defgroup lowi_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup lowi_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LOWI_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define LOWI_V01_IDL_MINOR_VERS 0x05
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOWI_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LOWI_V01_MAX_MESSAGE_ID 0x0028
/**
    @}
  */


/** @addtogroup lowi_qmi_consts
    @{
  */
#define LOWI_MAX_STRING_LENGTH_V01 255

/**  Maximum frequency allowed in discovery scan  */
#define LOWI_MAX_FREQ_LIST_SIZE_V01 20

/**  Maximum size for the list of APs to scan  */
#define LOWI_MAX_AP_SSID_LIST_SIZE_V01 20

/**  Maximum SSID length  */
#define LOWI_MAX_SSID_LEN_V01 32

/**  Mask for rtt3_supported field to indicate if AP supports the
 802.11 MC / 2 sided RTT  */
#define LOWI_AP_802_11_MC_V01 0x01

/**  Mask for rtt3_supported field to provide AP information for HT support  */
#define LOWI_HT_INFO_BIT_0_V01 0x02

/**  Mask for rtt3_supported field to provide AP information for HT support  */
#define LOWI_HT_INFO_BIT_1_V01 0x04

/**  Maximum number of APs allowed to be returned in a scan  */
#define LOWI_MAX_AP_ALLOWED_V01 25

/**  Maximum number of measurements per AP  */
#define LOWI_MAX_MEASUREMENTS_V01 1

/**  Maximum length of the country code  */
#define LOWI_MAX_COUNTRY_CODE_LENGTH_V01 5
#define LOWI_MAX_WLAN_STRING_LEN_V01 31

/**  Maximum interface length  */
#define LOWI_MAX_INTERFACE_LEN_V01 32

/**  Maximum number of APs allowed to be returned in a scan  */
#define LOWI_MAX_BATCHED_AP_PER_IND_V01 500

/**  Maximum number of measurements per AP  */
#define LOWI_MAX_MEASUREMENTS_BGSCAN_V01 150

/**  Maximum number of APs allowed to be returned in a scan  */
#define LOWI_MAX_AP_ALLOWED_BGSCAN_V01 25
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Response Message; Generic response definition. This message is used to tell
                    clients whether their message was accepted for further
                    processing or rejected. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}QmiLOWIGenResp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWIDISCOVERYSCANTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_PASSIVE_SCAN_V01 = 0, /**<  Passive scan. Waits for beacons on every channel for a specific time.  */
  LOWI_ACTIVE_SCAN_V01 = 1, /**<  Active scan. Sends a probe on the channel and waits to receive a probe
         response on the channel. Time spent on each channel is shorter compared
         to a passive scan.  */
  EQMILOWIDISCOVERYSCANTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiDiscoveryScanType_v01;
/**
    @}
  */

typedef uint64_t maskQmiLowiDiscoveryRequestBand_v01;
#define LOWI_TWO_POINT_FOUR_GHZ_V01 ((maskQmiLowiDiscoveryRequestBand_v01)0x00000001ull) /**<  2.4 GHz only.  */
#define LOWI_FIVE_GHZ_V01 ((maskQmiLowiDiscoveryRequestBand_v01)0x00000002ull) /**<  5 GHz only.  */
/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWIDISCOVERYREQUESTMODE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_FORCED_FRESH_V01 = 0, /**<  Request to perform a fresh scan. The request triggers a request
       that is sent to the Wi-Fi driver for a fresh scan.  */
  LOWI_FRESH_WITH_THROTTLING_V01 = 1, /**<  Request to perform a fresh scan with throttling. If the last fresh
       scan results are relatively fresh, this request is serviced
       from cache. The criteria to determine the scan freshness is: \n
       current time < last scan time + threshold (defined in the configuration file).  */
  LOWI_CACHE_FALLBACK_V01 = 2, /**<  Request to perform a fresh scan, if the scan results can not be provided
       from cache. Request provides a fall back tolerance to return the results
       from cache. If the results is cache are older than the fall back tolerance,
       then a fresh scan request is triggered.  */
  LOWI_CACHE_V01 = 3, /**<  Request to return results from cache  */
  EQMILOWIDISCOVERYREQUESTMODE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiDiscoveryRequestMode_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t isSsidValid;
  /**<   Indicates whether the SSID is valid. */

  uint32_t ssid_len;  /**< Must be set to # of elements in ssid */
  uint8_t ssid[LOWI_MAX_SSID_LEN_V01];
  /**<   SSID value. */
}sQmiLowiSsid_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Used by clients to trigger a WLAN scan to discover
                    nearby access points. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Scan Type */
  uint8_t scanType_valid;  /**< Must be set to true if scanType is being passed */
  eQmiLowiDiscoveryScanType_v01 scanType;
  /**<   Passive scan or active scan. */

  /* Optional */
  /*  Request Mode */
  uint8_t requestMode_valid;  /**< Must be set to true if requestMode is being passed */
  eQmiLowiDiscoveryRequestMode_v01 requestMode;
  /**<   Request mode. */

  /* Optional */
  /*  RF Band to Scan */
  uint8_t band_valid;  /**< Must be set to true if band is being passed */
  maskQmiLowiDiscoveryRequestBand_v01 band;
  /**<   RF band to scan. */

  /* Optional */
  /*  Measurement Age Filter */
  uint8_t measAgeFilterSec_valid;  /**< Must be set to true if measAgeFilterSec is being passed */
  uint32_t measAgeFilterSec;
  /**<   Measurement age filter in seconds. For a cache request, at the time of
       generating the response for the request, all measurements that are
       older than the current time (the filter age) are filtered out from the response.
       Thus a '0' age filter could potentially return no measurements from the cache.
       For a fresh scan request, at the time of generating the response for the request,
       all measurements that are older than the request time to the WLAN driver (the
       filter age) are filtered out from the response.
   */

  /* Optional */
  /*  Fallback Tolerance */
  uint8_t fallbackToleranceSec_valid;  /**< Must be set to true if fallbackToleranceSec is being passed */
  uint32_t fallbackToleranceSec;
  /**<   Fallback tolerance in seconds. Only valid when eRequestMode is CACHE_FALLBACK.
       At the time of evaluating this request, all requested channels should have the last measurement
       timestamp equal to or greater than the current time (fallback tolerance). If not, the fallback
       triggers a fresh scan. The client can provide 0 if they want to trigger a fresh scan as well.
   */

  /* Optional */
  /*  Request Timeout */
  uint8_t requestTimeoutSec_valid;  /**< Must be set to true if requestTimeoutSec is being passed */
  int32_t requestTimeoutSec;
  /**<   Request timeout in seconds. On expiry of this timeout, the request is dropped
       if it has not already been processed. The value should be 0 if the timeout is not valid.
   */

  /* Optional */
  /*  Channel Frequency */
  uint8_t channelFreq_valid;  /**< Must be set to true if channelFreq is being passed */
  uint32_t channelFreq_len;  /**< Must be set to # of elements in channelFreq */
  uint32_t channelFreq[LOWI_MAX_FREQ_LIST_SIZE_V01];
  /**<   Dynamic array containing the frequencies to be scanned. The number of
       entries in the vector should not be more than MAX_CHAN_INFO_SIZE, since
       that is the maximum LOWI currently supports.
   */

  /* Optional */
  /*  SSID List */
  uint8_t ssidList_valid;  /**< Must be set to true if ssidList is being passed */
  uint32_t ssidList_len;  /**< Must be set to # of elements in ssidList */
  sQmiLowiSsid_v01 ssidList[LOWI_MAX_AP_SSID_LIST_SIZE_V01];
  /**<   \n List of access points' SSIDs for which to scan. If not specified, the scan is not
         restricted to a set of APs. */
}QmiLowiDiscoveryRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWISCANSTATUS_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_SCAN_STATUS_UNKNOWN_V01 = 0,
  LOWI_SCAN_STATUS_SUCCESS_V01 = 1, /**<  Measurements were obtained successfully from the WLAN driver.
       Note that SUCCESS does not guarantee that there is at least one
       measurement in this packet. It is possible to have zero measurements
       and a SUCCESS if there are no APs in the vicinity.  */
  LOWI_SCAN_STATUS_BUSY_V01 = 2, /**<  Indicates that the number of pending clients has reached the maximum.  */
  LOWI_SCAN_STATUS_DRIVER_ERROR_V01 = 3, /**<  Unable to initiate the request to the driver.  */
  LOWI_SCAN_STATUS_DRIVER_TIMEOUT_V01 = 4, /**<  Unable to get a response from the driver.  */
  LOWI_SCAN_STATUS_INTERNAL_ERROR_V01 = 5, /**<  There is an internal error condition that causes
       LOWI to be unable to provide any measurements.  */
  LOWI_SCAN_STATUS_INVALID_REQ_V01 = 6, /**<  Invalid request.  */
  LOWI_SCAN_STATUS_NOT_SUPPORTED_V01 = 7, /**<  Request is not supported.  */
  LOWI_SCAN_STATUS_NO_WIFI_V01 = 8, /**<  Wi-Fi is not enabled.  */
  EQMILOWISCANSTATUS_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiScanStatus_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWISCANTYPERESPONSE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_WLAN_SCAN_TYPE_UNKNOWN_V01 = 0,
  LOWI_WLAN_SCAN_TYPE_PASSIVE_V01 = 1,
  LOWI_WLAN_SCAN_TYPE_ACTIVE_V01 = 2,
  EQMILOWISCANTYPERESPONSE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiScanTypeResponse_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWINODETYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_NODE_TYPE_UNKNOWN_V01 = 0,
  LOWI_ACCESS_POINT_V01 = 1,
  LOWI_PEER_DEVICE_V01 = 2,
  EQMILOWINODETYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiNodeType_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t valid;
  /**<   Indicates whether the Mobility Service Advertisement Protocol (MSAP)
       information is valid. */

  uint8_t protocolVersion;
  /**<   Protocol version. */

  uint32_t venueHash;
  /**<   Venue hash. */

  uint8_t serverIdx;
  /**<   Server index. */
}sQmiLowiMsapInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  int32_t meas_age;
  /**<   Measurement age in milliseconds. A value of -1 means information is not available.
   */

  int16_t rssi;
  /**<   Signal strength in 0.5 dBm steps. A signal strength of -10 is -5 in 0.5 dBm steps. */
}sLOWIDiscoveryMeasurementInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  int32_t lo24;
  /**<   Lower three bytes of the MAC address. */

  int32_t hi24;
  /**<   Upper three bytes of the MAC address. */
}sQmiLowiMacAddress_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  sQmiLowiMacAddress_v01 bssid;
  /**<   BSSID of the Wi-Fi node. */

  uint32_t frequency;
  /**<   Frequency in MHz at which the node is detected. */

  uint8_t isSecure;
  /**<   Indicates whether the access point is secure. */

  eQmiLowiNodeType_v01 type;
  /**<   Type of Wi-Fi node. */

  sQmiLowiSsid_v01 ssid;
  /**<   SSID. */

  sQmiLowiMsapInfo_v01 msapInfo;
  /**<   MSAP information. Valid if not NULL. */

  int8_t cellPowerLimitdBm;
  /**<   Cell power limit in dBm. Only valid for discovery scan results,
       if available. For ranging scan results, this will always be 0. */

  uint32_t transmitRate;
  /**<   Transmit rate of the node. 0 indicates the node is not available. */

  uint8_t rtt3_supported;
  /**<   Provides the RTT capabilities of the AP
  BIT0: LOWI_AP_802_11_MC - Indicates if 2 sided RTT is supported
  BIT1 & 2: LOWI_HT_INFO_BIT_0, LOWI_HT_INFO_BIT_1 provide the HT information
  as follows
  BIT2 - 0, BIT1 - 0: (SCN) No secondary channel is present
  BIT2 - 0, BIT1 - 1: (SCA) Secondary channel is above the primary channel
  BIT2 - 1, BIT1 - 0: Reserved
  BIT2 - 1, BIT1 - 1: (SCB) Secondary channel is below the primary channel
  */

  uint64_t tsf;
  /**<   Timestamp at which the beacon from the AP was last received. */

  uint32_t band_center_freq_1;
  /**<   Band center frequency 1 in MHz. */

  uint32_t band_center_freq_2;
  /**<   Band center frequency 2 in MHz. */

  uint32_t phy_mode;
  /**<   Phy Mode. */

  uint32_t tsf_delta;
  /**<   Delta between the Local TSF and TSF reported by AP in microseconds*/

  uint32_t measurementsInfo_len;  /**< Must be set to # of elements in measurementsInfo */
  sLOWIDiscoveryMeasurementInfo_v01 measurementsInfo[LOWI_MAX_MEASUREMENTS_V01];
  /**<   Dynamic array containing measurement information per Wi-Fi node.
       The discovery scan will only have one measurement whereas the
       the vector can contain information from multiple measurements for a ranging scan.
   */
}sLowiDiscoveryScanMeasurement_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Used by clients to trigger a WLAN scan to discover
                    nearby access points. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Scan Type */
  uint8_t scanType_valid;  /**< Must be set to true if scanType is being passed */
  eQmiLowiScanTypeResponse_v01 scanType;
  /**<   Type of measurement. */

  /* Optional */
  /*  Scan Status */
  uint8_t scanStatus_valid;  /**< Must be set to true if scanStatus is being passed */
  eQmiLowiScanStatus_v01 scanStatus;
  /**<   Scan status.*/

  /* Optional */
  /*  Scan Measurement */
  uint8_t scanMeasurements_valid;  /**< Must be set to true if scanMeasurements is being passed */
  uint32_t scanMeasurements_len;  /**< Must be set to # of elements in scanMeasurements */
  sLowiDiscoveryScanMeasurement_v01 scanMeasurements[LOWI_MAX_AP_ALLOWED_V01];
  /**<   \n Dynamic array containing the scan measurements for each AP. */

  /* Optional */
  /*  Scanned Frequency List */
  uint8_t scannedFreqList_valid;  /**< Must be set to true if scannedFreqList is being passed */
  uint32_t scannedFreqList_len;  /**< Must be set to # of elements in scannedFreqList */
  uint32_t scannedFreqList[LOWI_MAX_FREQ_LIST_SIZE_V01];
  /**<   Contains the scanned frequency list corresponding to the scan results. */

  /* Optional */
  /*  Country Code */
  uint8_t countryCode_valid;  /**< Must be set to true if countryCode is being passed */
  uint32_t countryCode_len;  /**< Must be set to # of elements in countryCode */
  uint8_t countryCode[LOWI_MAX_COUNTRY_CODE_LENGTH_V01];
  /**<   Country code of the access point. */

  /* Optional */
  /*  Last Indication */
  uint8_t isLast_valid;  /**< Must be set to true if isLast is being passed */
  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the series. TRUE if it is the last,
   FALSE if there is more to follow. */
}QmiLowiDiscoveryResponseInd_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Used by clients to query the QMI LOWI capabilities. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */
}QmiLowiCapabilitiesRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Used by clients to query the QMI LOWI capabilities. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  LOWI LP Version Number */
  uint8_t lowi_lp_version_valid;  /**< Must be set to true if lowi_lp_version is being passed */
  char lowi_lp_version[LOWI_MAX_STRING_LENGTH_V01 + 1];
  /**<   LOWI Low Power (LP) version number. */

  /* Optional */
  /*  Ranging Scan Supported */
  uint8_t rangingScanSupported_valid;  /**< Must be set to true if rangingScanSupported is being passed */
  uint8_t rangingScanSupported;
  /**<   TRUE if ranging scan is supported. */

  /* Optional */
  /*  Active Scan Supported */
  uint8_t activeScanSupported_valid;  /**< Must be set to true if activeScanSupported is being passed */
  uint8_t activeScanSupported;
  /**<   TRUE if active scan is supported. */

  /* Optional */
  /*  Passive Scan Supported */
  uint8_t truePassiveScanSupported_valid;  /**< Must be set to true if truePassiveScanSupported is being passed */
  uint8_t truePassiveScanSupported;
  /**<   TRUE if passive scan is supported. */

  /* Optional */
  /*  Snoop Scan Supported */
  uint8_t snoopScanSupported_valid;  /**< Must be set to true if snoopScanSupported is being passed */
  uint8_t snoopScanSupported;
  /**<   TRUE if snoop scan is supported. */

  /* Optional */
  /*  WLAN Events Supported */
  uint8_t wlanEventsSupported_valid;  /**< Must be set to true if wlanEventsSupported is being passed */
  uint8_t wlanEventsSupported;
  /**<   TRUE if WLAN events are supported. */

  /* Optional */
  /*  Chip OEM */
  uint8_t chipOEM_valid;  /**< Must be set to true if chipOEM is being passed */
  char chipOEM[LOWI_MAX_WLAN_STRING_LEN_V01 + 1];
  /**<   NULL-terminated string containing the chip manufacturer. */

  /* Optional */
  /*  Chip Name */
  uint8_t chipName_valid;  /**< Must be set to true if chipName is being passed */
  char chipName[LOWI_MAX_WLAN_STRING_LEN_V01 + 1];
  /**<   WLAN chipset name. */

  /* Optional */
  /*  WLAN Chipset ID */
  uint8_t wlan_chip_id_valid;  /**< Must be set to true if wlan_chip_id is being passed */
  uint32_t wlan_chip_id;
  /**<   WLAN chipset ID. */

  /* Optional */
  /*  Wi-Fi Driver Version */
  uint8_t wifiDriverVersion_valid;  /**< Must be set to true if wifiDriverVersion is being passed */
  char wifiDriverVersion[LOWI_MAX_WLAN_STRING_LEN_V01 + 1];
  /**<   NULL-terminated string containing the Wi-Fi driver host version. */

  /* Optional */
  /*  FW Version */
  uint8_t fwVersion_valid;  /**< Must be set to true if fwVersion is being passed */
  char fwVersion[LOWI_MAX_WLAN_STRING_LEN_V01 + 1];
  /**<   NULL-terminated string containing the firmware version. */

  /* Optional */
  /*  Batching supported */
  uint8_t batchingSupported_valid;  /**< Must be set to true if batchingSupported is being passed */
  uint8_t batchingSupported;
  /**<   TRUE if batching is supported. */

  /* Optional */
  /*  Batching Max buffer */
  uint8_t maxNumAPBatching_valid;  /**< Must be set to true if maxNumAPBatching is being passed */
  uint32_t maxNumAPBatching;
  /**<   Max num of APs buffered for batching. */

  /* Optional */
  /*  BGScan supported */
  uint8_t bgScanSupported_valid;  /**< Must be set to true if bgScanSupported is being passed */
  uint8_t bgScanSupported;
  /**<   TRUE if BGScan is supported. */

  /* Optional */
  /*  BGScan Max buffer Size */
  uint8_t maxBGScanBufferSize_valid;  /**< Must be set to true if maxBGScanBufferSize is being passed */
  uint32_t maxBGScanBufferSize;
  /**<   Max BGScan buffer size in bytes. */

  /* Optional */
  /*  BGScan Max AP Cache per scan  */
  uint8_t maxBGScanAPCachePerScan_valid;  /**< Must be set to true if maxBGScanAPCachePerScan is being passed */
  uint32_t maxBGScanAPCachePerScan;
  /**<   Max number of APs that can be stored in BGScan buffer per scan. */

  /* Optional */
  /*  BGScan Max reporting threshold */
  uint8_t maxBGScanReportingThreshold_valid;  /**< Must be set to true if maxBGScanReportingThreshold is being passed */
  uint32_t maxBGScanReportingThreshold;
  /**<   Max possible BGScan buffer threshold. */
}QmiLowiCapabilitiesResponseInd_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWIWLANINTERFACE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_DEV_STA_V01 = 0, /**<  WLAN device is in Station mode.  */
  LOWI_DEV_P2P_CLI_V01 = 1, /**<  WLAN device is in P2P Client mode.  */
  LOWI_WLAN_DEV_ANY_V01 = 2, /**<  WLAN device is in any mode (used for subscription purposes).  */
  EQMILOWIWLANINTERFACE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiWlanInterface_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Used by clients to subscribe for WLAN events, such as
                    Connect, Disconnect, and Handover. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Mandatory */
  /*  Subscribe/Unsubscribe */
  uint8_t subscribe;
  /**<   1 to subscribe, 0 to unsubscribe. */

  /* Optional */
  /*  Interface */
  uint8_t interface_valid;  /**< Must be set to true if interface is being passed */
  eQmiLowiWlanInterface_v01 interface;
  /**<   Interface for which the subscription is needed (P2P, Wi-Fi, etc.). */
}QmiLowiWlanInterfaceEventsSubscriptionRequest_v01;  /* Message */
/**
    @}
  */

typedef uint64_t maskWlanInterfaceEventType_v01;
#define LOWI_WLAN_ON_OFF_EVENT_V01 ((maskWlanInterfaceEventType_v01)0x00000001ull)
#define LOWI_WLAN_CONNECTION_EVENT_V01 ((maskWlanInterfaceEventType_v01)0x00000002ull)
#define LOWI_WLAN_HANDOFF_EVENT_V01 ((maskWlanInterfaceEventType_v01)0x00000004ull)
/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  sQmiLowiMacAddress_v01 bssid;
  /**<   BSSID of the Wi-Fi node. */

  uint32_t frequency;
  /**<   Frequency in MHz at which the node is detected. */

  sQmiLowiSsid_v01 ssid;
  /**<   SSID of the Wi-Fi node */

  int16_t rssi;
  /**<   Signal strength in 0.5 dBm steps, e.g., the signal strength of -10 dBm is -20 in 0.5 dBm steps. */
}sLowiNodeInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Used by clients to subscribe for WLAN events, such as
                    Connect, Disconnect, and Handover. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Event Type */
  uint8_t event_valid;  /**< Must be set to true if event is being passed */
  maskWlanInterfaceEventType_v01 event;
  /**<   Indicates the event type. */

  /* Optional */
  /*  WLAN On */
  uint8_t wlan_on_valid;  /**< Must be set to true if wlan_on is being passed */
  uint8_t wlan_on;
  /**<   Status of WLAN. 0 indicates off and 1 indicates on. */

  /* Optional */
  /*  WLAN Connection Status */
  uint8_t connected_valid;  /**< Must be set to true if connected is being passed */
  uint8_t connected;
  /**<   Status of the WLAN connection. 0 indicates disconnected and 1 indicates connected. */

  /* Optional */
  /*  WLAN Handoff Status */
  uint8_t handoff_valid;  /**< Must be set to true if handoff is being passed */
  uint8_t handoff;
  /**<   Status of a WLAN handoff. 0 indicates no handoff and 1 indicates handoff. */

  /* Optional */
  /*  Connected Node Information */
  uint8_t connectedNodeInfo_valid;  /**< Must be set to true if connectedNodeInfo is being passed */
  sLowiNodeInfo_v01 connectedNodeInfo;
  /**<   Information regarding the node with which the device is associated.
   Only available when connected to a Wi-Fi node. */

  /* Optional */
  /*  Handoff Node Information */
  uint8_t handoffNodeInfo_valid;  /**< Must be set to true if handoffNodeInfo is being passed */
  sLowiNodeInfo_v01 handoffNodeInfo;
  /**<   \n Information regarding the node to which the WLAN handoff happened.
   Only available when handoff happened to a Wi-Fi node. */
}QmiLowiWlanInterfaceEventsResponseInd_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Used by clients to query the current WLAN event state, e.g.,
                    Connect, Disconnect, and Handover events. */
typedef struct {

  /* Mandatory */
  /*  Base Request */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Interface */
  uint8_t interface_valid;  /**< Must be set to true if interface is being passed */
  eQmiLowiWlanInterface_v01 interface;
  /**<   Interface for which the subscription is needed (P2P, Wi-Fi, etc.). */
}QmiLowiWlanInterfaceEventTriggerRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to subscribe for async discovery scans. */
typedef struct {

  /* Mandatory */
  /*  Base Request */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Mandatory */
  /*  Subscribe/Unsubscribe */
  uint8_t subscribe;
  /**<   1 to subscribe, 0 to unsubscribe. */

  /* Mandatory */
  /*  Send Scans Not Sent to APSS. */
  uint8_t sendScansNotSentToApss;
  /**<   1 to only send scans not sent to APSS, 0 to send all scans. */

  /* Mandatory */
  /*  Request Expiry Time */
  uint32_t request_timer_sec;
  /**<   Request expires after the request timer runs out. */
}QmiLowiSnoopScanSubscriptionRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to subscribe for Batching of APs. */
typedef struct {

  /* Mandatory */
  /*  Base Request */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Mandatory */
  /*  Subscribe/Unsubscribe */
  uint8_t subscribe;
  /**<   1 to subscribe, 0 to unsubscribe. */

  /* Optional */
  /*  Percentage buffer full threshold. */
  uint8_t bufferFullThreshold_valid;  /**< Must be set to true if bufferFullThreshold is being passed */
  uint8_t bufferFullThreshold;
  /**<   Percentage buffer full threshold which indicates to the service,
   how much the buffer needs to be filled before sending the buffer full
   indication. Valid values are 50 - 90 percent. If the threshold is not
   provided in the request, threshold from last request is used, if available.
   Otherwise uses default as 90 percent. */

  /* Optional */
  /*  Max APs per scan */
  uint8_t maxApsPerScan_valid;  /**< Must be set to true if maxApsPerScan is being passed */
  uint32_t maxApsPerScan;
  /**<   Max APs to be accumulated in the BGScan buffer for each scan.
   Criteria for storage is highest RSSI. Only valid for BGScan request*/

  /* Optional */
  /*  Number of scans threshold. */
  uint8_t numScanThreshold_valid;  /**< Must be set to true if numScanThreshold is being passed */
  uint32_t numScanThreshold;
  /**<   Number of scans after which the client should receive a notification.
   If the threshold is not provided in the request, client does not get any notification.
   */
}QmiLowiBatchingSubscriptionRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Allows clients to subscribe for Batching of APs. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Buffer full */
  uint8_t bufferFull_valid;  /**< Must be set to true if bufferFull is being passed */
  uint8_t bufferFull;
  /**<   Buffer full indication. Buffer is full to the threshold provided in request.
  If the threshold was not provided, then this indication is sent when buffer is 90% full.*/

  /* Optional */
  /*  NUM of APs */
  uint8_t numAPAvailable_valid;  /**< Must be set to true if numAPAvailable is being passed */
  uint32_t numAPAvailable;
  /**<   Provides the number of available APs in the buffer.*/

  /* Optional */
  /*  Scan Threshold reached */
  uint8_t scanThresholdReached_valid;  /**< Must be set to true if scanThresholdReached is being passed */
  uint8_t scanThresholdReached;
  /**<   Scan threshold reached indication. Number of scans requested are completed.
  If the threshold was not provided, then this indication is never sent.*/

  /* Optional */
  /*  NUM of Scan cycles */
  uint8_t numScanCyclesAvailable_valid;  /**< Must be set to true if numScanCyclesAvailable is being passed */
  uint32_t numScanCyclesAvailable;
  /**<   Provides the number of scan cycles for which the data is available in BGScan buffer.*/
}QmiLowiBatchingSubscriptionInd_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to get the results of Batching. */
typedef struct {

  /* Mandatory */
  /*  Base Request */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Flush buffer indication */
  uint8_t flushBuffer_valid;  /**< Must be set to true if flushBuffer is being passed */
  uint8_t flushBuffer;
  /**<   Flag indicating whether to flush the results buffer after retrieving
  the results. 1 to flush the buffer, 0 to let the buffer continue
  overwriting. Default is 0 */

  /* Optional */
  /*  Max results to retrieve. */
  uint8_t maxResultsToRetrieve_valid;  /**< Must be set to true if maxResultsToRetrieve is being passed */
  uint32_t maxResultsToRetrieve;
  /**<   Maximum number of batched APs to retrieve*/
}QmiLowiGetBatchingResultsRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Allows clients to get the results of Batching. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Batched APs */
  uint8_t batchedAPs_valid;  /**< Must be set to true if batchedAPs is being passed */
  uint32_t batchedAPs_len;  /**< Must be set to # of elements in batchedAPs */
  sQmiLowiMacAddress_v01 batchedAPs[LOWI_MAX_BATCHED_AP_PER_IND_V01];
  /**<   \n Dynamic array containing the batched APs. */

  /* Optional */
  /*  Batched APs Age */
  uint8_t batchedAPAge_valid;  /**< Must be set to true if batchedAPAge is being passed */
  uint32_t batchedAPAge_len;  /**< Must be set to # of elements in batchedAPAge */
  uint32_t batchedAPAge[LOWI_MAX_BATCHED_AP_PER_IND_V01];
  /**<   \n Dynamic array containing the batched APs age cooresponding to the APs
  in the batched APs. */

  /* Optional */
  /*  Last Indication */
  uint8_t isLast_valid;  /**< Must be set to true if isLast is being passed */
  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the series. TRUE if it is the last,
   FALSE if there is more to follow. */
}QmiLowiGetBatchingResultsInd_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  int32_t meas_age;
  /**<   Measurement age in milliseconds. A value of -1 means information is not available.
   */

  int16_t rssi;
  /**<   Signal strength in 0.5 dBm steps. A signal strength of -10 is -5 in 0.5 dBm steps. */

  int32_t scan_id;
  /**<   Unique identifier to indicate the scan cycle in which this measurement was seen. */

  int32_t flags;
  /**<   A bitmask with additional information about the scan i.e. If the scan was interrupted */
}sLowiBGScanMeasurementInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  sQmiLowiMacAddress_v01 bssid;
  /**<   BSSID of the Wi-Fi node. */

  uint32_t frequency;
  /**<   Frequency in MHz at which the node is detected. */

  sQmiLowiSsid_v01 ssid;
  /**<   SSID. */

  uint32_t measurementsInfo_len;  /**< Must be set to # of elements in measurementsInfo */
  sLowiBGScanMeasurementInfo_v01 measurementsInfo[LOWI_MAX_MEASUREMENTS_BGSCAN_V01];
  /**<   Dynamic array containing measurement information per Wi-Fi node.*/

  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the measurements for this AP.
   TRUE if it is the last, FALSE if there are more to follow. */
}sLowiBGScanMeasurement_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Allows clients to get the results of BGScan Batching. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Scan Measurement for all APs */
  uint8_t scanMeasurements_valid;  /**< Must be set to true if scanMeasurements is being passed */
  uint32_t scanMeasurements_len;  /**< Must be set to # of elements in scanMeasurements */
  sLowiBGScanMeasurement_v01 scanMeasurements[LOWI_MAX_AP_ALLOWED_BGSCAN_V01];
  /**<   Dynamic array containing the scan measurements for each AP. */

  /* Optional */
  /*  Last Indication */
  uint8_t isLast_valid;  /**< Must be set to true if isLast is being passed */
  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the series. TRUE if it is the last,
   FALSE if there is more to follow. */
}QmiLowiGetBGScanBatchingResultsInd_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_LOWI_BATCHING_SUBSCRIPTION_V01
//#define REMOVE_QMI_LOWI_BGSCAN_BATCHING_SUBSCRIPTION_V01
//#define REMOVE_QMI_LOWI_CAPABILITIES_V01
//#define REMOVE_QMI_LOWI_DISCOVERY_V01
//#define REMOVE_QMI_LOWI_GET_BATCHING_RESULTS_V01
//#define REMOVE_QMI_LOWI_GET_BGSCAN_BATCHING_RESULTS_V01
//#define REMOVE_QMI_LOWI_SNOOP_SCAN_SUBSCRIPTION_V01
//#define REMOVE_QMI_LOWI_WLAN_INTERFACE_EVENTS_V01
//#define REMOVE_QMI_LOWI_WLAN_INTERFACE_EVENT_TRIGGER_V01

/*Service Message Definition*/
/** @addtogroup lowi_qmi_msg_ids
    @{
  */
#define QMI_LOWI_DISCOVERY_REQ_V01 0x0020
#define QMI_LOWI_DISCOVERY_RESP_V01 0x0020
#define QMI_LOWI_DISCOVERY_IND_V01 0x0020
#define QMI_LOWI_CAPABILITIES_REQ_V01 0x0021
#define QMI_LOWI_CAPABILITIES_RESP_V01 0x0021
#define QMI_LOWI_CAPABILITIES_IND_V01 0x0021
#define QMI_LOWI_WLAN_INTERFACE_EVENTS_REQ_V01 0x0022
#define QMI_LOWI_WLAN_INTERFACE_EVENTS_RESP_V01 0x0022
#define QMI_LOWI_WLAN_INTERFACE_EVENTS_IND_V01 0x0022
#define QMI_LOWI_WLAN_INTERFACE_EVENT_TRIGGER_REQ_V01 0x0023
#define QMI_LOWI_WLAN_INTERFACE_EVENT_TRIGGER_RESP_V01 0x0023
#define QMI_LOWI_WLAN_INTERFACE_EVENT_TRIGGER_IND_V01 0x0023
#define QMI_LOWI_SNOOP_SCAN_SUBSCRIPTION_REQ_V01 0x0024
#define QMI_LOWI_SNOOP_SCAN_SUBSCRIPTION_RESP_V01 0x0024
#define QMI_LOWI_SNOOP_SCAN_SUBSCRIPTION_IND_V01 0x0024
#define QMI_LOWI_BATCHING_SUBSCRIPTION_REQ_V01 0x0025
#define QMI_LOWI_BATCHING_SUBSCRIPTION_RESP_V01 0x0025
#define QMI_LOWI_BATCHING_SUBSCRIPTION_IND_V01 0x0025
#define QMI_LOWI_GET_BATCHING_RESULTS_REQ_V01 0x0026
#define QMI_LOWI_GET_BATCHING_RESULTS_RESP_V01 0x0026
#define QMI_LOWI_GET_BATCHING_RESULTS_IND_V01 0x0026
#define QMI_LOWI_BGSCAN_BATCHING_SUBSCRIPTION_REQ_V01 0x0027
#define QMI_LOWI_BGSCAN_BATCHING_SUBSCRIPTION_RESP_V01 0x0027
#define QMI_LOWI_BGSCAN_BATCHING_SUBSCRIPTION_IND_V01 0x0027
#define QMI_LOWI_GET_BGSCAN_BATCHING_RESULTS_REQ_V01 0x0028
#define QMI_LOWI_GET_BGSCAN_BATCHING_RESULTS_RESP_V01 0x0028
#define QMI_LOWI_GET_BGSCAN_BATCHING_RESULTS_IND_V01 0x0028
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro lowi_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type lowi_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define lowi_get_service_object_v01( ) \
          lowi_get_service_object_internal_v01( \
            LOWI_V01_IDL_MAJOR_VERS, LOWI_V01_IDL_MINOR_VERS, \
            LOWI_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

