#ifndef DSD_SERVICE_01_H
#define DSD_SERVICE_01_H
/**
  @file data_system_determination_v01.h

  @brief This is the public header file which defines the dsd service Data structures.

  This header file defines the types and structures that were defined in
  dsd. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Dec 28 2017 (Spin 0)
   From IDL File: data_system_determination_v01.idl */

/** @defgroup dsd_qmi_consts Constant values defined in the IDL */
/** @defgroup dsd_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup dsd_qmi_enums Enumerated types used in QMI messages */
/** @defgroup dsd_qmi_messages Structures sent as QMI messages */
/** @defgroup dsd_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup dsd_qmi_accessor Accessor for QMI service object */
/** @defgroup dsd_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dsd_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define DSD_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define DSD_V01_IDL_MINOR_VERS 0x24
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define DSD_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define DSD_V01_MAX_MESSAGE_ID 0x0055
/**
    @}
  */


/** @addtogroup dsd_qmi_consts
    @{
  */
#define QMI_DSD_MAC_ADDR_LEN_V01 6
#define QMI_DSD_IPV6_ADDR_LEN_V01 16
#define QMI_DSD_MAX_AVAIL_SYS_V01 15
#define QMI_DSD_MAX_APNS_V01 15
#define QMI_DSD_MAX_APN_LEN_V01 100
#define QMI_DSD_MAX_SSID_LEN_V01 32
#define QMI_DSD_MAX_SYSTEMS_V01 3
#define QMI_DSD_MAX_CHANNELS_V01 16
#define QMI_DSD_MAX_BSSIDS_V01 16
#define QMI_DSD_MAX_BSSIDS_PER_REPORT_V01 50
#define QMI_DSD_MAX_SSIDS_V01 8
#define QMI_DSD_MAX_NETWORK_MODE_LEN_V01 16
#define QMI_DSD_MAX_ICCID_LEN_V01 10
#define QMI_DSD_MAX_LTE_NUM_SCELL_V01 10
#define QMI_DSD_MCC_MNC_SIZE_V01 3
#define QMI_DSD_HPLMN_LIST_MAX_V01 32
#define QMI_DSD_EXCP_HPLMN_LIST_MAX_V01 32
#define QMI_DSD_MAX_FQDN_LEN_V01 200
#define QMI_DSD_MAX_HESSID_LEN_V01 6
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_BIND_SUBSCRIPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_PRIMARY_SUBS_V01 = 0x0001, /**<  Primary \n  */
  DSD_SECONDARY_SUBS_V01 = 0x0002, /**<  Secondary \n  */
  DSD_TERTIARY_SUBS_V01 = 0x0003, /**<  Tertiary  */
  DSD_BIND_SUBSCRIPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_bind_subscription_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_SYS_NETWORK_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_SYS_NETWORK_3GPP_V01 = 0, /**<  3GPP \n  */
  DSD_SYS_NETWORK_3GPP2_V01 = 1, /**<  3GPP2 \n  */
  DSD_SYS_NETWORK_WLAN_V01 = 2, /**<  WLAN  */
  DSD_SYS_NETWORK_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_sys_network_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_SYS_RAT_EX_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_SYS_RAT_EX_NULL_BEARER_V01 = 0x0, /**<  NULL bearer  */
  DSD_SYS_RAT_EX_3GPP_WCDMA_V01 = 0x1, /**<  3GPP WCDMA     */
  DSD_SYS_RAT_EX_3GPP_GERAN_V01 = 0x2, /**<  3GPP GERAN  */
  DSD_SYS_RAT_EX_3GPP_LTE_V01 = 0x3, /**<  3GPP LTE  */
  DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01 = 0x4, /**<  3GPP TDSCDMA  */
  DSD_SYS_RAT_EX_3GPP_WLAN_V01 = 0x5, /**<  3GPP WLAN  */
  DSD_SYS_RAT_EX_3GPP_MAX_V01 = 0x64, /**<  3GPP maximum  */
  DSD_SYS_RAT_EX_3GPP2_1X_V01 = 0x65, /**<  3GPP2 1X  */
  DSD_SYS_RAT_EX_3GPP2_HRPD_V01 = 0x66, /**<  3GPP2 HRPD  */
  DSD_SYS_RAT_EX_3GPP2_EHRPD_V01 = 0x67, /**<  3GPP2 EHRPD  */
  DSD_SYS_RAT_EX_3GPP2_WLAN_V01 = 0x68, /**<  3GPP2 WLAN  */
  DSD_SYS_RAT_EX_3GPP2_MAX_V01 = 0xC8, /**<  3GPP2 maximum  */
  DSD_SYS_RAT_EX_WLAN_V01 = 0xC9, /**<  WLAN  */
  DSD_SYS_RAT_EX_WLAN_MAX_V01 = 0x12C, /**<  WLAN maximum  */
  DSD_SYS_RAT_EX_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_sys_rat_ex_enum_v01;
/**
    @}
  */

typedef uint64_t dsd_sys_so_mask_v01;
#define QMI_DSD_SO_MASK_UNSPECIFIED_V01 ((dsd_sys_so_mask_v01)0x0000000000000000ull)
#define QMI_DSD_3GPP_SO_MASK_WCDMA_V01 ((dsd_sys_so_mask_v01)0x0000000000000001ull)
#define QMI_DSD_3GPP_SO_MASK_HSDPA_V01 ((dsd_sys_so_mask_v01)0x0000000000000002ull)
#define QMI_DSD_3GPP_SO_MASK_HSUPA_V01 ((dsd_sys_so_mask_v01)0x0000000000000004ull)
#define QMI_DSD_3GPP_SO_MASK_HSDPAPLUS_V01 ((dsd_sys_so_mask_v01)0x0000000000000008ull)
#define QMI_DSD_3GPP_SO_MASK_DC_HSDPAPLUS_V01 ((dsd_sys_so_mask_v01)0x0000000000000010ull)
#define QMI_DSD_3GPP_SO_MASK_64_QAM_V01 ((dsd_sys_so_mask_v01)0x0000000000000020ull)
#define QMI_DSD_3GPP_SO_MASK_HSPA_V01 ((dsd_sys_so_mask_v01)0x0000000000000040ull)
#define QMI_DSD_3GPP_SO_MASK_GPRS_V01 ((dsd_sys_so_mask_v01)0x0000000000000080ull)
#define QMI_DSD_3GPP_SO_MASK_EDGE_V01 ((dsd_sys_so_mask_v01)0x0000000000000100ull)
#define QMI_DSD_3GPP_SO_MASK_GSM_V01 ((dsd_sys_so_mask_v01)0x0000000000000200ull)
#define QMI_DSD_3GPP_SO_MASK_S2B_V01 ((dsd_sys_so_mask_v01)0x0000000000000400ull)
#define QMI_DSD_3GPP_SO_MASK_LTE_LIMITED_SRVC_V01 ((dsd_sys_so_mask_v01)0x0000000000000800ull)
#define QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01 ((dsd_sys_so_mask_v01)0x0000000000001000ull)
#define QMI_DSD_3GPP_SO_MASK_LTE_TDD_V01 ((dsd_sys_so_mask_v01)0x0000000000002000ull)
#define QMI_DSD_3GPP_SO_MASK_TDSCDMA_V01 ((dsd_sys_so_mask_v01)0x0000000000004000ull)
#define QMI_DSD_3GPP_SO_MASK_DC_HSUPA_V01 ((dsd_sys_so_mask_v01)0x0000000000008000ull)
#define QMI_DSD_3GPP_SO_MASK_LTE_CA_DL_V01 ((dsd_sys_so_mask_v01)0x0000000000010000ull)
#define QMI_DSD_3GPP_SO_MASK_LTE_CA_UL_V01 ((dsd_sys_so_mask_v01)0x0000000000020000ull)
#define QMI_DSD_3GPP2_SO_MASK_1X_IS95_V01 ((dsd_sys_so_mask_v01)0x0000000001000000ull)
#define QMI_DSD_3GPP2_SO_MASK_1X_IS2000_V01 ((dsd_sys_so_mask_v01)0x0000000002000000ull)
#define QMI_DSD_3GPP2_SO_MASK_1X_IS2000_REL_A_V01 ((dsd_sys_so_mask_v01)0x0000000004000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REV0_DPA_V01 ((dsd_sys_so_mask_v01)0x0000000008000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVA_DPA_V01 ((dsd_sys_so_mask_v01)0x0000000010000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVB_DPA_V01 ((dsd_sys_so_mask_v01)0x0000000020000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVA_MPA_V01 ((dsd_sys_so_mask_v01)0x0000000040000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MPA_V01 ((dsd_sys_so_mask_v01)0x0000000080000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVA_EMPA_V01 ((dsd_sys_so_mask_v01)0x0000000100000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVB_EMPA_V01 ((dsd_sys_so_mask_v01)0x0000000200000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_REVB_MMPA_V01 ((dsd_sys_so_mask_v01)0x0000000400000000ull)
#define QMI_DSD_3GPP2_SO_MASK_HDR_EVDO_FMC_V01 ((dsd_sys_so_mask_v01)0x0000000800000000ull)
#define QMI_DSD_3GPP2_SO_MASK_1X_CS_V01 ((dsd_sys_so_mask_v01)0x0000001000000000ull)
/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  dsd_sys_network_enum_v01 technology;
  /**<   Technology type. Values:\n
      - DSD_SYS_NETWORK_3GPP (0) --  3GPP \n
      - DSD_SYS_NETWORK_3GPP2 (1) --  3GPP2 \n
      - DSD_SYS_NETWORK_WLAN (2) --  WLAN
 */

  dsd_sys_rat_ex_enum_v01 rat_value;
  /**<   RAT value. Values:\n
      - DSD_SYS_RAT_EX_NULL_BEARER (0x0) --  NULL bearer
      - DSD_SYS_RAT_EX_3GPP_WCDMA (0x1) --  3GPP WCDMA
      - DSD_SYS_RAT_EX_3GPP_GERAN (0x2) --  3GPP GERAN
      - DSD_SYS_RAT_EX_3GPP_LTE (0x3) --  3GPP LTE
      - DSD_SYS_RAT_EX_3GPP_TDSCDMA (0x4) --  3GPP TDSCDMA
      - DSD_SYS_RAT_EX_3GPP_WLAN (0x5) --  3GPP WLAN
      - DSD_SYS_RAT_EX_3GPP_MAX (0x64) --  3GPP maximum
      - DSD_SYS_RAT_EX_3GPP2_1X (0x65) --  3GPP2 1X
      - DSD_SYS_RAT_EX_3GPP2_HRPD (0x66) --  3GPP2 HRPD
      - DSD_SYS_RAT_EX_3GPP2_EHRPD (0x67) --  3GPP2 EHRPD
      - DSD_SYS_RAT_EX_3GPP2_WLAN (0x68) --  3GPP2 WLAN
      - DSD_SYS_RAT_EX_3GPP2_MAX (0xC8) --  3GPP2 maximum
      - DSD_SYS_RAT_EX_WLAN (0xC9) --  WLAN
      - DSD_SYS_RAT_EX_WLAN_MAX (0x12C) --  WLAN maximum  */

  dsd_sys_so_mask_v01 so_mask;
  /**<   Service Option (SO) mask to indicate the service option or type of
       application.

       An SO mask value of zero indicates that this field is ignored.
       Values: \n
       - 0x00 -- SO Mask Unspecified \n
       3GPP SO Mask: \n
       - 0x01  -- WCDMA \n
       - 0x02  -- HSDPA \n
       - 0x04  -- HSUPA \n
       - 0x08  -- HSDPAPLUS \n
       - 0x10  -- DC HSDPAPLUS \n
       - 0x20  -- 64 QAM \n
       - 0x40  -- HSPA \n
       - 0x80  -- GPRS \n
       - 0x100 -- EDGE \n
       - 0x200 -- GSM \n
       - 0x400 -- S2B \n
       - 0x800 -- LTE Limited Service \n
       - 0x1000 -- LTE FDD \n
       - 0x2000 -- LTE TDD \n
       - 0x4000 -- TDSCDMA \n
       - 0x8000 -- DC HSUPA \n
       - 0x10000 -- LTE CA DL \n
       - 0x20000 -- LTE CA UL \n
       3GPP2 SO Mask: \n
       - 0x0001000000  -- 1X IS95 \n
       - 0x0002000000  -- 1X IS2000 \n
       - 0x0004000000  -- 1X IS2000 REL A \n
       - 0x0008000000  -- HDR REV0 DPA \n
       - 0x0010000000  -- HDR REVA DPA \n
       - 0x0020000000  -- HDR REVB DPA \n
       - 0x0040000000  -- HDR REVA MPA \n
       - 0x0080000000  -- HDR REVB MPA \n
       - 0x0100000000  -- HDR REVA EMPA \n
       - 0x0200000000  -- HDR REVB EMPA \n
       - 0x0400000000  -- HDR REVB MMPA \n
       - 0x0800000000  -- HDR EVDO FMC  \n
       - 0x1000000000  -- 1X Circuit Switched */
}dsd_system_status_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name. */

  uint32_t apn_avail_sys_len;  /**< Must be set to # of elements in apn_avail_sys */
  dsd_system_status_info_type_v01 apn_avail_sys[QMI_DSD_MAX_AVAIL_SYS_V01];
  /**<  \n Array of all available systems for the APN. The first entry in the
       list contains the preferred system for the APN.
  */
}dsd_apn_avail_sys_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name. */

  dsd_system_status_info_type_v01 curr_pref_sys;
  /**<   Current Preferred system for the APN. */

  dsd_system_status_info_type_v01 recommended_pref_sys;
  /**<   Recommended system for the APN. */
}dsd_apn_pref_sys_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  dsd_system_status_info_type_v01 curr_pref_sys;
  /**<   Current Preferred system. */

  dsd_system_status_info_type_v01 recommended_pref_sys;
  /**<   Recommended system. */
}dsd_global_pref_sys_info_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t dsd_wifi_meas_param_mask_v01;
#define QMI_DSD_WIFI_MEAS_PARAM_NONE_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000000ull) /**<  No Parameters  */
#define QMI_DSD_WIFI_MEAS_PARAM_RSSI_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000001ull) /**<  Received Signal Strength Indication  */
#define QMI_DSD_WIFI_MEAS_PARAM_SINR_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000002ull) /**<  Signal to Inteference plus Noise Ratio  */
#define QMI_DSD_WIFI_MEAS_PARAM_BSS_LOAD_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000004ull) /**<  Basic Service Set Load  */
#define QMI_DSD_WIFI_MEAS_PARAM_PHY_RATE_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000008ull) /**<  Physical Layer Transmission Rate  */
#define QMI_DSD_WIFI_MEAS_PARAM_PACKET_ERROR_RATE_V01 ((dsd_wifi_meas_param_mask_v01)0x0000000000000010ull) /**<  Packet Error Rate  */
/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_MEAS_REPORT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_RAW_V01 = 0, /**<  Report Raw Wi-Fi Measurements values periodically.  */
  DSD_WIFI_MEAS_REPORT_TYPE_ONE_TIME_AFTER_THRESHOLD_V01 = 1, /**<  Report Wi-Fi Measurements once after crossing the threshold.  */
  DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_AFTER_THRESHOLD_V01 = 2, /**<  Report Wi-Fi Measurements periodically after crossing the threshold.  */
  DSD_WIFI_MEAS_REPORT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_meas_report_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_NETWORK_MODE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_DSD_WIFI_NETWORK_MODE_802_11_1997_V01 = 0, /**<  Legacy network mode.  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11a_V01 = 1, /**<  OFDM network mode.  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11b_V01 = 2, /**<  802.11b  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11g_V01 = 3, /**<  802.11g  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11_2007_V01 = 4, /**<  802.11.2007  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11n_V01 = 5, /**<  802.11n  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11_2012_V01 = 6, /**<  802.11.2012  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ac_V01 = 7, /**<  802.11.ac  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ad_V01 = 8, /**<  802.11.ad  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11af_V01 = 9, /**<  802.11.af  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ah_V01 = 10, /**<  802.11.ah  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ai_V01 = 11, /**<  802.11.ai  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11aj_V01 = 12, /**<  802.11.aj  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11aq_V01 = 13, /**<  802.11.aq  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ax_V01 = 14, /**<  802.11.ax  */
  QMI_DSD_WIFI_NETWORK_MODE_802_11ay_V01 = 15, /**<  802.11.ay  */
  DSD_WIFI_NETWORK_MODE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_network_mode_type_enum_v01;
/**
    @}
  */

typedef uint64_t dsd_wqe_profile_type_mask_v01;
#define QMI_DSD_WQE_PROFILE_INACTIVE_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000000000000ull) /**<  Wi-Fi Quality Estimation profile inactive  */
#define QMI_DSD_WQE_PROFILE_INTERNET_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000000000001ull) /**<  Wi-Fi Quality Estimation profile internet  */
#define QMI_DSD_WQE_PROFILE_AUDIO_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000000000002ull) /**<  Wi-Fi Quality Estimation profile audio  */
#define QMI_DSD_WQE_PROFILE_VIDEO_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000000000004ull) /**<  Wi-Fi Quality Estimation profile video  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_1_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000100000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 1  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_2_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000200000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 2  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_3_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000400000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 3  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_4_V01 ((dsd_wqe_profile_type_mask_v01)0x0000000800000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 4  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_5_V01 ((dsd_wqe_profile_type_mask_v01)0x0000001000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 5  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_6_V01 ((dsd_wqe_profile_type_mask_v01)0x0000002000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 6  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_7_V01 ((dsd_wqe_profile_type_mask_v01)0x0000004000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 7  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_8_V01 ((dsd_wqe_profile_type_mask_v01)0x0000008000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 8  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_9_V01 ((dsd_wqe_profile_type_mask_v01)0x0000010000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 9  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_10_V01 ((dsd_wqe_profile_type_mask_v01)0x0000020000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 10  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_11_V01 ((dsd_wqe_profile_type_mask_v01)0x0000040000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 11  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_12_V01 ((dsd_wqe_profile_type_mask_v01)0x0000080000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 12  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_13_V01 ((dsd_wqe_profile_type_mask_v01)0x0000100000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 13  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_14_V01 ((dsd_wqe_profile_type_mask_v01)0x0000200000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 14  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_15_V01 ((dsd_wqe_profile_type_mask_v01)0x0000400000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 15  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_16_V01 ((dsd_wqe_profile_type_mask_v01)0x0000800000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 16  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_17_V01 ((dsd_wqe_profile_type_mask_v01)0x0001000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 17  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_18_V01 ((dsd_wqe_profile_type_mask_v01)0x0002000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 18  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_19_V01 ((dsd_wqe_profile_type_mask_v01)0x0004000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 19  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_20_V01 ((dsd_wqe_profile_type_mask_v01)0x0008000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 20  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_21_V01 ((dsd_wqe_profile_type_mask_v01)0x0010000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 21  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_22_V01 ((dsd_wqe_profile_type_mask_v01)0x0020000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 22  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_23_V01 ((dsd_wqe_profile_type_mask_v01)0x0040000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 23  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_24_V01 ((dsd_wqe_profile_type_mask_v01)0x0080000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 24  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_25_V01 ((dsd_wqe_profile_type_mask_v01)0x0100000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 25  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_26_V01 ((dsd_wqe_profile_type_mask_v01)0x0200000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 26  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_27_V01 ((dsd_wqe_profile_type_mask_v01)0x0400000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 27  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_28_V01 ((dsd_wqe_profile_type_mask_v01)0x0800000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 28  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_29_V01 ((dsd_wqe_profile_type_mask_v01)0x1000000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 29  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_30_V01 ((dsd_wqe_profile_type_mask_v01)0x2000000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 30  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_31_V01 ((dsd_wqe_profile_type_mask_v01)0x4000000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 31  */
#define QMI_DSD_WQE_PROFILE_IMS_TYPE_32_V01 ((dsd_wqe_profile_type_mask_v01)0x8000000000000000ull) /**<  Wi-Fi Quality Estimation profile for IMS Type 32  */
/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WQE_PROFILE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_DSD_WQE_PROFILE_TYPE_DEFAULT_V01 = 0, /**<  Wi-Fi Quality Estimation profile default  */
  QMI_DSD_WQE_PROFILE_TYPE_INTERNET_V01 = 1, /**<  Wi-Fi Quality Estimation profile internet  */
  QMI_DSD_WQE_PROFILE_TYPE_AUDIO_V01 = 2, /**<  Wi-Fi Quality Estimation profile audio  */
  QMI_DSD_WQE_PROFILE_TYPE_VIDEO_V01 = 3, /**<  Wi-Fi Quality Estimation profile video  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_1_V01 = 4, /**<  Wi-Fi Quality Estimation profile for IMS Type 1  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_2_V01 = 5, /**<  Wi-Fi Quality Estimation profile for IMS Type 2  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_3_V01 = 6, /**<  Wi-Fi Quality Estimation profile for IMS Type 3  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_4_V01 = 7, /**<  Wi-Fi Quality Estimation profile for IMS Type 4  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_5_V01 = 8, /**<  Wi-Fi Quality Estimation profile for IMS Type 5  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_6_V01 = 9, /**<  Wi-Fi Quality Estimation profile for IMS Type 6  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_7_V01 = 10, /**<  Wi-Fi Quality Estimation profile for IMS Type 7  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_8_V01 = 11, /**<  Wi-Fi Quality Estimation profile for IMS Type 8  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_9_V01 = 12, /**<  Wi-Fi Quality Estimation profile for IMS Type 9  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_10_V01 = 13, /**<  Wi-Fi Quality Estimation profile for IMS Type 10  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_11_V01 = 14, /**<  Wi-Fi Quality Estimation profile for IMS Type 11  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_12_V01 = 15, /**<  Wi-Fi Quality Estimation profile for IMS Type 12  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_13_V01 = 16, /**<  Wi-Fi Quality Estimation profile for IMS Type 13  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_14_V01 = 17, /**<  Wi-Fi Quality Estimation profile for IMS Type 14  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_15_V01 = 18, /**<  Wi-Fi Quality Estimation profile for IMS Type 15  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_16_V01 = 19, /**<  Wi-Fi Quality Estimation profile for IMS Type 16  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_17_V01 = 20, /**<  Wi-Fi Quality Estimation profile for IMS Type 17  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_18_V01 = 21, /**<  Wi-Fi Quality Estimation profile for IMS Type 18  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_19_V01 = 22, /**<  Wi-Fi Quality Estimation profile for IMS Type 19  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_20_V01 = 23, /**<  Wi-Fi Quality Estimation profile for IMS Type 20  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_21_V01 = 24, /**<  Wi-Fi Quality Estimation profile for IMS Type 21  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_22_V01 = 25, /**<  Wi-Fi Quality Estimation profile for IMS Type 22  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_23_V01 = 26, /**<  Wi-Fi Quality Estimation profile for IMS Type 23  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_24_V01 = 27, /**<  Wi-Fi Quality Estimation profile for IMS Type 24  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_25_V01 = 28, /**<  Wi-Fi Quality Estimation profile for IMS Type 25  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_26_V01 = 29, /**<  Wi-Fi Quality Estimation profile for IMS Type 26  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_27_V01 = 30, /**<  Wi-Fi Quality Estimation profile for IMS Type 27  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_28_V01 = 31, /**<  Wi-Fi Quality Estimation profile for IMS Type 28  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_29_V01 = 32, /**<  Wi-Fi Quality Estimation profile for IMS Type 29  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_30_V01 = 33, /**<  Wi-Fi Quality Estimation profile for IMS Type 30  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_31_V01 = 34, /**<  Wi-Fi Quality Estimation profile for IMS Type 31  */
  QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_32_V01 = 35, /**<  Wi-Fi Quality Estimation profile for IMS Type 32  */
  DSD_WQE_PROFILE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wqe_profile_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_DDS_SWITCH_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_DDS_SWITCH_PERMANENT_V01 = 0x00, /**<  Permanent(Default)  */
  DSD_DDS_SWITCH_TEMPORARY_V01 = 0x01, /**<  Temporary  */
  DSD_DDS_SWITCH_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_dds_switch_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the current system status. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_system_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the current system status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Available Systems */
  uint8_t avail_sys_valid;  /**< Must be set to true if avail_sys is being passed */
  uint32_t avail_sys_len;  /**< Must be set to # of elements in avail_sys */
  dsd_system_status_info_type_v01 avail_sys[QMI_DSD_MAX_AVAIL_SYS_V01];
  /**<   Array of all available systems. The first entry in the list is
       the preferred system.
  */

  /* Optional */
  /*  APN Available System Info */
  uint8_t apn_avail_sys_info_valid;  /**< Must be set to true if apn_avail_sys_info is being passed */
  uint32_t apn_avail_sys_info_len;  /**< Must be set to # of elements in apn_avail_sys_info */
  dsd_apn_avail_sys_info_type_v01 apn_avail_sys_info[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their preferred/available systems.
  */

  /* Optional */
  /* Current and Recommended Global Preferred System Info */
  uint8_t global_pref_sys_valid;  /**< Must be set to true if global_pref_sys is being passed */
  dsd_global_pref_sys_info_type_v01 global_pref_sys;
  /**<  \n The Global current and recommended preferred systems.
  */

  /* Optional */
  /* APN Current and Recommended Preferred System Info */
  uint8_t apn_pref_sys_valid;  /**< Must be set to true if apn_pref_sys is being passed */
  uint32_t apn_pref_sys_len;  /**< Must be set to # of elements in apn_pref_sys */
  dsd_apn_pref_sys_info_type_v01 apn_pref_sys[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their current and recommended
         preferred systems.
  */
}dsd_get_system_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Registers the requesting control point for data system status indications. */
typedef struct {

  /* Optional */
  /*  Suppress SO changes */
  uint8_t limit_so_mask_change_ind_valid;  /**< Must be set to true if limit_so_mask_change_ind is being passed */
  uint8_t limit_so_mask_change_ind;
  /**<   If this TLV is provided in the request message, the indication is
       generated only if the rat_value changes from the last indication.
  */

  /* Optional */
  /*  Register/Deregister for Data System Status Changes */
  uint8_t report_data_system_status_changes_valid;  /**< Must be set to true if report_data_system_status_changes is being passed */
  uint8_t report_data_system_status_changes;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report data system status change (default).
  */

  /* Optional */
  /*  Report only Preferred technology changes */
  uint8_t report_pref_tech_change_only_valid;  /**< Must be set to true if report_pref_tech_change_only is being passed */
  uint8_t report_pref_tech_change_only;
  /**<   If this TLV is provided and set to true in the request message, the indication
       is generated only if the Preferred Technology value (i.e. only technology
       field of the Preferred System) changes from the last indication.
  */
}dsd_system_status_change_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Registers the requesting control point for data system status indications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dsd_system_status_change_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the system status state changes. */
typedef struct {

  /* Optional */
  /*  Available Systems */
  uint8_t avail_sys_valid;  /**< Must be set to true if avail_sys is being passed */
  uint32_t avail_sys_len;  /**< Must be set to # of elements in avail_sys */
  dsd_system_status_info_type_v01 avail_sys[QMI_DSD_MAX_AVAIL_SYS_V01];
  /**<  \n Array of all available systems. The first entry in the list is
       the preferred system.
  */

  /* Optional */
  /*  APN Available System Info */
  uint8_t apn_avail_sys_info_valid;  /**< Must be set to true if apn_avail_sys_info is being passed */
  uint32_t apn_avail_sys_info_len;  /**< Must be set to # of elements in apn_avail_sys_info */
  dsd_apn_avail_sys_info_type_v01 apn_avail_sys_info[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their preferred/available systems.
  */

  /* Optional */
  /* Current and Recommended Global Preferred System Info */
  uint8_t global_pref_sys_valid;  /**< Must be set to true if global_pref_sys is being passed */
  dsd_global_pref_sys_info_type_v01 global_pref_sys;
  /**<  \n The Global current and recommended preferred systems.
  */

  /* Optional */
  /* APN Current and Recommended Preferred System Info */
  uint8_t apn_pref_sys_valid;  /**< Must be set to true if apn_pref_sys is being passed */
  uint32_t apn_pref_sys_len;  /**< Must be set to # of elements in apn_pref_sys */
  dsd_apn_pref_sys_info_type_v01 apn_pref_sys[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their current and recommended
         preferred systems.
  */
}dsd_system_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Associates the requesting control point to the subscription requested. */
typedef struct {

  /* Mandatory */
  /*  Bind Subscription */
  dsd_bind_subscription_enum_v01 bind_subs;
  /**<   Subscription to bind to. Values: \n
      - DSD_PRIMARY_SUBS (0x0001) --  Primary \n
      - DSD_SECONDARY_SUBS (0x0002) --  Secondary \n
      - DSD_TERTIARY_SUBS (0x0003) --  Tertiary
 */
}dsd_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Associates the requesting control point to the subscription requested. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dsd_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the subscription associated with the control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_bind_subscription_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the subscription associated with the control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Bound Subscription */
  uint8_t bind_subscription_valid;  /**< Must be set to true if bind_subscription is being passed */
  dsd_bind_subscription_enum_v01 bind_subscription;
  /**<   Values: \n
      - DSD_PRIMARY_SUBS (0x0001) --  Primary \n
      - DSD_SECONDARY_SUBS (0x0002) --  Secondary \n
      - DSD_TERTIARY_SUBS (0x0003) --  Tertiary
 */
}dsd_get_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WQE_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WQE_INACTIVE_V01 = 0, /**<  Inactive (Default) \n */
  DSD_WQE_ACTIVE_V01 = 1, /**<  Active  */
  DSD_WQE_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wqe_status_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ipv6_address[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   IPV6 address.
   */

  uint8_t prefix_len;
  /**<   IPV6 address prefix length.
   */
}dsd_ipv6_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_ASSOC_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_ASSOC_P2P_GO_V01 = 0, /**<  Peer to Peer Group Owner Wi-Fi association  */
  DSD_WIFI_ASSOC_P2P_CLIENT_V01 = 1, /**<  Peer to Peer Client Wi-Fi association  */
  DSD_WIFI_ASSOC_STA_V01 = 2, /**<  Station Wi-Fi association  */
  DSD_WIFI_ASSOC_SOFTAP_V01 = 3, /**<  Software Access Point Wi-Fi association  */
  DSD_WIFI_ASSOC_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_assoc_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_CONN_STATUS_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_CONN_STATUS_DISASSOCIATED_V01 = 0, /**<  Connection status specifying if Wi-Fi is
     disassociated from an access point  */
  DSD_WIFI_CONN_STATUS_ASSOCIATED_V01 = 1, /**<  Connection status specifying if Wi-Fi is
     associated with an access point  */
  DSD_WIFI_CONN_STATUS_IP_CONNECTED_V01 = 2, /**<  Connection status specifying if Wi-Fi is
     IP connected  */
  DSD_WIFI_CONN_STATUS_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_conn_status_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_BANDWIDTH_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_BANDWIDTH_TYPE_20MHz_V01 = 0, /**<  Bandwidth is 20MHz  */
  DSD_WIFI_BANDWIDTH_TYPE_40MHz_V01 = 1, /**<  Bandwidth is 40MHz  */
  DSD_WIFI_BANDWIDTH_TYPE_80MHz_V01 = 2, /**<  Bandwidth is 80MHz  */
  DSD_WIFI_BANDWIDTH_TYPE_160MHz_V01 = 3, /**<  Bandwidth is 160MHz  */
  DSD_WIFI_BANDWIDTH_TYPE_80MHz_80MHz_V01 = 4, /**<  Bandwidth is 80MHz+80MHz  */
  DSD_WIFI_BANDWIDTH_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_bandwidth_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Indicates if WLAN is available. Provides the IP and
           MAC addresses if WLAN is up. */
typedef struct {

  /* Mandatory */
  /*  WLAN Access Point MAC address. */
  uint8_t wlan_ap_mac_address[QMI_DSD_MAC_ADDR_LEN_V01];
  /**<   WLAN access point MAC address.  */

  /* Optional */
  /*  WLAN IPV4 address. */
  uint8_t wlan_ipv4_address_valid;  /**< Must be set to true if wlan_ipv4_address is being passed */
  uint32_t wlan_ipv4_address;
  /**<   WLAN IPV4 address. */

  /* Optional */
  /*  WLAN IPV6 address. */
  uint8_t wlan_ipv6_address_valid;  /**< Must be set to true if wlan_ipv6_address is being passed */
  dsd_ipv6_addr_type_v01 wlan_ipv6_address;

  /* Optional */
  /*  Wi-Fi Quality Estimation (WQE) status. */
  uint8_t wqe_status_valid;  /**< Must be set to true if wqe_status is being passed */
  dsd_wqe_status_enum_v01 wqe_status;
  /**<   Conveys if WQE was performed on the application
 processor before declaring that WLAN is available. \n
 Values: \n
      - DSD_WQE_INACTIVE (0) --  Inactive (Default) \n
      - DSD_WQE_ACTIVE (1) --  Active
 */

  /* Optional */
  /*  DNS IPV4 address 1. */
  uint8_t dns_ipv4_address_1_valid;  /**< Must be set to true if dns_ipv4_address_1 is being passed */
  uint32_t dns_ipv4_address_1;
  /**<   DNS IPV4 address. */

  /* Optional */
  /*  DNS IPV4 address 2. */
  uint8_t dns_ipv4_address_2_valid;  /**< Must be set to true if dns_ipv4_address_2 is being passed */
  uint32_t dns_ipv4_address_2;
  /**<   DNS IPV4 address. */

  /* Optional */
  /*  DNS IPV6 address 1. */
  uint8_t dns_ipv6_address_1_valid;  /**< Must be set to true if dns_ipv6_address_1 is being passed */
  uint8_t dns_ipv6_address_1[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   DNS IPV6 address. */

  /* Optional */
  /*  DNS IPV6 address 2. */
  uint8_t dns_ipv6_address_2_valid;  /**< Must be set to true if dns_ipv6_address_2 is being passed */
  uint8_t dns_ipv6_address_2[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   DNS IPV6 address. */

  /* Optional */
  /*  ePDG IPV4 address 1. */
  uint8_t epdg_ipv4_address_1_valid;  /**< Must be set to true if epdg_ipv4_address_1 is being passed */
  uint32_t epdg_ipv4_address_1;
  /**<   ePDG IPV4 address. */

  /* Optional */
  /*  ePDG IPV4 address 2. */
  uint8_t epdg_ipv4_address_2_valid;  /**< Must be set to true if epdg_ipv4_address_2 is being passed */
  uint32_t epdg_ipv4_address_2;
  /**<   ePDG IPV4 address. */

  /* Optional */
  /*  ePDG IPV6 address 1. */
  uint8_t epdg_ipv6_address_1_valid;  /**< Must be set to true if epdg_ipv6_address_1 is being passed */
  uint8_t epdg_ipv6_address_1[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   ePDG IPV6 address. */

  /* Optional */
  /*  ePDG IPV6 address 2. */
  uint8_t epdg_ipv6_address_2_valid;  /**< Must be set to true if epdg_ipv6_address_2 is being passed */
  uint8_t epdg_ipv6_address_2[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   ePDG IPV6 address. */

  /* Optional */
  /*  SSID. */
  uint8_t ssid_valid;  /**< Must be set to true if ssid is being passed */
  uint32_t ssid_len;  /**< Must be set to # of elements in ssid */
  uint8_t ssid[QMI_DSD_MAX_SSID_LEN_V01];
  /**<   Service Set Identifier. */

  /* Optional */
  /*  Channel */
  uint8_t channel_valid;  /**< Must be set to true if channel is being passed */
  uint16_t channel;
  /**<   Channel in MHz. */

  /* Optional */
  /*  Channel bandwidth */
  uint8_t channel_bandwidth_valid;  /**< Must be set to true if channel_bandwidth is being passed */
  uint16_t channel_bandwidth;
  /**<   Channel bandwith in MHz
  */

  /* Optional */
  /*  Wi-Fi Quality Estimation (WQE)Profile type. */
  uint8_t wqe_profile_type_valid;  /**< Must be set to true if wqe_profile_type is being passed */
  dsd_wqe_profile_type_mask_v01 wqe_profile_type;
  /**<   Conveys if WQE was performed on the Application
 for audio, video or internet data. Values \n
      - QMI_DSD_WQE_PROFILE_INACTIVE (0x0000000000000000) --  Wi-Fi Quality Estimation profile inactive
      - QMI_DSD_WQE_PROFILE_INTERNET (0x0000000000000001) --  Wi-Fi Quality Estimation profile internet
      - QMI_DSD_WQE_PROFILE_AUDIO (0x0000000000000002) --  Wi-Fi Quality Estimation profile audio
      - QMI_DSD_WQE_PROFILE_VIDEO (0x0000000000000004) --  Wi-Fi Quality Estimation profile video
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_1 (0x0000000100000000) --  Wi-Fi Quality Estimation profile for IMS Type 1
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_2 (0x0000000200000000) --  Wi-Fi Quality Estimation profile for IMS Type 2
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_3 (0x0000000400000000) --  Wi-Fi Quality Estimation profile for IMS Type 3
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_4 (0x0000000800000000) --  Wi-Fi Quality Estimation profile for IMS Type 4
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_5 (0x0000001000000000) --  Wi-Fi Quality Estimation profile for IMS Type 5
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_6 (0x0000002000000000) --  Wi-Fi Quality Estimation profile for IMS Type 6
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_7 (0x0000004000000000) --  Wi-Fi Quality Estimation profile for IMS Type 7
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_8 (0x0000008000000000) --  Wi-Fi Quality Estimation profile for IMS Type 8
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_9 (0x0000010000000000) --  Wi-Fi Quality Estimation profile for IMS Type 9
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_10 (0x0000020000000000) --  Wi-Fi Quality Estimation profile for IMS Type 10
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_11 (0x0000040000000000) --  Wi-Fi Quality Estimation profile for IMS Type 11
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_12 (0x0000080000000000) --  Wi-Fi Quality Estimation profile for IMS Type 12
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_13 (0x0000100000000000) --  Wi-Fi Quality Estimation profile for IMS Type 13
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_14 (0x0000200000000000) --  Wi-Fi Quality Estimation profile for IMS Type 14
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_15 (0x0000400000000000) --  Wi-Fi Quality Estimation profile for IMS Type 15
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_16 (0x0000800000000000) --  Wi-Fi Quality Estimation profile for IMS Type 16
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_17 (0x0001000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 17
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_18 (0x0002000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 18
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_19 (0x0004000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 19
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_20 (0x0008000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 20
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_21 (0x0010000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 21
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_22 (0x0020000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 22
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_23 (0x0040000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 23
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_24 (0x0080000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 24
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_25 (0x0100000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 25
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_26 (0x0200000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 26
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_27 (0x0400000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 27
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_28 (0x0800000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 28
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_29 (0x1000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 29
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_30 (0x2000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 30
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_31 (0x4000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 31
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_32 (0x8000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 32
 */

  /* Optional */
  /*  Wi-Fi Association Type */
  uint8_t wifi_assoc_type_valid;  /**< Must be set to true if wifi_assoc_type is being passed */
  dsd_wifi_assoc_type_enum_v01 wifi_assoc_type;
  /**<   Wi-Fi Association type. Values \n
      - DSD_WIFI_ASSOC_P2P_GO (0) --  Peer to Peer Group Owner Wi-Fi association
      - DSD_WIFI_ASSOC_P2P_CLIENT (1) --  Peer to Peer Client Wi-Fi association
      - DSD_WIFI_ASSOC_STA (2) --  Station Wi-Fi association
      - DSD_WIFI_ASSOC_SOFTAP (3) --  Software Access Point Wi-Fi association
 */

  /* Optional */
  /*  Network Mode */
  uint8_t network_mode_valid;  /**< Must be set to true if network_mode is being passed */
  dsd_wifi_network_mode_type_enum_v01 network_mode;
  /**<   Enum specifying the network mode for the channel. Values \n
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_1997 (0) --  Legacy network mode.
      - QMI_DSD_WIFI_NETWORK_MODE_802_11a (1) --  OFDM network mode.
      - QMI_DSD_WIFI_NETWORK_MODE_802_11b (2) --  802.11b
      - QMI_DSD_WIFI_NETWORK_MODE_802_11g (3) --  802.11g
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_2007 (4) --  802.11.2007
      - QMI_DSD_WIFI_NETWORK_MODE_802_11n (5) --  802.11n
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_2012 (6) --  802.11.2012
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ac (7) --  802.11.ac
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ad (8) --  802.11.ad
      - QMI_DSD_WIFI_NETWORK_MODE_802_11af (9) --  802.11.af
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ah (10) --  802.11.ah
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ai (11) --  802.11.ai
      - QMI_DSD_WIFI_NETWORK_MODE_802_11aj (12) --  802.11.aj
      - QMI_DSD_WIFI_NETWORK_MODE_802_11aq (13) --  802.11.aq
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ax (14) --  802.11.ax
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ay (15) --  802.11.ay
 */

  /* Optional */
  /*  Connection status */
  uint8_t connection_status_valid;  /**< Must be set to true if connection_status is being passed */
  dsd_wifi_conn_status_type_enum_v01 connection_status;
  /**<   Wi-Fi Connection status type. Values \n
      - DSD_WIFI_CONN_STATUS_DISASSOCIATED (0) --  Connection status specifying if Wi-Fi is
     disassociated from an access point
      - DSD_WIFI_CONN_STATUS_ASSOCIATED (1) --  Connection status specifying if Wi-Fi is
     associated with an access point
      - DSD_WIFI_CONN_STATUS_IP_CONNECTED (2) --  Connection status specifying if Wi-Fi is
     IP connected
 */

  /* Optional */
  /*  Secondary Channel */
  uint8_t secondary_channel_valid;  /**< Must be set to true if secondary_channel is being passed */
  uint16_t secondary_channel;
  /**<   Secondary channel in MHz.  */

  /* Optional */
  /*  Bandwidth Type */
  uint8_t bandwidth_type_valid;  /**< Must be set to true if bandwidth_type is being passed */
  dsd_wifi_bandwidth_type_enum_v01 bandwidth_type;
  /**<   Bandwidth type. Values: \n
      - DSD_WIFI_BANDWIDTH_TYPE_20MHz (0) --  Bandwidth is 20MHz
      - DSD_WIFI_BANDWIDTH_TYPE_40MHz (1) --  Bandwidth is 40MHz
      - DSD_WIFI_BANDWIDTH_TYPE_80MHz (2) --  Bandwidth is 80MHz
      - DSD_WIFI_BANDWIDTH_TYPE_160MHz (3) --  Bandwidth is 160MHz
      - DSD_WIFI_BANDWIDTH_TYPE_80MHz_80MHz (4) --  Bandwidth is 80MHz+80MHz
 */

  /* Optional */
  /*  Default Route */
  uint8_t is_default_route_valid;  /**< Must be set to true if is_default_route is being passed */
  uint8_t is_default_route;
  /**<   Specifies if Wifi is the default route.
       Values: \n
	  - 0 -- Wifi is not the default route. \n
	  - 1 -- Wifi is the default route. */

  /* Optional */
  /*  HESSID. */
  uint8_t hessid_valid;  /**< Must be set to true if hessid is being passed */
  uint8_t hessid[QMI_DSD_MAX_HESSID_LEN_V01];
  /**<   Homogenous Extended Service Set Identifier of the Wifi Access Point. */
}dsd_wlan_available_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Indicates if WLAN is available. Provides the IP and
           MAC addresses if WLAN is up. */
typedef struct {

  /* Mandatory */
  /*  Result Code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.     */
}dsd_wlan_available_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Indicates if WLAN is not available.  */
typedef struct {

  /* Optional */
  /*  Wi-Fi Quality Estimation (WQE) status. */
  uint8_t wqe_status_valid;  /**< Must be set to true if wqe_status is being passed */
  dsd_wqe_status_enum_v01 wqe_status;
  /**<   Conveys if WQE was performed on the Application
 Processor before declaring WLAN is unavailable. \n
 Values: \n
      - DSD_WQE_INACTIVE (0) --  Inactive (Default) \n
      - DSD_WQE_ACTIVE (1) --  Active
 */
}dsd_wlan_not_available_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Indicates if WLAN is not available.  */
typedef struct {

  /* Mandatory */
  /*  Result Code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.   */
}dsd_wlan_not_available_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WLAN_PREFERENCE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WLAN_LB_PREFERRED_V01 = 0, /**<  WLAN Local Breakout is preferred.  */
  DSD_WLAN_LB_NOT_PREFERRED_V01 = 1, /**<  WLAN Local Breakout is not preferred.  */
  DSD_WLAN_PREFERENCE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wlan_preference_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the preference for WLAN  */
typedef struct {

  /* Mandatory */
  /*  WLAN preference. */
  dsd_wlan_preference_enum_v01 wlan_preference;
  /**<   Preference for WLAN. \n
 Values: \n
      - DSD_WLAN_LB_PREFERRED (0) --  WLAN Local Breakout is preferred.
      - DSD_WLAN_LB_NOT_PREFERRED (1) --  WLAN Local Breakout is not preferred.
 */
}dsd_set_wlan_preference_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the preference for WLAN  */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_wlan_preference_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Gets the preference for WLAN  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_wlan_preference_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Gets the preference for WLAN  */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */

  /* Optional */
  /*  WLAN preference. */
  uint8_t wlan_preference_valid;  /**< Must be set to true if wlan_preference is being passed */
  dsd_wlan_preference_enum_v01 wlan_preference;
  /**<   Preference for WLAN. \n
 Values: \n
      - DSD_WLAN_LB_PREFERRED (0) --  WLAN Local Breakout is preferred.
      - DSD_WLAN_LB_NOT_PREFERRED (1) --  WLAN Local Breakout is not preferred.
 */
}dsd_get_wlan_preference_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_APN_PREF_SYS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_APN_PREF_SYS_WWAN_V01 = 0, /**<  WWAN is preferred.  */
  DSD_APN_PREF_SYS_WLAN_V01 = 1, /**<  WLAN is preferred.  */
  DSD_APN_PREF_SYS_IWLAN_V01 = 2, /**<  IWLAN is preferred.  */
  DSD_APN_PREF_SYS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_apn_pref_sys_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name. */

  dsd_apn_pref_sys_enum_v01 pref_sys;
  /**<   Preferred system for the APN. Values: \n
      - DSD_APN_PREF_SYS_WWAN (0) --  WWAN is preferred.
      - DSD_APN_PREF_SYS_WLAN (1) --  WLAN is preferred.
      - DSD_APN_PREF_SYS_IWLAN (2) --  IWLAN is preferred.
 */
}dsd_apn_pref_sys_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the highest technology preference for an APN. */
typedef struct {

  /* Mandatory */
  /*  APN Preferred System. */
  dsd_apn_pref_sys_type_v01 apn_pref_sys;
  /**<   Highest technology preference for an APN. \n
  */
}dsd_set_apn_preferred_system_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the highest technology preference for an APN. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_apn_preferred_system_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_MODEM_POWER_COST_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_MODEM_POWER_COST_NOT_SUPPORTED_V01 = 0, /**<  Not supported.  */
  DSD_MODEM_POWER_COST_LOW_V01 = 1, /**<  Low.  */
  DSD_MODEM_POWER_COST_MEDIUM_V01 = 2, /**<  Medium.  */
  DSD_MODEM_POWER_COST_HIGH_V01 = 3, /**<  High.  */
  DSD_MODEM_POWER_COST_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_modem_power_cost_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries for the cost of modem power. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_modem_power_cost_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries for the cost of modem power. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */

  /* Optional */
  /*  Modem Power Cost */
  uint8_t power_cost_valid;  /**< Must be set to true if power_cost is being passed */
  dsd_modem_power_cost_enum_v01 power_cost;
  /**<   Modem power cost. \n
  */
}dsd_get_modem_power_cost_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Starts a transaction to access the PDN policy database. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_pdn_policy_start_txn_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Starts a transaction to access the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */

  /* Optional */
  /*  Transaction ID */
  uint8_t txn_id_valid;  /**< Must be set to true if txn_id is being passed */
  uint32_t txn_id;
  /**<   Handle to a new transaction. \n
  */
}dsd_pdn_policy_start_txn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_PDN_POLICY_OVERRIDE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_PDN_POLICY_OVERRIDE_NONE_V01 = 0, /**<  No override (default). */
  DSD_PDN_POLICY_OVERRIDE_API_V01 = 1, /**<  API override.  */
  DSD_PDN_POLICY_OVERRIDE_OPTION_V01 = 2, /**<  UI option override.  */
  DSD_PDN_POLICY_OVERRIDE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_pdn_policy_override_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Adds an entry to the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */

  /* Mandatory */
  /*  APN Name */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   The APN uniquely identifies each entry.
  */

  /* Mandatory */
  /*  Supported System Priority List */
  uint32_t supported_system_priority_list_len;  /**< Must be set to # of elements in supported_system_priority_list */
  dsd_apn_pref_sys_enum_v01 supported_system_priority_list[QMI_DSD_MAX_SYSTEMS_V01];
  /**<   Supported systems for the APN in decreasing order of priority.\n
      - DSD_APN_PREF_SYS_WWAN (0) --  WWAN is preferred.
      - DSD_APN_PREF_SYS_WLAN (1) --  WLAN is preferred.
      - DSD_APN_PREF_SYS_IWLAN (2) --  IWLAN is preferred.
 */

  /* Optional */
  /*  Is Default */
  uint8_t is_default_valid;  /**< Must be set to true if is_default is being passed */
  uint8_t is_default;
  /**<   Specifies whether this is the default APN. Values: \n
       - 0 -- FALSE (Default value when TLV is absent) \n
       - 1 -- TRUE
  */

  /* Optional */
  /*  Override Type */
  uint8_t override_type_valid;  /**< Must be set to true if override_type is being passed */
  dsd_pdn_policy_override_enum_v01 override_type;
  /**<   Specifies whether an override is possible for the preferred system of the PDN.
 Values: \n
      - DSD_PDN_POLICY_OVERRIDE_NONE (0) --  No override (default).
      - DSD_PDN_POLICY_OVERRIDE_API (1) --  API override.
      - DSD_PDN_POLICY_OVERRIDE_OPTION (2) --  UI option override.
 */
}dsd_add_pdn_policy_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Adds an entry to the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_add_pdn_policy_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Modifies an existing entry in the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */

  /* Mandatory */
  /*  APN Name */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   The APN that uniquely identifies each entry.
  */

  /* Optional */
  /*  Supported System Priority List */
  uint8_t supported_system_priority_list_valid;  /**< Must be set to true if supported_system_priority_list is being passed */
  uint32_t supported_system_priority_list_len;  /**< Must be set to # of elements in supported_system_priority_list */
  dsd_apn_pref_sys_enum_v01 supported_system_priority_list[QMI_DSD_MAX_SYSTEMS_V01];
  /**<   Supported systems for the APN in decreasing order of priority.\n
      - DSD_APN_PREF_SYS_WWAN (0) --  WWAN is preferred.
      - DSD_APN_PREF_SYS_WLAN (1) --  WLAN is preferred.
      - DSD_APN_PREF_SYS_IWLAN (2) --  IWLAN is preferred.
 */

  /* Optional */
  /*  Is Default */
  uint8_t is_default_valid;  /**< Must be set to true if is_default is being passed */
  uint8_t is_default;
  /**<   Specifies whether this is the default APN. Values: \n
       - 0 -- FALSE  \n
       - 1 -- TRUE
  */

  /* Optional */
  /*  Override Type */
  uint8_t override_type_valid;  /**< Must be set to true if override_type is being passed */
  dsd_pdn_policy_override_enum_v01 override_type;
  /**<   Specifies whether an override is possible for the preferred system of the PDN.
 Values: \n
      - DSD_PDN_POLICY_OVERRIDE_NONE (0) --  No override (default).
      - DSD_PDN_POLICY_OVERRIDE_API (1) --  API override.
      - DSD_PDN_POLICY_OVERRIDE_OPTION (2) --  UI option override.
 */
}dsd_modify_pdn_policy_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Modifies an existing entry in the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_modify_pdn_policy_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Deletes an entry in the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */

  /* Mandatory */
  /*  APN Name */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   The APN that uniquely identifies each entry.
  */
}dsd_delete_pdn_policy_by_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Deletes an entry in the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_delete_pdn_policy_by_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name.
   */
}dsd_apn_name_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Retrieves a list of APNs from the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */
}dsd_get_pdn_policy_apn_list_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Retrieves a list of APNs from the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.     */

  /* Optional */
  /*  APN List */
  uint8_t apn_list_valid;  /**< Must be set to true if apn_list is being passed */
  uint32_t apn_list_len;  /**< Must be set to # of elements in apn_list */
  dsd_apn_name_type_v01 apn_list[QMI_DSD_MAX_APNS_V01];
  /**<   \n List of APNs.
  */
}dsd_get_pdn_policy_apn_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Retrieves settings for a particular APN from the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */

  /* Mandatory */
  /*  APN Name */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   The APN that uniquely identifies each entry.
  */
}dsd_get_pdn_policy_settings_for_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Retrieves settings for a particular APN from the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.     */

  /* Optional */
  /*  APN Name */
  uint8_t apn_name_valid;  /**< Must be set to true if apn_name is being passed */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   The APN that uniquely identifies each entry.
  */

  /* Optional */
  /*  Supported System Priority List */
  uint8_t supported_system_priority_list_valid;  /**< Must be set to true if supported_system_priority_list is being passed */
  uint32_t supported_system_priority_list_len;  /**< Must be set to # of elements in supported_system_priority_list */
  dsd_apn_pref_sys_enum_v01 supported_system_priority_list[QMI_DSD_MAX_SYSTEMS_V01];
  /**<   Supported systems for the APN in decreasing order of priority.\n
      - DSD_APN_PREF_SYS_WWAN (0) --  WWAN is preferred.
      - DSD_APN_PREF_SYS_WLAN (1) --  WLAN is preferred.
      - DSD_APN_PREF_SYS_IWLAN (2) --  IWLAN is preferred.
 */

  /* Optional */
  /*  Is Default */
  uint8_t is_default_valid;  /**< Must be set to true if is_default is being passed */
  uint8_t is_default;
  /**<   Specifies whether this is the default APN. Values: \n
       - 0 -- FALSE  \n
       - 1 -- TRUE
  */

  /* Optional */
  /*  Override Type */
  uint8_t override_type_valid;  /**< Must be set to true if override_type is being passed */
  dsd_pdn_policy_override_enum_v01 override_type;
  /**<   Specifies whether an override is possible for the preferred system of the PDN.
 Values: \n
      - DSD_PDN_POLICY_OVERRIDE_NONE (0) --  No override (default).
      - DSD_PDN_POLICY_OVERRIDE_API (1) --  API override.
      - DSD_PDN_POLICY_OVERRIDE_OPTION (2) --  UI option override.
 */
}dsd_get_pdn_policy_settings_for_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_PDN_POLICY_END_TRANSACTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_PDN_POLICY_TRANSACTION_COMMIT_V01 = 0, /**<  Commit the transaction changes to persistent storage.  */
  DSD_PDN_POLICY_TRANSACTION_CANCEL_V01 = 1, /**<  Rollback the transaction changes.  */
  DSD_PDN_POLICY_END_TRANSACTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_pdn_policy_end_transaction_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Ends the outstanding transaction on the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t txn_id;
  /**<   Transaction ID handle obtained using QMI_DSD_PDN_POLICY_START_ TXN.
  */

  /* Mandatory */
  /*  Txn Exec Type */
  dsd_pdn_policy_end_transaction_enum_v01 txn_exec_type;
  /**<   Either commit or cancel the transaction changes. Values: \n
      - DSD_PDN_POLICY_TRANSACTION_COMMIT (0) --  Commit the transaction changes to persistent storage.
      - DSD_PDN_POLICY_TRANSACTION_CANCEL (1) --  Rollback the transaction changes.
 */
}dsd_pdn_policy_end_txn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Ends the outstanding transaction on the PDN policy database. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.     */
}dsd_pdn_policy_end_txn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_APN_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_APN_TYPE_DEFAULT_V01 = 0, /**<  APN type for Default/Internet traffic  */
  DSD_APN_TYPE_IMS_V01 = 1, /**<  APN type for IMS  */
  DSD_APN_TYPE_MMS_V01 = 2, /**<  APN type for Multimedia Messaging Service  */
  DSD_APN_TYPE_DUN_V01 = 3, /**<  APN type for Dial Up Network  */
  DSD_APN_TYPE_SUPL_V01 = 4, /**<  APN type for Secure User Plane Location  */
  DSD_APN_TYPE_HIPRI_V01 = 5, /**<  APN type for High Priority Mobile Data  */
  DSD_APN_TYPE_FOTA_V01 = 6, /**<  APN type for over the air administration  */
  DSD_APN_TYPE_CBS_V01 = 7, /**<  APN type for Carrier Branded Services  */
  DSD_APN_TYPE_IA_V01 = 8, /**<  APN type for Initial Attach  */
  DSD_APN_TYPE_EMERGENCY_V01 = 9, /**<  APN type for emergency  */
  DSD_APN_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_apn_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  dsd_apn_type_enum_v01 apn_type;
  /**<   APN type.\n
 Values: \n
      - DSD_APN_TYPE_DEFAULT (0) --  APN type for Default/Internet traffic
      - DSD_APN_TYPE_IMS (1) --  APN type for IMS
      - DSD_APN_TYPE_MMS (2) --  APN type for Multimedia Messaging Service
      - DSD_APN_TYPE_DUN (3) --  APN type for Dial Up Network
      - DSD_APN_TYPE_SUPL (4) --  APN type for Secure User Plane Location
      - DSD_APN_TYPE_HIPRI (5) --  APN type for High Priority Mobile Data
      - DSD_APN_TYPE_FOTA (6) --  APN type for over the air administration
      - DSD_APN_TYPE_CBS (7) --  APN type for Carrier Branded Services
      - DSD_APN_TYPE_IA (8) --  APN type for Initial Attach
      - DSD_APN_TYPE_EMERGENCY (9) --  APN type for emergency
 */

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name.
   */
}dsd_apn_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the APN information. */
typedef struct {

  /* Mandatory */
  /*  APN Information */
  dsd_apn_info_type_v01 apn_info;

  /* Optional */
  /*  APN Invalid Flag */
  uint8_t apn_invalid_valid;  /**< Must be set to true if apn_invalid is being passed */
  uint8_t apn_invalid;
  /**<   Values: \n
       - 0 -- FALSE = APN is valid \n
       - 1 -- TRUE = APN is invalid \n
  */
}dsd_set_apn_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the APN information. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_apn_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Gets the APN information. */
typedef struct {

  /* Mandatory */
  /*  APN type enum */
  dsd_apn_type_enum_v01 apn_type;
  /**<   APN type enum. Values : \n
      - DSD_APN_TYPE_DEFAULT (0) --  APN type for Default/Internet traffic
      - DSD_APN_TYPE_IMS (1) --  APN type for IMS
      - DSD_APN_TYPE_MMS (2) --  APN type for Multimedia Messaging Service
      - DSD_APN_TYPE_DUN (3) --  APN type for Dial Up Network
      - DSD_APN_TYPE_SUPL (4) --  APN type for Secure User Plane Location
      - DSD_APN_TYPE_HIPRI (5) --  APN type for High Priority Mobile Data
      - DSD_APN_TYPE_FOTA (6) --  APN type for over the air administration
      - DSD_APN_TYPE_CBS (7) --  APN type for Carrier Branded Services
      - DSD_APN_TYPE_IA (8) --  APN type for Initial Attach
      - DSD_APN_TYPE_EMERGENCY (9) --  APN type for emergency
 */
}dsd_get_apn_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Gets the APN information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.     */

  /* Optional */
  /*  APN Name */
  uint8_t apn_name_valid;  /**< Must be set to true if apn_name is being passed */
  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN name.
   */
}dsd_get_apn_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_RAT_PREFERENCE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_RAT_PREFERENCE_INACTIVE_V01 = 0, /**<  RAT preference not set. \n  */
  DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01 = 1, /**<  Cellular only. \n  */
  DSD_RAT_PREFERENCE_WIFI_ONLY_V01 = 2, /**<  Wi-Fi only. \n  */
  DSD_RAT_PREFERENCE_CELLULAR_PREFERRED_V01 = 3, /**<  Cellular preferred. \n  */
  DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01 = 4, /**<  Wi-Fi preferred.  */
  DSD_RAT_PREFERENCE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_rat_preference_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Configures the data setting. */
typedef struct {

  /* Optional */
  /*  Data Service ON/OFF Switch */
  uint8_t data_service_switch_valid;  /**< Must be set to true if data_service_switch is being passed */
  uint8_t data_service_switch;
  /**<   Specifies whether data is ON/OFF \n
       Values: \n
       - 0 -- FALSE = Data Service is OFF \n
       - 1 -- TRUE = Data Service is ON
  */

  /* Optional */
  /*  Data Service Roaming ON/OFF Switch */
  uint8_t data_service_roaming_switch_valid;  /**< Must be set to true if data_service_roaming_switch is being passed */
  uint8_t data_service_roaming_switch;
  /**<   Specifies whether data roaming is ON/OFF. Values: \n
       - 0 -- FALSE = Data Service Roaming is OFF \n
       - 1 -- TRUE = Data Service Roaming is ON
  */

  /* Optional */
  /*  Radio Access Technology preference  */
  uint8_t rat_preference_valid;  /**< Must be set to true if rat_preference is being passed */
  dsd_rat_preference_enum_v01 rat_preference;
  /**<   Indicates RAT preference. \n
 Values: \n
      - DSD_RAT_PREFERENCE_INACTIVE (0) --  RAT preference not set. \n
      - DSD_RAT_PREFERENCE_CELLULAR_ONLY (1) --  Cellular only. \n
      - DSD_RAT_PREFERENCE_WIFI_ONLY (2) --  Wi-Fi only. \n
      - DSD_RAT_PREFERENCE_CELLULAR_PREFERRED (3) --  Cellular preferred. \n
      - DSD_RAT_PREFERENCE_WIFI_PREFERRED (4) --  Wi-Fi preferred.
 */

  /* Optional */
  /*  WIFI ON/OFF Switch */
  uint8_t wifi_switch_valid;  /**< Must be set to true if wifi_switch is being passed */
  uint8_t wifi_switch;
  /**<   Specifies whether wifi is ON/OFF. Values: \n
       - 0 -- FALSE = Wifi is OFF \n
       - 1 -- TRUE = Wifi is ON
  */
}dsd_notify_data_settings_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Configures the data setting. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_notify_data_settings_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the data settings. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_data_settings_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the data settings. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */

  /* Optional */
  /*  Data Service ON/OFF Switch */
  uint8_t data_service_switch_valid;  /**< Must be set to true if data_service_switch is being passed */
  uint8_t data_service_switch;
  /**<   Specifies whether data service is ON/OFF. \n
       Values: \n
       - 0 -- TRUE = Data Service is OFF \n
       - 1 -- TRUE = Data Service is ON
  */

  /* Optional */
  /*  Data Service Roaming ON/OFF Switch */
  uint8_t data_service_roaming_switch_valid;  /**< Must be set to true if data_service_roaming_switch is being passed */
  uint8_t data_service_roaming_switch;
  /**<   Specifies whether data roaming is ON/OFF. \n
       Values: \n
       - 0 -- TRUE = Data Service Roaming is OFF \n
       - 1 -- TRUE = Data Service Roaming is ON
  */

  /* Optional */
  /*  Radio Access Technology Preference  */
  uint8_t rat_preference_valid;  /**< Must be set to true if rat_preference is being passed */
  dsd_rat_preference_enum_v01 rat_preference;
  /**<   RAT preference \n
 Values: \n
      - DSD_RAT_PREFERENCE_INACTIVE (0) --  RAT preference not set. \n
      - DSD_RAT_PREFERENCE_CELLULAR_ONLY (1) --  Cellular only. \n
      - DSD_RAT_PREFERENCE_WIFI_ONLY (2) --  Wi-Fi only. \n
      - DSD_RAT_PREFERENCE_CELLULAR_PREFERRED (3) --  Cellular preferred. \n
      - DSD_RAT_PREFERENCE_WIFI_PREFERRED (4) --  Wi-Fi preferred.
 */

  /* Optional */
  /*  WIFI ON/OFF Switch */
  uint8_t wifi_switch_valid;  /**< Must be set to true if wifi_switch is being passed */
  uint8_t wifi_switch;
  /**<   Specifies whether wifi is ON/OFF. Values: \n
       - 0 -- FALSE = Wifi is OFF \n
       - 1 -- TRUE = Wifi is ON
  */
}dsd_get_data_settings_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_THERMAL_MITIGATION_ACTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_THERMAL_MITIGATION_ACTION_ALL_CALLS_ALLOWED_V01 = 0, /**<  All calls allowed \n  */
  DSD_THERMAL_MITIGATION_ACTION_IMS_CALLS_ONLY_V01 = 1, /**<  IMS calls only \n  */
  DSD_THERMAL_MITIGATION_ACTION_NO_CALLS_ALLOWED_V01 = 2, /**<  Data calls not allowed  */
  DSD_THERMAL_MITIGATION_ACTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_thermal_mitigation_action_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the thermal info changes. */
typedef struct {

  /* Optional */
  /*  Thermal Mitigation Info */
  uint8_t thermal_action_valid;  /**< Must be set to true if thermal_action is being passed */
  dsd_thermal_mitigation_action_enum_v01 thermal_action;
  /**<   Thermal mitigation action information.
      - DSD_THERMAL_MITIGATION_ACTION_ALL_CALLS_ALLOWED (0) --  All calls allowed \n
      - DSD_THERMAL_MITIGATION_ACTION_IMS_CALLS_ONLY (1) --  IMS calls only \n
      - DSD_THERMAL_MITIGATION_ACTION_NO_CALLS_ALLOWED (2) --  Data calls not allowed
 */
}dsd_thermal_info_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the current thermal mitigation information. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_thermal_mitigation_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the current thermal mitigation information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Thermal Mitigation Info */
  uint8_t thermal_action_valid;  /**< Must be set to true if thermal_action is being passed */
  dsd_thermal_mitigation_action_enum_v01 thermal_action;
  /**<   Thermal mitigation action information.
      - DSD_THERMAL_MITIGATION_ACTION_ALL_CALLS_ALLOWED (0) --  All calls allowed \n
      - DSD_THERMAL_MITIGATION_ACTION_IMS_CALLS_ONLY (1) --  IMS calls only \n
      - DSD_THERMAL_MITIGATION_ACTION_NO_CALLS_ALLOWED (2) --  Data calls not allowed
 */
}dsd_get_thermal_mitigation_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Registers the requesting control point for various indications. */
typedef struct {

  /* Optional */
  /*  Thermal Mitigation Info */
  uint8_t report_thermal_info_changes_valid;  /**< Must be set to true if report_thermal_info_changes is being passed */
  uint8_t report_thermal_info_changes;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report thermal info change.
  */

  /* Optional */
  /*  Wi-Fi Measurement Trigger */
  uint8_t report_wifi_meas_trigger_valid;  /**< Must be set to true if report_wifi_meas_trigger is being passed */
  uint8_t report_wifi_meas_trigger;
  /**<   Values: \n
	  - 0 -- Do not report. \n
	  - 1 -- Report Wi-Fi measurement trigger.
  */

  /* Optional */
  /*  WQE Profile Type Change  */
  uint8_t report_wqe_profile_type_changes_valid;  /**< Must be set to true if report_wqe_profile_type_changes is being passed */
  uint8_t report_wqe_profile_type_changes;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report WQE profile type change.
  */

  /* Optional */
  /*  WQE Profile Type Met  */
  uint8_t report_wqe_profile_met_valid;  /**< Must be set to true if report_wqe_profile_met is being passed */
  uint8_t report_wqe_profile_met;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report WQE profile type Met
             for the application.
  */

  /* Optional */
  /*  NAT Keep alive Info */
  uint8_t report_nat_keep_alive_info_valid;  /**< Must be set to true if report_nat_keep_alive_info is being passed */
  uint8_t report_nat_keep_alive_info;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report information for NAT Keep alive
             to the application.
  */

  /* Optional */
  /*  Roaming Status Change */
  uint8_t report_roaming_status_change_valid;  /**< Must be set to true if report_roaming_status_change is being passed */
  uint8_t report_roaming_status_change;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report information for Roaming Status change
             to the application.
    */

  /* Optional */
  /*  WWAN Connection State */
  uint8_t report_wwan_conn_state_type_valid;  /**< Must be set to true if report_wwan_conn_state_type is being passed */
  uint8_t report_wwan_conn_state_type;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report WWAN connection state to the application.
   */

  /* Optional */
  /*  Primary and Secondary cell information change for LTE */
  uint8_t report_lte_cell_info_change_valid;  /**< Must be set to true if report_lte_cell_info_change is being passed */
  uint8_t report_lte_cell_info_change;
  /**<   Values: \n
	  - 0 -- Do not report \n
	  - 1 -- Report information for change in primary and secondary
                 cell information of LTE RAT to the application.
    */

  /* Optional */
  /*  Report Current Default Data Subscription */
  uint8_t report_current_dds_valid;  /**< Must be set to true if report_current_dds is being passed */
  uint8_t report_current_dds;
  /**<    Values: \n.
	  - 0 -- Do not report \n
	  - 1 -- Report current DDS.
     */

  /* Optional */
  /*  Vacate Wifi band */
  uint8_t report_vacate_wifi_band_valid;  /**< Must be set to true if report_vacate_wifi_band is being passed */
  uint8_t report_vacate_wifi_band;
  /**<    Values: \n.
	  - 0 -- Do not report \n
	  - 1 -- Report Wifi band vacate request to TE.
     */
}dsd_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Registers the requesting control point for various indications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dsd_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bssid[QMI_DSD_MAC_ADDR_LEN_V01];
  /**<   WLAN access point MAC address.  */

  uint32_t channel_list_len;  /**< Must be set to # of elements in channel_list */
  uint16_t channel_list[QMI_DSD_MAX_CHANNELS_V01];
}dsd_wifi_ident_bssid_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t ssid_len;  /**< Must be set to # of elements in ssid */
  uint8_t ssid[QMI_DSD_MAX_SSID_LEN_V01];
  /**<   Service Set Identifier.  */

  uint8_t is_hidden;
  /**<   Specifies whether Service Set Identifier is hidden or not
       Values: \n
       - 0 -- FALSE = SSID is not hidden \n
       - 1 -- TRUE  = SSID is hidden \n
  */

  uint32_t bssid_list_len;  /**< Must be set to # of elements in bssid_list */
  dsd_wifi_ident_bssid_type_v01 bssid_list[QMI_DSD_MAX_BSSIDS_V01];
}dsd_wifi_ident_ssid_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  int16_t rssi_high;
  /**<   RSSI high threshold value for the channel. */

  int16_t rssi_low;
  /**<   RSSI low  threshold value for the channel. */
}dsd_wifi_ident_rssi_threshold_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  int16_t sinr_high;
  /**<   SINR high threshold value for the channel. */

  int16_t sinr_low;
  /**<   SINR low threshold value for the channel. */
}dsd_wifi_ident_sinr_threshold_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t station_count_high;
  /**<   Station count high threshold. */

  uint16_t station_count_low;
  /**<   Station count low threshold.  */

  uint16_t channel_utilization_high;
  /**<   Channel utilization high threshold.  */

  uint16_t channel_utilization_low;
  /**<   Channel utilization low threshold. */

  uint16_t available_admission_capacity_high;
  /**<   Available Admission Capacity high threshold. */

  uint16_t available_admission_capacity_low;
  /**<   Available Admission Capacity low threshold.  */
}dsd_wifi_ident_bss_load_threshold_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bssid[QMI_DSD_MAC_ADDR_LEN_V01];
  /**<   WLAN access point MAC address.  */
}dsd_wifi_blacklist_bssid_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_WLAN_FW_ACTION_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_WLAN_FW_ACTION_PNO_V01 = 0, /**<  Preferred Network Offload.
       WLAN firmware to provide measurement reports.   */
  DSD_WIFI_WLAN_FW_ACTION_PNO_ASSOCIATE_WN_V01 = 1, /**<  Preferred Network Offload for neighboring WLAN. This indicates TE to find a
       good neighbor and connect automatically. */
  DSD_WIFI_WLAN_FW_ACTION_LFR_V01 = 2, /**<  Legacy Fast Roaming.
       WLAN Firmware scans and reports measurements based on threshold.  */
  DSD_WIFI_WLAN_FW_ACTION_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_wlan_fw_action_enum_type_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates that the control point needs to start sending Wi-Fi measurements. */
typedef struct {

  /* Mandatory */
  /*  Wi-Fi Measurement ID */
  uint32_t wifi_meas_id;
  /**<   ID used to identify this particular start indication.
  */

  /* Mandatory */
  /*  Measurement Parameters */
  dsd_wifi_meas_param_mask_v01 meas_param;
  /**<   Bitmask of parameters for which measurements are to be captured. Values \n
      - QMI_DSD_WIFI_MEAS_PARAM_NONE (0x0000000000000000) --  No Parameters
      - QMI_DSD_WIFI_MEAS_PARAM_RSSI (0x0000000000000001) --  Received Signal Strength Indication
      - QMI_DSD_WIFI_MEAS_PARAM_SINR (0x0000000000000002) --  Signal to Inteference plus Noise Ratio
      - QMI_DSD_WIFI_MEAS_PARAM_BSS_LOAD (0x0000000000000004) --  Basic Service Set Load
      - QMI_DSD_WIFI_MEAS_PARAM_PHY_RATE (0x0000000000000008) --  Physical Layer Transmission Rate
      - QMI_DSD_WIFI_MEAS_PARAM_PACKET_ERROR_RATE (0x0000000000000010) --  Packet Error Rate
 */

  /* Mandatory */
  /*  Report Type */
  dsd_wifi_meas_report_type_enum_v01 report_type;
  /**<   Specifies the type of Wi-Fi measurement report to be captured. Values \n
      - DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_RAW (0) --  Report Raw Wi-Fi Measurements values periodically.
      - DSD_WIFI_MEAS_REPORT_TYPE_ONE_TIME_AFTER_THRESHOLD (1) --  Report Wi-Fi Measurements once after crossing the threshold.
      - DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_AFTER_THRESHOLD (2) --  Report Wi-Fi Measurements periodically after crossing the threshold.
 */

  /* Optional */
  /*  RSSI Threshold */
  uint8_t rssi_threshold_valid;  /**< Must be set to true if rssi_threshold is being passed */
  dsd_wifi_ident_rssi_threshold_type_v01 rssi_threshold;
  /**<   Contains the threshold values for received Signal Strength
       Indication value for the channel above or below which TE sends
       measurement reports.
  */

  /* Optional */
  /*  SINR Threshold */
  uint8_t sinr_threshold_valid;  /**< Must be set to true if sinr_threshold is being passed */
  dsd_wifi_ident_sinr_threshold_type_v01 sinr_threshold;
  /**<   Contains the threshold values for Signal Interference
       to noise ratio value for the channel above or below which TE sends
       measurement reports.
  */

  /* Optional */
  /*  BSS Load Threshold */
  uint8_t bss_load_threshold_valid;  /**< Must be set to true if bss_load_threshold is being passed */
  dsd_wifi_ident_bss_load_threshold_type_v01 bss_load_threshold;
  /**<   Contains the threshold values for all BSS load values above or
       below which TE sends measurement reports.
  */

  /* Optional */
  /*  Identifying Parameter List */
  uint8_t ident_param_list_valid;  /**< Must be set to true if ident_param_list is being passed */
  uint32_t ident_param_list_len;  /**< Must be set to # of elements in ident_param_list */
  dsd_wifi_ident_ssid_type_v01 ident_param_list[QMI_DSD_MAX_SSIDS_V01];
  /**<   Contains SSIDs, BSSIDs for each SSID and channel for each BSSID, for which
       measurements are desired.
  */

  /* Optional */
  /*  Sampling Timer */
  uint8_t sampling_timer_valid;  /**< Must be set to true if sampling_timer is being passed */
  uint32_t sampling_timer;
  /**<   Time period in milliseconds after which all measurements are to be sampled.
       If not specified, default value configured on the TE will be used.
  */

  /* Optional */
  /*  Report Interval */
  uint8_t report_interval_valid;  /**< Must be set to true if report_interval is being passed */
  uint32_t report_interval;
  /**<   Interval of time in milliseconds after which measurements are to be
       sent from TE. If not specified, default value configured on the TE
       will be used.
  */

  /* Optional */
  /*  Alpha */
  uint8_t alpha_valid;  /**< Must be set to true if alpha is being passed */
  float alpha;
  /**<   Coefficient used to calculate the average. If not specified, default value
       configured on the TE will be used.
  */

  /* Optional */
  /*  Time To Trigger */
  uint8_t time_to_trigger_valid;  /**< Must be set to true if time_to_trigger is being passed */
  uint64_t time_to_trigger;
  /**<   Specifies the amount of time in milliseconds, the TE has to wait before
       sending out the Wi-Fi measurement reports, once the threshold is hit.
       If not specified, the TE will send reports whenever the threshold
       conditions are met */

  /* Optional */
  /*  Blacklisted BSSID list */
  uint8_t blacklisted_bssids_valid;  /**< Must be set to true if blacklisted_bssids is being passed */
  uint32_t blacklisted_bssids_len;  /**< Must be set to # of elements in blacklisted_bssids */
  dsd_wifi_blacklist_bssid_type_v01 blacklisted_bssids[QMI_DSD_MAX_BSSIDS_V01];
  /**<   Contains the list of BSSIDs which are blacklisted.
  */

  /* Optional */
  /*  WLAN Firmware Action */
  uint8_t wlan_fw_action_valid;  /**< Must be set to true if wlan_fw_action is being passed */
  dsd_wifi_wlan_fw_action_enum_type_v01 wlan_fw_action;
  /**<   Indicates the action that needs to be taken by the WLAN firmware
 for Wi-Fi measurements.
 Values: \n
      - DSD_WIFI_WLAN_FW_ACTION_PNO (0) --  Preferred Network Offload.
       WLAN firmware to provide measurement reports.
      - DSD_WIFI_WLAN_FW_ACTION_PNO_ASSOCIATE_WN (1) --  Preferred Network Offload for neighboring WLAN. This indicates TE to find a
       good neighbor and connect automatically.
      - DSD_WIFI_WLAN_FW_ACTION_LFR (2) --  Legacy Fast Roaming.
       WLAN Firmware scans and reports measurements based on threshold.
 */
}dsd_start_wifi_meas_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates that the control point needs to stop sending Wi-Fi measurements. */
typedef struct {

  /* Mandatory */
  /*  Wi-Fi Measurement ID */
  uint32_t wifi_meas_id;
  /**<   ID used to link it to previous start indication.
   */
}dsd_stop_wifi_meas_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_MEAS_REPORT_CONFIG_OK_V01 = 0, /**<  Configuration OK \n  */
  DSD_WIFI_MEAS_REPORT_CONFIG_ERROR_V01 = 1, /**<  Configuration error \n  */
  DSD_WIFI_MEAS_REPORT_STOPPED_V01 = 2, /**<  Report Stopped  */
  DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_meas_report_config_status_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Status regarding handling the Wi-Fi measurement report. */
typedef struct {

  /* Mandatory */
  /*  Wi-Fi Measurement ID */
  uint32_t wifi_meas_id;
  /**<   ID provided in QMI_DSD_START_WIFI_MEAS_IND.
  */

  /* Mandatory */
  /*  Wi-Fi Report Configuration Status */
  dsd_wifi_meas_report_config_status_enum_v01 status;
  /**<   TE status regarding Wi-Fi report configuration. Values :\n
      - DSD_WIFI_MEAS_REPORT_CONFIG_OK (0) --  Configuration OK \n
      - DSD_WIFI_MEAS_REPORT_CONFIG_ERROR (1) --  Configuration error \n
      - DSD_WIFI_MEAS_REPORT_STOPPED (2) --  Report Stopped
 */
}dsd_wifi_meas_report_config_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Status regarding handling the Wi-Fi measurement report. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dsd_wifi_meas_report_config_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t station_count;
  /**<   Station count. */

  uint16_t channel_utilization;
  /**<   Channel utilization. */

  uint16_t available_admission_capacity;
  /**<   Available Admission Capacity. */
}dsd_wifi_bss_load_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_MEAS_THRESHOLD_STATE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_MEAS_THRESHOLD_NOT_CROSSED_V01 = 0, /**<  Threshold not crossed.  */
  DSD_WIFI_MEAS_THRESHOLD_HIGH_CROSSED_V01 = 1, /**<  High Threshold crossed.  */
  DSD_WIFI_MEAS_THRESHOLD_LOW_CROSSED_V01 = 2, /**<  Low Threshold crossed.  */
  DSD_WIFI_MEAS_THRESHOLD_STATE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_meas_threshold_state_type_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t downlink_phy_rate;
  /**<   Downlink Data Rate of Physical layer. */

  uint16_t uplink_phy_rate;
  /**<   Uplink Data Rate of Physical layer. */
}dsd_wifi_phy_rate_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t downlink_packet_error_rate;
  /**<   Downlink Packet Error Rate. */

  uint16_t uplink_packet_error_rate;
  /**<   Uplink Packet Error Rate. */
}dsd_wifi_packet_error_rate_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t channel;
  /**<   Channel in MHz.  */

  dsd_wifi_meas_threshold_state_type_enum_v01 threshold_state;
  /**<   Value specify the state of the threshold. Values \n
      - DSD_WIFI_MEAS_THRESHOLD_NOT_CROSSED (0) --  Threshold not crossed.
      - DSD_WIFI_MEAS_THRESHOLD_HIGH_CROSSED (1) --  High Threshold crossed.
      - DSD_WIFI_MEAS_THRESHOLD_LOW_CROSSED (2) --  Low Threshold crossed.
 */

  dsd_wifi_meas_param_mask_v01 valid_params;
  /**<   Mask of valid parameters
      - QMI_DSD_WIFI_MEAS_PARAM_NONE (0x0000000000000000) --  No Parameters
      - QMI_DSD_WIFI_MEAS_PARAM_RSSI (0x0000000000000001) --  Received Signal Strength Indication
      - QMI_DSD_WIFI_MEAS_PARAM_SINR (0x0000000000000002) --  Signal to Inteference plus Noise Ratio
      - QMI_DSD_WIFI_MEAS_PARAM_BSS_LOAD (0x0000000000000004) --  Basic Service Set Load
      - QMI_DSD_WIFI_MEAS_PARAM_PHY_RATE (0x0000000000000008) --  Physical Layer Transmission Rate
      - QMI_DSD_WIFI_MEAS_PARAM_PACKET_ERROR_RATE (0x0000000000000010) --  Packet Error Rate
 */

  int16_t rssi;
  /**<   Received Signal Strength Indication value for the channel
  */

  int16_t sinr;
  /**<   Signal to Inteference plus Noise Ratio value for the channel
  */

  dsd_wifi_bss_load_type_v01 bss_load;
  /**<   Basic Service Set Load value for the channel
  */

  dsd_wifi_phy_rate_type_v01 phy_rate;
  /**<   Physical layer Transmission rate for the given BSSID
  */

  dsd_wifi_packet_error_rate_type_v01 packet_error_rate;
  /**<   Packet Error Ratio for the given BSSID
  */

  dsd_wifi_network_mode_type_enum_v01 network_mode;
  /**<   ENUM specifying the network mode for the channel. Values \n
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_1997 (0) --  Legacy network mode.
      - QMI_DSD_WIFI_NETWORK_MODE_802_11a (1) --  OFDM network mode.
      - QMI_DSD_WIFI_NETWORK_MODE_802_11b (2) --  802.11b
      - QMI_DSD_WIFI_NETWORK_MODE_802_11g (3) --  802.11g
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_2007 (4) --  802.11.2007
      - QMI_DSD_WIFI_NETWORK_MODE_802_11n (5) --  802.11n
      - QMI_DSD_WIFI_NETWORK_MODE_802_11_2012 (6) --  802.11.2012
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ac (7) --  802.11.ac
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ad (8) --  802.11.ad
      - QMI_DSD_WIFI_NETWORK_MODE_802_11af (9) --  802.11.af
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ah (10) --  802.11.ah
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ai (11) --  802.11.ai
      - QMI_DSD_WIFI_NETWORK_MODE_802_11aj (12) --  802.11.aj
      - QMI_DSD_WIFI_NETWORK_MODE_802_11aq (13) --  802.11.aq
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ax (14) --  802.11.ax
      - QMI_DSD_WIFI_NETWORK_MODE_802_11ay (15) --  802.11.ay
 */
}dsd_wifi_meas_info_per_channel_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bssid[QMI_DSD_MAC_ADDR_LEN_V01];
  /**<   Basic Service Set Identifier.
  */

  dsd_wifi_meas_info_per_channel_type_v01 channel_info;
  /**<   Wi-Fi measurement information for a channel.
  */
}dsd_wifi_meas_info_per_bssid_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t secondary_channel;
  /**<   Secondary Channel in MHz.  */

  dsd_wifi_bandwidth_type_enum_v01 bandwidth_type;
  /**<   Bandwidth type per BSSID.
  */
}dsd_wifi_meas_secondary_bssid_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Report sent by TE with Wi-Fi measurements. */
typedef struct {

  /* Mandatory */
  /*  Wi-Fi Measurement ID */
  uint32_t wifi_meas_id;
  /**<   ID provided in QMI_DSD_START_WIFI_MEAS_IND.
  */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t tx_id;
  /**<   Since Wi-Fi measurements are sent per SSID and there could be multiple SSIDs
       for which information needs to be sent, or more than QMI_DSD_MAX_BSSIDS for
       a SSID, the information will be split into multiple
       QMI_DSD_WIFI_MEAS_REPORT_REQ messages, all of them identified by the
       same transaction id.
  */

  /* Mandatory */
  /*  Report Type */
  dsd_wifi_meas_report_type_enum_v01 report_type;
  /**<   Specifies the type of Wi-Fi measurement report that was specified in the
 QMI_DSD_START_WIFI_MEAS_IND. Values \n
      - DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_RAW (0) --  Report Raw Wi-Fi Measurements values periodically.
      - DSD_WIFI_MEAS_REPORT_TYPE_ONE_TIME_AFTER_THRESHOLD (1) --  Report Wi-Fi Measurements once after crossing the threshold.
      - DSD_WIFI_MEAS_REPORT_TYPE_PERIODIC_AFTER_THRESHOLD (2) --  Report Wi-Fi Measurements periodically after crossing the threshold.
 */

  /* Mandatory */
  /*  Is Last Report Of Transaction */
  uint8_t is_last_report_of_transaction;
  /**<   If there are multiple QMI_DSD_WIFI_MEAS_REPORT_REQ messages being sent
       for the transaction, this TLV identifies if there are more to follow.
       Values :\n
       - 0 -- More to follow \n
	   - 1 -- Is the last report of the transaction.
  */

  /* Mandatory */
  /*  Service Set Identifier */
  uint32_t ssid_len;  /**< Must be set to # of elements in ssid */
  uint8_t ssid[QMI_DSD_MAX_SSID_LEN_V01];
  /**<   Service Set Identifier.
  */

  /* Optional */
  /*  Per BSSID Info */
  uint8_t per_bssid_info_valid;  /**< Must be set to true if per_bssid_info is being passed */
  uint32_t per_bssid_info_len;  /**< Must be set to # of elements in per_bssid_info */
  dsd_wifi_meas_info_per_bssid_type_v01 per_bssid_info[QMI_DSD_MAX_BSSIDS_PER_REPORT_V01];
  /**<   Wi-Fi measurement information for each BSSID.
  */

  /* Optional */
  /*  Secondary BSSID Info */
  uint8_t sec_bssid_info_valid;  /**< Must be set to true if sec_bssid_info is being passed */
  uint32_t sec_bssid_info_len;  /**< Must be set to # of elements in sec_bssid_info */
  dsd_wifi_meas_secondary_bssid_info_type_v01 sec_bssid_info[QMI_DSD_MAX_BSSIDS_PER_REPORT_V01];
  /**<   Secondary Wi-Fi measurement information for each BSSID. The information provided in
       this TLV corresponds to the BSSID information in the per_bssid_info TLV.
  */
}dsd_wifi_meas_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Report sent by TE with Wi-Fi measurements. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dsd_wifi_meas_report_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t dsd_apn_call_type_mask_v01;
#define QMI_DSD_APN_CALL_TYPE_IDLE_V01 ((dsd_apn_call_type_mask_v01)0x0000000000000000ull) /**<  Idle  */
#define QMI_DSD_APN_CALL_TYPE_GENERAL_V01 ((dsd_apn_call_type_mask_v01)0x0000000000000001ull) /**<  General  */
#define QMI_DSD_APN_CALL_TYPE_VOICE_V01 ((dsd_apn_call_type_mask_v01)0x0000000000000002ull) /**<  Voice call  */
#define QMI_DSD_APN_CALL_TYPE_VIDEO_V01 ((dsd_apn_call_type_mask_v01)0x0000000000000004ull) /**<  Video call  */
/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the highest technology preference for an APN. */
typedef struct {

  /* Mandatory */
  /*  APN */
  char apn[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN \n
  */

  /* Mandatory */
  /*  APN Call Type */
  dsd_apn_call_type_mask_v01 apn_call_type;
  /**<   APN Call Type. Values \n:
      - QMI_DSD_APN_CALL_TYPE_IDLE (0x0000000000000000) --  Idle
      - QMI_DSD_APN_CALL_TYPE_GENERAL (0x0000000000000001) --  General
      - QMI_DSD_APN_CALL_TYPE_VOICE (0x0000000000000002) --  Voice call
      - QMI_DSD_APN_CALL_TYPE_VIDEO (0x0000000000000004) --  Video call
 */
}dsd_set_apn_call_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the highest technology preference for an APN. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_apn_call_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the Wifi Quality estimation profile type. */
typedef struct {

  /* Mandatory */
  /*  WQE Profile Type */
  dsd_wqe_profile_type_mask_v01 wqe_profile_type;
  /**<   Conveys if WQE is to be performed for the Application
 for audio, video or internet data. Values: \n
      - QMI_DSD_WQE_PROFILE_INACTIVE (0x0000000000000000) --  Wi-Fi Quality Estimation profile inactive
      - QMI_DSD_WQE_PROFILE_INTERNET (0x0000000000000001) --  Wi-Fi Quality Estimation profile internet
      - QMI_DSD_WQE_PROFILE_AUDIO (0x0000000000000002) --  Wi-Fi Quality Estimation profile audio
      - QMI_DSD_WQE_PROFILE_VIDEO (0x0000000000000004) --  Wi-Fi Quality Estimation profile video
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_1 (0x0000000100000000) --  Wi-Fi Quality Estimation profile for IMS Type 1
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_2 (0x0000000200000000) --  Wi-Fi Quality Estimation profile for IMS Type 2
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_3 (0x0000000400000000) --  Wi-Fi Quality Estimation profile for IMS Type 3
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_4 (0x0000000800000000) --  Wi-Fi Quality Estimation profile for IMS Type 4
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_5 (0x0000001000000000) --  Wi-Fi Quality Estimation profile for IMS Type 5
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_6 (0x0000002000000000) --  Wi-Fi Quality Estimation profile for IMS Type 6
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_7 (0x0000004000000000) --  Wi-Fi Quality Estimation profile for IMS Type 7
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_8 (0x0000008000000000) --  Wi-Fi Quality Estimation profile for IMS Type 8
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_9 (0x0000010000000000) --  Wi-Fi Quality Estimation profile for IMS Type 9
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_10 (0x0000020000000000) --  Wi-Fi Quality Estimation profile for IMS Type 10
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_11 (0x0000040000000000) --  Wi-Fi Quality Estimation profile for IMS Type 11
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_12 (0x0000080000000000) --  Wi-Fi Quality Estimation profile for IMS Type 12
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_13 (0x0000100000000000) --  Wi-Fi Quality Estimation profile for IMS Type 13
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_14 (0x0000200000000000) --  Wi-Fi Quality Estimation profile for IMS Type 14
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_15 (0x0000400000000000) --  Wi-Fi Quality Estimation profile for IMS Type 15
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_16 (0x0000800000000000) --  Wi-Fi Quality Estimation profile for IMS Type 16
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_17 (0x0001000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 17
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_18 (0x0002000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 18
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_19 (0x0004000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 19
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_20 (0x0008000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 20
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_21 (0x0010000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 21
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_22 (0x0020000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 22
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_23 (0x0040000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 23
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_24 (0x0080000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 24
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_25 (0x0100000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 25
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_26 (0x0200000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 26
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_27 (0x0400000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 27
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_28 (0x0800000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 28
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_29 (0x1000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 29
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_30 (0x2000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 30
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_31 (0x4000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 31
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_32 (0x8000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 32 ;
 */
}dsd_set_wqe_profile_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the Wifi Quality estimation profile type. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_wqe_profile_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the control point about the WQE Profile type set on the modem. */
typedef struct {

  /* Mandatory */
  /*  WQE Profile Type */
  dsd_wqe_profile_type_mask_v01 set_wqe_profile_type;
  /**<   Mask indicating the WQE profile type set on the modem. Values \n.
      - QMI_DSD_WQE_PROFILE_INACTIVE (0x0000000000000000) --  Wi-Fi Quality Estimation profile inactive
      - QMI_DSD_WQE_PROFILE_INTERNET (0x0000000000000001) --  Wi-Fi Quality Estimation profile internet
      - QMI_DSD_WQE_PROFILE_AUDIO (0x0000000000000002) --  Wi-Fi Quality Estimation profile audio
      - QMI_DSD_WQE_PROFILE_VIDEO (0x0000000000000004) --  Wi-Fi Quality Estimation profile video
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_1 (0x0000000100000000) --  Wi-Fi Quality Estimation profile for IMS Type 1
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_2 (0x0000000200000000) --  Wi-Fi Quality Estimation profile for IMS Type 2
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_3 (0x0000000400000000) --  Wi-Fi Quality Estimation profile for IMS Type 3
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_4 (0x0000000800000000) --  Wi-Fi Quality Estimation profile for IMS Type 4
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_5 (0x0000001000000000) --  Wi-Fi Quality Estimation profile for IMS Type 5
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_6 (0x0000002000000000) --  Wi-Fi Quality Estimation profile for IMS Type 6
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_7 (0x0000004000000000) --  Wi-Fi Quality Estimation profile for IMS Type 7
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_8 (0x0000008000000000) --  Wi-Fi Quality Estimation profile for IMS Type 8
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_9 (0x0000010000000000) --  Wi-Fi Quality Estimation profile for IMS Type 9
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_10 (0x0000020000000000) --  Wi-Fi Quality Estimation profile for IMS Type 10
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_11 (0x0000040000000000) --  Wi-Fi Quality Estimation profile for IMS Type 11
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_12 (0x0000080000000000) --  Wi-Fi Quality Estimation profile for IMS Type 12
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_13 (0x0000100000000000) --  Wi-Fi Quality Estimation profile for IMS Type 13
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_14 (0x0000200000000000) --  Wi-Fi Quality Estimation profile for IMS Type 14
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_15 (0x0000400000000000) --  Wi-Fi Quality Estimation profile for IMS Type 15
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_16 (0x0000800000000000) --  Wi-Fi Quality Estimation profile for IMS Type 16
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_17 (0x0001000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 17
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_18 (0x0002000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 18
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_19 (0x0004000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 19
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_20 (0x0008000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 20
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_21 (0x0010000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 21
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_22 (0x0020000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 22
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_23 (0x0040000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 23
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_24 (0x0080000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 24
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_25 (0x0100000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 25
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_26 (0x0200000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 26
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_27 (0x0400000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 27
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_28 (0x0800000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 28
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_29 (0x1000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 29
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_30 (0x2000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 30
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_31 (0x4000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 31
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_32 (0x8000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 32 ;
 */

  /* Optional */
  /*  ICCID */
  uint8_t iccid_valid;  /**< Must be set to true if iccid is being passed */
  uint32_t iccid_len;  /**< Must be set to # of elements in iccid */
  uint8_t iccid[QMI_DSD_MAX_ICCID_LEN_V01];
  /**<   BCD encoded packed array representing the integrated circuit card identifier,
        stored in the sim card. The TE can use the information provided by this
        field, to determine the operator for which the new WQE profile was set.
     */
}dsd_wqe_profile_type_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the control point about the WQE Profile type which was met
           on the modem. */
typedef struct {

  /* Mandatory */
  /*  WQE Profile Type */
  dsd_wqe_profile_type_mask_v01 met_wqe_profile_type;
  /**<   Mask indicating the WQE profile type that was used/met for the application.
 Values \n.
      - QMI_DSD_WQE_PROFILE_INACTIVE (0x0000000000000000) --  Wi-Fi Quality Estimation profile inactive
      - QMI_DSD_WQE_PROFILE_INTERNET (0x0000000000000001) --  Wi-Fi Quality Estimation profile internet
      - QMI_DSD_WQE_PROFILE_AUDIO (0x0000000000000002) --  Wi-Fi Quality Estimation profile audio
      - QMI_DSD_WQE_PROFILE_VIDEO (0x0000000000000004) --  Wi-Fi Quality Estimation profile video
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_1 (0x0000000100000000) --  Wi-Fi Quality Estimation profile for IMS Type 1
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_2 (0x0000000200000000) --  Wi-Fi Quality Estimation profile for IMS Type 2
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_3 (0x0000000400000000) --  Wi-Fi Quality Estimation profile for IMS Type 3
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_4 (0x0000000800000000) --  Wi-Fi Quality Estimation profile for IMS Type 4
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_5 (0x0000001000000000) --  Wi-Fi Quality Estimation profile for IMS Type 5
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_6 (0x0000002000000000) --  Wi-Fi Quality Estimation profile for IMS Type 6
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_7 (0x0000004000000000) --  Wi-Fi Quality Estimation profile for IMS Type 7
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_8 (0x0000008000000000) --  Wi-Fi Quality Estimation profile for IMS Type 8
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_9 (0x0000010000000000) --  Wi-Fi Quality Estimation profile for IMS Type 9
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_10 (0x0000020000000000) --  Wi-Fi Quality Estimation profile for IMS Type 10
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_11 (0x0000040000000000) --  Wi-Fi Quality Estimation profile for IMS Type 11
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_12 (0x0000080000000000) --  Wi-Fi Quality Estimation profile for IMS Type 12
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_13 (0x0000100000000000) --  Wi-Fi Quality Estimation profile for IMS Type 13
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_14 (0x0000200000000000) --  Wi-Fi Quality Estimation profile for IMS Type 14
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_15 (0x0000400000000000) --  Wi-Fi Quality Estimation profile for IMS Type 15
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_16 (0x0000800000000000) --  Wi-Fi Quality Estimation profile for IMS Type 16
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_17 (0x0001000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 17
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_18 (0x0002000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 18
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_19 (0x0004000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 19
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_20 (0x0008000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 20
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_21 (0x0010000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 21
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_22 (0x0020000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 22
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_23 (0x0040000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 23
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_24 (0x0080000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 24
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_25 (0x0100000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 25
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_26 (0x0200000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 26
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_27 (0x0400000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 27
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_28 (0x0800000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 28
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_29 (0x1000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 29
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_30 (0x2000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 30
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_31 (0x4000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 31
      - QMI_DSD_WQE_PROFILE_IMS_TYPE_32 (0x8000000000000000) --  Wi-Fi Quality Estimation profile for IMS Type 32 ;
 */
}dsd_wqe_profile_type_met_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Configures the TE for NAT keep alive transmissions */
typedef struct {

  /* Mandatory */
  /*  Trigger NAT Keep Alive  */
  uint8_t trigger_nat_keep_alive;
  /**<   Boolean to indicate whether NAT keep alive should be sent or not
        from TE.
        Values: \n
       - 0 -- FALSE = Do not trigger NAT keep alive  \n
       - 1 -- TRUE  = Trigger NAT keep alive */

  /* Optional */
  /*  Destination IPV4 Address Type */
  uint8_t dest_v4_address_valid;  /**< Must be set to true if dest_v4_address is being passed */
  uint32_t dest_v4_address;
  /**<   IPV4 Destination address information
        in little endian format*/

  /* Optional */
  /*  Destination IPV6 Address Type */
  uint8_t dest_ipv6_address_valid;  /**< Must be set to true if dest_ipv6_address is being passed */
  uint8_t dest_ipv6_address[QMI_DSD_IPV6_ADDR_LEN_V01];
  /**<   IPV6 destination address in network byte
       order; an 8-element array of 16-bit
       numbers, each of which is in big-endian
       format. */

  /* Optional */
  /*  Destination Port */
  uint8_t dest_port_valid;  /**< Must be set to true if dest_port is being passed */
  uint16_t dest_port;
  /**<   Destination port information */

  /* Optional */
  /*  Source Port */
  uint8_t source_port_valid;  /**< Must be set to true if source_port is being passed */
  uint16_t source_port;
  /**<   Source port information */

  /* Optional */
  /*  Timer Value */
  uint8_t timer_value_valid;  /**< Must be set to true if timer_value is being passed */
  uint32_t timer_value;
  /**<   Timer value in milliseconds to indicate the frequency of
        NAT Keep alive message that needs to be sent from TE*/
}dsd_nat_keep_alive_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_NAT_KEEP_ALIVE_CONFIG_OK_V01 = 0, /**<  Configuration OK \n  */
  DSD_NAT_KEEP_ALIVE_CONFIG_ERROR_V01 = 1, /**<  Configuration error \n  */
  DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_nat_keep_alive_config_status_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Used by TE to signal whether it is ok with the
             previously sent Nat keep alive info */
typedef struct {

  /* Mandatory */
  /*  Config Status */
  dsd_nat_keep_alive_config_status_enum_v01 status;
  /**<   Conveys if the NAT keep alive information indication was received by the TE and
 was configured successfully on the TE. Values:\n
      - DSD_NAT_KEEP_ALIVE_CONFIG_OK (0) --  Configuration OK \n
      - DSD_NAT_KEEP_ALIVE_CONFIG_ERROR (1) --  Configuration error \n
 */
}dsd_nat_keep_alive_config_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Used by TE to signal whether it is ok with the
             previously sent Nat keep alive info */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_nat_keep_alive_config_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WQE_PROFILE_QUALITY_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_DSD_WQE_PROFILE_QUALITY_MET_V01 = 0, /**<  WQE Quality status was met. */
  QMI_DSD_WQE_PROFILE_QUALITY_NOT_MET_V01 = 1, /**<  WQE Quality status not met. */
  QMI_DSD_WQE_PROFILE_QUALITY_INTERMEDIATE_RANGE_V01 = 2, /**<  WQE Quality status intermediate.  */
  QMI_DSD_WQE_PROFILE_QUALITY_UNKNOWN_V01 = 3, /**<  WQE Quality status unknown.  */
  DSD_WQE_PROFILE_QUALITY_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wqe_profile_quality_status_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WQE_PROFILE_QUALITY_STATUS_CAUSE_CODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_NONE_V01 = 0, /**<  WQE quality status code none. */
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_RSSI_V01 = 1, /**<  WQE Quality status rssi not met. */
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_MAC_V01 = 2, /**<  WQE Quality status mac metrics not met. */
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_INCONCLUSIVE_V01 = 3, /**<  WQE Quality status inconclusive */
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_BQ_FAIL_V01 = 4, /**<  WQE Quality status for degraded backhaul quality. */
  QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_INTERNET_CONNECTIVITY_FAIL_V01 = 5, /**<  WQE Quality status code internet connectivity fail. */
  DSD_WQE_PROFILE_QUALITY_STATUS_CAUSE_CODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wqe_profile_quality_status_cause_code_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Used by the TE to send the quality status of WQE profile */
typedef struct {

  /* Mandatory */
  /*  WQE Profile */
  dsd_wqe_profile_type_enum_v01 wqe_profile;
  /**<   The WQE profile for which the TE sends the quality status.
  */

  /* Optional */
  /*  WQE Profile Status */
  uint8_t quality_status_valid;  /**< Must be set to true if quality_status is being passed */
  dsd_wqe_profile_quality_status_enum_v01 quality_status;
  /**<   Specifies the status.\n
 Values: \n
      - QMI_DSD_WQE_PROFILE_QUALITY_MET (0) --  WQE Quality status was met.
      - QMI_DSD_WQE_PROFILE_QUALITY_NOT_MET (1) --  WQE Quality status not met.
      - QMI_DSD_WQE_PROFILE_QUALITY_INTERMEDIATE_RANGE (2) --  WQE Quality status intermediate.
      - QMI_DSD_WQE_PROFILE_QUALITY_UNKNOWN (3) --  WQE Quality status unknown.
 */

  /* Optional */
  /*  WQE Profile Status */
  uint8_t status_code_valid;  /**< Must be set to true if status_code is being passed */
  dsd_wqe_profile_quality_status_cause_code_enum_v01 status_code;
  /**<   Specifies the status code.\n
 Values: \n
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_NONE (0) --  WQE quality status code none.
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_RSSI (1) --  WQE Quality status rssi not met.
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_MAC (2) --  WQE Quality status mac metrics not met.
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_CQ_FAIL_INCONCLUSIVE (3) --  WQE Quality status inconclusive
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_BQ_FAIL (4) --  WQE Quality status for degraded backhaul quality.
      - QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_INTERNET_CONNECTIVITY_FAIL (5) --  WQE Quality status code internet connectivity fail.
 */
}dsd_set_wqe_profile_quality_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Used by the TE to send the quality status of WQE profile */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_wqe_profile_quality_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Used by the TE to signal the quality measurement
    information */
typedef struct {

  /* Optional */
  /*  In call LTE Low Threshold */
  uint8_t in_call_lte_rsrp_low_valid;  /**< Must be set to true if in_call_lte_rsrp_low is being passed */
  int16_t in_call_lte_rsrp_low;
  /**<   Conveys the Reference Signal Received Power low threshold value to be used
       during a call for LTE which is configured by TE.
  */

  /* Optional */
  /*  In call LTE Mid Threshold */
  uint8_t in_call_lte_rsrp_mid_valid;  /**< Must be set to true if in_call_lte_rsrp_mid is being passed */
  int16_t in_call_lte_rsrp_mid;
  /**<   Conveys the Reference Signal Received Power mid threshold value to be used
       during a call for LTE which is configured by TE.
  */

  /* Optional */
  /*  In call LTE High Threshold */
  uint8_t in_call_lte_rsrp_high_valid;  /**< Must be set to true if in_call_lte_rsrp_high is being passed */
  int16_t in_call_lte_rsrp_high;
  /**<   Conveys the Reference Signal Received Power high threshold value to be used
       during a call for LTE which is configured by TE.
  */

  /* Optional */
  /*  In call Wifi RSSI Low threshold  */
  uint8_t in_call_wifi_rssi_threshold_low_valid;  /**< Must be set to true if in_call_wifi_rssi_threshold_low is being passed */
  int16_t in_call_wifi_rssi_threshold_low;
  /**<   Conveys the RSSI low threshold parameter to be used during a call for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  In call Wifi RSSI High threshold  */
  uint8_t in_call_wifi_rssi_threshold_high_valid;  /**< Must be set to true if in_call_wifi_rssi_threshold_high is being passed */
  int16_t in_call_wifi_rssi_threshold_high;
  /**<   Conveys the RSSI high threshold parameter to be used during a call for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  In call Wifi SINR Low threshold  */
  uint8_t in_call_wifi_sinr_threshold_low_valid;  /**< Must be set to true if in_call_wifi_sinr_threshold_low is being passed */
  int16_t in_call_wifi_sinr_threshold_low;
  /**<   Conveys the SINR low threshold parameter to be used during a call for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  In call Wifi SINR High threshold  */
  uint8_t in_call_wifi_sinr_threshold_high_valid;  /**< Must be set to true if in_call_wifi_sinr_threshold_high is being passed */
  int16_t in_call_wifi_sinr_threshold_high;
  /**<   Conveys the SINR high threshold parameter to be used during a call for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  Idle LTE Threshold Low  */
  uint8_t idle_lte_rsrp_low_valid;  /**< Must be set to true if idle_lte_rsrp_low is being passed */
  int16_t idle_lte_rsrp_low;
  /**<   Conveys the Reference Signal Received Power low threshold parameter to be used
       when in idle state for LTE which is configured by TE.
  */

  /* Optional */
  /*  Idle LTE Threshold Mid  */
  uint8_t idle_lte_rsrp_mid_valid;  /**< Must be set to true if idle_lte_rsrp_mid is being passed */
  int16_t idle_lte_rsrp_mid;
  /**<   Conveys the Reference Signal Received Power mid threshold parameter to be used
       when in idle state for LTE which is configured by TE.
  */

  /* Optional */
  /*  Idle LTE Threshold High  */
  uint8_t idle_lte_rsrp_high_valid;  /**< Must be set to true if idle_lte_rsrp_high is being passed */
  int16_t idle_lte_rsrp_high;
  /**<   Conveys the Reference Signal Received Power high threshold parameter to be used
       when in idle state for LTE which is configured by TE.
  */

  /* Optional */
  /*  Idle Wifi RSSI threshold Low  */
  uint8_t idle_wifi_rssi_threshold_low_valid;  /**< Must be set to true if idle_wifi_rssi_threshold_low is being passed */
  int16_t idle_wifi_rssi_threshold_low;
  /**<   Conveys the RSSI low threshold parameter to be used when in idle state for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  Idle Wifi RSSI threshold High  */
  uint8_t idle_wifi_rssi_threshold_high_valid;  /**< Must be set to true if idle_wifi_rssi_threshold_high is being passed */
  int16_t idle_wifi_rssi_threshold_high;
  /**<   Conveys the RSSI high threshold parameter to be used when in idle state for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  Idle Wifi SINR threshold Low  */
  uint8_t idle_wifi_sinr_threshold_low_valid;  /**< Must be set to true if idle_wifi_sinr_threshold_low is being passed */
  int16_t idle_wifi_sinr_threshold_low;
  /**<   Conveys the SINR low threshold parameter to be used when in idle state for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  Idle Wifi SINR threshold High  */
  uint8_t idle_wifi_sinr_threshold_high_valid;  /**< Must be set to true if idle_wifi_sinr_threshold_high is being passed */
  int16_t idle_wifi_sinr_threshold_high;
  /**<   Conveys the SINR high threshold parameter to be used when in idle state for WLAN
       which is configured by TE.
  */

  /* Optional */
  /*  1x threshold Low  */
  uint8_t ecio_1x_threshold_low_valid;  /**< Must be set to true if ecio_1x_threshold_low is being passed */
  int16_t ecio_1x_threshold_low;
  /**<   Conveys the ecio low threshold parameter to be used for 1x which is configured by TE.
  */

  /* Optional */
  /*  1x threshold High  */
  uint8_t ecio_1x_threshold_high_valid;  /**< Must be set to true if ecio_1x_threshold_high is being passed */
  int16_t ecio_1x_threshold_high;
  /**<   Conveys the ecio high threshold parameter to be used for 1x which is configured by TE.
  */
}dsd_set_quality_measurement_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Used by the TE to signal the quality measurement
    information */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_quality_measurement_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Intializes the WQE profiles on the TE. */
typedef struct {

  /* Mandatory */
  /*  WQE Profile Enum type */
  dsd_wqe_profile_type_enum_v01 wqe_profile;
  /**<   WQE profile type. Values:\n
      - QMI_DSD_WQE_PROFILE_TYPE_DEFAULT (0) --  Wi-Fi Quality Estimation profile default
      - QMI_DSD_WQE_PROFILE_TYPE_INTERNET (1) --  Wi-Fi Quality Estimation profile internet
      - QMI_DSD_WQE_PROFILE_TYPE_AUDIO (2) --  Wi-Fi Quality Estimation profile audio
      - QMI_DSD_WQE_PROFILE_TYPE_VIDEO (3) --  Wi-Fi Quality Estimation profile video
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_1 (4) --  Wi-Fi Quality Estimation profile for IMS Type 1
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_2 (5) --  Wi-Fi Quality Estimation profile for IMS Type 2
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_3 (6) --  Wi-Fi Quality Estimation profile for IMS Type 3
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_4 (7) --  Wi-Fi Quality Estimation profile for IMS Type 4
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_5 (8) --  Wi-Fi Quality Estimation profile for IMS Type 5
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_6 (9) --  Wi-Fi Quality Estimation profile for IMS Type 6
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_7 (10) --  Wi-Fi Quality Estimation profile for IMS Type 7
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_8 (11) --  Wi-Fi Quality Estimation profile for IMS Type 8
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_9 (12) --  Wi-Fi Quality Estimation profile for IMS Type 9
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_10 (13) --  Wi-Fi Quality Estimation profile for IMS Type 10
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_11 (14) --  Wi-Fi Quality Estimation profile for IMS Type 11
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_12 (15) --  Wi-Fi Quality Estimation profile for IMS Type 12
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_13 (16) --  Wi-Fi Quality Estimation profile for IMS Type 13
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_14 (17) --  Wi-Fi Quality Estimation profile for IMS Type 14
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_15 (18) --  Wi-Fi Quality Estimation profile for IMS Type 15
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_16 (19) --  Wi-Fi Quality Estimation profile for IMS Type 16
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_17 (20) --  Wi-Fi Quality Estimation profile for IMS Type 17
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_18 (21) --  Wi-Fi Quality Estimation profile for IMS Type 18
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_19 (22) --  Wi-Fi Quality Estimation profile for IMS Type 19
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_20 (23) --  Wi-Fi Quality Estimation profile for IMS Type 20
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_21 (24) --  Wi-Fi Quality Estimation profile for IMS Type 21
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_22 (25) --  Wi-Fi Quality Estimation profile for IMS Type 22
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_23 (26) --  Wi-Fi Quality Estimation profile for IMS Type 23
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_24 (27) --  Wi-Fi Quality Estimation profile for IMS Type 24
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_25 (28) --  Wi-Fi Quality Estimation profile for IMS Type 25
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_26 (29) --  Wi-Fi Quality Estimation profile for IMS Type 26
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_27 (30) --  Wi-Fi Quality Estimation profile for IMS Type 27
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_28 (31) --  Wi-Fi Quality Estimation profile for IMS Type 28
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_29 (32) --  Wi-Fi Quality Estimation profile for IMS Type 29
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_30 (33) --  Wi-Fi Quality Estimation profile for IMS Type 30
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_31 (34) --  Wi-Fi Quality Estimation profile for IMS Type 31
      - QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_32 (35) --  Wi-Fi Quality Estimation profile for IMS Type 32
 */

  /* Optional */
  /*  Wifi RSSI Low threshold */
  uint8_t wifi_rssi_threshold_low_valid;  /**< Must be set to true if wifi_rssi_threshold_low is being passed */
  int16_t wifi_rssi_threshold_low;
  /**<   Conveys the RSSI low threshold parameter to be used for Wifi Quality Estmiation
  */

  /* Optional */
  /*  Wifi RSSI High threshold */
  uint8_t wifi_rssi_threshold_high_valid;  /**< Must be set to true if wifi_rssi_threshold_high is being passed */
  int16_t wifi_rssi_threshold_high;
  /**<   Conveys the RSSI high threshold parameter to be used for Wifi Quality Estmiation
  */

  /* Optional */
  /*  ICCID */
  uint8_t iccid_valid;  /**< Must be set to true if iccid is being passed */
  uint32_t iccid_len;  /**< Must be set to # of elements in iccid */
  uint8_t iccid[QMI_DSD_MAX_ICCID_LEN_V01];
  /**<   BCD encoded packed array representing the integrated circuit card identifier,
        stored in the sim card. The TE can use the information provided by this
        field, to determine the operator for which the new WQE profile is being
        initialized.
   */
}dsd_wqe_profile_init_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates UE roaming status change to TE */
typedef struct {

  /* Mandatory */
  /*  Roaming status Technology */
  dsd_sys_network_enum_v01 technology;
  /**<   Technology type. Values:\n
      - DSD_SYS_NETWORK_3GPP (0) --  3GPP \n
      - DSD_SYS_NETWORK_3GPP2 (1) --  3GPP2 \n
      - DSD_SYS_NETWORK_WLAN (2) --  WLAN
 */

  /* Optional */
  /*  Roaming Status */
  uint8_t roaming_status_valid;  /**< Must be set to true if roaming_status is being passed */
  uint8_t roaming_status;
  /**<   Boolean to indicate whether roaming status is on or off.
        Values: \n
       - 0 -- FALSE = Roaming off  \n
       - 1 -- TRUE  = Roaming on */
}dsd_roaming_status_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the current roaming status information */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_current_roaming_status_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the current roaming status information */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Roaming Status Info */
  uint8_t roaming_status_valid;  /**< Must be set to true if roaming_status is being passed */
  uint8_t roaming_status;
  /**<   Boolean to indicate whether roaming status is on or off.
        Values: \n
       - 0 -- FALSE = Roaming off  \n
       - 1 -- TRUE  = Roaming on */

  /* Optional */
  /*  Roaming status Technology */
  uint8_t technology_valid;  /**< Must be set to true if technology is being passed */
  dsd_sys_network_enum_v01 technology;
  /**<   Technology type. Values:\n
      - DSD_SYS_NETWORK_3GPP (0) --  3GPP \n
      - DSD_SYS_NETWORK_3GPP2 (1) --  3GPP2 \n
      - DSD_SYS_NETWORK_WLAN (2) --  WLAN
 */
}dsd_get_current_roaming_status_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WWAN_CONN_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WWAN_IDLE_STATE_V01 = 0x0000, /**<  WWAN is in idle state.
       Start uplink data monitor on the TE. \n  */
  DSD_WWAN_CONNECTED_STATE_V01 = 0x0001, /**<  WWAN is in connected state.
       Stop uplink data monitor on the TE. \n  */
  DSD_WWAN_INVALID_STATE_V01 = 0x00FF, /**<  WWAN is in invalid state. \n  */
  DSD_WWAN_CONN_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wwan_conn_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the control point about the WWAN connection
    state for TE to take appropriate uplink data monitor actions. */
typedef struct {

  /* Mandatory */
  /*  WWAN Connection State */
  dsd_wwan_conn_state_enum_type_v01 wwan_conn_state;
  /**<   Enum indicating the connection state of WWAN on the modem,
 based on which TE needs to take action based on certain
 traffic. Values \n.
      - DSD_WWAN_IDLE_STATE (0x0000) --  WWAN is in idle state.
       Start uplink data monitor on the TE. \n
      - DSD_WWAN_CONNECTED_STATE (0x0001) --  WWAN is in connected state.
       Stop uplink data monitor on the TE. \n
      - DSD_WWAN_INVALID_STATE (0x00FF) --  WWAN is in invalid state. \n ;
 */
}dsd_wwan_connection_state_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WWAN_ACTIVITY_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WWAN_UL_DATA_MONITOR_STOP_V01 = 0x0000, /**<  WWAN Uplink Data monitor stop.  */
  DSD_WWAN_UL_DATA_MONITOR_START_V01 = 0x0001, /**<  WWAN Uplink Data monitor start.  */
  DSD_WWAN_TRANSITION_TO_CONNECTED_V01 = 0x0002, /**<  Transition WWAN to connected state.   */
  DSD_WWAN_ACTIVITY_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wwan_activity_enum_type_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the activity needed to be performed by
    the modem for WWAN technology */
typedef struct {

  /* Mandatory */
  /*  WWAN Activity Type. */
  dsd_wwan_activity_enum_type_v01 activity_type;
  /**<   Enum indicating the activity to be performed by modem.
 Values \n.
      - DSD_WWAN_UL_DATA_MONITOR_STOP (0x0000) --  WWAN Uplink Data monitor stop.
      - DSD_WWAN_UL_DATA_MONITOR_START (0x0001) --  WWAN Uplink Data monitor start.
      - DSD_WWAN_TRANSITION_TO_CONNECTED (0x0002) --  Transition WWAN to connected state.  ;
 */
}dsd_set_wwan_activity_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the activity needed to be performed by
    the modem for WWAN technology */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}dsd_set_wwan_activity_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_LTE_CELL_BANDWIDTH_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_LTE_BW_6_V01 = 6,
  DSD_LTE_BW_15_V01 = 15,
  DSD_LTE_BW_25_V01 = 25,
  DSD_LTE_BW_50_V01 = 50,
  DSD_LTE_BW_75_V01 = 75,
  DSD_LTE_BW_100_V01 = 100,
  DSD_LTE_CELL_BANDWIDTH_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_lte_cell_bandwidth_enum_type_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_LTE_SCELL_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_LTE_SCELL_STATE_DECONFIGURED_V01 = 0,
  DSD_LTE_SCELL_STATE_CONFIGURED_DEACTIVATED_V01 = 1,
  DSD_LTE_SCELL_STATE_CONFIGURED_ACTIVATED_V01 = 2,
  DSD_LTE_SCELL_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_lte_scell_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
/**  Cell does not have context and is not configured yet
 Cell is in passive state. It wouldnt perform decoding
 Cell is in active state
 */
typedef struct {

  uint8_t serving_cell_id;

  /*  Serving Cell Id assigned to cell */
  uint16_t pci;

  /*  Physical cell id of the cell.   */
  uint32_t freq;

  /*  Cell's frequency  */
  dsd_lte_cell_bandwidth_enum_type_v01 dl_bandwidth;

  /*  Cell's Downlink Bandwidth   */
  uint32_t band_info;
}dsd_lte_cell_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  dsd_lte_cell_info_type_v01 cell_info;

  dsd_lte_scell_state_enum_type_v01 scell_state;

  uint8_t ul_enabled;
}dsd_lte_scell_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Queries the primary and secondary cell information for LTE RAT */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_lte_cell_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Queries the primary and secondary cell information for LTE RAT */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  LTE Primary cell information */
  uint8_t pcell_info_valid;  /**< Must be set to true if pcell_info is being passed */
  dsd_lte_cell_info_type_v01 pcell_info;

  /* Optional */
  /*  LTE Secondary cell information */
  uint8_t scell_info_valid;  /**< Must be set to true if scell_info is being passed */
  uint32_t scell_info_len;  /**< Must be set to # of elements in scell_info */
  dsd_lte_scell_info_type_v01 scell_info[QMI_DSD_MAX_LTE_NUM_SCELL_V01];
}dsd_get_lte_cell_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates change in primary and secondary cell information
           for LTE RAT. */
typedef struct {

  /* Mandatory */
  /*  LTE Primary cell information */
  dsd_lte_cell_info_type_v01 pcell_info;

  /* Mandatory */
  /*  LTE Secondary cell information */
  uint32_t scell_info_len;  /**< Must be set to # of elements in scell_info */
  dsd_lte_scell_info_type_v01 scell_info[QMI_DSD_MAX_LTE_NUM_SCELL_V01];
}dsd_lte_cell_info_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char mcc[QMI_DSD_MCC_MNC_SIZE_V01 + 1];

  char mnc[QMI_DSD_MCC_MNC_SIZE_V01 + 1];
}dsd_3gpp_mcc_mnc_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the 3GPP HPLMN LIST  */
typedef struct {

  /* Mandatory */
  /*  HPLMN List Type. */
  uint32_t hplmn_list_len;  /**< Must be set to # of elements in hplmn_list */
  dsd_3gpp_mcc_mnc_type_v01 hplmn_list[QMI_DSD_HPLMN_LIST_MAX_V01];
  /**<   HPLMN List info
   */

  /* Optional */
  /*  Exception HPLMN List Type. */
  uint8_t excp_hplmn_list_valid;  /**< Must be set to true if excp_hplmn_list is being passed */
  uint32_t excp_hplmn_list_len;  /**< Must be set to # of elements in excp_hplmn_list */
  dsd_3gpp_mcc_mnc_type_v01 excp_hplmn_list[QMI_DSD_EXCP_HPLMN_LIST_MAX_V01];
  /**<   Exception HPLMN List info
      */
}dsd_set_3gpp_hplmn_list_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the 3GPP HPLMN LIST  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}dsd_set_3gpp_hplmn_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Retrieves the 3GPP HPLMN LIST  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_3gpp_hplmn_list_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Retrieves the 3GPP HPLMN LIST  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  HPLMN List Type. */
  uint8_t hplmn_list_valid;  /**< Must be set to true if hplmn_list is being passed */
  uint32_t hplmn_list_len;  /**< Must be set to # of elements in hplmn_list */
  dsd_3gpp_mcc_mnc_type_v01 hplmn_list[QMI_DSD_HPLMN_LIST_MAX_V01];
  /**<   HPLMN List info
   */

  /* Optional */
  /*  Exception HPLMN List Type. */
  uint8_t excp_hplmn_list_valid;  /**< Must be set to true if excp_hplmn_list is being passed */
  uint32_t excp_hplmn_list_len;  /**< Must be set to # of elements in excp_hplmn_list */
  dsd_3gpp_mcc_mnc_type_v01 excp_hplmn_list[QMI_DSD_EXCP_HPLMN_LIST_MAX_V01];
  /**<   Exception HPLMN List info
     */
}dsd_get_3gpp_hplmn_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the DDS (Default Data Subscription) to specified subscription */
typedef struct {

  /* Mandatory */
  /*  New Default Data Subscirption */
  dsd_bind_subscription_enum_v01 subscription;
  /**<   Values: \n
      - DSD_PRIMARY_SUBS (0x0001) --  Primary \n
      - DSD_SECONDARY_SUBS (0x0002) --  Secondary \n
      - DSD_TERTIARY_SUBS (0x0003) --  Tertiary
 */

 /* Optional */
  /*  DDS Switch Type */
  uint8_t dds_switch_type_valid;  /**< Must be set to true if dds_switch_type is being passed */
  dsd_dds_switch_type_enum_v01 dds_switch_type;
  /**<   This TLV conatins DDS switch type. \n
      - DSD_DDS_SWITCH_PERMANENT (0x00) --  Permanent(Default) 
      - DSD_DDS_SWITCH_TEMPORARY (0x01) --  Temporary 
 */
}dsd_switch_dds_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the DDS (Default Data Subscription) to specified subscription */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}dsd_switch_dds_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_DDS_SWITCH_RESULT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_DDS_SWITCH_ALLOWED_V01 = 0x00,
  DSD_DDS_SWITCH_NOT_ALLOWED_V01 = 0x01,
  DSD_DDS_SWITCH_FAILED_V01 = 0x02,
  DSD_DDS_SWITCH_RESULT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_dds_switch_result_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the result of the DDS Switch operation requested using
    QMI_DSD_SWITCH_DDS_REQ */
typedef struct {

  /* Mandatory */
  /*  DDS Switch result code */
  dsd_dds_switch_result_enum_v01 dds_switch_result;
  /**<   This TLV contains the result code of DDS switch operation
      - DSD_DDS_SWITCH_ALLOWED (0x00) --
      - DSD_DDS_SWITCH_NOT_ALLOWED (0x01) --
      - DSD_DDS_SWITCH_FAILED (0x02) --
 */
}dsd_switch_dds_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Retrieves the current DDS */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dsd_get_current_dds_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Retrieves the current DDS */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Current Default Data Subscription */
  uint8_t dds_valid;  /**< Must be set to true if dds is being passed */
  dsd_bind_subscription_enum_v01 dds;
  /**<   This TLV contains the current DDS
      - DSD_PRIMARY_SUBS (0x0001) --  Primary \n
      - DSD_SECONDARY_SUBS (0x0002) --  Secondary \n
      - DSD_TERTIARY_SUBS (0x0003) --  Tertiary
 */

  /* Optional */
  /*  DDS Switch Type */
  uint8_t dds_switch_type_valid;  /**< Must be set to true if dds_switch_type is being passed */
  dsd_dds_switch_type_enum_v01 dds_switch_type;
  /**<   This TLV conatins DDS switch type. \n
      - DSD_DDS_SWITCH_PERMANENT (0x00) --  Permanent(Default) 
      - DSD_DDS_SWITCH_TEMPORARY (0x01) --  Temporary 
 */
}dsd_get_current_dds_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates the current default data subscription of the UE */
typedef struct {

  /* Mandatory */
  /*  Current Default Data Subscription */
  dsd_bind_subscription_enum_v01 dds;
  /**<   This TLV contains the current DDS
      - DSD_PRIMARY_SUBS (0x0001) --  Primary \n
      - DSD_SECONDARY_SUBS (0x0002) --  Secondary \n
      - DSD_TERTIARY_SUBS (0x0003) --  Tertiary
 */

  /* Optional */
  /*  DDS Switch Type */
  uint8_t dds_switch_type_valid;  /**< Must be set to true if dds_switch_type is being passed */
  dsd_dds_switch_type_enum_v01 dds_switch_type;
  /**<   This TLV conatins DDS switch type. \n
      - DSD_DDS_SWITCH_PERMANENT (0x00) --  Permanent(Default) 
      - DSD_DDS_SWITCH_TEMPORARY (0x01) --  Temporary 
 */
}dsd_current_dds_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t dsd_apn_type_mask_v01;
#define QMI_DSD_APN_TYPE_MASK_UNSPECIFIED_V01 ((dsd_apn_type_mask_v01)0x0000000000000000ull)
#define QMI_DSD_APN_TYPE_MASK_DEFAULT_V01 ((dsd_apn_type_mask_v01)0x0000000000000001ull) /**<  APN type for Default/Internet traffic  */
#define QMI_DSD_APN_TYPE_MASK_IMS_V01 ((dsd_apn_type_mask_v01)0x0000000000000002ull) /**<  APN type for IP Multimedia Subsystem  */
#define QMI_DSD_APN_TYPE_MASK_MMS_V01 ((dsd_apn_type_mask_v01)0x0000000000000004ull) /**<  APN type for Multimedia Messaging Service  */
#define QMI_DSD_APN_TYPE_MASK_DUN_V01 ((dsd_apn_type_mask_v01)0x0000000000000008ull) /**<  APN type for Dial Up Network  */
#define QMI_DSD_APN_TYPE_MASK_SUPL_V01 ((dsd_apn_type_mask_v01)0x0000000000000010ull) /**<  APN type for Secure User Plane Location  */
#define QMI_DSD_APN_TYPE_MASK_HIPRI_V01 ((dsd_apn_type_mask_v01)0x0000000000000020ull) /**<  APN type for High Priority Mobile Data  */
#define QMI_DSD_APN_TYPE_MASK_FOTA_V01 ((dsd_apn_type_mask_v01)0x0000000000000040ull) /**<  APN type for over the air administration  */
#define QMI_DSD_APN_TYPE_MASK_CBS_V01 ((dsd_apn_type_mask_v01)0x0000000000000080ull) /**<  APN type for Carrier Branded Services  */
#define QMI_DSD_APN_TYPE_MASK_IA_V01 ((dsd_apn_type_mask_v01)0x0000000000000100ull) /**<  APN type for Initial Attach  */
#define QMI_DSD_APN_TYPE_MASK_EMERGENCY_V01 ((dsd_apn_type_mask_v01)0x0000000000000200ull) /**<  APN type for emergency  */
/** @addtogroup dsd_qmi_aggregates
    @{
  */
typedef struct {

  char apn_name[QMI_DSD_MAX_APN_LEN_V01 + 1];
  /**<   APN Name
     */

  dsd_apn_type_mask_v01 apn_type_mask;
  /**<   APN Type mask Values:\n
      - QMI_DSD_APN_TYPE_MASK_UNSPECIFIED (0x0000000000000000) --
      - QMI_DSD_APN_TYPE_MASK_DEFAULT (0x0000000000000001) --  APN type for Default/Internet traffic
      - QMI_DSD_APN_TYPE_MASK_IMS (0x0000000000000002) --  APN type for IP Multimedia Subsystem
      - QMI_DSD_APN_TYPE_MASK_MMS (0x0000000000000004) --  APN type for Multimedia Messaging Service
      - QMI_DSD_APN_TYPE_MASK_DUN (0x0000000000000008) --  APN type for Dial Up Network
      - QMI_DSD_APN_TYPE_MASK_SUPL (0x0000000000000010) --  APN type for Secure User Plane Location
      - QMI_DSD_APN_TYPE_MASK_HIPRI (0x0000000000000020) --  APN type for High Priority Mobile Data
      - QMI_DSD_APN_TYPE_MASK_FOTA (0x0000000000000040) --  APN type for over the air administration
      - QMI_DSD_APN_TYPE_MASK_CBS (0x0000000000000080) --  APN type for Carrier Branded Services
      - QMI_DSD_APN_TYPE_MASK_IA (0x0000000000000100) --  APN type for Initial Attach
      - QMI_DSD_APN_TYPE_MASK_EMERGENCY (0x0000000000000200) --  APN type for emergency
 */
}dsd_apn_type_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Sets the APN type(s) for a given APN name. */
typedef struct {

  /* Mandatory */
  /*  APN Type Info */
  dsd_apn_type_info_type_v01 apn_type_info;
  /**<   APN type Info.
   */

  /* Optional */
  /*  APN Type Preference Mask */
  uint8_t apn_type_preference_mask_valid;  /**< Must be set to true if apn_type_preference_mask is being passed */
  dsd_apn_type_mask_v01 apn_type_preference_mask;
  /**<   APN type preference mask for the given APN Values:\n
      - QMI_DSD_APN_TYPE_MASK_UNSPECIFIED (0x0000000000000000) --
      - QMI_DSD_APN_TYPE_MASK_DEFAULT (0x0000000000000001) --  APN type for Default/Internet traffic
      - QMI_DSD_APN_TYPE_MASK_IMS (0x0000000000000002) --  APN type for IP Multimedia Subsystem
      - QMI_DSD_APN_TYPE_MASK_MMS (0x0000000000000004) --  APN type for Multimedia Messaging Service
      - QMI_DSD_APN_TYPE_MASK_DUN (0x0000000000000008) --  APN type for Dial Up Network
      - QMI_DSD_APN_TYPE_MASK_SUPL (0x0000000000000010) --  APN type for Secure User Plane Location
      - QMI_DSD_APN_TYPE_MASK_HIPRI (0x0000000000000020) --  APN type for High Priority Mobile Data
      - QMI_DSD_APN_TYPE_MASK_FOTA (0x0000000000000040) --  APN type for over the air administration
      - QMI_DSD_APN_TYPE_MASK_CBS (0x0000000000000080) --  APN type for Carrier Branded Services
      - QMI_DSD_APN_TYPE_MASK_IA (0x0000000000000100) --  APN type for Initial Attach
      - QMI_DSD_APN_TYPE_MASK_EMERGENCY (0x0000000000000200) --  APN type for emergency
 */
}dsd_set_apn_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Sets the APN type(s) for a given APN name. */
typedef struct {

  /* Mandatory */
  /*  Result code. */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       */
}dsd_set_apn_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Configure VOWIFI service on device */
typedef struct {

  /* Optional */
  /*  FQDN */
  uint8_t fqdn_valid;  /**< Must be set to true if fqdn is being passed */
  char fqdn[QMI_DSD_MAX_FQDN_LEN_V01 + 1];
  /**<   FQDN for the ePDG to be used for VoWIFI connectivity
     */

  /* Optional */
  /*  IPv4 address */
  uint8_t vowifi_ipv4_addr_valid;  /**< Must be set to true if vowifi_ipv4_addr is being passed */
  uint32_t vowifi_ipv4_addr;

  /* Optional */
  /*  IPv6 address */
  uint8_t vowifi_ipv6_addr_valid;  /**< Must be set to true if vowifi_ipv6_addr is being passed */
  uint8_t vowifi_ipv6_addr[QMI_DSD_IPV6_ADDR_LEN_V01];
}qmi_dsd_configure_vowifi_service_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Configure VOWIFI service on device */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}qmi_dsd_configure_vowifi_service_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t dsd_wifi_srv_cap_mask_v01;
#define DSD_WIFI_SRV_CAP_MASK_UNSPECIFIED_V01 ((dsd_wifi_srv_cap_mask_v01)0x0000000000000000ull)
#define DSD_WIFI_SRV_CAP_MASK_BAND_VACATE_CAPABLE_V01 ((dsd_wifi_srv_cap_mask_v01)0x0000000000000001ull) /**<  Wifi band vacate capable  */
/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Notify Wifi service capability to modem */
typedef struct {

  /* Mandatory */
  /*  Wifi Service Capability Mask */
  dsd_wifi_srv_cap_mask_v01 wifi_srv_cap_mask;
  /**<   Wifi service capability mask Values:\n
      - DSD_WIFI_SRV_CAP_MASK_UNSPECIFIED (0x0000000000000000) --
      - DSD_WIFI_SRV_CAP_MASK_BAND_VACATE_CAPABLE (0x0000000000000001) --  Wifi band vacate capable
 */
}dsd_wifi_service_cap_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Notify Wifi service capability to modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}dsd_wifi_service_cap_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_BAND_VACATE_REQUEST_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_BAND_VACATE_NOT_RESTRICTED_V01 = 0x00, /**<  Wifi can operate in any band  */
  DSD_WIFI_BAND_VACATE_5GHZ_V01 = 0x01, /**<  Wifi request to vacate 5GHz band  */
  DSD_WIFI_BAND_VACATE_REQUEST_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_band_vacate_request_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Indication Message; Indicates TE to vacate/not vacate Wifi band */
typedef struct {

  /* Mandatory */
  /*  Vacate action */
  dsd_wifi_band_vacate_request_enum_v01 wifi_band_vacate_request;
  /**<   Wifi band vacate request information Values: \n
      - DSD_WIFI_BAND_VACATE_NOT_RESTRICTED (0x00) --  Wifi can operate in any band
      - DSD_WIFI_BAND_VACATE_5GHZ (0x01) --  Wifi request to vacate 5GHz band
 */
}dsd_wifi_band_vacate_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_enums
    @{
  */
typedef enum {
  DSD_WIFI_BAND_VACATE_RESULT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DSD_WIFI_BAND_VACATE_NOT_ALLOWED_V01 = 0x00, /**<  Current Wifi conditions don't allow vacating requested band  */
  DSD_WIFI_BAND_VACATE_CURRENTLY_VACATED_V01 = 0x01, /**<  At time of the vacate request Wifi was not on requested band  */
  DSD_WIFI_BAND_VACATE_RESULT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dsd_wifi_band_vacate_result_enum_v01;
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Request Message; Indicates that Wifi did not vacate the requested band */
typedef struct {

  /* Mandatory */
  /*  Wifi band vacate result */
  dsd_wifi_band_vacate_result_enum_v01 wifi_band_vacate_result;
  /**<   Reason why Wifi did not vacate requested band Values: \n
      - DSD_WIFI_BAND_VACATE_NOT_ALLOWED (0x00) --  Current Wifi conditions don't allow vacating requested band
      - DSD_WIFI_BAND_VACATE_CURRENTLY_VACATED (0x01) --  At time of the vacate request Wifi was not on requested band
 */
}dsd_wifi_band_vacate_result_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsd_qmi_messages
    @{
  */
/** Response Message; Indicates that Wifi did not vacate the requested band */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}dsd_wifi_band_vacate_result_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_DSD_ADD_PDN_POLICY_V01
//#define REMOVE_QMI_DSD_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_DSD_CONFIGURE_VOWIFI_SERVICE_V01
//#define REMOVE_QMI_DSD_CURRENT_DDS_IND_V01
//#define REMOVE_QMI_DSD_DELETE_PDN_POLICY_BY_APN_V01
//#define REMOVE_QMI_DSD_GET_3GPP_HPLMN_LIST_V01
//#define REMOVE_QMI_DSD_GET_APN_INFO_V01
//#define REMOVE_QMI_DSD_GET_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_DSD_GET_CURRENT_DDS_V01
//#define REMOVE_QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_V01
//#define REMOVE_QMI_DSD_GET_DATA_SETTING_V01
//#define REMOVE_QMI_DSD_GET_LTE_CELL_INFO_V01
//#define REMOVE_QMI_DSD_GET_MODEM_POWER_COST_V01
//#define REMOVE_QMI_DSD_GET_PDN_POLICY_APN_LIST_V01
//#define REMOVE_QMI_DSD_GET_PDN_POLICY_SETTINGS_FOR_APN_V01
//#define REMOVE_QMI_DSD_GET_SYSTEM_STATUS_V01
//#define REMOVE_QMI_DSD_GET_THERMAL_MITIGATION_INFO_V01
//#define REMOVE_QMI_DSD_GET_WLAN_PREFERENCE_V01
//#define REMOVE_QMI_DSD_INDICATION_REGISTER_V01
//#define REMOVE_QMI_DSD_LTE_CELL_INFO_CHANGE_IND_V01
//#define REMOVE_QMI_DSD_MODIFY_PDN_POLICY_V01
//#define REMOVE_QMI_DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_V01
//#define REMOVE_QMI_DSD_NAT_KEEP_ALIVE_INFO_IND_V01
//#define REMOVE_QMI_DSD_NOTIFY_DATA_SETTING_V01
//#define REMOVE_QMI_DSD_PDN_POLICY_END_TXN_V01
//#define REMOVE_QMI_DSD_PDN_POLICY_START_TXN_V01
//#define REMOVE_QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01
//#define REMOVE_QMI_DSD_SET_3GPP_HPLMN_LIST_V01
//#define REMOVE_QMI_DSD_SET_APN_CALL_TYPE_V01
//#define REMOVE_QMI_DSD_SET_APN_INFO_V01
//#define REMOVE_QMI_DSD_SET_APN_PREFERRED_SYSTEM_V01
//#define REMOVE_QMI_DSD_SET_APN_TYPE_V01
//#define REMOVE_QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_V01
//#define REMOVE_QMI_DSD_SET_WLAN_PREFERENCE_V01
//#define REMOVE_QMI_DSD_SET_WQE_PROFILE_QUALITY_STATUS_V01
//#define REMOVE_QMI_DSD_SET_WQE_PROFILE_TYPE_V01
//#define REMOVE_QMI_DSD_SET_WWAN_ACTIVITY_V01
//#define REMOVE_QMI_DSD_START_WIFI_MEAS_IND_V01
//#define REMOVE_QMI_DSD_STOP_WIFI_MEAS_IND_V01
//#define REMOVE_QMI_DSD_SWITCH_DDS_V01
//#define REMOVE_QMI_DSD_SWITCH_DDS_IND_V01
//#define REMOVE_QMI_DSD_SYSTEM_STATUS_CHANGE_V01
//#define REMOVE_QMI_DSD_SYSTEM_STATUS_IND_V01
//#define REMOVE_QMI_DSD_THERMAL_INFO_CHANGE_IND_V01
//#define REMOVE_QMI_DSD_WIFI_BAND_VACATE_IND_V01
//#define REMOVE_QMI_DSD_WIFI_BAND_VACATE_RESULT_V01
//#define REMOVE_QMI_DSD_WIFI_MEAS_REPORT_V01
//#define REMOVE_QMI_DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_V01
//#define REMOVE_QMI_DSD_WIFI_SERVICE_CAP_V01
//#define REMOVE_QMI_DSD_WLAN_AVAILABLE_V01
//#define REMOVE_QMI_DSD_WLAN_NOT_AVAILABLE_V01
//#define REMOVE_QMI_DSD_WQE_PROFILE_INIT_IND_V01
//#define REMOVE_QMI_DSD_WQE_PROFILE_TYPE_IND_V01
//#define REMOVE_QMI_DSD_WQE_PROFILE_TYPE_MET_IND_V01
//#define REMOVE_QMI_DSD_WWAN_CONNECTION_STATE_IND_V01

/*Service Message Definition*/
/** @addtogroup dsd_qmi_msg_ids
    @{
  */
#define QMI_DSD_WLAN_AVAILABLE_REQ_V01 0x0020
#define QMI_DSD_WLAN_AVAILABLE_RESP_V01 0x0020
#define QMI_DSD_WLAN_NOT_AVAILABLE_REQ_V01 0x0021
#define QMI_DSD_WLAN_NOT_AVAILABLE_RESP_V01 0x0021
#define QMI_DSD_SET_WLAN_PREFERENCE_REQ_V01 0x0022
#define QMI_DSD_SET_WLAN_PREFERENCE_RESP_V01 0x0022
#define QMI_DSD_GET_WLAN_PREFERENCE_REQ_V01 0x0023
#define QMI_DSD_GET_WLAN_PREFERENCE_RESP_V01 0x0023
#define QMI_DSD_GET_SYSTEM_STATUS_REQ_V01 0x0024
#define QMI_DSD_GET_SYSTEM_STATUS_RESP_V01 0x0024
#define QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01 0x0025
#define QMI_DSD_SYSTEM_STATUS_CHANGE_RESP_V01 0x0025
#define QMI_DSD_SYSTEM_STATUS_IND_V01 0x0026
#define QMI_DSD_BIND_SUBSCRIPTION_REQ_V01 0x0027
#define QMI_DSD_BIND_SUBSCRIPTION_RESP_V01 0x0027
#define QMI_DSD_GET_BIND_SUBSCRIPTION_REQ_V01 0x0028
#define QMI_DSD_GET_BIND_SUBSCRIPTION_RESP_V01 0x0028
#define QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01 0x0029
#define QMI_DSD_SET_APN_PREFERRED_SYSTEM_RESP_V01 0x0029
#define QMI_DSD_GET_MODEM_POWER_COST_REQ_V01 0x002A
#define QMI_DSD_GET_MODEM_POWER_COST_RESP_V01 0x002A
#define QMI_DSD_PDN_POLICY_START_TXN_REQ_V01 0x002B
#define QMI_DSD_PDN_POLICY_START_TXN_RESP_V01 0x002B
#define QMI_DSD_ADD_PDN_POLICY_REQ_V01 0x002C
#define QMI_DSD_ADD_PDN_POLICY_RESP_V01 0x002C
#define QMI_DSD_MODIFY_PDN_POLICY_REQ_V01 0x002D
#define QMI_DSD_MODIFY_PDN_POLICY_RESP_V01 0x002D
#define QMI_DSD_DELETE_PDN_POLICY_BY_APN_REQ_V01 0x002E
#define QMI_DSD_DELETE_PDN_POLICY_BY_APN_RESP_V01 0x002E
#define QMI_DSD_GET_PDN_POLICY_APN_LIST_REQ_V01 0x002F
#define QMI_DSD_GET_PDN_POLICY_APN_LIST_RESP_V01 0x002F
#define QMI_DSD_GET_PDN_POLICY_SETTINGS_FOR_APN_REQ_V01 0x0030
#define QMI_DSD_GET_PDN_POLICY_SETTINGS_FOR_APN_RESP_V01 0x0030
#define QMI_DSD_PDN_POLICY_END_TXN_REQ_V01 0x0031
#define QMI_DSD_PDN_POLICY_END_TXN_RESP_V01 0x0031
#define QMI_DSD_SET_APN_INFO_REQ_V01 0x0032
#define QMI_DSD_SET_APN_INFO_RESP_V01 0x0032
#define QMI_DSD_GET_APN_INFO_REQ_V01 0x0033
#define QMI_DSD_GET_APN_INFO_RESP_V01 0x0033
#define QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01 0x0034
#define QMI_DSD_NOTIFY_DATA_SETTING_RESP_V01 0x0034
#define QMI_DSD_GET_DATA_SETTING_REQ_V01 0x0035
#define QMI_DSD_GET_DATA_SETTING_RESP_V01 0x0035
#define QMI_DSD_THERMAL_INFO_CHANGE_IND_V01 0x0036
#define QMI_DSD_GET_THERMAL_MITIGATION_INFO_REQ_V01 0x0037
#define QMI_DSD_GET_THERMAL_MITIGATION_INFO_RESP_V01 0x0037
#define QMI_DSD_INDICATION_REGISTER_REQ_V01 0x0038
#define QMI_DSD_INDICATION_REGISTER_RESP_V01 0x0038
#define QMI_DSD_START_WIFI_MEAS_IND_V01 0x0039
#define QMI_DSD_STOP_WIFI_MEAS_IND_V01 0x003A
#define QMI_DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_REQ_V01 0x003B
#define QMI_DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_RESP_V01 0x003B
#define QMI_DSD_WIFI_MEAS_REPORT_REQ_V01 0x003C
#define QMI_DSD_WIFI_MEAS_REPORT_RESP_V01 0x003C
#define QMI_DSD_SET_APN_CALL_TYPE_REQ_V01 0x003D
#define QMI_DSD_SET_APN_CALL_TYPE_RESP_V01 0x003D
#define QMI_DSD_SET_WQE_PROFILE_TYPE_REQ_V01 0x003E
#define QMI_DSD_SET_WQE_PROFILE_TYPE_RESP_V01 0x003E
#define QMI_DSD_WQE_PROFILE_TYPE_IND_V01 0x003F
#define QMI_DSD_WQE_PROFILE_TYPE_MET_IND_V01 0x0040
#define QMI_DSD_NAT_KEEP_ALIVE_INFO_IND_V01 0x0041
#define QMI_DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_REQ_V01 0x0042
#define QMI_DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_RESP_V01 0x0042
#define QMI_DSD_SET_WQE_PROFILE_QUALITY_STATUS_REQ_V01 0x0043
#define QMI_DSD_SET_WQE_PROFILE_QUALITY_STATUS_RESP_V01 0x0043
#define QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_REQ_V01 0x0044
#define QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_RESP_V01 0x0044
#define QMI_DSD_WQE_PROFILE_INIT_IND_V01 0x0045
#define QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01 0x0046
#define QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_REQ_V01 0x0047
#define QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_RESP_V01 0x0047
#define QMI_DSD_WWAN_CONNECTION_STATE_IND_V01 0x0048
#define QMI_DSD_SET_WWAN_ACTIVITY_REQ_V01 0x0049
#define QMI_DSD_SET_WWAN_ACTIVITY_RESP_V01 0x0049
#define QMI_DSD_LTE_CELL_INFO_CHANGE_IND_V01 0x004A
#define QMI_DSD_GET_LTE_CELL_INFO_REQ_V01 0x004B
#define QMI_DSD_GET_LTE_CELL_INFO_RESP_V01 0x004B
#define QMI_DSD_SET_3GPP_HPLMN_LIST_REQ_V01 0x004C
#define QMI_DSD_SET_3GPP_HPLMN_LIST_RESP_V01 0x004C
#define QMI_DSD_GET_3GPP_HPLMN_LIST_REQ_V01 0x004D
#define QMI_DSD_GET_3GPP_HPLMN_LIST_RESP_V01 0x004D
#define QMI_DSD_SWITCH_DDS_REQ_V01 0x004E
#define QMI_DSD_SWITCH_DDS_RESP_V01 0x004E
#define QMI_DSD_SWITCH_DDS_IND_V01 0x004E
#define QMI_DSD_GET_CURRENT_DDS_REQ_V01 0x004F
#define QMI_DSD_GET_CURRENT_DDS_RESP_V01 0x004F
#define QMI_DSD_CURRENT_DDS_IND_V01 0x0050
#define QMI_DSD_SET_APN_TYPE_REQ_V01 0x0051
#define QMI_DSD_SET_APN_TYPE_RESP_V01 0x0051
#define QMI_DSD_CONFIGURE_VOWIFI_SERVICE_REQ_V01 0x0052
#define QMI_DSD_CONFIGURE_VOWIFI_SERVICE_RESP_V01 0x0052
#define QMI_DSD_WIFI_SERVICE_CAP_REQ_V01 0x0053
#define QMI_DSD_WIFI_SERVICE_CAP_RESP_V01 0x0053
#define QMI_DSD_WIFI_BAND_VACATE_IND_V01 0x0054
#define QMI_DSD_WIFI_BAND_VACATE_RESULT_REQ_V01 0x0055
#define QMI_DSD_WIFI_BAND_VACATE_RESULT_RESP_V01 0x0055
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro dsd_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type dsd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define dsd_get_service_object_v01( ) \
          dsd_get_service_object_internal_v01( \
            DSD_V01_IDL_MAJOR_VERS, DSD_V01_IDL_MINOR_VERS, \
            DSD_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

