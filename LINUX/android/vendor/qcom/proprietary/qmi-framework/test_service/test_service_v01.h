#ifndef TEST_SERVICE_01_H
#define TEST_SERVICE_01_H
/**
  @file test_service_v01.h

  @brief This is the public header file which defines the test service Data structures.

  This header file defines the types and structures that were defined in
  test. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/test/api/test_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7 
   It was generated on: Sat Feb 22 2014 (Spin 3)
   From IDL File: test_service_v01.idl */

/** @defgroup test_qmi_consts Constant values defined in the IDL */
/** @defgroup test_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup test_qmi_enums Enumerated types used in QMI messages */
/** @defgroup test_qmi_messages Structures sent as QMI messages */
/** @defgroup test_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup test_qmi_accessor Accessor for QMI service object */
/** @defgroup test_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup test_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define TEST_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define TEST_V01_IDL_MINOR_VERS 0x07
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define TEST_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define TEST_V01_MAX_MESSAGE_ID 0x0028
/**
    @}
  */


/** @addtogroup test_qmi_consts 
    @{ 
  */

/**  Maximum size for a data TLV */
#define TEST_MED_DATA_SIZE_V01 8192

/**  Maximum size of a large data TLV, below a full 64k to accommodate 
 response, client_name, and service_name TLVs. */
#define TEST_LARGE_MAX_DATA_SIZE_V01 65000

/**  Maximum length of a client or service name */
#define TEST_MAX_NAME_SIZE_V01 255

/**  Maximum number of failed tests on a single link */
#define TEST_MAX_FAILED_LINKS_V01 32

/**  Maximum size of the throughput array */
#define TEST_MAX_THROUGHPUT_SIZE_V01 15

/**  Maximum number of tests to run */
#define TEST_MAX_LOOPBACK_TESTS_V01 24
/**
    @}
  */

/** @addtogroup test_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t name_len;  /**< Must be set to # of elements in name */
  char name[TEST_MAX_NAME_SIZE_V01];
  /**<   In a request, the optional name to identify a client. 
         In a response or an indication, the optional name to identify a 
         service. */
}test_name_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Tests a basic message passing between the client and the service. */
typedef struct {

  /* Mandatory */
  /*  Ping */
  char ping[4];
  /**<   Simple ping request. */

  /* Optional */
  /*  Optional Client Name */
  uint8_t client_name_valid;  /**< Must be set to true if client_name is being passed */
  test_name_type_v01 client_name;
}test_ping_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Tests a basic message passing between the client and the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Pong */
  uint8_t pong_valid;  /**< Must be set to true if pong is being passed */
  char pong[4];
  /**<   Simple pong response. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_ping_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Indication Message; Tests a basic message passing between the client and the service. */
typedef struct {

  /* Mandatory */
  /*  Hello Indication */
  char indication[5];
  /**<   Simple hello indication. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Tests variably sized messages. */
typedef struct {

  /* Mandatory */
  /*  Ping Data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[TEST_MED_DATA_SIZE_V01];
  /**<   Variably sized data request. */

  /* Optional */
  /*  Optional Client Name */
  uint8_t client_name_valid;  /**< Must be set to true if client_name is being passed */
  test_name_type_v01 client_name;
}test_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Tests variably sized messages. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Response Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[TEST_MED_DATA_SIZE_V01];
  /**<   Variably sized data response. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Tests large variably sized messages. */
typedef struct {

  /* Mandatory */
  /*  Ping Data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[TEST_LARGE_MAX_DATA_SIZE_V01];
  /**<   Variably sized data request. */

  /* Optional */
  /*  Optional Client Name */
  uint8_t client_name_valid;  /**< Must be set to true if client_name is being passed */
  test_name_type_v01 client_name;
}test_large_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Tests large variably sized messages. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Response Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[TEST_LARGE_MAX_DATA_SIZE_V01];
  /**<   Variably sized data response. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_large_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Registers for variably sized indications. */
typedef struct {

  /* Optional */
  /*  Number of Indications */
  uint8_t num_inds_valid;  /**< Must be set to true if num_inds is being passed */
  uint16_t num_inds;
  /**<   Number of indications to send. */

  /* Optional */
  /*  Size of Indications */
  uint8_t ind_size_valid;  /**< Must be set to true if ind_size is being passed */
  uint16_t ind_size;
  /**<   Maximum value is TEST_MED_DATA_SIZE. */

  /* Optional */
  /*  Delay Between Indications */
  uint8_t ms_delay_valid;  /**< Must be set to true if ms_delay is being passed */
  uint16_t ms_delay;
  /**<   Delay (in milliseconds) for the service to wait between sending indications. */

  /* Optional */
  /*  Indication Delayed Start */
  uint8_t num_reqs_delay_valid;  /**< Must be set to true if num_reqs_delay is being passed */
  uint16_t num_reqs_delay;
  /**<  
      Tells the service to wait until ind_num_reqs requests have been received 
      before sending any indications.
  */

  /* Optional */
  /*  Optional Client Name */
  uint8_t client_name_valid;  /**< Must be set to true if client_name is being passed */
  test_name_type_v01 client_name;
}test_data_ind_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Registers for variably sized indications. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_data_ind_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Indication Message; Tests sending variably sized indications. */
typedef struct {

  /* Mandatory */
  /*  Indication Data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[TEST_MED_DATA_SIZE_V01];
  /**<   Variably sized data indication. */

  /* Optional */
  /*  Optional Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;

  /* Optional */
  /*  Optional Checksum Value */
  uint8_t sum_valid;  /**< Must be set to true if sum is being passed */
  uint32_t sum;
  /**<   Optional checksum value to validate the data. */
}test_data_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Queries the service for its name. */
typedef struct {

  /* Optional */
  /*  Optional Client Name */
  uint8_t client_name_valid;  /**< Must be set to true if client_name is being passed */
  test_name_type_v01 client_name;
}test_get_service_name_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Queries the service for its name. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /* Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_get_service_name_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}test_null_req_msg_v01;

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Sends an empty request message. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /* Service Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
}test_null_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}test_null_ind_msg_v01;

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}test_reset_loopback_req_msg_v01;

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Resets all loopback services in the system. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}test_reset_loopback_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint64_t test_loopback_tests_mask_v01;
#define TEST_SYNC_PING_V01 ((test_loopback_tests_mask_v01)0x00000001ull) /**<  Synchronous ping test \n  */
#define TEST_ASYNC_PING_V01 ((test_loopback_tests_mask_v01)0x00000002ull) /**<  Asynchronous ping test \n  */
#define TEST_SYNC_PING_RAW_V01 ((test_loopback_tests_mask_v01)0x00000004ull) /**<  Synchronous raw ping test \n  */
#define TEST_ASYNC_PING_RAW_V01 ((test_loopback_tests_mask_v01)0x00000008ull) /**<  Asynchronous raw ping test \n  */
#define TEST_SYNC_DATA_V01 ((test_loopback_tests_mask_v01)0x00000010ull) /**<  Synchronous data test \n  */
#define TEST_ASYNC_DATA_V01 ((test_loopback_tests_mask_v01)0x00000020ull) /**<  Asynchronous data test \n  */
#define TEST_SYNC_DATA_RAW_V01 ((test_loopback_tests_mask_v01)0x00000040ull) /**<  Synchronous raw data test \n  */
#define TEST_ASYNC_DATA_RAW_V01 ((test_loopback_tests_mask_v01)0x00000080ull) /**<  Asynchronous raw data test \n  */
#define TEST_INDICATIONS_V01 ((test_loopback_tests_mask_v01)0x00000100ull) /**<  Indications test \n  */
#define TEST_DATA_THROUGHPUT_V01 ((test_loopback_tests_mask_v01)0x00000200ull) /**<  Data throughput test  */
/** @addtogroup test_qmi_messages
    @{
  */
/** Request Message; Initiates a loopback test in the system. */
typedef struct {

  /* Mandatory */
  /*  Tests */
  test_loopback_tests_mask_v01 tests;
  /**<   Which tests to run. Values: \n
      - TEST_SYNC_PING (0x00000001) --  Synchronous ping test \n 
      - TEST_ASYNC_PING (0x00000002) --  Asynchronous ping test \n 
      - TEST_SYNC_PING_RAW (0x00000004) --  Synchronous raw ping test \n 
      - TEST_ASYNC_PING_RAW (0x00000008) --  Asynchronous raw ping test \n 
      - TEST_SYNC_DATA (0x00000010) --  Synchronous data test \n 
      - TEST_ASYNC_DATA (0x00000020) --  Asynchronous data test \n 
      - TEST_SYNC_DATA_RAW (0x00000040) --  Synchronous raw data test \n 
      - TEST_ASYNC_DATA_RAW (0x00000080) --  Asynchronous raw data test \n 
      - TEST_INDICATIONS (0x00000100) --  Indications test \n 
      - TEST_DATA_THROUGHPUT (0x00000200) --  Data throughput test 
 */

  /* Optional */
  /*  Maximum Count */
  uint8_t max_count_valid;  /**< Must be set to true if max_count is being passed */
  uint16_t max_count;
  /**<   Maximum number of messages to send in the test. */

  /* Optional */
  /*  Data Size */
  uint8_t data_size_valid;  /**< Must be set to true if data_size is being passed */
  uint16_t data_size;
  /**<   Size of the data in a data or throughput test. */

  /* Optional */
  /*  Maximum Data Size */
  uint8_t max_data_size_valid;  /**< Must be set to true if max_data_size is being passed */
  uint16_t max_data_size;
  /**<   Maximum data size to perform variable data size tests. */

  /* Optional */
  /*  Timeout */
  uint8_t timeout_ms_valid;  /**< Must be set to true if timeout_ms is being passed */
  uint32_t timeout_ms;
  /**<   Timeout value the client is using to wait on all messages. 
       This value is halved each time this request message is 
       propagated, so that all other tests should finish or timeout
       before the original client does. */
}test_loopback_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_enums
    @{
  */
typedef enum {
  TEST_LOOPBACK_LINK_FAILURE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  TEST_LOOPBACK_LINK_DEFAULT_V01 = 0, /**<  Default value for out of range errors \n  */
  TEST_LOOPBACK_LINK_TIMEOUT_V01 = 1, /**<  Link failed because of a timeout \n  */
  TEST_LOOPBACK_LINK_FAILED_INSTANCE_V01 = 2, /**<  Link failed; service instance does not exist \n  */
  TEST_LOOPBACK_LINK_FAILED_CLNT_CREATION_V01 = 3, /**<  Link failed to create a new client \n  */
  TEST_LOOPBACK_LINK_UNKNOWN_ERR_V01 = 4, /**<  Link failed for unknown reasons \n  */
  TEST_LOOPBACK_LINK_FRAMEWORK_ERR_V01 = 5, /**<  Link failed because of the framework \n  */
  TEST_LOOPBACK_LINK_SERVICE_ERR_V01 = 6, /**<  Link failed on the service side  */
  TEST_LOOPBACK_LINK_FAILURE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}test_loopback_link_failure_enum_v01;
/**
    @}
  */

/** @addtogroup test_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t source;
  /**<   Source node of the test. */

  uint16_t dest;
  /**<   Destination node of the test. */
}test_loopback_instance_tuple_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup test_qmi_aggregates
    @{
  */
typedef struct {

  test_loopback_instance_tuple_type_v01 link;
  /**<   Information about the link on which the test takes. */

  test_loopback_link_failure_enum_v01 failure;
  /**<   Information about the failure on the link. Values: \n
      - TEST_LOOPBACK_LINK_DEFAULT (0) --  Default value for out of range errors \n 
      - TEST_LOOPBACK_LINK_TIMEOUT (1) --  Link failed because of a timeout \n 
      - TEST_LOOPBACK_LINK_FAILED_INSTANCE (2) --  Link failed; service instance does not exist \n 
      - TEST_LOOPBACK_LINK_FAILED_CLNT_CREATION (3) --  Link failed to create a new client \n 
      - TEST_LOOPBACK_LINK_UNKNOWN_ERR (4) --  Link failed for unknown reasons \n 
      - TEST_LOOPBACK_LINK_FRAMEWORK_ERR (5) --  Link failed because of the framework \n 
      - TEST_LOOPBACK_LINK_SERVICE_ERR (6) --  Link failed on the service side 
 */
}test_loopback_link_failure_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Response Message; Initiates a loopback test in the system. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Services */
  uint8_t num_services_valid;  /**< Must be set to true if num_services is being passed */
  uint8_t num_services;
  /**<   Number of services from which to expect a results indication. */

  /* Optional */
  /*  Links Failed */
  uint8_t links_failed_valid;  /**< Must be set to true if links_failed is being passed */
  uint32_t links_failed_len;  /**< Must be set to # of elements in links_failed */
  test_loopback_link_failure_type_v01 links_failed[TEST_MAX_FAILED_LINKS_V01];
  /**<   \n
       Information about which links in the system have failed (if any). */
}test_loopback_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup test_qmi_enums
    @{
  */
typedef enum {
  TEST_LOOPBACK_TEST_RESULT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  TEST_LOOPBACK_DEFAULT_V01 = 0, /**<  Default value for out of range errors \n  */
  TEST_LOOPBACK_SUCCESS_V01 = 1, /**<  Successful test \n  */
  TEST_LOOPBACK_FAILURE_V01 = 2, /**<  Failed test \n  */
  TEST_LOOPBACK_UNSUPPORTED_V01 = 3, /**<  Test is not supported \n  */
  TEST_LOOPBACK_TEST_TIME_OUT_V01 = 4, /**<  Test timed out \n  */
  TEST_LOOPBACK_CHECKSUM_FAIL_V01 = 5, /**<  Checksum on the data failed \n  */
  TEST_LOOPBACK_FRAMEWORK_ERROR_V01 = 6, /**<  Error in the framework during the test \n  */
  TEST_LOOPBACK_TEST_IN_PROGRESS_V01 = 7, /**<  Test is already in progress \n  */
  TEST_LOOPBACK_MEMORY_FAIL_V01 = 8, /**<  Memory allocation failure  */
  TEST_LOOPBACK_TEST_RESULT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}test_loopback_test_result_enum_v01;
/**
    @}
  */

/** @addtogroup test_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t data_size;
  /**<   Size of the throughput message. */

  uint16_t max_count;
  /**<   Maximum number of messages to send. */

  uint16_t count;
  /**<   Number of messages sent. */

  uint32_t us_duration;
  /**<   Time (in milliseconds) the throughput test took. */
}test_loopback_throughput_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup test_qmi_aggregates
    @{
  */
typedef struct {

  test_loopback_tests_mask_v01 test;
  /**<   Which test was run. Values: \n
      - TEST_SYNC_PING (0x00000001) --  Synchronous ping test \n 
      - TEST_ASYNC_PING (0x00000002) --  Asynchronous ping test \n 
      - TEST_SYNC_PING_RAW (0x00000004) --  Synchronous raw ping test \n 
      - TEST_ASYNC_PING_RAW (0x00000008) --  Asynchronous raw ping test \n 
      - TEST_SYNC_DATA (0x00000010) --  Synchronous data test \n 
      - TEST_ASYNC_DATA (0x00000020) --  Asynchronous data test \n 
      - TEST_SYNC_DATA_RAW (0x00000040) --  Synchronous raw data test \n 
      - TEST_ASYNC_DATA_RAW (0x00000080) --  Asynchronous raw data test \n 
      - TEST_INDICATIONS (0x00000100) --  Indications test \n 
      - TEST_DATA_THROUGHPUT (0x00000200) --  Data throughput test 
 */

  test_loopback_test_result_enum_v01 result;
  /**<   Test results. Values: \n
      - TEST_LOOPBACK_DEFAULT (0) --  Default value for out of range errors \n 
      - TEST_LOOPBACK_SUCCESS (1) --  Successful test \n 
      - TEST_LOOPBACK_FAILURE (2) --  Failed test \n 
      - TEST_LOOPBACK_UNSUPPORTED (3) --  Test is not supported \n 
      - TEST_LOOPBACK_TEST_TIME_OUT (4) --  Test timed out \n 
      - TEST_LOOPBACK_CHECKSUM_FAIL (5) --  Checksum on the data failed \n 
      - TEST_LOOPBACK_FRAMEWORK_ERROR (6) --  Error in the framework during the test \n 
      - TEST_LOOPBACK_TEST_IN_PROGRESS (7) --  Test is already in progress \n 
      - TEST_LOOPBACK_MEMORY_FAIL (8) --  Memory allocation failure 
 */

  uint32_t throughput_len;  /**< Must be set to # of elements in throughput */
  test_loopback_throughput_type_v01 throughput[TEST_MAX_THROUGHPUT_SIZE_V01];
  /**<   Throughput information. */
}test_loopback_test_results_v01;  /* Type */
/**
    @}
  */

/** @addtogroup test_qmi_messages
    @{
  */
/** Indication Message; Contains the results of the loopback test. */
typedef struct {

  /* Mandatory */
  /*  Results */
  uint32_t results_len;  /**< Must be set to # of elements in results */
  test_loopback_test_results_v01 results[TEST_MAX_LOOPBACK_TESTS_V01];
  /**<   \n
       (results returned from the other test services) */

  /* Optional */
  /*  Name */
  uint8_t service_name_valid;  /**< Must be set to true if service_name is being passed */
  test_name_type_v01 service_name;
  /**<   \n
       (name of the service on which the test was performed) */

  /* Optional */
  /*  Link */
  uint8_t link_valid;  /**< Must be set to true if link is being passed */
  test_loopback_instance_tuple_type_v01 link;
}test_loopback_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup test_qmi_msg_ids
    @{
  */
#define QMI_TEST_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_TEST_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_TEST_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_TEST_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_TEST_REQ_V01 0x0020
#define QMI_TEST_RESP_V01 0x0020
#define QMI_TEST_IND_V01 0x0020
#define QMI_TEST_DATA_REQ_V01 0x0021
#define QMI_TEST_DATA_RESP_V01 0x0021
#define QMI_TEST_LARGE_DATA_REQ_V01 0x0022
#define QMI_TEST_LARGE_DATA_RESP_V01 0x0022
#define QMI_TEST_DATA_IND_REG_REQ_V01 0x0023
#define QMI_TEST_DATA_IND_REG_RESP_V01 0x0023
#define QMI_TEST_DATA_IND_V01 0x0024
#define QMI_TEST_GET_SERVICE_NAME_REQ_V01 0x0025
#define QMI_TEST_GET_SERVICE_NAME_RESP_V01 0x0025
#define QMI_TEST_NULL_REQ_V01 0x0026
#define QMI_TEST_NULL_RESP_V01 0x0026
#define QMI_TEST_NULL_IND_V01 0x0026
#define QMI_TEST_RESET_LOOPBACK_REQ_V01 0x0027
#define QMI_TEST_RESET_LOOPBACK_RESP_V01 0x0027
#define QMI_TEST_LOOPBACK_REQ_V01 0x0028
#define QMI_TEST_LOOPBACK_RESP_V01 0x0028
#define QMI_TEST_LOOPBACK_IND_V01 0x0028
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro test_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type test_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define test_get_service_object_v01( ) \
          test_get_service_object_internal_v01( \
            TEST_V01_IDL_MAJOR_VERS, TEST_V01_IDL_MINOR_VERS, \
            TEST_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

