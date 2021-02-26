#ifndef LTE_SERVICE_01_H
#define LTE_SERVICE_01_H
/**
  @file lte_v01.h

  @brief This is the public header file which defines the lte service Data structures.

  This header file defines the types and structures that were defined in
  lte. It contains the constant values defined, enums, structures,
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
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed May 25 2016 (Spin 1)
   From IDL File: lte_v01.idl */

/** @defgroup lte_qmi_consts Constant values defined in the IDL */
/** @defgroup lte_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup lte_qmi_enums Enumerated types used in QMI messages */
/** @defgroup lte_qmi_messages Structures sent as QMI messages */
/** @defgroup lte_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup lte_qmi_accessor Accessor for QMI service object */
/** @defgroup lte_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup lte_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LTE_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define LTE_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LTE_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LTE_V01_MAX_MESSAGE_ID 0x0031
/**
    @}
  */


/** @addtogroup lte_qmi_consts
    @{
  */

/**  Maximum number of Announcing PLMNs in the policy definition  \n  */
#define QMI_LTE_DISC_DEDICATED_APN_NAME_MAX_V01 100

/**  Maximum number of Monitoring PLMNs in the policy definition \n  */
#define QMI_LTE_DISC_ANNOUNCING_POLICY_LIST_MAX_V01 50

/**  Max length of the Prose Application ID name  */
#define QMI_LTE_DISC_MONITORING_POLICY_LIST_MAX_V01 50

/**  Max length of the Prose Application ID name  */
#define QMI_LTE_DISC_PA_ID_NAME_MAX_V01 2048

/**  Max number of expressions (pa_ids) in the expression list \n  */
#define QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 2048

/**  Max number of events reported in match_event \n  */
#define QMI_LTE_DISC_NUM_PA_ID_MAX_V01 10

/**  Max string length for the metadata field in match event\n  */
#define QMI_LTE_DISC_NUM_MATCH_EVENTS_MAX_V01 10

/**  Max length for the BAK password PSK in provisioning request \n  */
#define QMI_LTE_DISC_METADATA_MAX_V01 16384

/**  Bitmask to map indications for registration purpose
     This is independent of the LTE-D feature and is common to
     various clients that can connect with the QMI LTE service
   */
#define QMI_LTE_DISC_PSK_MAX_LEN_V01 64
/**
    @}
  */

typedef uint64_t qmi_lte_indication_reg_mask_t_v01;
#define QMI_LTE_DISC_NOTIFICATION_IND_MASK_V01 ((qmi_lte_indication_reg_mask_t_v01)0x00000001ull) /**<  Bitmask to register for QMI_LTE_DISC_NOTIFICATION_IND  */
#define QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_MASK_V01 ((qmi_lte_indication_reg_mask_t_v01)0x00000002ull) /**<  Bitmask to register for QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND  */
#define QMI_LTE_DISC_MATCH_NOTIFICATION_IND_MASK_V01 ((qmi_lte_indication_reg_mask_t_v01)0x00000004ull) /**<  Bitmask to register for QMI_LTE_DISC_MATCH_NOTIFICATION_IND  */
#define QMI_LTE_DISC_PSK_EXPIRED_IND_MASK_V01 ((qmi_lte_indication_reg_mask_t_v01)0x00000008ull) /**<  Bitmask to register for QMI_LTE_DISC_MATCH_NOTIFICATION_IND  */
/** @addtogroup lte_qmi_enums
    @{
  */
typedef enum {
  QMI_LTE_DISC_CATEGORY_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_LTE_DISC_CATEGORY_HIGH_V01 = 0, /**<  Transmit PACs of associated OS AppID at a higher rate \n
   */
  QMI_LTE_DISC_CATEGORY_MEDIUM_V01 = 1, /**<  Transmit PACs of associated OS AppID at a medium rate \n
   */
  QMI_LTE_DISC_CATEGORY_LOW_V01 = 2, /**<  Transmit PACs of associated OS AppID at a low rate \n
   */
  QMI_LTE_DISC_CATEGORY_VERY_LOW_V01 = 3, /**<  Transmit PACs of associated OS AppID at a very low rate \n
   */
  QMI_LTE_DISC_CATEGORY_INVALID_V01 = 4, /**<  Invalid type identifier \n
   */
  QMI_LTE_DISC_CATEGORY_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_lte_disc_category_type_enum_v01;
/**
    @}
  */

/** @addtogroup lte_qmi_enums
    @{
  */
typedef enum {
  QMI_LTE_DISCOVERY_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_LTE_DISC_DISCOVERY_INVALID_V01 = 0, /**<  Invalid type identifier \n
   */
  QMI_LTE_DISC_DISCOVERY_OPEN_V01 = 1, /**<  Open Discovery \n
   */
  QMI_LTE_DISC_DISCOVERY_RESTRICTED_V01 = 2, /**<  Restricted Discovery \n
   */
  QMI_LTE_DISCOVERY_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_lte_discovery_type_enum_v01;
/**
    @}
  */

/** @addtogroup lte_qmi_enums
    @{
  */
typedef enum {
  QMI_LTE_DISC_RESULT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_LTE_DISC_SUCCESS_V01 = 0, /**<  Action was successful  \n  */
  QMI_LTE_DISC_IN_PROGRESS_V01 = 1, /**<  Action is temporarily successful \n  */
  QMI_LTE_DISC_FAILURE_V01 = 2, /**<  Action failed for a generic reason \n  */
  QMI_LTE_DISC_ERR_UNKNOWN_V01 = 3, /**<  Undeterministic result for the action \n  */
  QMI_LTE_DISC_ERR_NETWORK_TRANS_FAILURE_V01 = 4, /**<  Action failed as unable to connect with server  */
  QMI_LTE_DISC_ERR_MALFORMED_PC3_MSG_V01 = 5, /**<  Action failed due to malformed PC3 message \n  */
  QMI_LTE_DISC_ERR_INVALID_EXPRESSION_SCOPE_V01 = 6, /**<  Action failed as expression is invalid or defined out-of-scope \n  */
  QMI_LTE_DISC_ERR_UNKNOWN_EXPRESSION_V01 = 7, /**<  Action failed for an unknown expression \n  */
  QMI_LTE_DISC_ERR_INVALID_DISCOVERY_TYPE_V01 = 8, /**<  Action failed for an invalid discovery type \n  */
  QMI_LTE_DISC_ERR_INVALID_TX_ID_V01 = 9, /**<  Action failed for an invalid transaction ID \n  */
  QMI_LTE_DISC_ERR_SERVICE_UNAVAILABLE_V01 = 10, /**<  Action failed as LTE Discovery service is not available \n  */
  QMI_LTE_DISC_ERR_APP_AUTH_FAILURE_V01 = 11, /**<  Action failed as app is not authorized for LTE-D \n  */
  QMI_LTE_DISC_ERR_FEATURE_NOT_SUPPORTED_V01 = 12, /**<  Action failed as LTE-D feature is not supported \n  */
  QMI_LTE_DISC_ERR_VALIDITY_TIME_EXPIRED_V01 = 13, /**<  Action failed as validity time T_4005 has expired \n  */
  QMI_LTE_DISC_RESULT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_lte_disc_result_type_enum_v01;
/**
    @}
  */

/** @addtogroup lte_qmi_aggregates
    @{
  */
/**  Public Land Mobile Network (PLMN) ID.

  A PLMN ID is defined to be the combination of a Mobile Country Code (MCC) and
  Mobile Network Code (MNC). A PLMN ID is stored in three octets as below:


                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|


 */
typedef struct {

  uint8_t plmn_id[3];
  /**<   Public Land Mobile Network ID. */
}qmi_lte_disc_plmn_id_s_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lte_qmi_aggregates
    @{
  */
/**  PA ID definition -
    Needs to be deined as a structure as we can have an array of strings
    for the same OS App Id

 */
typedef struct {

  char prose_app_id_name[QMI_LTE_DISC_PA_ID_NAME_MAX_V01 + 1];
}qmi_lte_disc_prose_app_id_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lte_qmi_aggregates
    @{
  */
/**  128 bit values
 */
typedef struct {

  uint64_t value_1_64;

  uint64_t value_65_128;
}value_128_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lte_qmi_enums
    @{
  */
typedef enum {
  QMI_LTE_DISC_ANNOUNCING_POLICY_RANGE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_LTE_DISC_ANNOUNCING_POLICY_INVALID_V01 = 0, /**<  Invalid type identifier \n
   */
  QMI_LTE_DISC_ANNOUNCING_POLICY_SHORT_V01 = 1, /**<  The authorised announcing range is set to: Short. \n
   */
  QMI_LTE_DISC_ANNOUNCING_POLICY_MEDIUM_V01 = 2, /**<  The authorised announcing range is set to: Medium. \n
   */
  QMI_LTE_DISC_ANNOUNCING_POLICY_LONG_V01 = 3, /**<  The authorised announcing range is set to: Long. \n
   */
  QMI_LTE_DISC_ANNOUNCING_POLICY_RESERVED_V01 = 4, /**<  4 - 255 : reserved for future use
   */
  QMI_LTE_DISC_ANNOUNCING_POLICY_RANGE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_lte_disc_announcing_policy_range_enum_v01;
/**
    @}
  */

/** @addtogroup lte_qmi_aggregates
    @{
  */
/**  Enum for announcing policy range
 */
typedef struct {

  /*  PLMN ID  - as per definition TS 23.003 */
  qmi_lte_disc_plmn_id_s_type_v01 PlmnId;

  /*  Validity Time T_4005 - as per definition TS 24.333 */
  uint32_t T4005_ValidityTime;
  /**<   This field indicates how long the monitoring authorisation policy
       is valid. \n
       - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 525600
  */

  /*  Range - as per definition TS 24.333 */
  qmi_lte_disc_announcing_policy_range_enum_v01 Range;
}qmi_lte_disc_announcing_policy_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lte_qmi_aggregates
    @{
  */
typedef struct {

  qmi_lte_disc_plmn_id_s_type_v01 plmn_id;
  /**<   PLMN ID  - as per definition TS 23.003
  */

  /*  Validity Time T_4005 - as per definition TS 24.333 */
  uint32_t T4005_ValidityTime;
  /**<   This field indicates how long the monitoring authorisation policy
       is valid. \n
       - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 525600
  */

  /*  Remaining Time left for monitoring validity */
  uint32_t RemainingTime;
  /**<   This field indicates how long the monitoring authorisation policy
       is valid. \n
       - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 525600
  */
}qmi_lte_disc_monitoring_policy_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; Client can register for specific indications from a service. */
typedef struct {

  /* Mandatory */
  /*  Indication register mask */
  qmi_lte_indication_reg_mask_t_v01 indication_bitmask;
  /**<   Set of all indications that the client wishes to receive from
         the service
    */
}qmi_lte_indication_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; Client can register for specific indications from a service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
        - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
        - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}qmi_lte_indication_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to provide the PLMNs authorized or provisoned by OMA-DM for
             LTE-D activity. */
typedef struct {

  /* Mandatory */
  /*  OS ID  */
  value_128_t_v01 OsId;
  /**<   It is Based on the type of OS - HLOS, iOS, Android, Windows etc.
       Input is a 128 bit encoded object - UUID type as per RFC 4122.
  */

  /* Optional */
  uint8_t DedicatedApnName_valid;  /**< Must be set to true if DedicatedApnName is being passed */
  char DedicatedApnName[QMI_LTE_DISC_DEDICATED_APN_NAME_MAX_V01 + 1];
  /**<   Optional field to specify the dedicatedAPNName if applicable
  */

  /* Optional */
  /*  AnnouncingPolicy - List of Announcing PLMNs provisioned by OMA-DM */
  uint8_t AnnouncingPolicyList_valid;  /**< Must be set to true if AnnouncingPolicyList is being passed */
  uint32_t AnnouncingPolicyList_len;  /**< Must be set to # of elements in AnnouncingPolicyList */
  qmi_lte_disc_announcing_policy_info_type_v01 AnnouncingPolicyList[QMI_LTE_DISC_ANNOUNCING_POLICY_LIST_MAX_V01];

  /* Optional */
  /*  MonitoringPolicy - List of Monitoring PLMNs provisioned by OMA-DM */
  uint8_t MonitoringPolicyList_valid;  /**< Must be set to true if MonitoringPolicyList is being passed */
  uint32_t MonitoringPolicyList_len;  /**< Must be set to # of elements in MonitoringPolicyList */
  qmi_lte_disc_monitoring_policy_info_type_v01 MonitoringPolicyList[QMI_LTE_DISC_MONITORING_POLICY_LIST_MAX_V01];

  /* Optional */
  /*  BAK Password for PSK */
  uint8_t BAKPassword_valid;  /**< Must be set to true if BAKPassword is being passed */
  uint32_t BAKPassword_len;  /**< Must be set to # of elements in BAKPassword */
  uint8_t BAKPassword[QMI_LTE_DISC_PSK_MAX_LEN_V01];
}qmi_lte_disc_set_lted_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to provide the PLMNs authorized or provisoned by OMA-DM for
             LTE-D activity. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}qmi_lte_disc_set_lted_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used for retrieve the authorization/provisioning information. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_lte_disc_get_lted_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used for retrieve the authorization/provisioning information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  uint8_t DedicatedApnName_valid;  /**< Must be set to true if DedicatedApnName is being passed */
  char DedicatedApnName[QMI_LTE_DISC_DEDICATED_APN_NAME_MAX_V01 + 1];
  /**<   Optional field to specify the dedicatedAPNName if applicable
  */

  /* Optional */
  /*  List of Announcing PLMNs provisioned by OMA-DM */
  uint8_t AnnouncingPolicy_valid;  /**< Must be set to true if AnnouncingPolicy is being passed */
  uint32_t AnnouncingPolicy_len;  /**< Must be set to # of elements in AnnouncingPolicy */
  qmi_lte_disc_announcing_policy_info_type_v01 AnnouncingPolicy[QMI_LTE_DISC_ANNOUNCING_POLICY_LIST_MAX_V01];

  /* Optional */
  /*  List of Monitoring PLMNs provisioned by OMA-DM */
  uint8_t MonitoringPolicy_valid;  /**< Must be set to true if MonitoringPolicy is being passed */
  uint32_t MonitoringPolicy_len;  /**< Must be set to # of elements in MonitoringPolicy */
  qmi_lte_disc_monitoring_policy_info_type_v01 MonitoringPolicy[QMI_LTE_DISC_MONITORING_POLICY_LIST_MAX_V01];
}qmi_lte_disc_get_lted_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used for set the LTED category for each application. */
typedef struct {

  /* Optional */
  /*  OS App ID for which the lteDcategory is to be set */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  LteD Category - to determine the transmission priority of each OSAppID */
  uint8_t lted_category_valid;  /**< Must be set to true if lted_category is being passed */
  qmi_lte_disc_category_type_enum_v01 lted_category;
}qmi_lte_disc_set_lted_category_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used for set the LTED category for each application. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}qmi_lte_disc_set_lted_category_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to retrieve the LTED category per OSAppID. */
typedef struct {

  /* Optional */
  /*  OS App ID for which the lteDcategory is to be set */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];
}qmi_lte_disc_get_lted_category_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to retrieve the LTED category per OSAppID. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  LteD Category - to determine the transmission priority of each OSAppID */
  uint8_t lted_category_valid;  /**< Must be set to true if lted_category is being passed */
  qmi_lte_disc_category_type_enum_v01 lted_category;
}qmi_lte_disc_get_lted_category_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to terminate all processing for a specific application. */
typedef struct {

  /* Mandatory */
  /*  OS App ID for to identify which App on the client side needs to operate */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];
}qmi_lte_disc_terminate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to terminate all processing for a specific application. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}qmi_lte_disc_terminate_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to retrieve if LTED tx\rx is currently allowed or not. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_lte_disc_get_service_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to retrieve if LTED tx\rx is currently allowed or not. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  Announcing service capability */
  uint8_t PublishAllowed_valid;  /**< Must be set to true if PublishAllowed is being passed */
  uint8_t PublishAllowed;
  /**<   Indicates whether UE is currently capable of announcing/publishing
         a PAC or not
         0x00 (FALSE) -- Currently we are out of announcing coverage
         0x01 (TRUE) -- Curently we are in announcing coverage
    */

  /* Optional */
  /*  Monitoring service capability */
  uint8_t SubscribeAllowed_valid;  /**< Must be set to true if SubscribeAllowed is being passed */
  uint8_t SubscribeAllowed;
  /**<   Indicates whether UE is currently capable of monitoring a PAC or not
         0x00 (FALSE) -- Currently we are out of monitoring coverage
         0x01 (TRUE) -- Curently we are in monitoring coverage
    */
}qmi_lte_disc_get_service_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to specify the expression that the App wishes to publish */
typedef struct {

  /* Mandatory */
  /*  OS App ID for to identify which App on the client side needs to operate */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Discovery Type - Open or restricted */
  uint8_t DiscoveryType_valid;  /**< Must be set to true if DiscoveryType is being passed */
  qmi_lte_discovery_type_enum_v01 DiscoveryType;

  /* Optional */
  /*  Expression or PA ID to be published/subscribed */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Max time for which the announcement needs to happen */
  uint8_t PublishDuration_valid;  /**< Must be set to true if PublishDuration is being passed */
  uint32_t PublishDuration;
  /**<   - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1 min
       - Maximum value: 30 min
       - Default value: 15 min
  */

  /* Optional */
  /*  Max time for which the expression is valid */
  uint8_t ExpressionValidityTime_valid;  /**< Must be set to true if ExpressionValidityTime is being passed */
  uint32_t ExpressionValidityTime;
  /**<   - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 525600 (1 year)
       - Default value: not needed - governed by T 4005
  */

  /* Optional */
  /*  MetaData transmitted by the UE along with the PAC code */
  uint8_t MetaData_valid;  /**< Must be set to true if MetaData is being passed */
  char MetaData[QMI_LTE_DISC_METADATA_MAX_V01 + 1];
}qmi_lte_disc_publish_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to specify the expression that the App wishes to publish */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Expression or PA ID */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Result of the corresponding action for the expression */
  uint8_t ExpressionResult_valid;  /**< Must be set to true if ExpressionResult is being passed */
  qmi_lte_disc_result_type_enum_v01 ExpressionResult;
}qmi_lte_disc_publish_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to specify the expression that the App wishes to subscribe */
typedef struct {

  /* Mandatory */
  /*  OS App Id */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Discovery Type - Open or restricted */
  uint8_t DiscoveryType_valid;  /**< Must be set to true if DiscoveryType is being passed */
  qmi_lte_discovery_type_enum_v01 DiscoveryType;

  /* Optional */
  /*  Expression or PA ID to be published/subscribed */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Max time for which the announcement needs to happen */
  uint8_t SubscribeDuration_valid;  /**< Must be set to true if SubscribeDuration is being passed */
  uint32_t SubscribeDuration;
  /**<   - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 30 min
       - Default value: 15 min
  */

  /* Optional */
  /*  Max time for which the expression is valid */
  uint8_t ExpressionValidityTime_valid;  /**< Must be set to true if ExpressionValidityTime is being passed */
  uint32_t ExpressionValidityTime;
  /**<   - Units: minutes \n
       - Occurence: Once
       - Minimum value: 1
       - Maximum value: 525600 (1 year)
       - Default value: not needed - governed by T 4005
  */
}qmi_lte_disc_subscribe_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to specify the expression that the App wishes to subscribe */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Expression or PA ID */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Result of the corresponding action for the expression */
  uint8_t ExpressionResult_valid;  /**< Must be set to true if ExpressionResult is being passed */
  qmi_lte_disc_result_type_enum_v01 ExpressionResult;
}qmi_lte_disc_subscribe_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to specify which expressions the OsAppId wants to stop publishing */
typedef struct {

  /* Mandatory */
  /*  OS App ID for to identify which App on the client side needs to operate */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Expression or PA ID */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;
}qmi_lte_disc_cancel_publish_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to specify which expressions the OsAppId wants to stop publishing */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];
}qmi_lte_disc_cancel_publish_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; It is used to specify which expressions the OsAppId wants to stop monitoring */
typedef struct {

  /* Mandatory */
  /*  OS App ID for to identify which App on the client side needs to operate */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Expression or PA ID */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;
}qmi_lte_disc_cancel_subscribe_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; It is used to specify which expressions the OsAppId wants to stop monitoring */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];
}qmi_lte_disc_cancel_subscribe_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Indication Message; It is used to specify current LTED coverage i.e. if UE is allowed to
             do tx/rx operations or not. */
typedef struct {

  /* Optional */
  /*  Announcing coverage capability */
  uint8_t PublishAllowed_valid;  /**< Must be set to true if PublishAllowed is being passed */
  uint8_t PublishAllowed;
  /**<   Indicates whether UE is currently capable of announcing/publishing
         a PAC or not
         0x00 (FALSE) -- Currently we are out of announcing coverage
         0x01 (TRUE) -- Curently we are in announcing coverage
    */

  /* Optional */
  /*  Monitoring coverage capability */
  uint8_t SubscribeAllowed_valid;  /**< Must be set to true if SubscribeAllowed is being passed */
  uint8_t SubscribeAllowed;
  /**<   Indicates whether UE is currently capable of monitoring a PAC or not
         0x00 (FALSE) -- Currently we are out of monitoring coverage
         0x01 (TRUE) -- Curently we are in monitoring coverage
    */
}qmi_lte_disc_broadcast_notification_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Indication Message; It is used to specify match event details */
typedef struct {

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Expression or PA ID to be publishd/sunscribed */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Expression of the MATCHED PA ID that was published */
  uint8_t MatchedExpression_valid;  /**< Must be set to true if MatchedExpression is being passed */
  qmi_lte_disc_prose_app_id_v01 MatchedExpression;

  /* Optional */
  /*  State indication for each match event */
  uint8_t MatchEventState_valid;  /**< Must be set to true if MatchEventState is being passed */
  uint8_t MatchEventState;
  /**<   Indicates whether Match event is for start or end
         0x00 (FALSE) -- Match start
         0x01 (TRUE) -- Match End
    */

  /* Optional */
  /*  Index value to look for changes in MetaData field */
  uint8_t MetaDataIndex_valid;  /**< Must be set to true if MetaDataIndex is being passed */
  uint16_t MetaDataIndex;

  /* Optional */
  /*  MetaData received/transmitted by the UE along with the PAC code */
  uint8_t MetaData_valid;  /**< Must be set to true if MetaData is being passed */
  char MetaData[QMI_LTE_DISC_METADATA_MAX_V01 + 1];
}qmi_lte_disc_match_notification_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Indication Message; It is used to report error or various statuss and event notifications. */
typedef struct {

  /* Optional */
  /*  OS App ID for to identify which App on the client side needs to operate */
  uint8_t OsAppId_valid;  /**< Must be set to true if OsAppId is being passed */
  char OsAppId[QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01 + 1];

  /* Optional */
  /*  Authorization result for each OsAppId   */
  uint8_t AuthorizationResult_valid;  /**< Must be set to true if AuthorizationResult is being passed */
  qmi_lte_disc_result_type_enum_v01 AuthorizationResult;

  /* Optional */
  /*  Expression or PA ID */
  uint8_t Expression_valid;  /**< Must be set to true if Expression is being passed */
  qmi_lte_disc_prose_app_id_v01 Expression;

  /* Optional */
  /*  Result of the corresponding action for the expression */
  uint8_t ExpressionResult_valid;  /**< Must be set to true if ExpressionResult is being passed */
  qmi_lte_disc_result_type_enum_v01 ExpressionResult;

  /* Optional */
  /*  Activity status (for Publish) for each expression */
  uint8_t PublishStatusPerExpr_valid;  /**< Must be set to true if PublishStatusPerExpr is being passed */
  uint8_t PublishStatusPerExpr;
  /**<   To indicate pending status of publishing or monitoring the expression
         0x00 (FALSE) -- Transmission is completed
         0x01 (TRUE) -- pendingTransmission
                        Sent periodically while transmission is still pending
    */

  /* Optional */
  /*  Activity status (for Subscribe) for each expression */
  uint8_t SubscribeStatusPerExpr_valid;  /**< Must be set to true if SubscribeStatusPerExpr is being passed */
  uint8_t SubscribeStatusPerExpr;
  /**<   To indicate pending status of publishing or monitoring the expression
         0x00 (FALSE) -- Transmission/Subscription is completed
         0x01 (TRUE) -- pendingSubscription
                        Sent periodically while transmission is still pending
    */
}qmi_lte_disc_notification_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_enums
    @{
  */
typedef enum {
  QMI_LTE_BIND_SUBSCRIPTION_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LTE_DEFAULT_SUBSCRIPTION_V01 = 0x0000, /**<  Default data subscription \n  */
  LTE_PRIMARY_SUBSCRIPTION_V01 = 0x0001, /**<  Primary \n  */
  LTE_SECONDARY_SUBSCRIPTION_V01 = 0x0002, /**<  Secondary \n  */
  LTE_TERTIARY_SUBSCRIPTION_V01 = 0x0003, /**<  Tertiary \n  */
  LTE_SUBSCRIPTION_MAX_V01 = 0x00FF, /**<  Default value used in the absence of
       explicit binding */
  QMI_LTE_BIND_SUBSCRIPTION_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_lte_bind_subscription_type_enum_v01;
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; Binds the client to the specified subscription of the service */
typedef struct {

  /* Mandatory */
  /*  Subscription Identifier */
  qmi_lte_bind_subscription_type_enum_v01 subscription;
  /**<   Subscription to which the client is bound.
 Values: \n
      - LTE_DEFAULT_SUBSCRIPTION (0x0000) --  Default data subscription \n
      - LTE_PRIMARY_SUBSCRIPTION (0x0001) --  Primary \n
      - LTE_SECONDARY_SUBSCRIPTION (0x0002) --  Secondary \n
      - LTE_TERTIARY_SUBSCRIPTION (0x0003) --  Tertiary \n
      - LTE_SUBSCRIPTION_MAX (0x00FF) --  Default value used in the absence of
       explicit binding
 */
}qmi_lte_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; Binds the client to the specified subscription of the service */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}qmi_lte_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Request Message; Queries for the current subscription information of the client. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_lte_get_subscription_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Response Message; Queries for the current subscription information of the client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Binded Subscription */
  uint8_t subscription_valid;  /**< Must be set to true if subscription is being passed */
  qmi_lte_bind_subscription_type_enum_v01 subscription;
  /**<   Subscription to which the client is bound.
 Values: \n
      - LTE_DEFAULT_SUBSCRIPTION (0x0000) --  Default data subscription \n
      - LTE_PRIMARY_SUBSCRIPTION (0x0001) --  Primary \n
      - LTE_SECONDARY_SUBSCRIPTION (0x0002) --  Secondary \n
      - LTE_TERTIARY_SUBSCRIPTION (0x0003) --  Tertiary \n
      - LTE_SUBSCRIPTION_MAX (0x00FF) --  Default value used in the absence of
       explicit binding
 */

  /* Optional */
  /*  LTED Capability for the specific subscription */
  uint8_t LteDiscCapability_valid;  /**< Must be set to true if LteDiscCapability is being passed */
  uint8_t LteDiscCapability;
  /**<   Indicates if the specific subscription is LTE-D capable or not
  */
}qmi_lte_get_subscription_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Indication Message; Updates Subscription specific details */
typedef struct {

  /* Optional */
  /*  Binded Subscription */
  uint8_t subscription_valid;  /**< Must be set to true if subscription is being passed */
  qmi_lte_bind_subscription_type_enum_v01 subscription;
  /**<   Subscription to which the client is bound.
 Values: \n
      - LTE_DEFAULT_SUBSCRIPTION (0x0000) --  Default data subscription \n
      - LTE_PRIMARY_SUBSCRIPTION (0x0001) --  Primary \n
      - LTE_SECONDARY_SUBSCRIPTION (0x0002) --  Secondary \n
      - LTE_TERTIARY_SUBSCRIPTION (0x0003) --  Tertiary \n
      - LTE_SUBSCRIPTION_MAX (0x00FF) --  Default value used in the absence of
       explicit binding
 */

  /* Optional */
  /*  LTED Capability for the specific subscription */
  uint8_t LteDiscCapability_valid;  /**< Must be set to true if LteDiscCapability is being passed */
  uint8_t LteDiscCapability;
  /**<   Indicates is the specific subscription is LTE-D capable or not
  */
}qmi_lte_subscription_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lte_qmi_messages
    @{
  */
/** Indication Message; It is used to indicate that the BAK password (PSK) in provisioning req
    has expired and is no longer valid */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_lte_disc_psk_expired_ind_msg_v01;

  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_LTE_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_LTE_DISC_BROADCAST_NOTIFICATION_V01
//#define REMOVE_QMI_LTE_DISC_CANCEL_PUBLISH_V01
//#define REMOVE_QMI_LTE_DISC_CANCEL_SUBSCRIBE_V01
//#define REMOVE_QMI_LTE_DISC_GET_LTED_CATEGORY_V01
//#define REMOVE_QMI_LTE_DISC_GET_LTED_CONFIG_V01
//#define REMOVE_QMI_LTE_DISC_GET_SERVICE_STATUS_V01
//#define REMOVE_QMI_LTE_DISC_MATCH_NOTIFICATION_V01
//#define REMOVE_QMI_LTE_DISC_NOTIFICATION_V01
//#define REMOVE_QMI_LTE_DISC_PSK_EXPIRED_V01
//#define REMOVE_QMI_LTE_DISC_PUBLISH_V01
//#define REMOVE_QMI_LTE_DISC_SET_LTED_CATEGORY_V01
//#define REMOVE_QMI_LTE_DISC_SET_LTED_CONFIG_V01
//#define REMOVE_QMI_LTE_DISC_SUBSCRIBE_V01
//#define REMOVE_QMI_LTE_DISC_TERMINATE_V01
//#define REMOVE_QMI_LTE_GET_SUBSCRIPTION_INFO_V01
//#define REMOVE_QMI_LTE_INDICATION_REGISTER_V01
//#define REMOVE_QMI_LTE_SUBSCRIPTION_INFO_V01

/*Service Message Definition*/
/** @addtogroup lte_qmi_msg_ids
    @{
  */
#define QMI_LTE_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_LTE_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_LTE_DISC_SET_LTED_CONFIG_REQ_V01 0x0021
#define QMI_LTE_DISC_SET_LTED_CONFIG_RESP_V01 0x0021
#define QMI_LTE_DISC_GET_LTED_CONFIG_REQ_V01 0x0022
#define QMI_LTE_DISC_GET_LTED_CONFIG_RESP_V01 0x0022
#define QMI_LTE_DISC_SET_LTED_CATEGORY_REQ_V01 0x0023
#define QMI_LTE_DISC_SET_LTED_CATEGORY_RESP_V01 0x0023
#define QMI_LTE_DISC_GET_CATEGORY_REQ_V01 0x0024
#define QMI_LTE_DISC_GET_CATEGORY_RESP_V01 0x0024
#define QMI_LTE_DISC_TERMINATE_REQ_V01 0x0025
#define QMI_LTE_DISC_TERMINATE_RESP_V01 0x0025
#define QMI_LTE_DISC_GET_SERVICE_STATUS_REQ_V01 0x0026
#define QMI_LTE_DISC_GET_SERVICE_STATUS_RESP_V01 0x0026
#define QMI_LTE_DISC_PUBLISH_REQ_V01 0x0027
#define QMI_LTE_DISC_PUBLISH_RESP_V01 0x0027
#define QMI_LTE_DISC_SUBSCRIBE_REQ_V01 0x0028
#define QMI_LTE_DISC_SUBSCRIBE_RESP_V01 0x0028
#define QMI_LTE_DISC_CANCEL_PUBLISH_REQ_V01 0x0029
#define QMI_LTE_DISC_CANCEL_PUBLISH_RESP_V01 0x0029
#define QMI_LTE_DISC_CANCEL_SUBSCRIBE_REQ_V01 0x002A
#define QMI_LTE_DISC_CANCEL_SUBSCRIBE_RESP_V01 0x002A
#define QMI_LTE_DISC_NOTIFICATION_IND_V01 0x002B
#define QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_V01 0x002C
#define QMI_LTE_DISC_MATCH_NOTIFICATION_IND_V01 0x002D
#define QMI_LTE_BIND_SUBSCRIPTION_REQ_V01 0x002E
#define QMI_LTE_BIND_SUBSCRIPTION_RESP_V01 0x002E
#define QMI_LTE_GET_SUBSCRIPTION_INFO_REQ_V01 0x002F
#define QMI_LTE_GET_SUBSCRIPTION_INFO_RESP_V01 0x002F
#define QMI_LTE_SUBSCRIPTION_INFO_IND_V01 0x0030
#define QMI_LTE_DISC_PSK_EXPIRED_IND_V01 0x0031
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro lte_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type lte_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define lte_get_service_object_v01( ) \
          lte_get_service_object_internal_v01( \
            LTE_V01_IDL_MAJOR_VERS, LTE_V01_IDL_MINOR_VERS, \
            LTE_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

