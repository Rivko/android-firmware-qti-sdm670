#ifndef SERVREG_NOTIF_SERVICE_01_H
#define SERVREG_NOTIF_SERVICE_01_H
/**
  @file service_registry_notifier_v01.h
  @brief This is the public header file which defines the servreg_notif service Data structures.
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2
   It was generated on: Fri Apr 17 2015 (Spin 0)
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
#define SERVREG_NOTIF_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SERVREG_NOTIF_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SERVREG_NOTIF_V01_MAX_MESSAGE_ID 0x0023
/**
    @}
  */


/** @addtogroup servreg_notif_qmi_consts
    @{
  */

/**  Max length of the domain name i.e "soc/domain/subdomain" or service name i.e "provider/service" is 64 bytes */
#define QMI_SERVREG_NOTIF_NAME_LENGTH_V01 64
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
  SERVREG_NOTIF_SERVICE_STATE_UNINIT_V01 = 0xFFFFFFFF,
  QMI_SERVREG_NOTIF_SERVICE_STATE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_servreg_notif_service_state_enum_type_v01;
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Register or De-register as remote listener */
typedef struct {

  /* Mandatory */
  /*  Enable */
  uint8_t enable;
  /**<    Flag set to TRUE for registration and set to FALSE for de-registration as
       a remote listener. Service state update indication for this REQ is sent
       via the IND message : QMI_SERVREG_NOTIF_STATE_UPDATED_IND
  */

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
}qmi_servreg_notif_register_listener_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Register or De-register as remote listener */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Current state of the service */
  uint8_t curr_state_valid;  /**< Must be set to true if curr_state is being passed */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;
}qmi_servreg_notif_register_listener_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Request Message; Clients get the current sate of the service they are interested in. */
typedef struct {

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];
}qmi_servreg_notif_query_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Response Message; Clients get the current sate of the service they are interested in. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Current State */
  uint8_t curr_state_valid;  /**< Must be set to true if curr_state is being passed */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;
}qmi_servreg_notif_query_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup servreg_notif_qmi_messages
    @{
  */
/** Indication Message; This indication notifies clients when there is a state change of the
           service that the clients had registered as listeners via the
           QMI_SERVREG_NOTIF_REGISTER_LISTENER QMI message */
typedef struct {

  /* Mandatory */
  /*  Current State */
  qmi_servreg_notif_service_state_enum_type_v01 curr_state;

  /* Mandatory */
  /*  Service Name */
  char service_name[QMI_SERVREG_NOTIF_NAME_LENGTH_V01 + 1];

  /* Mandatory */
  /*  Transaction ID */
  uint16_t transaction_id;
  /**<    Transaction ID's will be different for different states of the service.
       Transaction ID is to pair the QMI_SERVREG_NOTIF_STATE_UPDATED_IND message
       with the QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ message.
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

  /* Mandatory */
  /*  Transaction ID */
  uint16_t transaction_id;
  /**<   Transaction id indicating for which state change of the service the ACK is
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

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_SERVREG_NOTIF_QUERY_STATE_V01
//#define REMOVE_QMI_SERVREG_NOTIF_REGISTER_LISTENER_V01
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

