#ifndef QOS_SERVICE_01_H
#define QOS_SERVICE_01_H
/**
  @file quality_of_service_v01.h

  @brief This is the public header file which defines the qos service Data structures.

  This header file defines the types and structures that were defined in
  qos. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.5
   It was generated on: Tue May 12 2015 (Spin 0)
   From IDL File: quality_of_service_v01.idl */

/** @defgroup qos_qmi_consts Constant values defined in the IDL */
/** @defgroup qos_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qos_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qos_qmi_messages Structures sent as QMI messages */
/** @defgroup qos_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qos_qmi_accessor Accessor for QMI service object */
/** @defgroup qos_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "data_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qos_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QOS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QOS_V01_IDL_MINOR_VERS 0x0C
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QOS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QOS_V01_MAX_MESSAGE_ID 0xFFFE
/**
    @}
  */


/** @addtogroup qos_qmi_consts
    @{
  */
#define QMI_QOS_MAX_QOS_SPECS_V01 10
#define QMI_QOS_MAX_FLOW_BIT_MASK_V01 5
#define QMI_QOS_IPV6_ADDR_LEN_V01 16
#define QMI_QOS_MAX_QOS_FLOWS_V01 8
#define QMI_QOS_MAX_QOS_FILTERS_V01 16
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Resets the QoS state variables of the requesting
           control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qos_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Resets the QoS state variables of the requesting
           control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qos_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_TECH_PREF_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_TECH_PREF_CDMA_V01 = -32767,
  QOS_TECH_PREF_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_tech_pref_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t network_supported_qos_profile_change_reporting;
  /**<   Report network supported profile change. Values: \n
       - 0 -- Do not report \n
       - 1 -- Report on change in power state
  */

  qos_tech_pref_enum_v01 ext_technology_preference;
  /**<   Technology preference to be used while registering for
         network-supported QoS. Values: \n
       - -32767 -- CDMA \n
  */
}qos_report_nw_supp_prof_ch_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Register for specific events. */
typedef struct {

  /* Optional */
  /*  Global Flow Report */
  uint8_t global_flow_reporting_valid;  /**< Must be set to true if global_flow_reporting is being passed */
  uint8_t global_flow_reporting;
  /**<   Global Flow Reporting. Values: \n
       - 0 -- Do not report \n
       - 1 -- Report on change in power state
     */

  /* Optional */
  /*  Network Supported QoS Profile */
  uint8_t nw_supp_qos_prof_valid;  /**< Must be set to true if nw_supp_qos_prof is being passed */
  qos_report_nw_supp_prof_ch_type_v01 nw_supp_qos_prof;
  /**<   Network Supported QoS Profile Change
         Values: \n
       - 0 -- Do not report \n
       - 1 -- Report on change in power state
    */
}qos_set_event_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Register for specific events. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_set_event_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_tech_pref_enum_v01 tech_pref;
  /**<   Technology preference to be used while registering for
         network-supported QoS. Values: \n
       - -32767 -- CDMA \n
    */

  uint8_t profile_count;
  /**<   Profile count – Number of profile values that will
        follow. If the count = 0, no additional data follows
    */

  uint16_t profile_value;
  /**<   Profile value – This field will repeat “count”
          number of times
    */
}qos_nw_supp_qos_prof_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Indication Message; Register for specific events. */
typedef struct {

  /* Optional */
  /*  Network Supported QoS Profiles */
  uint8_t nw_supp_qos_prof_ind_valid;  /**< Must be set to true if nw_supp_qos_prof_ind is being passed */
  qos_nw_supp_qos_prof_type_v01 nw_supp_qos_prof_ind;
}qos_event_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Create QoS flows. */
typedef struct {

  /* Optional */
  /*  QoS Request Opcode */
  uint8_t qos_req_opcode_valid;  /**< Must be set to true if qos_req_opcode is being passed */
  uint8_t qos_req_opcode;
  /**<   QoS request opcode. values:\n
         0x01 – QoS REQUEST\n
         0x02 – QoS CONFIGUR\n
    */
}qos_request_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Create QoS flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Mandatory */
  /*  QoS Identity List */
  uint32_t qos_identifier_len;  /**< Must be set to # of elements in qos_identifier */
  uint32_t qos_identifier[QMI_QOS_MAX_QOS_SPECS_V01];
  /**<   List of QoS flows to be suspended: Index identifying the QoS flow that has
       been negotiated
  */
}qos_request_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Releases flows. */
typedef struct {

  /* Mandatory */
  /*  QoS Identity List */
  uint32_t qos_identifier_len;  /**< Must be set to # of elements in qos_identifier */
  uint32_t qos_identifier[QMI_QOS_MAX_QOS_SPECS_V01];
}qos_release_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Releases flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_release_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Suspends one or more existing QoS flows. */
typedef struct {

  /* Mandatory */
  /*  QoS Identity List */
  uint32_t qos_identifier_len;  /**< Must be set to # of elements in qos_identifier */
  uint32_t qos_identifier[QMI_QOS_MAX_QOS_SPECS_V01];
}qos_suspend_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Suspends one or more existing QoS flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_suspend_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Resume flows. */
typedef struct {

  /* Mandatory */
  /*  QoS Identity List */
  uint32_t qos_identifier_len;  /**< Must be set to # of elements in qos_identifier */
  uint32_t qos_identifier[QMI_QOS_MAX_QOS_SPECS_V01];
}qos_resume_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Resume flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_resume_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; modify qos flows. */
typedef struct {

  /* Optional */
  /*  Primary QoS Modify Request */
  uint8_t request_primary_modify_valid;  /**< Must be set to true if request_primary_modify is being passed */
  uint8_t request_primary_modify;
  /**<   Primary QoS Modify_Request. Values\n
    0x01 – TRUE
  */
}qos_modify_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; modify qos flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_modify_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Queries QOS parameters. */
typedef struct {

  /* Mandatory */
  /*  QoS Identifier Negotiated */
  uint32_t qos_id;
  /**<   Index identifying the QoS flow that has been negotiated.
    */

  /* Optional */
  /*  Primary Granted QoS Request */
  uint8_t request_primary_valid;  /**< Must be set to true if request_primary is being passed */
  uint8_t request_primary;
  /**<   Values\n
       0x01 – TRUE*/
}qos_get_granted_qos_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Queries QOS parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Extended Error Code */
  uint8_t extended_error_code_valid;  /**< Must be set to true if extended_error_code is being passed */
  uint16_t extended_error_code;
}qos_get_granted_qos_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Gets the status of a QoS flow. */
typedef struct {

  /* Mandatory */
  /*  QoS Identity */
  uint32_t qos_id;
  /**<   Index identifying the QoS flow that has been negotiated.
  */
}qos_get_qos_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_STATUS_DEFAULT_V01 = 0x00, /**<  Default status  */
  QOS_STATUS_ACTIVATED_V01 = 0x01, /**<  Activated status  */
  QOS_STATUS_SUSPENDED_V01 = 0x02, /**<  Suspended status  */
  QOS_STATUS_GONE_V01 = 0x03, /**<  Gone status  */
  QOS_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_status_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Gets the status of a QoS flow. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Mandatory */
  /*  QoS Status */
  qos_status_enum_v01 qos_status;
  /**<   Current QoS instance status.    Values: \n
       0x01 - QOS_STATUS_ACTIVATED
       0x02 - QOS_STATUS_SUSPENDED
       0x03 - QOS_STATUS_GONE
   */
}qos_get_qos_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t qos_id;
  /**<   Index identifying the QoS flow that has been negotiated.
     */

  uint8_t qos_status;
  /**<   Current QoS flow status. Values\n
       0x01 – QMI_QOS_STATUS_ACTIVATED\n
       0x02 – QMI_QOS_STATUS_SUSPENDED\n
       0x03 – QMI_QOS_STATUS_GONE
  */

  uint8_t qos_event;
  /**<   QoS event. Values:\n
       0x01 – QMI_QOS_ACTIVATED_EV\n
       0x02 – QMI_QOS_SUSPENDED_EV\n
       0x03 – QMI_QOS_GONE_EV\n
       0x04 – QMI_QOS_MODIFY_ACCEPTED_EV\n
       0x05 – QMI_QOS_MODIFY_REJECTED_EV\n
       0x06 – QMI_QOS_INFO_CODE_UPDATED_EV
     */
}qos_status_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Indication Message; Gets the status of a QoS flow. */
typedef struct {

  /* Mandatory */
  /*  QoS Status Information */
  qos_status_info_type_v01 qos_status_info;

  /* Optional */
  /*  Reason for Failure */
  uint8_t reason_code_valid;  /**< Must be set to true if reason_code is being passed */
  uint8_t reason_code;
  /**<   Reason that the QoS state changed: Values:\n
   0x01 – QMI_QOS_INVALID_PARAMS\n
   0x02 – QMI_QOS_INTERNAL_CALL_ENDED\n
   0x03 – QMI_QOS_INTERNAL_ERROR\n
   0x04 – QMI_QOS_INSUFFICIENT_LOCAL_Resources\n
   0x05 – QMI_QOS_TIMED_OUT_OPERATION\n
   0x06 – QMI_QOS_INTERNAL_UNKNOWN_CAUSE_CODE\n
   0x07 – QMI_QOS_INTERNAL_MODIFY_IN_PROGRESS\n
   0x08 – QMI_QOS_NOT_SUPPORTED\n
   0x09 – QMI_QOS_NOT_AVAILABLE\n
   0x0A – QMI_QOS_NOT_GUARANTEED\n
   0x0B – QMI_QOS_INSUFFICIENT_NETWORK_RESOURCES\n
   0x0C – QMI_QOS_AWARE_SYSTEM\n
   0x0D – QMI_QOS_UNAWARE_SYSTEM\n
   0x0E – QOS_REJECTED_OPERATION\n
   0x0F – QMI_QOS_WILL_GRANT_WHEN_QOS_RESUMED\n
   0x10 – QMI_QOS_NETWORK_CALL_ENDED\n
   0x11 – QMI_QOS_NETWORK_SERVICE_NOT_AVAILABLE\n
   0x12 – QMI_QOS_NETWORK_L2_LINK_RELEASED\n
   0x13 – QMI_QOS_NETWORK_L2_LINK_REESTAB_REJ\n
   0x14 – QMI_QOS_NETWORK_L2_LINK_REESTAB_IND\n
   0x15 – QMI_QOS_NETWORK_UNKNOWN_CAUSE_CODE\n
   0x16 – QMI_NETWORK_BUSY
     */
}qos_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Get network qos status. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qos_get_qos_network_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Get network qos status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Mandatory */
  /*  Network QoS Support Status */
  uint8_t network_qos_support_status;
  /**<   Network QoS Support Status.    Values: \n
	0 - No QoS support in the network
	1 - Network supports QoS
  */
}qos_get_qos_network_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Indication Message; Get network qos status. */
typedef struct {

  /* Mandatory */
  /*  Network QoS Support Status */
  uint8_t network_qos_support_status;
  /**<   Network QoS Support Status.    Values: \n
          0x00 – Current network does not support QoS
          0x01 – Current network supports QoS
    */
}qos_network_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Allows the control point to get network supported QoS profile information */
typedef struct {

  /* Optional */
  /*  Extended Technology Preference */
  uint8_t ext_technology_preference_valid;  /**< Must be set to true if ext_technology_preference is being passed */
  qos_tech_pref_enum_v01 ext_technology_preference;
  /**<   Technology preference to be used while registering for
         network-supported QoS. Values: \n
       - -32767 -- CDMA \n
  */
}qos_get_nw_supported_qos_profiles_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Allows the control point to get network supported QoS profile information */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Mandatory */
  /*  Network Supported QoS Profiles */
  uint32_t nw_supp_qos_profiles_len;  /**< Must be set to # of elements in nw_supp_qos_profiles */
  qos_nw_supp_qos_prof_type_v01 nw_supp_qos_profiles[QMI_QOS_MAX_QOS_SPECS_V01];

  /* Optional */
  /*  Extended Error Info */
  uint8_t ext_error_info_valid;  /**< Must be set to true if ext_error_info is being passed */
  uint16_t ext_error_info;
  /**<   Data services error value returned by the IOCTL/lower layers.
     The possible values returned in this field are according to the spec Data Services API.
    */
}qos_get_nw_supported_qos_profiles_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Indication Message; Broadcasts indications specific to the primary flow. */
typedef struct {

  /* Mandatory */
  /*  QoS Primary Event  */
  uint16_t event;
  /**<   Event that causes the indication: values\n
            0x0001 – Primary flow QoS modify operation success\n
            0x0002 – Primary flow QoS modify operation failure
    */
}qos_primary_qos_event_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_IP_FAMILY_IPV4_V01 = 0x04,
  QOS_IP_FAMILY_IPV6_V01 = 0x06,
  QOS_IP_FAMILY_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_ip_family_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Releases flows. */
typedef struct {

  /* Mandatory */
  /*  IP Family Preference */
  qos_ip_family_enum_v01 ip_preference;
  /**<   Values: \n
         - 0x04 -- IPv4 \n
         - 0x06 -- IPv6
  */
}qos_set_client_ip_pref_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Releases flows. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_set_client_ip_pref_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Binds a control point to a muxed data port. */
typedef struct {

  /* Optional */
  /*  Peripheral End Point ID */
  uint8_t ep_id_valid;  /**< Must be set to true if ep_id is being passed */
  data_ep_id_type_v01 ep_id;
  /**<   The peripheral end point (physical data channel) to which
       the client binds.
  */

  /* Optional */
  /*  Mux ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;
  /**<   The mux id of the logical data channel to which
       the client binds (default value is 0).
  */

  /* Optional */
  /*  Data Port */
  uint8_t data_port_valid;  /**< Must be set to true if data_port is being passed */
  uint16_t data_port;
  /**<   SIO data port to which the client binds
  */
}qos_bind_data_port_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Binds a control point to a muxed data port. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qos_bind_data_port_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint32_t qos_req_oper_mask_v01;
#define QMI_QOS_MASK_STATS_GET_TX_Q_LVL_V01 ((qos_req_oper_mask_v01)0x00000001)
#define QMI_QOS_MASK_STATS_GET_RMAC3_INFO_V01 ((qos_req_oper_mask_v01)0x00000002)
#define QMI_QOS_MASK_STATS_GET_TX_STATUS_V01 ((qos_req_oper_mask_v01)0x00000004)
#define QMI_QOS_MASK_STATS_GET_INACTIVITY_TIMER_V01 ((qos_req_oper_mask_v01)0x00000008)
#define QMI_QOS_MASK_STATS_SET_INACTIVITY_TIMER_V01 ((qos_req_oper_mask_v01)0x00000010)
/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message;  */
typedef struct {

  /* Mandatory */
  /*  QoS Identifier Negotiated */
  uint32_t qos_id;

  /* Optional */
  /*  Requested Operation Mask */
  uint8_t requested_operation_valid;  /**< Must be set to true if requested_operation is being passed */
  qos_req_oper_mask_v01 requested_operation;
  /**<   Requested operation bit mask:
           Bit 0 – Get Tx queue level\n
           Bit 1 – Get HDR RMAC3 information\n
           Bit 2 – Get the transmission status\n
           Bit 3 – Get Inactivity timer\n
           Bit 4 – Set Inactivity timer\n
          Each bit set will cause the corresponding\n
          optional TLV to be sent in QMI_QOS_PERFORM_FLOW_OPERATION_RESP.
          All unlisted bits are reserved for future use and must be set to zero.
   */

  /* Optional */
  /*  Set Inactivity Timer */
  uint8_t inactivity_timer_valid;  /**< Must be set to true if inactivity_timer is being passed */
  uint32_t inactivity_timer;
  /**<   Inactivity timer TLV is needed if the requested operation bitmask has bit 4 set
   */

  /* Optional */
  /*  Requested Operation Mask */
  uint8_t is_primary_valid;  /**< Must be set to true if is_primary is being passed */
  uint8_t is_primary;
  /**<   Indicates whether the operation requested is for a default flow: values\n
      0x00 – FALSE\n
      0x01 - TRUE
   */
}qos_perform_flow_operation_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t current_new_data_cnt;
  /**<   Pending new data in the queue*/

  uint32_t wm_free_cnt;
  /**<   Empty space in the queue*/

  uint32_t total_pending_cnt;
  /**<   Total amount of data pending. New data + retransmission queue.*/
}qos_flow_oper_tx_q_level_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t ps_headroom_payload_size;
  /**<   PA headroom limited payload size in bytes.*/

  uint16_t bucket_level_payload_size;
  /**<   Bucket level equivalent payload size in bytes.*/

  uint16_t t2p_inflow_payload_size;
  /**<   Traffic-to-Pilot (T2P) inflow equivalent payload size in bytes. */
}qos_flow_oper_RMAC3_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bit_number;

  int16_t error_value;
}error_codes_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message;  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Tx Queue Level Information */
  uint8_t tx_q_info_valid;  /**< Must be set to true if tx_q_info is being passed */
  qos_flow_oper_tx_q_level_info_type_v01 tx_q_info;

  /* Optional */
  /*  HDR RMAC3 Information */
  uint8_t rmac3_info_valid;  /**< Must be set to true if rmac3_info is being passed */
  qos_flow_oper_RMAC3_info_type_v01 rmac3_info;

  /* Optional */
  /*  Tx Status */
  uint8_t tx_status_valid;  /**< Must be set to true if tx_status is being passed */
  uint8_t tx_status;
  /**<     Transmission Status: values \n
       0x01 – RLP packet dropped due to Stale Packet Dropping (SPD) or MARQ
       0x00 - RLP packet NOT dropped due to SPD or MARQ
   */

  /* Optional */
  /*  Inactivity Timer */
  uint8_t inactivity_timer_valid;  /**< Must be set to true if inactivity_timer is being passed */
  uint32_t inactivity_timer;
  /**<    Inactivity timer value returned when a Get operation is requested
   */

  /* Optional */
  /*  Operation Failure */
  uint8_t error_valid;  /**< Must be set to true if error is being passed */
  uint32_t error_len;  /**< Must be set to # of elements in error */
  error_codes_type_v01 error[QMI_QOS_MAX_FLOW_BIT_MASK_V01];
  /**<    Inactivity timer value returned when a Get operation is requested
   */
}qos_perform_flow_operation_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Queries filter parameters. */
typedef struct {

  /* Mandatory */
  /*  QoS Identifier Negotiated */
  uint32_t qos_id;
  /**<   Index identifying the QoS flow that has been negotiated.
   */
}qos_get_filter_params_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Queries filter parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qos_get_filter_params_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_BIND_SUBSCRIPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_PRIMARY_SUBS_V01 = 0x0001, /**<  Primary \n  */
  QOS_SECONDARY_SUBS_V01 = 0x0002, /**<  Secondary \n  */
  QOS_TERTIARY_SUBS_V01 = 0x0003, /**<  Tertiary   */
  QOS_BIND_SUBSCRIPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_bind_subscription_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Associates the requesting control point with the requested subscription. */
typedef struct {

  /* Mandatory */
  /*  Bind Subscription */
  qos_bind_subscription_enum_v01 bind_subs;
  /**<   Subscription to which to bind. Values: \n
      - QOS_PRIMARY_SUBS (0x0001) --  Primary \n
      - QOS_SECONDARY_SUBS (0x0002) --  Secondary \n
      - QOS_TERTIARY_SUBS (0x0003) --  Tertiary
 */
}qos_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Associates the requesting control point with the requested subscription. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qos_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Queries the subscription associated with the control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qos_get_bind_subscription_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Queries the subscription associated with the control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Bound Subscription */
  uint8_t bind_subscription_valid;  /**< Must be set to true if bind_subscription is being passed */
  qos_bind_subscription_enum_v01 bind_subscription;
  /**<   Values: \n
      - QOS_PRIMARY_SUBS (0x0001) --  Primary \n
      - QOS_SECONDARY_SUBS (0x0002) --  Secondary \n
      - QOS_TERTIARY_SUBS (0x0003) --  Tertiary
 */
}qos_get_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Sets the registration state for different QMI_QOS indications
           for the requesting control point. */
typedef struct {

  /* Optional */
  /*  Report Global QOS Flows */
  uint8_t report_global_qos_flows_valid;  /**< Must be set to true if report_global_qos_flows is being passed */
  uint8_t report_global_qos_flows;
  /**<   Values:  \n
       - 0 -- Do not report \n
       - 1 -- Report global QOS flows
  */
}qos_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Sets the registration state for different QMI_QOS indications
           for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qos_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_XPORT_PROTOCOL_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_PROTO_NONE_V01 = 0x00, /**<  No transport protocol\n  */
  QOS_PROTO_ICMP_V01 = 0x01, /**<  Internet Control Messaging Protocol\n  */
  QOS_PROTO_TCP_V01 = 0x06, /**<  Transmission Control Protocol\n  */
  QOS_PROTO_UDP_V01 = 0x11, /**<  User Datagram Protocol\n  */
  QOS_PROTO_ESP_V01 = 0x32, /**<  Encapsulating Security Payload protocol \n */
  QOS_PROTO_AH_V01 = 0x33, /**<  Authentication Header protocol \n */
  QOS_PROTO_ICMPV6_V01 = 0x3A, /**<  Internet Control Messaging Protocol for IPV6 \n */
  QOS_XPORT_PROTOCOL_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_xport_protocol_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ipv6_address[QMI_QOS_IPV6_ADDR_LEN_V01];
  /**<   IPv6 address.
   */

  uint8_t prefix_len;
  /**<   IPv6 address prefix length.
   */
}qos_ipv6_addr_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t qos_ipv4_filter_mask_v01;
#define QMI_QOS_IPV4_FILTER_MASK_NONE_V01 ((qos_ipv4_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_IPV4_FILTER_MASK_SRC_ADDR_V01 ((qos_ipv4_filter_mask_v01)0x0000000000000001ull) /**<  IPv4 source address  */
#define QMI_QOS_IPV4_FILTER_MASK_DEST_ADDR_V01 ((qos_ipv4_filter_mask_v01)0x0000000000000002ull) /**<  IPv4 destination address  */
#define QMI_QOS_IPV4_FILTER_MASK_TOS_V01 ((qos_ipv4_filter_mask_v01)0x0000000000000004ull) /**<  IPv4 type of service  */
typedef uint64_t qos_ipv6_filter_mask_v01;
#define QMI_QOS_IPV6_FILTER_MASK_NONE_V01 ((qos_ipv6_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_IPV6_FILTER_MASK_SRC_ADDR_V01 ((qos_ipv6_filter_mask_v01)0x0000000000000001ull) /**<  IPv6 source address  */
#define QMI_QOS_IPV6_FILTER_MASK_DEST_ADDR_V01 ((qos_ipv6_filter_mask_v01)0x0000000000000002ull) /**<  IPv6 destination address  */
#define QMI_QOS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01 ((qos_ipv6_filter_mask_v01)0x0000000000000004ull) /**<  IPv6 traffic class  */
#define QMI_QOS_IPV6_FILTER_MASK_FLOW_LABEL_V01 ((qos_ipv6_filter_mask_v01)0x0000000000000008ull) /**<  IPv6 flow label  */
/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t ipv4_addr;
  /**<   IPv4 address.
   */

  uint32_t subnet_mask;
  /**<   IPv4 subnet mask.
   */
}qos_ipv4_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t val;
  /**<   Type of service value. */

  uint8_t mask;
  /**<   Type of service mask. */
}qos_ipv4_tos_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ipv4_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_QOS_IPV4_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_IPV4_FILTER_MASK_SRC_ADDR (0x0000000000000001) --  IPv4 source address
      - QMI_QOS_IPV4_FILTER_MASK_DEST_ADDR (0x0000000000000002) --  IPv4 destination address
      - QMI_QOS_IPV4_FILTER_MASK_TOS (0x0000000000000004) --  IPv4 type of service
 */

  qos_ipv4_addr_type_v01 src_addr;
  /**<   IPv4 source address.
   */

  qos_ipv4_addr_type_v01 dest_addr;
  /**<   IPv4 destination address.
   */

  qos_ipv4_tos_type_v01 tos;
  /**<   IPv4 type of service.
   */
}qos_ipv4_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t val;
  /**<   Traffic class value. */

  uint8_t mask;
  /**<   Traffic class mask. */
}qos_ipv6_trf_cls_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ipv6_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_QOS_IPV6_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_IPV6_FILTER_MASK_SRC_ADDR (0x0000000000000001) --  IPv6 source address
      - QMI_QOS_IPV6_FILTER_MASK_DEST_ADDR (0x0000000000000002) --  IPv6 destination address
      - QMI_QOS_IPV6_FILTER_MASK_TRAFFIC_CLASS (0x0000000000000004) --  IPv6 traffic class
      - QMI_QOS_IPV6_FILTER_MASK_FLOW_LABEL (0x0000000000000008) --  IPv6 flow label
 */

  qos_ipv6_addr_type_v01 src_addr;
  /**<   IPv6 source address.
   */

  qos_ipv6_addr_type_v01 dest_addr;
  /**<   IPv6 destination address.
   */

  qos_ipv6_trf_cls_type_v01 trf_cls;
  /**<   IPv6 traffic class.
   */

  uint32_t flow_label;
  /**<   IPv6 flow label.
   */
}qos_ipv6_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ip_family_enum_v01 ip_version;
  /**<   Depending on the IP version set, either the IPv4 or the IPv6 information is valid. Values:\n
      - QOS_IP_FAMILY_IPV4 (0x04) --
      - QOS_IP_FAMILY_IPV6 (0x06) --
 */

  qos_ipv4_info_type_v01 v4_info;
  /**<   Filter parameters for IPv4.
   */

  qos_ipv6_info_type_v01 v6_info;
  /**<   Filter parameters for IPv6.
   */
}qos_ip_header_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t port;
  /**<   Port.
  */

  uint16_t range;
  /**<   Range.
   */
}qos_port_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t qos_port_info_filter_mask_v01;
#define QMI_QOS_PORT_INFO_FILTER_MASK_NONE_V01 ((qos_port_info_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01 ((qos_port_info_filter_mask_v01)0x0000000000000001ull) /**<  Source port  */
#define QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01 ((qos_port_info_filter_mask_v01)0x0000000000000002ull) /**<  Destination port  */
/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_port_info_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_QOS_PORT_INFO_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT (0x0000000000000001) --  Source port
      - QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT (0x0000000000000002) --  Destination port
 */

  qos_port_type_v01 src_port_info;
  /**<   Source port information.
  */

  qos_port_type_v01 dest_port_info;
  /**<   Destination port information.
   */
}qos_port_info_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t qos_icmp_filter_mask_v01;
#define QMI_QOS_ICMP_FILTER_MASK_NONE_V01 ((qos_icmp_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_ICMP_FILTER_MASK_MSG_TYPE_V01 ((qos_icmp_filter_mask_v01)0x0000000000000001ull) /**<  Message type  */
#define QMI_QOS_ICMP_FILTER_MASK_MSG_CODE_V01 ((qos_icmp_filter_mask_v01)0x0000000000000002ull) /**<  Message code  */
/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_icmp_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_QOS_ICMP_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_ICMP_FILTER_MASK_MSG_TYPE (0x0000000000000001) --  Message type
      - QMI_QOS_ICMP_FILTER_MASK_MSG_CODE (0x0000000000000002) --  Message code
 */

  uint8_t type;
  /**<   ICMP type.
  */

  uint8_t code;
  /**<   ICMP code.
   */
}qos_icmp_info_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t qos_ipsec_filter_mask_v01;
#define QMI_QOS_IPSEC_FILTER_MASK_NONE_V01 ((qos_ipsec_filter_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_IPSEC_FILTER_MASK_SPI_V01 ((qos_ipsec_filter_mask_v01)0x0000000000000001ull) /**<  Security parameter index  */
/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ipsec_filter_mask_v01 valid_params;
  /**<   The bits set in this mask denote which parameters contain valid values. Values: \n
      - QMI_QOS_IPSEC_FILTER_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_IPSEC_FILTER_MASK_SPI (0x0000000000000001) --  Security parameter index
 */

  uint32_t spi;
  /**<   Security parameter index for IPSEC.
   */
}qos_ipsec_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_xport_protocol_enum_v01 xport_protocol;
  /**<   Depending on the value in xport_protocol, only one field of icmp_info,
 tcp_info, udp_info, esp_info, or ah_info is valid. QOS_PROTO_NONE
 implies that no transport level protocol parameters are valid. Values:\n
      - QOS_PROTO_NONE (0x00) --  No transport protocol\n
      - QOS_PROTO_ICMP (0x01) --  Internet Control Messaging Protocol\n
      - QOS_PROTO_TCP (0x06) --  Transmission Control Protocol\n
      - QOS_PROTO_UDP (0x11) --  User Datagram Protocol\n
      - QOS_PROTO_ESP (0x32) --  Encapsulating Security Payload protocol \n
      - QOS_PROTO_AH (0x33) --  Authentication Header protocol \n
      - QOS_PROTO_ICMPV6 (0x3A) --  Internet Control Messaging Protocol for IPV6 \n
 */

  qos_port_info_type_v01 tcp_info;
  /**<   Filter parameters for TCP.
   */

  qos_port_info_type_v01 udp_info;
  /**<   Filter parameters for UDP.
   */

  qos_icmp_info_type_v01 icmp_info;
  /**<   Filter parameters for ICMP.
   */

  qos_ipsec_info_type_v01 esp_info;
  /**<   Filter parameters for ESP.
   */

  qos_ipsec_info_type_v01 ah_info;
  /**<   Filter parameters for AH.
    */
}qos_xport_header_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ip_header_type_v01 ip_info;
  /**<   Internet protocol filter parameters.
  */

  qos_xport_header_type_v01 xport_info;
  /**<   Transport level protocol filter parameters.
   */
}qos_filter_rule_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_filter_rule_type_v01 fltr_info;
  /**<   Filter parameters.
  */

  uint16_t filter_id;
  /**<   Unique identifier for each filter.
  */

  uint16_t filter_precedence;
  /**<   Specifies the order in which filters are applied. A lower numerical
       value has a higher precedence.
  */
}qos_filter_rule_resp_ind_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t qos_filter_param_error_mask_v01;
#define QMI_QOS_FILTER_PARAM_NONE_V01 ((qos_filter_param_error_mask_v01)0x0000000000000000ull) /**<  No errors \n  */
#define QMI_QOS_FILTER_PARAM_IP_VERSION_V01 ((qos_filter_param_error_mask_v01)0x0000000000000001ull) /**<  IP version \n  */
#define QMI_QOS_FILTER_PARAM_IPV4_SRC_ADDR_V01 ((qos_filter_param_error_mask_v01)0x0000000000000002ull) /**<  IPv4 source address \n  */
#define QMI_QOS_FILTER_PARAM_IPV4_DEST_ADDR_V01 ((qos_filter_param_error_mask_v01)0x0000000000000004ull) /**<  IPv4 destination address \n  */
#define QMI_QOS_FILTER_PARAM_IPV4_TOS_V01 ((qos_filter_param_error_mask_v01)0x0000000000000008ull) /**<  IPv4 type of service \n  */
#define QMI_QOS_FILTER_PARAM_IPV6_SRC_ADDR_V01 ((qos_filter_param_error_mask_v01)0x0000000000000010ull) /**<  IPv6 source address \n  */
#define QMI_QOS_FILTER_PARAM_IPV6_DEST_ADDR_V01 ((qos_filter_param_error_mask_v01)0x0000000000000020ull) /**<  IPv6 destination address \n  */
#define QMI_QOS_FILTER_PARAM_IPV6_TRF_CLS_V01 ((qos_filter_param_error_mask_v01)0x0000000000000040ull) /**<  IPv6 traffic class \n  */
#define QMI_QOS_FILTER_PARAM_IPV6_FLOW_LABEL_V01 ((qos_filter_param_error_mask_v01)0x0000000000000080ull) /**<  IPv6 flow label \n  */
#define QMI_QOS_FILTER_PARAM_XPORT_PROT_V01 ((qos_filter_param_error_mask_v01)0x0000000000000100ull) /**<  Transport protocol \n  */
#define QMI_QOS_FILTER_PARAM_TCP_SRC_PORT_V01 ((qos_filter_param_error_mask_v01)0x0000000000000200ull) /**<  TCP source port \n  */
#define QMI_QOS_FILTER_PARAM_TCP_DEST_PORT_V01 ((qos_filter_param_error_mask_v01)0x0000000000000400ull) /**<  TCP destination port \n  */
#define QMI_QOS_FILTER_PARAM_UDP_SRC_PORT_V01 ((qos_filter_param_error_mask_v01)0x0000000000000800ull) /**<  UDP source port \n  */
#define QMI_QOS_FILTER_PARAM_UDP_DEST_PORT_V01 ((qos_filter_param_error_mask_v01)0x0000000000001000ull) /**<  UDP destination port \n  */
#define QMI_QOS_FILTER_PARAM_ICMP_TYPE_V01 ((qos_filter_param_error_mask_v01)0x0000000000002000ull) /**<  ICMP type \n  */
#define QMI_QOS_FILTER_PARAM_ICMP_CODE_V01 ((qos_filter_param_error_mask_v01)0x0000000000004000ull) /**<  ICMP code \n  */
#define QMI_QOS_FILTER_PARAM_ESP_SPI_V01 ((qos_filter_param_error_mask_v01)0x0000000000008000ull) /**<  Encapsulating SPI \n  */
#define QMI_QOS_FILTER_PARAM_AH_SPI_V01 ((qos_filter_param_error_mask_v01)0x0000000000010000ull) /**<  Authentication header SPI  */
typedef uint64_t qos_ip_flow_params_mask_v01;
#define QMI_QOS_IP_FLOW_MASK_NONE_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000000ull) /**<  No parameters  */
#define QMI_QOS_IP_FLOW_MASK_TRF_CLASS_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000001ull) /**<  Traffic Class  */
#define QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000002ull) /**<  Data Rate Min Max  */
#define QMI_QOS_IP_FLOW_MASK_DATA_RATE_TOK_BUCKET_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000004ull) /**<  Data Rate Token Bucket  */
#define QMI_QOS_IP_FLOW_MASK_LATENCY_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000008ull) /**<  Latency  */
#define QMI_QOS_IP_FLOW_MASK_JITTER_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000010ull) /**<  Jitter  */
#define QMI_QOS_IP_FLOW_MASK_PKT_ERR_RATE_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000020ull) /**<  Packet Error Rate  */
#define QMI_QOS_IP_FLOW_MASK_MIN_POLICED_PKT_SIZE_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000040ull) /**<  Minimum Policed Packet Size  */
#define QMI_QOS_IP_FLOW_MASK_MAX_ALLOWED_PKT_SIZE_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000080ull) /**<  Maximum Allowed Packet Size  */
#define QMI_QOS_IP_FLOW_MASK_3GPP_RES_BER_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000100ull) /**<  3GPP Residual Bit Error Rate  */
#define QMI_QOS_IP_FLOW_MASK_3GPP_TRF_PRI_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000200ull) /**<  3GPP Traffic Handling Priority  */
#define QMI_QOS_IP_FLOW_MASK_3GPP2_PROFILE_ID_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000400ull) /**<  3GPP2 Profile ID  */
#define QMI_QOS_IP_FLOW_MASK_3GPP2_FLOW_PRIORITY_V01 ((qos_ip_flow_params_mask_v01)0x0000000000000800ull) /**<  3GPP2 Flow Priority  */
#define QMI_QOS_IP_FLOW_MASK_3GPP_IM_CN_FLAG_V01 ((qos_ip_flow_params_mask_v01)0x0000000000001000ull) /**<  3GPP IM CN Subsystem Signaling Flag  */
#define QMI_QOS_IP_FLOW_MASK_3GPP_SIG_IND_V01 ((qos_ip_flow_params_mask_v01)0x0000000000002000ull) /**<  3GPP Signaling Indication  */
#define QMI_QOS_IP_FLOW_MASK_LTE_QCI_V01 ((qos_ip_flow_params_mask_v01)0x0000000000004000ull) /**<  LTE QOS Class Identifier  */
/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI1_V01 = 0, /**<  Priority Level 1  */
  QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI2_V01 = 1, /**<  Priority Level 2  */
  QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI3_V01 = 2, /**<  Priority Level 3  */
  QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_flow_3gpp_traffic_handling_pri_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_FLOW_3GPP_RESIDUAL_BER_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE1_V01 = 0, /**<  5*10-2  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE2_V01 = 1, /**<  1*10-2  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE3_V01 = 2, /**<  5*10-3  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE4_V01 = 3, /**<  4*10-3  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE5_V01 = 4, /**<  1*10-3  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE6_V01 = 5, /**<  1*10-4  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE7_V01 = 6, /**<  1*10-5  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE8_V01 = 7, /**<  1*10-6  */
  QOS_FLOW_3GPP_RES_BIT_ERR_RATE9_V01 = 8, /**<  6*10-8  */
  QOS_FLOW_3GPP_RESIDUAL_BER_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_flow_3gpp_residual_ber_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_IP_FLOW_TRF_CLS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_IP_TRF_CLS_CONVERSATIONAL_V01 = 0, /**<  IP Traffic Class Conversational
   */
  QOS_IP_TRF_CLS_STREAMING_V01 = 1, /**<  IP Traffic Class Streaming
   */
  QOS_IP_TRF_CLS_INTERACTIVE_V01 = 2, /**<  IP Traffic Class Interactive
   */
  QOS_IP_TRF_CLS_BACKGROUND_V01 = 3, /**<  IP Traffic Class Background
   */
  QOS_IP_FLOW_TRF_CLS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_ip_flow_trf_cls_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_FLOW_LTE_QCI_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_FLOW_LTE_QCI_0_V01 = 0,
  QOS_FLOW_LTE_QCI_1_V01 = 1,
  QOS_FLOW_LTE_QCI_2_V01 = 2,
  QOS_FLOW_LTE_QCI_3_V01 = 3,
  QOS_FLOW_LTE_QCI_4_V01 = 4,
  QOS_FLOW_LTE_QCI_5_V01 = 5,
  QOS_FLOW_LTE_QCI_6_V01 = 6,
  QOS_FLOW_LTE_QCI_7_V01 = 7,
  QOS_FLOW_LTE_QCI_8_V01 = 8,
  QOS_FLOW_LTE_QCI_9_V01 = 9,
  QOS_FLOW_LTE_QCI_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_flow_lte_qci_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t data_rate_max;
  /**<   Maximum required data rate (bits per second).
  */

  uint64_t guaranteed_rate;
  /**<   Minimum guaranteed data rate (bits per second).
  */
}qos_flow_data_rate_min_max_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t peak_rate;
  /**<   Maximum rate at which data can be transmitted when the token bucket is
       full (bits per second). */

  uint32_t token_rate;
  /**<   Rate at which tokens will be put in the token bucket (bits per second);
       a token is required to be present in the bucket to send a byte of data.
  */

  uint32_t bucket_size;
  /**<   Maximum number of tokens that can be accumulated at any instance (bytes);
       controls the size of the burst that is allowed at any given time.
  */
}qos_flow_data_rate_token_bucket_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t ip_flow_pkt_error_rate_multiplier;
  /**<   Factor m in calculating packet error rate:
       E = m*10**(-p) */

  uint16_t ip_flow_pkt_error_rate_exponent;
  /**<   Factor p in calculating packet error rate, see above */
}qos_flow_pkt_err_rate_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  qos_ip_flow_params_mask_v01 flow_valid_params;
  /**<   Mask of parameters that are valid for this flow. Values :\n
      - QMI_QOS_IP_FLOW_MASK_NONE (0x0000000000000000) --  No parameters
      - QMI_QOS_IP_FLOW_MASK_TRF_CLASS (0x0000000000000001) --  Traffic Class
      - QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX (0x0000000000000002) --  Data Rate Min Max
      - QMI_QOS_IP_FLOW_MASK_DATA_RATE_TOK_BUCKET (0x0000000000000004) --  Data Rate Token Bucket
      - QMI_QOS_IP_FLOW_MASK_LATENCY (0x0000000000000008) --  Latency
      - QMI_QOS_IP_FLOW_MASK_JITTER (0x0000000000000010) --  Jitter
      - QMI_QOS_IP_FLOW_MASK_PKT_ERR_RATE (0x0000000000000020) --  Packet Error Rate
      - QMI_QOS_IP_FLOW_MASK_MIN_POLICED_PKT_SIZE (0x0000000000000040) --  Minimum Policed Packet Size
      - QMI_QOS_IP_FLOW_MASK_MAX_ALLOWED_PKT_SIZE (0x0000000000000080) --  Maximum Allowed Packet Size
      - QMI_QOS_IP_FLOW_MASK_3GPP_RES_BER (0x0000000000000100) --  3GPP Residual Bit Error Rate
      - QMI_QOS_IP_FLOW_MASK_3GPP_TRF_PRI (0x0000000000000200) --  3GPP Traffic Handling Priority
      - QMI_QOS_IP_FLOW_MASK_3GPP2_PROFILE_ID (0x0000000000000400) --  3GPP2 Profile ID
      - QMI_QOS_IP_FLOW_MASK_3GPP2_FLOW_PRIORITY (0x0000000000000800) --  3GPP2 Flow Priority
      - QMI_QOS_IP_FLOW_MASK_3GPP_IM_CN_FLAG (0x0000000000001000) --  3GPP IM CN Subsystem Signaling Flag
      - QMI_QOS_IP_FLOW_MASK_3GPP_SIG_IND (0x0000000000002000) --  3GPP Signaling Indication
      - QMI_QOS_IP_FLOW_MASK_LTE_QCI (0x0000000000004000) --  LTE QOS Class Identifier
 */

  qos_ip_flow_trf_cls_enum_v01 ip_flow_trf_cls;
  /**<   Requested traffic class. Values:\n
      - QOS_IP_TRF_CLS_CONVERSATIONAL (0) --  IP Traffic Class Conversational

      - QOS_IP_TRF_CLS_STREAMING (1) --  IP Traffic Class Streaming

      - QOS_IP_TRF_CLS_INTERACTIVE (2) --  IP Traffic Class Interactive

      - QOS_IP_TRF_CLS_BACKGROUND (3) --  IP Traffic Class Background

 */

  qos_flow_data_rate_min_max_type_v01 data_rate_min_max;
  /**<   IP Flow Data Rate Min Max
   */

  qos_flow_data_rate_token_bucket_type_v01 data_rate_token_bucket;
  /**<   IP Flow Data Rate Token Bucket
   */

  uint32_t ip_flow_latency;
  /**<   Maximum delay (in milliseconds) that can be tolerated by an IP packet
       during transfer through the wireless link. */

  uint32_t ip_flow_jitter;
  /**<   Difference between the maximum and minimum latency (in milliseconds)
       that can be tolerated by an IP packet during the transfer through the
	   wireless link. */

  qos_flow_pkt_err_rate_v01 ip_flow_pkt_error_rate;
  /**<   IP Flow Packet Error Rate
   */

  uint32_t ip_flow_min_policed_packet_size;
  /**<   Integer that defines the minimum packet size (in bytes) that will be
       policed for QoS guarantees. Any IP packets that are smaller than the
	   minimum specified policed size may not receive the requested QoS.
  */

  uint32_t ip_flow_max_allowed_packet_size;
  /**<   Integer that defines the maximum packet size (in bytes) allowed in the
       IP flow. Any IP packets greater in size than the maximum allowed packet
	   size are not queued for transmission.
  */

  qos_flow_3gpp_residual_ber_enum_v01 ip_flow_3gpp_residual_bit_error_rate;
  /**<   Indicates the undetected BER for each IP flow in the delivered packets.
 Applies only to 3GPP networks. Values : \n
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE1 (0) --  5*10-2
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE2 (1) --  1*10-2
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE3 (2) --  5*10-3
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE4 (3) --  4*10-3
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE5 (4) --  1*10-3
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE6 (5) --  1*10-4
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE7 (6) --  1*10-5
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE8 (7) --  1*10-6
      - QOS_FLOW_3GPP_RES_BIT_ERR_RATE9 (8) --  6*10-8
 */

  qos_flow_3gpp_traffic_handling_pri_enum_v01 ip_flow_3gpp_traffic_handling_priority;
  /**<   Defines the relative priority of the flow. Applies only to 3GPP networks.
 Values : \n
      - QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI1 (0) --  Priority Level 1
      - QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI2 (1) --  Priority Level 2
      - QOS_FLOW_3GPP_TRAFFIC_HANDLING_PRI3 (2) --  Priority Level 3
 */

  uint16_t ip_flow_3gpp2_profile_id;
  /**<   A profile ID is shorthand for a defined set of QoS flow parameters
       specified by the network; to be present while requesting QoS for a CDMA
	   device.
  */

  uint8_t ip_flow_3gpp2_flow_priority;
  /**<   Flow priority used by the network in case of contention between flows
       with same QoS. This parameter applies for CDMA devices.
  */

  uint8_t ip_flow_3gpp_im_cn_flag;
  /**<   IM CN subsystem signaling flag. This parameter applies only to 3GPP
       networks. Values : \n
       - 0x00 – FALSE \n
       - 0x01 – TRUE
  */

  uint8_t ip_flow_3gpp_sig_ind;
  /**<   This parameter applies only to 3GPP networks. Values : \n
       - 0x00 – FALSE \n
       - 0x01 – TRUE
  */

  qos_flow_lte_qci_enum_v01 ip_flow_lte_qci;
  /**<   QoS Class Identifier (QCI) is a required parameter to request QoS in LTE
       QCI values:
       QCI value 0 requests the network to assign the appropriate QCI value
       QCI values 1-4 are associated with guaranteed bitrates
       QCI values 5-9 are associated with nonguaranteed bitrates, so the values
	   specified as guaranteed and maximum bitrates are ignored
       For more detailed information, refer to [S1].
  */
}qos_flow_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Triggers QoS negotiation by providing QoS parameters. */
typedef struct {

  /* Optional */
  /*  TX QOS Flow */
  uint8_t tx_qos_flow_valid;  /**< Must be set to true if tx_qos_flow is being passed */
  uint32_t tx_qos_flow_len;  /**< Must be set to # of elements in tx_qos_flow */
  qos_flow_type_v01 tx_qos_flow[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   TX QOS flow parameters
   */

  /* Optional */
  /*  RX QOS Flow */
  uint8_t rx_qos_flow_valid;  /**< Must be set to true if rx_qos_flow is being passed */
  uint32_t rx_qos_flow_len;  /**< Must be set to # of elements in rx_qos_flow */
  qos_flow_type_v01 rx_qos_flow[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   RX QOS flow parameters
   */

  /* Optional */
  /*  TX QOS Filter */
  uint8_t tx_qos_filter_valid;  /**< Must be set to true if tx_qos_filter is being passed */
  uint32_t tx_qos_filter_len;  /**< Must be set to # of elements in tx_qos_filter */
  qos_filter_rule_type_v01 tx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   TX QOS filter parameters
    */

  /* Optional */
  /*  RX QOS Filter */
  uint8_t rx_qos_filter_valid;  /**< Must be set to true if rx_qos_filter is being passed */
  uint32_t rx_qos_filter_len;  /**< Must be set to # of elements in rx_qos_filter */
  qos_filter_rule_type_v01 rx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   RX QOS filter parameters
    */
}qos_request_qos_ex_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Triggers QoS negotiation by providing QoS parameters. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  QOS Id */
  uint8_t qos_id_valid;  /**< Must be set to true if qos_id is being passed */
  uint32_t qos_id;
  /**<   Identifier for the QoS flow requested.
  */

  /* Optional */
  /*  TX QOS Flow Parameters Error */
  uint8_t tx_qos_flow_error_valid;  /**< Must be set to true if tx_qos_flow_error is being passed */
  uint32_t tx_qos_flow_error_len;  /**< Must be set to # of elements in tx_qos_flow_error */
  qos_ip_flow_params_mask_v01 tx_qos_flow_error[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   TX QOS flow parameters error mask
  */

  /* Optional */
  /*  RX QOS Flow Parameters Error */
  uint8_t rx_qos_flow_error_valid;  /**< Must be set to true if rx_qos_flow_error is being passed */
  uint32_t rx_qos_flow_error_len;  /**< Must be set to # of elements in rx_qos_flow_error */
  qos_ip_flow_params_mask_v01 rx_qos_flow_error[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   RX QOS flow parameters error mask
  */

  /* Optional */
  /*  TX QOS Filter Parameters Error */
  uint8_t tx_qos_filter_error_valid;  /**< Must be set to true if tx_qos_filter_error is being passed */
  uint32_t tx_qos_filter_error_len;  /**< Must be set to # of elements in tx_qos_filter_error */
  qos_filter_param_error_mask_v01 tx_qos_filter_error[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   TX QOS filter parameters error mask
   */

  /* Optional */
  /*  RX QOS Filter Parameters Error */
  uint8_t rx_qos_filter_error_valid;  /**< Must be set to true if rx_qos_filter_error is being passed */
  uint32_t rx_qos_filter_error_len;  /**< Must be set to # of elements in rx_qos_filter_error */
  qos_filter_param_error_mask_v01 rx_qos_filter_error[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   RX QOS filter parameters error mask
   */
}qos_request_qos_ex_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_FLOW_STATE_CHANGE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_FLOW_ACTIVATED_V01 = 0, /**<  Flow Activated  */
  QOS_FLOW_MODIFIED_V01 = 1, /**<  Flow Modified  */
  QOS_FLOW_DELETED_V01 = 2, /**<  Flow Deleted  */
  QOS_FLOW_SUSPENDED_V01 = 3, /**<  Flow Suspended  */
  QOS_FLOW_ENABLED_V01 = 4, /**<  Flow Enabled  */
  QOS_FLOW_DISABLED_V01 = 5, /**<  Flow Disabled  */
  QOS_FLOW_STATE_CHANGE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_flow_state_change_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_enums
    @{
  */
typedef enum {
  QOS_FLOW_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QOS_FLOW_UE_INITIATED_QOS_V01 = 0, /**<  UE Initiated QOS flow  */
  QOS_FLOW_NETWORK_INITIATED_QOS_V01 = 1, /**<  Network Initiated QOS flow  */
  QOS_FLOW_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qos_flow_type_enum_v01;
/**
    @}
  */

/** @addtogroup qos_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t qos_id;
  /**<   QOS Identifier
  */

  uint8_t new_flow;
  /**<   Values : \n
       - 0 -- Existing flow \n
	   - 1 -- Newly added flow
  */

  qos_flow_state_change_enum_v01 state_change;
  /**<   State change of flow. Values : \n
      - QOS_FLOW_ACTIVATED (0) --  Flow Activated
      - QOS_FLOW_MODIFIED (1) --  Flow Modified
      - QOS_FLOW_DELETED (2) --  Flow Deleted
      - QOS_FLOW_SUSPENDED (3) --  Flow Suspended
      - QOS_FLOW_ENABLED (4) --  Flow Enabled
      - QOS_FLOW_DISABLED (5) --  Flow Disabled
 */
}qos_flow_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Indication Message; Indicates change of state in QOS flow. */
typedef struct {

  /* Mandatory */
  /*  QOS Flow State */
  qos_flow_info_type_v01 qos_flow_state;

  /* Optional */
  /*  TX QOS flow granted */
  uint8_t tx_qos_flow_granted_valid;  /**< Must be set to true if tx_qos_flow_granted is being passed */
  qos_flow_type_v01 tx_qos_flow_granted;
  /**<   Granted TX QOS flow */

  /* Optional */
  /*  RX QOS flow granted */
  uint8_t rx_qos_flow_granted_valid;  /**< Must be set to true if rx_qos_flow_granted is being passed */
  qos_flow_type_v01 rx_qos_flow_granted;
  /**<   Granted RX QOS flow */

  /* Optional */
  /*  TX QOS filters */
  uint8_t tx_qos_filters_valid;  /**< Must be set to true if tx_qos_filters is being passed */
  uint32_t tx_qos_filters_len;  /**< Must be set to # of elements in tx_qos_filters */
  qos_filter_rule_resp_ind_type_v01 tx_qos_filters[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   Filters that apply to granted TX QOS flow */

  /* Optional */
  /*  RX QOS filters */
  uint8_t rx_qos_filters_valid;  /**< Must be set to true if rx_qos_filters is being passed */
  uint32_t rx_qos_filters_len;  /**< Must be set to # of elements in rx_qos_filters */
  qos_filter_rule_resp_ind_type_v01 rx_qos_filters[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   Filters that apply to granted RX QOS flow */

  /* Optional */
  /*  QOS Flow Type */
  uint8_t flow_type_valid;  /**< Must be set to true if flow_type is being passed */
  qos_flow_type_enum_v01 flow_type;
  /**<   Indicates whether the QoS flow type is network-initated or UE-initiated.
       Values : \n
	   @ENUM
  */

  /* Optional */
  /*  Bearer Id */
  uint8_t bearer_id_valid;  /**< Must be set to true if bearer_id is being passed */
  uint8_t bearer_id;
  /**<   Bearer ID or Radio Link Protocol (RLP) ID of the activated flow.
  */

  /* Optional */
  /*  Flow Control Sequence Number */
  uint8_t fc_seq_num_valid;  /**< Must be set to true if fc_seq_num is being passed */
  uint16_t fc_seq_num;
  /**<   The sequence number of each flow enable and disable event. Whenever the
       flow is disabled, the sequence number is incremented. The sequence number
	   remains the same when the flow is enabled.
  */
}qos_global_qos_flow_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Modifies an existing QOS instance. */
typedef struct {

  /* Mandatory */
  /*  QOS Identifier */
  uint32_t qos_id;
  /**<   Identifier for the QoS flow/instance that has been negotiated and that
        is to be modified. The QoS identifier is used to reference the actual
        flow/filter specifications that are in effect as a result of the negotiation
        triggered by QMI_QOS_REQUEST_QOS_EX_REQ.*/

  /* Optional */
  /*  TX QOS Flow */
  uint8_t tx_qos_flow_valid;  /**< Must be set to true if tx_qos_flow is being passed */
  uint32_t tx_qos_flow_len;  /**< Must be set to # of elements in tx_qos_flow */
  qos_flow_type_v01 tx_qos_flow[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   TX QOS flow parameters
   */

  /* Optional */
  /*  RX QOS Flow */
  uint8_t rx_qos_flow_valid;  /**< Must be set to true if rx_qos_flow is being passed */
  uint32_t rx_qos_flow_len;  /**< Must be set to # of elements in rx_qos_flow */
  qos_flow_type_v01 rx_qos_flow[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   RX QOS flow parameters
   */

  /* Optional */
  /*  TX QOS Filter */
  uint8_t tx_qos_filter_valid;  /**< Must be set to true if tx_qos_filter is being passed */
  uint32_t tx_qos_filter_len;  /**< Must be set to # of elements in tx_qos_filter */
  qos_filter_rule_type_v01 tx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   TX QOS filter parameters
    */

  /* Optional */
  /*  RX QOS Filter */
  uint8_t rx_qos_filter_valid;  /**< Must be set to true if rx_qos_filter is being passed */
  uint32_t rx_qos_filter_len;  /**< Must be set to # of elements in rx_qos_filter */
  qos_filter_rule_type_v01 rx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   RX QOS filter parameters
    */
}qos_modify_qos_ex_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Modifies an existing QOS instance. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  TX QOS Flow Parameters Error */
  uint8_t tx_qos_flow_error_valid;  /**< Must be set to true if tx_qos_flow_error is being passed */
  uint32_t tx_qos_flow_error_len;  /**< Must be set to # of elements in tx_qos_flow_error */
  qos_ip_flow_params_mask_v01 tx_qos_flow_error[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   TX QOS flow parameters error mask
  */

  /* Optional */
  /*  RX QOS Flow Parameters Error */
  uint8_t rx_qos_flow_error_valid;  /**< Must be set to true if rx_qos_flow_error is being passed */
  uint32_t rx_qos_flow_error_len;  /**< Must be set to # of elements in rx_qos_flow_error */
  qos_ip_flow_params_mask_v01 rx_qos_flow_error[QMI_QOS_MAX_QOS_FLOWS_V01];
  /**<   RX QOS flow parameters error mask
  */

  /* Optional */
  /*  TX QOS Filter Parameters Error */
  uint8_t tx_qos_filter_error_valid;  /**< Must be set to true if tx_qos_filter_error is being passed */
  uint32_t tx_qos_filter_error_len;  /**< Must be set to # of elements in tx_qos_filter_error */
  qos_filter_param_error_mask_v01 tx_qos_filter_error[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   TX QOS filter parameters error mask
   */

  /* Optional */
  /*  RX QOS Filter Parameters Error */
  uint8_t rx_qos_filter_error_valid;  /**< Must be set to true if rx_qos_filter_error is being passed */
  uint32_t rx_qos_filter_error_len;  /**< Must be set to # of elements in rx_qos_filter_error */
  qos_filter_param_error_mask_v01 rx_qos_filter_error[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   RX QOS filter parameters error mask
   */
}qos_modify_qos_ex_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Request Message; Retrieves the QoS flow and filter parameters that are in effect for
           the specified QoS flow. */
typedef struct {

  /* Mandatory */
  /*  QOS Identifier */
  uint32_t qos_id;
  /**<   Identifier for the QoS flow/instance that has been negotiated and that
        is being queried. */
}qos_get_qos_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qos_qmi_messages
    @{
  */
/** Response Message; Retrieves the QoS flow and filter parameters that are in effect for
           the specified QoS flow. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  QoS Flow Status  */
  uint8_t flow_status_valid;  /**< Must be set to true if flow_status is being passed */
  qos_status_enum_v01 flow_status;
  /**<   Current flow status. Values :\n
      - QOS_STATUS_ACTIVATED (0x01) --  Activated status
      - QOS_STATUS_SUSPENDED (0x02) --  Suspended status
 */

  /* Optional */
  /*  TX QoS Granted Flow  */
  uint8_t tx_qos_flow_valid;  /**< Must be set to true if tx_qos_flow is being passed */
  qos_flow_type_v01 tx_qos_flow;
  /**<   Contains the TX flow specification granted by the network.
  */

  /* Optional */
  /*  RX QoS Granted Flow  */
  uint8_t rx_qos_flow_valid;  /**< Must be set to true if rx_qos_flow is being passed */
  qos_flow_type_v01 rx_qos_flow;
  /**<   Contains the RX flow specification granted by the network.
  */

  /* Optional */
  /*  TX QoS Filter Specs  */
  uint8_t tx_qos_filter_valid;  /**< Must be set to true if tx_qos_filter is being passed */
  uint32_t tx_qos_filter_len;  /**< Must be set to # of elements in tx_qos_filter */
  qos_filter_rule_resp_ind_type_v01 tx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   Contains one or more instances of the TX filter specification.
  */

  /* Optional */
  /*  RX QoS Filter Specs  */
  uint8_t rx_qos_filter_valid;  /**< Must be set to true if rx_qos_filter is being passed */
  uint32_t rx_qos_filter_len;  /**< Must be set to # of elements in rx_qos_filter */
  qos_filter_rule_resp_ind_type_v01 rx_qos_filter[QMI_QOS_MAX_QOS_FILTERS_V01];
  /**<   Contains one or more instances of the RX filter specification.
  */

  /* Optional */
  /*  Extended Error Info */
  uint8_t ext_error_info_valid;  /**< Must be set to true if ext_error_info is being passed */
  uint16_t ext_error_info;
  /**<   Data services error value returned by the
       IOCTL/lower layers. Refer to the DS error numbers
       published in [Q5] for the possible values returned in
       this field..
  */
}qos_get_qos_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_QOS_BIND_DATA_PORT_V01
//#define REMOVE_QMI_QOS_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_QOS_GET_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_QOS_GET_FILTER_PARAMS_V01
//#define REMOVE_QMI_QOS_GET_GRANTED_QOS_V01
//#define REMOVE_QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES_V01
//#define REMOVE_QMI_QOS_GET_QOS_INFO_V01
//#define REMOVE_QMI_QOS_GET_QOS_NETWORK_STATUS_V01
//#define REMOVE_QMI_QOS_GET_QOS_STATUS_V01
//#define REMOVE_QMI_QOS_GLOBAL_QOS_FLOW_IND_V01
//#define REMOVE_QMI_QOS_INDICATION_REGISTER_V01
//#define REMOVE_QMI_QOS_MODIFY_QOS_V01
//#define REMOVE_QMI_QOS_MODIFY_QOS_EX_V01
//#define REMOVE_QMI_QOS_PERFORM_FLOW_OPERATION_V01
//#define REMOVE_QMI_QOS_PRIMARY_QOS_EVENT_IND_V01
//#define REMOVE_QMI_QOS_RELEASE_QOS_V01
//#define REMOVE_QMI_QOS_REQUEST_QOS_V01
//#define REMOVE_QMI_QOS_REQUEST_QOS_EX_V01
//#define REMOVE_QMI_QOS_RESET_V01
//#define REMOVE_QMI_QOS_RESUME_QOS_V01
//#define REMOVE_QMI_QOS_SET_CLIENT_IP_PREF_V01
//#define REMOVE_QMI_QOS_SET_EVENT_REPORT_V01
//#define REMOVE_QMI_QOS_SUSPEND_QOS_V01

/*Service Message Definition*/
/** @addtogroup qos_qmi_msg_ids
    @{
  */
#define QMI_QOS_RESET_REQ_V01 0x0000
#define QMI_QOS_RESET_RESP_V01 0x0000
#define QMI_QOS_SET_EVENT_REPORT_REQ_V01 0x0001
#define QMI_QOS_SET_EVENT_REPORT_RESP_V01 0x0001
#define QMI_QOS_EVENT_REPORT_IND_V01 0x0001
#define QMI_QOS_REQUEST_QOS_REQ_V01 0x0020
#define QMI_QOS_REQUEST_QOS_RESP_V01 0x0020
#define QMI_QOS_RELEASE_QOS_REQ_V01 0x0021
#define QMI_QOS_RELEASE_QOS_RESP_V01 0x0021
#define QMI_QOS_SUSPEND_QOS_REQ_V01 0x0022
#define QMI_QOS_SUSPEND_QOS_RESP_V01 0x0022
#define QMI_QOS_RESUME_QOS_REQ_V01 0x0023
#define QMI_QOS_RESUME_QOS_RESP_V01 0x0023
#define QMI_QOS_MODIFY_QOS_REQ_V01 0x0024
#define QMI_QOS_MODIFY_QOS_RESP_V01 0x0024
#define QMI_QOS_GET_GRANTED_QOS_REQ_V01 0x0025
#define QMI_QOS_GET_GRANTED_QOS_RESP_V01 0x0025
#define QMI_QOS_GET_QOS_STATUS_REQ_V01 0x0026
#define QMI_QOS_GET_QOS_STATUS_RESP_V01 0x0026
#define QMI_QOS_STATUS_IND_V01 0x0026
#define QMI_QOS_GET_QOS_NETWORK_STATUS_REQ_V01 0x0027
#define QMI_QOS_GET_QOS_NETWORK_STATUS_RESP_V01 0x0027
#define QMI_QOS_NETWORK_STATUS_IND_V01 0x0027
#define QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES_REQ_V01 0x0028
#define QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES_RESP_V01 0x0028
#define QMI_QOS_PRIMARY_QOS_EVENT_IND_V01 0x0029
#define QMI_QOS_SET_CLIENT_IP_PREF_REQ_V01 0x002A
#define QMI_QOS_SET_CLIENT_IP_PREF_RESP_V01 0x002A
#define QMI_QOS_BIND_DATA_PORT_REQ_V01 0x002B
#define QMI_QOS_BIND_DATA_PORT_RESP_V01 0x002B
#define QMI_QOS_GET_FILTER_PARAMS_REQ_V01 0x002C
#define QMI_QOS_GET_FILTER_PARAMS_RESP_V01 0x002C
#define QMI_QOS_BIND_SUBSCRIPTION_REQ_V01 0x002D
#define QMI_QOS_BIND_SUBSCRIPTION_RESP_V01 0x002D
#define QMI_QOS_GET_BIND_SUBSCRIPTION_REQ_V01 0x002E
#define QMI_QOS_GET_BIND_SUBSCRIPTION_RESP_V01 0x002E
#define QMI_QOS_INDICATION_REGISTER_REQ_V01 0x002F
#define QMI_QOS_INDICATION_REGISTER_RESP_V01 0x002F
#define QMI_QOS_REQUEST_QOS_EX_REQ_V01 0x0030
#define QMI_QOS_REQUEST_QOS_EX_RESP_V01 0x0030
#define QMI_QOS_GLOBAL_QOS_FLOW_IND_V01 0x0031
#define QMI_QOS_MODIFY_QOS_EX_REQ_V01 0x0032
#define QMI_QOS_MODIFY_QOS_EX_RESP_V01 0x0032
#define QMI_QOS_GET_QOS_INFO_REQ_V01 0x0033
#define QMI_QOS_GET_QOS_INFO_RESP_V01 0x0033
#define QMI_QOS_PERFORM_FLOW_OPERATION_REQ_V01 0xFFFE
#define QMI_QOS_PERFORM_FLOW_OPERATION_RESP_V01 0xFFFE
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qos_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qos_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define qos_get_service_object_v01( ) \
          qos_get_service_object_internal_v01( \
            QOS_V01_IDL_MAJOR_VERS, QOS_V01_IDL_MINOR_VERS, \
            QOS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

