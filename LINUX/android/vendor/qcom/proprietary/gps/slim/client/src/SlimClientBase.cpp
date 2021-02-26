/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/*!
 * @file
 * @brief Implementation of C++ SLIM Client interface.
 *
 * @ingroup slim_ClientLibrary
 */
#include <SlimClientBase.h>

#include <SlimProxy.h>
#include <slim_os_api.h>

#include <string.h>

namespace slim
{
using slimproxy::Proxy;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
 * @brief Private data container
 *
 * This class hides all implementation details from SLIM API user.
 */
class SLIM_CLIENT_LOCAL ClientBase::Private
{
  friend class ClientBase;

  /**
   * @brief Service state.
   *
   * Each service can be in one of three states:
   * - Disabled, no data is received.
   * - Enabled, data is received as it becomes available.
   * - Error, service shall be restarted if necessary.
   *
   * Each service has a mask of available providers according to SLIM API
   * definition:
   * - SSC
   * - NATIVE
   * - SAMLITE
   * - OTHER
   *
   * The service maintains a set of open transactions. Open transaction is
   * originated by enable/disable request and terminated by callback processing.
   */
  struct SLIM_CLIENT_LOCAL ServiceStatus
  {
    /**
     * @brief Service transaction entry.
     */
    struct SLIM_CLIENT_LOCAL ServiceTx
    {
      bool  m_uEnable;      /**< Type of request */
      void  *m_pUserCookie; /**< User cookie */
    };

    /** @brief Last known state */
    ClientBase::ServiceState m_eState;
    /** @brief Last known provider mask */
    slimServiceProviderMaskT m_qProviderMask;
    /** @brief Array for mapping client transactions */
    ServiceTx                m_zTxMap[4];
    /** @brief Array usage mask */
    uint64_t                 m_tTxMapUse;
  };

  //! @brief State mutex
  mutable Mutex          m_zMutex;
  //! @brief Service status array.
  ServiceStatus          m_zServiceStatuses[eSLIM_SERVICE_LAST + 1];
  //! @brief Object state.
  State                  m_eState;
  //! @brief Parent object.
  ClientBase            *m_pzOwner;

  /**
   * @brief Internal function for intercepting SLIM messages.
   *
   * @param[in] tCookie  Cookie - instance of ClientBase object.
   * @param[in] pzHeader Message header.
   * @param[in] pMessage Optional message data.
   */
  static void proxyCallback
  (
    uint64_t tCookie,
    const slimMessageHeaderStructT *pzHeader,
    void *pMessage
  );

  /**
   * @brief Constructs client private object.
   */
  Private() :
    m_zMutex(),
    m_zServiceStatuses(),
    m_eState(eSTATE_CLOSED),
    m_pzOwner(0)
  {
    memset(m_zServiceStatuses, 0, sizeof(m_zServiceStatuses));
  }

  /**
   * @brief Maps user cookie pointer into internal transaction number.
   *
   * The function locates an unused entry in cookie map, marks it used and
   * provides entry index as transaction number.
   *
   * @param[in]  eService    Transaction service.
   * @param[in]  uEnable     Operation type.
   * @param[in]  pUserCookie User-provided cookie object.
   * @param[out] uTxIdOut    Available transaction number.
   *
   * @return Operation result
   * @retval eSLIM_SUCCESS On success
   */
  slimErrorEnumT openTx
  (
    slimServiceEnumT eService,
    bool uEnable,
    void *pUserCookie,
    uint8_t &uTxIdOut
  );
  /**
   * @brief Cancels transaction.
   *
   * The function locates entry in cookie map, marks it available and restores
   * service state.
   *
   * @param[in]  eService       Transaction service
   * @param[in]  uTxId          Internal transaction number
   */
  void cancelTx
  (
    slimServiceEnumT eService,
    uint8_t uTxId
  );
  /**
   * @brief Maps internal transaction number into user cookie pointer.
   *
   * The function locates entry in cookie map, marks it available and provides
   * user cookie stored in that entry.
   *
   * @param[in]  eService       Transaction service.
   * @param[in]  uTxId          Internal transaction number.
   * @param[in]  eStatus        Operation status.
   * @param[out] pUserCookieOut User cookie.
   *
   * @return true on success; false on error.
   */
  bool completeTx
  (
    slimServiceEnumT eService,
    uint8_t uTxId,
    slimErrorEnumT eStatus,
    void *&pUserCookieOut
  );
  /**
   * @brief Internal method for processing connection open result.
   *
   * @param[in] eStatus Operation status.
   */
  void handleOpenPrivate
  (
    slimErrorEnumT eStatus
  );
  /**
   * @brief Internal method for processing service change notification.
   *
   * @param[in] zData Event contents.
   */
  void handleServiceStatusPrivate
  (
    const slimServiceStatusEventStructT &zData
  );
  /**
   * @brief Internal method for service request result.
   *
   * @param[in] eService Service.
   * @param[in] uTxnId   Transaction number.
   * @param[in] eError   Operation status.
   */
  void handleServiceEnablePrivate
  (
    slimServiceEnumT eService,
    uint8_t          uTxnId,
    slimErrorEnumT   eError
  );
  /**
   * @brief Internal method for service error notification.
   *
   * @param[in] eService Service.
   * @param[in] eError   Error code.
   */
  void handleServiceErrorPrivate
  (
    slimServiceEnumT eService,
    slimErrorEnumT   eError
  );
};

/*----------------------------------------------------------------------------
 * ClientBase::Private
 * -------------------------------------------------------------------------*/

/**
 * @brief Internal function for intercepting SLIM messages.
 *
 * @param[in] tCookie  Cookie - instance of ClientBase object.
 * @param[in] pzHeader Message header.
 * @param[in] pMessage Optional message data.
 */
void ClientBase::Private::proxyCallback
(
  uint64_t tCookie,
  const slimMessageHeaderStructT *pzHeader,
  void *pMessage
)
{
  ClientBase::Private *pzObject =
      reinterpret_cast<ClientBase::Private*>((uintptr_t)tCookie);

  if (0 != pzObject)
  {
    switch (pzHeader->msgId)
    {
    case eSLIM_MESSAGE_ID_OPEN_RESP:
      pzObject->handleOpenPrivate(pzHeader->msgError);
      break;
    case eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
    case eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP:
      pzObject->handleServiceEnablePrivate(pzHeader->service,
          pzHeader->txnId,
          pzHeader->msgError);
      break;
    case eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handleSensorData(pzHeader->service,
            *reinterpret_cast<const slimSensorDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_QMD_DATA_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handleQmdData(pzHeader->service,
            *reinterpret_cast<const slimQmdDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_PEDOMETER_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handlePedometerData(pzHeader->service,
            *reinterpret_cast<const slimPedometerDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handleVehicleSensorData(pzHeader->service,
            *reinterpret_cast<const slimVehicleSensorDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handleVehicleMotionData(pzHeader->service,
            *reinterpret_cast<const slimVehicleMotionDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->m_pzOwner->handleVehicleOdometryData(pzHeader->service,
            *reinterpret_cast<const slimVehicleOdometryDataStructT*>(pMessage));
      }
      else
      {
        pzObject->handleServiceErrorPrivate(pzHeader->service,
            pzHeader->msgError);
      }
      break;
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
      if (eSLIM_SUCCESS == pzHeader->msgError)
      {
        pzObject->handleServiceStatusPrivate(
            *reinterpret_cast<const slimServiceStatusEventStructT*>(pMessage));
      }
      break;
    default:
      break;
    }
  }
}

/**
 * @brief Maps user cookie pointer into internal transaction number.
 *
 * The function locates an unused entry in cookie map, marks it used and
 * provides entry index as transaction number.
 *
 * @param[in]  eService    Transaction service.
 * @param[in]  uEnable     Operation type.
 * @param[in]  pUserCookie User-provided cookie object.
 * @param[out] uTxIdOut    Available transaction number.
 *
 * @return Operation result
 * @retval eSLIM_SUCCESS On success
 */
slimErrorEnumT ClientBase::Private::openTx
(
  slimServiceEnumT eService,
  bool uEnable,
  void *pUserCookie,
  uint8_t &uTxIdOut
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    uint64_t       tMask = 0;
    uint8_t        uI = 0;
    ServiceStatus &zServiceStatus = m_zServiceStatuses[eService];

    MutexLock _l(m_zMutex);
    if (eSTATE_OPENED != m_eState)
    {
      eRetVal = eSLIM_ERROR_INTERNAL;
    }
    else
    {
      eRetVal = eSLIM_ERROR_MAX_TXN_COUNT_REACHED;
      for (uI = 1, tMask = 2;
           uI < ARR_SIZE(zServiceStatus.m_zTxMap);
           ++uI, tMask <<= 1)
      {
        if (0 == (zServiceStatus.m_tTxMapUse & tMask))
        {
          zServiceStatus.m_tTxMapUse |= tMask;
          uTxIdOut = uI;
          zServiceStatus.m_zTxMap[uI].m_pUserCookie = pUserCookie;
          zServiceStatus.m_zTxMap[uI].m_uEnable = uEnable;
          eRetVal = eSLIM_SUCCESS;
          break;
        }
      }
    }
  }

  return eRetVal;
}

/**
 * @brief Cancels transaction.
 *
 * The function locates entry in cookie map, marks it available and restores
 * service state.
 *
 * @param[in]  eService       Transaction service
 * @param[in]  uTxId          Internal transaction number
 */
void ClientBase::Private::cancelTx
(
  slimServiceEnumT eService,
  uint8_t uTxId
)
{
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    ServiceStatus &zServiceStatus = m_zServiceStatuses[eService];
    if (uTxId < ARR_SIZE(zServiceStatus.m_zTxMap))
    {
      MutexLock _l(m_zMutex);

      uint64_t tMask = (uint64_t)1 << uTxId;
      if (0 != (zServiceStatus.m_tTxMapUse & tMask))
      {
        zServiceStatus.m_tTxMapUse ^= tMask;
      }
    }
  }
}

/**
 * @brief Maps internal transaction number into user cookie pointer.
 *
 * The function locates entry in cookie map, marks it available and provides
 * user cookie stored in that entry.
 *
 * @param[in]  eService       Transaction service.
 * @param[in]  uTxId          Internal transaction number.
 * @param[in]  eStatus        Operation status.
 * @param[out] pUserCookieOut User cookie.
 *
 * @return true on success; false on error.
 */
bool ClientBase::Private::completeTx
(
  slimServiceEnumT eService,
  uint8_t uTxId,
  slimErrorEnumT eStatus,
  void *&pUserCookieOut
)
{
  bool uRetVal = false;
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    ServiceStatus &zServiceStatus = m_zServiceStatuses[eService];
    if (uTxId < ARR_SIZE(zServiceStatus.m_zTxMap))
    {
      MutexLock _l(m_zMutex);

      uint64_t tMask = (uint64_t)1 << uTxId;
      if (0 != (zServiceStatus.m_tTxMapUse & tMask))
      {
        zServiceStatus.m_tTxMapUse ^= tMask;
        pUserCookieOut = zServiceStatus.m_zTxMap[uTxId].m_pUserCookie;
        if (eSLIM_SUCCESS == eStatus)
        {
          zServiceStatus.m_eState = zServiceStatus.m_zTxMap[uTxId].m_uEnable ?
              eSERVICE_STATE_ENABLED : eSERVICE_STATE_DISABLED;
        }
        else
        {
          zServiceStatus.m_eState = eSERVICE_STATE_ERROR;
        }
        uRetVal = true;
      }
    }
  }
  return uRetVal;
}

/**
 * @brief Internal method for processing connection open result.
 *
 * @param[in] eStatus Operation status.
 */
void ClientBase::Private::handleOpenPrivate
(
  slimErrorEnumT eStatus
)
{
  {
    MutexLock _l(m_zMutex);
    if (eSTATE_OPENING == m_eState)
    {
      if (eSLIM_SUCCESS == eStatus)
      {
        m_eState = eSTATE_OPENED;
      }
      else
      {
        m_eState = eSTATE_CLOSED;
      }
    }
  }
  m_pzOwner->handleOpenResult(eStatus);
}

/**
 * @brief Internal method for processing service change notification.
 *
 * @param[in] zData Event contents.
 */
void ClientBase::Private::handleServiceStatusPrivate
(
  const slimServiceStatusEventStructT &zData
)
{
  m_pzOwner->handleServiceStatus(zData);

  slimServiceProviderMaskT qOldMask = 0;
  slimServiceProviderMaskT qNewMask = 0;
  if (zData.service >= eSLIM_SERVICE_FIRST &&
      zData.service < (int)ARR_SIZE(m_zServiceStatuses))
  {
    MutexLock _l(m_zMutex);
    qOldMask = m_zServiceStatuses[zData.service].m_qProviderMask;
    switch (zData.serviceStatus)
    {
    case eSLIM_SERVICE_STATUS_AVAILABLE:
    case eSLIM_SERVICE_STATUS_PROVIDERS_CHANGED:
      qNewMask = m_zServiceStatuses[zData.service].m_qProviderMask =
          zData.providerFlags;
      break;
    case eSLIM_SERVICE_STATUS_UNAVAILABLE:
      qNewMask = m_zServiceStatuses[zData.service].m_qProviderMask = 0;
      m_zServiceStatuses[zData.service].m_eState = eSERVICE_STATE_DISABLED;
      break;
    case eSLIM_SERVICE_STATUS_NONE:
    default:
      break;
    }
  }
  if (qOldMask != qNewMask)
  {
    slimServiceProviderMaskT qRemoved = qOldMask & ~qNewMask;
    slimServiceProviderMaskT qAdded = qNewMask & ~qOldMask;
    m_pzOwner->handleServiceProviderChange(
        zData.service, qRemoved, qAdded, qNewMask);
  }
}
/**
 * @brief Internal method for service request result.
 *
 * @param[in] eService Service.
 * @param[in] uTxnId   Transaction number.
 * @param[in] eError   Operation status.
 */
void ClientBase::Private::handleServiceEnablePrivate
(
  slimServiceEnumT eService,
  uint8_t          uTxnId,
  slimErrorEnumT   eError
)
{
  void *pUserCookie = 0;
  if (completeTx(eService, uTxnId, eError, pUserCookie))
  {
    m_pzOwner->handleServiceEnableResult(eService, pUserCookie, eError);
  }
}
/**
 * @brief Internal method for service error notification.
 *
 * @param[in] eService Service.
 * @param[in] eError   Error code.
 */
void ClientBase::Private::handleServiceErrorPrivate
(
  slimServiceEnumT eService,
  slimErrorEnumT   eError
)
{
  if (eService >= eSLIM_SERVICE_FIRST &&
      eService < (int)ARR_SIZE(m_zServiceStatuses))
  {
    MutexLock _l(m_zMutex);
    m_zServiceStatuses[eService].m_eState = eSERVICE_STATE_ERROR;
  }
  m_pzOwner->handleServiceError(eService, eError);
}

/*----------------------------------------------------------------------------
 * ClientBase
 * -------------------------------------------------------------------------*/

/**
 * @brief Protected constructor.
 *
 * Method takes no parameters.
 */
ClientBase::ClientBase
(
) :  m_pzPrivate(new Private)
{
  m_pzPrivate->m_pzOwner = this;
}

/**
 * @brief Protected destructor.
 *
 * Method takes no parameters.
 */
ClientBase::~ClientBase()
{
  Proxy::close((slimClientHandleT)this);
  delete m_pzPrivate;
}

/**
 * @brief Callback method for handling open result.
 *
 * This method is called from internal working thread and indicates #open
 * request completion.
 *
 * The implementation hides QMI connectivity details and successful result
 * means proper internal resource allocation, not connection to SLIM daemon.
 *
 * The default implementation does nothing.
 *
 * @param[in] eError Operation result. eSLIM_SUCCESS on success.
 */
void ClientBase::handleOpenResult
(
  slimErrorEnumT eError
)
{
  SLIM_UNUSED(eError);
}

/**
 * @brief Callback method for handling service control operation.
 *
 * This method is called from internal working thread and indicates service
 * enable/disable request completion.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService    Controlled service.
 * @param[in] pUserCookie Transaction cookie.
 * @param[in] eError      Operation status. eSLIM_SUCCESS on success.
 */
void ClientBase::handleServiceEnableResult
(
  slimServiceEnumT eService,
  void            *pUserCookie,
  slimErrorEnumT   eError
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(pUserCookie);
  SLIM_UNUSED(eError);
}

/**
 * @brief Callback method for handling service error notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService    Controlled service.
 * @param[in] eError      Service error code.
 */
void ClientBase::handleServiceError
(
  slimServiceEnumT eService,
  slimErrorEnumT   eError
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(eError);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handleSensorData
(
  slimServiceEnumT eService,
  const slimSensorDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handleQmdData
(
  slimServiceEnumT eService,
  const slimQmdDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handlePedometerData
(
  slimServiceEnumT eService,
  const slimPedometerDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handleVehicleOdometryData
(
  slimServiceEnumT eService,
  const slimVehicleOdometryDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handleVehicleSensorData
(
  slimServiceEnumT eService,
  const slimVehicleSensorDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling service data notification.
 *
 * This method is called from internal working thread and indicates service
 * error condition.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Controlled service.
 * @param[in] zData    Service data.
 */
void ClientBase::handleVehicleMotionData
(
  slimServiceEnumT eService,
  const slimVehicleMotionDataStructT &zData
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(zData);
}

/**
 * @brief Callback method for handling provider changes.
 *
 * This method is called from internal working thread and indicates service
 * provider change.
 *
 * Service provider change is indicated when low-level manages to establish
 * connection to SLIM daemon, connection is lost, or another client reserves
 * service for own use.
 *
 * The default implementation updates cached service provider masks and invokes
 * #handleServiceProviderChange method.
 *
 * @param[in] zData Event data.
 */
void ClientBase::handleServiceStatus
(
  const slimServiceStatusEventStructT &zData
)
{
  SLIM_UNUSED(zData);
}
/**
 * @brief Callback method for handling provider changes.
 *
 * This method is called from internal working thread and indicates service
 * provider change.
 *
 * Service provider change is indicated when low-level manages to establish
 * connection to SLIM daemon, connection is lost, or another client reserves
 * service for own use.
 *
 * The default implementation does nothing.
 *
 * @param[in] eService Affected service.
 * @param[in] qRemoved Removed service providers.
 * @param[in] qAdded   Added service providers.
 * @param[in] qCurrent New state service providers.
*/
void ClientBase::handleServiceProviderChange
(
  slimServiceEnumT eService,
  slimServiceProviderMaskT qRemoved,
  slimServiceProviderMaskT qAdded,
  slimServiceProviderMaskT qCurrent
)
{
  SLIM_UNUSED(eService);
  SLIM_UNUSED(qRemoved);
  SLIM_UNUSED(qAdded);
  SLIM_UNUSED(qCurrent);
}
/**
 * @brief Request connection open operation.
 *
 * Method dispatches a request to open connection.
 *
 * @param[in] uAutoStart Flag to control auto-start feature.
 *
 * @return eSLIM_SUCCESS on successful connection initiation.
 */
slimErrorEnumT ClientBase::open
(
  bool uAutoStart
)
{
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    if (eSTATE_CLOSED != m_pzPrivate->m_eState)
    {
      return eSLIM_ERROR_INTERNAL;
    }
    m_pzPrivate->m_eState = eSTATE_OPENING;
  }

  slimOpenFlagsT qOpenFlags = 0;
  if (uAutoStart)
  {
    SLIM_MASK_SET(qOpenFlags, eSLIM_OPEN_FLAGS_PERSISTENT_SERVICES);
  }

  return Proxy::open(
      (slimClientHandleT)this,
      Private::proxyCallback,
      qOpenFlags,
      (uint64_t)(uintptr_t)m_pzPrivate);
}

/**
 * @brief Request connection close operation.
 *
 * Method dispatches a request to close connection. After this method is
 * returned the object do not receive any callback notifications and all
 * transactions are aborted.
 */
void ClientBase::close()
{
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    if (eSTATE_OPENED != m_pzPrivate->m_eState)
    {
      return;
    }
    m_pzPrivate->m_eState = eSTATE_CLOSING;
    memset(
        m_pzPrivate->m_zServiceStatuses,
        0,
        sizeof(m_pzPrivate->m_zServiceStatuses));
  }
  Proxy::close((slimClientHandleT)this);
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    m_pzPrivate->m_eState = eSTATE_CLOSED;
  }
}

/**
 * @brief Returns current connection state
 *
 * @return State of the client connection.
 */
ClientBase::State ClientBase::getState() const
{
  MutexLock _l(m_pzPrivate->m_zMutex);
  return m_pzPrivate->m_eState;
}

/**
 * @brief Tests if the service status contains given service
 *
 * This method is provided for API completeness. The better approach is to
 * provide implementation for #handleServiceProviderChange method or use
 * #getServiceProviderMask.
 *
 * @param[in] zStatus  Service status event.
 * @param[in] eService Service to check.
 *
 * @return true if the service is available.
 */
bool ClientBase::serviceAvailable
(
  const slimServiceStatusEventStructT &zStatus,
  slimServiceEnumT eService
) const
{
  return Proxy::serviceAvailable(&zStatus, eService);
}
/**
 * @brief Provide mask of available service providers for a service.
 *
 * @param[in] eService Service to query.
 *
 * @return Service provider mask. 0 if no providers can be used.
 */
slimServiceProviderMaskT ClientBase::getServiceProviderMask
(
  slimServiceEnumT eService
) const
{
  slimServiceProviderMaskT qMask = 0;
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    qMask = m_pzPrivate->m_zServiceStatuses[eService].m_qProviderMask;
  }
  return qMask;
}

/**
 * @brief Provide service state.
 *
 * @param[in] eService Service to query.
 *
 * @return Service state.
 */
ClientBase::ServiceState ClientBase::getServiceState
(
  slimServiceEnumT eService
) const
{
  ServiceState zServiceState = eSERVICE_STATE_ERROR;
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    zServiceState = m_pzPrivate->m_zServiceStatuses[eService].m_eState;
  }
  return zServiceState;
}

/**
 * @brief Provide service transaction state.
 *
 * Service has open transactions when there it is waiting for a response on
 * any request.
 *
 * @param[in] eService Service to query.
 *
 * @return true if service has open transactions, false otherwise.
 */
bool ClientBase::isServiceStateInTransition
(
  slimServiceEnumT eService
) const
{
  bool uRetVal = false;;
  if (eService >= eSLIM_SERVICE_FIRST && eService <= eSLIM_SERVICE_LAST)
  {
    MutexLock _l(m_pzPrivate->m_zMutex);
    uRetVal = m_pzPrivate->m_zServiceStatuses[eService].m_tTxMapUse != 0;
  }
  return uRetVal;
}
/**
 * @brief Request sensor data service enable or disable.
 *
 * @param[in] zRequest    Request details.
 * @param[in] pUserCookie User-provided cookie for matching this request
 *                        against #handleServiceEnableResult message.
 *
 * @return eSLIM_SUCCESS if request is taken for processing.
 */
slimErrorEnumT ClientBase::enableSensorData
(
  const slimEnableSensorDataRequestStructT &zRequest,
  void *pUserCookie
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  uint8_t        uTxId   = 0;

  eRetVal = m_pzPrivate->openTx(
          zRequest.sensor, zRequest.enableConf.enable, pUserCookie, uTxId);
  if (eSLIM_SUCCESS == eRetVal)
  {
    eRetVal = Proxy::enableSensorData(
      (slimClientHandleT)this,
      &zRequest,
      uTxId);

    if (eSLIM_SUCCESS != eRetVal)
    {
      m_pzPrivate->cancelTx(zRequest.sensor, uTxId);
    }
  }

  return eRetVal;
}
/**
 * @brief Request pedometer service enable or disable.
 *
 * @param[in] zRequest    Request details.
 * @param[in] pUserCookie User-provided cookie for matching this request against
 *                        #handleServiceEnableResult message.
 *
 * @return eSLIM_SUCCESS if request is taken for processing.
 */
slimErrorEnumT ClientBase::enablePedometer
(
  const slimEnablePedometerRequestStructT &zRequest,
  void *pUserCookie
)
{
  //! @brief Helper constant
  static const slimServiceEnumT eService = eSLIM_SERVICE_PEDOMETER;

  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  uint8_t        uTxId   = 0;

  eRetVal = m_pzPrivate->openTx(
              eService, zRequest.enableConf.enable, pUserCookie, uTxId);
  if (eSLIM_SUCCESS == eRetVal)
  {
    eRetVal = Proxy::enablePedometer(
      (slimClientHandleT)this,
      &zRequest,
      uTxId);
    if (eSLIM_SUCCESS != eRetVal)
    {
      m_pzPrivate->cancelTx(eService, uTxId);
    }
  }

  return eRetVal;
}
/**
 * @brief Request QMD service enable or disable.
 *
 * @param[in] zRequest    Request details.
 * @param[in] pUserCookie User-provided cookie for matching this request against
 *                        #handleServiceEnableResult message.
 *
 * @return eSLIM_SUCCESS if request is taken for processing.
 */
slimErrorEnumT ClientBase::enableQmdData
(
  const slimEnableQmdDataRequestStructT &zRequest,
  void *pUserCookie
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  uint8_t        uTxId   = 0;

  eRetVal = m_pzPrivate->openTx(
          zRequest.service, zRequest.enableConf.enable, pUserCookie, uTxId);
  if (eSLIM_SUCCESS == eRetVal)
  {
    eRetVal = Proxy::enableQmdData(
      (slimClientHandleT)this,
      &zRequest,
      uTxId);
    if (eSLIM_SUCCESS != eRetVal)
    {
      m_pzPrivate->cancelTx(zRequest.service, uTxId);
    }
  }

  return eRetVal;
}
/**
 * @brief Request vehicle sensor service enable or disable.
 *
 * @param[in] zRequest    Request details.
 * @param[in] pUserCookie User-provided cookie for matching this request against
 *                        #handleServiceEnableResult message.
 * @return eSLIM_SUCCESS if request is taken for processing.
 */
slimErrorEnumT ClientBase::enableVehicleData
(
  const slimEnableVehicleDataRequestStructT &zRequest,
  void *pUserCookie
)
{
  slimErrorEnumT eRetVal = eSLIM_ERROR_INTERNAL;
  uint8_t        uTxId   = 0;

  eRetVal = m_pzPrivate->openTx(
              zRequest.service, zRequest.enableConf.enable, pUserCookie, uTxId);
  if (eSLIM_SUCCESS == eRetVal)
  {
    eRetVal = Proxy::enableVehicleData(
      (slimClientHandleT)this,
      &zRequest,
      uTxId);
    if (eSLIM_SUCCESS != eRetVal)
    {
      m_pzPrivate->cancelTx(zRequest.service, uTxId);
    }
  }

  return eRetVal;
}

/**
 * @brief Request SLIM common time value
 *
 * @return Time value in milliseconds
 */
uint64_t ClientBase::getCommonTimeMs()
{
  return slim_TimeTickGetMilliseconds();
}

} /* namespace slim */

