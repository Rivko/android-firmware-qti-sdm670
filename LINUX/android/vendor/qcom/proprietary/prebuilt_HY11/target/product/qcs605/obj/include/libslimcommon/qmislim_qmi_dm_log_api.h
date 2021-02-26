#ifndef _QMISLIM_QMI_DM_LOG_H_INCLUDED_
#define _QMISLIM_QMI_DM_LOG_H_INCLUDED_

/**
 * @file
 * @brief QMI-SLIM QMI Service DM Logging API.
 *
 * @ingroup slim_QSQmiSvc
 */
/*
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/common/inc/qmislim_qmi_dm_log_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <qmi_idl_lib.h>
#include <qmi_csi.h>
#include <qmi_client.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


//! @addtogroup slim_QSLog
//! @{

//! @name DM Logging API
//! @{

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * @brief QMI-SLIM object for DM logging of CSI events
 *
 * This structure is used for generating DM events with QCSI information:
 * - Operation events:
 *   - QMI service registration
 *   - QMI service unregistration
 *   - Client connect event
 *   - Client disconnect event
 * - Message events:
 *   - Handling of request messages
 *   - Handling of response messages
 *   - Handling of indication messages
 *
 * Messages are always logged using QMI IDL encoding rules. Unencoded messages
 * are first encoded internally for logging.
 *
 * @sa slim_QSLogCSIRegister
 * @sa slim_QSLogCSIUnregister
 * @sa slim_QSLogCSIConnect
 * @sa slim_QSLogCSIDisconnect
 * @sa slim_QSLogCSIEncodedMessage
 */
typedef struct
{
  /** @brief QMI service number */
  uint32                      q_SvcId;
  /** @brief QMI service major revision */
  uint32                      q_SvcMajRev;
  /** @brief QMI service minor revision */
  uint32                      q_SvcMinRev;
  /** @brief QMI service instance identifier */
  uint32                      q_SvcInstanceId;
  /** @brief QMI IDL service object handle.
   *
   * Handle to service object, which has meta-data for encoding messages to
   * wire-frame format.
   */
  qmi_idl_service_object_type z_SvcObj;
  /** @brief Count of number of requests received by the service */
  uint32                      q_CntReq;
  /** @brief Count of number of responses sent by the service */
  uint32                      q_CntResp;
  /** @brief Count of number of indications sent by the service */
  uint32                      q_CntInd;
  /** @brief Count of connected clients */
  uint32                      q_SvcCount;
} slim_QSLogT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Processes service registration event.
 *
 * Method initializes service object and produces DM event.
 *
 * @param[out] pz_SvcInfo      CSI service logging object to initialize.
 * @param[in]  z_ServiceObject QMI IDL service object.
 * @param[in]  q_SvcInstanceId QCSI service instance identifier.
 * @param[in]  q_Error         QCSI error code.
 *
 * @sa slim_QSLogT
 * @sa slim_QSLogCSIUnregister
 */
//! @}
/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
 * @brief Processes service registration event.
 *
 * Method initializes service object and produces DM event.
 *
 * @param[out] pz_SvcInfo      CSI service logging object to initialize.
 * @param[in]  z_ServiceObject QMI IDL service object.
 * @param[in]  q_SvcInstanceId QCSI service instance identifier.
 * @param[in]  q_Error         QCSI error code.
 *
 * @sa slim_QSLogT
 * @sa slim_QSLogCSIUnregister
 */
void slim_QSLogCSIRegister
(
  slim_QSLogT* pz_SvcInfo,
  qmi_idl_service_object_type z_ServiceObject,
  uint32 q_SvcInstanceId,
  qmi_csi_error q_Error
);

/**
 * @brief Logs service unregistration event
 *
 * This method produces DM log message about service deregistration and releases
 * This method initializes service object entry and produces DM log message
 * about service registration.
 *
 * @param[in,out] pz_SvcInfo      Service registration object structure.
 *
 * @sa slim_QSLogT
 * @sa slim_QSLogCSIRegister
 */
void slim_QSLogCSIUnregister
(
  slim_QSLogT *pz_SvcInfo
);

/**
 * @brief Logs QMI client connect event.
 *
 * The function logs QMI client connect event.
 *
 * @param[in] pz_SvcInfo    Logging service object.
 * @param[in] z_CltHandle   QMI client connection handle.
 * @param[in] e_Error       Connect operation result.
 *
 * @sa slim_QSLogT
 */
void slim_QSLogCSIConnect
(
  slim_QSLogT *pz_SvcInfo,
  qmi_client_handle z_CltHandle,
  qmi_csi_cb_error e_Error
);

/**
 * @brief Logs QMI client disconnect event.
 *
 * The function logs QMI client disconnect event.
 *
 * @param[in] pz_SvcInfo    Logging service object.
 * @param[in] z_CltHandle   QMI client connection handle.
 *
 * @sa slim_QSLogT
 */
void slim_QSLogCSIDisconnect
(
  slim_QSLogT *pz_SvcInfo,
  qmi_client_handle z_CltHandle
);

/**
 * @brief Logs encoded QMI message.
 *
 * The function logs unencoded QMI message. As the logging requires QMI message
 * encoding, the temporary buffer is used. In case there is a memory failure,
 * the message event is logged without message contents (only metadata).
 *
 * @param[in] pz_SvcInfo    Logging service object.
 * @param[in] z_CltHandle   QMI client connection handle.
 * @param[in] e_MsgType     QMI message type.
 * @param[in] z_ReqHandle   QMI transaction handle.
 * @param[in] q_MsgId       QMI message identifier.
 * @param[in] q_MsgLength   Size of encoded message payload.
 * @param[in] p_Msg         Optional encoded message payload.
 *
 * @sa slim_QSLogT
 */
void slim_QSLogCSIEncodedMessage
(
  slim_QSLogT *pz_SvcInfo,
  qmi_client_handle z_CltHandle,
  qmi_idl_type_of_message_type e_MsgType,
  qmi_req_handle z_ReqHandle,
  uint32 q_MsgId,
  uint32 q_MsgLength,
  const void *p_Msg
);

/**
 * @brief Logs QCCI notifier operation start.
 *
 * @param[in] z_ClientObject  QCCI client handle.
 * @param[in] z_ServiceObject QMI service object.
 * @param[in] l_Error         Error code.
 *
 * @sa slim_QSLogCCINotifierDeinit
 */
void slim_QSLogCCINotifierInit
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject,
  qmi_client_error_type l_Error
);

/**
 * @brief Logs QCCI notifier operation end.
 *
 * @param[in] z_ClientObject  QCCI client handle.
 * @param[in] z_ServiceObject QMI service object.
 * @param[in] l_Error         Error code.
 *
 * @sa slim_QSLogCCINotifierInit
 */
void slim_QSLogCCINotifierDeinit
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject,
  qmi_client_error_type l_Error
);

/**
 * @brief Logs QCCI notifier service increment event.
 *
 * @param[in] z_ClientObject  QCCI client handle.
 * @param[in] z_ServiceObject QMI service object.
 *
 * @sa slim_QSLogCCINotifierServiceDec
 */
void slim_QSLogCCINotifierServiceInc
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject
);

/**
 * @brief Logs QCCI notifier service decrement event.
 *
 * @param[in] z_ClientObject  QCCI client handle.
 * @param[in] z_ServiceObject QMI service object.
 *
 * @sa slim_QSLogCCINotifierServiceInc
 */
void slim_QSLogCCINotifierServiceDec
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject
);

/**
 * @brief Logs client connect event from QCCI side.
 *
 * @param[in] z_ClientObject    QCCI client handle.
 * @param[in] z_ServiceObject   QMI service object.
 * @param[in] q_ServiceInstance Service instance.
 * @param[in] l_Error           Error code.
 *
 * @sa slim_QSLogCCIClientDisconnect
 */
void slim_QSLogCCIClientConnect
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject,
  qmi_service_instance q_ServiceInstance,
  qmi_client_error_type l_Error
);

/**
 * @brief Logs client disconnect event from QCCI side.
 *
 * @param[in] z_ClientObject    QCCI client handle.
 * @param[in] z_ServiceObject   QMI service object.
 * @param[in] q_ServiceInstance Service instance.
 * @param[in] l_Error           Error code.
 *
 * @sa slim_QSLogCCIClientConnect
 */
void slim_QSLogCCIClientDisconnect
(
  qmi_client_type z_ClientObject,
  qmi_idl_service_object_type z_ServiceObject,
  qmi_service_instance q_ServiceInstance,
  qmi_client_error_type l_Error
);

/**
 * @brief Configures DM logging.
 *
 * @param[in] v_EnableEventLogging    Forward QCSI events to log.
 * @param[in] v_EnableMessageLogging  Forward QCSI messages to log.
 * @param[in] v_EnableNotifierLogging Forward QCCI notifier events to log.
 * @param[in] v_EnableClientLogging   Forward QCCI client events to log.
 */
void slim_QSLogConfigure
(
  boolean v_EnableEventLogging,
  boolean v_EnableMessageLogging,
  boolean v_EnableNotifierLogging,
  boolean v_EnableClientLogging
);

//! @}
//! @}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _QMISLIM_QMI_DM_LOG_H_INCLUDED_ */
