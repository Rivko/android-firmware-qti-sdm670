/*
-----------------------------------------------------------------
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------
*/
#ifndef SERVREG_LOC_SERVICE_01_H
#define SERVREG_LOC_SERVICE_01_H
/**
  @file service_registry_locator_v01.h

  @brief This is the public header file which defines the servreg_loc service Data structures.

  This header file defines the types and structures that were defined in
  servreg_loc. It contains the constant values defined, enums, structures,
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

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Nov 30 2016 (Spin 0)
   From IDL File: service_registry_locator_v01.idl */

/** @defgroup servreg_loc_qmi_consts Constant values defined in the IDL */
/** @defgroup servreg_loc_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup servreg_loc_qmi_enums Enumerated types used in QMI messages */
/** @defgroup servreg_loc_qmi_messages Structures sent as QMI messages */
/** @defgroup servreg_loc_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup servreg_loc_qmi_accessor Accessor for QMI service object */
/** @defgroup servreg_loc_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup servreg_loc_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SERVREG_LOC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SERVREG_LOC_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SERVREG_LOC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SERVREG_LOC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup servreg_loc_qmi_consts
    @{
  */

/**  Max length of the domain name i.e "soc/domain/subdomain" or service name i.e "provider/service" is 64 bytes */
#define QMI_SERVREG_LOC_NAME_LENGTH_V01 64

/**  Max length of the PD(Process Domain) failure reason */
#define QMI_SERVREG_LOC_PFR_LEN_V01 256

/**  Max number of entries in the array carrying domain or service names */
#define QMI_SERVREG_LOC_LIST_LENGTH_V01 32
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_aggregates
    @{
  */
/**  Struct representing the name of the service or domain and the instance id
 */
typedef struct {

  char name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
  /**<   Name of the service or domain. */

  uint32_t instance_id;
  /**<   Instance ID. */

  uint8_t service_data_valid;
  /**<   Indicates whether the service data is valid. */

  uint32_t service_data;
  /**<   Service data. */
}servreg_loc_entry_v01;  /* Type */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_enums
    @{
  */
typedef enum {
  QMI_SERVREG_LOC_SERVICE_INSTANCE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_SERVREG_LOC_SERVICE_INSTANCE_APSS_V01 = 0x1, /**<  APSS service instance  */
  QMI_SERVREG_LOC_SERVICE_INSTANCE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_servreg_loc_service_instance_enum_type_v01;
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Request Message; Registers or deregisters a client to get update indication messages. */
typedef struct {

  /* Optional */
  /*  Enable Database Updated Indication */
  uint8_t enable_database_updated_indication_valid;  /**< Must be set to true if enable_database_updated_indication is being passed */
  uint8_t enable_database_updated_indication;
  /**<   Flag set to TRUE for registration and FALSE for deregistration 
      for updates. The update indication for this request is sent via the 
      QMI_SERVREG_LOC_DATABASE_UPDATED_IND indication. 
  */
}qmi_servreg_loc_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Response Message; Registers or deregisters a client to get update indication messages. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qmi_servreg_loc_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Request Message; Clients use this command to get the domain list for the selected
    service. */
typedef struct {

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
  /**<   Name of the service.
 */

  /* Optional */
  /*  Domain Offset */
  uint8_t domain_offset_valid;  /**< Must be set to true if domain_offset is being passed */
  uint32_t domain_offset;
  /**<   Domain offset. If the total number of domains is not received, clients
      can request the remaining domains starting with this offset value.
 */
}qmi_servreg_loc_get_domain_list_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Response Message; Clients use this command to get the domain list for the selected
    service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Total Number of Domains */
  uint8_t total_domains_valid;  /**< Must be set to true if total_domains is being passed */
  uint16_t total_domains;
  /**<   Total number of domains.
 */

  /* Optional */
  /*  Database Revision Count */
  uint8_t db_rev_count_valid;  /**< Must be set to true if db_rev_count is being passed */
  uint16_t db_rev_count;
  /**<   Database revision count. This value changes if the domain database on the
      server is updated.
 */

  /* Optional */
  /*  Domain List */
  uint8_t domain_list_valid;  /**< Must be set to true if domain_list is being passed */
  uint32_t domain_list_len;  /**< Must be set to # of elements in domain_list */
  servreg_loc_entry_v01 domain_list[QMI_SERVREG_LOC_LIST_LENGTH_V01];
}qmi_servreg_loc_get_domain_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Request Message; Pushes the domain name and list of services to the server that forms
           the central service registry database (on TN APSS only). */
typedef struct {

  /* Mandatory */
  /*  Domain Name */
  char domain_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];

  /* Mandatory */
  /*  Service List */
  uint32_t service_list_len;  /**< Must be set to # of elements in service_list */
  servreg_loc_entry_v01 service_list[QMI_SERVREG_LOC_LIST_LENGTH_V01];
}qmi_servreg_loc_register_service_list_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Response Message; Pushes the domain name and list of services to the server that forms
           the central service registry database (on TN APSS only). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qmi_servreg_loc_register_service_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Indication Message; This indication notifies clients when the central database is updated 
           (on TN APSS only). */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_servreg_loc_database_updated_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Request Message; Send PD failure reason information to the service. */
typedef struct {

  /* Mandatory */
  /*  Service name  */
  char serv_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];

  /* Mandatory */
  /*  PD failure information string */
  char pd_failure_reason[QMI_SERVREG_LOC_PFR_LEN_V01 + 1];
}qmi_servreg_loc_pfr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_loc_qmi_messages
    @{
  */
/** Response Message; Send PD failure reason information to the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qmi_servreg_loc_pfr_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SERVREG_LOC_DATABASE_UPDATED_IND_V01 
//#define REMOVE_QMI_SERVREG_LOC_GET_DOMAIN_LIST_V01 
//#define REMOVE_QMI_SERVREG_LOC_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_SERVREG_LOC_PFR_V01 
//#define REMOVE_QMI_SERVREG_LOC_REGISTER_DOMAIN_LIST_V01 

/*Service Message Definition*/
/** @addtogroup servreg_loc_qmi_msg_ids
    @{
  */
#define QMI_SERVREG_LOC_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_SERVREG_LOC_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_SERVREG_LOC_GET_DOMAIN_LIST_REQ_V01 0x0021
#define QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01 0x0021
#define QMI_SERVREG_LOC_REGISTER_SERVICE_LIST_REQ_V01 0x0022
#define QMI_SERVREG_LOC_REGISTER_SERVICE_LIST_RESP_V01 0x0022
#define QMI_SERVREG_LOC_DATABASE_UPDATED_IND_V01 0x0023
#define QMI_SERVREG_LOC_PFR_REQ_V01 0x0024
#define QMI_SERVREG_LOC_PFR_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro servreg_loc_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type servreg_loc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define servreg_loc_get_service_object_v01( ) \
          servreg_loc_get_service_object_internal_v01( \
            SERVREG_LOC_V01_IDL_MAJOR_VERS, SERVREG_LOC_V01_IDL_MINOR_VERS, \
            SERVREG_LOC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

