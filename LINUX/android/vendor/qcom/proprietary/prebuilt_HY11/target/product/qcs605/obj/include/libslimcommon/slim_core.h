#ifndef SLIM_CORE_H
#define SLIM_CORE_H
/**
  @file
  @brief This module initiates all requests to sensor services.

  SLIM core maintains all SLIM related data and provides services for both
  handling of SLIM client requests and provider data/responses.

  @ingroup slim_Core
*/

/*============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_core.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_provider_data.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_Core
//! @{

/*
  For details, check SLIM CORE version tracking QWiki page at:
  http://qwiki.qualcomm.com/qct-gpsone/SLIM/SLIM_CORE#Version

  When making any modification to any file in slim\common\core\ folder,
  please make appropriate changes at following places:
    1. Update SLIM CORE version tracking table at the Qwiki link above, and
    2. Update the SLIM CORE version id in slim_core.h header (this) file

  Updated:
    01/25/2017   -- initial version
*/
#define SLIM_CORE_VERSION_ID  (1)

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief This function returns SLIM-CORE version.

@return SLIM CORE version
*/
uint32_t slim_GetCoreVersion
(
  void
);

/**
@brief SLIM core initialization.

This function initializes SLIM module.

The initialization takes place before external services become available, and
initializes only internal structures without any providers.

The method shall be called after IPC facility is initialized for SLIM task,
but before other tasks could be ready.

@sa slim_CoreStart
*/
void slim_CoreInit
(
  void
);

/**
@brief SLIM core startup.

This function start SLIM core module and configures providers for different
SLIM services. No SLIM services are available before this function
is called.

This method shall be called after other subsystem startup is confirmed, so it
is safe to initiate requests to external (non-SLIM) tasks.

@sa slim_CoreInit
*/
void slim_CoreStart
(
  void
);

/**
@brief SLIM core configuration.

This function configures SLIM service providers using the given provider
setting.

@param[in] e_ProviderSetting Provider setting that should be used to configure
                             provider services.
*/
void slim_CoreUpdateConfiguration
(
  slim_ProviderSettingEnumType e_ProviderSetting
);

/**
@brief Engine function for opening client connection.

SLIM core implementation for opening client connection.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreOpenClientConnection
(
  const slim_OpenTxnStructType *pz_Txn
);

/**
@brief Engine function for closing client connection.

SLIM core implementation for closing client connection.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreCloseClientConnection
(
  const slim_CloseTxnStructType *pz_Txn
);

/**
@brief Engine function for forwarding message to provider implementation.

SLIM core implementation for forwarding message to provider implementation.

@param[in] e_Provider    Provider type.
@param[in] q_MessageId   Message id.
@param[in] q_MessageSize Payload size.
@param[in] p_Message     Pointer to the payload.
*/
void slim_CoreHandleProviderMessage
(
  slim_ProviderEnumType e_Provider,
  uint32 q_MessageId,
  uint32 q_MessageSize,
  const void *p_Message
);

/**
@brief Engine function for enabling sensor reporting.

SLIM core implementation for enabling sensor reporting.
Engine forwards the sensor reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableSensorDataRequest
(
  const slim_EnableSensorDataTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling motion data reporting.

SLIM core implementation for enabling motion data reporting.
Engine forwards the sensor reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableMotionDataRequest
(
  const slim_EnableMotionDataTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling pedometer reporting.

SLIM core implementation for enabling pedometer reporting.
Engine forwards the sensor reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnablePedometerRequest
(
  const slim_EnablePedometerTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling QMD data reporting.

SLIM core implementation for enabling QMD data reporting.
Engine forwards the QMD data reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableQmdDataRequest
(
  const slim_EnableQmdDataTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling SMD data reporting.

SLIM core implementation for enabling SMD data reporting.
Engine forwards the SMD data reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableSmdDataRequest
(
  const slim_EnableSmdDataTxnStructType *pz_Txn
);

/**
@brief Engine function for enable distance bound request.

SLIM core implementation for making distance bound request.
Engine forwards the distance bound request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableDistanceBoundRequest
(
  const slim_EnableDistanceBoundTxnStructType *pz_Txn
);

/**
@brief Engine function for set distance bound request.

SLIM core implementation for making distance bound request.
Engine forwards the distance bound request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreSetDistanceBoundRequest
(
  const slim_SetDistanceBoundTxnStructType *pz_Txn
);

/**
@brief Engine function for get distance bound request.

SLIM core implementation for making distance bound request.
Engine forwards the distance bound request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreGetDistanceBoundRequest
(
  const slim_GetDistanceBoundTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling vehicle data reporting.

SLIM core implementation for enabling vehicle data reporting.
Engine forwards the vehicle data reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableVehicleDataRequest
(
  const slim_EnableVehicleDataTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling pedestrian alignment data reporting.

SLIM core implementation for enabling pedestrian alignment data reporting.
Engine forwards the pedestrian alignment data reporting request to currently
active sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnablePedestrianAlignmentRequest
(
  const slim_EnablePedestrianAlignmentTxnStructType *pz_Txn
);

/**
@brief Engine function for enabling magnetic field data reporting.

SLIM core implementation for enabling magnetic field data reporting.
Engine forwards the reporting request to currently active
sensor service provider.

@param[in] pz_Txn Pointer to the request transaction data.
*/
void slim_CoreEnableMagneticFieldDataRequest
(
  const slim_EnableMagneticFieldDataTxnStructType *pz_Txn
);

/**
@brief Engine function for getting provider time.

SLIM core implementation for getting provider time.
Engine forwards the time request to the requested sensor service provider.

@param[in] e_Provider         Service provider
@param[in] e_Service          SLIM service
@param[in] t_ReferenceTime_ms Reference time for the request.
*/
void slim_CoreUpdateProviderTime
(
  slim_ProviderEnumType e_Provider,
  slimServiceEnumT      e_Service,
  uint64                t_ReferenceTime_ms
);

/**
@brief Notify function for provider configuration change.

SLIM providers should use this function to notify SLIM core when there has been
changes in the supported services by the provider.

@param[in] e_Provider    Sensor service provider the notification is originated
                         from.
@param[in] q_ServiceMask Services that were changed.
*/
void slim_CoreNotifyConfigurationChange
(
  slim_ProviderEnumType e_Provider,
  slimAvailableServiceMaskT q_ServiceMask
);

/**
@brief Notify function for request responses

SLIM providers should use this function to notify of request results. This
function should be used when the response has no payload.

@param[in] l_ServiceTxnId SLIM service transaction id.
@param[in] e_Provider     Sensor service provider the response is originated
                          from.
@param[in] e_MsgError     SLIM error code
*/
void slim_CoreNotifyResponseNoPayload
(
  int32 l_ServiceTxnId,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT e_MsgError
);

/**
@brief Notify function for request responses

SLIM providers should use this function to notify of request results.

@param[in] l_ServiceTxnId SLIM service transaction id.
@param[in] e_Provider     Sensor service provider the response is originated
                          from.
@param[in] e_MsgError     SLIM error code
@param[in] w_DataSize     Size of the payload.
@param[in] p_Data         Pointer to the payload.
*/
void slim_CoreNotifyResponse
(
  int32 l_ServiceTxnId,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT e_MsgError,
  uint16 w_DataSize,
  const void *p_Data
);

/**
@brief Notify function for time request response

SLIM providers should use this function to notify of time request results.

@param[in] l_ServiceTxnId    SLIM service transaction id.
@param[in] e_Provider        Sensor data provider sending the time data.
@param[in] e_MsgError        SLIM error code
@param[in] q_RemoteRxTime_ms Remote receive time.
@param[in] q_RemoteTxTime_ms Remote transmit time.
*/
void slim_CoreNotifyTimeResponse
(
  int32                 l_ServiceTxnId,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT        e_MsgError,
  uint32                q_RemoteRxTime_ms,
  uint32                q_RemoteTxTime_ms
);

/**
@brief Notify function for time request response

SLIM providers should use this function to notify of time request results.

@param[in] l_ServiceTxnId    SLIM service transaction id.
@param[in] e_Provider        Sensor data provider sending the time data.
@param[in] e_MsgError        SLIM error code
@param[in] t_RemoteRxTime_ms Remote receive time.
@param[in] t_RemoteTxTime_ms Remote transmit time.
*/
void slim_CoreNotifyTimeResponse64
(
  int32                 l_ServiceTxnId,
  slim_ProviderEnumType e_Provider,
  slimErrorEnumT        e_MsgError,
  uint64                t_RemoteRxTime_ms,
  uint64                t_RemoteTxTime_ms
);

/**
@brief Function for dispatching sensor data.

SLIM providers should use this function to send sensor data indications
to SLIM clients.

@param[in] p_Handle    Client handle.
@param[in] e_Provider  Sensor data provider.
@param[in] e_Service   SLIM service.
@param[in] e_Error     SLIM error.
@param[in] e_MessageId SLIM message id.
@param[in] w_DataSize  Size of the payload.
@param[in] p_Data      Pointer to the payload.
*/
void slim_CoreDispatchData
(
  slimClientHandleT p_Handle,
  slim_ProviderEnumType e_Provider,
  slimServiceEnumT e_Service,
  slimErrorEnumT e_Error,
  slimMessageIdEnumT e_MessageId,
  uint16 w_DataSize,
  const void *p_Data
);

/**
@brief Provides client handle for a pending transaction id.

Function provides client handle for a pending transaction id (if transaction
is found).

@param[in]  l_ServiceTxnId Transaction id.
@param[out] pp_HandleOut   Pointer to the SLIM client handle is stored to this
                           variable if the function succeeds.

@retval TRUE if the transaction is found.
*/
boolean slim_CoreClientHandleForTransactionId
(
  int32 l_ServiceTxnId,
  slimClientHandleT *pp_HandleOut
);

/**
@brief Queries core on pending events

@param[out] pt_EventTimeOut_ms Time of the next event.
@param[out] pq_EventParamOut   Parameter for the next event.

@retval TRUE  There is a pending event
@retval FALSE There are no pending events or error

@sa slim_CoreProcessTimerEvent
*/
boolean slim_CoreGetNextTimerEvent
(
  uint64 *pt_EventTimeOut_ms,
  uint32 *pq_EventParamOut
);

/**
@brief Performs execution of timer event handling

@param[in] q_EventParam Event parameter.

@sa slim_CoreGetNextTimerEvent
*/
void slim_CoreProcessTimerEvent
(
  uint32 q_EventParam
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_CORE_H */
