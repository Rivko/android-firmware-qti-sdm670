#ifndef AT_SERVICE_01_H
#define AT_SERVICE_01_H
/**
  @file access_terminal_service_v01.h

  @brief This is the public header file which defines the at service Data structures.

  This header file defines the types and structures that were defined in
  at. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Nov 24 2016 (Spin 0)
   From IDL File: access_terminal_service_v01.idl */

/** @defgroup at_qmi_consts Constant values defined in the IDL */
/** @defgroup at_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup at_qmi_enums Enumerated types used in QMI messages */
/** @defgroup at_qmi_messages Structures sent as QMI messages */
/** @defgroup at_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup at_qmi_accessor Accessor for QMI service object */
/** @defgroup at_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup at_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define AT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define AT_V01_IDL_MINOR_VERS 0x04
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define AT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define AT_V01_MAX_MESSAGE_ID 0x00FF
/**
    @}
  */


/** @addtogroup at_qmi_consts
    @{
  */
#define QMI_AT_DUMMY_BUFFER_LEN_MAX_V01 4096
#define QMI_AT_NUM_CMD_MAX_V01 20
#define QMI_AT_CMD_NAME_MAX_V01 20
#define QMI_AT_URC_LEN_MAX_V01 64
#define QMI_AT_NUM_TOKEN_MAX_V01 100
#define QMI_AT_CMD_RESP_LEN_MAX_V01 100
/**
    @}
  */

/** @addtogroup at_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t abort_flag;
  /**<   Abort flag
      0 : Command cannot be aborted
      1 : Command can be aborted
  */

  char at_cmd_name[QMI_AT_CMD_NAME_MAX_V01 + 1];
  /**<   AT Command Name
  */
}at_reg_at_cmd_type_v01;  /* Type */
/**
    @}
  */

typedef uint32_t at_opcode_mask_v01;
#define QMI_AT_MASK_NA_V01 ((at_opcode_mask_v01)0x01) /**<  Denotes presence of Name field  */
#define QMI_AT_MASK_EQ_V01 ((at_opcode_mask_v01)0x02) /**<  Denotes presence of equal (=) operator  */
#define QMI_AT_MASK_QU_V01 ((at_opcode_mask_v01)0x04) /**<  Denotes presence of question mark (?)  */
#define QMI_AT_MASK_AR_V01 ((at_opcode_mask_v01)0x08) /**<  Denotes presence of trailing argument operator */
/** @addtogroup at_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t at_handle;
  /**<   AT command handle */

  at_opcode_mask_v01 op_code;
  /**<   Op Code   */

  char at_cmd_name[QMI_AT_CMD_NAME_MAX_V01 + 1];
  /**<   AT Command Name */
}at_fwd_at_cmd_ind_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup at_qmi_enums
    @{
  */
typedef enum {
  AT_FWD_RESP_RESULT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  AT_RESULT_ERROR_V01 = 0x00, /**<  Result ERROR.
       This is to be set in case of ERROR or CME ERROR.
       The response buffer contains the entire details. */
  AT_RESULT_OK_V01 = 0x01, /**<  Result OK. This is to be set if the final response
       must send an OK result code to the terminal.
       The response buffer must not contain an OK.  */
  AT_RESULT_OTHER_V01 = 0x02, /**<  Result Other than above two. Set this for all other
       result codes other than the ones mentioned above.  */
  AT_FWD_RESP_RESULT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}at_fwd_resp_result_enum_v01;
/**
    @}
  */

/** @addtogroup at_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t urc_type;

  /*  AT URC type
    0 : URC_START
    URC buffer contains the start of a URC. More messages for the
    same URC could follow.

    1 : URC_END
    URC buffer contains the end of a URC. No more URC messages follow

    2 : URC_INTERMEDIATE
    URC buffer contains an intermediate part of a URC.

    3 : URC_COMPLETE
    URC buffer contains a complete URC. No other URC messages for
    the same URC are expected.
   */
  uint32_t at_urc_len;  /**< Must be set to # of elements in at_urc */
  uint8_t at_urc[QMI_AT_URC_LEN_MAX_V01];
  /**<   AT URC   */
}at_send_at_urc_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup at_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t at_handle;
  /**<   AT command handle
  */

  at_fwd_resp_result_enum_v01 result;
  /**<   Result of AT command
  */

  uint8_t response_type;
  /**<   AT cmd response type
    0 : RESPONSE_START.
    Partial response is present in the response buffer, and this is
    the first buffer of the response. More response buffers are to follow.

    1 : RESPONSE_END
    Response buffer completes the response for the AT command sent.
    There are partial response buffers

    2 : RESPONSE_INTERMEDIATE
    Intermediate response buffer. More response buffers are to follow.

    3 : RESPONSE_COMPLETE
    Response buffer contains a complete response and it must not contain an
    OK response. If only an OK response must be sent as a command response,
    the client can set only the response type as RESPONSE_COMPLETE and keep
    the at_resp response buffer at NULL.
   */

  uint32_t at_resp_len;  /**< Must be set to # of elements in at_resp */
  uint8_t at_resp[QMI_AT_CMD_RESP_LEN_MAX_V01];
  /**<   AT Command Response
   */
}at_fwd_resp_at_cmd_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; Resets the AT state variables of the requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; Resets the AT state variables of the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem.
    (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_req_at_cmd_fwd_req_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem.
    (Deprecated) */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_req_at_cmd_fwd_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This indication is used to forward an AT command to the control
    point.
    (Deprecated) */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_fwd_at_cmd_ind_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to send the response to an
    AT command previously forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  AT Command Response */
  at_fwd_resp_at_cmd_req_type_v01 resp_at_cmd_req;
  /**<   AT Command Response
   */
}at_fwd_resp_at_cmd_req_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to send the response to an
    AT command previously forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_fwd_resp_at_cmd_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This notification is used to notify the control point that a
    particular AT command that was forwarded earlier has been aborted. */
typedef struct {

  /* Mandatory */
  /*  AT Handle */
  uint32_t at_handle;
  /**<   Handle for the AT command to be aborted.
  */
}at_abort_at_cmd_ind_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to send the Unsolicited
    Result Code (URC) to the modem. */
typedef struct {

  /* Mandatory */
  /*  AT URC Request */
  at_send_at_urc_req_type_v01 urc_req;
  /**<   AT URC request type  */
}at_send_at_urc_req_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to send the Unsolicited
    Result Code (URC) to the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_send_at_urc_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This is an internal dummy indication message. Not going to be
    used by external clients. (Deprecated) */
typedef struct {

  /* Optional */
  uint8_t qmi_at_dummy_buffer_valid;  /**< Must be set to true if qmi_at_dummy_buffer is being passed */
  uint32_t qmi_at_dummy_buffer_len;  /**< Must be set to # of elements in qmi_at_dummy_buffer */
  char qmi_at_dummy_buffer[QMI_AT_DUMMY_BUFFER_LEN_MAX_V01];
  /**<   Dummy buffer. Used only to define a large buffer for IDL.
    */
}at_dummy_at_cmd_ind_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  AT commands */
  uint32_t at_cmd_len;  /**< Must be set to # of elements in at_cmd */
  at_reg_at_cmd_type_v01 at_cmd[QMI_AT_NUM_CMD_MAX_V01];
  /**<   List of AT commands.
  */
}at_req_at_cmd_fwd_ex_req_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_req_at_cmd_fwd_ex_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This indication is used to forward an AT command to the control
    point. */
typedef struct {

  /* Mandatory */
  /*  AT command information */
  at_fwd_at_cmd_ind_type_v01 at_cmd_info;
  /**<   AT command information
  */

  /* Optional */
  /*  List of Tokens */
  uint8_t token_valid;  /**< Must be set to true if token is being passed */
  uint32_t token_len;  /**< Must be set to # of elements in token */
  uint8_t token[QMI_AT_NUM_TOKEN_MAX_V01];
  /**<   List of Tokens
    */

  /* Optional */
  /*  V Value Information */
  uint8_t v_val_valid;  /**< Must be set to true if v_val is being passed */
  uint32_t v_val;
  /**<   V value information
  */

  /* Optional */
  /*  Q Value Information */
  uint8_t q_val_valid;  /**< Must be set to true if q_val is being passed */
  uint32_t q_val;
  /**<   Q value information
  */

  /* Optional */
  /*  S3 Value Information */
  uint8_t s3_val_valid;  /**< Must be set to true if s3_val is being passed */
  uint32_t s3_val;
  /**<   S3 value information
    */

  /* Optional */
  /*  S4 Value Information */
  uint8_t s4_val_valid;  /**< Must be set to true if s4_val is being passed */
  uint32_t s4_val;
  /**<   S4 value information
   */

  /* Optional */
  /*  CLIR Value Information */
  uint8_t clir_val_valid;  /**< Must be set to true if clir_val is being passed */
  uint32_t clir_val;
  /**<   CLIR value information
    */

  /* Optional */
  /*  COLP Value Information */
  uint8_t colp_val_valid;  /**< Must be set to true if colp_val is being passed */
  uint32_t colp_val;
  /**<   COLP value information
    */

  /* Optional */
  /*  CMEE Value Information */
  uint8_t cmee_val_valid;  /**< Must be set to true if cmee_val is being passed */
  uint32_t cmee_val;
  /**<   CMEE value information
  */

  /* Optional */
  /*  CCUG Value Information */
  uint8_t ccug_val_valid;  /**< Must be set to true if ccug_val is being passed */
  uint32_t ccug_val[3];
  /**<   CCUG value information
  */

  /* Optional */
  /*  CMEC Value Information */
  uint8_t cmec_val_valid;  /**< Must be set to true if cmec_val is being passed */
  uint32_t cmec_val[4];
  /**<   CMEC value information
    */
}at_fwd_at_cmd_ex_ind_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_AT_ABORT_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_DUMMY_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_FWD_AT_CMD_EX_IND_V01
//#define REMOVE_QMI_AT_FWD_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_FWD_RESP_AT_CMD_V01
//#define REMOVE_QMI_AT_REG_AT_CMD_FWD_V01
//#define REMOVE_QMI_AT_REG_AT_CMD_FWD_EX_V01
//#define REMOVE_QMI_AT_RESET_V01
//#define REMOVE_QMI_AT_SEND_AT_URC_V01

/*Service Message Definition*/
/** @addtogroup at_qmi_msg_ids
    @{
  */
#define QMI_AT_RESET_REQ_V01 0x0000
#define QMI_AT_RESET_RESP_V01 0x0000
#define QMI_AT_REG_AT_CMD_FWD_REQ_V01 0x0020
#define QMI_AT_REG_AT_CMD_FWD_RESP_V01 0x0020
#define QMI_AT_FWD_AT_CMD_IND_V01 0x0021
#define QMI_AT_FWD_RESP_AT_CMD_REQ_V01 0x0022
#define QMI_AT_FWD_RESP_AT_CMD_RESP_V01 0x0022
#define QMI_AT_ABORT_AT_CMD_IND_V01 0x0023
#define QMI_AT_SEND_AT_URC_REQ_V01 0x0024
#define QMI_AT_SEND_AT_URC_RESP_V01 0x0024
#define QMI_AT_REG_AT_CMD_FWD_EX_REQ_V01 0x0025
#define QMI_AT_REG_AT_CMD_FWD_EX_RESP_V01 0x0025
#define QMI_AT_FWD_AT_CMD_EX_IND_V01 0x0026
#define QMI_AT_DUMMY_AT_CMD_IND_V01 0x00FF
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro at_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type at_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define at_get_service_object_v01( ) \
          at_get_service_object_internal_v01( \
            AT_V01_IDL_MAJOR_VERS, AT_V01_IDL_MINOR_VERS, \
            AT_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

