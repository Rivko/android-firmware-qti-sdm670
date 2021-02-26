#ifndef SLIM_QMISLIM_PROVIDER_H
#define SLIM_QMISLIM_PROVIDER_H
/**
  @file
  @brief QMI-SLIM QMI Service API
  @ingroup slim_QS
*/
/*
  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
 */

/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/provider/qmislim/inc/qmislim_provider_api.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"
#include "slim_service_status.h"

#include "qmislim_common_api.h"
#include "qmislim_qmi_client_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

//! Registry of QMI-SLIM service provider objects
/*!
  Registry contains list of supported QMI-SLIM service providers and handles
  message forwarding from IPC tasks and provider objects.

  @sa slim_QSProviderT Contained provider objects
  @sa slim_QSProviderCore_NewProvider()
  @sa slim_QSProviderCore_DeleteProvider
  @sa slim_QSProviderCore_ProcessServiceAdd
  @sa slim_QSProviderCore_ProcessServiceRemove
  @sa slim_QSProviderCore_GetNextTimerEvent
  @sa slim_QSProviderCore_ProcessTimerEvent

  @ingroup slim_QSProvider
 */
typedef struct slim_QSProviderCoreS
{
  SLIM_QS_LIST(struct slim_QSProviderS,z_Providers);
  /**< @brief Field for provider object list */
  slim_QSNodeT         e_ThisNode;
  /**< @brief Identifier of the current node */
} slim_QSProviderCoreT;

//! Implementation of QMI-SLIM Service Provider
/*!
  QMI-SLIM service provider implements two interfaces:
  slim_ServiceProviderInterfaceT and  slim_QSQmiClientCbT.

  The execution context of SLIM service provider interfaces:
  - Context of SLIM task (all SLIM Provider methods) and life cycle methods.
  - Context of QMI Client callback (can be either context of SLIM task
    or context of QMI Client callback thread.

  @sa slim_ServiceProviderInterfaceT Implemented interface
  @sa slim_QSQmiClientCbT            Implemented interface

  @sa slim_QSProviderTxS             Contained objects

  @sa slim_QSProvider_Init()
  @sa slim_QSProvider_Deinit()
  @sa slim_QSProvider_TryConnect()
  @sa slim_QSProvider_GetNextTimerEvent()
  @sa slim_QSProvider_ProcessTimerEvent()

  @ingroup slim_QSProvider
 */
struct slim_QSProviderS
{
  SLIM_QS_LINK(slim_QSProviderT,z_Link);
  /**< @brief Containment list field */

  const slim_ServiceProviderInterfaceStructType *p_Vtbl;
  /**< @brief SLIM service Provider virtual function table */

  slim_QSMutexT z_Mutex;
  /**< @brief Mutex for state locking */

  slim_QSNodeT e_PeerNode;
  /**< @brief Type of the remote node.
   * This value controls QMI service selection by QMI client */
  slim_ProviderEnumType e_ProviderType;
  /**< @brief Type of the provider. Corresponds to @a e_PeerNode */
  SLIM_QS_LIST(slim_QSProviderTxT,z_Txs);
  /**< @brief List of opened transactions */

  slim_QSQmiClientT z_QmiClient;
  /**< @brief QMI Client (@ref slim_QSQmiClient) */
  const slim_QSQmiClientCb_VtT *pz_QmiClientCb;
  /**< @brief Virtual function table for @ref slim_QSQmiClientCb */

  volatile slimServiceProviderMaskT z_SPMasks[eSLIM_SERVICE_LAST + 1];
  /**< @brief Arrays for service provider masks */

  slim_ServiceStatusStructType z_ServiceStatus[eSLIM_SERVICE_LAST + 1];
  /**< @brief Service status entry table */
  slim_ProviderStatusStructType z_ProviderStatus;
  /**< @brief Provider status to keep track of service state. */
};
/**
 * @name Transaction Operations
 * @{
 */
/*!
 * @brief Transaction object for service provider
 *
 * Service provider manages list of opened transactions mainly for error
 * handling, as handling disconnects and timeouts.
 *
 * This type also abstracts SLIM Service provider API from QMI-SLIM QMI Client
 * API.
 *
 * @ingroup slim_QSProvider
 */
struct slim_QSProviderTxS
{
  SLIM_QS_LINK(slim_QSProviderTxT,z_Link);
  /**< @brief fields for maintaining containment list */
  int32 l_TxId;
  /**< @brief Transaction identifier supplied by SLIM */
  slimClientHandleT p_ClientHandle;
  /**< @brief Client handle or NULL if the transaction is not client-bound */
  slimMessageIdEnumT e_RespMsgId;
  /**< @brief Expected response message type for handling error cases */
  slimServiceEnumT e_Service;
  /**< @brief Service for response */
  uint16 w_ReportRateHz;
  /**< @brief Pre-transaction report rate */
  uint16 w_SampleCount;
  /**< @brief Pre-transaction sample size */
  uint16 w_ClientReportRateHz;
  /**< @brief Transaction report rate */
  uint16 w_ClientSampleCount;
  /**< @brief Transaction sample size */
  boolean b_EnableRequest;
  /**< @brief If transaction is for service enable or disable */
  boolean b_RateRequest;
  /**< @brief If the request is a rate request (multiplexing) */
  uint64_t t_RequestTimeMs;
  /**< Transaction start time in milliseconds */
};
/**
 * @}
 */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initialize subsystem.

  This method performs initialization of all global subsystem data including
  allocator facilities.

  This call must be made before usage of any other QMI-SLIM provider
  facilities.

  @retval TRUE on success
  @retval FALSE on error

  @sa slim_QSProviderCore_Deinit()
  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_Init
(
  slim_QSProviderCoreT *pz_Object,
  /**< [in] Provider registry object */
  slim_QSNodeT e_ThisNode
  /**< [in] Identifier of the node */
);
/**
  @brief Releases QMI-SLIM provider facilities.

  The call must be made if there is a need to clean up subsystem data. The
  call will fail if there are opened provider interfaces or active clients.

  @retval TRUE on success
  @retval FALSE on error

  @sa slim_QSProviderCore_Init()
  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_Deinit
(
  slim_QSProviderCoreT *pz_Object
  /**< [in] Provider registry object */
);

/**
  @brief Allocates and initializes SLIM service provider.

  Method allocates new instance of QMI-SLIM provider object, initializes it
  with appropriate parameters and returns SLIM provider interface pointer (not
  the object pointer).

  Upon initialization the provider is in disconnected state and doesn't have
  any service available.

  @return SLIM service provider interface or NULL on error
  @sa slim_QSProvider

  @ingroup slim_QSProviderCore
 */
slim_ServiceProviderInterfaceType *slim_QSProviderCore_NewProvider
(
  slim_QSProviderCoreT *pz_Object,
  /**< [in] Provider registry object */
  slim_QSNodeT   e_PeerNode
  /**< [in] Remote node identifier */
);
/**
  @brief Releases service provider.

  Method releases all resources associated with service provider. SLIM core
  will no longer receive any notifications even if there are pending
  transactions.

  Remote peer will be notified on provider removal asynchronously.

  @retval TRUE on success
  @retval FALSE on error
  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_DeleteProvider
(
  slim_QSProviderCoreT *pz_Object,
  /**< [in] Provider registry object */
  slim_ServiceProviderInterfaceType *pz_Iface
  /**< [in] Provider object interface */
);
/**
  @brief Handle QMI-SLIM service registration event

  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_ProcessServiceAdd
(
  slim_QSProviderCoreT *pz_Object
  /**< [in] Provider registry object */
);
/**
  @brief Handle QMI-SLIM service removal event

  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_ProcessServiceRemove
(
  slim_QSProviderCoreT *pz_Object
  /**< [in] Provider registry object */
);
/**
  @brief Queries service provider core on pending events

  The method queries all constructed providers if they have any pending timed
  events as transactions or timed reconnect requests. If there are any, a
  next closest event time is returned.

  @retval TRUE There is a pending event
  @retval FALSE There are no pending events or error
  @sa slim_QSProviderCore_ProcessTimerEvent
  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_GetNextTimerEvent
(
  slim_QSProviderCoreT *pz_Object,
  /**< [in] Provider registry object */
  uint64 *pt_EventTimeMs,
  /**< [out] Time of the next event */
  uint32 *pq_EventParam
  /**< [out] Parameter for the next event */
);
/**
  @brief Performs execution of timer event handling

  When system reaches time reported by @ref slim_QSProviderCore_GetNextTimerEvent
  method, it shall invoke this method. The method fires timed event processing
  for all constructed providers.

  @sa slim_QSProviderCore_GetNextTimerEvent
  @ingroup slim_QSProviderCore
 */
boolean slim_QSProviderCore_ProcessTimerEvent
(
  slim_QSProviderCoreT *pz_Object,
  /**< [in] Provider registry object */
  uint32 q_EventParam
  /**< [in] Event parameter */
);

#ifdef __cplusplus
}
#endif

#endif /* SLIM_QMISLIM_PROVIDER_H */

