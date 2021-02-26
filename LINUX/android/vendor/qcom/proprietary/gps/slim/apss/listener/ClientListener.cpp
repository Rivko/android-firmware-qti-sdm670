/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief Client listener implementation file.
 *
 * Client listener routes sensor data requests from various sensor clients to
 * the SLIM daemon task using SLIM API.
 *
 * @ingroup slim_APSS
 */

/* Platform library headers */
#include <inttypes.h>

#include "ClientListener.h"

#include "slim_internal_api.h"
#include "slim_os_log_api.h"

//! @addtogroup slim_APSS
//! @{

/* This unit logs as SLIM component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief This component logs as SLIM module.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_SLIM

//! @}

namespace slim
{

/*!
 * @brief SLIM callback
 *
 * This function is used for receiving and dispatching SLIM messages to
 * appropriate object instances
 *
 * @param[in] tCbData         Callback data used for connection opening.
 * @param[in] pzMessageHeader SLIM message header.
 * @param[in] pMessage        Optional SLIM message payload.
 */
void ClientListener::slimCallback
(
  uint64 tCbData,
  const slimMessageHeaderStructT *pzHeader,
  void *pMessage
)
{
  ClientListener *pObject = reinterpret_cast<ClientListener*>((uintptr_t)tCbData);

  switch (pzHeader->msgType)
  {
  case eSLIM_MESSAGE_TYPE_RESPONSE:
    switch (pzHeader->msgId)
    {
    case eSLIM_MESSAGE_ID_OPEN_RESP:
      pObject->processOpenResponse(*pzHeader);
      break;
    case eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP:
      pObject->handleServiceResponse(*pzHeader);
      break;
    default:
      break;
    }
    break;
  case eSLIM_MESSAGE_TYPE_INDICATION:
    if (eSLIM_SUCCESS == pzHeader->msgError && 0 != pMessage)
    {
      switch (pzHeader->msgId)
      {
      case eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
        if (sizeof(slimSensorDataStructT) == pzHeader->size)
        {
          pObject->handleSensorData(
              *pzHeader,
              *(const slimSensorDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_MOTION_DATA_IND:
        if (sizeof(slimMotionDataStructT) == pzHeader->size)
        {
          pObject->handleMotionData(
              *pzHeader,
              *(const slimMotionDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_PEDOMETER_IND:
        if (sizeof(slimPedometerDataStructT) == pzHeader->size)
        {
          pObject->handlePedometerData(
              *pzHeader,
              *(const slimPedometerDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_QMD_DATA_IND:
        if (sizeof(slimQmdDataStructT) == pzHeader->size)
        {
          pObject->handleQmdData(
              *pzHeader,
              *(const slimQmdDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND:
        if (sizeof(slimVehicleOdometryDataStructT) == pzHeader->size)
        {
          pObject->handleVehicleOdometryData(
              *pzHeader,
              *(const slimVehicleOdometryDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND:
        if (sizeof(slimVehicleSensorDataStructT) == pzHeader->size)
        {
          pObject->handleVehicleSensorData(
              *pzHeader,
              *(const slimVehicleSensorDataStructT*)pMessage);
        }
        break;
      case eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
        if (sizeof(slimVehicleMotionDataStructT) == pzHeader->size)
        {
          pObject->handleVehicleMotionData(
              *pzHeader,
              *(const slimVehicleMotionDataStructT*)pMessage);
        }
        break;
      default:
        break;
      }
    }
    else
    {
      pObject->handleServiceDataError(*pzHeader);
    }
    break;
  default:
    break;
  }
}
/*!
 * @brief Internal connection open handler
 *
 * Method asserts the connection to SLIM core has been successfully opened.
 * In case of error, an error is logged and process terminates.
 *
 * @param[in] zHeader SLIM message header
 */
void ClientListener::processOpenResponse
(
  const slimMessageHeaderStructT &zHeader
)
{
  {
    MutexLock _l(m_zStateMutex);
    if (eSLIM_SUCCESS == zHeader.msgError)
    {
      m_eState = eSTATE_OPENED;
    }
    else
    {
      m_eState = eSTATE_CLOSED;
    }
  }
  handleOpenResponse(zHeader);
}

/**
 * @brief Constructor
 *
 * Initializes SLIM client component in a closed state.
 */
ClientListener::ClientListener() :
    m_zContext(),
    m_eState(eSTATE_CLOSED),
    m_zStateMutex(true)
{
    SLIM_LOGD("ClientListener created. ClientListener: %p", this);
    memset(&m_zContext, 0, sizeof(m_zContext));
}

/**
 * @brief Destructor
 *
 * Releases SLIM client resources. This function must be called only when
 * connection is closed.
 */
ClientListener::~ClientListener()
{
}

/**
 * @brief Routes client register to the SLIM daemon task
 *
 * Function routes client register to the SLIM daemon task.
 *
 * @param[in] qClientId  Client identifier.
 * @param[in] qOpenFlags Connection parameters.
 */
slimErrorEnumT ClientListener::slimOpen
(
  uint32_t qClientId,
  slimOpenFlagsT qOpenFlags
)
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;

  SLIM_LOG_ENTRY();

  {
    MutexLock _l(m_zStateMutex);
    if (eSTATE_CLOSED != m_eState)
    {
      return eSLIM_ERROR_INTERNAL;
    }
    m_eState = eSTATE_OPENING;
    e_Status = slim_InternalOpen(qClientId, slimCallback,
        qOpenFlags, (uint64_t)reinterpret_cast<uintptr_t>(this), &m_zContext);
    if (eSLIM_SUCCESS != e_Status)
    {
      m_eState = eSTATE_CLOSED;
    }
  }

  if(eSLIM_SUCCESS != e_Status)
  {
      SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
  }
  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}

/**
 * @brief Routes client de-register to the SLIM daemon task
 *
 * Function routes client register to the SLIM daemon task.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS The connection has been successfully closed.
 */
slimErrorEnumT ClientListener::slimClose()
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;

  SLIM_LOG_ENTRY();
  e_Status = slim_InternalClose(&m_zContext);
  if(eSLIM_SUCCESS != e_Status)
  {
      SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
  }
  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}

/**
 * @brief Routes sensor data request to the SLIM daemon task
 *
 * Function routes sensor data request to the SLIM daemon task.
 *
 * @param[in] uTxId    Transaction number.
 * @param[in] zRequest Request transaction data.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS The request has been successfully accepted for
 *                       processing
 */
slimErrorEnumT ClientListener::processSensorDataRequest
(
  uint8_t uTxId,
  const slimEnableSensorDataRequestStructT &zRequest
)
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;
  SLIM_LOG_ENTRY();
  e_Status = slim_InternalEnableSensorData(&m_zContext, &zRequest, uTxId);
  if(eSLIM_SUCCESS != e_Status)
  {
    SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
  }
  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}

/**
 * @brief Routes pedometer data request to the SLIM daemon task
 *
 * Function routes pedometer data request to the SLIM daemon task.
 *
 * @param[in] uTxId    Transaction number.
 * @param[in] zRequest Request transaction data.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS The request has been successfully accepted for
 *                       processing
 */
slimErrorEnumT ClientListener::processPedometerDataRequest
(
  uint8_t uTxId,
  const slimEnablePedometerRequestStructT &zRequest
)
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;
  SLIM_LOG_ENTRY();
  e_Status = slim_InternalEnablePedometer(&m_zContext, &zRequest, uTxId);
  if(eSLIM_SUCCESS != e_Status)
  {
    SLIM_LOGE("Failed sending pedometer request: error=%d", e_Status);
  }
  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}

/**
 * @brief Routes motion data request to the SLIM daemon task
 *
 * Function routes motion data request to the SLIM daemon task.
 *
 * @param[in] uTxId    Transaction number.
 * @param[in] zRequest Request transaction data.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS The request has been successfully accepted for
 *                       processing
 */
slimErrorEnumT ClientListener::processMotionDataRequest
(
  uint8_t uTxId,
  const slimEnableMotionDataRequestStructT &zRequest
)
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;
  SLIM_LOG_ENTRY();
  e_Status = slim_InternalEnableMotionData(&m_zContext, &zRequest, uTxId);
  if(eSLIM_SUCCESS != e_Status)
  {
    SLIM_LOGE("Failed sending motion data request: error=%d", e_Status);
  }

  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}
/**
 * @brief Routes vehicle data request to the SLIM daemon task
 *
 * Function routes vehicle data request to the SLIM daemon task.
 *
 * @param[in] uTxId    Transaction number.
 * @param[in] zRequest Request transaction data.
 *
 * @return Operation result.
 * @retval eSLIM_SUCCESS The request has been successfully accepted for
 *                       processing
 */
slimErrorEnumT ClientListener::processVehicleDataRequest
(
  uint8_t uTxId,
  const slimEnableVehicleDataRequestStructT &zRequest
)
{
  slimErrorEnumT e_Status = eSLIM_ERROR_INTERNAL;
  SLIM_LOG_ENTRY();
  e_Status = slim_InternalEnableVehicleData(&m_zContext, &zRequest, uTxId);
  if(eSLIM_SUCCESS != e_Status)
  {
    SLIM_LOGE("Failed sending motion data request: error=%d", e_Status);
  }

  SLIM_LOG_EXIT(%d, e_Status);
  return e_Status;
}

/*!
 * @brief Method for handling connection open response.
 *
 * @param[in] zHeader Message header.
 */
void ClientListener::handleOpenResponse
(
  const slimMessageHeaderStructT &zHeader
)
{
  SLIM_LOGD("Open result: %d", zHeader.msgError);
}

/*!
 * @brief Method for handling sensor data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleSensorData
(
  const slimMessageHeaderStructT &zHeader,
  const slimSensorDataStructT &zData
)
{
  SLIM_LOGW("Ignoring sensor data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling pedometer data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handlePedometerData
(
  const slimMessageHeaderStructT &zHeader,
  const slimPedometerDataStructT &zData
)
{
  SLIM_LOGW("Ignoring sensor data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling motion data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleMotionData
(
  const slimMessageHeaderStructT &zHeader,
  const slimMotionDataStructT &zData
)
{
  SLIM_LOGW("Ignoring motion data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling QMD indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleQmdData
(
  const slimMessageHeaderStructT &zHeader,
  const slimQmdDataStructT &zData
)
{
  SLIM_LOGW("Ignoring QMD data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling vehicle sensor data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleVehicleSensorData
(
  const slimMessageHeaderStructT &zHeader,
  const slimVehicleSensorDataStructT &zData
)
{
  SLIM_LOGW("Ignoring sensor data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling vehicle motion data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleVehicleMotionData
(
  const slimMessageHeaderStructT &zHeader,
  const slimVehicleMotionDataStructT &zData
)
{
  SLIM_LOGW("Ignoring sensor data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling vehicle odometry data indication.
 *
 * @param[in] zHeader Message header.
 * @param[in] zData   Data payload.
 */
void ClientListener::handleVehicleOdometryData
(
  const slimMessageHeaderStructT &zHeader,
  const slimVehicleOdometryDataStructT &zData
)
{
  SLIM_LOGW("Ignoring sensor data: service=%d provider=%d",
      zHeader.service, zData.provider);
}

/*!
 * @brief Method for handling service error indication.
 *
 * @param[in] zHeader Message header.
 */
void ClientListener::handleServiceDataError
(
  const slimMessageHeaderStructT &zHeader
)
{
  SLIM_LOGW("Ignoring service data error: service=%d error=%d",
      zHeader.service, zHeader.msgError);
}

/*!
 * @brief Method for handling service enable/disable responses.
 *
 * @param[in] zHeader Message header.
 */
void ClientListener::handleServiceResponse
(
  const slimMessageHeaderStructT &zHeader
)
{
  SLIM_LOGD("Ignoring service response: service=%d error=%d",
      zHeader.service, zHeader.msgError);
}

} /* namespace slim */
