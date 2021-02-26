#ifndef SERVREG_NOTIF_SERVICE_01_H
#define SERVREG_NOTIF_SERVICE_01_H
/**
  @file service_registry_notifier_v01.h

  @brief This is the public header file which defines the servreg_notif service Data structures.

  This header file defines the types and structures that were defined in
  servreg_notif. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015-16 Qualcomm Technologies, Inc. 
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //components/rel/qmimsgs.adsp/2.6/servreg_notif/api/service_registry_notifier_v01.h#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Aug  2 2016 (Spin 0)
   From IDL File: service_registry_notifier_v01.idl */

/** @defgroup servreg_notif_qmi_consts Constant values defined in the IDL */
/** @defgroup servreg_notif_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup servreg_notif_qmi_enums Enumerated types used in QMI messages */
/** @defgroup servreg_notif_qmi_messages Structures sent as QMI messages */
/** @defgroup servreg_notif_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup servreg_notif_qmi_accessor Accessor for QMI service object */
/** @defgroup servreg_notif_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup servreg_notif_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SERVREG_NOTIF_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SERVREG_NOTIF_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SERVREG_NOTIF_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SERVREG_NOTIF_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup servreg_notif_qmi_consts
    @{
  */

/**  Max length of the domain name i.e "soc/domain/subdomain" or service name i.e "provider/service" is 64 bytes */
#define QMI_SERVREG_NOTIF_NAME_LENGTH_V01 64

/**  Max length of the process specifier */
#define QMI_SERVREG_NOTIF_PROCESS_SPECEFIER_NAME_LENGTH_V01 64
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_enums
    @{
  */
typedef enum {
  QMI_SERVREG_NOTIF_SERVICE_STATE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SERVREG_NOTIF_SERVICE_STATE_DOWN_V01 = 0x0FFFFFFF, 
  SERVREG_NOTIF_SERVICE_STATE_UP_V01 = 0x1FFFFFFF, 
  SERVREG_NOTIF_SERVICE_STATE_UNINIT_V01 = 0x7FFFFFFF, 
  QMI_SERVREG_NOTIF_SERVICE_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_servreg_notif_service_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Registers or deregisters a client with the service as a remote
    listener. */
typedef struct {

  /* Mandatory */
  /*  Enable */
  uint8_t enable;
  /**<    Flag set to TRUE for registration and FALSE for deregistration as 
       a remote listener. The service state update indication for this request is sent 
       via the QMI_SERVREG_NOTIF_STATE_UPDATED_IND indication.
  */

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
  /**<   Name of the service. */
}qmi_servreg_notif_register_listener_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Registers or deregisters a client with the service as a remote
    listener. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Current State */
  uint8_t curr_state_valid;  /**< Must be set to true if curr_state is being passed */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;
  /**<   Current state of the service. */
}qmi_servreg_notif_register_listener_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Clients use this command to get the current sate of the selected
    service. */
typedef struct {

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
  /**<   Name of the service. */
}qmi_servreg_notif_query_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Clients use this command to get the current sate of the selected
    service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Current State */
  uint8_t curr_state_valid;  /**< Must be set to true if curr_state is being passed */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;
  /**<   Current state of the service. */
}qmi_servreg_notif_query_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Indication Message; This indication notifies clients when there is a state change in the
           service to which the clients registered as listeners via the
           QMI_SERVREG_NOTIF_REGISTER_LISTENER message. */
typedef struct {

  /* Mandatory */
  /*  Current State */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;
  /**<   Current state of the service. */

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
  /**<   Name of the service. */

  /* Mandatory */
  /*  Transaction ID */
  uint16_t transaction_id;
  /**<    Transaction IDs are different for different states of the service.
       The transaction ID is used to pair the QMI_SERVREG_NOTIF_STATE_UPDATED_IND
       message with the QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ message.
  */
}qmi_servreg_notif_state_updated_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Clients send an ACK message indicating that they have acknowledged
           the service state change notification. It is up to the clients to
           decide which indication message should be acknowledged. */
typedef struct {

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
  /**<   Name of the service. */

  /* Mandatory */
  /*  Transaction ID */
  uint16_t transaction_id;
  /**<   Transaction ID indicating for which service state change the ACK is 
      being sent. 
  */
}qmi_servreg_notif_set_ack_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Clients send an ACK message indicating that they have acknowledged
           the service state change notification. It is up to the clients to
           decide which indication message should be acknowledged. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qmi_servreg_notif_set_ack_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Clients send request to restart a PD(Protection Domain) present in 
           a substem the PD name should be sent as a process specifier name as 
           string  for example "msm/modem/wlan_pd" which it want to restart.
           Once the request is received the service will verify the process
           specefier and then send apropriate error code in the response message,
           if the PD exists in that subsystem then it will initiate the PD restart. */
typedef struct {

  /* Mandatory */
  /*  Service Name */
  char process_spc_name[QMI_SERVREG_NOTIF_PROCESS_SPECEFIER_NAME_LENGTH_V01 + 1];
  /**<   Name of the process specefier name. */
}qmi_servreg_notif_restart_pd_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Clients send request to restart a PD(Protection Domain) present in 
           a substem the PD name should be sent as a process specifier name as 
           string  for example "msm/modem/wlan_pd" which it want to restart.
           Once the request is received the service will verify the process
           specefier and then send apropriate error code in the response message,
           if the PD exists in that subsystem then it will initiate the PD restart. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}qmi_servreg_notif_restart_pd_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SERVREG_NOTIF_QUERY_STATE_V01 
//#define REMOVE_QMI_SERVREG_NOTIF_REGISTER_LISTENER_V01 
//#define REMOVE_QMI_SERVREG_NOTIF_RESTART_PD_V01 
//#define REMOVE_QMI_SERVREG_NOTIF_STATE_UPDATED_IND_V01 
//#define REMOVE_QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_V01 

/*Service Message Definition*/
/** @addtogroup servreg_notif_qmi_msg_ids
    @{
  */
#define QMI_SERVREG_NOTIF_REGISTER_LISTENER_REQ_V01 0x0020
#define QMI_SERVREG_NOTIF_REGISTER_LISTENER_RESP_V01 0x0020
#define QMI_SERVREG_NOTIF_QUERY_STATE_REQ_V01 0x0021
#define QMI_SERVREG_NOTIF_QUERY_STATE_RESP_V01 0x0021
#define QMI_SERVREG_NOTIF_STATE_UPDATED_IND_V01 0x0022
#define QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ_V01 0x0023
#define QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_RESP_V01 0x0023
#define QMI_SERVREG_NOTIF_RESTART_PD_REQ_V01 0x0024
#define QMI_SERVREG_NOTIF_RESTART_PD_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro servreg_notif_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type servreg_notif_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define servreg_notif_get_service_object_v01( ) \
          servreg_notif_get_service_object_internal_v01( \
            SERVREG_NOTIF_V01_IDL_MAJOR_VERS, SERVREG_NOTIF_V01_IDL_MINOR_VERS, \
            SERVREG_NOTIF_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

