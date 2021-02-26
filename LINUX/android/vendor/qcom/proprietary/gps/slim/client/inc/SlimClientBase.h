/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
#ifndef __SLIM_CLIENT_BASE_H_INCLUDED__
#define __SLIM_CLIENT_BASE_H_INCLUDED__

/**
 * @file
 * @brief C++ API for SLIM
 *
 * This file is a part of C++ SLIM client API.
 *
 * @ingroup slim_API slim_ClientLibrary
 */

#include <slim_client_types.h>

//! @addtogroup slim_API
//! @{

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#ifndef SLIM_CLIENT_API
/*!
 * @brief Attribute for interface types
 *
 * This macro defines type attribute for use with functions and classes. This
 * attribute makes symbol visible by clients.
 *
 * @sa SLIM_CLIENT_LOCAL
 */
#define SLIM_CLIENT_API __attribute__ ((visibility ("default")))
/*!
 * @brief Attribute for private types
 *
 * This macro defines type attribute for use with functions and classes. This
 * attribute makes symbol invisible by clients.
 *
 * @sa SLIM_CLIENT_API
 */
#define SLIM_CLIENT_LOCAL __attribute__ ((visibility ("hidden")))
#endif

//! @}

//! @ingroup slim_ClientLibrary
//! @ingroup slim_API
namespace slim
{
/**
 * @brief Base class for SLIM client implementations.
 *
 * The class integrates low-level SLIM interface into object model. The implementor
 * shall override any of handle methods.
 *
 * @ingroup slim_ClientLibrary
 * @ingroup slim_API
 */
class SLIM_CLIENT_API ClientBase
{
public:
  /**
   * @brief Connection states.
   *
   * Connection state represents a logical connection status. The status is
   * controlled by #open() and #close() methods and verified by service control
   * methods.
   *
   * @sa #open()
   * @sa #close()
   * @sa #handleOpenResult(slimErrorEnumT)
   */
  enum State
  {
    eSTATE_CLOSED, /**< Connection is closed. */
    eSTATE_OPENING,/**< Connection is processing #open() request. */
    eSTATE_OPENED, /**< Connection is opened. */
    eSTATE_CLOSING,/**< Connection is processing #close() request. */
  };
  /**
   * @brief Service status enumeration.
   */
  enum ServiceState
  {
    eSERVICE_STATE_DISABLED,  /**< Service is disabled. */
    eSERVICE_STATE_ENABLED,   /**< Service is enabled. */
    eSERVICE_STATE_ERROR,     /**< Service reported an error. */
  };

private:

  //! @brief Private data container
  class SLIM_CLIENT_LOCAL Private;
  //! @brief Private instance data
  Private *m_pzPrivate;

protected:
  /**
   * @brief Protected constructor.
   *
   * Method takes no parameters.
   */
  ClientBase();
  /**
   * @brief Protected destructor.
   *
   * Method takes no parameters.
   */
  virtual ~ClientBase();

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
  virtual void handleOpenResult
  (
    slimErrorEnumT eError
  );
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
  virtual void handleServiceEnableResult
  (
    slimServiceEnumT eService,
    void            *pUserCookie,
    slimErrorEnumT   eError
  );
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
  virtual void handleServiceError
  (
    slimServiceEnumT eService,
    slimErrorEnumT   eError
  );
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
  virtual void handleSensorData
  (
    slimServiceEnumT eService,
    const slimSensorDataStructT &zData
  );
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
  virtual void handleQmdData
  (
    slimServiceEnumT eService,
    const slimQmdDataStructT &zData
  );
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
  virtual void handlePedometerData
  (
    slimServiceEnumT eService,
    const slimPedometerDataStructT &zData
  );
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
  virtual void handleVehicleOdometryData
  (
    slimServiceEnumT eService,
    const slimVehicleOdometryDataStructT &zData
  );
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
  virtual void handleVehicleSensorData
  (
    slimServiceEnumT eService,
    const slimVehicleSensorDataStructT &zData
  );
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
  virtual void handleVehicleMotionData
  (
    slimServiceEnumT eService,
    const slimVehicleMotionDataStructT &zData
  );
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
  virtual void handleServiceStatus
  (
    const slimServiceStatusEventStructT &zData
  );
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
  virtual void handleServiceProviderChange
  (
    slimServiceEnumT eService,
    slimServiceProviderMaskT qRemoved,
    slimServiceProviderMaskT qAdded,
    slimServiceProviderMaskT qCurrent
  );

public:
  /**
   * @brief Request connection open operation.
   *
   * Method dispatches a request to open connection.
   *
   * @param[in] uAutoStart Flag to control auto-start feature.
   *
   * @return eSLIM_SUCCESS on successful connection initiation.
   */
  slimErrorEnumT open
  (
    bool uAutoStart = false
  );
  /**
   * @brief Request connection close operation.
   *
   * Method dispatches a request to close connection. After this method is
   * returned the object do not receive any callback notifications and all
   * transactions are aborted.
   */
  void close();

  /**
   * @brief Returns current connection state
   *
   * @return State of the client connection.
   */
  State getState() const;
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
  bool serviceAvailable
  (
    const slimServiceStatusEventStructT &zStatus,
    slimServiceEnumT eService
  ) const;
  /**
   * @brief Provide mask of available service providers for a service.
   *
   * @param[in] eService Service to query.
   *
   * @return Service provider mask. 0 if no providers can be used.
   */
  slimServiceProviderMaskT getServiceProviderMask
  (
    slimServiceEnumT eService
  ) const;
  /**
   * @brief Provide service state.
   *
   * @param[in] eService Service to query.
   *
   * @return Service state.
   */
  ServiceState getServiceState
  (
    slimServiceEnumT eService
  ) const;
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
  bool isServiceStateInTransition
  (
    slimServiceEnumT eService
  ) const;
  /**
   * @brief Request sensor data service enable or disable.
   *
   * @param[in] zRequest    Request details.
   * @param[in] pUserCookie User-provided cookie for matching this request
   *                        against #handleServiceEnableResult message.
   *
   * @return eSLIM_SUCCESS if request is taken for processing.
   */
  slimErrorEnumT enableSensorData
  (
    const slimEnableSensorDataRequestStructT &zRequest,
    void *pUserCookie = 0
  );
  /**
   * @brief Request pedometer service enable or disable.
   *
   * @param[in] zRequest    Request details.
   * @param[in] pUserCookie User-provided cookie for matching this request
   *                        against #handleServiceEnableResult message.
   *
   * @return eSLIM_SUCCESS if request is taken for processing.
   */
  slimErrorEnumT enablePedometer
  (
    const slimEnablePedometerRequestStructT &zRequest,
    void *pUserCookie = 0
  );
  /**
   * @brief Request QMD service enable or disable.
   *
   * @param[in] zRequest    Request details.
   * @param[in] pUserCookie User-provided cookie for matching this request
   *                        against #handleServiceEnableResult message.
   *
   * @return eSLIM_SUCCESS if request is taken for processing.
   */
  slimErrorEnumT enableQmdData
  (
    const slimEnableQmdDataRequestStructT &zRequest,
    void *pUserCookie = 0
  );
  /**
   * @brief Request vehicle sensor service enable or disable.
   *
   * @param[in] zRequest    Request details.
   * @param[in] pUserCookie User-provided cookie for matching this request against
   *                        #handleServiceEnableResult message.
   * @return eSLIM_SUCCESS if request is taken for processing.
   */
  slimErrorEnumT enableVehicleData
  (
    const slimEnableVehicleDataRequestStructT &zRequest,
    void *pUserCookie = 0
  );

  /**
   * @brief Request SLIM common time value
   *
   * @return Time value in milliseconds
   */
  uint64_t getCommonTimeMs();
};
} /* namespace slim */


static const slimServiceEnumT eSLIM_SERVICE_FIRST = (slimServiceEnumT)0;
/**< @brief Constant for SLIM service iterators */

/**
 * @brief Prefix increment support for slimServiceEnumT iterators
 *
 * @param[in] eService Value reference to increment.
 *
 * @return Incremented value
 */
static inline slimServiceEnumT &operator++
(
  slimServiceEnumT &eService
)
{
  eService = static_cast<slimServiceEnumT>(eService + 1);
  return eService;
}

/**
 * @brief Postfix increment support for slimServiceEnumT iterators
 *
 * @param[in] eService Value reference to increment.
 *
 * @return Value before increment
 */
static inline slimServiceEnumT operator++
(
  slimServiceEnumT &eService,
  int
)
{
  slimServiceEnumT eOldService = eService;
  eService = static_cast<slimServiceEnumT>(eService + 1);
  return eOldService;
}

#endif /* SLIM_CLIENT_API_H_INCLUDED */
