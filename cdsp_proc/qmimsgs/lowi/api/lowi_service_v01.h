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
  Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.adsp/2.6/lowi/api/lowi_service_v01.h#14 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Mar 23 2018 (Spin 0)
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
#define LOWI_V01_IDL_MINOR_VERS 0x0C
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOWI_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LOWI_V01_MAX_MESSAGE_ID 0x002A
/**
    @}
  */


/** @addtogroup lowi_qmi_consts
    @{
  */

/**  Max length for LOWI Strings.  */
#define LOWI_MAX_STRING_LENGTH_V01 255

/**  Max size of LOWI IEs.  */
#define LOWI_MAX_LOCATION_IE_SIZE_V01 255

/**  Max length for Client Name Strings. 

 Structure to provide information elements.  */
#define LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 16

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

/**  @DESCRIPTION
  This command informs the service that client is interested in performing ranging
  scan. */
#define LOWI_MAX_NODES_ALLOWED_RANGING_V01 50

/**  Maximum number of ranging measurements per Wifi Node.  */
#define LOWI_MAX_RANGING_MEASUREMENTS_V01 31

/**  Maximum number of nodes allowed in one ranging response QMI message.  */
#define LOWI_MAX_NODES_ALLOWED_RANGING_RESPONSE_V01 10

/**  Maximum number of colocated bssids supported per AP.  */
#define LOWI_MAX_COLOCATED_BSSIDS_V01 25
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

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t isIEValid;
  /**<   Is IE Valid or not. */

  uint8_t id;
  /**<   Element identifier. */

  uint32_t locData_len;  /**< Must be set to # of elements in locData */
  uint8_t locData[LOWI_MAX_LOCATION_IE_SIZE_V01];
  /**<   Data blob */
}sLOWILocationIE_v01;  /* Type */
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
         to a passive scan. Performs Passive scan on DFS channels.  */
  LOWI_ACTIVE_SCAN_ONLY_V01 = 2, /**<  Perform a purely active scan using probe requests. No scan on channels
    restricted to use via Dynamic Frequency Selection (DFS) only.  */
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

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWIRADIOCHAINPREF_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_RADIO_CHAIN_PREF_UNKNOWN_V01 = 0, /**<  In a scan result, indicates that the radio chain preference used for the
       scan is not known  */
  LOWI_RADIO_CHAIN_PREF_LOW_LATENCY_V01 = 1, /**<  Prefer to use available radio chains in a way that minimizes time to
       complete the scan  */
  LOWI_RADIO_CHAIN_PREF_LOW_POWER_V01 = 2, /**<  Prefer to use available radio chains in a way that minimizes total power
       consumed for the scan  */
  LOWI_RADIO_CHAIN_PREF_HIGH_ACCURACY_V01 = 3, /**<  Prefer to use available radio chains in a way that maximizes accuracy of
       the scan result, e.g. RSSI measurements  */
  EQMILOWIRADIOCHAINPREF_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiRadioChainPref_v01;
/**
    @}
  */

typedef uint64_t maskQmiLowiRadioChain_v01;
#define LOWI_RADIO_CHAIN_UNKNOWN_V01 ((maskQmiLowiRadioChain_v01)0x00000000ull) /**<  Unknown Chain.  */
#define LOWI_RADIO_CHAIN_0_V01 ((maskQmiLowiRadioChain_v01)0x00000001ull) /**<  Chain 0.  */
#define LOWI_RADIO_CHAIN_1_V01 ((maskQmiLowiRadioChain_v01)0x00000002ull) /**<  Chain 1.  */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */

  /* Optional */
  /*  Radio Chain Preference  */
  uint8_t radioChainPref_valid;  /**< Must be set to true if radioChainPref is being passed */
  eQmiLowiRadioChainPref_v01 radioChainPref;
  /**<   \n Radio Chain Preference to choose between power, accuracy etc  */
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
  LOWI_WLAN_SCAN_TYPE_PASSIVE_V01 = 1, /**<  Passive scan performed  */
  LOWI_WLAN_SCAN_TYPE_ACTIVE_V01 = 2, /**<  Active scan performed. Passive scan performed on DFS channels  */
  LOWI_WLAN_SCAN_TYPE_ACTIVE_ONLY_V01 = 3, /**<  Pure active scan performed. No channels scanned restricted by DFS  */
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
  LOWI_ACCESS_POINT_V01 = 1, /**<  Access point  */
  LOWI_PEER_DEVICE_V01 = 2, /**<  Device which is part of a P2P group  */
  LOWI_NAN_DEVICE_V01 = 3, /**<  Device which is part of NAN group  */
  LOWI_STA_DEVICE_V01 = 4, /**<  Stand alone station (as in ad-hoc BSS)  */
  LOWI_SOFT_AP_V01 = 5, /**<  Device in Hotspot mode  */
  EQMILOWINODETYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiNodeType_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EQMILOWIENCRYPTIONTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_ENCRYPTION_TYPE_UNKNOWN_V01 = 0, 
  LOWI_ENCRYPTION_TYPE_OPEN_V01 = 1, 
  LOWI_ENCRYPTION_TYPE_WEP_V01 = 2, 
  LOWI_ENCRYPTION_TYPE_WPA_PSK_V01 = 3, 
  LOWI_ENCRYPTION_TYPE_WPA_EAP_V01 = 4, 
  EQMILOWIENCRYPTIONTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eQmiLowiEncryptionType_v01;
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

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  maskQmiLowiRadioChain_v01 radioChain;
  /**<   Information regarding the chain used to discover the AP. */

  int8_t rssiChain0;
  /**<   rssi value in dBm if chain0 used in scan. */

  int8_t rssiChain1;
  /**<   rssi value in dBm if chain1 used in scan. */
}sLowiAPRssiChainInfo_v01;  /* Type */
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

  /* Optional */
  /*  Connected Node Information */
  uint8_t connectedNodeInfo_valid;  /**< Must be set to true if connectedNodeInfo is being passed */
  sQmiLowiMacAddress_v01 connectedNodeInfo;
  /**<   Information regarding the node with which the device is associated.
   Only available when connected to a Wi-Fi node. */

  /* Optional */
  /*  Encryption Type */
  uint8_t encryptionType_valid;  /**< Must be set to true if encryptionType is being passed */
  uint32_t encryptionType_len;  /**< Must be set to # of elements in encryptionType */
  eQmiLowiEncryptionType_v01 encryptionType[LOWI_MAX_AP_ALLOWED_V01];
  /**<   \n Dynamic array containing the Encryption type for each AP. */

  /* Optional */
  /*  AP Capability Information */
  uint8_t capabilityInfo_valid;  /**< Must be set to true if capabilityInfo is being passed */
  uint32_t capabilityInfo_len;  /**< Must be set to # of elements in capabilityInfo */
  uint16_t capabilityInfo[LOWI_MAX_AP_ALLOWED_V01];
  /**<   \n Capability Information field sent by the AP (as in 802.11 7.3.1.4). */

  /* Optional */
  /*  Source of the scan */
  uint8_t sourceName_valid;  /**< Must be set to true if sourceName is being passed */
  char sourceName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the source that triggered the scan. */

  /* Optional */
  /*  Access Point RSSI and radio chain info */
  uint8_t apRssiChainInfo_valid;  /**< Must be set to true if apRssiChainInfo is being passed */
  uint32_t apRssiChainInfo_len;  /**< Must be set to # of elements in apRssiChainInfo */
  sLowiAPRssiChainInfo_v01 apRssiChainInfo[LOWI_MAX_AP_ALLOWED_V01];
  /**<   \n Dynamic array containing the chain0/chain1 RSSI info and radio chain mask for each AP. */

  /* Optional */
  /*  Radio Chain Preference */
  uint8_t radioChainPref_valid;  /**< Must be set to true if radioChainPref is being passed */
  eQmiLowiRadioChainPref_v01 radioChainPref;
  /**<   \n Radio Chain Preference to choose between power, accuracy etc  */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
}QmiLowiCapabilitiesRequest_v01;  /* Message */
/**
    @}
  */

typedef uint64_t mRangingPreamble_v01;
#define LOWI_PREAMBLE_LEGACY_V01 ((mRangingPreamble_v01)0x01ull) /**<  Legacy Preamble  */
#define LOWI_PREAMBLE_HT_V01 ((mRangingPreamble_v01)0x02ull) /**<  HT Preamble  */
#define LOWI_PREAMBLE_VHT_V01 ((mRangingPreamble_v01)0x04ull) /**<  VHT Preamble  */
typedef uint64_t mRangingBandwidth_v01;
#define LOWI_BW_20MHZ_V01 ((mRangingBandwidth_v01)0x01ull) 
#define LOWI_BW_40MHZ_V01 ((mRangingBandwidth_v01)0x02ull) 
#define LOWI_BW_80MHZ_V01 ((mRangingBandwidth_v01)0x04ull) 
#define LOWI_BW_160MHZ_V01 ((mRangingBandwidth_v01)0x08ull) 
typedef uint64_t mRangingCapabilities_v01;
#define LOWI_ONE_SIDED_RANGING_V01 ((mRangingCapabilities_v01)0x00000001ull) /**<  One sided Ranging supported from LOWI-LP  */
#define LOWI_TWO_SIDED_RANGING_V01 ((mRangingCapabilities_v01)0x00000002ull) /**<  FW supports supports two-sided ranging  */
#define LOWI_MULTIBURST_RANGING_V01 ((mRangingCapabilities_v01)0x00000004ull) /**<  FW supports Ranging multi burst measurements  */
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

  /* Optional */
  /*  Ranging Bandwidth supported */
  uint8_t rangingBw_valid;  /**< Must be set to true if rangingBw is being passed */
  mRangingBandwidth_v01 rangingBw;
  /**<   Bit mask representing the bandwidth supported for ranging requests */

  /* Optional */
  /*  Preamble supported */
  uint8_t preamble_valid;  /**< Must be set to true if preamble is being passed */
  mRangingPreamble_v01 preamble;
  /**<   Bit mask representing preamble supported for ranging requests */

  /* Optional */
  /*  Ranging Capabilities */
  uint8_t supportedCapabilities_valid;  /**< Must be set to true if supportedCapabilities is being passed */
  mRangingCapabilities_v01 supportedCapabilities;
  /**<   Bit mask representing supported capabilities. */

  /* Optional */
  /*  Radio Chain Preference Capabilities */
  uint8_t radioChainPrefSupported_valid;  /**< Must be set to true if radioChainPrefSupported is being passed */
  uint8_t radioChainPrefSupported;
  /**<   TRUE if radio chain preference is supported. */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
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

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
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

  /* Optional */
  /*  Batched APs Age */
  uint8_t batchedAPSSID_valid;  /**< Must be set to true if batchedAPSSID is being passed */
  uint32_t batchedAPSSID_len;  /**< Must be set to # of elements in batchedAPSSID */
  sQmiLowiSsid_v01 batchedAPSSID[LOWI_MAX_BATCHED_AP_PER_IND_V01];
  /**<   \n Dynamic array containing the batched APs SSID cooresponding to the APs
  in the batched APs. */
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

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  ERANGINGTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SINGLE_SIDED_RANGING_V01 = 0, /**<  Single sided Ranging  */
  TWO_SIDED_RANGING_V01 = 1, /**<  Two-sided Ranging as defined in 802.11mc spec  */
  BEST_EFFORT_RANGING_V01 = 2, /**<  One of the above, which ever the peer supports. If this option is chosen,
   LOWI-LP will ignore the BW and Preemble requested by the client  */
  ERANGINGTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eRangingType_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  ERANGINGBANDWIDTH_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  BW_UNKNOWN_V01 = -1, 
  BW_20MHZ_V01 = 0, 
  BW_40MHZ_V01 = 1, 
  BW_80MHZ_V01 = 2, 
  BW_160MHZ_V01 = 3, 
  ERANGINGBANDWIDTH_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eRangingBandwidth_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  ELOWIRANGINGREPORTTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_RANGING_REPORT_BASIC_V01 = 0, /**<  Basic measurement information provided in the response  */
  LOWI_RANGING_REPORT_DETAILED_V01 = 1, /**<  All above + Detailed measurement information provided in the response  */
  LOWI_RANGING_REPORT_AOA_V01 = 2, /**<  All above + AOA measurement information provided in the response  */
  LOWI_RANGING_REPORT_CFR_V01 = 3, /**<  All above + CFR information is provided in the response. Not supported yet.  */
  ELOWIRANGINGREPORTTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eLOWIRangingReportType_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  ERANGINGPREAMBLE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  RANGING_PREEMBLE_UNKNOWN_V01 = -1, 
  RANGING_PREAMBLE_LEGACY_V01 = 0, 
  RANGING_PREAMBLE_HT_V01 = 1, 
  RANGING_PREAMBLE_VHT_V01 = 2, 
  ERANGINGPREAMBLE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eRangingPreamble_v01;
/**
    @}
  */

/**  Defines the Bandwidth for Ranging 
 Defines the type of reports expected in the response. 
 Defines the Preamble for Ranging  */
typedef uint64_t mFTMOptions_v01;
#define FTM_ASAP_BIT_V01 ((mFTMOptions_v01)0x00000001ull) 
#define FTM_LCI_BIT_V01 ((mFTMOptions_v01)0x00000002ull) 
#define FTM_LOC_CIVIC_BIT_V01 ((mFTMOptions_v01)0x00000004ull) 
#define FTM_PTSF_TIMER_NO_PREF_V01 ((mFTMOptions_v01)0x00000008ull) 
#define FTM_LEG_ACK_ONLY_V01 ((mFTMOptions_v01)0x00000010ull) 
typedef uint64_t mUseFromCache_v01;
#define LOWI_USE_CH_INFO_FRM_CACHE_BIT_V01 ((mUseFromCache_v01)0x01ull) /**<  Use Channel Information from Cache if this bit is set. */
#define LOWI_USE_BW_FRM_CACHE_BIT_V01 ((mUseFromCache_v01)0x02ull) /**<  Use ranging Bandwidth from cache.  */
#define LOWI_USE_PREAMBLE_FRM_CACHE_BIT_V01 ((mUseFromCache_v01)0x04ull) /**<  Use preamble from cache.  */
/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  sQmiLowiMacAddress_v01 bssid;
  /**<   MacId of the Node. */

  uint32_t frequency;
  /**<   Frequency in Mhz, which the node is transmitting*/

  uint32_t bandCenterFreq1;
  /**<   Frequency of the center of total BW */

  uint32_t bandCenterFreq2;
  /**<   Frequency of the center of the second 80MHZ Lobe if BW is
   80MHz + 80MHz */

  eQmiLowiNodeType_v01 nodeType;
  /**<   Type of Node to range with. */

  sQmiLowiMacAddress_v01 spoofMacId;
  /**<   Spoof MacId needed for peer device ranging.
   Only valid if nodeType is PEER_DEVICE */

  eRangingType_v01 rangingType;
  /**<   The Type Of Ranging to be performed */

  eRangingBandwidth_v01 bandwidth;
  /**<   Bandwidth to be used*/

  mFTMOptions_v01 ftmRangingOptions;
  /**<   FTM Ranging parameters to be used for this node.
   Used for two sided ranging. */

  uint8_t numBurstExponent;
  /**<   Number of Bursts Exponent - This field indicates the number of
   bursts of FTM bursts to perform. The number of bursts is indicated
   as Num of Bursts = 2^ Burst exponent. Used for two sided ranging. */

  uint8_t burstDuration;
  /**<   Burst Duration - Duration of each FTM burst,
   * defined by the following table:
   *                                   Value               Burst Duration
   *                                     0/1                    Invalid    
   *                                     2                      250us
   *                                     3                      500us
   *                                     4                        1ms
   *                                     5                        2ms
   *                                     6                        4ms
   *                                     7                        8ms
   *                                     8                       16ms
   *                                     9                       32ms
   *                                     10                      64ms
   *                                     11                     128ms
   *                                     12-14                  Invalid
   *                                     15                 No Preference
   *                                     >15                    Invalid
   * All Invalid values will default to "No Preference"
   * Used for two sided ranging.
  */

  uint16_t burstPeriod;
  /**<   Burst Period (time between Bursts)
   Units: 100ms (0 - no preference)
   Used for two sided ranging. */

  eRangingPreamble_v01 preamble;
  /**<   Preamable to be used*/

  uint8_t numPktsPerMeas;
  /**<   Number of packets for each Ranging measurement
   NOTE: maximum of 5 per Ranging measurement. */

  uint8_t numRetriesPerMeas;
  /**<   Maximum number of times a measurement should be retried if the measurement
   fails (i.e. it yields no results). */

  mUseFromCache_v01 useFromCache;
  /**<   Defines, if certain fields are to be used from Cache. */
}sLOWIRangingNodeInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to perform Ranging measurements. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Scan Measurement for all APs */
  uint8_t nodes_valid;  /**< Must be set to true if nodes is being passed */
  uint32_t nodes_len;  /**< Must be set to # of elements in nodes */
  sLOWIRangingNodeInfo_v01 nodes[LOWI_MAX_NODES_ALLOWED_RANGING_V01];
  /**<   Dynamic array containing the nodes to perform Ranging on. */

  /* Optional */
  /*  Request Timeout */
  uint8_t requestTimeoutSec_valid;  /**< Must be set to true if requestTimeoutSec is being passed */
  int32_t requestTimeoutSec;
  /**<   Request timeout in seconds. On expiry of this timeout, the request is dropped
       if it has not already been processed. The value should be 0 if the timeout is not valid.*/

  /* Optional */
  /*  Request report type in the response */
  uint8_t reportType_valid;  /**< Must be set to true if reportType is being passed */
  eLOWIRangingReportType_v01 reportType;
  /**<   Indicates, the type of report expected in response */

  /* Optional */
  /*  Request Detailed Ranging results */
  uint8_t detailedRangingResults_valid;  /**< Must be set to true if detailedRangingResults is being passed */
  uint8_t detailedRangingResults;
  /**<   Indicates, if the response should contain the detailed ranging scan results or just the basic set.
   * By default only the basic ranging results (sLOWIRangingMeasurementBasicInfo) are provided and are
   * enough for most use cases, if client wants the detailed ranging results (sLOWIRangingMeasurementDetailedInfo),
   * it should set this request parameter to true.
   */

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Originator of the request. */
}QmiLowiRangingRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  ETARGETSTATUS_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_TARGET_STATUS_SUCCESS_V01 = 0, 
  LOWI_TARGET_STATUS_FAILURE_V01 = 1, 
  LOWI_TARGET_STATUS_FAIL_NO_RSP_V01 = 2, /**<  Target Doesn't respond to Ranging request  */
  LOWI_TARGET_STATUS_FAIL_REJECTED_V01 = 3, /**<  Target rejected two sided Ranging request - Applies to two sided ranging only  */
  LOWI_TARGET_STATUS_FAIL_FTM_TIMEOUT_V01 = 4, /**<  Timing measurement Timesout  */
  LOWI_TARGET_STATUS_TARGET_ON_DIFF_CHANNEL_V01 = 5, /**<  Target on different Channel - failure to Range  */
  LOWI_TARGET_STATUS_FAIL_TARGET_NOT_CAPABLE_V01 = 6, /**<  Target not capable of two sided ranging  */
  LOWI_TARGET_STATUS_FAIL_INVALID_TS_V01 = 7, /**<  Invalid Time stamps when ranging with Target  */
  LOWI_TARGET_STATUS_FAIL_TARGET_BUSY_TRY_LATER_V01 = 8, /**<  Target is busy, please try two sided ranging at a later time  */
  ETARGETSTATUS_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}eTargetStatus_v01;
/**
    @}
  */

typedef uint64_t maskRangingRespParams_v01;
#define LOWI_RANGING_RESP_PARAMS_LAST_BURST_V01 ((maskRangingRespParams_v01)0x00000001ull) /**<  Indicates the last burst */
#define LOWI_RANGING_RESP_PARAMS_LCI_AVAILABLE_V01 ((maskRangingRespParams_v01)0x00000002ull) /**<  Indicates the LCI info availability */
#define LOWI_RANGING_RESP_PARAMS_LCR_AVAILABLE_V01 ((maskRangingRespParams_v01)0x00000004ull) /**<  Indicates the LCI info availability */
#define LOWI_RANGING_RESP_PARAMS_COLOCATED_INFO_AVAILABLE_V01 ((maskRangingRespParams_v01)0x00000008ull) /**<  Indicates the Co located BSS info availability */
/** @addtogroup lowi_qmi_enums
    @{
  */
typedef enum {
  EPEEROEM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LOWI_PEER_OEM_UNKNOWN_V01 = 0, 
  LOWI_PEER_OEM_QTI_V01 = 1, 
  EPEEROEM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ePeerOEM_v01;
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
/**  Mask to indicate ranging response parameters. 

 * Provides the PEER OEM information
 * This info is going to be available through Ranging scans
 * on Lithium and through Discovery scans in future chip sets.
 
 */
typedef struct {

  int16_t rssi0p5dBm;
  /**<   Signal strength in 0.5 dBm steps. A signal strength of -10 is -5 in 0.5 dBm steps. */

  int32_t rttPs;
  /**<   RTT - value in pico sec. A value of 0 is considered to be invalid rtt. */

  eRangingPreamble_v01 txPreamble;
  /**<   Tx Ranging Preamble. */

  eRangingBandwidth_v01 txBw;
  /**<   Bandwidth of the transmitted ack from the device to Wifi Node. */

  eRangingPreamble_v01 rxPreamble;
  /**<   Rx ranging Preamble. */

  eRangingBandwidth_v01 rxBw;
  /**<   Bandwidth of the received frame from the Wifi Node. */
}sLOWIRangingMeasurementBasicInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  int32_t measAgeMsec;
  /**<   Measurement age in milliseconds. A value of -1 means information is not available. */

  uint32_t txBitrate;
  /**<   Tx bitrate in units of 100 Kbps. A value of 0 means information is not available. */

  uint8_t txNss;
  /**<   Number of spatial streams. A value of 0 indicates, information is not available. */

  uint8_t txMcsIdx;
  /**<   Modulation Coding Scheme (MCS) index defines the following:
   *  1) The Number of Streams used for TX & RX
   *  2) the Type of mudulation used
   *  3) the Coding Rate
   *  Note: Does not apply to legacy frames (frames using schemes prior to 802.11n). */

  uint8_t rxNss;
  /**<   Number of spatial streams. A value of 0 indicates, information is not available. */

  uint8_t rxMcsIdx;
  /**<   Modulation Coding Scheme (MCS) index defines the following:
   *  1) The Number of Streams used for TX & RX
   *  2) the Type of mudulation used
   *  3) the Coding Rate
   *  Note: Does not apply to legacy frames (frames using schemes prior to 802.11n). */

  uint32_t rxBitrate;
  /**<   Rx bitrate in units of 100 Kbps. A value of 0 means information is not available. */

  int32_t azimuthAOA;
  /**<   Azimuth for Angle of Arrival (AOA). */

  int32_t elevationAOA;
  /**<   Elevation for Angle of Arrival (AOA). */
}sLOWIRangingMeasurementDetailedInfo_v01;  /* Type */
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
  /**<   Primary channel Frequency in MHz. */

  eRangingType_v01 rangingType;
  /**<   Type of Ranging measurement performed, if applicable. */

  eTargetStatus_v01 targetStatus;
  /**<   Status for the measurements associated with this Target. */

  uint8_t numFramesAttempted;
  /**<   Total Ranging measurement Frames attempted   */

  uint8_t negotiatedNumFramesPerBurst;
  /**<   Number of "FTM frames per burst" negotiated with peer/target. */

  uint8_t retryAfterDuration;
  /**<   If Target/peer fails to accept an FTM session. Peer will
   *  provide when it to retry FTM session. this field has the
   *  time after which FTM session can be retried.
   *  uint: seconds */

  uint8_t negotiatedBurstExp;
  /**<   Number of "FTM bursts" negotiated with peer/target.
   *  This is indicated in the form of an exponent.
   *  The number of bursts = 2^negotiated_burst_exp */

  uint16_t actualBurstDuration;
  /**<   Actual time taken by FW to finish one burst of
   *  measurements (unit: ms) */

  uint16_t burstNumber;
  /**<   For a multiburst request, indicates to client the number of measurements completed.  */

  maskRangingRespParams_v01 params;
  /**<   Mask to indicate response params. */

  uint8_t maxBssidsIndicator;
  /**<   Indicates maximum possible number of BSSs, including the reference BSS
   * sharing the same antenna connector.
   * If this field is not configured (value 0), then look at the colocatedBssids field
   * in the extended results. LOWI_RANGING_RESP_PARAMS_COLOCATED_INFO_AVAILABLE indicates
   * if colocatedBssids available for this Node*/

  ePeerOEM_v01 peerOEM;
  /**<   Provides the OEM information.
   * Only provided as part of 2 sided ranging with the peer (RTT3). */
}sLowiRangingAPSpecificInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  sLowiRangingAPSpecificInfo_v01 apSpecificInfo;
  /**<   AP Specifc Ranging Info for each Wifi Node. */

  uint32_t basicMeasurementsInfo_len;  /**< Must be set to # of elements in basicMeasurementsInfo */
  sLOWIRangingMeasurementBasicInfo_v01 basicMeasurementsInfo[LOWI_MAX_RANGING_MEASUREMENTS_V01];
  /**<   Dynamic array containing basic measurement information per Wi-Fi node.
       The ranging scan can contain information from multiple measurements.*/

  uint32_t detailedMeasurementsInfo_len;  /**< Must be set to # of elements in detailedMeasurementsInfo */
  sLOWIRangingMeasurementDetailedInfo_v01 detailedMeasurementsInfo[LOWI_MAX_RANGING_MEASUREMENTS_V01];
  /**<   Dynamic array containing detailed measurement information per Wi-Fi node.
       The ranging scan can contain information from multiple measurements.*/
}sLowiRangingScanMeasurement_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t tx_chain_info_len;  /**< Must be set to # of elements in tx_chain_info */
  uint8_t tx_chain_info[LOWI_MAX_RANGING_MEASUREMENTS_V01];
  /**<   \n Array containing the tx chain mask used in ranging.
   * Consider following e.g if chain 0 is used then mask will be
   * 0x01, for chain 1 mask will be 0x02, for both chain
   * mask will be 0x03.*/

  uint32_t rx_chain_info_len;  /**< Must be set to # of elements in rx_chain_info */
  uint8_t rx_chain_info[LOWI_MAX_RANGING_MEASUREMENTS_V01];
  /**<   \n Array containing the rx chain mask used in ranging.
   * Consider following e.g if chain 0 is used then mask will be
   * 0x01, for chain 1 mask will be 0x02, for both chain
   * mask will be 0x03.*/
}sLowiRangingTxRxChainInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to perform Ranging measurements. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Ranging scan measurements of all Wifi Nodes */
  uint8_t scans_valid;  /**< Must be set to true if scans is being passed */
  uint32_t scans_len;  /**< Must be set to # of elements in scans */
  sLowiRangingScanMeasurement_v01 scans[LOWI_MAX_NODES_ALLOWED_RANGING_RESPONSE_V01];
  /**<   Dynamic array containing the ranging results for requested nodes. */

  /* Optional */
  /*  Last Indication */
  uint8_t isLast_valid;  /**< Must be set to true if isLast is being passed */
  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the series. TRUE if it is the last,
   FALSE if there is more to follow. */

  /* Optional */
  /*  Scan Status */
  uint8_t scanStatus_valid;  /**< Must be set to true if scanStatus is being passed */
  eQmiLowiScanStatus_v01 scanStatus;
  /**<   Scan status.*/

  /* Optional */
  /*  Source of the request */
  uint8_t sourceName_valid;  /**< Must be set to true if sourceName is being passed */
  char sourceName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   \n String containing the name of the Source that triggered the request. */

  /* Optional */
  /*  Tx and Rx chain information used in ranging */
  uint8_t txRxChainInfo_valid;  /**< Must be set to true if txRxChainInfo is being passed */
  uint32_t txRxChainInfo_len;  /**< Must be set to # of elements in txRxChainInfo */
  sLowiRangingTxRxChainInfo_v01 txRxChainInfo[LOWI_MAX_NODES_ALLOWED_RANGING_RESPONSE_V01];
}QmiLowiRangingResponseInd_v01;  /* Message */
/**
    @}
  */

typedef uint64_t maskExtRangingReqParams_v01;
#define LOWI_EXT_RANGING_RESULTS_LCI_V01 ((maskExtRangingReqParams_v01)0x00000001ull) /**<  LCI info request for the Wifi Node */
#define LOWI_EXT_RANGING_RESULTS_LCR_V01 ((maskExtRangingReqParams_v01)0x00000002ull) /**<  LCR info request for the Wifi Node */
#define LOWI_EXT_RANGING_RESULTS_COLOCATED_V01 ((maskExtRangingReqParams_v01)0x00000004ull) /**<  Co-located info request for the Wifi Node */
/** @addtogroup lowi_qmi_messages
    @{
  */
/** Request Message; Allows clients to get extended Ranging results. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  Originator of the request */
  uint8_t clientName_valid;  /**< Must be set to true if clientName is being passed */
  char clientName[LOWI_MAX_CLIENT_NAME_STRING_LEN_V01 + 1];
  /**<   String containing the name of the Originator of the request. */

  /* Optional */
  /*  Mask to pass request params */
  uint8_t params_valid;  /**< Must be set to true if params is being passed */
  maskExtRangingReqParams_v01 params;
  /**<   Mask to indicate the type of data expected in response. */

  /* Optional */
  /*  BSSID of the Wifi Node */
  uint8_t bssids_valid;  /**< Must be set to true if bssids is being passed */
  uint32_t bssids_len;  /**< Must be set to # of elements in bssids */
  sQmiLowiMacAddress_v01 bssids[LOWI_MAX_NODES_ALLOWED_RANGING_V01];
  /**<   BSSIDs of the Wi-Fi nodes for which exended ranging result is expected. */
}QmiLowiExtRangingResultsRequest_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lowi_qmi_messages
    @{
  */
/** Indication Message; Allows clients to get extended Ranging results. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint64_t requestId;
  /**<   Opaque request ID. Passed back in the response indication. */

  /* Optional */
  /*  BSSID of the Wifi Node */
  uint8_t bssid_valid;  /**< Must be set to true if bssid is being passed */
  sQmiLowiMacAddress_v01 bssid;
  /**<   BSSID for the Wi-Fi node for which the result are provided. */

  /* Optional */
  /*  LCI Info */
  uint8_t lciInfo_valid;  /**< Must be set to true if lciInfo is being passed */
  sLOWILocationIE_v01 lciInfo;
  /**<   LCI information element */

  /* Optional */
  /*  LCR Info */
  uint8_t lcrInfo_valid;  /**< Must be set to true if lcrInfo is being passed */
  sLOWILocationIE_v01 lcrInfo;
  /**<   LCR information element */

  /* Optional */
  /*  Request ID */
  uint8_t colocatedBssids_valid;  /**< Must be set to true if colocatedBssids is being passed */
  uint32_t colocatedBssids_len;  /**< Must be set to # of elements in colocatedBssids */
  sQmiLowiMacAddress_v01 colocatedBssids[LOWI_MAX_COLOCATED_BSSIDS_V01];
  /**<   Contains the explicit list of the Virtual mac addresses
   * which share the same antenna connector.
   * This field contains explicit Bssids only, if the maxBssidsIndicator field is 0. */

  /* Optional */
  /*  Last Indication */
  uint8_t isLast_valid;  /**< Must be set to true if isLast is being passed */
  uint8_t isLast;
  /**<   Indicates whether this is the last indication of the series. TRUE if it is the last,
   FALSE if there are more to follow. */
}QmiLowiExtRangingResultsResponseInd_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_LOWI_BATCHING_SUBSCRIPTION_V01 
//#define REMOVE_QMI_LOWI_BGSCAN_BATCHING_SUBSCRIPTION_V01 
//#define REMOVE_QMI_LOWI_CAPABILITIES_V01 
//#define REMOVE_QMI_LOWI_DISCOVERY_V01 
//#define REMOVE_QMI_LOWI_EXT_RANGING_RESULTS_V01 
//#define REMOVE_QMI_LOWI_GET_BATCHING_RESULTS_V01 
//#define REMOVE_QMI_LOWI_GET_BGSCAN_BATCHING_RESULTS_V01 
//#define REMOVE_QMI_LOWI_RANGING_V01 
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
#define QMI_LOWI_RANGING_REQ_V01 0x0029
#define QMI_LOWI_RANGING_RESP_V01 0x0029
#define QMI_LOWI_RANGING_IND_V01 0x0029
#define QMI_LOWI_EXT_RANGING_RESULTS_REQ_V01 0x002A
#define QMI_LOWI_EXT_RANGING_RESULTS_RESP_V01 0x002A
#define QMI_LOWI_EXT_RANGING_RESULTS_IND_V01 0x002A
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

