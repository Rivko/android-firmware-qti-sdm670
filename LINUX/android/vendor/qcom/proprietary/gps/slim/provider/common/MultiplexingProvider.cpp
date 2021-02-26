/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief Definition of base class for multiplexing providers.
 *
 * This file defines the implementation of common methods shared
 * between sensor providers.
 *
 * @ingroup slim_ProviderCommon
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <MultiplexingProvider.h>

#include <slim_core.h>
#include <slim_task.h>

#include <inttypes.h>

#include <slim_os_log_api.h>
#include <loc_pla.h>
#include <loc_cfg.h>

//! @addtogroup slim_ProviderCommon
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* This unit logs as MAIN component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Multiplexing provider logs as MAIN module
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_MAIN

//! @}

using namespace slim;
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * @brief Protected constructor
 *
 * @param[in] eProvider    Type of the provider.
 * @param[in] uCommonTime  When @a true provider reports all data using SLIM
 *                         Core timer API.
 * @param[in] eServiceMask Initial set of available services for this provider.
 */
MultiplexingProvider::MultiplexingProvider
(
  slim_ProviderEnumType eProvider,
  bool uCommonTime,
  slimAvailableServiceMaskT eServiceMask
) :
  ProviderBase(eProvider, uCommonTime, eServiceMask),
  m_zServiceStatus(),
  m_zProviderStatus()
{
  memset(m_zServiceStatus, 0, sizeof(m_zServiceStatus));
  memset(&m_zProviderStatus, 0, sizeof(m_zProviderStatus));
  m_zProviderStatus.pz_StatusArray = m_zServiceStatus;
  m_zProviderStatus.q_ServiceCount = SLIM_SERVICE_COUNT;
  slim_ServiceStatusInitialize(
      &m_zProviderStatus,
      SLIM_SERVICE_COUNT,
      (1u << SLIM_SERVICE_COUNT) - 1);
}

/**
 * @brief Updates client registration status and notifies SLIM core of the
 *        successful enable request.
 *
 * Function updates client registration status and notifies SLIM core of the
 * successful enable request.
 *
 * @param[in] pHandle             SLIM client handle.
 * @param[in] bEnable             TRUE if service was enabled.
 * @param[in] wBatchRateHz        Used batch rate (for device sensor data).
 * @param[in] wSampleCount        Used sample count (for device sensor data).
 * @param[in] wClientBatchRateHz  Client batch rate (for device sensor data).
 * @param[in] wClientSampleCount  Client sample count (for device sensor data).
 * @param[in] lServiceTxnId       Service transaction id.
 * @param[in] eService            SLIM service.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::notifySuccessfulEnableRequestForRate
(
  slimClientHandleT pHandle,
  bool bEnable,
  uint16_t wBatchRateHz,
  uint16_t wSampleCount,
  uint16_t wClientBatchRateHz,
  uint16_t wClientSampleCount,
  int32_t lServiceTxnId,
  slimServiceEnumT eService
)
{
  /* Set new service status */
  slim_ServiceStatusEnableForRate(
      &m_zProviderStatus,
      eService,
      bEnable,
      wBatchRateHz,
      wSampleCount,
      wClientBatchRateHz,
      wClientSampleCount,
      pHandle);

  /* Send response to client */
  return routeGenericResponse(lServiceTxnId, eSLIM_SUCCESS);
}

/**
 * @brief Updates client registration status and notifies SLIM core of the
 * successful enable request.
 *
 * Function updates client registration status and notifies SLIM core of the
 * successful enable request.
 *
 * @param  pHandle       SLIM client handle.
 * @param  bEnable       TRUE if service was enabled.
 * @param  lServiceTxnId Service transaction id.
 * @param  eService      SLIM service.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::notifySuccessfulEnableRequest
(
  slimClientHandleT pHandle,
  bool bEnable,
  int32_t lServiceTxnId,
  slimServiceEnumT eService
)
{
  /* Set new service status */
  slim_ServiceStatusEnable(&m_zProviderStatus, eService, bEnable, pHandle);

  /* Send response to client */
  return routeGenericResponse(lServiceTxnId, eSLIM_SUCCESS);
}

/**
 * @brief Handler for messages targeted for provider.
 *
 * Function handles messages sent via SLIM core to provider.
 *
 * @param[in] qMessageId      Message id.
 * @param[in] qMsgPayloadSize Message size.
 * @param[in] pMsgPayload     Pointer to the message payload.
 */
void MultiplexingProvider::handleProviderMessage
(
  uint32_t qMessageId,
  uint32_t qMsgPayloadSize,
  const void *pMsgPayload
)
{

  SLIM_LOG_ENTRY();

  SLIM_LOGD("Got message: id=%" PRIu32 " size=%" PRIu32, qMessageId, qMsgPayloadSize);

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
  case eMSG_ID_SENSOR_DATA_INJECT:
  case eMSG_ID_PEDOMETER_INJECT:
  case eMSG_ID_MOTION_DATA_INJECT:
  case eMSG_ID_VEHICLE_ODOMETRY_DATA_INJECT:
  case eMSG_ID_VEHICLE_SENSOR_DATA_INJECT:
  case eMSG_ID_VEHICLE_MOTION_DATA_INJECT:
  {
    if (sizeof(IndicationDataMsg) == qMsgPayloadSize)
    {
      IndicationDataMsg zMessage;
      slim_Memscpy(&zMessage, sizeof(zMessage), pMsgPayload, qMsgPayloadSize);
      dispatchIndication(zMessage);
    }
    break;
  }
  case eMSG_ID_GENERIC_SERVICE_RESPONSE:
  {
    ResponseDataMsg z_Message;
    slim_Memscpy(&z_Message, sizeof(z_Message), pMsgPayload, qMsgPayloadSize);
    dispatchGenericResponse(z_Message);
    break;
  }
  case eMSG_ID_TIME_SYNC_DATA_INJECT:
  {
    ResponseDataMsg z_Message;
    slim_Memscpy(&z_Message, sizeof(z_Message), pMsgPayload, qMsgPayloadSize);
    dispatchTimeResponse(z_Message);
    break;
  }
  default:
    SLIM_LOGE("Received unsupported message %" PRIu32, qMessageId);
    break;
  }
}

/**
@brief Opens provider connection for specific client.

Function opens provider connection for a specific client.

@param[in] pHandle Pointer to the client handle.
@return eSLIM_SUCCESS if connection is opened successfully.
*/
slimErrorEnumT MultiplexingProvider::openClientConnection
(
  slimClientHandleT pHandle
)
{
  SLIM_LOG_ENTRY();

  slim_ServiceStatusAddClient(&m_zProviderStatus, pHandle);

  return eSLIM_SUCCESS;
}

/**
 * @brief Closes provider connection for specific client.
 *
 * Function closes sensor data provider connection for a specific client and
 * frees all allocated resources.
 *
 * @param[in] pHandle Client handle.
 *
 * @return eSLIM_SUCCESS if connection is closed successfully.
 */
slimErrorEnumT MultiplexingProvider::closeClientConnection
(
  slimClientHandleT pHandle
)
{
  SLIM_LOG_ENTRY();

  slim_ServiceStatusRemoveClient(&m_zProviderStatus, pHandle);

  return eSLIM_SUCCESS;
}

/**
 * @brief Initiates time offset request.
 *
 * Function for making the time request. Successful response enable SLIM to
 * calculate the offset between modem time and sensor time.
 *
 * @param[in] lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if time request is made successfully.
 */
slimErrorEnumT MultiplexingProvider::getTimeUpdate
(
  int32_t lTxnId
)
{
  SLIM_LOG_ENTRY();

  SLIM_LOGD("Requesting time update: txn=%" PRId32, lTxnId);

  uint64_t tProviderTimeMs = slim_TimeTickGetMilliseconds();
  return routeTimeResponse(
      lTxnId, eSLIM_SUCCESS, tProviderTimeMs, tProviderTimeMs);
}

/*!
 * @brief Initiates sensor data request.
 *
 * Function initiates sensor data request. If request is successful, sensor
 * data streaming is either started or stopped.
 *
 * The method uses multiplexing services and implementation class shall
 * provide #doUpdateSensorStatus(bool,uint16_t,uint16_t,slimServiceEnumT) method
 * and use #routeIndication(slimServiceEnumT,const slimSensorDataStructT&) for
 * data reporting.
 *
 * @param[in]  zTxn   Transaction data.
 * @param[in]  lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if sensor data request is made successfully.
 */
slimErrorEnumT MultiplexingProvider::enableSensorDataRequest
(
  const slim_EnableSensorDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  slimErrorEnumT eRetVal = eSLIM_SUCCESS;
  boolean bSendRequest = FALSE;
  uint16_t wReportRateHz = 0;
  uint16_t wSampleCount = 0;
  uint16_t wClientReportRateHz = 0;
  uint16_t wClientSampleCount = 0;
  boolean bSendEnableRequest = zTxn.z_Request.enableConf.enable;

  SLIM_LOG_ENTRY();

  if (0 == SLIM_MASK_IS_SET(m_qServiceMask, zTxn.z_Request.sensor))
  {
    SLIM_LOGE("Service %d is not enabled", zTxn.z_Request.sensor);
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }

  if (zTxn.z_Request.enableConf.enable)
  {
    wReportRateHz = zTxn.z_Request.reportRate;
    wSampleCount = zTxn.z_Request.sampleCount;
    wClientReportRateHz = zTxn.z_Request.reportRate;
    wClientSampleCount = zTxn.z_Request.sampleCount;

    slim_ServiceStatusSetTimeBatching(&m_zProviderStatus,
                                      zTxn.z_Request.sensor,
                                      wReportRateHz,
                                      wSampleCount,
                                      zTxn.z_Request.batchAlignReq.batchStartTime,
                                      zTxn.z_TxnData.p_Handle);
  }
  else
  {
    /* In case of Disable Sensor Data clear all buffers */
    cleanBatchingBuffers();
    SLIM_LOGD("Going to call slim_ServiceStatusResetBatchStartTime ");
    slim_ServiceStatusResetTimeBatching(&m_zProviderStatus,
                                        zTxn.z_Request.sensor,
                                        zTxn.z_TxnData.p_Handle);
  }

  SLIM_LOGI("Enable sensor data: sensor=%u enable=%u report rate=%" PRIu16
            " sample count=%" PRIu16,
      zTxn.z_Request.sensor, zTxn.z_Request.enableConf.enable,
      (uint16_t)wClientReportRateHz, (uint16_t)wClientSampleCount);

  /* Always the highest requested rate */
  slim_ServiceStatusGetSampleRate(
    &m_zProviderStatus,
    zTxn.z_Request.sensor,
    zTxn.z_TxnData.p_Handle,
    &wReportRateHz,
    &wSampleCount);

  /* Check if request is valid for this client. */
  eRetVal = slim_ServiceStatusCanEnableForRate(
    &m_zProviderStatus,
    zTxn.z_TxnData.p_Handle,
    zTxn.z_Request.sensor,
    zTxn.z_Request.enableConf.enable,
    wReportRateHz,
    wSampleCount,
    &bSendRequest,
    &bSendEnableRequest);

  if (eSLIM_SUCCESS == eRetVal)
  {
    /* Send request if it is not already sent. */
    if (bSendRequest)
    {
      eRetVal = doUpdateSensorStatus(
          bSendEnableRequest,
          wReportRateHz,
          wSampleCount,
          zTxn.z_Request.sensor);
    }
    else
    {
      eRetVal = eSLIM_SUCCESS;
    }

    if (eSLIM_SUCCESS == eRetVal)
    {
      /* Error is notified by SLIM core so notify only success to client here.
      */
      notifySuccessfulEnableRequestForRate(
          zTxn.z_TxnData.p_Handle,
          zTxn.z_Request.enableConf.enable,
          wReportRateHz,
          wSampleCount,
          wClientReportRateHz,
          wClientSampleCount,
          lTxnId,
          zTxn.z_Request.sensor);
    }
    else
    {
      SLIM_LOGE("Failed enabling sensor service %d. Error=%d",
                zTxn.z_Request.sensor, eRetVal);
    }
    /*Enable batching if requested */
    if (zTxn.z_Request.batchAlignReq_valid)
    {
        slim_ServiceStatusSetBatchingEnabled(&m_zProviderStatus, zTxn.z_Request.sensor, FALSE);
    }

  }
  return eRetVal;
}

/**
 * @brief Initiates pedometer request.
 *
 * Function initiates pedometer request. If request is successful, pedometer
 * data streaming is either started or stopped.
 *
 * The method uses multiplexing services and implementation class shall
 * provide #doUpdatePedometerStatus(bool,bool,uint32_t) method and use
 * #routeIndication(const slimPedometerDataStructT&) for data reporting.
 *
 * @param[in]  zTxn   Transaction data.
 * @param[in]  lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if sensor data request is made successfully.
 */
slimErrorEnumT MultiplexingProvider::enablePedometerRequest
(
  const slim_EnablePedometerTxnStructType &zTxn,
  int32_t lTxnId
)
{
  slimErrorEnumT eRetVal = eSLIM_SUCCESS;
  boolean b_SendRequest = FALSE;

  SLIM_LOG_ENTRY();

  if (0 == SLIM_MASK_IS_SET(m_qServiceMask, eSLIM_SERVICE_PEDOMETER))
  {
    SLIM_LOGE("Pedometer is not enabled");
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }

  /* Check if request is valid for this client. */
  eRetVal = slim_ServiceStatusCanEnable(&m_zProviderStatus,
      zTxn.z_TxnData.p_Handle, eSLIM_SERVICE_PEDOMETER,
      zTxn.z_Request.enableConf.enable, &b_SendRequest);

  if (eSLIM_SUCCESS == eRetVal)
  {
    if (b_SendRequest)
    {
      uint8_t u_ResetStepCount =
          zTxn.z_Request.resetStepCount_valid ?
              zTxn.z_Request.resetStepCount : (uint8_t) FALSE;
      uint32_t q_StepCountThreshold =
          zTxn.z_Request.stepCountThreshold_valid ?
              zTxn.z_Request.stepCountThreshold : 0;
      eRetVal = doUpdatePedometerStatus(zTxn.z_Request.enableConf.enable,
          u_ResetStepCount, q_StepCountThreshold);
    }
    else
    {
      eRetVal = eSLIM_SUCCESS;
    }

    if (eSLIM_SUCCESS == eRetVal)
    {
      /* Error is notified by SLIM core so notify only success to client here.
       */
      notifySuccessfulEnableRequest(zTxn.z_TxnData.p_Handle,
          zTxn.z_Request.enableConf.enable, lTxnId,
          eSLIM_SERVICE_PEDOMETER);
    }
    else
    {
      SLIM_LOGE("Failed enabling pedometer data: %d", eRetVal);
    }
  }
  return eRetVal;
}
/**
 * @brief Initiates motion data request.
 *
 * Function initiates motion request. If request is successful, motion data
 * streaming is either started or stopped.
 *
 * The method uses multiplexing services and implementation class shall
 * provide #doUpdateMotionDataStatus(bool) method and use
 * #routeIndication(const slimMotionDataStructT&) for data reporting.
 *
 * @param[in]  zTxn   Transaction data.
 * @param[in]  lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if sensor data request is made successfully.
 */
slimErrorEnumT MultiplexingProvider::enableMotionDataRequest
(
  const slim_EnableMotionDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  slimErrorEnumT eRetVal = eSLIM_SUCCESS;
  boolean bSendRequest = FALSE;

  SLIM_LOG_ENTRY()
  ;

  if (0 == SLIM_MASK_IS_SET(m_qServiceMask, eSLIM_SERVICE_MOTION_DATA))
  {
    SLIM_LOGE("Motion data is not enabled");
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }

  /* Check if request is valid for this client. */
  eRetVal = slim_ServiceStatusCanEnable(&m_zProviderStatus,
      zTxn.z_TxnData.p_Handle, eSLIM_SERVICE_MOTION_DATA,
      zTxn.z_Request.enableConf.enable, &bSendRequest);

  if (eSLIM_SUCCESS == eRetVal)
  {
    if (bSendRequest)
    {
      eRetVal = doUpdateMotionDataStatus(zTxn.z_Request.enableConf.enable);
    }
    else
    {
      eRetVal = eSLIM_SUCCESS;
    }

    if (eSLIM_SUCCESS == eRetVal)
    {
      /* Error is notified by SLIM core so notify only success to client here.
       */
      notifySuccessfulEnableRequest(zTxn.z_TxnData.p_Handle,
          zTxn.z_Request.enableConf.enable, lTxnId,
          eSLIM_SERVICE_MOTION_DATA);
    }
    else
    {
      SLIM_LOGE("Failed enabling motion data: %d", eRetVal);
    }
  }
  return eRetVal;
}
/**
 * @brief Initiates vehicle data request.
 *
 * Function initiates vehicle sensor or odometry request. If request is
 * successful, vehicle data streaming is either started or stopped.
 *
 * The method uses multiplexing services and implementation class shall
 * provide #doUpdateVehicleDataStatus(bool,slimServiceEnumT) method and use
 * #routeIndication(slimServiceEnumT,const slimVehicleSensorDataStructT&) or
 * #routeIndication(const slimVehicleOdometryDataStructT&) for data reporting.
 *
 * @param[in]  zTxn   Transaction data.
 * @param[in]  lTxnId Service transaction id.
 *
 * @return eSLIM_SUCCESS if sensor data request is made successfully.
 */
slimErrorEnumT MultiplexingProvider::enableVehicleDataRequest
(
  const slim_EnableVehicleDataTxnStructType &zTxn,
  int32_t lTxnId
)
{
  slimErrorEnumT eRetVal = eSLIM_SUCCESS;
  boolean b_SendRequest = FALSE;

  SLIM_LOG_ENTRY()
  ;

  if (0 == SLIM_MASK_IS_SET(m_qServiceMask, zTxn.z_Request.service))
  {
    SLIM_LOGE("Vehicle service is not enabled: %d", zTxn.z_Request.service);
    return eSLIM_ERROR_SERVICE_NOT_ENABLED;
  }

  /* Check if request is valid for this client. */
  eRetVal = slim_ServiceStatusCanEnable(
      &m_zProviderStatus,
      zTxn.z_TxnData.p_Handle,
      zTxn.z_Request.service,
      zTxn.z_Request.enableConf.enable,
      &b_SendRequest);

  if (eSLIM_SUCCESS == eRetVal)
  {
    if (b_SendRequest)
    {
      eRetVal = doUpdateVehicleDataStatus(zTxn.z_Request.enableConf.enable,
          zTxn.z_Request.service);
    }
    else
    {
      eRetVal = eSLIM_SUCCESS;
    }

    if (eSLIM_SUCCESS == eRetVal)
    {
      /* Error is notified by SLIM core so notify only success to client here.
       */
      notifySuccessfulEnableRequest(
          zTxn.z_TxnData.p_Handle,
          zTxn.z_Request.enableConf.enable,
          lTxnId,
          zTxn.z_Request.service);
    }
    else
    {
      SLIM_LOGE("Failed enabling vehicle data: %d", eRetVal);
    }
  }
  return eRetVal;
}

/**
 * @brief Provides timesync mt offset for the SLIM service.
 *
 * Function provides timesync mt offset for the SLIM service.
 *
 * @param[in]  pHandle      Pointer to the client handle
 * @param[in]  eService     SLIM service.
 * @param[out] zMtOffsetOut MT offset reference. Data is stored to this
 *                          variable if found.
 * @return true if mt data was found. false otherwise.
 */
bool MultiplexingProvider::getMtOffsetForService
(
  slimClientHandleT pHandle,
  slimServiceEnumT eService,
  slim_ServiceMtDataStructType &zMtOffsetOut
)
{
  boolean bFound = FALSE;

  SLIM_LOG_ENTRY();

  SLIM_UNUSED(pHandle);

  bFound = slim_ServiceStatusGetMtOffset(
      &m_zProviderStatus, eService, &zMtOffsetOut);

  return bFound;
}

/**
 * @brief Sets timesync mt offset for the SLIM service.
 *
 * Function sets timesync mt offset for the SLIM service.
 *
 * @param[in] pHandle     Pointer to the client handle. If NULL, data is
 *                        applied to all clients.
 * @param[in] eService    SLIM service. If eSLIM_SERVICE_NONE, data is applied
 *                        to all services.
 * @param[in] zMtOffset   MT offset to be set.
 * @param[in] bSetForAllClients true if data should be applied to all clients.
 *                               In this case parameter pHandle is ignored.
 *                               Otherwise false.
 */
void MultiplexingProvider::setMtOffsetForService
(
  slimClientHandleT pHandle,
  slimServiceEnumT eService,
  const slim_ServiceMtDataStructType &zMtOffset,
  bool bSetForAllClients
)
{
  SLIM_LOG_ENTRY();

  SLIM_UNUSED(pHandle);
  SLIM_UNUSED(bSetForAllClients);

  slim_ServiceStatusSetMtOffset(
      &m_zProviderStatus, eService, &zMtOffset);
}
/**
 * @brief Method for enabling or disabling sensor service.
 *
 * @param[in] uEnable       Flag that indicates if the service shall be
 *                          enabled or disabled.
 * @param[in] wReportRateHz Requested reporting rate in Hz.
 * @param[in] wSampleCount  Requested sample count in one batch.
 * @param[in] eService      Service to control.
 *
 * @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT MultiplexingProvider::doUpdateSensorStatus
(
  bool uEnable,
  uint16_t wReportRateHz,
  uint16_t wSampleCount,
  slimServiceEnumT eService
)
{
  SLIM_UNUSED(uEnable);
  SLIM_UNUSED(wReportRateHz);
  SLIM_UNUSED(wSampleCount);
  SLIM_UNUSED(eService);
  return eSLIM_ERROR_UNSUPPORTED;
}
/**
 * @brief Method for enabling or disabling pedometer.
 *
 * @param[in] uEnable             Flag that indicates if the service shall be
 *                                enabled or disabled.
 * @param[in] uResetStepCount     Pedometer parameter.
 * @param[in] qStepCountThreshold Pedometer parameter.
 *
 * @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT MultiplexingProvider::doUpdatePedometerStatus
(
  bool uEnable,
  bool uResetStepCount,
  uint32_t qStepCountThreshold
)
{
  SLIM_UNUSED(uEnable);
  SLIM_UNUSED(uResetStepCount);
  SLIM_UNUSED(qStepCountThreshold);
  return eSLIM_ERROR_UNSUPPORTED;
}
/**
 * @brief Method for enabling or disabling motion data service.
 *
 * @param[in] uEnable Flag that indicates if the service shall be enabled or
 *                    disabled.
 *
 * @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT MultiplexingProvider::doUpdateMotionDataStatus
(
  bool uEnable
)
{
  SLIM_UNUSED(uEnable);
  return eSLIM_ERROR_UNSUPPORTED;
}
/**
 * @brief Method for enabling or disabling vehicle services.
 *
 * @param[in] uEnable Flag that indicates if the service shall be enabled or
 *                    disabled.
 * @param[in] eService      Service to control.
 *
 * @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT MultiplexingProvider::doUpdateVehicleDataStatus
(
  bool uEnable,
  slimServiceEnumT eService
)
{
  SLIM_UNUSED(uEnable);
  SLIM_UNUSED(eService);
  return eSLIM_ERROR_UNSUPPORTED;
}

/**
 * @brief Route function for data indications
 *
 * Provider modules should use this function to route data indications through
 * SLIM-AP daemon task.
 *
 * @param[in] eService  SLIM service.
 * @param[in] eError    Indication error code.
 * @param[in] pData     Message data or 0.
 * @param[in] qSize     Message size.
 *
 * @return Operation status.
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  slimServiceEnumT   eService,
  slimErrorEnumT     eError,
  const void  *pzData,
  uint32_t qSize
)
{
  IndicationDataMsg zInd;
  uint32_t eProviderMessageId = eMSG_ID_INVALID;

  memset(&zInd, 0, sizeof(zInd));
  zInd.eService = eService;
  zInd.eError = eError;

  switch (eService)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
  case eSLIM_SERVICE_SENSOR_GYRO:
  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
  case eSLIM_SERVICE_SENSOR_BARO:
    eProviderMessageId = eMSG_ID_SENSOR_DATA_INJECT;
    if (0 != pzData && 0 != qSize)
    {
      slim_Memscpy(&zInd.data.sensorData, sizeof(zInd.data.sensorData),
          pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_MOTION_DATA:
    eProviderMessageId = eMSG_ID_MOTION_DATA_INJECT;
    if (0 != pzData && 0 != qSize)
    {
      slim_Memscpy(&zInd.data.motionData, sizeof(zInd.data.motionData),
          pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_PEDOMETER:
    eProviderMessageId = eMSG_ID_PEDOMETER_INJECT;
    if (0 != pzData && 0 != qSize)
    {
      slim_Memscpy(&zInd.data.pedometerData, sizeof(zInd.data.pedometerData),
          pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_VEHICLE_ACCEL:
  case eSLIM_SERVICE_VEHICLE_GYRO:
    eProviderMessageId = eMSG_ID_VEHICLE_SENSOR_DATA_INJECT;
    if (0 != pzData && 0 != qSize)
    {
      slim_Memscpy(&zInd.data.vehicleSensorData,
          sizeof(zInd.data.vehicleSensorData), pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_VEHICLE_ODOMETRY:
    eProviderMessageId = eMSG_ID_VEHICLE_ODOMETRY_DATA_INJECT;
    if (0 != pzData && 0!= qSize)
    {
      slim_Memscpy(
          &zInd.data.vehicleOdometryData,
          sizeof(zInd.data.vehicleOdometryData),
          pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_VEHICLE_SPEED:
  case eSLIM_SERVICE_VEHICLE_DWS:
  case eSLIM_SERVICE_VEHICLE_GEAR:
    eProviderMessageId = eMSG_ID_VEHICLE_MOTION_DATA_INJECT;
    if (0 != pzData && 0 != qSize)
    {
      slim_Memscpy(&zInd.data.vehicleMotionData,
          sizeof(zInd.data.vehicleMotionData), pzData, qSize);
    }
    break;
  case eSLIM_SERVICE_AMD:
  case eSLIM_SERVICE_RMD:
  case eSLIM_SERVICE_SMD:
  case eSLIM_SERVICE_DISTANCE_BOUND:
  case eSLIM_SERVICE_PED_ALIGNMENT:
  case eSLIM_SERVICE_MAG_FIELD_DATA:
  default:
    break;
  }

  if (eMSG_ID_INVALID != eProviderMessageId)
  {
    if (0 == slim_SendProviderIpcToTask(m_eProvider, eProviderMessageId, &zInd,
        sizeof(zInd)))
    {
      return eSLIM_SUCCESS;
    }
    return eSLIM_ERROR_IPC_COMMUNICATION;
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
 * @brief Multiplex sensor data
 *
 * @param[in] eService Reporting service
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  slimServiceEnumT   eService,
  const slimSensorDataStructT &zData
)
{
  return routeIndication(eService, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex motion data
 *
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
 const slimMotionDataStructT &zData
)
{
  return routeIndication(
      eSLIM_SERVICE_MOTION_DATA, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex pedometer data
 *
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  const slimPedometerDataStructT &zData
)
{
  return routeIndication(
      eSLIM_SERVICE_PEDOMETER, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex vehicle odometry data
 *
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  const slimVehicleOdometryDataStructT &zData
)
{
  return routeIndication(
      eSLIM_SERVICE_VEHICLE_ODOMETRY, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex vehicle sensor data
 *
 * @param[in] eService Reporting service
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  slimServiceEnumT   eService,
  const slimVehicleSensorDataStructT &zData
)
{
  return routeIndication(eService, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex vehicle motion data
 *
 * @param[in] eService Reporting service
 * @param[in] zData    Service data
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  slimServiceEnumT   eService,
  const slimVehicleMotionDataStructT &zData
)
{
  return routeIndication(eService, eSLIM_SUCCESS, &zData, sizeof(zData));
}

/**
 * @brief Multiplex service error indication
 *
 * @param[in] eService Reporting service
 * @param[in] eError   Service error
 *
 * @return Operation status
 * @retval eSLIM_SUCCESS On success.
 */
slimErrorEnumT MultiplexingProvider::routeIndication
(
  slimServiceEnumT   eService,
  slimErrorEnumT     eError
)
{
  if (eSLIM_SUCCESS != eError)
  {
    return routeIndication(eService,eError,0,0);
  }
  return eSLIM_ERROR_BAD_ARGUMENT;
}

/**
 * @brief Dispatch function for data indications
 *
 * Sensor providers should use this function to dispatch data indications
 * to SLIM core.
 *
 * @param[in] zMessage Message structure.
 * @internal
 */
void MultiplexingProvider::dispatchIndication
(
  const IndicationDataMsg &zSlimDaemonMessage
)
{
  SLIM_LOG_ENTRY();

  uint32 qDataSize = 0;
  void *pData = 0;
  slimMessageIdEnumT eMessageId = eSLIM_MESSAGE_ID_NONE;

  switch (zSlimDaemonMessage.eService)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
  case eSLIM_SERVICE_SENSOR_GYRO:
  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
  case eSLIM_SERVICE_SENSOR_BARO:
      qDataSize = sizeof(zSlimDaemonMessage.data.sensorData);
      pData = (void*)&zSlimDaemonMessage.data.sensorData;
      eMessageId = eSLIM_MESSAGE_ID_SENSOR_DATA_IND;
      break;
  case eSLIM_SERVICE_PEDOMETER:
      qDataSize = sizeof(zSlimDaemonMessage.data.pedometerData);
      pData = (void*)&zSlimDaemonMessage.data.pedometerData;
      eMessageId = eSLIM_MESSAGE_ID_PEDOMETER_IND;
      break;
  case eSLIM_SERVICE_MOTION_DATA:
      qDataSize = sizeof(zSlimDaemonMessage.data.motionData);
      pData = (void*)&zSlimDaemonMessage.data.motionData;
      eMessageId = eSLIM_MESSAGE_ID_MOTION_DATA_IND;
      break;
  case eSLIM_SERVICE_VEHICLE_ODOMETRY:
      qDataSize = sizeof(zSlimDaemonMessage.data.vehicleOdometryData);
      pData = (void*)&zSlimDaemonMessage.data.vehicleOdometryData;
      eMessageId = eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND;
      break;
  case eSLIM_SERVICE_VEHICLE_ACCEL:
  case eSLIM_SERVICE_VEHICLE_GYRO:
      qDataSize = sizeof(zSlimDaemonMessage.data.vehicleSensorData);
      pData = (void*)&zSlimDaemonMessage.data.vehicleSensorData;
      eMessageId = eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND;
      break;
  case eSLIM_SERVICE_VEHICLE_SPEED:
  case eSLIM_SERVICE_VEHICLE_DWS:
  case eSLIM_SERVICE_VEHICLE_GEAR:
      qDataSize = sizeof(zSlimDaemonMessage.data.vehicleMotionData);
      pData = (void*)&zSlimDaemonMessage.data.vehicleMotionData;
      eMessageId = eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND;
      break;
  default:
      SLIM_LOGE("Unknown indication received %d.",
          zSlimDaemonMessage.eService);
      break;
  }
  if (eSLIM_SUCCESS != zSlimDaemonMessage.eError)
  {
    pData = 0;
    qDataSize = 0;
  }
  if ((eSLIM_MESSAGE_ID_NONE != eMessageId) && pData)
  {
    slim_ServiceStatusDispatchDataToRegistered(
      &m_zProviderStatus,
      m_eProvider,
      zSlimDaemonMessage.eService,
      zSlimDaemonMessage.eError,
      eMessageId,
      qDataSize,
      pData);
  }
}

/**
 * @brief Route function for time responses
 *
 * Provider modules should use this function to route time responses through
 * SLIM-AP daemon task.
 *
 * @param[in] lTxnId             SLIM transaction id.
 * @param[in] eError             Error code.
 * @param[in] tRemoteRxTimeMsecs Remote time of provider.
 * @param[in] tRemoteTxTimeMsecs Remote time of provider.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS On success
 */
slimErrorEnumT MultiplexingProvider::routeTimeResponse
(
  int32_t lTxnId,
  slimErrorEnumT eError,
  uint64_t tRemoteRxTimeMsecs,
  uint64_t tRemoteTxTimeMsecs
)
{
  SLIM_LOG_ENTRY();
  ResponseDataMsg z_Message;
  z_Message.serviceTxnId = lTxnId;
  z_Message.eError = eError;
  z_Message.data.remoteProviderRxTime = tRemoteRxTimeMsecs;
  z_Message.data.remoteProviderTxTime = tRemoteTxTimeMsecs;

  SLIM_LOGD("Routing time response, tx=%" PRIi32 " status=%d", lTxnId, eError);

  if (0 == slim_SendProviderIpcToTask(
      m_eProvider,
      eMSG_ID_TIME_SYNC_DATA_INJECT,
      &z_Message,
      sizeof(z_Message)))
  {
    return eSLIM_SUCCESS;
  }
  SLIM_LOGE("Failed to send IPC");
  return eSLIM_ERROR_IPC_COMMUNICATION;
}

/**
 * @brief Dispatch function for time response.
 *
 * AP provider modules should use this function to dispatch time responses
 * to SLIM core.
 *
 * @param[in] zMessage Message structure.
 * @internal
 */
void MultiplexingProvider::dispatchTimeResponse
(
  const ResponseDataMsg &zMessage
)
{
  SLIM_LOG_ENTRY();

  slim_CoreNotifyTimeResponse64(
      zMessage.serviceTxnId,
      m_eProvider,
      zMessage.eError,
      zMessage.data.remoteProviderRxTime,
      zMessage.data.remoteProviderTxTime);
}

/**
 * @brief Route function for generic responses
 *
 * Provider modules should use this function to route generic service
 * responses through SLIM-AP daemon task.
 *
 * @param[in] lTxnId    SLIM transaction id.
 * @param[in] eError    Error code.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS On success
 */
slimErrorEnumT MultiplexingProvider::routeGenericResponse
(
  int32_t lTxnId,
  slimErrorEnumT eError
)
{
  SLIM_LOG_ENTRY();
  ResponseDataMsg z_Message;
  memset(&z_Message, 0, sizeof(z_Message));
  z_Message.serviceTxnId = lTxnId;
  z_Message.eError = eError;

  if (0 == slim_SendProviderIpcToTask(
      m_eProvider,
      eMSG_ID_GENERIC_SERVICE_RESPONSE,
      &z_Message,
      sizeof(z_Message)))
  {
    return eSLIM_SUCCESS;
  }
  return eSLIM_ERROR_IPC_COMMUNICATION;
}

/**
 * @brief Dispatch function for generic responses
 *
 * Provider modules should use this function to dispatch generic service
 * responses to SLIM core.
 *
 * @param[in] zMessage Message structure.
 * @internal
 */
void MultiplexingProvider::dispatchGenericResponse
(
  const ResponseDataMsg &zMessage
)
{
  SLIM_LOG_ENTRY();

  slim_CoreNotifyResponseNoPayload(
      zMessage.serviceTxnId,
      m_eProvider,
      zMessage.eError);
}

/**
@brief Keeps the Sensor data in the buffer and update the Dispatch Time to send the sensor data.

Function wraps the slim_ServiceStatusGetDispatchTime function.

@param[in] eService               SLIM service.
@param[in] SensorData             ACCEL or GYRO SensorData will hold the Sensor data.
@param[in] SensorType             ACCEL or GYRO Sensor Type.
@param[in] tCurrentTimestamp      Current Timestamp for the sensor data.
@param[in] tDispatchTimestamp     Current Dispatch Timestamp for the sensor data.
@param[in] bIsPPSTimeNotReached   To Synch once PPS Time Received flag should be kept
@param[in] wBatchCount            Number of sensor samples to be kept in a batch

@retval void
*/
bool MultiplexingProvider::isBatchReady
(
  slimServiceEnumT eService,
  slimSensorDataStructT& sensorData,
  const slimSensorDataStructT& inputSensorData,
  uint64_t& tDispatchTimestamp,
  bool& bIsPPSTimeNotReached,
  const uint16_t& wBatchCount
)
{
  static bool bIsFirstCall = true;
  static uint32_t qCountBasedBatchingCfg = 0;
  static uint32_t qSyncOnceCfg = 0;
  bool bReturnCode = false;

  if (true == bIsFirstCall)
  {
     loc_param_s_type z_sapParameterTable[] =
     {
        { "COUNT_BASED_BATCHING", &qCountBasedBatchingCfg, NULL, 'n' },
        { "SYNC_ONCE", &qSyncOnceCfg, NULL, 'n' },
     };

     /* Read Batching configuration and Time Sync Needed */
     UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
     SLIM_LOGD("qCountBasedBatchingCfg = %d and qSyncOnceCfg = %d \n",
               qCountBasedBatchingCfg,
               qSyncOnceCfg);
     bIsFirstCall = false;
  }

  /* Copy the slimSensorSampleStructT of first one directly  */
  sensorData.samples[sensorData.samples_len] = inputSensorData.samples[0];
  /* CurrentTimestamp = inputSensorData's Time Stamp (i.e  inputSensorData.timeBase) */
  const uint64_t& tCurrentTimestamp = inputSensorData.timeBase;
  if (sensorData.samples_len == 0)
  {
     /* Single Batch Start Time is read from the very first sample only */
     sensorData.timeBase = tCurrentTimestamp; //Convert it to msec;
     sensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
     sensorData.timeSource = eSLIM_TIME_SOURCE_UNSPECIFIED;
     sensorData.flags = 0;
     /* sensorType = inputSensorData's sensorType */
     sensorData.sensorType = inputSensorData.sensorType;

     /* Count based batching will just make a single batch to a predefined COUNT
     It will not guarantee  the sync w.r.to Time. These Hooks are needed for new
     QDR core architecture under development.
     So in case of COUNT_BASED_BATCHING, there is no need of Sync Time */
     if (0 == qCountBasedBatchingCfg)
     {
        /* For Optimization query only once the
        DispatchTime for an entire batch. */
        tDispatchTimestamp = slim_ServiceStatusGetDispatchTime(&m_zProviderStatus,
                                                               eService,
                                                               tCurrentTimestamp);
        SLIM_LOGD("tDispatchTimestamp=%" PRIu64 " ms. \n", tDispatchTimestamp);
     }
  }
  sensorData.samples[sensorData.samples_len].sampleTimeOffset =
     slim_TimeToClockTicks((tCurrentTimestamp) - sensorData.timeBase);
  sensorData.samples_len++;
  /* Count based batching will just make a single batch to a predefined COUNT
  It will not gaurentee the sync w.r.to Time. */
  if (0 != qCountBasedBatchingCfg)
  {
     /* Even in case of Count based batching, there can be one need of sync the batch once for all
     just after the first PPS arrives in a QDR Session These Hooks are needed for new
     QDR core architecture under development.*/
     if (0 != qSyncOnceCfg)
     {
        if (bIsPPSTimeNotReached)
        {
           /* For Optimization query only once the
           DispatchTime for an entire batch. */
           tDispatchTimestamp = slim_ServiceStatusGetProviderBatchStartTime(&m_zProviderStatus,
                                                                            eService);
           SLIM_LOGD("tDispatchTimestamp=%" PRIu64 " ms. \n", tDispatchTimestamp);
           if (tDispatchTimestamp > 0ull)
           {
              bIsPPSTimeNotReached = false;
           }
        }
      }

     if ((0 != qSyncOnceCfg) && (tDispatchTimestamp > 0ull) &&
     (tCurrentTimestamp >= tDispatchTimestamp))
      {
          SLIM_LOGD("Go for routeIndication eService = %d tDispatchTimestamp=%" PRIu64 " ms. \
             tCurrentTimestamp = %" PRIu64 " ms. sensorData.samples_len = %d. \n", eService, \
             tDispatchTimestamp, tCurrentTimestamp, sensorData.samples_len);
          tDispatchTimestamp = 0ull;
          bReturnCode = true;
      }
     else if (sensorData.samples_len == wBatchCount ||
        sensorData.samples_len >= SLIM_SENSOR_MAX_SAMPLE_SETS)
      {
        SLIM_LOGD("Go for routeIndication eService = %d tDispatchTimestamp=%" PRIu64 " ms. \
             tCurrentTimestamp = %" PRIu64 " ms. sensorData.samples_len = %d. \n", eService, \
             tDispatchTimestamp, tCurrentTimestamp, sensorData.samples_len);
        bReturnCode = true;
      }
   }
  /* Time Based Batching case */
   else  if (tCurrentTimestamp > tDispatchTimestamp  ||
      sensorData.samples_len >= SLIM_SENSOR_MAX_SAMPLE_SETS)
  {
     SLIM_LOGD("Go for routeIndication eService = %d tDispatchTimestamp=%" PRIu64 " ms. \
        tCurrentTimestamp = %" PRIu64 " ms. sensorData.samples_len = %d. \n", eService, \
        tDispatchTimestamp, tCurrentTimestamp, sensorData.samples_len);
     bReturnCode = true;
  }

  return bReturnCode;
}

/**
@brief Method for Setting the batching status

@param[in] eService     Service to control.

*/
void MultiplexingProvider::enableBatching(slimServiceEnumT eService)
{
     slim_ServiceStatusSetBatchingEnabled(&m_zProviderStatus, eService, FALSE);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

