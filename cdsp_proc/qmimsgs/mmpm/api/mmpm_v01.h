#ifndef DSPPM_SVC_SERVICE_01_H
#define DSPPM_SVC_SERVICE_01_H
/**
  @file mmpm_v01.h

  @brief This is the public header file which defines the dsppm_svc service Data structures.

  This header file defines the types and structures that were defined in
  dsppm_svc. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/mmpm/api/mmpm_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Aug 17 2018 (Spin 0)
   From IDL File: mmpm_v01.idl */

/** @defgroup dsppm_svc_qmi_consts Constant values defined in the IDL */
/** @defgroup dsppm_svc_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup dsppm_svc_qmi_enums Enumerated types used in QMI messages */
/** @defgroup dsppm_svc_qmi_messages Structures sent as QMI messages */
/** @defgroup dsppm_svc_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup dsppm_svc_qmi_accessor Accessor for QMI service object */
/** @defgroup dsppm_svc_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dsppm_svc_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define DSPPM_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define DSPPM_SVC_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define DSPPM_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define DSPPM_SVC_V01_MAX_MESSAGE_ID 0x0003
/**
    @}
  */


/** @addtogroup dsppm_svc_qmi_consts
    @{
  */
#define DSPPM_QMI_SVC_ID_V01 1058
#define MAX_CLIENT_NAME_SIZE_V01 32
#define MAX_NUM_REQUESTS_V01 32
#define MAX_NUM_RESOURCES_V01 8
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMIRSCIDTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_RSC_ID_POWER_V01 = 0, /**<  GDFS for MMSS cores.  */
  MMPM_QMI_RSC_ID_REG_PROG_V01 = 1, /**<  Register programming clocks.  */
  MMPM_QMI_RSC_ID_CORE_CLK_V01 = 2, /**<  Core clocks requested by passing in clock settings.  */
  MMPM_QMI_RSC_ID_SLEEP_LATENCY_V01 = 3, /**<  Interrupt latency requested by passing in values.  */
  MMPM_QMI_RSC_ID_MIPS_EXT_V01 = 4, /**<  Extended MIPS requested by passing in MIPS, request type, and
         code location values.  */
  MMPM_QMI_RSC_ID_GENERIC_BW_V01 = 5, /**<  Generic bandwidth defined by the endpoints.  */
  MMPM_QMI_RSC_ID_GENERIC_BW_EXT_V01 = 6, /**<  Generic bandwidth defined by the endpoints. Bandwidth voting through
         this resource ID is intended for audio and voice clients, and has an
         internal compensation factor automatically applied to the vote to
         ensure audio and voice usecases are provided the requested bandwidth.
          */
  MMPM_QMI_RSC_ID_MPPS_V01 = 7, /**<  Request for MPPS (million packets per second) requirement
        for audio/voice clients.  This will be converted internally
        to aDSP clock requirement
          */
  MMPMQMIRSCIDTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiRscIdType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMICLIENTTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_CLIENT_TYPE_NORMAL_V01 = 0, /**<  Normal Client.  */
  MMPM_QMI_CLIENT_TYPE_PERIODIC_V01 = 1, /**<  Periodic Client.  */
  MMPMQMICLIENTTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiClientType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMIREGPROGSPEEDTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_REG_PROG_NORM_V01 = 0, /**<  Register programming at normal speed.  */
  MMPM_QMI_REG_PROG_FAST_V01 = 1, /**<  Register programming at fast speed.  */
  MMPMQMIREGPROGSPEEDTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiRegProgSpeedType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMIFREQMATCHTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_FREQ_AT_LEAST_V01 = 0, /**<  At least.  */
  MMPM_QMI_FREQ_AT_MOST_V01 = 1, /**<  At most.  */
  MMPM_QMI_FREQ_CLOSEST_V01 = 2, /**<  Closest.  */
  MMPM_QMI_FREQ_EXACT_V01 = 3, /**<  Exact.  */
  MMPMQMIFREQMATCHTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiFreqMatchType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMIBWUSAGELPASSTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_BW_USAGE_LPASS_DSP_V01 = 0, /**<  Bandwidth usage by the aDSP.  */
  MMPM_QMI_BW_USAGE_LPASS_DMA_V01 = 1, /**<  Bandwidth usage by DMA/BAM.  */
  MMPM_QMI_BW_USAGE_LPASS_EXT_CPU_V01 = 2, /**<  Bandwidth usage by the CPU outside
                                        of the LPASS.  */
  MMPMQMIBWUSAGELPASSTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiBwUsageLpassType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMICODELOCATIONTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_CODE_LOCATION_DDR_V01 = 0, /**<  DDR Code location.  */
  MMPM_QMI_CODE_LOCATION_OCIMEM_V01 = 1, /**<  OCIMEM Code location.  */
  MMPMQMICODELOCATIONTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiCodeLocationType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMIMIPSREQUESTFNTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_MIPS_REQUEST_CPU_CLOCK_ONLY_V01 = 0, /**<  MIPS request adjusts only the CPU clock. The bandwidth to cover
                                                instruction fetches is requested separately.  */
  MMPM_QMI_MIPS_REQUEST_CPU_CLOCK_AND_BW_V01 = 1, /**<  MIPS request adjusts CPU clock and bandwidth required to support
                                                instruction fetches. The location of the code must be provided in
                                                this request. */
  MMPMQMIMIPSREQUESTFNTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiMipsRequestFnType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_enums
    @{
  */
typedef enum {
  MMPMQMISTATUSTYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MMPM_QMI_STATUS_SUCCESS_V01 = 0, /**<  Success.  */
  MMPM_QMI_STATUS_FAILED_V01 = 1, /**<  General failure.  */
  MMPM_QMI_STATUS_BADPARM_V01 = 2, /**<  Invalid parameter.  */
  MMPM_QMI_STATUS_INVALIDFORMAT_V01 = 3, /**<  Invalid format.  */
  MMPM_QMI_STATUS_UNSUPPORTED_V01 = 4, /**<  API is not supported.  */
  MMPM_QMI_STATUS_NOTINITIALIZED_V01 = 5, /**<  Not initialized.  */
  MMPMQMISTATUSTYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}MmpmQmiStatusType_v01;
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t coreId;
  /**<   Core ID with which the client should register.      */

  uint32_t instanceId;
  /**<   Instance ID with which the client should register.  */

  char clientName[MAX_CLIENT_NAME_SIZE_V01];
  /**<   Client Name with which the client should register.  */

  MmpmQmiClientType_v01 clientType;
  /**<   Type of the client.  */
}mmpm_register_param_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t clkId;
  /**<   Clock ID corresponding to the Core Clock Request.  */

  uint32_t clkFreq;
  /**<   Clock Frequency corresponding to the Core Clock Request in Hertz.  */

  MmpmQmiFreqMatchType_v01 freqMatch;
  /**<   Frequency Match Type corresponding to the Core Clock Request.  */
}mmpm_clk_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t masterPort;
  /**<   Master Port corresponding to BW Request.     */

  uint32_t slavePort;
  /**<   Slave Port corresponding to BW Request.     */

  uint32_t bytesPerSec;
  /**<   BW value in Bytes/Second     */

  uint32_t usagePercent;
  /**<   Usage Percentage 0-100 %.     */

  MmpmQmiBwUsageLpassType_v01 usageType;
  /**<   BW Usage Type.     */
}mmpm_bw_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t mppsTotal;
  /**<   MPPS Total corresponding to the MPPS Request.     */

  uint32_t adspFloorClock;
  /**<   ADSP Floor Clock in Hertz.     */
}mmpm_mpps_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t mipsTotal;
  /**<   MIPS Total corresponding to the MIPS Request.     */

  uint32_t mipsPerThread;
  /**<   MIPS per Thread.       */

  MmpmQmiCodeLocationType_v01 codeLocation;
  /**<   Code Location.     */

  MmpmQmiMipsRequestFnType_v01 reqOperation;
  /**<   Request Operation.     */
}mmpm_mips_req_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_aggregates
    @{
  */
typedef struct {

  MmpmQmiRscIdType_v01 rscId;
  /**<   Resource ID corresponding to the requests. */

  MmpmQmiStatusType_v01 status;
  /**<   MMPM Return status. */
}mmpm_rsc_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Request Message; This command registers the client with ADSPPM */
typedef struct {

  /* Mandatory */
  mmpm_register_param_type_v01 regParam;
  /**<  Parameter for MMPM registration information.*/
}mmpm_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Response Message; This command registers the client with ADSPPM */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI Response type.*/

  /* Optional */
  uint8_t clientId_valid;  /**< Must be set to true if clientId is being passed */
  uint32_t clientId;
  /**<   Client ID.*/
}mmpm_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Request Message; This command de-registers the client with ADSPPM */
typedef struct {

  /* Mandatory */
  uint32_t clientId;
  /**<   Client ID.*/
}mmpm_deregister_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Response Message; This command de-registers the client with ADSPPM */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI Response type.*/

  /* Optional */
  uint8_t mmpmStatus_valid;  /**< Must be set to true if mmpmStatus is being passed */
  MmpmQmiStatusType_v01 mmpmStatus;
  /**<   MMPM Status.*/
}mmpm_deregister_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Request Message; This command places a request with ADSPPM */
typedef struct {

  /* Mandatory */
  uint32_t clientId;
  /**<   Client ID.*/

  /* Mandatory */
  uint32_t rscId_len;  /**< Must be set to # of elements in rscId */
  MmpmQmiRscIdType_v01 rscId[MAX_NUM_RESOURCES_V01];
  /**<   Resource ID Corresponding to the requests.*/

  /* Optional */
  uint8_t regProgSpeed_valid;  /**< Must be set to true if regProgSpeed is being passed */
  MmpmQmiRegProgSpeedType_v01 regProgSpeed;
  /**<   Parameter for Reg Programming request .*/

  /* Optional */
  uint8_t coreClockReq_valid;  /**< Must be set to true if coreClockReq is being passed */
  uint32_t coreClockReq_len;  /**< Must be set to # of elements in coreClockReq */
  mmpm_clk_req_type_v01 coreClockReq[MAX_NUM_REQUESTS_V01];
  /**<   Parameter for Core Clock request.  */

  /* Optional */
  uint8_t genBwReq_valid;  /**< Must be set to true if genBwReq is being passed */
  uint32_t genBwReq_len;  /**< Must be set to # of elements in genBwReq */
  mmpm_bw_req_type_v01 genBwReq[MAX_NUM_REQUESTS_V01];
  /**<   Parameter for Band Width request.  */

  /* Optional */
  uint8_t sleepLatReq_valid;  /**< Must be set to true if sleepLatReq is being passed */
  uint32_t sleepLatReq;
  /**<   Sleep latency value in micro seconds.  */

  /* Optional */
  uint8_t mipsReq_valid;  /**< Must be set to true if mipsReq is being passed */
  mmpm_mips_req_type_v01 mipsReq;
  /**<   Parameter for MIPS request.  */

  /* Optional */
  uint8_t mppsReq_valid;  /**< Must be set to true if mppsReq is being passed */
  mmpm_mpps_req_type_v01 mppsReq;
  /**<   Parameter for MPPS request.  */
}mmpm_request_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Response Message; This command places a request with ADSPPM */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI Response type.*/

  /* Optional */
  uint8_t reqReturnStatus_valid;  /**< Must be set to true if reqReturnStatus is being passed */
  uint32_t reqReturnStatus_len;  /**< Must be set to # of elements in reqReturnStatus */
  mmpm_rsc_status_type_v01 reqReturnStatus[MAX_NUM_RESOURCES_V01];
  /**<   Return Status corresponding to each individual request.*/
}mmpm_request_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Request Message; This command releases a request with ADSPPM */
typedef struct {

  /* Mandatory */
  uint32_t clientId;
  /**<   Client ID.*/

  /* Mandatory */
  uint32_t rscId_len;  /**< Must be set to # of elements in rscId */
  MmpmQmiRscIdType_v01 rscId[MAX_NUM_RESOURCES_V01];
  /**<   Resource ID corresponding to the requests.*/
}mmpm_release_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dsppm_svc_qmi_messages
    @{
  */
/** Response Message; This command releases a request with ADSPPM */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI Response type.*/

  /* Optional */
  uint8_t reqReturnStatus_valid;  /**< Must be set to true if reqReturnStatus is being passed */
  uint32_t reqReturnStatus_len;  /**< Must be set to # of elements in reqReturnStatus */
  mmpm_rsc_status_type_v01 reqReturnStatus[MAX_NUM_RESOURCES_V01];
  /**<   Return Status corresponding to each individual request.*/
}mmpm_release_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_DSPPM_DEREGISTER_V01 
//#define REMOVE_QMI_DSPPM_REGISTER_V01 
//#define REMOVE_QMI_DSPPM_RELEASE_V01 
//#define REMOVE_QMI_DSPPM_REQUEST_V01 

/*Service Message Definition*/
/** @addtogroup dsppm_svc_qmi_msg_ids
    @{
  */
#define QMI_DSPPM_REGISTER_REQ_V01 0x0000
#define QMI_DSPPM_REGISTER_RESP_V01 0x0000
#define QMI_DSPPM_DEREGISTER_REQ_V01 0x0001
#define QMI_DSPPM_DEREGISTER_RESP_V01 0x0001
#define QMI_DSPPM_REQUEST_REQ_V01 0x0002
#define QMI_DSPPM_REQUEST_RESP_V01 0x0002
#define QMI_DSPPM_RELEASE_REQ_V01 0x0003
#define QMI_DSPPM_RELEASE_RESP_V01 0x0003
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro dsppm_svc_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type dsppm_svc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define dsppm_svc_get_service_object_v01( ) \
          dsppm_svc_get_service_object_internal_v01( \
            DSPPM_SVC_V01_IDL_MAJOR_VERS, DSPPM_SVC_V01_IDL_MINOR_VERS, \
            DSPPM_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

