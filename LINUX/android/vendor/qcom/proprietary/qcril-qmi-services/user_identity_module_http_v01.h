#ifndef UIM_HTTP_SERVICE_01_H
#define UIM_HTTP_SERVICE_01_H
/**
  @file user_identity_module_http_v01.h

  @brief This is the public header file which defines the uim_http service Data structures.

  This header file defines the types and structures that were defined in
  uim_http. It contains the constant values defined, enums, structures,
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
   It was generated on: Tue Mar  8 2016 (Spin 0)
   From IDL File: user_identity_module_http_v01.idl */

/** @defgroup uim_http_qmi_consts Constant values defined in the IDL */
/** @defgroup uim_http_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup uim_http_qmi_enums Enumerated types used in QMI messages */
/** @defgroup uim_http_qmi_messages Structures sent as QMI messages */
/** @defgroup uim_http_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup uim_http_qmi_accessor Accessor for QMI service object */
/** @defgroup uim_http_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup uim_http_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define UIM_HTTP_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define UIM_HTTP_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define UIM_HTTP_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define UIM_HTTP_V01_MAX_MESSAGE_ID 0x0021
/**
    @}
  */


/** @addtogroup uim_http_qmi_consts
    @{
  */
#define UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 2048
#define UIM_HTTP_HEADER_NAME_MAX_LEN_V01 32
#define UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 256
#define UIM_HTTP_CUST_HEADER_MAX_COUNT_V01 10
#define UIM_HTTP_URL_MAX_LEN_V01 512
/**
    @}
  */

/** @addtogroup uim_http_qmi_messages
    @{
  */
/** Request Message; Resets the service state variables of the requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}uim_http_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup uim_http_qmi_messages
    @{
  */
/** Response Message; Resets the service state variables of the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}uim_http_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_http_qmi_aggregates
    @{
  */
typedef struct {

  char name[UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1];

  char value[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1];
}uim_http_header_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_http_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t total_size;
  /**<   Total size*/

  uint32_t segment_offset;
  /**<   Offset of the segment*/
}uim_http_segment_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_http_qmi_enums
    @{
  */
typedef enum {
  UIM_HTTP_RESULT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_HTTP_TRANSACTION_SUCCESSFUL_V01 = 0x0, 
  UIM_HTTP_UNKNOWN_ERROR_V01 = 0x1, 
  UIM_HTTP_SERVER_ERROR_V01 = 0x2, /**<  Server error \n  */
  UIM_HTTP_TLS_ERROR_V01 = 0x3, /**<  TLS error    \n  */
  UIM_HTTP_NETWORK_ERROR_V01 = 0x4, /**<  Network error \n  */
  UIM_HTTP_RESULT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_http_result_type_enum_v01;
/**
    @}
  */

/** @addtogroup uim_http_qmi_messages
    @{
  */
/** Request Message; Exchanges the HTTP request/response between modem clients and server */
typedef struct {

  /* Mandatory */
  /*  Result of HTTP Request Transaction */
  uim_http_result_type_enum_v01 result;
  /**<   This TLV contains the result of the HTTP request*/

  /* Mandatory */
  /*  Token ID */
  uint32_t token_id;
  /**<   Identifier for a request and response pair*/

  /* Optional */
  /*  Custom Headers in HTTP Response */
  uint8_t headers_valid;  /**< Must be set to true if headers is being passed */
  uint32_t headers_len;  /**< Must be set to # of elements in headers */
  uim_http_header_type_v01 headers[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];
  /**<   If the response of the HTTP request is chunked,
       the custom header is contained only in the response sent in the first chunk
  */

  /* Optional */
  /*  Segment Information */
  uint8_t segment_info_valid;  /**< Must be set to true if segment_info is being passed */
  uim_http_segment_info_type_v01 segment_info;
  /**<   Total size of the response and offset of the segment in the message.*/

  /* Optional */
  /*  Payload Body */
  uint8_t payload_body_valid;  /**< Must be set to true if payload_body is being passed */
  uint32_t payload_body_len;  /**< Must be set to # of elements in payload_body */
  uint8_t payload_body[UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01];
  /**<   Response returned from the control point.*/
}uim_http_transaction_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_http_qmi_messages
    @{
  */
/** Response Message; Exchanges the HTTP request/response between modem clients and server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}uim_http_transaction_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_http_qmi_aggregates
    @{
  */
typedef struct {

  char content_type[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1];

  uint32_t custom_header_len;  /**< Must be set to # of elements in custom_header */
  uim_http_header_type_v01 custom_header[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];
}uim_http_req_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_http_qmi_messages
    @{
  */
/** Indication Message; Indication to the control point to transmit a HTTP request to the server. */
typedef struct {

  /* Mandatory */
  /*  Token ID */
  uint32_t token_id;
  /**<   Identifier for a request and response pair*/

  /* Optional */
  /*  URL */
  uint8_t url_valid;  /**< Must be set to true if url is being passed */
  char url[UIM_HTTP_URL_MAX_LEN_V01 + 1];
  /**<   URL of the HTTP request to be performed.
       If the payload body of the HTTP request is chunked, 
       the URL is contained only in the indication with the first chunk
  */

  /* Optional */
  /*  Custom Headers */
  uint8_t headers_valid;  /**< Must be set to true if headers is being passed */
  uim_http_req_info_type_v01 headers;
  /**<   Request headers sent to control point.
       If the payload body of the HTTP request is chunked, 
       the custom headers are contained only in the indication with the first chunk
  */

  /* Optional */
  /*  Segment Information */
  uint8_t segment_info_valid;  /**< Must be set to true if segment_info is being passed */
  uim_http_segment_info_type_v01 segment_info;
  /**<   Total size of the payload of the HTTP request and offset of the segment.
       If this TLV is missing, the control point shall assume that the
       indication contains the entire body of the HTTP request.
       This TLV is required in all the indications when the payload is chunked
  */

  /* Optional */
  /*  Payload Body */
  uint8_t payload_body_valid;  /**< Must be set to true if payload_body is being passed */
  uint32_t payload_body_len;  /**< Must be set to # of elements in payload_body */
  uint8_t payload_body[UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01];
  /**<   Payload body of the HTTP request.*/
}uim_http_transaction_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_UIM_HTTP_RESET_V01 
//#define REMOVE_QMI_UIM_HTTP_TRANSACTION_V01 
//#define REMOVE_QMI_UIM_HTTP_TRANSACTION_IND_V01 

/*Service Message Definition*/
/** @addtogroup uim_http_qmi_msg_ids
    @{
  */
#define QMI_UIM_HTTP_RESET_REQ_V01 0x0020
#define QMI_UIM_HTTP_RESET_RESP_V01 0x0020
#define QMI_UIM_HTTP_TRANSACTION_REQ_V01 0x0021
#define QMI_UIM_HTTP_TRANSACTION_RESP_V01 0x0021
#define QMI_UIM_HTTP_TRANSACTION_IND_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro uim_http_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type uim_http_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define uim_http_get_service_object_v01( ) \
          uim_http_get_service_object_internal_v01( \
            UIM_HTTP_V01_IDL_MAJOR_VERS, UIM_HTTP_V01_IDL_MINOR_VERS, \
            UIM_HTTP_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

