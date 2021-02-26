#ifndef SNS_CLIENT_SVC_SERVICE_01_H
#define SNS_CLIENT_SVC_SERVICE_01_H
/**
  @file sns_client_api_v01.h

  @brief This is the public header file which defines the SNS_CLIENT_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_CLIENT_SVC. It contains the constant values defined, enums, structures,
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
  
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Aug 24 2017 (Spin 0)
   From IDL File: sns_client_api_v01.idl */

/** @defgroup SNS_CLIENT_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_CLIENT_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_CLIENT_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_CLIENT_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_CLIENT_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_CLIENT_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_CLIENT_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_CLIENT_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_CLIENT_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_CLIENT_SVC_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_CLIENT_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_CLIENT_SVC_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup SNS_CLIENT_SVC_qmi_consts
    @{
  */

/**   QMI Service ID for this Sensors Service  */
#define SNS_CLIENT_SVC_ID_V01 400

/**   Maximum size of a request message  */
#define SNS_CLIENT_REQ_LEN_MAX_V01 1000

/**   Maximum size of a report indication message  */
#define SNS_CLIENT_REPORT_LEN_MAX_V01 1000

/**   Maximum size of a jumbo report indication message  */
#define SNS_CLIENT_JUMBO_REPORT_LEN_MAX_V01 62000
/**
    @}
  */

/** @addtogroup SNS_CLIENT_SVC_qmi_messages
    @{
  */
/** Request Message; Send a query to the Sensors subsystem.  An immediate response will
    always be sent, indicating the request receipt. */
typedef struct {

  /* Mandatory */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[SNS_CLIENT_REQ_LEN_MAX_V01];
  /**<   Protocol Buffer encoded request message buffer; see sns_client.proto
  for a list of available request messages. */

  /* Optional */
  uint8_t use_jumbo_report_valid;  /**< Must be set to true if use_jumbo_report is being passed */
  uint8_t use_jumbo_report;
  /**<   By default clients will receive all single and batched events within
  sns_client_report_ind_msg.  Single events larger than SNS_CLIENT_REPORT_LEN_MAX
  will be dropped and not sent; batches of events will be split to fit.  This
  option enables support of sns_client_jumbo_report_ind_msg; events may be
  sent within either indication message.  The payload size is only guaranteed
  to be less than SNS_CLIENT_JUMBO_REPORT_LEN_MAX. */
}sns_client_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_CLIENT_SVC_qmi_messages
    @{
  */
/** Response Message; Send a query to the Sensors subsystem.  An immediate response will
    always be sent, indicating the request receipt. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;

  /* Optional */
  uint8_t client_id_valid;  /**< Must be set to true if client_id is being passed */
  uint64_t client_id;
  /**<   Client ID identifies the client connection.  Generated by service, and
  returned in all sns_client_report_ind_msg.  Will be the same value in all
  sns_client_resp_msg/sns_client_report_ind_msg for this client connection. */

  /* Optional */
  uint8_t result_valid;  /**< Must be set to true if result is being passed */
  uint32_t result;
  /**<   Result status of the request; see sns_pb_error_type */
}sns_client_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_CLIENT_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing one or more Sensor-generated events */
typedef struct {

  /* Mandatory */
  uint64_t client_id;
  /**<   Client ID identifies the client connection.  */

  /* Mandatory */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[SNS_CLIENT_REPORT_LEN_MAX_V01];
  /**<   Protocol Buffer encoded event message buffer; see sns_client.proto for
  for a list of possible event messages/formats. */
}sns_client_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_CLIENT_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing one or more Sensor-generated events */
typedef struct {

  /* Mandatory */
  uint64_t client_id;
  /**<   Client ID identifies the client connection.  */

  /* Mandatory */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[SNS_CLIENT_JUMBO_REPORT_LEN_MAX_V01];
  /**<   Protocol Buffer encoded event message buffer; see sns_client.proto for
  for a list of possible event messages/formats. */
}sns_client_jumbo_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SNS_CLIENT_GET_SUPPORTED_MSGS_V01 
//#define REMOVE_SNS_CLIENT_V01 
//#define REMOVE_SNS_CLIENT_JUMBO_REPORT_V01 
//#define REMOVE_SNS_CLIENT_REPORT_V01 

/*Service Message Definition*/
/** @addtogroup SNS_CLIENT_SVC_qmi_msg_ids
    @{
  */
#define QMI_SNS_CLIENT_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_SNS_CLIENT_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define SNS_CLIENT_REQ_V01 0x0020
#define SNS_CLIENT_RESP_V01 0x0020
#define SNS_CLIENT_REPORT_IND_V01 0x0021
#define SNS_CLIENT_JUMBO_REPORT_IND_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_CLIENT_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_CLIENT_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_CLIENT_SVC_get_service_object_v01( ) \
          SNS_CLIENT_SVC_get_service_object_internal_v01( \
            SNS_CLIENT_SVC_V01_IDL_MAJOR_VERS, SNS_CLIENT_SVC_V01_IDL_MINOR_VERS, \
            SNS_CLIENT_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

