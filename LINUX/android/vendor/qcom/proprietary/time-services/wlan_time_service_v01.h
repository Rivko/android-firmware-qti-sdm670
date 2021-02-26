#ifndef WLAN_TIME_SERVICE_01_H
#define WLAN_TIME_SERVICE_01_H
/**
  @file wlan_time_service_v01.h

  @brief This is the public header file which defines the wlan_time service Data structures.

  This header file defines the types and structures that were defined in
  wlan_time. It contains the constant values defined, enums, structures,
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
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed Oct 12 2016 (Spin 0)
   From IDL File: wlan_time_service_v01.idl */

/** @defgroup wlan_time_qmi_consts Constant values defined in the IDL */
/** @defgroup wlan_time_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup wlan_time_qmi_enums Enumerated types used in QMI messages */
/** @defgroup wlan_time_qmi_messages Structures sent as QMI messages */
/** @defgroup wlan_time_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup wlan_time_qmi_accessor Accessor for QMI service object */
/** @defgroup wlan_time_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wlan_time_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define WLAN_TIME_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define WLAN_TIME_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define WLAN_TIME_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define WLAN_TIME_V01_MAX_MESSAGE_ID 0x0026
/**
    @}
  */


/** @addtogroup wlan_time_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_enums
    @{
  */
typedef enum {
  WLAN_TIME_SERVICE_BASE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WLAN_SERVICE_BASE_RTC_V01 = 0, /**<  RTC base for providing time services */
  WLAN_SERVICE_BASE_USER_V01 = 2, /**<  USER base(having Apps Processor time)
                                    for providing time services */
  WLAN_SERVICE_BASE_WLAN_V01 = 16, /**<  WLAN base for providing time services */
  WLAN_TIME_SERVICE_BASE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlan_time_service_base_enum_v01;
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Request Message; client sets the time specified by the base
                 on wlan chip using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base to be set on the WLAN. Values: \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */

  /* Mandatory */
  /*  full time Value */
  uint64_t time;

  /* Mandatory */
  /*  Processor Timetick */
  uint64_t timetick;

  /* Mandatory */
  /* timetick value if required, can be used by anyone.
                                 either by client or by server
 Reserved for future Use */
  uint64_t reserved;
}apps_set_time_on_wlan_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Response Message; client sets the time specified by the base
                 on wlan chip using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}apps_set_time_on_wlan_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Request Message; client gets the time specified by the base
                 from wlan chip using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base to be set on the WLAN. Values: \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */
}apps_get_time_from_wlan_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Response Message; client gets the time specified by the base
                 from wlan chip using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base to be set on the WLAN. Values: \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */

  /* Mandatory */
  /*  full time Value */
  uint64_t time;

  /* Mandatory */
  /*  Processor Timetick */
  uint64_t timetick;

  /* Mandatory */
  /* timetick value if required, can be used by anyone.
                                 either by client or by server
 Reserved for future Use */
  uint64_t reserved;
}apps_get_time_from_wlan_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Request Message; client want to stop getting the time updates for specified base
                 from wlan chip using the TIME service.
				 if TRUE  = WLAN chip stops sending the time.
				 if FALSE = WLAN chip sends sending the time. */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base to be set on the WLAN. Values: \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */

  /* Mandatory */
  /*  Reserved for future Use */
  uint8_t update;
  /**<   TRUE = stop updations from apps to wlan
       FALSE = start updations from apps to wlan    */
}wlan_stop_updates_to_apps_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Response Message; client want to stop getting the time updates for specified base
                 from wlan chip using the TIME service.
				 if TRUE  = WLAN chip stops sending the time.
				 if FALSE = WLAN chip sends sending the time. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}wlan_stop_updates_to_apps_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Request Message; To calculate the QMI transaction delay between the Apps Processor
                 and the wlan Chip.  */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  int64_t dummy;
  /**<   Will be used to calculate QMI delay in future \n
       @ENUM ()
     */
}qmi_wlan_qmi_delay_cal_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Response Message; To calculate the QMI transaction delay between the Apps Processor
                 and the wlan Chip.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Mandatory */
  /*  Time Service Base */
  int64_t dummy;
  /**<   Will be used to calculate QMI delay in future \n
       @ENUM ()
  */
}qmi_wlan_qmi_delay_cal_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Indication Message; indication to apps processor to stop or start sending the
	             time updates to the wlan chip. */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base for which the indication is to be sent. \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */

  /* Mandatory */
  /*  boolean */
  uint8_t update;
  /**<   TRUE = stop updations from apps to wlan
       FALSE = start updations from apps to wlan  */
}qmi_apps_stop_update_to_wlan_message_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Indication Message; Indication to the Apps before setting the time
	             on apps. */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base for which the indication is to be sent. \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */
}qmi_wlan_set_time_on_apps_message_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlan_time_qmi_messages
    @{
  */
/** Indication Message; Indication to the Apps before setting the time
	             on apps. */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  wlan_time_service_base_enum_v01 base;
  /**<   Specifies the time base for which the indication is to be sent. \n
      - WLAN_SERVICE_BASE_RTC (0) --  RTC base for providing time services
      - WLAN_SERVICE_BASE_USER (2) --  USER base(having Apps Processor time)
                                    for providing time services
      - WLAN_SERVICE_BASE_WLAN (16) --  WLAN base for providing time services
 */
}qmi_wlan_get_time_from_apps_message_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_APPS_GET_TIME_FROM_WLAN_BASE_V01
//#define REMOVE_QMI_APPS_SET_TIME_ON_WLAN_BASE_V01
//#define REMOVE_QMI_WLAN_GET_SUPPORTED_FIELDS_V01
//#define REMOVE_QMI_WLAN_GET_SUPPORTED_MSGS_V01
//#define REMOVE_QMI_WLAN_GET_TIME_FROM_APPS_MESSAGE_V01
//#define REMOVE_QMI_WLAN_QMI_DELAY_CAL_V01
//#define REMOVE_QMI_WLAN_STOP_UPDATES_TO_APPS_V01

/*Service Message Definition*/
/** @addtogroup wlan_time_qmi_msg_ids
    @{
  */
#define QMI_WLAN_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_WLAN_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_WLAN_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_WLAN_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_APPS_SET_TIME_ON_WLAN_BASE_REQ_V01 0x0020
#define QMI_APPS_SET_TIME_ON_WLAN_BASE_RESP_V01 0x0020
#define QMI_APPS_GET_TIME_FROM_WLAN_BASE_REQ_V01 0x0021
#define QMI_APPS_GET_TIME_FROM_WLAN_BASE_RESP_V01 0x0021
#define QMI_WLAN_STOP_UPDATES_TO_APPS_REQ_V01 0x0022
#define QMI_WLAN_STOP_UPDATES_TO_APPS_RESP_V01 0x0022
#define QMI_WLAN_QMI_DELAY_CAL_REQ_V01 0x0023
#define QMI_WLAN_QMI_DELAY_CAL_RESP_V01 0x0023
#define QMI_APPS_STOP_UPDATES_TO_WLAN_IND_V01 0x0024
#define QMI_WLAN_SET_TIME_ON_APPS_IND_V01 0x0025
#define QMI_WLAN_GET_TIME_FROM_APPS_IND_V01 0x0026
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro wlan_time_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type wlan_time_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define wlan_time_get_service_object_v01( ) \
          wlan_time_get_service_object_internal_v01( \
            WLAN_TIME_V01_IDL_MAJOR_VERS, WLAN_TIME_V01_IDL_MINOR_VERS, \
            WLAN_TIME_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

