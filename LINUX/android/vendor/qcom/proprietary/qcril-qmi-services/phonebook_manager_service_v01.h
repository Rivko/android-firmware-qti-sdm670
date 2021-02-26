#ifndef PBM_SERVICE_01_H
#define PBM_SERVICE_01_H
/**
  @file phonebook_manager_service_v01.h

  @brief This is the public header file which defines the pbm service Data structures.

  This header file defines the types and structures that were defined in
  pbm. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2010-2014, 2016 - 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.7/pbm/api/phonebook_manager_service_v01.h#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Apr 13 2017 (Spin 0)
   From IDL File: phonebook_manager_service_v01.idl */

/** @defgroup pbm_qmi_consts Constant values defined in the IDL */
/** @defgroup pbm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup pbm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup pbm_qmi_messages Structures sent as QMI messages */
/** @defgroup pbm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup pbm_qmi_accessor Accessor for QMI service object */
/** @defgroup pbm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup pbm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define PBM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define PBM_V01_IDL_MINOR_VERS 0x13
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define PBM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define PBM_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup pbm_qmi_consts
    @{
  */
#define QMI_PBM_MAX_PB_SETS_V01 4
#define QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01 4
#define QMI_PBM_MAX_NUM_SESSIONS_V01 6
#define QMI_PBM_MAX_NUM_PBS_V01 8
#define QMI_PBM_NUMBER_MAX_LENGTH_V01 40
#define QMI_PBM_NAME_MAX_LENGTH_V01 241
#define QMI_PBM_NAME_MAX_LENGTH_EXT_V01 255
#define QMI_PBM_AD_NUM_MAX_V01 4
#define QMI_PBM_GRP_ID_MAX_V01 10
#define QMI_PBM_EMAIL_INFO_MAX_V01 2
#define QMI_PBM_EMAIL_MAX_LENGTH_V01 241
#define QMI_PBM_EMAIL_MAX_LENGTH_EXT_V01 255
#define QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01 10
#define QMI_PBM_MAX_NUM_SNAME_INFO_V01 10
#define QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01 10
#define QMI_PBM_MAX_NUM_AD_NUM_COUNT_V01 4
#define QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01 10
#define QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01 10
#define QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01 10
#define QMI_PBM_REC_IDS_MAX_V01 1000
#define QMI_PBM_EMER_NUM_MAX_LENGTH_V01 6
#define QMI_PBM_MAX_NUM_EMER_NUM_V01 10
#define QMI_PBM_MAX_NUM_NV_EMER_NUM_V01 11
#define QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01 50
#define QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01 50
#define QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01 6
#define QMI_PBM_MAX_NUM_CARD_ECC_EMER_NUM_V01 255
#define QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01 6
#define QMI_PBM_MAX_NUM_NETWORK_ECC_EMER_NUM_V01 16
#define QMI_PBM_MAX_NUM_MCC_EMER_NUM_V01 20
#define QMI_PBM_MAX_NUM_SLOT_V01 3
#define QMI_PBM_EMER_CAT_POLICE_BIT_V01 0
#define QMI_PBM_EMER_CAT_AMBULANCE_BIT_V01 1
#define QMI_PBM_EMER_CAT_FIRE_BRIGADE_BIT_V01 2
#define QMI_PBM_EMER_CAT_MARINE_GUARD_BIT_V01 3
#define QMI_PBM_EMER_CAT_MOUNTAIN_RESCUE_BIT_V01 4
#define QMI_PBM_EMER_CAT_MANUAL_ECALL_BIT_V01 5
#define QMI_PBM_EMER_CAT_AUTO_ECALL_BIT_V01 6
#define QMI_PBM_EMER_CAT_SPARE_BIT_V01 7
#define QMI_PBM_MAX_NUM_PHONEBOOK_READY_INFO_V01 6
#define QMI_PBM_MAX_NUM_USIM_FIELDS_IN_RECORD_V01 20
#define QMI_PBM_MAX_NUM_GROUPS_DATA_V01 6
#define QMI_PBM_MAX_NUM_GROUP_COUNT_V01 40
#define QMI_PBM_GROUP_NAME_MAX_LENGTH_V01 241
#define QMI_PBM_MAX_NUM_AAS_COUNT_V01 40
#define QMI_PBM_AAS_NAME_MAX_LENGTH_V01 241
#define QMI_PBM_MAX_NUM_PBSET_CAP_INSTANCE_V01 10
#define QMI_PBM_NAME_MAX_LENGTH_EXT_UNDECODED_V01 255
#define QMI_PBM_EMAIL_MAX_LENGTH_EXT_UNDECODED_V01 255
/**
    @}
  */

typedef uint32_t pbm_reg_mask_type_v01;
#define PBM_REG_RECORD_UPDATE_EVENTS_V01 ((pbm_reg_mask_type_v01)0x01) /**<  Record update events  */
#define PBM_REG_PHONEBOOK_READY_EVENTS_V01 ((pbm_reg_mask_type_v01)0x02) /**<  Phonebook ready events  */
#define PBM_REG_EMERGENCY_NUMBER_LIST_EVENTS_V01 ((pbm_reg_mask_type_v01)0x04) /**<  Emergency number list events  */
#define PBM_REG_HIDDEN_RECORD_STATUS_EVENTS_V01 ((pbm_reg_mask_type_v01)0x08) /**<  Hidden Record Status events  */
#define PBM_REG_AAS_UPDATE_EVENTS_V01 ((pbm_reg_mask_type_v01)0x10) /**<  Additional number alpha string update events  */
#define PBM_REG_GAS_UPDATE_EVENTS_V01 ((pbm_reg_mask_type_v01)0x20) /**<  Grouping information alpha string update events  */
/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Sets the registration state for different QMI_PBM indications
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Event Registration Mask */
  pbm_reg_mask_type_v01 reg_mask;
  /**<   Bitmask of the events to be registered: \n
      - PBM_REG_RECORD_UPDATE_EVENTS (0x01) --  Record update events 
      - PBM_REG_PHONEBOOK_READY_EVENTS (0x02) --  Phonebook ready events 
      - PBM_REG_EMERGENCY_NUMBER_LIST_EVENTS (0x04) --  Emergency number list events 
      - PBM_REG_HIDDEN_RECORD_STATUS_EVENTS (0x08) --  Hidden Record Status events 
      - PBM_REG_AAS_UPDATE_EVENTS (0x10) --  Additional number alpha string update events 
      - PBM_REG_GAS_UPDATE_EVENTS (0x20) --  Grouping information alpha string update events 
 */
}pbm_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Sets the registration state for different QMI_PBM indications
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Event Registration Mask */
  uint8_t reg_mask_valid;  /**< Must be set to true if reg_mask is being passed */
  pbm_reg_mask_type_v01 reg_mask;
  /**<   Bitmask of the events to be registered: \n
      - PBM_REG_RECORD_UPDATE_EVENTS (0x01) --  Record update events 
      - PBM_REG_PHONEBOOK_READY_EVENTS (0x02) --  Phonebook ready events 
      - PBM_REG_EMERGENCY_NUMBER_LIST_EVENTS (0x04) --  Emergency number list events 
      - PBM_REG_HIDDEN_RECORD_STATUS_EVENTS (0x08) --  Hidden Record Status events 
      - PBM_REG_AAS_UPDATE_EVENTS (0x10) --  Additional number alpha string update events 
      - PBM_REG_GAS_UPDATE_EVENTS (0x20) --  Grouping information alpha string update events 
 */
}pbm_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_SESSION_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_SESSION_TYPE_GW_PRIMARY_V01 = 0x00, /**<  GSM/WCDMA primary session  */
  PBM_SESSION_TYPE_1X_PRIMARY_V01 = 0x01, /**<  1x primary session  */
  PBM_SESSION_TYPE_GW_SECONDARY_V01 = 0x02, /**<  GSM/WCDMA secondary session  */
  PBM_SESSION_TYPE_1X_SECONDARY_V01 = 0x03, /**<  1x secondary session  */
  PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1_V01 = 0x04, /**<  Nonprovisioning session on slot 1  */
  PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2_V01 = 0x05, /**<  Nonprovisioning session on slot 2  */
  PBM_SESSION_TYPE_GLOBAL_PB_SLOT1_V01 = 0x06, /**<  Global phonebook session on slot 1  */
  PBM_SESSION_TYPE_GLOBAL_PB_SLOT2_V01 = 0x07, /**<  Global phonebook session on slot 2  */
  PBM_SESSION_TYPE_GW_TERTIARY_V01 = 0x08, /**<  GSM/WCDMA tertiary session  */
  PBM_SESSION_TYPE_1X_TERTIARY_V01 = 0x09, /**<  1x tertiary session  */
  PBM_SESSION_TYPE_GLOBAL_PB_SLOT3_V01 = 0x0A, /**<  Global phonebook session on slot 3  */
  PBM_SESSION_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_session_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */
}pbm_phonebook_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns the capabilities of the PB requested. */
typedef struct {

  /* Mandatory */
  /*  Phonebook Information */
  pbm_phonebook_info_type_v01 phonebook_info;
}pbm_get_pb_capabilities_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t used_records;
  /**<   Records used.*/

  uint16_t max_records;
  /**<   Maximum possible records for this phonebook.*/

  uint8_t max_num_len;
  /**<   Maximum number length.*/

  uint8_t max_name_len;
  /**<   Maximum name length.*/
}pbm_capability_basic_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_grp;
  /**<   Maximum groups possible.*/

  uint8_t max_grp_tag_len;
  /**<   Maximum grouping information alpha string length.*/
}pbm_group_capability_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_ad_num;
  /**<   Maximum additional numbers possible.*/

  uint8_t max_ad_num_len;
  /**<   Maximum additional number length.*/

  uint8_t max_ad_num_tag_len;
  /**<   Maximum additional number alpha string length.*/
}pbm_ad_num_capability_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_email;
  /**<   Maximum emails possible.*/

  uint8_t max_email_len;
  /**<   Maximum email address length.*/
}pbm_email_capability_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_records;
  /**<   Maximum Grouping information Alpha String (GAS) records possible.*/

  uint8_t used_records;
  /**<   GAS records used.*/

  uint8_t max_gas_string_len;
  /**<   Maximum GAS string length.*/
}pbm_gas_capability_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_records;
  /**<   Maximum Additional number Alpha String (AAS) records possible.*/

  uint8_t used_records;
  /**<   AAS records used.*/

  uint8_t max_aas_string_len;
  /**<   Maximum AAS string length.*/
}pbm_aas_capability_type_v01;  /* Type */
/**
    @}
  */

typedef uint64_t pbm_pin_mask_type_v01;
#define PBM_CARD_PIN1_V01 ((pbm_pin_mask_type_v01)0x01ull) /**<  Phonebook protected by PIN1  */
#define PBM_CARD_PIN2_V01 ((pbm_pin_mask_type_v01)0x02ull) /**<  Phonebook protected by PIN2  */
#define PBM_CARD_ADM1_V01 ((pbm_pin_mask_type_v01)0x04ull) /**<  Phonebook protected by ADM1  */
#define PBM_CARD_ADM2_V01 ((pbm_pin_mask_type_v01)0x08ull) /**<  Phonebook protected by ADM2  */
/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_PROTECTION_METHOD_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_ACCESS_ALLOWED_V01 = 0, /**<  Access is allowed. If the method is PBM_ACCESS_ALLOWED,
       the pin_mask is insignificant.  */
  PBM_ACCESS_NEVER_ALLOWED_V01 = 1, /**<  Access is never allowed.  */
  PBM_ACCESS_AND_ALLOWED_V01 = 2, /**<  Access is allowed only when all PINs are verified.  */
  PBM_ACCESS_OR_ALLOWED_V01 = 3, /**<  Access is allowed when any PIN is verified.  */
  PBM_ACCESS_SINGLE_ALLOWED_V01 = 4, /**<  Access is allowed when one PIN is verified.  */
  PBM_PROTECTION_METHOD_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_protection_method_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_protection_method_v01 protection_method;
  /**<   Protection method. Values: \n
      - PBM_ACCESS_ALLOWED (0) --  Access is allowed. If the method is PBM_ACCESS_ALLOWED,
       the pin_mask is insignificant. 
      - PBM_ACCESS_NEVER_ALLOWED (1) --  Access is never allowed. 
      - PBM_ACCESS_AND_ALLOWED (2) --  Access is allowed only when all PINs are verified. 
      - PBM_ACCESS_OR_ALLOWED (3) --  Access is allowed when any PIN is verified. 
      - PBM_ACCESS_SINGLE_ALLOWED (4) --  Access is allowed when one PIN is verified. 
 */

  pbm_pin_mask_type_v01 pin_mask;
  /**<   Bitmask of the different SIM PIN to be verified: \n
      - PBM_CARD_PIN1 (0x01) --  Phonebook protected by PIN1 
      - PBM_CARD_PIN2 (0x02) --  Phonebook protected by PIN2 
      - PBM_CARD_ADM1 (0x04) --  Phonebook protected by ADM1 
      - PBM_CARD_ADM2 (0x08) --  Phonebook protected by ADM2 
 */
}pbm_access_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_adn_records;
  /**<   Maximum possible records for this phonebook set.*/

  uint8_t used_adn_records;
  /**<   Records used in this phonebook set.*/

  uint8_t max_adn_num_len;
  /**<   Maximum number length for this phonebook set.*/

  uint8_t max_adn_name_len;
  /**<   Maximum name length for this phonebook set.*/
}pbm_adn_caps_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t max_records;
  /**<   Maximum records possible in this phonebook set.*/

  uint8_t used_records;
  /**<   Used records in this phonebook set.*/

  uint8_t max_record_len;
  /**<   Maximum record length.*/
}pbm_record_capability_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_record_capability_ext_type_v01 record_caps;
  /**<   Record-related capabilities, such as maximum records, used records, and record length for the USIM file.*/

  uint8_t mapping_type;
  /**<   Mapping between this USIM field and the ADN. */
}pbm_usim_capability_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t pb_set_id;
  /**<   Identification of the phonebook set.*/

  pbm_adn_caps_info_v01 adn_caps;
  /**<   ADN capabilities in this phonebook set.*/

  uint32_t email_caps_len;  /**< Must be set to # of elements in email_caps */
  pbm_usim_capability_ext_type_v01 email_caps[QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01];
  /**<   Email capabilities in this phonebook set.*/

  uint32_t ad_num_caps_len;  /**< Must be set to # of elements in ad_num_caps */
  pbm_usim_capability_ext_type_v01 ad_num_caps[QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01];
  /**<   Additional number capabilities in this phonebook set.*/

  uint32_t sne_caps_len;  /**< Must be set to # of elements in sne_caps */
  pbm_usim_capability_ext_type_v01 sne_caps[QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01];
  /**<   Secondary name capabilities in this phonebook set.*/

  uint32_t grp_caps_len;  /**< Must be set to # of elements in grp_caps */
  pbm_usim_capability_ext_type_v01 grp_caps[QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01];
  /**<   Group capabilities in this phonebook set.*/
}pbm_pb_set_cap_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t num_of_pb_sets;
  /**<   Number of phonebook sets.*/

  pbm_record_capability_ext_type_v01 aas_caps;
  /**<   AAS capabilities. */

  pbm_record_capability_ext_type_v01 gas_caps;
  /**<   GAS capabilities. */

  uint32_t pbset_caps_len;  /**< Must be set to # of elements in pbset_caps */
  pbm_pb_set_cap_info_type_v01 pbset_caps[QMI_PBM_MAX_PB_SETS_V01];
  /**<   ADN and USIM capabilities. */
}pbm_pbr_cap_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns the capabilities of the PB requested. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Capability Basic Information */
  uint8_t capability_basic_info_valid;  /**< Must be set to true if capability_basic_info is being passed */
  pbm_capability_basic_info_type_v01 capability_basic_info;

  /* Optional */
  /*  Group Capability */
  uint8_t group_capability_valid;  /**< Must be set to true if group_capability is being passed */
  pbm_group_capability_type_v01 group_capability;

  /* Optional */
  /*  Additional Number Capability */
  uint8_t ad_num_capability_valid;  /**< Must be set to true if ad_num_capability is being passed */
  pbm_ad_num_capability_type_v01 ad_num_capability;

  /* Optional */
  /*  Email Capability */
  uint8_t email_capability_valid;  /**< Must be set to true if email_capability is being passed */
  pbm_email_capability_type_v01 email_capability;

  /* Optional */
  /*  Second Name Capability */
  uint8_t max_second_name_len_valid;  /**< Must be set to true if max_second_name_len is being passed */
  uint8_t max_second_name_len;
  /**<   Maximum length of second name.*/

  /* Optional */
  /*  Hidden Records Capability */
  uint8_t is_hidden_entry_supported_valid;  /**< Must be set to true if is_hidden_entry_supported is being passed */
  uint8_t is_hidden_entry_supported;
  /**<   Whether hidden entry is supported: \n
         - 0 -- FALSE \n
         - 1 -- TRUE */

  /* Optional */
  /*  Grouping Information Alpha String Capability */
  uint8_t gas_capability_valid;  /**< Must be set to true if gas_capability is being passed */
  pbm_gas_capability_type_v01 gas_capability;

  /* Optional */
  /*  Additional Number Alpha String Capability */
  uint8_t aas_capability_valid;  /**< Must be set to true if aas_capability is being passed */
  pbm_aas_capability_type_v01 aas_capability;

  /* Optional */
  /*  Phonebook Write Access Information */
  uint8_t write_access_info_valid;  /**< Must be set to true if write_access_info is being passed */
  pbm_access_info_type_v01 write_access_info;

  /* Optional */
  /*  Number of Phonebook Sets */
  uint8_t num_pb_sets_valid;  /**< Must be set to true if num_pb_sets is being passed */
  uint16_t num_pb_sets;
  /**<   Number of phonebook sets. */

  /* Optional */
  /*  Phonebook Set Capability Information  */
  uint8_t adn_pb_set_info_valid;  /**< Must be set to true if adn_pb_set_info is being passed */
  pbm_pbr_cap_info_type_v01 adn_pb_set_info;
}pbm_get_pb_capabilities_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns the capabilities of the PBs for all available
             sessions. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pbm_get_all_pb_capabilities_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t used_records;
  /**<   Records used.*/

  uint16_t max_records;
  /**<   Maximum possible records for this phonebook.*/

  uint8_t max_num_len;
  /**<   Maximum number length.*/

  uint8_t max_name_len;
  /**<   Maximum name length.*/
}pbm_capability_basic_info_array_elem_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint32_t capability_basic_info_len;  /**< Must be set to # of elements in capability_basic_info */
  pbm_capability_basic_info_array_elem_type_v01 capability_basic_info[QMI_PBM_MAX_NUM_PBS_V01];
}pbm_capability_basic_info_array_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_grp;
  /**<   Maximum groups possible.*/

  uint8_t max_grp_tag_len;
  /**<   Maximum grouping information alpha string length.*/
}pbm_group_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_ad_num;
  /**<   Maximum additional numbers possible.*/

  uint8_t max_ad_num_len;
  /**<   Maximum additional number length.*/

  uint8_t max_ad_num_tag_len;
  /**<   Maximum additional number alpha string length.*/
}pbm_ad_num_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_email;
  /**<   Maximum emails possible.*/

  uint8_t max_email_len;
  /**<   Maximum email address length.*/
}pbm_email_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_second_name_len;
  /**<   Maximum second name length.*/
}pbm_second_name_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t is_hidden_entry_supported;
  /**<   Whether hidden entry is supported: \n
         - 0 -- FALSE \n
         - 1 -- TRUE
  */
}pbm_hidden_records_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_records;
  /**<   Maximum GAS records possible.*/

  uint8_t used_records;
  /**<   GAS records used.*/

  uint8_t max_gas_string_len;
  /**<   Maximum GAS string length.*/
}pbm_gas_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  uint8_t max_records;
  /**<   Maximum AAS records possible.*/

  uint8_t used_records;
  /**<   AAS records used.*/

  uint8_t max_aas_string_len;
  /**<   Maximum AAS string length.*/
}pbm_aas_capability_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type values: \n
      - PBM_SESSION_TYPE_GW_PRIMARY (0x00) --  GSM/WCDMA primary session 
      - PBM_SESSION_TYPE_1X_PRIMARY (0x01) --  1x primary session 
      - PBM_SESSION_TYPE_GW_SECONDARY (0x02) --  GSM/WCDMA secondary session 
      - PBM_SESSION_TYPE_1X_SECONDARY (0x03) --  1x secondary session 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT1 (0x04) --  Nonprovisioning session on slot 1 
      - PBM_SESSION_TYPE_NON_PROVISIONING_SLOT2 (0x05) --  Nonprovisioning session on slot 2 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT1 (0x06) --  Global phonebook session on slot 1 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT2 (0x07) --  Global phonebook session on slot 2 
      - PBM_SESSION_TYPE_GW_TERTIARY (0x08) --  GSM/WCDMA tertiary session 
      - PBM_SESSION_TYPE_1X_TERTIARY (0x09) --  1x tertiary session 
      - PBM_SESSION_TYPE_GLOBAL_PB_SLOT3 (0x0A) --  Global phonebook session on slot 3 

 Session type details are provided in Table \ref{tbl:SessTypes}.
 */

  pbm_pbr_cap_info_type_v01 pb_set_info;
}pbm_pb_set_cap_info_with_session_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns the capabilities of the PBs for all available
             sessions. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Capability Basic Info Array */
  uint8_t capability_basic_info_array_valid;  /**< Must be set to true if capability_basic_info_array is being passed */
  uint32_t capability_basic_info_array_len;  /**< Must be set to # of elements in capability_basic_info_array */
  pbm_capability_basic_info_array_type_v01 capability_basic_info_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Group Capability Array */
  uint8_t group_capability_array_valid;  /**< Must be set to true if group_capability_array is being passed */
  uint32_t group_capability_array_len;  /**< Must be set to # of elements in group_capability_array */
  pbm_group_capability_with_session_type_v01 group_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Additional Number Capability Array */
  uint8_t ad_num_capability_array_valid;  /**< Must be set to true if ad_num_capability_array is being passed */
  uint32_t ad_num_capability_array_len;  /**< Must be set to # of elements in ad_num_capability_array */
  pbm_ad_num_capability_with_session_type_v01 ad_num_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Email Capability Array */
  uint8_t email_capability_array_valid;  /**< Must be set to true if email_capability_array is being passed */
  uint32_t email_capability_array_len;  /**< Must be set to # of elements in email_capability_array */
  pbm_email_capability_with_session_type_v01 email_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Second Name Capability Array */
  uint8_t second_name_capability_array_valid;  /**< Must be set to true if second_name_capability_array is being passed */
  uint32_t second_name_capability_array_len;  /**< Must be set to # of elements in second_name_capability_array */
  pbm_second_name_capability_with_session_type_v01 second_name_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Hidden Records Capability Array */
  uint8_t hidden_records_capability_array_valid;  /**< Must be set to true if hidden_records_capability_array is being passed */
  uint32_t hidden_records_capability_array_len;  /**< Must be set to # of elements in hidden_records_capability_array */
  pbm_hidden_records_capability_with_session_type_v01 hidden_records_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Grouping Information Alpha String Capability Array */
  uint8_t gas_capability_array_valid;  /**< Must be set to true if gas_capability_array is being passed */
  uint32_t gas_capability_array_len;  /**< Must be set to # of elements in gas_capability_array */
  pbm_gas_capability_with_session_type_v01 gas_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Additional Number Alpha String Capability Array */
  uint8_t aas_capability_array_valid;  /**< Must be set to true if aas_capability_array is being passed */
  uint32_t aas_capability_array_len;  /**< Must be set to # of elements in aas_capability_array */
  pbm_aas_capability_with_session_type_v01 aas_capability_array[QMI_PBM_MAX_NUM_SESSIONS_V01];

  /* Optional */
  /*  Phonebook Set Capability Information Array */
  uint8_t pb_set_info_array_valid;  /**< Must be set to true if pb_set_info_array is being passed */
  uint32_t pb_set_info_array_len;  /**< Must be set to # of elements in pb_set_info_array */
  pbm_pb_set_cap_info_with_session_type_v01 pb_set_info_array[QMI_PBM_MAX_NUM_SESSIONS_V01];
}pbm_get_all_pb_capabilities_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t record_start_id;
  /**<   Start identifier of the record to be read.*/

  uint16_t record_end_id;
  /**<   End identifier of the record to be read.*/
}pbm_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_record_info_type_v01 record_info;
}pbm_read_records_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Number of Records */
  uint8_t num_of_recs_valid;  /**< Must be set to true if num_of_recs is being passed */
  uint16_t num_of_recs;
  /**<   Indicates the total number of records returned in the subsequent
       QMI_PBM_RECORD_READ_IND indications.
  */
}pbm_read_records_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_NUM_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_NUM_TYPE_UNKNOWN_V01 = 0x00, /**<  Unknown  */
  PBM_NUM_TYPE_INTERNATIONAL_V01 = 0x01, /**<  International  */
  PBM_NUM_TYPE_NATIONAL_V01 = 0x02, /**<  Domestic  */
  PBM_NUM_TYPE_NETWORK_SPECIFIC_V01 = 0x03, /**<  Network-specific  */
  PBM_NUM_TYPE_DEDICATED_ACCESS_V01 = 0x04, /**<  Dedicated access  */
  PBM_NUM_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_num_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_NUM_PLAN_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_NUM_PLAN_UNKNOWN_V01 = 0x00, /**<  Unknown  */
  PBM_NUM_PLAN_ISDN_V01 = 0x01, /**<  ISDN  */
  PBM_NUM_PLAN_DATA_V01 = 0x02, /**<  Data  */
  PBM_NUM_PLAN_TELEX_V01 = 0x03, /**<  Telex  */
  PBM_NUM_PLAN_NATIONAL_V01 = 0x04, /**<  National  */
  PBM_NUM_PLAN_PRIVATE_V01 = 0x05, /**<  Private  */
  PBM_NUM_PLAN_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_num_plan_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session typedetails are provided in Table \ref{tbl:SessTypes}. */

  uint16_t phonebook_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t record_id;
  /**<   Record identifier. */

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.*/

  uint32_t name_len;  /**< Must be set to # of elements in name */
  char name[QMI_PBM_NAME_MAX_LENGTH_V01];
  /**<   Name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_record_information_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t ad_number_len;  /**< Must be set to # of elements in ad_number */
  char ad_number[QMI_PBM_NUMBER_MAX_LENGTH_V01];

  uint8_t ad_num_tag_id;
  /**<   References the type of additional number (i.e., record number in the AAS
       elementary file on the card).
  */
}pbm_ad_num_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t email_address_len;  /**< Must be set to # of elements in email_address */
  char email_address[QMI_PBM_EMAIL_MAX_LENGTH_V01];
  /**<   Email address in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_email_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Record identifier.*/

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.*/

  uint32_t name_len;  /**< Must be set to # of elements in name */
  char name[QMI_PBM_NAME_MAX_LENGTH_V01];
  /**<   Name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_record_instance_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t seq_num;
  /**<   Sequence number of the indication.*/

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint32_t record_instances_len;  /**< Must be set to # of elements in record_instances */
  pbm_record_instance_type_v01 record_instances[QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01];
}pbm_basic_record_data_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t sname_len;  /**< Must be set to # of elements in sname */
  char sname[QMI_PBM_NAME_MAX_LENGTH_V01];
  /**<   Second Name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_sname_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t ad_nums_len;  /**< Must be set to # of elements in ad_nums */
  pbm_ad_num_info_type_v01 ad_nums[QMI_PBM_MAX_NUM_AD_NUM_COUNT_V01];
}pbm_ad_num_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t grp_id_len;  /**< Must be set to # of elements in grp_id */
  uint8_t grp_id[QMI_PBM_GRP_ID_MAX_V01];
  /**<   Group ID -- References the type of group (i.e., the record number in the
       GAS elementary file on the card).
  */
}pbm_grp_id_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t email_len;  /**< Must be set to # of elements in email */
  pbm_email_info_type_v01 email[QMI_PBM_EMAIL_INFO_MAX_V01];
}pbm_email_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint8_t is_hidden;
  /**<   Whether the record is hidden: \n
         - 0 -- FALSE \n
         - 1 -- TRUE
  */
}pbm_hidden_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Provides the record(s) that were requested using
             QMI_PBM_READ_RECORDS. */
typedef struct {

  /* Mandatory */
  /*  Array of Basic Record Data */
  pbm_basic_record_data_type_v01 basic_record_data;

  /* Optional */
  /*  Array of Second Name Information */
  uint8_t sname_info_array_valid;  /**< Must be set to true if sname_info_array is being passed */
  uint32_t sname_info_array_len;  /**< Must be set to # of elements in sname_info_array */
  pbm_sname_info_type_v01 sname_info_array[QMI_PBM_MAX_NUM_SNAME_INFO_V01];

  /* Optional */
  /*  Array of Additional Number Information */
  uint8_t ad_num_info_array_valid;  /**< Must be set to true if ad_num_info_array is being passed */
  uint32_t ad_num_info_array_len;  /**< Must be set to # of elements in ad_num_info_array */
  pbm_ad_num_record_info_type_v01 ad_num_info_array[QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01];

  /* Optional */
  /*  Array of Group ID Information */
  uint8_t grp_id_info_array_valid;  /**< Must be set to true if grp_id_info_array is being passed */
  uint32_t grp_id_info_array_len;  /**< Must be set to # of elements in grp_id_info_array */
  pbm_grp_id_record_info_type_v01 grp_id_info_array[QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01];

  /* Optional */
  /*  Array of Email Information */
  uint8_t email_info_array_valid;  /**< Must be set to true if email_info_array is being passed */
  uint32_t email_info_array_len;  /**< Must be set to # of elements in email_info_array */
  pbm_email_record_info_type_v01 email_info_array[QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01];

  /* Optional */
  /*  Array of Hidden Information */
  uint8_t hidden_record_info_array_valid;  /**< Must be set to true if hidden_record_info_array is being passed */
  uint32_t hidden_record_info_array_len;  /**< Must be set to # of elements in hidden_record_info_array */
  pbm_hidden_record_info_type_v01 hidden_record_info_array[QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01];
}pbm_record_read_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Adds a new record or modifies an existing record. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_record_information_type_v01 record_information;

  /* Optional */
  /*  Second Name Information */
  uint8_t sname_valid;  /**< Must be set to true if sname is being passed */
  uint32_t sname_len;  /**< Must be set to # of elements in sname */
  char sname[QMI_PBM_NAME_MAX_LENGTH_V01];
  /**<   Second Name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/

  /* Optional */
  /*  Additional Number Information */
  uint8_t ad_num_info_valid;  /**< Must be set to true if ad_num_info is being passed */
  uint32_t ad_num_info_len;  /**< Must be set to # of elements in ad_num_info */
  pbm_ad_num_info_type_v01 ad_num_info[QMI_PBM_AD_NUM_MAX_V01];

  /* Optional */
  /*  Group ID Information */
  uint8_t grp_id_valid;  /**< Must be set to true if grp_id is being passed */
  uint32_t grp_id_len;  /**< Must be set to # of elements in grp_id */
  uint8_t grp_id[QMI_PBM_GRP_ID_MAX_V01];
  /**<   Group ID -- References the type of group (i.e., record number in the GAS
       elementary file on the card).
  */

  /* Optional */
  /*  Email Information */
  uint8_t email_info_valid;  /**< Must be set to true if email_info is being passed */
  uint32_t email_info_len;  /**< Must be set to # of elements in email_info */
  pbm_email_info_type_v01 email_info[QMI_PBM_EMAIL_INFO_MAX_V01];

  /* Optional */
  /*  Hidden Information */
  uint8_t is_hidden_valid;  /**< Must be set to true if is_hidden is being passed */
  uint8_t is_hidden;
  /**<   Whether a record is hidden: \n
         - 0 -- FALSE \n
         - 1 -- TRUE
  */
}pbm_write_record_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Adds a new record or modifies an existing record. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Record Information */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint16_t record_id;
  /**<   Identifier of the record that has been added or updated.*/
}pbm_write_record_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t record_id;
  /**<   Identifier of the record to be deleted.*/
}pbm_record_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Deletes a PB record. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_record_id_type_v01 record_info;
}pbm_delete_record_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Deletes a PB record. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Record ID */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint16_t record_id;
  /**<   Identifier of the record that is deleted.*/
}pbm_delete_record_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Deletes all records of a PB. */
typedef struct {

  /* Mandatory */
  /*  Phonebook Information */
  pbm_phonebook_info_type_v01 phonebook_info;
}pbm_delete_all_pb_records_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Deletes all records of a PB. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}pbm_delete_all_pb_records_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Searches the records by a specified name or number. */
typedef struct {

  /* Mandatory */
  /*  Search Information */
  pbm_phonebook_info_type_v01 search_info;

  /* Optional */
  /*  Number Information */
  uint8_t number_valid;  /**< Must be set to true if number is being passed */
  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.*/

  /* Optional */
  /*  Name Information */
  uint8_t name_valid;  /**< Must be set to true if name is being passed */
  uint32_t name_len;  /**< Must be set to # of elements in name */
  char name[QMI_PBM_NAME_MAX_LENGTH_V01];
  /**<   Name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_search_records_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Searches the records by a specified name or number. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Record IDs List */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint32_t record_id_len;  /**< Must be set to # of elements in record_id */
  uint16_t record_id[QMI_PBM_REC_IDS_MAX_V01];
  /**<   Identifier of the record that matches the criterion.*/
}pbm_search_records_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_OPERATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_OPERATION_ADD_V01 = 0x01, /**<  Add  */
  PBM_OPERATION_MODIFY_V01 = 0x02, /**<  Modify  */
  PBM_OPERATION_DELETE_V01 = 0x03, /**<  Delete  */
  PBM_OPERATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_operation_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types provided in Table \ref{tbl:suppPB}. */

  pbm_operation_enum_v01 operation;
  /**<   Action performed on the record: \n
      - PBM_OPERATION_ADD (0x01) --  Add 
      - PBM_OPERATION_MODIFY (0x02) --  Modify 
      - PBM_OPERATION_DELETE (0x03) --  Delete 
 */

  uint16_t record_id;
  /**<   Identifier of the record that is updated.*/
}pbm_record_update_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates a change in any PB record. */
typedef struct {

  /* Mandatory */
  /*  Record Update Information */
  pbm_record_update_info_type_v01 record_update_info;
}pbm_record_update_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_REFRESH_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_REFRESH_STATUS_START_V01 = 0x01, /**<  Refresh status start  */
  PBM_REFRESH_STATUS_END_V01 = 0x02, /**<  Refresh status end  */
  PBM_REFRESH_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_refresh_status_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  pbm_refresh_status_enum_v01 status;
  /**<   Current refresh status: \n
      - PBM_REFRESH_STATUS_START (0x01) --  Refresh status start 
      - PBM_REFRESH_STATUS_END (0x02) --  Refresh status end 
 */
}pbm_refresh_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates the status of a PB refresh. */
typedef struct {

  /* Mandatory */
  /*  Refresh Information */
  pbm_refresh_info_type_v01 refresh_info;
}pbm_refresh_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates the PB of a session that is ready to be accessed. */
typedef struct {

  /* Mandatory */
  /*  Phonebook Ready Information */
  pbm_phonebook_info_type_v01 phonebook_ready_info;
}pbm_pb_ready_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t emer_num_len;  /**< Must be set to # of elements in emer_num */
  char emer_num[QMI_PBM_EMER_NUM_MAX_LENGTH_V01];
  /**<   Emergency number.*/
}pbm_emer_num_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t cat;
  /**<   Bitmask of Emergency number categories, which are provided in \n
       Table \ref{tbl:EmgcyCat}. */

  uint32_t emer_num_len;  /**< Must be set to # of elements in emer_num */
  char emer_num[QMI_PBM_EMER_NUM_MAX_LENGTH_V01];
  /**<   Emergency number.*/
}pbm_ecc_emer_num_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint32_t emer_nums_len;  /**< Must be set to # of elements in emer_nums */
  pbm_ecc_emer_num_type_v01 emer_nums[QMI_PBM_MAX_NUM_CARD_ECC_EMER_NUM_V01];
}pbm_card_emer_num_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint32_t emer_nums_len;  /**< Must be set to # of elements in emer_nums */
  pbm_ecc_emer_num_type_v01 emer_nums[QMI_PBM_MAX_NUM_NETWORK_ECC_EMER_NUM_V01];
}pbm_network_emer_num_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t emer_nums_len;  /**< Must be set to # of elements in emer_nums */
  pbm_emer_num_type_v01 emer_nums[QMI_PBM_MAX_NUM_MCC_EMER_NUM_V01];
}pbm_mcc_emer_num_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates the consolidated list of emergency numbers applicable
             at any point in time. */
typedef struct {

  /* Mandatory */
  /*  Hardcoded Emergency Numbers -- Deprecated */
  uint32_t emer_nums_len;  /**< Must be set to # of elements in emer_nums */
  pbm_emer_num_type_v01 emer_nums[QMI_PBM_MAX_NUM_EMER_NUM_V01];
  /**<   \n This TLV is deprecated from minor version 10. 
   */

  /* Optional */
  /*  NV Emergency Numbers -- Deprecated */
  uint8_t nv_emer_nums_valid;  /**< Must be set to true if nv_emer_nums is being passed */
  uint32_t nv_emer_nums_len;  /**< Must be set to # of elements in nv_emer_nums */
  pbm_emer_num_type_v01 nv_emer_nums[QMI_PBM_MAX_NUM_NV_EMER_NUM_V01];
  /**<   \n This TLV is deprecated from minor version 10. 
   */

  /* Optional */
  /*  Card Emergency Numbers */
  uint8_t card_emer_nums_valid;  /**< Must be set to true if card_emer_nums is being passed */
  uint32_t card_emer_nums_len;  /**< Must be set to # of elements in card_emer_nums */
  pbm_card_emer_num_type_v01 card_emer_nums[QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01];

  /* Optional */
  /*  Network Emergency Numbers */
  uint8_t network_emer_nums_valid;  /**< Must be set to true if network_emer_nums is being passed */
  uint32_t network_emer_nums_len;  /**< Must be set to # of elements in network_emer_nums */
  pbm_network_emer_num_type_v01 network_emer_nums[QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01];

  /* Optional */
  /*  Hardcoded Emergency Numbers Extended */
  uint8_t emer_nums_extended_valid;  /**< Must be set to true if emer_nums_extended is being passed */
  uint32_t emer_nums_extended_len;  /**< Must be set to # of elements in emer_nums_extended */
  pbm_emer_num_type_v01 emer_nums_extended[QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01];

  /* Optional */
  /*  NV Emergency Numbers Extended */
  uint8_t nv_emer_nums_extended_valid;  /**< Must be set to true if nv_emer_nums_extended is being passed */
  uint32_t nv_emer_nums_extended_len;  /**< Must be set to # of elements in nv_emer_nums_extended */
  pbm_emer_num_type_v01 nv_emer_nums_extended[QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01];

  /* Optional */
  /*  Hardcoded Emergency Numbers Extended with Category  */
  uint8_t emer_nums_extended_cat_valid;  /**< Must be set to true if emer_nums_extended_cat is being passed */
  uint32_t emer_nums_extended_cat_len;  /**< Must be set to # of elements in emer_nums_extended_cat */
  uint8_t emer_nums_extended_cat[QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01];
  /**<   Bitmask of Emergency number categories, which are provided in \n
       Table \ref{tbl:EmgcyCat}. */

  /* Optional */
  /*  NV Emergency Numbers Extended with Category */
  uint8_t nv_emer_nums_extended_cat_valid;  /**< Must be set to true if nv_emer_nums_extended_cat is being passed */
  uint32_t nv_emer_nums_extended_cat_len;  /**< Must be set to # of elements in nv_emer_nums_extended_cat */
  uint8_t nv_emer_nums_extended_cat[QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01];
  /**<   Bitmask of Emergency number categories, which are provided in \n
       Table \ref{tbl:EmgcyCat}. */

  /* Optional */
  /*  MCC Based Emergency Numbers for each slot */
  uint8_t mcc_emer_nums_valid;  /**< Must be set to true if mcc_emer_nums is being passed */
  uint32_t mcc_emer_nums_len;  /**< Must be set to # of elements in mcc_emer_nums */
  pbm_mcc_emer_num_type_v01 mcc_emer_nums[QMI_PBM_MAX_NUM_SLOT_V01];
}pbm_emergency_list_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef uint16_t pbm_pb_mask_type_v01;
#define PBM_PB_TYPE_ADN_V01 ((pbm_pb_mask_type_v01)0x01) /**<  Abbreviated dialing numbers phonebook  */
#define PBM_PB_TYPE_FDN_V01 ((pbm_pb_mask_type_v01)0x02) /**<  Fixed dialing numbers phonebook  */
#define PBM_PB_TYPE_MSISDN_V01 ((pbm_pb_mask_type_v01)0x04) /**<  Mobile subscriber integrated services digital network phonebook  */
#define PBM_PB_TYPE_MBDN_V01 ((pbm_pb_mask_type_v01)0x08) /**<  Mail box dialing numbers phonebook  */
#define PBM_PB_TYPE_SDN_V01 ((pbm_pb_mask_type_v01)0x10) /**<  Service dialing numbers phonebook  */
#define PBM_PB_TYPE_BDN_V01 ((pbm_pb_mask_type_v01)0x20) /**<  Barred dialing numbers phonebook  */
#define PBM_PB_TYPE_LND_V01 ((pbm_pb_mask_type_v01)0x40) /**<  Last number dialed phonebook  */
#define PBM_PB_TYPE_MBN_V01 ((pbm_pb_mask_type_v01)0x80) /**<  Mail Box Numbers phonebook  */
#define PBM_PB_TYPE_GAS_V01 ((pbm_pb_mask_type_v01)0x100) /**<  Grouping information alpha string phonebook  */
#define PBM_PB_TYPE_AAS_V01 ((pbm_pb_mask_type_v01)0x200) /**<  Additional number alpha string phonebook  */
/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_pb_mask_type_v01 pb_bit_mask;
  /**<   Bitmask of phonebook types: \n
      - PBM_PB_TYPE_ADN (0x01) --  Abbreviated dialing numbers phonebook 
      - PBM_PB_TYPE_FDN (0x02) --  Fixed dialing numbers phonebook 
      - PBM_PB_TYPE_MSISDN (0x04) --  Mobile subscriber integrated services digital network phonebook 
      - PBM_PB_TYPE_MBDN (0x08) --  Mail box dialing numbers phonebook 
      - PBM_PB_TYPE_SDN (0x10) --  Service dialing numbers phonebook 
      - PBM_PB_TYPE_BDN (0x20) --  Barred dialing numbers phonebook 
      - PBM_PB_TYPE_LND (0x40) --  Last number dialed phonebook 
      - PBM_PB_TYPE_MBN (0x80) --  Mail Box Numbers phonebook 
      - PBM_PB_TYPE_GAS (0x100) --  Grouping information alpha string phonebook 
      - PBM_PB_TYPE_AAS (0x200) --  Additional number alpha string phonebook 
 */
}pbm_phonebook_ready_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates that all PBs in the specified sessions are ready to
             be accessed. */
typedef struct {

  /* Mandatory */
  /*  Phonebook Ready Information */
  uint32_t phonebook_ready_info_len;  /**< Must be set to # of elements in phonebook_ready_info */
  pbm_phonebook_ready_info_type_v01 phonebook_ready_info[QMI_PBM_MAX_NUM_PHONEBOOK_READY_INFO_V01];
}pbm_all_pb_init_done_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_FIELD_ID_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_FIELD_EMAIL_V01 = 0x00, /**<  Email field  */
  PBM_FIELD_ANR_V01 = 0x01, /**<  Additional number field  */
  PBM_FIELD_SNE_V01 = 0x02, /**<  Second name field  */
  PBM_FIELD_GRP_V01 = 0x03, /**<  Group field  */
  PBM_FIELD_ID_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_field_id_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_MAPPING_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_MAPPING_TYPE_1_V01 = 0x01, /**<  Linear mapping between the ADN and USIM fields  */
  PBM_MAPPING_TYPE_2_V01 = 0x02, /**<  Nonlinear mapping between the ADN and USIM fields  */
  PBM_MAPPING_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_mapping_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_field_id_type_enum_v01 field_id;
  /**<   Field identifier. Values: \n
      - PBM_FIELD_EMAIL (0x00) --  Email field 
      - PBM_FIELD_ANR (0x01) --  Additional number field 
      - PBM_FIELD_SNE (0x02) --  Second name field 
      - PBM_FIELD_GRP (0x03) --  Group field 
 */

  pbm_mapping_type_enum_v01 mapping_type;
  /**<   Mapping type for the EF corresponding to this field. 
 Values: \n 
      - PBM_MAPPING_TYPE_1 (0x01) --  Linear mapping between the ADN and USIM fields 
      - PBM_MAPPING_TYPE_2 (0x02) --  Nonlinear mapping between the ADN and USIM fields 
 */

  uint16_t num_of_records;
  /**<   Number of records present in EF for this field. */

  uint16_t num_free_records;
  /**<   Number of free records for this field. */

  uint16_t record_length;
  /**<   Length of the record clients use to fill the field.   */
}pbm_usim_field_capability_array_elem_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns a list of all emergency numbers. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pbm_get_emergency_list_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns a list of all emergency numbers. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Hardcoded Emergency Numbers -- Deprecated */
  uint8_t pbm_emer_nums_valid;  /**< Must be set to true if pbm_emer_nums is being passed */
  uint32_t pbm_emer_nums_len;  /**< Must be set to # of elements in pbm_emer_nums */
  pbm_emer_num_type_v01 pbm_emer_nums[QMI_PBM_MAX_NUM_EMER_NUM_V01];
  /**<   \n This TLV is deprecated from minor version 10. 
   */

  /* Optional */
  /*  NV Emergency Numbers -- Deprecated */
  uint8_t pbm_nv_emer_nums_valid;  /**< Must be set to true if pbm_nv_emer_nums is being passed */
  uint32_t pbm_nv_emer_nums_len;  /**< Must be set to # of elements in pbm_nv_emer_nums */
  pbm_emer_num_type_v01 pbm_nv_emer_nums[QMI_PBM_MAX_NUM_NV_EMER_NUM_V01];
  /**<   \n This TLV is deprecated from minor version 10. 
   */

  /* Optional */
  /*  Card Emergency Numbers */
  uint8_t card_emer_nums_valid;  /**< Must be set to true if card_emer_nums is being passed */
  uint32_t card_emer_nums_len;  /**< Must be set to # of elements in card_emer_nums */
  pbm_card_emer_num_type_v01 card_emer_nums[QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01];

  /* Optional */
  /*  Network Emergency Numbers */
  uint8_t network_emer_nums_valid;  /**< Must be set to true if network_emer_nums is being passed */
  uint32_t network_emer_nums_len;  /**< Must be set to # of elements in network_emer_nums */
  pbm_network_emer_num_type_v01 network_emer_nums[QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01];

  /* Optional */
  /*  Hard Coded Emergency Numbers Extended */
  uint8_t emer_nums_extended_valid;  /**< Must be set to true if emer_nums_extended is being passed */
  uint32_t emer_nums_extended_len;  /**< Must be set to # of elements in emer_nums_extended */
  pbm_emer_num_type_v01 emer_nums_extended[QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01];

  /* Optional */
  /*  NV Emergency Numbers Extended */
  uint8_t nv_emer_nums_extended_valid;  /**< Must be set to true if nv_emer_nums_extended is being passed */
  uint32_t nv_emer_nums_extended_len;  /**< Must be set to # of elements in nv_emer_nums_extended */
  pbm_emer_num_type_v01 nv_emer_nums_extended[QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01];

  /* Optional */
  /*  Hardcoded Emergency Numbers Extended with Category  */
  uint8_t emer_nums_extended_cat_valid;  /**< Must be set to true if emer_nums_extended_cat is being passed */
  uint32_t emer_nums_extended_cat_len;  /**< Must be set to # of elements in emer_nums_extended_cat */
  uint8_t emer_nums_extended_cat[QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01];
  /**<   Bitmask of Emergency number categories, which are provided in \n
       Table \ref{tbl:EmgcyCat}. */

  /* Optional */
  /*  NV Emergency Numbers Extended with Category */
  uint8_t nv_emer_nums_extended_cat_valid;  /**< Must be set to true if nv_emer_nums_extended_cat is being passed */
  uint32_t nv_emer_nums_extended_cat_len;  /**< Must be set to # of elements in nv_emer_nums_extended_cat */
  uint8_t nv_emer_nums_extended_cat[QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01];
  /**<   Bitmask of Emergency number categories, which are provided in \n
       Table \ref{tbl:EmgcyCat}. */

  /* Optional */
  /*  MCC Based Emergency Numbers for each slot */
  uint8_t mcc_emer_nums_valid;  /**< Must be set to true if mcc_emer_nums is being passed */
  uint32_t mcc_emer_nums_len;  /**< Must be set to # of elements in mcc_emer_nums */
  pbm_mcc_emer_num_type_v01 mcc_emer_nums[QMI_PBM_MAX_NUM_SLOT_V01];
}pbm_get_emergency_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns a list of group names and their corresponding identifiers
             for all sessions. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pbm_get_all_groups_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t grp_id;
  /**<   Group identifier -- References the type of group (same as the record
       number in a GAS elementary file on the card).
  */

  uint32_t grp_name_len;  /**< Must be set to # of elements in grp_name */
  char grp_name[QMI_PBM_GROUP_NAME_MAX_LENGTH_V01];
  /**<   Group name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_groups_instance_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint32_t grp_array_len;  /**< Must be set to # of elements in grp_array */
  pbm_groups_instance_type_v01 grp_array[QMI_PBM_MAX_NUM_GROUP_COUNT_V01];
}pbm_groups_data_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns a list of group names and their corresponding identifiers
             for all sessions. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Groups Data */
  uint8_t groups_data_valid;  /**< Must be set to true if groups_data is being passed */
  uint32_t groups_data_len;  /**< Must be set to # of elements in groups_data */
  pbm_groups_data_type_v01 groups_data[QMI_PBM_MAX_NUM_GROUPS_DATA_V01];
}pbm_get_all_groups_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_GROUP_OPERATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_GROUP_OPERATION_ADD_V01 = 0, /**<  Add  */
  PBM_GROUP_OPERATION_MODIFY_V01 = 1, /**<  Modify  */
  PBM_GROUP_OPERATION_DELETE_V01 = 2, /**<  Delete  */
  PBM_GROUP_OPERATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_group_operation_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_group_operation_enum_v01 operation;
  /**<   Operation performed on a Group name: \n 
      - PBM_GROUP_OPERATION_ADD (0) --  Add 
      - PBM_GROUP_OPERATION_MODIFY (1) --  Modify 
      - PBM_GROUP_OPERATION_DELETE (2) --  Delete 
 */

  uint8_t group_id;
  /**<   Group identifier -- References the type of group (same as the record 
       number in a GAS elementary file on the card).
  */

  uint32_t grp_name_len;  /**< Must be set to # of elements in grp_name */
  char grp_name[QMI_PBM_GROUP_NAME_MAX_LENGTH_V01];
  /**<   Group name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_grp_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Adds, modifies, or deletes a group. */
typedef struct {

  /* Mandatory */
  /*  Group Information */
  pbm_grp_info_type_v01 grp_info;
}pbm_set_group_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint8_t group_id;
  /**<   Group identifier -- References the type of group (same as the record 
       number in a GAS elementary file on the card).
  */
}pbm_grp_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Adds, modifies, or deletes a group. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Group Identifier */
  uint8_t grp_id_valid;  /**< Must be set to true if grp_id is being passed */
  pbm_grp_id_type_v01 grp_id;
}pbm_set_group_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns the current state of the requested phonebook. */
typedef struct {

  /* Mandatory */
  /*  Phonebook Information */
  pbm_phonebook_info_type_v01 phonebook_info;
}pbm_get_pb_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_PB_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_PB_STATE_READY_V01 = 0x00, /**<  Ready  */
  PBM_PB_STATE_NOT_READY_V01 = 0x01, /**<  Not ready  */
  PBM_PB_STATE_NOT_AVAILABLE_V01 = 0x02, /**<  Not available  */
  PBM_PB_STATE_PIN_RESTRICTION_V01 = 0x03, /**<  PIN restriction  */
  PBM_PB_STATE_PUK_RESTRICTION_V01 = 0x04, /**<  PUK restriction  */
  PBM_PB_STATE_INVALIDATED_V01 = 0x05, /**<  Invalidated  */
  PBM_PB_STATE_SYNC_V01 = 0x06, /**<  In synchronization  */
  PBM_PB_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_pb_state_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  pbm_pb_state_enum_v01 state;
  /**<   State of the phonebook: \n
      - PBM_PB_STATE_READY (0x00) --  Ready 
      - PBM_PB_STATE_NOT_READY (0x01) --  Not ready 
      - PBM_PB_STATE_NOT_AVAILABLE (0x02) --  Not available 
      - PBM_PB_STATE_PIN_RESTRICTION (0x03) --  PIN restriction 
      - PBM_PB_STATE_PUK_RESTRICTION (0x04) --  PUK restriction 
      - PBM_PB_STATE_INVALIDATED (0x05) --  Invalidated 
      - PBM_PB_STATE_SYNC (0x06) --  In synchronization 
 */
}pbm_phonebook_state_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns the current state of the requested phonebook. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Phonebook State */
  uint8_t phonebook_state_valid;  /**< Must be set to true if phonebook_state is being passed */
  pbm_phonebook_state_type_v01 phonebook_state;
}pbm_get_pb_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the Record Read operation for all the
             hidden records. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */
}pbm_read_all_hidden_records_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the Record Read operation for all the
             hidden records. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Number of Records */
  uint8_t num_of_recs_valid;  /**< Must be set to true if num_of_recs is being passed */
  uint16_t num_of_recs;
  /**<   Indicates the total number of records returned in the subsequent
       QMI_PBM_RECORD_READ_INDs
  */
}pbm_read_all_hidden_records_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_HIDDEN_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_HIDDEN_STATUS_VALID_V01 = 0x01, /**<  Hidden records are valid and accessible.  */
  PBM_HIDDEN_STATUS_NOT_VALID_V01 = 0x02, /**<  Hidden records are not valid and cannot be accessed.  */
  PBM_HIDDEN_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_hidden_status_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_hidden_status_enum_v01 status;
  /**<   Current status of hidden records: \n
      - PBM_HIDDEN_STATUS_VALID (0x01) --  Hidden records are valid and accessible. 
      - PBM_HIDDEN_STATUS_NOT_VALID (0x02) --  Hidden records are not valid and cannot be accessed. 
 */
}pbm_hidden_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates the status of hidden records in the session. */
typedef struct {

  /* Mandatory */
  /*  Hidden Status */
  pbm_hidden_status_type_v01 status_info;
}pbm_hidden_record_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in \n
       Table \ref{tbl:suppPB}. */

  uint16_t record_id;
  /**<   Record identifier.*/
}pbm_base_record_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Gets the empty record identifier subsequent to
             the identifier of the record specified in the request. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_base_record_info_type_v01 record_info;
}pbm_get_next_empty_record_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Gets the empty record identifier subsequent to
             the identifier of the record specified in the request. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Record ID */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint16_t record_id;
  /**<   Identifier of the empty record.*/
}pbm_get_next_empty_record_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Message used to get the nonempty record identifier subsequent
             to the identifier of the record specified in the request. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_base_record_info_type_v01 record_info;
}pbm_get_next_non_empty_record_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Message used to get the nonempty record identifier subsequent
             to the identifier of the record specified in the request. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Record ID */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint16_t record_id;
  /**<   Identifier of the nonempty record.*/
}pbm_get_next_non_empty_record_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Returns a list of additional number alpha strings 
             and the corresponding identifiers for all sessions. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pbm_get_all_aas_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t aas_id;
  /**<   AAS identifier -- References the type of AAS (same as the record
       number in an AAS elementary file on the card).
  */

  uint32_t alpha_len;  /**< Must be set to # of elements in alpha */
  char alpha[QMI_PBM_AAS_NAME_MAX_LENGTH_V01];
  /**<   Additional number Alpha String in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_aas_instance_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint32_t aas_array_len;  /**< Must be set to # of elements in aas_array */
  pbm_aas_instance_type_v01 aas_array[QMI_PBM_MAX_NUM_AAS_COUNT_V01];
}pbm_aas_data_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Returns a list of additional number alpha strings 
             and the corresponding identifiers for all sessions. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  AAS Data */
  uint8_t aas_data_valid;  /**< Must be set to true if aas_data is being passed */
  uint32_t aas_data_len;  /**< Must be set to # of elements in aas_data */
  pbm_aas_data_type_v01 aas_data[QMI_PBM_MAX_NUM_SESSIONS_V01];
}pbm_get_all_aas_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_AAS_OPERATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_AAS_OPERATION_ADD_V01 = 0, /**<  Add  */
  PBM_AAS_OPERATION_MODIFY_V01 = 1, /**<  Modify  */
  PBM_AAS_OPERATION_DELETE_V01 = 2, /**<  Delete  */
  PBM_AAS_OPERATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_aas_operation_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_aas_operation_enum_v01 operation;
  /**<   Operation performed on the AAS: \n 
      - PBM_AAS_OPERATION_ADD (0) --  Add 
      - PBM_AAS_OPERATION_MODIFY (1) --  Modify 
      - PBM_AAS_OPERATION_DELETE (2) --  Delete 
 */

  uint8_t aas_id;
  /**<   AAS identifier -- References the type of AAS (same as the record 
       number in an AAS elementary file on the card).
  */

  uint32_t alpha_len;  /**< Must be set to # of elements in alpha */
  char alpha[QMI_PBM_AAS_NAME_MAX_LENGTH_V01];
  /**<   Additional number alpha string in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_aas_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Adds, modifies, or deletes an additional number alpha string. */
typedef struct {

  /* Mandatory */
  /*  AAS Information */
  pbm_aas_info_type_v01 aas_info;
}pbm_set_aas_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint8_t aas_id;
  /**<   AAS identifier -- References the type of AAS (same as the record 
       number in an AAS elementary file on the card).
  */
}pbm_aas_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Adds, modifies, or deletes an additional number alpha string. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  AAS Identifier */
  uint8_t aas_id_valid;  /**< Must be set to true if aas_id is being passed */
  pbm_aas_id_type_v01 aas_id;
}pbm_set_aas_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_operation_enum_v01 operation;
  /**<   Action performed on the AAS item: \n
      - PBM_OPERATION_ADD (0x01) --  Add 
      - PBM_OPERATION_MODIFY (0x02) --  Modify 
      - PBM_OPERATION_DELETE (0x03) --  Delete 
 */

  uint8_t aas_id;
  /**<   Identifier of the AAS item that is updated.*/

  uint32_t alpha_len;  /**< Must be set to # of elements in alpha */
  char alpha[QMI_PBM_AAS_NAME_MAX_LENGTH_V01];
  /**<   Additional number alpha string in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_aas_update_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates changes in an additional number alpha string item.  */
typedef struct {

  /* Mandatory */
  /*  AAS Update Information */
  pbm_aas_update_info_type_v01 aas_update_info;

  /* Optional */
  /*  AAS Extended String */
  uint8_t aas_ext_valid;  /**< Must be set to true if aas_ext is being passed */
  uint32_t aas_ext_len;  /**< Must be set to # of elements in aas_ext */
  uint16_t aas_ext[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   AAS extended string. */
}pbm_aas_update_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  pbm_operation_enum_v01 operation;
  /**<   Action performed on the GAS item: \n
      - PBM_OPERATION_ADD (0x01) --  Add 
      - PBM_OPERATION_MODIFY (0x02) --  Modify 
      - PBM_OPERATION_DELETE (0x03) --  Delete 
 */

  uint8_t gas_id;
  /**<   Identifier of the GAS item that is updated.*/

  uint32_t grp_name_len;  /**< Must be set to # of elements in grp_name */
  char grp_name[QMI_PBM_GROUP_NAME_MAX_LENGTH_V01];
  /**<   Group name in UCS2.
 Each UCS2 character must be sent in Little Endian format.*/
}pbm_gas_update_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates changes in a grouping information alpha string
             item. */
typedef struct {

  /* Mandatory */
  /*  AAS Update Information */
  pbm_gas_update_info_type_v01 gas_update_info;

  /* Optional */
  /*  GAS Extended String */
  uint8_t gas_ext_valid;  /**< Must be set to true if gas_ext is being passed */
  uint32_t gas_ext_len;  /**< Must be set to # of elements in gas_ext */
  uint16_t gas_ext[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
}pbm_gas_update_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_SUBS_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_SUBS_TYPE_PRIMARY_V01 = 0x00, /**<  Primary subscription  */
  PBM_SUBS_TYPE_SECONDARY_V01 = 0x01, /**<  Secondary subscription  */
  PBM_SUBS_TYPE_TERTIARY_V01 = 0x02, /**<  Tertiary subscription  */
  PBM_SUBS_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_subs_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Binds a subscription type to a specific PBM client ID. */
typedef struct {

  /* Mandatory */
  /*  Subscription Type */
  pbm_subs_type_enum_v01 subs_type;
  /**<   Subscription type: \n
      - PBM_SUBS_TYPE_PRIMARY (0x00) --  Primary subscription 
      - PBM_SUBS_TYPE_SECONDARY (0x01) --  Secondary subscription 
      - PBM_SUBS_TYPE_TERTIARY (0x02) --  Tertiary subscription 
 */
}pbm_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Binds a subscription type to a specific PBM client ID. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}pbm_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Gets the subscription to which the client 
             is bound. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}pbm_get_subscription_binding_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Gets the subscription to which the client 
             is bound. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Bound Subscription Type */
  uint8_t subs_type_valid;  /**< Must be set to true if subs_type is being passed */
  pbm_subs_type_enum_v01 subs_type;
  /**<   Bound subscription type: \n
      - PBM_SUBS_TYPE_PRIMARY (0x00) --  Primary subscription 
      - PBM_SUBS_TYPE_SECONDARY (0x01) --  Secondary subscription 
      - PBM_SUBS_TYPE_TERTIARY (0x02) --  Tertiary subscription 
 */
}pbm_get_subscription_binding_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the ADN phonebook Set Capabilities Read operation by 
             specifying the range of the phonebook sets to be read. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  /* Optional */
  /*  Start Identifier of the Phonebook Set to be Read */
  uint8_t start_id_valid;  /**< Must be set to true if start_id is being passed */
  uint16_t start_id;
  /**<   Start identifier. */

  /* Optional */
  /*  End Identifier of the Phonebook Set to be Read */
  uint8_t end_id_valid;  /**< Must be set to true if end_id is being passed */
  uint16_t end_id;
  /**<   End identifier. */
}pbm_read_pbset_caps_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the ADN phonebook Set Capabilities Read operation by 
             specifying the range of the phonebook sets to be read. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Number of PB Sets */
  uint8_t num_of_pbsets_valid;  /**< Must be set to true if num_of_pbsets is being passed */
  uint16_t num_of_pbsets;
  /**<   Indicates the total number of phonebook sets returned in the subsequent
       QMI_PBM_PBSET_CAP_READ_INDs.
  */
}pbm_read_pbset_caps_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t pb_set;
  /**<   Identifier of the PB set.  */

  uint32_t usim_field_capability_len;  /**< Must be set to # of elements in usim_field_capability */
  pbm_usim_field_capability_array_elem_type_v01 usim_field_capability[QMI_PBM_MAX_NUM_USIM_FIELDS_IN_RECORD_V01];
}pbm_usim_field_capability_array_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint32_t pbset_cap_instances_len;  /**< Must be set to # of elements in pbset_cap_instances */
  pbm_usim_field_capability_array_type_v01 pbset_cap_instances[QMI_PBM_MAX_NUM_PBSET_CAP_INSTANCE_V01];
}pbm_usim_field_capabilities_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Provides the record(s) that were requested using
             QMI_PBM_READ_PBSET_CAPS. */
typedef struct {

  /* Mandatory */
  /*  Array of Phonebook Set Capabilties Data */
  pbm_usim_field_capabilities_type_v01 usim_field_capabilities;
}pbm_pbset_cap_read_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the extended Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_record_info_type_v01 record_info;
}pbm_read_records_ext_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the extended Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Number of Records */
  uint8_t num_of_recs_valid;  /**< Must be set to true if num_of_recs is being passed */
  uint16_t num_of_recs;
  /**<   Indicates the total number of records returned in the subsequent
       QMI_PBM_RECORD_READ_EXT_IND indications.
  */
}pbm_read_records_ext_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Record identifier.*/

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.*/

  uint32_t name_len;  /**< Must be set to # of elements in name */
  uint16_t name[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   Name in UCS2.*/
}pbm_record_instance_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t seq_num;
  /**<   Sequence number of the indication.*/

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in Table \ref{tbl:suppPB}. */

  uint32_t record_instances_len;  /**< Must be set to # of elements in record_instances */
  pbm_record_instance_ext_type_v01 record_instances[QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01];
}pbm_basic_record_data_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t sname_len;  /**< Must be set to # of elements in sname */
  uint16_t sname[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   Second name in UCS2.*/
}pbm_sname_info_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t email_address_len;  /**< Must be set to # of elements in email_address */
  uint16_t email_address[QMI_PBM_EMAIL_MAX_LENGTH_EXT_V01];
  /**<   Email address in UCS2.*/
}pbm_email_info_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.*/

  uint32_t email_len;  /**< Must be set to # of elements in email */
  pbm_email_info_ext_type_v01 email[QMI_PBM_EMAIL_INFO_MAX_V01];
}pbm_email_record_info_ext_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Provides the extended record(s) that were requested using
             QMI_PBM_READ_RECORDS_EXT. */
typedef struct {

  /* Mandatory */
  /*  Array of Basic Record Data */
  pbm_basic_record_data_ext_type_v01 basic_record_data;

  /* Optional */
  /*  Array of Second Name Information */
  uint8_t sname_info_array_valid;  /**< Must be set to true if sname_info_array is being passed */
  uint32_t sname_info_array_len;  /**< Must be set to # of elements in sname_info_array */
  pbm_sname_info_ext_type_v01 sname_info_array[QMI_PBM_MAX_NUM_SNAME_INFO_V01];

  /* Optional */
  /*  Array of Additional Number Information */
  uint8_t ad_num_info_array_valid;  /**< Must be set to true if ad_num_info_array is being passed */
  uint32_t ad_num_info_array_len;  /**< Must be set to # of elements in ad_num_info_array */
  pbm_ad_num_record_info_type_v01 ad_num_info_array[QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01];

  /* Optional */
  /*  Array of Group ID Information */
  uint8_t grp_id_info_array_valid;  /**< Must be set to true if grp_id_info_array is being passed */
  uint32_t grp_id_info_array_len;  /**< Must be set to # of elements in grp_id_info_array */
  pbm_grp_id_record_info_type_v01 grp_id_info_array[QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01];

  /* Optional */
  /*  Array of Email Information */
  uint8_t email_info_array_valid;  /**< Must be set to true if email_info_array is being passed */
  uint32_t email_info_array_len;  /**< Must be set to # of elements in email_info_array */
  pbm_email_record_info_ext_type_v01 email_info_array[QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01];

  /* Optional */
  /*  Array of Hidden Information */
  uint8_t hidden_record_info_array_valid;  /**< Must be set to true if hidden_record_info_array is being passed */
  uint32_t hidden_record_info_array_len;  /**< Must be set to # of elements in hidden_record_info_array */
  pbm_hidden_record_info_type_v01 hidden_record_info_array[QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01];
}pbm_record_read_ext_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
}pbm_num_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Adds a new extended record or modifies an existing extended record. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_base_record_info_type_v01 record_info;

  /* Optional */
  /*  Primary ADN Number Information */
  uint8_t num_valid;  /**< Must be set to true if num is being passed */
  pbm_num_info_type_v01 num;
  /**<   \n Primary number in ASCII.*/

  /* Optional */
  /*  Primary Name Information */
  uint8_t name_valid;  /**< Must be set to true if name is being passed */
  uint32_t name_len;  /**< Must be set to # of elements in name */
  uint16_t name[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   Name in UCS2.*/

  /* Optional */
  /*  Second Name Information */
  uint8_t sname_valid;  /**< Must be set to true if sname is being passed */
  uint32_t sname_len;  /**< Must be set to # of elements in sname */
  uint16_t sname[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   Second name in UCS2.*/

  /* Optional */
  /*  Additional Number Information */
  uint8_t ad_num_info_valid;  /**< Must be set to true if ad_num_info is being passed */
  uint32_t ad_num_info_len;  /**< Must be set to # of elements in ad_num_info */
  pbm_ad_num_info_type_v01 ad_num_info[QMI_PBM_AD_NUM_MAX_V01];

  /* Optional */
  /*  Group ID Information */
  uint8_t grp_id_valid;  /**< Must be set to true if grp_id is being passed */
  uint32_t grp_id_len;  /**< Must be set to # of elements in grp_id */
  uint8_t grp_id[QMI_PBM_GRP_ID_MAX_V01];
  /**<   Group ID. References the type of group (i.e., record number in the GAS
       elementary file on the card).
  */

  /* Optional */
  /*  Email Information */
  uint8_t email_info_valid;  /**< Must be set to true if email_info is being passed */
  uint32_t email_info_len;  /**< Must be set to # of elements in email_info */
  pbm_email_info_ext_type_v01 email_info[QMI_PBM_EMAIL_INFO_MAX_V01];

  /* Optional */
  /*  Hidden Information */
  uint8_t is_hidden_valid;  /**< Must be set to true if is_hidden is being passed */
  uint8_t is_hidden;
  /**<   Whether a record is hidden: \n
         - 0 -- FALSE \n
         - 1 -- TRUE
  */
}pbm_write_record_ext_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Adds a new extended record or modifies an existing extended record. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Record Information */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint16_t record_id;
  /**<   Identifier of the record that has been added or updated.*/
}pbm_write_record_ext_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Searches the records by a specified extended name or number. */
typedef struct {

  /* Mandatory */
  /*  Search Information */
  pbm_phonebook_info_type_v01 search_info;

  /* Optional */
  /*  Number Information */
  uint8_t number_valid;  /**< Must be set to true if number is being passed */
  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.*/

  /* Optional */
  /*  Name Information */
  uint8_t name_valid;  /**< Must be set to true if name is being passed */
  uint32_t name_len;  /**< Must be set to # of elements in name */
  uint16_t name[QMI_PBM_NAME_MAX_LENGTH_EXT_V01];
  /**<   Name in UCS2.*/
}pbm_search_records_ext_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Searches the records by a specified extended name or number. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Record IDs List */
  uint8_t record_id_valid;  /**< Must be set to true if record_id is being passed */
  uint32_t record_id_len;  /**< Must be set to # of elements in record_id */
  uint16_t record_id[QMI_PBM_REC_IDS_MAX_V01];
  /**<   Identifier of the record that matches the criterion.*/
}pbm_search_records_ext_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the Record Read operation for all extended hidden records. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */
}pbm_read_all_hidden_records_ext_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the Record Read operation for all extended hidden records. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Number of Records */
  uint8_t num_of_recs_valid;  /**< Must be set to true if num_of_recs is being passed */
  uint16_t num_of_recs;
  /**<   Indicates the total number of records returned in the subsequent
       QMI_PBM_RECORD_READ_EXT_IND indications.
  */
}pbm_read_all_hidden_records_ext_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_enums
    @{
  */
typedef enum {
  PBM_SLOT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  PBM_SLOT_NONE_V01 = 0, /**<  None  */
  PBM_SLOT_1_V01 = 1, /**<  Slot 1  */
  PBM_SLOT_2_V01 = 2, /**<  Slot 2  */
  PBM_SLOT_3_V01 = 3, /**<  Slot 3  */
  PBM_SLOT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}pbm_slot_type_enum_v01;
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Indicates that all available Phonebooks in the specified 
             SIM are ready to be accessed. */
typedef struct {

  /* Mandatory */
  /*  Slot Ready Information */
  pbm_slot_type_enum_v01 slot_id;
  /**<   Valid values: \n
      - PBM_SLOT_1 (1) --  Slot 1 
      - PBM_SLOT_2 (2) --  Slot 2 
      - PBM_SLOT_3 (3) --  Slot 3 
 */
}pbm_sim_init_done_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Initiates the undecoded Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Record Information */
  pbm_record_info_type_v01 record_info;
}pbm_read_records_ext_undecoded_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Initiates the undecoded Record Read operation by specifying the
             range of the records to be read. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Number of Records */
  uint8_t num_of_recs_valid;  /**< Must be set to true if num_of_recs is being passed */
  uint16_t num_of_recs;
  /**<   Indicates the number of records to be returned in subsequent 
       QMI_PBM_RECORD_READ_EXT_UNDECODED_IND indications.
  */
}pbm_read_records_ext_undecoded_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Record identifier. 
  */

  pbm_num_type_enum_v01 num_type;
  /**<   Type of number, as per \hyperref[S1]{[S1]}: \n
      - PBM_NUM_TYPE_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_TYPE_INTERNATIONAL (0x01) --  International 
      - PBM_NUM_TYPE_NATIONAL (0x02) --  Domestic 
      - PBM_NUM_TYPE_NETWORK_SPECIFIC (0x03) --  Network-specific 
      - PBM_NUM_TYPE_DEDICATED_ACCESS (0x04) --  Dedicated access 
 */

  pbm_num_plan_enum_v01 num_plan;
  /**<   Number plan: \n
      - PBM_NUM_PLAN_UNKNOWN (0x00) --  Unknown 
      - PBM_NUM_PLAN_ISDN (0x01) --  ISDN 
      - PBM_NUM_PLAN_DATA (0x02) --  Data 
      - PBM_NUM_PLAN_TELEX (0x03) --  Telex 
      - PBM_NUM_PLAN_NATIONAL (0x04) --  National 
      - PBM_NUM_PLAN_PRIVATE (0x05) --  Private 
 */

  uint32_t number_len;  /**< Must be set to # of elements in number */
  char number[QMI_PBM_NUMBER_MAX_LENGTH_V01];
  /**<   Number in ASCII.
  */

  uint32_t name_len;  /**< Must be set to # of elements in name */
  uint8_t name[QMI_PBM_NAME_MAX_LENGTH_EXT_UNDECODED_V01];
  /**<   Name read from SIM.
  */
}pbm_record_instance_undecoded_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t seq_num;
  /**<   Sequence number of the indication.
  */

  pbm_session_type_enum_v01 session_type;
  /**<   Session type details are provided in Table \ref{tbl:SessTypes}. */

  uint16_t pb_type;
  /**<   Phonebook types are provided in Table \ref{tbl:suppPB}. */

  uint32_t record_instances_len;  /**< Must be set to # of elements in record_instances */
  pbm_record_instance_undecoded_type_v01 record_instances[QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01];
}pbm_basic_record_data_undecoded_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.
  */

  uint32_t sname_len;  /**< Must be set to # of elements in sname */
  uint8_t sname[QMI_PBM_NAME_MAX_LENGTH_EXT_UNDECODED_V01];
  /**<   Second name (sname) as read from the SIM.
  */
}pbm_sname_info_undecoded_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t email_address_len;  /**< Must be set to # of elements in email_address */
  uint8_t email_address[QMI_PBM_EMAIL_MAX_LENGTH_EXT_UNDECODED_V01];
  /**<   Email as read from the SIM .
  */
}pbm_email_info_undecoded_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record_id;
  /**<   Identifier of the record returned.
  */

  uint32_t email_len;  /**< Must be set to # of elements in email */
  pbm_email_info_undecoded_type_v01 email[QMI_PBM_EMAIL_INFO_MAX_V01];
}pbm_email_record_info_undecoded_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Indication Message; Provides the undecoded record(s) that were requested by 
             QMI_PBM_READ_RECORDS_EXT_UNDECODED */
typedef struct {

  /* Mandatory */
  /*  Array of Basic Record Data */
  pbm_basic_record_data_undecoded_type_v01 basic_record_data;

  /* Optional */
  /*  Array of Second Name Information */
  uint8_t sname_info_array_valid;  /**< Must be set to true if sname_info_array is being passed */
  uint32_t sname_info_array_len;  /**< Must be set to # of elements in sname_info_array */
  pbm_sname_info_undecoded_type_v01 sname_info_array[QMI_PBM_MAX_NUM_SNAME_INFO_V01];

  /* Optional */
  /*  Array of Additional Number Information */
  uint8_t ad_num_info_array_valid;  /**< Must be set to true if ad_num_info_array is being passed */
  uint32_t ad_num_info_array_len;  /**< Must be set to # of elements in ad_num_info_array */
  pbm_ad_num_record_info_type_v01 ad_num_info_array[QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01];

  /* Optional */
  /*  Array of Group ID Information */
  uint8_t grp_id_info_array_valid;  /**< Must be set to true if grp_id_info_array is being passed */
  uint32_t grp_id_info_array_len;  /**< Must be set to # of elements in grp_id_info_array */
  pbm_grp_id_record_info_type_v01 grp_id_info_array[QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01];

  /* Optional */
  /*  Array of Email Information */
  uint8_t email_info_array_valid;  /**< Must be set to true if email_info_array is being passed */
  uint32_t email_info_array_len;  /**< Must be set to # of elements in email_info_array */
  pbm_email_record_info_undecoded_type_v01 email_info_array[QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01];

  /* Optional */
  /*  Array of Hidden Information */
  uint8_t hidden_record_info_array_valid;  /**< Must be set to true if hidden_record_info_array is being passed */
  uint32_t hidden_record_info_array_len;  /**< Must be set to # of elements in hidden_record_info_array */
  pbm_hidden_record_info_type_v01 hidden_record_info_array[QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01];
}pbm_record_read_ext_undecoded_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t phonebook_caching_status_mask_type_v01;
#define PBM_GPB_ENABLE_V01 ((phonebook_caching_status_mask_type_v01)0x01ull) /**<  Global phonebook enable  */
#define PBM_LPB_ENABLE_V01 ((phonebook_caching_status_mask_type_v01)0x02ull) /**<  Local phonebook enable  */
/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Configure the phonebook caching status of the QMI_PBM service. */
typedef struct {

  /* Mandatory */
  /*  Phonebook caching status */
  phonebook_caching_status_mask_type_v01 pbm_caching_status_mask;
  /**<   Bitmask of the caching status values to be configured: \n
      - PBM_GPB_ENABLE (0x01) --  Global phonebook enable 
      - PBM_LPB_ENABLE (0x02) --  Local phonebook enable 
 */
}pbm_set_configuration_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Configure the phonebook caching status of the QMI_PBM service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}pbm_set_configuration_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t phonebook_feature_status_mask_type_v01;
#define PBM_CENTRALIZED_ECC_V01 ((phonebook_feature_status_mask_type_v01)0x01ull) /**<  Centralized ECC feature  */
/** @addtogroup pbm_qmi_messages
    @{
  */
/** Request Message; Get configuration of the phonebook feature status. */
typedef struct {

  /* Optional */
  /*  Phonebook feature status */
  uint8_t pbm_feature_status_mask_valid;  /**< Must be set to true if pbm_feature_status_mask is being passed */
  phonebook_feature_status_mask_type_v01 pbm_feature_status_mask;
  /**<   Bitmask of the feature status values to be checked: \n
      - PBM_CENTRALIZED_ECC (0x01) --  Centralized ECC feature 
 \n
 All other bits are reserved for future use; if the TLV is missing,
 the service returns all feature status values in the response.
 */
}pbm_get_configuration_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup pbm_qmi_messages
    @{
  */
/** Response Message; Get configuration of the phonebook feature status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Optional */
  /*  Centralized ECC */
  uint8_t centralized_ecc_valid;  /**< Must be set to true if centralized_ecc is being passed */
  uint8_t centralized_ecc;
  /**<   Indicate whether centralized ECC is supported. Valid values:\n
         - 0 -- Centralized ECC is not supported\n
         - 1 -- Centralized ECC is supported
  */
}pbm_get_configuration_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_PBM_AAS_UPDATE_IND_V01 
//#define REMOVE_QMI_PBM_ALL_PB_INIT_DONE_IND_V01 
//#define REMOVE_QMI_PBM_BIND_SUBSCRIPTION_V01 
//#define REMOVE_QMI_PBM_DELETE_ALL_PB_RECORDS_V01 
//#define REMOVE_QMI_PBM_DELETE_RECORD_V01 
//#define REMOVE_QMI_PBM_EMERGENCY_LIST_IND_V01 
//#define REMOVE_QMI_PBM_GAS_UPDATE_IND_V01 
//#define REMOVE_QMI_PBM_GET_ALL_AAS_V01 
//#define REMOVE_QMI_PBM_GET_ALL_GROUPS_V01 
//#define REMOVE_QMI_PBM_GET_ALL_PB_CAPABILITIES_V01 
//#define REMOVE_QMI_PBM_GET_CONFIGURATION_V01 
//#define REMOVE_QMI_PBM_GET_EMERGENCY_LIST_V01 
//#define REMOVE_QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_V01 
//#define REMOVE_QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_V01 
//#define REMOVE_QMI_PBM_GET_PB_CAPABILITIES_V01 
//#define REMOVE_QMI_PBM_GET_PB_STATE_V01 
//#define REMOVE_QMI_PBM_GET_SUBSCRIPTION_BINDING_V01 
//#define REMOVE_QMI_PBM_HIDDEN_RECORD_STATUS_IND_V01 
//#define REMOVE_QMI_PBM_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_PBM_PBSET_CAP_READ_IND_V01 
//#define REMOVE_QMI_PBM_PB_READY_IND_V01 
//#define REMOVE_QMI_PBM_READ_ALL_HIDDEN_RECORDS_V01 
//#define REMOVE_QMI_PBM_READ_ALL_HIDDEN_RECORDS_EXT_V01 
//#define REMOVE_QMI_PBM_READ_PBSET_CAPS_V01 
//#define REMOVE_QMI_PBM_READ_RECORDS_V01 
//#define REMOVE_QMI_PBM_READ_RECORDS_EXT_V01 
//#define REMOVE_QMI_PBM_READ_RECORDS_EXT_UNDECODED_V01 
//#define REMOVE_QMI_PBM_RECORD_READ_EXT_IND_V01 
//#define REMOVE_QMI_PBM_RECORD_READ_EXT_UNDECODED_IND_V01 
//#define REMOVE_QMI_PBM_RECORD_READ_IND_V01 
//#define REMOVE_QMI_PBM_RECORD_UPDATE_IND_V01 
//#define REMOVE_QMI_PBM_REFRESH_IND_V01 
//#define REMOVE_QMI_PBM_SEARCH_RECORDS_V01 
//#define REMOVE_QMI_PBM_SEARCH_RECORDS_EXT_V01 
//#define REMOVE_QMI_PBM_SET_AAS_V01 
//#define REMOVE_QMI_PBM_SET_CONFIGURATION_V01 
//#define REMOVE_QMI_PBM_SET_GROUP_INFO_V01 
//#define REMOVE_QMI_PBM_SIM_INIT_DONE_IND_V01 
//#define REMOVE_QMI_PBM_WRITE_RECORD_V01 
//#define REMOVE_QMI_PBM_WRITE_RECORD_EXT_V01 

/*Service Message Definition*/
/** @addtogroup pbm_qmi_msg_ids
    @{
  */
#define QMI_PBM_INDICATION_REGISTER_REQ_V01 0x0001
#define QMI_PBM_INDICATION_REGISTER_RESP_V01 0x0001
#define QMI_PBM_GET_PB_CAPABILITIES_REQ_V01 0x0002
#define QMI_GET_PB_CAPABILITIES_RESP_V01 0x0002
#define QMI_PBM_GET_ALL_PB_CAPABILITIES_REQ_V01 0x0003
#define QMI_PBM_GET_ALL_PB_CAPABILITIES_RESP_V01 0x0003
#define QMI_PBM_READ_RECORDS_REQ_V01 0x0004
#define QMI_PBM_READ_RECORDS_RESP_V01 0x0004
#define QMI_PBM_RECORD_READ_IND_V01 0x0004
#define QMI_PBM_WRITE_RECORD_REQ_V01 0x0005
#define QMI_PBM_WRITE_RECORD_RESP_V01 0x0005
#define QMI_PBM_DELETE_RECORD_REQ_V01 0x0006
#define QMI_PBM_DELETE_RECORD_RESP_V01 0x0006
#define QMI_PBM_DELETE_ALL_PB_RECORDS_REQ_V01 0x0007
#define QMI_PBM_DELETE_ALL_PB_RECORDS_RESP_V01 0x0007
#define QMI_PBM_SEARCH_RECORDS_REQ_V01 0x0008
#define QMI_PBM_SEARCH_RECORDS_RESP_V01 0x0008
#define QMI_PBM_RECORD_UPDATE_IND_V01 0x0009
#define QMI_PBM_REFRESH_IND_V01 0x000A
#define QMI_PBM_PB_READY_IND_V01 0x000B
#define QMI_PBM_EMERGENCY_LIST_IND_V01 0x000C
#define QMI_PBM_ALL_PB_INIT_DONE_IND_V01 0x000D
#define QMI_PBM_GET_EMERGENCY_LIST_REQ_V01 0x000E
#define QMI_PBM_GET_EMERGENCY_LIST_RESP_V01 0x000E
#define QMI_PBM_GET_ALL_GROUPS_REQ_V01 0x000F
#define QMI_PBM_GET_ALL_GROUPS_RESP_V01 0x000F
#define QMI_PBM_SET_GROUP_INFO_REQ_V01 0x0010
#define QMI_PBM_SET_GROUP_INFO_RESP_V01 0x0010
#define QMI_PBM_GET_PB_STATE_REQ_V01 0x0011
#define QMI_PBM_GET_PB_STATE_RESP_V01 0x0011
#define QMI_PBM_READ_ALL_HIDDEN_RECORDS_REQ_V01 0x0012
#define QMI_PBM_READ_ALL_HIDDEN_RECORDS_RESP_V01 0x0012
#define QMI_PBM_HIDDEN_RECORD_STATUS_IND_V01 0x0013
#define QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_REQ_V01 0x0014
#define QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_RESP_V01 0x0014
#define QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_REQ_V01 0x0015
#define QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_RESP_V01 0x0015
#define QMI_PBM_GET_ALL_AAS_REQ_V01 0x0016
#define QMI_PBM_GET_ALL_AAS_RESP_V01 0x0016
#define QMI_PBM_SET_AAS_REQ_V01 0x0017
#define QMI_PBM_SET_AAS_RESP_V01 0x0017
#define QMI_PBM_AAS_UPDATE_IND_V01 0x0018
#define QMI_PBM_GAS_UPDATE_IND_V01 0x0019
#define QMI_PBM_BIND_SUBSCRIPTION_REQ_V01 0x001A
#define QMI_PBM_BIND_SUBSCRIPTION_RESP_V01 0x001A
#define QMI_PBM_GET_SUBSCRIPTION_BINDING_REQ_V01 0x001B
#define QMI_PBM_GET_SUBSCRIPTION_BINDING_RESP_V01 0x001B
#define QMI_PBM_READ_PBSET_CAPS_REQ_V01 0x001C
#define QMI_PBM_READ_PBSET_CAPS_RESP_V01 0x001C
#define QMI_PBM_PBSET_CAP_READ_IND_V01 0x001C
#define QMI_PBM_READ_RECORDS_EXT_REQ_V01 0x001D
#define QMI_PBM_READ_RECORDS_EXT_RESP_V01 0x001D
#define QMI_PBM_RECORD_READ_EXT_IND_V01 0x001D
#define QMI_PBM_WRITE_RECORD_EXT_REQ_V01 0x001E
#define QMI_PBM_WRITE_RECORD_EXT_RESP_V01 0x001E
#define QMI_PBM_SEARCH_RECORDS_EXT_REQ_V01 0x001F
#define QMI_PBM_SEARCH_RECORDS_EXT_RESP_V01 0x001F
#define QMI_PBM_READ_ALL_HIDDEN_RECORDS_EXT_REQ_V01 0x0020
#define QMI_PBM_READ_ALL_HIDDEN_RECORDS_EXT_RESP_V01 0x0020
#define QMI_PBM_SIM_INIT_DONE_IND_V01 0x0021
#define QMI_PBM_READ_RECORDS_EXT_UNDECODED_REQ_V01 0x0022
#define QMI_PBM_READ_RECORDS_EXT_UNDECODED_RESP_V01 0x0022
#define QMI_PBM_RECORD_READ_EXT_UNDECODED_IND_V01 0x0022
#define QMI_PBM_SET_CONFIGURATION_REQ_V01 0x0023
#define QMI_PBM_SET_CONFIGURATION_RESP_V01 0x0023
#define QMI_PBM_GET_CONFIGURATION_REQ_V01 0x0024
#define QMI_PBM_GET_CONFIGURATION_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro pbm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type pbm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define pbm_get_service_object_v01( ) \
          pbm_get_service_object_internal_v01( \
            PBM_V01_IDL_MAJOR_VERS, PBM_V01_IDL_MINOR_VERS, \
            PBM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

