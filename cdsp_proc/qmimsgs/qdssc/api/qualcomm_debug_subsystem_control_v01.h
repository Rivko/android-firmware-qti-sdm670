#ifndef QDSSC_SERVICE_01_H
#define QDSSC_SERVICE_01_H
/**
  @file qualcomm_debug_subsystem_control_v01.h

  @brief This is the public header file which defines the qdssc service Data structures.

  This header file defines the types and structures that were defined in
  qdssc. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/qdssc/api/qualcomm_debug_subsystem_control_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon May 13 2019 (Spin 0)
   From IDL File: qualcomm_debug_subsystem_control_v01.idl */

/** @defgroup qdssc_qmi_consts Constant values defined in the IDL */
/** @defgroup qdssc_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qdssc_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qdssc_qmi_messages Structures sent as QMI messages */
/** @defgroup qdssc_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qdssc_qmi_accessor Accessor for QMI service object */
/** @defgroup qdssc_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qdssc_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QDSSC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QDSSC_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QDSSC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QDSSC_V01_MAX_MESSAGE_ID 0x002F
/**
    @}
  */


/** @addtogroup qdssc_qmi_consts
    @{
  */

/** 

 Software event tag */
#define QDSSC_SWEVT_TAG_MAX_LEN_V01 12

/** 

 ETM configuration command */
#define QDSSC_ETM_CONFIG_MAX_LEN_V01 128
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_SVC_INSTANCE_ID_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01 = 0, 
  QDSSC_SVC_INSTANCE_ID_APPS_V01 = 1, /**<  Applications  */
  QDSSC_SVC_INSTANCE_ID_MODEM_V01 = 2, /**<  Modem  */
  QDSSC_SVC_INSTANCE_ID_WCN_V01 = 3, /**<  Wireless Communication Network (WCN)/Pronto  */
  QDSSC_SVC_INSTANCE_ID_RPM_V01 = 4, /**<  Resource Power Manager (RPM)  */
  QDSSC_SVC_INSTANCE_ID_ADSP_V01 = 5, /**<  Audio Digital Signal Processor (ADSP)  */
  QDSSC_SVC_INSTANCE_ID_VENUS_V01 = 6, 
  QDSSC_SVC_INSTANCE_ID_GNSS_V01 = 7, 
  QDSSC_SVC_INSTANCE_ID_SENSOR_V01 = 8, /**<  Sensor (Sensor root)  */
  QDSSC_SVC_INSTANCE_ID_AUDIO_V01 = 9, /**<  Audio  */
  QDSSC_SVC_INSTANCE_ID_VPU_V01 = 10, 
  QDSSC_SVC_INSTANCE_ID_MODEM2_V01 = 11, /**<  Modem2  */
  QDSSC_SVC_INSTANCE_ID_SENSOR2_V01 = 12, /**<  Sensor2 (Sensor user)  */
  QDSSC_SVC_INSTANCE_ID_CDSP_V01 = 13, /**<  CDSP  */
  QDSSC_SVC_INSTANCE_ID_NPU_V01 = 14, /**<  NPU  */
  QDSSC_SVC_INSTANCE_ID_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_svc_instance_id_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_SWT_OUTPUT_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_SWT_OUTPUT_STATE_DISABLED_V01 = 0, /**<   Disabled \n  */
  QDSSC_SWT_OUTPUT_STATE_ENABLED_V01 = 1, /**<   Enabled  */
  QDSSC_SWT_OUTPUT_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_swt_output_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the state of software tracing on the processor. This state reflects
           the global control of all software-instrumented traces. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qdssc_get_swt_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the state of software tracing on the processor. This state reflects
           the global control of all software-instrumented traces. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Software Trace Output State */
  uint8_t state_valid;  /**< Must be set to true if state is being passed */
  qdssc_swt_output_state_enum_type_v01 state;
  /**<   Present when the result code in the response is QMI_RESULT_SUCCESS.
 Values: \n
      - QDSSC_SWT_OUTPUT_STATE_DISABLED (0) --   Disabled \n 
      - QDSSC_SWT_OUTPUT_STATE_ENABLED (1) --   Enabled 
 */
}qdssc_get_swt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables all software-instrumented tracing on the processor. */
typedef struct {

  /* Mandatory */
  /*  Software Trace Output State */
  qdssc_swt_output_state_enum_type_v01 state;
  /**<   State to configure the software trace output.
 Values: \n
      - QDSSC_SWT_OUTPUT_STATE_DISABLED (0) --   Disabled \n 
      - QDSSC_SWT_OUTPUT_STATE_ENABLED (1) --   Enabled 
 */
}qdssc_set_swt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables all software-instrumented tracing on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_swt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_ENTITY_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_ENTITY_NONE_V01 = 0, /**<  None \n  */
  QDSSC_ENTITY_TDS_V01 = 1, /**<   Standard trace \n  */
  QDSSC_ENTITY_ULOG_V01 = 11, /**<   Universal log messages \n  */
  QDSSC_ENTITY_PROF_V01 = 12, /**<   System profiling trace \n  */
  QDSSC_ENTITY_DIAG_V01 = 13, /**<   Diagnostic message logs  */
  QDSSC_ENTITY_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_entity_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_ENTITY_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_ENTITY_STATE_DISABLED_V01 = 0, /**<  Disabled \n  */
  QDSSC_ENTITY_STATE_ENABLED_V01 = 1, /**<  Enabled  */
  QDSSC_ENTITY_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_entity_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the state of software tracing from an entity on the processor. */
typedef struct {

  /* Mandatory */
  /*  Entity ID */
  qdssc_entity_enum_type_v01 entity_id;
  /**<   Entity ID associated with a type of software trace.
 Values: \n
      - QDSSC_ENTITY_NONE (0) --  None \n 
      - QDSSC_ENTITY_TDS (1) --   Standard trace \n 
      - QDSSC_ENTITY_ULOG (11) --   Universal log messages \n 
      - QDSSC_ENTITY_PROF (12) --   System profiling trace \n 
      - QDSSC_ENTITY_DIAG (13) --   Diagnostic message logs 
 */
}qdssc_get_entity_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the state of software tracing from an entity on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Entity State */
  uint8_t state_valid;  /**< Must be set to true if state is being passed */
  qdssc_entity_state_enum_type_v01 state;
  /**<   State of the entity's software trace output.
 Values: \n
      - QDSSC_ENTITY_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ENTITY_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_get_entity_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables software tracing from an entity on the processor. */
typedef struct {

  /* Mandatory */
  /*  Entity ID */
  qdssc_entity_enum_type_v01 entity_id;
  /**<   Entity ID associated with a type of software trace. Values:\n
      - QDSSC_ENTITY_NONE (0) --  None \n 
      - QDSSC_ENTITY_TDS (1) --   Standard trace \n 
      - QDSSC_ENTITY_ULOG (11) --   Universal log messages \n 
      - QDSSC_ENTITY_PROF (12) --   System profiling trace \n 
      - QDSSC_ENTITY_DIAG (13) --   Diagnostic message logs 
 */

  /* Mandatory */
  /*  Entity State */
  qdssc_entity_state_enum_type_v01 state;
  /**<   State to configure software tracing associated with the entity.
 Values: \n
      - QDSSC_ENTITY_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ENTITY_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_set_entity_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables software tracing from an entity on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_entity_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the state of software tracing for all entities on the processor. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qdssc_get_entity_all_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the state of software tracing for all entities on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Mask of Entity States */
  uint8_t mask_valid;  /**< Must be set to true if mask is being passed */
  uint32_t mask[8];
  /**<   Bitmask of the state of software trace output for all entities.
    */
}qdssc_get_entity_all_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables software tracing from all entities on the processor. */
typedef struct {

  /* Mandatory */
  /*  Entity State */
  qdssc_entity_state_enum_type_v01 state;
  /**<   State to configure software tracing of all entities.
 Values: \n
      - QDSSC_ENTITY_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ENTITY_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_set_entity_all_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables software tracing from all entities on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_entity_all_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the software event tag of the processor. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qdssc_get_swevt_tag_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the software event tag of the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Software Event Tag String */
  uint8_t tag_str_valid;  /**< Must be set to true if tag_str is being passed */
  char tag_str[QDSSC_SWEVT_TAG_MAX_LEN_V01 + 1];
  /**<   Tag for software events associated with the processor.
         Present when the result code in the response is QMI_RESULT_SUCCESS.
    */
}qdssc_get_swevt_tag_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_EVENT_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_EVENT_STATE_DISABLED_V01 = 0, /**<  Disabled \n  */
  QDSSC_EVENT_STATE_ENABLED_V01 = 1, /**<  Enabled  */
  QDSSC_EVENT_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_event_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the state of software tracing for a software event on the processor. */
typedef struct {

  /* Mandatory */
  /*  Software Event ID */
  uint32_t event_id;
  /**<   Value identifying a software trace event. */
}qdssc_get_swevt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the state of software tracing for a software event on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Software Event State */
  uint8_t state_valid;  /**< Must be set to true if state is being passed */
  qdssc_event_state_enum_type_v01 state;
  /**<   State of software tracing for the event.
 Values: \n
      - QDSSC_EVENT_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_EVENT_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_get_swevt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables tracing of a software event on the processor. */
typedef struct {

  /* Mandatory */
  /*  Software Event ID */
  uint32_t event_id;
  /**<   Value identifying a software trace event. */

  /* Mandatory */
  /*  Software Event State */
  qdssc_event_state_enum_type_v01 state;
  /**<   State to configure tracing of the software event.
 Values: \n
      - QDSSC_EVENT_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_EVENT_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_set_swevt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables tracing of a software event on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_swevt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_RPM_SWE_LOG_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_RPM_SWE_LOG_RAM_V01 = 0, /**<  Log to RAM (memory) \n  */
  QDSSC_RPM_SWE_LOG_QDSS_V01 = 1, /**<  Log to QDSS  */
  QDSSC_RPM_SWE_LOG_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_rpm_swe_log_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Configures software events on the Resource Power Manager (RPM) processor. */
typedef struct {

  /* Mandatory */
  /*  RPM Software Event Log */
  qdssc_rpm_swe_log_enum_type_v01 log;
  /**<   Location where the RPM's software events are logged.
 Values: \n
      - QDSSC_RPM_SWE_LOG_RAM (0) --  Log to RAM (memory) \n 
      - QDSSC_RPM_SWE_LOG_QDSS (1) --  Log to QDSS 
 */

  /* Mandatory */
  /*  RPM Software Event Group */
  uint8_t group;
  /**<   Group of software events to be configured. Valid values: 0 to 15. */

  /* Mandatory */
  /*  RPM Software Events Bitmask */
  uint64_t bitmask;
  /**<    Bitmask of software events within the group to be disabled (bit value 0)
          or enabled (bit value 1). The bit position corresponds to event within
          the group. For example: bit position 0 (LSB) corresponds to
          event 0 in group N, where "group" value = N.
    */
}qdssc_set_rpm_swe_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Configures software events on the Resource Power Manager (RPM) processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_rpm_swe_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Configures a device for a hardware event. */
typedef struct {

  /* Mandatory */
  /*  Register Address */
  uint32_t regaddr;
  /**<   Address of the hardware event register to be set. */

  /* Mandatory */
  /*  Register Value */
  uint32_t value;
  /**<   Configuration value to which to set the register. */
}qdssc_set_hwe_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Configures a device for a hardware event. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_hwe_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_enums
    @{
  */
typedef enum {
  QDSSC_ETM_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QDSSC_ETM_STATE_DISABLED_V01 = 0, /**<  Disabled \n  */
  QDSSC_ETM_STATE_ENABLED_V01 = 1, /**<  Enabled  */
  QDSSC_ETM_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qdssc_etm_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the state of the Embedded Trace Macrocell's (ETM) trace for the
           processor. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qdssc_get_etm_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the state of the Embedded Trace Macrocell's (ETM) trace for the
           processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  ETM Output State */
  uint8_t state_valid;  /**< Must be set to true if state is being passed */
  qdssc_etm_state_enum_type_v01 state;
  /**<   Present when the result code is QMI_RESULT_SUCCESS.
 Values: \n
      - QDSSC_ETM_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ETM_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_get_etm_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables ETM's trace on the processor. */
typedef struct {

  /* Mandatory */
  /*  ETM Output State */
  qdssc_etm_state_enum_type_v01 state;
  /**<   State to configure the ETM's output.
 Values: \n
      - QDSSC_ETM_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ETM_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_set_etm_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables ETM's trace on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_etm_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Enables or disables ETM's trace on the RPM processor. */
typedef struct {

  /* Mandatory */
  /*  RPM ETM Output State */
  qdssc_etm_state_enum_type_v01 state;
  /**<   State to configure the RPM's ETM output.
 Values: \n
      - QDSSC_ETM_STATE_DISABLED (0) --  Disabled \n 
      - QDSSC_ETM_STATE_ENABLED (1) --  Enabled 
 */
}qdssc_set_rpm_etm_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Enables or disables ETM's trace on the RPM processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_rpm_etm_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Gets the configuration of the ETM for the processor. */
typedef struct {

  /* Mandatory */
  /*  ETM Query String */
  char query_str[QDSSC_ETM_CONFIG_MAX_LEN_V01 + 1];
  /**<   Query for the configuration of the ETM associated with the 
    processor. The string format is processor-dependent and is documented 
    in \hyperref[80-NV553-1]{80-NV553-1}. It is typically passed 
    through the associated Debug Agent instruction.
    */
}qdssc_get_etm_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Gets the configuration of the ETM for the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  ETM Settings String */
  uint8_t settings_str_valid;  /**< Must be set to true if settings_str is being passed */
  char settings_str[QDSSC_ETM_CONFIG_MAX_LEN_V01 + 1];
  /**<   Configuration setting of the ETM associated with the processor.
        This setting is valid when the result code is QMI_RESULT_SUCCESS. 
        The format is processor-dependent and documented in
        \hyperref[80-NV553-1]{80-NV553-1}.
    */
}qdssc_get_etm_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Request Message; Sets the configuration for the ETM on the processor. */
typedef struct {

  /* Mandatory */
  /*  ETM Configuration Command String */
  char command_str[QDSSC_ETM_CONFIG_MAX_LEN_V01 + 1];
  /**<   Command string for configuring the ETM associated with the 
    processor. The string format is processor-dependent and documented in
    \hyperref[80-NV553-1]{80-NV553-1}. It is typically passed through 
    the associated Debug Agent instruction.
    */
}qdssc_set_etm_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qdssc_qmi_messages
    @{
  */
/** Response Message; Sets the configuration for the ETM on the processor. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qdssc_set_etm_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_QDSSC_GET_ENTITY_V01 
//#define REMOVE_QMI_QDSSC_GET_ENTITY_ALL_V01 
//#define REMOVE_QMI_QDSSC_GET_ETM_V01 
//#define REMOVE_QMI_QDSSC_GET_ETM_CONFIG_V01 
//#define REMOVE_QMI_QDSSC_GET_SWEVT_V01 
//#define REMOVE_QMI_QDSSC_GET_SWEVT_TAG_V01 
//#define REMOVE_QMI_QDSSC_GET_SWT_V01 
//#define REMOVE_QMI_QDSSC_SET_ENTITY_V01 
//#define REMOVE_QMI_QDSSC_SET_ENTITY_ALL_V01 
//#define REMOVE_QMI_QDSSC_SET_ETM_V01 
//#define REMOVE_QMI_QDSSC_SET_ETM_CONFIG_V01 
//#define REMOVE_QMI_QDSSC_SET_HWE_REG_V01 
//#define REMOVE_QMI_QDSSC_SET_RPM_ETM_V01 
//#define REMOVE_QMI_QDSSC_SET_RPM_SWE_V01 
//#define REMOVE_QMI_QDSSC_SET_SWEVT_V01 
//#define REMOVE_QMI_QDSSC_SET_SWT_V01 

/*Service Message Definition*/
/** @addtogroup qdssc_qmi_msg_ids
    @{
  */
#define QMI_QDSSC_GET_SWT_REQ_V01 0x0020
#define QMI_QDSSC_GET_SWT_RESP_V01 0x0020
#define QMI_QDSSC_SET_SWT_REQ_V01 0x0021
#define QMI_QDSSC_SET_SWT_RESP_V01 0x0021
#define QMI_QDSSC_GET_ENTITY_REQ_V01 0x0022
#define QMI_QDSSC_GET_ENTITY_RESP_V01 0x0022
#define QMI_QDSSC_SET_ENTITY_REQ_V01 0x0023
#define QMI_QDSSC_SET_ENTITY_RESP_V01 0x0023
#define QMI_QDSSC_GET_ENTITY_ALL_REQ_V01 0x0024
#define QMI_QDSSC_GET_ENTITY_ALL_RESP_V01 0x0024
#define QMI_QDSSC_SET_ENTITY_ALL_REQ_V01 0x0025
#define QMI_QDSSC_SET_ENTITY_ALL_RESP_V01 0x0025
#define QMI_QDSSC_GET_SWEVT_TAG_REQ_V01 0x0026
#define QMI_QDSSC_GET_SWEVT_TAG_RESP_V01 0x0026
#define QMI_QDSSC_GET_SWEVT_REQ_V01 0x0027
#define QMI_QDSSC_GET_SWEVT_RESP_V01 0x0027
#define QMI_QDSSC_SET_SWEVT_REQ_V01 0x0028
#define QMI_QDSSC_SET_SWEVT_RESP_V01 0x0028
#define QMI_QDSSC_SET_RPM_SWE_REQ_V01 0x0029
#define QMI_QDSSC_SET_RPM_SWE_RESP_V01 0x0029
#define QMI_QDSSC_SET_HWE_REG_REQ_V01 0x002A
#define QMI_QDSSC_SET_HWE_REG_RESP_V01 0x002A
#define QMI_QDSSC_GET_ETM_REQ_V01 0x002B
#define QMI_QDSSC_GET_ETM_RESP_V01 0x002B
#define QMI_QDSSC_SET_ETM_REQ_V01 0x002C
#define QMI_QDSSC_SET_ETM_RESP_V01 0x002C
#define QMI_QDSSC_SET_RPM_ETM_REQ_V01 0x002D
#define QMI_QDSSC_SET_RPM_ETM_RESP_V01 0x002D
#define QMI_QDSSC_GET_ETM_CONFIG_REQ_V01 0x002E
#define QMI_QDSSC_GET_ETM_CONFIG_RESP_V01 0x002E
#define QMI_QDSSC_SET_ETM_CONFIG_REQ_V01 0x002F
#define QMI_QDSSC_SET_ETM_CONFIG_RESP_V01 0x002F
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qdssc_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qdssc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define qdssc_get_service_object_v01( ) \
          qdssc_get_service_object_internal_v01( \
            QDSSC_V01_IDL_MAJOR_VERS, QDSSC_V01_IDL_MINOR_VERS, \
            QDSSC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

