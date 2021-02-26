#ifndef IMSS_SERVICE_01_H
#define IMSS_SERVICE_01_H
/**
  @file ip_multimedia_subsystem_settings_v01.h

  @brief This is the public header file which defines the imss service Data structures.

  This header file defines the types and structures that were defined in
  imss. It contains the constant values defined, enums, structures,
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



  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/imss/api/ip_multimedia_subsystem_settings_v01.h#16 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Jun 14 2017 (Spin 0)
   From IDL File: ip_multimedia_subsystem_settings_v01.idl */

/** @defgroup imss_qmi_consts Constant values defined in the IDL */
/** @defgroup imss_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup imss_qmi_enums Enumerated types used in QMI messages */
/** @defgroup imss_qmi_messages Structures sent as QMI messages */
/** @defgroup imss_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup imss_qmi_accessor Accessor for QMI service object */
/** @defgroup imss_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "ip_multimedia_subsystem_ims_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup imss_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IMSS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IMSS_V01_IDL_MINOR_VERS 0x3B
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IMSS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IMSS_V01_MAX_MESSAGE_ID 0x009A
/**
    @}
  */


/** @addtogroup imss_qmi_consts
    @{
  */

/** 

 Enumeration for all the common IMS error codes */
#define IMS_SETTINGS_STRING_LEN_MAX_V01 255
#define IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01 255
#define IMS_SETTINGS_REG_CONFIG_USER_NAME_LEN_V01 127
#define IMS_SETTINGS_REG_CONFIG_PRIVATE_URI_LEN_V01 127
#define IMS_SETTINGS_REG_CONFIG_DISPLAY_NAME_LEN_V01 63
#define IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01 31
#define IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01 269
#define IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01 127
#define IMS_SETTINGS_VOIP_AMR_MODE_STR_LEN_V01 31
#define IMS_SETTINGS_VOIP_AMR_WB_MODE_STR_LEN_V01 31
#define IMS_SETTINGS_POL_MGR_APN_NAME_STR_LEN_V01 49
#define IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01 10
#define IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01 10
#define IMS_SETTINGS_POL_MGR_APN_SIZE_V01 6
#define IMS_SETTINGS_POL_MGR_RAT_MAX_V01 2
#define IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01 127
#define IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01 128
#define IMS_SETTINGS_UT_APN_NAME_LEN_V01 64
#define IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01 127
#define IMS_SETTINGS_SMS_PSI_LEN_V01 128
#define IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01 80
#define IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01 10
#define IMS_SETTINGS_CSD_LEN_V01 128
#define IMS_SETTINGS_CSD_CODEC_MAX_V01 4
#define IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 15
#define IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 127
#define IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 99
#define IMS_SETTINGS_FEATURE_TAG_LEN_V01 127
#define IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01 127
#define IMS_SETTINGS_IMS_USER_AGENT_LEN_V01 1023
#define IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01 19
#define IMS_SETTINGS_RM_STRING_LEN_MAX_V01 255
#define IMS_SETTINGS_IMS_STRING_LEN_MAX_V01 255
#define IMS_SETTINGS_IMS_SMS_PSI_LEN_V01 127
#define IMS_SETTINGS_RTT_DATA_LEN_MAX_V01 127
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_RSP_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_MSG_NO_ERR_V01 = 0, /**<  No error  */
  IMS_SETTINGS_MSG_IMS_NOT_READY_V01 = 1, /**<  Service indication  */
  IMS_SETTINGS_MSG_FILE_NOT_AVAILABLE_V01 = 2, /**<  Settings file is not available  */
  IMS_SETTINGS_MSG_READ_FAILED_V01 = 3, /**<  Read failure  */
  IMS_SETTINGS_MSG_WRITE_FAILED_V01 = 4, /**<  Write failure  */
  IMS_SETTINGS_MSG_OTHER_INTERNAL_ERR_V01 = 5, /**<  Other settings internal error  */
  IMS_SETTINGS_RSP_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_rsp_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_SMS_FORMAT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_SMS_FORMAT_3GPP2_V01 = 0, /**<  3GPP2 SMS format      */
  IMS_SETTINGS_SMS_FORMAT_3GPP_V01 = 1, /**<  3GPP SMS format  */
  IMS_SETTINGS_SMS_FORMAT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_sms_format_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_H264_PROFILE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_H264_PROFILE_BASELINE_V01 = 0x00, /**<  Baseline profile */
  IMS_SETTINGS_H264_PROFILE_MAIN_V01 = 0x01, /**<  Main profile */
  IMS_SETTINGS_H264_PROFILE_EXTENDED_V01 = 0x02, /**<  Extended profile */
  IMS_SETTINGS_H264_PROFILE_HIGH_V01 = 0x03, /**<  High profile */
  IMS_SETTINGS_H264_PROFILE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_h264_profile_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_H264_LEVEL_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_H264_LEVEL1_V01 = 0x00, /**<  Level 1 */
  IMS_SETTINGS_H264_LEVEL1B_V01 = 0x01, /**<  Level 1b */
  IMS_SETTINGS_H264_LEVEL11_V01 = 0x02, /**<  Level 1.1 */
  IMS_SETTINGS_H264_LEVEL12_V01 = 0x03, /**<  Level 1.2 */
  IMS_SETTINGS_H264_LEVEL13_V01 = 0x04, /**<  Level 1.3 */
  IMS_SETTINGS_H264_LEVEL2_V01 = 0x05, /**<  Level 2 */
  IMS_SETTINGS_H264_LEVEL21_V01 = 0x06, /**<  Level 2.1 */
  IMS_SETTINGS_H264_LEVEL22_V01 = 0x07, /**<  Level 2.2 */
  IMS_SETTINGS_H264_LEVEL3_V01 = 0x08, /**<  Level 3 */
  IMS_SETTINGS_H264_LEVEL31_V01 = 0x09, /**<  Level 3.1 */
  IMS_SETTINGS_H264_LEVEL32_V01 = 0x0A, /**<  Level 3.2 */
  IMS_SETTINGS_H264_LEVEL4_V01 = 0x0B, /**<  Level 4 */
  IMS_SETTINGS_H264_LEVEL41_V01 = 0x0C, /**<  Level 4.1 */
  IMS_SETTINGS_H264_LEVEL42_V01 = 0x0D, /**<  Level 4.2 */
  IMS_SETTINGS_H264_LEVEL5_V01 = 0x0E, /**<  Level 5 */
  IMS_SETTINGS_H264_LEVEL51_V01 = 0x0F, /**<  Level 5.1 */
  IMS_SETTINGS_H264_LEVEL_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_h264_level_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_VIDEO_RESOLUTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_SQCIF_RESOLUTION_V01 = 0x00, /**<  SQCIF */
  IMS_SETTINGS_QCIF_RESOLUTION_V01 = 0x01, /**<  QCIF */
  IMS_SETTINGS_CIF_RESOLUTION_V01 = 0x02, /**<  CIF */
  IMS_SETTINGS_QQVGA_RESOLUTION_V01 = 0x03, /**<  QQVGA */
  IMS_SETTINGS_QVGA_RESOLUTION_V01 = 0x04, /**<  QVGA */
  IMS_SETTINGS_VGA_RESOLUTION_V01 = 0x05, /**<  VGA  */
  IMS_VIDEO_RESOLUTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_video_resolution_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_VIDEO_CODEC_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CODEC_MPEG4_XVID_V01 = 0x00, /**<  XVID MPEG4 codec */
  IMS_SETTINGS_CODEC_MPEG4_ISO_V01 = 0x01, /**<  ISO MPEG4 codec */
  IMS_SETTINGS_CODEC_H263_V01 = 0x02, /**<  H.263 codec */
  IMS_SETTINGS_CODEC_H264_V01 = 0x03, /**<  H.264 codec */
  IMS_SETTINGS_VIDEO_CODEC_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_video_codec_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_NONE_V01 = 0x00, /**<  No scheme used  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_DIGEST_V01 = 0x01, /**<  Digest  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_SAG_V01 = 0x02, /**<  Token  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKA_V01 = 0x03, /**<  AKA  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_CAVE_V01 = 0x04, /**<  CAVE  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKAV2_V01 = 0x05, /**<  AKAv2  */
  IMS_SETTINGS_CONFIG_AUTH_SCHEME_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_config_auth_scheme_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_CONFIG_INITIAL_AUTH_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CONFIG_INITIAL_AUTH_NONE_V01 = 0x00, /**<  None  */
  IMS_SETTINGS_CONFIG_INITIAL_AUTH_AUTHORIZATION_V01 = 0x01, /**<  Authorization  */
  IMS_SETTINGS_CONFIG_INITIAL_AUTH_PROXY_AUTHORIZATION_V01 = 0x02, /**<  Proxy authorization  */
  IMS_SETTINGS_CONFIG_INITIAL_AUTH_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_config_initial_auth_type_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_REGMGR_CONFIG_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_REGMGR_CONFIG_IETF_V01 = 0x00, /**<  IETF Configuration mode */
  IMS_SETTINGS_REGMGR_CONFIG_EARLY_IMS_V01 = 0x01, /**<  Early IMS Configuration mode */
  IMS_SETTINGS_REGMGR_CONFIG_IMS_V01 = 0x02, /**<  IMS Configuration mode */
  IMS_SETTINGS_REGMGR_CONFIG_IMS_NO_IPSEC_V01 = 0x03, /**<  IMS No IPSec Configuration mode */
  IMS_SETTINGS_REGMGR_CONFIG_IMS_NONE_V01 = 0x04, /**<  No configuration mode */
  IMS_SETTINGS_REGMGR_CONFIG_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_regmgr_config_mode_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_QIPCALL_VT_QUALITY_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_VT_QUALITY_LEVEL_0_V01 = 0x00, /**<  VT quality selector level 0  */
  IMS_SETTINGS_VT_QUALITY_LEVEL_1_V01 = 0x01, /**<  VT quality selector level 1  */
  IMS_SETTINGS_VT_QUALITY_LEVEL_2_V01 = 0x02, /**<  VT quality selector level 2  */
  IMS_SETTINGS_QIPCALL_VT_QUALITY_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_qipcall_vt_quality_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_IP_ADDR_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_IP_TYPE_UNKNOWN_V01 = 0x00, /**<  Unknown IP address type \n  */
  IMS_SETTINGS_IP_TYPE_IPV4_V01 = 0x01, /**<  IPv4 address \n  */
  IMS_SETTINGS_IP_TYPE_IPV6_V01 = 0x02, /**<  IPv6 address \n  */
  IMS_SETTINGS_IP_TYPE_IPV4V6_V01 = 0x04, /**<  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n  */
  IMS_SETTINGS_IP_TYPE_IPV6V4_V01 = 0x05, /**<  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n  */
  IMS_SETTINGS_IP_TYPE_IPV4V6PIP_V01 = 0x06, /**<  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n  */
  IMS_SETTINGS_IP_TYPE_IPV6V4PIP_V01 = 0x07, /**<  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online  */
  IMS_SETTINGS_IP_ADDR_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_ip_addr_type_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_WFC_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01 = 0, /**<  WFC is not supported  */
  IMS_SETTINGS_WFC_STATUS_ON_V01 = 1, /**<  WFC is enabled  */
  IMS_SETTINGS_WFC_STATUS_OFF_V01 = 2, /**<  WFC is disabled  */
  IMS_SETTINGS_WFC_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_wfc_status_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_WFC_PREFERENCE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_WFC_CALL_PREF_NONE_V01 = 0, /**<  None  */
  IMS_SETTINGS_WFC_WLAN_PREFERRED_V01 = 1, /**<  WLAN preferred mode  */
  IMS_SETTINGS_WFC_WLAN_ONLY_V01 = 2, /**<  WLAN only mode  */
  IMS_SETTINGS_WFC_CELLULAR_PREFERRED_V01 = 3, /**<  Cellular preferred mode  */
  IMS_SETTINGS_WFC_CELLULAR_ONLY_V01 = 4, /**<  Cellular only mode  */
  IMS_SETTINGS_WFC_PREFERENCE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_wfc_preference_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_WFC_ROAMING_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_WFC_ROAMING_NOT_SUPPORTED_V01 = 0, /**<  WFC roaming is not supported  */
  IMS_SETTINGS_WFC_ROAMING_ENABLED_V01 = 1, /**<  WFC roaming is enabled  */
  IMS_SETTINGS_WFC_ROAMING_DISABLED_V01 = 2, /**<  WFC roaming is disabled  */
  IMS_SETTINGS_WFC_ROAMING_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_wfc_roaming_enum_v01;
/**
    @}
  */

/**  Enumeration for all the IMS Settings service specific Response 
     messages.

  SMS configuration format enumeration to specify the format of
     the SMS.
 
  H.264 profile values enumeration to specify the profile of
     the H.264 codec.
  
  H.264 level values enumeration to specify the level of
      the H.264 codec.
  
  Video resolution values enumeration to specify the resolution of
     the IMS video.
  
  Video codec values enumeration to specify the codec used for
     the IMS video.
  
  Authentication scheme values enumeration to specify the authentication 
     scheme followed.
  
  Initial authorization type values enumeration to specify the 
     authorization type.
  
  REGMGR CONFIG MODE values enumeration to specify the registration 
     configuration mode.
  
  VT quality selector values enumeration to specify the video quality in a 
     VT call.
  
  IP address version type.
  
 Wi-Fi call (WFC) values enumeration to specifiy the WFC status.
  
  WFC values enumeration to specifiy the preference mode.
  
  WFC values enumeration to specifiy the roaming mode.
  
  Bitmask values for IPSec integrity algorithm.
   */
typedef uint64_t ims_settings_integ_algo_mask_v01;
#define IMS_SETTINGS_INTEG_ALGO_HMAC_SHA_1_96_V01 ((ims_settings_integ_algo_mask_v01)0x01ull) /**<  HMAC-SHA-1-96 algorithm is used for IPSec integrity  */
#define IMS_SETTINGS_INTEG_ALGO_HMAC_MD5_96_V01 ((ims_settings_integ_algo_mask_v01)0x02ull) /**<  HMAC-MD5-96 algorithm is used for IPSec integrity  */
/**   Bitmask values for IPSec encryption algorithm.
   */
typedef uint64_t ims_settings_encrypt_algo_mask_v01;
#define IMS_SETTINGS_ENCRYPT_ALGO_NULL_V01 ((ims_settings_encrypt_algo_mask_v01)0x01ull) /**<  NULL algorithm is used for IPSec encryption  */
#define IMS_SETTINGS_ENCRYPT_ALGO_AES_CBC_V01 ((ims_settings_encrypt_algo_mask_v01)0x02ull) /**<  AES-CBC algorithm is used for IPSec encryption  */
#define IMS_SETTINGS_ENCRYPT_ALGO_DES_EDE3_CBC_V01 ((ims_settings_encrypt_algo_mask_v01)0x04ull) /**<  DES-EDE3-CBC algorithm is used for IPSec encryption  */
/**   Bitmask values to indicate the service type.
   */
typedef uint64_t ims_settings_service_type_mask_v01;
#define IMS_SETTINGS_SERVICE_TYPE_VOLTE_V01 ((ims_settings_service_type_mask_v01)0x01ull) /**<  Bitmask to indicate the VoLTE service.  */
#define IMS_SETTINGS_SERVICE_TYPE_VT_V01 ((ims_settings_service_type_mask_v01)0x02ull) /**<  Bitmask to indicate the Video Telephony service.  */
#define IMS_SETTINGS_SERVICE_TYPE_SMS_V01 ((ims_settings_service_type_mask_v01)0x04ull) /**<  Bitmask to indicate the SMS service.  */
#define IMS_SETTINGS_SERVICE_TYPE_IM_V01 ((ims_settings_service_type_mask_v01)0x08ull) /**<  Bitmask to indicate the Instant Messaging service.  */
#define IMS_SETTINGS_SERVICE_TYPE_VS_V01 ((ims_settings_service_type_mask_v01)0x10ull) /**<  Bitmask to indicate the Video Share service.  */
#define IMS_SETTINGS_SERVICE_TYPE_IS_V01 ((ims_settings_service_type_mask_v01)0x20ull) /**<  Bitmask to indicate the Image Share service.  */
#define IMS_SETTINGS_SERVICE_TYPE_MSRP_V01 ((ims_settings_service_type_mask_v01)0x40ull) /**<  Bitmask to indicate the MSRP service.  */
#define IMS_SETTINGS_SERVICE_TYPE_GL_V01 ((ims_settings_service_type_mask_v01)0x80ull) /**<  Bitmask to indicate the Geo-Location service.  */
#define IMS_SETTINGS_SERVICE_TYPE_PRESENCE_V01 ((ims_settings_service_type_mask_v01)0x100ull) /**<  Bitmask to indicate the Presence service.  */
#define IMS_SETTINGS_SERVICE_TYPE_FT_V01 ((ims_settings_service_type_mask_v01)0x200ull) /**<  Bitmask to indicate the File Transfer service.  */
#define IMS_SETTINGS_SERVICE_TYPE_RCS_ALL_V01 ((ims_settings_service_type_mask_v01)0x400ull) /**<  Bitmask to indicate all the RCS services.  */
#define IMS_SETTINGS_SERVICE_TYPE_SM_V01 ((ims_settings_service_type_mask_v01)0x800ull) /**<  Bitmask to indicate the StandaloneMessaging Infromation service.  */
#define IMS_SETTINGS_SERVICE_TYPE_FT_TN_V01 ((ims_settings_service_type_mask_v01)0x1000ull) /**<  Bitmask to indicate the File transfer Thumbnail support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_FT_SF_V01 ((ims_settings_service_type_mask_v01)0x2000ull) /**<  Bitmask to indicate the File transfer Store and Forward support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_FT_HTTP_V01 ((ims_settings_service_type_mask_v01)0x4000ull) /**<  Bitmask to indicate the File transfer HTTP support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_DEFAULT_V01 ((ims_settings_service_type_mask_v01)0x8000ull) /**<  Bitmask to indicate the default services. If default service is enabled, operator mode will take preference.  */
#define IMS_SETTINGS_SERVICE_TYPE_VS_DURINGCS_V01 ((ims_settings_service_type_mask_v01)0x10000ull) /**<  Bitmask to indicate the Video sharing during CS call support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_SP_V01 ((ims_settings_service_type_mask_v01)0x20000ull) /**<  Bitmask to indicate the Social presence support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_CPD_V01 ((ims_settings_service_type_mask_v01)0x40000ull) /**<  Bitmask to indicate the Capability discovery via presence support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_GL_FT_V01 ((ims_settings_service_type_mask_v01)0x80000ull) /**<  Bitmask to indicate the Geo location Push support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_GL_PULL_V01 ((ims_settings_service_type_mask_v01)0x100000ull) /**<  Bitmask to indicate the Geo location Pull using File Transfer support information service.  */
#define IMS_SETTINGS_SERVICE_TYPE_FULL_GC_V01 ((ims_settings_service_type_mask_v01)0x200000ull) /**<  Bitmask to indicate the Full Store and Forward Group Chat information service.  */
/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_AUDIO_OFFLOAD_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_AUDIO_OFFLOAD_AP_V01 = 1, /**<  Audio offload to AP  */
  IMS_SETTINGS_AUDIO_OFFLOAD_NONE_V01 = 2, /**<  No audio offload  */
  IMS_SETTINGS_AUDIO_OFFLOAD_MODEM_V01 = 3, /**<  Audio offload to MODEM  */
  IMS_SETTINGS_AUDIO_OFFLOAD_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_audio_offload_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_HANDOVER_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_HO_ENABLED_ALL_V01 = 1, /**<  Handover is enabled from WWAN to WLAN and WLAN to WWAN  */
  IMS_SETTINGS_HO_DISABLED_ALL_V01 = 2, /**<  Handover is disabled from WWAN to WLAN and WLAN to WWAN   */
  IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY_V01 = 3, /**<  Handover is only enabled from WLAN to WWAN  */
  IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY_V01 = 4, /**<  Handover is only enabled from WWAN to WLAN  */
  IMS_SETTINGS_HO_ENABLED_IWLAN_TO_WWAN_ONLY_V01 = 5, /**<  Handover is only enabled from IWLAN to WWAN  */
  IMS_SETTINGS_HO_ENABLED_WWAN_TO_IWLAN_ONLY_V01 = 6, /**<  Handover is only enabled from WWAN to IWLAN  */
  IMS_SETTINGS_HANDOVER_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_handover_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_CODEC_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CODEC_AMR_V01 = 0x00, /**<  AMR codec  */
  IMS_SETTINGS_CODEC_G711_V01 = 0x01, /**<  G711 codec  */
  IMS_SETTINGS_CODEC_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_codec_enum_v01;
/**
    @}
  */

/**   Audio offload option enumeration to specify the offload option.      
 
  IMS PDN handover option enumeration to specify if handover is enabled between WWAN and WLAN.      
 
 Codec values enumeration to specify the codec used.
  
 Bitmask values for OMADM services.
  */
typedef uint64_t ims_settings_omadm_services_type_mask_v01;
#define IMS_SETTINGS_OMADM_SERVICES_BIT_MASK_NONE_V01 ((ims_settings_omadm_services_type_mask_v01)0x0ull) /**<  NO service enable  */
#define IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01 ((ims_settings_omadm_services_type_mask_v01)0x1ull) /**<  Service enable  */
#define IMS_SETTINGS_OMADM_SERVICES_VOLTE_ENABLE_BIT_MASK_V01 ((ims_settings_omadm_services_type_mask_v01)0x2ull) /**<  VoLTE service enable  */
#define IMS_SETTINGS_OMADM_SERVICES_VT_ENABLE_BIT_MASK_V01 ((ims_settings_omadm_services_type_mask_v01)0x4ull) /**<  VT service enable  */
#define IMS_SETTINGS_OMADM_SERVICES_PRESENCE_ENABLE_BIT_MASK_V01 ((ims_settings_omadm_services_type_mask_v01)0x8ull) /**<  Presence service enable  */
#define IMS_SETTINGS_OMADM_SERVICES_VOWIFI_ENABLE_BIT_MASK_V01 ((ims_settings_omadm_services_type_mask_v01)0x10ull) /**<  VoWiFi service enable  */
/**  Bitmask values for AMR NB values.
  */
typedef uint64_t ims_settings_amr_nb_codec_mode_type_mask_v01;
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_0_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x0ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_1_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x1ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_2_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x2ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_3_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x4ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_4_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x8ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_5_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x10ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_6_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x20ull) 
#define IMS_SETTINGS_AMR_NB_CODEC_MODE_7_BIT_MASK_V01 ((ims_settings_amr_nb_codec_mode_type_mask_v01)0x40ull) 
/**  AMR 4.75 kpbs. 
 AMR 5.15 kpbs. 
 AMR 5.90 kpbs. 
 AMR 6.70 kpbs. 
 AMR 7.40 kpbs. 
 AMR 7.95 kpbs. 
 AMR 10.20 kpbs. 
 AMR 12.10 kpbs. 
 Bitmask values for AMR WB values.
  */
typedef uint64_t ims_settings_amr_wb_codec_mode_type_mask_v01;
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_8_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x0ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_9_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x1ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_10_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x2ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_11_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x4ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_12_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x8ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_13_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x10ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_14_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x20ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_15_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x40ull) 
#define IMS_SETTINGS_AMR_WB_CODEC_MODE_16_BIT_MASK_V01 ((ims_settings_amr_wb_codec_mode_type_mask_v01)0x80ull) 
/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_CALL_MODE_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CALL_MODE_PREFERRED_NONE_V01 = 0, 
  IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01 = 1, 
  IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01 = 2, 
  IMS_SETTINGS_CALL_MODE_PREFERRED_WLAN_ONLY_V01 = 3, 
  IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_ONLY_V01 = 4, 
  IMS_SETTINGS_CALL_MODE_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_call_mode_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_CALL_ID_MODE_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_CALL_ID_MODE_INIT_V01 = 0, 
  IMS_SETTINGS_CALL_ID_MODE_NONE_V01 = 1, 
  IMS_SETTINGS_CALL_ID_MODE_PRESENT_V01 = 2, 
  IMS_SETTINGS_CALL_ID_MODE_RESTRICT_V01 = 3, 
  IMS_SETTINGS_CALL_ID_MODE_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_call_id_mode_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_AMR_NB_CODEC_MODE_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_AMR_NB_CODEC_MODE_0_V01 = 0, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_1_V01 = 1, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_2_V01 = 2, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_3_V01 = 3, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_4_V01 = 4, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_5_V01 = 5, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_6_V01 = 6, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_7_V01 = 7, 
  IMS_SETTINGS_AMR_NB_CODEC_MODE_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_amr_nb_codec_mode_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_AMR_WB_CODEC_MODE_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_AMR_WB_CODEC_MODE_8_V01 = 1, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_9_V01 = 2, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_10_V01 = 3, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_11_V01 = 4, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_12_V01 = 5, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_13_V01 = 6, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_14_V01 = 7, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_15_V01 = 8, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_16_V01 = 9, 
  IMS_SETTINGS_AMR_WB_CODEC_MODE_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_amr_wb_codec_mode_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_QVP_RTP_SDP_BW_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_QVP_RTP_SDP_BW_UNKNOWN_V01 = -1, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_NB_V01 = 0, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_WB_V01 = 1, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_SWB_V01 = 2, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_FB_V01 = 3, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_NB_WB_V01 = 4, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_NB_SWB_V01 = 5, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_NB_FB_V01 = 6, 
  IMS_SETTINGS_QVP_RTP_SDP_BW_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_qvp_rtp_sdp_bw_option_enum_v01;
/**
    @}
  */

/** @addtogroup imss_qmi_enums
    @{
  */
typedef enum {
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_OPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_UNK_V01 = -1, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_0_V01 = 0, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_1_V01 = 1, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_2_V01 = 2, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_3_V01 = 3, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_4_V01 = 4, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_5_V01 = 5, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_6_V01 = 6, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_7_V01 = 7, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_8_V01 = 8, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_9_V01 = 9, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_10_V01 = 10, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_11_V01 = 11, 
  IMS_SETTINGS_EVS_SDP_CODEC_MODE_OPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_settings_evs_sdp_codec_mode_option_enum_v01;
/**
    @}
  */

/**  AMR-WB 6.60 kbps. 
 AMR-WB 8.85 kbps. 
 AMR-WB 12.65 kbps. 
 AMR-WB 14.25 kbps. 
 AMR-WB 15.85 kbps. 
 AMR-WB 18.25 kbps. 
 AMR-WB 19.85 kbps. 
 AMR-WB 23.05 kbps. 
 AMR-WB 23.85 kbps. 
  Call mode preferred enumeration to specify the mode option.
 
  Call ID mode preferred enumeration to specify the mode option.
 
  AMR NB Codec enumeration to specify the codec option.
 
 AMR 4.75 kpbs. 
 AMR 5.15 kpbs. 
 AMR 5.90 kpbs. 
 AMR 6.70 kpbs. 
 AMR 7.40 kpbs. 
 AMR 7.95 kpbs. 
 AMR 10.20 kpbs. 
 AMR 12.10 kpbs. 
  AMR WB Codec enumeration to specify the codec option.
 
  QVP RTP SDP bandwidth specify the codec option.
 
unknown bandwidth 
8 kHz bandwidth 
16 kHz bandwidth 
32 kHz bandwidth 
48 kHz bandwidth 
8-16 kHz bandwidth 
8-32 kHz bandwidth 
8-48 kHz bandwidth 
unknown mode 
 EVS 2.8 kbps 
 EVS 7.2 kbps 
 EVS 8.0 kbps 
 EVS 9.6 kbps 
 EVS 13.2 kbps 
 EVS 16.4 kbps 
 EVS 24.4 kbps 
 EVS 32.0 kbps 
 EVS 48.0 kbps 
 EVS 64.0 kbps 
 EVS 96.0 kbps 
 EVS 128.0 kbps 
 Bitmask Values for the radio access technology (RAT) option.
  */
typedef uint64_t ims_settings_rat_type_mask_v01;
#define IMS_SETTINGS_RAT_MASK_NONE_V01 ((ims_settings_rat_type_mask_v01)0x00ull) 
#define IMS_SETTINGS_RAT_MASK_GPRS_V01 ((ims_settings_rat_type_mask_v01)0x02ull) 
#define IMS_SETTINGS_RAT_MASK_EDGE_V01 ((ims_settings_rat_type_mask_v01)0x04ull) 
#define IMS_SETTINGS_RAT_MASK_WCDMA_V01 ((ims_settings_rat_type_mask_v01)0x08ull) 
#define IMS_SETTINGS_RAT_MASK_WLAN_V01 ((ims_settings_rat_type_mask_v01)0x10ull) 
#define IMS_SETTINGS_RAT_MASK_CDMA_V01 ((ims_settings_rat_type_mask_v01)0x20ull) 
#define IMS_SETTINGS_RAT_MASK_IWLAN_V01 ((ims_settings_rat_type_mask_v01)0x40ull) 
#define IMS_SETTINGS_RAT_MASK_DOR0_V01 ((ims_settings_rat_type_mask_v01)0x80ull) 
#define IMS_SETTINGS_RAT_MASK_DORA_V01 ((ims_settings_rat_type_mask_v01)0x100ull) 
#define IMS_SETTINGS_RAT_MASK_EHRPD_V01 ((ims_settings_rat_type_mask_v01)0x200ull) 
#define IMS_SETTINGS_RAT_MASK_LTE_V01 ((ims_settings_rat_type_mask_v01)0x400ull) 
#define IMS_SETTINGS_RAT_MASK_DORB_V01 ((ims_settings_rat_type_mask_v01)0x800ull) 
#define IMS_SETTINGS_RAT_MASK_EPC_V01 ((ims_settings_rat_type_mask_v01)0x1000ull) 
#define IMS_SETTINGS_RAT_MASK_HSPA_V01 ((ims_settings_rat_type_mask_v01)0x2000ull) 
#define IMS_SETTINGS_RAT_MASK_W_HSPA_V01 ((ims_settings_rat_type_mask_v01)0x4000ull) 
#define IMS_SETTINGS_RAT_MASK_LTE_LIMITED_V01 ((ims_settings_rat_type_mask_v01)0x8000ull) 
#define IMS_SETTINGS_RAT_MASK_TDSCDMA_V01 ((ims_settings_rat_type_mask_v01)0x800000ull) 
/**  None  
 GPRS  
 EDGE 
 WCDMA 
 WLAN 
 CDMA 1.x 
 IWLAN 
 CDMA DO Rev0 
 CDMA DO RevA 
 CDMA eHRPD 
 LTE 
 CDMA DO RevB 
 EPC 
 HSPA 
 W+HSPA 
 LTE limited 
 TDSCDMA.  */
typedef uint64_t ims_settings_ho_disabled_mask_v01;
#define IMS_SETTINGS_HO_DISABLED_NONE_V01 ((ims_settings_ho_disabled_mask_v01)0x00ull) 
#define IMS_SETTINGS_HO_DISABLED_IWLAN_TO_LTE_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x01ull) 
#define IMS_SETTINGS_HO_DISABLED_IWLAN_TO_3G_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x02ull) 
#define IMS_SETTINGS_HO_DISABLED_IWLAN_TO_2G_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x04ull) 
#define IMS_SETTINGS_HO_DISABLED_IWLAN_TO_EHRPD_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x08ull) 
#define IMS_SETTINGS_HO_DISABLED_LTE_TO_IWLAN_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x0100ull) 
#define IMS_SETTINGS_HO_DISABLED_3G_TO_IWLAN_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x0200ull) 
#define IMS_SETTINGS_HO_DISABLED_2G_TO_IWLAN_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x0400ull) 
#define IMS_SETTINGS_HO_DISABLED_EHPRD_TO_IWLAN_ONLY_V01 ((ims_settings_ho_disabled_mask_v01)0x0800ull) 
/** @addtogroup imss_qmi_aggregates
    @{
  */
/**  0x0001 to 0x00FF for IWLAN_To_WWAN 
 0x0100 to 0xFF00 for WWAN_To_IWLAN 
 Structures required by IMS service specific messages.
 
 */
typedef struct {

  uint16_t rat;
  /**<   RAT. */

  uint8_t apn_type_apn_index;
  /**<   APN type APN index. */

  uint16_t service_mask;
  /**<   Service mask. */

  uint8_t auth_type_security_type;
  /**<   Authentication type security type. */

  uint8_t ip_type_info;
  /**<   IP type info. */
}ims_settings_pol_man_rat_apn_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
/**  Structures required by IMS service specific messages
 
 */
typedef struct {

  uint16_t rat;
  /**<   RAT. */

  uint8_t apn_type_apn_index;
  /**<   APN type APN index. */

  uint8_t auth_type_security_type;
  /**<   Authentication type security type. */

  uint8_t ip_type_info;
  /**<   IP type info. */

  ims_settings_service_type_mask_v01 service_mask_extend;
  /**<   Service mask extended. */
}ims_settings_pol_man_rat_apn_info_v02_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t pol_mgr_rat_apn_fallback;
  /**<   Sequence of the fallback APN for a specific RAT. */

  uint16_t pol_mgr_service_priority_wwan;
  /**<   Priority of a specific service on WWAN over WLAN. */
}ims_settings_pol_mgr_rat_apn_fb_sp_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
typedef struct {

  char pol_mgr_apn_name[IMS_SETTINGS_POL_MGR_APN_NAME_STR_LEN_V01 + 1];
  /**<   Access point. */
}ims_settings_pol_mgr_apn_name_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ipsec_enable_valid;
  /**<   Enable/disable IPSec.
  */

  uint8_t ipsec_enable;
  /**<   IPSec value.
  */
}ims_settings_pol_mgr_rat_ipsec_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
/**  Structures required by IMS service-specific messages.
 
 */
typedef struct {

  ims_settings_codec_enum_v01 codec_info;
  /**<   Codec information. */

  uint8_t codec_profile_number;
  /**<   Profile number for the codec. */

  uint8_t pay_load_number;
  /**<   Payload number. */
}ims_settings_csd_codec_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t qdj_optimization2_enabled;
  /**<   QDJ Optimization 2 enabled flag. */

  uint16_t qdj_go_through_threshold;
  /**<   QDJ go through threshold value, in frames. This threshold is only used
       when QDJ Optimization 2 is enabled. */

  uint16_t qdj_drop_threshold;
  /**<   QDJ drop threshold: the maximum delay in the frame, in milliseconds.
       This threshold is only used when QDJ Optimization 2 is enabled.    */
}ims_settings_qdj_optimization2_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t dynamic_pt_mode0;
  /**<   Dynamic payload type for non-interleaved mode. \n
       Valid range of values: 96 to 127. */

  uint16_t dynamic_pt_mode1;
  /**<   Dynamic payload type for interleaved mode. \n
       Valid range of values: 96 to 127. */
}ims_settings_vt_h264_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_aggregates
    @{
  */
/**  Structures required by RTT Data
 
 */
typedef struct {

  uint16_t total_message_size;
  /**<   Total message size received from UE
    */

  uint16_t current_message_sequence_number;
  /**<   Current message sequence number
    */

  uint16_t highest_message_sequence_number;
  /**<   Highest message sequence number
    */
}ims_settings_rtt_optional_data_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS Session Initiation Protocol (SIP) configuration 
             parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  SIP Port Number */
  uint8_t sip_local_port_valid;  /**< Must be set to true if sip_local_port is being passed */
  uint16_t sip_local_port;
  /**<   Primary call session control function SIP port number.  */

  /* Optional */
  /*  Timer SIP Registration */
  uint8_t timer_sip_reg_valid;  /**< Must be set to true if timer_sip_reg is being passed */
  uint32_t timer_sip_reg;
  /**<   Initial SIP registration duration, in seconds, from the 
         User Equipment (UE). */

  /* Optional */
  /*  Subscribe Timer */
  uint8_t subscribe_timer_valid;  /**< Must be set to true if subscribe_timer is being passed */
  uint32_t subscribe_timer;
  /**<   Duration, in seconds, of the subscription by the UE for IMS 
         registration notifications. */

  /* Optional */
  /*  Timer T1   */
  uint8_t timer_t1_valid;  /**< Must be set to true if timer_t1 is being passed */
  uint32_t timer_t1;
  /**<   RTT estimate, in milliseconds. */

  /* Optional */
  /*  Timer T2  */
  uint8_t timer_t2_valid;  /**< Must be set to true if timer_t2 is being passed */
  uint32_t timer_t2;
  /**<   Maximum retransmit interval, in milliseconds, for non-invite requests 
         and invite responses. */

  /* Optional */
  /*  Timer TF  */
  uint8_t timer_tf_valid;  /**< Must be set to true if timer_tf is being passed */
  uint32_t timer_tf;
  /**<   Non-invite transaction timeout timer, in milliseconds.  */

  /* Optional */
  /*  Sigcomp Status */
  uint8_t sigcomp_enabled_valid;  /**< Must be set to true if sigcomp_enabled is being passed */
  uint8_t sigcomp_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Timer TJ */
  uint8_t timer_tj_valid;  /**< Must be set to true if timer_tj is being passed */
  uint16_t timer_tj;
  /**<   Wait time, in milliseconds, for the non-invite request retransmission. 
         If the value exceeds the range of uint16, it is set to 0xFFFF. 
    */

  /* Optional */
  /*  Timer TJ Extended */
  uint8_t timer_tj_ext_valid;  /**< Must be set to true if timer_tj_ext is being passed */
  uint32_t timer_tj_ext;
  /**<   Wait time, in milliseconds, for the non-invite request 
         retransmission. */

  /* Optional */
  /*  Keep Alive Status */
  uint8_t keepalive_enabled_valid;  /**< Must be set to true if keepalive_enabled is being passed */
  uint8_t keepalive_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable (default)
    */

  /* Optional */
  /*  NAT-RTO Timer Value */
  uint8_t nat_rto_timer_valid;  /**< Must be set to true if nat_rto_timer is being passed */
  uint32_t nat_rto_timer;
  /**<   Request timeout value, in milliseconds, used in NAT implementation. 
         Default value is 500. 
    */

  /* Optional */
  /*  SIP_TIMER_OPERATOR_MODE_A Timer Value */
  uint8_t sip_timer_operator_mode_a_valid;  /**< Must be set to true if sip_timer_operator_mode_a is being passed */
  uint32_t sip_timer_operator_mode_a;
  /**<   SIP timer operator mode A, in seconds; valid range of values is 
         0 to 30. If this TLV is not included in the request, a value of 
         6 seconds is used.
    */

  /* Optional */
  /*  SIP Timer B Value */
  uint8_t timer_tb_value_valid;  /**< Must be set to true if timer_tb_value is being passed */
  uint32_t timer_tb_value;
  /**<   SIP timer B's value, in milliseconds. If this TLV is not included in 
         the request, a value of 0 is used.
    */

  /* Optional */
  /*  SIP GRUU Support Enable Flag */
  uint8_t gruu_enabled_valid;  /**< Must be set to true if gruu_enabled is being passed */
  uint8_t gruu_enabled;
  /**<   SIP Globally Routable User-Agent URI (GRUU) support enable flag.
         If this TLV is not included in the request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Transport Protocol Switch Support */
  uint8_t transport_switch_enabled_valid;  /**< Must be set to true if transport_switch_enabled is being passed */
  uint8_t transport_switch_enabled;
  /**<   SIP transport protocol switching support enable flag per 
         \hyperref[RFC3261]{RFC 3261}.  If this TLV is not included in the 
         request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Maximum TCP Transport Backoff Timer Value */
  uint8_t tcp_max_backoff_timer_value_valid;  /**< Must be set to true if tcp_max_backoff_timer_value is being passed */
  uint32_t tcp_max_backoff_timer_value;
  /**<   Maximum timeout, in milliseconds, for TCP transport of SIP packets 
         after which SIP packets are sent via UDP. If this TLV is not included 
         in the request, a value of 10000 (that is, 10 seconds) is used.
    */

  /* Optional */
  /*  SIP GZIP Decoding Outbuffer Multiplier Value */
  uint8_t gzip_decoding_outbuffer_multiplier_valid;  /**< Must be set to true if gzip_decoding_outbuffer_multiplier is being passed */
  uint8_t gzip_decoding_outbuffer_multiplier;
  /**<   SIP GZIP decoding outbuffer multiplier, the compression multiplier 
         value. If this TLV is not included in the request, a value of 40 is 
         used.
    */

  /* Optional */
  /*  SIP Timer D Value */
  uint8_t timer_td_value_valid;  /**< Must be set to true if timer_td_value is being passed */
  uint32_t timer_td_value;
  /**<   SIP timer D's value, in milliseconds. 
         Timer D is the wait time for response retransmits of the invite client 
         transactions. If this TLV is not included in the request, a value of 
         130000 (that is, 130 seconds) is used.
    */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   SIP timer T4's value, in milliseconds. 
         Timer T4 is the maximum duration that a SIP message can remain in the 
         network.
    */

  /* Optional */
  /*  SIP Timer A */
  uint8_t timer_ta_value_valid;  /**< Must be set to true if timer_ta_value is being passed */
  uint32_t timer_ta_value;
  /**<   SIP timer A's value, in milliseconds. 
         Timer A is the INVITE request retransmit interval, for UDP only
    */

  /* Optional */
  /*  SIP Timer E */
  uint8_t timer_te_value_valid;  /**< Must be set to true if timer_te_value is being passed */
  uint32_t timer_te_value;
  /**<   SIP timer E's value, in milliseconds. 
         Timer E is the value Non-INVITE request retransmit interval, 
         for UDP only.
    */

  /* Optional */
  /*  SIP Timer G */
  uint8_t timer_tg_value_valid;  /**< Must be set to true if timer_tg_value is being passed */
  uint32_t timer_tg_value;
  /**<   SIP timer G's value, in milliseconds. 
         Timer G is the value of INVITE response retransmit interval.
    */

  /* Optional */
  /*  SIP Timer H */
  uint8_t timer_th_value_valid;  /**< Must be set to true if timer_th_value is being passed */
  uint32_t timer_th_value;
  /**<   SIP timer H's value, in milliseconds. 
         Timer H is the value of wait time for ACK receipt.
    */

  /* Optional */
  /*  SIP Timer I */
  uint8_t timer_ti_value_valid;  /**< Must be set to true if timer_ti_value is being passed */
  uint32_t timer_ti_value;
  /**<   SIP timer I's value, in milliseconds. 
         Timer I is the value of wait time for ACK retransmits.
    */

  /* Optional */
  /*  SIP Timer K */
  uint8_t timer_tk_value_valid;  /**< Must be set to true if timer_tk_value is being passed */
  uint32_t timer_tk_value;
  /**<   SIP timer K's value, in milliseconds. 
         Timer K is the value of wait time for response retransmits.
    */

  /* Optional */
  /*  SIP Timer C */
  uint8_t timer_tc_value_valid;  /**< Must be set to true if timer_tc_value is being passed */
  uint32_t timer_tc_value;
  /**<   SIP timer C's value, in milliseconds. 
         Timer C is the value of wait time for response retransmits.
    */
}ims_settings_set_sip_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS Session Initiation Protocol (SIP) configuration 
             parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.\n
         Values: \n
         - 0 -- No error \n
         - 1 -- Not ready \n
         - 2 -- File not available \n
         - 3 -- Message read failed \n
         - 4 -- Message write failed \n
         - 5 -- Other internal error 
    */
}ims_settings_set_sip_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS registration manager configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Primary Call Session Control Function Port */
  uint8_t regmgr_config_pcscf_port_valid;  /**< Must be set to true if regmgr_config_pcscf_port is being passed */
  uint16_t regmgr_config_pcscf_port;
  /**<   Primary call session control function port. */

  /* Optional */
  /*  CSCF Port */
  uint8_t regmgr_primary_cscf_valid;  /**< Must be set to true if regmgr_primary_cscf is being passed */
  char regmgr_primary_cscf[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   Call session control port, fully qualified domain name. */

  /* Optional */
  /*  IMS Test Mode */
  uint8_t ims_test_mode_enabled_valid;  /**< Must be set to true if ims_test_mode_enabled is being passed */
  uint8_t ims_test_mode_enabled;
  /**<  
        Values: \n
        -TRUE -- Enable, no IMS registration \n
        -FALSE -- Disable, IMS registration is initiated
    */
}ims_settings_set_reg_mgr_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS registration manager configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_reg_mgr_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS SMS configuration parameters for the 
             requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 */

  /* Optional */
  /*  SMS Over IP Network Indication Flag  */
  uint8_t sms_over_ip_network_indication_valid;  /**< Must be set to true if sms_over_ip_network_indication is being passed */
  uint8_t sms_over_ip_network_indication;
  /**<   Values: \n
        -TRUE -- Turn on Mobile-Originated (MO) SMS \n
        -FALSE -- Turn off MO SMS
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   Phone context universal resource identifier. */

  /* Optional */
  /*  SMS PSI String */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_SMS_PSI_LEN_V01 + 1];
  /**<   
        SMS PSI string value.
    */
}ims_settings_set_sms_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS SMS configuration parameters for the 
             requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_sms_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS user configuration parameters for the requesting 
             control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IMS Domain Name */
  uint8_t ims_domain_valid;  /**< Must be set to true if ims_domain is being passed */
  char ims_domain[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   IMS domain name. */
}ims_settings_set_user_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS user configuration parameters for the requesting 
             control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_user_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS Voice over Internet Protocol (VoIP) configuration 
             parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Session Duration */
  uint8_t session_expiry_timer_valid;  /**< Must be set to true if session_expiry_timer is being passed */
  uint16_t session_expiry_timer;
  /**<   Session duration, in seconds. */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value for session timer, in seconds. */

  /* Optional */
  /*  Enable AMR WB  */
  uint8_t amr_wb_enable_valid;  /**< Must be set to true if amr_wb_enable is being passed */
  uint8_t amr_wb_enable;
  /**<   Flag to enable/disable Adaptive Multirate codec (AMR) Wideband (WB) 
         audio. \n
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Enable SCR for AMR */
  uint8_t scr_amr_enable_valid;  /**< Must be set to true if scr_amr_enable is being passed */
  uint8_t scr_amr_enable;
  /**<   Flag to enable/disable Source Controlled Rate (SCR) for 
         AMR narrowband (NB). \n
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable  
    */

  /* Optional */
  /*  Enable SCR for AMR WB */
  uint8_t scr_amr_wb_enable_valid;  /**< Must be set to true if scr_amr_wb_enable is being passed */
  uint8_t scr_amr_wb_enable;
  /**<   Flag to enable/disable SCR for AMR WB audio. \n
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable
    */

  /* Optional */
  /*  AMR NB Modes Allowed */
  uint8_t amr_mode_valid;  /**< Must be set to true if amr_mode is being passed */
  uint8_t amr_mode;
  /**<   Bitmask for AMR NB modes allowed. \n
        Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps
    */

  /* Optional */
  /*  AMR WB Modes Allowed */
  uint8_t amr_wb_mode_valid;  /**< Must be set to true if amr_wb_mode is being passed */
  uint16_t amr_wb_mode;
  /**<   Bitmask for AMR WB modes allowed. \n
        Values: \n
        - 0x1 - 6.60 kbps \n
        - 0x2 - 8.85 kbps \n
        - 0x4 - 12.65 kbps \n
        - 0x8 - 14.25 kbps \n
        - 0x10 - 15.85 kbps \n
        - 0x20 - 18.25 kbps \n
        - 0x40 - 19.85 kbps \n 
        - 0x80 - 23.05 kbps \n
        - 0x100 - 23.85 kbps
    */

  /* Optional */
  /*  AMR Octet Aligned */
  uint8_t amr_octet_align_valid;  /**< Must be set to true if amr_octet_align is being passed */
  uint8_t amr_octet_align;
  /**<    Flag indicating whether the octet is aligned for AMR NB audio. \n
        Values: \n
        -TRUE -- Aligned \n
        -FALSE -- Not aligned, Bandwidth Efficient mode
    */

  /* Optional */
  /*  AMR WB Octet Aligned */
  uint8_t amr_wb_octet_align_valid;  /**< Must be set to true if amr_wb_octet_align is being passed */
  uint8_t amr_wb_octet_align;
  /**<   Flag indicating if the octet is aligned for AMR WB audio. \n
        Values: \n
        -TRUE -- Aligned \n
        -FALSE -- Not aligned, Bandwidth Efficient mode
    */

  /* Optional */
  /*  Ringing Timer  */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint16_t ringing_timer;
  /**<   Duration of the ringing timer, in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
    */

  /* Optional */
  /*  Ringback Timer Duration  */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint16_t ringback_timer;
  /**<   Duration of the ringback timer, in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
    */

  /* Optional */
  /*  RTP/RTCP Inactivity Timer Duration */
  uint8_t rtp_rtcp_inactivity_timer_valid;  /**< Must be set to true if rtp_rtcp_inactivity_timer is being passed */
  uint16_t rtp_rtcp_inactivity_timer;
  /**<   Duration of the RTP/RTCP inactivity timer, in seconds. If no 
         RTP/RTCP packet is received before the expiration of this timer, 
         the call is disconnected.
    */

  /* Optional */
  /*  AMR NB Modes Allowed String */
  uint8_t amr_mode_str_valid;  /**< Must be set to true if amr_mode_str is being passed */
  char amr_mode_str[IMS_SETTINGS_VOIP_AMR_MODE_STR_LEN_V01 + 1];
  /**<   String consisting of AMR NB modes allowed. */

  /* Optional */
  /*  AMR WB Modes Allowed String */
  uint8_t amr_wb_mode_str_valid;  /**< Must be set to true if amr_wb_mode_str is being passed */
  char amr_wb_mode_str[IMS_SETTINGS_VOIP_AMR_WB_MODE_STR_LEN_V01 + 1];
  /**<   String consisting of AMR WB modes allowed. */

  /* Optional */
  /*  VoLTE to 1xRTT Silent Redial Flag  */
  uint8_t voip_silent_redial_enabled_valid;  /**< Must be set to true if voip_silent_redial_enabled is being passed */
  uint8_t voip_silent_redial_enabled;
  /**<   Flag that allows a device to silently redial over 1xRTT.
         If this TLV is not included in the request, a value of TRUE 
         (that is, enabled) is used.
    */

  /* Optional */
  /*  VoIP Preferred RTP Payload Type */
  uint8_t voip_preferred_rtp_payload_type_valid;  /**< Must be set to true if voip_preferred_rtp_payload_type is being passed */
  uint16_t voip_preferred_rtp_payload_type;
  /**<   Values for the VoIP preferred codec mode. Must be set only when
         G.711 support is required in addition to AMR and AMR-WB.\n
         Refer to \color{blue}\href{http://www.iana.org/assignments/rtp-parameters/rtp-parameters.xhtml\#rtp-parameters-1}
         {Real-Time Transport Protocol (RTP) Parameters}~\color{black} 
         for possible values.\n
         If an unsupported codec value is set, CODEC MIME is the default audio 
         codec and the G.711 codec is ignored.
    */

  /* Optional */
  /*  VoIP Configuration Conference Factory URI */
  uint8_t voip_config_confURI_valid;  /**< Must be set to true if voip_config_confURI is being passed */
  char voip_config_confURI[IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01 + 1];
  /**<   VoIP configuration conference factory URI.     */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled (default)
    */
}ims_settings_set_voip_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS Voice over Internet Protocol (VoIP) configuration 
             parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_voip_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS presence-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Publish Expiry Timer    */
  uint8_t publish_expiry_timer_valid;  /**< Must be set to true if publish_expiry_timer is being passed */
  uint32_t publish_expiry_timer;
  /**<   Publish timer, in seconds, when publish is sent on an IMS network using  
         4G radio access technology.    
    */

  /* Optional */
  /*  Publish Extended Expiry Timer */
  uint8_t publish_extended_expiry_timer_valid;  /**< Must be set to true if publish_extended_expiry_timer is being passed */
  uint32_t publish_extended_expiry_timer;
  /**<   Publish extended timer, in seconds, when publish is sent on an IMS 
         network in a non-4G radio access technology or when in Airplane 
         Power-Down mode in a 4G radio access technology.
    */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t minimum_publish_interval_valid;  /**< Must be set to true if minimum_publish_interval is being passed */
  uint32_t minimum_publish_interval;
  /**<   Duration, in seconds, between successive publish requests.
    */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t capability_poll_list_subscription_expiry_timer_valid;  /**< Must be set to true if capability_poll_list_subscription_expiry_timer is being passed */
  uint32_t capability_poll_list_subscription_expiry_timer;
  /**<   Expiry timer value, in seconds, for the list subscription request. */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enable_valid;  /**< Must be set to true if capability_discovery_enable is being passed */
  uint8_t capability_discovery_enable;
  /**<   Flag indicating whether discovery capability is enabled. \n
         Values: \n
         -TRUE -- Presence publishes/subscribes and processes any notification 
                  received. \n
         -FALSE -- Presence does not publish/subscribe and 
                  ignores any notification received */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiration_valid;  /**< Must be set to true if capabilites_cache_expiration is being passed */
  uint32_t capabilites_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved capability is 
         considered valid.
    */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiration_valid;  /**< Must be set to true if availability_cache_expiration is being passed */
  uint32_t availability_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved availability is 
         considered valid. */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Duration of time, in seconds, between successive capability polling. */

  /* Optional */
  /*  Maximum Subscription List Entries */
  uint8_t max_subcription_list_entries_valid;  /**< Must be set to true if max_subcription_list_entries is being passed */
  uint32_t max_subcription_list_entries;
  /**<   Maximum number of entries that can be kept in the list subscription.
    */

  /* Optional */
  /*  VoLTE User Opted In Status */
  uint8_t volte_user_opted_in_status_valid;  /**< Must be set to true if volte_user_opted_in_status is being passed */
  uint8_t volte_user_opted_in_status;
  /**<   Flag indicating whether VoLTE service is accepted by the user. 
         Values: \n
         -TRUE -- Accepted \n
         -FALSE -- Not accepted
    */

  /* Optional */
  /*  Last Published ETAG */
  uint8_t last_publish_etag_valid;  /**< Must be set to true if last_publish_etag is being passed */
  char last_publish_etag[IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01 + 1];
  /**<   Last published ETAG. */

  /* Optional */
  /*  Last Published Time */
  uint8_t last_published_time_valid;  /**< Must be set to true if last_published_time is being passed */
  uint32_t last_published_time;
  /**<   Last published time. */

  /* Optional */
  /*  Last Negotiated Published Expire */
  uint8_t last_negotiated_published_expire_valid;  /**< Must be set to true if last_negotiated_published_expire is being passed */
  uint32_t last_negotiated_published_expire;
  /**<   Last negotiated published expire, in seconds. */

  /* Optional */
  /*  GZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression enabled. 
         Values: \n
         -TRUE -- Enabled \n
         -FALSE -- Disabled
                  */

  /* Optional */
  /*  Presence Notification Wait Duration */
  uint8_t presence_notify_wait_duration_valid;  /**< Must be set to true if presence_notify_wait_duration is being passed */
  uint16_t presence_notify_wait_duration;
  /**<   Presence notification wait duration, in seconds. */

  /* Optional */
  /*  Publish Error Recovery Timer (Deprecated) */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds.
    This TLV is deprecated and is now part of
    QMI_IMS_SETTINGS_SET_ PRESENCE_EXT_CONFIG_REQ.
    */
}ims_settings_set_presence_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS presence-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_presence_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS presence extended-related configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds. */

  /* Optional */
  /*  Publish User Agent */
  uint8_t publish_user_agent_valid;  /**< Must be set to true if publish_user_agent is being passed */
  char publish_user_agent[IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01 + 1];
  /**<   User agent. */
}ims_settings_set_presence_ext_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS presence extended-related configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_presence_ext_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS media-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  H.264 Profile */
  uint8_t h264_profile_valid;  /**< Must be set to true if h264_profile is being passed */
  ims_settings_h264_profile_enum_v01 h264_profile;
  /**<   Profile used for the H.264 codec. Values: \n
      - IMS_SETTINGS_H264_PROFILE_BASELINE (0x00) --  Baseline profile
      - IMS_SETTINGS_H264_PROFILE_MAIN (0x01) --  Main profile
      - IMS_SETTINGS_H264_PROFILE_EXTENDED (0x02) --  Extended profile
      - IMS_SETTINGS_H264_PROFILE_HIGH (0x03) --  High profile*/

  /* Optional */
  /*  H.264 Level */
  uint8_t h264_level_valid;  /**< Must be set to true if h264_level is being passed */
  ims_settings_h264_level_enum_v01 h264_level;
  /**<   Level used for the H.264 codec. Values: \n
      - IMS_SETTINGS_H264_LEVEL1 (0x00) --  Level 1
      - IMS_SETTINGS_H264_LEVEL1B (0x01) --  Level 1b
      - IMS_SETTINGS_H264_LEVEL11 (0x02) --  Level 1.1
      - IMS_SETTINGS_H264_LEVEL12 (0x03) --  Level 1.2
      - IMS_SETTINGS_H264_LEVEL13 (0x04) --  Level 1.3
      - IMS_SETTINGS_H264_LEVEL2 (0x05) --  Level 2
      - IMS_SETTINGS_H264_LEVEL21 (0x06) --  Level 2.1
      - IMS_SETTINGS_H264_LEVEL22 (0x07) --  Level 2.2
      - IMS_SETTINGS_H264_LEVEL3 (0x08) --  Level 3
      - IMS_SETTINGS_H264_LEVEL31 (0x09) --  Level 3.1
      - IMS_SETTINGS_H264_LEVEL32 (0x0A) --  Level 3.2
      - IMS_SETTINGS_H264_LEVEL4 (0x0B) --  Level 4
      - IMS_SETTINGS_H264_LEVEL41 (0x0C) --  Level 4.1
      - IMS_SETTINGS_H264_LEVEL42 (0x0D) --  Level 4.2
      - IMS_SETTINGS_H264_LEVEL5 (0x0E) --  Level 5
      - IMS_SETTINGS_H264_LEVEL51 (0x0F) --  Level 5.1*/

  /* Optional */
  /*  Video Bitrate */
  uint8_t video_bitrate_valid;  /**< Must be set to true if video_bitrate is being passed */
  uint16_t video_bitrate;
  /**<   Bitrate of the video, in kbps. */

  /* Optional */
  /*  Video Refresh Rate */
  uint8_t video_frames_per_second_valid;  /**< Must be set to true if video_frames_per_second is being passed */
  uint8_t video_frames_per_second;
  /**<   Video refresh rate, in frames per second. */

  /* Optional */
  /*  Video Display Resolution  */
  uint8_t video_resolution_valid;  /**< Must be set to true if video_resolution is being passed */
  ims_video_resolution_enum_v01 video_resolution;
  /**<   Resolution of the video display. Values: \n
      - IMS_SETTINGS_SQCIF_RESOLUTION (0x00) --  SQCIF
      - IMS_SETTINGS_QCIF_RESOLUTION (0x01) --  QCIF
      - IMS_SETTINGS_CIF_RESOLUTION (0x02) --  CIF
      - IMS_SETTINGS_QQVGA_RESOLUTION (0x03) --  QQVGA
      - IMS_SETTINGS_QVGA_RESOLUTION (0x04) --  QVGA
      - IMS_SETTINGS_VGA_RESOLUTION (0x05) --  VGA */

  /* Optional */
  /*  Video Codec */
  uint8_t video_codec_valid;  /**< Must be set to true if video_codec is being passed */
  ims_settings_video_codec_enum_v01 video_codec;
  /**<   Codec used for the video. Values: \n
      - IMS_SETTINGS_CODEC_MPEG4_XVID (0x00) --  XVID MPEG4 codec
      - IMS_SETTINGS_CODEC_MPEG4_ISO (0x01) --  ISO MPEG4 codec
      - IMS_SETTINGS_CODEC_H263 (0x02) --  H.263 codec
      - IMS_SETTINGS_CODEC_H264 (0x03) --  H.264 codec*/
}ims_settings_set_media_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS media-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_media_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS QIPCall-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  VT Calling Status */
  uint8_t vt_calling_enabled_valid;  /**< Must be set to true if vt_calling_enabled is being passed */
  uint8_t vt_calling_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Emergency Call Timer */
  uint8_t emerg_call_timer_valid;  /**< Must be set to true if emerg_call_timer is being passed */
  uint32_t emerg_call_timer;
  /**<   Emergency call timer.
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a videotelephony (VT) call. If this 
 TLV is not present in the request, a value of 
 IMS_SETTINGS_VT_QUALITY_LEVEL_0 (that is, high quality) is used. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for a speech codec.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_end_port_valid;  /**< Must be set to true if speech_end_port is being passed */
  uint16_t speech_end_port;
  /**<   
         Largest RTP port number for a speech codec.
    */

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Default Encoding Mode */
  uint8_t amr_default_mode_valid;  /**< Must be set to true if amr_default_mode is being passed */
  uint8_t amr_default_mode;
  /**<   
        AMR default encoding mode.
    */

  /* Optional */
  /*  Minimum Video RTP Port Range */
  uint8_t video_rtp_port_start_valid;  /**< Must be set to true if video_rtp_port_start is being passed */
  uint16_t video_rtp_port_start;
  /**<   
        Minimum video RTP port range.   
    */

  /* Optional */
  /*  Maximum Video RTP Port Range */
  uint8_t video_rtp_port_end_valid;  /**< Must be set to true if video_rtp_port_end is being passed */
  uint16_t video_rtp_port_end;
  /**<   
        Maximum video RTP port range.   
    */

  /* Optional */
  /*  Emergency Call Setup Timer on WLAN */
  uint8_t emerg_call_timer_wlan_valid;  /**< Must be set to true if emerg_call_timer_wlan is being passed */
  uint32_t emerg_call_timer_wlan;
  /**<   Emergency call setup timer on WLAN.
    */
}ims_settings_set_qipcall_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS QIPCall-related configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_qipcall_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS registration manager extended configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Reregistration Delay */
  uint8_t reregistration_delay_valid;  /**< Must be set to true if reregistration_delay is being passed */
  uint16_t reregistration_delay;
  /**<  IMS reregistration wait time when RAT transitions from eHRPD to LTE, 
        in seconds.*/

  /* Optional */
  /*  Delay Length for iRAT Transition (Deprecated) */
  uint8_t t_delay_valid;  /**< Must be set to true if t_delay is being passed */
  uint16_t t_delay;
  /**<   Delay length for an Inter-Radio Access Technology (iRAT) transition, 
         in seconds; allowed integer value range is 0 to 600. If this TLV is 
         not present in the request, a value of 0 is used.
        
        Note: This TLV is deprecated; it was a duplicate.
        Use the Reregistration Delay TLV instead. 
    */

  /* Optional */
  /*  RegRetryBaseTime */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   RegRetryBaseTime value, in seconds. 
         RegRetryBaseTime is the value of the base-time parameter of the 
         flow recovery algorithm.
    */

  /* Optional */
  /*  RegRetryMaxTime */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   RegRetryMaxTime value, in seconds. 
         RegRetryMaxTime is the value of the max-time parameter of the 
         flow recovery algorithm.
    */
}ims_settings_set_reg_mgr_extended_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS registration manager extended configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_reg_mgr_extended_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS policy manager configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Policy Manager RAT APN Information Array */
  uint8_t pol_mgr_rat_apn_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_info is being passed */
  ims_settings_pol_man_rat_apn_info_v01 pol_mgr_rat_apn_info[IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01];

  /* Optional */
  /*  Policy Manager RAT APN Fallback and Service Priority Information Array */
  uint8_t pol_mgr_rat_apn_fb_sp_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_fb_sp_info is being passed */
  ims_settings_pol_mgr_rat_apn_fb_sp_info_v01 pol_mgr_rat_apn_fb_sp_info[IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01];

  /* Optional */
  /*  Policy Manager Allowed Services Over WLAN */
  uint8_t pol_mgr_allowed_services_wlan_valid;  /**< Must be set to true if pol_mgr_allowed_services_wlan is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_allowed_services_wlan;
  /**<   Bitmask indicating which services are allowed over WLAN. */

  /* Optional */
  /*  Policy Manager Add All Feature Tags */
  uint8_t pol_mgr_add_all_fts_valid;  /**< Must be set to true if pol_mgr_add_all_fts is being passed */
  uint8_t pol_mgr_add_all_fts;
  /**<   Indicates whether to add all feature tag list or application. */

  /* Optional */
  /*  Policy Manager ACS Priority */
  uint8_t pol_mgr_acs_priority_valid;  /**< Must be set to true if pol_mgr_acs_priority is being passed */
  uint8_t pol_mgr_acs_priority;
  /**<  Priority of ACS values. */

  /* Optional */
  /*  Policy Manager ISIM Priority */
  uint8_t pol_mgr_isim_priority_valid;  /**< Must be set to true if pol_mgr_isim_priority is being passed */
  uint8_t pol_mgr_isim_priority;
  /**<  Priority of ISIM values. */

  /* Optional */
  /*  Policy Manager NV Priority */
  uint8_t pol_mgr_nv_priority_valid;  /**< Must be set to true if pol_mgr_nv_priority is being passed */
  uint8_t pol_mgr_nv_priority;
  /**<  Priority of preconfiguration NV values. */

  /* Optional */
  /*  Policy Manager PCO Priority */
  uint8_t pol_mgr_pco_priority_valid;  /**< Must be set to true if pol_mgr_pco_priority is being passed */
  uint8_t pol_mgr_pco_priority;
  /**<  Priority of PCO values. */

  /* Optional */
  /*  Policy Manager IMS Service Priority */
  uint8_t pol_mgr_ims_service_status_valid;  /**< Must be set to true if pol_mgr_ims_service_status is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_ims_service_status;
  /**<   Bitmask indicating the services that are enabled on the device. */

  /* Optional */
  /*  Policy Manager Access Point Name List */
  uint8_t pol_mgr_apn_name_valid;  /**< Must be set to true if pol_mgr_apn_name is being passed */
  ims_settings_pol_mgr_apn_name_v01 pol_mgr_apn_name[IMS_SETTINGS_POL_MGR_APN_SIZE_V01];
}ims_settings_set_pol_mgr_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS policy manager configuration parameters 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_pol_mgr_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS RCS standalone messaging configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  RCS Standalone Messaging Authorization */
  uint8_t standalone_message_auth_valid;  /**< Must be set to true if standalone_message_auth is being passed */
  uint8_t standalone_message_auth;
  /**<   Values:\n
         - TRUE - Authorized\n
         - FALSE - Unauthorized
    */

  /* Optional */
  /*  RCS Standalone Message Maximum Size */
  uint8_t standalone_message_max_size_valid;  /**< Must be set to true if standalone_message_max_size is being passed */
  uint32_t standalone_message_max_size;
  /**<   Maximum size of a standalone message.
    */

  /* Optional */
  /*  RCS Standalone Message Explorer URI */
  uint8_t standalone_message_explorer_uri_valid;  /**< Must be set to true if standalone_message_explorer_uri is being passed */
  char standalone_message_explorer_uri[IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01 + 1];
  /**<   Standalone message explorer URI.
    */
}ims_settings_set_rcs_sm_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS RCS standalone messaging configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_rcs_sm_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS Ut Interface configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Disable Ut Interface Status */
  uint8_t disable_ut_valid;  /**< Must be set to true if disable_ut is being passed */
  uint8_t disable_ut;
  /**<   Values:  \n
         - TRUE -- Disable  \n
         - FALSE -- Enable (default) \n
         If this TLV is not included in the request, a value of FALSE 
         (that is, Enable) is used.
    */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t ut_apn_name_valid;  /**< Must be set to true if ut_apn_name is being passed */
  char ut_apn_name[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut Interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type */
  uint8_t ut_ip_addr_type_valid;  /**< Must be set to true if ut_ip_addr_type is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type;
  /**<   Ut Interface IP address type. If this TLV is not present in the 
 request, a value of IMS_SETTINGS_IP_TYPE_ UNKNOWN is used. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */

  /* Optional */
  /*  Ut Interface Access Point Name for Wi-Fi */
  uint8_t ut_apn_name_apn2_valid;  /**< Must be set to true if ut_apn_name_apn2 is being passed */
  char ut_apn_name_apn2[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type for Wi-Fi */
  uint8_t ut_ip_addr_type_apn2_valid;  /**< Must be set to true if ut_ip_addr_type_apn2 is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type_apn2;
  /**<   Ut interface IP address type. If this TLV is not present in the 
 request, a value of IMS_SETTINGS_IP_TYPE_ UNKNOWN is used. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */
}ims_settings_set_ut_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS Ut Interface configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_ut_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS client provisioning configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Enable Client Provisioning */
  uint8_t enable_client_provisioning_valid;  /**< Must be set to true if enable_client_provisioning is being passed */
  uint8_t enable_client_provisioning;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable (default) \n
         If this TLV is not included in the request, a value of FALSE 
         (that is, Disable) is used.
    */

  /* Optional */
  /*  Enable VoLTE Support Through Client Provisioning */
  uint8_t enable_volte_valid;  /**< Must be set to true if enable_volte is being passed */
  uint8_t enable_volte;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable (default) \n
         If this TLV is not included in the request, a value of FALSE 
         (that is, Disable) is used.
    */

  /* Optional */
  /*  Enable VT Support Through Client Provisioning */
  uint8_t enable_vt_valid;  /**< Must be set to true if enable_vt is being passed */
  uint8_t enable_vt;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable (default) \n
         If this TLV is not included in the request, a value of FALSE 
         (that is, Disable) is used.
    */

  /* Optional */
  /*  Enable Presence Support Through Client Provisioning */
  uint8_t enable_presence_valid;  /**< Must be set to true if enable_presence is being passed */
  uint8_t enable_presence;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable (default) \n
         If this TLV is not included in the request, a value of  FALSE 
         (that is, Disable) is used.
    */

  /* Optional */
  /*  Wi-Fi Call Setting */
  uint8_t wifi_call_valid;  /**< Must be set to true if wifi_call is being passed */
  ims_settings_wfc_status_enum_v01 wifi_call;
  /**<   Wi-Fi Call (WFC) status. Values: \n
      - IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED (0) --  WFC is not supported 
      - IMS_SETTINGS_WFC_STATUS_ON (1) --  WFC is enabled 
      - IMS_SETTINGS_WFC_STATUS_OFF (2) --  WFC is disabled 
 */

  /* Optional */
  /*  Wi-Fi Call Preference Setting */
  uint8_t wifi_call_preference_valid;  /**< Must be set to true if wifi_call_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_preference;
  /**<   WFC home preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */

  /* Optional */
  /*  Wi-Fi Call Roaming Setting */
  uint8_t wifi_call_roaming_valid;  /**< Must be set to true if wifi_call_roaming is being passed */
  ims_settings_wfc_roaming_enum_v01 wifi_call_roaming;
  /**<   WFC roaming mode. Values: \n
      - IMS_SETTINGS_WFC_ROAMING_NOT_SUPPORTED (0) --  WFC roaming is not supported 
      - IMS_SETTINGS_WFC_ROAMING_ENABLED (1) --  WFC roaming is enabled 
      - IMS_SETTINGS_WFC_ROAMING_DISABLED (2) --  WFC roaming is disabled 
 */

  /* Optional */
  /*  Enable VoWiFi Support Through Client Provisioning */
  uint8_t enable_vowifi_valid;  /**< Must be set to true if enable_vowifi is being passed */
  uint8_t enable_vowifi;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable (default) \n
         If this TLV is not included in the request, a value of  FALSE 
         (that is, Disable) is used.
    */

  /* Optional */
  /*  VoWiFi Provisioning ID */
  uint8_t vowifi_provId_valid;  /**< Must be set to true if vowifi_provId is being passed */
  char vowifi_provId[IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01 + 1];
  /**<   Standalone message of the VoWiFi provisioning ID.
    */

  /* Optional */
  /*  WFC Roam Preference */
  uint8_t wifi_call_roam_preference_valid;  /**< Must be set to true if wifi_call_roam_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_roam_preference;
  /**<   WFC roaming preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */
}ims_settings_set_client_provisioning_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS client provisioning configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_client_provisioning_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the APCS_COMPLETE status 
             for the requesting control point. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_set_apcs_complete_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the APCS_COMPLETE status 
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_apcs_complete_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS VT configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  H.263 Payload Type in 3G */
  uint8_t vt_3g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_3g_h263_dynamic_pt is being passed */
  uint16_t vt_3g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 3G. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  H.263 Payload Type in 4G */
  uint8_t vt_4g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_4g_h263_dynamic_pt is being passed */
  uint16_t vt_4g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 4G. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  Number of 4G H.264 Configurations */
  uint8_t num_vt_4g_h264_config_valid;  /**< Must be set to true if num_vt_4g_h264_config is being passed */
  uint8_t num_vt_4g_h264_config;
  /**<   
        Number of currently available H.264 configurations. This TLV is used to
        update the H.264 dynamic payload. If this TLV does not match the current
        number of available H.264 configurations, it returns an error message.
    */

  /* Optional */
  /*  4G H.264 Configuration Information */
  uint8_t vt_4g_h264_info_valid;  /**< Must be set to true if vt_4g_h264_info is being passed */
  ims_settings_vt_h264_info_v01 vt_4g_h264_info[IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01];
  /**<   \n(Array of H.264 information parameters in 4G.) */
}ims_settings_set_vt_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS VT configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_vt_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the RCS configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Enable RCS Messaging Service Status  */
  uint8_t enable_rcs_messaging_valid;  /**< Must be set to true if enable_rcs_messaging is being passed */
  uint8_t enable_rcs_messaging;
  /**<   
        RCS messaging service status. Values: \n
        - TRUE -- Enable \n
        - FALSE -- Disable (default)
        */
}ims_settings_set_acs_network_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the RCS configuration parameters
             for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_acs_network_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS handover configuration parameters for the requesting 
             control point. */
typedef struct {

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to WWAN */
  uint8_t wlan_to_wwan_hys_timer_valid;  /**< Must be set to true if wlan_to_wwan_hys_timer is being passed */
  uint32_t wlan_to_wwan_hys_timer;
  /**<   IMS handover hysteresis timer for WLAN to WWAN, in seconds.
	     This timer is started if the LTE to WLAN idle mode handover
	     occurs because of LTE RAT loss. When in cellular-preferred mode, WLAN
	     to LTE handover only occurs after LTE is available for this period
	     of time. If this value is not present, a default of 120 sec is
	     used.
	*/

  /* Optional */
  /*  Voice Services Handover Hysteresis Timer WLAN to 1X */
  uint8_t wlan_to_1x_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_hys_timer is being passed */
  uint32_t wlan_to_1x_hys_timer;
  /**<   Hysteresis timer for moving voice services from WLAN to 1X, in seconds.
	     This timer is started if a 1X system is used for voice service and
		 1X quality drops below a threshold in idle mode. When in
		 cellular-preferred mode, voice service  moves to 1X only after 1X is
		 available with improved quality for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN to WLAN */
  uint8_t wwan_to_wlan_hys_timer_valid;  /**< Must be set to true if wwan_to_wlan_hys_timer is being passed */
  uint32_t wwan_to_wlan_hys_timer;
  /**<   IMS handover hysteresis timer for WWAN to WLAN, in seconds.
	     In Wi-Fi preferred mode, this timer is started if a WLAN to LTE idle
	     mode handover occurs because of WLAN RAT loss. An LTE to WLAN handover
	     only occurs after WLAN is available for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Enabled Between WWAN and WLAN */
  uint8_t ims_pdn_ho_enabled_valid;  /**< Must be set to true if ims_pdn_ho_enabled is being passed */
  ims_settings_handover_option_enum_v01 ims_pdn_ho_enabled;
  /**<   IMS handover mode. Values: \n
      - IMS_SETTINGS_HO_ENABLED_ALL (1) --  Handover is enabled from WWAN to WLAN and WLAN to WWAN 
      - IMS_SETTINGS_HO_DISABLED_ALL (2) --  Handover is disabled from WWAN to WLAN and WLAN to WWAN  
      - IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY (3) --  Handover is only enabled from WLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY (4) --  Handover is only enabled from WWAN to WLAN 
      - IMS_SETTINGS_HO_ENABLED_IWLAN_TO_WWAN_ONLY (5) --  Handover is only enabled from IWLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_IWLAN_ONLY (6) --  Handover is only enabled from WWAN to IWLAN 
 */

  /* Optional */
  /*  IMS 1x RSSI Threshold */
  uint8_t iIMSHO1xQualTH_valid;  /**< Must be set to true if iIMSHO1xQualTH is being passed */
  int32_t iIMSHO1xQualTH;
  /**<   This value defines the 1x threshold value for comparing averaged signal strength.
         Scenario: If the signal quality of 1x falls below iIMSHO1xQualTH and Wi-Fi is available, 
         the device deregisters from 1x and connects to IMS PDN over Wi-Fi for voice calls. \n
		 Default value --> -7 db
	*/

  /* Optional */
  /*  IMS LTE Threshold1 */
  uint8_t iIMSHOLTEQualTH1_valid;  /**< Must be set to true if iIMSHOLTEQualTH1 is being passed */
  int32_t iIMSHOLTEQualTH1;
  /**<   This value defines LTE Threshold 1. 
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -117 db
	*/

  /* Optional */
  /*  IMS LTE Threshold2 */
  uint8_t iIMSHOLTEQualTH2_valid;  /**< Must be set to true if iIMSHOLTEQualTH2 is being passed */
  int32_t iIMSHOLTEQualTH2;
  /**<   This value defines LTE Threshold 2.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice only. \n
		 Default value --> -115 db
	*/

  /* Optional */
  /*  IMS LTE Threshold3  */
  uint8_t iIMSHOLTEQualTH3_valid;  /**< Must be set to true if iIMSHOLTEQualTH3 is being passed */
  int32_t iIMSHOLTEQualTH3;
  /**<   This value defines LTE Threshold 3.     
         Handover scenario: Wi-Fi to LTE and LTE to Wi-Fi.
         Used for In-call mode for voice only. \n
		 Default value --> -110 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdA */
  uint8_t iIMSHOWIFIQualTHA_valid;  /**< Must be set to true if iIMSHOWIFIQualTHA is being passed */
  int32_t iIMSHOWIFIQualTHA;
  /**<   This value defines Wi-Fi Threshold A.
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -85 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdB */
  uint8_t iIMSHOWIFIQualTHB_valid;  /**< Must be set to true if iIMSHOWIFIQualTHB is being passed */
  int32_t iIMSHOWIFIQualTHB;
  /**<   This value defines Wi-Fi Threshold B.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice and video.
		 Default value --> -95 db
	*/

  /* Optional */
  /*  HO disabled mask */
  uint8_t ho_disabled_mask_valid;  /**< Must be set to true if ho_disabled_mask is being passed */
  ims_settings_ho_disabled_mask_v01 ho_disabled_mask;
  /**<   Mask of Ho disabled
         If not set, Default is disabled with all RATs
    */

  /* Optional */
  /*  IMS LTE RSRQ Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Src is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Src;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -120 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Src is being passed */
  int32_t iIR92_LTE_DL_SNR_Src;
  /**<   This value defines LTE SNR Threshold (scaled dB) when UE handover algorithm is running in IR92 mode.    
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> 95 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS LTE RSRQ Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Target is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Target;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -100 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Target is being passed */
  int32_t iIR92_LTE_DL_SNR_Target;
  /**<   This value defines LTE SNR Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> 105 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS Wifi RSSI Source threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_Out_valid;  /**< Must be set to true if iIR92_Wifi_Rove_Out is being passed */
  int32_t iIR92_Wifi_Rove_Out;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -85 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Target threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_In_valid;  /**< Must be set to true if iIR92_Wifi_Rove_In is being passed */
  int32_t iIR92_Wifi_Rove_In;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.   
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -65 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Repoint threhsold for IR92 */
  uint8_t iIR92_Wifi_Repoint_valid;  /**< Must be set to true if iIR92_Wifi_Repoint is being passed */
  int32_t iIR92_Wifi_Repoint;
  /**<   This value defines Wifi RSSI Threshold (dB) for CS-Repoint for IR92.     
         Handover scenario: Used to page incoming calls over CS damain as Wifi RSSI falls below this value.
         Used for In-call mode
		 Default value --> -75 dB
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP Radio */
  uint8_t wlan_to_3gpp_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp_idle_mode_hys_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving voice services from WLAN to 3GPP RAT in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from UTRAN\GERAN RAT to 
     WLAN during idle mode because of UTRAN\GERAN RAT loss. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP2 Radio */
  uint8_t wlan_to_3gpp2_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp2_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp2_idle_mode_hys_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving voice services from WLAN to 3GPP2 RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from eHRPD\eVDO RAT to 
     WLAN during idle mode because of eHRPD\eVDO RAT loss. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 1x Radio */
  uint8_t wlan_to_1x_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_1x_idle_mode_hys_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving voice services from WLAN to 1x RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN is brought up on WLAN 
     during idle mode because of 1X RAT loss.
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_cell_pref_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving voice services from WLAN to LTE RATs during VOIP call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during VOIP call because of LTE RAT loss. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_wifi_pref_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving voice services from WLAN to LTE RAT during VOIP call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during VOIP call because of WLAN RAT unavailability for IMS. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_cell_pref_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during Video call because of LTE RAT loss. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_wifi_pref_timer;
  /**<   QMI Set request for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during Video call because of WLAN RAT unavailability for IMS. 
     If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN Radio to WLAN */
  uint8_t wwan_to_wlan_all_mode_hysteresis_timer_valid;  /**< Must be set to true if wwan_to_wlan_all_mode_hysteresis_timer is being passed */
  uint32_t wwan_to_wlan_all_mode_hysteresis_timer;
  /**<   
	This timer is started when ims handovers/moves from wifi to any cellular rat (lte, ehrpd, etc) to avoid coming back to wifi 
	immediately and there by avoiding pdn ping-pongs. It is started during idle mode irrespective of user preference (cellular or wifi preference). 
    If value is not present, default value of 120 seconds is used. 
	*/
}ims_settings_set_handover_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS handover configuration parameters for the requesting 
             control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_handover_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS application's VoLTE for Internet Connected Endpoint
             (VICE) configuration parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t qipcall_vice_enabled_valid;  /**< Must be set to true if qipcall_vice_enabled is being passed */
  uint8_t qipcall_vice_enabled;
  /**<   Indicates whether VICE is enabled. */
}ims_settings_set_qipcall_vice_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS application's VoLTE for Internet Connected Endpoint
             (VICE) configuration parameters for the requesting control point. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_qipcall_vice_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS policy manager configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  IPSec Enabled */
  uint8_t rat_ipsec_enabled_valid;  /**< Must be set to true if rat_ipsec_enabled is being passed */
  ims_settings_pol_mgr_rat_ipsec_info_v01 rat_ipsec_enabled[IMS_SETTINGS_POL_MGR_RAT_MAX_V01];
  /**<   Indicates whether IPSec is enabled. */
}ims_settings_set_pol_mgr_rat_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS policy manager configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_pol_mgr_rat_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the SIP configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_sip_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the SIP configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  SIP Local Port */
  uint8_t sip_local_port_valid;  /**< Must be set to true if sip_local_port is being passed */
  uint16_t sip_local_port;
  /**<   Primary call session control function SIP port number.
    */

  /* Optional */
  /*  SIP Registration Timer */
  uint8_t timer_sip_reg_valid;  /**< Must be set to true if timer_sip_reg is being passed */
  uint32_t timer_sip_reg;
  /**<   Initial SIP registration duration, in seconds, from the UE.
    */

  /* Optional */
  /*  Subscribe Timer */
  uint8_t subscribe_timer_valid;  /**< Must be set to true if subscribe_timer is being passed */
  uint32_t subscribe_timer;
  /**<   Duration, in seconds, of the subscription by the UE for IMS 
         registration notifications.
    */

  /* Optional */
  /*  Timer T1  */
  uint8_t timer_t1_valid;  /**< Must be set to true if timer_t1 is being passed */
  uint32_t timer_t1;
  /**<   RTT estimate, in milliseconds.
    */

  /* Optional */
  /*  Timer T2  */
  uint8_t timer_t2_valid;  /**< Must be set to true if timer_t2 is being passed */
  uint32_t timer_t2;
  /**<   Maximum retransmit interval, in milliseconds, for non-invite requests 
         and invite responses.
    */

  /* Optional */
  /*  Timer TF  */
  uint8_t timer_tf_valid;  /**< Must be set to true if timer_tf is being passed */
  uint32_t timer_tf;
  /**<   Non-invite transaction timeout timer, in milliseconds.
    */

  /* Optional */
  /*  Sigcomp Status */
  uint8_t sigcomp_enabled_valid;  /**< Must be set to true if sigcomp_enabled is being passed */
  uint8_t sigcomp_enabled;
  /**<  
        Values: \n
        -TRUE -- SigComp enabled \n
        -FALSE -- SigComp disabled
    */

  /* Optional */
  /*  Timer TJ */
  uint8_t timer_tj_valid;  /**< Must be set to true if timer_tj is being passed */
  uint16_t timer_tj;
  /**<   Wait time, in milliseconds, for the non-invite request retransmission. 
         If the value exceeds the range of uint16, it is set to 0xFFFF.
    */

  /* Optional */
  /*  Timer TJ Extended */
  uint8_t timer_tj_ext_valid;  /**< Must be set to true if timer_tj_ext is being passed */
  uint32_t timer_tj_ext;
  /**<   Wait time, in milliseconds, for the non-invite request 
         retransmission. */

  /* Optional */
  /*  Keep Alive Status */
  uint8_t keepalive_enabled_valid;  /**< Must be set to true if keepalive_enabled is being passed */
  uint8_t keepalive_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  NAT-RTO Timer Value */
  uint8_t nat_rto_timer_valid;  /**< Must be set to true if nat_rto_timer is being passed */
  uint32_t nat_rto_timer;
  /**<   Requests timeout value, in milliseconds, used in NAT implementation. */

  /* Optional */
  /*  SIP_TIMER_OPERATOR_MODE_A Timer Value */
  uint8_t sip_timer_operator_mode_a_valid;  /**< Must be set to true if sip_timer_operator_mode_a is being passed */
  uint32_t sip_timer_operator_mode_a;
  /**<   SIP timer operator mode A, in seconds; valid range of values is 
         0 to 30. If this TLV is not included in the request, a value of 
         6 seconds is used.
    */

  /* Optional */
  /*  SIP Timer B Value */
  uint8_t timer_tb_value_valid;  /**< Must be set to true if timer_tb_value is being passed */
  uint32_t timer_tb_value;
  /**<   SIP timer B's value, in milliseconds. If this TLV is not included in 
         the request, a value of 0 is used.
    */

  /* Optional */
  /*  SIP GRUU Support Enable Flag */
  uint8_t gruu_enabled_valid;  /**< Must be set to true if gruu_enabled is being passed */
  uint8_t gruu_enabled;
  /**<   SIP GRUU support enable flag.
         If this TLV is not included in the request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Transport Protocol Switch Support */
  uint8_t transport_switch_enabled_valid;  /**< Must be set to true if transport_switch_enabled is being passed */
  uint8_t transport_switch_enabled;
  /**<   SIP transport protocol switching support enable flag per 
         \hyperref[RFC3261]{RFC 3261}.  If this TLV is not included in the 
         request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Maximum TCP Transport Backoff Timer Value */
  uint8_t tcp_max_backoff_timer_value_valid;  /**< Must be set to true if tcp_max_backoff_timer_value is being passed */
  uint32_t tcp_max_backoff_timer_value;
  /**<   Maximum timeout, in milliseconds, for TCP transport of SIP packets 
         after which SIP packets are sent via UDP. If this TLV is not included 
         in the request, a value of 10000 (that is, 10 seconds) is used.
    */

  /* Optional */
  /*  SIP GZIP Decoding Outbuffer Multiplier Value */
  uint8_t gzip_decoding_outbuffer_multiplier_valid;  /**< Must be set to true if gzip_decoding_outbuffer_multiplier is being passed */
  uint8_t gzip_decoding_outbuffer_multiplier;
  /**<   SIP GZIP decoding outbuffer multiplier, the compression multiplier 
         value. If this TLV is not included in the request, a value of 40 is 
         used.
    */

  /* Optional */
  /*  SIP Timer D Value */
  uint8_t timer_td_value_valid;  /**< Must be set to true if timer_td_value is being passed */
  uint32_t timer_td_value;
  /**<   SIP timer D's value, in milliseconds. 
         Timer D is the wait time for response retransmits of the invite client 
         transactions.
    */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   SIP timer T4's value, in milliseconds. 
         Timer T4 is the maximum duration that a SIP message can remain in the 
         network.
    */

  /* Optional */
  /*  SIP Timer A */
  uint8_t timer_ta_value_valid;  /**< Must be set to true if timer_ta_value is being passed */
  uint32_t timer_ta_value;
  /**<   SIP timer A's value, in milliseconds. 
         Timer A is the INVITE request retransmit interval, for UDP only
    */

  /* Optional */
  /*  SIP Timer E */
  uint8_t timer_te_value_valid;  /**< Must be set to true if timer_te_value is being passed */
  uint32_t timer_te_value;
  /**<   SIP timer E's value, in milliseconds. 
         Timer E is the value Non-INVITE request retransmit interval, 
         for UDP only.
    */

  /* Optional */
  /*  SIP Timer G */
  uint8_t timer_tg_value_valid;  /**< Must be set to true if timer_tg_value is being passed */
  uint32_t timer_tg_value;
  /**<   SIP timer G's value, in milliseconds. 
         Timer G is the value of INVITE response retransmit interval.
    */

  /* Optional */
  /*  SIP Timer H */
  uint8_t timer_th_value_valid;  /**< Must be set to true if timer_th_value is being passed */
  uint32_t timer_th_value;
  /**<   SIP timer H's value, in milliseconds. 
         Timer H is the value of wait time for ACK receipt.
    */

  /* Optional */
  /*  SIP Timer I */
  uint8_t timer_ti_value_valid;  /**< Must be set to true if timer_ti_value is being passed */
  uint32_t timer_ti_value;
  /**<   SIP timer I's value, in milliseconds. 
         Timer I is the value of wait time for ACK retransmits.
    */

  /* Optional */
  /*  SIP Timer K */
  uint8_t timer_tk_value_valid;  /**< Must be set to true if timer_tk_value is being passed */
  uint32_t timer_tk_value;
  /**<   SIP timer K's value, in milliseconds. 
         Timer K is the value of wait time for response retransmits.
    */

  /* Optional */
  /*  SIP Timer C */
  uint8_t timer_tc_value_valid;  /**< Must be set to true if timer_tc_value is being passed */
  uint32_t timer_tc_value;
  /**<   SIP timer C's value, in milliseconds. 
         Timer C is the value of wait time for response retransmits.
    */
}ims_settings_get_sip_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the registration manager configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_reg_mgr_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the registration manager configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
       error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Proxy Call Session Control Function Port */
  uint8_t regmgr_config_pcscf_port_valid;  /**< Must be set to true if regmgr_config_pcscf_port is being passed */
  uint16_t regmgr_config_pcscf_port;
  /**<   Proxy CSCF port.*/

  /* Optional */
  /*  Primary CSCF Port */
  uint8_t regmgr_primary_cscf_valid;  /**< Must be set to true if regmgr_primary_cscf is being passed */
  char regmgr_primary_cscf[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   Primary CSCF port, fully qualified domain name.  */

  /* Optional */
  /*  IMS Test Mode */
  uint8_t ims_test_mode_valid;  /**< Must be set to true if ims_test_mode is being passed */
  uint8_t ims_test_mode;
  /**<  
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */
}ims_settings_get_reg_mgr_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the SMS configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_sms_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the SMS configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response.*/

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 */

  /* Optional */
  /*  SMS Over IP Network Indication Flag  */
  uint8_t sms_over_ip_network_indication_valid;  /**< Must be set to true if sms_over_ip_network_indication is being passed */
  uint8_t sms_over_ip_network_indication;
  /**<  
        Values: \n
        - TRUE -- MO SMS turned on \n
        - FALSE -- MO SMS turned off
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   Phone context universal resource identifier. */

  /* Optional */
  /*  SMS PSI String */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_SMS_PSI_LEN_V01 + 1];
  /**<   
        SMS PSI string value.
    */
}ims_settings_get_sms_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the user configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_user_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the user configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. A settings-specific error code is returned when 
         the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  IMS Domain Name */
  uint8_t ims_domain_valid;  /**< Must be set to true if ims_domain is being passed */
  char ims_domain[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   IMS domain name. */
}ims_settings_get_user_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the VoIP configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_voip_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the VoIP configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Session Duration */
  uint8_t session_expiry_timer_valid;  /**< Must be set to true if session_expiry_timer is being passed */
  uint16_t session_expiry_timer;
  /**<   Session duration, in seconds. */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value, in seconds, for session timer. */

  /* Optional */
  /*  Enable AMR WB  */
  uint8_t amr_wb_enable_valid;  /**< Must be set to true if amr_wb_enable is being passed */
  uint8_t amr_wb_enable;
  /**<   Flag indicating AMR WB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  Enable SCR AMR  */
  uint8_t scr_amr_enable_valid;  /**< Must be set to true if scr_amr_enable is being passed */
  uint8_t scr_amr_enable;
  /**<   Flag indicating SCR for AMR NB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  Enable SCR AMR WB */
  uint8_t scr_amr_wb_enable_valid;  /**< Must be set to true if scr_amr_wb_enable is being passed */
  uint8_t scr_amr_wb_enable;
  /**<   Flag indicating SCR for AMR WB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  AMR NB Mode */
  uint8_t amr_mode_valid;  /**< Must be set to true if amr_mode is being passed */
  uint8_t amr_mode;
  /**<   Bitmask indicating AMR NB modes. \n
        Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps 
    */

  /* Optional */
  /*  AMR WB Mode */
  uint8_t amr_wb_mode_valid;  /**< Must be set to true if amr_wb_mode is being passed */
  uint16_t amr_wb_mode;
  /**<   Bitmask indicating AMR WB modes. \n
        Values: \n
        - 0x1 -- 6.60 kbps \n
        - 0x2 -- 8.85 kbps \n
        - 0x4 -- 12.65 kbps \n
        - 0x8 -- 14.25 kbps \n
        - 0x10 -- 15.85 kbps \n
        - 0x20 -- 18.25 kbps \n
        - 0x40 -- 19.85 kbps \n
        - 0x80 -- 23.05 kbps \n
        - 0x100 -- 23.85 kbps 
    */

  /* Optional */
  /*  AMR NB Octet Aligned */
  uint8_t amr_octet_align_valid;  /**< Must be set to true if amr_octet_align is being passed */
  uint8_t amr_octet_align;
  /**<   Flag indicating whether the octet is aligned for AMR NB audio. \n
        Values: \n
        -TRUE -- Octet aligned \n
        -FALSE -- Octet not aligned, Bandwidth Efficient Mode
    */

  /* Optional */
  /*  AMR WB Octet Aligned */
  uint8_t amr_wb_octet_align_valid;  /**< Must be set to true if amr_wb_octet_align is being passed */
  uint8_t amr_wb_octet_align;
  /**<   Flag indicating whether the octet is aligned for AMR WB audio. \n
        Values: \n
        -TRUE -- Octet aligned \n
        -FALSE -- Octet not aligned, Bandwidth Efficient Mode
    */

  /* Optional */
  /*  Ringing Timer Duration */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint16_t ringing_timer;
  /**<   Duration, in seconds, of the ringing timer. The ringing
         timer is started on the ringing event. If the call is not answered
         within the duration of this timer, the call is
         disconnected.
    */

  /* Optional */
  /*  Ringback Timer Duration */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint16_t ringback_timer;
  /**<   Duration, in seconds, of the ringback timer. The ringback
         timer is started on the ringback event. If the call is not 
         answered within the duration of this timer, the call is
         disconnected.
    */

  /* Optional */
  /*  RTP/RTCP Inactivity Timer Duration  */
  uint8_t rtp_rtcp_inactivity_timer_valid;  /**< Must be set to true if rtp_rtcp_inactivity_timer is being passed */
  uint16_t rtp_rtcp_inactivity_timer;
  /**<   Duration, in seconds, of the RTP/RTCP inactivity timer. If no
         RTP/RTCP packet is received before the expiration of this timer, 
         the call is disconnected.
    */

  /* Optional */
  /*  VoLTE to 1xRTT Silent Redial Flag */
  uint8_t voip_silent_redial_enabled_valid;  /**< Must be set to true if voip_silent_redial_enabled is being passed */
  uint8_t voip_silent_redial_enabled;
  /**<   Flag that allows a device to silently redial over 1xRTT.
         If this TLV is not included in the request, a value of TRUE 
         (that is, enabled) is used.*/

  /* Optional */
  /*  VoIP Preferred RTP Payload Type */
  uint8_t voip_preferred_rtp_payload_type_valid;  /**< Must be set to true if voip_preferred_rtp_payload_type is being passed */
  uint16_t voip_preferred_rtp_payload_type;
  /**<   Values for the VoIP preferred codec mode. Must be set only when
         G.711 support is required in addition to AMR and AMR-WB.\n
         Refer to \color{blue}\href{http://www.iana.org/assignments/rtp-parameters/rtp-parameters.xhtml\#rtp-parameters-1}
         {Real-TimeTransport Protocol (RTP) Parameters}~\color{black} 
         for possible values.\n 
         If an unsupported codec value is set, CODEC MIME is used 
         as the default audio codec and the G.711 codec is ignored.
    */

  /* Optional */
  /*  VoIP Configuration Conference Factory URI */
  uint8_t voip_config_confURI_valid;  /**< Must be set to true if voip_config_confURI is being passed */
  char voip_config_confURI[IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01 + 1];
  /**<   VoIP configuration conference factory URI.  */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled
    */
}ims_settings_get_voip_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the presence-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_presence_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the presence-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
       */

  /* Optional */
  /*  Publish Timer */
  uint8_t publish_expiry_timer_valid;  /**< Must be set to true if publish_expiry_timer is being passed */
  uint32_t publish_expiry_timer;
  /**<   Publish timer, in seconds, when publish is sent on an IMS network
         using 4G radio access technology. */

  /* Optional */
  /*  Publish Extended Expiry  */
  uint8_t publish_extended_expiry_timer_valid;  /**< Must be set to true if publish_extended_expiry_timer is being passed */
  uint32_t publish_extended_expiry_timer;
  /**<    Publish extended timer, in seconds, when publish is sent on an IMS 
          network in a non-4G radio access technology, or when in Airplane 
          Power-Down mode in a 4G radio access technology.
     */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t minimum_publish_interval_valid;  /**< Must be set to true if minimum_publish_interval is being passed */
  uint32_t minimum_publish_interval;
  /**<   Duration, in seconds, between successive publish requests. */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t capability_poll_list_subscription_expiry_timer_valid;  /**< Must be set to true if capability_poll_list_subscription_expiry_timer is being passed */
  uint32_t capability_poll_list_subscription_expiry_timer;
  /**<   Expiry timer value, in seconds, for the list subscription request. */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enable_valid;  /**< Must be set to true if capability_discovery_enable is being passed */
  uint8_t capability_discovery_enable;
  /**<   Flag indicating whether or not discovery capability is enabled.
        Values: \n
        -TRUE -- Presence publishes/subscribes and processes 
                 any notifications received \n
        -FALSE -- Presence does not publish/subscribe and ignores 
                  any notification received
    */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiration_valid;  /**< Must be set to true if capabilites_cache_expiration is being passed */
  uint32_t capabilites_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved capability is 
         considered valid. */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiration_valid;  /**< Must be set to true if availability_cache_expiration is being passed */
  uint32_t availability_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved capability is 
         considered valid.  */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Duration of time, in seconds, for which the retrieved capability is 
         considered valid. */

  /* Optional */
  /*  Maximum Subscription List Entries */
  uint8_t max_subcription_list_entries_valid;  /**< Must be set to true if max_subcription_list_entries is being passed */
  uint32_t max_subcription_list_entries;
  /**<    Maximum number of entries that can be kept in the list subscription.*/

  /* Optional */
  /*  VoLTE User Opted In Status */
  uint8_t volte_user_opted_in_status_valid;  /**< Must be set to true if volte_user_opted_in_status is being passed */
  uint8_t volte_user_opted_in_status;
  /**<    Flag indicating whether or not VoLTE service is accepted by the user.
          Values: \n
          -TRUE -- Accepted \n
          -FALSE -- Not accepted
    */

  /* Optional */
  /*  Last Published Entity Tag */
  uint8_t last_publish_etag_valid;  /**< Must be set to true if last_publish_etag is being passed */
  char last_publish_etag[IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01 + 1];
  /**<   Last published Entity Tag (ETAG). */

  /* Optional */
  /*  Last Published Time */
  uint8_t last_published_time_valid;  /**< Must be set to true if last_published_time is being passed */
  uint32_t last_published_time;
  /**<   Last published time. */

  /* Optional */
  /*  Last Negotiated Published Expire */
  uint8_t last_negotiated_published_expire_valid;  /**< Must be set to true if last_negotiated_published_expire is being passed */
  uint32_t last_negotiated_published_expire;
  /**<   Last negotiated published expire, in seconds. */

  /* Optional */
  /*  GNU ZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression is enabled. 
         Values: \n
         -TRUE -- Enabled \n
         -FALSE -- Disabled
                  */

  /* Optional */
  /*  Presence Notification Wait Duration */
  uint8_t presence_notify_wait_duration_valid;  /**< Must be set to true if presence_notify_wait_duration is being passed */
  uint16_t presence_notify_wait_duration;
  /**<   Presence notification wait duration, in seconds. */

  /* Optional */
  /*  Publish Error Recovery Timer (Deprecated) */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds.
         This TLV is deprecated and is now part of
         QMI_IMS_SETTINGS_GET_ PRESENCE_EXT_CONFIG_REQ.
    */
}ims_settings_get_presence_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the presence extended-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_presence_ext_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the presence extended-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
           described in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds. */

  /* Optional */
  /*  Publish User Agent */
  uint8_t publish_user_agent_valid;  /**< Must be set to true if publish_user_agent is being passed */
  char publish_user_agent[IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01 + 1];
  /**<   User agent. */
}ims_settings_get_presence_ext_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the media-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_media_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the media-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  H.264 Profile */
  uint8_t h264_profile_valid;  /**< Must be set to true if h264_profile is being passed */
  ims_settings_h264_profile_enum_v01 h264_profile;
  /**<   Profile used for H.264 codec. Values: \n
      - IMS_SETTINGS_H264_PROFILE_BASELINE (0x00) --  Baseline profile
      - IMS_SETTINGS_H264_PROFILE_MAIN (0x01) --  Main profile
      - IMS_SETTINGS_H264_PROFILE_EXTENDED (0x02) --  Extended profile
      - IMS_SETTINGS_H264_PROFILE_HIGH (0x03) --  High profile */

  /* Optional */
  /*  H.264 Level */
  uint8_t h264_level_valid;  /**< Must be set to true if h264_level is being passed */
  ims_settings_h264_level_enum_v01 h264_level;
  /**<   Level used for H.264 codec. Values: \n
      - IMS_SETTINGS_H264_LEVEL1 (0x00) --  Level 1
      - IMS_SETTINGS_H264_LEVEL1B (0x01) --  Level 1b
      - IMS_SETTINGS_H264_LEVEL11 (0x02) --  Level 1.1
      - IMS_SETTINGS_H264_LEVEL12 (0x03) --  Level 1.2
      - IMS_SETTINGS_H264_LEVEL13 (0x04) --  Level 1.3
      - IMS_SETTINGS_H264_LEVEL2 (0x05) --  Level 2
      - IMS_SETTINGS_H264_LEVEL21 (0x06) --  Level 2.1
      - IMS_SETTINGS_H264_LEVEL22 (0x07) --  Level 2.2
      - IMS_SETTINGS_H264_LEVEL3 (0x08) --  Level 3
      - IMS_SETTINGS_H264_LEVEL31 (0x09) --  Level 3.1
      - IMS_SETTINGS_H264_LEVEL32 (0x0A) --  Level 3.2
      - IMS_SETTINGS_H264_LEVEL4 (0x0B) --  Level 4
      - IMS_SETTINGS_H264_LEVEL41 (0x0C) --  Level 4.1
      - IMS_SETTINGS_H264_LEVEL42 (0x0D) --  Level 4.2
      - IMS_SETTINGS_H264_LEVEL5 (0x0E) --  Level 5
      - IMS_SETTINGS_H264_LEVEL51 (0x0F) --  Level 5.1 */

  /* Optional */
  /*  Video Bitrate */
  uint8_t video_bitrate_valid;  /**< Must be set to true if video_bitrate is being passed */
  uint16_t video_bitrate;
  /**<   Bitrate of the video, in kbps. */

  /* Optional */
  /*  Video Refresh Rate */
  uint8_t video_frames_per_second_valid;  /**< Must be set to true if video_frames_per_second is being passed */
  uint8_t video_frames_per_second;
  /**<   Video refresh rate, in frames per second. */

  /* Optional */
  /*  Video Display Resolution  */
  uint8_t video_resolution_valid;  /**< Must be set to true if video_resolution is being passed */
  ims_video_resolution_enum_v01 video_resolution;
  /**<   Resolution of the video display. Values: \n
      - IMS_SETTINGS_SQCIF_RESOLUTION (0x00) --  SQCIF
      - IMS_SETTINGS_QCIF_RESOLUTION (0x01) --  QCIF
      - IMS_SETTINGS_CIF_RESOLUTION (0x02) --  CIF
      - IMS_SETTINGS_QQVGA_RESOLUTION (0x03) --  QQVGA
      - IMS_SETTINGS_QVGA_RESOLUTION (0x04) --  QVGA
      - IMS_SETTINGS_VGA_RESOLUTION (0x05) --  VGA  */

  /* Optional */
  /*  Video Codec */
  uint8_t video_codec_valid;  /**< Must be set to true if video_codec is being passed */
  ims_settings_video_codec_enum_v01 video_codec;
  /**<   Codec used for the video. Values: \n
      - IMS_SETTINGS_CODEC_MPEG4_XVID (0x00) --  XVID MPEG4 codec
      - IMS_SETTINGS_CODEC_MPEG4_ISO (0x01) --  ISO MPEG4 codec
      - IMS_SETTINGS_CODEC_H263 (0x02) --  H.263 codec
      - IMS_SETTINGS_CODEC_H264 (0x03) --  H.264 codec */

  /* Optional */
  /*  Lipsync Drop Upper Limit  */
  uint8_t lipsync_drop_upper_limit_valid;  /**< Must be set to true if lipsync_drop_upper_limit is being passed */
  uint16_t lipsync_drop_upper_limit;
  /**<   Lipsync drop upper limit in units of video samples for video clock rate
         of 90kHz. */

  /* Optional */
  /*  Lipsync Drop Lower Limit  */
  uint8_t lipsync_drop_lower_limit_valid;  /**< Must be set to true if lipsync_drop_lower_limit is being passed */
  uint16_t lipsync_drop_lower_limit;
  /**<   Lipsync drop lower limit in units of video samples for video clock rate
         of 90kHz. */

  /* Optional */
  /*  RTP MTU Size  */
  uint8_t rtp_mtu_size_valid;  /**< Must be set to true if rtp_mtu_size is being passed */
  uint16_t rtp_mtu_size;
  /**<   RTP Maximum Transmission Unit (MTU) size. */

  /* Optional */
  /*  QDJ Time Warping Enable Option */
  uint8_t qdj_time_warping_enabled_valid;  /**< Must be set to true if qdj_time_warping_enabled is being passed */
  uint8_t qdj_time_warping_enabled;
  /**<   Qualcomm Dejitter buffer (QDJ) time warping. Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  QDJ IBA Maximum Value */
  uint8_t qdj_iba_max_valid;  /**< Must be set to true if qdj_iba_max is being passed */
  uint8_t qdj_iba_max;
  /**<   Maximum number of chances given to a frame, which decides underflow in 
         QDJ. While dequeuing a frame with sequence number x, this value is
         the maximum number of times to wait and look for x before moving 
         to the next frame (x+1) dequeue. 
    */

  /* Optional */
  /*  QDJ Maximum Frames to Start Dequeue */
  uint8_t qdj_max_frames_at_start_valid;  /**< Must be set to true if qdj_max_frames_at_start is being passed */
  uint8_t qdj_max_frames_at_start;
  /**<   Number of frames required in QDJ to start a dequeue. */

  /* Optional */
  /*  QDJ Maximum Dejitter Delay */
  uint8_t qdj_max_delay_valid;  /**< Must be set to true if qdj_max_delay is being passed */
  uint8_t qdj_max_delay;
  /**<   Maximum QDJ dejitter delay, in milliseconds. */

  /* Optional */
  /*  QDJ Minimum Dejitter Delay */
  uint8_t qdj_min_delay_valid;  /**< Must be set to true if qdj_min_delay is being passed */
  uint8_t qdj_min_delay;
  /**<   Minimum QDJ dejitter delay, in milliseconds. */

  /* Optional */
  /*  QDJ Optimization2 Information */
  uint8_t qdj_optimization2_info_valid;  /**< Must be set to true if qdj_optimization2_info is being passed */
  ims_settings_qdj_optimization2_info_v01 qdj_optimization2_info;

  /* Optional */
  /*  QDJ Maximum Frames at Run */
  uint8_t qdj_max_frames_at_run_valid;  /**< Must be set to true if qdj_max_frames_at_run is being passed */
  uint8_t qdj_max_frames_at_run;
  /**<   Maximum number of frames to keep in
         the queue. The oldest frame is dropped if received at frame
         (this value+1).
     */

  /* Optional */
  /*  QDJ Maximum Bumped Delay */
  uint8_t qdj_max_bumped_up_delay_valid;  /**< Must be set to true if qdj_max_bumped_up_delay is being passed */
  uint8_t qdj_max_bumped_up_delay;
  /**<   QDJ maximum bumped delay, in milliseconds. 
         This is QDJ-specific and used to decide whether to change the 
         maximum target delay if the underflow is too large. The target delay 
         varies from the minimum target delay to the maximum target delay. 
         However, in certain extreme conditions where the underflow is huge or 
         frequent packet bundling occurs, QDJ bumps up the target delay to 
         a value higher than the maximum delay, but not greater than 
         the maximum bumped delay. 
    */

  /* Optional */
  /*  QDJ Jitter Increment */
  uint8_t qdj_jitter_increment_valid;  /**< Must be set to true if qdj_jitter_increment is being passed */
  uint8_t qdj_jitter_increment;
  /**<   QDJ step delay, in milliseconds. This value is used when updating QDJ 
         for each talk spurt. */

  /* Optional */
  /*  QDJ Target Underflow Rate */
  uint8_t qdj_target_underflow_valid;  /**< Must be set to true if qdj_target_underflow is being passed */
  uint16_t qdj_target_underflow;
  /**<   Percentage of QDJ underflow, multiplied by 1000. */

  /* Optional */
  /*  QDJ Drop Threshold */
  uint8_t qdj_default_jitter_valid;  /**< Must be set to true if qdj_default_jitter is being passed */
  uint16_t qdj_default_jitter;
  /**<   QDJ default jitter: the initial default jitter, in milliseconds, to be 
         added in QDJ play out. 
    */

  /* Optional */
  /*  Gmin */
  uint8_t gmin_valid;  /**< Must be set to true if gmin is being passed */
  uint8_t gmin;
  /**<   Number of frames in a run that defines a gap and burst matrices in 
         RTCP XR report per \hyperref[RFC3611]{RFC 3611}.
    */

  /* Optional */
  /*  Transmit System Delay */
  uint8_t tx_system_delay_valid;  /**< Must be set to true if tx_system_delay is being passed */
  uint8_t tx_system_delay;
  /**<   Tx system delay, in milliseconds, that is used to calculate the end 
         system delay in an RTCP XR report.
    */

  /* Optional */
  /*  Receive System Delay */
  uint8_t rx_system_delay_valid;  /**< Must be set to true if rx_system_delay is being passed */
  uint8_t rx_system_delay;
  /**<   Rx system delay, in milliseconds, that is used to calculate the end 
         system delay in an RTCP XR report.
    */

  /* Optional */
  /*  Audio Offload */
  uint8_t audio_offload_valid;  /**< Must be set to true if audio_offload is being passed */
  ims_settings_audio_offload_option_enum_v01 audio_offload;
  /**<   Audio offload option. Values: \n
      - IMS_SETTINGS_AUDIO_OFFLOAD_AP (1) --  Audio offload to AP 
      - IMS_SETTINGS_AUDIO_OFFLOAD_NONE (2) --  No audio offload 
      - IMS_SETTINGS_AUDIO_OFFLOAD_MODEM (3) --  Audio offload to MODEM  */
}ims_settings_get_media_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the QIPCall-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_qipcall_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the QIPCall-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  VT Calling Status */
  uint8_t vt_calling_enabled_valid;  /**< Must be set to true if vt_calling_enabled is being passed */
  uint8_t vt_calling_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Emergency Call Timer */
  uint8_t emerg_call_timer_valid;  /**< Must be set to true if emerg_call_timer is being passed */
  uint32_t emerg_call_timer;
  /**<   Emergency call timer.
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a VT call. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for the speech codec.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_end_port_valid;  /**< Must be set to true if speech_end_port is being passed */
  uint16_t speech_end_port;
  /**<   
         Largest RTP port number for the speech codec.
    */

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband.
    */

  /* Optional */
  /*  AMR Default Encoding Mode */
  uint8_t amr_default_mode_valid;  /**< Must be set to true if amr_default_mode is being passed */
  uint8_t amr_default_mode;
  /**<   
        AMR default encoding mode.
    */

  /* Optional */
  /*  Minimum Video RTP Port Range */
  uint8_t video_rtp_port_start_valid;  /**< Must be set to true if video_rtp_port_start is being passed */
  uint16_t video_rtp_port_start;
  /**<   
        Minimum video RTP port range. 
    */

  /* Optional */
  /*  Maximum Video RTP Port Range */
  uint8_t video_rtp_port_end_valid;  /**< Must be set to true if video_rtp_port_end is being passed */
  uint16_t video_rtp_port_end;
  /**<   
        Maximum video RTP port range.  
    */

  /* Optional */
  /*  Emergency Call Setup Timer on WLAN */
  uint8_t emerg_call_timer_wlan_valid;  /**< Must be set to true if emerg_call_timer_wlan is being passed */
  uint32_t emerg_call_timer_wlan;
  /**<   Emergency call setup timer on WLAN.
    */
}ims_settings_get_qipcall_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the registration manager extended configuration. (Deprecated)
             parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_reg_mgr_extended_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the registration manager extended configuration. (Deprecated)
             parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
       error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Reregistration Delay */
  uint8_t reregistration_delay_valid;  /**< Must be set to true if reregistration_delay is being passed */
  uint16_t reregistration_delay;
  /**<  IMS reregistration wait time when RAT transitions from eHRPD to LTE, in 
        seconds.*/

  /* Optional */
  /*  Delay Length for iRAT Transition (Deprecated) */
  uint8_t t_delay_valid;  /**< Must be set to true if t_delay is being passed */
  uint16_t t_delay;
  /**<   The length of the delay for an iRAT transition, in seconds; 
         allowed integer value range is 0 to 600. If this 
         TLV is not present in the request, a value of 0 is used.
         
        Note: This TLV is deprecated and was a duplicate.
        Use the Reregistration Delay TLV instead. 
    */

  /* Optional */
  /*  RegRetryBaseTime */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   RegRetryBaseTime value, in seconds. 
         RegRetryBaseTime is the value of the base-time parameter of the 
         flow recovery algorithm.
    */

  /* Optional */
  /*  RegRetryMaxTime */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   RegRetryMaxTime value, in seconds. 
         RegRetryMaxTime is the value of the max-time parameter of the 
         flow recovery algorithm.
    */
}ims_settings_get_reg_mgr_extended_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the policy manager configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_pol_mgr_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the policy manager configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
       error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Policy Manager RAT APN Information Array */
  uint8_t pol_mgr_rat_apn_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_info is being passed */
  ims_settings_pol_man_rat_apn_info_v01 pol_mgr_rat_apn_info[IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01];

  /* Optional */
  /*  Policy Manager RAT APN Fallback and Service Priority Information Array */
  uint8_t pol_mgr_rat_apn_fb_sp_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_fb_sp_info is being passed */
  ims_settings_pol_mgr_rat_apn_fb_sp_info_v01 pol_mgr_rat_apn_fb_sp_info[IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01];

  /* Optional */
  /*  Policy Manager Allowed Services Over WLAN */
  uint8_t pol_mgr_allowed_services_wlan_valid;  /**< Must be set to true if pol_mgr_allowed_services_wlan is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_allowed_services_wlan;
  /**<   Bitmask indicating which services are allowed over WLAN. */

  /* Optional */
  /*  Policy Manager Add All Feature Tags */
  uint8_t pol_mgr_add_all_fts_valid;  /**< Must be set to true if pol_mgr_add_all_fts is being passed */
  uint8_t pol_mgr_add_all_fts;
  /**<   Indicates whether to add all the feature tag list or application. */

  /* Optional */
  /*  Policy Manager ACS Priority */
  uint8_t pol_mgr_acs_priority_valid;  /**< Must be set to true if pol_mgr_acs_priority is being passed */
  uint8_t pol_mgr_acs_priority;
  /**<   Priority of ACS values. */

  /* Optional */
  /*  Policy Manager ISIM Priority */
  uint8_t pol_mgr_isim_priority_valid;  /**< Must be set to true if pol_mgr_isim_priority is being passed */
  uint8_t pol_mgr_isim_priority;
  /**<   Priority of ISIM values. */

  /* Optional */
  /*  Policy Manager NV Priority */
  uint8_t pol_mgr_nv_priority_valid;  /**< Must be set to true if pol_mgr_nv_priority is being passed */
  uint8_t pol_mgr_nv_priority;
  /**<   Priority of preconfiguration NV values. */

  /* Optional */
  /*  Policy Manager PCO Priority */
  uint8_t pol_mgr_pco_priority_valid;  /**< Must be set to true if pol_mgr_pco_priority is being passed */
  uint8_t pol_mgr_pco_priority;
  /**<   Priority of PCO values. */

  /* Optional */
  /*  Policy Manager IMS Service Priority */
  uint8_t pol_mgr_ims_service_status_valid;  /**< Must be set to true if pol_mgr_ims_service_status is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_ims_service_status;
  /**<   Bitmask indicating which services are enabled on the device. */

  /* Optional */
  /*  Policy Manager Access Point Name List */
  uint8_t pol_mgr_apn_name_valid;  /**< Must be set to true if pol_mgr_apn_name is being passed */
  ims_settings_pol_mgr_apn_name_v01 pol_mgr_apn_name[IMS_SETTINGS_POL_MGR_APN_SIZE_V01];
  /**<   \n(IMS access point names.) */

  /* Optional */
  /*  Policy Manager RAT APN Extended Info Array */
  uint8_t pol_mgr_rat_apn_info_extend_valid;  /**< Must be set to true if pol_mgr_rat_apn_info_extend is being passed */
  ims_settings_pol_man_rat_apn_info_v02_v01 pol_mgr_rat_apn_info_extend[IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01];
}ims_settings_get_pol_mgr_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the RCS standalone messaging configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_rcs_sm_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the RCS standalone messaging configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  RCS Standalone Messaging Authorization */
  uint8_t standalone_message_auth_valid;  /**< Must be set to true if standalone_message_auth is being passed */
  uint8_t standalone_message_auth;
  /**<   Values: \n
         - TRUE - Authorized\n
         - FALSE - Unauthorized
    */

  /* Optional */
  /*  RCS Standalone Message Maximum Size */
  uint8_t standalone_message_max_size_valid;  /**< Must be set to true if standalone_message_max_size is being passed */
  uint32_t standalone_message_max_size;
  /**<   Maximum size of a standalone message.
    */

  /* Optional */
  /*  RCS Standalone Message Explorer URI */
  uint8_t standalone_message_explorer_uri_valid;  /**< Must be set to true if standalone_message_explorer_uri is being passed */
  char standalone_message_explorer_uri[IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01 + 1];
  /**<   Standalone message explorer URI.
    */
}ims_settings_get_rcs_sm_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the Ut Interface configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_ut_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the Ut Interface configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  Disable Ut Interface Status */
  uint8_t disable_ut_valid;  /**< Must be set to true if disable_ut is being passed */
  uint8_t disable_ut;
  /**<   Values: \n
        -TRUE -- Disable \n
        -FALSE -- Enable
    */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t ut_apn_name_valid;  /**< Must be set to true if ut_apn_name is being passed */
  char ut_apn_name[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut Interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type */
  uint8_t ut_ip_addr_type_valid;  /**< Must be set to true if ut_ip_addr_type is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type;
  /**<   Ut Interface IP address type. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */

  /* Optional */
  /*  Ut Interface Access Point Name for Wi-Fi */
  uint8_t ut_apn_name_apn2_valid;  /**< Must be set to true if ut_apn_name_apn2 is being passed */
  char ut_apn_name_apn2[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type for Wi-Fi */
  uint8_t ut_ip_addr_type_apn2_valid;  /**< Must be set to true if ut_ip_addr_type_apn2 is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type_apn2;
  /**<   Ut interface IP address type. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */
}ims_settings_get_ut_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the client provisioning configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_client_provisioning_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the client provisioning configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned 
         when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  Enable Client Provisioning */
  uint8_t enable_client_provisioning_valid;  /**< Must be set to true if enable_client_provisioning is being passed */
  uint8_t enable_client_provisioning;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Enable VoLTE Support Through Client Provisioning */
  uint8_t enable_volte_valid;  /**< Must be set to true if enable_volte is being passed */
  uint8_t enable_volte;
  /**<   Values:  \n
         - TRUE -- Enable \n
         - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Enable VT Support Through Client Provisioning */
  uint8_t enable_vt_valid;  /**< Must be set to true if enable_vt is being passed */
  uint8_t enable_vt;
  /**<   Values:  \n
         - TRUE -- Enable \n
         - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Enable Presence Support Through Client Provisioning */
  uint8_t enable_presence_valid;  /**< Must be set to true if enable_presence is being passed */
  uint8_t enable_presence;
  /**<   Values:  \n
         - TRUE -- Enable \n
         - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Wi-Fi Call Setting */
  uint8_t wifi_call_valid;  /**< Must be set to true if wifi_call is being passed */
  ims_settings_wfc_status_enum_v01 wifi_call;
  /**<   WFC status. Values: \n
      - IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED (0) --  WFC is not supported 
      - IMS_SETTINGS_WFC_STATUS_ON (1) --  WFC is enabled 
      - IMS_SETTINGS_WFC_STATUS_OFF (2) --  WFC is disabled 
 */

  /* Optional */
  /*  Wi-Fi Call Preference Setting */
  uint8_t wifi_call_preference_valid;  /**< Must be set to true if wifi_call_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_preference;
  /**<   WFC home preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */

  /* Optional */
  /*  Wi-Fi Call Roaming Setting */
  uint8_t wifi_call_roaming_valid;  /**< Must be set to true if wifi_call_roaming is being passed */
  ims_settings_wfc_roaming_enum_v01 wifi_call_roaming;
  /**<   WFC roaming mode. Values: \n
      - IMS_SETTINGS_WFC_ROAMING_NOT_SUPPORTED (0) --  WFC roaming is not supported 
      - IMS_SETTINGS_WFC_ROAMING_ENABLED (1) --  WFC roaming is enabled 
      - IMS_SETTINGS_WFC_ROAMING_DISABLED (2) --  WFC roaming is disabled 
 */

  /* Optional */
  /*  Enable VoWiFi Support Through Client Provisioning */
  uint8_t enable_vowifi_valid;  /**< Must be set to true if enable_vowifi is being passed */
  uint8_t enable_vowifi;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  VoWiFi Provisioning ID */
  uint8_t vowifi_provId_valid;  /**< Must be set to true if vowifi_provId is being passed */
  char vowifi_provId[IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01 + 1];
  /**<   Standalone message of the VoWiFi provisioning ID.
    */

  /* Optional */
  /*  WFC Roam Preference */
  uint8_t wifi_call_roam_preference_valid;  /**< Must be set to true if wifi_call_roam_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_roam_preference;
  /**<   WFC roaming preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */
}ims_settings_get_client_provisioning_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the VT configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_vt_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the VT configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  H.263 Payload Type in 3G */
  uint8_t vt_3g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_3g_h263_dynamic_pt is being passed */
  uint16_t vt_3g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 3G .\n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  H.263 Payload Type in 4G */
  uint8_t vt_4g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_4g_h263_dynamic_pt is being passed */
  uint16_t vt_4g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 4G. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  Number of 4G H.264 Configurations */
  uint8_t num_vt_4g_h264_config_valid;  /**< Must be set to true if num_vt_4g_h264_config is being passed */
  uint8_t num_vt_4g_h264_config;
  /**<   
        Number of configurations present in the 4G H.264 Configuration 
        Information TLV.
    */

  /* Optional */
  /*  4G H.264 Configuration Information */
  uint8_t vt_4g_h264_info_valid;  /**< Must be set to true if vt_4g_h264_info is being passed */
  ims_settings_vt_h264_info_v01 vt_4g_h264_info[IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01];
  /**<   \n(Array of H.264 information parameters in 4G.) */
}ims_settings_get_vt_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the configuration parameters received from the RCS auto 
             configuration server. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_acs_network_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the configuration parameters received from the RCS auto 
             configuration server. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  IR.94 Video Calling Service Status */
  uint8_t ir94_video_calling_service_enabled_valid;  /**< Must be set to true if ir94_video_calling_service_enabled is being passed */
  uint8_t ir94_video_calling_service_enabled;
  /**<   
        Authorization for the user to use an IR.94 video calling service.
        Values:\n
        - TRUE -- IR.94 video calling is enabled \n
        - FALSE -- IR.94 video calling is disabled
    */

  /* Optional */
  /*  RCS Messaging Service Status  */
  uint8_t rcs_messaging_enabled_valid;  /**< Must be set to true if rcs_messaging_enabled is being passed */
  uint8_t rcs_messaging_enabled;
  /**<   
        RCS messaging status. Values: \n
        - TRUE -- RCS messaging is enabled \n
        - FALSE -- RCS messaging is disabled
        */
}ims_settings_get_acs_network_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the SIP read-only-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_sip_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the SIP read-only-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   Maximum duration, in milliseconds, that a message remains in the 
        network.
    */

  /* Optional */
  /*  TCP Threshold Value */
  uint8_t tcp_threshold_value_valid;  /**< Must be set to true if tcp_threshold_value is being passed */
  uint16_t tcp_threshold_value;
  /**<   Defines the packet size limiting value, in bytes.
    */

  /* Optional */
  /*  Compact Form Enabled */
  uint8_t compact_form_enabled_valid;  /**< Must be set to true if compact_form_enabled is being passed */
  uint8_t compact_form_enabled;
  /**<   Indicates whether the SIP compact form is enabled.
    */

  /* Optional */
  /*  Authentication Scheme */
  uint8_t settings_auth_scheme_valid;  /**< Must be set to true if settings_auth_scheme is being passed */
  ims_settings_config_auth_scheme_enum_v01 settings_auth_scheme;
  /**<   Authentication scheme configuration. Values: \n
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_NONE (0x00) --  No scheme used 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_DIGEST (0x01) --  Digest 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_SAG (0x02) --  Token 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKA (0x03) --  AKA 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_CAVE (0x04) --  CAVE 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKAV2 (0x05) --  AKAv2 
 */

  /* Optional */
  /*  Initial Authorization Type */
  uint8_t settings_initial_auth_config_valid;  /**< Must be set to true if settings_initial_auth_config is being passed */
  ims_settings_config_initial_auth_type_enum_v01 settings_initial_auth_config;
  /**<   Initial authorization type value. Values: \n
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_NONE (0x00) --  None 
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_AUTHORIZATION (0x01) --  Authorization 
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_PROXY_AUTHORIZATION (0x02) --  Proxy authorization 
 */

  /* Optional */
  /*  Authorization Header Value */
  uint8_t auth_header_value_valid;  /**< Must be set to true if auth_header_value is being passed */
  char auth_header_value[IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01 + 1];
  /**<   Authorization header value. */

  /* Optional */
  /*  Proxy Route Value */
  uint8_t proxy_route_value_valid;  /**< Must be set to true if proxy_route_value is being passed */
  char proxy_route_value[IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01 + 1];
  /**<   Route value to be used by the shared configuration. */
}ims_settings_get_sip_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the network read-only-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_network_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the network read-only-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  IPv6 Enabled */
  uint8_t ipv6_enabled_valid;  /**< Must be set to true if ipv6_enabled is being passed */
  uint8_t ipv6_enabled;
  /**<   Indicates whether the IPV6 address is enabled. */

  /* Optional */
  /*  IPSec Integrity Scheme */
  uint8_t ip_sec_int_scheme_valid;  /**< Must be set to true if ip_sec_int_scheme is being passed */
  ims_settings_integ_algo_mask_v01 ip_sec_int_scheme;
  /**<   Bitmask indicating the integrity algorithm combination. Values: \n
      - IMS_SETTINGS_INTEG_ALGO_HMAC_SHA_1_96 (0x01) --  HMAC-SHA-1-96 algorithm is used for IPSec integrity 
      - IMS_SETTINGS_INTEG_ALGO_HMAC_MD5_96 (0x02) --  HMAC-MD5-96 algorithm is used for IPSec integrity  
 */

  /* Optional */
  /*  IPSec Encryption Algorithm */
  uint8_t ip_sec_enc_algo_valid;  /**< Must be set to true if ip_sec_enc_algo is being passed */
  ims_settings_encrypt_algo_mask_v01 ip_sec_enc_algo;
  /**<   Bitmask indicating the IPSec encryption algorithm combination. 
 Values: \n
      - IMS_SETTINGS_ENCRYPT_ALGO_NULL (0x01) --  NULL algorithm is used for IPSec encryption 
      - IMS_SETTINGS_ENCRYPT_ALGO_AES_CBC (0x02) --  AES-CBC algorithm is used for IPSec encryption 
      - IMS_SETTINGS_ENCRYPT_ALGO_DES_EDE3_CBC (0x04) --  DES-EDE3-CBC algorithm is used for IPSec encryption  
 */

  /* Optional */
  /*  Chunk Size of MSRP Packet */
  uint8_t msrp_pkt_size_valid;  /**< Must be set to true if msrp_pkt_size is being passed */
  uint16_t msrp_pkt_size;
  /**<   Indicates MSRP packet chunk size in KB (kilobytes). 
         Default value: 2KB.
    */
}ims_settings_get_network_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the VoIP read-only-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_voip_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the VoIP read-only-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  VoIP Configuration Expiration */
  uint8_t voip_config_expires_valid;  /**< Must be set to true if voip_config_expires is being passed */
  uint16_t voip_config_expires;
  /**<   VoIP configuration expiration timer. */

  /* Optional */
  /*  VoIP Session Timer Enabled */
  uint8_t voip_session_timer_enabled_valid;  /**< Must be set to true if voip_session_timer_enabled is being passed */
  uint8_t voip_session_timer_enabled;
  /**<   Indicates whether the VoIP session is timer enabled. */
}ims_settings_get_voip_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the user read-only-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_user_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the user read-only-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Registration Configuration User Name */
  uint8_t reg_config_userName_valid;  /**< Must be set to true if reg_config_userName is being passed */
  char reg_config_userName[IMS_SETTINGS_REG_CONFIG_USER_NAME_LEN_V01 + 1];
  /**<   Registration configuration user name. */

  /* Optional */
  /*  Registration Configuration Private URI */
  uint8_t reg_config_privateURI_valid;  /**< Must be set to true if reg_config_privateURI is being passed */
  char reg_config_privateURI[IMS_SETTINGS_REG_CONFIG_PRIVATE_URI_LEN_V01 + 1];
  /**<   Registration configuration private URI. */

  /* Optional */
  /*  Registration Configuration Display Name */
  uint8_t reg_config_displayName_valid;  /**< Must be set to true if reg_config_displayName is being passed */
  uint16_t reg_config_displayName[IMS_SETTINGS_REG_CONFIG_DISPLAY_NAME_LEN_V01 + 1];
  /**<   Registration configuration display name. */
}ims_settings_get_user_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the registration manager read-only-related configuration 
             parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_reg_mgr_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the registration manager read-only-related configuration 
             parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Registration Configuration Mode */
  uint8_t settings_regmgr_mode_config_valid;  /**< Must be set to true if settings_regmgr_mode_config is being passed */
  ims_settings_regmgr_config_mode_enum_v01 settings_regmgr_mode_config;
  /**<   Registration configuration mode value. Values: \n
      - IMS_SETTINGS_REGMGR_CONFIG_IETF (0x00) --  IETF Configuration mode
      - IMS_SETTINGS_REGMGR_CONFIG_EARLY_IMS (0x01) --  Early IMS Configuration mode
      - IMS_SETTINGS_REGMGR_CONFIG_IMS (0x02) --  IMS Configuration mode
      - IMS_SETTINGS_REGMGR_CONFIG_IMS_NO_IPSEC (0x03) --  IMS No IPSec Configuration mode
      - IMS_SETTINGS_REGMGR_CONFIG_IMS_NONE (0x04) --  No configuration mode
 */

  /* Optional */
  /*  RegMgr PDP Profile Name */
  uint8_t regmgr_pdp_profilename_valid;  /**< Must be set to true if regmgr_pdp_profilename is being passed */
  char regmgr_pdp_profilename[IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01 + 1];
  /**<   Registration manager PDP profile name. */
}ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the RCS automatic configuration read-only-related 
             configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_rcs_auto_config_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the RCS automatic configuration read-only-related 
             configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Device Type */
  uint8_t rcsOnly_device_type_valid;  /**< Must be set to true if rcsOnly_device_type is being passed */
  uint8_t rcsOnly_device_type;
  /**<   RCS device type configuration. */

  /* Optional */
  /*  RCS PDP Profile Name */
  uint8_t rcs_pdp_profilename_valid;  /**< Must be set to true if rcs_pdp_profilename is being passed */
  char rcs_pdp_profilename[IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01 + 1];
  /**<   RCS APN profile name.   */

  /* Optional */
  /*  Internet PDP Profile Name */
  uint8_t internet_pdp_profilename_valid;  /**< Must be set to true if internet_pdp_profilename is being passed */
  char internet_pdp_profilename[IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01 + 1];
  /**<   Internet APN profile name.       */

  /* Optional */
  /*  PCO Configuration Priority */
  uint8_t pco_config_priority_valid;  /**< Must be set to true if pco_config_priority is being passed */
  uint8_t pco_config_priority;
  /**<   Priority of PCO configuration */

  /* Optional */
  /*  ISIM Configuration Priority */
  uint8_t isim_config_priority_valid;  /**< Must be set to true if isim_config_priority is being passed */
  uint8_t isim_config_priority;
  /**<   Priority of ISIM configuration. */

  /* Optional */
  /*  Preconfiguration Priority */
  uint8_t preconfig_priority_valid;  /**< Must be set to true if preconfig_priority is being passed */
  uint8_t preconfig_priority;
  /**<   Preconfiguration priority. */

  /* Optional */
  /*  Automatic Configuration Priority */
  uint8_t autoconfig_priority_valid;  /**< Must be set to true if autoconfig_priority is being passed */
  uint8_t autoconfig_priority;
  /**<   Automatic configuration priority. */

  /* Optional */
  /*  RCS LTE FT List */
  uint8_t rcs_lte_ft_list_valid;  /**< Must be set to true if rcs_lte_ft_list is being passed */
  char rcs_lte_ft_list[IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01 + 1];
  /**<   List of RCS FTs to be supported in the LTE RAT.  */

  /* Optional */
  /*  RCS HSPA FT List */
  uint8_t rcs_hspa_ft_list_valid;  /**< Must be set to true if rcs_hspa_ft_list is being passed */
  char rcs_hspa_ft_list[IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01 + 1];
  /**<   List of RCS FTs to be supported in the HSPA RAT.  */

  /* Optional */
  /*  RCS Wi-Fi FT List */
  uint8_t rcs_wifi_ft_list_valid;  /**< Must be set to true if rcs_wifi_ft_list is being passed */
  char rcs_wifi_ft_list[IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01 + 1];
  /**<   List of RCS FTs to be supported in the Wi-Fi\reg RAT.    */

  /* Optional */
  /*  Disable Auto Configuration */
  uint8_t disable_auto_config_valid;  /**< Must be set to true if disable_auto_config is being passed */
  uint8_t disable_auto_config;
  /**<   Flag indicating whether to disable auto configuration of RCS. Values:\n
         - TRUE -- Disable\n
         - FALSE -- Enable
    */
}ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the RCS IMS core automatic configuration 
             read-only-related configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_rcs_imscore_auto_config_read_only_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the RCS IMS core automatic configuration 
             read-only-related configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  RCS Timer T1 */
  uint8_t rcs_timer_t1_valid;  /**< Must be set to true if rcs_timer_t1 is being passed */
  uint32_t rcs_timer_t1;
  /**<   SIP timer 1 is retrieved using RCS automatic configuration. */

  /* Optional */
  /*  RCS Timer T2 */
  uint8_t rcs_timer_t2_valid;  /**< Must be set to true if rcs_timer_t2 is being passed */
  uint32_t rcs_timer_t2;
  /**<   SIP timer 2 is retrieved using RCS automatic configuration. */

  /* Optional */
  /*  RCS Timer T4 */
  uint8_t rcs_timer_t4_valid;  /**< Must be set to true if rcs_timer_t4 is being passed */
  uint32_t rcs_timer_t4;
  /**<   SIP timer 4 is retrieved using RCS automatic configuration. */
}ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the handover configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_handover_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the handover configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. A settings-specific error code is returned when 
         the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard 
         response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to WWAN */
  uint8_t wlan_to_wwan_hys_timer_valid;  /**< Must be set to true if wlan_to_wwan_hys_timer is being passed */
  uint32_t wlan_to_wwan_hys_timer;
  /**<   IMS handover hysteresis timer for WLAN to WWAN, in seconds.
	     This timer is started if the LTE to WLAN idle mode handover
	     occurs because of LTE RAT loss. When in cellular-preferred mode,
	     WLAN to LTE handover only occurs after LTE is available for this
	     period of time.  If this value is not present, a default of 120 sec
	     is used.
	*/

  /* Optional */
  /*  Voice Services Handover Hysteresis Timer WLAN to 1X */
  uint8_t wlan_to_1x_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_hys_timer is being passed */
  uint32_t wlan_to_1x_hys_timer;
  /**<   Hysteresis timer for moving voice services from WLAN to 1X, in seconds.
	     This timer is started if a 1X system is used for voice service and
		 1X quality drops below a threshold in idle mode. When in
		 cellular-preferred mode, voice service moves to 1X only after 1X has
		 been available with improved quality for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN to WLAN */
  uint8_t wwan_to_wlan_hys_timer_valid;  /**< Must be set to true if wwan_to_wlan_hys_timer is being passed */
  uint32_t wwan_to_wlan_hys_timer;
  /**<   IMS handover hysteresis timer for WWAN to WLAN, in seconds.
	     In Wi-Fi preferred mode, this timer is started if a WLAN to LTE idle
	     mode handover occurs because of WLAN RAT loss. LTE to WLAN handover
	     only occurs after WLAN is available for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Enabled Between WWAN and WLAN */
  uint8_t ims_pdn_ho_enabled_valid;  /**< Must be set to true if ims_pdn_ho_enabled is being passed */
  ims_settings_handover_option_enum_v01 ims_pdn_ho_enabled;
  /**<   IMS handover mode. Values: \n
      - IMS_SETTINGS_HO_ENABLED_ALL (1) --  Handover is enabled from WWAN to WLAN and WLAN to WWAN 
      - IMS_SETTINGS_HO_DISABLED_ALL (2) --  Handover is disabled from WWAN to WLAN and WLAN to WWAN  
      - IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY (3) --  Handover is only enabled from WLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY (4) --  Handover is only enabled from WWAN to WLAN 
      - IMS_SETTINGS_HO_ENABLED_IWLAN_TO_WWAN_ONLY (5) --  Handover is only enabled from IWLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_IWLAN_ONLY (6) --  Handover is only enabled from WWAN to IWLAN 
 */

  /* Optional */
  /*  IMS 1x RSSI Threshold */
  uint8_t iIMSHO1xQualTH_valid;  /**< Must be set to true if iIMSHO1xQualTH is being passed */
  int32_t iIMSHO1xQualTH;
  /**<   This value defines the 1x threshold value for comparing averaged signal strength.
         Scenario: If the signal quality of 1x falls below iIMSHO1xQualTH and Wi-Fi is available, 
         the device deregisters from 1x and connects to IMS PDN over Wi-Fi for voice calls.	\n
		 Default value --> -7 db
	*/

  /* Optional */
  /*  IMS LTE Threshold1 */
  uint8_t iIMSHOLTEQualTH1_valid;  /**< Must be set to true if iIMSHOLTEQualTH1 is being passed */
  int32_t iIMSHOLTEQualTH1;
  /**<   This value defines LTE Threshold 1. 
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -117 db
	*/

  /* Optional */
  /*  IMS LTE Threshold2 */
  uint8_t iIMSHOLTEQualTH2_valid;  /**< Must be set to true if iIMSHOLTEQualTH2 is being passed */
  int32_t iIMSHOLTEQualTH2;
  /**<   This value defines LTE Threshold 2.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice only. \n
		 Default value --> -115 db
	*/

  /* Optional */
  /*  IMS LTE Threshold3  */
  uint8_t iIMSHOLTEQualTH3_valid;  /**< Must be set to true if iIMSHOLTEQualTH3 is being passed */
  int32_t iIMSHOLTEQualTH3;
  /**<   This value defines LTE Threshold 3.     
         Handover scenario: Wi-Fi to LTE and LTE to Wi-Fi.
         Used for In-call mode for voice only. \n
		 Default value --> -110 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdA */
  uint8_t iIMSHOWIFIQualTHA_valid;  /**< Must be set to true if iIMSHOWIFIQualTHA is being passed */
  int32_t iIMSHOWIFIQualTHA;
  /**<   This value defines Wi-Fi Threshold A.
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -85 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdB */
  uint8_t iIMSHOWIFIQualTHB_valid;  /**< Must be set to true if iIMSHOWIFIQualTHB is being passed */
  int32_t iIMSHOWIFIQualTHB;
  /**<   This value defines Wi-Fi Threshold B.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice and video. \n
		 Default value --> -95 db
	*/

  /* Optional */
  /*  HO disabled mask */
  uint8_t ho_disabled_mask_valid;  /**< Must be set to true if ho_disabled_mask is being passed */
  ims_settings_ho_disabled_mask_v01 ho_disabled_mask;
  /**<   Mask of Ho disabled
    */

  /* Optional */
  /*  IMS LTE RSRQ Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Src is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Src;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -120 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Src is being passed */
  int32_t iIR92_LTE_DL_SNR_Src;
  /**<   This value defines LTE SNR Threshold (scaled dB) when UE handover algorithm is running in IR92 mode.    
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> 95 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS LTE RSRQ Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Target is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Target;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -100 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Target is being passed */
  int32_t iIR92_LTE_DL_SNR_Target;
  /**<   This value defines LTE SNR Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> 105 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS Wifi RSSI Source threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_Out_valid;  /**< Must be set to true if iIR92_Wifi_Rove_Out is being passed */
  int32_t iIR92_Wifi_Rove_Out;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -85 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Target threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_In_valid;  /**< Must be set to true if iIR92_Wifi_Rove_In is being passed */
  int32_t iIR92_Wifi_Rove_In;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.   
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -65 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Repoint threhsold for IR92 */
  uint8_t iIR92_Wifi_Repoint_valid;  /**< Must be set to true if iIR92_Wifi_Repoint is being passed */
  int32_t iIR92_Wifi_Repoint;
  /**<   This value defines Wifi RSSI Threshold (dB) for CS-Repoint for IR92.     
         Handover scenario: Used to page incoming calls over CS damain as Wifi RSSI falls below this value.
         Used for In-call mode
		 Default value --> -75 dB
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP Radio */
  uint8_t wlan_to_3gpp_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp_idle_mode_hys_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving voice services from WLAN to 3GPP RAT in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from UTRAN\GERAN RAT to 
     WLAN during idle mode because of UTRAN\GERAN RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP2 Radio */
  uint8_t wlan_to_3gpp2_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp2_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp2_idle_mode_hys_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving voice services from WLAN to 3GPP2 RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from eHRPD\eVDO RAT to 
     WLAN during idle mode because of eHRPD\eVDO RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 1x Radio */
  uint8_t wlan_to_1x_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_1x_idle_mode_hys_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving voice services from WLAN to 1x RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN is brought up on WLAN 
     during idle mode because of 1X RAT loss.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_cell_pref_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving voice services from WLAN to LTE RATs during VOIP call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during VOIP call because of LTE RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_wifi_pref_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving voice services from WLAN to LTE RAT during VOIP call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during VOIP call because of WLAN RAT unavailability for IMS. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_cell_pref_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during Video call because of LTE RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_wifi_pref_timer;
  /**<   QMI Get response for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during Video call because of WLAN RAT unavailability for IMS. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN Radio to WLAN */
  uint8_t wwan_to_wlan_all_mode_hysteresis_timer_valid;  /**< Must be set to true if wwan_to_wlan_all_mode_hysteresis_timer is being passed */
  uint32_t wwan_to_wlan_all_mode_hysteresis_timer;
  /**<   
	This timer is started when ims handovers/moves from wifi to any cellular rat (lte, ehrpd, etc) to avoid coming back to wifi 
	immediately and there by avoiding pdn ping-pongs. It is started during idle mode irrespective of user preference (cellular or wifi preference).  
	*/
}ims_settings_get_handover_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS application's VICE configuration parameters. (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_qipcall_vice_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS application's VICE configuration parameters. (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. A settings-specific error code is returned when 
         the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t qipcall_vice_enabled_valid;  /**< Must be set to true if qipcall_vice_enabled is being passed */
  uint8_t qipcall_vice_enabled;
  /**<   Indicates whether VICE is enabled. */
}ims_settings_get_qipcall_vice_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS policy manager configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_pol_mgr_rat_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS policy manager configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. A settings-specific error code is returned when 
         the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  IPSec enabled or not */
  uint8_t rat_ipsec_enabled_valid;  /**< Must be set to true if rat_ipsec_enabled is being passed */
  ims_settings_pol_mgr_rat_ipsec_info_v01 rat_ipsec_enabled[IMS_SETTINGS_POL_MGR_RAT_MAX_V01];
  /**<   Indicates whether the IPSec is enabled. */
}ims_settings_get_pol_mgr_rat_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the registration state for various settings service 
             indications for the requesting control points. */
typedef struct {

  /* Optional */
  /*  SIP Configuration (Deprecated) */
  uint8_t sip_config_valid;  /**< Must be set to true if sip_config is being passed */
  uint8_t sip_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Registration Manager Configuration */
  uint8_t reg_mgr_config_valid;  /**< Must be set to true if reg_mgr_config is being passed */
  uint8_t reg_mgr_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  SMS Configuration */
  uint8_t sms_config_valid;  /**< Must be set to true if sms_config is being passed */
  uint8_t sms_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  User Configuration (Deprecated) */
  uint8_t user_config_valid;  /**< Must be set to true if user_config is being passed */
  uint8_t user_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  VoIP Configuration (Deprecated) */
  uint8_t voip_config_valid;  /**< Must be set to true if voip_config is being passed */
  uint8_t voip_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  Presence Configuration */
  uint8_t presence_config_valid;  /**< Must be set to true if presence_config is being passed */
  uint8_t presence_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  Media Configuration (Deprecated) */
  uint8_t media_config_valid;  /**< Must be set to true if media_config is being passed */
  uint8_t media_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  QIPCall Configuration (Deprecated) */
  uint8_t qipcall_config_valid;  /**< Must be set to true if qipcall_config is being passed */
  uint8_t qipcall_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  SIP Read-only Configuration (Deprecated) */
  uint8_t sip_read_only_config_valid;  /**< Must be set to true if sip_read_only_config is being passed */
  uint8_t sip_read_only_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  Network Read-only Configuration (Deprecated) */
  uint8_t network_read_only_config_valid;  /**< Must be set to true if network_read_only_config is being passed */
  uint8_t network_read_only_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable 
  */

  /* Optional */
  /*  Registration Manager Extended Configuration (Deprecated) */
  uint8_t reg_mgr_extended_config_valid;  /**< Must be set to true if reg_mgr_extended_config is being passed */
  uint8_t reg_mgr_extended_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Policy Manager Configuration */
  uint8_t pol_mgr_config_valid;  /**< Must be set to true if pol_mgr_config is being passed */
  uint8_t pol_mgr_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Presence Extended Configuration (Deprecated) */
  uint8_t presence_ext_config_valid;  /**< Must be set to true if presence_ext_config is being passed */
  uint8_t presence_ext_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  RCS Standalone Messaging Configuration (Deprecated) */
  uint8_t rcs_sm_config_valid;  /**< Must be set to true if rcs_sm_config is being passed */
  uint8_t rcs_sm_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Ut Interface Configuration (Deprecated) */
  uint8_t ut_config_valid;  /**< Must be set to true if ut_config is being passed */
  uint8_t ut_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Client Provisioning Configuration (Deprecated) */
  uint8_t client_provisioning_config_valid;  /**< Must be set to true if client_provisioning_config is being passed */
  uint8_t client_provisioning_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  VT Configuration (Deprecated) */
  uint8_t vt_config_valid;  /**< Must be set to true if vt_config is being passed */
  uint8_t vt_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  ACS Network Configuration/RCS Configuration (Deprecated) */
  uint8_t acs_network_config_valid;  /**< Must be set to true if acs_network_config is being passed */
  uint8_t acs_network_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
    */

  /* Optional */
  /*  Handover Configuration */
  uint8_t handover_config_valid;  /**< Must be set to true if handover_config is being passed */
  uint8_t handover_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  VICE Configuration (Deprecated) */
  uint8_t qipcall_vice_config_valid;  /**< Must be set to true if qipcall_vice_config is being passed */
  uint8_t qipcall_vice_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  RAT Configuration (Deprecated) */
  uint8_t pol_mgr_rat_config_valid;  /**< Must be set to true if pol_mgr_rat_config is being passed */
  uint8_t pol_mgr_rat_config;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Voice Dynamic Configuration */
  uint8_t voice_dynamic_config_enabled_valid;  /**< Must be set to true if voice_dynamic_config_enabled is being passed */
  uint8_t voice_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Codec Dynamic Configuration */
  uint8_t codec_dynamic_config_enabled_valid;  /**< Must be set to true if codec_dynamic_config_enabled is being passed */
  uint8_t codec_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Video Dynamic Configuration */
  uint8_t video_dynamic_config_enabled_valid;  /**< Must be set to true if video_dynamic_config_enabled is being passed */
  uint8_t video_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Emergency Dynamic Configuration */
  uint8_t emer_dynamic_config_enabled_valid;  /**< Must be set to true if emer_dynamic_config_enabled is being passed */
  uint8_t emer_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Test Dynamic Configuration */
  uint8_t test_dynamic_config_enabled_valid;  /**< Must be set to true if test_dynamic_config_enabled is being passed */
  uint8_t test_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  RTP Dynamic Configuration */
  uint8_t rtp_dynamic_config_enabled_valid;  /**< Must be set to true if rtp_dynamic_config_enabled is being passed */
  uint8_t rtp_dynamic_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  IMS Configuration */
  uint8_t ims_config_enabled_valid;  /**< Must be set to true if ims_config_enabled is being passed */
  uint8_t ims_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
    */

  /* Optional */
  /*  IMS Common Configuration */
  uint8_t ims_common_config_enabled_valid;  /**< Must be set to true if ims_common_config_enabled is being passed */
  uint8_t ims_common_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  IMS Service Enable Configuration */
  uint8_t ims_service_enable_config_enabled_valid;  /**< Must be set to true if ims_service_enable_config_enabled is being passed */
  uint8_t ims_service_enable_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  RCS Configuration */
  uint8_t rcs_config_enabled_valid;  /**< Must be set to true if rcs_config_enabled is being passed */
  uint8_t rcs_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  XCAP Configuration */
  uint8_t xcap_config_enabled_valid;  /**< Must be set to true if xcap_config_enabled is being passed */
  uint8_t xcap_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  EVS Configuration */
  uint8_t qipcall_evs_config_enabled_valid;  /**< Must be set to true if qipcall_evs_config_enabled is being passed */
  uint8_t qipcall_evs_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */

  /* Optional */
  /*  Real Time Text Configuration */
  uint8_t rtt_config_enabled_valid;  /**< Must be set to true if rtt_config_enabled is being passed */
  uint8_t rtt_config_enabled;
  /**<   Values: \n
       - 0x00 -- Disable \n
       - 0x01 -- Enable
  */
}ims_settings_config_ind_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the registration state for various settings service 
             indications for the requesting control points. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}ims_settings_config_ind_reg_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the codec information for a profile. */
typedef struct {

  /* Mandatory */
  /*  Profile ID */
  uint8_t profile_number;
  /**<   Profile ID number.  */

  /* Optional */
  /*  Codec Info */
  uint8_t codec_data_valid;  /**< Must be set to true if codec_data is being passed */
  uint32_t codec_data_len;  /**< Must be set to # of elements in codec_data */
  ims_settings_csd_codec_info_v01 codec_data[IMS_SETTINGS_CSD_CODEC_MAX_V01];
  /**<   Codec information associated with the profile ID.  */
}ims_settings_set_config_csd_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the codec information for a profile. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}ims_settings_set_config_csd_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the SIP configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  SIP Port Number */
  uint8_t sip_local_port_valid;  /**< Must be set to true if sip_local_port is being passed */
  uint16_t sip_local_port;
  /**<   Primary call session control function SIP port number.
    */

  /* Optional */
  /*  SIP Registration Timer */
  uint8_t timer_sip_reg_valid;  /**< Must be set to true if timer_sip_reg is being passed */
  uint32_t timer_sip_reg;
  /**<   Initial SIP registration duration, in seconds, from the UE.
    */

  /* Optional */
  /*  Subscribe Timer */
  uint8_t subscribe_timer_valid;  /**< Must be set to true if subscribe_timer is being passed */
  uint32_t subscribe_timer;
  /**<   Duration, in seconds, of the subscription by the UE for IMS 
         registration notifications.
    */

  /* Optional */
  /*  Timer T1  */
  uint8_t timer_t1_valid;  /**< Must be set to true if timer_t1 is being passed */
  uint32_t timer_t1;
  /**<   RTT estimate, in milliseconds.
    */

  /* Optional */
  /*  Timer T2   */
  uint8_t timer_t2_valid;  /**< Must be set to true if timer_t2 is being passed */
  uint32_t timer_t2;
  /**<   Maximum retransmit interval, in milliseconds, for non-invite requests 
         and invite responses.
    */

  /* Optional */
  /*  Timer TF   */
  uint8_t timer_tf_valid;  /**< Must be set to true if timer_tf is being passed */
  uint32_t timer_tf;
  /**<   Non-invite transaction timeout timer, in milliseconds.
    */

  /* Optional */
  /*  Sigcomp Status */
  uint8_t sigcomp_enabled_valid;  /**< Must be set to true if sigcomp_enabled is being passed */
  uint8_t sigcomp_enabled;
  /**<  
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  Timer TJ */
  uint8_t timer_tj_valid;  /**< Must be set to true if timer_tj is being passed */
  uint16_t timer_tj;
  /**<   Wait time, in milliseconds, for the non-invite request retransmission. 
         If the value exceeds the range of uint16, it is set to 0xFFFF. */

  /* Optional */
  /*  Timer TJ Extended */
  uint8_t timer_tj_ext_valid;  /**< Must be set to true if timer_tj_ext is being passed */
  uint32_t timer_tj_ext;
  /**<   Wait time, in milliseconds, for the non-invite request 
         retransmission. */

  /* Optional */
  /*  Keep Alive Status */
  uint8_t keepalive_enabled_valid;  /**< Must be set to true if keepalive_enabled is being passed */
  uint8_t keepalive_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  NAT-RTO Timer Value */
  uint8_t nat_rto_timer_valid;  /**< Must be set to true if nat_rto_timer is being passed */
  uint32_t nat_rto_timer;
  /**<   Request timeout value, in milliseconds, used in NAT implementation. */

  /* Optional */
  /*  SIP_TIMER_OPERATOR_MODE_A Timer Value */
  uint8_t sip_timer_operator_mode_a_valid;  /**< Must be set to true if sip_timer_operator_mode_a is being passed */
  uint32_t sip_timer_operator_mode_a;
  /**<   SIP timer operator mode A, in seconds; valid range of values is 0 to 
         30. If this TLV is not included in the request, a value of 6 seconds is 
         used.
    */

  /* Optional */
  /*  SIP Timer B Value */
  uint8_t timer_tb_value_valid;  /**< Must be set to true if timer_tb_value is being passed */
  uint32_t timer_tb_value;
  /**<   SIP timer B's value, in milliseconds. If this TLV is not included in 
         the request, a value of 0 is used.
    */

  /* Optional */
  /*  SIP GRUU Support Enable Flag */
  uint8_t gruu_enabled_valid;  /**< Must be set to true if gruu_enabled is being passed */
  uint8_t gruu_enabled;
  /**<   SIP GRUU support enable flag.
         If this TLV is not included in the request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Transport Protocol Switch Support */
  uint8_t transport_switch_enabled_valid;  /**< Must be set to true if transport_switch_enabled is being passed */
  uint8_t transport_switch_enabled;
  /**<   SIP transport protocol switching support enable flag per
         \hyperref[RFC3261]{RFC 3261}. If this TLV is not included in the 
         request, a value of FALSE is used.
    */

  /* Optional */
  /*  SIP Maximum TCP Transport Backoff Timer Value */
  uint8_t tcp_max_backoff_timer_value_valid;  /**< Must be set to true if tcp_max_backoff_timer_value is being passed */
  uint32_t tcp_max_backoff_timer_value;
  /**<   Maximum timeout, in milliseconds, for TCP transport of SIP packets 
         after which SIP packets are sent via UDP. If this TLV is not included 
         in the request, a value of 10000 (that is, 10 seconds) is used.
    */

  /* Optional */
  /*  SIP GZIP Decoding Outbuffer Multiplier Value */
  uint8_t gzip_decoding_outbuffer_multiplier_valid;  /**< Must be set to true if gzip_decoding_outbuffer_multiplier is being passed */
  uint8_t gzip_decoding_outbuffer_multiplier;
  /**<   SIP GZIP decoding outbuffer multiplier, the compression multiplier 
         value. If this TLV is not included in the request, a value of 40 is 
         used.
    */

  /* Optional */
  /*  SIP Timer D Value */
  uint8_t timer_td_value_valid;  /**< Must be set to true if timer_td_value is being passed */
  uint32_t timer_td_value;
  /**<   SIP timer D's value, in milliseconds. 
         Timer D is the wait time for response retransmits of the invite client 
         transactions.
    */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   SIP timer T4's value, in milliseconds. 
         Timer T4 is the maximum duration that a SIP message can remain in the 
         network.
    */

  /* Optional */
  /*  SIP Timer A */
  uint8_t timer_ta_value_valid;  /**< Must be set to true if timer_ta_value is being passed */
  uint32_t timer_ta_value;
  /**<   SIP timer A's value, in milliseconds. 
         Timer A is the INVITE request retransmit interval, for UDP only
    */

  /* Optional */
  /*  SIP Timer E */
  uint8_t timer_te_value_valid;  /**< Must be set to true if timer_te_value is being passed */
  uint32_t timer_te_value;
  /**<   SIP timer E's value, in milliseconds. 
         Timer E is the value Non-INVITE request retransmit interval, 
         for UDP only.
    */

  /* Optional */
  /*  SIP Timer G */
  uint8_t timer_tg_value_valid;  /**< Must be set to true if timer_tg_value is being passed */
  uint32_t timer_tg_value;
  /**<   SIP timer G's value, in milliseconds. 
         Timer G is the value of INVITE response retransmit interval.
    */

  /* Optional */
  /*  SIP Timer H */
  uint8_t timer_th_value_valid;  /**< Must be set to true if timer_th_value is being passed */
  uint32_t timer_th_value;
  /**<   SIP timer H's value, in milliseconds. 
         Timer H is the value of wait time for ACK receipt.
    */

  /* Optional */
  /*  SIP Timer I */
  uint8_t timer_ti_value_valid;  /**< Must be set to true if timer_ti_value is being passed */
  uint32_t timer_ti_value;
  /**<   SIP timer I's value, in milliseconds. 
         Timer I is the value of wait time for ACK retransmits.
    */

  /* Optional */
  /*  SIP Timer K */
  uint8_t timer_tk_value_valid;  /**< Must be set to true if timer_tk_value is being passed */
  uint32_t timer_tk_value;
  /**<   SIP timer K's value, in milliseconds. 
         Timer K is the value of wait time for response retransmits.
    */

  /* Optional */
  /*  SIP Timer C */
  uint8_t timer_tc_value_valid;  /**< Must be set to true if timer_tc_value is being passed */
  uint32_t timer_tc_value;
  /**<   SIP timer C's value, in milliseconds. 
         Timer C is the value of wait time for response retransmits.
    */
}ims_settings_sip_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the registration manager configuration 
             parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Primary CSCF Port */
  uint8_t regmgr_config_pcscf_port_valid;  /**< Must be set to true if regmgr_config_pcscf_port is being passed */
  uint16_t regmgr_config_pcscf_port;
  /**<   Primary CSCF port. */

  /* Optional */
  /*  CSCF Port */
  uint8_t regmgr_primary_cscf_valid;  /**< Must be set to true if regmgr_primary_cscf is being passed */
  char regmgr_primary_cscf[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   CSCF port, fully qualified domain name.  */

  /* Optional */
  /*  IMS Test Mode */
  uint8_t ims_test_mode_valid;  /**< Must be set to true if ims_test_mode is being passed */
  uint8_t ims_test_mode;
  /**<  
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */
}ims_settings_reg_mgr_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the SMS configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 */

  /* Optional */
  /*  SMS Over IP Network Indication Flag */
  uint8_t sms_over_ip_network_indication_valid;  /**< Must be set to true if sms_over_ip_network_indication is being passed */
  uint8_t sms_over_ip_network_indication;
  /**<  
        Values: \n
        -TRUE -- MO SMS turned on \n
        -FALSE -- MO SMS turned off
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   Phone context universal resource identifier. */

  /* Optional */
  /*  SMS PSI String */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_SMS_PSI_LEN_V01 + 1];
  /**<   
        SMS PSI string value.
    */
}ims_settings_sms_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the user configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IMS Domain Name */
  uint8_t ims_domain_valid;  /**< Must be set to true if ims_domain is being passed */
  char ims_domain[IMS_SETTINGS_STRING_LEN_MAX_V01 + 1];
  /**<   IMS domain name. */
}ims_settings_user_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the VoIP configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Session Duration */
  uint8_t session_expiry_timer_valid;  /**< Must be set to true if session_expiry_timer is being passed */
  uint16_t session_expiry_timer;
  /**<   Session duration, in seconds. */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value, in seconds, for the session timer. */

  /* Optional */
  /*  Enable AMR WB  */
  uint8_t amr_wb_enable_valid;  /**< Must be set to true if amr_wb_enable is being passed */
  uint8_t amr_wb_enable;
  /**<   Flag indicating AMR WB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  Enable SCR AMR NB */
  uint8_t scr_amr_enable_valid;  /**< Must be set to true if scr_amr_enable is being passed */
  uint8_t scr_amr_enable;
  /**<   Flag indicating SCR for AMR NB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  Enable SCR AMR WB  */
  uint8_t scr_amr_wb_enable_valid;  /**< Must be set to true if scr_amr_wb_enable is being passed */
  uint8_t scr_amr_wb_enable;
  /**<   Flag indicating SCR for AMR WB audio. \n
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled
    */

  /* Optional */
  /*  AMR NB Mode */
  uint8_t amr_mode_valid;  /**< Must be set to true if amr_mode is being passed */
  uint8_t amr_mode;
  /**<   Bitmask indicating AMR NB modes. \n
        Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps
    */

  /* Optional */
  /*  AMR WB Mode */
  uint8_t amr_wb_mode_valid;  /**< Must be set to true if amr_wb_mode is being passed */
  uint16_t amr_wb_mode;
  /**<   Bitmask indicating AMR WB modes. \n
        Values: \n
        - 0x1 -- 6.60 kbps \n
        - 0x2 -- 8.85 kbps \n
        - 0x4 -- 12.65 kbps \n
        - 0x8 -- 14.25 kbps \n
        - 0x10 -- 15.85 kbps \n
        - 0x20 -- 18.25 kbps \n
        - 0x40 -- 19.85 kbps \n
        - 0x80 -- 23.05 kbps \n
        - 0x100 -- 23.85 kbps
    */

  /* Optional */
  /*  AMR NB Octet Aligned */
  uint8_t amr_octet_align_valid;  /**< Must be set to true if amr_octet_align is being passed */
  uint8_t amr_octet_align;
  /**<   Flag indicating if the octet is aligned for AMR NB audio. \n
        Values: \n
        -TRUE -- Octet aligned \n
        -FALSE -- Octet not aligned, Bandwidth Efficient mode
    */

  /* Optional */
  /*  AMR WB Octet Aligned */
  uint8_t amr_wb_octet_align_valid;  /**< Must be set to true if amr_wb_octet_align is being passed */
  uint8_t amr_wb_octet_align;
  /**<   Flag indicating if the octet is aligned for AMR WB audio. \n
        Values: \n
        -TRUE -- Octet aligned \n
        -FALSE -- Octet not aligned, Bandwidth Efficient mode
    */

  /* Optional */
  /*  Ringing Timer Duration */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint16_t ringing_timer;
  /**<   Duration, in seconds, of the ringing timer. The ringing timer 
         starts on the ringing event. If the call is not answered within
         the duration of this timer, the call is disconnected.
    */

  /* Optional */
  /*  Ringback Timer Duration */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint16_t ringback_timer;
  /**<   Duration, in seconds, of the ringback timers. The ringback timer 
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
    */

  /* Optional */
  /*  RTP/RTCP Inactivity Timer Duration */
  uint8_t rtp_rtcp_inactivity_timer_valid;  /**< Must be set to true if rtp_rtcp_inactivity_timer is being passed */
  uint16_t rtp_rtcp_inactivity_timer;
  /**<   Duration, in seconds, of the RTP/RTCP inactivity timer. If no 
         RTP/RTCP packet is received before the expiration of this timer, 
         the call is disconnected.
    */

  /* Optional */
  /*  VoLTE to 1xRTT Silent Redial Flag */
  uint8_t voip_silent_redial_enabled_valid;  /**< Must be set to true if voip_silent_redial_enabled is being passed */
  uint8_t voip_silent_redial_enabled;
  /**<   Flag that allows a device to silently redial over 1xRTT.
         If this TLV is not included in the request, a value of TRUE 
         (that is, enabled) is used.*/

  /* Optional */
  /*  VoIP Preferred RTP Payload Type */
  uint8_t voip_preferred_rtp_payload_type_valid;  /**< Must be set to true if voip_preferred_rtp_payload_type is being passed */
  uint16_t voip_preferred_rtp_payload_type;
  /**<   Values for the VoIP preferred codec mode. Must be set only when
         G.711 support is required in addition to AMR and AMR-WB.\n
         Refer to \color{blue}\href{http://www.iana.org/assignments/rtp-parameters/rtp-parameters.xhtml\#rtp-parameters-1}
         {Real-TimeTransport Protocol (RTP) Parameters}~\color{black} 
         for possible values.\n 
         If an unsupported codec value is set, CODEC MIME is used 
         as the default audio codec and the G.711 codec is ignored.
    */

  /* Optional */
  /*  VoIP Configuration Conference Factory URI */
  uint8_t voip_config_confURI_valid;  /**< Must be set to true if voip_config_confURI is being passed */
  char voip_config_confURI[IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01 + 1];
  /**<   VoIP configuration conference factory URI.      */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled
     */
}ims_settings_voip_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the presence-related configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Publish Timer  */
  uint8_t publish_expiry_timer_valid;  /**< Must be set to true if publish_expiry_timer is being passed */
  uint32_t publish_expiry_timer;
  /**<   Publish timer, in seconds, when publish is sent on an IMS network using 
         4G radio access technology.
    */

  /* Optional */
  /*  Publish Extended Timer  */
  uint8_t publish_extended_expiry_timer_valid;  /**< Must be set to true if publish_extended_expiry_timer is being passed */
  uint32_t publish_extended_expiry_timer;
  /**<   Publish extended timer, in seconds, when publish is sent
         on an IMS network using non-4G radio access technology, 
         or when in Airplane Power-Down mode using 4G radio access technology.
    */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t minimum_publish_interval_valid;  /**< Must be set to true if minimum_publish_interval is being passed */
  uint32_t minimum_publish_interval;
  /**<   Duration of time, in seconds, between successive publish requests.
    */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t capability_poll_list_subscription_expiry_timer_valid;  /**< Must be set to true if capability_poll_list_subscription_expiry_timer is being passed */
  uint32_t capability_poll_list_subscription_expiry_timer;
  /**<   Timer, in seconds, for the list subscribe request.
    */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enable_valid;  /**< Must be set to true if capability_discovery_enable is being passed */
  uint8_t capability_discovery_enable;
  /**<   Flag indicating whether or not discovery capability is enabled.
        Values: \n
        -TRUE -- Presence publishes/subscribes and processes any notifications 
                 received \n
        -FALSE -- Presence does not publish/subscribe and 
                  ignores any notification received
    */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiration_valid;  /**< Must be set to true if capabilites_cache_expiration is being passed */
  uint32_t capabilites_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved capability is 
         considered valid.
    */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiration_valid;  /**< Must be set to true if availability_cache_expiration is being passed */
  uint32_t availability_cache_expiration;
  /**<   Duration of time, in seconds, for which the retrieved availability is 
         considered valid.
    */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Duration of time, in seconds, for which the retrieved availability is 
         considered valid.
    */

  /* Optional */
  /*  Maximum Subscription List Entries */
  uint8_t max_subcription_list_entries_valid;  /**< Must be set to true if max_subcription_list_entries is being passed */
  uint32_t max_subcription_list_entries;
  /**<   Maximum number of entries that can be kept in the list subscription.
    */

  /* Optional */
  /*  VoLTE User Opted In Status */
  uint8_t volte_user_opted_in_status_valid;  /**< Must be set to true if volte_user_opted_in_status is being passed */
  uint8_t volte_user_opted_in_status;
  /**<   Flag indicating whether or not VoLTE service is accepted by the user. 
         Values: \n
         -TRUE -- Accepted \n
         -FALSE -- Not accepted
    */

  /* Optional */
  /*  Last Published Entity Tag */
  uint8_t last_publish_etag_valid;  /**< Must be set to true if last_publish_etag is being passed */
  char last_publish_etag[IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01 + 1];
  /**<   Last published ETAG. */

  /* Optional */
  /*  Last Published Time */
  uint8_t last_published_time_valid;  /**< Must be set to true if last_published_time is being passed */
  uint32_t last_published_time;
  /**<   Last published time. */

  /* Optional */
  /*  Last Negotiated Published Expire */
  uint8_t last_negotiated_published_expire_valid;  /**< Must be set to true if last_negotiated_published_expire is being passed */
  uint32_t last_negotiated_published_expire;
  /**<   Last negotiated published expire, in seconds. */

  /* Optional */
  /*  GNU ZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression enabled. 
         Values: \n
         -TRUE -- Enabled \n
         -FALSE -- Disabled
                  */

  /* Optional */
  /*  Presence Notification Wait Duration */
  uint8_t presence_notify_wait_duration_valid;  /**< Must be set to true if presence_notify_wait_duration is being passed */
  uint16_t presence_notify_wait_duration;
  /**<   Presence notification wait duration, in seconds. */

  /* Optional */
  /*  Publish Error Recovery Timer (Deprecated) */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds.
         This TLV is now deprecated and is now part of
         QMI_IMS_SETTINGS_ PRESENCE_EXT_CONFIG_IND.
    */
}ims_settings_presence_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the presence extended-related configuration 
             parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer, in seconds. */

  /* Optional */
  /*  Publish User Agent */
  uint8_t publish_user_agent_valid;  /**< Must be set to true if publish_user_agent is being passed */
  char publish_user_agent[IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01 + 1];
  /**<   User agent. */
}ims_settings_presence_ext_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the media-related configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  H.264 Profile */
  uint8_t h264_profile_valid;  /**< Must be set to true if h264_profile is being passed */
  ims_settings_h264_profile_enum_v01 h264_profile;
  /**<   Profile used for H.264 codec. Values: \n
      - IMS_SETTINGS_H264_PROFILE_BASELINE (0x00) --  Baseline profile
      - IMS_SETTINGS_H264_PROFILE_MAIN (0x01) --  Main profile
      - IMS_SETTINGS_H264_PROFILE_EXTENDED (0x02) --  Extended profile
      - IMS_SETTINGS_H264_PROFILE_HIGH (0x03) --  High profile */

  /* Optional */
  /*  H.264 Level */
  uint8_t h264_level_valid;  /**< Must be set to true if h264_level is being passed */
  ims_settings_h264_level_enum_v01 h264_level;
  /**<   Level used for H.264 codec. Values: \n
      - IMS_SETTINGS_H264_LEVEL1 (0x00) --  Level 1
      - IMS_SETTINGS_H264_LEVEL1B (0x01) --  Level 1b
      - IMS_SETTINGS_H264_LEVEL11 (0x02) --  Level 1.1
      - IMS_SETTINGS_H264_LEVEL12 (0x03) --  Level 1.2
      - IMS_SETTINGS_H264_LEVEL13 (0x04) --  Level 1.3
      - IMS_SETTINGS_H264_LEVEL2 (0x05) --  Level 2
      - IMS_SETTINGS_H264_LEVEL21 (0x06) --  Level 2.1
      - IMS_SETTINGS_H264_LEVEL22 (0x07) --  Level 2.2
      - IMS_SETTINGS_H264_LEVEL3 (0x08) --  Level 3
      - IMS_SETTINGS_H264_LEVEL31 (0x09) --  Level 3.1
      - IMS_SETTINGS_H264_LEVEL32 (0x0A) --  Level 3.2
      - IMS_SETTINGS_H264_LEVEL4 (0x0B) --  Level 4
      - IMS_SETTINGS_H264_LEVEL41 (0x0C) --  Level 4.1
      - IMS_SETTINGS_H264_LEVEL42 (0x0D) --  Level 4.2
      - IMS_SETTINGS_H264_LEVEL5 (0x0E) --  Level 5
      - IMS_SETTINGS_H264_LEVEL51 (0x0F) --  Level 5.1 */

  /* Optional */
  /*  Video Bitrate */
  uint8_t video_bitrate_valid;  /**< Must be set to true if video_bitrate is being passed */
  uint16_t video_bitrate;
  /**<   Bitrate of the video, in kbps. */

  /* Optional */
  /*  Video Refresh Rate */
  uint8_t video_frames_per_second_valid;  /**< Must be set to true if video_frames_per_second is being passed */
  uint8_t video_frames_per_second;
  /**<   Video refresh rate, in frames per second. */

  /* Optional */
  /*  Video Display Resolution  */
  uint8_t video_resolution_valid;  /**< Must be set to true if video_resolution is being passed */
  ims_video_resolution_enum_v01 video_resolution;
  /**<   Resolution of the video display. Values: \n
      - IMS_SETTINGS_SQCIF_RESOLUTION (0x00) --  SQCIF
      - IMS_SETTINGS_QCIF_RESOLUTION (0x01) --  QCIF
      - IMS_SETTINGS_CIF_RESOLUTION (0x02) --  CIF
      - IMS_SETTINGS_QQVGA_RESOLUTION (0x03) --  QQVGA
      - IMS_SETTINGS_QVGA_RESOLUTION (0x04) --  QVGA
      - IMS_SETTINGS_VGA_RESOLUTION (0x05) --  VGA  */

  /* Optional */
  /*  Video Codec */
  uint8_t video_codec_valid;  /**< Must be set to true if video_codec is being passed */
  ims_settings_video_codec_enum_v01 video_codec;
  /**<   Codec used for the video. Values: \n
      - IMS_SETTINGS_CODEC_MPEG4_XVID (0x00) --  XVID MPEG4 codec
      - IMS_SETTINGS_CODEC_MPEG4_ISO (0x01) --  ISO MPEG4 codec
      - IMS_SETTINGS_CODEC_H263 (0x02) --  H.263 codec
      - IMS_SETTINGS_CODEC_H264 (0x03) --  H.264 codec */

  /* Optional */
  /*  Lipsync Drop Upper Limit  */
  uint8_t lipsync_drop_upper_limit_valid;  /**< Must be set to true if lipsync_drop_upper_limit is being passed */
  uint16_t lipsync_drop_upper_limit;
  /**<   Lipsync drop upper limit in units of video samples for video clock rate
         of 90kHz. */

  /* Optional */
  /*  Lipsync Drop Lower Limit  */
  uint8_t lipsync_drop_lower_limit_valid;  /**< Must be set to true if lipsync_drop_lower_limit is being passed */
  uint16_t lipsync_drop_lower_limit;
  /**<   Lipsync drop lower limit in units of video samples for video clock rate
         of 90kHz. */

  /* Optional */
  /*  RTP MTU Size  */
  uint8_t rtp_mtu_size_valid;  /**< Must be set to true if rtp_mtu_size is being passed */
  uint16_t rtp_mtu_size;
  /**<   RTP MTU size. */

  /* Optional */
  /*  QDJ Time Warping Enable Option */
  uint8_t qdj_time_warping_enabled_valid;  /**< Must be set to true if qdj_time_warping_enabled is being passed */
  uint8_t qdj_time_warping_enabled;
  /**<   Qualcomm Dejitter buffer (QDJ) time warping. Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  QDJ IBA Maximum Value */
  uint8_t qdj_iba_max_valid;  /**< Must be set to true if qdj_iba_max is being passed */
  uint8_t qdj_iba_max;
  /**<   Maximum number of chances given to a frame, which decides underflow in 
         QDJ. While dequeuing a frame with sequence number x, this value is
         the maximum number of times to wait and look for x before moving 
         to the next frame (x+1) dequeue. 
    */

  /* Optional */
  /*  QDJ Maximum Frames to Start Dequeue */
  uint8_t qdj_max_frames_at_start_valid;  /**< Must be set to true if qdj_max_frames_at_start is being passed */
  uint8_t qdj_max_frames_at_start;
  /**<   Number of frames required in QDJ to start a dequeue. */

  /* Optional */
  /*  QDJ Maximum Dejitter Delay */
  uint8_t qdj_max_delay_valid;  /**< Must be set to true if qdj_max_delay is being passed */
  uint8_t qdj_max_delay;
  /**<   Maximum QDJ dejitter delay, in milliseconds. */

  /* Optional */
  /*  QDJ Minimum Dejitter Delay */
  uint8_t qdj_min_delay_valid;  /**< Must be set to true if qdj_min_delay is being passed */
  uint8_t qdj_min_delay;
  /**<   Minimum QDJ dejitter delay, in milliseconds. */

  /* Optional */
  /*  QDJ Optimization2 Information */
  uint8_t qdj_optimization2_info_valid;  /**< Must be set to true if qdj_optimization2_info is being passed */
  ims_settings_qdj_optimization2_info_v01 qdj_optimization2_info;

  /* Optional */
  /*  QDJ Maximum Frames at Run */
  uint8_t qdj_max_frames_at_run_valid;  /**< Must be set to true if qdj_max_frames_at_run is being passed */
  uint8_t qdj_max_frames_at_run;
  /**<   Maximum number of frames to keep in
         the queue. The oldest frame is dropped if received at frame
         (this value+1).
     */

  /* Optional */
  /*  QDJ Maximum Bumped Delay */
  uint8_t qdj_max_bumped_up_delay_valid;  /**< Must be set to true if qdj_max_bumped_up_delay is being passed */
  uint8_t qdj_max_bumped_up_delay;
  /**<   QDJ maximum bumped delay, in milliseconds. 
         This is QDJ-specific and used to decide whether to change the 
         maximum target delay if the underflow is too large. The target delay 
         varies from the minimum target delay to the maximum target delay. 
         However, in certain extreme conditions where the underflow is huge or 
         frequent packet bundling occurs, QDJ bumps up the target delay to 
         a value higher than the maximum delay, but not greater than 
         the maximum bumped delay. 
    */

  /* Optional */
  /*  QDJ Jitter Increment */
  uint8_t qdj_jitter_increment_valid;  /**< Must be set to true if qdj_jitter_increment is being passed */
  uint8_t qdj_jitter_increment;
  /**<   QDJ step delay, in milliseconds. This value is used when updating QDJ 
         for each talk spurt. */

  /* Optional */
  /*  QDJ Target Underflow Rate */
  uint8_t qdj_target_underflow_valid;  /**< Must be set to true if qdj_target_underflow is being passed */
  uint16_t qdj_target_underflow;
  /**<   Percentage of QDJ underflow, multiplied by 1000. */

  /* Optional */
  /*  QDJ Drop Threshold */
  uint8_t qdj_default_jitter_valid;  /**< Must be set to true if qdj_default_jitter is being passed */
  uint16_t qdj_default_jitter;
  /**<   QDJ default jitter: the initial default jitter, in milliseconds, to be 
         added in QDJ play out. 
    */

  /* Optional */
  /*  Gmin */
  uint8_t gmin_valid;  /**< Must be set to true if gmin is being passed */
  uint8_t gmin;
  /**<   Number of frames in a run that defines a gap and burst matrices in 
         RTCP XR report per \hyperref[RFC3611]{RFC 3611}.
    */

  /* Optional */
  /*  Transmit System Delay */
  uint8_t tx_system_delay_valid;  /**< Must be set to true if tx_system_delay is being passed */
  uint8_t tx_system_delay;
  /**<   Tx system delay, in milliseconds, that is used to calculate the end 
         system delay in an RTCP XR report.
    */

  /* Optional */
  /*  Receive System Delay */
  uint8_t rx_system_delay_valid;  /**< Must be set to true if rx_system_delay is being passed */
  uint8_t rx_system_delay;
  /**<   Rx system delay, in milliseconds, that is used to calculate the end 
         system delay in an RTCP XR report.
    */

  /* Optional */
  /*  Audio Offload */
  uint8_t audio_offload_valid;  /**< Must be set to true if audio_offload is being passed */
  ims_settings_audio_offload_option_enum_v01 audio_offload;
  /**<   Audio offload option. Values: \n
      - IMS_SETTINGS_AUDIO_OFFLOAD_AP (1) --  Audio offload to AP 
      - IMS_SETTINGS_AUDIO_OFFLOAD_NONE (2) --  No audio offload 
      - IMS_SETTINGS_AUDIO_OFFLOAD_MODEM (3) --  Audio offload to MODEM  */
}ims_settings_media_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the QIPCall-related configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  VT Calling Status */
  uint8_t vt_calling_enabled_valid;  /**< Must be set to true if vt_calling_enabled is being passed */
  uint8_t vt_calling_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Emergency Call Timer */
  uint8_t emerg_call_timer_valid;  /**< Must be set to true if emerg_call_timer is being passed */
  uint32_t emerg_call_timer;
  /**<   Emergency call timer.
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a VT call. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for the speech codec.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_end_port_valid;  /**< Must be set to true if speech_end_port is being passed */
  uint16_t speech_end_port;
  /**<   
         Largest RTP port number for the speech codec.
    */

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband.
    */

  /* Optional */
  /*  AMR Default Encoding Mode */
  uint8_t amr_default_mode_valid;  /**< Must be set to true if amr_default_mode is being passed */
  uint8_t amr_default_mode;
  /**<   
        AMR default encoding mode.
    */

  /* Optional */
  /*  Minimum Video RTP Port Range */
  uint8_t video_rtp_port_start_valid;  /**< Must be set to true if video_rtp_port_start is being passed */
  uint16_t video_rtp_port_start;
  /**<   
        Minimum video RTP port range.   
    */

  /* Optional */
  /*  Maximum Video RTP Port Range */
  uint8_t video_rtp_port_end_valid;  /**< Must be set to true if video_rtp_port_end is being passed */
  uint16_t video_rtp_port_end;
  /**<   
        Maximum video RTP port range.   
    */

  /* Optional */
  /*  Emergency Call Setup Timer on WLAN */
  uint8_t emerg_call_timer_wlan_valid;  /**< Must be set to true if emerg_call_timer_wlan is being passed */
  uint32_t emerg_call_timer_wlan;
  /**<   Emergency call setup timer on WLAN.
    */
}ims_settings_qipcall_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the registration manager extended configuration 
             parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Reregistration Delay */
  uint8_t reregistration_delay_valid;  /**< Must be set to true if reregistration_delay is being passed */
  uint16_t reregistration_delay;
  /**<   IMS reregistration wait time when RAT transitions from eHRPD to LTE, 
         in seconds.*/

  /* Optional */
  /*  Delay Length for iRAT Transition (Deprecated) */
  uint8_t t_delay_valid;  /**< Must be set to true if t_delay is being passed */
  uint16_t t_delay;
  /**<   Length of the delay for an iRAT transition, in seconds; allowed integer
         value range is 0 to 600. If this TLV is not present in the request, 
         a value of 0 is used.
         
        Note: This TLV is deprecated and was a duplicate.
        Use the Reregistration Delay TLV instead. 
    */

  /* Optional */
  /*  RegRetryBaseTime */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   RegRetryBaseTime value, in seconds. 
         RegRetryBaseTime is the value of the base-time parameter of the 
         flow recovery algorithm.
    */

  /* Optional */
  /*  RegRetryMaxTime */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   RegRetryMaxTime value, in seconds. 
         RegRetryMaxTime is the value of the max-time parameter of the 
         flow recovery algorithm.
    */
}ims_settings_reg_mgr_extended_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the policy manager configuration 
             parameters change. */
typedef struct {

  /* Optional */
  /*  Policy Manager RAT APN Information Array */
  uint8_t pol_mgr_rat_apn_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_info is being passed */
  ims_settings_pol_man_rat_apn_info_v01 pol_mgr_rat_apn_info[IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01];
  /**<   \n(Array of RAT and APN information parameters.) */

  /* Optional */
  /*  Policy Manager RAT APN Fallback and Service Priority Information Array */
  uint8_t pol_mgr_rat_apn_fb_sp_info_valid;  /**< Must be set to true if pol_mgr_rat_apn_fb_sp_info is being passed */
  ims_settings_pol_mgr_rat_apn_fb_sp_info_v01 pol_mgr_rat_apn_fb_sp_info[IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01];
  /**<   \n(Array of RAT and APN and their fallback and service priority 
           information parameters.) */

  /* Optional */
  /*  Policy Manager Allowed Services Over WLAN */
  uint8_t pol_mgr_allowed_services_wlan_valid;  /**< Must be set to true if pol_mgr_allowed_services_wlan is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_allowed_services_wlan;
  /**<   Bitmask indicating which services are allowed over WLAN. */

  /* Optional */
  /*  Policy Manager Add All Feature Tags */
  uint8_t pol_mgr_add_all_fts_valid;  /**< Must be set to true if pol_mgr_add_all_fts is being passed */
  uint8_t pol_mgr_add_all_fts;
  /**<  Indicates whether to add all the feature tag list or application. */

  /* Optional */
  /*  Policy Manager ACS Priority */
  uint8_t pol_mgr_acs_priority_valid;  /**< Must be set to true if pol_mgr_acs_priority is being passed */
  uint8_t pol_mgr_acs_priority;
  /**<  Priority of ACS values. */

  /* Optional */
  /*  Policy Manager ISIM Priority */
  uint8_t pol_mgr_isim_priority_valid;  /**< Must be set to true if pol_mgr_isim_priority is being passed */
  uint8_t pol_mgr_isim_priority;
  /**<  Priority of ISIM values. */

  /* Optional */
  /*  Policy Manager NV Priority */
  uint8_t pol_mgr_nv_priority_valid;  /**< Must be set to true if pol_mgr_nv_priority is being passed */
  uint8_t pol_mgr_nv_priority;
  /**<  Priority of preconfiguration NV values. */

  /* Optional */
  /*  Policy Manager PCO Priority */
  uint8_t pol_mgr_pco_priority_valid;  /**< Must be set to true if pol_mgr_pco_priority is being passed */
  uint8_t pol_mgr_pco_priority;
  /**<  Priority of PCO values. */

  /* Optional */
  /*  Policy Manager IMS Service Priority */
  uint8_t pol_mgr_ims_service_status_valid;  /**< Must be set to true if pol_mgr_ims_service_status is being passed */
  ims_settings_service_type_mask_v01 pol_mgr_ims_service_status;
  /**<   Bitmask indicating the services that are enabled on the device. */

  /* Optional */
  /*  Policy Manager Access Point Name List */
  uint8_t pol_mgr_apn_name_valid;  /**< Must be set to true if pol_mgr_apn_name is being passed */
  ims_settings_pol_mgr_apn_name_v01 pol_mgr_apn_name[IMS_SETTINGS_POL_MGR_APN_SIZE_V01];
  /**<  \n(IMS access point names.) */

  /* Optional */
  /*  Policy Manager RAT APN Information Array */
  uint8_t pol_mgr_rat_apn_info_extend_valid;  /**< Must be set to true if pol_mgr_rat_apn_info_extend is being passed */
  ims_settings_pol_man_rat_apn_info_v02_v01 pol_mgr_rat_apn_info_extend[IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01];
  /**<   \n(Array of RAT and APN information parameters.) */
}ims_settings_pol_mgr_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the RCS standalone messaging configuration
             parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  RCS Standalone Messaging Authorization */
  uint8_t standalone_message_auth_valid;  /**< Must be set to true if standalone_message_auth is being passed */
  uint8_t standalone_message_auth;
  /**<   Values: \n
         - TRUE - Authorized\n
         - FALSE - Unauthorized
    */

  /* Optional */
  /*  RCS Standalone Message Maximum Size */
  uint8_t standalone_message_max_size_valid;  /**< Must be set to true if standalone_message_max_size is being passed */
  uint32_t standalone_message_max_size;
  /**<   Maximum size of a standalone message.
    */

  /* Optional */
  /*  RCS Standalone Message Explorer URI */
  uint8_t standalone_message_explorer_uri_valid;  /**< Must be set to true if standalone_message_explorer_uri is being passed */
  char standalone_message_explorer_uri[IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01 + 1];
  /**<   Standalone message explorer URI.
    */
}ims_settings_rcs_sm_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the Ut Interface configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Ut Interface Disable Status */
  uint8_t ut_disabled_valid;  /**< Must be set to true if ut_disabled is being passed */
  uint8_t ut_disabled;
  /**<   Values: \n
        -TRUE -- Disable  \n
        -FALSE -- Enable
    */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t ut_apn_name_valid;  /**< Must be set to true if ut_apn_name is being passed */
  char ut_apn_name[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut Interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type */
  uint8_t ut_ip_addr_type_valid;  /**< Must be set to true if ut_ip_addr_type is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type;
  /**<   Ut Interface IP address type. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */

  /* Optional */
  /*  Ut Interface Access Point Name for Wi-Fi */
  uint8_t ut_apn_name_apn2_valid;  /**< Must be set to true if ut_apn_name_apn2 is being passed */
  char ut_apn_name_apn2[IMS_SETTINGS_UT_APN_NAME_LEN_V01 + 1];
  /**<   Ut interface APN string. */

  /* Optional */
  /*  Ut Interface IP Address Type for Wi-Fi */
  uint8_t ut_ip_addr_type_apn2_valid;  /**< Must be set to true if ut_ip_addr_type_apn2 is being passed */
  ims_settings_ip_addr_type_enum_v01 ut_ip_addr_type_apn2;
  /**<   Ut interface IP address type for APN2. Values:\n
      - IMS_SETTINGS_IP_TYPE_UNKNOWN (0x00) --  Unknown IP address type \n 
      - IMS_SETTINGS_IP_TYPE_IPV4 (0x01) --  IPv4 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV6 (0x02) --  IPv6 address \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6 (0x04) --  IPV4V6 address -- Primary IPv4, Fallback IPv6 \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4 (0x05) --  IPV6V4 address -- Primary IPv6, Fallback IPv4 \n 
      - IMS_SETTINGS_IP_TYPE_IPV4V6PIP (0x06) --  IPV4V6 address -- Primary IPv4, Fallback IPv6; use primary IP type after LPM/online \n 
      - IMS_SETTINGS_IP_TYPE_IPV6V4PIP (0x07) --  IPV6V4 address -- Primary IPv6, Fallback IPv4; use primary IP type after LPM/online 
 */
}ims_settings_ut_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the client provisioning configuration parameters 
             change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Client Provisioning Enabled Status */
  uint8_t client_provisioning_enabled_valid;  /**< Must be set to true if client_provisioning_enabled is being passed */
  uint8_t client_provisioning_enabled;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Enabled VoLTE Support Through Client Provisioning */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Enabled VT Support Through Client Provisioning */
  uint8_t vt_enabled_valid;  /**< Must be set to true if vt_enabled is being passed */
  uint8_t vt_enabled;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Enabled Presence Support Through Client Provisioning */
  uint8_t presence_enabled_valid;  /**< Must be set to true if presence_enabled is being passed */
  uint8_t presence_enabled;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Wi-Fi Call Setting */
  uint8_t wifi_call_valid;  /**< Must be set to true if wifi_call is being passed */
  ims_settings_wfc_status_enum_v01 wifi_call;
  /**<   WFC status. Values: \n
      - IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED (0) --  WFC is not supported 
      - IMS_SETTINGS_WFC_STATUS_ON (1) --  WFC is enabled 
      - IMS_SETTINGS_WFC_STATUS_OFF (2) --  WFC is disabled 
 */

  /* Optional */
  /*  Wi-Fi Call Preference Setting */
  uint8_t wifi_call_preference_valid;  /**< Must be set to true if wifi_call_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_preference;
  /**<   WFC home preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */

  /* Optional */
  /*  Wi-Fi Call Roaming Setting */
  uint8_t wifi_call_roaming_valid;  /**< Must be set to true if wifi_call_roaming is being passed */
  ims_settings_wfc_roaming_enum_v01 wifi_call_roaming;
  /**<   WFC roaming mode. Values: \n
      - IMS_SETTINGS_WFC_ROAMING_NOT_SUPPORTED (0) --  WFC roaming is not supported 
      - IMS_SETTINGS_WFC_ROAMING_ENABLED (1) --  WFC roaming is enabled 
      - IMS_SETTINGS_WFC_ROAMING_DISABLED (2) --  WFC roaming is disabled 
 */

  /* Optional */
  /*  Enabled VoWiFi Support Through Client Provisioning */
  uint8_t vowifi_enabled_valid;  /**< Must be set to true if vowifi_enabled is being passed */
  uint8_t vowifi_enabled;
  /**<   Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  VoWiFi Provisioning ID */
  uint8_t vowifi_provId_valid;  /**< Must be set to true if vowifi_provId is being passed */
  char vowifi_provId[IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01 + 1];
  /**<   Standalone message of the VoWiFi provisioning ID.
    */

  /* Optional */
  /*  WFC Roam Preference */
  uint8_t wifi_call_roam_preference_valid;  /**< Must be set to true if wifi_call_roam_preference is being passed */
  ims_settings_wfc_preference_v01 wifi_call_roam_preference;
  /**<   WFC roaming preference mode. Values: \n
      - IMS_SETTINGS_WFC_CALL_PREF_NONE (0) --  None 
      - IMS_SETTINGS_WFC_WLAN_PREFERRED (1) --  WLAN preferred mode 
      - IMS_SETTINGS_WFC_WLAN_ONLY (2) --  WLAN only mode 
      - IMS_SETTINGS_WFC_CELLULAR_PREFERRED (3) --  Cellular preferred mode 
      - IMS_SETTINGS_WFC_CELLULAR_ONLY (4) --  Cellular only mode 
 */
}ims_settings_client_provisioning_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the VT configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  H.263 Payload Type in 3G */
  uint8_t vt_3g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_3g_h263_dynamic_pt is being passed */
  uint16_t vt_3g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 3G. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  H.263 Payload Type in 4G */
  uint8_t vt_4g_h263_dynamic_pt_valid;  /**< Must be set to true if vt_4g_h263_dynamic_pt is being passed */
  uint16_t vt_4g_h263_dynamic_pt;
  /**<   
        Dynamic payload type for H.263 video encoding in 4G. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  Number of 4G H.264 Configurations */
  uint8_t num_vt_4g_h264_config_valid;  /**< Must be set to true if num_vt_4g_h264_config is being passed */
  uint8_t num_vt_4g_h264_config;
  /**<   
        Number of configuration present in the 4G H.264 Configuration
        Information TLV.
    */

  /* Optional */
  /*  4G H.264 Configuration Information */
  uint8_t vt_4g_h264_info_valid;  /**< Must be set to true if vt_4g_h264_info is being passed */
  ims_settings_vt_h264_info_v01 vt_4g_h264_info[IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01];
  /**<   \n(Array of H.264 information parameters in 4G.) */
}ims_settings_vt_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the RCS configuration parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IR.94 Video Calling Service Status */
  uint8_t ir94_video_calling_service_enabled_valid;  /**< Must be set to true if ir94_video_calling_service_enabled is being passed */
  uint8_t ir94_video_calling_service_enabled;
  /**<   
        User authorization to use an IR.94 video calling service. Values:\n
        - TRUE -- IR.94 video calling is enabled \n
        - FALSE -- IR.94 video calling is disabled.
    */

  /* Optional */
  /*  RCS Messaging Service Status  */
  uint8_t rcs_messaging_enabled_valid;  /**< Must be set to true if rcs_messaging_enabled is being passed */
  uint8_t rcs_messaging_enabled;
  /**<   
        RCS messaging status. Values: \n
        - TRUE -- RCS messaging is enabled \n
        - FALSE -- RCS messaging is disabled 
    */
}ims_settings_acs_network_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the SIP read-only-related configuration parameters 
             change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   Maximum duration, in milliseconds, that a message remains in the 
         network.
    */

  /* Optional */
  /*  TCP Threshold Value */
  uint8_t tcp_threshold_value_valid;  /**< Must be set to true if tcp_threshold_value is being passed */
  uint16_t tcp_threshold_value;
  /**<   Defines the packet size limiting value, in bytes.
    */

  /* Optional */
  /*  Compact Form Enabled */
  uint8_t compact_form_enabled_valid;  /**< Must be set to true if compact_form_enabled is being passed */
  uint8_t compact_form_enabled;
  /**<   Indicates whether the SIP compact form is enabled.
    */

  /* Optional */
  /*  Authentication Scheme */
  uint8_t settings_auth_scheme_valid;  /**< Must be set to true if settings_auth_scheme is being passed */
  ims_settings_config_auth_scheme_enum_v01 settings_auth_scheme;
  /**<   Authentication scheme configuration. Values: \n
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_NONE (0x00) --  No scheme used 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_DIGEST (0x01) --  Digest 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_SAG (0x02) --  Token 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKA (0x03) --  AKA 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_CAVE (0x04) --  CAVE 
      - IMS_SETTINGS_CONFIG_AUTH_SCHEME_AKAV2 (0x05) --  AKAv2 
 */

  /* Optional */
  /*  Initial Authorization Type */
  uint8_t settings_initial_auth_config_valid;  /**< Must be set to true if settings_initial_auth_config is being passed */
  ims_settings_config_initial_auth_type_enum_v01 settings_initial_auth_config;
  /**<   Initial authorization type value. Values: \n
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_NONE (0x00) --  None 
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_AUTHORIZATION (0x01) --  Authorization 
      - IMS_SETTINGS_CONFIG_INITIAL_AUTH_PROXY_AUTHORIZATION (0x02) --  Proxy authorization 
 */

  /* Optional */
  /*  Authorization Header Value */
  uint8_t auth_header_value_valid;  /**< Must be set to true if auth_header_value is being passed */
  char auth_header_value[IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01 + 1];
  /**<   Authorization header value. */

  /* Optional */
  /*  Proxy Route Value */
  uint8_t proxy_route_value_valid;  /**< Must be set to true if proxy_route_value is being passed */
  char proxy_route_value[IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01 + 1];
  /**<   Route value to be used by the shared configuration. */
}ims_settings_sip_read_only_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the network read-only-related configuration 
             parameters change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IPv6 Enabled */
  uint8_t ipv6_enabled_valid;  /**< Must be set to true if ipv6_enabled is being passed */
  uint8_t ipv6_enabled;
  /**<   Indicates whether the IPv6 address is enabled. */

  /* Optional */
  /*  IPSec Integrity Scheme */
  uint8_t ip_sec_int_scheme_valid;  /**< Must be set to true if ip_sec_int_scheme is being passed */
  ims_settings_integ_algo_mask_v01 ip_sec_int_scheme;
  /**<   Bitmask indicating the integrity algorithm combination. 
 Values: \n
      - IMS_SETTINGS_INTEG_ALGO_HMAC_SHA_1_96 (0x01) --  HMAC-SHA-1-96 algorithm is used for IPSec integrity 
      - IMS_SETTINGS_INTEG_ALGO_HMAC_MD5_96 (0x02) --  HMAC-MD5-96 algorithm is used for IPSec integrity  
 */

  /* Optional */
  /*  IPSec Encryption Algorithm */
  uint8_t ip_sec_enc_algo_valid;  /**< Must be set to true if ip_sec_enc_algo is being passed */
  ims_settings_encrypt_algo_mask_v01 ip_sec_enc_algo;
  /**<   Bitmask indicating the IPSec encryption algorithm combination. 
 Values: \n
      - IMS_SETTINGS_ENCRYPT_ALGO_NULL (0x01) --  NULL algorithm is used for IPSec encryption 
      - IMS_SETTINGS_ENCRYPT_ALGO_AES_CBC (0x02) --  AES-CBC algorithm is used for IPSec encryption 
      - IMS_SETTINGS_ENCRYPT_ALGO_DES_EDE3_CBC (0x04) --  DES-EDE3-CBC algorithm is used for IPSec encryption  
 */

  /* Optional */
  /*  Chunk Size of MSRP Packet */
  uint8_t msrp_pkt_size_valid;  /**< Must be set to true if msrp_pkt_size is being passed */
  uint16_t msrp_pkt_size;
  /**<   Indicates MSRP packet chunk size in KB (kilobytes).
         Default value: 2KB.
    */
}ims_settings_network_read_only_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the handover configuration parameters change. */
typedef struct {

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to WWAN */
  uint8_t wlan_to_wwan_hys_timer_valid;  /**< Must be set to true if wlan_to_wwan_hys_timer is being passed */
  uint32_t wlan_to_wwan_hys_timer;
  /**<   IMS handover hysteresis timer for WLAN to WWAN, in seconds.
	     This timer is started if the LTE to WLAN idle mode handover
	     occurs due to LTE RAT loss. When in cellular-preferred mode, WLAN to
	     LTE handover only occurs after LTE has been available for this period
	     of time. If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  Voice Services Handover Hysteresis Timer WLAN to 1X */
  uint8_t wlan_to_1x_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_hys_timer is being passed */
  uint32_t wlan_to_1x_hys_timer;
  /**<   Hysteresis timer for moving voice services from WLAN to 1X, in seconds.
	     This timer is started if a 1X system is used for voice service and
		 1X quality drops below a threshold in idle mode. When in
		 cellular-preferred mode, voice service moves to 1X only after 1X is
		 available with improved quality for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN to WLAN */
  uint8_t wwan_to_wlan_hys_timer_valid;  /**< Must be set to true if wwan_to_wlan_hys_timer is being passed */
  uint32_t wwan_to_wlan_hys_timer;
  /**<   IMS handover hysteresis timer for WWAN to WLAN, in seconds.
	     In Wi-Fi preferred mode, this timer is started if a WLAN to LTE idle
	     mode handover occurs because of WLAN RAT loss. LTE to WLAN handover
	     only occurs after WLAN is available for this period of time.
		 If this value is not present, a default of 120 sec is used.
	*/

  /* Optional */
  /*  IMS Handover Enabled Between WWAN and WLAN */
  uint8_t ims_pdn_ho_enabled_valid;  /**< Must be set to true if ims_pdn_ho_enabled is being passed */
  ims_settings_handover_option_enum_v01 ims_pdn_ho_enabled;
  /**<   IMS handover mode. Values: \n
      - IMS_SETTINGS_HO_ENABLED_ALL (1) --  Handover is enabled from WWAN to WLAN and WLAN to WWAN 
      - IMS_SETTINGS_HO_DISABLED_ALL (2) --  Handover is disabled from WWAN to WLAN and WLAN to WWAN  
      - IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY (3) --  Handover is only enabled from WLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY (4) --  Handover is only enabled from WWAN to WLAN 
      - IMS_SETTINGS_HO_ENABLED_IWLAN_TO_WWAN_ONLY (5) --  Handover is only enabled from IWLAN to WWAN 
      - IMS_SETTINGS_HO_ENABLED_WWAN_TO_IWLAN_ONLY (6) --  Handover is only enabled from WWAN to IWLAN 
 */

  /* Optional */
  /*  IMS 1x RSSI Threshold */
  uint8_t iIMSHO1xQualTH_valid;  /**< Must be set to true if iIMSHO1xQualTH is being passed */
  int32_t iIMSHO1xQualTH;
  /**<   This value defines the 1x threshold value for comparing averaged signal strength.
         Scenario: If the signal quality of 1x falls below iIMSHO1xQualTH and Wi-Fi is available, 
         the device deregisters from 1x and connects to IMS PDN over Wi-Fi for voice calls. \n
		 Default value --> -7 db
	*/

  /* Optional */
  /*  IMS LTE Threshold1 */
  uint8_t iIMSHOLTEQualTH1_valid;  /**< Must be set to true if iIMSHOLTEQualTH1 is being passed */
  int32_t iIMSHOLTEQualTH1;
  /**<   This value defines LTE Threshold 1. 
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -117 db
	*/

  /* Optional */
  /*  IMS LTE Threshold2 */
  uint8_t iIMSHOLTEQualTH2_valid;  /**< Must be set to true if iIMSHOLTEQualTH2 is being passed */
  int32_t iIMSHOLTEQualTH2;
  /**<   This value defines LTE Threshold 2.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice only. \n
		 Default value --> -115 db
	*/

  /* Optional */
  /*  IMS LTE Threshold3  */
  uint8_t iIMSHOLTEQualTH3_valid;  /**< Must be set to true if iIMSHOLTEQualTH3 is being passed */
  int32_t iIMSHOLTEQualTH3;
  /**<   This value defines LTE Threshold 3.     
         Handover scenario: Wi-Fi to LTE and LTE to Wi-Fi.
         Used for In-call mode for voice only. \n
		 Default value --> -110 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdA */
  uint8_t iIMSHOWIFIQualTHA_valid;  /**< Must be set to true if iIMSHOWIFIQualTHA is being passed */
  int32_t iIMSHOWIFIQualTHA;
  /**<   This value defines Wi-Fi Threshold A.
         Handover scenario: LTE to Wi-Fi. 
         Used for In-call mode for voice and video. \n
		 Default value --> -85 db
	*/

  /* Optional */
  /*  IMS Wi-Fi ThresholdB */
  uint8_t iIMSHOWIFIQualTHB_valid;  /**< Must be set to true if iIMSHOWIFIQualTHB is being passed */
  int32_t iIMSHOWIFIQualTHB;
  /**<   This value defines Wi-Fi Threshold B.
         Handover scenario: Wi-Fi to LTE. 
         Used for In-call mode for voice and video. \n
		 Default value --> -95 db
	*/

  /* Optional */
  /*  HO disabled mask */
  uint8_t ho_disabled_mask_valid;  /**< Must be set to true if ho_disabled_mask is being passed */
  ims_settings_ho_disabled_mask_v01 ho_disabled_mask;
  /**<   Mask of Ho disabled
    */

  /* Optional */
  /*  IMS LTE RSRQ Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Src is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Src;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -120 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Source threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Src_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Src is being passed */
  int32_t iIR92_LTE_DL_SNR_Src;
  /**<   This value defines LTE SNR Threshold (scaled dB) when UE handover algorithm is running in IR92 mode.    
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> 95 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS LTE RSRQ Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_RSRQ_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_RSRQ_Target is being passed */
  int32_t iIR92_LTE_DL_RSRQ_Target;
  /**<   This value defines LTE RSRQ Threshold (dB) when UE handover algorithm is running in IR92 mode.  
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -100 dB
	*/

  /* Optional */
  /*  IMS LTE SNR Target threhsold for IR92 */
  uint8_t iIR92_LTE_DL_SNR_Target_valid;  /**< Must be set to true if iIR92_LTE_DL_SNR_Target is being passed */
  int32_t iIR92_LTE_DL_SNR_Target;
  /**<   This value defines LTE SNR Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> 105 dB (scaled: (log_SNR[db] + 200)/2)
	*/

  /* Optional */
  /*  IMS Wifi RSSI Source threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_Out_valid;  /**< Must be set to true if iIR92_Wifi_Rove_Out is being passed */
  int32_t iIR92_Wifi_Rove_Out;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.     
         Handover scenario: Wi-Fi to LTE.
         Used for In-call mode and idle mode.
		 Default value --> -85 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Target threhsold for IR92 */
  uint8_t iIR92_Wifi_Rove_In_valid;  /**< Must be set to true if iIR92_Wifi_Rove_In is being passed */
  int32_t iIR92_Wifi_Rove_In;
  /**<   This value defines Wifi RSSI Threshold (dB) when UE handover algorithm is running in IR92 mode.   
         Handover scenario: LTE to Wi-Fi.
         Used for In-call mode and idle mode.
		 Default value --> -65 dB
	*/

  /* Optional */
  /*  IMS Wifi RSSI Repoint threhsold for IR92 */
  uint8_t iIR92_Wifi_Repoint_valid;  /**< Must be set to true if iIR92_Wifi_Repoint is being passed */
  int32_t iIR92_Wifi_Repoint;
  /**<   This value defines Wifi RSSI Threshold (dB) for CS-Repoint for IR92.     
         Handover scenario: Used to page incoming calls over CS damain as Wifi RSSI falls below this value.
         Used for In-call mode
		 Default value --> -75 dB
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP Radio */
  uint8_t wlan_to_3gpp_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp_idle_mode_hys_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving voice services from WLAN to 3GPP RAT in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from UTRAN\GERAN RAT to 
     WLAN during idle mode because of UTRAN\GERAN RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 3GPP2 Radio */
  uint8_t wlan_to_3gpp2_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_3gpp2_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_3gpp2_idle_mode_hys_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving voice services from WLAN to 3GPP2 RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from eHRPD\eVDO RAT to 
     WLAN during idle mode because of eHRPD\eVDO RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to 1x Radio */
  uint8_t wlan_to_1x_idle_mode_hys_timer_valid;  /**< Must be set to true if wlan_to_1x_idle_mode_hys_timer is being passed */
  uint32_t wlan_to_1x_idle_mode_hys_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving voice services from WLAN to 1x RATs in seconds.
     This timer is started in cellular preferred mode if IMS PDN is brought up on WLAN 
     during idle mode because of 1X RAT loss.
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_cell_pref_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving voice services from WLAN to LTE RATs during VOIP call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during VOIP call because of LTE RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_voip_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_voip_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_voip_wifi_pref_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving voice services from WLAN to LTE RAT during VOIP call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during VOIP call because of WLAN RAT unavailability for IMS. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_cell_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_cell_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_cell_pref_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in cellular preferred mode if IMS PDN moved from LTE RAT to 
     WLAN during Video call because of LTE RAT loss. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WLAN to LTE Radio */
  uint8_t wlan_to_lte_vt_wifi_pref_timer_valid;  /**< Must be set to true if wlan_to_lte_vt_wifi_pref_timer is being passed */
  uint32_t wlan_to_lte_vt_wifi_pref_timer;
  /**<   QMI Indication for the Hysteresis timer for not moving Video services from WLAN to LTE RAT during during Video call in seconds.
     This timer is started in Wi-Fi preferred mode if IMS PDN moved from WLAN RAT to 
     LTE during Video call because of WLAN RAT unavailability for IMS. 
	*/

  /* Optional */
  /*  IMS Handover Hysteresis Timer WWAN Radio to WLAN */
  uint8_t wwan_to_wlan_all_mode_hysteresis_timer_valid;  /**< Must be set to true if wwan_to_wlan_all_mode_hysteresis_timer is being passed */
  uint32_t wwan_to_wlan_all_mode_hysteresis_timer;
  /**<   
	This timer is started when ims handovers/moves from wifi to any cellular rat (lte, ehrpd, etc) to avoid coming back to wifi 
	immediately and there by avoiding pdn ping-pongs. It is started during idle mode irrespective of user preference (cellular or wifi preference). 
	*/
}ims_settings_handover_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS application's  VICE configuration parameters
             change. (Deprecated) */
typedef struct {

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t qipcall_vice_enabled_valid;  /**< Must be set to true if qipcall_vice_enabled is being passed */
  uint8_t qipcall_vice_enabled;
  /**<   Indicates whether VICE is enabled. */
}ims_settings_qipcall_vice_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS policy manager parameters change. */
typedef struct {

  /* Optional */
  /*  IPSec Enabled or Not */
  uint8_t rat_ipsec_enabled_valid;  /**< Must be set to true if rat_ipsec_enabled is being passed */
  ims_settings_pol_mgr_rat_ipsec_info_v01 rat_ipsec_enabled[IMS_SETTINGS_POL_MGR_RAT_MAX_V01];
  /**<   Indicates whether the IPSec is enabled. */
}ims_settings_pol_mgr_rat_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS Session Initiation Protocol (SIP) configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Timer T1   */
  uint8_t timer_t1_valid;  /**< Must be set to true if timer_t1 is being passed */
  uint32_t timer_t1;
  /**<   RTT estimate, in milliseconds. Default value as per RFC3261 if not set .*/

  /* Optional */
  /*  Timer T2  */
  uint8_t timer_t2_valid;  /**< Must be set to true if timer_t2 is being passed */
  uint32_t timer_t2;
  /**<   Maximum retransmit interval, in milliseconds, for non-INVITE requests 
         and invite responses. Default value as per RFC3261 if not set. */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   SIP timer T4 value, in milliseconds. 
         Timer T4 is the maximum duration that a SIP message can remain in the 
         network. Default value as per RFC3261 if not set. */

  /* Optional */
  /*  SIP Timer A */
  uint8_t timer_ta_valid;  /**< Must be set to true if timer_ta is being passed */
  uint32_t timer_ta;
  /**<   SIP timer A value, in milliseconds. 
         Timer A is the INVITE request retransmit interval; for UDP only.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer B Value */
  uint8_t timer_tb_valid;  /**< Must be set to true if timer_tb is being passed */
  uint32_t timer_tb;
  /**<   SIP timer B value, in milliseconds. 
         INVITE transaction timeout timer.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer C */
  uint8_t timer_tc_valid;  /**< Must be set to true if timer_tc is being passed */
  uint32_t timer_tc;
  /**<   SIP timer C value, in milliseconds. 
         Proxy INVITE transaction timeout.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer D Value */
  uint8_t timer_td_valid;  /**< Must be set to true if timer_td is being passed */
  uint32_t timer_td;
  /**<   SIP timer D value, in milliseconds. 
         Wait time for response retransmits.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer E */
  uint8_t timer_te_valid;  /**< Must be set to true if timer_te is being passed */
  uint32_t timer_te;
  /**<   SIP timer E value, in milliseconds. 
         Timer E is the value non-INVITE request retransmit interval; for UDP only.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  Timer TF  */
  uint8_t timer_tf_valid;  /**< Must be set to true if timer_tf is being passed */
  uint32_t timer_tf;
  /**<   SIP timer F value, in milliseconds.
         Timer F is the value non-INVITE transaction timeout timer.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer G */
  uint8_t timer_tg_valid;  /**< Must be set to true if timer_tg is being passed */
  uint32_t timer_tg;
  /**<   SIP timer G value, in milliseconds. 
         Timer G is the value of the INVITE response retransmit interval.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer H */
  uint8_t timer_th_valid;  /**< Must be set to true if timer_th is being passed */
  uint32_t timer_th;
  /**<   SIP timer H value, in milliseconds. 
         Timer H is the value of the wait time for an ACK receipt.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer I */
  uint8_t timer_ti_valid;  /**< Must be set to true if timer_ti is being passed */
  uint32_t timer_ti;
  /**<   SIP timer I value, in milliseconds. 
         Timer I is the value of the wait time for ACK retransmits.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  Timer TJ */
  uint8_t timer_tj_valid;  /**< Must be set to true if timer_tj is being passed */
  uint32_t timer_tj;
  /**<   Wait time, in milliseconds,
         for non-INVITE request retransmits.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  SIP Timer K */
  uint8_t timer_tk_valid;  /**< Must be set to true if timer_tk is being passed */
  uint32_t timer_tk;
  /**<   SIP timer K value, in milliseconds. 
         Timer K is the value of the wait time for response retransmits.
         Default value as per RFC3261 if not set.
    */

  /* Optional */
  /*  Keep Alive Status */
  uint8_t keepalive_enabled_valid;  /**< Must be set to true if keepalive_enabled is being passed */
  uint8_t keepalive_enabled;
  /**<   Applicable only when the UE is behind NAT.
         Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable (default)
    */

  /* Optional */
  /*  SIP Call setup Timer */
  uint8_t timer_tcallsetup_valid;  /**< Must be set to true if timer_tcallsetup is being passed */
  uint32_t timer_tcallsetup;
  /**<   SIP call setup timer value, in milliseconds.
         If not set, an operator-defined default value will be used.
    */
}ims_settings_set_sip_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS Session Initiation Protocol (SIP) configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.\n
         Values: \n
         - 0 -- No error \n
         - 1 -- Not ready \n
         - 2 -- File not available \n
         - 3 -- Message read failed \n
         - 4 -- Message write failed \n
         - 5 -- Other internal error 
    */
}ims_settings_set_sip_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the SIP configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_sip_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the SIP configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.\n
         Values: \n
         - 0 -- No error \n
         - 1 -- Not ready \n
         - 2 -- File not available \n
         - 3 -- Message read failed \n
         - 4 -- Message write failed \n
         - 5 -- Other internal error 
    */

  /* Optional */
  /*  Timer T1   */
  uint8_t timer_t1_valid;  /**< Must be set to true if timer_t1 is being passed */
  uint32_t timer_t1;
  /**<   RTT estimate, in milliseconds. */

  /* Optional */
  /*  Timer T2  */
  uint8_t timer_t2_valid;  /**< Must be set to true if timer_t2 is being passed */
  uint32_t timer_t2;
  /**<   Maximum retransmit interval, in milliseconds, for non-INVITE requests 
         and INVITE responses. */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t timer_t4_valid;  /**< Must be set to true if timer_t4 is being passed */
  uint32_t timer_t4;
  /**<   SIP timer T4 value, in milliseconds. 
         Timer T4 is the maximum duration that a SIP message can remain in the network.
    */

  /* Optional */
  /*  SIP Timer A */
  uint8_t timer_ta_valid;  /**< Must be set to true if timer_ta is being passed */
  uint32_t timer_ta;
  /**<   SIP timer A value, in milliseconds. 
         Timer A is the INVITE request retransmit interval; for UDP only.
    */

  /* Optional */
  /*  SIP Timer B Value */
  uint8_t timer_tb_valid;  /**< Must be set to true if timer_tb is being passed */
  uint32_t timer_tb;
  /**<   SIP timer B value, in milliseconds. 
         INVITE transaction timeout timer.
    */

  /* Optional */
  /*  SIP Timer C */
  uint8_t timer_tc_valid;  /**< Must be set to true if timer_tc is being passed */
  uint32_t timer_tc;
  /**<   SIP timer C value, in milliseconds. 
         Proxy INVITE transaction timeout.
    */

  /* Optional */
  /*  SIP Timer D Value */
  uint8_t timer_td_valid;  /**< Must be set to true if timer_td is being passed */
  uint32_t timer_td;
  /**<   SIP timer D value, in milliseconds. 
         Wait time for response retransmits.
    */

  /* Optional */
  /*  SIP Timer E */
  uint8_t timer_te_valid;  /**< Must be set to true if timer_te is being passed */
  uint32_t timer_te;
  /**<   SIP timer E value, in milliseconds. 
         Timer E is the value of the non-INVITE request retransmit interval; for UDP only.
    */

  /* Optional */
  /*  Timer TF  */
  uint8_t timer_tf_valid;  /**< Must be set to true if timer_tf is being passed */
  uint32_t timer_tf;
  /**<   SIP timer F value, in milliseconds.
         Timer F is the value of the non-INVITE transaction timeout timer.
    */

  /* Optional */
  /*  SIP Timer G */
  uint8_t timer_tg_valid;  /**< Must be set to true if timer_tg is being passed */
  uint32_t timer_tg;
  /**<   SIP timer G value, in milliseconds. 
         Timer G is the value of the INVITE response retransmit interval.
    */

  /* Optional */
  /*  SIP Timer H */
  uint8_t timer_th_valid;  /**< Must be set to true if timer_th is being passed */
  uint32_t timer_th;
  /**<   SIP timer H value, in milliseconds. 
         Timer H is the value of the wait time for an ACK receipt.
    */

  /* Optional */
  /*  SIP Timer I */
  uint8_t timer_ti_valid;  /**< Must be set to true if timer_ti is being passed */
  uint32_t timer_ti;
  /**<   SIP timer I value, in milliseconds. 
         Timer I is the value of the wait time for ACK retransmits.
    */

  /* Optional */
  /*  Timer TJ */
  uint8_t timer_tj_valid;  /**< Must be set to true if timer_tj is being passed */
  uint32_t timer_tj;
  /**<   Wait time, in milliseconds,
         for non-INVITE request retransmits
    */

  /* Optional */
  /*  SIP Timer K */
  uint8_t timer_tk_valid;  /**< Must be set to true if timer_tk is being passed */
  uint32_t timer_tk;
  /**<   SIP timer Ks value, in milliseconds. 
         Timer K is the value of the wait time for response retransmits. */

  /* Optional */
  /*  Keep Alive Status */
  uint8_t keepalive_enabled_valid;  /**< Must be set to true if keepalive_enabled is being passed */
  uint8_t keepalive_enabled;
  /**<   Applicable only when the UE is behind NAT.
         Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable (default)
    */

  /* Optional */
  /*  SIP Call setup Timer */
  uint8_t timer_tcallsetup_valid;  /**< Must be set to true if timer_tcallsetup is being passed */
  uint32_t timer_tcallsetup;
  /**<   SIP call setup timer value, in milliseconds.
    */
}ims_settings_get_sip_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS presence-related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Publish Timer */
  uint8_t publish_timer_valid;  /**< Must be set to true if publish_timer is being passed */
  uint32_t publish_timer;
  /**<   Expires the header value for the PUBLISH request when the device is IMS registered on an LTE network (in seconds).
         If not set, the default is 1200.
    */

  /* Optional */
  /*  Publish Extended Timer */
  uint8_t publish_extended_timer_valid;  /**< Must be set to true if publish_extended_timer is being passed */
  uint32_t publish_extended_timer;
  /**<   Expires the header value for the PUBLISH request when the device is on an eHRPD network (in seconds).
         If not set, the default is 86400.
    */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t publish_src_throttle_timer_valid;  /**< Must be set to true if publish_src_throttle_timer is being passed */
  uint32_t publish_src_throttle_timer;
  /**<   Timer in seconds that determines the minimal interval between each PUBLISH request;
         the device must wait until this timer expires before it sends next PUBLISH request.
         If not set, the default is 60.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer in seconds.
         If not set, the default is 21600.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t list_subscription_expiry_valid;  /**< Must be set to true if list_subscription_expiry is being passed */
  uint32_t list_subscription_expiry;
  /**<   Expires the header value for the list SUBSCRIBE request when performing capability polling for more than one contact number (in seconds).
         If not set, the default is 30.
    */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiry_valid;  /**< Must be set to true if capabilites_cache_expiry is being passed */
  uint32_t capabilites_cache_expiry;
  /**<   How long (in seconds) cached capability information of a particular contact is still valid before it is updated by Capability Polling
         If not set, Default used as 7776000
    */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiry_valid;  /**< Must be set to true if availability_cache_expiry is being passed */
  uint32_t availability_cache_expiry;
  /**<   How long (in seconds) cached availability information of a particular contact is considered still valid before a new Availability Fetch needs to be sent.
         If not set, the default is 60 sec.
    */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Time interval in seconds between each Capability Polling.
         If not set, the default is 625000.
    */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t max_enties_in_list_subscribe_valid;  /**< Must be set to true if max_enties_in_list_subscribe is being passed */
  uint32_t max_enties_in_list_subscribe;
  /**<   Maximum number of contacts that can be included in a list subscribe request.
         The default value is 100.
    */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enabled_valid;  /**< Must be set to true if capability_discovery_enabled is being passed */
  uint8_t capability_discovery_enabled;
  /**<   Flag indicating whether discovery capability is enabled. \n
         Values: \n
         -TRUE -- Presence publishes/subscribes and processes any notification 
                  received (default) \n
         -FALSE -- Presence does not publish/subscribe and 
                  ignores any notification received */

  /* Optional */
  /*  GZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression is enabled. 
         Values: \n
         -TRUE -- Enabled (default) \n
         -FALSE -- Disabled
    */

  /* Optional */
  /*  User Agent */
  uint8_t user_agent_valid;  /**< Must be set to true if user_agent is being passed */
  char user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   User agent string.
         If not set, the default is NULL.
    */
}ims_settings_set_presence_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS presence-related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_presence_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the presence-related configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_presence_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the presence-related configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
       */

  /* Optional */
  /*  Publish Timer */
  uint8_t publish_timer_valid;  /**< Must be set to true if publish_timer is being passed */
  uint32_t publish_timer;
  /**<   Expires the header value for the PUBLISH request when the device is IMS registered on an LTE network (in seconds).
    */

  /* Optional */
  /*  Publish Extended Timer */
  uint8_t publish_extended_timer_valid;  /**< Must be set to true if publish_extended_timer is being passed */
  uint32_t publish_extended_timer;
  /**<   Expires the header value for the PUBLISH request when the device is on an eHRPD network (in seconds).
    */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t publish_src_throttle_timer_valid;  /**< Must be set to true if publish_src_throttle_timer is being passed */
  uint32_t publish_src_throttle_timer;
  /**<   Timer in seconds that determines the minimum interval between each PUBLISH request;
         the device must wait until this timer expires before it sends the next PUBLISH request.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer in seconds.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t list_subscription_expiry_valid;  /**< Must be set to true if list_subscription_expiry is being passed */
  uint32_t list_subscription_expiry;
  /**<   Expires the header value for the list SUBSCRIBE request when performing capability polling for more than one contact number (in seconds).
    */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiry_valid;  /**< Must be set to true if capabilites_cache_expiry is being passed */
  uint32_t capabilites_cache_expiry;
  /**<   How long (in seconds) cached capability information of a particular contact is still valid before it is updated by Capability Polling.
    */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiry_valid;  /**< Must be set to true if availability_cache_expiry is being passed */
  uint32_t availability_cache_expiry;
  /**<   How long (in seconds) cached availability information of a particular contact is considered still valid before a new Availability Fetch needs to be sent.
    */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Time interval in seconds between each Capability Polling. */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t max_enties_in_list_subscribe_valid;  /**< Must be set to true if max_enties_in_list_subscribe is being passed */
  uint32_t max_enties_in_list_subscribe;
  /**<   Maximum number of contacts that can be included in a list subscribe request.
    */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enabled_valid;  /**< Must be set to true if capability_discovery_enabled is being passed */
  uint8_t capability_discovery_enabled;
  /**<   Flag indicating whether discovery capability is enabled. \n
         Values: \n
         -TRUE -- Presence publishes/subscribes and processes any notification 
                  received \n
         -FALSE -- Presence does not publish/subscribe and 
                  ignores any notification received */

  /* Optional */
  /*  GZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression is enabled. 
         Values: \n
         -TRUE -- Enabled \n
         -FALSE -- Disabled
    */

  /* Optional */
  /*  User Agent */
  uint8_t user_agent_valid;  /**< Must be set to true if user_agent is being passed */
  char user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   User agent. */
}ims_settings_get_presence_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the presence-related configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Publish Timer */
  uint8_t publish_timer_valid;  /**< Must be set to true if publish_timer is being passed */
  uint32_t publish_timer;
  /**<   Expires the header value for the PUBLISH request when the device is IMS registered on an LTE network (in seconds).
    */

  /* Optional */
  /*  Publish Extended Timer */
  uint8_t publish_extended_timer_valid;  /**< Must be set to true if publish_extended_timer is being passed */
  uint32_t publish_extended_timer;
  /**<   Expires the header value for the PUBLISH request when the device is on an eHRPD network (in seconds).
    */

  /* Optional */
  /*  Minimum Publish Interval */
  uint8_t publish_src_throttle_timer_valid;  /**< Must be set to true if publish_src_throttle_timer is being passed */
  uint32_t publish_src_throttle_timer;
  /**<   Timer in seconds that determines the minimuml interval between each PUBLISH request;
         the device must wait until this timer expires before it sends the next PUBLISH request.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t publish_error_recovery_timer_valid;  /**< Must be set to true if publish_error_recovery_timer is being passed */
  uint32_t publish_error_recovery_timer;
  /**<   Publish error recovery timer in seconds.
    */

  /* Optional */
  /*  Publish Error Recovery Timer */
  uint8_t list_subscription_expiry_valid;  /**< Must be set to true if list_subscription_expiry is being passed */
  uint32_t list_subscription_expiry;
  /**<   Expires the header value for the list SUBSCRIBE request when performing capability polling for more than one contact number (in seconds).
    */

  /* Optional */
  /*  Cache Capability Expiration */
  uint8_t capabilites_cache_expiry_valid;  /**< Must be set to true if capabilites_cache_expiry is being passed */
  uint32_t capabilites_cache_expiry;
  /**<   How long (in seconds) cached capability information of a particular contact is still valid before it is updated by Capability Polling.
    */

  /* Optional */
  /*  Cache Availability Expiration */
  uint8_t availability_cache_expiry_valid;  /**< Must be set to true if availability_cache_expiry is being passed */
  uint32_t availability_cache_expiry;
  /**<   How long (in seconds) cached availability information of a particular contact is considered still valid before a new Availability Fetch needs to be sent.
    */

  /* Optional */
  /*  Capability Poll Interval */
  uint8_t capability_poll_interval_valid;  /**< Must be set to true if capability_poll_interval is being passed */
  uint32_t capability_poll_interval;
  /**<   Time interval in seconds between each Capability Polling. */

  /* Optional */
  /*  Capability Poll List Subscription Expiry Timer */
  uint8_t max_enties_in_list_subscribe_valid;  /**< Must be set to true if max_enties_in_list_subscribe is being passed */
  uint32_t max_enties_in_list_subscribe;
  /**<   Maximum number of contacts that can be included in a list subscribe request.
    */

  /* Optional */
  /*  Discovery Capability Enabled */
  uint8_t capability_discovery_enabled_valid;  /**< Must be set to true if capability_discovery_enabled is being passed */
  uint8_t capability_discovery_enabled;
  /**<   Flag indicating whether discovery capability is enabled. \n
         Values: \n
         -TRUE -- Presence publishes/subscribes and processes any notification 
                  received \n
         -FALSE -- Presence does not publish/subscribe and 
                  ignores any notification received */

  /* Optional */
  /*  GZIP Enabled */
  uint8_t gzip_enabled_valid;  /**< Must be set to true if gzip_enabled is being passed */
  uint8_t gzip_enabled;
  /**<   Flag indicating whether GZIP compression is enabled. 
         Values: \n
         -TRUE -- Enabled \n
         -FALSE -- Disabled
    */

  /* Optional */
  /*  User Agent */
  uint8_t user_agent_valid;  /**< Must be set to true if user_agent is being passed */
  char user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   User agent. */
}ims_settings_presence_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS RCS standalone messaging configuration parameters
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  RCS Client Config String */
  uint8_t auto_config_user_agent_valid;  /**< Must be set to true if auto_config_user_agent is being passed */
  char auto_config_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   RCS client configuration string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS XDM Client Config String */
  uint8_t xdm_client_user_agent_valid;  /**< Must be set to true if xdm_client_user_agent is being passed */
  char xdm_client_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   RCS XDM client configuration string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Client Vendor String */
  uint8_t client_vendor_valid;  /**< Must be set to true if client_vendor is being passed */
  char client_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client vendor string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Client Version String */
  uint8_t client_version_valid;  /**< Must be set to true if client_version is being passed */
  char client_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client version string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Terminal Vendor */
  uint8_t terminal_vendor_valid;  /**< Must be set to true if terminal_vendor is being passed */
  char terminal_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal vendor string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Terminal Model */
  uint8_t terminal_model_valid;  /**< Must be set to true if terminal_model is being passed */
  char terminal_model[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal model string.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Terminal Software Version */
  uint8_t terminal_sw_version_valid;  /**< Must be set to true if terminal_sw_version is being passed */
  char terminal_sw_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal software version.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  RCS Software Version Supported by Client */
  uint8_t rcs_version_valid;  /**< Must be set to true if rcs_version is being passed */
  char rcs_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   RCS Software Version Supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Version of Provisioning specification supported by Client */
  uint8_t provisioning_version_valid;  /**< Must be set to true if provisioning_version is being passed */
  char provisioning_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Version of Provisioning specification supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Default SMS App */
  uint8_t default_sms_app_valid;  /**< Must be set to true if default_sms_app is being passed */
  uint8_t default_sms_app;
  /**<   Default SMS App
	     0 - HLOS does not allow user to select SMS application or the client cannot identify the selected SMS application
		 1 - RCS messaging client is selected as the default SMS application
		 2 - RCS messaging client is not selected as the default SMS application
	    If not set, default value is 0. */

  /* Optional */
  /*  Default Visual VoiceMail App. */
  uint8_t default_vvm_app_valid;  /**< Must be set to true if default_vvm_app is being passed */
  uint8_t default_vvm_app;
  /**<   Default Visual VM App
	     0 - indicates that the RCS messaging client is not selected as the default VM application
		 1 - indicates that the RCS messaging client is selected as the default VM application
	    If not set, default value is 0. */
}ims_settings_set_rcs_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS RCS standalone messaging configuration parameters
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_rcs_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the RCS standalone messaging configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_rcs_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the RCS standalone messaging configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
       */

  /* Optional */
  /*  RCS Client Config String */
  uint8_t auto_config_user_agent_valid;  /**< Must be set to true if auto_config_user_agent is being passed */
  char auto_config_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   Client configuration string.
    */

  /* Optional */
  /*  RCS XDM Client Config String */
  uint8_t xdm_client_user_agent_valid;  /**< Must be set to true if xdm_client_user_agent is being passed */
  char xdm_client_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   XDM client configuration string.
    */

  /* Optional */
  /*  RCS Client Vendor String */
  uint8_t client_vendor_valid;  /**< Must be set to true if client_vendor is being passed */
  char client_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client vendor string.
    */

  /* Optional */
  /*  RCS Client Version String */
  uint8_t client_version_valid;  /**< Must be set to true if client_version is being passed */
  char client_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client version string.
    */

  /* Optional */
  /*  RCS Terminal Vendor */
  uint8_t terminal_vendor_valid;  /**< Must be set to true if terminal_vendor is being passed */
  char terminal_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal vendor string.
    */

  /* Optional */
  /*  RCS Terminal Model */
  uint8_t terminal_model_valid;  /**< Must be set to true if terminal_model is being passed */
  char terminal_model[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal model.
    */

  /* Optional */
  /*  RCS Terminal Software Version */
  uint8_t terminal_sw_version_valid;  /**< Must be set to true if terminal_sw_version is being passed */
  char terminal_sw_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal software version.
    */

  /* Optional */
  /*  RCS Software Version Supported by Client */
  uint8_t rcs_version_valid;  /**< Must be set to true if rcs_version is being passed */
  char rcs_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   RCS Software Version Supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Version of Provisioning specification supported by Client */
  uint8_t provisioning_version_valid;  /**< Must be set to true if provisioning_version is being passed */
  char provisioning_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Version of Provisioning specification supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Default SMS App */
  uint8_t default_sms_app_valid;  /**< Must be set to true if default_sms_app is being passed */
  uint8_t default_sms_app;
  /**<   Default SMS App
	     0 - HLOS does not allow user to select SMS application or the client cannot identify the selected SMS application
		 1 - RCS messaging client is selected as the default SMS application
		 2 - RCS messaging client is not selected as the default SMS application
	    If not set, default value is 0. */

  /* Optional */
  /*  Default Visual VoiceMail App. */
  uint8_t default_vvm_app_valid;  /**< Must be set to true if default_vvm_app is being passed */
  uint8_t default_vvm_app;
  /**<   Default Visual VM App
	     0 - indicates that the RCS messaging client is not selected as the default VM application
		 1 - indicates that the RCS messaging client is selected as the default VM application
	    If not set, default value is 0. */
}ims_settings_get_rcs_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the RCS standalone messaging configuration
             parameters change. */
typedef struct {

  /* Optional */
  /*  RCS Client Config String */
  uint8_t auto_config_user_agent_valid;  /**< Must be set to true if auto_config_user_agent is being passed */
  char auto_config_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   Client configuration string.
    */

  /* Optional */
  /*  RCS XDM Client Config String */
  uint8_t xdm_client_user_agent_valid;  /**< Must be set to true if xdm_client_user_agent is being passed */
  char xdm_client_user_agent[IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01 + 1];
  /**<   XDM client configuration string.
    */

  /* Optional */
  /*  RCS Client Vendor String */
  uint8_t client_vendor_valid;  /**< Must be set to true if client_vendor is being passed */
  char client_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client vendor string.
    */

  /* Optional */
  /*  RCS Client Version String */
  uint8_t client_version_valid;  /**< Must be set to true if client_version is being passed */
  char client_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Client version string.
    */

  /* Optional */
  /*  RCS Terminal Vendor */
  uint8_t terminal_vendor_valid;  /**< Must be set to true if terminal_vendor is being passed */
  char terminal_vendor[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal vendor string.
    */

  /* Optional */
  /*  RCS Terminal Model */
  uint8_t terminal_model_valid;  /**< Must be set to true if terminal_model is being passed */
  char terminal_model[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal model.
    */

  /* Optional */
  /*  RCS Terminal Software Version */
  uint8_t terminal_sw_version_valid;  /**< Must be set to true if terminal_sw_version is being passed */
  char terminal_sw_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Terminal software version.
    */

  /* Optional */
  /*  RCS Software Version Supported by Client */
  uint8_t rcs_version_valid;  /**< Must be set to true if rcs_version is being passed */
  char rcs_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   RCS Software Version Supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Version of Provisioning specification supported by Client */
  uint8_t provisioning_version_valid;  /**< Must be set to true if provisioning_version is being passed */
  char provisioning_version[IMS_SETTINGS_RCS_STRING_LEN_MAX_V01 + 1];
  /**<   Version of Provisioning specification supported by Client.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Default SMS App */
  uint8_t default_sms_app_valid;  /**< Must be set to true if default_sms_app is being passed */
  uint8_t default_sms_app;
  /**<   Default SMS App
	     0 - HLOS does not allow user to select SMS application or the client cannot identify the selected SMS application
		 1 - RCS messaging client is selected as the default SMS application
		 2 - RCS messaging client is not selected as the default SMS application
	    If not set, default value is 0. */

  /* Optional */
  /*  Default Visual VoiceMail App. */
  uint8_t default_vvm_app_valid;  /**< Must be set to true if default_vvm_app is being passed */
  uint8_t default_vvm_app;
  /**<   Default Visual VM App
	     0 - indicates that the RCS messaging client is not selected as the default VM application
		 1 - indicates that the RCS messaging client is selected as the default VM application
	    If not set, default value is 0. */
}ims_settings_rcs_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS registration manager configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Target Delay Timer for IRAT */
  uint8_t reg_irat_hystersis_timer_valid;  /**< Must be set to true if reg_irat_hystersis_timer is being passed */
  uint16_t reg_irat_hystersis_timer;
  /**<   Target delay timer for IRAT in seconds.
         If not set, the default is 0.
    */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   Registration retry base time in seconds.
         If not set, the default is 30.
    */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   Registration retry maximum time in seconds.
         If not set, the default is 7200.
    */

  /* Optional */
  /*  PCSCF Port */
  uint8_t regmgr_primary_pcscf_valid;  /**< Must be set to true if regmgr_primary_pcscf is being passed */
  char regmgr_primary_pcscf[IMS_SETTINGS_RM_STRING_LEN_MAX_V01 + 1];
  /**<   Registration preconfigured server base address.
         If not set, the default is NULL.
    */

  /* Optional */
  /*  Allows Configuration of the PSCSF Port */
  uint8_t regmgr_pcscf_port_valid;  /**< Must be set to true if regmgr_pcscf_port is being passed */
  uint16_t regmgr_pcscf_port;
  /**<   Allows configuration of the PSCSF port.
         If not set, the default is 5060.
    */
}ims_settings_set_reg_mgr_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS registration manager configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_reg_mgr_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the registration manager configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_reg_mgr_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the registration manager configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
       error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Target Delay Timer for IRAT */
  uint8_t reg_irat_hystersis_timer_valid;  /**< Must be set to true if reg_irat_hystersis_timer is being passed */
  uint16_t reg_irat_hystersis_timer;
  /**<   Target delay timer for IRAT in seconds. */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   Registration retry base time in seconds. */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   Registration retry maximum time in seconds. */

  /* Optional */
  /*  PCSCF Port */
  uint8_t regmgr_primary_pcscf_valid;  /**< Must be set to true if regmgr_primary_pcscf is being passed */
  char regmgr_primary_pcscf[IMS_SETTINGS_RM_STRING_LEN_MAX_V01 + 1];
  /**<   Registration of the preconfigured server base address. */

  /* Optional */
  /*  Allows Configuration of the PSCSF Port */
  uint8_t regmgr_pcscf_port_valid;  /**< Must be set to true if regmgr_pcscf_port is being passed */
  uint16_t regmgr_pcscf_port;
  /**<   Allows configuration of the PSCSF port. */
}ims_settings_get_reg_mgr_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the registration manager configuration 
             parameters change. */
typedef struct {

  /* Optional */
  /*  Target Delay Timer for IRAT */
  uint8_t reg_irat_hystersis_timer_valid;  /**< Must be set to true if reg_irat_hystersis_timer is being passed */
  uint16_t reg_irat_hystersis_timer;
  /**<   Target delay timer for IRAT in seconds. */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t reg_retry_base_time_valid;  /**< Must be set to true if reg_retry_base_time is being passed */
  uint16_t reg_retry_base_time;
  /**<   Registration retry base time in seconds. */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t reg_retry_max_time_valid;  /**< Must be set to true if reg_retry_max_time is being passed */
  uint16_t reg_retry_max_time;
  /**<   Registration retry maximum time in seconds. */

  /* Optional */
  /*  PCSCF Port */
  uint8_t regmgr_primary_pcscf_valid;  /**< Must be set to true if regmgr_primary_pcscf is being passed */
  char regmgr_primary_pcscf[IMS_SETTINGS_RM_STRING_LEN_MAX_V01 + 1];
  /**<   Registration preconfigured server base address. */

  /* Optional */
  /*  Allows Configuration of the PSCSF Port */
  uint8_t regmgr_pcscf_port_valid;  /**< Must be set to true if regmgr_pcscf_port is being passed */
  uint16_t regmgr_pcscf_port;
  /**<   Allows configuration of the PSCSF port. */
}ims_settings_reg_mgr_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS XCAP configuration parameters
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Ut Interface IP Address Type for WWan */
  uint8_t ip_addr_type_wwan_valid;  /**< Must be set to true if ip_addr_type_wwan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wwan;
  /**<   IP address type for WWAN.
         If not set, the default value is IMS_SETTINGS_IP_TYPE_UNKNOWN.
    */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t xcap_apn_wwan_valid;  /**< Must be set to true if xcap_apn_wwan is being passed */
  char xcap_apn_wwan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WWAN.
         If not set, use as per operator request.
    */

  /* Optional */
  /*  Ut Interface IP Address Type for WLAN */
  uint8_t ip_addr_type_wlan_valid;  /**< Must be set to true if ip_addr_type_wlan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wlan;
  /**<   IP address type for WLAN.
         If not set, the default value is IMS_SETTINGS_IP_TYPE_UNKNOWN.
    */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t xcap_apn_wlan_valid;  /**< Must be set to true if xcap_apn_wlan is being passed */
  char xcap_apn_wlan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WLAN.
         If not set, use as per operator request.
    */
}ims_settings_set_xcap_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS XCAP configuration parameters
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_xcap_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the XCAP interface configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_xcap_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the XCAP interface configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Settings standard response type. A settings-specific error code is 
         returned when the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  Ut Interface IP Address Type */
  uint8_t ip_addr_type_wwan_valid;  /**< Must be set to true if ip_addr_type_wwan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wwan;
  /**<   IP address type for WWAN. */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t xcap_apn_wwan_valid;  /**< Must be set to true if xcap_apn_wwan is being passed */
  char xcap_apn_wwan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WWAN. */

  /* Optional */
  /*  Ut Interface IP Address Type */
  uint8_t ip_addr_type_wlan_valid;  /**< Must be set to true if ip_addr_type_wlan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wlan;
  /**<   IP address type for WLAN. */

  /* Optional */
  /*  Ut Interface Access Point Name */
  uint8_t xcap_apn_wlan_valid;  /**< Must be set to true if xcap_apn_wlan is being passed */
  char xcap_apn_wlan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WLAN. */
}ims_settings_get_xcap_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the XCAP configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Ut Interface IP Address Type for WWAN */
  uint8_t ip_addr_type_wwan_valid;  /**< Must be set to true if ip_addr_type_wwan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wwan;
  /**<   IP address type for WWAN. */

  /* Optional */
  /*  Ut Interface Access Point Name for WWAN */
  uint8_t xcap_apn_wwan_valid;  /**< Must be set to true if xcap_apn_wwan is being passed */
  char xcap_apn_wwan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WWAN. */

  /* Optional */
  /*  Ut Interface IP Address Type for WLAN */
  uint8_t ip_addr_type_wlan_valid;  /**< Must be set to true if ip_addr_type_wlan is being passed */
  ims_settings_ip_addr_type_enum_v01 ip_addr_type_wlan;
  /**<   IP address type for WLAN. */

  /* Optional */
  /*  Ut Interface Access Point Name for WLAN */
  uint8_t xcap_apn_wlan_valid;  /**< Must be set to true if xcap_apn_wlan is being passed */
  char xcap_apn_wlan[IMS_SETTINGS_XCAP_APN_NAME_LEN_V01 + 1];
  /**<   XCAP interface APN string for WLAN. */
}ims_settings_xcap_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS SMS configuration parameters for the 
             requesting control point. */
typedef struct {

  /* Optional */
  /*  SMS PSI */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_IMS_SMS_PSI_LEN_V01 + 1];
  /**<   SMS PSI string value. 
         If not set, the default is NULL.
    */

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 If not set, the default is as per IMS_SETTINGS_SMS_FORMAT_3GPP.
 */
}ims_settings_set_sms_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS SMS configuration parameters for the 
             requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type  */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response 
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_sms_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the SMS configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_sms_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the SMS configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response.*/

  /* Optional */
  /*  SMS PSI */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_IMS_SMS_PSI_LEN_V01 + 1];
  /**<   SMS PSI string value. */

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 */
}ims_settings_get_sms_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the SMS configuration parameters change. */
typedef struct {

  /* Optional */
  /*  SMS PSI */
  uint8_t sms_psi_valid;  /**< Must be set to true if sms_psi is being passed */
  char sms_psi[IMS_SETTINGS_IMS_SMS_PSI_LEN_V01 + 1];
  /**<   SMS PSI string value. */

  /* Optional */
  /*  SMS Format */
  uint8_t sms_format_valid;  /**< Must be set to true if sms_format is being passed */
  ims_settings_sms_format_enum_v01 sms_format;
  /**<   Values: \n
      - IMS_SETTINGS_SMS_FORMAT_3GPP2 (0) --  3GPP2 SMS format     
      - IMS_SETTINGS_SMS_FORMAT_3GPP (1) --  3GPP SMS format 
 */
}ims_settings_sms_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the voice dynamic configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Audio Feature Tag */
  uint8_t audio_feature_tag_valid;  /**< Must be set to true if audio_feature_tag is being passed */
  char audio_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Default string used as "urn:urn-7:3gpp-service.ims.icsi.mmtel"
         if not present in the request as per the 3GPP standard.
    */

  /* Optional */
  /*  Session Expires Timer */
  uint8_t session_expires_valid;  /**< Must be set to true if session_expires is being passed */
  uint16_t session_expires;
  /**<   Minimum allowed value for the session timer, in seconds.
         The default minimum value is the min_session_expiry value.
    */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value for the session timer, in seconds.
         The default, if not set, is as per RFC4028.
    */

  /* Optional */
  /*  Conference URI */
  uint8_t conference_uri_valid;  /**< Must be set to true if conference_uri is being passed */
  char conference_uri[IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01 + 1];
  /**<   If not set, IMS internally generates a conference URI based on MNC/MCC in theUICC card as per 3GPP specs.
    */

  /* Optional */
  /*  Enable Silent Redial  */
  uint8_t silent_redial_enable_valid;  /**< Must be set to true if silent_redial_enable is being passed */
  uint8_t silent_redial_enable;
  /**<   Flag to enable/disable the silent redial feature.
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable (default)
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t caller_id_mode_valid;  /**< Must be set to true if caller_id_mode is being passed */
  ims_settings_call_id_mode_option_enum_v01 caller_id_mode;
  /**<   Caller ID mode type.
         If not set, the default is IMS_SETTINGS_CALL_ID_MODE_NONE.
    */

  /* Optional */
  /*  Ringing Timer  */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint32_t ringing_timer;
  /**<   Duration of the ringing timer, in milliseconds.
         If not set, the default is 30000.
    */

  /* Optional */
  /*  Ringback Timer Duration  */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint32_t ringback_timer;
  /**<   Duration of the ringback timer, in milliseconds.
         If not set, the default is 35000.
    */

  /* Optional */
  /*  RTP LinkAliveness Timer  */
  uint8_t rtp_link_aliveness_timer_valid;  /**< Must be set to true if rtp_link_aliveness_timer is being passed */
  uint32_t rtp_link_aliveness_timer;
  /**<   Duration of the link aliveness timer, in seconds.
         If not set, the default is 10.
    */

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t vice_enabled_valid;  /**< Must be set to true if vice_enabled is being passed */
  uint8_t vice_enabled;
  /**<   Indicates whether VICE is enabled.
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled (default)
    */
}ims_settings_set_voice_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the voice dynamic configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_voice_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the voice dynamic configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_voice_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the voice dynamic configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Audio Feature Tag */
  uint8_t audio_feature_tag_valid;  /**< Must be set to true if audio_feature_tag is being passed */
  char audio_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Audio feature tag.
    */

  /* Optional */
  /*  Session Expires Timer */
  uint8_t session_expires_valid;  /**< Must be set to true if session_expires is being passed */
  uint16_t session_expires;
  /**<   Session expiration timer, in seconds. */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value for the session timer, in seconds. */

  /* Optional */
  /*  Conference URI */
  uint8_t conference_uri_valid;  /**< Must be set to true if conference_uri is being passed */
  char conference_uri[IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01 + 1];
  /**<   Conference URI.
    */

  /* Optional */
  /*  Enable Silent Redial  */
  uint8_t silent_redial_enable_valid;  /**< Must be set to true if silent_redial_enable is being passed */
  uint8_t silent_redial_enable;
  /**<   Flag to enable/disable the silent redial feature.
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t caller_id_mode_valid;  /**< Must be set to true if caller_id_mode is being passed */
  ims_settings_call_id_mode_option_enum_v01 caller_id_mode;
  /**<   Caller ID mode type.
    */

  /* Optional */
  /*  Ringing Timer  */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint32_t ringing_timer;
  /**<   Duration of the ringing timer, in seconds.
    */

  /* Optional */
  /*  Ringback Timer Duration  */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint32_t ringback_timer;
  /**<   Duration of the ringback timer, in seconds.
    */

  /* Optional */
  /*  RTP Link Aliveness Timer  */
  uint8_t rtp_link_aliveness_timer_valid;  /**< Must be set to true if rtp_link_aliveness_timer is being passed */
  uint32_t rtp_link_aliveness_timer;
  /**<   Duration of the link aliveness timer, in seconds.
    */

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t vice_enabled_valid;  /**< Must be set to true if vice_enabled is being passed */
  uint8_t vice_enabled;
  /**<   Indicates whether VICE is enabled.
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled 
    */
}ims_settings_get_voice_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the voice dynamic configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Audio Feature Tag */
  uint8_t audio_feature_tag_valid;  /**< Must be set to true if audio_feature_tag is being passed */
  char audio_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Audio feature tag.
    */

  /* Optional */
  /*  Session Expires Timer */
  uint8_t session_expires_valid;  /**< Must be set to true if session_expires is being passed */
  uint16_t session_expires;
  /**<   Session expiration timer, in seconds. */

  /* Optional */
  /*  Minimum Session Timer */
  uint8_t min_session_expiry_valid;  /**< Must be set to true if min_session_expiry is being passed */
  uint16_t min_session_expiry;
  /**<   Minimum allowed value for the session timer, in seconds. */

  /* Optional */
  /*  Conference URI */
  uint8_t conference_uri_valid;  /**< Must be set to true if conference_uri is being passed */
  char conference_uri[IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01 + 1];
  /**<   Conference URI.
    */

  /* Optional */
  /*  Enable Silent Redial  */
  uint8_t silent_redial_enable_valid;  /**< Must be set to true if silent_redial_enable is being passed */
  uint8_t silent_redial_enable;
  /**<   Flag to enable/disable the silent redial feature.
        Values: \n
        -TRUE -- Enable \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t caller_id_mode_valid;  /**< Must be set to true if caller_id_mode is being passed */
  ims_settings_call_id_mode_option_enum_v01 caller_id_mode;
  /**<   Caller ID mode type.
    */

  /* Optional */
  /*  Ringing Timer  */
  uint8_t ringing_timer_valid;  /**< Must be set to true if ringing_timer is being passed */
  uint32_t ringing_timer;
  /**<   Duration of the ringing timer, in seconds.
    */

  /* Optional */
  /*  Ringback Timer Duration  */
  uint8_t ringback_timer_valid;  /**< Must be set to true if ringback_timer is being passed */
  uint32_t ringback_timer;
  /**<   Duration of the ringback timer, in seconds.
    */

  /* Optional */
  /*  RTP Link Aliveness Timer  */
  uint8_t rtp_link_aliveness_timer_valid;  /**< Must be set to true if rtp_link_aliveness_timer is being passed */
  uint32_t rtp_link_aliveness_timer;
  /**<   Duration of the link aliveness timer, in seconds.
    */

  /* Optional */
  /*  IMS Application VICE Enabled */
  uint8_t vice_enabled_valid;  /**< Must be set to true if vice_enabled is being passed */
  uint8_t vice_enabled;
  /**<   Indicates whether VICE is enabled.
        Values: \n
        -TRUE -- Enabled \n
        -FALSE -- Disabled 
    */
}ims_settings_voice_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the codec related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization. \n
        If not set, the default is 98, as per RFC3551.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
        If not set, the default is 97.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
        If not set, the default is 101.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
        If not set, the default is 100.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband. \n
        Valid range of values: 96 to 127.
        If not set, the default is 99.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband. \n
        Valid range of values: 96 to 127.
        If not set, the default is 102.
    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_valid;  /**< Must be set to true if amr_mode_set is being passed */
  ims_settings_amr_nb_codec_mode_option_enum_v01 amr_mode_set;
  /**<   AMR encoding mode.
         If not set, the default is all modes enabled.
         Use TLV amr_mode_set_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_valid;  /**< Must be set to true if amr_wb_mode_set is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_mode_set;
  /**<   AMR wideband encoding mode.
         If not set, default used as all mode enable
         Use TLV ims_settings_amr_wb_codec_mode_type_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Default Encoding Mode */
  uint8_t amr_wb_default_mode_valid;  /**< Must be set to true if amr_wb_default_mode is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_default_mode;
  /**<   AMR wideband default encoding mode.
         If not set, the default is IMS_SETTINGS_AMR_WB_CODEC_MODE_16.
    */

  /* Optional */
  /*  AMR WB Enable */
  uint8_t amr_wb_enabled_valid;  /**< Must be set to true if amr_wb_enabled is being passed */
  uint8_t amr_wb_enabled;
  /**<   AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled (default) \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR Enable */
  uint8_t scr_amr_enabled_valid;  /**< Must be set to true if scr_amr_enabled is being passed */
  uint8_t scr_amr_enabled;
  /**<   SCR AMR enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled (default) \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR WB Enable */
  uint8_t scr_amr_wb_enabled_valid;  /**< Must be set to true if scr_amr_wb_enabled is being passed */
  uint8_t scr_amr_wb_enabled;
  /**<   SCR AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled (default) \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   
        EVS primary value, payload size. \n
        Valid range of values: -1 to 32767
        If not set, default value -1 will be used.
    */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   
        EVS SDP Codec bit rate. \n
        If not set, default value IMS_SETTINGS_EVS_SDP_CODEC_MODE_UNK.
    */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   
        QVP RTP SDP bandwidth. \n
        If not set, default value IMS_SETTINGS_QVP_RTP_SDP_BW_UNKNOWN.    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_mask_valid;  /**< Must be set to true if amr_mode_set_mask is being passed */
  ims_settings_amr_nb_codec_mode_type_mask_v01 amr_mode_set_mask;
  /**<   
        AMR encoding mode.
        Narrowband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
        If not set, the default value IMS_SETTINGS_AMR_NB_CODEC_MODE_0_BIT_MASK.
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_mask_valid;  /**< Must be set to true if amr_wb_mode_set_mask is being passed */
  ims_settings_amr_wb_codec_mode_type_mask_v01 amr_wb_mode_set_mask;
  /**<   
        AMR WB encoding mode.
        Wideband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
        If not set, default value IMS_SETTINGS_AMR_WB_CODEC_MODE_8_BIT_MASK.
    */
}ims_settings_set_codec_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the codec related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_codec_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the codec related configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_codec_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the codec related configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_valid;  /**< Must be set to true if amr_mode_set is being passed */
  ims_settings_amr_nb_codec_mode_option_enum_v01 amr_mode_set;
  /**<   
        AMR encoding mode.
        Use TLV amr_mode_set_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_valid;  /**< Must be set to true if amr_wb_mode_set is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_mode_set;
  /**<   
        AMR wideband encoding mode.
        Use TLV ims_settings_amr_wb_codec_mode_type_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Default Encoding Mode */
  uint8_t amr_wb_default_mode_valid;  /**< Must be set to true if amr_wb_default_mode is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_default_mode;
  /**<   
        AMR wideband default encoding mode.
    */

  /* Optional */
  /*  AMR WB Enable */
  uint8_t amr_wb_enabled_valid;  /**< Must be set to true if amr_wb_enabled is being passed */
  uint8_t amr_wb_enabled;
  /**<   AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR Enable */
  uint8_t scr_amr_enabled_valid;  /**< Must be set to true if scr_amr_enabled is being passed */
  uint8_t scr_amr_enabled;
  /**<   SCR AMR enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR WB Enable */
  uint8_t scr_amr_wb_enabled_valid;  /**< Must be set to true if scr_amr_wb_enabled is being passed */
  uint8_t scr_amr_wb_enabled;
  /**<   SCR AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   
        EVS primary value.
    */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   
        EVS SDP Codec bit rate.
    */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   
        QVP RTP SDP bandwidth.
    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_mask_valid;  /**< Must be set to true if amr_mode_set_mask is being passed */
  ims_settings_amr_nb_codec_mode_type_mask_v01 amr_mode_set_mask;
  /**<   
        AMR encoding mode.
        Narrowband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_mask_valid;  /**< Must be set to true if amr_wb_mode_set_mask is being passed */
  ims_settings_amr_wb_codec_mode_type_mask_v01 amr_wb_mode_set_mask;
  /**<   
        AMR WB encoding mode.
        Wideband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
    */
}ims_settings_get_codec_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when codec related parameters change. */
typedef struct {

  /* Optional */
  /*  AMR-WB Octet Aligned Payload Type */
  uint8_t amr_wb_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_wb_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_wb_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR-WB Bandwidth Efficient Payload Type */
  uint8_t amr_wb_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_wb_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_wb_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR-WB in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Octet Aligned Payload Type */
  uint8_t amr_octet_aligned_dynamic_pt_valid;  /**< Must be set to true if amr_octet_aligned_dynamic_pt is being passed */
  uint16_t amr_octet_aligned_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in octet-aligned packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Payload Type */
  uint8_t amr_bandwidth_efficient_dynamic_pt_valid;  /**< Must be set to true if amr_bandwidth_efficient_dynamic_pt is being passed */
  uint16_t amr_bandwidth_efficient_dynamic_pt;
  /**<   
        Dynamic payload type for AMR in bandwidth-efficient packetization. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Wideband Payload Type */
  uint8_t dtmf_wb_dynamic_pt_valid;  /**< Must be set to true if dtmf_wb_dynamic_pt is being passed */
  uint16_t dtmf_wb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at wideband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  DTMF Narrowband Payload Type */
  uint8_t dtmf_nb_dynamic_pt_valid;  /**< Must be set to true if dtmf_nb_dynamic_pt is being passed */
  uint16_t dtmf_nb_dynamic_pt;
  /**<   
        Dynamic payload type for DTMF at narrowband. \n
        Valid range of values: 96 to 127.
    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_valid;  /**< Must be set to true if amr_mode_set is being passed */
  ims_settings_amr_nb_codec_mode_option_enum_v01 amr_mode_set;
  /**<   
        AMR encoding mode.
        Use TLV amr_mode_set_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_valid;  /**< Must be set to true if amr_wb_mode_set is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_mode_set;
  /**<   
        AMR WB encoding mode.
        Use TLV ims_settings_amr_wb_codec_mode_type_mask instead of amr_mode_set
    */

  /* Optional */
  /*  AMR Wideband Default Encoding Mode */
  uint8_t amr_wb_default_mode_valid;  /**< Must be set to true if amr_wb_default_mode is being passed */
  ims_settings_amr_wb_codec_mode_option_enum_v01 amr_wb_default_mode;
  /**<   
        AMR wideband default encoding mode.
    */

  /* Optional */
  /*  AMR WB Enable */
  uint8_t amr_wb_enabled_valid;  /**< Must be set to true if amr_wb_enabled is being passed */
  uint8_t amr_wb_enabled;
  /**<   AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR Enable */
  uint8_t scr_amr_enabled_valid;  /**< Must be set to true if scr_amr_enabled is being passed */
  uint8_t scr_amr_enabled;
  /**<   SCR AMR enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  SCR AMR WB Enable */
  uint8_t scr_amr_wb_enabled_valid;  /**< Must be set to true if scr_amr_wb_enabled is being passed */
  uint8_t scr_amr_wb_enabled;
  /**<   SCR AMR WB enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   
        EVS primary value.
    */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   
        EVS SDP Codec bit rate.
    */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   
        QVP RTP SDP bandwidth.
    */

  /* Optional */
  /*  AMR Encoding Mode */
  uint8_t amr_mode_set_mask_valid;  /**< Must be set to true if amr_mode_set_mask is being passed */
  ims_settings_amr_nb_codec_mode_type_mask_v01 amr_mode_set_mask;
  /**<   
        AMR encoding mode.
        Narrowband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
    */

  /* Optional */
  /*  AMR Wideband Encoding Mode */
  uint8_t amr_wb_mode_set_mask_valid;  /**< Must be set to true if amr_wb_mode_set_mask is being passed */
  ims_settings_amr_wb_codec_mode_type_mask_v01 amr_wb_mode_set_mask;
  /**<   
        AMR WB encoding mode.
        Wideband Codec modes negotiated in the SDP for this session. This is a bitmask 
        variable that indicates modes negotiated for this session.
    */
}ims_settings_codec_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the video dynamic configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Video Feature Tag */
  uint8_t video_feature_tag_valid;  /**< Must be set to true if video_feature_tag is being passed */
  char video_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Enable video as per an operator request.
         If not set, the default is NULL (video is disabled).
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a videotelephony (VT) call. If this 
 TLV is not present in the request, a value of 
 IMS_SETTINGS_VT_QUALITY_LEVEL_0 (that is, high quality) is used. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */
}ims_settings_set_video_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the video dynamic configuration 
             parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_video_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the video dynamic configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_video_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the video dynamic configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Video Feature Tag */
  uint8_t video_feature_tag_valid;  /**< Must be set to true if video_feature_tag is being passed */
  char video_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Video feature tag.
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a videotelephony (VT) call. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */
}ims_settings_get_video_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the video dynamic configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Video Feature Tag */
  uint8_t video_feature_tag_valid;  /**< Must be set to true if video_feature_tag is being passed */
  char video_feature_tag[IMS_SETTINGS_FEATURE_TAG_LEN_V01 + 1];
  /**<   Video feature tag.
    */

  /* Optional */
  /*  VT Quality Selector */
  uint8_t vt_quality_selector_valid;  /**< Must be set to true if vt_quality_selector is being passed */
  ims_settings_qipcall_vt_quality_enum_v01 vt_quality_selector;
  /**<   Values for video quality in a videotelephony (VT) call. Values: \n
      - IMS_SETTINGS_VT_QUALITY_LEVEL_0 (0x00) --  VT quality selector level 0 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_1 (0x01) --  VT quality selector level 1 
      - IMS_SETTINGS_VT_QUALITY_LEVEL_2 (0x02) --  VT quality selector level 2 
 */
}ims_settings_video_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the emergency dynamic configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Emergency Call Setup Timer */
  uint8_t e911_call_setup_timer_valid;  /**< Must be set to true if e911_call_setup_timer is being passed */
  uint32_t e911_call_setup_timer;
  /**<   
        Emergency call setup timer, in milliseconds.
        If not set, the default is 0.
    */

  /* Optional */
  /*  Emergency WLAN Call Setup Timer */
  uint8_t e911_wlan_call_setup_timer_valid;  /**< Must be set to true if e911_wlan_call_setup_timer is being passed */
  uint32_t e911_wlan_call_setup_timer;
  /**<   
        Emergency call setup timer over WLAN, in milliseconds.
        If not set, the default is 0.
    */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled (default)
    */

  /* Optional */
  /*  Emergency WWAN Call Wait Timer */
  uint8_t e911_wwan_call_connect_timer_valid;  /**< Must be set to true if e911_wwan_call_connect_timer is being passed */
  uint32_t e911_wwan_call_connect_timer;
  /**<   
        Emergency call wait timer over WWAN, in milliseconds.
        If not set, the default is 0.
    */

  /* Optional */
  /*  Emergency WLAN Call Wait Timer */
  uint8_t e911_wlan_call_connect_timer_valid;  /**< Must be set to true if e911_wlan_call_connect_timer is being passed */
  uint32_t e911_wlan_call_connect_timer;
  /**<   
        Emergency call wait timer over WLAN, in milliseconds.
        If not set, the default is 0.
    */
}ims_settings_set_emer_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the emergency dynamic configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_emer_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the emergency dynamic configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_emer_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the emergency dynamic configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Emergency Call Setup Timer */
  uint8_t e911_call_setup_timer_valid;  /**< Must be set to true if e911_call_setup_timer is being passed */
  uint32_t e911_call_setup_timer;
  /**<   
        Emergency call setup timer, in milliseconds.
    */

  /* Optional */
  /*  Emergency WLAN Call Setup Timer */
  uint8_t e911_wlan_call_setup_timer_valid;  /**< Must be set to true if e911_wlan_call_setup_timer is being passed */
  uint32_t e911_wlan_call_setup_timer;
  /**<   
        Emergency call setup timer over WLAN, in milliseconds.
    */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  Emergency WWAN Call Wait Timer */
  uint8_t e911_wwan_call_connect_timer_valid;  /**< Must be set to true if e911_wwan_call_connect_timer is being passed */
  uint32_t e911_wwan_call_connect_timer;
  /**<   
        Emergency call wait timer over WWAN, in milliseconds.
        If not set, the default is 0.
    */

  /* Optional */
  /*  Emergency WLAN Call Wait Timer */
  uint8_t e911_wlan_call_connect_timer_valid;  /**< Must be set to true if e911_wlan_call_connect_timer is being passed */
  uint32_t e911_wlan_call_connect_timer;
  /**<   
        Emergency call wait timer over WLAN, in milliseconds.
        If not set, the default is 0.
         */
}ims_settings_get_emer_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the emergency dynamic configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Emergency Call Setup Timer */
  uint8_t e911_call_setup_timer_valid;  /**< Must be set to true if e911_call_setup_timer is being passed */
  uint32_t e911_call_setup_timer;
  /**<   
        Emergency call setup timer, in milliseconds.
    */

  /* Optional */
  /*  Emergency WLAN Call Setup Timer */
  uint8_t e911_wlan_call_setup_timer_valid;  /**< Must be set to true if e911_wlan_call_setup_timer is being passed */
  uint32_t e911_wlan_call_setup_timer;
  /**<   
        Emergency call setup timer over WLAN, in milliseconds.
    */

  /* Optional */
  /*  Emergency Silent Redial */
  uint8_t e911_silent_redial_enabled_valid;  /**< Must be set to true if e911_silent_redial_enabled is being passed */
  uint8_t e911_silent_redial_enabled;
  /**<   Emergency silent redial enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  Emergency WWAN Call Wait Timer */
  uint8_t e911_wwan_call_connect_timer_valid;  /**< Must be set to true if e911_wwan_call_connect_timer is being passed */
  uint32_t e911_wwan_call_connect_timer;
  /**<   
        Emergency call wait timer over WWAN, in milliseconds.
        If not set, the default is 0.
    */

  /* Optional */
  /*  Emergency WLAN Call Wait Timer */
  uint8_t e911_wlan_call_connect_timer_valid;  /**< Must be set to true if e911_wlan_call_connect_timer is being passed */
  uint32_t e911_wlan_call_connect_timer;
  /**<   
        Emergency call wait timer over WLAN, in milliseconds.
        If not set, the default is 0.
         */
}ims_settings_emer_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the test dynamic related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Emergency Test Mode */
  uint8_t e911_testmode_enabled_valid;  /**< Must be set to true if e911_testmode_enabled is being passed */
  uint8_t e911_testmode_enabled;
  /**<   Emergency test mode (enabled/disabled). Values: \n
        - TRUE -- Enabled (default)  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  Conformance Test Status */
  uint8_t conformance_test_enabled_valid;  /**< Must be set to true if conformance_test_enabled is being passed */
  uint8_t conformance_test_enabled;
  /**<   Conformance test status (enabled/disabled). Values: \n
        -TRUE -- Enabled  \n
        -FALSE -- Disabled (default)
    */
}ims_settings_set_test_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the test dynamic related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_test_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the test dynamic related configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_test_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the test dynamic related configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Emergency Test Mode */
  uint8_t e911_testmode_enabled_valid;  /**< Must be set to true if e911_testmode_enabled is being passed */
  uint8_t e911_testmode_enabled;
  /**<   Emergency test mode (enabled/disabled). Values: \n
        -TRUE -- Enable  \n
        -FALSE -- Disable 
    */

  /* Optional */
  /*  Conformance Test Status */
  uint8_t conformance_test_enabled_valid;  /**< Must be set to true if conformance_test_enabled is being passed */
  uint8_t conformance_test_enabled;
  /**<   Conformance test status (enabled/disabled). Values: \n
        -TRUE -- Enabled  \n
        -FALSE -- Disabled 
    */
}ims_settings_get_test_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the test dynamic related configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Emergency Test Mode */
  uint8_t e911_testmode_enabled_valid;  /**< Must be set to true if e911_testmode_enabled is being passed */
  uint8_t e911_testmode_enabled;
  /**<   Emergency test mode. Values: \n
        -TRUE -- Enabled  \n
        -FALSE -- Disabled 
    */

  /* Optional */
  /*  Conformance Test Status */
  uint8_t conformance_test_enabled_valid;  /**< Must be set to true if conformance_test_enabled is being passed */
  uint8_t conformance_test_enabled;
  /**<   Conformance test status. Values: \n
        -TRUE -- Enabled  \n
        -FALSE -- Disabled 
    */
}ims_settings_test_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the RTP dynamic related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_stop_port_valid;  /**< Must be set to true if speech_stop_port is being passed */
  uint16_t speech_stop_port;
  /**<   
         Largest RTP port number for a speech codec.
         The default maximum value is the speech_start_port value.
    */

  /* Optional */
  /*  Smallest RTP Port Number for Video */
  uint8_t video_start_port_valid;  /**< Must be set to true if video_start_port is being passed */
  uint16_t video_start_port;
  /**<   
         Smallest RTP port number for a video codec.
         If not set, the default minimum value is 60010.
    */

  /* Optional */
  /*  Largest RTP Port Number for Video */
  uint8_t video_end_port_valid;  /**< Must be set to true if video_end_port is being passed */
  uint16_t video_end_port;
  /**<   
         Largest RTP port number for a video codec.
         If not set, the default maximum value is 60020.
    */
}ims_settings_set_rtp_dynamic_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the RTP dynamic related configuration parameters 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are 
                             described in the error codes section of each 
                             message definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_rtp_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the RTP dynamic related configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_rtp_dynamic_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the RTP dynamic related configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for a speech codec.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_stop_port_valid;  /**< Must be set to true if speech_stop_port is being passed */
  uint16_t speech_stop_port;
  /**<   
         Largest RTP port number for a speech codec.
    */

  /* Optional */
  /*  Smallest RTP Port Number for Video */
  uint8_t video_start_port_valid;  /**< Must be set to true if video_start_port is being passed */
  uint16_t video_start_port;
  /**<   
         Smallest RTP port number for a video codec.
    */

  /* Optional */
  /*  Largest RTP Port Number for Video */
  uint8_t video_end_port_valid;  /**< Must be set to true if video_end_port is being passed */
  uint16_t video_end_port;
  /**<   
         Largest RTP port number for a video codec.
    */
}ims_settings_get_rtp_dynamic_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the RTP dynamic related configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Smallest RTP Port Number */
  uint8_t speech_start_port_valid;  /**< Must be set to true if speech_start_port is being passed */
  uint16_t speech_start_port;
  /**<   
         Smallest RTP port number for a speech codec.
    */

  /* Optional */
  /*  Largest RTP Port Number */
  uint8_t speech_stop_port_valid;  /**< Must be set to true if speech_stop_port is being passed */
  uint16_t speech_stop_port;
  /**<   
         Largest RTP port number for a speech codec.
    */

  /* Optional */
  /*  Smallest RTP Port Number for Video */
  uint8_t video_start_port_valid;  /**< Must be set to true if video_start_port is being passed */
  uint16_t video_start_port;
  /**<   
         Smallest RTP port number for a video codec.
    */

  /* Optional */
  /*  Largest RTP Port Number for Video */
  uint8_t video_end_port_valid;  /**< Must be set to true if video_end_port is being passed */
  uint16_t video_end_port;
  /**<   
         Largest RTP port number for a video codec.
    */
}ims_settings_rtp_dynamic_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  IMS User Agent */
  char ims_user_agent[IMS_SETTINGS_IMS_USER_AGENT_LEN_V01 + 1];
  /**<   User agent string that is to be sent with a SIP message.
    */
}ims_settings_set_ims_new_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_ims_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_ims_new_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  IMS User Agent */
  uint8_t ims_user_agent_valid;  /**< Must be set to true if ims_user_agent is being passed */
  char ims_user_agent[IMS_SETTINGS_IMS_USER_AGENT_LEN_V01 + 1];
  /**<   User agent string that is to be sent with a SIP message.
    */
}ims_settings_get_ims_new_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS configuration parameters change. */
typedef struct {

  /* Mandatory */
  /*  IMS User Agent */
  char ims_user_agent[IMS_SETTINGS_IMS_USER_AGENT_LEN_V01 + 1];
  /**<   User agent string that is to be sent with a SIP message.
    */
}ims_settings_ims_new_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS common parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Acceptable Radio Access Technology */
  uint8_t allowed_rat_mask_valid;  /**< Must be set to true if allowed_rat_mask is being passed */
  ims_settings_rat_type_mask_v01 allowed_rat_mask;
  /**<   Mask with possible RATs allowed at the IMS level to proceed further.
         If not set, use as per operator request.
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_IMS_STRING_LEN_MAX_V01 + 1];
  /**<   Configure the domain name for a phone.
         If a domain name is not configured here, it will be derived from the
         SIM card. Each operator has its own value.
    */
}ims_settings_set_ims_common_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS common parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_ims_common_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS common configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_ims_common_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS common configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Acceptable Radio Access Technology */
  uint8_t allowed_rat_mask_valid;  /**< Must be set to true if allowed_rat_mask is being passed */
  ims_settings_rat_type_mask_v01 allowed_rat_mask;
  /**<   Mask of possible RATs allowed at the IMS level to proceed further. */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_IMS_STRING_LEN_MAX_V01 + 1];
  /**<   Configure the domain name a phone. */
}ims_settings_get_ims_common_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS Common configuration parameters change. */
typedef struct {

  /* Optional */
  /*  Acceptable Radio Access Technology */
  uint8_t allowed_rat_mask_valid;  /**< Must be set to true if allowed_rat_mask is being passed */
  ims_settings_rat_type_mask_v01 allowed_rat_mask;
  /**<   Mask with possible RATs allowed at the IMS level to proceed further. */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t phone_context_uri_valid;  /**< Must be set to true if phone_context_uri is being passed */
  char phone_context_uri[IMS_SETTINGS_IMS_STRING_LEN_MAX_V01 + 1];
  /**<   Configure the domain name for a phone. */
}ims_settings_ims_common_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS service enable configuration parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Toggle voice calling support on LTE.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Enable VT Support Through Client Provisioning */
  uint8_t videotelephony_enabled_valid;  /**< Must be set to true if videotelephony_enabled is being passed */
  uint8_t videotelephony_enabled;
  /**<   Toggle videotelephony support on the device.
          Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Toggle mobile data status.
          Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Service Mask Status by Network */
  uint8_t service_mask_by_network_enabled_valid;  /**< Must be set to true if service_mask_by_network_enabled is being passed */
  ims_settings_omadm_services_type_mask_v01 service_mask_by_network_enabled;
  /**<   Mask of services enabled by the network through OMA-DM.
         If not set, the default is enabled with all services.
    */

  /* Optional */
  /*  Enable Wi-Fi Calling Support Through Client Provisioning */
  uint8_t wifi_calling_enabled_valid;  /**< Must be set to true if wifi_calling_enabled is being passed */
  uint8_t wifi_calling_enabled;
  /**<   Toggle support for calls on Wi-Fi.
         Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Call Mode Preference */
  uint8_t call_mode_preference_valid;  /**< Must be set to true if call_mode_preference is being passed */
  ims_settings_call_mode_option_enum_v01 call_mode_preference;
  /**<   When Wi-Fi is enabled, a call mode preference can be selected for calls
         as cellular preferred or Wi-Fi preferred.
         If not set, the default is operator specific.
    */

  /* Optional */
  /*  Enable Wi-Fi Calling Support in Roaming Through Client Provisioning */
  uint8_t wifi_calling_in_roaming_enabled_valid;  /**< Must be set to true if wifi_calling_in_roaming_enabled is being passed */
  uint8_t wifi_calling_in_roaming_enabled;
  /**<   Toggle support for calls on Wi-Fi in a roaming network.
         Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t wifi_provisioning_id_valid;  /**< Must be set to true if wifi_provisioning_id is being passed */
  char wifi_provisioning_id[IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01 + 1];
  /**<   Unique identifier required to enable Wi-Fi as per an operator request. */

  /* Optional */
  /*  IMS Service Status */
  uint8_t ims_service_enabled_valid;  /**< Must be set to true if ims_service_enabled is being passed */
  uint8_t ims_service_enabled;
  /**<   Enable/disable IMS Normal Registration mode.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  UT Status */
  uint8_t ut_enabled_valid;  /**< Must be set to true if ut_enabled is being passed */
  uint8_t ut_enabled;
  /**<   Enable/disable the UT feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  SMS Service Status */
  uint8_t sms_enabled_valid;  /**< Must be set to true if sms_enabled is being passed */
  uint8_t sms_enabled;
  /**<   SMS over IP network indication.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  DAN Service Status */
  uint8_t dan_enabled_valid;  /**< Must be set to true if dan_enabled is being passed */
  uint8_t dan_enabled;
  /**<   Enable/disable the DAN feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  USSD Status */
  uint8_t ussd_enabled_valid;  /**< Must be set to true if ussd_enabled is being passed */
  uint8_t ussd_enabled;
  /**<   Enable/disable the USSD feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  MWI Status */
  uint8_t mwi_enabled_valid;  /**< Must be set to true if mwi_enabled is being passed */
  uint8_t mwi_enabled;
  /**<   Enable/disable the MWI feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable (default)
    */

  /* Optional */
  /*  Presence Service Status */
  uint8_t presence_enabled_valid;  /**< Must be set to true if presence_enabled is being passed */
  uint8_t presence_enabled;
  /**<   Enable/disable the presence service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */

  /* Optional */
  /*  Autoconfig Service Status */
  uint8_t autoconfig_enabled_valid;  /**< Must be set to true if autoconfig_enabled is being passed */
  uint8_t autoconfig_enabled;
  /**<   Enable/disable the autoconfiguration service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */

  /* Optional */
  /*  XDM Client Status */
  uint8_t xdm_client_enabled_valid;  /**< Must be set to true if xdm_client_enabled is being passed */
  uint8_t xdm_client_enabled;
  /**<   Enable/disable the XDM client.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */

  /* Optional */
  /*  RCS Messaging Service Status */
  uint8_t rcs_messaging_enabled_valid;  /**< Must be set to true if rcs_messaging_enabled is being passed */
  uint8_t rcs_messaging_enabled;
  /**<   Enable/disable RCS messaging.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */

  /* Optional */
  /*  Call Mode Preference Roaming Service Status */
  uint8_t call_mode_pref_roam_enabled_valid;  /**< Must be set to true if call_mode_pref_roam_enabled is being passed */
  uint8_t call_mode_pref_roam_enabled;
  /**<   Enable/disable the call mode preference roaming service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */

  /* Optional */
  /*  RTT Service Status */
  uint8_t rtt_enabled_valid;  /**< Must be set to true if rtt_enabled is being passed */
  uint8_t rtt_enabled;
  /**<   Enable/disable the Real time text service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */
}ims_settings_set_ims_service_enable_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS service enable configuration parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_ims_service_enable_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS service enable configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_ims_service_enable_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS service enable configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Toggle voice calling support on LTE.
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  Enabled VT Support Through Client Provisioning */
  uint8_t videotelephony_enabled_valid;  /**< Must be set to true if videotelephony_enabled is being passed */
  uint8_t videotelephony_enabled;
  /**<   Toggle videotelephony support on the device.
          Values:  \n
         - TRUE -- Enabled  \n
         - FALSE -- Disabled  (default)
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Toggle the mobile data status.
          Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  Service Mask Status by Network */
  uint8_t service_mask_by_network_enabled_valid;  /**< Must be set to true if service_mask_by_network_enabled is being passed */
  ims_settings_omadm_services_type_mask_v01 service_mask_by_network_enabled;
  /**<   Mask of services enabled by the network through OMA-DM. */

  /* Optional */
  /*  Enable Wi-Fi Calling Support Through Client Provisioning */
  uint8_t wifi_calling_enabled_valid;  /**< Must be set to true if wifi_calling_enabled is being passed */
  uint8_t wifi_calling_enabled;
  /**<   Toggle support for calls on Wi-Fi.
         Values:  \n
         - TRUE -- Enabled  \n
         - FALSE -- Disabled  (default)
    */

  /* Optional */
  /*  Call Mode Preference */
  uint8_t call_mode_preference_valid;  /**< Must be set to true if call_mode_preference is being passed */
  ims_settings_call_mode_option_enum_v01 call_mode_preference;
  /**<   When Wi-Fi is enabled, the call mode preference can be selected for
         calls as cellular preferred or Wi-Fi preferred. */

  /* Optional */
  /*  Enable Wi-Fi Calling Support in Roaming Through Client Provisioning */
  uint8_t wifi_calling_in_roaming_enabled_valid;  /**< Must be set to true if wifi_calling_in_roaming_enabled is being passed */
  uint8_t wifi_calling_in_roaming_enabled;
  /**<   Toggle support for calls on Wi-Fi in a roaming network.
         Values:  \n
         - TRUE -- Enabled  \n
         - FALSE -- Disabled  (default)
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t wifi_provisioning_id_valid;  /**< Must be set to true if wifi_provisioning_id is being passed */
  char wifi_provisioning_id[IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01 + 1];
  /**<   Unique identifier required to enable Wi-Fi as per an operator request. */

  /* Optional */
  /*  IMS Service Status */
  uint8_t ims_service_enabled_valid;  /**< Must be set to true if ims_service_enabled is being passed */
  uint8_t ims_service_enabled;
  /**<   IMS Normal Registration mode enabled/disabled.
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  UT Status */
  uint8_t ut_enabled_valid;  /**< Must be set to true if ut_enabled is being passed */
  uint8_t ut_enabled;
  /**<   UT feature enabled/disabled.
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  SMS Service Status */
  uint8_t sms_enabled_valid;  /**< Must be set to true if sms_enabled is being passed */
  uint8_t sms_enabled;
  /**<   SMS over IP network indication enabled/disabled.
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  DAN Service Status */
  uint8_t dan_enabled_valid;  /**< Must be set to true if dan_enabled is being passed */
  uint8_t dan_enabled;
  /**<   DAN feature enabled/disabled.
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  USSD Status */
  uint8_t ussd_enabled_valid;  /**< Must be set to true if ussd_enabled is being passed */
  uint8_t ussd_enabled;
  /**<   USSD feature enabled/disabled.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  MWI Status */
  uint8_t mwi_enabled_valid;  /**< Must be set to true if mwi_enabled is being passed */
  uint8_t mwi_enabled;
  /**<   MWI feature enabled/disabled. 
         Values: \n
        - TRUE -- Enabled  \n
        - FALSE -- Disabled 
    */

  /* Optional */
  /*  Presence Service Status */
  uint8_t presence_enabled_valid;  /**< Must be set to true if presence_enabled is being passed */
  uint8_t presence_enabled;
  /**<   Presence service enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  Autoconfig Service Status */
  uint8_t autoconfig_enabled_valid;  /**< Must be set to true if autoconfig_enabled is being passed */
  uint8_t autoconfig_enabled;
  /**<   Autoconfiguration service enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  XDM Client Status */
  uint8_t xdm_client_enabled_valid;  /**< Must be set to true if xdm_client_enabled is being passed */
  uint8_t xdm_client_enabled;
  /**<   XDM client enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  RCS Messaging Service Status */
  uint8_t rcs_messaging_enabled_valid;  /**< Must be set to true if rcs_messaging_enabled is being passed */
  uint8_t rcs_messaging_enabled;
  /**<   RCS messaging enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  Call Mode Preference Roaming Service Status */
  uint8_t call_mode_pref_roam_enabled_valid;  /**< Must be set to true if call_mode_pref_roam_enabled is being passed */
  uint8_t call_mode_pref_roam_enabled;
  /**<   Call mode preference roaming enabled/disabled.
         Values: \n
        - TRUE (1) -- Enabled  \n
        - FALSE (0) -- Disabled 
    */

  /* Optional */
  /*  RTT Service Status */
  uint8_t rtt_enabled_valid;  /**< Must be set to true if rtt_enabled is being passed */
  uint8_t rtt_enabled;
  /**<   Enable/disable the Real time text service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */
}ims_settings_get_ims_service_enable_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS service enable configuration parameters change. */
typedef struct {

  /* Optional */
  /*  VoLTE Status */
  uint8_t volte_enabled_valid;  /**< Must be set to true if volte_enabled is being passed */
  uint8_t volte_enabled;
  /**<   Toggle voice calling support on LTE.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  Enable VT Support Through Client Provisioning */
  uint8_t videotelephony_enabled_valid;  /**< Must be set to true if videotelephony_enabled is being passed */
  uint8_t videotelephony_enabled;
  /**<   Toggle videotelephony support on the device.
          Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Mobile Data Status */
  uint8_t mobile_data_enabled_valid;  /**< Must be set to true if mobile_data_enabled is being passed */
  uint8_t mobile_data_enabled;
  /**<   Toggle the mobile data status.
          Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  Service Mask Status by Network */
  uint8_t service_mask_by_network_enabled_valid;  /**< Must be set to true if service_mask_by_network_enabled is being passed */
  ims_settings_omadm_services_type_mask_v01 service_mask_by_network_enabled;
  /**<   Mask of services enabled by the network through OMA-DM. */

  /* Optional */
  /*  Enable Wi-Fi Calling Support Through Client Provisioning */
  uint8_t wifi_calling_enabled_valid;  /**< Must be set to true if wifi_calling_enabled is being passed */
  uint8_t wifi_calling_enabled;
  /**<   Toggle support for calls on wifi
         Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Call Mode Preference */
  uint8_t call_mode_preference_valid;  /**< Must be set to true if call_mode_preference is being passed */
  ims_settings_call_mode_option_enum_v01 call_mode_preference;
  /**<   When Wi-Fi is enabled, the call mode preference can be selected for
         calls as cellular preferred or Wi-Fi preferred. */

  /* Optional */
  /*  Enable Wi-Fi Calling Support in Roaming Through Client Provisioning */
  uint8_t wifi_calling_in_roaming_enabled_valid;  /**< Must be set to true if wifi_calling_in_roaming_enabled is being passed */
  uint8_t wifi_calling_in_roaming_enabled;
  /**<   Toggle support for calls on Wi-Fi in a roaming network.
         Values:  \n
         - TRUE -- Enable  \n
         - FALSE -- Disable  (default)
    */

  /* Optional */
  /*  Phone Context Universal Resource Identifier */
  uint8_t wifi_provisioning_id_valid;  /**< Must be set to true if wifi_provisioning_id is being passed */
  char wifi_provisioning_id[IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01 + 1];
  /**<   Unique identifier required to enable Wi-Fi as per an operator request. */

  /* Optional */
  /*  IMS Service Status */
  uint8_t ims_service_enabled_valid;  /**< Must be set to true if ims_service_enabled is being passed */
  uint8_t ims_service_enabled;
  /**<   Enable/disable IMS Normal Registration mode.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  UT Status */
  uint8_t ut_enabled_valid;  /**< Must be set to true if ut_enabled is being passed */
  uint8_t ut_enabled;
  /**<   Enable/disable the UT feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  SMS Service Status */
  uint8_t sms_enabled_valid;  /**< Must be set to true if sms_enabled is being passed */
  uint8_t sms_enabled;
  /**<   Enable/disable SMS over IP network indication.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  DAN Service Status */
  uint8_t dan_enabled_valid;  /**< Must be set to true if dan_enabled is being passed */
  uint8_t dan_enabled;
  /**<   Enable/disable the DAN feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  USSD Status */
  uint8_t ussd_enabled_valid;  /**< Must be set to true if ussd_enabled is being passed */
  uint8_t ussd_enabled;
  /**<   Enable/disable the USSD feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  MWI Status */
  uint8_t mwi_enabled_valid;  /**< Must be set to true if mwi_enabled is being passed */
  uint8_t mwi_enabled;
  /**<   Enable/disable the MWI feature.
         Values: \n
        - TRUE -- Enable  \n
        - FALSE -- Disable 
    */

  /* Optional */
  /*  Presence Service Status */
  uint8_t presence_enabled_valid;  /**< Must be set to true if presence_enabled is being passed */
  uint8_t presence_enabled;
  /**<   Enable/disable the presence service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable 
    */

  /* Optional */
  /*  Autoconfig Service Status */
  uint8_t autoconfig_enabled_valid;  /**< Must be set to true if autoconfig_enabled is being passed */
  uint8_t autoconfig_enabled;
  /**<   Enable/disable the autoconfiguration service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable 
    */

  /* Optional */
  /*  XDM Client Status */
  uint8_t xdm_client_enabled_valid;  /**< Must be set to true if xdm_client_enabled is being passed */
  uint8_t xdm_client_enabled;
  /**<   Enable/disable the XDM client.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable 
    */

  /* Optional */
  /*  RCS Messaging Service Status */
  uint8_t rcs_messaging_enabled_valid;  /**< Must be set to true if rcs_messaging_enabled is being passed */
  uint8_t rcs_messaging_enabled;
  /**<   Enable/disable RCS messaging.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable 
    */

  /* Optional */
  /*  Call Mode Preference Roaming Serivce Status */
  uint8_t call_mode_pref_roam_enabled_valid;  /**< Must be set to true if call_mode_pref_roam_enabled is being passed */
  uint8_t call_mode_pref_roam_enabled;
  /**<   Enable/disable call mode preference roaming.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable 
    */

  /* Optional */
  /*  RTT Service Status */
  uint8_t rtt_enabled_valid;  /**< Must be set to true if rtt_enabled is being passed */
  uint8_t rtt_enabled;
  /**<   Enable/disable the Real time text service.
         Values: \n
        - TRUE (1) -- Enable  \n
        - FALSE (0) -- Disable (default)
    */
}ims_settings_ims_service_enable_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Sets the IMS application's QIPCALL EVS
             configuration parameters for the requesting control point. */
typedef struct {

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   EVS primary value, payload size. \n
         Valid range of values: -1 to 32767
         If not set, default value -1 will be used.
    */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   EVS SDP Codec bit rate.
         If not set, default value IMS_SETTINGS_EVS_SDP_CODEC_MODE_UNK.
    */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   QVP RTP SDP bandwidth.
         If not set, default value IMS_SETTINGS_QVP_RTP_SDP_BW_UNKNOWN.
    */
}ims_settings_set_qipcall_evs_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Sets the IMS application's QIPCALL EVS
             configuration parameters for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_set_qipcall_evs_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS application's QIPCALL EVS configuration parameters. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_settings_get_qipcall_evs_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS application's QIPCALL EVS configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. A settings-specific error code is returned when 
         the standard response error type is QMI_ERR_CAUSE_CODE.
    */

  /* Optional */
  /*  Settings Response */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   Settings response. */

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   EVS primary value. */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   EVS SDP Codec bit rate. */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   QVP RTP SDP bandwidth. */
}ims_settings_get_qipcall_evs_config_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS application's  EVS configuration parameters
             change. */
typedef struct {

  /* Optional */
  /*  EVS Primary */
  uint8_t evs_primary_valid;  /**< Must be set to true if evs_primary is being passed */
  int16_t evs_primary;
  /**<   EVS primary value. */

  /* Optional */
  /*  EVS SDP Codec bit rate */
  uint8_t evs_sdp_codec_valid;  /**< Must be set to true if evs_sdp_codec is being passed */
  ims_settings_evs_sdp_codec_mode_option_enum_v01 evs_sdp_codec;
  /**<   EVS SDP Codec bit rate. */

  /* Optional */
  /*  SDP bandwidth */
  uint8_t qvp_rtp_sdp_bw_valid;  /**< Must be set to true if qvp_rtp_sdp_bw is being passed */
  ims_settings_qvp_rtp_sdp_bw_option_enum_v01 qvp_rtp_sdp_bw;
  /**<   QVP RTP SDP bandwidth. */
}ims_settings_qipcall_evs_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Real time text from the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Received RTT Data */
  uint32_t rtt_data_len;  /**< Must be set to # of elements in rtt_data */
  uint8_t rtt_data[IMS_SETTINGS_RTT_DATA_LEN_MAX_V01];
  /**<   Actual data string received from UE, in UTF8 encoding format
    */

  /* Optional */
  /*  Send RTT Optional Data */
  uint8_t rtt_optional_data_valid;  /**< Must be set to true if rtt_optional_data is being passed */
  ims_settings_rtt_optional_data_v01 rtt_optional_data;
  /**<   Total RTT Optional data
    */

  /* Optional */
  /*  Send RTT Message Reference Number */
  uint8_t rtt_reference_number_valid;  /**< Must be set to true if rtt_reference_number is being passed */
  uint16_t rtt_reference_number;
  /**<    RTT Reference Number
    */
}ims_settings_send_rtt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Real time text from the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described 
                            in the error codes section of each message 
                            definition.
    */

  /* Optional */
  /*  Settings Standard Response Type */
  uint8_t settings_resp_valid;  /**< Must be set to true if settings_resp is being passed */
  ims_settings_rsp_enum_v01 settings_resp;
  /**<   A settings-specific error code is returned when the standard response
         error type is QMI_ERR_CAUSE_CODE.
    */
}ims_settings_send_rtt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indication sent with RTT data. */
typedef struct {

  /* Mandatory */
  /*  Received RTT Data */
  uint32_t rtt_data_len;  /**< Must be set to # of elements in rtt_data */
  uint8_t rtt_data[IMS_SETTINGS_RTT_DATA_LEN_MAX_V01];
  /**<   Actual data string received from UE, in UTF8 encoding format
    */

  /* Optional */
  /*  Received RTT Optional Data */
  uint8_t rtt_optional_data_valid;  /**< Must be set to true if rtt_optional_data is being passed */
  ims_settings_rtt_optional_data_v01 rtt_optional_data;
  /**<   Total RTT Optional data
    */

  /* Optional */
  /*  Send RTT Message Reference Number */
  uint8_t rtt_reference_number_valid;  /**< Must be set to true if rtt_reference_number is being passed */
  uint16_t rtt_reference_number;
  /**<    RTT Reference Number
    	*/
}ims_settings_rtt_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Binds subscription for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Bind subscription */
  ims_subscription_type_enum_v01 subscription_type;
  /**<   Subscription information to bind
  */
}ims_set_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Binds subscription for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
  - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
  - qmi_error_type  -- Error code. Possible error code values are described
  in the error codes section of each message
  definition.
  */

  /* Optional */
  /*  IMS Common Standard Response Type */
  uint8_t common_resp_valid;  /**< Must be set to true if common_resp is being passed */
  ims_common_resp_enum_v01 common_resp;
  /**<   A ims-common-specific error code is returned when the standard response
  error type is QMI_ERR_CAUSE_CODE.
  */
}ims_set_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Request Message; Gets the active subscription status for IMS from the
requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ims_get_active_subscription_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Response Message; Gets the active subscription status for IMS from the
requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  IMS Common Standard Response Type */
  uint8_t common_resp_valid;  /**< Must be set to true if common_resp is being passed */
  ims_common_resp_enum_v01 common_resp;
  /**<   A ims-common-specific error code is returned when the standard response
  error type is QMI_ERR_CAUSE_CODE.
  */

  /* Optional */
  /*  Primary Subscription Status */
  uint8_t active_on_primary_subscription_valid;  /**< Must be set to true if active_on_primary_subscription is being passed */
  uint8_t active_on_primary_subscription;
  /**<   Values: \n
  - TRUE  -- IMS active on primary subscription
  - FALSE -- (default)
  */

  /* Optional */
  /*  Secondary Subscription Status */
  uint8_t active_on_secondary_subscription_valid;  /**< Must be set to true if active_on_secondary_subscription is being passed */
  uint8_t active_on_secondary_subscription;
  /**<   Values: \n
  - TRUE  -- IMS active on secondary subscription
  - FALSE -- (default)
  */

  /* Optional */
  /*  Tertiary Subscription Status */
  uint8_t active_on_tertiary_subscription_valid;  /**< Must be set to true if active_on_tertiary_subscription is being passed */
  uint8_t active_on_tertiary_subscription;
  /**<   Values: \n
  - TRUE  -- IMS active on tertiary subscription
  - FALSE -- (default)
  */
}ims_get_active_subscription_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imss_qmi_messages
    @{
  */
/** Indication Message; Indicates Clients whether IMS is Active or Inactive on the sub on which Client is bound. */
typedef struct {

  /* Mandatory */
  /*  IMS Active Subscription Status */
  uint8_t isActive;
  /**<   Specifies whether IMS is active or not
  Values: \n
  - TRUE -- Enable  \n
  - FALSE -- Disable
  */
}ims_active_subscription_ind_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IMS_ACTIVE_SUBSCRIPTION_IND_V01 
//#define REMOVE_QMI_IMS_GET_ACTIVE_SUBSCRIPTION_STATUS_V01 
//#define REMOVE_QMI_IMS_SETTINGS_ACS_NETWORK_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_CLIENT_PROVISIONING_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_CODEC_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_CONFIG_IND_REG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_EMER_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_ACS_NETWORK_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_IMS_COMMON_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_MEDIA_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_NETWORK_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_POL_MGR_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_POL_MGR_RAT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_PRESENCE_EXT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_QIPCALL_EVS_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_QIPCALL_VICE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_RCS_AUTO_CONFIG_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_RCS_IMSCORE_AUTO_CONFIG_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_RCS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_RCS_SM_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_REG_MGR_EXTENDED_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_REG_MGR_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SIP_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SIP_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SMS_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SUPPORTED_FIELDS_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_TEST_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_USER_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_USER_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_UT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_VOIP_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_VOIP_READ_ONLY_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_VT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_GET_XCAP_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_HANDOVER_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_IMS_COMMON_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_IMS_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_MEDIA_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_NETWORK_READ_ONLY_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_POL_MGR_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_POL_MGR_RAT_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_PRESENCE_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_PRESENCE_EXT_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_PRESENCE_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_QIPCALL_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_QIPCALL_EVS_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_QIPCALL_VICE_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_RCS_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_RCS_SM_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_REG_MGR_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_REG_MGR_EXTENDED_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_REG_MGR_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_RTP_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_RTT_RCVD_DATA_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SEND_RTT_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_ACS_NETWORK_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_APCS_COMPLETE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_CSD_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_IMS_COMMON_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_MEDIA_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_POL_MGR_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_POL_MGR_RAT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_PRESENCE_EXT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_QIPCALL_EVS_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_QIPCALL_VICE_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_RCS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_RCS_SM_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_REG_MGR_EXTENDED_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_SIP_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_SMS_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_TEST_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_USER_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_UT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_VOIP_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_VT_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SET_XCAP_NEW_CONFIG_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SIP_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SIP_READ_ONLY_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SMS_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_SMS_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_TEST_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_USER_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_UT_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_VIDEO_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_VOICE_DYNAMIC_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_VOIP_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_VT_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SETTINGS_XCAP_NEW_CONFIG_IND_V01 
//#define REMOVE_QMI_IMS_SET_BIND_SUBSCRIPTION_V01 

/*Service Message Definition*/
/** @addtogroup imss_qmi_msg_ids
    @{
  */
#define QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_IMS_SETTINGS_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_IMS_SETTINGS_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_IMS_SETTINGS_SET_SIP_CONFIG_REQ_V01 0x0020
#define QMI_IMS_SETTINGS_SET_SIP_CONFIG_RSP_V01 0x0020
#define QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_REQ_V01 0x0021
#define QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_RSP_V01 0x0021
#define QMI_IMS_SETTINGS_SET_SMS_CONFIG_REQ_V01 0x0022
#define QMI_IMS_SETTINGS_SET_SMS_CONFIG_RSP_V01 0x0022
#define QMI_IMS_SETTINGS_SET_USER_CONFIG_REQ_V01 0x0023
#define QMI_IMS_SETTINGS_SET_USER_CONFIG_RSP_V01 0x0023
#define QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01 0x0024
#define QMI_IMS_SETTINGS_SET_VOIP_CONFIG_RSP_V01 0x0024
#define QMI_IMS_SETTINGS_GET_SIP_CONFIG_REQ_V01 0x0025
#define QMI_IMS_SETTINGS_GET_SIP_CONFIG_RSP_V01 0x0025
#define QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_REQ_V01 0x0026
#define QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_RSP_V01 0x0026
#define QMI_IMS_SETTINGS_GET_SMS_CONFIG_REQ_V01 0x0027
#define QMI_IMS_SETTINGS_GET_SMS_CONFIG_RSP_V01 0x0027
#define QMI_IMS_SETTINGS_GET_USER_CONFIG_REQ_V01 0x0028
#define QMI_IMS_SETTINGS_GET_USER_CONFIG_RSP_V01 0x0028
#define QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01 0x0029
#define QMI_IMS_SETTINGS_GET_VOIP_CONFIG_RSP_V01 0x0029
#define QMI_IMS_SETTINGS_CONFIG_IND_REG_REQ_V01 0x002A
#define QMI_IMS_SETTINGS_CONFIG_IND_REG_RSP_V01 0x002A
#define QMI_IMS_SETTINGS_SIP_CONFIG_IND_V01 0x002B
#define QMI_IMS_SETTINGS_REG_MGR_CONFIG_IND_V01 0x002C
#define QMI_IMS_SETTINGS_SMS_CONFIG_IND_V01 0x002D
#define QMI_IMS_SETTINGS_USER_CONFIG_IND_V01 0x002E
#define QMI_IMS_SETTINGS_VOIP_CONFIG_IND_V01 0x002F
#define QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01 0x0030
#define QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_RSP_V01 0x0030
#define QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01 0x0031
#define QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_RSP_V01 0x0031
#define QMI_IMS_SETTINGS_PRESENCE_CONFIG_IND_V01 0x0032
#define QMI_IMS_SETTINGS_SET_MEDIA_CONFIG_REQ_V01 0x0033
#define QMI_IMS_SETTINGS_SET_MEDIA_CONFIG_RSP_V01 0x0033
#define QMI_IMS_SETTINGS_GET_MEDIA_CONFIG_REQ_V01 0x0034
#define QMI_IMS_SETTINGS_GET_MEDIA_CONFIG_RSP_V01 0x0034
#define QMI_IMS_SETTINGS_MEDIA_CONFIG_IND_V01 0x0035
#define QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_REQ_V01 0x0036
#define QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_RSP_V01 0x0036
#define QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_REQ_V01 0x0037
#define QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_RSP_V01 0x0037
#define QMI_IMS_SETTINGS_QIPCALL_CONFIG_IND_V01 0x0038
#define QMI_IMS_SETTINGS_GET_SIP_READ_ONLY_CONFIG_REQ_V01 0x0039
#define QMI_IMS_SETTINGS_GET_SIP_READ_ONLY_CONFIG_RSP_V01 0x0039
#define QMI_IMS_SETTINGS_SIP_READ_ONLY_CONFIG_IND_V01 0x003A
#define QMI_IMS_SETTINGS_GET_NETWORK_READ_ONLY_CONFIG_REQ_V01 0x003D
#define QMI_IMS_SETTINGS_GET_NETWORK_READ_ONLY_CONFIG_RSP_V01 0x003D
#define QMI_IMS_SETTINGS_NETWORK_READ_ONLY_CONFIG_IND_V01 0x003E
#define QMI_IMS_SETTINGS_GET_VOIP_READ_ONLY_CONFIG_REQ_V01 0x003F
#define QMI_IMS_SETTINGS_GET_VOIP_READ_ONLY_CONFIG_RSP_V01 0x003F
#define QMI_IMS_SETTINGS_GET_USER_READ_ONLY_CONFIG_REQ_V01 0x0040
#define QMI_IMS_SETTINGS_GET_USER_READ_ONLY_CONFIG_RSP_V01 0x0040
#define QMI_IMS_SETTINGS_GET_REG_MGR_READ_ONLY_CONFIG_REQ_V01 0x0041
#define QMI_IMS_SETTINGS_GET_REG_MGR_READ_ONLY_CONFIG_RSP_V01 0x0041
#define QMI_IMS_SETTINGS_GET_RCS_AUTO_CONFIG_READ_ONLY_CONFIG_REQ_V01 0x0042
#define QMI_IMS_SETTINGS_GET_RCS_AUTO_CONFIG_READ_ONLY_CONFIG_RSP_V01 0x0042
#define QMI_IMS_SETTINGS_GET_RCS_IMSCORE_AUTO_CONFIG_READ_ONLY_CONFIG_REQ_V01 0x0043
#define QMI_IMS_SETTINGS_GET_RCS_IMSCORE_AUTO_CONFIG_READ_ONLY_CONFIG_RSP_V01 0x0043
#define QMI_IMS_SETTINGS_SET_REG_MGR_EXTENDED_CONFIG_REQ_V01 0x0044
#define QMI_IMS_SETTINGS_SET_REG_MGR_EXTENDED_CONFIG_RSP_V01 0x0044
#define QMI_IMS_SETTINGS_GET_REG_MGR_EXTENDED_CONFIG_REQ_V01 0x0045
#define QMI_IMS_SETTINGS_GET_REG_MGR_EXTENDED_CONFIG_RSP_V01 0x0045
#define QMI_IMS_SETTINGS_REG_MGR_EXTENDED_CONFIG_IND_V01 0x0046
#define QMI_IMS_SETTINGS_SET_POL_MGR_CONFIG_REQ_V01 0x0047
#define QMI_IMS_SETTINGS_SET_POL_MGR_CONFIG_RSP_V01 0x0047
#define QMI_IMS_SETTINGS_GET_POL_MGR_CONFIG_REQ_V01 0x0048
#define QMI_IMS_SETTINGS_GET_POL_MGR_CONFIG_RSP_V01 0x0048
#define QMI_IMS_SETTINGS_POL_MGR_CONFIG_IND_V01 0x0049
#define QMI_IMS_SETTINGS_SET_PRESENCE_EXT_CONFIG_REQ_V01 0x004A
#define QMI_IMS_SETTINGS_SET_PRESENCE_EXT_CONFIG_RSP_V01 0x004A
#define QMI_IMS_SETTINGS_GET_PRESENCE_EXT_CONFIG_REQ_V01 0x004B
#define QMI_IMS_SETTINGS_GET_PRESENCE_EXT_CONFIG_RSP_V01 0x004B
#define QMI_IMS_SETTINGS_PRESENCE_EXT_CONFIG_IND_V01 0x004C
#define QMI_IMS_SETTINGS_SET_RCS_SM_CONFIG_REQ_V01 0x004D
#define QMI_IMS_SETTINGS_SET_RCS_SM_CONFIG_RSP_V01 0x004D
#define QMI_IMS_SETTINGS_GET_RCS_SM_CONFIG_REQ_V01 0x004E
#define QMI_IMS_SETTINGS_GET_RCS_SM_CONFIG_RSP_V01 0x004E
#define QMI_IMS_SETTINGS_RCS_SM_CONFIG_IND_V01 0x004F
#define QMI_IMS_SETTINGS_SET_UT_CONFIG_REQ_V01 0x0050
#define QMI_IMS_SETTINGS_SET_UT_CONFIG_RSP_V01 0x0050
#define QMI_IMS_SETTINGS_GET_UT_CONFIG_REQ_V01 0x0051
#define QMI_IMS_SETTINGS_GET_UT_CONFIG_RSP_V01 0x0051
#define QMI_IMS_SETTINGS_UT_CONFIG_IND_V01 0x0052
#define QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_REQ_V01 0x0053
#define QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_RSP_V01 0x0053
#define QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_REQ_V01 0x0054
#define QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_RSP_V01 0x0054
#define QMI_IMS_SETTINGS_CLIENT_PROVISIONING_CONFIG_IND_V01 0x0055
#define QMI_IMS_SETTINGS_SET_APCS_COMPLETE_CONFIG_REQ_V01 0x0056
#define QMI_IMS_SETTINGS_SET_APCS_COMPLETE_CONFIG_RESP_V01 0x0056
#define QMI_IMS_SETTINGS_SET_VT_CONFIG_REQ_V01 0x0057
#define QMI_IMS_SETTINGS_SET_VT_CONFIG_RSP_V01 0x0057
#define QMI_IMS_SETTINGS_GET_VT_CONFIG_REQ_V01 0x0058
#define QMI_IMS_SETTINGS_GET_VT_CONFIG_RSP_V01 0x0058
#define QMI_IMS_SETTINGS_VT_CONFIG_IND_V01 0x0059
#define QMI_IMS_SETTINGS_GET_ACS_NETWORK_CONFIG_REQ_V01 0x005A
#define QMI_IMS_SETTINGS_GET_ACS_NETWORK_CONFIG_RSP_V01 0x005A
#define QMI_IMS_SETTINGS_ACS_NETWORK_CONFIG_IND_V01 0x005B
#define QMI_IMS_SETTINGS_SET_ACS_NETWORK_CONFIG_REQ_V01 0x005C
#define QMI_IMS_SETTINGS_SET_ACS_NETWORK_CONFIG_RSP_V01 0x005C
#define QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_REQ_V01 0x005D
#define QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01 0x005D
#define QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_REQ_V01 0x005E
#define QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01 0x005E
#define QMI_IMS_SETTINGS_HANDOVER_CONFIG_IND_V01 0x005F
#define QMI_IMS_SETTINGS_SET_QIPCALL_VICE_CONFIG_REQ_V01 0x0060
#define QMI_IMS_SETTINGS_SET_QIPCALL_VICE_CONFIG_RSP_V01 0x0060
#define QMI_IMS_SETTINGS_GET_QIPCALL_VICE_CONFIG_REQ_V01 0x0061
#define QMI_IMS_SETTINGS_GET_QIPCALL_VICE_CONFIG_RSP_V01 0x0061
#define QMI_IMS_SETTINGS_QIPCALL_VICE_CONFIG_IND_V01 0x0062
#define QMI_IMS_SETTINGS_SET_POL_MGR_RAT_CONFIG_REQ_V01 0x0063
#define QMI_IMS_SETTINGS_SET_POL_MGR_RAT_CONFIG_RSP_V01 0x0063
#define QMI_IMS_SETTINGS_GET_POL_MGR_RAT_CONFIG_REQ_V01 0x0064
#define QMI_IMS_SETTINGS_GET_POL_MGR_RAT_CONFIG_RSP_V01 0x0064
#define QMI_IMS_SETTINGS_POL_MGR_RAT_CONFIG_IND_V01 0x0065
#define QMI_IMS_SETTINGS_SET_CSD_CONFIG_REQ_V01 0x0066
#define QMI_IMS_SETTINGS_SET_CSD_CONFIG_RSP_V01 0x0066
#define QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_REQ_V01 0x0067
#define QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01 0x0067
#define QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_REQ_V01 0x0068
#define QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01 0x0068
#define QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_REQ_V01 0x0069
#define QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01 0x0069
#define QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_REQ_V01 0x006A
#define QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01 0x006A
#define QMI_IMS_SETTINGS_PRESENCE_NEW_CONFIG_IND_V01 0x006B
#define QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_REQ_V01 0x006C
#define QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_RSP_V01 0x006C
#define QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_REQ_V01 0x006D
#define QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_RSP_V01 0x006D
#define QMI_IMS_SETTINGS_RCS_NEW_CONFIG_IND_V01 0x006E
#define QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_REQ_V01 0x006F
#define QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01 0x006F
#define QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_REQ_V01 0x0070
#define QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01 0x0070
#define QMI_IMS_SETTINGS_REG_MGR_NEW_CONFIG_IND_V01 0x0071
#define QMI_IMS_SETTINGS_SET_XCAP_NEW_CONFIG_REQ_V01 0x0072
#define QMI_IMS_SETTINGS_SET_XCAP_NEW_CONFIG_RSP_V01 0x0072
#define QMI_IMS_SETTINGS_GET_XCAP_NEW_CONFIG_REQ_V01 0x0073
#define QMI_IMS_SETTINGS_GET_XCAP_NEW_CONFIG_RSP_V01 0x0073
#define QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_REQ_V01 0x0074
#define QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_RSP_V01 0x0074
#define QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_REQ_V01 0x0075
#define QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_RSP_V01 0x0075
#define QMI_IMS_SETTINGS_SMS_NEW_CONFIG_IND_V01 0x0076
#define QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_REQ_V01 0x0077
#define QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01 0x0077
#define QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_REQ_V01 0x0078
#define QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01 0x0078
#define QMI_IMS_SETTINGS_VOICE_DYNAMIC_CONFIG_IND_V01 0x0079
#define QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_REQ_V01 0x007A
#define QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01 0x007A
#define QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_REQ_V01 0x007B
#define QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01 0x007B
#define QMI_IMS_SETTINGS_CODEC_DYNAMIC_CONFIG_IND_V01 0x007C
#define QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_REQ_V01 0x007D
#define QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_RSP_V01 0x007D
#define QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_REQ_V01 0x007E
#define QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_RSP_V01 0x007E
#define QMI_IMS_SETTINGS_VIDEO_DYNAMIC_CONFIG_IND_V01 0x007F
#define QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_REQ_V01 0x0080
#define QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_RSP_V01 0x0080
#define QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_REQ_V01 0x0081
#define QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_RSP_V01 0x0081
#define QMI_IMS_SETTINGS_EMER_DYNAMIC_CONFIG_IND_V01 0x0082
#define QMI_IMS_SETTINGS_SET_TEST_DYNAMIC_CONFIG_REQ_V01 0x0083
#define QMI_IMS_SETTINGS_SET_TEST_DYNAMIC_CONFIG_RSP_V01 0x0083
#define QMI_IMS_SETTINGS_GET_TEST_DYNAMIC_CONFIG_REQ_V01 0x0084
#define QMI_IMS_SETTINGS_GET_TEST_DYNAMIC_CONFIG_RSP_V01 0x0084
#define QMI_IMS_SETTINGS_TEST_DYNAMIC_CONFIG_IND_V01 0x0085
#define QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_REQ_V01 0x0086
#define QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_RSP_V01 0x0086
#define QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_REQ_V01 0x0087
#define QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_RSP_V01 0x0087
#define QMI_IMS_SETTINGS_RTP_DYNAMIC_CONFIG_IND_V01 0x0088
#define QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_REQ_V01 0x0089
#define QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_RSP_V01 0x0089
#define QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_REQ_V01 0x008A
#define QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_RSP_V01 0x008A
#define QMI_IMS_SETTINGS_IMS_NEW_CONFIG_IND_V01 0x008B
#define QMI_IMS_SETTINGS_SET_IMS_COMMON_CONFIG_REQ_V01 0x008C
#define QMI_IMS_SETTINGS_SET_IMS_COMMON_CONFIG_RSP_V01 0x008C
#define QMI_IMS_SETTINGS_GET_IMS_COMMON_CONFIG_REQ_V01 0x008D
#define QMI_IMS_SETTINGS_GET_IMS_COMMON_CONFIG_RSP_V01 0x008D
#define QMI_IMS_SETTINGS_IMS_COMMON_CONFIG_IND_V01 0x008E
#define QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01 0x008F
#define QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01 0x008F
#define QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01 0x0090
#define QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01 0x0090
#define QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01 0x0091
#define QMI_IMS_SETTINGS_XCAP_NEW_CONFIG_IND_V01 0x0092
#define QMI_IMS_SETTINGS_SET_QIPCALL_EVS_CONFIG_REQ_V01 0x0093
#define QMI_IMS_SETTINGS_SET_QIPCALL_EVS_CONFIG_RSP_V01 0x0093
#define QMI_IMS_SETTINGS_GET_QIPCALL_EVS_CONFIG_REQ_V01 0x0094
#define QMI_IMS_SETTINGS_GET_QIPCALL_EVS_CONFIG_RSP_V01 0x0094
#define QMI_IMS_SETTINGS_QIPCALL_EVS_CONFIG_IND_V01 0x0095
#define QMI_IMS_SETTINGS_SEND_RTT_REQ_V01 0x0096
#define QMI_IMS_SETTINGS_SEND_RTT_RESP_V01 0x0096
#define QMI_IMS_SETTINGS_RTT_RCVD_DATA_IND_V01 0x0097
#define QMI_IMS_SET_BIND_SUBSCRIPTION_REQ_V01 0x0098
#define QMI_IMS_SET_BIND_SUBSCRIPTION_RESP_V01 0x0098
#define QMI_IMS_ACTIVE_SUBSCRIPTION_IND_V01 0x0099
#define QMI_IMS_GET_ACTIVE_SUBSCRIPTION_STATUS_REQ_V01 0x009A
#define QMI_IMS_GET_ACTIVE_SUBSCRIPTION_STATUS_RESP_V01 0x009A
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro imss_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type imss_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define imss_get_service_object_v01( ) \
          imss_get_service_object_internal_v01( \
            IMSS_V01_IDL_MAJOR_VERS, IMSS_V01_IDL_MINOR_VERS, \
            IMSS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

