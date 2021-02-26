/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief C++ implementation of base class for SLIM providers.
 *
 * This file defines the implementation of common methods shared
 * between service providers
 *
 * @ingroup slim_ProviderCommon
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <ProviderBase.h>

#include <slim_core.h>
#include <slim_task.h>

#include <inttypes.h>

#include <slim_os_log_api.h>

//! @addtogroup slim_ProviderCommon
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* This unit logs as MAIN component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief ProviderBase logs as MAIN module
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_MAIN

//! @}

using namespace slim;
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/**
@brief SLIM provider VTBL for providers with common time base.
@internal
*/
const slim_ServiceProviderInterfaceStructType ProviderBase::c_zVtblC =
{
  TRUE,
  slimSupportsService,
  slimOpenClientConnection,
  slimCloseClientConnection,
  slimGetMtOffsetForService,
  slimSetMtOffsetForService,
  slimHandleProviderMessage,
  slimGetTimeUpdate,
  slimEnableSensorDataRequest,
  slimEnableMotionDataRequest,
  slimEnablePedometerRequest,
  0 /* No support for QMD request */,
  0 /* No support for SMD request */,
  0 /* No support for distance bound request */,
  0 /* No support for distance bound request */,
  0 /* No support for distance bound request */,
  slimEnableVehicleDataRequest,
  0 /* No support for pedestrian alignment data */,
  0 /* No support for magnetic field data */,
  0 /* No support for cancel transaction request */
};
/**
@brief SLIM provider VTBL for providers with own time base.
@internal
*/
const slim_ServiceProviderInterfaceStructType ProviderBase::c_zVtblU =
{
  FALSE,
  slimSupportsService,
  slimOpenClientConnection,
  slimCloseClientConnection,
  slimGetMtOffsetForService,
  slimSetMtOffsetForService,
  slimHandleProviderMessage,
  slimGetTimeUpdate,
  slimEnableSensorDataRequest,
  slimEnableMotionDataRequest,
  slimEnablePedometerRequest,
  0 /* No support for QMD request */,
  0 /* No support for SMD request */,
  0 /* No support for distance bound request */,
  0 /* No support for distance bound request */,
  0 /* No support for distance bound request */,
  slimEnableVehicleDataRequest,
  0 /* No support for pedestrian alignment data */,
  0 /* No support for magnetic field data */,
  0 /* No support for cancel transaction request */
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * @brief Converts SLIM provider interface into object pointer
 *
 * @param[in] pzProvider Provider interface.
 *
 * @return Object pointer.
 */
ProviderBase *ProviderBase::castSlimInterface
(
  slim_ServiceProviderInterfaceType *pzProvider
)
{
  if (0 == pzProvider)
  {
    return 0;
  }
  ssize_t tOffset = (char*)&((ProviderBase*)1)->m_pzVtbl - (char*)1;
  return (ProviderBase*)((char*)pzProvider - tOffset);
}

/**
 * @brief Forwards call to instance method
 *
 * @param[in] pzProvider Provider interface.
 * @param[in] pHandle    SLIM client handle.
 *
 * @return Operation result
 */
slimErrorEnumT ProviderBase::slimOpenClientConnection
(
  slim_ServiceProviderInterfaceType *pzProvider,
  const slimClientHandleT pHandle
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject)
  {
    return pzObject->openClientConnection(pHandle);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
 * @brief Forwards call to instance method
 *
 * @param[in] pzProvider Provider interface.
 * @param[in] pHandle    SLIM client handle.
 *
 * @return Operation result
 */
slimErrorEnumT ProviderBase::slimCloseClientConnection
(
  slim_ServiceProviderInterfaceType *pzProvider,
  const slimClientHandleT pHandle
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject)
  {
    return pzObject->closeClientConnection(pHandle);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
 * @brief Forwards call to instance method
 *
 * @param[in] pzProvider      Provider interface.
 * @param[in] qMessageId      Message identifier.
 * @param[in] qMsgPayloadSize Message payload size.
 * @param[in] pMsgPayload     Message payload.
 */
void ProviderBase::slimHandleProviderMessage
(
  slim_ServiceProviderInterfaceType *pzProvider,
  uint32 q_MessageId,
  uint32 q_MsgPayloadSize,
  const void *p_MsgPayload
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject)
  {
    pzObject->handleProviderMessage(q_MessageId, q_MsgPayloadSize, p_MsgPayload);
  }
}

/**
@brief Initiates time offset request.

Function for making the time request. Succesfull response enable SLIM to
calculate the offset between modem time and sensor time.

@param  pz_Provider : Provider object
@param  l_ServiceTxnId : Service transaction id.
@return eSLIM_SUCCESS if time request is made successfully.
*/
slimErrorEnumT ProviderBase::slimGetTimeUpdate
(
  slim_ServiceProviderInterfaceType *pzProvider,
  int32 l_ServiceTxnId
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject)
  {
    return pzObject->getTimeUpdate(l_ServiceTxnId);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
@brief Forwards call to instance method
@return Operation result
@internal
*/
slimErrorEnumT ProviderBase::slimEnableSensorDataRequest
(
  slim_ServiceProviderInterfaceType *pzProvider,
  const slim_EnableSensorDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject && 0 != pz_Txn)
  {
    return pzObject->enableSensorDataRequest(*pz_Txn, l_ServiceTxnId);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
@brief Forwards call to instance method
@return Operation result
@internal
*/
slimErrorEnumT ProviderBase::slimEnablePedometerRequest
(
  slim_ServiceProviderInterfaceType *pz_Provider,
  const slim_EnablePedometerTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
)
{
  ProviderBase *pzObject = castSlimInterface(pz_Provider);
  if (0 != pzObject && 0 != pz_Txn)
  {
    return pzObject->enablePedometerRequest(*pz_Txn, l_ServiceTxnId);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
@brief Forwards call to instance method
@return Operation result
@internal
*/
slimErrorEnumT ProviderBase::slimEnableMotionDataRequest
(
  slim_ServiceProviderInterfaceType *pz_Provider,
  const slim_EnableMotionDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
)
{
  ProviderBase *pzObject = castSlimInterface(pz_Provider);
  if (0 != pzObject && 0 != pz_Txn)
  {
    return pzObject->enableMotionDataRequest(*pz_Txn, l_ServiceTxnId);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}
/**
@brief Forwards call to instance method
@return Operation result
@internal
*/
slimErrorEnumT ProviderBase::slimEnableVehicleDataRequest
(
  slim_ServiceProviderInterfaceType *pz_Provider,
  const slim_EnableVehicleDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
)
{
  ProviderBase *pzObject = castSlimInterface(pz_Provider);
  if (0 != pzObject && 0 != pz_Txn)
  {
    return pzObject->enableVehicleDataRequest(*pz_Txn, l_ServiceTxnId);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
@brief Returns TRUE if service is supported by the provider.

Function returns TRUE if service is supported by the provider.

@param[in] pz_Provider Provider object
@param[in] e_Service   SLIM service
@return TRUE if service is supported. FALSE otherwise.
*/
boolean ProviderBase::slimSupportsService
(
  slim_ServiceProviderInterfaceType *pzProvider,
  slimServiceEnumT e_Service
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject)
  {
    return pzObject->supportsService(e_Service);
  }
  return FALSE;
}

/**
@brief Provides timesync mt offset for the SLIM service.

Function provides timesync mt offset for the SLIM service.

@param[in]  pz_Provider Provider object
@param[in]  p_Handle    Pointer to the client handle
@param[in]  e_Service   SLIM service.
@param[out] pz_MtOffset Pointer to mt offset. Data is stored to this variable
                        if found.
@return TRUE if mt data was found. FALSE otherwise.
*/
boolean ProviderBase::slimGetMtOffsetForService
(
  slim_ServiceProviderInterfaceType *pzProvider,
  const slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slim_ServiceMtDataStructType *pz_MtOffset
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject && 0 != pz_MtOffset)
  {
    return pzObject->getMtOffsetForService(p_Handle, e_Service, *pz_MtOffset);
  }
  return FALSE;
}

/**
@brief Sets timesync mt offset for the SLIM service.

Function sets timesync mt offset for the SLIM service.

@param[in] pz_Provider  Provider object
@param[in] p_Handle     Pointer to the client handle. If NULL, data is applied
                        to all clients.
@param[in] e_Service    SLIM service. If eSLIM_SERVICE_NONE, data is applied
                        to all services.
@param[in] pz_MtOffset Pointer to mt offset to be set.
@param[in] b_SetForAllClients TRUE if data should be applied to all clients.
                              In this case parameter p_Handle is ignored.
                              Otherwise FALSE.
*/
void ProviderBase::slimSetMtOffsetForService
(
  slim_ServiceProviderInterfaceType *pzProvider,
  const slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  const slim_ServiceMtDataStructType *pz_MtOffset,
  boolean b_SetForAllClients
)
{
  ProviderBase *pzObject = castSlimInterface(pzProvider);
  if (0 != pzObject && 0 != pz_MtOffset)
  {
    pzObject->setMtOffsetForService(p_Handle, e_Service, *pz_MtOffset, b_SetForAllClients);
  }
}

/**
@brief Constructs provider

Method initializes SLIM API function table and sets provider type and
initial set of enabled services.

@param[in] eProvider    Type of the provider.
@param[in] uCommonTime  When @a true provider reports all data using SLIM
                        Core timer API.
@param[in] eServiceMask Initial set of available services for this provider.
*/
ProviderBase::ProviderBase
(
  slim_ProviderEnumType eProvider,
  bool uCommonTime,
  slimAvailableServiceMaskT eServiceMask
) :
  m_eProvider(eProvider),
  m_pzVtbl(uCommonTime ? &c_zVtblC : &c_zVtblU),
  m_qServiceMask(eServiceMask)
{
}

/**
@brief Terminates provider operation.
*/
ProviderBase::~ProviderBase()
{
}

/**
@brief Route function for provider messages

Provider modules should use this function to route messages to self through
SLIM-AP daemon task.

@param[in] qMessageId Provider-specific message identifier.
@param[in] qSize      Data size.
@param[in] pData      Data to route (can be 0).
@retval Operation status; eSLIM_SUCCESS if routed.
*/
slimErrorEnumT ProviderBase::routeMessageToSelf
(
  uint32_t qMessageId,
  uint32_t qSize,
  const void *pData
)
{
  if (0 == slim_SendProviderIpcToTask(
      m_eProvider, qMessageId, pData, qSize))
  {
    return eSLIM_SUCCESS;
  }
  return eSLIM_ERROR_IPC_COMMUNICATION;
}

/**
@brief Initiates sensor data request.

Function initiates sensor data request. If request is successful, sensor
data streaming is either started or stopped.

@param[in]  zTxn   Transaction data.
@param[in]  lTxnId Service transaction id.
@return eSLIM_SUCCESS if sensor data request is made successfully.
*/
slimErrorEnumT ProviderBase::enableSensorDataRequest
(
  const slim_EnableSensorDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  SLIM_UNUSED(zTxn);
  SLIM_UNUSED(lTxnId);

  SLIM_LOGE("Provider doesn't implement sensor data.");

  return eSLIM_ERROR_UNSUPPORTED;
}

/**
@brief Initiates pedometer request.

Function initiates pedometer request. If request is successful, pedometer
data streaming is either started or stopped.

@param[in]  zTxn   Transaction data.
@param[in]  lTxnId Service transaction id.
@return eSLIM_SUCCESS if sensor data request is made successfully.
*/
slimErrorEnumT ProviderBase::enablePedometerRequest
(
  const slim_EnablePedometerTxnStructType &zTxn,
  int32_t lTxnId
)
{
  SLIM_UNUSED(zTxn);
  SLIM_UNUSED(lTxnId);
  return eSLIM_ERROR_UNSUPPORTED;
}

/**
@brief Initiates motion data request.

Function initiates motion request. If request is successful, motion data
streaming is either started or stopped.

@param[in]  zTxn   Transaction data.
@param[in]  lTxnId Service transaction id.
@return eSLIM_SUCCESS if sensor data request is made successfully.
*/
slimErrorEnumT ProviderBase::enableMotionDataRequest
(
  const slim_EnableMotionDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  SLIM_UNUSED(zTxn);
  SLIM_UNUSED(lTxnId);
  return eSLIM_ERROR_UNSUPPORTED;
}

/**
@brief Initiates vehicle data request.

Function initiates vehicle sensor or odometry request. If request is
successful, vehicle data streaming is either started or stopped.

@param[in]  zTxn   Transaction data.
@param[in]  lTxnId Service transaction id.
@return eSLIM_SUCCESS if sensor data request is made successfully.
*/
slimErrorEnumT ProviderBase::enableVehicleDataRequest
(
  const slim_EnableVehicleDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  SLIM_UNUSED(zTxn);
  SLIM_UNUSED(lTxnId);
  return eSLIM_ERROR_UNSUPPORTED;
}

/**
@brief Tests if service is supported by the provider.

Function returns @a true if service is supported by the provider.

@param[in] eService   SLIM service
@return true if service is supported. false otherwise.
*/
bool ProviderBase::supportsService
(
  slimServiceEnumT eService
)
{
  if (SLIM_MASK_IS_SET(m_qServiceMask, eService))
  {
    return true;
  }

  return false;
}

/**
@brief Forwards configuration change event to SLIM core.

@param[in] qChangedServiceMask Modified services.
@sa routeConfigurationChange
*/
void ProviderBase::dispatchConfigurationChange
(
  slimAvailableServiceMaskT qChangedServiceMask
)
{
  slim_CoreNotifyConfigurationChange(m_eProvider, qChangedServiceMask);
}

/**
@brief Handler for messages targeted for provider.

Function handles messages sent via SLIM core to provider.

@param[in] qMessageId      Message id.
@param[in] qMsgPayloadSize Message size.
@param[in] pMsgPayload     Pointer to the message payload.
*/
void ProviderBase::handleProviderMessage
(
  uint32_t qMessageId,
  uint32_t qMsgPayloadSize,
  const void *pMsgPayload
)
{
  SLIM_LOG_ENTRY();

  switch (qMessageId)
  {
  case eMSG_ID_CONFIGURATION_CHANGE:
    if (sizeof(slimAvailableServiceMaskT) == qMsgPayloadSize)
    {
      slimAvailableServiceMaskT qChangedServiceMask = 0;
      slim_Memscpy(&qChangedServiceMask, sizeof(qChangedServiceMask),
          pMsgPayload, qMsgPayloadSize);
      dispatchConfigurationChange(qChangedServiceMask);
    }
    break;
  default:
    break;
  }
}

/**
@brief Sends a configuration change message to self.

Provider modules should use this function to route messages to self through
SLIM-AP daemon task.

@param[in] qChangedServiceMask Modified services.

@return Operation result.
@retval eSLIM_SUCCESS The message has been successfully routed.

@sa #dispatchConfigurationChange
*/
slimErrorEnumT ProviderBase::routeConfigurationChange
(
  slimAvailableServiceMaskT qChangedServiceMask
)
{
  SLIM_LOG_ENTRY();
  if (0 == slim_SendProviderIpcToTask(
      m_eProvider,
      eMSG_ID_CONFIGURATION_CHANGE,
      &qChangedServiceMask,
      sizeof(qChangedServiceMask)))
  {
    return eSLIM_SUCCESS;
  }
  return eSLIM_ERROR_IPC_COMMUNICATION;
}


/**
 * @brief Dispatch function for time response.
 *
 * AP provider modules should use this function to dispatch time responses
 * to SLIM core.
 *
 * @param[in] lTxId           Transaction number.
 * @param[in] tRemoteRxTimeMs Remote provider request receive time in ms.
 * @param[in] tRemoteTxTimeMs Remote provider response dispatch time in ms.
 */
void ProviderBase::dispatchTimeResponse
(
  int32_t lTxId,
  uint64_t tRemoteRxTimeMs,
  uint64_t tRemoteTxTimeMs
)
{
  SLIM_LOG_ENTRY();

  slim_CoreNotifyTimeResponse64(
      lTxId,
      m_eProvider,
      eSLIM_SUCCESS,
      tRemoteRxTimeMs,
      tRemoteTxTimeMs);
}

/**
 * @brief Dispatch function for error time response.
 *
 * AP provider modules should use this function to dispatch time responses
 * to SLIM core.
 *
 * @param[in] lTxId           Transaction number.
 * @param[in] eError          Error code.
 */
void ProviderBase::dispatchTimeResponse
(
  int32_t lTxId,
  slimErrorEnumT eError
)
{
  SLIM_LOG_ENTRY();

  slim_CoreNotifyTimeResponse64(lTxId, m_eProvider, eError, 0, 0);
}
/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
