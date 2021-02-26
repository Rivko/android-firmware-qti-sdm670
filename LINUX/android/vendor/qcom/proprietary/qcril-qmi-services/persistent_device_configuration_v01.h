#ifndef PDC_SERVICE_01_H
#define PDC_SERVICE_01_H
/**
  @file persistent_device_configuration_v01.h

  @brief This is the public header file which defines the pdc service Data structures.

  This header file defines the types and structures that were defined in
  pdc. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.AT.4.0.c1/Main/modem_proc/qmimsgs/pdc/api/persistent_device_configuration_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Oct  3 2017 (Spin 0)
   From IDL File: persistent_device_configuration_v01.idl */

/** @defgroup pdc_qmi_consts Constant values defined in the IDL */
/** @defgroup pdc_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup pdc_qmi_enums Enumerated types used in QMI messages */
/** @defgroup pdc_qmi_messages Structures sent as QMI messages */
/** @defgroup pdc_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup pdc_qmi_accessor Accessor for QMI service object */
/** @defgroup pdc_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup pdc_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define PDC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define PDC_V01_IDL_MINOR_VERS 0x0D
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define PDC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define PDC_V01_MAX_MESSAGE_ID 0x0030
/**
    @}
  */


/** @addtogroup pdc_qmi_consts
    @{
  */
#define PDC_CONFIG_ID_SIZE_MAX_V01 124
#define PDC_CONFIG_DESC_SIZE_MAX_V01 255
#define PDC_CONFIG_LIST_SIZE_MAX_V01 25
#define PDC_CONFIG_FRAME_SIZE_MAX_V01 32768
#define PDC_CONFIG_PATH_SIZE_MAX_V01 255
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_CONFIG_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_CONFIG_TYPE_MODEM_PLATFORM_V01 = 0x00, /**<  Modem platform configuration type  */
  PDC_CONFIG_TYPE_MODEM_SW_V01 = 0x01, /**<  Modem software configuration type  */
  PDC_CONFIG_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_config_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_STORAGE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_STORAGE_LOCAL_V01 = 0x00, /**<  Local storage for the configuration  */
  PDC_STORAGE_REMOTE_V01 = 0x01, /**<  Remote storage for the configuration  */
  PDC_STORAGE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_storage_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_ACTIVATION_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_ACTIVATION_REGULAR_V01 = 0x00, /**<  Regular activation for selected configuration(s).  */
  PDC_ACTIVATION_REFRESH_ONLY_V01 = 0x01, /**<  Refresh modem without selecting any configuration; used
       for APPS centric configuration update. 
   */
  PDC_ACTIVATION_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_activation_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_REFRESH_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_REFRESH_MODE_DISABLED_V01 = 0x00, /**<  Refresh disabled  */
  PDC_REFRESH_MODE_ENABLED_V01 = 0x01, /**<  Refresh enabled  */
  PDC_REFRESH_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_refresh_mode_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_SELECTION_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_SELECTION_MODE_DISABLED_V01 = 0x00, /**<  Selection disabled  */
  PDC_SELECTION_MODE_IIN_V01 = 0x01, /**<  IIN based selection  */
  PDC_SELECTION_MODE_IMSI_V01 = 0x02, /**<  IMSI based selection  */
  PDC_SELECTION_MODE_HYBRID_V01 = 0x03, /**<  IIN selection first, then IMSI based selection if there's mismatch  */
  PDC_SELECTION_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_selection_mode_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_SELECTION_CARRIER_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_SELECTION_OPEN_MARKET_V01 = 0x00, 
  PDC_SELECTION_CARRIER_VZW_V01 = 0x01, 
  PDC_SELECTION_CARRIER_SPRINT_V01 = 0x02, 
  PDC_SELECTION_CARRIER_ATT_V01 = 0x03, 
  PDC_SELECTION_CARRIER_VODAFONE_V01 = 0x04, 
  PDC_SELECTION_CARRIER_TMOBILE_V01 = 0x05, 
  PDC_SELECTION_CARRIER_TELUS_V01 = 0x06, 
  PDC_SELECTION_CARRIER_KDDI_V01 = 0x07, 
  PDC_SELECTION_CARRIER_GEN_UMTS_EU_V01 = 0x08, 
  PDC_SELECTION_CARRIER_GEN_UMTS_NA_V01 = 0x09, 
  PDC_SELECTION_CARRIER_GEN_C2K_V01 = 0x0A, 
  PDC_SELECTION_CARRIER_ORANGE_V01 = 0x0B, 
  PDC_SELECTION_CARRIER_TELEFONICA_V01 = 0x0C, 
  PDC_SELECTION_CARRIER_DOCOMO_V01 = 0x0D, 
  PDC_SELECTION_CARRIER_TEL_ITALIA_V01 = 0x0E, 
  PDC_SELECTION_CARRIER_TELSTRA_V01 = 0x0F, 
  PDC_SELECTION_CARRIER_LUCACELL_V01 = 0x10, 
  PDC_SELECTION_CARRIER_BELL_MOB_V01 = 0x11, 
  PDC_SELECTION_CARRIER_TELCOM_NZ_V01 = 0x12, 
  PDC_SELECTION_CARRIER_CHINA_TEL_V01 = 0x13, 
  PDC_SELECTION_CARRIER_C2K_OMH_V01 = 0x14, 
  PDC_SELECTION_CARRIER_CHINA_UNI_V01 = 0x15, 
  PDC_SELECTION_CARRIER_AMX_V01 = 0x16, 
  PDC_SELECTION_CARRIER_NORX_V01 = 0x17, 
  PDC_SELECTION_CARRIER_US_CELLULAR_V01 = 0x18, 
  PDC_SELECTION_CARRIER_WONE_V01 = 0x19, 
  PDC_SELECTION_CARRIER_AIRTEL_V01 = 0x1A, 
  PDC_SELECTION_CARRIER_RELIANCE_V01 = 0x1B, 
  PDC_SELECTION_CARRIER_SOFTBANK_V01 = 0x1C, 
  PDC_SELECTION_CARRIER_DT_V01 = 0x1F, 
  PDC_SELECTION_CARRIER_CMCC_V01 = 0x20, 
  PDC_SELECTION_CARRIER_VIVO_V01 = 0x21, 
  PDC_SELECTION_CARRIER_EE_V01 = 0x22, 
  PDC_SELECTION_CARRIER_CHERRY_V01 = 0x23, 
  PDC_SELECTION_CARRIER_IMOBILE_V01 = 0x24, 
  PDC_SELECTION_CARRIER_SMARTFREN_V01 = 0x25, 
  PDC_SELECTION_CARRIER_LGU_V01 = 0x26, 
  PDC_SELECTION_CARRIER_SKT_V01 = 0x27, 
  PDC_SELECTION_CARRIER_TTA_V01 = 0x28, 
  PDC_SELECTION_CARRIER_BEELINE_V01 = 0x29, 
  PDC_SELECTION_CARRIER_H3G_V01 = 0x2A, 
  PDC_SELECTION_CARRIER_TIM_V01 = 0x2B, 
  PDC_SELECTION_CARRIER_MOV_V01 = 0x2C, 
  PDC_SELECTION_CARRIER_YTL_V01 = 0x2D, 
  PDC_SELECTION_CARRIER_AIS_V01 = 0x2E, 
  PDC_SELECTION_CARRIER_TRUEMOVE_V01 = 0x2F, 
  PDC_SELECTION_CARRIER_DTAC_V01 = 0x30, 
  PDC_SELECTION_CARRIER_HKT_V01 = 0x31, 
  PDC_SELECTION_CARRIER_M1_V01 = 0x32, 
  PDC_SELECTION_CARRIER_STARHUB_V01 = 0x33, 
  PDC_SELECTION_CARRIER_SINGTEL_V01 = 0x34, 
  PDC_SELECTION_CARRIER_CSL_V01 = 0x35, 
  PDC_SELECTION_CARRIER_3HK_V01 = 0x36, 
  PDC_SELECTION_CARRIER_SMARTONE_V01 = 0x37, 
  PDC_SELECTION_CARRIER_SFR_V01 = 0x38, 
  PDC_SELECTION_CARRIER_PI_V01 = 0x39, 
  PDC_SELECTION_CARRIER_RUSSIA_V01 = 0x3A, 
  PDC_SELECTION_CARRIER_BOUYGUES_V01 = 0x3B, 
  PDC_SELECTION_CARRIER_MEGAFON_V01 = 0x3C, 
  PDC_SELECTION_CARRIER_UMOBILE_V01 = 0x3D, 
  PDC_SELECTION_CARRIER_SUNRISE_V01 = 0x3E, 
  PDC_SELECTION_CARRIER_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_selection_carrier_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_REFRESH_EVENT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_EVENT_REFRESH_START_V01 = 0x00, /**<  Refresh start event  */
  PDC_EVENT_REFRESH_COMPLETE_V01 = 0x01, /**<  Refresh complete event  */
  PDC_REFRESH_EVENT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_refresh_event_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_DEPLOYMENT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_DEPLOYMENT_TYPE_COMMERCIAL_V01 = 0x00, /**<  Commercial configuration type  */
  PDC_DEPLOYMENT_TYPE_LAB_V01 = 0x01, /**<  Lab configuration type  */
  PDC_DEPLOYMENT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_deployment_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_DEVICE_MODE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_DEVICE_MODE_SS_V01 = 0x01, 
  PDC_DEVICE_MODE_SR_DSDS_V01 = 0x02, 
  PDC_DEVICE_MODE_DR_DSDS_V01 = 0x03, /**<  Device Mode type  */
  PDC_DEVICE_MODE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_device_mode_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_OS_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_OS_TYPE_NONE_V01 = 0x01, 
  PDC_OS_TYPE_ANDROID_V01 = 0x02, 
  PDC_OS_TYPE_WINDOWS_V01 = 0x03, /**<  OS type  */
  PDC_OS_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_os_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_enums
    @{
  */
typedef enum {
  PDC_MARKET_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PDC_MARKET_TYPE_GENERIC_V01 = 0x00, 
  PDC_MARKET_TYPE_AUTOMOTIVE_V01 = 0x01, /**<  Market type  */
  PDC_MARKET_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pdc_market_type_enum_v01;
/**
    @}
  */

/** @addtogroup pdc_qmi_aggregates
    @{
  */
typedef struct {

  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /*  Configuration ID */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration.
  */
}pdc_config_info_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pdc_qmi_aggregates
    @{
  */
typedef struct {

  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: Clients are to ignore configuration types that are invalid or 
 unknown.
 */

  /*  Configuration ID */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration.
  */
}pdc_config_info_resp_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pdc_qmi_aggregates
    @{
  */
typedef struct {

  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /*  Configuration ID */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration.
  */

  /*  Total Configuration Size */
  uint32_t total_config_size;
  /**<   Total size of the configuration.
  */

  /*  Configuration Data Frame */
  uint32_t config_frame_len;  /**< Must be set to # of elements in config_frame */
  uint8_t config_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  /**<   Next frame of the configuration data to be stored.
  */
}pdc_load_config_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Resets the PDC state variables of the requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pdc_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Resets the PDC state variables of the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Maintains control point registration for service indications. */
typedef struct {

  /* Optional */
  /*  Component Configuration */
  uint8_t reg_config_change_valid;  /**< Must be set to true if reg_config_change is being passed */
  uint8_t reg_config_change;
  /**<   Controls the reporting of QMI_PDC_CONFIG_CHANGE_IND. Values: \n
       - 0x00 -- Disable (default)  \n
       - 0x01 -- Enable
   */
}pdc_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Maintains control point registration for service indications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication that a configuration has changed. */
typedef struct {

  /* Mandatory */
  /*  New Configuration */
  pdc_config_info_resp_type_v01 new_config_info;
}pdc_config_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Gets the active configuration. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID from which to query selected configuration.

       Note: If the value is greater than or equal to the number of subscriptions
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Slot ID */
  uint8_t slot_id_valid;  /**< Must be set to true if slot_id is being passed */
  uint32_t slot_id;
  /**<   Slot ID from which to query selected configuration.

       Note: If the value is greater than or equal to the number of slots
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */
}pdc_get_selected_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Gets the active configuration. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_selected_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the read selected configuration result. */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE              -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL          -- Internal error \n
       - 0x0010 -- QMI_ERR_NOT_ PROVISIONED   -- Specified configuration was 
                                                     not found
   */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated this indication.
  */

  /* Optional */
  /*  Active Configuration ID */
  uint8_t active_config_id_valid;  /**< Must be set to true if active_config_id is being passed */
  uint32_t active_config_id_len;  /**< Must be set to # of elements in active_config_id */
  uint8_t active_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the active configuration.
  */

  /* Optional */
  /*  Pending Configuration ID */
  uint8_t pending_config_id_valid;  /**< Must be set to true if pending_config_id is being passed */
  uint32_t pending_config_id_len;  /**< Must be set to # of elements in pending_config_id */
  uint8_t pending_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the pending configuration.
  */
}pdc_get_selected_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Sets an available configuration for the device.
    \label{idl:setSelectConfig} */
typedef struct {

  /* Mandatory */
  /*  New Configuration */
  pdc_config_info_req_type_v01 new_config_info;

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID to set the selected configuration.

       Note: If the value is greater than or equal to the number of subscriptions the 
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Slot ID */
  uint8_t slot_id_valid;  /**< Must be set to true if slot_id is being passed */
  uint32_t slot_id;
  /**<   Slot ID to set the selected configuration.

       Note: If the value is greater than or equal to the number of slots the 
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */
}pdc_set_selected_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Sets an available configuration for the device.
    \label{idl:setSelectConfig} */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_set_selected_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the set selected configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE        -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL    -- Internal error \n
       - 0x0029 -- QMI_ERR_INVALID_ID  -- Specified argument already exists 
       
    */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */
}pdc_set_selected_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Lists the configurations currently available on the device.
    \label{idl:listConfig} */
typedef struct {

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  List Configuration Type */
  uint8_t config_type_valid;  /**< Must be set to true if config_type is being passed */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Support for multiple list request */
  uint8_t multi_support_valid;  /**< Must be set to true if multi_support is being passed */
  uint8_t multi_support;
  /**<   Flag to indicate whether client support for multiple list request
  */
}pdc_list_configs_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Lists the configurations currently available on the device.
    \label{idl:listConfig} */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_list_configs_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the list configuration result. */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE               -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL           -- Internal error
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Configuration List */
  uint8_t config_list_valid;  /**< Must be set to true if config_list is being passed */
  uint32_t config_list_len;  /**< Must be set to # of elements in config_list */
  pdc_config_info_resp_type_v01 config_list[PDC_CONFIG_LIST_SIZE_MAX_V01];

  /* Optional */
  /*  More flag */
  uint8_t more_available_valid;  /**< Must be set to true if more_available is being passed */
  uint8_t more_available;
  /**<   Flag to indicate if there're more configs available to be queried
  */
}pdc_list_configs_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Deletes a configuration from the device. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Configuration ID */
  uint8_t config_id_valid;  /**< Must be set to true if config_id is being passed */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration.
  */
}pdc_delete_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Deletes a configuration from the device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_delete_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the delete configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE        -- Success \n 
       - 0x0003 -- QMI_ERR_INTERNAL    -- Internal error \n
       - 0x0029 -- QMI_ERR_INVALID_ID  -- Specified argument already exists
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */
}pdc_delete_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Loads the specified configuration to device memory. */
typedef struct {

  /* Mandatory */
  /*  Load Configuration */
  pdc_load_config_info_type_v01 load_config_info;

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Storage Mode */
  uint8_t storage_valid;  /**< Must be set to true if storage is being passed */
  pdc_storage_enum_v01 storage;
  /**<   Storage mode. Values: \n
      - PDC_STORAGE_LOCAL (0x00) --  Local storage for the configuration 
      - PDC_STORAGE_REMOTE (0x01) --  Remote storage for the configuration 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */
}pdc_load_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Loads the specified configuration to device memory. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */

  /* Optional */
  /*  Frame Data Reset */
  uint8_t frame_data_reset_valid;  /**< Must be set to true if frame_data_reset is being passed */
  uint8_t frame_data_reset;
  /**<   Frame data has been reset due to an error. Value:\n
       - 0x01 -- Frame data reset
   */
}pdc_load_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the load configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE         -- Success \n
       - 0x0002 -- QMI_ERR_NO_MEMORY    -- Insufficient memory to store 
                                               the configuration \n
       - 0x0003 -- QMI_ERR_INTERNAL     -- Internal error \n
       - 0x0022 -- QMI_ERR_ AUTHENTICATION_FAILED -- Configuration 
                                                         authentication failed \n
       - 0x0029 -- QMI_ERR_INVALID_ID   -- Specified argument already 
                                               exists \n
       - 0x0030 -- QMI_ERR_INVALID_ARG  -- Sum of the frame data is  
                                               greater than the total
  
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Received Configuration Size */
  uint8_t received_config_size_valid;  /**< Must be set to true if received_config_size is being passed */
  uint32_t received_config_size;
  /**<   Total size of the configuration data received.
  */

  /* Optional */
  /*  Remaining Configuration Size */
  uint8_t remaining_config_size_valid;  /**< Must be set to true if remaining_config_size is being passed */
  uint32_t remaining_config_size;
  /**<   Total size of the configuration data remaining.
  */

  /* Optional */
  /*  Frame Data Reset */
  uint8_t frame_data_reset_valid;  /**< Must be set to true if frame_data_reset is being passed */
  uint8_t frame_data_reset;
  /**<   Frame data has been reset due to an error. Value:\n 
       - 0x01 -- Frame data reset
   */
}pdc_load_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Activates a pending configuration for the component.
    \label{idl:activeConfig} */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Activation Type */
  uint8_t activation_type_valid;  /**< Must be set to true if activation_type is being passed */
  pdc_activation_type_enum_v01 activation_type;
  /**<   Type of activation. Values: \n
      - PDC_ACTIVATION_REGULAR (0x00) --  Regular activation for selected configuration(s). 
      - PDC_ACTIVATION_REFRESH_ONLY (0x01) --  Refresh modem without selecting any configuration; used
       for APPS centric configuration update. 
  

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID from which to activate.
  */

  /* Optional */
  /*  Slot ID */
  uint8_t slot_id_valid;  /**< Must be set to true if slot_id is being passed */
  uint32_t slot_id;
  /**<   Slot ID associated with the Subscription ID from which to activate; the 
       mapping of the Slot ID and Subscription ID should be consistent with the 
       UIM session type.
  */
}pdc_activate_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Activates a pending configuration for the component.
    \label{idl:activeConfig} */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_activate_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the activate configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE             -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL         -- Internal error \n
       - 0x0010 -- QMI_ERR_NOT_ PROVISIONED -- Specified configuration was
                                                   not found
        
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */
}pdc_activate_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Queries additional information for a configuration. */
typedef struct {

  /* Mandatory */
  /*  Configuration ID */
  pdc_config_info_req_type_v01 new_config_info;

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */
}pdc_get_config_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Queries additional information for a configuration. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_config_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the read configuration information result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE          -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL      -- Internal error \n
       - 0x0029 -- QMI_ERR_INVALID_ID    -- Specified argument already exists \n
       - 0x0052 -- QMI_ERR_ACCESS_ DENIED -- Access to the remote file path is 
                                             denied or failed
 
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Configuration Size */
  uint8_t config_size_valid;  /**< Must be set to true if config_size is being passed */
  uint32_t config_size;
  /**<   Size of the configuration in memory.
  */

  /* Optional */
  /*  Configuration Description */
  uint8_t config_desc_valid;  /**< Must be set to true if config_desc is being passed */
  uint32_t config_desc_len;  /**< Must be set to # of elements in config_desc */
  uint8_t config_desc[PDC_CONFIG_DESC_SIZE_MAX_V01];
  /**<   ASCII string containing the description of the configuration read from 
       memory.
  */

  /* Optional */
  /*  Configuration Version */
  uint8_t config_version_valid;  /**< Must be set to true if config_version is being passed */
  uint32_t config_version;
  /**<   Version of the configuration in memory.
  */

  /* Optional */
  /*  Storage Mode */
  uint8_t storage_valid;  /**< Must be set to true if storage is being passed */
  pdc_storage_enum_v01 storage;
  /**<   Storage mode. Values: \n
      - PDC_STORAGE_LOCAL (0x00) --  Local storage for the configuration 
      - PDC_STORAGE_REMOTE (0x01) --  Remote storage for the configuration 
 */

  /* Optional */
  /*  Path */
  uint8_t path_valid;  /**< Must be set to true if path is being passed */
  uint16_t path[PDC_CONFIG_PATH_SIZE_MAX_V01 + 1];
  /**<   ASCII string containing the last truncated 255 characters of the 
       configuration path on the remote.
  */

  /* Optional */
  /*  Configuration Base Version */
  uint8_t base_version_valid;  /**< Must be set to true if base_version is being passed */
  uint32_t base_version;
  /**<   Base version of the configuration in memory.
  */
}pdc_get_config_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Queries the maximum and current sizes for each configuration memory
           store. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */
}pdc_get_config_limits_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Queries the maximum and current sizes for each configuration memory
           store. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_config_limits_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the read configuration limits result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE               -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL           -- Internal error
  
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Maximum Configuration Size */
  uint8_t max_config_size_valid;  /**< Must be set to true if max_config_size is being passed */
  uint64_t max_config_size;
  /**<   Maximum size of the configurations in memory.
  */

  /* Optional */
  /*  Current Configuration Size */
  uint8_t curr_config_size_valid;  /**< Must be set to true if curr_config_size is being passed */
  uint64_t curr_config_size;
  /**<   Current size of the configurations in memory.
  */
}pdc_get_config_limits_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Gets the default configuration information for a specified
           configuration type currently embedded with the loaded image. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */
}pdc_get_default_config_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Gets the default configuration information for a specified
           configuration type currently embedded with the loaded image. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_default_config_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the default configuration result information. */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE               -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL           -- Internal error
  
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Configuration Version */
  uint8_t config_version_valid;  /**< Must be set to true if config_version is being passed */
  uint32_t config_version;
  /**<   Version of the configuration in memory.
  */

  /* Optional */
  /*  Configuration Size */
  uint8_t config_size_valid;  /**< Must be set to true if config_size is being passed */
  uint32_t config_size;
  /**<   Size of the configuration in memory.
  */

  /* Optional */
  /*  Configuration Description */
  uint8_t config_desc_valid;  /**< Must be set to true if config_desc is being passed */
  uint32_t config_desc_len;  /**< Must be set to # of elements in config_desc */
  uint8_t config_desc[PDC_CONFIG_DESC_SIZE_MAX_V01];
  /**<   ASCII string containing the description of the configuration read from 
       memory.
  */
}pdc_get_default_config_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Deactivates an active configuration for the component. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID to deactivate configuration from

       Note: If the value is greater than or equal to the number of subscriptions
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */
}pdc_deactivate_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Deactivates an active configuration for the component. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_deactivate_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the deactivate configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE             -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL         -- Internal error \n
       - 0x0010 -- QMI_ERR_NOT_ PROVISIONED -- Specified configuration was
                                                   not found
 
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */
}pdc_deactivate_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Validates a specified configuration for the component. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Configuration ID */
  uint8_t config_id_valid;  /**< Must be set to true if config_id is being passed */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration to be validated
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID to validate configuration from

       Note: If the value is greater than or equal to the number of subscriptions
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Path */
  uint8_t path_valid;  /**< Must be set to true if path is being passed */
  uint16_t path[PDC_CONFIG_PATH_SIZE_MAX_V01 + 1];
  /**<   ASCII string containing diff file path to be stored on the remote file server.
  */
}pdc_validate_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Validates a specified configuration for the component. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_validate_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the validate configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE             -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL         -- Internal error \n
       - 0x0010 -- QMI_ERR_NOT_ PROVISIONED -- Specified configuration was
                                                   not found
 
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Result Format */
  uint8_t result_format_valid;  /**< Must be set to true if result_format is being passed */
  uint32_t result_format;
  /**<   The version of result.
  */

  /* Optional */
  /*  Frame Index */
  uint8_t frame_index_valid;  /**< Must be set to true if frame_index is being passed */
  uint32_t frame_index;
  /**<   The index of data frame starting from 0, 0xFFFFFFFF inidicates the last frame.
  */

  /* Optional */
  /*  Result Data Frame */
  uint8_t result_frame_valid;  /**< Must be set to true if result_frame is being passed */
  uint32_t result_frame_len;  /**< Must be set to # of elements in result_frame */
  uint8_t result_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  /**<   Next frame of the configuration data to be stored.
  */
}pdc_validate_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Gets the configuration features. */
typedef struct {

  /* Mandatory */
  /*  Slot ID */
  uint32_t slot_id;
  /**<   Slot ID from which to query configuration features.

       Note: If the value is greater than or equal to the number of slots
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */
}pdc_get_feature_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Gets the configuration features. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_feature_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the read configuration feature result. */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE              -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL          -- Internal error \n
       - 0x0010 -- QMI_ERR_NOT_ PROVISIONED   -- Specified configuration was 
                                                     not found
   */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated this indication.
  */

  /* Optional */
  /*  Selection Mode */
  uint8_t selection_mode_valid;  /**< Must be set to true if selection_mode is being passed */
  pdc_selection_mode_enum_v01 selection_mode;
  /**<   Type of selection mode. Values: \n
      - PDC_SELECTION_MODE_DISABLED (0x00) --  Selection disabled 
      - PDC_SELECTION_MODE_IIN (0x01) --  IIN based selection 
      - PDC_SELECTION_MODE_IMSI (0x02) --  IMSI based selection 
      - PDC_SELECTION_MODE_HYBRID (0x03) --  IIN selection first, then IMSI based selection if there's mismatch 
 */

  /* Optional */
  /*  Selection Carrier */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  pdc_selection_carrier_enum_v01 carrier;
  /**<   Type of selection subsidized carrier or open market. Values: \n
      - PDC_SELECTION_OPEN_MARKET (0x00) -- 
      - PDC_SELECTION_CARRIER_VZW (0x01) -- 
      - PDC_SELECTION_CARRIER_SPRINT (0x02) -- 
      - PDC_SELECTION_CARRIER_ATT (0x03) -- 
      - PDC_SELECTION_CARRIER_VODAFONE (0x04) -- 
      - PDC_SELECTION_CARRIER_TMOBILE (0x05) -- 
      - PDC_SELECTION_CARRIER_TELUS (0x06) -- 
      - PDC_SELECTION_CARRIER_KDDI (0x07) -- 
      - PDC_SELECTION_CARRIER_GEN_UMTS_EU (0x08) -- 
      - PDC_SELECTION_CARRIER_GEN_UMTS_NA (0x09) -- 
      - PDC_SELECTION_CARRIER_GEN_C2K (0x0A) -- 
      - PDC_SELECTION_CARRIER_ORANGE (0x0B) -- 
      - PDC_SELECTION_CARRIER_TELEFONICA (0x0C) -- 
      - PDC_SELECTION_CARRIER_DOCOMO (0x0D) -- 
      - PDC_SELECTION_CARRIER_TEL_ITALIA (0x0E) -- 
      - PDC_SELECTION_CARRIER_TELSTRA (0x0F) -- 
      - PDC_SELECTION_CARRIER_LUCACELL (0x10) -- 
      - PDC_SELECTION_CARRIER_BELL_MOB (0x11) -- 
      - PDC_SELECTION_CARRIER_TELCOM_NZ (0x12) -- 
      - PDC_SELECTION_CARRIER_CHINA_TEL (0x13) -- 
      - PDC_SELECTION_CARRIER_C2K_OMH (0x14) -- 
      - PDC_SELECTION_CARRIER_CHINA_UNI (0x15) -- 
      - PDC_SELECTION_CARRIER_AMX (0x16) -- 
      - PDC_SELECTION_CARRIER_NORX (0x17) -- 
      - PDC_SELECTION_CARRIER_US_CELLULAR (0x18) -- 
      - PDC_SELECTION_CARRIER_WONE (0x19) -- 
      - PDC_SELECTION_CARRIER_AIRTEL (0x1A) -- 
      - PDC_SELECTION_CARRIER_RELIANCE (0x1B) -- 
      - PDC_SELECTION_CARRIER_SOFTBANK (0x1C) -- 
      - PDC_SELECTION_CARRIER_DT (0x1F) -- 
      - PDC_SELECTION_CARRIER_CMCC (0x20) -- 
      - PDC_SELECTION_CARRIER_VIVO (0x21) -- 
      - PDC_SELECTION_CARRIER_EE (0x22) -- 
      - PDC_SELECTION_CARRIER_CHERRY (0x23) -- 
      - PDC_SELECTION_CARRIER_IMOBILE (0x24) -- 
      - PDC_SELECTION_CARRIER_SMARTFREN (0x25) -- 
      - PDC_SELECTION_CARRIER_LGU (0x26) -- 
      - PDC_SELECTION_CARRIER_SKT (0x27) -- 
      - PDC_SELECTION_CARRIER_TTA (0x28) -- 
      - PDC_SELECTION_CARRIER_BEELINE (0x29) -- 
      - PDC_SELECTION_CARRIER_H3G (0x2A) -- 
      - PDC_SELECTION_CARRIER_TIM (0x2B) -- 
      - PDC_SELECTION_CARRIER_MOV (0x2C) -- 
      - PDC_SELECTION_CARRIER_YTL (0x2D) -- 
      - PDC_SELECTION_CARRIER_AIS (0x2E) -- 
      - PDC_SELECTION_CARRIER_TRUEMOVE (0x2F) -- 
      - PDC_SELECTION_CARRIER_DTAC (0x30) -- 
      - PDC_SELECTION_CARRIER_HKT (0x31) -- 
      - PDC_SELECTION_CARRIER_M1 (0x32) -- 
      - PDC_SELECTION_CARRIER_STARHUB (0x33) -- 
      - PDC_SELECTION_CARRIER_SINGTEL (0x34) -- 
      - PDC_SELECTION_CARRIER_CSL (0x35) -- 
      - PDC_SELECTION_CARRIER_3HK (0x36) -- 
      - PDC_SELECTION_CARRIER_SMARTONE (0x37) -- 
      - PDC_SELECTION_CARRIER_SFR (0x38) -- 
      - PDC_SELECTION_CARRIER_PI (0x39) -- 
      - PDC_SELECTION_CARRIER_RUSSIA (0x3A) -- 
      - PDC_SELECTION_CARRIER_BOUYGUES (0x3B) -- 
      - PDC_SELECTION_CARRIER_MEGAFON (0x3C) -- 
      - PDC_SELECTION_CARRIER_UMOBILE (0x3D) -- 
      - PDC_SELECTION_CARRIER_SUNRISE (0x3E) -- 
 */

  /* Optional */
  /*  Flexible Mapping */
  uint8_t flex_mapping_valid;  /**< Must be set to true if flex_mapping is being passed */
  uint8_t flex_mapping;
  /**<   Flexible mapping setting.
  */

  /* Optional */
  /*  Refresh Mode */
  uint8_t refresh_mode_valid;  /**< Must be set to true if refresh_mode is being passed */
  pdc_refresh_mode_enum_v01 refresh_mode;
  /**<   Type of refresh mode. Values: \n
      - PDC_REFRESH_MODE_DISABLED (0x00) --  Refresh disabled 
      - PDC_REFRESH_MODE_ENABLED (0x01) --  Refresh enabled 
 */

  /* Optional */
  /*  Device Mode type */
  uint8_t multisim_valid;  /**< Must be set to true if multisim is being passed */
  pdc_device_mode_type_enum_v01 multisim;
  /**<   Type of device_modes. Values: \n
      - PDC_DEVICE_MODE_SS (0x01) -- 
      - PDC_DEVICE_MODE_SR_DSDS (0x02) -- 
      - PDC_DEVICE_MODE_DR_DSDS (0x03) --  Device Mode type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  OS type */
  uint8_t os_valid;  /**< Must be set to true if os is being passed */
  pdc_os_type_enum_v01 os;
  /**<   Type of os Values: \n
      - PDC_OS_TYPE_NONE (0x01) -- 
      - PDC_OS_TYPE_ANDROID (0x02) -- 
      - PDC_OS_TYPE_WINDOWS (0x03) --  OS type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Market type */
  uint8_t market_valid;  /**< Must be set to true if market is being passed */
  pdc_market_type_enum_v01 market;
  /**<   Type of markets. Values: \n
      - PDC_MARKET_TYPE_GENERIC (0x00) -- 
      - PDC_MARKET_TYPE_AUTOMOTIVE (0x01) --  Market type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Deployment type */
  uint8_t deployment_valid;  /**< Must be set to true if deployment is being passed */
  pdc_deployment_type_enum_v01 deployment;
  /**<   Type of deployments. Values: \n
      - PDC_DEPLOYMENT_TYPE_COMMERCIAL (0x00) --  Commercial configuration type 
      - PDC_DEPLOYMENT_TYPE_LAB (0x01) --  Lab configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  VoLTE capability */
  uint8_t ims_feature_valid;  /**< Must be set to true if ims_feature is being passed */
  uint16_t ims_feature;
  /**<   ims_feature setting
  */

  /* Optional */
  /*  Variant selection */
  uint8_t variant_valid;  /**< Must be set to true if variant is being passed */
  uint16_t variant;
  /**<   Variant setting. To be used for misc. variations
  */

  /* Optional */
  /*  Country Selection */
  uint8_t country_valid;  /**< Must be set to true if country is being passed */
  uint16_t country;
  /**<   Type of country id. Values: \n
       
       Note: Country codes are based on those listed in ISO 3166 standard.
  */
}pdc_get_feature_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Sets configuration features for the device.
    \label{idl:setSelectConfig} */
typedef struct {

  /* Mandatory */
  /*  Slot ID */
  uint32_t slot_id;
  /**<   Slot ID to set the configuration features.

       Note: If the value is greater than or equal to the number of slots the 
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Selection Mode */
  uint8_t selection_mode_valid;  /**< Must be set to true if selection_mode is being passed */
  pdc_selection_mode_enum_v01 selection_mode;
  /**<   Type of selection mode. Values: \n
      - PDC_SELECTION_MODE_DISABLED (0x00) --  Selection disabled 
      - PDC_SELECTION_MODE_IIN (0x01) --  IIN based selection 
      - PDC_SELECTION_MODE_IMSI (0x02) --  IMSI based selection 
      - PDC_SELECTION_MODE_HYBRID (0x03) --  IIN selection first, then IMSI based selection if there's mismatch 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Selection Carrier */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  pdc_selection_carrier_enum_v01 carrier;
  /**<   Type of selection subsidized carrier or open market. Values: \n
      - PDC_SELECTION_OPEN_MARKET (0x00) -- 
      - PDC_SELECTION_CARRIER_VZW (0x01) -- 
      - PDC_SELECTION_CARRIER_SPRINT (0x02) -- 
      - PDC_SELECTION_CARRIER_ATT (0x03) -- 
      - PDC_SELECTION_CARRIER_VODAFONE (0x04) -- 
      - PDC_SELECTION_CARRIER_TMOBILE (0x05) -- 
      - PDC_SELECTION_CARRIER_TELUS (0x06) -- 
      - PDC_SELECTION_CARRIER_KDDI (0x07) -- 
      - PDC_SELECTION_CARRIER_GEN_UMTS_EU (0x08) -- 
      - PDC_SELECTION_CARRIER_GEN_UMTS_NA (0x09) -- 
      - PDC_SELECTION_CARRIER_GEN_C2K (0x0A) -- 
      - PDC_SELECTION_CARRIER_ORANGE (0x0B) -- 
      - PDC_SELECTION_CARRIER_TELEFONICA (0x0C) -- 
      - PDC_SELECTION_CARRIER_DOCOMO (0x0D) -- 
      - PDC_SELECTION_CARRIER_TEL_ITALIA (0x0E) -- 
      - PDC_SELECTION_CARRIER_TELSTRA (0x0F) -- 
      - PDC_SELECTION_CARRIER_LUCACELL (0x10) -- 
      - PDC_SELECTION_CARRIER_BELL_MOB (0x11) -- 
      - PDC_SELECTION_CARRIER_TELCOM_NZ (0x12) -- 
      - PDC_SELECTION_CARRIER_CHINA_TEL (0x13) -- 
      - PDC_SELECTION_CARRIER_C2K_OMH (0x14) -- 
      - PDC_SELECTION_CARRIER_CHINA_UNI (0x15) -- 
      - PDC_SELECTION_CARRIER_AMX (0x16) -- 
      - PDC_SELECTION_CARRIER_NORX (0x17) -- 
      - PDC_SELECTION_CARRIER_US_CELLULAR (0x18) -- 
      - PDC_SELECTION_CARRIER_WONE (0x19) -- 
      - PDC_SELECTION_CARRIER_AIRTEL (0x1A) -- 
      - PDC_SELECTION_CARRIER_RELIANCE (0x1B) -- 
      - PDC_SELECTION_CARRIER_SOFTBANK (0x1C) -- 
      - PDC_SELECTION_CARRIER_DT (0x1F) -- 
      - PDC_SELECTION_CARRIER_CMCC (0x20) -- 
      - PDC_SELECTION_CARRIER_VIVO (0x21) -- 
      - PDC_SELECTION_CARRIER_EE (0x22) -- 
      - PDC_SELECTION_CARRIER_CHERRY (0x23) -- 
      - PDC_SELECTION_CARRIER_IMOBILE (0x24) -- 
      - PDC_SELECTION_CARRIER_SMARTFREN (0x25) -- 
      - PDC_SELECTION_CARRIER_LGU (0x26) -- 
      - PDC_SELECTION_CARRIER_SKT (0x27) -- 
      - PDC_SELECTION_CARRIER_TTA (0x28) -- 
      - PDC_SELECTION_CARRIER_BEELINE (0x29) -- 
      - PDC_SELECTION_CARRIER_H3G (0x2A) -- 
      - PDC_SELECTION_CARRIER_TIM (0x2B) -- 
      - PDC_SELECTION_CARRIER_MOV (0x2C) -- 
      - PDC_SELECTION_CARRIER_YTL (0x2D) -- 
      - PDC_SELECTION_CARRIER_AIS (0x2E) -- 
      - PDC_SELECTION_CARRIER_TRUEMOVE (0x2F) -- 
      - PDC_SELECTION_CARRIER_DTAC (0x30) -- 
      - PDC_SELECTION_CARRIER_HKT (0x31) -- 
      - PDC_SELECTION_CARRIER_M1 (0x32) -- 
      - PDC_SELECTION_CARRIER_STARHUB (0x33) -- 
      - PDC_SELECTION_CARRIER_SINGTEL (0x34) -- 
      - PDC_SELECTION_CARRIER_CSL (0x35) -- 
      - PDC_SELECTION_CARRIER_3HK (0x36) -- 
      - PDC_SELECTION_CARRIER_SMARTONE (0x37) -- 
      - PDC_SELECTION_CARRIER_SFR (0x38) -- 
      - PDC_SELECTION_CARRIER_PI (0x39) -- 
      - PDC_SELECTION_CARRIER_RUSSIA (0x3A) -- 
      - PDC_SELECTION_CARRIER_BOUYGUES (0x3B) -- 
      - PDC_SELECTION_CARRIER_MEGAFON (0x3C) -- 
      - PDC_SELECTION_CARRIER_UMOBILE (0x3D) -- 
      - PDC_SELECTION_CARRIER_SUNRISE (0x3E) -- 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Flexible Mapping */
  uint8_t flex_mapping_valid;  /**< Must be set to true if flex_mapping is being passed */
  uint8_t flex_mapping;
  /**<   Flexible mapping setting.
  */

  /* Optional */
  /*  Refresh Mode */
  uint8_t refresh_mode_valid;  /**< Must be set to true if refresh_mode is being passed */
  pdc_refresh_mode_enum_v01 refresh_mode;
  /**<   Type of refresh mode. Values: \n
      - PDC_REFRESH_MODE_DISABLED (0x00) --  Refresh disabled 
      - PDC_REFRESH_MODE_ENABLED (0x01) --  Refresh enabled 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Device Mode type */
  uint8_t multisim_valid;  /**< Must be set to true if multisim is being passed */
  pdc_device_mode_type_enum_v01 multisim;
  /**<   Type of device_modes. Values: \n
      - PDC_DEVICE_MODE_SS (0x01) -- 
      - PDC_DEVICE_MODE_SR_DSDS (0x02) -- 
      - PDC_DEVICE_MODE_DR_DSDS (0x03) --  Device Mode type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  OS type */
  uint8_t os_valid;  /**< Must be set to true if os is being passed */
  pdc_os_type_enum_v01 os;
  /**<   Type of os Values: \n
      - PDC_OS_TYPE_NONE (0x01) -- 
      - PDC_OS_TYPE_ANDROID (0x02) -- 
      - PDC_OS_TYPE_WINDOWS (0x03) --  OS type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Market type */
  uint8_t market_valid;  /**< Must be set to true if market is being passed */
  pdc_market_type_enum_v01 market;
  /**<   Type of markets. Values: \n
      - PDC_MARKET_TYPE_GENERIC (0x00) -- 
      - PDC_MARKET_TYPE_AUTOMOTIVE (0x01) --  Market type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Deployment type */
  uint8_t deployment_valid;  /**< Must be set to true if deployment is being passed */
  pdc_deployment_type_enum_v01 deployment;
  /**<   Type of deployments. Values: \n
      - PDC_DEPLOYMENT_TYPE_COMMERCIAL (0x00) --  Commercial configuration type 
      - PDC_DEPLOYMENT_TYPE_LAB (0x01) --  Lab configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  VoLTE capability */
  uint8_t ims_feature_valid;  /**< Must be set to true if ims_feature is being passed */
  uint16_t ims_feature;
  /**<   ims_feature setting
  */

  /* Optional */
  /*  Variant selection */
  uint8_t variant_valid;  /**< Must be set to true if variant is being passed */
  uint16_t variant;
  /**<   Variant setting. To be used for misc. variations
  */

  /* Optional */
  /*  Country Selection */
  uint8_t country_valid;  /**< Must be set to true if country is being passed */
  uint16_t country;
  /**<   Type of country id. Values: \n
       
       Note: Country codes are based on those listed in ISO 3166 standard.
  */
}pdc_set_feature_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Sets configuration features for the device.
    \label{idl:setSelectConfig} */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_set_feature_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the set feature configuration result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE        -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL    -- Internal error \n
       - 0x0029 -- QMI_ERR_INVALID_ID  -- Specified argument already exists 
       
    */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */
}pdc_set_feature_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication for refresh events. */
typedef struct {

  /* Mandatory */
  /*  Refresh event */
  pdc_refresh_event_enum_v01 refresh_event;
  /**<   Refresh events. Values: \n
      - PDC_EVENT_REFRESH_START (0x00) --  Refresh start event 
      - PDC_EVENT_REFRESH_COMPLETE (0x01) --  Refresh complete event 
 */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID that refresh event associated to.
  */

  /* Optional */
  /*  Slot ID */
  uint8_t slot_id_valid;  /**< Must be set to true if slot_id is being passed */
  uint32_t slot_id;
  /**<   Slot ID that refresh event associated to.
  */
}pdc_refresh_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Request Message; Retrieves the entire configuration file from device. */
typedef struct {

  /* Mandatory */
  /*  Configuration Type */
  pdc_config_type_enum_v01 config_type;
  /**<   Type of configuration. Values: \n
      - PDC_CONFIG_TYPE_MODEM_PLATFORM (0x00) --  Modem platform configuration type 
      - PDC_CONFIG_TYPE_MODEM_SW (0x01) --  Modem software configuration type 

 Note: All other values are reserved, and the service responds with 
 QMI_ERR_NOT_SUPPORTED.
 */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token used to identify the indication sent when the request is complete.
  */

  /* Optional */
  /*  Configuration ID */
  uint8_t config_id_valid;  /**< Must be set to true if config_id is being passed */
  uint32_t config_id_len;  /**< Must be set to # of elements in config_id */
  uint8_t config_id[PDC_CONFIG_ID_SIZE_MAX_V01];
  /**<   Unique ID for the configuration to be retrieved
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t subscription_id_valid;  /**< Must be set to true if subscription_id is being passed */
  uint32_t subscription_id;
  /**<   Subscription ID of active configuration to be retrieved from device if 
       config_id is not provided

       Note: If the value is greater than or equal to the number of subscriptions
       device allows, the service responds with QMI_ERR_NOT_SUPPORTED 
  */

  /* Optional */
  /*  Frame Index */
  uint8_t frame_index_valid;  /**< Must be set to true if frame_index is being passed */
  uint32_t frame_index;
  /**<   The index of expected data frame starting from 0.

       Note: If frame index TLV is not present in the request, the indications for 
       all the frames will be sent out sequentially all together.     
  */
}pdc_get_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Response Message; Retrieves the entire configuration file from device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message 
                          definition.
  */
}pdc_get_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pdc_qmi_messages
    @{
  */
/** Indication Message; Indication with the read configuration limits result.  */
typedef struct {

  /* Mandatory */
  /*  Indication Error Code */
  qmi_error_type_v01 error;
  /**<   Error code. Values: \n
       
       - 0x0000 -- QMI_ERR_NONE               -- Success \n
       - 0x0003 -- QMI_ERR_INTERNAL           -- Internal error
  
  */

  /* Optional */
  /*  Indication Token */
  uint8_t ind_token_valid;  /**< Must be set to true if ind_token is being passed */
  uint32_t ind_token;
  /**<   Token passed in the request that generated.
  */

  /* Optional */
  /*  Frame Index */
  uint8_t frame_index_valid;  /**< Must be set to true if frame_index is being passed */
  uint32_t frame_index;
  /**<   The index of data frame starting from 0, 0xFFFFFFFF inidicates the last frame.
  */

  /* Optional */
  /*  Result Data Frame */
  uint8_t config_frame_valid;  /**< Must be set to true if config_frame is being passed */
  uint32_t config_frame_len;  /**< Must be set to # of elements in config_frame */
  uint8_t config_frame[PDC_CONFIG_FRAME_SIZE_MAX_V01];
  /**<   Next frame of the configuration data to be stored.
  */
}pdc_get_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_PDC_ACTIVATE_CONFIG_V01 
//#define REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01 
//#define REMOVE_QMI_PDC_DEACTIVATE_CONFIG_V01 
//#define REMOVE_QMI_PDC_DEACTIVATE_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_DELETE_CONFIG_V01 
//#define REMOVE_QMI_PDC_DELETE_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_INFO_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_INFO_IND_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_LIMITS_V01 
//#define REMOVE_QMI_PDC_GET_CONFIG_LIMITS_IND_V01 
//#define REMOVE_QMI_PDC_GET_DEFAULT_CONFIG_INFO_V01 
//#define REMOVE_QMI_PDC_GET_DEFAULT_CONFIG_INFO_IND_V01 
//#define REMOVE_QMI_PDC_GET_FEATURE_V01 
//#define REMOVE_QMI_PDC_GET_FEATURE_IND_V01 
//#define REMOVE_QMI_PDC_GET_SELECTED_CONFIG_V01 
//#define REMOVE_QMI_PDC_GET_SELECTED_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_GET_SUPPORTED_FIELDS_V01 
//#define REMOVE_QMI_PDC_GET_SUPPORTED_MSGS_V01 
//#define REMOVE_QMI_PDC_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_PDC_LIST_CONFIGS_V01 
//#define REMOVE_QMI_PDC_LIST_CONFIGS_IND_V01 
//#define REMOVE_QMI_PDC_LOAD_CONFIG_V01 
//#define REMOVE_QMI_PDC_LOAD_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_REFRESH_IND_V01 
//#define REMOVE_QMI_PDC_RESET_V01 
//#define REMOVE_QMI_PDC_SET_FEATURE_V01 
//#define REMOVE_QMI_PDC_SET_FEATURE_IND_V01 
//#define REMOVE_QMI_PDC_SET_SELECTED_CONFIG_V01 
//#define REMOVE_QMI_PDC_SET_SELECTED_CONFIG_IND_V01 
//#define REMOVE_QMI_PDC_VALIDATE_CONFIG_V01 
//#define REMOVE_QMI_PDC_VALIDATE_CONFIG_IND_V01 

/*Service Message Definition*/
/** @addtogroup pdc_qmi_msg_ids
    @{
  */
#define QMI_PDC_RESET_REQ_V01 0x0000
#define QMI_PDC_RESET_RESP_V01 0x0000
#define QMI_PDC_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_PDC_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_PDC_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_PDC_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_PDC_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_PDC_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_PDC_CONFIG_CHANGE_IND_V01 0x0021
#define QMI_PDC_GET_SELECTED_CONFIG_REQ_V01 0x0022
#define QMI_PDC_GET_SELECTED_CONFIG_RESP_V01 0x0022
#define QMI_PDC_GET_SELECTED_CONFIG_IND_V01 0x0022
#define QMI_PDC_SET_SELECTED_CONFIG_REQ_V01 0x0023
#define QMI_PDC_SET_SELECTED_CONFIG_RESP_V01 0x0023
#define QMI_PDC_SET_SELECTED_CONFIG_IND_V01 0x0023
#define QMI_PDC_LIST_CONFIGS_REQ_V01 0x0024
#define QMI_PDC_LIST_CONFIGS_RESP_V01 0x0024
#define QMI_PDC_LIST_CONFIGS_IND_V01 0x0024
#define QMI_PDC_DELETE_CONFIG_REQ_V01 0x0025
#define QMI_PDC_DELETE_CONFIG_RESP_V01 0x0025
#define QMI_PDC_DELETE_CONFIG_IND_V01 0x0025
#define QMI_PDC_LOAD_CONFIG_REQ_V01 0x0026
#define QMI_PDC_LOAD_CONFIG_RESP_V01 0x0026
#define QMI_PDC_LOAD_CONFIG_IND_V01 0x0026
#define QMI_PDC_ACTIVATE_CONFIG_REQ_V01 0x0027
#define QMI_PDC_ACTIVATE_CONFIG_RESP_V01 0x0027
#define QMI_PDC_ACTIVATE_CONFIG_IND_V01 0x0027
#define QMI_PDC_GET_CONFIG_INFO_REQ_V01 0x0028
#define QMI_PDC_GET_CONFIG_INFO_RESP_V01 0x0028
#define QMI_PDC_GET_CONFIG_INFO_IND_V01 0x0028
#define QMI_PDC_GET_CONFIG_LIMITS_REQ_V01 0x0029
#define QMI_PDC_GET_CONFIG_LIMITS_RESP_V01 0x0029
#define QMI_PDC_GET_CONFIG_LIMITS_IND_V01 0x0029
#define QMI_PDC_GET_DEFAULT_CONFIG_INFO_REQ_V01 0x002A
#define QMI_PDC_GET_DEFAULT_CONFIG_INFO_RESP_V01 0x002A
#define QMI_PDC_GET_DEFAULT_CONFIG_INFO_IND_V01 0x002A
#define QMI_PDC_DEACTIVATE_CONFIG_REQ_V01 0x002B
#define QMI_PDC_DEACTIVATE_CONFIG_RESP_V01 0x002B
#define QMI_PDC_DEACTIVATE_CONFIG_IND_V01 0x002B
#define QMI_PDC_VALIDATE_CONFIG_REQ_V01 0x002C
#define QMI_PDC_VALIDATE_CONFIG_RESP_V01 0x002C
#define QMI_PDC_VALIDATE_CONFIG_IND_V01 0x002C
#define QMI_PDC_GET_FEATURE_REQ_V01 0x002D
#define QMI_PDC_GET_FEATURE_RESP_V01 0x002D
#define QMI_PDC_GET_FEATURE_IND_V01 0x002D
#define QMI_PDC_SET_FEATURE_REQ_V01 0x002E
#define QMI_PDC_SET_FEATURE_RESP_V01 0x002E
#define QMI_PDC_SET_FEATURE_IND_V01 0x002E
#define QMI_PDC_REFRESH_IND_V01 0x002F
#define QMI_PDC_GET_CONFIG_REQ_V01 0x0030
#define QMI_PDC_GET_CONFIG_RESP_V01 0x0030
#define QMI_PDC_GET_CONFIG_IND_V01 0x0030
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro pdc_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type pdc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define pdc_get_service_object_v01( ) \
          pdc_get_service_object_internal_v01( \
            PDC_V01_IDL_MAJOR_VERS, PDC_V01_IDL_MINOR_VERS, \
            PDC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

