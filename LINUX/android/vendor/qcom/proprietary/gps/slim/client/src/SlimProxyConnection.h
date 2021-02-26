/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Header file for SLIM client connection component
 *
 * @ingroup slim_ClientLibrary
 */
#ifndef SLIM_PROXY_CONNECTION_H
#define SLIM_PROXY_CONNECTION_H

#include <QmiSlimClient.h>

#include <slim_utils.h>

//! @addtogroup slim_ClientLibrary
//! @{

namespace slimproxy
{
class Proxy;

/**
 * @brief Object for encapsulating QMI-SLIM client
 *
 * The connection object is created for any successful SLIM open request.
 * The object internally maintains connection state and forwards callback
 * notifications as appropriate for the duration of SLIM session.
 *
 * All SLIM API client interaction occurs using callback function, which
 * is invoked from a worker thread, while all QCCI callback methods are
 * routed into the worker thread using messaging.
 *
 * @ingroup slim_ClientLibrary
 */
class SLIM_CLIENT_LOCAL Connection :
    public virtual qmislim::Client::ICallback
{
private:
  friend class Proxy;

  /**
   * Object states
   */
  enum QmiState
  {
    eSTATE_DISCONNECTED,
    /**< No connection */
    eSTATE_DISCONNECTING,
    /**< Connection is terminating */
    eSTATE_REGISTERING,
    /**< Connection is available and client is registering */
    eSTATE_REGISTERED,
    /**< Connection is available and client is registered. */
  };
  /**
   * Target object states.
   */
  enum TargetState
  {
    eRELEASED,
    /**< Terminate connection and release object */
    eREGISTERED,
    /**< Keep client connected and registered */
  };
  /**
   * @brief Message object for passing call information.
   *
   * Message is used for passing call information from QMI-SLIM QMI Client
   * callback thread (QCCI) to main handler thread.
   */
  struct ConnectionMessage
  {
    slimClientHandleT    m_pHandle;
    /**< Client handle */
    union
    {
      struct
      {
        slimErrorEnumT       m_eError;
        slimMessageIdEnumT   m_eMsgId;
      } m_zGeneric;
      /**< Generic response data */
      struct
      {
        int32_t              m_lError;
      } m_zError;
      /**< QCCI stack error data */
      struct
      {
        slimServiceEnumT     m_eService;
        slimServiceProviderMaskT m_qServiceProviderMask;
      } m_zServiceChange;
      /**< Service change notification data */
      struct
      {
        slimServiceEnumT     m_eService;
        slimErrorEnumT       m_eError;
        uint8_t              m_uTxId;
      } m_zEnableResponse;
      /**< Enable service response data */
      struct
      {
        //! @brief SLIM Service
        slimServiceEnumT     m_eService;
        //! @brief SLIM Operation Status
        slimErrorEnumT       m_eError;
        //! @brief SLIM Message Id
        slimMessageIdEnumT   m_eMsgId;
        //! @brief Data size
        uint32_t             m_qDataSize;
        //! @brief Data
        union
        {
          //! @brief Sensor Data
          slimSensorDataStructT    m_zSensorData;
          //! @brief Pedometer Data
          slimPedometerDataStructT m_zPedometerData;
          //! @brief QMD Data
          slimQmdDataStructT       m_zQmdData;
          //! @brief Vehicle Odometry Data
          slimVehicleOdometryDataStructT m_zVehicleOdometryData;
          //! @brief Vehicle Sensor Data
          slimVehicleSensorDataStructT   m_zVehicleSensorData;
        } m_zContents;
      } m_zData;
      /**< Service data indication */
    } m_zPayload;
  };
  /*!
   * @brief Service state object
   *
   * Service state object contains necessary information for controlling
   * single service state machine.
   *
   * Service states are divided into:
   * - Client state -- The state desired by client.
   * - Confirmed state -- Last known (valid/confirmed) service state.
   * - Actual state -- Either known or transitional service state.
   */
  struct ServiceState
  {
    //! @brief Constant to mark service enable request
    static const uint8_t ENABLE_TRUE = 1;
    //! @brief Constant to mark service disable request
    static const uint8_t ENABLE_FALSE = 0;
    //! @brief Constant to mark service error request
    static const uint8_t ENABLE_RETRY = 99;

    /*!
     * @brief Service state parameters
     *
     * The parameters of the service state.
     */
    union Params
    {
      //! @brief Common field for all enable/disable states
      slimEnableServiceDataStructT        m_zEnableConf;
      //! @brief Sensor state parameters.
      slimEnableSensorDataRequestStructT  m_zEnableSensorDataRequest;
      //! @brief Pedometer state parameters.
      slimEnablePedometerRequestStructT   m_zEnablePedometerRequest;
      //! @brief QMD state parameters.
      slimEnableQmdDataRequestStructT     m_zEnableQmdDataRequest;
      //! @brief Vehicle data state parameters.
      slimEnableVehicleDataRequestStructT m_zEnableVehicleDataRequest;
      //! @brief Service timeout parameters
      struct
      {
        //! @brief Type of the timeout
        slimEnableServiceDataStructT        m_zEnableConf;
        //! @brief Next retry attempt time
        uint64_t                            m_tRetryTimeMs;
      } m_zTimeoutRequest;
    };
    slimServiceProviderMaskT m_qProviderMask;
    /**< Cached SLIM service provider mask for service indications */
    Params m_zActive;       /**< Present state */
    Params m_zInTransition; /**< Current transitional state (in progress) */
    Params m_zTarget;       /**< Latest requested state */
  };
  /**
   * @brief Transaction information container
   *
   * The object keeps request-specific information for the duration of QCCI
   * transaction.
   */
  class Transaction
  {
  public:
    /** @brief Service to which transaction belongs */
    slimServiceEnumT   m_eService;
    /** @brief User-provider transaction number */
    uint8_t            m_uTxn;
    /** @brief SLIM response message type */
    slimMessageIdEnumT m_eMessageId;
    /** @brief Time information for time request operation */
    uint32_t           m_qReferenceTimeMs;
  public:
    /**
     * Object constructor.
     */
    Transaction
    (
        slimServiceEnumT   eService,
        /**< [in] SLIM Service */
        uint8_t uTxn,
        /**< [in] SLIM client transaction number */
        slimMessageIdEnumT eMessageId
        /**< [in] Expected response type */
    );
  };
  QmiState                    m_eQmiState;
  /**< Object state */
  TargetState                 m_eTargetState;
  /**< Desired object state */
  slimClientHandleT           m_pHandle;
  /*!< Client handle */
  slimNotifyCallbackFunctionT m_pfnCallback;
  /*!< Client callback */
  slimOpenFlagsT              m_qOpenFlags;
  /*!< Client flags for opening new connections */
  uint64_t                    m_tCallbackData;
  /*!< Client callback data */
  uint64_t                    m_tExpireTimeMs;
  /**< Pending operation timestamp */
  qmislim::Client             m_zQmiClient;
  /*!< QMI-SLIM QMI client object */
  ServiceState                m_zServiceStates[eSLIM_SERVICE_LAST + 1];
  /*!< Service state information including current, desired and provider states */
  bool                        m_uAutoStart;
  /*!< Service autostart control flags */
private:

  /**
   * @brief State change method
   *
   * The method performs state change operations for connect and disconnect
   * operations.
   */
  void nextState();

  /** Sends message to main worker thread */
  void onQmiClientResponse
  (
    void                *pUserCookie,
    slimErrorEnumT       eError,
    slimMessageIdEnumT   eMsgId,
    const void          *pData,
    uint32_t             qDataSize
  );
  /** Sends message to main worker thread */
  void onQmiClientData
  (
    slimServiceEnumT     eService,
    slimErrorEnumT       eError,
    slimMessageIdEnumT   eMsgId,
    const void          *pData,
    uint32_t             qDataSize
  );
  /** Sends message to main worker thread */
  void onQmiClientTimeUpdate
  (
    void                *pUserCookie,
    slimErrorEnumT       eError,
    uint64_t             tRemoteRxTimeMs,
    uint64_t             tRemoteTxTimeMs
  );
  /** Sends message to main worker thread */
  void onQmiClientError
  (
     int32_t             lError
  );
  /** Sends message to main worker thread */
  void onQmiClientServiceChange
  (
    slimServiceEnumT         eService,
    slimServiceProviderMaskT qServiceProviderMask
  );
  /** Sends message to main worker thread */
  void onQmiClientDisconnect();
  /** Sends message to main worker thread */
  void onQmiClientRegistered
  (
    slimErrorEnumT       eError
  );
  /** Sends message to main worker thread */
  void onQmiClientUnregistered
  (
    slimErrorEnumT       eError
  );

  /**
   * @brief Send message to worker thread
   */
  void sendMessage
  (
    uint32_t qMsgId,
    /**< [in] Message identifier */
    const ConnectionMessage &zMsg
    /**< [in] Message payload */
  );
  /**
   * @brief Invoke callback if not disabled.
   */
  void invokeCallback
  (
    const slimMessageHeaderStructT &zHeader,
    /**< [in] Message header */
    void *pMessage
    /**< [in] Message payload */
  );
  /**
   * @brief Provides message identifier for service response callback.
   */
  slimMessageIdEnumT getServiceResponseMessageId
  (
    slimServiceEnumT eService
    /**<[in] Service type */
  );
  /**
   * @brief Provides message identifier for service indication callback.
   */
  slimMessageIdEnumT getServiceIndicationMessageId
  (
    slimServiceEnumT eService
    /**<[in] Service type */
  );

  void handleEnableRequest
  (
    uint8_t uTxnId,
    slimServiceEnumT eService,
    bool uUserCall
  );

public:
  /**
   * @brief Object constructor
   * Initiates object state
   */
  Connection
  (
    const slim_OpenTxnStructType &zArgs
    /**<[in] Connection open parameters */
  );
  /**
   * @brief Object destructor.
   */
  virtual ~Connection();
  /**
   * @brief Initiate operation
   *
   * @retval Operation status: true on success; false otherwise
   */
  bool start();
  /**
   * @brief Request object release
   * The method puts object to eRELEASED state and processes disconnect
   * and release when ready.
   */
  void stopAndRelease();
  /**
   * @brief Initiates sensor enable transaction request
   *
   * Method opens a transaction and forwards call to QMI-SLIM QMI client if
   * connection is registered, and/or reports an error using callback function.
   */
  void handleSensorRequest
  (
    uint8_t uTxnId,
    /**< [in] Transaction number */
    const slimEnableSensorDataRequestStructT &zRequest
    /**< [in] Transaction request data */
  );
  /**
   * @brief Initiates pedometer enable transaction request
   *
   * Method opens a transaction and forwards call to QMI-SLIM QMI client if
   * connection is registered, and/or reports an error using callback function.
   */
  void handlePedometerRequest
  (
    uint8_t uTxnId,
    /**< [in] Transaction number */
    const slimEnablePedometerRequestStructT &zRequest
    /**< [in] Transaction request data */
  );
  /**
   * @brief Initiates QMD enable transaction request
   *
   * Method opens a transaction and forwards call to QMI-SLIM QMI client if
   * connection is registered, and/or reports an error using callback function.
   */
  void handleQmdRequest
  (
    uint8_t uTxnId,
    /**< [in] Transaction number */
    const slimEnableQmdDataRequestStructT &zRequest
    /**< [in] Transaction request data */
  );
  /**
   * @brief Initiates vehicle service enable transaction request
   *
   * Method opens a transaction and forwards call to QMI-SLIM QMI client if
   * connection is registered, and/or reports an error using callback function.
   */
  void handleVehicleRequest
  (
    uint8_t uTxnId,
    /**< [in] Transaction number */
    const slimEnableVehicleDataRequestStructT &zRequest
    /**< [in] Transaction request data */
  );

  /**
   * @brief Processes QMI stack error
   *
   * Method initiates disconnect procedure.
   */
  void handleConnectionError
  (
    int32_t lError
    /**<[in] QCCI error code */
  );
  /**
   * @brief Processes disconnect result
   *
   * Depending on the state, the object may request to reconnect or release
   * itself.
   */
  void handleConnectionDisconnect
  (
  );
  /**
   * @brief Process client registration result.
   *
   * If not successful, disconnect is requested.
   */
  void handleConnectionRegistered
  (
    slimErrorEnumT eError
    /**< [in] Operation status. */
  );
  /**
   * @brief Process client unregistration result.
   *
   * If not successful, disconnect is requested.
   */
  void handleConnectionUnregistered
  (
    slimErrorEnumT eError
    /**< [in] Operation status. */
  );
  /**
   * @brief Handle service change notification.
   *
   * Computes service status and notifies SLIM API client.
   */
  void handleConnectionServiceChange
  (
    slimServiceEnumT eService,
    /**<[in] Updated service */
    slimServiceProviderMaskT qProviderMask
    /**<[in] New service provider mask */
  );
  /**
   * @brief Handle service enable result.
   *
   * Notifies SLIM API client.
   */
  void handleConnectionServiceResponse
  (
    slimServiceEnumT eService,
    /**<[in] Reporting service */
    slimErrorEnumT eError,
    /**<[in] Operation status */
    uint8_t uTxId
    /**<[in] User transaction id */
  );
  /**
   * @brief Handle service notification.
   *
   * Notifies SLIM API client.
   */
  void handleConnectionServiceData
  (
    slimServiceEnumT eService,
    /**<[in] Reporting service */
    slimErrorEnumT eError,
    /**<[in] Operation status */
    uint32_t qSize,
    /**<[in] Message data size */
    const void *pData
    /**<[in] Message data */
  );

  /**
   * @brief Query next event time.
   *
   * @return true if next event time is available.
   */
  bool getNextStateTime
  (
    uint64_t &tTimeMsOut
    /**<[out] Next event time in milliseconds */
  );
};

};//namespace

//! @}

#endif //SLIM_PROXY_H
