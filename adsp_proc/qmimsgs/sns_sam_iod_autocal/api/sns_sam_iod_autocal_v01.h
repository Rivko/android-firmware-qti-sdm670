#ifndef SNS_SAM_IOD_AUTOCAL_SVC_SERVICE_01_H
#define SNS_SAM_IOD_AUTOCAL_SVC_SERVICE_01_H
/**
  @file sns_sam_iod_autocal_v01.h

  @brief This is the public header file which defines the SNS_SAM_IOD_AUTOCAL_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_IOD_AUTOCAL_SVC. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/sns_sam_iod_autocal/api/sns_sam_iod_autocal_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.4 
   It was generated on: Thu May  7 2015 (Spin 0)
   From IDL File: sns_sam_iod_autocal_v01.idl */

/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_IOD_AUTOCAL_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_consts 
    @{ 
  */
#define SNS_SAM_IOD_AUTOCAL_SUID_V01 0xb9b65987126198c3
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_IOD_AUTOCAL_STATUS_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_IOD_AUTOCAL_STATUS_NOT_CONVERGED_V01 = 0, /**<  Auto-calibration status not converged  */
  SNS_SAM_IOD_AUTOCAL_STATUS_CONVERGED_V01 = 1, /**<  Auto-calibration status converged  */
  SNS_SAM_IOD_AUTOCAL_STATUS_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_iod_autocal_status_e_v01;
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sunset_ts;
  /**<   UTC timestamp for sunset in secs from epoch */

  uint32_t sunrise_ts;
  /**<   UTC timestamp for sunrise in secs from epoch */
}sns_sam_iod_autocal_sunset_sunrise_ts_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Report Period */
  uint32_t report_period;
  /**<   Unit of seconds, Q16; value of 0 means reporting on new event only 
         and is only supported for this algorithm
      */

  /* Mandatory */
  /*  Time of Day */
  uint32_t timeofday;
  /**<   UTC timestamp in secs from epoch to sync the ADSP clock */

  /* Mandatory */
  /*  Sunset and Sunrise Timestamps */
  sns_sam_iod_autocal_sunset_sunrise_ts_s_v01 sunset_sunrise_ts;
  /**<   UTC sunset/sunrise timestamps in secs from epoch for the current 
         location of the client. Sunset/sunrise timestamps will be used by 
         the IOD autocalibration algorithm to determine when to resume/suspend
         processing sensors data for autocalibration during the night. 
         The timestamps specified in this request will be used by the algorithm 
         until they are updated in the SNS_SAM_IOD_AUTOCAL_UPDATE request.
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
}sns_sam_iod_autocal_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
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
}sns_sam_iod_autocal_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance to be disabled.  */
}sns_sam_iod_autocal_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
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
}sns_sam_iod_autocal_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
    @{
  */
/** Request Message; This command gets report from the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_iod_autocal_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
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
  /*  Convergence Status */
  uint8_t convergence_status_valid;  /**< Must be set to true if convergence_status is being passed */
  sns_sam_iod_autocal_status_e_v01 convergence_status;
  /**<  
 Convergence status, can be either 
 SNS_SAM_IOD_AUTOCAL_STATUS_NOT_CONVERGED 
 or SNS_SAM_IOD_AUTOCAL_STATUS_CONVERGED
      - SNS_SAM_IOD_AUTOCAL_STATUS_NOT_CONVERGED (0) --  Auto-calibration status not converged 
      - SNS_SAM_IOD_AUTOCAL_STATUS_CONVERGED (1) --  Auto-calibration status converged 
 */

  /* Optional */
  /*  Number of Cumulative Samples */
  uint8_t cumulative_num_samples_valid;  /**< Must be set to true if cumulative_num_samples is being passed */
  uint32_t cumulative_num_samples;
  /**<  
       Cumulative number of samples received by IOD auto-calibration.
     */
}sns_sam_iod_autocal_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
    @{
  */
/** Indication Message; Output report from the IOD algorithm. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  /*  Convergence Status */
  sns_sam_iod_autocal_status_e_v01 convergence_status;
  /**<  
 Convergence status, can be either 
 SNS_SAM_IOD_AUTOCAL_STATUS_NOT_CONVERGED 
 or SNS_SAM_IOD_AUTOCAL_STATUS_CONVERGED
      - SNS_SAM_IOD_AUTOCAL_STATUS_NOT_CONVERGED (0) --  Auto-calibration status not converged 
      - SNS_SAM_IOD_AUTOCAL_STATUS_CONVERGED (1) --  Auto-calibration status converged 
 */

  /* Optional */
  /*  Auto-calibration Coefficient */
  uint8_t auto_cal_coeff_valid;  /**< Must be set to true if auto_cal_coeff is being passed */
  float auto_cal_coeff;
  /**<  
       Auto-calibration coefficient 
     */
}sns_sam_iod_autocal_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
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
}sns_sam_iod_autocal_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
    @{
  */
/** Request Message; This command configures the IOD algorithm after it has been enabled. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  /*  Sunset and Sunrise Timestamps */
  sns_sam_iod_autocal_sunset_sunrise_ts_s_v01 sunset_sunrise_ts;
  /**<   UTC sunset/sunrise timestamps in secs from epoch for the current 
         location of the client Sunset/sunrise timestamps will be used by  
         the IOD auto-calibration algorithm to determine when to resume/suspend
         processing sensors data for auto-calibration during the night 
     */
}sns_sam_iod_autocal_update_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_messages
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
}sns_sam_iod_autocal_update_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_CANCEL_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_DISABLE_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_ENABLE_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_ERROR_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_GET_REPORT_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_REPORT_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_UPDATE_V01 
//#define REMOVE_SNS_SAM_IOD_AUTOCAL_VERSION_V01 

/*Service Message Definition*/
/** @addtogroup SNS_SAM_IOD_AUTOCAL_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_IOD_AUTOCAL_CANCEL_REQ_V01 0x0000
#define SNS_SAM_IOD_AUTOCAL_CANCEL_RESP_V01 0x0000
#define SNS_SAM_IOD_AUTOCAL_VERSION_REQ_V01 0x0001
#define SNS_SAM_IOD_AUTOCAL_VERSION_RESP_V01 0x0001
#define SNS_SAM_IOD_AUTOCAL_ENABLE_REQ_V01 0x0002
#define SNS_SAM_IOD_AUTOCAL_ENABLE_RESP_V01 0x0002
#define SNS_SAM_IOD_AUTOCAL_DISABLE_REQ_V01 0x0003
#define SNS_SAM_IOD_AUTOCAL_DISABLE_RESP_V01 0x0003
#define SNS_SAM_IOD_AUTOCAL_GET_REPORT_REQ_V01 0x0004
#define SNS_SAM_IOD_AUTOCAL_GET_REPORT_RESP_V01 0x0004
#define SNS_SAM_IOD_AUTOCAL_REPORT_IND_V01 0x0005
#define SNS_SAM_IOD_AUTOCAL_ERROR_IND_V01 0x0006
#define SNS_SAM_IOD_AUTOCAL_UPDATE_REQ_V01 0x0020
#define SNS_SAM_IOD_AUTOCAL_UPDATE_RESP_V01 0x0020
#define SNS_SAM_IOD_AUTOCAL_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_IOD_AUTOCAL_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_IOD_AUTOCAL_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_IOD_AUTOCAL_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_SAM_IOD_AUTOCAL_SVC_get_service_object_v01( ) \
          SNS_SAM_IOD_AUTOCAL_SVC_get_service_object_internal_v01( \
            SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_IOD_AUTOCAL_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

