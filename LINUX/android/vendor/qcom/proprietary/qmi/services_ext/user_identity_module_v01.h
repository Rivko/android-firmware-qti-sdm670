#ifndef UIM_SERVICE_01_H
#define UIM_SERVICE_01_H
/**
  @file user_identity_module_v01.h
  
  @brief This is the public header file which defines the uim service Data structures.

  This header file defines the types and structures that were defined in 
  uim. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/uim/main/latest/api/user_identity_module_v01.h#34 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5 
   It was generated on: Sat Oct 26 2013 (Spin 0)
   From IDL File: user_identity_module_v01.idl */

/** @defgroup uim_qmi_consts Constant values defined in the IDL */
/** @defgroup uim_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup uim_qmi_enums Enumerated types used in QMI messages */
/** @defgroup uim_qmi_messages Structures sent as QMI messages */
/** @defgroup uim_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup uim_qmi_accessor Accessor for QMI service object */
/** @defgroup uim_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup uim_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define UIM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define UIM_V01_IDL_MINOR_VERS 0x25
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define UIM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define UIM_V01_MAX_MESSAGE_ID 0x004D
/** 
    @} 
  */


/** @addtogroup uim_qmi_consts 
    @{ 
  */
#define QMI_UIM_CARDS_MAX_V01 2
#define QMI_UIM_EXTENDED_CARDS_MAX_V01 5
#define QMI_UIM_APPS_MAX_V01 8
#define QMI_UIM_EXTENDED_APPS_MAX_V01 20
#define QMI_UIM_AID_MAX_V01 32
#define QMI_UIM_LABEL_MAX_V01 255
#define QMI_UIM_PATH_MAX_V01 10
#define QMI_UIM_PIN_MAX_V01 8
#define QMI_UIM_ENCRYPTED_PIN_MAX_V01 255
#define QMI_UIM_CK_MAX_V01 16
#define QMI_UIM_CONTENT_TRANSPARENT_MAX_V01 4096
#define QMI_UIM_CONTENT_RECORD_MAX_V01 255
#define QMI_UIM_FILE_ATTR_RAW_VALUE_MAX_V01 1024
#define QMI_UIM_REFRESH_FILES_MAX_V01 100
#define QMI_UIM_AUTHENTICATE_DATA_MAX_V01 1024
#define QMI_UIM_AUTHENTICATE_RESULT_MAX_V01 1024
#define QMI_UIM_APDU_DATA_MAX_V01 1024
#define QMI_UIM_PERSO_FEATURE_MAX_V01 20
#define QMI_UIM_ATR_DATA_MAX_V01 255
#define QMI_UIM_CARD_READER_DATA_MAX_V01 255
#define QMI_UIM_CONTENT_ADDITIONAL_RECORD_MAX_V01 4096
#define QMI_UIM_SELECT_RESPONSE_MAX_V01 255
#define QMI_UIM_ATR_MAX_V01 33
#define QMI_UIM_ENCRYPTED_IMSI_MAX_V01 255
#define QMI_UIM_PHYSICAL_SLOTS_MAX_V01 5
#define QMI_UIM_ICCID_LEN_MAX_V01 10
#define QMI_UIM_MCC_LEN_V01 3
#define QMI_UIM_MNC_MAX_V01 3
#define QMI_UIM_IRM_CODE_LEN_V01 4
#define QMI_UIM_MSIN_MAX_V01 10
#define QMI_UIM_PERSO_NUM_NW_MAX_V01 85
#define QMI_UIM_PERSO_NUM_NS_MAX_V01 64
#define QMI_UIM_PERSO_NUM_GW_SP_MAX_V01 64
#define QMI_UIM_PERSO_NUM_GW_CP_MAX_V01 51
#define QMI_UIM_PERSO_NUM_SIM_MAX_V01 32
#define QMI_UIM_PERSO_NUM_1X_NW2_MAX_V01 128
#define QMI_UIM_INCREASE_DATA_MAX_V01 127
#define QMI_UIM_SEC_ATTRIBUTE_PIN1_BIT_V01 0
#define QMI_UIM_SEC_ATTRIBUTE_PIN2_BIT_V01 1
#define QMI_UIM_SEC_ATTRIBUTE_UPIN_BIT_V01 2
#define QMI_UIM_SEC_ATTRIBUTE_ADM_BIT_V01 3
#define QMI_UIM_CAP_FDN_BIT_V01 0
#define QMI_UIM_CAP_HIDDEN_PIN_BIT_V01 1
#define QMI_UIM_CAP_EF_DIR_INDEX_BIT_V01 2
#define QMI_UIM_EVENT_CARD_STATUS_BIT_V01 0
#define QMI_UIM_EVENT_SAP_CONNECTION_BIT_V01 1
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}uim_reset_req_msg_v01;

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Resets the issuing control points state kept by the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */
}uim_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SESSION_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SESSION_TYPE_PRIMARY_GW_V01 = 0x00, 
  UIM_SESSION_TYPE_PRIMARY_1X_V01 = 0x01, 
  UIM_SESSION_TYPE_SECONDARY_GW_V01 = 0x02, 
  UIM_SESSION_TYPE_SECONDARY_1X_V01 = 0x03, 
  UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01 = 0x04, 
  UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01 = 0x05, 
  UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01 = 0x06, 
  UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01 = 0x07, 
  UIM_SESSION_TYPE_CHANNEL_ID_SLOT_1_V01 = 0x08, 
  UIM_SESSION_TYPE_CHANNEL_ID_SLOT_2_V01 = 0x09, 
  UIM_SESSION_TYPE_TERTIARY_GW_V01 = 0x0A, 
  UIM_SESSION_TYPE_TERTIARY_1X_V01 = 0x0B, 
  UIM_SESSION_TYPE_QUATERNARY_GW_V01 = 0x0C, 
  UIM_SESSION_TYPE_QUATERNARY_1X_V01 = 0x0D, 
  UIM_SESSION_TYPE_QUINARY_GW_V01 = 0x0E, 
  UIM_SESSION_TYPE_QUINARY_1X_V01 = 0x0F, 
  UIM_SESSION_TYPE_NONPROVISIONING_SLOT_3_V01 = 0x10, 
  UIM_SESSION_TYPE_NONPROVISIONING_SLOT_4_V01 = 0x11, 
  UIM_SESSION_TYPE_NONPROVISIONING_SLOT_5_V01 = 0x12, 
  UIM_SESSION_TYPE_CARD_ON_SLOT_3_V01 = 0x13, 
  UIM_SESSION_TYPE_CARD_ON_SLOT_4_V01 = 0x14, 
  UIM_SESSION_TYPE_CARD_ON_SLOT_5_V01 = 0x15, 
  UIM_SESSION_TYPE_CHANNEL_ID_SLOT_3_V01 = 0x16, 
  UIM_SESSION_TYPE_CHANNEL_ID_SLOT_4_V01 = 0x17, 
  UIM_SESSION_TYPE_CHANNEL_ID_SLOT_5_V01 = 0x18, 
  UIM_SESSION_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_session_type_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_session_type_enum_v01 session_type;
  /**<   Indicates the session type. Valid values: 0 through 24.
       See Table \ref{tbl:sessions} for the list of available session types
       with their values and descriptions.
  */

  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value or channel ID. This value is required
       for nonprovisioning and for logical channel session types. It is
       ignored in all other cases.
  */
}uim_session_information_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t file_id;
  /**<   File ID.*/

  uint32_t path_len;  /**< Must be set to # of elements in path */
  uint8_t path[QMI_UIM_PATH_MAX_V01];
  /**<   File path. This value must be the complete path of the file, which
       is a sequence block of 2 bytes (e.g., 0x3F00 0x7FFF).
  */
}uim_file_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t offset;
  /**<   Offset for the Read operation.*/

  uint16_t length;
  /**<   Length of the content to be read. The value 0 is used to read the
       complete file.
  */
}uim_read_transparent_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Provides read access to any transparent file in the card and
    provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  Read Transparent */
  uim_read_transparent_type_v01 read_transparent;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Encrypt Data */
  uint8_t encryption_valid;  /**< Must be set to true if encryption is being passed */
  uint8_t encryption;
  /**<   Indicates whether the data read from the card is to be encrypted.*/
}uim_read_transparent_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t sw1;
  /**<   SW1 received from the card.*/

  uint8_t sw2;
  /**<   SW2 received from the card.*/
}uim_card_result_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t content_len;  /**< Must be set to # of elements in content */
  uint8_t content[QMI_UIM_CONTENT_TRANSPARENT_MAX_V01];
  /**<   Read content; sequence of bytes as read from the card.*/
}uim_read_transparent_result_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Provides read access to any transparent file in the card and
    provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Read Result */
  uint8_t read_result_valid;  /**< Must be set to true if read_result is being passed */
  uim_read_transparent_result_type_v01 read_result;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Encrypted Data */
  uint8_t encryption_valid;  /**< Must be set to true if encryption is being passed */
  uint8_t encryption;
  /**<   Indicates whether the data from the card passed in read_result is
       encrypted.
  */

  /* Optional */
  /*  Requested Length */
  uint8_t file_length_valid;  /**< Must be set to true if file_length is being passed */
  uint16_t file_length;
  /**<   When this TLV is present, the requested length exceeds the maximum size
       supported by the QMI UIM. QMI_ERR_INSUFFICIENT_RESOURCES is returned to
       the client and this value indicates the total length.
  */

  /* Optional */
  /*  Long Data Token */
  uint8_t long_data_token_valid;  /**< Must be set to true if long_data_token is being passed */
  uint32_t long_data_token;
  /**<   When this TLV is present, the requested length exceeds the maximum size
       supported by QMI UIM. QMI_ERR_INSUFFICIENT_ RESOURCES is returned to
       the client, and the result is provided in subsequent indications.
       Long data token is used to reconstruct the entire read response,
       possibly spanning multiple indications.
  */
}uim_read_transparent_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a read transparent command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Read Result */
  uint8_t read_result_valid;  /**< Must be set to true if read_result is being passed */
  uim_read_transparent_result_type_v01 read_result;

  /* Optional */
  /*  Encrypted Data */
  uint8_t encryption_valid;  /**< Must be set to true if encryption is being passed */
  uint8_t encryption;
  /**<   Indicates whether the data from the card passed in the read_result is
       encrypted.
  */

  /* Optional */
  /*  Requested Length */
  uint8_t file_length_valid;  /**< Must be set to true if file_length is being passed */
  uint16_t file_length;
  /**<   When this TLV is present, the requested length exceeds the maximum size
       supported by the QMI UIM. QMI_ERR_INSUFFICIENT_RESOURCES is returned to
       the client, and this value indicates the total length.
  */

  /* Optional */
  /*  Long Data Token */
  uint8_t long_data_token_valid;  /**< Must be set to true if long_data_token is being passed */
  uint32_t long_data_token;
  /**<   When this TLV is present, the read result exceeds the maximum size
       supported by Read result TLV, and the result is provided in subsequent
       QMI_UIM_READ_TRANSPARENT_ LONG_IND messages. Long data token is used to
       reconstruct the entire read response, possibly spanning multiple
       indications.
  */
}uim_read_transparent_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record;
  /**<   Record number (starting from 1).*/

  uint16_t length;
  /**<   Length of the content to be read. The value 0 is used to read the
       complete record.
  */
}uim_read_record_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Provides read access to a specific record in a linear
           fixed or cyclic file in the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  Read Record */
  uim_read_record_type_v01 read_record;

  /* Optional */
  /*  Last Record */
  uint8_t last_record_valid;  /**< Must be set to true if last_record is being passed */
  uint16_t last_record;
  /**<   Last record. This value is used to read multiple records at the same time.*/

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_read_record_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t content_len;  /**< Must be set to # of elements in content */
  uint8_t content[QMI_UIM_CONTENT_RECORD_MAX_V01];
  /**<   Read content. The content is the sequence of bytes as read from the card.*/
}uim_read_record_result_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t additional_record_len;  /**< Must be set to # of elements in additional_record */
  uint8_t additional_record[QMI_UIM_CONTENT_ADDITIONAL_RECORD_MAX_V01];
  /**<   Read content of all the additional records. Each record has
       the same size as the first record.
  */
}uim_read_additional_record_result_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Provides read access to a specific record in a linear
           fixed or cyclic file in the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Read Result */
  uint8_t read_result_valid;  /**< Must be set to true if read_result is being passed */
  uim_read_record_result_type_v01 read_result;

  /* Optional */
  /*  Additional Read Result */
  uint8_t additional_read_result_valid;  /**< Must be set to true if additional_read_result is being passed */
  uim_read_additional_record_result_type_v01 additional_read_result;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */
}uim_read_record_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a read record command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Read Result */
  uint8_t read_result_valid;  /**< Must be set to true if read_result is being passed */
  uim_read_record_result_type_v01 read_result;

  /* Optional */
  /*  Additional Read Result */
  uint8_t additional_read_result_valid;  /**< Must be set to true if additional_read_result is being passed */
  uim_read_additional_record_result_type_v01 additional_read_result;
}uim_read_record_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t offset;
  /**<   Offset for the Write operation.*/

  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_UIM_CONTENT_TRANSPARENT_MAX_V01];
  /**<   Content to write.*/
}uim_write_transparent_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Provides write access to any transparent
           file in the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  Write Transparent */
  uim_write_transparent_type_v01 write_transparent;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_write_transparent_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Provides write access to any transparent
           file in the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */
}uim_write_transparent_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a write transparent command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_write_transparent_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t record;
  /**<   Record number (starting from 1). This field is ignored for cyclic files.*/

  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_UIM_CONTENT_RECORD_MAX_V01];
  /**<   Content to write.*/
}uim_write_record_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Provides write access to a specific record in a
             linear fixed or cyclic file in the card and provides access by
             the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  Write Record */
  uim_write_record_type_v01 write_record;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_write_record_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Provides write access to a specific record in a
             linear fixed or cyclic file in the card and provides access by
             the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */
}uim_write_record_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a write record command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_write_record_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Retrieves the file attributes for any EF or DF in
             the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */

  /* Optional */
  /*  UICC Security Attributes */
  uint8_t uicc_security_attributes_valid;  /**< Must be set to true if uicc_security_attributes is being passed */
  uint8_t uicc_security_attributes;
  /**<   Indicates whether the security attributes are required by the client.
       Valid values:\n
         - 0 -- Skip security attributes of the file for the UICC card\n
         - 1 -- Return security attributes of the file for the UICC card\n
       If the UICC Security Attributes TLV is missing, security attributes are
       returned. This TLV is ignored for ICC cards.
  */
}uim_get_file_attributes_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_FILE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_FILE_TYPE_TRANSPARENT_V01 = 0x00, 
  UIM_FILE_TYPE_CYCLIC_V01 = 0x01, 
  UIM_FILE_TYPE_LINEAR_FIXED_V01 = 0x02, 
  UIM_FILE_TYPE_DEDICATED_FILE_V01 = 0x03, 
  UIM_FILE_TYPE_MASTER_FILE_V01 = 0x04, 
  UIM_FILE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_file_type_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SEC_CONDITION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SEC_CONDITION_ALWAYS_V01 = 0x00, 
  UIM_SEC_CONDITION_NEVER_V01 = 0x01, 
  UIM_SEC_CONDITION_AND_COND_V01 = 0x02, 
  UIM_SEC_CONDITION_OR_COND_V01 = 0x03, 
  UIM_SEC_CONDITION_SINGLE_COND_V01 = 0x04, 
  UIM_SEC_CONDITION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sec_condition_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t file_size;
  /**<   File size.*/

  uint16_t file_id;
  /**<   File ID.*/

  uim_file_type_enum_v01 file_type;
  /**<   File type. Valid values:\n
         - 0 -- Transparent\n
         - 1 -- Cyclic\n
         - 2 -- Linear fixed\n
         - 3 -- Dedicated file\n
         - 4 -- Master file
  */

  uint16_t rec_size;
  /**<   Size of the records (only for cyclic and linear fixed files).*/

  uint16_t rec_count;
  /**<   Total number of records (only for linear fixed files).*/

  uim_sec_condition_enum_v01 sec_read;
  /**<   Read security attributes. Valid values:\n
         - 0 -- Always\n
         - 1 -- Never\n
         - 2 -- AND condition\n
         - 3 -- OR condition\n
         - 4 -- Single condition
  */

  uint16_t sec_read_mask;
  /**<   Mask with read security attributes. This field is valid only when
       required by security attributes. Valid values:\n
         - Bit 0 -- PIN1\n
         - Bit 1 -- PIN2\n
         - Bit 2 -- UPIN\n
         - Bit 3 -- ADM
  */

  uim_sec_condition_enum_v01 sec_write;
  /**<   Write security attributes. Valid values:\n
         - 0 -- Always\n
         - 1 -- Never\n
         - 2 -- AND condition\n
         - 3 -- OR condition\n
         - 4 -- Single condition
  */

  uint16_t sec_write_mask;
  /**<   Mask with write security attributes. This field is valid only when
       required by security attributes. Valid values:\n
         - Bit 0 -- PIN1\n
         - Bit 1 -- PIN2\n
         - Bit 2 -- UPIN\n
         - Bit 3 -- ADM
  */

  uim_sec_condition_enum_v01 sec_increase;
  /**<   Increase security attributes. Valid values:\n
         - 0 -- Always\n
         - 1 -- Never\n
         - 2 -- AND condition\n
         - 3 -- OR condition\n
         - 4 -- Single condition
  */

  uint16_t sec_increase_mask;
  /**<   Mask with increase security attributes. This field is valid only when
       required by security attributes. Valid values:\n
         - Bit 0 -- PIN1\n
         - Bit 1 -- PIN2\n
         - Bit 2 -- UPIN\n
         - Bit 3 -- ADM
  */

  uim_sec_condition_enum_v01 sec_deactivate;
  /**<   Deactivate security attributes. Valid values:\n
         - 0 -- Always\n
         - 1 -- Never\n
         - 2 -- AND condition\n
         - 3 -- OR condition\n
         - 4 -- Single condition
  */

  uint16_t sec_deactivate_mask;
  /**<   Mask with deactivate security attributes. This field is valid only when
       required by security attributes. Valid values:\n
         - Bit 0 -- PIN1\n
         - Bit 1 -- PIN2\n
         - Bit 2 -- UPIN\n
         - Bit 3 -- ADM
  */

  uim_sec_condition_enum_v01 sec_activate;
  /**<   Activate security attributes. Valid values:\n
         - 0 -- Always\n
         - 1 -- Never\n
         - 2 -- AND condition\n
         - 3 -- OR condition\n
         - 4 -- Single condition
  */

  uint16_t sec_activate_mask;
  /**<   Mask with Activate security attributes. This field is valid only when
       required by security attributes. Valid values:\n
         - Bit 0 -- PIN1\n
         - Bit 1 -- PIN2\n
         - Bit 2 -- UPIN\n
         - Bit 3 -- ADM
  */

  uint32_t raw_value_len;  /**< Must be set to # of elements in raw_value */
  uint8_t raw_value[QMI_UIM_FILE_ATTR_RAW_VALUE_MAX_V01];
  /**<   Raw value of file attributes.*/
}uim_file_attributes_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_FILE_STATUS_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_FILE_STATUS_DEACTIVATED_V01 = 0x00, /**<  File is deactivated.  */
  UIM_FILE_STATUS_ACTIVATED_V01 = 0x01, /**<  File is activated.  */
  UIM_FILE_STATUS_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_file_status_type_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves the file attributes for any EF or DF in
             the card and provides access by the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  File Attributes */
  uint8_t file_attributes_valid;  /**< Must be set to true if file_attributes is being passed */
  uim_file_attributes_type_v01 file_attributes;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  File Status */
  uint8_t file_status_valid;  /**< Must be set to true if file_status is being passed */
  uim_file_status_type_v01 file_status;
  /**<   Indicates the status of the file. Valid values:
      - UIM_FILE_STATUS_DEACTIVATED (0x00) --  File is deactivated. 
      - UIM_FILE_STATUS_ACTIVATED (0x01) --  File is activated. 
 Other values are reserved for future use and are to be ignored by
 the client.
 */
}uim_get_file_attributes_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a get file attributes command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  File Attributes */
  uint8_t file_attributes_valid;  /**< Must be set to true if file_attributes is being passed */
  uim_file_attributes_type_v01 file_attributes;

  /* Optional */
  /*  File Status */
  uint8_t file_status_valid;  /**< Must be set to true if file_status is being passed */
  uim_file_status_type_v01 file_status;
  /**<   Indicates the status of the file. Valid values:
      - UIM_FILE_STATUS_DEACTIVATED (0x00) --  File is deactivated. 
      - UIM_FILE_STATUS_ACTIVATED (0x01) --  File is activated. 
 Other values are reserved for future use and are to be ignored by
 the client.
 */
}uim_get_file_attributes_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t file_id;
  /**<   File ID.*/

  uint32_t path_len;  /**< Must be set to # of elements in path */
  uint8_t path[QMI_UIM_PATH_MAX_V01];
  /**<   Path value. This value must be the complete path of the file, which
       is a sequence block of 2 bytes (e.g., 0x3F00 0x7FFF).
  */
}uim_register_refresh_file_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t register_flag;
  /**<   Flag that indicates whether to register or deregister for refresh
       indications. Valid values:\n
         - 0 -- Deregister\n
         - 1 -- Register
  */

  uint8_t vote_for_init;
  /**<   Flag that indicates whether to vote for the init when there is a
       refresh. Valid values:\n
         - 0 -- Client does not vote for initialization\n
         - 1 -- Client votes for initialization
  */

  uint32_t files_len;  /**< Must be set to # of elements in files */
  uim_register_refresh_file_id_type_v01 files[QMI_UIM_REFRESH_FILES_MAX_V01];
  /**<   List of files to register for refresh.*/
}uim_register_refresh_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Registers for file change notifications triggered by the card. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Register Refresh */
  uim_register_refresh_type_v01 register_refresh;
}uim_refresh_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Registers for file change notifications triggered by the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_refresh_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Enables the client to indicate whether it is OK to start
    the Refresh procedure. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  OK for Refresh */
  uint8_t ok_to_refresh;
  /**<   Indicates whether a refresh is OK. Valid values:\n
         - 0 -- Not OK to refresh\n
         - 1 -- OK to refresh
  */
}uim_refresh_ok_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Enables the client to indicate whether it is OK to start
    the Refresh procedure. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_refresh_ok_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Invoked when the client has finished the Refresh procedure. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Refresh Complete */
  uint8_t refresh_success;
  /**<   Indicates whether the refresh was successful. Valid values:\n
         - 0 -- Refresh was not completed successfully\n
         - 1 -- Refresh was completed successfully
  */
}uim_refresh_complete_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Invoked when the client has finished the Refresh procedure. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_refresh_complete_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Provides the ability to retrieve the last refresh event. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;
}uim_refresh_get_last_event_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_REFRESH_STAGE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_REFRESH_STAGE_WAIT_FOR_OK_V01 = 0x00, 
  UIM_REFRESH_STAGE_START_V01 = 0x01, 
  UIM_REFRESH_STAGE_END_WITH_SUCCESS_V01 = 0x02, 
  UIM_REFRESH_STAGE_END_WITH_FAILURE_V01 = 0x03, 
  UIM_REFRESH_STAGE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_refresh_stage_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_REFRESH_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_REFRESH_MODE_RESET_V01 = 0x00, 
  UIM_REFRESH_MODE_INIT_V01 = 0x01, 
  UIM_REFRESH_MODE_INIT_FCN_V01 = 0x02, 
  UIM_REFRESH_MODE_FCN_V01 = 0x03, 
  UIM_REFRESH_MODE_INIT_FULL_FCN_V01 = 0x04, 
  UIM_REFRESH_MODE_APP_RESET_V01 = 0x05, 
  UIM_REFRESH_MODE_3G_RESET_V01 = 0x06, 
  UIM_REFRESH_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_refresh_mode_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_refresh_stage_enum_v01 stage;
  /**<   Indicates the stage of the Refresh procedure. Valid values:\n
        - 0 -- Waiting for OK to refresh\n
        - 1 -- Refresh started\n
        - 2 -- Refresh ended successfully\n
        - 3 -- Refresh failed
  */

  uim_refresh_mode_enum_v01 mode;
  /**<   Indicates the Refresh mode. Valid values:\n
        - 0 -- Reset\n
        - 1 -- Init\n
        - 2 -- Init and FCN\n
        - 3 -- FCN\n
        - 4 -- Init and Full FCN\n
        - 5 -- Application reset\n
        - 6 -- 3G session reset
  */

  uim_session_type_enum_v01 session_type;
  /**<   Indicates the session type. Valid values: 0 through 7 and 10 through 21.
       See Table \ref{tbl:sessions} for the list of available session types
       with their values and descriptions.
  */

  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value. This value is required only for a
       nonprovisioning session type and is ignored in all other cases.
  */

  uint32_t files_len;  /**< Must be set to # of elements in files */
  uim_register_refresh_file_id_type_v01 files[QMI_UIM_REFRESH_FILES_MAX_V01];
  /**<   List of files refreshed.*/
}uim_refresh_event_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Provides the ability to retrieve the last refresh event. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Refresh Event */
  uint8_t refresh_event_valid;  /**< Must be set to true if refresh_event is being passed */
  uim_refresh_event_type_v01 refresh_event;
}uim_refresh_get_last_event_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PIN_OPERATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PIN_OPERATION_DISABLE_V01 = 0x00, 
  UIM_PIN_OPERATION_ENABLE_V01 = 0x01, 
  UIM_PIN_OPERATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_pin_operation_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PIN_ID_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PIN_ID_PIN_1_V01 = 0x01, 
  UIM_PIN_ID_PIN_2_V01 = 0x02, 
  UIM_PIN_ID_UNIVERSAL_V01 = 0x03, 
  UIM_PIN_ID_HIDDEN_KEY_V01 = 0x04, 
  UIM_PIN_ID_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_pin_id_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_pin_id_enum_v01 pin_id;
  /**<   Indicates the PIN ID to be enabled or disabled. Valid values:\n
        - 1 -- PIN1 (also called PIN)\n
        - 2 -- PIN2\n
        - 3 -- Universal PIN\n
        - 4 -- Hidden key
  */

  uim_pin_operation_enum_v01 pin_operation;
  /**<   Indicates whether the PIN is enabled or disabled. Valid values:\n
        - 0 -- Disable the PIN\n
        - 1 -- Enable the PIN
  */

  uint32_t pin_value_len;  /**< Must be set to # of elements in pin_value */
  char pin_value[QMI_UIM_PIN_MAX_V01];
  /**<   PIN value. This value is a sequence of ASCII characters.*/
}uim_set_pin_protection_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PIN_KEY_REF_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PIN_KEY_REF_APPL_1_V01 = 0x01, 
  UIM_PIN_KEY_REF_APPL_2_V01 = 0x02, 
  UIM_PIN_KEY_REF_APPL_3_V01 = 0x03, 
  UIM_PIN_KEY_REF_APPL_4_V01 = 0x04, 
  UIM_PIN_KEY_REF_APPL_5_V01 = 0x05, 
  UIM_PIN_KEY_REF_APPL_6_V01 = 0x06, 
  UIM_PIN_KEY_REF_APPL_7_V01 = 0x07, 
  UIM_PIN_KEY_REF_APPL_8_V01 = 0x08, 
  UIM_PIN_KEY_REF_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_pin_key_ref_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Enables or disables the protection of the UIM contents by a
    specific PIN. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Set PIN Protection */
  uim_set_pin_protection_type_v01 set_pin_protection;

  /* Optional */
  /*  Key Reference ID */
  uint8_t pin_key_ref_id_valid;  /**< Must be set to true if pin_key_ref_id is being passed */
  uim_pin_key_ref_enum_v01 pin_key_ref_id;
  /**<   Indicates the PIN key reference ID. Valid values are from 1 to 8,
       respectively, for application 1 to application 8, as specified in
       \hyperref[S3]{[S3]}. This TLV is used only for PIN1 and PIN2 and
       is ignored in all other cases.
  */

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_set_pin_protection_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t verify_left;
  /**<   Number of remaining attempts to verify the PIN.*/

  uint8_t unblock_left;
  /**<   Number of remaining attempts to unblock the PIN.*/
}uim_pin_retries_left_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t enc_pin1_value_len;  /**< Must be set to # of elements in enc_pin1_value */
  uint8_t enc_pin1_value[QMI_UIM_ENCRYPTED_PIN_MAX_V01];
  /**<   Encrypted PIN1 value.*/
}uim_encrypted_pin_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Enables or disables the protection of the UIM contents by a
    specific PIN. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the enable/disable
      operation has failed.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 is enabled
      successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_set_pin_protection_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a set PIN protection command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the PIN operation
      has failed.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 operation is
      completed successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_set_pin_protection_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_pin_id_enum_v01 pin_id;
  /**<   Indicates the PIN ID to be verified. Valid values:\n
        - 1 - PIN1 (also called PIN)\n
        - 2 - PIN2\n
        - 3 - Universal PIN\n
        - 4 - Hidden key
  */

  uint32_t pin_value_len;  /**< Must be set to # of elements in pin_value */
  char pin_value[QMI_UIM_PIN_MAX_V01];
  /**<   PIN value. This value is a sequence of ASCII characters.*/
}uim_verify_pin_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Verifies the PIN before the card content is accessed. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Verify PIN */
  uim_verify_pin_type_v01 verify_pin;

  /* Optional */
  /*  Encrypted Value of PIN1 (This field can be used only when the PIN type is
      PIN1.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Key Reference ID */
  uint8_t pin_key_ref_id_valid;  /**< Must be set to true if pin_key_ref_id is being passed */
  uim_pin_key_ref_enum_v01 pin_key_ref_id;
  /**<   Indicates the PIN key reference ID. Valid values are from 1 to 8,
       respectively, for application 1 to application 8, as specified in
       \hyperref[S3]{[S3]}. This TLV is used only for PIN1 and PIN2 and is
       ignored in all other cases.
  */

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_verify_pin_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Verifies the PIN before the card content is accessed. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the verify operation
      fails.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 is verified
      successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_verify_pin_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a verify PIN command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the PIN operation
      has failed.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 operation is
      completed successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_verify_pin_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_pin_id_enum_v01 pin_id;
  /**<   Indicates the PIN ID to be unblocked. Valid values:\n
        - 1 -- PIN1 (also called PIN)\n
        - 2 -- PIN2\n
        - 3 -- Universal PIN
  */

  uint32_t puk_value_len;  /**< Must be set to # of elements in puk_value */
  char puk_value[QMI_UIM_PIN_MAX_V01];
  /**<   PIN unlock key value. This value is a sequence of ASCII characters.*/

  uint32_t new_pin_value_len;  /**< Must be set to # of elements in new_pin_value */
  char new_pin_value[QMI_UIM_PIN_MAX_V01];
  /**<   New PIN value. This value is a sequence of ASCII characters.*/
}uim_unblock_pin_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Unblocks a blocked PIN using the PUK code. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Unblock PIN */
  uim_unblock_pin_type_v01 unblock_pin;

  /* Optional */
  /*  Key Reference ID */
  uint8_t pin_key_ref_id_valid;  /**< Must be set to true if pin_key_ref_id is being passed */
  uim_pin_key_ref_enum_v01 pin_key_ref_id;
  /**<   Indicates the PIN key reference ID. Valid values are from 1 to 8,
       respectively, for application 1 to application 8, as specified in
       \hyperref[S3]{[S3]}. This TLV is used only for PIN1 and PIN2 and is
       ignored in all other cases.
  */

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_unblock_pin_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Unblocks a blocked PIN using the PUK code. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the unblock operation
      fails.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 is unblocked
      successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_unblock_pin_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with an unblock PIN command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the PIN operation
      has failed.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 operation is
      completed successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_unblock_pin_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_pin_id_enum_v01 pin_id;
  /**<   Indicates the PIN ID to be changed. Valid values:\n
        - 1 -- PIN1 (also called PIN)\n
        - 2 -- PIN2\n
        - 3 -- Universal PIN\n
        - 4 -- Hidden key
  */

  uint32_t old_pin_value_len;  /**< Must be set to # of elements in old_pin_value */
  char old_pin_value[QMI_UIM_PIN_MAX_V01];
  /**<   Old PIN value. The value is a sequence of ASCII characters.*/

  uint32_t new_pin_value_len;  /**< Must be set to # of elements in new_pin_value */
  char new_pin_value[QMI_UIM_PIN_MAX_V01];
  /**<   New PIN value. This value is a sequence of ASCII characters.*/
}uim_change_pin_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Changes the value of the specified PIN. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Change PIN */
  uim_change_pin_type_v01 change_pin;

  /* Optional */
  /*  Key Reference ID */
  uint8_t pin_key_ref_id_valid;  /**< Must be set to true if pin_key_ref_id is being passed */
  uim_pin_key_ref_enum_v01 pin_key_ref_id;
  /**<   Indicates the PIN key reference ID. Valid values are from 1 to 8,
       respectively,  for application 1 to application 8, as specified in
       \hyperref[S3]{[S3]}. This TLV is used only for PIN1 and PIN2 and
       ignored in all other cases.
  */

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_change_pin_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Changes the value of the specified PIN. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the change PIN
      operation fails.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 is changed
      successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_change_pin_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a change PIN command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when the PIN operation
      has failed.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_pin_retries_left_type_v01 retries_left;

  /* Optional */
  /*  Encrypted PIN1 (This value is returned only when PIN1 operation is
      completed successfully and the feature is supported.)
   */
  uint8_t encrypted_pin1_valid;  /**< Must be set to true if encrypted_pin1 is being passed */
  uim_encrypted_pin_type_v01 encrypted_pin1;

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;
}uim_change_pin_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PERSO_FEATURE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PERSO_FEATURE_GW_NETWORK_V01 = 0x00, 
  UIM_PERSO_FEATURE_GW_NETWORK_SUBSET_V01 = 0x01, 
  UIM_PERSO_FEATURE_GW_SERVICE_PROVIDER_V01 = 0x02, 
  UIM_PERSO_FEATURE_GW_CORPORATE_V01 = 0x03, 
  UIM_PERSO_FEATURE_GW_UIM_V01 = 0x04, 
  UIM_PERSO_FEATURE_1X_NETWORK_TYPE_1_V01 = 0x05, 
  UIM_PERSO_FEATURE_1X_NETWORK_TYPE_2_V01 = 0x06, 
  UIM_PERSO_FEATURE_1X_HRPD_V01 = 0x07, 
  UIM_PERSO_FEATURE_1X_SERVICE_PROVIDER_V01 = 0x08, 
  UIM_PERSO_FEATURE_1X_CORPORATE_V01 = 0x09, 
  UIM_PERSO_FEATURE_1X_RUIM_V01 = 0x0A, 
  UIM_PERSO_FEATURE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_perso_feature_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PERSO_OPERATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PERSO_OPERATION_DEACTIVATE_V01 = 0x00, 
  UIM_PERSO_OPERATION_UNBLOCK_V01 = 0x01, 
  UIM_PERSO_OPERATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_perso_operation_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_perso_feature_enum_v01 feature;
  /**<   Indicates the personalization feature to deactivate or unblock.
       Valid values:\n
         - 0 -- GW network personalization\n
         - 1 -- GW network subset personalization\n
         - 2 -- GW service provider personalization\n
         - 3 -- GW corporate personalization\n
         - 4 -- GW UIM personalization\n
         - 5 -- 1X network type 1 personalization\n
         - 6 -- 1X network type 2 personalization\n
         - 7 -- 1X HRPD personalization\n
         - 8 -- 1X service provider personalization\n
         - 9 -- 1X corporate personalization\n
         - 10 -- 1X RUIM personalization
  */

  uim_perso_operation_enum_v01 operation;
  /**<   Indicates the operation to perform. Valid values:\n
        - 0 -- Deactivate personalization.\n
        - 1 -- Unblock personalization.
  */

  uint32_t ck_value_len;  /**< Must be set to # of elements in ck_value */
  char ck_value[QMI_UIM_CK_MAX_V01];
  /**<   Control key value. This value is a sequence of ASCII characters.*/
}uim_depersonalization_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Deactivates or unblocks the personalization on the phone. */
typedef struct {

  /* Mandatory */
  /*  Depersonalization */
  uim_depersonalization_type_v01 depersonalization;
}uim_depersonalization_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t verify_left;
  /**<   Number of the remaining attempts to verify the personalization.*/

  uint8_t unblock_left;
  /**<   Number of the remaining attempts to unblock the personalization.*/
}uim_perso_retries_left_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Deactivates or unblocks the personalization on the phone. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when deactivation or
      unblock fails.)
   */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_perso_retries_left_type_v01 retries_left;
}uim_depersonalization_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SLOT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SLOT_1_V01 = 0x01, 
  UIM_SLOT_2_V01 = 0x02, 
  UIM_SLOT_3_V01 = 0x03, 
  UIM_SLOT_4_V01 = 0x04, 
  UIM_SLOT_5_V01 = 0x05, 
  UIM_SLOT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_slot_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Powers down the card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */
}uim_power_down_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Powers down the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_power_down_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Powers up the card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Optional */
  /*  Ignore Hot-Swap Switch */
  uint8_t ignore_hotswap_switch_valid;  /**< Must be set to true if ignore_hotswap_switch is being passed */
  uint8_t ignore_hotswap_switch;
  /**<   Indicates if the hot-swap switch needs to be ignored during the
       initialization of the card. Valid values:\n
        - 0 -- Checks the hot-swap switch status\n
        - 1 -- Ignores the hot-swap switch status
  */
}uim_power_up_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Powers up the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_power_up_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Retrieves the current status of the card. */
typedef struct {

  /* Optional */
  /*  Extended Card Status */
  uint8_t extended_card_status_valid;  /**< Must be set to true if extended_card_status is being passed */
  uint8_t extended_card_status;
  /**<   Indicates whether the extended card status is requested. Valid values:\n
         - 0 -- Legacy card status (up to 2 cards)\n
         - 1 -- Extended card status\n
       If the Extended Card Status TLV is missing, the legacy card status with
       support for only 2 cards is returned.
  */
}uim_get_card_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_CARD_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_CARD_STATE_ABSENT_V01 = 0x00, 
  UIM_CARD_STATE_PRESENT_V01 = 0x01, 
  UIM_CARD_STATE_ERROR_V01 = 0x02, 
  UIM_CARD_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_card_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_EXTENDED_CARD_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_EXTENDED_CARD_STATE_ABSENT_V01 = 0x00, 
  UIM_EXTENDED_CARD_STATE_PRESENT_V01 = 0x01, 
  UIM_EXTENDED_CARD_STATE_ERROR_V01 = 0x02, 
  UIM_EXTENDED_CARD_STATE_UNKNOWN_V01 = 0x03, 
  UIM_EXTENDED_CARD_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_extended_card_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PIN_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PIN_STATE_UNKNOWN_V01 = 0x00, 
  UIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01 = 0x01, 
  UIM_PIN_STATE_ENABLED_VERIFIED_V01 = 0x02, 
  UIM_PIN_STATE_DISABLED_V01 = 0x03, 
  UIM_PIN_STATE_BLOCKED_V01 = 0x04, 
  UIM_PIN_STATE_PERMANENTLY_BLOCKED_V01 = 0x05, 
  UIM_PIN_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_pin_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_CARD_ERROR_CODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_CARD_ERROR_CODE_UNKNOWN_V01 = 0x00, /**<  Unknown  */
  UIM_CARD_ERROR_CODE_POWER_DOWN_V01 = 0x01, /**<  Power down  */
  UIM_CARD_ERROR_CODE_POLL_ERROR_V01 = 0x02, /**<  Poll error  */
  UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED_V01 = 0x03, /**<  No ATR received  */
  UIM_CARD_ERROR_CODE_VOLT_MISMATCH_V01 = 0x04, /**<  Volt mismatch  */
  UIM_CARD_ERROR_CODE_PARITY_ERROR_V01 = 0x05, /**<  Parity error  */
  UIM_CARD_ERROR_CODE_POSSIBLY_REMOVED_V01 = 0x06, /**<  Unknown, possibly removed  */
  UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS_V01 = 0x07, /**<  Card returned technical problems  */
  UIM_CARD_ERROR_CODE_NULL_BYTES_V01 = 0x08, /**<  Card returned NULL bytes  */
  UIM_CARD_ERROR_CODE_SAP_CONNECTED_V01 = 0x09, /**<  Terminal in SAP mode  */
  UIM_CARD_ERROR_CODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_card_error_code_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_APP_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_APP_TYPE_UNKNOWN_V01 = 0x00, 
  UIM_APP_TYPE_SIM_V01 = 0x01, 
  UIM_APP_TYPE_USIM_V01 = 0x02, 
  UIM_APP_TYPE_RUIM_V01 = 0x03, 
  UIM_APP_TYPE_CSIM_V01 = 0x04, 
  UIM_APP_TYPE_ISIM_V01 = 0x05, 
  UIM_APP_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_app_type_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_APP_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_APP_STATE_UNKNOWN_V01 = 0x00, 
  UIM_APP_STATE_DETECTED_V01 = 0x01, 
  UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01 = 0x02, 
  UIM_APP_STATE_PUK1_OR_PUK_REQ_V01 = 0x03, 
  UIM_APP_STATE_PERSON_CHECK_REQ_V01 = 0x04, 
  UIM_APP_STATE_PIN1_PERM_BLOCKED_V01 = 0x05, 
  UIM_APP_STATE_ILLEGAL_V01 = 0x06, 
  UIM_APP_STATE_READY_V01 = 0x07, 
  UIM_APP_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_app_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PERSO_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PERSO_STATE_UNKNOWN_V01 = 0x00, 
  UIM_PERSO_STATE_IN_PROGRESS_V01 = 0x01, 
  UIM_PERSO_STATE_READY_V01 = 0x02, 
  UIM_PERSO_STATE_CODE_REQ_V01 = 0x03, 
  UIM_PERSO_STATE_PUK_REQ_V01 = 0x04, 
  UIM_PERSO_STATE_PERMANENTLY_BLOCKED_V01 = 0x05, 
  UIM_PERSO_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_perso_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_UNIV_PIN_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_UNIV_PIN_PIN1_USED_V01 = 0x00, 
  UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01 = 0x01, 
  UIM_UNIV_PIN_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_univ_pin_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PERSO_FEATURE_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PERSO_FEATURE_STATUS_GW_NETWORK_V01 = 0x00, 
  UIM_PERSO_FEATURE_STATUS_GW_NETWORK_SUBSET_V01 = 0x01, 
  UIM_PERSO_FEATURE_STATUS_GW_SERVICE_PROVIDER_V01 = 0x02, 
  UIM_PERSO_FEATURE_STATUS_GW_CORPORATE_V01 = 0x03, 
  UIM_PERSO_FEATURE_STATUS_GW_UIM_V01 = 0x04, 
  UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_1_V01 = 0x05, 
  UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_2_V01 = 0x06, 
  UIM_PERSO_FEATURE_STATUS_1X_HRPD_V01 = 0x07, 
  UIM_PERSO_FEATURE_STATUS_1X_SERVICE_PROVIDER_V01 = 0x08, 
  UIM_PERSO_FEATURE_STATUS_1X_CORPORATE_V01 = 0x09, 
  UIM_PERSO_FEATURE_STATUS_1X_RUIM_V01 = 0x0A, 
  UIM_PERSO_FEATURE_STATUS_UNKNOWN_V01 = 0x0B, 
  UIM_PERSO_FEATURE_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_perso_feature_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_pin_state_enum_v01 pin_state;
  /**<   Indicates the state of PIN. Valid values:\n
        - 0 -- Unknown\n
        - 1 -- Enabled and not verified\n
        - 2 -- Enabled and verified\n
        - 3 -- Disabled\n
        - 4 -- Blocked\n
        - 5 -- Permanently blocked
  */

  uint8_t pin_retries;
  /**<   Indicates the number of retries remaining to verify this PIN.*/

  uint8_t puk_retries;
  /**<   Indicates the number of retries remaining to unblock this PIN.*/
}uim_pin_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_app_type_enum_v01 app_type;
  /**<   Indicates the type of the application. Valid values:\n
        - 0 -- Unknown\n
        - 1 -- SIM card\n
        - 2 -- USIM application\n
        - 3 -- RUIM card\n
        - 4 -- CSIM application\n
        - 5 -- ISIM application\n
       Other values are reserved for the future and are to be handled as
       ``Unknown''.
  */

  uim_app_state_enum_v01 app_state;
  /**<   Indicates the state of the application. Valid values:\n
        - 0 -- Unknown\n
        - 1 -- Detected\n
        - 2 -- PIN1 or UPIN is required\n
        - 3 -- PUK1 or PUK for UPIN is required\n
        - 4 -- Personalization state must be checked\n
        - 5 -- PIN1 is blocked\n
        - 6 -- Illegal\n
        - 7 -- Ready
  */

  uim_perso_state_enum_v01 perso_state;
  /**<   Indicates the state of the personalization for the application.
       Valid values:\n
        - 0 -- Unknown\n
        - 1 -- Personalization operation is in progress\n
        - 2 -- Ready\n
        - 3 -- Personalization code is required\n
        - 4 -- PUK for personalization code is required\n
        - 5 -- Permanently blocked
  */

  uim_perso_feature_status_enum_v01 perso_feature;
  /**<   Indicates the personalization feature. This applies only when a
       personalization code is required to deactivate or unblock
       personalization. Valid values:\n
        - 0  -- GW network personalization\n
        - 1  -- GW network subset personalization\n
        - 2  -- GW service provider personalization\n
        - 3  -- GW corporate personalization\n
        - 4  -- GW UIM personalization\n
        - 5  -- 1X network type 1 personalization\n
        - 6  -- 1X network type 2 personalization\n
        - 7  -- 1X HRPD personalization\n
        - 8  -- 1X service provider personalization\n
        - 9  -- 1X corporate personalization\n
        - 10 -- 1X RUIM personalization\n
        - 11 -- Unknown
  */

  uint8_t perso_retries;
  /**<   Indicates the number of retries remaining to disable the personalization.*/

  uint8_t perso_unblock_retries;
  /**<   Indicates the number of retries remaining to unblock the personalization.*/

  uint32_t aid_value_len;  /**< Must be set to # of elements in aid_value */
  uint8_t aid_value[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value.*/

  uim_univ_pin_enum_v01 univ_pin;
  /**<   Indicates whether UPIN replaces PIN1. Valid values:\n
        - 0 -- PIN1 is used\n
        - 1 -- UPIN replaces PIN1
  */

  uim_pin_info_type_v01 pin1;
  /**<   Contains information about PIN1.*/

  uim_pin_info_type_v01 pin2;
  /**<   Contains information about PIN2.*/
}app_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_card_state_enum_v01 card_state;
  /**<   Indicates the state of the card. Valid values:\n
        - 0 -- Absent\n
        - 1 -- Present\n
        - 2 -- Error
  */

  uim_pin_info_type_v01 upin;
  /**<   Contains information about the universal PIN.*/

  uim_card_error_code_enum_v01 error_code;
  /**<   Indicates the reason for the card error, and is valid only when the card
 state is Error. Valid values:\n
      - UIM_CARD_ERROR_CODE_UNKNOWN (0x00) --  Unknown 
      - UIM_CARD_ERROR_CODE_POWER_DOWN (0x01) --  Power down 
      - UIM_CARD_ERROR_CODE_POLL_ERROR (0x02) --  Poll error 
      - UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED (0x03) --  No ATR received 
      - UIM_CARD_ERROR_CODE_VOLT_MISMATCH (0x04) --  Volt mismatch 
      - UIM_CARD_ERROR_CODE_PARITY_ERROR (0x05) --  Parity error 
      - UIM_CARD_ERROR_CODE_POSSIBLY_REMOVED (0x06) --  Unknown, possibly removed 
      - UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS (0x07) --  Card returned technical problems 
      - UIM_CARD_ERROR_CODE_NULL_BYTES (0x08) --  Card returned NULL bytes 
      - UIM_CARD_ERROR_CODE_SAP_CONNECTED (0x09) --  Terminal in SAP mode 
 \n
 Other values are possible and reserved for future use. When an
 unknown value is received, it is to be handled as ``Unknown''.
 */

  uint32_t app_info_len;  /**< Must be set to # of elements in app_info */
  app_info_type_v01 app_info[QMI_UIM_APPS_MAX_V01];
  /**<   Contains the application information.*/
}card_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t index_gw_pri;
  /**<   Index of the primary GW provisioning application. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
  */

  uint16_t index_1x_pri;
  /**<   Index of the primary 1X provisioning application. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
    */

  uint16_t index_gw_sec;
  /**<   Index of the secondary GW provisioning application. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
  */

  uint16_t index_1x_sec;
  /**<   Index of the secondary 1X provisioning application. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
  */

  uint32_t card_info_len;  /**< Must be set to # of elements in card_info */
  card_info_type_v01 card_info[QMI_UIM_CARDS_MAX_V01];
  /**<   Contains the card information.*/
}uim_card_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_extended_card_state_enum_v01 card_state;
  /**<   Indicates the state of the card. Valid values:\n
        - 0 -- Absent\n
        - 1 -- Present\n
        - 2 -- Error\n
        - 3 -- Unknown
  */

  uim_pin_info_type_v01 upin;
  /**<   Contains information about the universal PIN.*/

  uim_card_error_code_enum_v01 error_code;
  /**<   Indicates the reason for the card error, and is valid only when the card
 state is Error. Valid values:\n
      - UIM_CARD_ERROR_CODE_UNKNOWN (0x00) --  Unknown 
      - UIM_CARD_ERROR_CODE_POWER_DOWN (0x01) --  Power down 
      - UIM_CARD_ERROR_CODE_POLL_ERROR (0x02) --  Poll error 
      - UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED (0x03) --  No ATR received 
      - UIM_CARD_ERROR_CODE_VOLT_MISMATCH (0x04) --  Volt mismatch 
      - UIM_CARD_ERROR_CODE_PARITY_ERROR (0x05) --  Parity error 
      - UIM_CARD_ERROR_CODE_POSSIBLY_REMOVED (0x06) --  Unknown, possibly removed 
      - UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS (0x07) --  Card returned technical problems 
      - UIM_CARD_ERROR_CODE_NULL_BYTES (0x08) --  Card returned NULL bytes 
      - UIM_CARD_ERROR_CODE_SAP_CONNECTED (0x09) --  Terminal in SAP mode 
 \n
 Other values are possible and reserved for future use. When an
 unknown value is received, it is to be handled as ``Unknown''.
 */

  uint32_t app_info_len;  /**< Must be set to # of elements in app_info */
  app_info_type_v01 app_info[QMI_UIM_EXTENDED_APPS_MAX_V01];
  /**<   Contains the application information.*/
}extended_card_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t index_gw_len;  /**< Must be set to # of elements in index_gw */
  uint16_t index_gw[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Index of the GW provisioning applications. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
  */

  uint32_t index_1x_len;  /**< Must be set to # of elements in index_1x */
  uint16_t index_1x[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Index of the 1X provisioning applications. The most significant
       byte indicates the slot (starting from 0), while the least significant
       byte indicates the application for that slot (starting from 0). The
       value 0xFFFF identifies that the session does not exist.
    */

  uint32_t card_info_len;  /**< Must be set to # of elements in card_info */
  extended_card_info_type_v01 card_info[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Contains the card information.*/
}uim_extended_card_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_HOT_SWAP_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_HOT_SWAP_NOT_SUPPORTED_V01 = 0x00, 
  UIM_HOT_SWAP_STATUS_NOT_SUPPORTED_V01 = 0x01, 
  UIM_HOT_SWAP_STATUS_PRESENT_V01 = 0x02, 
  UIM_HOT_SWAP_STATUS_NOT_PRESENT_V01 = 0x03, 
  UIM_HOT_SWAP_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_hot_swap_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves the current status of the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Status */
  uint8_t card_status_valid;  /**< Must be set to true if card_status is being passed */
  uim_card_status_type_v01 card_status;

  /* Optional */
  /*  Hot-Swap Status */
  uint8_t hot_swap_valid;  /**< Must be set to true if hot_swap is being passed */
  uint32_t hot_swap_len;  /**< Must be set to # of elements in hot_swap */
  uim_hot_swap_status_enum_v01 hot_swap[QMI_UIM_CARDS_MAX_V01];
  /**<   Indicates the status of the hot-swap switch. Valid values:\n
        - 0 -- Hot-swap is not supported\n
        - 1 -- Hot-swap is supported, but the status of the switch is not
               supported\n
        - 2 -- Switch indicates that the card is present\n
        - 3 -- Switch indicates that the card is not present
  */

  /* Optional */
  /*  Valid Card Status */
  uint8_t card_status_validity_valid;  /**< Must be set to true if card_status_validity is being passed */
  uint32_t card_status_validity_len;  /**< Must be set to # of elements in card_status_validity */
  uint8_t card_status_validity[QMI_UIM_CARDS_MAX_V01];
  /**<   Indicates whether the reported status of the card on the corresponding
       slot is valid. Valid values:\n
        - 0 -- Status of the card is unknown (even when reported as absent)\n
        - 1 -- Status of the card is valid
  */

  /* Optional */
  /*  Extended Card Status */
  uint8_t extended_card_status_valid;  /**< Must be set to true if extended_card_status is being passed */
  uim_extended_card_status_type_v01 extended_card_status;

  /* Optional */
  /*  Extended Hot-Swap Status */
  uint8_t extended_hot_swap_valid;  /**< Must be set to true if extended_hot_swap is being passed */
  uint32_t extended_hot_swap_len;  /**< Must be set to # of elements in extended_hot_swap */
  uim_hot_swap_status_enum_v01 extended_hot_swap[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Indicates the status of the hot-swap switch. Valid values:\n
        - 0 -- Hot-swap is not supported\n
        - 1 -- Hot-swap is supported, but the status of the switch is not
               supported\n
        - 2 -- Switch indicates that the card is present\n
        - 3 -- Switch indicates that the card is not present
  */

  /* Optional */
  /*  SIM Busy Status */
  uint8_t sim_busy_status_valid;  /**< Must be set to true if sim_busy_status is being passed */
  uint32_t sim_busy_status_len;  /**< Must be set to # of elements in sim_busy_status */
  uint8_t sim_busy_status[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Indicates whether the card on the corresponding slot is busy. Valid
       values:\n
        - 0 -- SIM card is not busy \n
        - 1 -- SIM card is busy
  */
}uim_get_card_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Registers for event notifications from the card. */
typedef struct {

  /* Mandatory */
  /*  Event Mask */
  uint32_t event_mask;
  /**<   Bitmask of the events to be received by the client. Additional
       bits are reserved for future use and must be set to zero by the caller.\n
        - Bit 0 -- Card status\n
        - Bit 1 -- SAP connection\n
        - Bit 2 -- Extended card status\n
        - Bit 3 -- Close of provisioning sessions\n
        - Bit 4 -- Physical slot status\n
        - Bit 5 -- SIM busy status
  */
}uim_event_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Registers for event notifications from the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Event Registration Result */
  uint8_t event_mask_valid;  /**< Must be set to true if event_mask is being passed */
  uint32_t event_mask;
  /**<   Bitmask of the events that were successfully enabled. This result
       can be different from the mask used in the request when
       notifications are not supported. Additional bits are reserved for
       future use and can be ignored by the caller.\n
        - Bit 0 -- Card status\n
        - Bit 1 -- SAP connection\n
        - Bit 2 -- Extended card status\n
        - Bit 3 -- Close of provisioning sessions\n
        - Bit 4 -- Physical slot status\n
        - Bit 5 -- SIM busy status
  */
}uim_event_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates that the status of the card has changed. */
typedef struct {

  /* Optional */
  /*  Card Status */
  uint8_t card_status_valid;  /**< Must be set to true if card_status is being passed */
  uim_card_status_type_v01 card_status;

  /* Optional */
  /*  Valid Card Status */
  uint8_t card_status_validity_valid;  /**< Must be set to true if card_status_validity is being passed */
  uint32_t card_status_validity_len;  /**< Must be set to # of elements in card_status_validity */
  uint8_t card_status_validity[QMI_UIM_CARDS_MAX_V01];
  /**<   Indicates whether the reported status of the card on the corresponding
       slot is valid. Valid values: \n
        - 0 -- Status of the card is unknown (even if reported as absent)\n
        - 1 -- Status of the card is valid
  */

  /* Optional */
  /*  Extended Card Status */
  uint8_t extended_card_status_valid;  /**< Must be set to true if extended_card_status is being passed */
  uim_extended_card_status_type_v01 extended_card_status;
}uim_status_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Received when a refresh command comes from the card. */
typedef struct {

  /* Optional */
  /*  Refresh Event */
  uint8_t refresh_event_valid;  /**< Must be set to true if refresh_event is being passed */
  uim_refresh_event_type_v01 refresh_event;
}uim_refresh_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_AUTH_CONTEXT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_AUTH_CONTEXT_RUN_GSM_ALG_V01 = 0x00, 
  UIM_AUTH_CONTEXT_RUN_CAVE_ALG_V01 = 0x01, 
  UIM_AUTH_CONTEXT_GSM_SEC_V01 = 0x02, 
  UIM_AUTH_CONTEXT_3G_SEC_V01 = 0x03, 
  UIM_AUTH_CONTEXT_VGCS_VBS_SEC_V01 = 0x04, 
  UIM_AUTH_CONTEXT_GBA_SEC_BOOTSTRAPPING_V01 = 0x05, 
  UIM_AUTH_CONTEXT_GBA_SEC_NAF_DERIVATION_V01 = 0x06, 
  UIM_AUTH_CONTEXT_MBMS_SEC_MSK_UPDATE_V01 = 0x07, 
  UIM_AUTH_CONTEXT_MBMS_SEC_MTK_GENERATION_V01 = 0x08, 
  UIM_AUTH_CONTEXT_MBMS_SEC_MSK_DELETION_V01 = 0x09, 
  UIM_AUTH_CONTEXT_MBMS_SEC_MUK_DELETION_V01 = 0x0A, 
  UIM_AUTH_CONTEXT_IMS_AKA_SEC_V01 = 0x0B, 
  UIM_AUTH_CONTEXT_HTTP_DIGEST_SEC_V01 = 0x0C, 
  UIM_AUTH_CONTEXT_COMPUTE_IP_CHAP_V01 = 0x0D, 
  UIM_AUTH_CONTEXT_COMPUTE_IP_MN_HA_V01 = 0x0E, 
  UIM_AUTH_CONTEXT_COMPUTE_IP_MIP_RRQ_HASH_V01 = 0x0F, 
  UIM_AUTH_CONTEXT_COMPUTE_IP_MN_AAA_V01 = 0x10, 
  UIM_AUTH_CONTEXT_COMPUTE_IP_HRPD_ACCESS_V01 = 0x11, 
  UIM_AUTH_CONTEXT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_auth_context_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_auth_context_enum_v01 context;
  /**<   Authenticate context. Valid values:\n
        - 0  -- Runs the GSM alogrithm (valid only on a 2G SIM card, as
                specified in \hyperref[S8]{[S8]})\n
        - 1  -- Runs the CAVE algorithm (valid only on a RUIM card, as
                specified in \hyperref[S9]{[S9]})\n
        - 2  -- GSM security context (valid only on a USIM application, as
                specified in \hyperref[S4]{[S4]})\n
        - 3  -- 3G security context (valid only on a USIM application, as
                specified in \hyperref[S4]{[S4]})\n
        - 4  -- VGCS/VBS security context (valid only on a USIM application, as
                specified in \hyperref[S4]{[S4]})\n
        - 5  -- GBA security context, Bootstrapping mode (valid only on
                a USIM or ISIM application, as
                specified in \hyperref[S4]{[S4]} and
                \hyperref[S10]{[S10]})\n
        - 6  -- GBA security context, NAF Derivation mode (valid only on
                a USIM or ISIM application, as
                specified in \hyperref[S4]{[S4]} and
                \hyperref[S10]{[S10]})\n
        - 7  -- MBMS security context, MSK Update mode (valid only on
                a USIM application, as
                specified in \hyperref[S4]{[S4]})\n
        - 8  -- MBMS security context, MTK Generation mode (valid only
                on a USIM application, as
                specified in \hyperref[S4]{[S4]})\n
        - 9  -- MBMS security context, MSK Deletion mode (valid only on
                a USIM application, as specified in
                \hyperref[S4]{[S4]})\n
        - 10 -- MBMS security context, MUK Deletion mode (valid only on
                a USIM application, as specified in
                \hyperref[S4]{[S4]})\n
        - 11 -- IMS AKA security context (valid only on a ISIM application, as
                specified in \hyperref[S10]{[S10]})\n
        - 12 -- HTTP-digest security context valid only on an ISIM
                application, as specified in
                \hyperref[S10]{[S10]})\n
        - 13 -- Compute IP authentication, CHAP (valid only on RUIM or CSIM, as
                specified in \hyperref[S9]{[S9]} and
                \hyperref[S11]{[S11]})\n
        - 14 -- Compute IP authentication, MN-HA authenticator (valid only on
                RUIM or CSIM, as specified in
                \hyperref[S9]{[S9]} and
                \hyperref[S11]{[S11]})\n
        - 15 -- Compute IP authentication, MIP-RRQ hash (valid only on RUIM
                or CSIM, as specified in
                \hyperref[S9]{[S9]} and
                \hyperref[S11]{[S11]})\n
        - 16 -- Compute IP authentication, MN-AAA authenticator (valid only
                on RUIM or CSIM, as specified in
                \hyperref[S9]{[S9]} and
                \hyperref[S11]{[S11]})\n
        - 17 -- Compute IP authentication, HRPD access authenticator (valid
                only on RUIM or CSIM, as specified in
                \hyperref[S9]{[S9]} and
                \hyperref[S11]{[S11]})\n
       Other values are possible and reserved for future use.
  */

  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_UIM_AUTHENTICATE_DATA_MAX_V01];
  /**<   Authenticate data.*/
}uim_authentication_data_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Executes the authentication algorithm on the card. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Authentication Data */
  uim_authentication_data_type_v01 authentication_data;

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_authenticate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Executes the authentication algorithm on the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Authenticate Result */
  uint8_t content_valid;  /**< Must be set to true if content is being passed */
  uint32_t content_len;  /**< Must be set to # of elements in content */
  uint8_t content[QMI_UIM_AUTHENTICATE_RESULT_MAX_V01];
  /**<   Authenticate data. This value is a sequence of bytes returned from the card.*/

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */
}uim_authenticate_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with an authenticate command result. */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.*/

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Authenticate Result */
  uint8_t content_valid;  /**< Must be set to true if content is being passed */
  uint32_t content_len;  /**< Must be set to # of elements in content */
  uint8_t content[QMI_UIM_AUTHENTICATE_RESULT_MAX_V01];
  /**<   Authenticate data. This value is a sequence of bytes returned from the card.*/
}uim_authenticate_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Forcibly closes a session to a nonprovisioning application. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;
}uim_close_session_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Forcibly closes a session to a nonprovisioning application. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  - Error code. Possible error code values are described
                         in the error codes section of each message definition.
  */
}uim_close_session_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Retrieves the status of a service on the card. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Capabilities Mask */
  uint32_t mask;
  /**<   Bitmask of the capabilities the client retrieves from the card.
       Additional bits are reserved for future use and are to be set to zero.
       \begin{itemize1}
       \item Bit 0 -- FDN status
       \item Bit 1 -- Hidden key PIN status (for USIM cards only)
       \item Bit 2 -- Index in the EF-DIR (for UICC cards only)
       \item Bit 3 -- ESN status (for 1X sessions only)
       \item Bit 4 -- ACL status (for USIM cards only)
       \vspace{-0.18in}
       \end{itemize1}
  */
}uim_get_service_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_FDN_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_FDN_STATUS_NOT_AVAILABLE_V01 = 0x00, 
  UIM_FDN_STATUS_AVAILABLE_DISABLED_V01 = 0x01, 
  UIM_FDN_STATUS_AVAILABLE_ENABLED_V01 = 0x02, 
  UIM_FDN_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_fdn_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_HIDDEN_KEY_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_HIDDEN_KEY_STATUS_NOT_SUPPORTED_V01 = 0x00, 
  UIM_HIDDEN_KEY_STATUS_ENABLED_NOT_VERIFIED_V01 = 0x01, 
  UIM_HIDDEN_KEY_STATUS_ENABLED_VERIFIED_V01 = 0x02, 
  UIM_HIDDEN_KEY_STATUS_DISABLED_V01 = 0x03, 
  UIM_HIDDEN_KEY_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_hidden_key_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_ESN_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_ESN_STATUS_NO_CHANGE_V01 = 0x00, 
  UIM_ESN_STATUS_CHANGED_V01 = 0x01, 
  UIM_ESN_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_esn_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_ACL_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_ACL_STATUS_NOT_AVAILABLE_V01 = 0x00, 
  UIM_ACL_STATUS_AVAILABLE_DISABLED_V01 = 0x01, 
  UIM_ACL_STATUS_AVAILABLE_ENABLED_V01 = 0x02, 
  UIM_ACL_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_acl_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves the status of a service on the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  FDN Status */
  uint8_t fdn_status_valid;  /**< Must be set to true if fdn_status is being passed */
  uim_fdn_status_enum_v01 fdn_status;
  /**<   Fixed dialing number status. Valid values:\n
        - 0 -- FDN is not available\n
        - 1 -- FDN is available and disabled\n
        - 2 -- FDN is available and enabled
  */

  /* Optional */
  /*  Hidden Key Status */
  uint8_t hidden_key_status_valid;  /**< Must be set to true if hidden_key_status is being passed */
  uim_hidden_key_status_enum_v01 hidden_key_status;
  /**<   Status of the hidden key PIN. Valid values:\n
        - 0 -- Not supported\n
        - 1 -- Enabled and not verified\n
        - 2 -- Enabled and verified\n
        - 3 -- Disabled
  */

  /* Optional */
  /*  Index */
  uint8_t index_valid;  /**< Must be set to true if index is being passed */
  uint8_t index;
  /**<   Index of the application in the EF_DIR file, starting from 1.*/

  /* Optional */
  /*  ESN Status */
  uint8_t esn_status_valid;  /**< Must be set to true if esn_status is being passed */
  uim_esn_status_enum_v01 esn_status;
  /**<   ESN status. Valid values:\n
        - 0 -- ESN is not changed\n
        - 1 -- ESN is changed
  */

  /* Optional */
  /*  ACL Status */
  uint8_t acl_status_valid;  /**< Must be set to true if acl_status is being passed */
  uim_acl_status_enum_v01 acl_status;
  /**<   Access point name control list service status. Valid values:\n
        - 0 -- ACL is not available\n
        - 1 -- ACL is available and disabled\n
        - 2 -- ACL is available and enabled
  */
}uim_get_service_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Changes the status of a service on the card. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Optional */
  /*  FDN Service Enable/Disable */
  uint8_t fdn_status_valid;  /**< Must be set to true if fdn_status is being passed */
  uint8_t fdn_status;
  /**<   Enables or disables the FDN service. Valid values:\n
        - 0 -- Disables the FDN service\n
        - 1 -- Enables the FDN service
  */

  /* Optional */
  /*  ACL Service Enable/Disable */
  uint8_t enable_acl_valid;  /**< Must be set to true if enable_acl is being passed */
  uint8_t enable_acl;
  /**<   Enables or disables the ACL service. Valid values:\n
        - 0 -- Disables the ACL service\n
        - 1 -- Enables the ACL service
  */
}uim_set_service_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Changes the status of a service on the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_set_service_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_session_type_enum_v01 session_type;
  /**<   Indicates the session type. Only provisioning sessions can be used.
       Valid values: 0 through 3 and 10 through 15.
       See Table \ref{tbl:sessions} for the list of available session types
       with their values and descriptions.
  */

  uint8_t activate;
  /**<   Indicates whether to deactivate or activate the provisioning session.
       Valid values:\n
        - 0 -- Deactivate\n
        - 1 -- Activate
  */
}uim_session_change_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
}uim_application_information_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Changes the provisioning session. */
typedef struct {

  /* Mandatory */
  /*  Session Change */
  uim_session_change_type_v01 session_change;

  /* Optional */
  /*  Application Information */
  uint8_t application_information_valid;  /**< Must be set to true if application_information is being passed */
  uim_application_information_type_v01 application_information;
}uim_change_provisioning_session_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Changes the provisioning session. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_change_provisioning_session_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Gets the label of an application on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Application Information */
  uim_application_information_type_v01 application_information;
}uim_get_label_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Gets the label of an application on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Application Label */
  uint8_t label_valid;  /**< Must be set to true if label is being passed */
  uint32_t label_len;  /**< Must be set to # of elements in label */
  char label[QMI_UIM_LABEL_MAX_V01];
  /**<   Value of the label in ASCII characters.*/
}uim_get_label_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef uint32_t uim_get_configuration_mask_v01;
#define UIM_GET_CONFIGURATION_AUTOMATIC_SELECTION_V01 ((uim_get_configuration_mask_v01)0x01) 
#define UIM_GET_CONFIGURATION_PERSONALIZATION_STATUS_V01 ((uim_get_configuration_mask_v01)0x02) 
#define UIM_GET_CONFIGURATION_HALT_SUBSCRIPTION_V01 ((uim_get_configuration_mask_v01)0x04) 
/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Gets the modem configuration for the UIM module. */
typedef struct {

  /* Optional */
  /*  Configuration Mask */
  uint8_t configuration_mask_valid;  /**< Must be set to true if configuration_mask is being passed */
  uim_get_configuration_mask_v01 configuration_mask;
  /**<   Requested configurations:\n
       - Bit 0  -- Automatic selection\n
       - Bit 1  -- Personalization status\n
       - Bit 2  -- Halt subscription\n
       All other bits are reserved for future use; if the TLV is missing,
       the service returns all configuration items in the response.
  */
}uim_get_configuration_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_perso_feature_enum_v01 feature;
  /**<   Indicates the personalization feature to deactivate or unblock.
       Valid values:\n
        - 0  -- GW network personalization\n
        - 1  -- GW network subset personalization\n
        - 2  -- GW service provider personalization\n
        - 3  -- GW corporate personalization\n
        - 4  -- GW UIM personalization\n
        - 5  -- 1X network type 1 personalization\n
        - 6  -- 1X network type 2 personalization\n
        - 7  -- 1X HRPD personalization\n
        - 8  -- 1X service provider personalization\n
        - 9  -- 1X corporate personalization\n
        - 10 -- 1X RUIM personalization
  */

  uint8_t verify_left;
  /**<   Number of the remaining attempts to verify the personalization feature.*/

  uint8_t unblock_left;
  /**<   Number of the remaining attempts to unblock the personalization feature.*/
}uim_personalization_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Gets the modem configuration for the UIM module. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Automatic Selection */
  uint8_t automatic_selection_valid;  /**< Must be set to true if automatic_selection is being passed */
  uint8_t automatic_selection;
  /**<   Indicates whether the modem is configured to automatically select
       the provisioning sessions at powerup. Valid values:\n
         - 0 -- Automatic provisioning is off\n
         - 1 -- Automatic provisioning is on
  */

  /* Optional */
  /*  Personalization Status */
  uint8_t personalization_status_valid;  /**< Must be set to true if personalization_status is being passed */
  uint32_t personalization_status_len;  /**< Must be set to # of elements in personalization_status */
  uim_personalization_status_type_v01 personalization_status[QMI_UIM_PERSO_FEATURE_MAX_V01];

  /* Optional */
  /*  Halt Subscription Publication */
  uint8_t halt_subscription_valid;  /**< Must be set to true if halt_subscription is being passed */
  uint8_t halt_subscription;
  /**<   Indicates if the modem is configured to publish the subscription
       after successful initialization. Valid values:\n
         - 0 -- Modem proceeds with publishing the subscription\n
         - 1 -- Modem does not publish the subscription
  */
}uim_get_configuration_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_APDU_PROCEDURE_BYTES_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_APDU_RETURN_PROCEDURE_BYTES_V01 = 0x00, 
  UIM_APDU_SKIP_PROCEDURE_BYTES_V01 = 0x01, 
  UIM_APDU_PROCEDURE_BYTES_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_apdu_procedure_bytes_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Sends an APDU to the card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Mandatory */
  /*  APDU */
  uint32_t apdu_len;  /**< Must be set to # of elements in apdu */
  uint8_t apdu[QMI_UIM_APDU_DATA_MAX_V01];
  /**<   Content of the APDU.*/

  /* Optional */
  /*  Logical Channel */
  uint8_t channel_id_valid;  /**< Must be set to true if channel_id is being passed */
  uint8_t channel_id;
  /**<   ID of the logical channel to be used.*/

  /* Optional */
  /*  Intermediate Procedure Bytes */
  uint8_t procedure_bytes_valid;  /**< Must be set to true if procedure_bytes is being passed */
  uim_apdu_procedure_bytes_enum_v01 procedure_bytes;
  /**<   Indicates whether the client is expecting the intermediate procedure
       bytes or the final result from the card. Valid values:\n
        - 0 -- Return intermediate procedure bytes (Default)\n
        - 1 -- Return final result and status words
  */
}uim_send_apdu_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t total_length;
  /**<   Indicates the total length of the response APDU from the card*/

  uint32_t token;
  /**<   Token used in the subsequent indications with APDU response*/
}uim_send_apdu_long_response_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Sends an APDU to the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  APDU Response */
  uint8_t apdu_valid;  /**< Must be set to true if apdu is being passed */
  uint32_t apdu_len;  /**< Must be set to # of elements in apdu */
  uint8_t apdu[QMI_UIM_APDU_DATA_MAX_V01];
  /**<   Content of the application protocol data unit response from the card.*/

  /* Optional */
  /*  Long APDU Response */
  uint8_t long_response_valid;  /**< Must be set to true if long_response is being passed */
  uim_send_apdu_long_response_type_v01 long_response;
  /**<   Token and total length to reconstruct the APDU response from the card.*/
}uim_send_apdu_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t token;
  /**<   Unique token used in the previous QMI_UIM_SEND_APDU response.*/

  uint16_t total_length;
  /**<   Total length of the response APDU from the card.*/

  uint16_t offset;
  /**<   Offset of the current chunk, starting from 0.*/

  uint32_t apdu_len;  /**< Must be set to # of elements in apdu */
  uint8_t apdu[QMI_UIM_APDU_DATA_MAX_V01];
  /**<   Content of the data chunk of the APDU response from the card.*/
}uim_send_apdu_long_response_chunk_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a Send APDU command result. */
typedef struct {

  /* Mandatory */
  /*  Chunk of APDU Response */
  uim_send_apdu_long_response_chunk_type_v01 long_response_chunk;
}uim_send_apdu_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SAP_CONNECT_OP_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SAP_OP_DISCONNECT_V01 = 0x00, 
  UIM_SAP_OP_CONNECT_V01 = 0x01, 
  UIM_SAP_OP_CHECK_STATUS_V01 = 0x02, 
  UIM_SAP_CONNECT_OP_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sap_connect_op_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_sap_connect_op_enum_v01 connect;
  /**<   Indicates the operation to perform. Valid values:\n
        - 0 -- Disconnect\n
        - 1 -- Connect\n
        - 2 -- Check status
  */

  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */
}uim_sap_connect_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SAP_DISCONNECT_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SAP_DISCONNECT_IMMEDIATE_V01 = 0x00, 
  UIM_SAP_DISCONNECT_GRACEFULL_V01 = 0x01, 
  UIM_SAP_DISCONNECT_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sap_disconnect_mode_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SAP_CONNECTION_CONDITION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SAP_CONNECTION_COND_BLOCK_VOICE_OR_DATA_V01 = 0x00, 
  UIM_SAP_CONNECTION_COND_BLOCK_DATA_V01 = 0x01, 
  UIM_SAP_CONNECTION_COND_BLOCK_VOICE_V01 = 0x02, 
  UIM_SAP_CONNECTION_COND_BLOCK_NONE_V01 = 0x03, 
  UIM_SAP_CONNECTION_CONDITION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sap_connection_condition_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Establishes and releases a connection to the UIM module as a SAP
    client. */
typedef struct {

  /* Mandatory */
  /*  SAP Connect */
  uim_sap_connect_type_v01 sap_connect;

  /* Optional */
  /*  Disconnect Mode (This TLV is used only for disconnection and is ignored
      in other cases.)
   */
  uint8_t disconnect_mode_valid;  /**< Must be set to true if disconnect_mode is being passed */
  uim_sap_disconnect_mode_enum_v01 disconnect_mode;
  /**<   Indicates the type of disconnection to perform. Valid values:\n
        - 0 -- Immediate disconnect\n
        - 1 -- Graceful shutdown of the connection
  */

  /* Optional */
  /*  Intermediate GET RESPONSE */
  uint8_t intermediate_get_response_valid;  /**< Must be set to true if intermediate_get_response is being passed */
  uint8_t intermediate_get_response;
  /**<   Indicates whether the client is expecting the intermediate GET RESPONSE
       procedure bytes or the final result from the card. Valid values:\n
        - 0 -- Return final result and status words\n
        - 1 -- Return intermediate GET RESPONSE procedure bytes
  */

  /* Optional */
  /*  Condition to Enable a SAP Connection */
  uint8_t connection_condition_valid;  /**< Must be set to true if connection_condition is being passed */
  uim_sap_connection_condition_enum_v01 connection_condition;
  /**<   Indicates the type of condition to apply during a connection request.
       When this TLV is missing, the default behavior is to block a SAP
       connection when a voice or data call is active. Valid
       values: \begin{itemize1}
       \item 0 -- Block a SAP connection when a voice or data call is active
                  (Default)
       \item 1 -- Block a SAP connection when a data call is active
       \item 2 -- Block a SAP connection when a voice call is active
       \item 3 -- Allow a SAP connection in all cases
       \vspace{-0.18in}
       \end{itemize1}
  */
}uim_sap_connection_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SAP_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SAP_STATE_NOT_ENABLED_V01 = 0x00, 
  UIM_SAP_STATE_CONNECTING_V01 = 0x01, 
  UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01 = 0x02, 
  UIM_SAP_STATE_CONNECTION_ERROR_V01 = 0x03, 
  UIM_SAP_STATE_DISCONNECTING_V01 = 0x04, 
  UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY_V01 = 0x05, 
  UIM_SAP_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sap_status_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Establishes and releases a connection to the UIM module as a SAP
    client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  SAP State (This TLV is returned only when the client requested the SAP
      state.)
   */
  uint8_t sap_state_valid;  /**< Must be set to true if sap_state is being passed */
  uim_sap_status_enum_v01 sap_state;
  /**<   Indicates the SAP connection status. Valid values:\n
        - 0 -- Not enabled\n
        - 1 -- Connecting\n
        - 2 -- Connected successfully\n
        - 3 -- Connection error\n
        - 4 -- Disconnecting\n
        - 5 -- Disconnected successfully
  */
}uim_sap_connection_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SAP_REQUEST_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SAP_REQUEST_GET_ATR_V01 = 0x00, 
  UIM_SAP_REQUEST_SEND_APDU_V01 = 0x01, 
  UIM_SAP_REQUEST_POWER_SIM_OFF_V01 = 0x02, 
  UIM_SAP_REQUEST_POWER_SIM_ON_V01 = 0x03, 
  UIM_SAP_REQUEST_RESET_SIM_V01 = 0x04, 
  UIM_SAP_REQUEST_CARD_READER_STATUS_V01 = 0x05, 
  UIM_SAP_REQUEST_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_sap_request_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_sap_request_enum_v01 sap_request;
  /**<   Indicates the SAP request. Valid values:\n
        - 0 -- Gets the ATR\n
        - 1 -- Sends the APDU\n
        - 2 -- Powers off the SIM\n
        - 3 -- Powers on the SIM\n
        - 4 -- Resets the SIM\n
        - 5 -- Indicates the card reader status
  */

  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */
}uim_sap_request_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Executes a SAP request. */
typedef struct {

  /* Mandatory */
  /*  SAP Request */
  uim_sap_request_type_v01 sap_request;

  /* Optional */
  /*  APDU Data (This TLV must be present for the Send APDU request. For all
      other cases, if present, the TLV is ignored.)
   */
  uint8_t apdu_valid;  /**< Must be set to true if apdu is being passed */
  uint32_t apdu_len;  /**< Must be set to # of elements in apdu */
  uint8_t apdu[QMI_UIM_APDU_DATA_MAX_V01];
  /**<   APDU value.*/
}uim_sap_request_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Executes a SAP request. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  ATR Value (This TLV is present only for the Get ATR request.) */
  uint8_t ATR_value_valid;  /**< Must be set to true if ATR_value is being passed */
  uint32_t ATR_value_len;  /**< Must be set to # of elements in ATR_value */
  uint8_t ATR_value[QMI_UIM_ATR_DATA_MAX_V01];
  /**<   Value of the ATR from the card.*/

  /* Optional */
  /*  APDU Result (This TLV is present only for the Send APDU request.) */
  uint8_t apdu_valid;  /**< Must be set to true if apdu is being passed */
  uint32_t apdu_len;  /**< Must be set to # of elements in apdu */
  uint8_t apdu[QMI_UIM_APDU_DATA_MAX_V01];
  /**<   APDU response.*/

  /* Optional */
  /*  Card Reader Status (This TLV is present only for a card reader status
      request.)
   */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  uint32_t status_len;  /**< Must be set to # of elements in status */
  uint8_t status[QMI_UIM_CARD_READER_DATA_MAX_V01];
  /**<   Value of the card reader status.*/
}uim_sap_request_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_sap_status_enum_v01 sap_state;
  /**<   Indicates the SAP connection status. Valid values:\n
        - 0 -- Not enabled\n
        - 1 -- Connecting\n
        - 2 -- Connected successfully\n
        - 3 -- Connection error\n
        - 4 -- Disconnecting\n
        - 5 -- Disconnected successfully
  */

  uim_slot_enum_v01 slot;
  /**<   Indicates the slot in use. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */
}uim_sap_indication_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates a change in the status of the SAP connection. */
typedef struct {

  /* Optional */
  /*  Card Status */
  uint8_t sap_connection_event_valid;  /**< Must be set to true if sap_connection_event is being passed */
  uim_sap_indication_type_v01 sap_connection_event;
}uim_sap_connection_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_FILE_CONTROL_INFORMATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_FILE_CONTROL_INFORMATION_NO_DATA_V01 = 0x00, 
  UIM_FILE_CONTROL_INFORMATION_FCP_V01 = 0x01, 
  UIM_FILE_CONTROL_INFORMATION_FCI_V01 = 0x02, 
  UIM_FILE_CONTROL_INFORMATION_FCI_WITH_INTERFACES_V01 = 0x03, 
  UIM_FILE_CONTROL_INFORMATION_FMD_V01 = 0x04, 
  UIM_FILE_CONTROL_INFORMATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_file_control_information_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Opens and closes a logical channel to an application on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Optional */
  /*  Open the Logical Channel */
  uint8_t aid_valid;  /**< Must be set to true if aid is being passed */
  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value.*/

  /* Optional */
  /*  Close the Logical Channel */
  uint8_t channel_id_valid;  /**< Must be set to true if channel_id is being passed */
  uint8_t channel_id;
  /**<   ID of the logical channel to be closed.*/

  /* Optional */
  /*  Template Requested from the Card in SELECT */
  uint8_t file_control_information_valid;  /**< Must be set to true if file_control_information is being passed */
  uim_file_control_information_enum_v01 file_control_information;
  /**<   Template requested from the card in the SELECT command when the
       application is selected. When this TLV is missing, FCP is requested
       by default. Valid values:\n
        - 0 -- No data\n
        - 1 -- FCP\n
        - 2 -- FCI\n
        - 3 -- FCI with interfaces\n
        - 4 -- FMD
  */

  /* Optional */
  /*  Termination of the Application */
  uint8_t terminate_application_valid;  /**< Must be set to true if terminate_application is being passed */
  uint8_t terminate_application;
  /**<   Indicates whether the application is terminated with a SELECT request
       before closing the associated logical channel. When the Termination of
       the Application TLV is missing, the default behavior is TRUE to
       terminate the application.
  */
}uim_logical_channel_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Opens and closes a logical channel to an application on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Channel ID */
  uint8_t channel_id_valid;  /**< Must be set to true if channel_id is being passed */
  uint8_t channel_id;
  /**<   ID of the logical channel that was opened.*/

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Response to SELECT */
  uint8_t select_response_valid;  /**< Must be set to true if select_response is being passed */
  uint32_t select_response_len;  /**< Must be set to # of elements in select_response */
  uint8_t select_response[QMI_UIM_SELECT_RESPONSE_MAX_V01];
  /**<   Raw value of the response sent by the card to the initial SELECT
       command.
  */
}uim_logical_channel_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Enables the client to indicate whether it is OK to
    proceed with publishing the subscription. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  OK for Subscription */
  uint8_t ok_for_subscription;
  /**<   Indicates whether it is OK to publish the subscription. Valid values:\n
         - 0 -- Not OK to publish the subscription\n
         - 1 -- OK to publish the subscription
  */

  /* Optional */
  /*  Encrypted IMSI */
  uint8_t encrypted_imsi_valid;  /**< Must be set to true if encrypted_imsi is being passed */
  uint32_t encrypted_imsi_len;  /**< Must be set to # of elements in encrypted_imsi */
  uint8_t encrypted_imsi[QMI_UIM_ENCRYPTED_IMSI_MAX_V01];
  /**<   Encrypted value of the IMSI of the session. The decrypted value is checked
       against the IMSI of the card, and the command is executed only if it matches.
  */
}uim_subscription_ok_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Enables the client to indicate whether it is OK to
    proceed with publishing the subscription. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_subscription_ok_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Retrieves the Answer to Reset of a specific card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */
}uim_get_atr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves the Answer to Reset of a specific card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  ATR Value */
  uint8_t atr_value_valid;  /**< Must be set to true if atr_value is being passed */
  uint32_t atr_value_len;  /**< Must be set to # of elements in atr_value */
  uint8_t atr_value[QMI_UIM_ATR_MAX_V01];
  /**<   Raw value of the ATR sent by the card during the initialization.*/
}uim_get_atr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Opens a logical channel on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Optional */
  /*  AID */
  uint8_t aid_valid;  /**< Must be set to true if aid is being passed */
  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value.*/

  /* Optional */
  /*  Template Requested from the Card in SELECT */
  uint8_t file_control_information_valid;  /**< Must be set to true if file_control_information is being passed */
  uim_file_control_information_enum_v01 file_control_information;
  /**<   Template requested from the card in the SELECT command when the
       application is selected. When this TLV is missing, FCP is requested
       by default. Valid values:\n
        - 0 -- No data\n
        - 1 -- FCP\n
        - 2 -- FCI\n
        - 3 -- FCI with interfaces\n
        - 4 -- FMD
  */
}uim_open_logical_channel_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Opens a logical channel on a UICC card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Channel ID */
  uint8_t channel_id_valid;  /**< Must be set to true if channel_id is being passed */
  uint8_t channel_id;
  /**<   ID of the logical channel that was opened.*/

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Response to SELECT */
  uint8_t select_response_valid;  /**< Must be set to true if select_response is being passed */
  uint32_t select_response_len;  /**< Must be set to # of elements in select_response */
  uint8_t select_response[QMI_UIM_SELECT_RESPONSE_MAX_V01];
  /**<   Raw value of the response sent by the card to the initial SELECT
       command.
  */
}uim_open_logical_channel_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_SESSION_CLOSE_CAUSE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_SESSION_CLOSED_CAUSE_UNKNOWN_V01 = 0x00, /**<  Unknown cause.  */
  UIM_SESSION_CLOSED_CAUSE_CLIENT_REQUEST_V01 = 0x01, /**<  Client requested to deactivate the session.  */
  UIM_SESSION_CLOSED_CAUSE_CARD_ERROR_V01 = 0x02, /**<  Card error.  */
  UIM_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN_V01 = 0x03, /**<  SIM card is powered down.  */
  UIM_SESSION_CLOSED_CAUSE_CARD_REMOVED_V01 = 0x04, /**<  SIM card is removed.  */
  UIM_SESSION_CLOSED_CAUSE_REFRESH_V01 = 0x05, /**<  Session is deactivated as a part of a Refresh procedure.  */
  UIM_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL_V01 = 0x06, /**<  Status of the PIN cannot be retrieved from the SIM.  */
  UIM_SESSION_CLOSED_CAUSE_RECOVERY_V01 = 0x07, /**<  Internal card recovery.  */
  UIM_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT_V01 = 0x08, /**<  FDN service is enabled on the SIM, but the terminal does not support
       it.  */
  UIM_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL_V01 = 0x09, /**<  Personalzation failure.  */
  UIM_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_V01 = 0x0A, /**<  Content of the file is invalid.  */
  UIM_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_V01 = 0x0B, /**<  Mandatory file is missing.  */
  UIM_SESSION_CLOSE_CAUSE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_session_close_cause_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates either a provisioning or a nonprovisioning session is
             closed by the modem. */
typedef struct {

  /* Mandatory */
  /*  Slot */
  uim_slot_enum_v01 slot;
  /**<   Indicates the slot to be used. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Optional */
  /*  Application ID */
  uint8_t aid_valid;  /**< Must be set to true if aid is being passed */
  uint32_t aid_len;  /**< Must be set to # of elements in aid */
  uint8_t aid[QMI_UIM_AID_MAX_V01];
  /**<   Application identifier value.*/

  /* Optional */
  /*  Logical Channel */
  uint8_t channel_id_valid;  /**< Must be set to true if channel_id is being passed */
  uint8_t channel_id;
  /**<   ID of the logical channel being closed.*/

  /* Optional */
  /*  Provisioning Session */
  uint8_t provisioning_session_valid;  /**< Must be set to true if provisioning_session is being passed */
  uim_session_type_enum_v01 provisioning_session;
  /**<   Indicates the provisioning session type that is closed.
       Valid values: 0 to 3 and 10 to 15. See Table \ref{tbl:sessions} for
       the list of available session types with their values and descriptions.
  */

  /* Optional */
  /*  Session Closure Cause */
  uint8_t cause_valid;  /**< Must be set to true if cause is being passed */
  uim_session_close_cause_enum_v01 cause;
  /**<   Indicates the cause of a session closure. See
       Table \ref{tbl:sessionClosures} for the causes of session closures with
       their values and descriptions.

       Other values are reserved for the future use and are to be handled
       as an unknown cause by the client.
  */

  /* Optional */
  /*  File ID */
  uint8_t file_id_valid;  /**< Must be set to true if file_id is being passed */
  uint16_t file_id;
  /**<   Indicates the file ID of the mandatory file that is missing, or is
       present but contains invalid content. */
}uim_session_closed_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Registers for file change notifications triggered by the card for
             all files. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  Register for Refresh */
  uint8_t register_for_refresh;
  /**<   Indicates whether to register or deregister for refresh indications.
       Valid values:\n
        - 0 -- Deregister\n
        - 1 -- Register
  */

  /* Optional */
  /*  Vote for Initialization */
  uint8_t vote_for_init_valid;  /**< Must be set to true if vote_for_init is being passed */
  uint8_t vote_for_init;
  /**<   Indicates whether to vote for initialization when there is a
       refresh. When the Vote for Initialization TLV is missing, the default
       behavior is to not wait for the client vote. Valid values:\n
         - 0 -- Client does not vote for initialization (Default)\n
         - 1 -- Client votes for initialization
  */
}uim_refresh_register_all_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Registers for file change notifications triggered by the card for
             all files. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_refresh_register_all_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Changes the activation status of the file on the card. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  File Status */
  uim_file_status_type_v01 file_status;
  /**<   Activates or deactivates the file. Valid values:\n
      - UIM_FILE_STATUS_DEACTIVATED (0x00) --  File is deactivated. 
      - UIM_FILE_STATUS_ACTIVATED (0x01) --  File is activated. 
 */
}uim_set_file_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Changes the activation status of the file on the card. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_set_file_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PHYSICAL_SLOT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PHY_SLOT_1_V01 = 0x01, /**<  Slot 1\n  */
  UIM_PHY_SLOT_2_V01 = 0x02, /**<  Slot 2\n  */
  UIM_PHY_SLOT_3_V01 = 0x03, /**<  Slot 3\n  */
  UIM_PHY_SLOT_4_V01 = 0x04, /**<  Slot 4\n  */
  UIM_PHY_SLOT_5_V01 = 0x05, /**<  Slot 5  */
  UIM_PHYSICAL_SLOT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_physical_slot_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Switches the binding between a logical slot and a physical slot. */
typedef struct {

  /* Mandatory */
  /*  Logical Slot */
  uim_slot_enum_v01 logical_slot;
  /**<   Logical slot that is switched to the physical slot. Valid values:\n
        - 1 -- Slot 1\n
        - 2 -- Slot 2\n
        - 3 -- Slot 3\n
        - 4 -- Slot 4\n
        - 5 -- Slot 5
  */

  /* Mandatory */
  /*  Physical Slot */
  uim_physical_slot_enum_v01 physical_slot;
  /**<   Physical slot that the logical slot is to be switched to. Valid values:\n
      - UIM_PHY_SLOT_1 (0x01) --  Slot 1\n 
      - UIM_PHY_SLOT_2 (0x02) --  Slot 2\n 
      - UIM_PHY_SLOT_3 (0x03) --  Slot 3\n 
      - UIM_PHY_SLOT_4 (0x04) --  Slot 4\n 
      - UIM_PHY_SLOT_5 (0x05) --  Slot 5 
 */
}uim_switch_slot_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Switches the binding between a logical slot and a physical slot. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */
}uim_switch_slot_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}uim_get_slots_status_req_msg_v01;

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PHYSICAL_SLOT_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PHYSICAL_SLOT_STATE_INACTIVE_V01 = 0x00, /**<  Inactive\n  */
  UIM_PHYSICAL_SLOT_STATE_ACTIVE_V01 = 0x01, /**<  Active  */
  UIM_PHYSICAL_SLOT_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_physical_slot_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PHYSICAL_CARD_STATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PHYSICAL_CARD_STATE_UNKNOWN_V01 = 0x00, /**<  Unknown  */
  UIM_PHYSICAL_CARD_STATE_ABSENT_V01 = 0x01, /**<  Absent  */
  UIM_PHYSICAL_CARD_STATE_PRESENT_V01 = 0x02, /**<  Present  */
  UIM_PHYSICAL_CARD_STATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_physical_card_state_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_physical_card_state_enum_v01 physical_card_status;
  /**<   Status of the card in the physical slot. Valid values:\n
      - UIM_PHYSICAL_CARD_STATE_UNKNOWN (0x00) --  Unknown 
      - UIM_PHYSICAL_CARD_STATE_ABSENT (0x01) --  Absent 
      - UIM_PHYSICAL_CARD_STATE_PRESENT (0x02) --  Present 
 */

  uim_physical_slot_state_enum_v01 physical_slot_state;
  /**<   State of the physical slot. Valid values:\n
      - UIM_PHYSICAL_SLOT_STATE_INACTIVE (0x00) --  Inactive\n 
      - UIM_PHYSICAL_SLOT_STATE_ACTIVE (0x01) --  Active 
 */

  uim_slot_enum_v01 logical_slot;
  /**<   Logical slot associated with this physical slot. This is 
       valid only if the physical slot is active. Valid values:\n
       - 1 -- Slot 1\n
       - 2 -- Slot 2\n
       - 3 -- Slot 3\n
       - 4 -- Slot 4\n
       - 5 -- Slot 5
  */

  uint32_t iccid_len;  /**< Must be set to # of elements in iccid */
  uint8_t iccid[QMI_UIM_ICCID_LEN_MAX_V01];
  /**<   Contains the ICCID of the card in the physical slot.*/
}uim_physical_slot_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves the current status of the physical and logical slots. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Physical Slot Status */
  uint8_t physical_slot_status_valid;  /**< Must be set to true if physical_slot_status is being passed */
  uint32_t physical_slot_status_len;  /**< Must be set to # of elements in physical_slot_status */
  uim_physical_slot_status_type_v01 physical_slot_status[QMI_UIM_PHYSICAL_SLOTS_MAX_V01];
  /**<   \n
       Information regarding the various physical slots on the device.\n
       The number of elements correspond with the number of physical slots 
       on the device.
  */
}uim_get_slots_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates that the status of the physical slot has changed. */
typedef struct {

  /* Optional */
  /*  Physical Slot Status */
  uint8_t physical_slot_status_valid;  /**< Must be set to true if physical_slot_status is being passed */
  uint32_t physical_slot_status_len;  /**< Must be set to # of elements in physical_slot_status */
  uim_physical_slot_status_type_v01 physical_slot_status[QMI_UIM_PHYSICAL_SLOTS_MAX_V01];
}uim_slot_status_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t long_data_token;
  /**<   Unique long data token used in the previous QMI_UIM_READ_TRANSPARENT
       response or indication.
  */

  uint16_t total_length;
  /**<   Total length of the read data. */

  uint16_t offset;
  /**<   Offset of the current chunk (starting from 0). */

  uim_read_transparent_result_type_v01 read_result;
  /**<   Content of the read data chunk. */
}uim_read_transparent_long_response_chunk_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indication with a read transparent command result. */
typedef struct {

  /* Mandatory */
  /*  Chunk of Read Transparent Data */
  uim_read_transparent_long_response_chunk_type_v01 long_response_chunk;

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Encrypted Data */
  uint8_t encryption_valid;  /**< Must be set to true if encryption is being passed */
  uint8_t encryption;
  /**<   Indicates whether the data from the card passed in read_result is
       encrypted. Valid values:\n
         - 0 -- Data in read_result is unencrypted \n
         - 1 -- Data in read_result is encrypted \n
       If the Encrypted Data TLV is missing, the read_result data is in
       unencrypted format.
  */
}uim_read_transparent_long_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates the busy status of a SIM card. */
typedef struct {

  /* Optional */
  /*  SIM Busy Status */
  uint8_t sim_busy_status_valid;  /**< Must be set to true if sim_busy_status is being passed */
  uint32_t sim_busy_status_len;  /**< Must be set to # of elements in sim_busy_status */
  uint8_t sim_busy_status[QMI_UIM_EXTENDED_CARDS_MAX_V01];
  /**<   Indicates whether the card on the corresponding slot is busy. Valid
       values:\n
        - 0 -- SIM card is not busy \n
        - 1 -- SIM card is busy
  */
}uim_sim_busy_status_ind_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}uim_get_plmn_name_table_info_req_msg_v01;

/** @addtogroup uim_qmi_enums
    @{
  */
typedef enum {
  UIM_PLMN_NAME_TABLE_SOURCE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  UIM_PLMN_NAME_TABLE_SOURCE_HARDCODED_V01 = 0x01, /**<  Table is hardcoded in the modem source code  */
  UIM_PLMN_NAME_TABLE_SOURCE_EFS_V01 = 0x02, /**<  Table is fetched from the modem file system  */
  UIM_PLMN_NAME_TABLE_SOURCE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}uim_plmn_name_table_source_enum_v01;
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t month;
  /**<   Month */

  uint8_t day;
  /**<   Day of the month */

  uint16_t year;
  /**<   Year */
}uim_plmn_name_release_date_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Retrieves information about the SE.13 table used by the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Release Date */
  uint8_t release_date_valid;  /**< Must be set to true if release_date is being passed */
  uim_plmn_name_release_date_info_type_v01 release_date;
  /**<   \n
       Information regarding the release date of the SE.13 table database
       used by the modem.
  */

  /* Optional */
  /*  Table Source */
  uint8_t table_source_valid;  /**< Must be set to true if table_source is being passed */
  uim_plmn_name_table_source_enum_v01 table_source;
  /**<   Information regarding the source of the used SE.13 table database.\n
 Valid values:\n
      - UIM_PLMN_NAME_TABLE_SOURCE_HARDCODED (0x01) --  Table is hardcoded in the modem source code 
      - UIM_PLMN_NAME_TABLE_SOURCE_EFS (0x02) --  Table is fetched from the modem file system 
 \n
 Other values are reserved for future use and are to be ignored by
 the client.
 */
}uim_get_plmn_name_table_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  char mcc[QMI_UIM_MCC_LEN_V01];
  /**<   MCC value in ASCII characters.*/

  uint32_t mnc_len;  /**< Must be set to # of elements in mnc */
  char mnc[QMI_UIM_MNC_MAX_V01];
  /**<   MNC value in ASCII characters.*/
}uim_network_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_network_perso_type_v01 network;
  /**<   MCC and MNC network information.*/

  char digit6;
  /**<   Digit 6 of the IMSI in ASCII character.*/

  char digit7;
  /**<   Digit 7 of the IMSI in ASCII character.*/
}uim_gw_network_subset_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_network_perso_type_v01 network;
  /**<   MCC and MNC network information.*/

  uint8_t gid1;
  /**<   Service provider code found in GID1.*/
}uim_gw_sp_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_network_perso_type_v01 network;
  /**<   MCC and MNC network information.*/

  uint8_t gid1;
  /**<   Service provider code found in GID1.*/

  uint8_t gid2;
  /**<   Corporate customer code found in GID2.*/
}uim_gw_corporate_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  uim_network_perso_type_v01 network;
  /**<   MCC and MNC network information.*/

  uint32_t msin_len;  /**< Must be set to # of elements in msin */
  char msin[QMI_UIM_MSIN_MAX_V01];
  /**<   MSIN value stored on the card in ASCII characters.*/
}uim_sim_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_aggregates
    @{
  */
typedef struct {

  char irm_code[QMI_UIM_IRM_CODE_LEN_V01];
  /**<   First 4 digits of the IRM based MIN of IMSI_M in ASCII characters.*/
}uim_1x_network_type2_perso_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Activates and sets the personalization data on the phone */
typedef struct {

  /* Mandatory */
  /*  Control Key */
  uint32_t ck_value_len;  /**< Must be set to # of elements in ck_value */
  char ck_value[QMI_UIM_CK_MAX_V01];
  /**<   Control key value. This value is a sequence of ASCII characters. 
  */

  /* Optional */
  /*  GW Network Personalization */
  uint8_t feature_gw_network_perso_valid;  /**< Must be set to true if feature_gw_network_perso is being passed */
  uint32_t feature_gw_network_perso_len;  /**< Must be set to # of elements in feature_gw_network_perso */
  uim_network_perso_type_v01 feature_gw_network_perso[QMI_UIM_PERSO_NUM_NW_MAX_V01];

  /* Optional */
  /*  GW Network Subset Personalization */
  uint8_t feature_gw_network_subset_perso_valid;  /**< Must be set to true if feature_gw_network_subset_perso is being passed */
  uint32_t feature_gw_network_subset_perso_len;  /**< Must be set to # of elements in feature_gw_network_subset_perso */
  uim_gw_network_subset_perso_type_v01 feature_gw_network_subset_perso[QMI_UIM_PERSO_NUM_NS_MAX_V01];

  /* Optional */
  /*  GW Service Provider Personalization */
  uint8_t feature_gw_sp_perso_valid;  /**< Must be set to true if feature_gw_sp_perso is being passed */
  uint32_t feature_gw_sp_perso_len;  /**< Must be set to # of elements in feature_gw_sp_perso */
  uim_gw_sp_perso_type_v01 feature_gw_sp_perso[QMI_UIM_PERSO_NUM_GW_SP_MAX_V01];

  /* Optional */
  /*  GW Corporate Personalization */
  uint8_t feature_gw_corporate_perso_valid;  /**< Must be set to true if feature_gw_corporate_perso is being passed */
  uint32_t feature_gw_corporate_perso_len;  /**< Must be set to # of elements in feature_gw_corporate_perso */
  uim_gw_corporate_perso_type_v01 feature_gw_corporate_perso[QMI_UIM_PERSO_NUM_GW_CP_MAX_V01];

  /* Optional */
  /*  GW SIM Personalization */
  uint8_t feature_gw_sim_perso_valid;  /**< Must be set to true if feature_gw_sim_perso is being passed */
  uint32_t feature_gw_sim_perso_len;  /**< Must be set to # of elements in feature_gw_sim_perso */
  uim_sim_perso_type_v01 feature_gw_sim_perso[QMI_UIM_PERSO_NUM_SIM_MAX_V01];

  /* Optional */
  /*  1X Network Type 1 Personalization */
  uint8_t feature_1x_network1_perso_valid;  /**< Must be set to true if feature_1x_network1_perso is being passed */
  uint32_t feature_1x_network1_perso_len;  /**< Must be set to # of elements in feature_1x_network1_perso */
  uim_network_perso_type_v01 feature_1x_network1_perso[QMI_UIM_PERSO_NUM_NW_MAX_V01];

  /* Optional */
  /*  1X Network type 2 Personalization */
  uint8_t feature_1x_network2_perso_valid;  /**< Must be set to true if feature_1x_network2_perso is being passed */
  uint32_t feature_1x_network2_perso_len;  /**< Must be set to # of elements in feature_1x_network2_perso */
  uim_1x_network_type2_perso_type_v01 feature_1x_network2_perso[QMI_UIM_PERSO_NUM_1X_NW2_MAX_V01];

  /* Optional */
  /*  1X RUIM Personalization */
  uint8_t feature_1x_ruim_perso_valid;  /**< Must be set to true if feature_1x_ruim_perso is being passed */
  uint32_t feature_1x_ruim_perso_len;  /**< Must be set to # of elements in feature_1x_ruim_perso */
  uim_sim_perso_type_v01 feature_1x_ruim_perso[QMI_UIM_PERSO_NUM_SIM_MAX_V01];
}uim_personalization_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Activates and sets the personalization data on the phone */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Retries Remaining (This value is returned only when activation and
      setting perso data fails.)
  
 Retries Remaining */
  uint8_t retries_left_valid;  /**< Must be set to true if retries_left is being passed */
  uim_perso_retries_left_type_v01 retries_left;
}uim_personalization_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Request Message; Perform increase operation for any file on the card and provides 
             access by the path. */
typedef struct {

  /* Mandatory */
  /*  Session Information */
  uim_session_information_type_v01 session_information;

  /* Mandatory */
  /*  File ID */
  uim_file_id_type_v01 file_id;

  /* Mandatory */
  /*  Value to be Added */
  uint32_t increase_data_len;  /**< Must be set to # of elements in increase_data */
  uint8_t increase_data[QMI_UIM_INCREASE_DATA_MAX_V01];

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result must be provided
       in a subsequent indication.
  */
}uim_increase_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Response Message; Perform increase operation for any file on the card and provides 
             access by the path. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Value of the Increase Record */
  uint8_t increase_result_valid;  /**< Must be set to true if increase_result is being passed */
  uint32_t increase_result_len;  /**< Must be set to # of elements in increase_result */
  uint8_t increase_result[QMI_UIM_INCREASE_DATA_MAX_V01];

  /* Optional */
  /*  Response in Indication */
  uint8_t indication_token_valid;  /**< Must be set to true if indication_token is being passed */
  uint32_t indication_token;
  /**<   When this TLV is present, it indicates that the result is provided
       in a subsequent indication.
  */
}uim_increase_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uim_qmi_messages
    @{
  */
/** Indication Message; Indicates clients with increase confirmation */
typedef struct {

  /* Mandatory */
  /*  Token of the Original Request */
  uint32_t indication_token;
  /**<   Indicates the token of the original request.
  */

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:\n
       qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE.\n
       qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message
                          definition.
  */

  /* Optional */
  /*  Card Result */
  uint8_t card_result_valid;  /**< Must be set to true if card_result is being passed */
  uim_card_result_type_v01 card_result;

  /* Optional */
  /*  Value of the Increase Record */
  uint8_t increase_result_valid;  /**< Must be set to true if increase_result is being passed */
  uint32_t increase_result_len;  /**< Must be set to # of elements in increase_result */
  uint8_t increase_result[QMI_UIM_INCREASE_DATA_MAX_V01];
}uim_increase_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup uim_qmi_msg_ids
    @{
  */
#define QMI_UIM_RESET_REQ_V01 0x0000
#define QMI_UIM_RESET_RESP_V01 0x0000
#define QMI_UIM_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_UIM_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_UIM_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_UIM_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_UIM_READ_TRANSPARENT_REQ_V01 0x0020
#define QMI_UIM_READ_TRANSPARENT_RESP_V01 0x0020
#define QMI_UIM_READ_TRANSPARENT_IND_V01 0x0020
#define QMI_UIM_READ_RECORD_REQ_V01 0x0021
#define QMI_UIM_READ_RECORD_RESP_V01 0x0021
#define QMI_UIM_READ_RECORD_IND_V01 0x0021
#define QMI_UIM_WRITE_TRANSPARENT_REQ_V01 0x0022
#define QMI_UIM_WRITE_TRANSPARENT_RESP_V01 0x0022
#define QMI_UIM_WRITE_TRANSPARENT_IND_V01 0x0022
#define QMI_UIM_WRITE_RECORD_REQ_V01 0x0023
#define QMI_UIM_WRITE_RECORD_RESP_V01 0x0023
#define QMI_UIM_WRITE_RECORD_IND_V01 0x0023
#define QMI_UIM_GET_FILE_ATTRIBUTES_REQ_V01 0x0024
#define QMI_UIM_GET_FILE_ATTRIBUTES_RESP_V01 0x0024
#define QMI_UIM_GET_FILE_ATTRIBUTES_IND_V01 0x0024
#define QMI_UIM_SET_PIN_PROTECTION_REQ_V01 0x0025
#define QMI_UIM_SET_PIN_PROTECTION_RESP_V01 0x0025
#define QMI_UIM_SET_PIN_PROTECTION_IND_V01 0x0025
#define QMI_UIM_VERIFY_PIN_REQ_V01 0x0026
#define QMI_UIM_VERIFY_PIN_RESP_V01 0x0026
#define QMI_UIM_VERIFY_PIN_IND_V01 0x0026
#define QMI_UIM_UNBLOCK_PIN_REQ_V01 0x0027
#define QMI_UIM_UNBLOCK_PIN_RESP_V01 0x0027
#define QMI_UIM_UNBLOCK_PIN_IND_V01 0x0027
#define QMI_UIM_CHANGE_PIN_REQ_V01 0x0028
#define QMI_UIM_CHANGE_PIN_RESP_V01 0x0028
#define QMI_UIM_CHANGE_PIN_IND_V01 0x0028
#define QMI_UIM_DEPERSONALIZATION_REQ_V01 0x0029
#define QMI_UIM_DEPERSONALIZATION_RESP_V01 0x0029
#define QMI_UIM_REFRESH_REGISTER_REQ_V01 0x002A
#define QMI_UIM_REFRESH_REGISTER_RESP_V01 0x002A
#define QMI_UIM_REFRESH_OK_REQ_V01 0x002B
#define QMI_UIM_REFRESH_OK_RESP_V01 0x002B
#define QMI_UIM_REFRESH_COMPLETE_REQ_V01 0x002C
#define QMI_UIM_REFRESH_COMPLETE_RESP_V01 0x002C
#define QMI_UIM_REFRESH_GET_LAST_EVENT_REQ_V01 0x002D
#define QMI_UIM_REFRESH_GET_LAST_EVENT_RESP_V01 0x002D
#define QMI_UIM_EVENT_REG_REQ_V01 0x002E
#define QMI_UIM_EVENT_REG_RESP_V01 0x002E
#define QMI_UIM_GET_CARD_STATUS_REQ_V01 0x002F
#define QMI_UIM_GET_CARD_STATUS_RESP_V01 0x002F
#define QMI_UIM_POWER_DOWN_REQ_V01 0x0030
#define QMI_UIM_POWER_DOWN_RESP_V01 0x0030
#define QMI_UIM_POWER_UP_REQ_V01 0x0031
#define QMI_UIM_POWER_UP_RESP_V01 0x0031
#define QMI_UIM_STATUS_CHANGE_IND_V01 0x0032
#define QMI_UIM_REFRESH_IND_V01 0x0033
#define QMI_UIM_AUTHENTICATE_REQ_V01 0x0034
#define QMI_UIM_AUTHENTICATE_RESP_V01 0x0034
#define QMI_UIM_AUTHENTICATE_IND_V01 0x0034
#define QMI_UIM_CLOSE_SESSION_REQ_V01 0x0035
#define QMI_UIM_CLOSE_SESSION_RESP_V01 0x0035
#define QMI_UIM_GET_SERVICE_STATUS_REQ_V01 0x0036
#define QMI_UIM_GET_SERVICE_STATUS_RESP_V01 0x0036
#define QMI_UIM_SET_SERVICE_STATUS_REQ_V01 0x0037
#define QMI_UIM_SET_SERVICE_STATUS_RESP_V01 0x0037
#define QMI_UIM_CHANGE_PROVISIONING_SESSION_REQ_V01 0x0038
#define QMI_UIM_CHANGE_PROVISIONING_SESSION_RESP_V01 0x0038
#define QMI_UIM_GET_LABEL_REQ_V01 0x0039
#define QMI_UIM_GET_LABEL_RESP_V01 0x0039
#define QMI_UIM_GET_CONFIGURATION_REQ_V01 0x003A
#define QMI_UIM_GET_CONFIGURATION_RESP_V01 0x003A
#define QMI_UIM_SEND_APDU_REQ_V01 0x003B
#define QMI_UIM_SEND_APDU_RESP_V01 0x003B
#define QMI_UIM_SEND_APDU_IND_V01 0x003B
#define QMI_UIM_SAP_CONNECTION_REQ_V01 0x003C
#define QMI_UIM_SAP_CONNECTION_RESP_V01 0x003C
#define QMI_UIM_SAP_REQUEST_REQ_V01 0x003D
#define QMI_UIM_SAP_REQUEST_RESP_V01 0x003D
#define QMI_UIM_SAP_CONNECTION_IND_V01 0x003E
#define QMI_UIM_LOGICAL_CHANNEL_REQ_V01 0x003F
#define QMI_UIM_LOGICAL_CHANNEL_RESP_V01 0x003F
#define QMI_UIM_SUBSCRIPTION_OK_REQ_V01 0x0040
#define QMI_UIM_SUBSCRIPTION_OK_RESP_V01 0x0040
#define QMI_UIM_GET_ATR_REQ_V01 0x0041
#define QMI_UIM_GET_ATR_RESP_V01 0x0041
#define QMI_UIM_OPEN_LOGICAL_CHANNEL_REQ_V01 0x0042
#define QMI_UIM_OPEN_LOGICAL_CHANNEL_RESP_V01 0x0042
#define QMI_UIM_SESSION_CLOSED_IND_V01 0x0043
#define QMI_UIM_REFRESH_REGISTER_ALL_REQ_V01 0x0044
#define QMI_UIM_REFRESH_REGISTER_ALL_RESP_V01 0x0044
#define QMI_UIM_SET_FILE_STATUS_REQ_V01 0x0045
#define QMI_UIM_SET_FILE_STATUS_RESP_V01 0x0045
#define QMI_UIM_SWITCH_SLOT_REQ_V01 0x0046
#define QMI_UIM_SWITCH_SLOT_RESP_V01 0x0046
#define QMI_UIM_GET_SLOTS_STATUS_REQ_V01 0x0047
#define QMI_UIM_GET_SLOTS_STATUS_RESP_V01 0x0047
#define QMI_UIM_SLOT_STATUS_CHANGE_IND_V01 0x0048
#define QMI_UIM_READ_TRANSPARENT_LONG_IND_V01 0x0049
#define QMI_UIM_SIM_BUSY_STATUS_IND_V01 0x004A
#define QMI_UIM_GET_PLMN_NAME_TABLE_INFO_REQ_V01 0x004B
#define QMI_UIM_GET_PLMN_NAME_TABLE_INFO_RESP_V01 0x004B
#define QMI_UIM_PERSONALIZATION_REQ_V01 0x004C
#define QMI_UIM_PERSONALIZATION_RESP_V01 0x004C
#define QMI_UIM_INCREASE_REQ_V01 0x004D
#define QMI_UIM_INCREASE_RESP_V01 0x004D
#define QMI_UIM_INCREASE_IND_V01 0x004D
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro uim_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type uim_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define uim_get_service_object_v01( ) \
          uim_get_service_object_internal_v01( \
            UIM_V01_IDL_MAJOR_VERS, UIM_V01_IDL_MINOR_VERS, \
            UIM_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

