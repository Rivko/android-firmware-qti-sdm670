#ifndef AOSTLM_AUDIT_SERVICE_01_H
#define AOSTLM_AUDIT_SERVICE_01_H
/**
  @file aostlm_audit_v01.h

  @brief This is the public header file which defines the aostlm_audit service Data structures.

  This header file defines the types and structures that were defined in
  aostlm_audit. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/aostlm_audit/api/aostlm_audit_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Fri Jan 23 2015 (Spin 0)
   From IDL File: aostlm_audit_v01.idl */

/** @defgroup aostlm_audit_qmi_consts Constant values defined in the IDL */
/** @defgroup aostlm_audit_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup aostlm_audit_qmi_enums Enumerated types used in QMI messages */
/** @defgroup aostlm_audit_qmi_messages Structures sent as QMI messages */
/** @defgroup aostlm_audit_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup aostlm_audit_qmi_accessor Accessor for QMI service object */
/** @defgroup aostlm_audit_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aostlm_audit_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define AOSTLM_AUDIT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define AOSTLM_AUDIT_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define AOSTLM_AUDIT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define AOSTLM_AUDIT_V01_MAX_MESSAGE_ID 0x0020
/**
    @}
  */


/** @addtogroup aostlm_audit_qmi_consts 
    @{ 
  */

/**  Maximum length of the AOSTLM QMI message  */
#define AOSTLM_MAX_AUDIT_MSG_LENGTH_V01 3072
/**
    @}
  */

/** @addtogroup aostlm_audit_qmi_enums
    @{
  */
typedef enum {
  AOSTLM_INSTANCE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  AOSTLM_AUDIT_INSTANCE_MPSS_V01 = 0, 
  AOSTLM_AUDIT_INSTANCE_ADSP_V01 = 1, 
  AOSTLM_INSTANCE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}aostlm_instance_enum_type_v01;
/**
    @}
  */

/** @addtogroup aostlm_audit_qmi_messages
    @{
  */
/** Request Message; This command sends the request for an audit report to the appropriate
    subsystem */
typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}aostlm_audit_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup aostlm_audit_qmi_messages
    @{
  */
/** Response Message; This command sends the request for an audit report to the appropriate
    subsystem */
typedef struct {

  /* Mandatory */
  /*  Response */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Audit Report */
  uint8_t audit_report_valid;  /**< Must be set to true if audit_report is being passed */
  uint32_t audit_report_len;  /**< Must be set to # of elements in audit_report */
  uint8_t audit_report[AOSTLM_MAX_AUDIT_MSG_LENGTH_V01];
  /**<   The audit_report field is encrypted using a shared-secret key.
       It contains the list of feature IDs that have been activated
       in a subsystem (ADSP, MPSS) and a count of how many times
       that feature has been used.
  */
}aostlm_audit_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_AOSTLM_AUDIT_REQ_V01 

/*Service Message Definition*/
/** @addtogroup aostlm_audit_qmi_msg_ids
    @{
  */
#define AOSTLM_AUDIT_REQ_V01 0x0020
#define AOSTLM_AUDIT_RESP_V01 0x0020
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro aostlm_audit_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type aostlm_audit_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define aostlm_audit_get_service_object_v01( ) \
          aostlm_audit_get_service_object_internal_v01( \
            AOSTLM_AUDIT_V01_IDL_MAJOR_VERS, AOSTLM_AUDIT_V01_IDL_MINOR_VERS, \
            AOSTLM_AUDIT_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

