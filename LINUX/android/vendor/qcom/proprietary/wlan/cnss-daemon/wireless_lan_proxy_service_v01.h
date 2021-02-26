#ifndef WLPS_SERVICE_01_H
#define WLPS_SERVICE_01_H
/**
  @file wireless_lan_proxy_service_v01.h

  @brief This is the public header file which defines the wlps service Data structures.

  This header file defines the types and structures that were defined in
  wlps. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Jun 26 2017 (Spin 0)
   From IDL File: wireless_lan_proxy_service_v01.idl */

/** @defgroup wlps_qmi_consts Constant values defined in the IDL */
/** @defgroup wlps_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup wlps_qmi_enums Enumerated types used in QMI messages */
/** @defgroup wlps_qmi_messages Structures sent as QMI messages */
/** @defgroup wlps_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup wlps_qmi_accessor Accessor for QMI service object */
/** @defgroup wlps_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wlps_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define WLPS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define WLPS_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define WLPS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define WLPS_V01_MAX_MESSAGE_ID 0x0021
/**
    @}
  */


/** @addtogroup wlps_qmi_consts
    @{
  */
#define QMI_WLPS_CSI_OPTIONS_WLANFW_INSTANCE_ID_V01 11
#define QMI_WLPS_CSI_OPTIONS_ADSP_INSTANCE_ID_V01 12

/**  Maximum SSID length  */
#define QMI_WLPS_MAX_SSID_LEN_V01 32

/**  Maximum BSSID length  */
#define QMI_WLPS_MAX_BSSID_LEN_V01 6

/**  Maximum string length  */
#define QMI_WLPS_MAX_STR_LEN_V01 16

/**  Maximum number of channels  */
#define QMI_WLPS_MAX_NUM_CHAN_V01 128

/**  Maximum country code length  */
#define QMI_WLPS_COUNTRY_CODE_LEN_V01 3

/**  Maximum number of channels for RTT  */
#define QMI_WLPS_MAX_NUM_RTT_CHAN_V01 196
/**
    @}
  */

/** @addtogroup wlps_qmi_enums
    @{
  */
typedef enum {
  WLPS_VDEV_MODE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLPS_INFRA_STATION_V01 = 0, /**<  Station mode  */
  QMI_WLPS_SOFTAP_V01 = 1, /**<  Soft AP mode  */
  QMI_WLPS_P2P_CLIENT_V01 = 2, /**<  P2P client mode  */
  QMI_WLPS_P2P_GO_V01 = 3, /**<  P2P GO mode  */
  QMI_WLPS_MONITOR_V01 = 4, /**<  Monitor mode  */
  QMI_WLPS_FTM_V01 = 5, /**<  FTM mode  */
  QMI_WLPS_IBSS_V01 = 6, /**<  IBSS mode  */
  QMI_WLPS_P2P_DEVICE_V01 = 7, /**<  P2P device mode  */
  WLPS_VDEV_MODE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlps_vdev_mode_t_v01;
/**
    @}
  */

/** @addtogroup wlps_qmi_aggregates
    @{
  */
/**  WLAN Mode 
 Structure for virtual device basic information
 */
typedef struct {

  uint8_t vdev_id;
  /**<   Virtual device ID. */

  wlps_vdev_mode_t_v01 vdev_mode;
  /**<   Virtual device mode.  */
}wlps_vdev_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlps_qmi_aggregates
    @{
  */
/**  Structure for virtual device connection information
 */
typedef struct {

  uint8_t is_connected;
  /**<   Whether or not the virtual device connection is established. */

  int8_t rssi;
  /**<   RSSI level */

  uint8_t country_code[QMI_WLPS_COUNTRY_CODE_LEN_V01];
  /**<   Country code. */

  uint32_t freq;
  /**<   Connection frequency. */

  uint8_t bssid[QMI_WLPS_MAX_BSSID_LEN_V01];
  /**<   Basic service set identification. */

  uint32_t ssid_len;  /**< Must be set to # of elements in ssid */
  uint8_t ssid[QMI_WLPS_MAX_SSID_LEN_V01];
  /**<   Service set identifier. */
}wlps_vdev_conn_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlps_qmi_aggregates
    @{
  */
/**  Structure for RTT OEM channel information
 */
typedef struct {

  uint32_t chan_number;
  /**<   channel ID, channel number */

  uint32_t mhz;
  /**<   primary 20 MHz channel frequency in mhz */

  uint32_t band_center_freq1;
  /**<   Center frequency 1 in MHz*/

  uint32_t band_center_freq2;

  /*  Center frequency 2 in MHz - valid only for 11acvht 80plus80 mode */
  uint32_t info;

  /*  channel info described below  */
  uint32_t reg_info_1;

  /*  contains min power, max power, reg power and reg class id.   */
  uint32_t reg_info_2;
}wlps_rtt_oem_channel_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlps_qmi_messages
    @{
  */
/** Request Message; Sends the chip, host, and firmware version information. */
typedef struct {

  /* Mandatory */
  /*  Chip ID */
  uint32_t chip_id;
  /**<   Chip's unique identifier. */

  /* Mandatory */
  /*  Chip Name */
  char chip_name[QMI_WLPS_MAX_STR_LEN_V01];
  /**<   Chip's name. */

  /* Mandatory */
  /*  Chip Manufacturer */
  char chip_from[QMI_WLPS_MAX_STR_LEN_V01];
  /**<   Chip manufacturer. */

  /* Mandatory */
  /*  Host Driver Version */
  char host_version[QMI_WLPS_MAX_STR_LEN_V01];
  /**<   Version of the host driver. */

  /* Mandatory */
  /*  Firmware Driver Version */
  char fw_version[QMI_WLPS_MAX_STR_LEN_V01];
  /**<   Version of the firmware driver. */
}wlps_update_client_version_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlps_qmi_messages
    @{
  */
/** Response Message; Sends the chip, host, and firmware version information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlps_update_client_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlps_qmi_messages
    @{
  */
/** Request Message; Sends the WLAN status. */
typedef struct {

  /* Mandatory */
  /*  Firmware Service Bit */
  uint8_t fw_adsp_support;
  /**<    Indicates whether or not the firmware supports the service.
        The WLPS server ignores all messages if this bit is 0. */

  /* Mandatory */
  /*  WLAN Power Status */
  uint8_t is_on;
  /**<   Whether or not the power is on. Values: \n
       -TRUE -- Power is on \n
       -FALSE -- Power is off
  */

  /* Optional */
  /*  Virtual Device Information */
  uint8_t vdev_info_valid;  /**< Must be set to true if vdev_info is being passed */
  wlps_vdev_info_s_v01 vdev_info;

  /* Optional */
  /*  Virtual Device Connection Information */
  uint8_t vdev_conn_info_valid;  /**< Must be set to true if vdev_conn_info is being passed */
  wlps_vdev_conn_info_s_v01 vdev_conn_info;

  /* Optional */
  /*  Channel Information */
  uint8_t channel_info_valid;  /**< Must be set to true if channel_info is being passed */
  uint32_t channel_info_len;  /**< Must be set to # of elements in channel_info */
  uint8_t channel_info[QMI_WLPS_MAX_NUM_CHAN_V01];
  /**<   Channel information. */

  /* Optional */
  /*  RTT OEM Channel Information */
  uint8_t rtt_oem_channel_info_valid;  /**< Must be set to true if rtt_oem_channel_info is being passed */
  uint32_t rtt_oem_channel_info_len;  /**< Must be set to # of elements in rtt_oem_channel_info */
  wlps_rtt_oem_channel_info_s_v01 rtt_oem_channel_info[QMI_WLPS_MAX_NUM_RTT_CHAN_V01];
}wlps_update_client_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlps_qmi_messages
    @{
  */
/** Response Message; Sends the WLAN status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlps_update_client_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_WLPS_UPDATE_CLIENT_STATUS_V01 
//#define REMOVE_QMI_WLPS_UPDATE_CLIENT_VERSION_V01 

/*Service Message Definition*/
/** @addtogroup wlps_qmi_msg_ids
    @{
  */
#define QMI_WLPS_UPDATE_CLIENT_VERSION_REQ_V01 0x0020
#define QMI_WLPS_UPDATE_CLIENT_VERSION_RESP_V01 0x0020
#define QMI_WLPS_UPDATE_CLIENT_STATUS_REQ_V01 0x0021
#define QMI_WLPS_UPDATE_CLIENT_STATUS_RESP_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro wlps_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type wlps_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define wlps_get_service_object_v01( ) \
          wlps_get_service_object_internal_v01( \
            WLPS_V01_IDL_MAJOR_VERS, WLPS_V01_IDL_MINOR_VERS, \
            WLPS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

