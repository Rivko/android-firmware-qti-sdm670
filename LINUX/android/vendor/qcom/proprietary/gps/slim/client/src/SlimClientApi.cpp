/**
 @file
 @brief Implementation of entry points to SLIM API services.

 Sensor service requests are sent to SLIM owning task IPC
 queue. They are handled in the SLIM owning task context.

 @ingroup slim_ClientLibrary
*/

/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include <slim_api.h>
#include <SlimProxy.h>

//! @addtogroup slim_ClientLibrary
//! @{

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Static Variable Definitions
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Static Function Declarations and Definitions
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/
/**
@brief Registers SLIM client with the SLIM service.

Before client can use SLIM service, this function should be always called.
It should be called with a predefined client handle that is needed in all
communications with the SLIM service. Client should also provide a
pointer to a callback function that is called whenever SLIM data is
available.

When client connection is opened, SLIM notifies which SLIM services are available
using client callback function (@see eSLIM_MESSAGE_ID_SERVICE_STATUS_IND).
Client can use @see slim_ServiceAvailable function to check the availability
of a specific service when it has received the service status indication.

@param[in]  p_Handle       Handle to identify client.
@param[in]  fn_Callback    A pointer to the client's callback function to
                           process received data.
@param[in]  t_CallbackData This data is set by the client, and will be passed
                           unmodified as a parameter to the callback function.
@param[in]  q_OpenFlags    Flags for client connection.

@return eSLIM_SUCCESS if handle is opened successfully.
Otherwise SLIM error code which means that handle is invalid and SLIM
services cannot be used with this handle.
*/
slimErrorEnumT slim_Open
(
  slimClientHandleT p_Handle,
  slimNotifyCallbackFunctionT fn_Callback,
  slimOpenFlagsT q_OpenFlags,
  uint64_t t_CallbackData
)
{
  return slimproxy::Proxy::open(
        p_Handle, fn_Callback, q_OpenFlags, t_CallbackData);
}

/**
@brief This function de-registers a client, and releases the client handle.

The client context shall not be used after calling slim_Close().

Before calling slim_Close() a client should disable sensor services it is using.
Registered callback functions will not be called after this function has returned.
However, the callbacks may be called while the client is in the process of closing.
Thus, clients should take care to insure their registered callback function will
execute properly until after this function has returned or close event is delivered.

@param[in]  p_Handle       Handle to identify client.

@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_Close
(
  slimClientHandleT p_Handle
)
{
  return slimproxy::Proxy::close(p_Handle);
}

/**
@brief Checks if specific SLIM service is available.

Function checks if specific SLIM service is available.
When service status indication is received from SLIM
(@see eSLIM_MESSAGE_ID_SERVICE_STATUS_IND), the received payload can be given
to this function to see if a specific SLIM service is available.

@param[in] pz_Status Payload from service status indication.
@param[in] e_Service SLIM service which availability should be checked.
@return true if SLIM service is available. Otherwise false.
*/
bool slim_ServiceAvailable
(
  const slimServiceStatusEventStructT *pz_Status,
  slimServiceEnumT e_Service
)
{
  return slimproxy::Proxy::serviceAvailable(pz_Status, e_Service);
}

/**
@brief Enabling or disabling of sensor data streaming.

Device sensor data streaming can be started or stopped with this function.
This function should be called once per each sensor configuration (accelerometer,
accelerometer temperature, gyroscope, gyroscope temperature, calibrated or
uncalibrated magnetometer, or barometer).
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data of
the response. If service is started successfully, indications are reported
periodically via registered client callback function.

Final result to sensor data streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_SENSOR_ACCEL or
            eSLIM_SERVICE_SENSOR_ACCEL_TEMP or
            eSLIM_SERVICE_SENSOR_GYRO or
            eSLIM_SERVICE_SENSOR_GYRO_TEMP or
            eSLIM_SERVICE_SENSOR_MAG_CALIB or
            eSLIM_SERVICE_SENSOR_MAG_UNCALIB or
            eSLIM_SERVICE_SENSOR_BARO depending on the request type
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP/no payload

Sensor data indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_SENSOR_ACCEL or
            eSLIM_SERVICE_SENSOR_ACCEL_TEMP or
            eSLIM_SERVICE_SENSOR_GYRO or
            eSLIM_SERVICE_SENSOR_GYRO_TEMP or
            eSLIM_SERVICE_SENSOR_MAG_CALIB or
            eSLIM_SERVICE_SENSOR_MAG_UNCALIB or
            eSLIM_SERVICE_SENSOR_BARO depending on the requested service type
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_SENSOR_DATA_IND/slimSensorDataStructT

@param[in]  p_Handle   Handle from @ref slim_Open call.
@param[in]  pz_Request Data for sensor configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_EnableSensorData
(
  slimClientHandleT p_Handle,
  const slimEnableSensorDataRequestStructT *pz_Request,
  uint8_t u_TxnId
)
{
  return slimproxy::Proxy::enableSensorData(p_Handle, pz_Request, u_TxnId);
}

/**
@brief Enabling or disabling of pedometer reporting.

Pedometer reporting can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to pedometer reporting request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_PEDOMETER
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP/no payload

Pedometer indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_PEDOMETER
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_PEDOMETER_IND/slimPedometerDataStructT

@param[in]  p_Handle   Handle from @ref slim_Open call.
@param[in]  pz_Request Data for pedometer configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_EnablePedometer
(
  slimClientHandleT p_Handle,
  const slimEnablePedometerRequestStructT *pz_Request,
  uint8_t u_TxnId
)
{
  return slimproxy::Proxy::enablePedometer(p_Handle, pz_Request, u_TxnId);
}

/**
@brief Enabling or disabling of QMD data streaming.

QMD (AMD/RMD) data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to QMD streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_AMD or
            eSLIM_SERVICE_RMD depending on the request type
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP/no payload

QMD data indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_AMD or
            eSLIM_SERVICE_RMD depending on the requested service type
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_QMD_DATA_IND/slimQmdDataStructT

@param[in]  p_Handle   Handle from @ref slim_Open call.
@param[in]  pz_Request Data for QMD data configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_EnableQmdData
(
  slimClientHandleT p_Handle,
  const slimEnableQmdDataRequestStructT *pz_Request,
  uint8_t u_TxnId
)
{
  return slimproxy::Proxy::enableQmdData(p_Handle, pz_Request, u_TxnId);
}

/**
@brief Enabling or disabling of vehicle data streaming.

Vehicle data streaming can be started or stopped with this function.
This function should be called once per each sensor configuration (accelerometer,
angular rate, odometry). Response to this configuration request is sent
via registered client callback function and possible error is indicated
in the message header data of the response. If service is started successfully,
indications are reported periodically via registered client callback function.

Final result to sensor data streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_VEHICLE_ACCEL or
            eSLIM_SERVICE_VEHICLE_GYRO or
            eSLIM_SERVICE_VEHICLE_ODOMETRY depending on the request type
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - Payload: No payload.

Vehicle data indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_VEHICLE_ACCEL or
            eSLIM_SERVICE_VEHICLE_GYRO or
            eSLIM_SERVICE_VEHICLE_ODOMETRY or
            eSLIM_SERVICE_VEHICLE_SPEED or
            eSLIM_SERVICE_VEHICLE_DWS or
            eSLIM_SERVICE_VEHICLE_GEAR depending on the requested service type
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND/slimVehicleSensorDataStructT
    - eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND/slimVehicleOdometryDataStructT
    - eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND/slimVehicleMotionDataStructT

@param[in]  p_Handle   Handle from @ref slim_Open call.
@param[in]  pz_Request Data for the vehicle data streaming configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_EnableVehicleData
(
  slimClientHandleT p_Handle,
  const slimEnableVehicleDataRequestStructT *pz_Request,
  uint8_t u_TxnId
)
{
  return slimproxy::Proxy::enableVehicleData(p_Handle, pz_Request, u_TxnId);
}

//! @}
