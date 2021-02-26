/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief Declaration of base class for multiplexing providers.
 *
 * This file defines the implementation of common methods shared
 * between sensor providers.
 *
 * @ingroup slim_ProviderCommon
 */
#ifndef __MULTIPLEXING_PROVIDER_H_INCLUDED__
#define __MULTIPLEXING_PROVIDER_H_INCLUDED__

#include <ProviderBase.h>

//! @addtogroup slim_ProviderCommon
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
namespace slim
{

/**
 * @brief Multiplexing provider base class.
 *
 * Multiplexing provider implements default methods for service operation. It
 * provides routing methods that send service data to all clients that enabled
 * it.
 *
 * @ingroup slim_ProviderCommon
 */
class MultiplexingProvider : public ProviderBase
{
protected:
  /*!
   * @brief Common provider-specific message enumeration.
   *
   * The enumeration defines common message identifiers that are used for
   * routing information between provider tasks and SLIM core task.
   *
   * @sa MultiplexingProvider::routeIndication
   * @sa MultiplexingProvider::dispatchIndication
   * @sa MultiplexingProvider::routeTimeResponse
   * @sa MultiplexingProvider::dispatchTimeResponse
   * @sa MultiplexingProvider::routeGenericResponse
   * @sa MultiplexingProvider::dispatchGenericResponse
   */
  enum
  {
      eMSG_ID_SENSOR_DATA_INJECT = eMSG_ID_BASE_COMMON_END,
      /**< Sensor data message identifier */
      eMSG_ID_TIME_SYNC_DATA_INJECT,
      /**< Time response message identifier */
      eMSG_ID_MOTION_DATA_INJECT,
      /**< Motion data message identifier */
      eMSG_ID_PEDOMETER_INJECT,
      /**< Pedometer data message identifier */
      eMSG_ID_VEHICLE_ODOMETRY_DATA_INJECT,
      /**< Vehicle odometry data message identifier */
      eMSG_ID_VEHICLE_SENSOR_DATA_INJECT,
      /**< Vehicle sensor data message identifier */
      eMSG_ID_VEHICLE_MOTION_DATA_INJECT,
      /**< Vehicle motion data message identifier */
      eMSG_ID_GENERIC_SERVICE_RESPONSE,
      /**< Generic response (no payload) message identifier */
      eMSG_ID_COMMON_END,
      /**< Last identifier for sub-class usage */
  };
private:

  //! @brief Container for indication messages
  struct IndicationDataMsg
  {
    //! @brief SLIM service.
    slimServiceEnumT   eService;
    //! @brief SLIM error code.
    slimErrorEnumT     eError;
    //! @brief Payload union.
    union {
      //! @brief Sensor service data
      slimSensorDataStructT sensorData;
      //! @brief Pedometer service data
      slimPedometerDataStructT pedometerData;
      //! @brief Motion service data
      slimMotionDataStructT motionData;
      //! @brief Vehicle sensor service data
      slimVehicleSensorDataStructT vehicleSensorData;
      //! @brief Vehicle odometry service data
      slimVehicleOdometryDataStructT vehicleOdometryData;
      //! @brief Vehicle motion service data
      slimVehicleMotionDataStructT vehicleMotionData;
    } data;
  };

  //! @brief Container for response messages
  struct ResponseDataMsg
  {
      //! @brief SLIM error code.
      slimErrorEnumT eError;
      //! @brief SLIM transaction number.
      int32 serviceTxnId;
      //! @brief Payload union.
      union {
        struct
        {
          //! @brief Remote provider request time in milliseconds.
          uint64_t remoteProviderRxTime;
          //! @brief Remote provider response time in milliseconds.
          uint64_t remoteProviderTxTime;
        };
      } data;
  };

protected:
  /*! @brief Service status keeps track of service state. */
  slim_ServiceStatusStructType  m_zServiceStatus[SLIM_SERVICE_COUNT];
  /*! @brief Provider status for multiplexing control */
  slim_ProviderStatusStructType m_zProviderStatus;
  /**
   * @brief Protected constructor
   *
   * @param[in] eProvider    Type of the provider.
   * @param[in] uCommonTime  When @a true provider reports all data using SLIM
   *                         Core timer API.
   * @param[in] eServiceMask Initial set of available services for this provider.
   */
  MultiplexingProvider
  (
    slim_ProviderEnumType eProvider,
    bool uCommonTime,
    slimAvailableServiceMaskT eServiceMask
  );

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
  slimErrorEnumT notifySuccessfulEnableRequestForRate
  (
    slimClientHandleT pHandle,
    bool bEnable,
    uint16_t wBatchRateHz,
    uint16_t wSampleCount,
    uint16_t wClientBatchRateHz,
    uint16_t wClientSampleCount,
    int32_t lServiceTxnId,
    slimServiceEnumT eService
  );
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
  slimErrorEnumT notifySuccessfulEnableRequest
  (
    slimClientHandleT pHandle,
    bool b_Enable,
    int32_t lServiceTxnId,
    slimServiceEnumT eService
  );

  /**
   * @brief Opens provider connection for specific client.
   *
   * Function opens provider connection for a specific client.
   *
   * @param[in] pHandle Client handle.
   *
   * @return eSLIM_SUCCESS if connection is opened successfully.
   */
  virtual slimErrorEnumT openClientConnection
  (
    slimClientHandleT pHandle
  );
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
  virtual slimErrorEnumT closeClientConnection
  (
    const slimClientHandleT pHandle
  );
  /**
   * @brief Handler for messages targeted for provider.
   *
   * Function handles messages sent via SLIM core to provider.
   *
   * @param[in] qMessageId      Message id.
   * @param[in] qMsgPayloadSize Message size.
   * @param[in] pMsgPayload     Pointer to the message payload.
   */
  virtual void handleProviderMessage
  (
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
  );

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
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );

  /*!
   * @brief Initiates sensor data request.
   *
   * Function initiates sensor data request. If request is successful, sensor
   * data streaming is either started or stopped.
   *
   * The method uses multiplexing services and implementation class shall
   * provide #doUpdateSensorStatus(bool,uint16_t,uint16_t,slimServiceEnumT)
   * method and use #routeIndication(slimServiceEnumT,const slimSensorDataStructT&)
   * for data reporting.
   *
   * @param[in]  zTxn   Transaction data.
   * @param[in]  lTxnId Service transaction id.
   *
   * @return eSLIM_SUCCESS if sensor data request is made successfully.
   */
  virtual slimErrorEnumT enableSensorDataRequest
  (
    const slim_EnableSensorDataTxnStructType &zTxn,
    int32_t lTxnId
  );
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
  virtual slimErrorEnumT enablePedometerRequest
  (
    const slim_EnablePedometerTxnStructType &zTxn,
    int32_t lTxnId
  );
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
  virtual slimErrorEnumT enableMotionDataRequest
  (
    const slim_EnableMotionDataTxnStructType &zTxn,
    int32_t lTxnId
  );
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
  virtual slimErrorEnumT enableVehicleDataRequest
  (
    const slim_EnableVehicleDataTxnStructType &zTxn,
    int32_t lTxnId
  );

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
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );
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
  virtual slimErrorEnumT doUpdatePedometerStatus
  (
    bool uEnable,
    bool uResetStepCount,
    uint32_t qStepCountThreshold
  );
  /**
   * @brief Method for enabling or disabling motion data service.
   *
   * @param[in] uEnable Flag that indicates if the service shall be enabled or
   *                    disabled.
   *
   * @return eSLIM_SUCCESS is operation succeeded.
   */
  virtual slimErrorEnumT doUpdateMotionDataStatus
  (
    bool uEnable
  );
  /**
   * @brief Method for enabling or disabling vehicle services.
   *
   * @param[in] uEnable Flag that indicates if the service shall be enabled or
   *                    disabled.
   * @param[in] eService      Service to control.
   *
   * @return eSLIM_SUCCESS is operation succeeded.
   */
  virtual slimErrorEnumT doUpdateVehicleDataStatus
  (
    bool uEnable,
    slimServiceEnumT eService
  );

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
  virtual bool getMtOffsetForService
  (
    slimClientHandleT pHandle,
    slimServiceEnumT eService,
    slim_ServiceMtDataStructType &zMtOffsetOut
  );

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
  virtual void setMtOffsetForService
  (
    slimClientHandleT pHandle,
    slimServiceEnumT eService,
    const slim_ServiceMtDataStructType &zMtOffset,
    bool bSetForAllClients
  );

  /**
  @brief Converts sensor type from sensor number to slim sensor enum number
  This virtual function is needed because it can be used by other providers.
  Overriden by derived class like SensorHalProvider.

  @param[in] sensorType - sensor number
  @return slimSensorTypeEnumT enum value
  */
  virtual slimSensorTypeEnumT SensorTypeToSlimSensorType
  (
    int sensorType
  )
  {
     return eSLIM_SENSOR_TYPE_MAX;
  }

  /**
  @brief Converts sensor number to sensor name.
  This virtual function is needed because it can be used by other providers.
  Overriden by derived class like SensorHalProvider

  @param[in] type - sensor number
  @return character pointer (name of sensor)
  */
  virtual char const* getSensorName
  (
    int type
  )
  {
     return "ukn";
  }

  /**
  @brief Converts slim sensor number to corresponding service
   This virtual function is needed because it can be used by other providers.
   Overriden by derived class like SensorHalProvider

  @param[in] slimSensorTypeEnumT  slimSensorType- sensor enum value
  @return slimServiceEnumT service corresponding to the sensor number
  */
  virtual slimServiceEnumT SlimSensorTypeToService
  (
     slimSensorTypeEnumT slimSensorType
  )
  {
     return eSLIM_SERVICE_NONE;
  }

private:
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
  slimErrorEnumT routeIndication
  (
    slimServiceEnumT   eService,
    slimErrorEnumT     eError,
    const void  *pData,
    uint32_t qSize
  );
protected:
  /**
   * @brief Multiplex sensor data
   *
   * @param[in] eService Reporting service
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    slimServiceEnumT   eService,
    const slimSensorDataStructT &zData
  );
  /**
   * @brief Multiplex motion data
   *
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
   const slimMotionDataStructT &zData
  );
  /**
   * @brief Multiplex pedometer data
   *
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    const slimPedometerDataStructT &zData
  );
  /**
   * @brief Multiplex vehicle odometry data
   *
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    const slimVehicleOdometryDataStructT &zData
  );
  /**
   * @brief Multiplex vehicle sensor data
   *
   * @param[in] eService Reporting service
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    slimServiceEnumT   eService,
    const slimVehicleSensorDataStructT &zData
  );
  /**
   * @brief Multiplex vehicle motion data
   *
   * @param[in] eService Reporting service
   * @param[in] zData    Service data
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    slimServiceEnumT   eService,
    const slimVehicleMotionDataStructT &zData
  );
  /**
   * @brief Multiplex service error indication
   *
   * @param[in] eService Reporting service
   * @param[in] eError   Service error
   *
   * @return Operation status
   * @retval eSLIM_SUCCESS On success.
   */
  slimErrorEnumT routeIndication
  (
    slimServiceEnumT   eService,
    slimErrorEnumT     eError
  );

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
  slimErrorEnumT routeTimeResponse
  (
    int32_t lTxnId,
    slimErrorEnumT eError,
    uint64_t tRemoteRxTimeMsecs,
    uint64_t tRemoteTxTimeMsecs
  );

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
  slimErrorEnumT routeGenericResponse
  (
    int32_t lTxnId,
    slimErrorEnumT eError
  );

  /**
  @brief Keeps the Sensor data in the buffer and update the Dispatch Time to send the sensor data.

  Function wraps the slim_ServiceStatusGetDispatchTime function.

  @param[in] eService               SLIM service.
  @param[in] SensorData             ACCEL or GYRO SensorData will hold the Sensor data.
  @param[in] inputSensorData        Iput ACCEL or GYRO Single SensorData.
  @param[in] tDispatchTimestamp     Current Dispatch Timestamp for the sensor data.
  @param[in] bIsPPSTimeNotReached   To Synch once PPS Time Received flag should be kept
  @param[in] wBatchCount            Number of sensor samples to be kept in a batch

  @retval void
  */
  bool isBatchReady
  (
    slimServiceEnumT eService,
    slimSensorDataStructT& sensorData,
    const slimSensorDataStructT& inputSensorData,
    uint64_t& tDispatchTimestamp,
    bool& bIsPPSTimeNotReached,
    const uint16_t& wBatchCount
  );

   /**
  * @brief clean the buffers used for Batching in case of Disable sensor data.
  *  Overriden by the derived class like SensorHalProvider.

  * @return void.
  */
  virtual void cleanBatchingBuffers()
  {
  }

  /**
  @brief Method for Setting the batching status

  @param[in] eService     Service to control.
  */
  void enableBatching
  (
    slimServiceEnumT eService
  );


  // Make base class dispatch methods visible
  using ProviderBase::dispatchTimeResponse;

private:
  /**
   * @brief Dispatch function for data indications
   *
   * Sensor providers should use this function to dispatch data indications
   * to SLIM core.
   *
   * @param[in] zMessage Message structure.
   * @internal
   */
  void dispatchIndication
  (
    const IndicationDataMsg &zMessage
  );

  /**
   * @brief Dispatch function for time response.
   *
   * AP provider modules should use this function to dispatch time responses
   * to SLIM core.
   *
   * @param[in] zMessage Message structure.
   * @internal
   */
  void dispatchTimeResponse
  (
    const ResponseDataMsg &zMessage
  );
  /**
   * @brief Dispatch function for generic responses
   *
   * Provider modules should use this function to dispatch generic service
   * responses to SLIM core.
   *
   * @param[in] zMessage Message structure.
   * @internal
   */
  void dispatchGenericResponse
  (
    const ResponseDataMsg &zMessage
  );
};

} /* namespace slim */

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

//! @}

#endif //__MULTIPLEXING_PROVIDER_H_INCLUDED__
