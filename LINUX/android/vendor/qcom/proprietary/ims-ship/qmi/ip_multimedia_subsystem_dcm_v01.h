#ifndef IMSDCM_SERVICE_01_H
#define IMSDCM_SERVICE_01_H
/**
  @file ip_multimedia_subsystem_dcm_v01.h

  @brief This is the public header file which defines the imsdcm service Data structures.

  This header file defines the types and structures that were defined in
  imsdcm. It contains the constant values defined, enums, structures,
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
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed May 17 2017 (Spin 0)
   From IDL File: ip_multimedia_subsystem_dcm_v01.idl */

/** @defgroup imsdcm_qmi_consts Constant values defined in the IDL */
/** @defgroup imsdcm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup imsdcm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup imsdcm_qmi_messages Structures sent as QMI messages */
/** @defgroup imsdcm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup imsdcm_qmi_accessor Accessor for QMI service object */
/** @defgroup imsdcm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup imsdcm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IMSDCM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IMSDCM_V01_IDL_MINOR_VERS 0x0F
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IMSDCM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IMSDCM_V01_MAX_MESSAGE_ID 0x0034
/**
    @}
  */


/** @addtogroup imsdcm_qmi_consts
    @{
  */

/**  Maximum bytes needed to store an IP address(IPv4/IPv6)  */
#define IMS_DCM_APN_STRING_LEN_MAX_V01 100

/**  Maximum bytes needed to log message  */
#define IMS_DCM_IP_ADDR_STRING_LEN_MAX_V01 40

/**  Enumeration for all the IMS supported Radio Access Technology. */
#define IMS_DCM_LOG_STRING_LEN_MAX_V01 1024
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_RAT_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_RAT_EHRPD_V01 = 0, /**<  CDMA eHRPD \n  */
  IMS_DCM_RAT_LTE_V01 = 1, /**<  LTE \n  */
  IMS_DCM_RAT_EPC_V01 = 2, /**<  EPC \n  */
  IMS_DCM_RAT_WLAN_V01 = 3, /**<  WLAN  */
  IMS_DCM_RAT_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_rat_type_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_BT_STATE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_BT_CRITERIA_MET_V01 = 0, /**<  Where Bluetooth quality criteria is MET \n  */
  IMS_DCM_BT_CRITERIA_NOT_MET_V01 = 1, /**<  Where Bluetooth quality criteria is NOT_MET \n  */
  IMS_DCM_BT_FAILURE_V01 = 2, /**<  Where IMS failed to communicate with BT interface  */
  IMS_DCM_BT_STATE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_bt_state_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_APN_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_APN_IMS_V01 = 0, /**<  IMS \n  */
  IMS_DCM_APN_INTERNET_V01 = 1, /**<  Internet \n  */
  IMS_DCM_APN_EMERGENCY_V01 = 2, /**<  Emergency \n  */
  IMS_DCM_APN_RCS_V01 = 3, /**<  RCS \n  */
  IMS_DCM_APN_UT_V01 = 4, /**<  UT \n  */
  IMS_DCM_APN_WLAN_V01 = 5, /**<  WLAN  */
  IMS_DCM_APN_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_apn_type_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_IP_ADDRESS_FAMILY_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_IPV4_V01 = 0, /**<  IPv4 address \n  */
  IMS_DCM_IPV6_V01 = 1, /**<  IPv6 address  */
  IMS_DCM_IP_ADDRESS_FAMILY_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_ip_address_family_type_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_WIFI_ATTACH_STATUS_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_WIFI_STATE_ATTACH_TO_WIFI_V01 = 0, /**<  IMS attached to WiFi \n  */
  IMS_DCM_WIFI_STATE_DETACH_FROM_WIFI_V01 = 1, /**<  IMS not attached to WiFi  */
  IMS_DCM_WIFI_ATTACH_STATUS_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_wifi_attach_status_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_WIFI_QUALITY_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_WIFI_QUALITY_GOOD_V01 = 0, /**<  Where RSSI value is beyond the threshold \n  */
  IMS_DCM_WIFI_QUALITY_BAD_V01 = 1, /**<  Where RSSI value is below the threshold \n  */
  IMS_DCM_WIFI_IFACE_FAILURE_V01 = 2, /**<  Where IMS failed to communicate with WiFi interface  */
  IMS_DCM_WIFI_QUALITY_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_wifi_quality_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_HO_MEASUREMENT_REPORT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_GOOD_V01 = 0, /**<  Where RSSI and media values are beyond the threshold \n  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM_V01 = 1, /**<  Where RSSI and media values are between high and low  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_BAD_V01 = 2, /**<  Where RSSI and media values are below the threshold  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_FAILURE_V01 = 3, /**<  When there is a failure in WLAN driver/supplicant  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM_HIGH_V01 = 4, /**<  Where RSSI and media value is high  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM_LOW_V01 = 5, /**<  Where RSSI and media value is low  */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_VOICE_CALL_MET_V01 = 6, /**<    Where RSSI value is greated than voice call threshold   */
  IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_VOICE_CALL_NOT_MET_V01 = 7, /**<    WWhere RSSI value is less than voice call threshold  */
  IMS_DCM_HO_MEASUREMENT_REPORT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_ho_measurement_report_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_DCM_HO_MEASUREMENT_MODULE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_DCM_HO_MEASUREMENT_MODULE_WLAN_UL_V01 = 0, /**<   Module is WLAN uplink for which report is needed \n  */
  IMS_DCM_HO_MEASUREMENT_MODULE_WLAN_DL_V01 = 1, /**<   Module is WLAN downlink for which report is needed  \n  */
  IMS_DCM_HO_MEASUREMENT_MODULE_MEDIA_UL_V01 = 2, /**<   Module is media uplink for which report is needed  */
  IMS_DCM_HO_MEASUREMENT_MODULE_MEDIA_DL_V01 = 3, /**<   Module is media downlink for which report is needed  */
  IMS_DCM_HO_MEASUREMENT_MODULE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_dcm_ho_measurement_module_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_enums
    @{
  */
typedef enum {
  IMS_INSTANCE_ID_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_INST_ID_NONE_V01 = -1, /**<  No instanceID  */
  IMS_INST_ID_GLOBAL_V01 = 0, /**<  Instance ID Global  */
  IMS_INST_ID_1_V01 = 1, /**<  Instance ID 1  */
  IMS_INST_ID_2_V01 = 2, /**<  Instance ID 2  */
  IMS_INSTANCE_ID_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_instance_id_enum_v01;
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold the IP address obtained by UE.

 */
typedef struct {

  ims_dcm_ip_address_family_type_v01 family_type;
  /**<   IP address type. Values: \n
      - IMS_DCM_IPV4 (0) --  IPv4 address \n 
      - IMS_DCM_IPV6 (1) --  IPv6 address 
 */

  uint32_t ipaddr_len;  /**< Must be set to # of elements in ipaddr */
  char ipaddr[IMS_DCM_IP_ADDR_STRING_LEN_MAX_V01];
  /**<   Buffer containing the IP address of the interface. 
  For IPv4, this confirms to dot-decimal notation. 
  For IPv6, this confirms to RFC 5952.
  */
}imsdcm_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold the link local address obtained by UE.

 */
typedef struct {

  uint16_t port;
  /**<   Modem end point port. */

  ims_dcm_ip_address_family_type_v01 family_type;
  /**<   IP address type. Values: \n
      - IMS_DCM_IPV4 (0) --  IPv4 address \n 
      - IMS_DCM_IPV6 (1) --  IPv6 address 
 */

  uint32_t ipaddr_len;  /**< Must be set to # of elements in ipaddr */
  char ipaddr[IMS_DCM_IP_ADDR_STRING_LEN_MAX_V01];
  /**<   Buffer containing the IP address of the interface. 
       For IPv4, this confirms to dot-decimal notation.
       For IPv6, this confirms to RFC 5952.
  */
}imsdcm_link_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold information required to bring up the PDP.

 */
typedef struct {

  char apn_name[IMS_DCM_APN_STRING_LEN_MAX_V01 + 1];
  /**<   APN name. */

  ims_dcm_apn_type_v01 apn_type;
  /**<   APN type. Values:\n
      - IMS_DCM_APN_IMS (0) --  IMS \n 
      - IMS_DCM_APN_INTERNET (1) --  Internet \n 
      - IMS_DCM_APN_EMERGENCY (2) --  Emergency \n 
      - IMS_DCM_APN_RCS (3) --  RCS \n 
      - IMS_DCM_APN_UT (4) --  UT \n 
      - IMS_DCM_APN_WLAN (5) --  WLAN 
 */

  ims_dcm_rat_type_v01 rat_type;
  /**<   Radio access technology. Values:\n
      - IMS_DCM_RAT_EHRPD (0) --  CDMA eHRPD \n 
      - IMS_DCM_RAT_LTE (1) --  LTE \n 
      - IMS_DCM_RAT_EPC (2) --  EPC \n 
      - IMS_DCM_RAT_WLAN (3) --  WLAN  
 */

  ims_dcm_ip_address_family_type_v01 family_type;
  /**<   IP address family. Values:\n
      - IMS_DCM_IPV4 (0) --  IPv4 address \n 
      - IMS_DCM_IPV6 (1) --  IPv6 address 
 */

  int32_t profile_num;
  /**<   Data services (DS) profile number */
}ims_dcm_pdp_information_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold information required in algorithm.

 */
typedef struct {

  uint32_t rtcp_jitter_good;
  /**<   Threshold good for RTCP jitter */

  uint32_t rtcp_jitter_acceptable;
  /**<   Threshold acceptable for RTCP jitter */

  uint32_t rtcp_jitter_fair;
  /**<   Threshold fair for RTCP jitter */

  uint32_t rtcp_jitter_bad;
  /**<   Threshold bad for RTCP jitter */
}ims_dcm_rtcp_jitter_threshold_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold information required in algorithm.

 */
typedef struct {

  uint32_t rtp_frame_loss_good;
  /**<   RTP frame loss threshold for good condition */

  uint32_t rtp_frame_loss_acceptable;
  /**<   RTP frame loss threshold for acceptable condition */

  uint32_t rtp_frame_loss_fair;
  /**<   RTP frame loss threshold for fair condition */

  uint32_t rtp_frame_loss_bad;
  /**<   RTP frame loss threshold for bad condition */
}ims_dcm_rtp_frame_loss_threshold_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold information required in Handover media report.

 */
typedef struct {

  uint32_t rtcp_jitter_ul;
  /**<   Current RTCP jitter buffer value for uplink */

  uint32_t rtcp_jitter_dl;
  /**<   Current RTCP jitter buffer value for downlink */

  uint32_t rtp_frame_loss_ul;
  /**<   Current RTP frame loss value for uplink */

  uint32_t rtp_frame_loss_dl;
  /**<   Current RTP frame loss value for downlink */
}ims_dcm_handover_media_report_v01;  /* Type */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_aggregates
    @{
  */
/**  Structure to hold information required for wifi Timezone data.

 */
typedef struct {

  uint16_t dcm_tm_sec;
  /**<     seconds value */

  uint16_t dcm_tm_min;
  /**<     minutes value */

  uint16_t dcm_tm_hour;
  /**<     hours value */

  uint16_t dcm_tm_mday;
  /**<     day of month */

  uint16_t dcm_tm_mon;
  /**<     month value */

  uint16_t dcm_tm_year;
  /**<     years value */

  uint16_t dcm_tm_wday;
  /**<     day of the week */

  int16_t dcm_tm_tzone;
  /**<     timezone value */

  uint64_t dcm_epoch_time;
  /**<     epoch value */
}ims_dcm_tz_info_v01;  /* Type */
/**
    @}
  */

/**  Bitmask values for OMADM services.
  */
typedef uint64_t ims_dcm_services_type_mask_v01;
#define IMS_DCM_SERVICES_BIT_MASK_NONE_V01 ((ims_dcm_services_type_mask_v01)0x0ull) /**<  NO service enable  */
#define IMS_DCM_SERVICES_CM_ENABLE_BIT_MASK_V01 ((ims_dcm_services_type_mask_v01)0x1ull) /**<  CM Service enable  */
#define IMS_DCM_SERVICES_UCE_ENABLE_BIT_MASK_V01 ((ims_dcm_services_type_mask_v01)0x2ull) /**<  UCE service enable  */
/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Brings up the PDN. */
typedef struct {

  /* Mandatory */
  /*  PDP Information */
  ims_dcm_pdp_information_v01 pdp_info;

  /* Optional */
  /*  PDP Request Sequence Number */
  uint8_t pdp_req_seq_no_valid;  /**< Must be set to true if pdp_req_seq_no is being passed */
  uint32_t pdp_req_seq_no;
  /**<   Cookie to help the client uniquely identify this specific PDP request.
  */

  /* Optional */
  /*  Subscription ID */
  uint8_t pdp_subs_id_valid;  /**< Must be set to true if pdp_subs_id is being passed */
  uint32_t pdp_subs_id;
  /**<   Subscription ID for identifying LTE slot in case of DS-DS.
       This value will be passed to data layer when activating PDP.
  */

  /* Optional */
  /*  SIM Slot ID */
  uint8_t pdp_slot_id_valid;  /**< Must be set to true if pdp_slot_id is being passed */
  uint32_t pdp_slot_id;
  /**<   SIM Slot ID for identifying SIM slot in case of DS-DS.
       This value will be passed to data CNE for the SLOT info for DSDS feature set.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<  INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2 
 */
}ims_dcm_pdp_activate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Brings up the PDN. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            Only if processing was successful, will an
                            indication be sent which will hold information
                            about the result of processing.
  */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID to uniquely identify the interface. 0 indicates a failure
  */

  /* Optional */
  /*  PDP Request Sequence Number */
  uint8_t pdp_req_seq_no_valid;  /**< Must be set to true if pdp_req_seq_no is being passed */
  uint32_t pdp_req_seq_no;
  /**<   Cookie to help the client uniquely identify this specific PDP request.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2 
 */
}ims_dcm_pdp_activate_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Informs the client about the IP address obtained as a result of 
	PDP activation. */
typedef struct {

  /* Mandatory */
  /*  PDP Error */
  qmi_response_type_v01 pdp_error;
  /**<   PDP Error Information
       Indicates the reason for the PDP activation failure.
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type -- Error code. Possible error code values are
      - QMI_ERR_NONE -- Success case
      - QMI_ERR_INVALID_PROFILE -- Invalid ds profile specified \n
      - QMI_ERR_INVALID_PDP_TYPE -- Unsupported PDP type specified \n
  */

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID to uniquely identify the interface. 0 indicates a failure. More details
       about the reason for failure can be obtained from the pdp_error field.
  */

  /* Optional */
  /*  PDP Request Sequence Number */
  uint8_t pdp_req_seq_no_valid;  /**< Must be set to true if pdp_req_seq_no is being passed */
  uint32_t pdp_req_seq_no;
  /**<   Cookie to help the client uniquely identify this specific PDP request.
  */

  /* Optional */
  /*  Address Information */
  uint8_t addr_info_valid;  /**< Must be set to true if addr_info is being passed */
  imsdcm_addr_type_v01 addr_info;
  /**<   \n
    IP address obtained as a result of PDN bring-up.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_activate_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Request to log message */
typedef struct {

  /* Optional */
  /*  Log Message Request */
  uint8_t ims_dcm_log_msg_valid;  /**< Must be set to true if ims_dcm_log_msg is being passed */
  uint32_t ims_dcm_log_msg_len;  /**< Must be set to # of elements in ims_dcm_log_msg */
  char ims_dcm_log_msg[IMS_DCM_LOG_STRING_LEN_MAX_V01];
  /**<   Log String.\n
     Values:\n
	   String message to log to the APP side
    */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance forthe request. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_log_request_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Request to log message */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            Only if processing was successful, will an
                            indication be sent which will hold information
                            about the result of processing.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance forthe request.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_log_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Request to register for HLOS events */
typedef struct {

  /* Optional */
  /*  Bluetooth Event */
  uint8_t ims_dcm_bluetooth_event_valid;  /**< Must be set to true if ims_dcm_bluetooth_event is being passed */
  uint8_t ims_dcm_bluetooth_event;
  /**<   Bluetooth quality information.\n
     Values:\n
       - TRUE  -- Modem is interested in receiving Bluetooth event \n
       - FALSE -- Modem is not interested in Bluetooth event
	   By default the value is set to False, indicating that modem is not interested in getting Bluetooth events
    */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance forthe request.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_register_app_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Request to register for HLOS events */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            Only if processing was successful, will an
                            indication be sent which will hold information
                            about the result of processing.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance forthe request.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_register_app_state_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Indication with the event status from app. */
typedef struct {

  /* Optional */
  /*  BT Quality  */
  uint8_t bt_state_valid;  /**< Must be set to true if bt_state is being passed */
  ims_dcm_bt_state_v01 bt_state;
  /**<   BT state information. Indicates the BT state. Values: \n
      - IMS_DCM_BT_CRITERIA_MET (0) --  Where Bluetooth quality criteria is MET \n 
      - IMS_DCM_BT_CRITERIA_NOT_MET (1) --  Where Bluetooth quality criteria is NOT_MET \n 
      - IMS_DCM_BT_FAILURE (2) --  Where IMS failed to communicate with BT interface 
 */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance forthe request.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_app_state_ind_bt_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Indication with the logging event status from app. */
typedef struct {

  /* Optional */
  /*  Logging Supported by App or Not  */
  uint8_t ims_dcm_logging_event_supported_valid;  /**< Must be set to true if ims_dcm_logging_event_supported is being passed */
  uint8_t ims_dcm_logging_event_supported;
  /**<   Logging Supported by App or Not. Indicates the Logging Support on App. Values: \n
                  @ENUM()
				  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for the request. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_app_state_ind_logging_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Brings down the previously activated PDP. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_deactivate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Brings down the previously activated PDP. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_deactivate_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Obtains the IP address associated
             with the specified PDP ID. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_get_ip_address_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Obtains the IP address associated
             with the specified PDP ID. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition.
   */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP.Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_get_ip_address_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Informs the client about the IP address obtained as a result of the query. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */

  /* Optional */
  /*  Address Information */
  uint8_t addr_info_valid;  /**< Must be set to true if addr_info is being passed */
  imsdcm_addr_type_v01 addr_info;
  /**<   \n
    IP address obtained as a result of PDN bring-up.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_pdp_get_ip_address_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Sets the modem local link address. */
typedef struct {

  /* Mandatory */
  /*  PDP Information */
  imsdcm_link_addr_type_v01 link_local_ip_addr;
  /**<   \n
     Request to update the link local IP.
  */
}ims_dcm_modem_link_add_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Sets the modem local link address. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            If processing was successful, an
                            indication is sent that holds information
                            about the processing result.
  */
}ims_dcm_modem_link_add_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Informs the client about the change in IP address. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */

  /* Optional */
  /*  Address Information */
  uint8_t addr_info_valid;  /**< Must be set to true if addr_info is being passed */
  imsdcm_addr_type_v01 addr_info;
  /**<   \n
  IP address obtained as a result of PDN bring-up.
  */

  /* Optional */
  /*  INSTANCE ID */
  uint8_t eInstance_id_valid;  /**< Must be set to true if eInstance_id is being passed */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_address_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Starts WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */

  /* Mandatory */
  /*  RSSI Threshold Value  */
  uint32_t rssi_threshold_add;
  /**<   RSSI threshold value to be compared while 
       offloading from LTE to WiFi (units in dbm) */

  /* Mandatory */
  /*  Max RSSI Tolerance Value  */
  uint32_t rssi_threshold_drop;
  /**<   Max RSSI tolerance value to be used to decide 
       the timer value (close/far) when average is \n
       - Gaining towards iRSSIThresholdAdd OR \n
       - Falling towards iRSSIThresholdDrop (units in dbm)*/

  /* Mandatory */
  /*  RSSI Threshold Close Value  */
  uint32_t rssi_threshold_close;
  /**<   RSSI threshold close value to be compared with the difference 
       between average RSSI (iRSSIThresholdAdd/iRSSIThresholdDrop) 
	   (units in dbm) to choose the right timer   */

  /* Mandatory */
  /*  RSSI Timer Close Value  */
  uint32_t rssi_timer_close;
  /**<   Timer value to be set when average RSSI 
       value is far from iRSSIThresholdAdd/iRSSIThresholdDrop (units in seconds)   	 */

  /* Mandatory */
  /*  RSSI Timer Far Value  */
  uint32_t rssi_timer_far;
  /**<   Timer value to be set when average RSSI \n
       is above iRSSIThresholdAdd (next scan cycle units in seconds)   	 */

  /* Mandatory */
  /*  RSSI Average Timer on Attach Value  */
  uint32_t rssi_avg_timer_on_attach;
  /**<   Timer value to be set when average RSSI 
       is above iRSSIThresholdAdd (next scan cycle units in seconds)   */

  /* Mandatory */
  /*  RSSI Average Timer on Camp Value  */
  uint32_t rssi_avg_timer_on_camp;
  /**<   Timer value to be set when average RSSI 
       is above iRSSIThresholdAdd (next scan cycle units in seconds)   */

  /* Mandatory */
  /*  RSSI Sampling Timer Value  */
  uint32_t rssi_sampling_timer;
  /**<   Timer value to be set when average RSSI \n
       is above iRSSIThresholdAdd (next scan cycle units in seconds)   */

  /* Mandatory */
  /*  RSSI Average Internal Value  */
  uint32_t rssi_average_interval;
  /**<   Timer value to be set when average RSSI 
       is above iRSSIThresholdAdd (next scan cycle units in seconds)   */

  /* Mandatory */
  /*  Current Attach Status */
  ims_dcm_wifi_attach_status_v01 curattachstatus;
  /**<   Current attach status. Values: \n 
      - IMS_DCM_WIFI_STATE_ATTACH_TO_WIFI (0) --  IMS attached to WiFi \n 
      - IMS_DCM_WIFI_STATE_DETACH_FROM_WIFI (1) --  IMS not attached to WiFi 
 */
}ims_dcm_get_wifi_quality_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Starts WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            Only if processing was successful, will an
                            indication be sent which will hold information
                            about the result of processing.
  */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */
}ims_dcm_get_wifi_quality_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Informs the client about the WiFi quality report. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID to uniquely identify the interface. 0 indicates a failure. More details
       about the reason for failure can be obtained from the pdp_error field.
  */

  /* Mandatory */
  /*  WiFi Quality Report */
  ims_dcm_wifi_quality_v01 qualityreport;
  /**<   WiFi quality information. Indicates the quality report. Values: \n
      - IMS_DCM_WIFI_QUALITY_GOOD (0) --  Where RSSI value is beyond the threshold \n 
      - IMS_DCM_WIFI_QUALITY_BAD (1) --  Where RSSI value is below the threshold \n 
      - IMS_DCM_WIFI_IFACE_FAILURE (2) --  Where IMS failed to communicate with WiFi interface 
*/
}ims_dcm_get_wifi_quality_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Stops WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */
}ims_dcm_stop_wifi_quality_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Stops WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition.
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_stop_wifi_quality_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Updates WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Mandatory */
  /*  PDP ID */
  ims_dcm_wifi_attach_status_v01 attachstatus;
  /**<   Attach status. Values: \n 
      - IMS_DCM_WIFI_STATE_ATTACH_TO_WIFI (0) --  IMS attached to WiFi \n 
      - IMS_DCM_WIFI_STATE_DETACH_FROM_WIFI (1) --  IMS not attached to WiFi 
 */
}ims_dcm_update_wifi_quality_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Updates WiFi quality measurements. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_update_wifi_quality_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Gets Handover metrics. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */

  /* Mandatory */
  /*  Sample Timer Value */
  uint32_t sampling_timer_value;
  /**<   Sampling timer is the interval of time to collect the next sample (rssi and media) in milliseconds. */

  /* Mandatory */
  /*  Monitor Timer Value */
  uint32_t monitor_timer_value;
  /**<   Monitor timer is used to monitor rssi and media metrics to collect enough samples before applying handover algorithm (in seconds). */

  /* Mandatory */
  /*  RSSI Threshold High */
  uint32_t rssi_threshold_high;
  /**<   Threshold value high for RSSI */

  /* Mandatory */
  /*  RSSI Threshold Low  */
  uint32_t rssi_threshold_low;
  /**<   Threshold value low for RSSI   	 */

  /* Optional */
  /*  Source RAT Type */
  uint8_t source_rat_type_valid;  /**< Must be set to true if source_rat_type is being passed */
  ims_dcm_rat_type_v01 source_rat_type;
  /**<   Source RAT type. When the init request message sent to service 
  at that time, Source RAT may not be available. If it is not set in this 
  request, it will be mandatorily set in the start request message; the value is 
  essential for calculation after the start request is sent. */

  /* Optional */
  /*  Target RAT Type */
  uint8_t target_rat_type_valid;  /**< Must be set to true if target_rat_type is being passed */
  ims_dcm_rat_type_v01 target_rat_type;
  /**<   Target RAT type. When the init request message is sent to the service, at 
  that time the Target RAT may not be available. If it is not set in this request 
  it will be mandatorily set in the start request message; the value is essential for 
  calculation after the start request is sent. */

  /* Optional */
  /*  RTCP Jitter Threshold */
  uint8_t rtcp_jitter_threshold_valid;  /**< Must be set to true if rtcp_jitter_threshold is being passed */
  ims_dcm_rtcp_jitter_threshold_v01 rtcp_jitter_threshold;
  /**<   RCTP jitter threshold values to be used in algorithm */

  /* Optional */
  /*  Rtp Frame Loss Threshold */
  uint8_t rtp_frame_loss_threshold_valid;  /**< Must be set to true if rtp_frame_loss_threshold is being passed */
  ims_dcm_rtp_frame_loss_threshold_v01 rtp_frame_loss_threshold;
  /**<   RTP frame loss threshold values to be used in algorithm */

  /* Optional */
  /*  RSSI Threshold Re-Point or Medium Value */
  uint8_t rssi_threshold_repoint_valid;  /**< Must be set to true if rssi_threshold_repoint is being passed */
  uint32_t rssi_threshold_repoint;
  /**<   Threshold value repoint for RSSI */

  /* Optional */
  /*  Threshold value to monitor call quality */
  uint8_t rssi_threshold_call_quality_valid;  /**< Must be set to true if rssi_threshold_call_quality is being passed */
  uint32_t rssi_threshold_call_quality;
  /**<   Threshold value to monitor call quality */
}ims_dcm_get_ho_measurement_init_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Gets Handover metrics. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       - qmi_error_type  -- Error code. Possible error code values are 
                            described in the error codes section of each 
                            message definition.
                            Only if processing was successful will an
                            indication be sent which will hold information
                            about the processing result.
  */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface.
  */
}ims_dcm_get_ho_measurement_init_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Starts Handover. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Mandatory */
  /*  Source RAT type */
  ims_dcm_rat_type_v01 source_rat_type;
  /**<   Source RAT type */

  /* Mandatory */
  /*  Target RAT type */
  ims_dcm_rat_type_v01 target_rat_type;
  /**<   Target RAT type */

  /* Mandatory */
  /*  Module Name */
  ims_dcm_ho_measurement_module_v01 module_name;
  /**<   Module name on which measurement is to be started. */
}ims_dcm_ho_measurement_start_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Starts Handover. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_ho_measurement_start_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Stops Handover. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Mandatory */
  /*  Module Name */
  ims_dcm_ho_measurement_module_v01 module_name;
  /**<   Module name on which measurement is to be stopped. */
}ims_dcm_ho_measurement_stop_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Stops Handover. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_ho_measurement_stop_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Indication Message; Informs the client about the change in IP address. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies the interface. */

  /* Mandatory */
  /*  Module Name */
  ims_dcm_ho_measurement_module_v01 module_name;
  /**<   Module name for the report that needs to be sent. */

  /* Mandatory */
  /*  Handover Report */
  ims_dcm_ho_measurement_report_v01 handover_report;
  /**<   WiFi quality information indicated by the quality report. Values: \n
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_GOOD (0) --  Where RSSI and media values are beyond the threshold \n 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM (1) --  Where RSSI and media values are between high and low 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_BAD (2) --  Where RSSI and media values are below the threshold 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_FAILURE (3) --  When there is a failure in WLAN driver/supplicant 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM_HIGH (4) --  Where RSSI and media value is high 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_MEDIUM_LOW (5) --  Where RSSI and media value is low 
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_VOICE_CALL_MET (6) --    Where RSSI value is greated than voice call threshold  
      - IMS_DCM_HO_MEASUREMENT_REPORT_WIFI_VOICE_CALL_NOT_MET (7) --    WWhere RSSI value is less than voice call threshold */

  /* Optional */
  /*  Current RSSI Value */
  uint8_t current_rssi_value_valid;  /**< Must be set to true if current_rssi_value is being passed */
  uint32_t current_rssi_value;
  /**<   Current average RSSI value */

  /* Optional */
  /*  Handover Media Report */
  uint8_t handover_media_report_valid;  /**< Must be set to true if handover_media_report is being passed */
  ims_dcm_handover_media_report_v01 handover_media_report;
  /**<   Current media related metrics.  */

  /* Optional */
  /*  Handover Media Report */
  uint8_t voice_call_report_valid;  /**< Must be set to true if voice_call_report is being passed */
  ims_dcm_ho_measurement_report_v01 voice_call_report;
  /**<   Current media related metrics.  */
}ims_dcm_ho_measurement_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Updates Handover status. */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Mandatory */
  /*  Source RAT type */
  ims_dcm_rat_type_v01 source_rat_type;
  /**<   Source RAT type */

  /* Mandatory */
  /*  Target RAT type */
  ims_dcm_rat_type_v01 target_rat_type;
  /**<   Target RAT type */
}ims_dcm_ho_measurement_status_update_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Updates Handover status. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition.
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_ho_measurement_status_update_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Uninitializes handover */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */
}ims_dcm_ho_measurement_uninit_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Uninitializes handover */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Optional */
  /*  PDP ID */
  uint8_t pdp_id_valid;  /**< Must be set to true if pdp_id is being passed */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */
}ims_dcm_ho_measurement_uninit_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Timezone information */
typedef struct {

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP request. */

  /* Mandatory */
  /*  Request sequence number */
  uint32_t req_seq_no;
  /**<   Cookie to help the client uniquely identify this specific request. */
}ims_dcm_wlan_tz_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Timezone information */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
      - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
      - qmi_error_type  -- Error code. Possible error code values are 
                           described in the error codes section of each 
                           message definition
    */

  /* Mandatory */
  /*  PDP ID */
  uint8_t pdp_id;
  /**<   PDP ID that uniquely identifies this specific PDP. */

  /* Mandatory */
  /*  Request sequence number */
  uint32_t req_seq_no;
  /**<   Cookie to help the client uniquely identify this specific request. */

  /* Mandatory */
  /*  WLAN timezone information */
  ims_dcm_tz_info_v01 tz_info;
  /**<     Wlan timestamp along with UTC timezone value. */
}ims_dcm_wlan_tz_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Destroys the instance. */
typedef struct {

  /* Mandatory */
  /*  INSTANCE ID */
  ims_instance_id_enum_v01 eInstance_id;
  /**<   INSTANCE ID that identifies the instance for this specific PDP. Values:
      - IMS_INST_ID_NONE (-1) --  No instanceID 
      - IMS_INST_ID_GLOBAL (0) --  Instance ID Global 
      - IMS_INST_ID_1 (1) --  Instance ID 1 
      - IMS_INST_ID_2 (2) --  Instance ID 2  */
}ims_dcm_sub_destroy_instance_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Destroys the instance. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
  - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
  - qmi_error_type  -- Error code. Possible error code values are
                                   described in the error codes section of each
                                   message definition.
  If processing was successful, an indication is sent that holds information
  about the processing result.
  */
}ims_dcm_sub_destroy_instance_rsp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Request Message; Destroys the instance. */
typedef struct {

  /* Mandatory */
  /*  Service mask */
  ims_dcm_services_type_mask_v01 services_type_mask;
  /**<   Service mask is sent. */
}ims_dcm_service_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup imsdcm_qmi_messages
    @{
  */
/** Response Message; Destroys the instance. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
  - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
  - qmi_error_type  -- Error code. Possible error code values are
                                   described in the error codes section of each
                                   message definition.
  If processing was successful, an indication is sent that holds information
  about the processing result.
  */
}ims_dcm_service_enable_rsp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IMS_DCM_ADDRESS_CHANGE_IND_V01 
//#define REMOVE_QMI_IMS_DCM_APP_STATE_BLUETOOTH_IND_V01 
//#define REMOVE_QMI_IMS_DCM_APP_STATE_LOGGING_IND_V01 
//#define REMOVE_QMI_IMS_DCM_GET_IP_ADDRESS_V01 
//#define REMOVE_QMI_IMS_DCM_GET_IP_ADDRESS_IND_V01 
//#define REMOVE_QMI_IMS_DCM_GET_WIFI_QUALITY_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_INIT_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_REPORT_IND_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_START_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_STATUS_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_STOP_V01 
//#define REMOVE_QMI_IMS_DCM_HO_MEASUREMENT_UNINIT_V01 
//#define REMOVE_QMI_IMS_DCM_LOG_V01 
//#define REMOVE_QMI_IMS_DCM_MODEM_LINK_ADDR_V01 
//#define REMOVE_QMI_IMS_DCM_PDP_ACTIVATE_V01 
//#define REMOVE_QMI_IMS_DCM_PDP_ACTIVATE_IND_V01 
//#define REMOVE_QMI_IMS_DCM_PDP_DEACTIVATE_V01 
//#define REMOVE_QMI_IMS_DCM_SERVICE_ENABLE_STATUS_V01 
//#define REMOVE_QMI_IMS_DCM_STOP_WIFI_QUALITY_V01 
//#define REMOVE_QMI_IMS_DCM_SUB_DESTROY_INSTANCE_V01 
//#define REMOVE_QMI_IMS_DCM_UPDATE_WIFI_QUALITY_V01 
//#define REMOVE_QMI_IMS_DCM_WIFI_QUALITY_IND_V01 
//#define REMOVE_QMI_IMS_DCM_WLAN_TZ_V01 
//#define REMOVE_QMI_IMS_REGISTER_DCM_APP_STATE_V01 

/*Service Message Definition*/
/** @addtogroup imsdcm_qmi_msg_ids
    @{
  */
#define QMI_IMS_DCM_PDP_ACTIVATE_REQ_V01 0x0020
#define QMI_IMS_DCM_PDP_ACTIVATE_RSP_V01 0x0020
#define QMI_IMS_DCM_PDP_ACTIVATE_IND_V01 0x0020
#define QMI_IMS_DCM_PDP_DEACTIVATE_REQ_V01 0x0021
#define QMI_IMS_DCM_PDP_DEACTIVATE_RSP_V01 0x0021
#define QMI_IMS_DCM_GET_IP_ADDRESS_REQ_V01 0x0022
#define QMI_IMS_DCM_GET_IP_ADDRESS_RSP_V01 0x0022
#define QMI_IMS_DCM_GET_IP_ADDRESS_IND_V01 0x0022
#define QMI_IMS_DCM_LINK_ADDR_REQ_V01 0x0023
#define QMI_IMS_DCM_LINK_ADDR_RSP_V01 0x0023
#define QMI_IMS_DCM_ADDRESS_CHANGE_IND_V01 0x0024
#define QMI_IMS_DCM_GET_WIFI_QUALITY_REQ_V01 0x0025
#define QMI_IMS_DCM_GET_WIFI_QUALITY_RSP_V01 0x0025
#define QMI_IMS_DCM_WIFI_QUALITY_IND_V01 0x0025
#define QMI_IMS_DCM_STOP_WIFI_QUALITY_REQ_V01 0x0026
#define QMI_IMS_DCM_STOP_WIFI_QUALITY_RSP_V01 0x0026
#define QMI_IMS_DCM_UPDATE_WIFI_QUALITY_REQ_V01 0x0027
#define QMI_IMS_DCM_UPDATE_WIFI_QUALITY_RSP_V01 0x0027
#define QMI_IMS_DCM_HO_MEASUREMENT_INIT_REQ_V01 0x0028
#define QMI_IMS_DCM_HO_MEASUREMENT_INIT_RSP_V01 0x0028
#define QMI_IMS_DCM_HO_MEASUREMENT_START_REQ_V01 0x0029
#define QMI_IMS_DCM_HO_MEASUREMENT_START_RSP_V01 0x0029
#define QMI_IMS_DCM_HO_MEASUREMENT_STOP_REQ_V01 0x002A
#define QMI_IMS_DCM_HO_MEASUREMENT_STOP_RSP_V01 0x002A
#define QMI_IMS_DCM_HO_MEASUREMENT_REPORT_IND_V01 0x002B
#define QMI_IMS_DCM_HO_MEASUREMENT_STATUS_REQ_V01 0x002C
#define QMI_IMS_DCM_HO_MEASUREMENT_STATUS_RSP_V01 0x002C
#define QMI_IMS_DCM_HO_MEASUREMENT_UNINIT_REQ_V01 0x002D
#define QMI_IMS_DCM_HO_MEASUREMENT_UNINIT_RSP_V01 0x002D
#define QMI_IMS_DCM_REGISTER_APP_STATE_REQ_V01 0x002E
#define QMI_IMS_DCM_REGISTER_APP_STATE_RSP_V01 0x002E
#define QMI_IMS_DCM_APP_STATE_BT_IND_V01 0x002F
#define QMI_IMS_DCM_APP_STATE_LOGGING_IND_V01 0x0030
#define QMI_IMS_DCM_LOG_REQ_V01 0x0031
#define QMI_IMS_DCM_LOG_RSP_V01 0x0031
#define QMI_IMS_DCM_WLAN_TZ_REQ_V01 0x0032
#define QMI_IMS_DCM_WLAN_TZ_RSP_V01 0x0032
#define QMI_IMS_DCM_SUB_DESTROY_INSTANCE_REQ_V01 0x0033
#define QMI_IMS_DCM_SUB_DESTROY_INSTANCE_RSP_V01 0x0033
#define QMI_IMS_DCM_SERVICE_ENABLE_STATUS_REQ_V01 0x0034
#define QMI_IMS_DCM_SERVICE_ENABLE_STATUS_RSP_V01 0x0034
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro imsdcm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type imsdcm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define imsdcm_get_service_object_v01( ) \
          imsdcm_get_service_object_internal_v01( \
            IMSDCM_V01_IDL_MAJOR_VERS, IMSDCM_V01_IDL_MINOR_VERS, \
            IMSDCM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

