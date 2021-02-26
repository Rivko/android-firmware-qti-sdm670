#ifndef SLIMBUS_SERVICE_01_H
#define SLIMBUS_SERVICE_01_H
/**
  @file slimbus_v01.h

  @brief This is the public header file which defines the slimbus service Data structures.

  This header file defines the types and structures that were defined in
  slimbus. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/slimbus/api/slimbus_v01.h#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Oct 28 2016 (Spin 0)
   From IDL File: slimbus_v01.idl */

/** @defgroup slimbus_qmi_consts Constant values defined in the IDL */
/** @defgroup slimbus_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup slimbus_qmi_enums Enumerated types used in QMI messages */
/** @defgroup slimbus_qmi_messages Structures sent as QMI messages */
/** @defgroup slimbus_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup slimbus_qmi_accessor Accessor for QMI service object */
/** @defgroup slimbus_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup slimbus_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SLIMBUS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SLIMBUS_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SLIMBUS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SLIMBUS_V01_MAX_MESSAGE_ID 0x0023
/**
    @}
  */


/** @addtogroup slimbus_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup slimbus_qmi_enums
    @{
  */
typedef enum {
  SLIMBUS_MODE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SLIMBUS_MODE_SATELLITE_V01 = 1, /**<  Satellite mode  */
  SLIMBUS_MODE_MASTER_V01 = 2, /**<  Master mode  */
  SLIMBUS_MODE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}slimbus_mode_enum_type_v01;
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Request Message; Selects the SLIMbus hardware instance to be used for 
                      subsequent commands. */
typedef struct {

  /* Mandatory */
  /*  Hardware Instance Selection */
  uint32_t instance;
  /**<   Zero-based SLIMbus hardware instance. */

  /* Optional */
  /*  Mode Request Operation */
  uint8_t mode_valid;  /**< Must be set to true if mode is being passed */
  slimbus_mode_enum_type_v01 mode;
  /**<   Software driver mode. Values: \n
      - SLIMBUS_MODE_SATELLITE (1) --  Satellite mode 
      - SLIMBUS_MODE_MASTER (2) --  Master mode  */
}slimbus_select_inst_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Response Message; Selects the SLIMbus hardware instance to be used for 
                      subsequent commands. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type */
}slimbus_select_inst_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_enums
    @{
  */
typedef enum {
  SLIMBUS_PM_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SLIMBUS_PM_INACTIVE_V01 = 1, /**<  SLIMbus hardware is allowed to be idle  */
  SLIMBUS_PM_ACTIVE_V01 = 2, /**<  SLIMbus hardware is in the Active state  */
  SLIMBUS_PM_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}slimbus_pm_enum_type_v01;
/**
    @}
  */

/** @addtogroup slimbus_qmi_enums
    @{
  */
typedef enum {
  SLIMBUS_RESP_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SLIMBUS_RESP_SYNCHRONOUS_V01 = 1, /**<  Response to power request is sent synchronously  */
  SLIMBUS_RESP_DEFERRED_V01 = 2, /**<  Response to power request is deferred until 
        QMI_SLIMBUS_SEND_DEFERRED_RESPONSE request is sent  */
  SLIMBUS_RESP_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}slimbus_resp_enum_type_v01;
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Request Message; Performs a power request for SLIMbus. */
typedef struct {

  /* Mandatory */
  /*  Power Request Operation */
  slimbus_pm_enum_type_v01 pm_req;
  /**<   Slimbus hardware power state. Values: \n
      - SLIMBUS_PM_INACTIVE (1) --  SLIMbus hardware is allowed to be idle 
      - SLIMBUS_PM_ACTIVE (2) --  SLIMbus hardware is in the Active state  */

  /* Optional */
  /*  Response Type */
  uint8_t resp_type_valid;  /**< Must be set to true if resp_type is being passed */
  slimbus_resp_enum_type_v01 resp_type;
  /**<   Slimbus response type. Values: \n
      - SLIMBUS_RESP_SYNCHRONOUS (1) --  Response to power request is sent synchronously 
      - SLIMBUS_RESP_DEFERRED (2) --  Response to power request is deferred until 
        QMI_SLIMBUS_SEND_DEFERRED_RESPONSE request is sent  */
}slimbus_power_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Response Message; Performs a power request for SLIMbus. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type */
}slimbus_power_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Request Message; Requests SLIMbus to check the framer state and perform
                      a recovery if the framer is in a bad state. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}slimbus_check_framer_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Response Message; Requests SLIMbus to check the framer state and perform
                      a recovery if the framer is in a bad state. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type */
}slimbus_check_framer_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Request Message; Check and send any deferred responses. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}slimbus_send_deferred_response_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup slimbus_qmi_messages
    @{
  */
/** Response Message; Check and send any deferred responses. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type */
}slimbus_send_deferred_response_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SLIMBUS_CHECK_FRAMER_V01 
//#define REMOVE_QMI_SLIMBUS_POWER_V01 
//#define REMOVE_QMI_SLIMBUS_SELECT_INSTANCE_V01 
//#define REMOVE_QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_V01 

/*Service Message Definition*/
/** @addtogroup slimbus_qmi_msg_ids
    @{
  */
#define QMI_SLIMBUS_SELECT_INSTANCE_REQ_V01 0x0020
#define QMI_SLIMBUS_SELECT_INSTANCE_RESP_V01 0x0020
#define QMI_SLIMBUS_POWER_REQ_V01 0x0021
#define QMI_SLIMBUS_POWER_RESP_V01 0x0021
#define QMI_SLIMBUS_CHECK_FRAMER_REQ_V01 0x0022
#define QMI_SLIMBUS_CHECK_FRAMER_RESP_V01 0x0022
#define QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_REQ_V01 0x0023
#define QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_RESP_V01 0x0023
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro slimbus_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type slimbus_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define slimbus_get_service_object_v01( ) \
          slimbus_get_service_object_internal_v01( \
            SLIMBUS_V01_IDL_MAJOR_VERS, SLIMBUS_V01_IDL_MINOR_VERS, \
            SLIMBUS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif


