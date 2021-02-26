/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Implementation file for SLIM client connection component
 *
 * @ingroup slim_ClientLibrary
 */
#include <inttypes.h>

#include <comdef.h>
#include <qmi_client.h>
#include <slim_os_log_api.h>
#include <SlimProxy.h>
#include <SlimProxyConnection.h>
#include <SlimClientBase.h>
#include <string.h>

namespace slimproxy
{

//! @addtogroup slim_ClientLibrary
//! @{

static const uint64_t PEER_RECONNECT_TIMEOUT_MS = 2000;
static const uint64_t SERVICE_RETRY_TIMEOUT_MS = 2000;

//! @}

/**
 * Object constructor.
 */
Connection::Transaction::Transaction
(
    slimServiceEnumT   eService,
    /**< [in] SLIM Service */
    uint8_t uTxn,
    /**< [in] SLIM client transaction number */
    slimMessageIdEnumT eMessageId
    /**< [in] Expected response type */
) :
    m_eService(eService),
    m_uTxn(uTxn),
    m_eMessageId(eMessageId),
    m_qReferenceTimeMs(0)
{
}

/**
 * @brief State change method
 *
 * The method performs state change operations for connect and disconnect
 * operations.
 */
void Connection::nextState()
{
  switch (m_eTargetState)
  {
  case eRELEASED:
  {
    switch(m_eQmiState)
    {
    case eSTATE_DISCONNECTED:
      delete this;
      break;
    case eSTATE_DISCONNECTING:
      break;
    case eSTATE_REGISTERED:
    case eSTATE_REGISTERING:
      m_eQmiState = eSTATE_DISCONNECTING;
      if (!m_zQmiClient.Disconnect())
      {
        delete this;
      }
      break;
    }
    break;
  }
  case eREGISTERED:
  {
    switch(m_eQmiState)
    {
    case eSTATE_REGISTERED:
      if (m_uAutoStart)
      {
        for (slimServiceEnumT eService = eSLIM_SERVICE_FIRST;
            eService <= eSLIM_SERVICE_LAST; ++eService)
        {
          ServiceState &zServiceState = m_zServiceStates[eService];
          if (ServiceState::ENABLE_RETRY == zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable &&
              zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs <= slim_TimeTickGetMilliseconds())
          {
            zServiceState.m_zInTransition = zServiceState.m_zActive;
            handleEnableRequest(0, eService, false);
          }
          else
          {
            handleEnableRequest(0, eService, false);
          }
        }
      }
      break;
    case eSTATE_REGISTERING:
      break;
    case eSTATE_DISCONNECTED:
      if (m_zQmiClient.Connect())
      {
        m_tExpireTimeMs = slim_TimeTickGetMilliseconds() + PEER_RECONNECT_TIMEOUT_MS;
        m_eQmiState = eSTATE_REGISTERING;
        slimErrorEnumT eStatus = m_zQmiClient.Register();
        if (eSLIM_SUCCESS != eStatus)
        {
          m_eQmiState = eSTATE_DISCONNECTING;
          if (!m_zQmiClient.Disconnect())
          {
            handleConnectionDisconnect();
          }
        }
      }
      break;
    case eSTATE_DISCONNECTING:
      break;
    }
    break;
  }
  }
}

/** Sends message to main worker thread */
void Connection::onQmiClientResponse
(
  void                *pUserCookie,
  slimErrorEnumT       eError,
  slimMessageIdEnumT   eMsgId,
  const void          *pData,
  uint32_t             qDataSize
)
{
  SLIM_UNUSED(pData);

  SLIM_LOGD("Response: err=%d msg=%d size=%u",
      eError, eMsgId, qDataSize);

  Transaction *pzTransaction = reinterpret_cast<Transaction*>(pUserCookie);

  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zEnableResponse.m_eService = pzTransaction->m_eService;
  zMsg.m_zPayload.m_zEnableResponse.m_eError = eError;
  zMsg.m_zPayload.m_zEnableResponse.m_uTxId = pzTransaction->m_uTxn;
  delete pzTransaction;

  sendMessage(Proxy::eIPC_CONNECTION_ENABLE_RESPONSE, zMsg);
}

/** Sends message to main worker thread */
void Connection::onQmiClientData
(
  slimServiceEnumT     eService,
  slimErrorEnumT       eError,
  slimMessageIdEnumT   eMsgId,
  const void          *pData,
  uint32_t             qDataSize
)
{

  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zData.m_eService = eService;
  zMsg.m_zPayload.m_zData.m_eMsgId = eMsgId;
  zMsg.m_zPayload.m_zData.m_eError = eError;
  zMsg.m_zPayload.m_zData.m_qDataSize = qDataSize;
  memscpy(&zMsg.m_zPayload.m_zData.m_zContents,
      sizeof(zMsg.m_zPayload.m_zData.m_zContents), pData, qDataSize);

  sendMessage(Proxy::eIPC_CONNECTION_DATA, zMsg);
}

/** Sends message to main worker thread */
void Connection::onQmiClientTimeUpdate
(
  void                *pUserCookie,
  slimErrorEnumT       eError,
  uint64_t             tRemoteRxTimeMs,
  uint64_t             tRemoteTxTimeMs
)
{
  SLIM_UNUSED(pUserCookie);
  SLIM_UNUSED(eError);
  SLIM_UNUSED(tRemoteRxTimeMs);
  SLIM_UNUSED(tRemoteTxTimeMs);

  //! @todo If remote processor connections or provider time requests get
  //!       supported, add time response handling here.
}

/** Sends message to main worker thread */
void Connection::onQmiClientError
(
  int32_t             lError
)
{
  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zError.m_lError = lError;

  SLIM_LOGW("QMI error: %" PRId32, lError);

  sendMessage(Proxy::eIPC_CONNECTION_ERROR, zMsg);
}
/** Sends message to main worker thread */
void Connection::onQmiClientServiceChange
(
  slimServiceEnumT         eService,
  slimServiceProviderMaskT qServiceProviderMask
)
{
  SLIM_LOGD("Service change: svc=%d %08" PRIx32,
      eService, (uint32_t)qServiceProviderMask);

  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zServiceChange.m_eService = eService;
  zMsg.m_zPayload.m_zServiceChange.m_qServiceProviderMask = qServiceProviderMask;

  sendMessage(Proxy::eIPC_CONNECTION_SERVICE_CHANGE, zMsg);
}

/** Sends message to main worker thread */
void Connection::onQmiClientDisconnect()
{
  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  sendMessage(Proxy::eIPC_CONNECTION_DISCONNECT, zMsg);
}

/** Sends message to main worker thread */
void Connection::onQmiClientRegistered
(
   slimErrorEnumT       eError
)
{
  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zGeneric.m_eError = eError;

  sendMessage(Proxy::eIPC_CONNECTION_REGISTERED, zMsg);
}

/** Sends message to main worker thread */
void Connection::onQmiClientUnregistered
(
   slimErrorEnumT       eError
)
{
  ConnectionMessage zMsg;
  zMsg.m_pHandle = m_pHandle;
  zMsg.m_zPayload.m_zGeneric.m_eError = eError;

  sendMessage(Proxy::eIPC_CONNECTION_UNREGISTERED, zMsg);
}

/**
 * @brief Send message to worker thread
 */
void Connection::sendMessage
(
  uint32_t qMsgId,
  /**< [in] Message identifier */
  const ConnectionMessage &zMsg
  /**< [in] Message payload */
)
{
  Proxy::sendIpcToMain(qMsgId, &zMsg, sizeof(zMsg));
}

/**
 * @brief Invoke callback if not disabled.
 */
void Connection::invokeCallback
(
  const slimMessageHeaderStructT &zHeader,
  /**< [in] Message header */
  void *pMessage
  /**< [in] Message payload */
)
{
  /* It is safe to invoke callback as all processing methods are called from
   * the worker's thread and guarded by mutex by default.
   */
  if (0 != m_pfnCallback)
  {
    m_pfnCallback(m_tCallbackData, &zHeader, pMessage);
  }
}

/**
 * @brief Provides message identifier for service response callback.
 */
slimMessageIdEnumT Connection::getServiceResponseMessageId
(
  slimServiceEnumT eService
  /**<[in] Service type */
)
{
  slimMessageIdEnumT eMessageId;
  switch (eService)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
  case eSLIM_SERVICE_SENSOR_GYRO:
  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
  case eSLIM_SERVICE_SENSOR_BARO:
  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
    eMessageId = eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_AMD:
  case eSLIM_SERVICE_RMD:
    eMessageId = eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_SMD:
    eMessageId = eSLIM_MESSAGE_ID_SMD_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_MOTION_DATA:
    eMessageId = eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_PEDOMETER:
    eMessageId = eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_DISTANCE_BOUND:
    eMessageId = eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_VEHICLE_ACCEL:
  case eSLIM_SERVICE_VEHICLE_GYRO:
  case eSLIM_SERVICE_VEHICLE_ODOMETRY:
  case eSLIM_SERVICE_VEHICLE_SPEED:
  case eSLIM_SERVICE_VEHICLE_DWS:
  case eSLIM_SERVICE_VEHICLE_GEAR:
    eMessageId = eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_MAG_FIELD_DATA:
    eMessageId = eSLIM_MESSAGE_ID_MAG_FIELD_DATA_ENABLE_RESP;
    break;
  case eSLIM_SERVICE_PED_ALIGNMENT:
    eMessageId = eSLIM_MESSAGE_ID_PED_ALIGNMENT_ENABLE_RESP;
    break;
  default:
    eMessageId = eSLIM_MESSAGE_ID_NONE;
    break;
  }

  return eMessageId;
}

/**
 * @brief Provides message identifier for service indication callback.
 */
slimMessageIdEnumT Connection::getServiceIndicationMessageId
(
  slimServiceEnumT eService
  /**<[in] Service type */
)
{
  slimMessageIdEnumT eMessageId;
  switch (eService)
  {
  case eSLIM_SERVICE_SENSOR_ACCEL:
  case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
  case eSLIM_SERVICE_SENSOR_GYRO:
  case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
  case eSLIM_SERVICE_SENSOR_BARO:
  case eSLIM_SERVICE_SENSOR_MAG_CALIB:
  case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
    eMessageId = eSLIM_MESSAGE_ID_SENSOR_DATA_IND;
    break;
  case eSLIM_SERVICE_AMD:
  case eSLIM_SERVICE_RMD:
    eMessageId = eSLIM_MESSAGE_ID_QMD_DATA_IND;
    break;
  case eSLIM_SERVICE_SMD:
    eMessageId = eSLIM_MESSAGE_ID_SMD_DATA_IND;
    break;
  case eSLIM_SERVICE_MOTION_DATA:
    eMessageId = eSLIM_MESSAGE_ID_MOTION_DATA_IND;
    break;
  case eSLIM_SERVICE_PEDOMETER:
    eMessageId = eSLIM_MESSAGE_ID_PEDOMETER_IND;
    break;
  case eSLIM_SERVICE_DISTANCE_BOUND:
    eMessageId = eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_IND;
    break;
  case eSLIM_SERVICE_VEHICLE_ACCEL:
  case eSLIM_SERVICE_VEHICLE_GYRO:
    eMessageId = eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND;
    break;
  case eSLIM_SERVICE_VEHICLE_ODOMETRY:
    eMessageId = eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND;
    break;
  case eSLIM_SERVICE_MAG_FIELD_DATA:
    eMessageId = eSLIM_MESSAGE_ID_MAG_FIELD_DATA_IND;
    break;
  case eSLIM_SERVICE_PED_ALIGNMENT:
    eMessageId = eSLIM_MESSAGE_ID_PED_ALIGNMENT_IND;
    break;
  case eSLIM_SERVICE_VEHICLE_SPEED:
  case eSLIM_SERVICE_VEHICLE_DWS:
  case eSLIM_SERVICE_VEHICLE_GEAR:
    eMessageId = eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND;
    break;
  default:
    eMessageId = eSLIM_MESSAGE_ID_NONE;
    break;
  }
  return eMessageId;
}

void Connection::handleEnableRequest
(
  uint8_t uTxnId,
  slimServiceEnumT eService,
  bool uUserCall
)
{
  ServiceState &zServiceState = m_zServiceStates[eService];
  slimErrorEnumT eStatus = eSLIM_ERROR_INTERNAL;
  bool uSendRequest = false;

  if (m_uAutoStart)
  {
    // State computation:
    // Active != InTransition: Transaction is in progress
    // Transition != Target: Pending new transaction

    bool uInTransaction = 0 != memcmp(
            &zServiceState.m_zActive,
            &zServiceState.m_zInTransition,
            sizeof(zServiceState.m_zActive));
    bool uNewTarget = 0 != memcmp(
        &zServiceState.m_zInTransition,
        &zServiceState.m_zTarget,
        sizeof(zServiceState.m_zActive));
    if (!uInTransaction && uNewTarget)
    {
      if (ServiceState::ENABLE_FALSE == zServiceState.m_zTarget.m_zEnableConf.enable)
      {
        // Can disable always.
        uSendRequest = true;
      }
      else
      {
        const slimServiceProviderMaskT qProviderMask = zServiceState.m_qProviderMask;
        const slimServiceProviderMaskT qProviderFlags = zServiceState.m_zTarget.m_zEnableConf.providerFlags;
        if (0 != qProviderMask &&
            (0 == qProviderFlags ||
             SLIM_MASK_IS_SET(qProviderFlags, eSLIM_SERVICE_PROVIDER_DEFAULT) ||
             0 != (qProviderFlags & qProviderMask)))
        {
          // Can enable because selected provider is available
          uSendRequest = true;
        }
      }
    }

    if (uUserCall)
    {
      // In auto-start mode we always reply service enable request with success.
      slimMessageHeaderStructT zHdr;
      zHdr.service = eService;
      zHdr.msgType = eSLIM_MESSAGE_TYPE_RESPONSE;
      zHdr.msgId = getServiceResponseMessageId(eService);
      zHdr.msgError = eSLIM_SUCCESS;
      zHdr.size = 0;
      zHdr.txnId = uTxnId;

      invokeCallback(zHdr, 0);
    }
  }
  else
  {
    // In compatibility mode request is always forwarded
    uSendRequest = true;
    SLIM_LOGD("Non-autorestart mode");
  }

  if (uSendRequest)
  {
    zServiceState.m_zInTransition = zServiceState.m_zTarget;
    Transaction *pzTx = new Transaction(
        eService, uTxnId, eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP);
    if (0 != pzTx)
    {
      SLIM_LOGD("Enable service=%d enable=%" PRIu8, eService,
                zServiceState.m_zInTransition.m_zEnableConf.enable);
      switch (eService)
      {
      case eSLIM_SERVICE_SENSOR_ACCEL:
      case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      case eSLIM_SERVICE_SENSOR_GYRO:
      case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
      case eSLIM_SERVICE_SENSOR_MAG_CALIB:
      case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
      case eSLIM_SERVICE_SENSOR_BARO:
        eStatus = m_zQmiClient.EnableSensorData(
            zServiceState.m_zInTransition.m_zEnableSensorDataRequest,
            pzTx);
        break;
      case eSLIM_SERVICE_AMD:
      case eSLIM_SERVICE_RMD:
        eStatus = m_zQmiClient.EnableQmdData(
            zServiceState.m_zInTransition.m_zEnableQmdDataRequest,
            pzTx);
        break;
      case eSLIM_SERVICE_PEDOMETER:
        eStatus = m_zQmiClient.EnablePedometer(
            zServiceState.m_zInTransition.m_zEnablePedometerRequest,
            pzTx);
        break;
      case eSLIM_SERVICE_VEHICLE_ACCEL:
      case eSLIM_SERVICE_VEHICLE_GYRO:
      case eSLIM_SERVICE_VEHICLE_ODOMETRY:
      case eSLIM_SERVICE_VEHICLE_SPEED:
      case eSLIM_SERVICE_VEHICLE_DWS:
      case eSLIM_SERVICE_VEHICLE_GEAR:
        eStatus = m_zQmiClient.EnableVehicleData(
            zServiceState.m_zTarget.m_zEnableVehicleDataRequest,
            pzTx);
        break;
      default:
        eStatus = eSLIM_ERROR_UNSUPPORTED;
      }
      if (eSLIM_SUCCESS != eStatus)
      {
        delete pzTx;
      }
    }
    else
    {
      eStatus = eSLIM_ERROR_ALLOCATION;
    }
    if (eSLIM_SUCCESS != eStatus)
    {
      zServiceState.m_zInTransition = zServiceState.m_zActive;
      if (m_uAutoStart)
      {
        zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable = ServiceState::ENABLE_RETRY;
        zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs = slim_TimeTickGetMilliseconds() + 2000;
      }
      else
      {
        slimMessageHeaderStructT zHdr;
        zHdr.service = eService;
        zHdr.msgType = eSLIM_MESSAGE_TYPE_RESPONSE;
        zHdr.msgId = getServiceResponseMessageId(eService);
        zHdr.msgError = eStatus;
        zHdr.size = 0;
        zHdr.txnId = uTxnId;

        invokeCallback(zHdr, 0);
      }
    }
  }

}

Connection::Connection
(
  const slim_OpenTxnStructType &zArgs
) :
    m_eQmiState(eSTATE_DISCONNECTED),
    m_eTargetState(eREGISTERED),
    m_pHandle(zArgs.p_Handle),
    m_pfnCallback(zArgs.fn_Callback),
    m_qOpenFlags(zArgs.q_OpenFlags),
    m_tCallbackData(zArgs.t_CallbackData),
    m_tExpireTimeMs(0),
    m_zQmiClient(qmislim::eNODE_AP, qmislim::eNODE_AP),
    m_uAutoStart(SLIM_MASK_IS_SET(
        zArgs.q_OpenFlags, eSLIM_OPEN_FLAGS_PERSISTENT_SERVICES))
{
  memset(m_zServiceStates, 0, sizeof(m_zServiceStates));
  m_zQmiClient.setCallback(this);
}
Connection::~Connection()
{
  m_zQmiClient.setCallback(0);
}

/**
 * @brief Initiates sensor enable transaction request
 *
 * Method opens a transaction and forwards call to QMI-SLIM QMI client if
 * connection is registered, and/or reports an error using callback function.
 */
void Connection::handleSensorRequest
(
  uint8_t uTxnId,
  /**< [in] Transaction number */
  const slimEnableSensorDataRequestStructT &zRequest
  /**< [in] Transaction request data */
)
{
  ServiceState &zServiceState = m_zServiceStates[zRequest.sensor];
  memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
  if (zRequest.enableConf.enable)
  {
    SLIM_LOGD("Sensor enable");
    zServiceState.m_zTarget.m_zEnableSensorDataRequest = zRequest;
  }
  else
  {
    SLIM_LOGD("Sensor disable");
    zServiceState.m_zTarget.m_zEnableSensorDataRequest = zRequest;
  }
  handleEnableRequest(uTxnId, zRequest.sensor, true);
}
/**
 * @brief Initiates pedometer enable transaction request
 *
 * Method opens a transaction and forwards call to QMI-SLIM QMI client if
 * connection is registered, and/or reports an error using callback function.
 */
void Connection::handlePedometerRequest
(
  uint8_t uTxnId,
  /**< [in] Transaction number */
  const slimEnablePedometerRequestStructT &zRequest
  /**< [in] Transaction request data */
)
{
  ServiceState &zServiceState = m_zServiceStates[eSLIM_SERVICE_PEDOMETER];
  memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
  if (zRequest.enableConf.enable)
  {
    zServiceState.m_zTarget.m_zEnablePedometerRequest = zRequest;
  }
  handleEnableRequest(uTxnId, eSLIM_SERVICE_PEDOMETER, true);
}
/**
 * @brief Initiates QMD enable transaction request
 *
 * Method opens a transaction and forwards call to QMI-SLIM QMI client if
 * connection is registered, and/or reports an error using callback function.
 */
void Connection::handleQmdRequest
(
  uint8_t uTxnId,
  /**< [in] Transaction number */
  const slimEnableQmdDataRequestStructT &zRequest
  /**< [in] Transaction request data */
)
{
  ServiceState &zServiceState = m_zServiceStates[zRequest.service];
  memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
  if (zRequest.enableConf.enable)
  {
    zServiceState.m_zTarget.m_zEnableQmdDataRequest = zRequest;
  }
  handleEnableRequest(uTxnId, zRequest.service, true);
}
/**
 * @brief Initiates vehicle service enable transaction request
 *
 * Method opens a transaction and forwards call to QMI-SLIM QMI client if
 * connection is registered, and/or reports an error using callback function.
 */
void Connection::handleVehicleRequest
(
  uint8_t uTxnId,
  /**< [in] Transaction number */
  const slimEnableVehicleDataRequestStructT &zRequest
)
{
  ServiceState &zServiceState = m_zServiceStates[zRequest.service];
  if (zRequest.enableConf.enable)
  {
    zServiceState.m_zTarget.m_zEnableVehicleDataRequest = zRequest;
  }
  else
  {
    memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
  }
  handleEnableRequest(uTxnId, zRequest.service, true);
}

/**
 * @brief Initiate operation
 *
 * @retval Operation status: true on success; false otherwise
 */
bool Connection::start()
{
  if (m_zQmiClient.Connect())
  {
    m_eQmiState = eSTATE_REGISTERING;
    if (eSLIM_SUCCESS != m_zQmiClient.Register())
    {
      handleConnectionError(QMI_INTERNAL_ERR);
    }
  }
  return true;
}
/**
 * @brief Request object release
 * The method puts object to eRELEASED state and processes disconnect
 * and release when ready.
 */
void Connection::stopAndRelease()
{
  m_eTargetState = eRELEASED;
  /* Setting callback to 0 disables all event propagation */
  m_pfnCallback = 0;
  nextState();
}

/**
 * @brief Processes QMI stack error
 *
 * Method initiates disconnect procedure.
 */
void Connection::handleConnectionError
(
  int32_t lError
  /**<[in] QCCI error code */
)
{
  SLIM_LOGW("QCCI connection error: %" PRId32, lError);
  if (!m_zQmiClient.isDisconnected())
  {
    SLIM_LOGD("Disconnecting");
    m_eQmiState = eSTATE_DISCONNECTING;
    if (!m_zQmiClient.Disconnect())
    {
      SLIM_LOGE("Disconnect error");
      m_eQmiState = eSTATE_DISCONNECTED;
      handleConnectionDisconnect();
    }
  }
}
/**
 * @brief Processes disconnect result
 *
 * Depending on the state, the object may request to reconnect or release
 * itself.
 */
void Connection::handleConnectionDisconnect()
{
  m_tExpireTimeMs = 0;

  /* First, update client's service masks. */
  slimMessageHeaderStructT zHdr;
  slimServiceStatusEventStructT zData;

  zHdr.service = eSLIM_SERVICE_NONE;
  zHdr.msgType = eSLIM_MESSAGE_TYPE_INDICATION;
  zHdr.msgId = eSLIM_MESSAGE_ID_SERVICE_STATUS_IND;
  zHdr.msgError = eSLIM_SUCCESS;
  zHdr.size = sizeof(zData);
  zHdr.txnId = 0;

  zData.availableServicesMask = 0;
  zData.providerFlags = 0;
  zData.service = eSLIM_SERVICE_NONE;;
  zData.serviceStatus = eSLIM_SERVICE_STATUS_UNAVAILABLE;

  slimServiceEnumT eService;
  for (eService = eSLIM_SERVICE_FIRST; eService <= eSLIM_SERVICE_LAST; ++eService)
  {
    ServiceState &zServiceState = m_zServiceStates[eService];
    if (0 != zServiceState.m_qProviderMask)
    {
      zServiceState.m_qProviderMask = 0;
      memset(&zServiceState.m_zActive, 0, sizeof(zServiceState.m_zActive));
      memset(&zServiceState.m_zInTransition, 0, sizeof(zServiceState.m_zInTransition));
      if (!m_uAutoStart)
      {
        memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
      }

      zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable = ServiceState::ENABLE_RETRY;
      zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs = UINT64_MAX;

      zData.service = eService;

      invokeCallback(zHdr, &zData);
    }
  }
  m_eQmiState = eSTATE_DISCONNECTED;
  nextState();
}

/**
 * @brief Process client registration result.
 *
 * If not successful, disconnect is requested.
 */
void Connection::handleConnectionRegistered
(
  slimErrorEnumT eError
  /**< [in] Operation status. */
)
{
  m_tExpireTimeMs = 0;
  if (eSLIM_SUCCESS == eError)
  {
    m_eQmiState = eSTATE_REGISTERED;
    for (slimServiceEnumT eService = eSLIM_SERVICE_FIRST; eService <= eSLIM_SERVICE_LAST; ++eService)
    {
      ServiceState &zServiceState = m_zServiceStates[eService];
      zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable = ServiceState::ENABLE_RETRY;
      zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs = 0;
    }

    nextState();
  }
  else
  {
    handleConnectionError(QMI_INTERNAL_ERR);
  }
}

/**
 * @brief Process client unregistration result.
 *
 * If not successful, disconnect is requested.
 */
void Connection::handleConnectionUnregistered
(
  slimErrorEnumT eError
  /**< [in] Operation status. */
)
{
  if (eSLIM_SUCCESS != eError)
  {
    handleConnectionError(QMI_INTERNAL_ERR);
  }
}

/**
 * @brief Handle service change notification.
 *
 * Computes service status and notifies SLIM API client.
 */
void Connection::handleConnectionServiceChange
(
  slimServiceEnumT eService,
  /**<[in] Updated service */
  slimServiceProviderMaskT qProviderMask
  /**<[in] New service provider mask */
)
{
  if (eService >= eSLIM_SERVICE_FIRST &&
      eService <= eSLIM_SERVICE_LAST)
  {
    ServiceState &zServiceState = m_zServiceStates[eService];
    slimServiceStatusEnumT eServiceStatus;

    if (0 == zServiceState.m_qProviderMask &&
        0 != qProviderMask)
    {
      eServiceStatus = eSLIM_SERVICE_STATUS_AVAILABLE;
    }
    else if (0 == qProviderMask)
    {
      eServiceStatus = eSLIM_SERVICE_STATUS_UNAVAILABLE;
    }
    else
    {
      eServiceStatus = eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED;
    }

    if (0 == qProviderMask)
    {
      // Service is disabled.
      memset(&zServiceState.m_zActive, 0, sizeof(zServiceState.m_zActive));
      memset(&zServiceState.m_zInTransition, 0, sizeof(zServiceState.m_zInTransition));
      zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable = ServiceState::ENABLE_RETRY;
      zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs = UINT64_MAX;
      if (!m_uAutoStart)
      {
        memset(&zServiceState.m_zTarget, 0, sizeof(zServiceState.m_zTarget));
      }
    }
    else
    {
      // Service may be re-enabled.
      if (ServiceState::ENABLE_RETRY == zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable &&
          UINT64_MAX == zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs)
      {
        if (0 == zServiceState.m_zTarget.m_zEnableConf.providerFlags ||
            SLIM_MASK_IS_SET(zServiceState.m_zTarget.m_zEnableConf.providerFlags, eSLIM_SERVICE_PROVIDER_DEFAULT) ||
            0 != (zServiceState.m_zTarget.m_zEnableConf.providerFlags & qProviderMask))
        {
          zServiceState.m_zInTransition = zServiceState.m_zActive;
        }
      }
    }

    bool uRemoved = 0 == qProviderMask;
    zServiceState.m_qProviderMask = qProviderMask;

    slimAvailableServiceMaskT qServiceMask = 0;
    for (slimServiceEnumT eIt = eSLIM_SERVICE_FIRST; eIt <= eSLIM_SERVICE_LAST; ++eIt)
    {
      if (0 != m_zServiceStates[eIt].m_qProviderMask)
      {
        SLIM_MASK_SET(qServiceMask, eIt);
      }
    }

    slimMessageHeaderStructT zHdr;
    slimServiceStatusEventStructT zData;

    zHdr.service = eSLIM_SERVICE_NONE;
    zHdr.msgType = eSLIM_MESSAGE_TYPE_INDICATION;
    zHdr.msgId = eSLIM_MESSAGE_ID_SERVICE_STATUS_IND;
    zHdr.msgError = eSLIM_SUCCESS;
    zHdr.size = sizeof(zData);
    zHdr.txnId = 0;

    zData.availableServicesMask = qServiceMask;
    zData.providerFlags = qProviderMask;
    zData.service = eService;
    zData.serviceStatus = eServiceStatus;

    invokeCallback(zHdr, &zData);

    if (m_uAutoStart)
    {
      handleEnableRequest(0, eService, false);
    }
  }
}
/**
 * @brief Handle service enable result.
 *
 * Notifies SLIM API client.
 */
void Connection::handleConnectionServiceResponse
(
  slimServiceEnumT eService,
  /**<[in] Reporting service */
  slimErrorEnumT eError,
  /**<[in] Operation status */
  uint8_t uTxId
  /**<[in] User transaction id */
)
{
  if (m_uAutoStart)
  {
    ServiceState &zServiceState = m_zServiceStates[eService];
    if (eSLIM_SUCCESS == eError)
    {
      zServiceState.m_zActive = zServiceState.m_zInTransition;
      handleEnableRequest(0, eService, false);
    }
    else
    {
      zServiceState.m_zInTransition = zServiceState.m_zActive;
      if (0 != memcmp(
              &zServiceState.m_zActive,
              &zServiceState.m_zTarget,
              sizeof(zServiceState.m_zTarget)))
      {
        zServiceState.m_zInTransition.m_zTimeoutRequest.m_zEnableConf.enable =
            ServiceState::ENABLE_RETRY;
        zServiceState.m_zInTransition.m_zTimeoutRequest.m_tRetryTimeMs =
            slim_TimeTickGetMilliseconds() + SERVICE_RETRY_TIMEOUT_MS;
      }
    }
  }
  else
  {
    slimMessageHeaderStructT zHdr;
    zHdr.service = eService;
    zHdr.msgType = eSLIM_MESSAGE_TYPE_RESPONSE;
    zHdr.msgId = getServiceResponseMessageId(eService);
    zHdr.msgError = eError;
    zHdr.size = 0;
    zHdr.txnId = uTxId;

    invokeCallback(zHdr, 0);
  }
}
/**
 * @brief Handle service notification.
 *
 * Notifies SLIM API client.
 */
void Connection::handleConnectionServiceData
(
  slimServiceEnumT eService,
  /**<[in] Reporting service */
  slimErrorEnumT eError,
  /**<[in] Operation status */
  uint32_t qSize,
  /**<[in] Message data size */
  const void *pData
  /**<[in] Message data */
)
{
  slimMessageHeaderStructT zHdr;
  zHdr.service = eService;
  zHdr.msgType = eSLIM_MESSAGE_TYPE_INDICATION;
  zHdr.msgId = getServiceIndicationMessageId(eService);
  zHdr.msgError = eError;
  zHdr.size = qSize;
  zHdr.txnId = 0;

  invokeCallback(zHdr, const_cast<void*>(pData));
}

/**
 * @brief Query next event time.
 *
 * @return true if next event time is available.
 */
bool Connection::getNextStateTime
(
  uint64_t &tTimeMsOut
  /**<[out] Next event time in milliseconds */
)
{
  //! @todo Implement timed operation retry support in case of non-persistent
  //!       failures.
  SLIM_UNUSED(tTimeMsOut);
  return false;
}

} /* namespace slimproxy */

