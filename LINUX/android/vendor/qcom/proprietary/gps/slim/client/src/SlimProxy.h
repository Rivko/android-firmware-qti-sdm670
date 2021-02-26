/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Header file for SLIM Proxy component.
 *
 * @ingroup slim_ClientLibrary
 */
#ifndef SLIM_PROXY_H
#define SLIM_PROXY_H

#include <errno.h>
#include <grp.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <slim_utils.h>

#include <map>

#include <QmiSlimMonitor.h>

/*!
 * @brief Internal namespace used by SLIM client library.
 *
 * @ingroup slim_ClientLibrary
 */
namespace slimproxy
{

using slim::Mutex;
using slim::Condition;
class Connection;

//! @addtogroup slim_ClientLibrary
//! @{

/**
 * @brief Class that handles SLIM API implementation using QMI-SLIM stack.
 *
 * Class implements single-threaded worker, that receives messages from
 * SLIM client and QMI-SLIM QCCI callback threads and performs event
 * processing.
 *
 * @ingroup slim_ClientLibrary
 */
class SLIM_CLIENT_LOCAL Proxy :
    private virtual qmislim::Monitor::ICallback
{
private:
  friend class Connection;
  /**
   * Standard map for matching connection objects vs client handles.
   */
  typedef std::map<slimClientHandleT,Connection*> ClientMap;

  /* Enumeration for event handling loop */
  enum
  {
    eIPC_OS_TIMER = C_OS_MSG_ID_TIMER_EXPIRY,
    /**< Timer message */
    eIPC_SLIM_OPEN_REQUEST = C_USR_MSG_ID_FIRST,
    /**< SLIM API open request message */
    eIPC_SLIM_CLOSE_REQUEST,
    /**< SLIM API close request message */
    eIPC_SLIM_SENSOR_REQUEST,
    /**< SLIM API enable sensor request message */
    eIPC_SLIM_PEDOMETER_REQUEST,
    /**< SLIM API enable pedometer request message */
    eIPC_SLIM_QMD_REQUEST,
    /**< SLIM API enable QMD request message */
    eIPC_SLIM_VEHICLE_REQUEST,
    /**< SLIM API enable vehicle service request message */
    eIPC_MONITOR_SERVICE_ADDED,
    /**< QMI-SLIM QMI monitor new service message */
    eIPC_MONITOR_SERVICE_REMOVED,
    /**< QMI-SLIM QMI monitor service removed message */
    eIPC_MONITOR_SHUTDOWN,
    /**< QMI-SLIM QMI monitor shutdown message */
    eIPC_CONNECTION_ERROR,
    /**< QMI-SLIM QCCI connection error message */
    eIPC_CONNECTION_DISCONNECT,
    /**< QMI-SLIM QCCI disconnect completed message */
    eIPC_CONNECTION_REGISTERED,
    /**< QMI-SLIM QCCI registration status message */
    eIPC_CONNECTION_UNREGISTERED,
    /**< QMI-SLIM QCCI deregistration status message */
    eIPC_CONNECTION_SERVICE_CHANGE,
    /**< QMI-SLIM QCCI service change message */
    eIPC_CONNECTION_ENABLE_RESPONSE,
    /**< QMI-SLIM QCCI service enable response message */
    eIPC_CONNECTION_DATA,
    /**< QMI-SLIM QCCI service data message */
  };
  /**
   * Object states.
   */
  enum InitState
  {
    eSTATE_INITIAL,
    /**< Initial (non-initialized) state */
    eSTATE_READY,
    /**< Initialized state */
    eSTATE_ERROR,
    /**< Error state for indicating initialization failure */
  };
  /**
   * @brief Message payload for SLIM API messages.
   */
  union MessageData
  {
    slim_OpenTxnStructType zOpenRequest;
    slim_CloseTxnStructType zCloseRequest;
    slim_EnableSensorDataTxnStructType zSensorRequest;
    slim_EnablePedometerTxnStructType zPedometerRequest;
    slim_EnableQmdDataTxnStructType zQmdRequest;
    slim_EnableVehicleDataTxnStructType zVehicleRequest;
  };

  static Mutex c_zStateMutex;
  /**< Initialization mutex */
  static Condition  c_zStateCond;
  /**< Initialization condition variable */
  static Proxy      *c_pzInstance;
  /**< Object singleton instance */
  static const uint32_t  c_qThreadId;
  /**< Object IPC thread id */
  static const uint32_t  c_qDefaultTimerId;
  /**< Object IPC timer id */
  // static volatile bool   c_uReady;
  static volatile InitState c_eInitState;
  /**< Singleton initialization status */
  static volatile uint32_t c_qInitCount;

  slim_ThreadT        m_zThread;
  /**< Thread object */
  MessageData         m_zMessageData;
  /**< Buffer for unpacking SLIM API messages */
  ClientMap           m_zClientMap;
  /**< Map for exiting QMI-SLIM connections. */
  qmislim::Monitor    m_zQmiMonitor;
  /**< QMI-SLIM QMI monitor service */
  slim_TimerT        *m_pzEventTimer;
  /**< Event timer */
  Mutex               m_zMutex;
  /** Mutex for connection state locks*/

  /**
   * @brief Library initialization
   *
   * Function initializes logging and instantiates worker instance.
   */
  static void configure() __attribute__((constructor));
  /**
   * @brief Object state verification.
   *
   * The method checks for proper singleton object initialization.
   * @return Object status.
   */
  static bool checkStatus();
  /**
   * @brief Sends IPC message to worker thread
   */
  static slimErrorEnumT sendIpcToMain
  (
    uint32_t qMsgId,
    /**< [in] Message number */
    const void *pData,
    /**< [in] Message data */
    size_t qSize
    /**< [in] Message data size */
  );

  /**
   * @brief Thread entry function
   */
  static void threadMain
  (
    void *pArg
    /**< [in] Parameter (SlimProxy*) */
  );

  /**
   * @brief Checks if the client connection is in open state.
   *
   * This method shall be called from the protected context.
   *
   * @return true if connection object exists and in open state.
   */
  static bool isClientOpen
  (
    slimClientHandleT pHandle
    /**[in] Client handle pointer */
  );

  /**
   * @brief Constructs object
   */
  Proxy();
  /**
   * @brief Destroys object
   */
  ~Proxy();
  /**
   * @brief Finds connection object for given handle
   * @return Connection object or 0 if not found
   */
  Connection *findConnection
  (
    slimClientHandleT pHandle
    /**<[in] SLIM client handle */
  );
  /**
   * @brief Starts operation
   * Method initializes QMI-SLIM service monitor and starts worker thread.
   * @return Operation status; false on error.
   */
  bool start();
  /**
   * @brief Main event handling loop function.
   *
   * The function processes inbound messages and dispatches them to one
   * of object's or connection methods.
   */
  void runEventLoop();


  /* SLIM API messages */
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
  void handleOpen
  (
    const slim_OpenTxnStructType &zRequest
    /**< [in] Request information object */
  );
  /**
   * @brief Initiate SLIM client close request.
   *
   * Disconnects (if connected) and releases connection object.
   *
   * This method is called from worker thread context.
   */
  void handleClose
  (
    const slim_CloseTxnStructType &zRequest
    /**< [in] Request information object */
  );
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
  void handleSensorRequest
  (
    const slim_EnableSensorDataTxnStructType &zRequest
    /**< [in] Request information object */
  );
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
  void handlePedometerRequest
  (
    const slim_EnablePedometerTxnStructType &zRequest
    /**< [in] Request information object */
  );
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
  void handleQmdRequest
  (
    const slim_EnableQmdDataTxnStructType &zRequest
    /**< [in] Request information object */
  );
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
  void handleVehicleRequest
  (
    const slim_EnableVehicleDataTxnStructType &zRequest
    /**< [in] Request information object */
  );

  /* QMI-SLIM messages */
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
  void handleConnectionError
  (
    slimClientHandleT pHandle,
    /**< [in] Client handle */
    int32_t lError
    /**< [in] QCCI error code */
  );
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
  void handleConnectionDisconnect
  (
    slimClientHandleT pHandle
    /**< [in] Client handle */
  );
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
  void handleConnectionRegistered
  (
    slimClientHandleT pHandle,
    /**< [in] Client handle */
    slimErrorEnumT eError
    /**< [in] SLIM operation status */
  );
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
  void handleConnectionUnregistered
  (
    slimClientHandleT pHandle,
    /**< [in] Client handle */
    slimErrorEnumT eError
    /**< [in] SLIM operation status */
  );
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
  void handleConnectionServiceChange
  (
    slimClientHandleT pHandle,
    /**< [in] Client handle */
    slimServiceEnumT eService,
    /**< [in] SLIM service */
    slimServiceProviderMaskT qProviderMask
    /**< [in] SLIM service provider mask */
  );
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
  void handleConnectionServiceResponse
  (
    slimClientHandleT pHandle,
    /**< [in] Client handle */
    slimServiceEnumT eService,
    /**< [in] SLIM service */
    slimErrorEnumT eError,
    /**< [in] SLIM operation status */
    uint8_t uTxId
  );
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
  void handleConnectionServiceData
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
  );

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
  void handleServiceAdded();
  /**
   * @brief Process QMI monitor service removed event.
   *
   * This method is called from worker thread context.
   * @sa QmiMonitor_ServiceRemoved
   */
  void handleServiceRemoved();
  /**
   * @brief Process QMI monitor shutdown event.
   *
   * This method is called from worker thread context.
   * @sa QmiMonitor_HandleReleased
   */
  void handleMonitorShutdown();

  /**
   * @brief Process timer events.
   *
   * This method is called from worker thread context.
   */
  void handleTimer
  (
    uint32_t qTimerId,
    /**<[in] Timer id */
    uint32_t qTimerParam
    /**<[in] Timer parameter */
  );
  /**
   * @brief Update timer according to timeout information.
   */
  void updateTimer();

  /* QMI-SLIM monitor calls */
  /**
   * @brief Sends @ref eIPC_MONITOR_SERVICE_ADDED message to self.
   *
   * This method is called from QMI monitor QCCI context.
   * @sa handleServiceAdded
   */
  void onQmiMonitorServiceAdded();
  /**
   * @brief Sends @ref eIPC_MONITOR_SERVICE_REMOVED message to self.
   *
   * This method is called from QMI monitor QCCI context.
   * @sa handleServiceRemoved
   */
  void onQmiMonitorServiceRemoved();
  /**
   * @brief Sends @ref eIPC_MONITOR_SHUTDOWN message to self.
   *
   * This method is called from QMI monitor QCCI context.
   * @sa handleMonitorShutdown
   */
  void onQmiMonitorReleased();

public:
  /**
   * @brief Sends eIPC_SLIM_OPEN_REQUEST message to self.
   *
   * This method is called from SLIM API client thread context.
   *
   * @return Operation status; eSLIM_SUCCESS if message is accepted for
   *         processing.
   * @sa handleOpen
   *
   * @sa eIPC_SLIM_OPEN_REQUEST
   */
  static slimErrorEnumT open
  (
    slimClientHandleT p_Handle,
    /**<[in] Client handle */
    slimNotifyCallbackFunctionT fn_Callback,
    /**<[in] Callback function */
    slimOpenFlagsT q_OpenFlags,
    /**<[in] Open flags */
    uint64 t_CallbackData
    /**<[in] Callback data */
  );
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
  static slimErrorEnumT close
  (
    slimClientHandleT p_Handle
    /**<[in] Client handle */
  );
  /**
   * @brief Service availability test.
   *
   * This method is called from SLIM API client thread context.
   *
   * @retval TRUE if service is available
   * @retval FALSE if service is not available or invalid argument
   */
  static bool serviceAvailable
  (
    const slimServiceStatusEventStructT *pz_Status,
    /**<[in] Status container */
    slimServiceEnumT e_Service
    /**<[in] Service to test */
  );
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
  static slimErrorEnumT enableSensorData
  (
    slimClientHandleT p_Handle,
    /**<[in] Client handle */
    const slimEnableSensorDataRequestStructT *pz_Request,
    /**<[in] Request data */
    uint8 u_TxnId
    /**<[in] Request transaction id */
  );
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
  static slimErrorEnumT enablePedometer
  (
    slimClientHandleT p_Handle,
    /**<[in] Client handle */
    const slimEnablePedometerRequestStructT *pz_Request,
    /**<[in] Request data */
    uint8 u_TxnId
    /**<[in] Request transaction id */
  );
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
  static slimErrorEnumT enableQmdData
  (
    slimClientHandleT p_Handle,
    /**<[in] Client handle */
    const slimEnableQmdDataRequestStructT *pz_Request,
    /**<[in] Request data */
    uint8 u_TxnId
    /**<[in] Request transaction id */
  );
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
  static slimErrorEnumT enableVehicleData
  (
    slimClientHandleT p_Handle,
    /**<[in] Client handle */
    const slimEnableVehicleDataRequestStructT *pz_Request,
    /**<[in] Request data */
    uint8 u_TxnId
    /**<[in] Request transaction id */
  );
};

//! @}

};//namespace

#endif //SLIM_PROXY_H
