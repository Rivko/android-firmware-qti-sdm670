#ifndef QMISLIM_QMI_CLIENT_API_H
#define QMISLIM_QMI_CLIENT_API_H
/**
  @file
  @brief QMI-SLIM QMI Client API

  This header file defines API for QMI-SLIM QMI client object interface.

  @sa slim_QSQmiClientT
  @ingroup slim_QSQmiClient

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/qmislim/qmiclient/inc/qmislim_qmi_client_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_client_types.h"
#include "qmislim_qmi_api.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*!
  @brief Constant to indicate no timeout

  @sa slim_QSQmiClient_GetNextEventTime
  @ingroup slim_QSQmiClient
 */
#define SLIM_QS_QMI_CLIENT_EXPIRE_TIME_NEVER UINT64_MAX

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Type for QMI client request callback

  @sa qmi_client_recv_msg_async_cb
  @ingroup slim_QSQmiClient
 */
typedef void slim_QSQmiClientRecvMsgAsyncCbFnT
(
  qmi_client_type                user_handle,
  /**< [in] QMI handle */
  unsigned int                   msg_id,
  /**< [in] QMI message identifier */
  void                           *resp_c_struct,
  /**< [in] Decoded QMI response message */
  unsigned int                   resp_c_struct_len,
  /**< [in] Decoded QMI response message size in bytes */
  void                           *resp_cb_data,
  /**< [in] QMI client object */
  qmi_client_error_type          transp_err
  /**< [in] QMI transport error code if any */
);

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/
/*
 * ===========================================================================
 * slim_QSQmiClientCbT Interface
 * ===========================================================================
 */


/**
  @brief slim_QSQmiClientCbT interface type
  @ingroup slim_QSQmiClientCb
 */
typedef const slim_QSQmiClientCb_VtT * const slim_QSQmiClientCbT;

/**
  @brief Response callback notification.

  @param[in] pz_Iface     Called interface object.
  @param[in] pz_Client    Caller client instance.
  @param[in] p_UserCookie User cookie object supplied when making a request.
  @param[in] e_Service    Responding service. If not known, eSLIM_SERVICE_NONE.
  @param[in] e_Error      Error status, eSLIM_SUCCESS when no error.
  @param[in] e_MsgId      Message identifier.
  @param[in] p_Data       Optional data, if @a w_DataSize is not 0.
  @param[in] w_DataSize   Size of attached data.
  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnRespFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  void                *p_UserCookie,
  slimErrorEnumT       e_Error,
  slimMessageIdEnumT   e_MsgId,
  const void          *p_Data,
  uint16               w_DataSize
);
/**
  @brief Notification of service data or error

  @param[in] pz_Iface     Called interface object.
  @param[in] e_Service    Responding service. If not known, eSLIM_SERVICE_NONE.
  @param[in] e_Error      Error status, eSLIM_SUCCESS when no error.
  @param[in] e_MsgId      Message identifier.
  @param[in] p_Data       Optional data, if @a q_DataSize is not 0.
  @param[in] q_DataSize   Size of attached data.
  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnDataFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  slimServiceEnumT     e_Service,
  slimErrorEnumT       e_Error,
  slimMessageIdEnumT   e_MsgId,
  const void          *p_Data,
  uint32               q_DataSize
);
/**
  @brief Notification on time update result

  @param[in] pz_Iface     Called interface object.
  @param[in] p_UserCookie User cookie object supplied with time request.
  @param[in] e_Error      Error status, eSLIM_SUCCESS when no error.
  @param[in] t_RemoteRxTimeMs Request receival time in milliseconds.
  @param[in] t_RemoteTxTimeMs Response dispatch time in milliseconds.
  @sa slim_QSQmiClient_GetTimeUpdate
  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnTimeUpdateFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  void                *p_UserCookie,
  slimErrorEnumT       e_Error,
  uint64               t_RemoteRxTimeMs,
  uint64               t_RemoteTxTimeMs
);
/**
  @brief Subsystem error notification

  This method is called whenever QMI framework reports an error. Usual
  case is a peer disconnect, as transport errors usually reported using
  different means (i.e. request-response).

  @param[in] pz_Iface     Called interface object.
  @param[in] l_QmiErr     QMI error code.

  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnErrFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  int32                l_QmiErr
);
/**
  @brief Available service list change notification.

  The notification is received upon processing corresponding remote peer service
  change indication.

  @param[in] pz_Iface     Called interface object.
  @param[in] e_Service    Responding service.
  @param[in] q_ServiceProviderMask Service provider mask.

  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnServiceChangeFnT
(
  slim_QSQmiClientCbT     *pz_Iface,
  slimServiceEnumT         e_Service,
  slimServiceProviderMaskT q_ServiceProviderMask
);
/**
  @brief Disconnect completion notification

  @param[in] pz_Iface     Called interface object.

  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnDisconnectFnT
(
  slim_QSQmiClientCbT *pz_Iface
);
/**
  @brief Client registration report

  @param[in] pz_Iface     Called interface object.
  @param[in] e_Error      Error status, eSLIM_SUCCESS when no error.

  @sa slim_QSQmiClient_Register
  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnRegisteredFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  slimErrorEnumT       e_Error
);
/**
  @brief Client deregistration report

  @param[in] pz_Iface     Called interface object.
  @param[in] e_Error      Error status, eSLIM_SUCCESS when no error.

  @sa slim_QSQmiClient_Unregister
  @ingroup slim_QSQmiClientCb
 */
typedef void slim_QSQmiClientCb_OnUnregisteredFnT
(
  slim_QSQmiClientCbT *pz_Iface,
  slimErrorEnumT       e_Error
);

/**
 * @brief Virtual function table for slim_QSQmiClientCbT
 * @ingroup slim_QSQmiClientCb
 *
 */
struct slim_QSQmiClientCb_VtS
{
  slim_QSQmiClientCb_OnRespFnT          *pfn_OnResponse;
  /**< @brief Response received notification */
  slim_QSQmiClientCb_OnDataFnT          *pfn_OnData;
  /**< @brief Sensor data received notification */
  slim_QSQmiClientCb_OnTimeUpdateFnT    *pfn_OnTimeUpdate;
  /**< @brief Time update received notification */
  slim_QSQmiClientCb_OnErrFnT           *pfn_OnError;
  /**< @brief Subsystem error notification */
  slim_QSQmiClientCb_OnServiceChangeFnT *pfn_OnServiceChange;
  /**< @brief Available service list change notification */
  slim_QSQmiClientCb_OnDisconnectFnT    *pfn_OnDisconnect;
  /**< @brief Client disconnect notification */
  slim_QSQmiClientCb_OnRegisteredFnT    *pfn_OnRegistered;
  /**< @brief Client registration report */
  slim_QSQmiClientCb_OnUnregisteredFnT  *pfn_OnUnregistered;
  /**< @brief Client unregistration report */
};
/** @} */

/*
 * ===========================================================================
 * slim_QSQmiClientT Object
 * ===========================================================================
 */

/**
  @brief State enumeration

  QMI-SLIM client has the following main states:
  - @ref eSLIM_QS_QMI_CLIENT_STATE_DISCONNECTED
  - @ref eSLIM_QS_QMI_CLIENT_STATE_REGISTERED

  Other states are transitional.

  @ingroup slim_QSQmiClient
 */
typedef enum
{
  eSLIM_QS_QMI_CLIENT_STATE_DISCONNECTED,
  /**< @brief Client is not connected to service peer */
  eSLIM_QS_QMI_CLIENT_STATE_CONNECTED,
  /**< @brief Client is connected to service peer but not registered */
  eSLIM_QS_QMI_CLIENT_STATE_REGISTERING,
  /**< @brief Client is connected to service peer, registration is in progress */
  eSLIM_QS_QMI_CLIENT_STATE_REGISTERED,
  /**< @brief Client is connected to service peer and registered */
  eSLIM_QS_QMI_CLIENT_STATE_UNREGISTERING,
  /**< @brief Client is connected to service peer and unregistering */
  eSLIM_QS_QMI_CLIENT_STATE_DISCONNECTING,
  /**< @brief Asynchronous disconnect is in progress */
} slim_QSQmiClientStateT;

/**
  @brief Client transaction object

  The transaction object is used for tracking QMI request-response
  transactions, controlling timeouts and managing other transaction-related
  data,

  The following methods are applicable:
  - @ref QSQmiClientTx_Init
  - @ref QSQmiClientTx_Deinit

  @ingroup slim_QSQmiClient
 */
struct slim_QSQmiClientTxS
{
  SLIM_QS_LINK(slim_QSQmiClientTxT,z_Link);
  /**< @brief Link entry */
  slim_QSQmiClientT                    *pz_Client;
  /**< @brief Container */
  unsigned int                          q_MsgId;
  /**< @brief QMI message ID expected for response */
  qmi_txn_handle                        z_QmiTxnHandle;
  /**< @brief Internal QMI transaction handle */
  uint64                                t_ExpireTimeMs;
  /**< @brief Transaction expiration time */
  slim_QSQmiRespUnionT                  z_Resp;
  /**< @brief Buffer for handling response messages */
  void                                 *p_UserCookie;
  /**< @brief User-associated transaction data */
  slimMessageIdEnumT                    e_SlimMsgId;
  /**< @brief Corresponding SLIM message type for response */
  slim_QSQmiClientRecvMsgAsyncCbFnT    *pfn_CallbackFn;
  /**< @brief Function pointer for processing errors */
};

/**
  @brief QMI Client object for QMI-SLIM

  This structure manages QMI client operations for QMI-SLIM service.
  @ingroup slim_QSQmiClient
 */
struct slim_QSQmiClientS
{
  slim_QSMutexT                         z_Mutex;
  /**< @brief Mutex for state locks */
  slim_QSNodeT                          e_ThisNode;
  /**< @brief Type of this node, used for registration */
  slim_QSNodeT                          e_PeerNode;
  /**< @brief Type of remote node, used for service connection */

  volatile slim_QSQmiClientStateT       e_State;
  /*!< @brief Connection status */
  qmi_client_type                       pz_QmiHandle;
  /*!< @brief QMI server handle */
  qmi_idl_service_object_type           pz_QmiServiceObject;
  /*!< @brief QMI service object */
  qmi_client_os_params                  z_QmiOsParams;
  /*!< @brief QMI OS parameters */
  qmi_service_instance                  u_QmiServiceInstanceId;
  /*!< @brief QMI peer service instance id */
  qmi_service_info                      z_QmiServiceInfo;
  /**< @brief Last connected address */

  slim_QSQmiIndUnionT                   z_QmiInd;
  /**< @brief Buffer for decoding QMI indications */
  slim_QSSlimMessageDataT               z_SlimMsg;
  /**< @brief Buffer for decoding indication messages */

  slim_QSQmiClientCbT                  *pz_Callback;
  /*!< @brief Callback interface for reporting events */
  uint64                                t_TxTimeoutMs;
  /**< @brief Transaction timeout in milliseconds

   If transaction timeout expires, the transaction is cancelled with error.
   @sa SLIM_QS_DEFAULT_TX_TIMEOUT_MS
   */
  uint64                                t_ReconnectTimeoutMs;
  /**< @brief Reconnect timeout in milliseconds

   The reconnect timeout is used when the QCCI reports the service is
   available but connection fails.

   @sa SLIM_QS_DEFAULT_RECONNECT_TIMEOUT_MS
   */
  uint64                                t_ReconnectTimeMs;
  /**< @brief Time for reconnect attempt.

   The reconnect attempt time is set when the connection attempt fails though
   QCCI reports service availability.
  */

  SLIM_QS_LIST(slim_QSQmiClientTxT,z_Txs);
  /**< @brief List of opened transactions */
  /* QMI Client TX Object Pool */
  slim_QSQmiClientTxT z_BufferQCltTxs[SLIM_QS_ALLOCATOR_QMI_CLT_TX_COUNT];
  /**< @brief Static object buffer */
  SLIM_QS_LIST(slim_QSQmiClientTxT,z_FreeQCltTxs);
  /**< @brief Free object list */
};

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief QMI Client constructor.

  @retval TRUE on success
  @retval FALSE QMI error or connection is not available
  @ingroup slim_QSQmiClient
 */
boolean slim_QSQmiClient_Init
(
  slim_QSQmiClientT     *pz_Object,
  /**< [out] QMI Client object. */
  slim_QSNodeT           e_ThisNode,
  /**< [in] Local node type. */
  slim_QSNodeT           e_PeerNode,
  /**< [in] Peer node type. */
  slim_QSQmiClientCbT   *pz_Callback
  /**< [in] Callback interface for event notifications. */
);
/**
  @brief QMI Client destructor

  @retval TRUE on success
  @retval FALSE QMI error or connection is not available
  @ingroup slim_QSQmiClient
 */
boolean slim_QSQmiClient_Deinit
(
  slim_QSQmiClientT     *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Requests QMI Client to connect to service if available

  @retval TRUE on success
  @retval FALSE QMI error or connection is not available
  @ingroup slim_QSQmiClient
 */
boolean slim_QSQmiClient_Connect
(
  slim_QSQmiClientT *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Initiates disconnect procedure.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiClient
 */
boolean slim_QSQmiClient_Disconnect
(
  slim_QSQmiClientT *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Registers client with remote peer

  Before any request can be sent, the client must be registered with
  this call. If the call succeeds, the callback will be invoked when
  operation is completed.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_Register
(
  slim_QSQmiClientT     *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Unregisters client with remote peer

  Removes client registration (but keeps QMI connection).

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_Unregister
(
  slim_QSQmiClientT *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Sends time update request to peer.

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_GetTimeUpdate
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  uint64 t_CurrentMs,
  /**< [in] Current time for peer time conversion */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
);

/**
  @brief Helper method to disable any service

  This method allow direct service disabling-

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_DisableData
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  slimServiceEnumT e_Service,
  /**< [in] SLIM service to disable. */
  void *p_UserCookie,
  /**< [in] User cookie for identifying response. */
  slimMessageIdEnumT e_SlimMsgId
  /**< [in] SLIM message type for response. */
);

/**
  @brief Sends sensor data request to peer.

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_EnableSensorData
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  const slimEnableSensorDataRequestStructT *pz_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
);
/**
  @brief Sends pedometer request to peer.

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_EnablePedometer
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  const slimEnablePedometerRequestStructT *pz_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
);
/**
  @brief Sends AMD/RMD data request to peer

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_EnableQmdData
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  const slimEnableQmdDataRequestStructT *pz_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
);

/**
  @brief Sends vehicle data request to peer

  @return Error status, eSLIM_SUCCESS when no error.

  @ingroup slim_QSQmiClient
 */
slimErrorEnumT slim_QSQmiClient_EnableVehicleData
(
  slim_QSQmiClientT *pz_Object,
  /**< [in] QMI Client object. */
  const slimEnableVehicleDataRequestStructT *pz_Request,
  /**< [in] SLIM request data. */
  void *p_UserCookie
  /**< [in] User cookie for identifying response. */
);

/**
  @brief Checks if QMI client has any pending timed events.

  @return Time in milliseconds till next event check
  @retval SLIM_QS_QMI_CLIENT_EXPIRE_TIME_NEVER if there are no events

  @ingroup slim_QSQmiClient
 */
uint64 slim_QSQmiClient_GetNextEventTime
(
  slim_QSQmiClientT *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Performs pending event processing.

  @retval TRUE on success
  @retval FALSE on error

  @ingroup slim_QSQmiClient
 */
boolean slim_QSQmiClient_ProcessTimeEvents
(
  slim_QSQmiClientT *pz_Object
  /**< [in] QMI Client object. */
);
/**
  @brief Allocates QMI-SLIM QMI client transaction object
  @return Allocated object pointer or NULL
 */
slim_QSQmiClientTxT *slim_QSQmiClientTx_Alloc(void);
/**
  @brief Releases object
  @param[in] pz_Object Object pointer to release
 */
void slim_QSQmiClientTx_Free(slim_QSQmiClientTxT *pz_Object);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_QMI_CLIENT_API_H */

