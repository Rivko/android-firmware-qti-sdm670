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
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/coex/api/coexistence_service_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Sep  8 2016 (Spin 0)
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
#define COEX_V01_IDL_MINOR_VERS 0x14
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define COEX_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define COEX_V01_MAX_MESSAGE_ID 0x0044
/**
    @}
  */


/** @addtogroup coex_qmi_consts
    @{
  */
#define COEX_WLAN_MAX_WIFI_CHNLS_PER_CONN_V01 8
#define COEX_WWAN_MAX_BANDS_TO_MONITOR_V01 32
#define COEX_WWAN_MAX_BANDS_PER_TECH_V01 16
#define COEX_MAX_VICTIM_TBL_ENTRIES_V01 32
#define COEX_MAX_BT_AFH_CHANNEL_RANGE_V01 4
#define COEX_MAX_RB_THRES_PER_CONFLICT_V01 6
#define COEX_MAX_TX_ANTENNA_V01 8
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
  /**<   Uplink frequency and bandwidth, set to 0 in receive only
     configurations. */

  coex_band_type_v01 dl_band;
  /**<   Downlink frequency and bandwidth. */
}coex_xsver_band_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t coex_indicate_band_direction_v01;
#define COEX_UL_BAND_DIRECTION_V01 ((coex_indicate_band_direction_v01)0x0000000000000001ull) /**<  Band information is for uplink  */
#define COEX_DL_BAND_DIRECTION_V01 ((coex_indicate_band_direction_v01)0x0000000000000002ull) /**<  Band information is for downlink  */
typedef uint64_t coex_wwan_operating_dimension_v01;
#define COEX_TECH_OPERATING_DIMENSION_FDD_V01 ((coex_wwan_operating_dimension_v01)0x0000000000000001ull) /**<  Technology is operating in the Frequency Division Duplex dimension  */
#define COEX_TECH_OPERATING_DIMENSION_TDD_V01 ((coex_wwan_operating_dimension_v01)0x0000000000000002ull) /**<  Technology is operating in the Time Division Duplex dimension  */
/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_CARRIER_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_CARRIER_PRIMARY_V01 = 0, /**<  Primary component carrier ID  */
  COEX_CARRIER_SECONDARY_0_V01 = 1, /**<  Component carrier ID for first secondary carrier  */
  COEX_CARRIER_SECONDARY_1_V01 = 2, /**<  Component carrier ID for second secondary carrier  */
  COEX_CARRIER_SECONDARY_2_V01 = 3, /**<  Component carrier ID for third secondary carrier  */
  COEX_CARRIER_SECONDARY_3_V01 = 4, /**<  Component carrier ID for fourth secondary carrier  */
  COEX_CARRIER_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_carrier_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t freq;
  /**<   Band center frequency in kHz. */

  uint32_t bandwidth;
  /**<   Bandwidth in Hz. */

  coex_indicate_band_direction_v01 direction;
  /**<   Indicates whether the band is for uplink, downlink, or both. */
}coex_band_type_and_direction_v01;  /* Type */
/**
    @}
  */

typedef uint64_t coex_policy_config_mask_v01;
#define COEX_PCM_TOGGLE_FRAME_SYNC_V01 ((coex_policy_config_mask_v01)0x0000000000000001ull) /**<  Toggle (or do not toggle) the FRAME_SYNC register/signal  */
#define COEX_PCM_TOGGLE_TX_ACTIVE_V01 ((coex_policy_config_mask_v01)0x0000000000000002ull) /**<  Toggle (or do not toggle) the TX_ACTIVE register/signal line  */
#define COEX_PCM_TOGGLE_RX_PRIORITY_V01 ((coex_policy_config_mask_v01)0x0000000000000004ull) /**<  Toggle (or do not toggle) the RX_PRIORITY register/signal line  */
#define COEX_PCM_REACT_TO_WCN_PRIORITY_V01 ((coex_policy_config_mask_v01)0x0000000000000008ull) /**<  React (or do not react) to the WCN_PRIORITY signal line/register  */
#define COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01 ((coex_policy_config_mask_v01)0x0000000000000010ull) /**<  Send (or do not send) the WCI-2 standard's Type 3 inactivity duration message  */
#define COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE_V01 ((coex_policy_config_mask_v01)0x0000000000000020ull) /**<  Send (or do not send) out the WCI-2 standard's Type~6 Tx advance notice message  */
#define COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01 ((coex_policy_config_mask_v01)0x0000000000000040ull) /**<  Enforce the controller's Tx power limit  */
#define COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 ((coex_policy_config_mask_v01)0x0000000000000080ull) /**<  React (or do not react) to and enforce the WCI-2 standard's Type 6 MWS Tx power limit  */
#define COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 ((coex_policy_config_mask_v01)0x0000000000000100ull) /**<  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem connected state (bit~3)  */
#define COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 ((coex_policy_config_mask_v01)0x0000000000000200ull) /**<  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem quantized Tx power state (bit~4)  */
#define COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ_V01 ((coex_policy_config_mask_v01)0x0000000000000400ull) /**<  Send (or do not send) the WCI-2 standard's Type 4 technology SCAN frequency information  */
#define COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 ((coex_policy_config_mask_v01)0x0000000000000800ull) /**<  Send (or do not send) the WCI-2 standard's Type 7 modem cellular Tx antenna selection information  */
#define COEX_PCM_RESERVED_FOR_CONTROLLER_V01 ((coex_policy_config_mask_v01)0x0000000000001000ull) /**<  Policy bitmask reserved for use by the controller  */
typedef uint64_t coex_wcn_policy_config_mask_v01;
#define COEX_PCM_ENFORCE_WWAN_RX_IMD_PROTECTION_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000001ull) /**<  Protect the WWAN receive. Apply the static power backoff or block 
       the WLAN transmit based on WLAN RSSI  */
#define COEX_PCM_ENFORCE_DYNAMIC_WLAN_POWER_BACKOFF_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000002ull) /**<  Dynamically adjust the WLAN transmit power based on the modem Rx 
       de-sensing because of the WLAN transmit  */
#define COEX_PCM_ENFORCE_BT_AFH_UPDATE_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000004ull) /**<  Update the Bluetooth AFH channels range  */
#define COEX_PCM_ENFORCE_WLAN_CHANNEL_AVOIDANCE_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000008ull) /**<  WLAN is to carry out channel avoidance  */
#define COEX_PCM_ENFORCE_TDM_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000010ull) /**<  Run TDM, which is a mechanism to avoid WLAN transmit and receive 
       when the WWAN transmit is active  */
#define COEX_PCM_ENFORCE_MAX_WWAN_PROTECTION_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000020ull) /**<  Assume WWAN is extra sensitive to de-sensing due to the WLAN 
       transmit, such as when using a suboptimal RF filter, and apply aggressive
       power backoff or TDM  */
#define COEX_PCM_ENFORCE_TDM_STICKY_V01 ((coex_wcn_policy_config_mask_v01)0x0000000000000040ull) /**<  Run sticky TDM, which is similar to COEX_PCM_ENFORCE_TDM except after TDM
       is triggered, it stays triggered when the WLAN RX rate is recovered to high.  */
typedef uint64_t coex_wcn_behavior_mask_v01;
#define COEX_WCN_BHVR_NONE_V01 ((coex_wcn_behavior_mask_v01)0x0000000000000001ull) /**<  WCN does not exercise any coexistence methods  */
#define COEX_WCN_BHVR_QMI_LITE_V01 ((coex_wcn_behavior_mask_v01)0x0000000000000002ull) /**<  WCN uses QMI Lite communication  */
#define COEX_WCN_BHVR_SMEM_DATA_V01 ((coex_wcn_behavior_mask_v01)0x0000000000000004ull) /**<  WCN uses SMEM for data plane communication  */
typedef uint64_t coex_antenna_mask_v01;
#define COEX_ANTENNA_BAND_2GHZ_CHAIN0_V01 ((coex_antenna_mask_v01)0x0000000000000001ull) /**<  Chain 0 antenna for 2GHz   */
#define COEX_ANTENNA_BAND_2GHZ_CHAIN1_V01 ((coex_antenna_mask_v01)0x0000000000000002ull) /**<  Chain 1 antenna for 2GHz   */
#define COEX_ANTENNA_BAND_5GHZ_CHAIN0_V01 ((coex_antenna_mask_v01)0x0000000000000004ull) /**<  Chain 0 antenna for 5GHz   */
#define COEX_ANTENNA_BAND_5GHZ_CHAIN1_V01 ((coex_antenna_mask_v01)0x0000000000000008ull) /**<  Chain 1 antenna for 5GHz   */
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
  /**<   Uplink and downlink frequency bandwidth details for this band. */

  coex_enable_band_filter_mask_v01 band_mask;
  /**<   Enable or disable filtering mask for uplink, downlink, or both. */
}coex_filter_band_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  float min;
  /**<   Minimum value for this range. */

  float max;
  /**<   Maximum value for this range. */
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
  /**<   Window of frames over which the frame denial applies. */
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
  COEX_GSM_TECH_V01 = 2, /**<  GSM (Instance 1)\n  */
  COEX_ONEX_TECH_V01 = 3, /**<  CDMA2000\textsuperscript{\textregistered} 1X \n  */
  COEX_HDR_TECH_V01 = 4, /**<  HDR \n  */
  COEX_WCDMA_TECH_V01 = 5, /**<  WCDMA (Instance 1)\n  */
  COEX_GSM2_TECH_V01 = 6, /**<  GSM (Instance 2)\n  */
  COEX_GSM3_TECH_V01 = 7, /**<  GSM (Instance 3)\n  */
  COEX_WCDMA2_TECH_V01 = 8, /**<  WCDMA (Instance 2)  */
  COEX_WWAN_TECH_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wwan_tech_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_indicate_band_direction_v01 direction;
  /**<   Indicates whether the band is for uplink, downlink, or both. Values:\n
      - COEX_UL_BAND_DIRECTION (0x0000000000000001) --  Band information is for uplink 
      - COEX_DL_BAND_DIRECTION (0x0000000000000002) --  Band information is for downlink 
 */

  coex_wwan_operating_dimension_v01 operating_dim;
  /**<   Specifies the operating dimension to which this range refers: FDD, 
 TDD, or both. Values:\n
      - COEX_TECH_OPERATING_DIMENSION_FDD (0x0000000000000001) --  Technology is operating in the Frequency Division Duplex dimension 
      - COEX_TECH_OPERATING_DIMENSION_TDD (0x0000000000000002) --  Technology is operating in the Time Division Duplex dimension 
 */

  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology to which this range refers. Values:\n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2)  
 */

  uint32_t freq_start;
  /**<   Range lower bound frequency, in kHz. for example, 1 = 1kHz. */

  uint32_t freq_stop;
  /**<   Range upper bound frequency, in kHz. for example, 1 = 1kHz.   */
}coex_wwan_band_range_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  int16_t tx_pwr_thld;
  /**<   Technology's Tx power threshold, in dB10, that is, (power, in dB, multiplied by 10). */

  uint16_t tx_bw_thld;
  /**<   Technology's Tx bandwidth usage. This represents the amount of resources 
       used by a technology. For example, in LTE, this is the number of RBs.   */
}coex_wwan_tx_thlds_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_frame_denial_duty_cycle_params_type_v01 duty_cycle_params;
  /**<   Maximum duty cycle. */

  uint32_t max_cont_denials;
  /**<   Maximum continuous denials.   */
}coex_wwan_tx_frame_denials_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wwan_tx_thlds_type_v01 tx_thlds;
  /**<   Tx thresholds over which to limit or blank. */

  coex_wwan_tx_frame_denials_params_type_v01 frame_denials;
  /**<   Tx frame denial parameters when reacting to WCN priority.   */
}coex_wcn_prio_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t start;
  /**<   Lower bound RB range value. */

  uint8_t end;
  /**<   Upper bound RB range value.   */
}coex_rb_threshold_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t rb_thresh_len;  /**< Must be set to # of elements in rb_thresh */
  coex_rb_threshold_type_v01 rb_thresh[COEX_MAX_RB_THRES_PER_CONFLICT_V01];
  /**<   RB thresholds that the current Tx advance must be within to get
       transmitted outside the MDM. Upper and lower bound values are inclusive. */

  int16_t tx_power_thresh;
  /**<   Power threshold that advanced Tx must be above to get passed
       outside the MDM. Value is in dB10: the power, in dB, multiplied by 10.   */
}coex_tx_advanced_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  float power;
  /**<   Power limit (in dBM) for Tx.   */
}coex_pwr_limit_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wcn_prio_params_type_v01 wcn_prio;
  /**<   COEX_PCM_REACT_TO_WCN_PRIORITY parameters.   */

  coex_tx_advanced_params_type_v01 tx_adv;
  /**<   COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE parameters.  */

  coex_pwr_limit_params_type_v01 pwr_lmt;
  /**<   COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT parameters. */
}coex_conflict_mdm_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t antenna_id;
  /**<   Unique antenna identifier. */

  uint8_t old_state;
  /**<   Previous switch state of the antenna. */

  uint8_t new_state;
  /**<   Current switch state of the antenna.   */
}coex_tx_antenna_state_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t coex_wlan_mode_mask_v01;
#define COEX_WLAN_CONN_MODE_V01 ((coex_wlan_mode_mask_v01)0x0000000000000001ull) /**<  Connected mode  */
#define COEX_WLAN_SCAN_MODE_V01 ((coex_wlan_mode_mask_v01)0x0000000000000002ull) /**<  Scan mode  */
#define COEX_WLAN_HIGH_PRIO_MODE_V01 ((coex_wlan_mode_mask_v01)0x0000000000000004ull) /**<  High Priority mode  */
/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_SCAN_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_SCAN_STOP_V01 = 0, /**<  WLAN stops scanning  */
  COEX_WLAN_SCAN_START_V01 = 1, /**<  WLAN starts scanning (includes active and positive scan)  */
  COEX_WLAN_SCAN_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_scan_state_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_HIGH_PRIO_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_HIGH_PRIO_STOP_V01 = 0, /**<  WLAN stops a high priority event  */
  COEX_WLAN_HIGH_PRIO_START_V01 = 1, /**<  WLAN starts a high priority event  */
  COEX_WLAN_HIGH_PRIO_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_high_prio_state_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WLAN_CONN_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WLAN_CONN_DISABLED_V01 = 0, /**<  WLAN connection has been disabled
        (either the connection was unsuccessful or torn down)  */
  COEX_WLAN_CONN_SETUP_V01 = 1, /**<  WLAN is setting up the connection
        (includes association or authentication or DHCP)  */
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
  COEX_WLAN_CONN_MODE_NONE_V01 = 0, /**<  WLAN is not in Connected mode  */
  COEX_WLAN_CONN_MODE_STATION_V01 = 1, /**<  WLAN is in Station Connected mode  */
  COEX_WLAN_CONN_MODE_SOFTAP_V01 = 2, /**<  WLAN is in Soft Access Point Connected mode  */
  COEX_WLAN_CONN_MODE_P2P_GROUP_OWNER_V01 = 3, /**<  WLAN is in Peer-to-peer Group Owner Connected mode  */
  COEX_WLAN_CONN_MODE_P2P_CLIENT_V01 = 4, /**<  WLAN is in Peer-to-peer Client Connected mode  */
  COEX_WLAN_CONN_MODE_AMP_V01 = 5, /**<  WLAN is in Amplify Connected mode  */
  COEX_WLAN_CONN_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wlan_conn_mode_enum_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_WCN_TECH_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_WIFI_TECH_V01 = 0, /**<  Wi-Fi  */
  COEX_BT_TECH_V01 = 1, /**<  Bluetooth    */
  COEX_WCN_TECH_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_wcn_tech_v01;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wlan_mode_mask_v01 mode;
  /**<   Mode(s) this band is in. Values:\n
      - COEX_WLAN_CONN_MODE (0x0000000000000001) --  Connected mode 
      - COEX_WLAN_SCAN_MODE (0x0000000000000002) --  Scan mode 
      - COEX_WLAN_HIGH_PRIO_MODE (0x0000000000000004) --  High Priority mode 
 */

  coex_wcn_tech_v01 tech;
  /**<   Specifies the technology to which this range refers. Values:\n
      - COEX_WIFI_TECH (0) --  Wi-Fi 
      - COEX_BT_TECH (1) --  Bluetooth   
 */

  uint32_t freq_start;
  /**<   Range lower bound frequency, in kHz; for example, 1 = 1kHz. */

  uint32_t freq_stop;
  /**<   Range upper bound frequency, in kHz; for example, 1 = 1kHz.   */
}coex_wcn_band_range_type_v01;  /* Type */
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

  uint32_t id;
  /**<   WLAN high priority event's unique ID; allows multiple
       events to occur simultaneously. \n
	   Note: ID is not valid after the event stops.
  */

  coex_band_type_v01 band;
  /**<   WLAN frequency and bandwidth.
       Valid center frequencies are 2400 to 2500 MHz. 
       Valid bandwidths are 20, 40, or 80 MHz.
  */

  coex_wlan_high_prio_state_enum_v01 state;
  /**<   WLAN high priority event's current state. Values: \n
     - 0x00 -- COEX_WLAN_HIGH_PRIO_STOP -- WLAN stopped event \n
	 - 0x01 -- COEX_WLAN_HIGH_PRIO_START -- WLAN started event
  */
}coex_wlan_high_prio_info_type_v01;  /* Type */
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
         of a single connection might include several disjoint
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

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t block_tx;
  /**<   Enable blocking the WLAN transmit when RSSI is too low.     */

  uint8_t static_backoff_tx_pwr;
  /**<   Static WLAN transmit power, in dBm.   */
}coex_wwan_rx_imd_protection_params_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t start_bt_channel;
  /**<   Bluetooth start channel number; values: 1-79. This value must be less 
       than or equal to end_bt_channel. */

  uint8_t end_bt_channel;
  /**<   Bluetooth end channel number; values: 1-79. This value must be greater
       than or equal to start_bt_channel.   */
}coex_bt_afh_update_range_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wwan_rx_imd_protection_params_type_v01 imd_protection_params;
  /**<   WWAN receive IMD protection policy parameters. */

  uint32_t bt_afh_update_len;  /**< Must be set to # of elements in bt_afh_update */
  coex_bt_afh_update_range_type_v01 bt_afh_update[COEX_MAX_BT_AFH_CHANNEL_RANGE_V01];
  /**<   Bluetooth AFH policy parameters. These are ranges that Bluetooth should avoid
       when hopping; upper and lower bounds are inclusive.    */
}coex_conflict_wcn_params_type_v01;  /* Type */
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
  COEX_LTE_TDD_LINK_NORMAL_V01 = 0, /**<  Normal cyclic prefix  */
  COEX_LTE_TDD_LINK_EXTENDED_V01 = 1, /**<   Extended cyclic prefix  */
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
        are used for uplink, downlink, and special. Refer to Table
        4.2.2 in the \textit{Bluetooth Core Specification Addendum 3 rev. 2}
        for more information.
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
     Refer to Table 4.2.2 in the \textit{Bluetooth Core Specification 
     Addendum 3 rev. 2} for more information.
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
  /**<    LTE TDD uplink configuration. Values: \n
     -0x00 -- COEX_LTE_TDD_LINK_ NORMAL -- Normal cyclic prefix \n
     -0x01 -- COEX_LTE_TDD_LINK_ EXTENDED -- Extended cyclic prefix
  */

  coex_lte_tdd_link_config_enum_v01 dl_config;
  /**<    LTE TDD downlink configuration. Values: \n
     -0x00 -- COEX_LTE_TDD_LINK_ NORMAL -- Normal cyclic prefix \n
     -0x01 -- COEX_LTE_TDD_LINK_ EXTENDED -- Extended cyclic prefix
  */
}coex_lte_tdd_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_carrier_v01 id;
  /**<   Identifies which carrier ID the information refers to. Secondary carriers
       can become active or inactive independent of other carriers, so it is
       possible to have COEX_CARRIER_PRIMARY and COEX_CARRIER_SECONDARY_2
       but not COEX_CARRIER_SECONDARY_1. */

  coex_wwan_operating_dimension_v01 operating_dim;
  /**<   Specifies the operating dimension in which this carrier is operating: 
 FDD or TDD. Values:\n
      - COEX_TECH_OPERATING_DIMENSION_FDD (0x0000000000000001) --  Technology is operating in the Frequency Division Duplex dimension 
      - COEX_TECH_OPERATING_DIMENSION_TDD (0x0000000000000002) --  Technology is operating in the Time Division Duplex dimension 
 */

  coex_lte_tdd_info_type_v01 tdd_info;
  /**<   Identifies component carrier TDD info if this carrier is operating
       in TDD mode. It is possible for different carriers to have different
       operating modes. This information is not valid if operating_dim is 
       FDD. */

  int8_t frame_boundary_offset;
  /**<    LTE frame offset of this carrier from primary carrier, in microseconds. 
        Maximum range is \textpm 32 \textmu s. */
}coex_lte_carrier_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wwan_band_range_type_v01 wwan;
  /**<   WWAN band information for this conflict.     */

  coex_wcn_band_range_type_v01 wcn;
  /**<   WCN band information for this conflict. */

  coex_policy_config_mask_v01 mdm_policy;
  /**<   Action to be taken by MDM when this conflict is found. Values:\n
      - COEX_PCM_TOGGLE_FRAME_SYNC (0x0000000000000001) --  Toggle (or do not toggle) the FRAME_SYNC register/signal 
      - COEX_PCM_TOGGLE_TX_ACTIVE (0x0000000000000002) --  Toggle (or do not toggle) the TX_ACTIVE register/signal line 
      - COEX_PCM_TOGGLE_RX_PRIORITY (0x0000000000000004) --  Toggle (or do not toggle) the RX_PRIORITY register/signal line 
      - COEX_PCM_REACT_TO_WCN_PRIORITY (0x0000000000000008) --  React (or do not react) to the WCN_PRIORITY signal line/register 
      - COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN (0x0000000000000010) --  Send (or do not send) the WCI-2 standard's Type 3 inactivity duration message 
      - COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE (0x0000000000000020) --  Send (or do not send) out the WCI-2 standard's Type~6 Tx advance notice message 
      - COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT (0x0000000000000040) --  Enforce the controller's Tx power limit 
      - COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT (0x0000000000000080) --  React (or do not react) to and enforce the WCI-2 standard's Type 6 MWS Tx power limit 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE (0x0000000000000100) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem connected state (bit~3) 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE (0x0000000000000200) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem quantized Tx power state (bit~4) 
      - COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ (0x0000000000000400) --  Send (or do not send) the WCI-2 standard's Type 4 technology SCAN frequency information 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL (0x0000000000000800) --  Send (or do not send) the WCI-2 standard's Type 7 modem cellular Tx antenna selection information 
      - COEX_PCM_RESERVED_FOR_CONTROLLER (0x0000000000001000) --  Policy bitmask reserved for use by the controller 
 */

  coex_wcn_policy_config_mask_v01 wcn_policy;
  /**<   Action to be taken by WCN when this conflict is found. Values:\n
      - COEX_PCM_ENFORCE_WWAN_RX_IMD_PROTECTION (0x0000000000000001) --  Protect the WWAN receive. Apply the static power backoff or block 
       the WLAN transmit based on WLAN RSSI 
      - COEX_PCM_ENFORCE_DYNAMIC_WLAN_POWER_BACKOFF (0x0000000000000002) --  Dynamically adjust the WLAN transmit power based on the modem Rx 
       de-sensing because of the WLAN transmit 
      - COEX_PCM_ENFORCE_BT_AFH_UPDATE (0x0000000000000004) --  Update the Bluetooth AFH channels range 
      - COEX_PCM_ENFORCE_WLAN_CHANNEL_AVOIDANCE (0x0000000000000008) --  WLAN is to carry out channel avoidance 
      - COEX_PCM_ENFORCE_TDM (0x0000000000000010) --  Run TDM, which is a mechanism to avoid WLAN transmit and receive 
       when the WWAN transmit is active 
      - COEX_PCM_ENFORCE_MAX_WWAN_PROTECTION (0x0000000000000020) --  Assume WWAN is extra sensitive to de-sensing due to the WLAN 
       transmit, such as when using a suboptimal RF filter, and apply aggressive
       power backoff or TDM 
      - COEX_PCM_ENFORCE_TDM_STICKY (0x0000000000000040) --  Run sticky TDM, which is similar to COEX_PCM_ENFORCE_TDM except after TDM
       is triggered, it stays triggered when the WLAN RX rate is recovered to high. 
 */

  coex_conflict_wcn_params_type_v01 wcn_params;
  /**<   Parameters used to carry out actions on WCN policies. */

  coex_conflict_mdm_params_type_v01 mdm_params;
  /**<   Parameters used to carry out actions on MDM policies. */
}coex_conflict_definition_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Resets the state information of the requesting control point
          maintained by the COEX service. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

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

  /* Optional */
  /*  Control for LTE 5GHz State Indication */
  uint8_t lte_5ghz_state_valid;  /**< Must be set to true if lte_5ghz_state is being passed */
  uint8_t lte_5ghz_state;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Control for Tx Antenna State Update */
  uint8_t tx_antenna_state_valid;  /**< Must be set to true if tx_antenna_state is being passed */
  uint8_t tx_antenna_state;
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
/** Indication Message; Indicates the WWAN state to the client. */
typedef struct {

  /* Optional */
  /*  LTE Band Information */
  uint8_t lte_band_info_valid;  /**< Must be set to true if lte_band_info is being passed */
  coex_xsver_band_type_v01 lte_band_info;
  /**<   \n Contains a set of center frequency and bandwidth
     for each uplink and downlink.
     Valid bandwidths are 2 (represents 1.4), 3, 5, 10, 15, 20 MHz.
  */

  /* Optional */
  /*  LTE TDD Information */
  uint8_t lte_tdd_info_valid;  /**< Must be set to true if lte_tdd_info is being passed */
  coex_lte_tdd_info_type_v01 lte_tdd_info;
  /**<   \n  Note: If LTE is operating with carrier aggregation, this TLV
           reflects the configuration of the primary carrier.
  */

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
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  TD-SCDMA Band Information Set */
  uint8_t tdscdma_band_info_set_valid;  /**< Must be set to true if tdscdma_band_info_set is being passed */
  uint32_t tdscdma_band_info_set_len;  /**< Must be set to # of elements in tdscdma_band_info_set */
  coex_band_type_and_direction_v01 tdscdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all TD-SCDMA center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.         
  */

  /* Optional */
  /*  GSM [Instance 1] Band Information Set */
  uint8_t gsm_band_info_set_valid;  /**< Must be set to true if gsm_band_info_set is being passed */
  uint32_t gsm_band_info_set_len;  /**< Must be set to # of elements in gsm_band_info_set */
  coex_band_type_and_direction_v01 gsm_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 1) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  ONEX Band Information Set */
  uint8_t onex_band_info_set_valid;  /**< Must be set to true if onex_band_info_set is being passed */
  uint32_t onex_band_info_set_len;  /**< Must be set to # of elements in onex_band_info_set */
  coex_band_type_and_direction_v01 onex_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all ONEX (CDMA2000\textsuperscript{\textregistered} 1X)
     center frequencies and bandwidths for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.          
  */

  /* Optional */
  /*  HDR Band Information Set */
  uint8_t hdr_band_info_set_valid;  /**< Must be set to true if hdr_band_info_set is being passed */
  uint32_t hdr_band_info_set_len;  /**< Must be set to # of elements in hdr_band_info_set */
  coex_band_type_and_direction_v01 hdr_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all HDR center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  WCDMA [Instance 1] Band Information Set */
  uint8_t wcdma_band_info_set_valid;  /**< Must be set to true if wcdma_band_info_set is being passed */
  uint32_t wcdma_band_info_set_len;  /**< Must be set to # of elements in wcdma_band_info_set */
  coex_band_type_and_direction_v01 wcdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA (Instance 1) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.     
  */

  /* Optional */
  /*  GSM [Instance 2] Band Information Set */
  uint8_t gsm2_band_info_set_valid;  /**< Must be set to true if gsm2_band_info_set is being passed */
  uint32_t gsm2_band_info_set_len;  /**< Must be set to # of elements in gsm2_band_info_set */
  coex_band_type_and_direction_v01 gsm2_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 2) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  GSM [Instance 3] Band Information Set */
  uint8_t gsm3_band_info_set_valid;  /**< Must be set to true if gsm3_band_info_set is being passed */
  uint32_t gsm3_band_info_set_len;  /**< Must be set to # of elements in gsm3_band_info_set */
  coex_band_type_and_direction_v01 gsm3_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 3) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  WCDMA [Instance 2] Band Information Set */
  uint8_t wcdma2_band_info_set_valid;  /**< Must be set to true if wcdma2_band_info_set is being passed */
  uint32_t wcdma2_band_info_set_len;  /**< Must be set to # of elements in wcdma2_band_info_set */
  coex_band_type_and_direction_v01 wcdma2_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA (Instance 2) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  LTE Component Carrier Information */
  uint8_t lte_carrier_info_set_valid;  /**< Must be set to true if lte_carrier_info_set is being passed */
  uint32_t lte_carrier_info_set_len;  /**< Must be set to # of elements in lte_carrier_info_set */
  coex_lte_carrier_info_type_v01 lte_carrier_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains information specific to each active LTE component carrier.
     To match this TLV to the LTE Band Information Set TLV, use the same 
     index for both arrays.\n
     Note: A carrier must contain one downlink band and can contain up to one uplink 
           band. In other words, a carrier ID can be shared by one uplink and one
           downlink. In this case, the carrier information is the same for both.
  */
}coex_wwan_state_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Provides the client with the WWAN state, containing the
              same information as QMI_COEX_WWAN_STATE_IND. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_wwan_state_req_msg_v01;

  /* Message */
/**
    @}
  */

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
     for each uplink and downlink.
     Valid bandwidths are: 2 (represents 1.4), 3, 5, 10, 15, and 20 MHz.
  */

  /* Optional */
  /*  LTE TDD Information */
  uint8_t lte_tdd_info_valid;  /**< Must be set to true if lte_tdd_info is being passed */
  coex_lte_tdd_info_type_v01 lte_tdd_info;
  /**<   \n Note: If LTE is operating with carrier aggregation, this TLV
                reflects the configuration of the primary carrier.
  */

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
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  TD-SCDMA Band Information Set */
  uint8_t tdscdma_band_info_set_valid;  /**< Must be set to true if tdscdma_band_info_set is being passed */
  uint32_t tdscdma_band_info_set_len;  /**< Must be set to # of elements in tdscdma_band_info_set */
  coex_band_type_and_direction_v01 tdscdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all TD-SCDMA center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.       
  */

  /* Optional */
  /*  GSM [Instance 1] Band Information Set */
  uint8_t gsm_band_info_set_valid;  /**< Must be set to true if gsm_band_info_set is being passed */
  uint32_t gsm_band_info_set_len;  /**< Must be set to # of elements in gsm_band_info_set */
  coex_band_type_and_direction_v01 gsm_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 1) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  ONEX Band Information Set */
  uint8_t onex_band_info_set_valid;  /**< Must be set to true if onex_band_info_set is being passed */
  uint32_t onex_band_info_set_len;  /**< Must be set to # of elements in onex_band_info_set */
  coex_band_type_and_direction_v01 onex_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all ONEX center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.       
  */

  /* Optional */
  /*  HDR Band Information Set */
  uint8_t hdr_band_info_set_valid;  /**< Must be set to true if hdr_band_info_set is being passed */
  uint32_t hdr_band_info_set_len;  /**< Must be set to # of elements in hdr_band_info_set */
  coex_band_type_and_direction_v01 hdr_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all HDR center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  WCDMA [Instance 1] Band Information Set */
  uint8_t wcdma_band_info_set_valid;  /**< Must be set to true if wcdma_band_info_set is being passed */
  uint32_t wcdma_band_info_set_len;  /**< Must be set to # of elements in wcdma_band_info_set */
  coex_band_type_and_direction_v01 wcdma_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA (Instance 1) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  GSM [Instance 2] Band Information Set */
  uint8_t gsm2_band_info_set_valid;  /**< Must be set to true if gsm2_band_info_set is being passed */
  uint32_t gsm2_band_info_set_len;  /**< Must be set to # of elements in gsm2_band_info_set */
  coex_band_type_and_direction_v01 gsm2_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 2) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  GSM [Instance 3] Band Information Set */
  uint8_t gsm3_band_info_set_valid;  /**< Must be set to true if gsm3_band_info_set is being passed */
  uint32_t gsm3_band_info_set_len;  /**< Must be set to # of elements in gsm3_band_info_set */
  coex_band_type_and_direction_v01 gsm3_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all GSM (instance 3) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.        
  */

  /* Optional */
  /*  WCDMA [Instance 2] Band Information Set */
  uint8_t wcdma2_band_info_set_valid;  /**< Must be set to true if wcdma2_band_info_set is being passed */
  uint32_t wcdma2_band_info_set_len;  /**< Must be set to # of elements in wcdma2_band_info_set */
  coex_band_type_and_direction_v01 wcdma2_band_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all WCDMA (Instance 2) center frequencies and bandwidths
     for uplink and downlink. Note that a band can be
     specified as both uplink and downlink by combining the mask values 
     of uplink and downlink.
  */

  /* Optional */
  /*  LTE Component Carrier Information */
  uint8_t lte_carrier_info_set_valid;  /**< Must be set to true if lte_carrier_info_set is being passed */
  uint32_t lte_carrier_info_set_len;  /**< Must be set to # of elements in lte_carrier_info_set */
  coex_lte_carrier_info_type_v01 lte_carrier_info_set[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains information specific to each active LTE component carrier.
     To match this TLV to the LTE Band Information Set TLV, use the same 
     index for both arrays.\n
     Note: A carrier must contain one downlink band and can contain up to one uplink 
           band. In other words, a carrier ID can be shared by one uplink and one
           downlink. In this case, the carrier information is the same for both.
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
  /*  WLAN Scan Information (Deprecated) */
  uint8_t scan_info_valid;  /**< Must be set to true if scan_info is being passed */
  coex_wlan_scan_info_type_v01 scan_info;

  /* Optional */
  /*  WLAN Connection Information */
  uint8_t conn_info_valid;  /**< Must be set to true if conn_info is being passed */
  coex_wlan_conn_info_type_v01 conn_info;

  /* Optional */
  /*  WLAN High Priority Information */
  uint8_t high_prio_info_valid;  /**< Must be set to true if high_prio_info is being passed */
  coex_wlan_high_prio_info_type_v01 high_prio_info;
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
              WLAN scan state of the client. (Deprecated) */
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
              WLAN scan state of the client. (Deprecated) */
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
      - COEX_PCM_TOGGLE_FRAME_SYNC (0x0000000000000001) --  Toggle (or do not toggle) the FRAME_SYNC register/signal 
      - COEX_PCM_TOGGLE_TX_ACTIVE (0x0000000000000002) --  Toggle (or do not toggle) the TX_ACTIVE register/signal line 
      - COEX_PCM_TOGGLE_RX_PRIORITY (0x0000000000000004) --  Toggle (or do not toggle) the RX_PRIORITY register/signal line 
      - COEX_PCM_REACT_TO_WCN_PRIORITY (0x0000000000000008) --  React (or do not react) to the WCN_PRIORITY signal line/register 
      - COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN (0x0000000000000010) --  Send (or do not send) the WCI-2 standard's Type 3 inactivity duration message 
      - COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE (0x0000000000000020) --  Send (or do not send) out the WCI-2 standard's Type~6 Tx advance notice message 
      - COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT (0x0000000000000040) --  Enforce the controller's Tx power limit 
      - COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT (0x0000000000000080) --  React (or do not react) to and enforce the WCI-2 standard's Type 6 MWS Tx power limit 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE (0x0000000000000100) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem connected state (bit~3) 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE (0x0000000000000200) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem quantized Tx power state (bit~4) 
      - COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ (0x0000000000000400) --  Send (or do not send) the WCI-2 standard's Type 4 technology SCAN frequency information 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL (0x0000000000000800) --  Send (or do not send) the WCI-2 standard's Type 7 modem cellular Tx antenna selection information 
      - COEX_PCM_RESERVED_FOR_CONTROLLER (0x0000000000001000) --  Policy bitmask reserved for use by the controller 
 \n
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
             that the system reacts to WCN_PRIORITY irrespective of the
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
             previously set value.
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
            previously set value.
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
            previously set value.
  */

  /* Optional */
  /*  Power Threshold for Advance Tx Notification (Type 6) */
  uint8_t tx_power_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if tx_power_threshold_for_adv_tx_notice is being passed */
  int16_t tx_power_threshold_for_adv_tx_notice;
  /**<   Power threshold (in dB10) to decide whether to send out an LTE advance
       Tx notification (type 6) if the COEX_PCM_ SEND_WCI2_TYPE6_TX_ADV_ NOTICE
       policy bit is set.

       Note: At startup the default threshold value is -1280 dB10. If the 
            COEX_ PCM_SEND_WCI2_TYPE6_TX_ ADV_NOTICE policy bit is set, the 
            service uses this value. For all other cases, unless explicitly 
            specified, the service uses the previously set threshold value.
  */

  /* Optional */
  /*  Resource Block Threshold for Advance Tx Notification (Type 6) */
  uint8_t rb_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if rb_threshold_for_adv_tx_notice is being passed */
  uint8_t rb_threshold_for_adv_tx_notice;
  /**<   System's instantaneous RB count threshold to decide
       whether to send out LTE advance Tx notification (type 6) if
       the COEX_PCM_SEND_WCI2_ TYPE6_ TX_ADV_NOTICE policy bit is set.

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system always sends out an advance Tx notification 
             (type 6) irrespective of the number of RBs in use. For all other 
             cases, unless explicitly specified, the service uses the 
             previously set threshold value.
  */

  /* Optional */
  /*  RAT Report Connected State Holdoff Timer */
  uint8_t holdoff_timer_for_rat_conn_state_valid;  /**< Must be set to true if holdoff_timer_for_rat_conn_state is being passed */
  uint16_t holdoff_timer_for_rat_conn_state;
  /**<   Timeout value (in milliseconds) for the timer that is set to absorb fast 
       glitches (transitions) of the cumulative RAT modem-connected 
       state bit. This bit is shared over a WCI2 Type 7 byte (bit[3]).
       Valid range: 0 to 500 milliseconds.

       Note: At startup the default value is 200 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set value.
  */

  /* Optional */
  /*  RAT Quantized Average Tx Power Filter Alpha */
  uint8_t filter_alpha_for_rat_power_state_valid;  /**< Must be set to true if filter_alpha_for_rat_power_state is being passed */
  float filter_alpha_for_rat_power_state;
  /**<   Alpha coefficient for the first-order filter for the RAT (LTE) Tx power 
       state that the system must maintain.
       Valid range: 0.0000 to 0.9999.

       Note: At startup the default value is 0.0000 (minimum) and for all other
             cases, unless explicitly specified, the service uses the
             previously set value.
  */

  /* Optional */
  /*  Power Threshold for MDM Quantized Power State  */
  uint8_t tx_power_threshold_for_rat_power_state_valid;  /**< Must be set to true if tx_power_threshold_for_rat_power_state is being passed */
  uint16_t tx_power_threshold_for_rat_power_state;
  /**<   Power threshold (in mW) to decide whether to send a Type 7 message with
       the MDM power state bit set to 1 or 0. The RAT (LTE) Tx power is averaged
       with a first-order filter with filter_alpha_for_rat_power_state as the 
       time constant.  If the output of this filter is greater than the 
       threshold, the bit is set to 1, otherwise the bit is cleared to 0.
       Then a Type 7 message is sent if the COEX_PCM_SEND_WCI2_TYPE7_ 
       MDM_PWR_STATE policy bit is set.

       Note: At startup the default threshold value is 0 mW. If the 
            COEX_PCM_SEND_WCI2_TYPE7_ MDM_PWR_STATE policy bit is set, the 
            service uses this value. For all other cases, unless explicitly 
            specified, the service uses the previously set threshold value.
   */

  /* Optional */
  /*  RAT Report Tx Power State Holdoff Timer */
  uint8_t holdoff_timer_for_rat_power_state_valid;  /**< Must be set to true if holdoff_timer_for_rat_power_state is being passed */
  uint16_t holdoff_timer_for_rat_power_state;
  /**<   Timeout value (in milliseconds) for the timer that is set
       to absorb fast glitches (transitions) of the cumulative RAT Tx power state
       bit shared over WCI2 Type 7 byte (bit[4]).
       Valid range: 0 to 1000 milliseconds.

       Note: At startup the default value is 200 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set value.
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

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the service's understanding of the last request of the
              client to update the policy for coexistence algorithms. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_policy_req_msg_v01;

  /* Message */
/**
    @}
  */

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
      - COEX_PCM_TOGGLE_FRAME_SYNC (0x0000000000000001) --  Toggle (or do not toggle) the FRAME_SYNC register/signal 
      - COEX_PCM_TOGGLE_TX_ACTIVE (0x0000000000000002) --  Toggle (or do not toggle) the TX_ACTIVE register/signal line 
      - COEX_PCM_TOGGLE_RX_PRIORITY (0x0000000000000004) --  Toggle (or do not toggle) the RX_PRIORITY register/signal line 
      - COEX_PCM_REACT_TO_WCN_PRIORITY (0x0000000000000008) --  React (or do not react) to the WCN_PRIORITY signal line/register 
      - COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN (0x0000000000000010) --  Send (or do not send) the WCI-2 standard's Type 3 inactivity duration message 
      - COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE (0x0000000000000020) --  Send (or do not send) out the WCI-2 standard's Type~6 Tx advance notice message 
      - COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT (0x0000000000000040) --  Enforce the controller's Tx power limit 
      - COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT (0x0000000000000080) --  React (or do not react) to and enforce the WCI-2 standard's Type 6 MWS Tx power limit 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE (0x0000000000000100) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem connected state (bit~3) 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE (0x0000000000000200) --  Send (or do not send) the WCI-2 standard's Type 7 cumulative modem quantized Tx power state (bit~4) 
      - COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ (0x0000000000000400) --  Send (or do not send) the WCI-2 standard's Type 4 technology SCAN frequency information 
      - COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL (0x0000000000000800) --  Send (or do not send) the WCI-2 standard's Type 7 modem cellular Tx antenna selection information 
      - COEX_PCM_RESERVED_FOR_CONTROLLER (0x0000000000001000) --  Policy bitmask reserved for use by the controller 
 \n
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
             that the system reacts to WCN_PRIORITY irrespective of the
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
  /*  Power Threshold for Advance Tx Notification (Type 6) */
  uint8_t tx_power_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if tx_power_threshold_for_adv_tx_notice is being passed */
  int16_t tx_power_threshold_for_adv_tx_notice;
  /**<   Power threshold (in dB10) to decide whether to send out an LTE advance
       Tx notification (type 6) if COEX_PCM_ SEND_WCI2_TYPE6_TX_ADV_ NOTICE
       policy bit is set.

       Note: At startup the default threshold value is -1280 dB10. If the 
             COEX_ PCM_SEND_WCI2_TYPE6_TX_ ADV_NOTICE policy bit is set, the 
             service uses this value. For all other cases, unless specified 
             explicitly, the service uses the previously set threshold value. 
  */

  /* Optional */
  /*  Resource Block Threshold for Advance Tx Notification (Type 6) */
  uint8_t rb_threshold_for_adv_tx_notice_valid;  /**< Must be set to true if rb_threshold_for_adv_tx_notice is being passed */
  uint8_t rb_threshold_for_adv_tx_notice;
  /**<   System's instantaneous RB count threshold to decide
       whether to send out an LTE advance Tx notification (type 6) if
       the COEX_PCM_SEND_WCI2_ TYPE6_TX_ADV_ NOTICE policy bit is set.

       Note: At startup the default threshold value is 0 (minimum), indicating
             that the system always sends out an advance Tx notification (type 6)
             irrespective of the number of RBs in use. For all other cases,
             unless specified explicitly, the service uses the previously
             set threshold value.
  */

  /* Optional */
  /*  RAT Report Connected State Holdoff Timer */
  uint8_t holdoff_timer_for_rat_conn_state_valid;  /**< Must be set to true if holdoff_timer_for_rat_conn_state is being passed */
  uint16_t holdoff_timer_for_rat_conn_state;
  /**<   Timeout value (in milliseconds) for the timer that is set to absorb fast 
       glitches (transitions) of the cumulative RAT modem-connected state bit
       shared over a WCI2 Type 7 byte (bit[3]).
       Valid range: 0 to 500 milliseconds.

       Note: At startup the default value is 200 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set value.
  */

  /* Optional */
  /*  RAT Quantized Average Tx Power Filter Alpha */
  uint8_t filter_alpha_for_rat_power_state_valid;  /**< Must be set to true if filter_alpha_for_rat_power_state is being passed */
  float filter_alpha_for_rat_power_state;
  /**<   Alpha coefficient for the first-order filter for the RAT (LTE) Tx power 
       state that the system must maintain. Valid range: 0.0000 to 0.9999.

       Note: At startup the default value is 0.0000 (minimum) and for all other
             cases, unless explicitly specified, the service uses the
             previously set value.
  */

  /* Optional */
  /*  Power Threshold for RAT Quantized Power State */
  uint8_t tx_power_threshold_for_rat_power_state_valid;  /**< Must be set to true if tx_power_threshold_for_rat_power_state is being passed */
  uint16_t tx_power_threshold_for_rat_power_state;
  /**<   Power threshold (in mW) to decide whether to send a Type 7 message with
       the MDM power state bit set to 1 or 0. The RAT (LTE) Tx power is averaged 
       using a first-order filter with filter_alpha_for_rat_power_state as the 
       time constant. If the output of this filter is greater than the 
       threshold value, the bit is set to 1; otherwise, the bit is cleared to 0. 
       Then a Type 7 message is sent if the COEX_PCM_SEND_WCI2_TYPE7_ MDM_PWR_STATE 
       policy bit is set.

       Note: At startup the default threshold value is 0 mW. If the 
            COEX_PCM_SEND_WCI2_TYPE7_ MDM_PWR_STATE policy bit is set, the 
            service uses this value. For all other cases, unless explicitly 
            specified, the service uses the previously set threshold value.
  */

  /* Optional */
  /*  RAT Report Tx Power State Holdoff Timer */
  uint8_t holdoff_timer_for_rat_power_state_valid;  /**< Must be set to true if holdoff_timer_for_rat_power_state is being passed */
  uint16_t holdoff_timer_for_rat_power_state;
  /**<   Timeout value (in milliseconds) for the timer that is set
       to absorb fast glitches (transitions) of the cumulative RAT Tx power 
       state bit which is shared over a WCI2 type 7 byte (bit[4]).
       Valid range: 0 to 1000 milliseconds.

       Note: At startup the default value is 200 milliseconds and for all
            other cases, unless explicitly specified, the service uses the
            previously set value.
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
       and Wi-Fi\reg is inactive.
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

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to stop collecting/collating the LTE BLER metric. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_metrics_lte_bler_stop_req_msg_v01;

  /* Message */
/**
    @}
  */

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
/** Request Message; Request to start collecting/collating the LTE 
    Signal-to-Interference Plus Noise Ratio (SINR) metric. */
typedef struct {

  /* Mandatory */
  /*  Alpha */
  float alpha;
  /**<   Filter parameter for the LTE SINR metric.
       Valid range: 0 to 1 with 1/100th precision.
  */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE component carrier for which to start collecting 
 metrics, if LTE is doing carrier aggregation. If this field is not present, 
 COEX_CARRIER_ PRIMARY is assumed. Values:\n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_lte_sinr_start_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request to start collecting/collating the LTE 
    Signal-to-Interference Plus Noise Ratio (SINR) metric. */
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
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE component carrier for which to start collecting 
 metrics, if LTE is doing carrier aggregation. If this field is not present, 
 COEX_CARRIER_ PRIMARY is assumed. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_lte_sinr_start_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to read current filter output for the LTE SINR metric. */
typedef struct {

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE component carrier for which the read command is
 issued, if LTE is doing carrier aggregation. If this field is not present, 
 COEX_CARRIER_ PRIMARY is assumed. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_lte_sinr_read_req_msg_v01;  /* Message */
/**
    @}
  */

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
  /**<   Filter output for the total LTE SINR metric, in dBM.
  */

  /* Optional */
  /*  SINR Only Bluetooth Active */
  uint8_t sinr_bt_only_valid;  /**< Must be set to true if sinr_bt_only is being passed */
  float sinr_bt_only;
  /**<   Filter output for the LTE SINR metric (in dBM) when Bluetooth is active
       and Wi-Fi is inactive.
  */

  /* Optional */
  /*  SINR Only Wi-Fi Active */
  uint8_t sinr_wifi_only_valid;  /**< Must be set to true if sinr_wifi_only is being passed */
  float sinr_wifi_only;
  /**<   Filter output for the LTE SINR metric (in dBM) when Wi-Fi is active
       and Bluetooth is inactive.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Active */
  uint8_t sinr_bt_and_wifi_valid;  /**< Must be set to true if sinr_bt_and_wifi is being passed */
  float sinr_bt_and_wifi;
  /**<   Filter output for the LTE SINR metric (in dBM) when both Bluetooth 
       and Wi-Fi are active.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Inactive */
  uint8_t sinr_lte_only_valid;  /**< Must be set to true if sinr_lte_only is being passed */
  float sinr_lte_only;
  /**<   Filter output for the LTE SINR metric (in dBM) when both Bluetooth and 
       Wi-Fi are inactive.
  */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE carrier for which the read command was issued. If 
 this field is not present, COEX_CARRIER_ PRIMARY is assumed. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_lte_sinr_read_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request to stop collecting/collating the LTE SINR metric. */
typedef struct {

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE component carrier for which to stop collecting SINR
 metrics. If this field is not present, COEX_ CARRIER_PRIMARY is assumed.
 Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_lte_sinr_stop_req_msg_v01;  /* Message */
/**
    @}
  */

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

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the LTE component carrier for which to stop collecting SINR
 metrics. If this field is not present, COEX_ CARRIER_PRIMARY is assumed.
 Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
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
       across and the appropriate mask to enable or disable filtering for uplink,
       downlink, or both.
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

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the service's understanding of the client's last
          request to update the band info for COEX algorithms. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_band_filter_info_req_msg_v01;

  /* Message */
/**
    @}
  */

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
       across, and appropriate mask to enable/disable filtering for uplink.
       downlink, or both.
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

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the WCI-2 standard-related MWS offset and jitter parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_wci2_mws_params_req_msg_v01;

  /* Message */
/**
    @}
  */

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
  /*  MWS Rx Deassert Offset */
  uint8_t mws_rx_deassert_offset_valid;  /**< Must be set to true if mws_rx_deassert_offset is being passed */
  coex_range_type_v01 mws_rx_deassert_offset;
  /**<   \n Provides the system's current range of deassert
       offset (in microseconds) for the Rx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Rx Deassert Jitter */
  uint8_t mws_rx_deassert_jitter_valid;  /**< Must be set to true if mws_rx_deassert_jitter is being passed */
  coex_range_type_v01 mws_rx_deassert_jitter;
  /**<   \n Provides the system's current range of deassert
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
  /*  MWS Tx Deassert Offset */
  uint8_t mws_tx_deassert_offset_valid;  /**< Must be set to true if mws_tx_deassert_offset is being passed */
  coex_range_type_v01 mws_tx_deassert_offset;
  /**<   \n Provides the system's current range of deassert
       offset (in microseconds) for the Tx bit of the WCI-2 Type 0 message.
  */

  /* Optional */
  /*  MWS Tx Deassert Jitter */
  uint8_t mws_tx_deassert_jitter_valid;  /**< Must be set to true if mws_tx_deassert_jitter is being passed */
  coex_range_type_v01 mws_tx_deassert_jitter;
  /**<   \n Provides the system's current range of deassert
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
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
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
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
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
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
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
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
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
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
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
       - 0x01 -- WCN is scanning; WWAN might send indications\n
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

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns parameters related to processing conflicting WCN
              and WWAN bands. */
typedef struct {

  /* Mandatory */
  /*  Victim Table Offset */
  uint32_t victim_tbl_offset;
  /**<   Offset in the complete victim table from which to extract the sub-table
       with maximum size COEX_MAX_VICTIM_TBL_ENTRIES in the response. 
       QMI_ERR_INVALID_ARG_V01 is returned if this TLV is outside the bounds of 
       the complete table.
  */
}coex_get_conflict_params_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns parameters related to processing conflicting WCN
              and WWAN bands. */
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
  /*  WCN Behavior */
  uint8_t wcn_behavior_valid;  /**< Must be set to true if wcn_behavior is being passed */
  coex_wcn_behavior_mask_v01 wcn_behavior;
  /**<   Action to take during a conflict situation between WWAN and WCN. Values:\n
      - COEX_WCN_BHVR_NONE (0x0000000000000001) --  WCN does not exercise any coexistence methods 
      - COEX_WCN_BHVR_QMI_LITE (0x0000000000000002) --  WCN uses QMI Lite communication 
      - COEX_WCN_BHVR_SMEM_DATA (0x0000000000000004) --  WCN uses SMEM for data plane communication 
 */

  /* Optional */
  /*  Victim Table Offset */
  uint8_t victim_tbl_offset_valid;  /**< Must be set to true if victim_tbl_offset is being passed */
  uint32_t victim_tbl_offset;
  /**<   Offset in the complete victim table from which the provided table in this 
       message was extracted. This number is equal to the offset in the
       request message.
  */

  /* Optional */
  /*  Victim Table Size */
  uint8_t victim_tbl_complete_size_valid;  /**< Must be set to true if victim_tbl_complete_size is being passed */
  uint32_t victim_tbl_complete_size;
  /**<   Size of the complete victim table. The client uses this to determine when the
       complete victim table has been sent. If this value is greater than the size of the
       provided victim table upon request, the client can make more requests with a
       larger offset to get more pieces of the complete victim table.
  */

  /* Optional */
  /*  Victim Table */
  uint8_t victim_tbl_valid;  /**< Must be set to true if victim_tbl is being passed */
  uint32_t victim_tbl_len;  /**< Must be set to # of elements in victim_tbl */
  coex_conflict_definition_type_v01 victim_tbl[COEX_MAX_VICTIM_TBL_ENTRIES_V01];
  /**<   \n
       Table of conflict definitions between WWAN and WCN and the action to
       take during a conflict situation. Note that index 0 in this table
       corresponds to the entry at index victim_tbl_offset in the overall
       victim table. This table alone might not be the entire victim table. 
  */

  /* Optional */
  /*  Victim Table Groups */
  uint8_t victim_tbl_groups_valid;  /**< Must be set to true if victim_tbl_groups is being passed */
  uint32_t victim_tbl_groups_len;  /**< Must be set to # of elements in victim_tbl_groups */
  uint32_t victim_tbl_groups[COEX_MAX_VICTIM_TBL_ENTRIES_V01];
  /**<   Grouping number of this conflict. Each group number maps one-to-one
       with the Victim Table TLV. \n
       -  If the group numbers for multiple conflicts are the same, all WWAN and
          WCN band ranges must match for the conflicts to be found. If one
          or more conflicts of a group do not match the active WWAN and WCN
          frequencies, none of the conflicts is considered a match. \n
       - 0 is a special value that indicates "do not care" for the group. 
  */
}coex_get_conflict_params_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Starts collecting and collating metrics for a specified RAT. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which to start collecting metrics.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Mandatory */
  /*  Alpha */
  float alpha;
  /**<   Filter parameter for the metric.
       Valid range: 0 to 1, with 1/100th precision.
  */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which to start 
 collecting metrics. If this TLV is not present, COEX_CARRIER_PRIMARY 
 is assumed. If the specified technology does not support carrier 
 aggregation, this TLV is ignored. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_start_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Starts collecting and collating metrics for a specified RAT. */
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
  /**<   Specifies the technology for which to start collecting metrics.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which to start
 collecting metrics. If this TLV is not present, COEX_CARRIER_PRIMARY is
 assumed. If the technology does not support carrier aggregation, this TLV
 is not present. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_start_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Reads the current filter output for the metric. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   Specifies the technology for which the read command is issued.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which the read 
 command is issued. If this TLV is not present, COEX_CARRIER_PRIMARY is 
 assumed. If the technology does not support carrier aggregation, this 
 TLV is ignored. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_read_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Reads the current filter output for the metric. */
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
  /**<   Specifies the technology for which the read command is issued.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Optional */
  /*  Total SINR */
  uint8_t sinr_valid;  /**< Must be set to true if sinr is being passed */
  float sinr;
  /**<   Filter output for the total SINR metric, in dBM.
  */

  /* Optional */
  /*  SINR Only Bluetooth Active */
  uint8_t sinr_bt_only_valid;  /**< Must be set to true if sinr_bt_only is being passed */
  float sinr_bt_only;
  /**<   Filter output for the SINR metric (in dBM) when Bluetooth is active
       and Wi-Fi is inactive.
  */

  /* Optional */
  /*  SINR Only Wi-Fi Active */
  uint8_t sinr_wifi_only_valid;  /**< Must be set to true if sinr_wifi_only is being passed */
  float sinr_wifi_only;
  /**<   Filter output for the SINR metric (in dBM) when Wi-Fi is active
       and Bluetooth is inactive.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Active */
  uint8_t sinr_bt_and_wifi_valid;  /**< Must be set to true if sinr_bt_and_wifi is being passed */
  float sinr_bt_and_wifi;
  /**<   Filter output for the SINR metric (in dBM) when both Bluetooth 
       and Wi-Fi are active.
  */

  /* Optional */
  /*  SINR Both Bluetooth and Wi-Fi Inactive */
  uint8_t sinr_mdm_only_valid;  /**< Must be set to true if sinr_mdm_only is being passed */
  float sinr_mdm_only;
  /**<   Filter output for the SINR metric (in dBM) when both Bluetooth and 
       Wi-Fi are inactive.
  */

  /* Optional */
  /*  Total Narrowband Noise */
  uint8_t nb_noise_valid;  /**< Must be set to true if nb_noise is being passed */
  float nb_noise;
  /**<   Filter output for the total narrowband noise metric, in dBM.
  */

  /* Optional */
  /*  Narrowband Noise Only Bluetooth Active */
  uint8_t nb_noise_bt_only_valid;  /**< Must be set to true if nb_noise_bt_only is being passed */
  float nb_noise_bt_only;
  /**<   Filter output for the narrowband noise metric (in dBM) when Bluetooth 
       is active and Wi-Fi is inactive.
  */

  /* Optional */
  /*  Narrowband Noise Only Wi-Fi Active */
  uint8_t nb_noise_wifi_only_valid;  /**< Must be set to true if nb_noise_wifi_only is being passed */
  float nb_noise_wifi_only;
  /**<   Filter output for the narrowband noise metric (in dBM) when Wi-Fi is active
       and Bluetooth is inactive.
  */

  /* Optional */
  /*  Narrowband Noise Both Bluetooth and Wi-Fi Active */
  uint8_t nb_noise_bt_and_wifi_valid;  /**< Must be set to true if nb_noise_bt_and_wifi is being passed */
  float nb_noise_bt_and_wifi;
  /**<   Filter output for the narrowband noise metric (in dBM) when both Bluetooth 
       and Wi-Fi are active.
  */

  /* Optional */
  /*  Narrowband Noise Both Bluetooth and Wi-Fi Inactive */
  uint8_t nb_noise_mdm_only_valid;  /**< Must be set to true if nb_noise_mdm_only is being passed */
  float nb_noise_mdm_only;
  /**<   Filter output for the narrowband noise metric (in dBM) when both Bluetooth and 
       Wi-Fi are inactive.
  */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which the read 
 command was issued. If this TLV is not present, COEX_CARRIER_PRIMARY is
 assumed. If the technology does not support carrier aggregation, this 
 TLV is not present. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_read_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Stops the collecting and collating of metrics for a 
              specified technology. */
typedef struct {

  /* Mandatory */
  /*  Technology */
  coex_wwan_tech_v01 tech;
  /**<   The technology for which to stop collecting metrics.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which to stop collecting
 metrics. If this TLV is not present, COEX_CARRIER_PRIMARY is assumed.
 If the technology does not support carrier aggregation, this field
 is ignored. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_stop_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Stops the collecting and collating of metrics for a 
              specified technology. */
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
  /**<   The technology for which to stop collecting metrics.
 Values: \n
      - COEX_LTE_TECH (0) --  LTE 
      - COEX_TDSCDMA_TECH (1) --  TD-SCDMA \n 
      - COEX_GSM_TECH (2) --  GSM (Instance 1)\n 
      - COEX_ONEX_TECH (3) --  CDMA2000\textsuperscript{\textregistered} 1X \n 
      - COEX_HDR_TECH (4) --  HDR \n 
      - COEX_WCDMA_TECH (5) --  WCDMA (Instance 1)\n 
      - COEX_GSM2_TECH (6) --  GSM (Instance 2)\n 
      - COEX_GSM3_TECH (7) --  GSM (Instance 3)\n 
      - COEX_WCDMA2_TECH (8) --  WCDMA (Instance 2) 
 */

  /* Optional */
  /*  Carrier ID */
  uint8_t carrier_valid;  /**< Must be set to true if carrier is being passed */
  coex_carrier_v01 carrier;
  /**<   Specifies the carrier of the specified technology for which to stop collecting
 metrics. If this TLV is not present, COEX_CARRIER_PRIMARY is assumed.
 If the technology does not support carrier aggregation, this TLV
 is not present. Values: \n
      - COEX_CARRIER_PRIMARY (0) --  Primary component carrier ID 
      - COEX_CARRIER_SECONDARY_0 (1) --  Component carrier ID for first secondary carrier 
      - COEX_CARRIER_SECONDARY_1 (2) --  Component carrier ID for second secondary carrier 
      - COEX_CARRIER_SECONDARY_2 (3) --  Component carrier ID for third secondary carrier 
      - COEX_CARRIER_SECONDARY_3 (4) --  Component carrier ID for fourth secondary carrier 
 */
}coex_metrics_stop_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Sets the current list of bands to monitor for 
              reporting RAT scan frequencies when COEX algorithms are active. */
typedef struct {

  /* Optional */
  /*  Bands to Monitor */
  uint8_t bands_valid;  /**< Must be set to true if bands is being passed */
  uint32_t bands_len;  /**< Must be set to # of elements in bands */
  coex_band_type_v01 bands[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  /**<   \n WWAN frequency and bandwidth sets to monitor and report over 
          WCI2 Type 4 when a RAT performs a frequency scan.

          Note: Only indices 0 to 30 are valid. A total of 31 entries of
                WCI type 4 payload has 5 bits to represent this index. Bit 0
                is reserved. Therefore, the 0 to 30 indices in this filter 
                are represented as bits 1 to 31 as the payload of WCI2 Type 4 
                message.
  */
}coex_set_scan_freq_band_filter_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Sets the current list of bands to monitor for 
              reporting RAT scan frequencies when COEX algorithms are active. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_set_scan_freq_band_filter_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the most recent RAT scan frequencies to monitor for 
              the current list of bands. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}coex_get_scan_freq_band_filter_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the most recent RAT scan frequencies to monitor for 
              the current list of bands. */
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
  coex_band_type_v01 bands[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  /**<   \n WWAN frequency and bandwidth sets to monitor and report over 
          WCI2 Type 4 when a RAT performs a frequency scan.
  */
}coex_get_scan_freq_band_filter_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Returns the most recently set WLAN high priority state of 
              the client. */
typedef struct {

  /* Mandatory */
  /*  WLAN High Priority ID */
  uint32_t id;
  /**<   WLAN high priority unique ID; allows multiple high priority events to
     occur simultaneously.
  */
}coex_get_wlan_high_prio_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Returns the most recently set WLAN high priority state of 
              the client. */
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
  /*  WLAN High Priority Information */
  uint8_t high_prio_info_valid;  /**< Must be set to true if high_prio_info is being passed */
  coex_wlan_high_prio_info_type_v01 high_prio_info;
}coex_get_wlan_high_prio_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Enables or disables LTE-U. */
typedef struct {

  /* Mandatory */
  /*  LTE-U Configure */
  uint8_t enable;
  /**<   Set true to enable LTE-U. Else LTE-U is disabled.
  */
}coex_configure_lte_5ghz_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Enables or disables LTE-U. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_configure_lte_5ghz_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent out by the service to report LTE 5GHz state. */
typedef struct {

  /* Optional */
  /*  LTE 5GHz state. */
  uint8_t lte_5ghz_active_valid;  /**< Must be set to true if lte_5ghz_active is being passed */
  uint8_t lte_5ghz_active;
  /**<   Provides the state of LTE 5GHz.
  */

  /* Optional */
  /*  LTE 5GHz Frequency Set */
  uint8_t frequency_valid;  /**< Must be set to true if frequency is being passed */
  uint32_t frequency_len;  /**< Must be set to # of elements in frequency */
  uint32_t frequency[COEX_WWAN_MAX_BANDS_PER_TECH_V01];
  /**<   \n Contains all LTE 5GHz center frequencies. 
  */
}coex_lte_5ghz_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Request antennas for cold boot calibration. */
typedef struct {

  /* Mandatory */
  /*  Antennas to switch */
  coex_antenna_mask_v01 antenna;
  /**<   Antennas to be switched to WLAN.
  */
}coex_antenna_switch_to_wlan_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Request antennas for cold boot calibration. */
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
  /*  Antennas granted */
  uint8_t grant_valid;  /**< Must be set to true if grant is being passed */
  coex_antenna_mask_v01 grant;
  /**<   Antennas granted.
  */
}coex_antenna_switch_to_wlan_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Release antennas after cold boot calibration. */
typedef struct {

  /* Mandatory */
  /*  Antennas to switch */
  coex_antenna_mask_v01 antenna;
  /**<   Antennas to be switched back to MDM.
  */
}coex_antenna_switch_to_mdm_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Release antennas after cold boot calibration. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE\n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}coex_antenna_switch_to_mdm_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indication sent out by the service to report antenna state. */
typedef struct {

  /* Optional */
  /*  Antenna Client Switch Position */
  uint8_t tx_antenna_state_valid;  /**< Must be set to true if tx_antenna_state is being passed */
  uint32_t tx_antenna_state_len;  /**< Must be set to # of elements in tx_antenna_state */
  coex_tx_antenna_state_type_v01 tx_antenna_state[COEX_MAX_TX_ANTENNA_V01];
  /**<   \n Contains switch positions of Tx antennas. 
  */
}coex_tx_antenna_state_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_COEX_CONDITION_FAIL_IND_V01 
//#define REMOVE_QMI_COEX_CONDITION_SUCCESS_IND_V01 
//#define REMOVE_QMI_COEX_CONFIGURE_LTE_5GHZ_V01 
//#define REMOVE_QMI_COEX_GET_BAND_FILTER_INFO_V01 
//#define REMOVE_QMI_COEX_GET_CONFLICT_PARAMS_V01 
//#define REMOVE_QMI_COEX_GET_POLICY_V01 
//#define REMOVE_QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_V01 
//#define REMOVE_QMI_COEX_GET_SLEEP_NOTIFICATION_V01 
//#define REMOVE_QMI_COEX_GET_SUPPORTED_FIELDS_V01 
//#define REMOVE_QMI_COEX_GET_SUPPORTED_MSGS_V01 
//#define REMOVE_QMI_COEX_GET_WCI2_MWS_PARAMS_V01 
//#define REMOVE_QMI_COEX_GET_WLAN_CONN_STATE_V01 
//#define REMOVE_QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_V01 
//#define REMOVE_QMI_COEX_GET_WLAN_SCAN_STATE_V01 
//#define REMOVE_QMI_COEX_GET_WWAN_STATE_V01 
//#define REMOVE_QMI_COEX_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_COEX_LTE_5GHZ_STATUS_IND_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_BLER_IND_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_BLER_START_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_BLER_STOP_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_START_V01 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_V01 
//#define REMOVE_QMI_COEX_METRICS_READ_V01 
//#define REMOVE_QMI_COEX_METRICS_START_V01 
//#define REMOVE_QMI_COEX_METRICS_STOP_V01 
//#define REMOVE_QMI_COEX_RESET_V01 
//#define REMOVE_QMI_COEX_SET_BAND_FILTER_INFO_V01 
//#define REMOVE_QMI_COEX_SET_POLICY_V01 
//#define REMOVE_QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_V01 
//#define REMOVE_QMI_COEX_SET_SLEEP_NOTIFICATION_V01 
//#define REMOVE_QMI_COEX_SET_WLAN_STATE_V01 
//#define REMOVE_QMI_COEX_SLEEP_IND_V01 
//#define REMOVE_QMI_COEX_SWITCH_ANTENNA_TO_MDM_V01 
//#define REMOVE_QMI_COEX_SWITCH_ANTENNA_TO_WLAN_V01 
//#define REMOVE_QMI_COEX_TX_ANTENNA_STATE_IND_V01 
//#define REMOVE_QMI_COEX_WAKEUP_IND_V01 
//#define REMOVE_QMI_COEX_WCN_WAKE_SYNC_V01 
//#define REMOVE_QMI_COEX_WCN_WAKE_SYNC_IND_V01 
//#define REMOVE_QMI_COEX_WWAN_STATE_IND_V01 

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
#define QMI_COEX_GET_CONFLICT_PARAMS_REQ_V01 0x0039
#define QMI_COEX_GET_CONFLICT_PARAMS_RESP_V01 0x0039
#define QMI_COEX_METRICS_START_REQ_V01 0x003A
#define QMI_COEX_METRICS_START_RESP_V01 0x003A
#define QMI_COEX_METRICS_READ_REQ_V01 0x003B
#define QMI_COEX_METRICS_READ_RESP_V01 0x003B
#define QMI_COEX_METRICS_STOP_REQ_V01 0x003C
#define QMI_COEX_METRICS_STOP_RESP_V01 0x003C
#define QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_REQ_V01 0x003D
#define QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_RESP_V01 0x003D
#define QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_REQ_V01 0x003E
#define QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_RESP_V01 0x003E
#define QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_REQ_V01 0x003F
#define QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_RESP_V01 0x003F
#define QMI_COEX_CONFIGURE_LTE_5GHZ_REQ_V01 0x0040
#define QMI_COEX_CONFIGURE_LTE_5GHZ_RESP_V01 0x0040
#define QMI_COEX_LTE_5GHZ_STATUS_IND_V01 0x0041
#define QMI_COEX_SWITCH_ANTENNA_TO_WLAN_REQ_V01 0x0042
#define QMI_COEX_SWITCH_ANTENNA_TO_WLAN_RESP_V01 0x0042
#define QMI_COEX_SWITCH_ANTENNA_TO_MDM_REQ_V01 0x0043
#define QMI_COEX_SWITCH_ANTENNA_TO_MDM_RESP_V01 0x0043
#define QMI_COEX_TX_ANTENNA_STATE_IND_V01 0x0044
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

