#ifndef COEX_SERVICE_01_H
#define COEX_SERVICE_01_H
/**
  @file coexistence_service_v01.h
  
  @brief This is the public header file which defines the coex service Data structures.

  This header file defines the types and structures that were defined in 
  coex. It contains the constant values defined, enums, structures,
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


  $Header: //components/rel/qmimsgs.adsp/2.6/coex/api/coexistence_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5 
   It was generated on: Tue Oct  8 2013 (Spin 0)
   From IDL File: coexistence_service_v01.idl */

/** @defgroup coex_qmi_consts Constant values defined in the IDL */
/** @defgroup coex_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup coex_qmi_enums Enumerated types used in QMI messages */
/** @defgroup coex_qmi_messages Structures sent as QMI messages */
/** @defgroup coex_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup coex_qmi_accessor Accessor for QMI service object */
/** @defgroup coex_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup coex_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define COEX_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define COEX_V01_IDL_MINOR_VERS 0x0A
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define COEX_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define COEX_V01_MAX_MESSAGE_ID 0x0038
/** 
    @} 
  */


/** @addtogroup coex_qmi_consts 
    @{ 
  */
#define COEX_WLAN_MAX_WIFI_CHNLS_PER_CONN_V01 8
#define COEX_WWAN_MAX_BANDS_TO_MONITOR_V01 32
#define COEX_WWAN_MAX_BANDS_PER_TECH_V01 16
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t freq;
  /**<   Band center frequency in MHz. */

  uint32_t bandwidth;
  /**<   Bandwidth in MHz. */
}coex_band_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_band_type_v01 ul_band;
  /**<   UL frequency and bandwidth, set to 0 in receive only
     configurations. */

  coex_band_type_v01 dl_band;
  /**<   DL frequency and bandwidth. */
}coex_xsver_band_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t coex_indicate_band_direction_v01;
#define COEX_UL_BAND_DIRECTION_V01 ((coex_indicate_band_direction_v01)0x0000000000000001ull) /**<  Indicates whether or not the band information is for uplink.  */
#define COEX_DL_BAND_DIRECTION_V01 ((coex_indicate_band_direction_v01)0x0000000000000002ull) /**<  Indicates whether or not the band information is for downlink.  */
/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t freq;
  /**<   Band center frequency in kHz. */

  uint32_t bandwidth;
  /**<   Bandwidth in Hz. */

  coex_indicate_band_direction_v01 direction;
  /**<   Indicates whether the band is for uplink, downlink or both. */
}coex_band_type_and_direction_v01;  /* Type */
/**
    @}
  */

typedef uint64_t coex_policy_config_mask_v01;
#define COEX_PCM_TOGGLE_FRAME_SYNC_V01 ((coex_policy_config_mask_v01)0x0000000000000001ull) /**<  Toggle (or do not toggle) the FRAME_SYNC register/signal.  */
#define COEX_PCM_TOGGLE_TX_ACTIVE_V01 ((coex_policy_config_mask_v01)0x0000000000000002ull) /**<  Toggle (or do not toggle) the TX_ACTIVE register/signal line.  */
#define COEX_PCM_TOGGLE_RX_PRIORITY_V01 ((coex_policy_config_mask_v01)0x0000000000000004ull) /**<  Toggle (or do not toggle) the RX_PRIORITY register/signal line.  */
#define COEX_PCM_REACT_TO_WCN_PRIORITY_V01 ((coex_policy_config_mask_v01)0x0000000000000008ull) /**<  React (or do not react) to WCN_PRIORITY signal line/register.  */
#define COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01 ((coex_policy_config_mask_v01)0x0000000000000010ull) /**<  Send (or do not send) out the WCI-2 standard's Type 3 (inactivity duration) message.  */
#define COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE_V01 ((coex_policy_config_mask_v01)0x0000000000000020ull) /**<  Send (or do not send) out the WCI-2 standard's Type 6 Tx advance notice message.  */
#define COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01 ((coex_policy_config_mask_v01)0x0000000000000040ull) /**<  Enforce controller's Tx Power Limit.  */
#define COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 ((coex_policy_config_mask_v01)0x0000000000000080ull) /**<  React to and enforce the WCI-2 standard's Type 6 MWS Tx power limit.  */
/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_APT_TABLE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_APT_TABLE_DEFAULT_V01 = 0, /**<  Default (High Efficiency) APT table for RF  */
  COEX_APT_TABLE_SECONDARY_V01 = 1, /**<  Secondary (High Linearity) APT table for RF  */
  COEX_APT_TABLE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_apt_table_enum_v01;
/**
    @}
  */

typedef uint64_t coex_txfrmdnl_condition_failure_mask_v01;
#define COEX_TFDCFM_CONT_TX_FRAME_DENIAL_THLD_CROSSED_V01 ((coex_txfrmdnl_condition_failure_mask_v01)0x0000000000000001ull) /**<  Contiguous Tx frame denial threshold crossed  */
#define COEX_TFDCFM_TX_FRAME_DENIAL_DUTY_CYCLE_CROSSED_V01 ((coex_txfrmdnl_condition_failure_mask_v01)0x0000000000000002ull) /**<  Allowed Tx frame denial duty cycle crossed  */
typedef uint64_t coex_pwrlmt_condition_fail_mask_v01;
#define COEX_PLCFM_LINK_PATH_LOSS_THLD_CROSSED_V01 ((coex_pwrlmt_condition_fail_mask_v01)0x0000000000000001ull) /**<  Link path loss threshold was crossed  */
#define COEX_PLCFM_FILTERED_RB_THLD_CROSSED_V01 ((coex_pwrlmt_condition_fail_mask_v01)0x0000000000000002ull) /**<  Filtered RB usage threshold was crossed  */
#define COEX_PLCFM_UE_IN_RACH_V01 ((coex_pwrlmt_condition_fail_mask_v01)0x0000000000000004ull) /**<  UE is presently in RACH  */
#define COEX_PLCFM_RRC_PROCEDURE_ACTIVE_V01 ((coex_pwrlmt_condition_fail_mask_v01)0x0000000000000008ull) /**<  RRC procedure is active  */
#define COEX_PLCFM_WCI2_TX_PWRLMT_TIMED_OUT_V01 ((coex_pwrlmt_condition_fail_mask_v01)0x0000000000000010ull) /**<  WCI-2's Tx power limit enforce request timed out  */
typedef uint64_t coex_pwrlmt_condition_success_mask_v01;
#define COEX_PLCSM_WCI2_TX_PWR_LMT_ENFORCED_V01 ((coex_pwrlmt_condition_success_mask_v01)0x0000000000000001ull) /**<  WCI-2 standard's Type 6 MWS Tx power limit request was granted
       and enforced  */
#define COEX_PLCSM_CONTROLLER_TX_PWR_LMT_ENFORCED_V01 ((coex_pwrlmt_condition_success_mask_v01)0x0000000000000002ull) /**<  Controller's Tx power limit request was enforced  */
typedef uint64_t coex_enable_band_filter_mask_v01;
#define COEX_ENABLE_UL_BAND_INFO_V01 ((coex_enable_band_filter_mask_v01)0x0000000000000001ull) /**<  Enable or disable filtering the band info for uplink only  */
#define COEX_ENABLE_DL_BAND_INFO_V01 ((coex_enable_band_filter_mask_v01)0x0000000000000002ull) /**<  Enable or disable filtering band info for downlink only  */
/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_xsver_band_type_v01 band_info;
  /**<   Uplink and downlink frequency bandwidth details for this band */

  coex_enable_band_filter_mask_v01 band_mask;
  /**<   Enable or disable uplink and/or downlink filtering mask */
}coex_filter_band_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  float min;
  /**<   Minimum value for this range */

  float max;
  /**<   Maximum value for this range */
}coex_range_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t max_allowed_frame_denials;
  /**<   Number of maximum allowed frame denials over the window. */

  uint32_t frame_denial_window;
  /**<   Window of frames over which frame denial applies. */
}coex_frame_denial_duty_cycle_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WWAN_TECH_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TECH_V01 = 0, /**<  LTE  */
  COEX_TDSCDMA_TECH_V01 = 1, /**<  TD-SCDMA \n  */
  COEX_GSM_TECH_V01 = 2, /**<  GSM \n  */
  COEX_ONEX_TECH_V01 = 3, /**<  CDMA2000\textsuperscript{\textregistered} 1X \n  */
  COEX_HDR_TECH_V01 = 4, /**<  HDR \n  */
  COEX_WCDMA_TECH_V01 = 5, /**<  WCDMA \n  */
  COEX_WWAN_TECH_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wwan_tech_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_SCAN_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_SCAN_STOP_V01 = 0, /**<  WLAN stops scanning.  */
  COEX_WLAN_SCAN_START_V01 = 1, /**<  WLAN starts scanning (includes active and positive scan).  */
  COEX_WLAN_SCAN_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_scan_state_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_CONN_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_CONN_DISABLED_V01 = 0, /**<  WLAN connection has been disabled
        (either the connection was unsuccessful or torn down).  */
  COEX_WLAN_CONN_SETUP_V01 = 1, /**<  WLAN is setting up the connection
        (includes association or authentication or DHCP).  */
  COEX_WLAN_CONN_STEADY_V01 = 2, /**<  WLAN connection has been established and is in a steady state  */
  COEX_WLAN_CONN_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_conn_state_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_CONN_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_CONN_MODE_NONE_V01 = 0, /**<  WLAN is not in Connected mode.  */
  COEX_WLAN_CONN_MODE_STATION_V01 = 1, /**<  WLAN is in Station Connected mode.  */
  COEX_WLAN_CONN_MODE_SOFTAP_V01 = 2, /**<  WLAN is in Soft Access Point Connected mode.  */
  COEX_WLAN_CONN_MODE_P2P_GROUP_OWNER_V01 = 3, /**<  WLAN is in Peer-to-peer Group Owner Connected mode.  */
  COEX_WLAN_CONN_MODE_P2P_CLIENT_V01 = 4, /**<  WLAN is in Peer-to-peer Client Connected mode.  */
  COEX_WLAN_CONN_MODE_AMP_V01 = 5, /**<  WLAN is in Amplify Connected mode.  */
  COEX_WLAN_CONN_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_conn_mode_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t id;
  /**<   WLAN scan's unique ID allowing multiple scans
       to occur simultaneously.
	   Note: Scan ID is not valid after the scan stops.
  */

  coex_band_type_v01 band;
  /**<   WLAN frequency and bandwidth.
       Valid center frequencies are 2400 to 2500 MHz.
       Valid bandwidths are 20, 40, or 80 MHz.
  */

  coex_wlan_scan_state_enum_v01 state;
  /**<   WLAN scan's current state. Values: \n
     - 0x00 -- COEX_WLAN_SCAN_STOP -- WLAN stopped scanning \n
	 - 0x01 -- COEX_WLAN_SCAN_START -- WLAN started scanning
  */
}coex_wlan_scan_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t handle;
  /**<   Unique handle of the WLAN connection, allowing the service to
       track multiple WLAN connections.\n
       Note: Connection handle is not valid after it is disabled.
  */

  uint32_t band_len;  /**< Must be set to # of elements in band */
  coex_band_type_v01 band[COEX_WLAN_MAX_WIFI_CHNLS_PER_CONN_V01];
  /**<   WLAN frequency and bandwidth sets.
     Valid center frequencies are 2400 to 2500 MHz.
     Valid bandwidths are 20, 40, or 80 MHz.
	 Note: Accounts for the fact that the frequency occupation
	       of a single connection may include several disjoint
		   chunks of frequency.
  */

  coex_wlan_conn_state_enum_v01 state;
  /**<   Informs the service of the current state of the WLAN connection
        (based on the passed-in handle). Values: \n
     - 0x00 -- COEX_WLAN_CONN_ DISABLED -- Connection is disabled
       (either connection was unsuccessful or torn down) \n
	 - 0x01 -- COEX_WLAN_CONN_ SETUP -- Setting up connection
       includes association, authentication, or DHCP \n
	 - 0x02 -- COEX_WLAN_CONN_ STEADY -- Connection has been
       established and is in a steady state
  */

  coex_wlan_conn_mode_enum_v01 mode;
  /**<   Informs the service of the current connected mode for a
	   WLAN connection based on the passed-in handle. Values: \n
     - 0x00 -- COEX_WLAN_CONN_ MODE_NONE -- No connection has been
       established yet \n
	 - 0x01 -- COEX_WLAN_CONN_ MODE_STATION -- In Station
       Connected mode \n
	 - 0x02 -- COEX_WLAN_CONN_ MODE_SOFTAP -- In Soft Access
       Point Connected mode \n
	 - 0x03 -- COEX_WLAN_CONN_ MODE_P2P_GROUP_OWNER -- In
       Peer-to-peer Group Owner Connected mode \n
	 - 0x04 -- COEX_WLAN_CONN_ MODE_P2P_CLIENT -- In
       Peer-to-peer Client Connected mode \n
	 - 0x05 -- COEX_WLAN_CONN_ MODE_AMP -- In Amplify
       Connected mode
  */
}coex_wlan_conn_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_CONFIG_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_CONFIG_0_V01 = 0, 
  COEX_LTE_TDD_CONFIG_1_V01 = 1, 
  COEX_LTE_TDD_CONFIG_2_V01 = 2, 
  COEX_LTE_TDD_CONFIG_3_V01 = 3, 
  COEX_LTE_TDD_CONFIG_4_V01 = 4, 
  COEX_LTE_TDD_CONFIG_5_V01 = 5, 
  COEX_LTE_TDD_CONFIG_6_V01 = 6, 
  COEX_LTE_TDD_CONFIG_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_config_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_SUBFRAME_CONFIG_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_SUBFRAME_CONFIG_0_V01 = 0, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_1_V01 = 1, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_2_V01 = 2, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_3_V01 = 3, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_4_V01 = 4, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_5_V01 = 5, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_6_V01 = 6, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_7_V01 = 7, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_8_V01 = 8, 
  COEX_LTE_TDD_SUBFRAME_CONFIG_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_subframe_config_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_LINK_CONFIG_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_LINK_NORMAL_V01 = 0, /**<  Normal cyclic prefix.  */
  COEX_LTE_TDD_LINK_EXTENDED_V01 = 1, /**<   Extended cyclic prefix.  */
  COEX_LTE_TDD_LINK_CONFIG_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_link_config_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t frame_offset;
  /**<    LTE TDD frame offset in microseconds. */

  coex_lte_tdd_config_enum_v01 tdd_config;
  /**<    LTE TDD configuration. This value specifies which subframes
        are used for uplink, downlink, and special. Refer to table
        4.2.2 in \hyperref[S1]{[S1]} for more information.
        Values: \n
     -0x00 -- COEX_LTE_TDD_CONFIG_0 \n
     -0x01 -- COEX_LTE_TDD_CONFIG_1 \n
     -0x02 -- COEX_LTE_TDD_CONFIG_2 \n
     -0x03 -- COEX_LTE_TDD_CONFIG_3 \n
     -0x04 -- COEX_LTE_TDD_CONFIG_4 \n
     -0x05 -- COEX_LTE_TDD_CONFIG_5 \n
     -0x06 -- COEX_LTE_TDD_CONFIG_6
  */

  coex_lte_tdd_subframe_config_enum_v01 subframe_config;
  /**<    This value specifies the configuration of LTE TDD subframes.
     Refer to table 4.2.2 in \hyperref[S1]{[S1]} for more information.
     Values: \n
     -0x00 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_0 \n
     -0x01 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_1 \n
     -0x02 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_2 \n
     -0x03 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_3 \n
     -0x04 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_4 \n
     -0x05 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_5 \n
     -0x06 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_6 \n
     -0x07 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_7 \n
     -0x08 -- COEX_LTE_TDD_ SUBFRAME_CONFIG_8
  */

  coex_lte_tdd_link_config_enum_v01 ul_config;
  /**<    LTE TDD UL configuration. Values: \n
     -0x00 -- COEX_LTE_TDD_LINK_ NORMAL -- Normal cyclic prefix \n
     -0x01 -- COEX_LTE_TDD_LINK_ EXTENDED -- Extended cyclic prefix
  */

  coex_lte_tdd_link_config_enum_v01 dl_config;
  /**<    LTE TDD DL configuration. Values: \n
     -0x00 -- COEX_LTE_TDD_LINK_ NORMAL -- Normal cyclic prefix \n
     -0x01 -- COEX_LTE_TDD_LINK_ EXTENDED -- Extended cyclic prefix
  */
}coex_lte_tdd_info_type_v01;  /* Type */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_reset_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Resets the state information of the requesting control point
	      maintained by the COEX service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Sets the registration state for different
              COEX indications for the requesting control point.
              \label{idl:indicationRegister}. */
typedef struct {

  /* Optional */
  /*  Control for WWAN State Indication */
  uint8_t report_coex_wwan_state_valid;  /**< Must be set to true if report_coex_wwan_state is being passed */
  uint8_t report_coex_wwan_state;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for LTE Metric: BLER Stats Indication */
  uint8_t report_coex_metrics_lte_bler_valid;  /**< Must be set to true if report_coex_metrics_lte_bler is being passed */
  uint8_t report_coex_metrics_lte_bler;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for Fail Condition Indication */
  uint8_t report_coex_fail_condition_valid;  /**< Must be set to true if report_coex_fail_condition is being passed */
  uint8_t report_coex_fail_condition;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for Success Condition Indication */
  uint8_t report_coex_success_condition_valid;  /**< Must be set to true if report_coex_success_condition is being passed */
  uint8_t report_coex_success_condition;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for COEX Sleep */
  uint8_t report_coex_sleep_valid;  /**< Must be set to true if report_coex_sleep is being passed */
  uint8_t report_coex_sleep;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for COEX Wake-up */
  uint8_t report_coex_wakeup_valid;  /**< Must be set to true if report_coex_wakeup is being passed */
  uint8_t report_coex_wakeup;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for COEX WWAN/WCN Page Sync Indication */
  uint8_t report_coex_page_sync_valid;  /**< Must be set to true if report_coex_page_sync is being passed */
  uint8_t report_coex_page_sync;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */
}coex_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Sets the registration state for different
              COEX indications for the requesting control point.
              \label{idl:indicationRegister}. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
  */
}coex_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the the WWAN state to the client. */
typedef struct {

  /* Optional */
  /*  LTE Band Information */
  uint8_t lte_band_info_valid;  /**< Must be set to true if lte_band_info is being passed */
  coex_xsver_band_type_v01 lte_band_info;
  /**<   \n Contains a set of center frequency and bandwidth
     for each uplink (UL) and downlink (DL).
     Valid bandwidths are 2 (represents 1.4), 3, 5, 10, 15, 20 MHz.
  */

  /* Optional */
  /*  LTE TDD Information */
  uint8_t lte_tdd_info_valid;  /**< Must be set to true if lte_tdd_info is being passed */
  coex_lte_tdd_info_type_v01 lte_tdd_info;

  /* Optional */
  /*  LTE Off Period */
  uint8_t lte_off_period_valid;  /**< Must be set to true if lte_off_period is being passed */
  uint32_t lte_off_period;
  /**<   Indicates the duration (in milliseconds) for which
     LTE is going to sleep. A value of 0xFFFFFFFF indicates either LTE is off
	 indefinitely or is going out of the system.
  */

  /* Optional */
  /*  LTE Band Information Set */
  uint8_t lte_band_info_set_valid;  /**< Must be set to true if lte_band_info_set is being passed */
  uint32_t lte_band_info_set_len;  /**< Must be set to # of elements in lte_band_info_set */
  coex_band_type_and_direction_v01 lte_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all LTE center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.     
  */

  /* Optional */
  /*  TDSCDMA Band Information Set */
  uint8_t tdscdma_band_info_set_valid;  /**< Must be set to true if tdscdma_band_info_set is being passed */
  uint32_t tdscdma_band_info_set_len;  /**< Must be set to # of elements in tdscdma_band_info_set */
  coex_band_type_and_direction_v01 tdscdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all TD-SCDMA center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.         
  */

  /* Optional */
  /*  GSM Band Information Set */
  uint8_t gsm_band_info_set_valid;  /**< Must be set to true if gsm_band_info_set is being passed */
  uint32_t gsm_band_info_set_len;  /**< Must be set to # of elements in gsm_band_info_set */
  coex_band_type_and_direction_v01 gsm_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.        
  */

  /* Optional */
  /*  ONEX Band Information Set */
  uint8_t onex_band_info_set_valid;  /**< Must be set to true if onex_band_info_set is being passed */
  uint32_t onex_band_info_set_len;  /**< Must be set to # of elements in onex_band_info_set */
  coex_band_type_and_direction_v01 onex_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all ONEX (CDMA2000\textsuperscript{\textregistered} 1X)
     center frequencies and bandwidths for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.          
  */

  /* Optional */
  /*  HDR Band Information Set */
  uint8_t hdr_band_info_set_valid;  /**< Must be set to true if hdr_band_info_set is being passed */
  uint32_t hdr_band_info_set_len;  /**< Must be set to # of elements in hdr_band_info_set */
  coex_band_type_and_direction_v01 hdr_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all HDR center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.        
  */

  /* Optional */
  /*  WCDMA Band Information Set */
  uint8_t wcdma_band_info_set_valid;  /**< Must be set to true if wcdma_band_info_set is being passed */
  uint32_t wcdma_band_info_set_len;  /**< Must be set to # of elements in wcdma_band_info_set */
  coex_band_type_and_direction_v01 wcdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.     
  */
}coex_wwan_state_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_wwan_state_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Provides the client with the WWAN state, containing the
              same information as QMI_COEX_WWAN_STATE_IND. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  LTE Band Information */
  uint8_t lte_band_info_valid;  /**< Must be set to true if lte_band_info is being passed */
  coex_xsver_band_type_v01 lte_band_info;
  /**<   \n Contains a set of center frequency and bandwidth
     for each UL and DL.
     Valid bandwidths are: 2 (represents 1.4), 3, 5, 10, 15, and 20 MHz.
  */

  /* Optional */
  /*  LTE TDD Information */
  uint8_t lte_tdd_info_valid;  /**< Must be set to true if lte_tdd_info is being passed */
  coex_lte_tdd_info_type_v01 lte_tdd_info;

  /* Optional */
  /*  LTE Off Period */
  uint8_t lte_off_period_valid;  /**< Must be set to true if lte_off_period is being passed */
  uint32_t lte_off_period;
  /**<   Indicates the duration (in milliseconds) for which
     LTE is going to sleep. A value of 0xFFFFFFFF indicates either LTE is off
	 indefinitely or is going out of the system.
  */

  /* Optional */
  /*  LTE Band Information Set */
  uint8_t lte_band_info_set_valid;  /**< Must be set to true if lte_band_info_set is being passed */
  uint32_t lte_band_info_set_len;  /**< Must be set to # of elements in lte_band_info_set */
  coex_band_type_and_direction_v01 lte_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all LTE center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.       
  */

  /* Optional */
  /*  TDSCDMA Band Information Set */
  uint8_t tdscdma_band_info_set_valid;  /**< Must be set to true if tdscdma_band_info_set is being passed */
  uint32_t tdscdma_band_info_set_len;  /**< Must be set to # of elements in tdscdma_band_info_set */
  coex_band_type_and_direction_v01 tdscdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all TD-SCDMA center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.       
  */

  /* Optional */
  /*  GSM Band Information Set */
  uint8_t gsm_band_info_set_valid;  /**< Must be set to true if gsm_band_info_set is being passed */
  uint32_t gsm_band_info_set_len;  /**< Must be set to # of elements in gsm_band_info_set */
  coex_band_type_and_direction_v01 gsm_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.
  */

  /* Optional */
  /*  ONEX Band Information Set */
  uint8_t onex_band_info_set_valid;  /**< Must be set to true if onex_band_info_set is being passed */
  uint32_t onex_band_info_set_len;  /**< Must be set to # of elements in onex_band_info_set */
  coex_band_type_and_direction_v01 onex_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all ONEX center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.       
  */

  /* Optional */
  /*  HDR Band Information Set */
  uint8_t hdr_band_info_set_valid;  /**< Must be set to true if hdr_band_info_set is being passed */
  uint32_t hdr_band_info_set_len;  /**< Must be set to # of elements in hdr_band_info_set */
  coex_band_type_and_direction_v01 hdr_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all HDR center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.
  */

  /* Optional */
  /*  WCDMA Band Information Set */
  uint8_t wcdma_band_info_set_valid;  /**< Must be set to true if wcdma_band_info_set is being passed */
  uint32_t wcdma_band_info_set_len;  /**< Must be set to # of elements in wcdma_band_info_set */
  coex_band_type_and_direction_v01 wcdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA center frequencies and bandwidths
     for UL and DL. Note that a band can be
	 specified as both UL and DL by combining the mask values 
	 of UL and DL.
  */
}coex_get_wwan_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Informs the service of the WLAN state of the client. */
typedef struct {

  /* Optional */
  /*  WLAN Scan Information */
  uint8_t scan_info_valid;  /**< Must be set to true if scan_info is being passed */
  coex_wlan_scan_info_type_v01 scan_info;

  /* Optional */
  /*  WLAN Connection Information */
  uint8_t conn_info_valid;  /**< Must be set to true if conn_info is being passed */
  coex_wlan_conn_info_type_v01 conn_info;
}coex_set_wlan_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Informs the service of the WLAN state of the client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_set_wlan_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the service's understanding of the
	          WLAN scan state of the client. */
typedef struct {

  /* Mandatory */
  /*  WLAN Scan ID */
  uint32_t id;
  /**<   WLAN scan unique ID allowing multiple scans to occur simultaneously.
  */
}coex_get_wlan_scan_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the service's understanding of the
	          WLAN scan state of the client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  WLAN Scan Information */
  uint8_t scan_info_valid;  /**< Must be set to true if scan_info is being passed */
  coex_wlan_scan_info_type_v01 scan_info;
}coex_get_wlan_scan_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the service's understanding of the
	          WLAN connection state of the client. */
typedef struct {

  /* Mandatory */
  /*  WLAN Connection ID */
  uint32_t conn_handle;
  /**<   WLAN connection's unique handle.
  */
}coex_get_wlan_conn_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the service's understanding of the
	          WLAN connection state of the client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  WLAN Connection Information */
  uint8_t conn_info_valid;  /**< Must be set to true if conn_info is being passed */
  coex_wlan_conn_info_type_v01 conn_info;
  /**<   \n WLAN information for a specific connection.
  */
}coex_get_wlan_conn_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Sets the current policy for coexistence algorithms. */
typedef struct {

  /* Optional */
  /*  Policy */
  uint8_t policy_valid;  /**< Must be set to true if policy is being passed */
  coex_policy_config_mask_v01 policy;
  /**<   COEX policy to follow (based on OR-ing appropriate mask-bits). Values: \n
      - COEX_PCM_TOGGLE_FRAME_SYNC (0x0000000000000001) --  Toggle (or do not toggle) the FRAME_SYNC register/signal. 
      - COEX_PCM_TOGGLE_TX_ACTIVE (0x0000000000000002) --  Toggle (or do not toggle) the TX_ACTIVE register/signal line. 
      - COEX_PCM_TOGGLE_RX_PRIORITY (0x0000000000000004) --  Toggle (or do not toggle) the RX_PRIORITY register/signal line. 
      - COEX_PCM_REACT_TO_WCN_PRIORITY (0x0000000000000008) --  React (or do not react) to WCN_PRIORITY signal line/register. 
      - COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN (0x0000000000000010) --  Send (or do not send) out the WCI-2 standard's Type 3 (inactivity duration) message. 
      - COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE (0x0000000000000020) --  Send (or do not send) out the WCI-2 standard's Type 6 Tx advance notice message. 
      - COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT (0x0000000000000040) --  Enforce controller's Tx Power Limit. 
      - COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT (0x0000000000000080) --  React to and enforce the WCI-2 standard's Type 6 MWS Tx power limit. 

 Note: At startup the default value for the policy is all zeros,
 indicating that COEX algorithms are inactive and none of the
 policies are enabled. For all other cases, unless specified
 explicitly, the service uses the last set policy value.
 */

  /* Optional */
  /*  Power Threshold */
  uint8_t power_threshold_valid;  /**< Must be set to true if power_threshold is being passed */
  int8_t power_threshold;
  /**<   Power threshold (in dBM) to decide whether to react to WCI-2's
       WCN priority (if available).\n
       Note: At startup the default threshold value is -128 dBM. For all
            other cases, unless specified explicitly, the service uses the
            previously set threshold value. If the policy bit
            COEX_PCM_REACT_ TO_WCN_PRIORITY is set, the service uses this value.
  */

  /* Optional */
  /*  Resource Block Threshold */
  uint8_t rb_threshold_valid;  /**< Must be set to true if rb_threshold is being passed */
  uint32_t rb_threshold;
  /**<   System's instantaneous resource block (RB) count threshold to decide
       if there is a need to react to WCI-2's WCN priority (if available).

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system will react to WCN_PRIORITY irrespective of the
             number of RBs in use. For all other cases, unless specified 
             explicitly, the service uses the previously set threshold 
             value.
  */

  /* Optional */
  /*  LTE Tx Continuous Subframe Denials Threshold */
  uint8_t lte_tx_continuous_subframe_denials_threshold_valid;  /**< Must be set to true if lte_tx_continuous_subframe_denials_threshold is being passed */
  uint32_t lte_tx_continuous_subframe_denials_threshold;
  /**<   Maximum number of continuous LTE Tx sub-frame denials
       allowed in the system while reacting to WCI-2's WCN priority.

       Note: The default startup value is dynamic based upon current
             implementation. Therefore, the client can use the GET_POLICY
             message to query the current system value.
  */

  /* Optional */
  /*  LTE Tx Subframe Denials Parameters */
  uint8_t lte_tx_subrame_denial_params_valid;  /**< Must be set to true if lte_tx_subrame_denial_params is being passed */
  coex_frame_denial_duty_cycle_params_type_v01 lte_tx_subrame_denial_params;
  /**<   System's parameters for LTE Tx subframe denials allowed
       in the system while reacting to WCI-2's WCN priority (if available).

       Note: The default startup value is dynamic, based on current
             implementation. Therefore, the client can use the GET_POLICY
             message to query the current system value.
  */

  /* Optional */
  /*  APT Table */
  uint8_t apt_table_valid;  /**< Must be set to true if apt_table is being passed */
  coex_apt_table_enum_v01 apt_table;
  /**<   The AP's current selection of the APT table for the system's RF.
 Values: \n
      - COEX_APT_TABLE_DEFAULT (0) --  Default (High Efficiency) APT table for RF 
      - COEX_APT_TABLE_SECONDARY (1) --  Secondary (High Linearity) APT table for RF 

 Note: At startup the default value is COEX_APT_TABLE_DEFAULT
 and for all other cases, unless explicitly specified, the
 service uses the previously set threshold value.
 */

  /* Optional */
  /*  Controller Tx Power Limit */
  uint8_t controller_tx_power_limit_valid;  /**< Must be set to true if controller_tx_power_limit is being passed */
  float controller_tx_power_limit;
  /**<   Controllers's LTE Tx power limit (in dBM) is provided to enforce if the
       COEX_PCM_ENFORCE_ CONTROLLER_TX_POWER_LIMIT POLICY bitmask is set.

       Note: At startup the default threshold value is 127 dBM (max) and for all
             other cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  WCI-2 Tx Power Limit */
  uint8_t wci2_power_limit_valid;  /**< Must be set to true if wci2_power_limit is being passed */
  float wci2_power_limit;
  /**<   WCI-2's LTE Tx power limit (in dBM), if available, is provided to react to
       and enforce if the COEX_PCM_REACT_ TO_WCI2_TYPE6_TX_POWER_LIMIT POLICY
       bitmask is set and if a corresponding WCI-2 Type 6 message is received to
       enable enforcing.

       Note: At startup the default threshold value is 127 dBM (max) and for all
             other cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  Link Path-Loss Threshold */
  uint8_t link_path_loss_threshold_valid;  /**< Must be set to true if link_path_loss_threshold is being passed */
  float link_path_loss_threshold;
  /**<   System's link path-loss threshold to observe before deciding to enforce
       a minimum of either the controllers's or WCI-2's Tx Power limit (in dB),
       if available. If a threshold is not present then it is ignored.

       Note: At startup the default threshold value is FLT_MAX dB and for all other cases,
             unless explicitly specified, the service uses the previously set threshold
             value. FLT_MAX is the system's maximum for the float type.
  */

  /* Optional */
  /*  Resource Block Filter Alpha */
  uint8_t rb_filter_alpha_valid;  /**< Must be set to true if rb_filter_alpha is being passed */
  float rb_filter_alpha;
  /**<   Alpha coefficient for the first-order filter for the RB count
       which the system must maintain. The accepted range for this parameter is [0-1].

       Note: At startup the default value is 0 (minimum) and for all other
             cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  Filtered Resource Block Threshold */
  uint8_t filtered_rb_threshold_valid;  /**< Must be set to true if filtered_rb_threshold is being passed */
  float filtered_rb_threshold;
  /**<   System's filtered first-order RB usage count threshold
       to observe before deciding to enforce the minimum of either the
       controller's or WCI-2's Tx power limit (in dBM), if available.

       Note: At startup the default threshold value is 0 (minimum) and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  WCI-2 Tx Power Limit Timeout */
  uint8_t wci2_tx_pwrlmt_timeout_valid;  /**< Must be set to true if wci2_tx_pwrlmt_timeout is being passed */
  uint16_t wci2_tx_pwrlmt_timeout;
  /**<   Timeout value (in milliseconds) for the timer that is set
       when the WCI-2 type 6 request to enforce the WCI-2's Tx power limit
       comes in and all appropriate conditions are met. This enforcement is
       only for a limited duty cycle.

       Note: At startup the default value is 150 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  Controller Tx Power Limit Timeout */
  uint8_t controller_tx_pwrlmt_timeout_valid;  /**< Must be set to true if controller_tx_pwrlmt_timeout is being passed */
  uint16_t controller_tx_pwrlmt_timeout;
  /**<   Timeout value (in milliseconds) for the timer that is set
       when the controller's Tx power limit request comes in and
       all appropriate conditions are met.  This enforcement is only for a
       limited duty cycle.

       Note: At startup the default value is 150 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  Power Threshold for Advance Tx Notification (Type6) */
  uint8_t tx_power_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if tx_power_threshold_for_adv_tx_notice is being passed */
  int16_t tx_power_threshold_for_adv_tx_notice;
  /**<   Power threshold (in dB10) to decide whether to send out LTE advance
       Tx notification (type 6) if COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE
       policy is set

       Note: At startup the default threshold value is -1280 dB10. For all
            other cases, unless specified explicitly, the service uses the
            previously set threshold value. If the policy bit
            COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE is set, the service uses
            this value.
  */

  /* Optional */
  /*  Resource Block Threshold for Advance Tx Notification (Type6) */
  uint8_t rb_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if rb_threshold_for_adv_tx_notice is being passed */
  uint8_t rb_threshold_for_adv_tx_notice;
  /**<   System's instantaneous resource block (RB) count threshold to decide
       if there is a need to send out LTE advance Tx notification (type 6) if
       COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE policy is set

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system will always send out advance Tx notification (type6)
             irrespective of the number of RBs in use. For all other cases,
             unless specified explicitly, the service uses the previously
             set threshold value.
  */
}coex_set_policy_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Sets the current policy for coexistence algorithms. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_set_policy_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_policy_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the service's understanding of the last request of the
              client to update the policy for coexistence algorithms. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Policy */
  uint8_t policy_valid;  /**< Must be set to true if policy is being passed */
  coex_policy_config_mask_v01 policy;
  /**<   Indicates the current COEX policy to be followed
 (based on OR-ing of the appropriate bitmasks). Values: \n
      - COEX_PCM_TOGGLE_FRAME_SYNC (0x0000000000000001) --  Toggle (or do not toggle) the FRAME_SYNC register/signal. 
      - COEX_PCM_TOGGLE_TX_ACTIVE (0x0000000000000002) --  Toggle (or do not toggle) the TX_ACTIVE register/signal line. 
      - COEX_PCM_TOGGLE_RX_PRIORITY (0x0000000000000004) --  Toggle (or do not toggle) the RX_PRIORITY register/signal line. 
      - COEX_PCM_REACT_TO_WCN_PRIORITY (0x0000000000000008) --  React (or do not react) to WCN_PRIORITY signal line/register. 
      - COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN (0x0000000000000010) --  Send (or do not send) out the WCI-2 standard's Type 3 (inactivity duration) message. 
      - COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE (0x0000000000000020) --  Send (or do not send) out the WCI-2 standard's Type 6 Tx advance notice message. 
      - COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT (0x0000000000000040) --  Enforce controller's Tx Power Limit. 
      - COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT (0x0000000000000080) --  React to and enforce the WCI-2 standard's Type 6 MWS Tx power limit. 
 Note: At startup the default value for the policy is all zeros,
 indicating that COEX algorithms are inactive and none of the
 policies are enabled. For all other cases, unless explicitly
 specified, the service uses the last set policy value.
 */

  /* Optional */
  /*  Power Threshold */
  uint8_t power_threshold_valid;  /**< Must be set to true if power_threshold is being passed */
  int8_t power_threshold;
  /**<   Power threshold (in dBM) to decide whether to react to WCI-2's
       WCN priority (if available).

       Note: At startup the default threshold value is -128 dBM. For all
             other cases, unless specified explicitly, the service uses the
             previously set threshold value.

             If the policy bit COEX_PCM_REACT_ TO_WCN_PRIORITY
             is set, the service uses this value.
  */

  /* Optional */
  /*  Resource Block Threshold */
  uint8_t rb_threshold_valid;  /**< Must be set to true if rb_threshold is being passed */
  uint32_t rb_threshold;
  /**<   System's instantaneous RB count threshold to decide
       if there is a need to react to WCI-2's WCN priority (if available).

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system will react to WCN_PRIORITY irrespective of the
             number of RBs in use. For all other cases, unless specified 
			 explicitly, the service uses the previously set threshold 
			 value.
  */

  /* Optional */
  /*  LTE Tx Continuous Frame Denials Threshold */
  uint8_t lte_tx_continuous_subframe_denials_threshold_valid;  /**< Must be set to true if lte_tx_continuous_subframe_denials_threshold is being passed */
  uint32_t lte_tx_continuous_subframe_denials_threshold;
  /**<   The maximum number of continuous LTE Tx sub-frame denials
       allowed in the system while reacting to WCI-2's WCN priority.

       Note: The default startup value is dynamic based on the current
             implementation, therefore the client can use the GET_POLICY
             message to query the current system value.
  */

  /* Optional */
  /*  LTE Tx Subframe Denials Parameters */
  uint8_t lte_tx_subrame_denial_params_valid;  /**< Must be set to true if lte_tx_subrame_denial_params is being passed */
  coex_frame_denial_duty_cycle_params_type_v01 lte_tx_subrame_denial_params;
  /**<   The system's parameters for LTE Tx subframe denials allowed in the
       system while reacting to WCI-2's WCN priority (if available).

       Note: The default startup value is dynamic based on the current
             implementation, therefore the client can use the GET_POLICY
             message to query the current system value.
  */

  /* Optional */
  /*  APT Table */
  uint8_t apt_table_valid;  /**< Must be set to true if apt_table is being passed */
  coex_apt_table_enum_v01 apt_table;
  /**<   The AP's current selection of the APT table for the system's RF. Values: \n
      - COEX_APT_TABLE_DEFAULT (0) --  Default (High Efficiency) APT table for RF 
      - COEX_APT_TABLE_SECONDARY (1) --  Secondary (High Linearity) APT table for RF 

 Note: At startup the default value is COEX_APT_TABLE_DEFAULT
 and for all other cases, unless explicitly specified, the
 service uses the previously set threshold value.
 */

  /* Optional */
  /*  Controller Tx Power Limit */
  uint8_t controller_tx_power_limit_valid;  /**< Must be set to true if controller_tx_power_limit is being passed */
  float controller_tx_power_limit;
  /**<   Controllers's LTE Tx power limit (in dBM) is provided to enforce if the
       COEX_PCM_ENFORCE_ CONTROLLER_TX_POWER_ LIMIT POLICY bitmask is set.

       Note: At startup the default threshold value is 127 dBM (max) and for all
             other cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  WCI-2 Tx Power Limit */
  uint8_t wci2_power_limit_valid;  /**< Must be set to true if wci2_power_limit is being passed */
  float wci2_power_limit;
  /**<   WCI-2's LTE Tx power limit (in dBM), if available, is provided to react
       to and enforce if the COEX_PCM_REACT_ TO_WCI2_TYPE6_TX_POWER_LIMIT
       POLICY bitmask is set and if a corresponding WCI-2 Type 6 message is
       received to enable enforcing.

       Note: At startup the default threshold value is 127 dBM (max) and for all
             other cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  Link Path-Loss Threshold */
  uint8_t link_path_loss_threshold_valid;  /**< Must be set to true if link_path_loss_threshold is being passed */
  float link_path_loss_threshold;
  /**<   System's link path-loss threshold to observe before deciding to enforce
       the minimum of either the controllers's or WCI-2's Tx power limit (in dB),
       if available. If a threshold is not present then it is ignored.

        Note: At startup the default threshold value is FLT_MAX dB and for all other cases,
             unless explicitly specified, the service uses the previously set threshold
             value. FLT_MAX is the system's maximum for the float type.
  */

  /* Optional */
  /*  Resource Block Filter Alpha */
  uint8_t rb_filter_alpha_valid;  /**< Must be set to true if rb_filter_alpha is being passed */
  float rb_filter_alpha;
  /**<   Alpha coefficient for the first-order filter for the RB
       count that the system must maintain. The accepted range for this
       parameter is [0--1].

       Note: At startup the default value is 0 (minimum), for all other
             cases, unless explicitly specified, the service uses the
             previously set threshold value.
  */

  /* Optional */
  /*  Filtered Resource Block Threshold */
  uint8_t filtered_rb_threshold_valid;  /**< Must be set to true if filtered_rb_threshold is being passed */
  float filtered_rb_threshold;
  /**<   System's filtered first-order RB usage count threshold
       to observe before deciding whether to enforce the minimum of either the
       controller's or WCI-2's Tx power limit (in dBM), if available.

       Note: At startup the default threshold value is 0 (minimum) and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  WCI-2 Tx Power Limit Timeout */
  uint8_t wci2_tx_pwrlmt_timeout_valid;  /**< Must be set to true if wci2_tx_pwrlmt_timeout is being passed */
  uint16_t wci2_tx_pwrlmt_timeout;
  /**<   Timeout value (in milliseconds) for the timer that is set when the
       WCI-2 type 6 request to enforce the WCI-2's Tx power limit comes in
       and all appropriate conditions are met. This enforcement is only for
       a limited duty cycle.

       Note: At startup the default value is 150 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  Controller Tx Power Limit Timeout */
  uint8_t controller_tx_pwrlmt_timeout_valid;  /**< Must be set to true if controller_tx_pwrlmt_timeout is being passed */
  uint16_t controller_tx_pwrlmt_timeout;
  /**<   Timeout value (in milliseconds) for the timer that is set
       when the controller's Tx power limit request comes in and
       all appropriate conditions are met. This enforcement is only for a
       limited duty cycle.

       Note: At startup the default value is 150 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set threshold value.
  */

  /* Optional */
  /*  Power Threshold for Advance Tx Notification (Type6) */
  uint8_t tx_power_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if tx_power_threshold_for_adv_tx_notice is being passed */
  int16_t tx_power_threshold_for_adv_tx_notice;
  /**<   Power threshold (in dB10) to decide whether to send out LTE advance
       Tx notification (type 6) if COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE
       policy is set

       Note: At startup the default threshold value is -1280 dB10. For all
            other cases, unless specified explicitly, the service uses the
            previously set threshold value. If the policy bit
            COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE is set, the service uses
            this value.
  */

  /* Optional */
  /*  Resource Block Threshold for Advance Tx Notification (Type6) */
  uint8_t rb_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if rb_threshold_for_adv_tx_notice is being passed */
  uint8_t rb_threshold_for_adv_tx_notice;
  /**<   System's instantaneous resource block (RB) count threshold to decide
       if there is a need to send out LTE advance Tx notification (type 6) if
       COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE policy is set

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system will always send out advance Tx notification (type6)
             irrespective of the number of RBs in use. For all other cases,
             unless specified explicitly, the service uses the previously
             set threshold value.
  */
}coex_get_policy_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to start collecting/collating the LTE BLER metric. */
typedef struct {

  /* Mandatory */
  /*  Transport Block Count */
  uint32_t tb_cnt;
  /**<   Window/count of LTE transport blocks over which the
       Block Error Rate (BLER) must be calculated.
  */

  /* Mandatory */
  /*  Error Threshold Transport Block Count */
  uint32_t threshold_err_tb_cnt;
  /**<   Error threshold for the LTE transport block over which the
       service reports the BLER statistics.
  */
}coex_metrics_lte_bler_start_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to start collecting/collating the LTE BLER metric. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_metrics_lte_bler_start_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent out by the service for the LTE BLER metrics. */
typedef struct {

  /* Optional */
  /*  Transport Block Count */
  uint8_t tb_cnt_valid;  /**< Must be set to true if tb_cnt is being passed */
  uint32_t tb_cnt;
  /**<   Current count of LTE transport blocks over which the
       BLER is collected.
  */

  /* Optional */
  /*  Errored Transport Block Count */
  uint8_t errored_tb_cnt_valid;  /**< Must be set to true if errored_tb_cnt is being passed */
  uint32_t errored_tb_cnt;
  /**<   Current count of errored LTE transport blocks over the
       total count, used by the client to compute the LTE BLER metric.
  */

  /* Optional */
  /*  Transport Block Count with Only Bluetooth Active */
  uint8_t tb_cnt_bt_only_valid;  /**< Must be set to true if tb_cnt_bt_only is being passed */
  uint32_t tb_cnt_bt_only;
  /**<   Current count of LTE transport blocks over which the
       BLER is collected while Bluetooth is active
       and Wi-Fi is inactive.
  */

  /* Optional */
  /*  Errored Transport Block Count with Only Bluetooth Active */
  uint8_t errored_tb_cnt_bt_only_valid;  /**< Must be set to true if errored_tb_cnt_bt_only is being passed */
  uint32_t errored_tb_cnt_bt_only;
  /**<   Current count of errored LTE transport blocks over the
       total count, used by the client to compute the LTE BLER metric.
       This is collected while Bluetooth is active and Wi-Fi is inactive.
  */

  /* Optional */
  /*  Transport Block Count with Only Wi-Fi Active */
  uint8_t tb_cnt_wifi_only_valid;  /**< Must be set to true if tb_cnt_wifi_only is being passed */
  uint32_t tb_cnt_wifi_only;
  /**<   Current count of LTE transport blocks over which the
       BLER is collected while Wi-Fi is active
       and Bluetooth is inactive.
  */

  /* Optional */
  /*  Errored Transport Block Count with Only Wi-Fi Active */
  uint8_t errored_tb_cnt_wifi_only_valid;  /**< Must be set to true if errored_tb_cnt_wifi_only is being passed */
  uint32_t errored_tb_cnt_wifi_only;
  /**<   Current count of errored LTE transport blocks over the
       total count, used by the client to compute the LTE BLER metric.
       This is collected while Wi-Fi is active and Bluetooth is inactive.
  */

  /* Optional */
  /*  Transport Block Count with Bluetooth and Wi-Fi Active */
  uint8_t tb_cnt_bt_wifi_valid;  /**< Must be set to true if tb_cnt_bt_wifi is being passed */
  uint32_t tb_cnt_bt_wifi;
  /**<   Current count of LTE transport blocks over which the
       BLER is collected while both Wi-Fi and 
       Bluetooth are active.
  */

  /* Optional */
  /*  Errored Transport Block Count with Bluetooth and Wi-Fi Active */
  uint8_t errored_tb_cnt_bt_wifi_valid;  /**< Must be set to true if errored_tb_cnt_bt_wifi is being passed */
  uint32_t errored_tb_cnt_bt_wifi;
  /**<   Current count of errored LTE transport blocks over the
       total count, used by the client to compute the LTE BLER metric.
       This is collected while both Wi-Fi and Bluetooth are active.
  */

  /* Optional */
  /*  Transport Block Count with Only LTE Active */
  uint8_t tb_cnt_lte_only_valid;  /**< Must be set to true if tb_cnt_lte_only is being passed */
  uint32_t tb_cnt_lte_only;
  /**<   Current count of LTE transport blocks over which the
       BLER is collected while both Wi-Fi and 
       Bluetooth are inactive.
  */

  /* Optional */
  /*  Errored Transport Block Count with Only LTE Active */
  uint8_t errored_tb_cnt_lte_only_valid;  /**< Must be set to true if errored_tb_cnt_lte_only is being passed */
  uint32_t errored_tb_cnt_lte_only;
  /**<   Current count of errored LTE transport blocks over the
       total count, used by the client to compute the LTE BLER metric.
       This is collected while both Wi-Fi and Bluetooth are inactive.
  */
}coex_metrics_lte_bler_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_metrics_lte_bler_stop_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to stop collecting/collating the LTE BLER metric. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_metrics_lte_bler_stop_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to start collecting/collating the LTE Signal-to-Interface
    Plus Noise Ratio (SINR) metric. */
typedef struct {

  /* Mandatory */
  /*  Alpha */
  float alpha;
  /**<   Filter parameter for the LTE SINR metric.
       Valid range: 0 to 1 with 1/100th precision.
  */
}coex_metrics_lte_sinr_start_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to start collecting/collating the LTE Signal-to-Interface
    Plus Noise Ratio (SINR) metric. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_metrics_lte_sinr_start_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_metrics_lte_sinr_read_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to read current filter output for the LTE SINR metric. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Total SINR */
  uint8_t sinr_valid;  /**< Must be set to true if sinr is being passed */
  float sinr;
  /**<   Filter output for the total LTE SINR metric in dBM.
  */

  /* Optional */
  /*  SINR Only Bluetooth Active */
  uint8_t sinr_bt_only_valid;  /**< Must be set to true if sinr_bt_only is being passed */
  float sinr_bt_only;
  /**<   Filter output for the LTE SINR metric in dBM when Bluetooth is active
       and Wi-Fi is inactive.
  */

  /* Optional */
  /*  SINR Only Wi-Fi Active */
  uint8_t sinr_wifi_only_valid;  /**< Must be set to true if sinr_wifi_only is being passed */
  float sinr_wifi_only;
  /**<   Filter output for the LTE SINR metric in dBM when Wi-Fi is active
       and Bluetooth is inactive.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Active */
  uint8_t sinr_bt_and_wifi_valid;  /**< Must be set to true if sinr_bt_and_wifi is being passed */
  float sinr_bt_and_wifi;
  /**<   Filter output for the LTE SINR metric in dBM when both Bluetooth 
       and Wi-Fi are active.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Inactive */
  uint8_t sinr_lte_only_valid;  /**< Must be set to true if sinr_lte_only is being passed */
  float sinr_lte_only;
  /**<   Filter output for the LTE SINR metric in dBM when both Bluetooth and 
       Wi-Fi are inactive.
  */
}coex_metrics_lte_sinr_read_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_metrics_lte_sinr_stop_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to stop collecting/collating the LTE SINR metric. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_metrics_lte_sinr_stop_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to set the current list of bands to monitor for COEX. */
typedef struct {

  /* Optional */
  /*  Bands to Monitor */
  uint8_t bands_valid;  /**< Must be set to true if bands is being passed */
  uint32_t bands_len;  /**< Must be set to # of elements in bands */
  coex_filter_band_info_type_v01 bands[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  /**<   \n WWAN frequency and bandwidth sets to monitor and enforce COEX algorithms
       across and the appropriate mask to enable or disable filtering for uplink
       and/or downlink.
  */
}coex_set_band_filter_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to set the current list of bands to monitor for COEX. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_set_band_filter_info_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_band_filter_info_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the service's understanding of the client's last
	      request to update the band info for COEX algorithms. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Bands to Monitor */
  uint8_t bands_valid;  /**< Must be set to true if bands is being passed */
  uint32_t bands_len;  /**< Must be set to # of elements in bands */
  coex_filter_band_info_type_v01 bands[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  /**<   \n
       WWAN frequency and bandwidth sets to monitor and enforce COEX algorithms
       across, and appropriate mask to enable/disable filtering for uplink
       and/or downlink.
  */
}coex_get_band_filter_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent out by the service to report COEX fail conditions. */
typedef struct {

  /* Optional */
  /*  Tx Sub-frame Denials Status */
  uint8_t tx_subframe_denials_status_valid;  /**< Must be set to true if tx_subframe_denials_status is being passed */
  coex_txfrmdnl_condition_failure_mask_v01 tx_subframe_denials_status;
  /**<   Informs client when the system observes that the Tx subframe denial
 count exceeds the threshold, or that the duty cycle threshold is exceeded.

 Note: Internally, when this case is hit the system ignores any
 and all further requests to abort Tx (hence resetting the POLICY)
 until a new updated POLICY request comes from the client.\n
 Values:\n
      - COEX_TFDCFM_CONT_TX_FRAME_DENIAL_THLD_CROSSED (0x0000000000000001) --  Contiguous Tx frame denial threshold crossed 
      - COEX_TFDCFM_TX_FRAME_DENIAL_DUTY_CYCLE_CROSSED (0x0000000000000002) --  Allowed Tx frame denial duty cycle crossed 
 */

  /* Optional */
  /*  Controller Tx Power Limit Failure Condition */
  uint8_t controller_tx_pwrlmt_fail_cond_valid;  /**< Must be set to true if controller_tx_pwrlmt_fail_cond is being passed */
  coex_pwrlmt_condition_fail_mask_v01 controller_tx_pwrlmt_fail_cond;
  /**<   Provides the current reason (mask) for the failure of the
 enforcement of the controller Tx power limit. Values:\n

      - COEX_PLCFM_LINK_PATH_LOSS_THLD_CROSSED (0x0000000000000001) --  Link path loss threshold was crossed 
      - COEX_PLCFM_FILTERED_RB_THLD_CROSSED (0x0000000000000002) --  Filtered RB usage threshold was crossed 
      - COEX_PLCFM_UE_IN_RACH (0x0000000000000004) --  UE is presently in RACH 
      - COEX_PLCFM_RRC_PROCEDURE_ACTIVE (0x0000000000000008) --  RRC procedure is active 
 */

  /* Optional */
  /*  WCI-2 Tx Power Limit Failure Condition */
  uint8_t wci2_tx_pwrlmt_fail_cond_valid;  /**< Must be set to true if wci2_tx_pwrlmt_fail_cond is being passed */
  coex_pwrlmt_condition_fail_mask_v01 wci2_tx_pwrlmt_fail_cond;
  /**<   Provides the current reason (mask) for the failure of the enforcement
 of the WCI-2's request to enforce Tx power limit.\n
 Values:\n
      - COEX_PLCFM_LINK_PATH_LOSS_THLD_CROSSED (0x0000000000000001) --  Link path loss threshold was crossed 
      - COEX_PLCFM_FILTERED_RB_THLD_CROSSED (0x0000000000000002) --  Filtered RB usage threshold was crossed 
      - COEX_PLCFM_UE_IN_RACH (0x0000000000000004) --  UE is presently in RACH 
      - COEX_PLCFM_RRC_PROCEDURE_ACTIVE (0x0000000000000008) --  RRC procedure is active 
      - COEX_PLCFM_WCI2_TX_PWRLMT_TIMED_OUT (0x0000000000000010) --  WCI-2's Tx power limit enforce request timed out 
 */
}coex_condition_fail_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent out by the service to report COEX success conditions. */
typedef struct {

  /* Optional */
  /*  Tx Power Limit Success Case */
  uint8_t tx_pwrlmt_success_case_valid;  /**< Must be set to true if tx_pwrlmt_success_case is being passed */
  coex_pwrlmt_condition_success_mask_v01 tx_pwrlmt_success_case;
  /**<   Provides the current reason for the success of the enforcement
 of the Tx power limit. Values:\n
      - COEX_PLCSM_WCI2_TX_PWR_LMT_ENFORCED (0x0000000000000001) --  WCI-2 standard's Type 6 MWS Tx power limit request was granted
       and enforced 
      - COEX_PLCSM_CONTROLLER_TX_PWR_LMT_ENFORCED (0x0000000000000002) --  Controller's Tx power limit request was enforced 
 */
}coex_condition_success_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_wci2_mws_params_req_msg_v01;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the WCI-2 standard-related MWS offset and jitter parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  MWS Frame Sync Assert Offset */
  uint8_t mws_frame_sync_assert_offset_valid;  /**< Must be set to true if mws_frame_sync_assert_offset is being passed */
  coex_range_type_v01 mws_frame_sync_assert_offset;
  /**<   \n Provides the system's current range of assert
       offset (in microseconds) for the frame sync bit of the WCI-2 Type 0
       message.
  */

  /* Optional */
  /*  MWS Frame Sync Assert Jitter */
  uint8_t mws_frame_sync_assert_jitter_valid;  /**< Must be set to true if mws_frame_sync_assert_jitter is being passed */
  coex_range_type_v01 mws_frame_sync_assert_jitter;
  /**<   \n  Provides the system's current range of assert
           jitter (in microseconds) for the frame sync bit of the WCI-2
           Type 0 message.
  */

  /* Optional */
  /*  MWS Rx Assert Offset */
  uint8_t mws_rx_assert_offset_valid;  /**< Must be set to true if mws_rx_assert_offset is being passed */
  coex_range_type_v01 mws_rx_assert_offset;
  /**<   \n  Provides the system's current range of assert
       offset (in microseconds) for the Rx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Rx Assert Jitter */
  uint8_t mws_rx_assert_jitter_valid;  /**< Must be set to true if mws_rx_assert_jitter is being passed */
  coex_range_type_v01 mws_rx_assert_jitter;
  /**<   \n Provides the system's current range of assert
       jitter (in microseconds) for the Rx bit of the WCI-2 Type 0 message.
   */

  /* Optional */
  /*  MWS Rx De-assert Offset */
  uint8_t mws_rx_deassert_offset_valid;  /**< Must be set to true if mws_rx_deassert_offset is being passed */
  coex_range_type_v01 mws_rx_deassert_offset;
  /**<   \n Provides the system's current range of de-assert
       offset (in microseconds) for the Rx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Rx De-assert Jitter */
  uint8_t mws_rx_deassert_jitter_valid;  /**< Must be set to true if mws_rx_deassert_jitter is being passed */
  coex_range_type_v01 mws_rx_deassert_jitter;
  /**<   \n Provides the system's current range of de-assert
       jitter (in microseconds) for the Rx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Tx Assert Offset */
  uint8_t mws_tx_assert_offset_valid;  /**< Must be set to true if mws_tx_assert_offset is being passed */
  coex_range_type_v01 mws_tx_assert_offset;
  /**<   \n Provides the system's current range of assert
       offset (in microseconds) for the Tx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Tx Assert Jitter */
  uint8_t mws_tx_assert_jitter_valid;  /**< Must be set to true if mws_tx_assert_jitter is being passed */
  coex_range_type_v01 mws_tx_assert_jitter;
  /**<   \n Provides the system's current range of assert
       jitter (in microseconds) for the Tx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Tx De-assert Offset */
  uint8_t mws_tx_deassert_offset_valid;  /**< Must be set to true if mws_tx_deassert_offset is being passed */
  coex_range_type_v01 mws_tx_deassert_offset;
  /**<   \n Provides the system's current range of de-assert
       offset (in microseconds) for the Tx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Tx De-assert Jitter */
  uint8_t mws_tx_deassert_jitter_valid;  /**< Must be set to true if mws_tx_deassert_jitter is being passed */
  coex_range_type_v01 mws_tx_deassert_jitter;
  /**<   \n Provides the system's current range of de-assert
       jitter (in microseconds) for the Tx bit of the WCI-2 Type 0 message.
  */
}coex_get_wci2_mws_params_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Notifies the service to send sleep indications at a specified 
	          threshold. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which a sleep indication is required.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM \n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA \n 
 */

  /* Mandatory */
  /*  Duration Threshold */
  uint32_t off_period_threshold;
  /**<   The threshold (in microseconds) for the service to notify
     the client of sleep durations. The default threshold is zero
	 meaning all sleep indications are sent.
  */
}coex_set_sleep_notification_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Notifies the service to send sleep indications at a specified 
	          threshold. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_set_sleep_notification_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Retrieves the threshold value the service is using to send
	          sleep notifications. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which a sleep indication threshold is required.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM \n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA \n 
 */
}coex_get_sleep_notification_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Retrieves the threshold value the service is using to send
	          sleep notifications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Technology */
  uint8_t tech_valid;  /**< Must be set to true if tech is being passed */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which the sleep threshold is set.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM \n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA \n 
 */

  /* Optional */
  /*  Duration Threshold */
  uint8_t off_period_threshold_valid;  /**< Must be set to true if off_period_threshold is being passed */
  uint32_t off_period_threshold;
  /**<   The threshold (in microseconds) for the service to notify
     the client of sleep durations. The default threshold is zero 
	 meaning all sleep indications are sent.
  */
}coex_get_sleep_notification_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the service's sleep duration. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which the sleep indication is required.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM \n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA \n 
 */

  /* Mandatory */
  /*  Off Period  */
  uint32_t off_period;
  /**<   Indicates the duration (in microseconds) for which 
     the technology is going to sleep. A value of 0xFFFFFFFF (4,294,967,295)
	 indicates either the technology is off indefinitely or is going out of
	 the system.
    */
}coex_sleep_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the time it takes for the service to wake up. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which the wake-up period is set.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM \n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA \n 
 */

  /* Mandatory */
  /*  Wake-up Period */
  uint32_t time_to_wakeup;
  /**<   Indicates the duration (in microseconds) it takes for the service
       to wake up.
  */
}coex_wakeup_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Starts or stops the page scan synchronization between WWAN
              and WCN to save power. */
typedef struct {

  /* Mandatory */
  /*  WWAN/WCN Page Scan Synchronization Control */
  uint8_t scan_enabled;
  /**<   Values: \n
       - 0x00 -- WCN is not scanning; WWAN does not send indications \n
       - 0x01 -- WCN is scanning; WWAN may send indications\n
       Note: At startup the default value is 0x00.
  */

  /* Optional */
  /*  WCN Scan Interval */
  uint8_t scan_interval_valid;  /**< Must be set to true if scan_interval is being passed */
  uint32_t scan_interval;
  /**<   Current WCN scan interval, in milliseconds.
  */
}coex_wcn_wake_sync_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Starts or stops the page scan synchronization between WWAN
              and WCN to save power. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described in
                            the error codes section of each message definition.
  */
}coex_wcn_wake_sync_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent by the service to synchronize WWAN and WCN
              wake-up for a page interval. */
typedef struct {

  /* Mandatory */
  /*  Current WWAN Page Interval */
  uint32_t page_interval;
  /**<   WWAN page cycle, in milliseconds.
  */

  /* Optional */
  /*  Modem Timestamp */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint64_t timestamp;
  /**<   Modem message timestamp in Qtimer ticks (current counter value).
       Qtimer is a 56-bit deep global counter that gives a resolution with the
       19.2 MHz clock of 0 to 118.927924 years. It is present across all
       subsystems of the system on chip.
  */
}coex_wcn_wake_sync_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup coex_qmi_msg_ids
    @{
  */
#define QMI_COEX_RESET_REQ_V01 0x0000
#define QMI_COEX_RESET_RESP_V01 0x0000
#define QMI_COEX_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_COEX_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_COEX_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_COEX_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_COEX_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_COEX_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_COEX_WWAN_STATE_IND_V01 0x0021
#define QMI_COEX_GET_WWAN_STATE_REQ_V01 0x0022
#define QMI_COEX_GET_WWAN_STATE_RESP_V01 0x0022
#define QMI_COEX_SET_WLAN_STATE_REQ_V01 0x0023
#define QMI_COEX_SET_WLAN_STATE_RESP_V01 0x0023
#define QMI_COEX_GET_WLAN_SCAN_STATE_REQ_V01 0x0024
#define QMI_COEX_GET_WLAN_SCAN_STATE_RESP_V01 0x0024
#define QMI_COEX_GET_WLAN_CONN_STATE_REQ_V01 0x0025
#define QMI_COEX_GET_WLAN_CONN_STATE_RESP_V01 0x0025
#define QMI_COEX_SET_POLICY_REQ_V01 0x0026
#define QMI_COEX_SET_POLICY_RESP_V01 0x0026
#define QMI_COEX_GET_POLICY_REQ_V01 0x0027
#define QMI_COEX_GET_POLICY_RESP_V01 0x0027
#define QMI_COEX_METRICS_LTE_BLER_START_REQ_V01 0x0028
#define QMI_COEX_METRICS_LTE_BLER_START_RESP_V01 0x0028
#define QMI_COEX_METRICS_LTE_BLER_IND_V01 0x0029
#define QMI_COEX_METRICS_LTE_BLER_STOP_REQ_V01 0x002A
#define QMI_COEX_METRICS_LTE_BLER_STOP_RESP_V01 0x002A
#define QMI_COEX_METRICS_LTE_SINR_START_REQ_V01 0x002B
#define QMI_COEX_METRICS_LTE_SINR_START_RESP_V01 0x002B
#define QMI_COEX_METRICS_LTE_SINR_READ_REQ_V01 0x002C
#define QMI_COEX_METRICS_LTE_SINR_READ_RESP_V01 0x002C
#define QMI_COEX_METRICS_LTE_SINR_STOP_REQ_V01 0x002D
#define QMI_COEX_METRICS_LTE_SINR_STOP_RESP_V01 0x002D
#define QMI_COEX_SET_BAND_FILTER_INFO_REQ_V01 0x002E
#define QMI_COEX_SET_BAND_FILTER_INFO_RESP_V01 0x002E
#define QMI_COEX_GET_BAND_FILTER_INFO_REQ_V01 0x002F
#define QMI_COEX_GET_BAND_FILTER_INFO_RESP_V01 0x002F
#define QMI_COEX_CONDITION_FAIL_IND_V01 0x0030
#define QMI_COEX_CONDITION_SUCCESS_IND_V01 0x0031
#define QMI_COEX_GET_WCI2_MWS_PARAMS_REQ_V01 0x0032
#define QMI_COEX_GET_WCI2_MWS_PARAMS_RESP_V01 0x0032
#define QMI_COEX_GET_SLEEP_NOTIFICATION_REQ_V01 0x0033
#define QMI_COEX_GET_SLEEP_NOTIFICATION_RESP_V01 0x0033
#define QMI_COEX_SET_SLEEP_NOTIFICATION_REQ_V01 0x0034
#define QMI_COEX_SET_SLEEP_NOTIFICATION_RESP_V01 0x0034
#define QMI_COEX_SLEEP_IND_V01 0x0035
#define QMI_COEX_WAKEUP_IND_V01 0x0036
#define QMI_COEX_WCN_WAKE_SYNC_REQ_V01 0x0037
#define QMI_COEX_WCN_WAKE_SYNC_RESP_V01 0x0037
#define QMI_COEX_WCN_WAKE_SYNC_IND_V01 0x0038
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro coex_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type coex_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define coex_get_service_object_v01( ) \
          coex_get_service_object_internal_v01( \
            COEX_V01_IDL_MAJOR_VERS, COEX_V01_IDL_MINOR_VERS, \
            COEX_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

