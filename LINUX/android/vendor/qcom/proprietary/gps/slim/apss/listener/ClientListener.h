/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief Client Listener header file

Client listener routes sensor data requests from various sensor
clients to the SLIM daemon task.

@ingroup slim_APSS
*/
#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "slim_utils.h"
#include "slim_internal_client_types.h"

//! @addtogroup slim_APSS
//! @{

/*----------------------------------------------------------------------------
* Declarations
* -------------------------------------------------------------------------*/
namespace slim
{
/*!
 * @brief Base class for internal SLIM clients.
 *
 * Unline external clients, that shall use SLIM client API and communicate
 * through a library interface, this client enables internal IPC integration
 * with SLIM framework.
 *
 * The class uses Internal SLIM Client API for operation.
 */
class ClientListener
{
public:
  /*!
   * @brief Object state enumeration
   */
  enum State
  {
    eSTATE_CLOSED, //!< Connection is closed
    eSTATE_OPENING,//!< Connection is in process of opening
    eSTATE_OPENED, //!< Connection is opened
  };
private:
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
  static void slimCallback
  (
    uint64 tCbData,
    const slimMessageHeaderStructT *pzMessageHeader,
    void *pMessage
  );
  /*!
   * @brief Internal connection open handler
   *
   * Method asserts the connection to SLIM core has been successfully opened.
   * In case of error, an error is logged and process terminates.
   *
   * @param[in] zHeader SLIM message header
   */
  void processOpenResponse
  (
    const slimMessageHeaderStructT &zHeader
  );

  /*!
   * @brief SLIM Context object
   *
   * This object is required by internal SLIM client API to handle various
   * error situations.
   */
  slimClientContextT m_zContext;
  /*!
   * @brief Object state.
   */
  State              m_eState;
protected:
  /*!
   * @brief State mutex
   *
   * State mutex is used for controlling service and connection state machine
   * transitions.
   */
  Mutex             m_zStateMutex;

  /*!
   * @brief Method for handling connection open response.
   *
   * @param[in] zHeader Message header.
   */
  virtual void handleOpenResponse
  (
    const slimMessageHeaderStructT &zHeader
  );
  /*!
   * @brief Method for handling sensor data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleSensorData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimSensorDataStructT &zData
  );
  /*!
   * @brief Method for handling pedometer data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handlePedometerData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimPedometerDataStructT &zData
  );
  /*!
   * @brief Method for handling motion data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleMotionData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimMotionDataStructT &zData
  );
  /*!
   * @brief Method for handling QMD indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleQmdData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimQmdDataStructT &zData
  );
  /*!
   * @brief Method for handling vehicle sensor data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleVehicleSensorData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimVehicleSensorDataStructT &zData
  );
  /*!
   * @brief Method for handling vehicle motion  data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleVehicleMotionData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimVehicleMotionDataStructT &zData
  );
  /*!
   * @brief Method for handling vehicle odometry data indication.
   *
   * @param[in] zHeader Message header.
   * @param[in] zData   Data payload.
   */
  virtual void handleVehicleOdometryData
  (
    const slimMessageHeaderStructT &zHeader,
    const slimVehicleOdometryDataStructT &zData
  );
  /*!
   * @brief Method for handling service error indication.
   *
   * @param[in] zHeader Message header.
   */
  virtual void handleServiceDataError
  (
    const slimMessageHeaderStructT &zHeader
  );
  /*!
   * @brief Method for handling service enable/disable responses.
   *
   * @param[in] zHeader Message header.
   */
  virtual void handleServiceResponse
  (
    const slimMessageHeaderStructT &zHeader
  );

  /**
   * @brief Constructor
   *
   * Initializes SLIM client component in a closed state.
   */
  ClientListener();
  /**
   * @brief Destructor
   *
   * Releases SLIM client resources. This function must be called only when
   * connection is closed.
   */
  virtual ~ClientListener();

  /**
   * @brief Routes client register to the SLIM daemon task
   *
   * Function routes client register to the SLIM daemon task.
   *
   * @param[in] qClientId  Client identifier.
   * @param[in] qOpenFlags Connection parameters.
   *
   * @return Operation result.
   * @retval eSLIM_SUCCESS The request has been successfully accepted for
   *                       processing
   */
  slimErrorEnumT slimOpen
  (
    uint32_t qClientId,
    slimOpenFlagsT qOpenFlags
  );
  /**
   * @brief Routes client de-register to the SLIM daemon task
   *
   * Function routes client register to the SLIM daemon task.
   *
   * @return Operation result.
   * @retval eSLIM_SUCCESS The connection has been successfully closed.
   */
  slimErrorEnumT slimClose();
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
  slimErrorEnumT processSensorDataRequest
  (
    uint8_t uTxId,
    const slimEnableSensorDataRequestStructT &zRequest
  );
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
  slimErrorEnumT processPedometerDataRequest
  (
    uint8_t uTxId,
    const slimEnablePedometerRequestStructT &zRequest
  );
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
  slimErrorEnumT processMotionDataRequest
  (
    uint8_t uTxId,
    const slimEnableMotionDataRequestStructT &zRequest
  );
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
  slimErrorEnumT processVehicleDataRequest
  (
    uint8_t uTxId,
    const slimEnableVehicleDataRequestStructT &zRequest
  );
};

} /* namespace slim */

//! @}

#endif //CLIENT_LISTENER_H
