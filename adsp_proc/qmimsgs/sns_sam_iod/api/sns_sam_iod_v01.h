#ifndef SNS_SAM_IOD_SVC_SERVICE_01_H
#define SNS_SAM_IOD_SVC_SERVICE_01_H
/**
  @file sns_sam_iod_v01.h

  @brief This is the public header file which defines the SNS_SAM_IOD_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_IOD_SVC. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/sns_sam_iod/api/sns_sam_iod_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.4 
   It was generated on: Thu May  7 2015 (Spin 1)
   From IDL File: sns_sam_iod_v01.idl */

/** @defgroup SNS_SAM_IOD_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_IOD_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_IOD_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_IOD_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_IOD_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_IOD_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_IOD_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_IOD_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_IOD_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_IOD_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_IOD_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_IOD_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_IOD_SVC_qmi_consts 
    @{ 
  */
#define SNS_SAM_IOD_SUID_V01 0xb9b65987126198b2
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_IOD_OUTPUT_CLASS_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_IOD_OUTPUT_CLASS_UNKNOWN_V01 = 0, /**<  Unknown output class  */
  SNS_SAM_IOD_OUTPUT_CLASS_INDOOR_V01 = 1, /**<  Indoor output class  */
  SNS_SAM_IOD_OUTPUT_CLASS_OUTDOOR_V01 = 2, /**<  Outdoor output class  */
  SNS_SAM_IOD_OUTPUT_CLASS_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_iod_output_class_e_v01;
/**
    @}
  */

typedef uint64_t sns_sam_iod_reason_unknown_m_v01;
#define SNS_SAM_IOD_REASON_UNKNOWN_LOW_CONFIDENCE_MASK_V01 ((sns_sam_iod_reason_unknown_m_v01)0x001ull) /**<  Unknown output class is reported due to low confidence  */
#define SNS_SAM_IOD_REASON_UNKNOWN_NIGHT_MASK_V01 ((sns_sam_iod_reason_unknown_m_v01)0x002ull) /**<  Unknown output class is reported due to night condition  */
#define SNS_SAM_IOD_REASON_UNKNOWN_CONCEALED_MASK_V01 ((sns_sam_iod_reason_unknown_m_v01)0x004ull) /**<  Unknown output class is reported because the device is concealed  */
#define SNS_SAM_IOD_REASON_UNKNOWN_ALS_LOW_MASK_V01 ((sns_sam_iod_reason_unknown_m_v01)0x008ull) /**<  Unknown output class is reported due to low light from ALS sensor  */
#define SNS_SAM_IOD_REASON_PARAMETERS_NOT_INITIALIZED_V01 ((sns_sam_iod_reason_unknown_m_v01)0x010ull) /**<  Unknown output class is reported due to uninitialized parameters such
       as auto-calibration value. This unknown reason will be set regardless
       of time of day   */
/** @addtogroup SNS_SAM_IOD_SVC_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sunset_ts;
  /**<   UTC timestamp for sunset in secs from epoch */

  uint32_t sunrise_ts;
  /**<   UTC timestamp for sunrise in secs from epoch */
}sns_sam_iod_sunset_sunrise_ts_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_aggregates
    @{
  */
typedef struct {

  /*  Report Timestamp */
  uint32_t timestamp;
  /**<  
     UTC timestamp in secs since epoch
   */

  /*  Output Class */
  sns_sam_iod_output_class_e_v01 output_class;
  /**<  
 IOD output class, can be either SNS_SAM_IOD_OUTPUT_CLASS_UNKNOWN or
 SNS_SAM_IOD_OUTPUT_CLASS_INDOOR/SNS_SAM_IOD_OUTPUT_CLASS_OUTDOOR
      - SNS_SAM_IOD_OUTPUT_CLASS_UNKNOWN (0) --  Unknown output class 
      - SNS_SAM_IOD_OUTPUT_CLASS_INDOOR (1) --  Indoor output class 
      - SNS_SAM_IOD_OUTPUT_CLASS_OUTDOOR (2) --  Outdoor output class 
 */

  /*  Reasons for Unknown Class */
  sns_sam_iod_reason_unknown_m_v01 reason_unknown;
  /**<  
 Bitmask to identify reasons for SNS_SAM_IOD_OUTPUT_CLASS_UNKNOWN
 output class. This is valid only if the output_class is
 SNS_SAM_IOD_OUTPUT_CLASS_UNKNOWN and should be ignored otherwise
      - SNS_SAM_IOD_REASON_UNKNOWN_LOW_CONFIDENCE_MASK (0x001) --  Unknown output class is reported due to low confidence 
      - SNS_SAM_IOD_REASON_UNKNOWN_NIGHT_MASK (0x002) --  Unknown output class is reported due to night condition 
      - SNS_SAM_IOD_REASON_UNKNOWN_CONCEALED_MASK (0x004) --  Unknown output class is reported because the device is concealed 
      - SNS_SAM_IOD_REASON_UNKNOWN_ALS_LOW_MASK (0x008) --  Unknown output class is reported due to low light from ALS sensor 
      - SNS_SAM_IOD_REASON_PARAMETERS_NOT_INITIALIZED (0x010) --  Unknown output class is reported due to uninitialized parameters such
       as auto-calibration value. This unknown reason will be set regardless
       of time of day  
 */

  /*  Confidence */
  float confidence;
  /**<  
    Confidence with which a reported output class was detected, that is
    for indoor or outdoor reported output class this field would mean
    confidence of detection of indoor or outdoor class respectively.
    For the unknown reported output class this field would mean confidence
    of detection of indoor class. Range is from 0 to 1.
   */
}sns_sam_iod_report_data_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Report Period */
  uint32_t report_period;
  /**<   Unit of seconds, Q16; value of 0 means reporting on new event only */

  /* Mandatory */
  /*  Time of Day */
  uint32_t timeofday;
  /**<   UTC timestamp in secs from epoch to sync the ADSP clock */

  /* Mandatory */
  /*  Sunset and Sunrise Timestamps */
  sns_sam_iod_sunset_sunrise_ts_s_v01 sunset_sunrise_ts;
  /**<   UTC sunset/sunrise timestamps in isecs from epoch
       Sunset/sunrise timestamps will be used by the IOD algorithm to
       determine when to disable/re-enable processing sensors data for
       the night and to report unknown output state with the
       SNS_SAM_IOD_REASON_UNKNOWN_NIGHT reason
   */

  /* Optional */
  /*  Suspend Notification */
  uint8_t notify_suspend_valid;  /**< Must be set to true if notify_suspend is being passed */
  sns_suspend_notification_s_v01 notify_suspend;
  /**<   Identifies if indications for this request should be sent
       when the processor is in suspend state.

       If this field is not specified, default value will be set to
       notify_suspend->proc_type                  = SNS_PROC_APPS
       notify_suspend->send_indications_during_suspend  = FALSE

       This field does not have any bearing on error indication
       messages, which will be sent even during suspend.
   */
}sns_sam_iod_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Algorithm instance ID maintained/assigned by SAM.
    The client shall use this instance ID for future messages associated with
    current algorithm instance.
   */
}sns_sam_iod_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance to be disabled.  */
}sns_sam_iod_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Algorithm instance ID maintained/assigned by SAM.
    The client shall use this instance ID for future messages associated with
    current algorithm instance.
   */
}sns_sam_iod_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command gets report from the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_iod_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command gets report from the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Algorithm instance ID maintained/assigned by SAM.
    The client shall use this instance ID for future messages associated with
    current algorithm instance.
   */

  /* Optional */
  /*  IOD Algorithm Output Report */
  uint8_t report_data_valid;  /**< Must be set to true if report_data is being passed */
  sns_sam_iod_report_data_s_v01 report_data;
  /**<   IOD algorithm output report  */
}sns_sam_iod_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Indication Message; Output report from the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  /*  Sunset and Sunrise Timestamps */
  uint32_t timestamp;
  /**<   Timestamp at which this output is applicable, in SSC ticks */

  /* Mandatory */
  /*  IOD Algorithm Output Report */
  sns_sam_iod_report_data_s_v01 report_data;
  /**<   IOD algorithm output report.  */
}sns_sam_iod_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error report from a sensor algorithm. */
typedef struct {

  /* Mandatory */
  /*  Error Code */
  uint8_t error;
  /**<   Sensors error code */

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
}sns_sam_iod_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command configures the IOD algorithm after it has been enabled. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Optional */
  /*  Sunset and Sunrise Timestamps */
  uint8_t sunset_sunrise_ts_valid;  /**< Must be set to true if sunset_sunrise_ts is being passed */
  sns_sam_iod_sunset_sunrise_ts_s_v01 sunset_sunrise_ts;
  /**<   UTC sunset/sunrise timestamps in secs from epoch
       Sunset/sunrise timestamps will be used by the IOD algorithm to
       determine when to disable/re-enable processing sensors data for the
       night and to report unknown output state with the
       SNS_SAM_IOD_REASON_UNKNOWN_NIGHT reason
   */

  /* Optional */
  /*  Confidence Threshold Indoor */
  uint8_t confidence_threshold_indoor_valid;  /**< Must be set to true if confidence_threshold_indoor is being passed */
  float confidence_threshold_indoor;
  /**<   Threshold for confidence for the indoor output class to be reported in
       SNS_SAM_IOD_REPORT_IND must be above 0.5 ; 0.6 value is used by default
   */

  /* Optional */
  /*  Confidence Threshold Outdoor */
  uint8_t confidence_threshold_outdoor_valid;  /**< Must be set to true if confidence_threshold_outdoor is being passed */
  float confidence_threshold_outdoor;
  /**<   Threshold for confidence for the outdoor output class to be reported in
       SNS_SAM_IOD_REPORT_IND must be above 0.5 ; 0.6 value is used by default
   */
}sns_sam_iod_update_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_SVC_qmi_messages
    @{
  */
/** Request Message; This command configures the IOD algorithm after it has been enabled. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Algorithm instance ID maintained/assigned by SAM.
    The client shall use this instance ID for future messages associated with
    current algorithm instance.
   */
}sns_sam_iod_update_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V01 
//#define REMOVE_SNS_SAM_IOD_CANCEL_V01 
//#define REMOVE_SNS_SAM_IOD_DISABLE_V01 
//#define REMOVE_SNS_SAM_IOD_ENABLE_V01 
//#define REMOVE_SNS_SAM_IOD_ERROR_V01 
//#define REMOVE_SNS_SAM_IOD_GET_REPORT_V01 
//#define REMOVE_SNS_SAM_IOD_REPORT_V01 
//#define REMOVE_SNS_SAM_IOD_UPDATE_V01 
//#define REMOVE_SNS_SAM_IOD_VERSION_V01 

/*Service Message Definition*/
/** @addtogroup SNS_SAM_IOD_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_IOD_CANCEL_REQ_V01 0x0000
#define SNS_SAM_IOD_CANCEL_RESP_V01 0x0000
#define SNS_SAM_IOD_VERSION_REQ_V01 0x0001
#define SNS_SAM_IOD_VERSION_RESP_V01 0x0001
#define SNS_SAM_IOD_ENABLE_REQ_V01 0x0002
#define SNS_SAM_IOD_ENABLE_RESP_V01 0x0002
#define SNS_SAM_IOD_DISABLE_REQ_V01 0x0003
#define SNS_SAM_IOD_DISABLE_RESP_V01 0x0003
#define SNS_SAM_IOD_GET_REPORT_REQ_V01 0x0004
#define SNS_SAM_IOD_GET_REPORT_RESP_V01 0x0004
#define SNS_SAM_IOD_REPORT_IND_V01 0x0005
#define SNS_SAM_IOD_ERROR_IND_V01 0x0006
#define SNS_SAM_IOD_UPDATE_REQ_V01 0x0020
#define SNS_SAM_IOD_UPDATE_RESP_V01 0x0020
#define SNS_SAM_IOD_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_IOD_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_IOD_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_IOD_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_SAM_IOD_SVC_get_service_object_v01( ) \
          SNS_SAM_IOD_SVC_get_service_object_internal_v01( \
            SNS_SAM_IOD_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_IOD_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_IOD_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

