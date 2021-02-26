#ifndef QUPM_SERVICE_01_H
#define QUPM_SERVICE_01_H
/**
  @file qup_manager_v01.h

  @brief This is the public header file which defines the qupm service Data structures.

  This header file defines the types and structures that were defined in
  qupm. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/qupm/api/qup_manager_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.13 
   It was generated on: Wed Sep 24 2014 (Spin 0)
   From IDL File: qup_manager_v01.idl */

/** @defgroup qupm_qmi_consts Constant values defined in the IDL */
/** @defgroup qupm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qupm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qupm_qmi_messages Structures sent as QMI messages */
/** @defgroup qupm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qupm_qmi_accessor Accessor for QMI service object */
/** @defgroup qupm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qupm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QUPM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QUPM_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QUPM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QUPM_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup qupm_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup qupm_qmi_enums
    @{
  */
typedef enum {
  QUPM_INSTANCE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUPM_INSTANCE_ADSP_V01 = 1, 
  QUPM_INSTANCE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qupm_instance_enum_type_v01;
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Request Message; This command returns the ready status of the QUP 
                      Manager service */
typedef struct {

  /* Mandatory */
  /*  QUP ID */
  uint32_t qup_id;
  /**<   QUP ID hardware instance (1:N)*/

  /* Optional */
  /*  Optional Flags */
  uint8_t flags_valid;  /**< Must be set to true if flags is being passed */
  uint32_t flags;
  /**<   OEM specific flags*/
}qupm_ready_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Response Message; This command returns the ready status of the QUP 
                      Manager service */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type*/

  /* Optional */
  /*  OEM Status */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  uint32_t status;
  /**<   OEM specific status response*/
}qupm_ready_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Request Message; This command transfers QUP ownership to the service host */
typedef struct {

  /* Mandatory */
  /*  QUP ID */
  uint32_t qup_id;
  /**<   QUP ID hardware instance (1:N)*/

  /* Optional */
  /*  Optional Flags */
  uint8_t flags_valid;  /**< Must be set to true if flags is being passed */
  uint32_t flags;
  /**<   OEM specific flags*/
}qupm_take_ownership_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Response Message; This command transfers QUP ownership to the service host */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type*/

  /* Optional */
  /*  OEM Status */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  uint32_t status;
  /**<   OEM specific status response*/
}qupm_take_ownership_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Request Message; This command transfers QUP ownership to the caller (APSS) */
typedef struct {

  /* Mandatory */
  /*  QUP ID */
  uint32_t qup_id;
  /**<   QUP ID hardware instance (1:N)*/

  /* Optional */
  /*  Optional Flags */
  uint8_t flags_valid;  /**< Must be set to true if flags is being passed */
  uint32_t flags;
  /**<   OEM specific flags*/
}qupm_give_ownership_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qupm_qmi_messages
    @{
  */
/** Response Message; This command transfers QUP ownership to the caller (APSS) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type*/

  /* Optional */
  /*  OEM Status */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  uint32_t status;
  /**<   OEM specific status response*/
}qupm_give_ownership_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_QUPM_GIVE_OWNERSHIP_V01 
//#define REMOVE_QMI_QUPM_READY_V01 
//#define REMOVE_QMI_QUPM_TAKE_OWNERSHIP_V01 

/*Service Message Definition*/
/** @addtogroup qupm_qmi_msg_ids
    @{
  */
#define QMI_QUPM_READY_REQ_V01 0x0020
#define QMI_QUPM_READY_RESP_V01 0x0020
#define QMI_QUPM_TAKE_OWNERSHIP_REQ_V01 0x0021
#define QMI_QUPM_TAKE_OWNERSHIP_RESP_V01 0x0021
#define QMI_QUPM_GIVE_OWNERSHIP_REQ_V01 0x0022
#define QMI_QUPM_GIVE_OWNERSHIP_RESP_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qupm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qupm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define qupm_get_service_object_v01( ) \
          qupm_get_service_object_internal_v01( \
            QUPM_V01_IDL_MAJOR_VERS, QUPM_V01_IDL_MINOR_VERS, \
            QUPM_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

