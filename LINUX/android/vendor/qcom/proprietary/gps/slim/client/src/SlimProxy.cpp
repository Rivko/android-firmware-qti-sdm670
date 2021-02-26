/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Implementation file for SLIM Proxy component.
 *
 * @ingroup slim_ClientLibrary
 */
#include "SlimProxy.h"
#include "SlimProxyConnection.h"

#include <inttypes.h>
#include <string.h>

/* Utility headers */
#include <loc_cfg.h>
/* QMI stack DM logging */
#include <qmislim_qmi_dm_log_api.h>

//! @addtogroup slim_ClientLibrary
//! @{

//! @}

namespace slimproxy
{
using slim::Condition;
using slim::Mutex;
using slim::MutexLock;

Mutex      Proxy::c_zStateMutex;
Condition  Proxy::c_zStateCond;
Proxy     *Proxy::c_pzInstance = 0;
const uint32_t  Proxy::c_qThreadId = THREAD_ID_SLIM_API;
const uint32_t  Proxy::c_qDefaultTimerId = 1;
volatile Proxy::InitState Proxy::c_eInitState = Proxy::eSTATE_INITIAL;
volatile uint32_t Proxy::c_qInitCount = 0;

/**
 * @brief Library initialization
 *
 * Function initializes logging and instantiates worker instance.
 */
void Proxy::configure()
{
#if defined(__ANDROID__)
  slim_LogSetAndroidTag("libslimclient");
#endif
  slim_LogSetDiagLevel(SLIM_LOG_NONE);
  slim_QSLogConfigure(FALSE, FALSE, FALSE, FALSE);
}
/**
 * @brief Object state verification.
 *
 * The method checks for proper singleton object initialization.
 * @return Object status.
 */
bool Proxy::checkStatus()
{
  bool uStatus = true;
  MutexLock _l(c_zStateMutex);
  ++c_qInitCount;
  if (0 == c_pzInstance)
  {
    c_pzInstance = new Proxy;
    if (!c_pzInstance->start())
    {
      SLIM_LOGE("Failed to start thread");
      delete c_pzInstance;
      c_pzInstance = NULL;
      uStatus = false;
    }
  }
  while (uStatus && eSTATE_INITIAL == c_eInitState)
  {
    c_zStateCond.wait(c_zStateMutex);
  }
  if (eSTATE_READY == c_eInitState)
  {
    SLIM_LOGD("Status is READY");
    uStatus = true;
  }
  else
  {
    SLIM_LOGD("Status is NOT READY");
    uStatus = false;
  }
  if (0 == --c_qInitCount && eSTATE_ERROR == c_eInitState)
  {
    SLIM_LOGD("Resetting state");
    c_eInitState = eSTATE_INITIAL;
  }
  return uStatus;
}
/**
 * @brief Sends IPC message to worker thread
 */
slimErrorEnumT Proxy::sendIpcToMain
(
  uint32_t qMsgId,
  /**< [in] Message number */
  const void *pData,
  /**< [in] Message data */
  size_t qSize
  /**< [in] Message data size */
)
{
  if (slim_IpcSendData(c_qThreadId, qMsgId, pData, (uint32)qSize))
  {
    return eSLIM_SUCCESS;
  }
  else
  {
    return eSLIM_ERROR_IPC_COMMUNICATION;
  }
}
/**
 * @brief Thread entry function
 */
void Proxy::threadMain
(
  void *pArg
  /**< [in] Parameter (SlimProxy*) */
)
{
  Proxy *pzThread = reinterpret_cast<Proxy*>(pArg);
  if (0 != pzThread)
  {
    InitState eState = eSTATE_ERROR;
    if (slim_IpcStart(c_qThreadId) &&
        pzThread->m_zQmiMonitor.SetCallback(pzThread) &&
        pzThread->m_zQmiMonitor.Start() &&
        0 != (pzThread->m_pzEventTimer = slim_TimerCreate(c_qDefaultTimerId, c_qThreadId)))
    {
      eState = eSTATE_READY;
    }

    SLIM_LOGD("Initial state: %d", eState);

    {
      MutexLock _l(c_zStateMutex);
      c_eInitState = eState;
      c_zStateCond.broadcast();
    }

    pzThread->runEventLoop();
  }
}

/**
 * @brief Checks if the client connection is in open state.
 *
 * This method shall be called from the protected context.
 *
 * @return true if connection object exists and in open state.
 */
bool Proxy::isClientOpen
(
  slimClientHandleT pHandle
  /**[in] Client handle pointer */
)
{
  bool uRetVal = false;
  Connection *pzConnection = c_pzInstance->findConnection(pHandle);
  if (0 != pzConnection)
  {
     uRetVal = (Connection::eREGISTERED == pzConnection->m_eTargetState &&
                0 != pzConnection->m_pfnCallback);
  }
  return uRetVal;
}

/**
 * @brief Constructs object
 */
Proxy::Proxy() : m_pzEventTimer(0), m_zMutex(true)
{
}
/**
 * @brief Destroys object
 */
Proxy::~Proxy()
{
}

/**
 * @brief Starts operation
 * Method initializes QMI-SLIM service monitor and starts worker thread.
 * @return Operation status; false on error.
 */
bool Proxy::start()
{
  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "SlimApi"))
  {
    return false;
  }
  return true;
}

/**
 * @brief Main event handling loop function.
 *
 * The function processes inbound messages and dispatches them to one
 * of object's or connection methods.
 */
void Proxy::runEventLoop()
{

  for (;;)
  {
    slim_IpcMsgT *pzMsg = 0;
    while (0 == (pzMsg = slim_IpcReceive()));
    SLIM_LOGD("Got message: %" PRIu32, (uint32_t)pzMsg->q_MsgId);
    MutexLock _l(c_pzInstance->m_zMutex);
    switch (pzMsg->q_MsgId)
    {
    case eIPC_OS_TIMER:
    {
      slim_TimerMsgT zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleTimer(zMsg.q_TimerId, zMsg.q_Param);
      break;
    }
    case eIPC_SLIM_OPEN_REQUEST:
      memscpy(&m_zMessageData.zOpenRequest,
          sizeof(m_zMessageData.zOpenRequest), pzMsg->p_Data, pzMsg->q_Size);
      handleOpen(m_zMessageData.zOpenRequest);
      break;
    case eIPC_SLIM_CLOSE_REQUEST:
      memscpy(&m_zMessageData.zCloseRequest,
          sizeof(m_zMessageData.zCloseRequest), pzMsg->p_Data, pzMsg->q_Size);
      handleClose(m_zMessageData.zCloseRequest);
      break;
    case eIPC_SLIM_SENSOR_REQUEST:
      memscpy(&m_zMessageData.zSensorRequest,
          sizeof(m_zMessageData.zSensorRequest),
          pzMsg->p_Data, pzMsg->q_Size);
      handleSensorRequest(m_zMessageData.zSensorRequest);
      break;
    case eIPC_SLIM_PEDOMETER_REQUEST:
      memscpy(&m_zMessageData.zPedometerRequest,
          sizeof(m_zMessageData.zPedometerRequest),
          pzMsg->p_Data, pzMsg->q_Size);
      handlePedometerRequest(m_zMessageData.zPedometerRequest);
      break;
    case eIPC_SLIM_QMD_REQUEST:
      memscpy(&m_zMessageData.zQmdRequest,
          sizeof(m_zMessageData.zQmdRequest),
          pzMsg->p_Data, pzMsg->q_Size);
      handleQmdRequest(m_zMessageData.zQmdRequest);
      break;
    case eIPC_SLIM_VEHICLE_REQUEST:
      memscpy(&m_zMessageData.zVehicleRequest,
          sizeof(m_zMessageData.zVehicleRequest),
          pzMsg->p_Data, pzMsg->q_Size);
      handleVehicleRequest(m_zMessageData.zVehicleRequest);
      break;
    case eIPC_MONITOR_SERVICE_ADDED:
      handleServiceAdded();
      break;
    case eIPC_MONITOR_SERVICE_REMOVED:
      handleServiceRemoved();
      break;
    case eIPC_MONITOR_SHUTDOWN:
      handleMonitorShutdown();
      break;
    case eIPC_CONNECTION_ERROR:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionError(zMsg.m_pHandle, zMsg.m_zPayload.m_zError.m_lError);
      break;
    }
    case eIPC_CONNECTION_DISCONNECT:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionDisconnect(zMsg.m_pHandle);
      break;
    }
    case eIPC_CONNECTION_REGISTERED:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionRegistered(
          zMsg.m_pHandle, zMsg.m_zPayload.m_zGeneric.m_eError);
      break;
    }
    case eIPC_CONNECTION_UNREGISTERED:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionUnregistered(
          zMsg.m_pHandle, zMsg.m_zPayload.m_zGeneric.m_eError);
      break;
    }
    case eIPC_CONNECTION_SERVICE_CHANGE:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionServiceChange(
          zMsg.m_pHandle,
          zMsg.m_zPayload.m_zServiceChange.m_eService,
          zMsg.m_zPayload.m_zServiceChange.m_qServiceProviderMask);
      break;
    }
    case eIPC_CONNECTION_ENABLE_RESPONSE:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionServiceResponse(
          zMsg.m_pHandle,
          zMsg.m_zPayload.m_zEnableResponse.m_eService,
          zMsg.m_zPayload.m_zEnableResponse.m_eError,
          zMsg.m_zPayload.m_zEnableResponse.m_uTxId);
      break;
    }
    case eIPC_CONNECTION_DATA:
    {
      Connection::ConnectionMessage zMsg;
      memscpy(&zMsg, sizeof(zMsg), pzMsg->p_Data, pzMsg->q_Size);
      handleConnectionServiceData(
          zMsg.m_pHandle,
          zMsg.m_zPayload.m_zData.m_eService,
          zMsg.m_zPayload.m_zData.m_eError,
          zMsg.m_zPayload.m_zData.m_qDataSize,
          &zMsg.m_zPayload.m_zData.m_zContents);
      break;
    }

    default:
      break;
    }
    slim_IpcDelete(pzMsg);
    updateTimer();
  }
}
/**
 * @brief Sends eIPC_SLIM_OPEN_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handleOpen
 * @sa eIPC_SLIM_OPEN_REQUEST
 */
slimErrorEnumT Proxy::open
(
  slimClientHandleT p_Handle,
  /**<[in] Client handle */
  slimNotifyCallbackFunctionT fn_Callback,
  /**<[in] Callback function */
  slimOpenFlagsT q_OpenFlags,
  /**<[in] Open flags */
  uint64 t_CallbackData
  /**<[in] Callback data */
)
{
  if (!checkStatus())
  {
    return eSLIM_ERROR_INTERNAL;
  }
  if (0 == fn_Callback)
  {
    return eSLIM_ERROR_BAD_ARGUMENT;
  }

  slim_OpenTxnStructType z_Txn;
  memset(&z_Txn, 0, sizeof(z_Txn));
  z_Txn.p_Handle = p_Handle;
  z_Txn.fn_Callback = fn_Callback;
  z_Txn.q_OpenFlags = q_OpenFlags;
  z_Txn.t_CallbackData = t_CallbackData;

  return sendIpcToMain(eIPC_SLIM_OPEN_REQUEST, &z_Txn, sizeof(z_Txn));
}
/**
 * @brief Sends eIPC_SLIM_CLOSE_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handleClose
 * @sa eIPC_SLIM_CLOSE_REQUEST
 */
slimErrorEnumT Proxy::close
(
  slimClientHandleT p_Handle
  /**<[in] Client handle */
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (checkStatus())
  {
    MutexLock _l(c_pzInstance->m_zMutex);
    Connection *pzConnection = c_pzInstance->findConnection(p_Handle);
    if (0 != pzConnection)
    {
      if (Connection::eREGISTERED == pzConnection->m_eTargetState &&
          0 != pzConnection->m_pfnCallback)
      {
        pzConnection->m_pfnCallback = 0;
        slim_CloseTxnStructType z_Txn;
        memset(&z_Txn, 0, sizeof(z_Txn));
        z_Txn.z_TxnData.p_Handle = p_Handle;

        eRetVal = sendIpcToMain(eIPC_SLIM_CLOSE_REQUEST, &z_Txn, sizeof(z_Txn));
      }
      else
      {
        eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
      }
    }
    else
    {
      eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
    }
  }
  else
  {
    eRetVal = eSLIM_ERROR_INTERNAL;
  }
  return eRetVal;
}
/**
 * @brief Service availability test.
 *
 * This method is called from SLIM API client thread context.
 *
 * @retval TRUE if service is available
 * @retval FALSE if service is not available or invalid argument
 */
bool Proxy::serviceAvailable
(
  const slimServiceStatusEventStructT *pz_Status,
  /**<[in] Status container */
  slimServiceEnumT e_Service
  /**<[in] Service to test */
)
{
  if (!checkStatus())
  {
    return false;
  }
  if (NULL != pz_Status && e_Service >= 0 && e_Service <= eSLIM_SERVICE_LAST)
  {
      return SLIM_MASK_IS_SET(pz_Status->availableServicesMask, e_Service) ?
          true : false;
  }
  return false;
}
/**
 * @brief Sends eIPC_SLIM_SENSOR_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handleSensorRequest
 * @sa eIPC_SLIM_SENSOR_REQUEST
 */
slimErrorEnumT Proxy::enableSensorData
(
  slimClientHandleT p_Handle,
  /**<[in] Client handle */
  const slimEnableSensorDataRequestStructT *pz_Request,
  /**<[in] Request data */
  uint8 u_TxnId
  /**<[in] Request transaction id */
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (checkStatus())
  {
    if (0 != pz_Request)
    {
      bool uValidService = false;
      switch (pz_Request->sensor)
      {
      case eSLIM_SERVICE_SENSOR_ACCEL:
      case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
      case eSLIM_SERVICE_SENSOR_GYRO:
      case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
      case eSLIM_SERVICE_SENSOR_BARO:
      case eSLIM_SERVICE_SENSOR_MAG_CALIB:
      case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
        uValidService = true;
        break;
      default:
        uValidService = false;
        break;
      }
      if (uValidService)
      {
        MutexLock _l(c_pzInstance->m_zMutex);
        if (isClientOpen(p_Handle))
        {
          slim_EnableSensorDataTxnStructType z_Txn;
          z_Txn.z_TxnData.p_Handle = p_Handle;
          z_Txn.z_TxnData.u_ClientTxnId = u_TxnId;
          z_Txn.z_Request = *pz_Request;
          eRetVal = sendIpcToMain(eIPC_SLIM_SENSOR_REQUEST, &z_Txn, sizeof(z_Txn));
        }
        else
        {
          eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
        }
      }
      else
      {
        eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
      }
    }
    else
    {
      eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
    }
  }
  else
  {
    eRetVal = eSLIM_ERROR_INTERNAL;
  }

  return eRetVal;
}
/**
 * @brief Sends eIPC_SLIM_PEDOMETER_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handlePedometerRequest
 * @sa eIPC_SLIM_PEDOMETER_REQUEST
 */
slimErrorEnumT Proxy::enablePedometer
(
  slimClientHandleT p_Handle,
  /**<[in] Client handle */
  const slimEnablePedometerRequestStructT *pz_Request,
  /**<[in] Request data */
  uint8 u_TxnId
  /**<[in] Request transaction id */
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (checkStatus())
  {
    if (0 != pz_Request)
    {
      MutexLock _l(c_pzInstance->m_zMutex);
      if (isClientOpen(p_Handle))
      {
        slim_EnablePedometerTxnStructType z_Txn;
        z_Txn.z_TxnData.p_Handle = p_Handle;
        z_Txn.z_TxnData.u_ClientTxnId = u_TxnId;
        z_Txn.z_Request = *pz_Request;

        eRetVal = sendIpcToMain(eIPC_SLIM_PEDOMETER_REQUEST, &z_Txn, sizeof(z_Txn));
      }
      else
      {
        eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
      }
    }
    else
    {
      eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
    }
  }
  else
  {
    eRetVal = eSLIM_ERROR_INTERNAL;
  }

  return eRetVal;
}
/**
 * @brief Sends eIPC_SLIM_QMD_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handleQmdRequest
 * @sa eIPC_SLIM_QMD_REQUEST
 */
slimErrorEnumT Proxy::enableQmdData
(
  slimClientHandleT p_Handle,
  /**<[in] Client handle */
  const slimEnableQmdDataRequestStructT *pz_Request,
  /**<[in] Request data */
  uint8 u_TxnId
  /**<[in] Request transaction id */
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (checkStatus())
  {
    if (0 != pz_Request)
    {
      bool uValidService = false;
      switch (pz_Request->service)
      {
      case eSLIM_SERVICE_AMD:
      case eSLIM_SERVICE_RMD:
        uValidService = true;
        break;
      default:
        uValidService = false;
        break;
      }
      if (uValidService)
      {
        MutexLock _l(c_pzInstance->m_zMutex);
        if (isClientOpen(p_Handle))
        {
          slim_EnableQmdDataTxnStructType z_Txn;
          z_Txn.z_TxnData.p_Handle = p_Handle;
          z_Txn.z_TxnData.u_ClientTxnId = u_TxnId;
          z_Txn.z_Request = *pz_Request;

          eRetVal = sendIpcToMain(eIPC_SLIM_QMD_REQUEST, &z_Txn, sizeof(z_Txn));
        }
        else
        {
          eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
        }
      }
      else
      {
        eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
      }
    }
    else
    {
      eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
    }
  }
  else
  {
    eRetVal = eSLIM_ERROR_INTERNAL;
  }

  return eRetVal;
}

/**
 * @brief Sends eIPC_SLIM_VEHICLE_REQUEST message to self.
 *
 * This method is called from SLIM API client thread context.
 *
 * @return Operation status; eSLIM_SUCCESS if message is accepted for
 *         processing.
 * @sa handleVehicleRequest
 * @sa eIPC_SLIM_VEHICLE_REQUEST
 */
slimErrorEnumT Proxy::enableVehicleData
(
  slimClientHandleT p_Handle,
  /**<[in] Client handle */
  const slimEnableVehicleDataRequestStructT *pz_Request,
  /**<[in] Request data */
  uint8 u_TxnId
  /**<[in] Request transaction id */
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (checkStatus())
  {
    if (0 != pz_Request)
    {
      bool uValidService = false;
      switch (pz_Request->service)
      {
      case eSLIM_SERVICE_VEHICLE_ACCEL:
      case eSLIM_SERVICE_VEHICLE_GYRO:
      case eSLIM_SERVICE_VEHICLE_ODOMETRY:
      case eSLIM_SERVICE_VEHICLE_SPEED:
      case eSLIM_SERVICE_VEHICLE_DWS:
      case eSLIM_SERVICE_VEHICLE_GEAR:
        uValidService = true;
        break;
      default:
        uValidService = false;
        break;
      }
      if (uValidService)
      {
        MutexLock _l(c_pzInstance->m_zMutex);
        if (isClientOpen(p_Handle))
        {

          slim_EnableVehicleDataTxnStructType z_Txn;
          z_Txn.z_TxnData.p_Handle = p_Handle;
          z_Txn.z_TxnData.u_ClientTxnId = u_TxnId;
          z_Txn.z_Request = *pz_Request;

          eRetVal = sendIpcToMain(eIPC_SLIM_VEHICLE_REQUEST, &z_Txn, sizeof(z_Txn));
        }
        else
        {
          eRetVal = eSLIM_ERROR_CLIENT_HANDLE_INVALID;
        }
      }
      else
      {
        eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
      }
    }
    else
    {
      eRetVal = eSLIM_ERROR_BAD_ARGUMENT;
    }
  }
  else
  {
    eRetVal = eSLIM_ERROR_INTERNAL;
  }

  return eRetVal;
}
/**
 * @brief Initiate SLIM client open request.
 *
 * Method creates QMI-SLIM QMI Client object and initiates connection and
 * registration operations. If QMI connection is not available immediately,
 * actual connection and registration are postponed until next service event
 * (@ref eIPC_MONITOR_SERVICE_ADDED).
 *
 * This method is called from worker thread context.
 * @sa handleServiceAdded
 */
void Proxy::handleOpen
(
  const slim_OpenTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  ClientMap::const_iterator zIt = m_zClientMap.find(zRequest.p_Handle);
  if (m_zClientMap.end() != zIt)
  {
    static const slimMessageHeaderStructT zOpenError =
    {
        eSLIM_SERVICE_NONE,
        eSLIM_MESSAGE_TYPE_RESPONSE,
        eSLIM_ERROR_CLIENT_HANDLE_INVALID,
        eSLIM_MESSAGE_ID_OPEN_RESP,
        0,
        0,
    };
    zRequest.fn_Callback(zRequest.t_CallbackData, &zOpenError, 0);
  }
  else
  {
    Connection *pzX = new Connection(zRequest);
    if (pzX->start())
    {
      static const slimMessageHeaderStructT zOpenSuccess =
      {
          eSLIM_SERVICE_NONE,
          eSLIM_MESSAGE_TYPE_RESPONSE,
          eSLIM_SUCCESS,
          eSLIM_MESSAGE_ID_OPEN_RESP,
          0,
          0,
      };
      m_zClientMap[zRequest.p_Handle] = pzX;
      zRequest.fn_Callback(zRequest.t_CallbackData, &zOpenSuccess, 0);
    }
    else
    {
      static const slimMessageHeaderStructT zOpenError =
      {
          eSLIM_SERVICE_NONE,
          eSLIM_MESSAGE_TYPE_RESPONSE,
          eSLIM_ERROR_INTERNAL,
          eSLIM_MESSAGE_ID_OPEN_RESP,
          0,
          0,
      };
      zRequest.fn_Callback(zRequest.t_CallbackData, &zOpenError, 0);
    }
  }
}
/**
 * @brief Initiate SLIM client close request.
 *
 * Disconnects (if connected) and releases connection object.
 *
 * This method is called from worker thread context.
 */
void Proxy::handleClose
(
  const slim_CloseTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  ClientMap::iterator zIt = m_zClientMap.find(zRequest.z_TxnData.p_Handle);
  if (m_zClientMap.end() != zIt)
  {
    Connection *pzConnection = zIt->second;
    m_zClientMap.erase(zIt);
    pzConnection->stopAndRelease();
  }
  else
  {
    SLIM_LOGE("Bad client handle: %p", zRequest.z_TxnData.p_Handle);
  }
}
/**
 * @brief Initiate SLIM sensor request.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleSensorRequest
 */
void Proxy::handleSensorRequest
(
  const slim_EnableSensorDataTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  Connection *pzConnection = findConnection(zRequest.z_TxnData.p_Handle);
  if (0 != pzConnection)
  {
    pzConnection->handleSensorRequest(zRequest.z_TxnData.u_ClientTxnId, zRequest.z_Request);
  }
  else
  {
    SLIM_LOGE("Bad client handle: %p", zRequest.z_TxnData.p_Handle);
  }
}
/**
 * @brief Initiate SLIM pedometer request.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handlePedometerRequest
 */
void Proxy::handlePedometerRequest
(
  const slim_EnablePedometerTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  Connection *pzConnection = findConnection(zRequest.z_TxnData.p_Handle);
  if (0 != pzConnection)
  {
    pzConnection->handlePedometerRequest(zRequest.z_TxnData.u_ClientTxnId, zRequest.z_Request);
  }
  else
  {
    SLIM_LOGE("Bad client handle: %p", zRequest.z_TxnData.p_Handle);
  }
}

/**
 * @brief Initiate SLIM QMD request.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleQmdRequest
 */
void Proxy::handleQmdRequest
(
  const slim_EnableQmdDataTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  Connection *pzConnection = findConnection(zRequest.z_TxnData.p_Handle);
  if (0 != pzConnection)
  {
    pzConnection->handleQmdRequest(zRequest.z_TxnData.u_ClientTxnId, zRequest.z_Request);
  }
  else
  {
    SLIM_LOGE("Bad client handle: %p", zRequest.z_TxnData.p_Handle);
  }
}
/**
 * @brief Initiate SLIM vehicle request.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleVehicleRequest
 */
void Proxy::handleVehicleRequest
(
  const slim_EnableVehicleDataTxnStructType &zRequest
  /**< [in] Request information object */
)
{
  Connection *pzConnection = findConnection(zRequest.z_TxnData.p_Handle);
  if (0 != pzConnection)
  {
    pzConnection->handleVehicleRequest(zRequest.z_TxnData.u_ClientTxnId, zRequest.z_Request);
  }
  else
  {
    SLIM_LOGE("Bad client handle: %p", zRequest.z_TxnData.p_Handle);
  }
}
void Proxy::onQmiMonitorServiceAdded()
{
  sendIpcToMain(eIPC_MONITOR_SERVICE_ADDED, 0, 0);
}
void Proxy::onQmiMonitorServiceRemoved()
{
  sendIpcToMain(eIPC_MONITOR_SERVICE_REMOVED, 0, 0);
}
void Proxy::onQmiMonitorReleased()
{
  sendIpcToMain(eIPC_MONITOR_SHUTDOWN, 0, 0);
}
/**
 * @brief Finds connection object for given handle
 * @return Connection object or 0 if not found
 */
Connection *Proxy::findConnection
(
  slimClientHandleT pHandle
  /**<[in] SLIM client handle */
)
{
  ClientMap::const_iterator zIt = m_zClientMap.find(pHandle);
  if (m_zClientMap.end() != zIt)
  {
    Connection *pzConnection = zIt->second;
    return pzConnection;
  }
  else
  {
    return 0;
  }
}

/**
 * @brief Process QCCI stack error
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionError
 */
void Proxy::handleConnectionError
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  int32_t lError
  /**< [in] QCCI error code */
)
{
  SLIM_LOGW("Connection error:%" PRIi32, lError);

  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionError(lError);
  }
}
/**
 * @brief Process QCCI stack disconnect report
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionDisconnect
 */
void Proxy::handleConnectionDisconnect
(
  slimClientHandleT pHandle
  /**< [in] Client handle */
)
{
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionDisconnect();
  }
}
/**
 * @brief Process connection registration result
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionRegistered
 */
void Proxy::handleConnectionRegistered
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  slimErrorEnumT eError
  /**< [in] SLIM operation status */
)
{
  SLIM_LOGD("CLT Registered: clt=%p status=%d", pHandle, eError);
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionRegistered(eError);
  }
}
/**
 * @brief Process connection deregistration result
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionUnregistered
 */
void Proxy::handleConnectionUnregistered
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  slimErrorEnumT eError
  /**< [in] SLIM operation status */
)
{
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionUnregistered(eError);
  }
}
/**
 * @brief Process connection service change event
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionServiceChange
 */
void Proxy::handleConnectionServiceChange
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  slimServiceEnumT eService,
  /**< [in] SLIM service */
  slimServiceProviderMaskT qProviderMask
  /**< [in] SLIM service provider mask */
)
{
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionServiceChange(eService, qProviderMask);
  }
}
/**
 * @brief Process connection service response event.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionServiceResponse
 */
void Proxy::handleConnectionServiceResponse
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  slimServiceEnumT eService,
  /**< [in] SLIM service */
  slimErrorEnumT eError,
  /**< [in] SLIM operation status */
  uint8_t uTxId
)
{
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionServiceResponse(eService, eError, uTxId);
  }
}
/**
 * @brief Process connection service data/error indication.
 *
 * Method locates connection object for the client handle and forwards call
 * to it.
 *
 * This method is called from worker thread context.
 *
 * @sa Connection::handleConnectionServiceData
 */
void Proxy::handleConnectionServiceData
(
  slimClientHandleT pHandle,
  /**< [in] Client handle */
  slimServiceEnumT eService,
  /**< [in] SLIM service */
  slimErrorEnumT eError,
  /**< [in] SLIM operation status */
  uint32_t qSize,
  /**< [in] Payload size */
  const void *pData
  /**< [in] Payload data */
)
{
  Connection *pzConnection = findConnection(pHandle);
  if (0 != pzConnection)
  {
    pzConnection->handleConnectionServiceData(eService, eError, qSize, pData);
  }
}

/**
 * @brief Process QMI monitor new service event.
 *
 * The method iterates through available connection objects and attempts
 * to re-initiate QMI-SLIM peer connect and register operations if object
 * is disconnected.
 *
 * This method is called from worker thread context.
 * @sa QmiMonitor_ServiceAdded
 */
void Proxy::handleServiceAdded()
{
  ClientMap::const_iterator it;
  for (it = m_zClientMap.begin(); it != m_zClientMap.end(); ++it)
  {
    Connection *pzConnection = it->second;
    pzConnection->nextState();
  }
}
/**
 * @brief Process QMI monitor service removed event.
 *
 * This method is called from worker thread context.
 * @sa QmiMonitor_ServiceRemoved
 */
void Proxy::handleServiceRemoved()
{
  // Ignore message
}
/**
 * @brief Process QMI monitor shutdown event.
 *
 * This method is called from worker thread context.
 * @sa QmiMonitor_HandleReleased
 */
void Proxy::handleMonitorShutdown()
{
  SLIM_LOGE("Unexpected monitor shutdown event");
}
/**
 * @brief Process timer events.
 *
 * This method is called from worker thread context.
 */
void Proxy::handleTimer
(
  uint32_t qTimerId,
  /**<[in] Timer id */
  uint32_t qTimerParam
  /**<[in] Timer parameter */
)
{
  SLIM_LOGD("Timer operation: timer=%" PRIu32 " param=%" PRIu32,
      qTimerId, qTimerParam);

  ClientMap::const_iterator it;
  for (it = m_zClientMap.begin(); it != m_zClientMap.end(); ++it)
  {
    Connection *pzConnection = it->second;
    pzConnection->nextState();
  }
}
/**
 * @brief Update timer according to timeout information.
 */
void Proxy::updateTimer()
{
  uint64_t tNextEventTimeMs = UINT64_MAX;

  ClientMap::const_iterator it;
  for (it = m_zClientMap.begin(); it != m_zClientMap.end(); ++it)
  {
    Connection *pzConnection = it->second;
    uint64_t tConnectionEventTime = 0;
    if (pzConnection->getNextStateTime(tConnectionEventTime) &&
        tConnectionEventTime < tNextEventTimeMs)
    {
      tNextEventTimeMs = tConnectionEventTime;
    }
  }

  if (UINT64_MAX == tNextEventTimeMs)
  {
    slim_TimerStop(m_pzEventTimer);
  }
  else
  {
    int64_t tDiffMs = tNextEventTimeMs - slim_TimeTickGetMilliseconds();
    uint32_t qDiffMs;
    if (tDiffMs < 1)
    {
      qDiffMs = 1;
    }
    else if (tDiffMs < UINT32_MAX)
    {
      qDiffMs = (uint32_t)tDiffMs;
    }
    else
    {
      qDiffMs = UINT32_MAX;
    }
    slim_TimerStart(m_pzEventTimer, qDiffMs, 0);
  }
}

};//namespace
