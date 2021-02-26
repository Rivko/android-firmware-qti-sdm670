#ifndef IP_MULTIMEDIA_SUBSYSTEM_IMS_COMMON_SERVICE_01_H
#define IP_MULTIMEDIA_SUBSYSTEM_IMS_COMMON_SERVICE_01_H
/**
  @file ip_multimedia_subsystem_ims_common_v01.h

  @brief This is the public header file which defines the ip_multimedia_subsystem_ims_common service Data structures.

  This header file defines the types and structures that were defined in
  ip_multimedia_subsystem_ims_common. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2017 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/imss/api/ip_multimedia_subsystem_ims_common_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Jan 31 2017 (Spin 0)
   From IDL File: ip_multimedia_subsystem_ims_common_v01.idl */

/** @defgroup ip_multimedia_subsystem_ims_common_qmi_consts Constant values defined in the IDL */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_messages Structures sent as QMI messages */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_accessor Accessor for QMI service object */
/** @defgroup ip_multimedia_subsystem_ims_common_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ip_multimedia_subsystem_ims_common_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IP_MULTIMEDIA_SUBSYSTEM_IMS_COMMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IP_MULTIMEDIA_SUBSYSTEM_IMS_COMMON_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IP_MULTIMEDIA_SUBSYSTEM_IMS_COMMON_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup ip_multimedia_subsystem_ims_common_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup ip_multimedia_subsystem_ims_common_qmi_enums
    @{
  */
typedef enum {
  IMS_COMMON_RESP_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_COMMON_MSG_NO_ERR_V01 = 0, /**<  No error  */
  IMS_COMMON_MSG_IMS_NOT_READY_V01 = 1, /**<  Service indication  */
  IMS_COMMON_MSG_FILE_NOT_AVAILABLE_V01 = 2, /**<  Settings file is not available  */
  IMS_COMMON_MSG_READ_FAILED_V01 = 3, /**<  Read failure  */
  IMS_COMMON_MSG_WRITE_FAILED_V01 = 4, /**<  Write failure  */
  IMS_COMMON_MSG_OTHER_INTERNAL_ERR_V01 = 5, /**<  Other settings internal error  */
  IMS_COMMON_RESP_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_common_resp_enum_v01;
/**
    @}
  */

/** @addtogroup ip_multimedia_subsystem_ims_common_qmi_enums
    @{
  */
typedef enum {
  IMS_SUBSCRIPTION_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_SUBSCRIPTION_TYPE_NONE_V01 = -1, /**<  None subscription  */
  IMS_SUBSCRIPTION_TYPE_PRIMARY_V01 = 0, /**<  Primary subscription  */
  IMS_SUBSCRIPTION_TYPE_SECONDARY_V01 = 1, /**<  Secondary subscription  */
  IMS_SUBSCRIPTION_TYPE_TERTIARY_V01 = 2, /**<  Tertiary subscription  */
  IMS_SUBSCRIPTION_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_subscription_type_enum_v01;
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ip_multimedia_subsystem_ims_common_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

