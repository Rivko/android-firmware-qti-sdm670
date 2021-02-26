#ifndef BLM_SERVICE_01_H
#define BLM_SERVICE_01_H
/**
  @file bandwidth_limits_management_v01.h

  @brief This is the public header file which defines the blm service Data structures.

  This header file defines the types and structures that were defined in
  blm. It contains the constant values defined, enums, structures,
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
  
  Copyright (c) 2015 Qualcomm Technologies, Inc.  
  All Rights Reserved
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //source/qcom/qct/interfaces/qmi/blm/main/latest/api/bandwidth_limits_management_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.6 
   It was generated on: Thu Aug  6 2015 (Spin 1)
   From IDL File: bandwidth_limits_management_v01.idl */

/** @defgroup blm_qmi_consts Constant values defined in the IDL */
/** @defgroup blm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup blm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup blm_qmi_messages Structures sent as QMI messages */
/** @defgroup blm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup blm_qmi_accessor Accessor for QMI service object */
/** @defgroup blm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup blm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define BLM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define BLM_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define BLM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define BLM_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup blm_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup blm_qmi_enums
    @{
  */
typedef enum {
  BANDWIDTH_LEVEL_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  LEVEL_0_V01 = 0, 
  LEVEL_1_V01 = 1, 
  LEVEL_2_V01 = 2, 
  BANDWIDTH_LEVEL_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}bandwidth_level_type_v01;
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Request Message; This command or message group allows client to send request to service
	wih bandwidth thresholds to monitor and mitigation level to apply */
typedef struct {

  /* Mandatory */
  /*  Average Bandwidth Low Threshold */
  uint64_t ab_low_threshold;
  /**<   Average bandwidth low threshold in MBytes\second
  */

  /* Mandatory */
  /*  Average Bandwidth High Threshold */
  uint64_t ab_high_threshold;
  /**<   Average bandwidth high threshold in bytes\second
  */

  /* Mandatory */
  /*  Instantaneous Bandwidth Low Threshold */
  uint64_t ib_low_threshod;
  /**<   Instantaneous bandwidth low threshold in bytes\second
  */

  /* Mandatory */
  /*  Instantaneous Bandwidth High Threshold */
  uint64_t ib_high_threshold;
  /**<   Instantaneous bandwidth high threshold in bytes\second
  */

  /* Mandatory */
  /*  Mitigation Level */
  uint32_t mitigation_level;
  /**<   Mitigation level to set. No mitigation is defined as zero. 
       Each increment is predefined to have increasing mitigation
       effect. 

       Client can get max possible mitigation levels on the service
       by using  QMI_BLM_MAX_MITIGATION_LEVEL message. If client 
       requested mitigation level is higher than the max supported 
       level, then it would be capped to max supported mitigation level.
  */
}blm_monitor_bw_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Response Message; This command or message group allows client to send request to service
	wih bandwidth thresholds to monitor and mitigation level to apply */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}blm_monitor_bw_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Request Message; Message to register for the required indications from the service */
typedef struct {

  /* Optional */
  /*  Enable Bandwidth Usage Level */
  uint8_t enable_bw_usage_level_valid;  /**< Must be set to true if enable_bw_usage_level is being passed */
  uint8_t enable_bw_usage_level;
  /**<   Control flag to enable or disable the BW usage level indications.
       TRUE  - means register for BW usage level indications
       FALSE - means de-register from BW usage level indications
  */
}blm_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Response Message; Message to register for the required indications from the service */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}blm_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Indication Message; This command or message group allows server to indicate the bandwidth 
    usage level */
typedef struct {

  /* Mandatory */
  /*  Average Bandwidth Level */
  bandwidth_level_type_v01 ab_level;
  /**<   LEVEL_0 indicates average bandwidth is less than ab_low_threshold
       LEVEL_1 indicates average bandwidth is between ab_low_threshold and
               ab_high_threshold
       LEVEL_2 indicates average bandwidth is greater than ab_high_threshold
  */

  /* Mandatory */
  /*  Instantaneous Bandwidth Level  */
  bandwidth_level_type_v01 ib_level;
  /**<   LEVEL_0 indicates instantaneous bandwidth is less than ib_low_threshold
       LEVEL_1 indicates instantaneous bandwidth is between ib_low_thresholdwith
               and ib_high_threshold
       LEVEL_2 indicates instantaneous bandwidth is greater than ib_high_threshold
    */
}blm_bw_level_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Request Message; This command or message group allows clients to get max possible
    mitigation level supported on the service */
typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}blm_get_max_mitigation_level_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Response Message; This command or message group allows clients to get max possible
    mitigation level supported on the service */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Mitigation Level */
  uint8_t max_mitigation_level_valid;  /**< Must be set to true if max_mitigation_level is being passed */
  uint32_t max_mitigation_level;
  /**<   Maximum number of mitigation levels supported on the service.
  */
}blm_get_max_mitigation_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Request Message; This command or message group allows clients to send a request 
    to get info like bandwidth thresholds, mitigation and bandwidth levels */
typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}blm_get_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup blm_qmi_messages
    @{
  */
/** Response Message; This command or message group allows clients to send a request 
    to get info like bandwidth thresholds, mitigation and bandwidth levels */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Average Bandwidth Low Threshold */
  uint8_t ab_low_threshold_valid;  /**< Must be set to true if ab_low_threshold is being passed */
  uint64_t ab_low_threshold;
  /**<   Average bandwidth low threshold in bytes\second
  */

  /* Optional */
  /*  Average Bandwidth High Threshold */
  uint8_t ab_high_threshold_valid;  /**< Must be set to true if ab_high_threshold is being passed */
  uint64_t ab_high_threshold;
  /**<   Average bandwidth high threshold in bytes\second
  */

  /* Optional */
  /*  Instantaneous Bandwidth Low Threshold */
  uint8_t ib_low_threshod_valid;  /**< Must be set to true if ib_low_threshod is being passed */
  uint64_t ib_low_threshod;
  /**<   Instantaneous bandwidth low threshold in bytes\second
  */

  /* Optional */
  /*  Instantaneous Bandwidth High Threshold */
  uint8_t ib_high_threshold_valid;  /**< Must be set to true if ib_high_threshold is being passed */
  uint64_t ib_high_threshold;
  /**<   Instantaneous bandwidth high threshold in bytes\second
  */

  /* Optional */
  /*  Mitigation Level */
  uint8_t mitigation_level_valid;  /**< Must be set to true if mitigation_level is being passed */
  uint32_t mitigation_level;
  /**<   Current mitigation level which is applied on the service. 
  */

  /* Optional */
  /*  Average Bandwidth Level */
  uint8_t ab_level_valid;  /**< Must be set to true if ab_level is being passed */
  bandwidth_level_type_v01 ab_level;
  /**<   LEVEL_0 indicates average bandwidth is less than ab_low_threshold
       LEVEL_1 indicates average bandwidth is between ab_low_threshold and
               ab_high_threshold
       LEVEL_2 indicates average bandwidth is greater than ab_high_threshold
  */

  /* Optional */
  /*  Instantaneous Bandwidth Level  */
  uint8_t ib_level_valid;  /**< Must be set to true if ib_level is being passed */
  bandwidth_level_type_v01 ib_level;
  /**<   LEVEL_0 indicates instantaneous bandwidth is less than ib_low_threshold
       LEVEL_1 indicates instantaneous bandwidth is between ib_low_threshold
               and ib_high_threshold
       LEVEL_2 indicates instantaneous bandwidth is greater than ib_high_threshold
   */
}blm_get_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_BLM_BW_LEVEL_IND_V01 
//#define REMOVE_QMI_BLM_GET_INFO_V01 
//#define REMOVE_QMI_BLM_GET_MAX_MITIGATION_LEVEL_V01 
//#define REMOVE_QMI_BLM_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_BLM_MONITOR_BW_V01 

/*Service Message Definition*/
/** @addtogroup blm_qmi_msg_ids
    @{
  */
#define QMI_BLM_MONITOR_BW_REQ_V01 0x0020
#define QMI_BLM_MONITOR_BW_RESP_V01 0x0020
#define QMI_BLM_INDICATION_REGISTER_REQ_V01 0x0021
#define QMI_BLM_INDICATION_REGISTER_RESP_V01 0x0021
#define QMI_BLM_BW_LEVEL_IND_V01 0x0022
#define QMI_BLM_GET_MAX_MITIGATION_LEVEL_REQ_V01 0x0023
#define QMI_BLM_GET_MAX_MITIGATION_LEVEL_RESP_V01 0x0023
#define QMI_BLM_GET_INFO_REQ_V01 0x0024
#define QMI_BLM_GET_INFO_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro blm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type blm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define blm_get_service_object_v01( ) \
          blm_get_service_object_internal_v01( \
            BLM_V01_IDL_MAJOR_VERS, BLM_V01_IDL_MINOR_VERS, \
            BLM_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

