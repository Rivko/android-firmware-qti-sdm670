#ifndef SLIM_API_H
#define SLIM_API_H
/**
  @file
  @brief SLIM API header file

  SLIM service provides sensor data to registered clients.

  All clients communicate with the SLIM service via function calls with a
  client handle, and receive responses and indications via registered callback
  function.

  @ingroup slim_Api
*/
/*============================================================================

               Copyright (c) 2013-2017 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/client/inc/slim_api.h#5 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include <stdbool.h>
#include "slim_client_types.h"

/**
@addtogroup slim_Api
@{
*/

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#ifndef SLIM_API
/**
@brief Macro for proper function linkage

This macro depends on selected operation system and compilation type. Possible
values are:
- extern -- Static library compilation/use on most platforms; shared library on
            Linux/Android.
- __declspec(dllexport) -- Shared library build on Win32 platform.
- __declspec(dllimport) -- Shared library use on Win32 platform.
*/
#define SLIM_API extern
#endif /* SLIM_API */

#ifdef __cplusplus
extern "C" {
#endif

/*
  For details, check SLIM API version tracking QWiki page at:
  http://qwiki.qualcomm.com/qct-gpsone/SLIM/SLIM_API#Version

  When adding a new sensor service type, or updating an existing service
  type in SLIM, please make appropriate changes at following places:
    1. Update SLIM API version tracking table at the Qwiki link above, and
    2. Update the SLIM API version id in slim_api.h header (this) file

  Updated:
    02/01/2016   -- initial version
    01/25/2017   -- added three new vehicle services
*/
#define SLIM_API_VERSION_ID  (2)

/*----------------------------------------------------------------------------
* Client types for different clients
* -------------------------------------------------------------------------*/
/*
Client types are defined only on MP.
*/
/**
@brief Handle for SAP use
*/
extern const uint32_t slim_SAPHandle;
/**
@brief Handle for Geofencing use
*/
extern const uint32_t slim_GFHandle;
/**
@brief Handle for SAMLite use
*/
extern const uint32_t slim_SAMLHandle;

/*----------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/**
@brief This function returns SLIM-API version.

@return SLIM API version
*/
SLIM_API uint32_t slim_GetApiVersion
(
  void
);

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
SLIM_API slimErrorEnumT slim_Open
(
  slimClientHandleT p_Handle,
  slimNotifyCallbackFunctionT fn_Callback,
  slimOpenFlagsT q_OpenFlags,
  uint64_t t_CallbackData
);

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
SLIM_API slimErrorEnumT slim_Close
(
  slimClientHandleT p_Handle
);

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
SLIM_API bool slim_ServiceAvailable
(
  const slimServiceStatusEventStructT *pz_Status,
  slimServiceEnumT e_Service
);

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

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for sensor configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableSensorData
(
  slimClientHandleT p_Handle,
  const slimEnableSensorDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of motion data streaming.

Motion data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to motion data streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_MOTION_DATA
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP/no payload

Motion data indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_MOTION_DATA
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_MOTION_DATA_IND/slimMotionDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for motion data configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/

SLIM_API slimErrorEnumT slim_EnableMotionData
(
  slimClientHandleT p_Handle,
  const slimEnableMotionDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

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

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for pedometer configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnablePedometer
(
  slimClientHandleT p_Handle,
  const slimEnablePedometerRequestStructT *pz_Request,
  uint8_t u_TxnId
);

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

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for QMD data configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableQmdData
(
  slimClientHandleT p_Handle,
  const slimEnableQmdDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of SMD data streaming.

SMD data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to SMD streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_SMD
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_SMD_DATA_ENABLE_RESP/no payload

SMD data indications are provided to client via registered callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_SMD
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_SMD_DATA_IND/slimSmdDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for SMD data configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableSmdData
(
  slimClientHandleT p_Handle,
  const slimEnableSmdDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling Distance Bound service.

Distance Bound (CMC-based) service can be started or stopped with this
function. Response to this configuration request is sent via registered
client callback function and possible error is indicated in the message header
data of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to distance bound service enable request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_DISTANCE_BOUND
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_ENABLE_RESP/no payload

Distance bound data indications are provided to client via registered callback
function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_DISTANCE_BOUND
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_DISTANCE_BOUND_IND/slimDistanceBoundDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for distance bound service configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableDistanceBound
(
  slimClientHandleT p_Handle,
  const slimEnableDistanceBoundRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Set request for Distance Bound service.

When the distance bound service has been enabled, client can set the desired
bounds with SET-message. The response for the set request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_DISTANCE_BOUND
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_SET_RESP/no payload.

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for distance bound set request
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_SetDistanceBound
(
  slimClientHandleT p_Handle,
  const slimSetDistanceBoundRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Get request for Distance Bound service.

Distance bound report can be gotten also by request (after the reporting has
been enabled). Result to the get request is provided to client via registered
callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_DISTANCE_BOUND
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_GET_RESP/slimDistanceBoundDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_GetDistanceBoundReport
(
  slimClientHandleT p_Handle,
  uint8_t u_TxnId
);

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
            eSLIM_SERVICE_VEHICLE_ODOMETRY or
            eSLIM_SERVICE_VEHICLE_SPEED or
            eSLIM_SERVICE_VEHICLE_DWS or
            eSLIM_SERVICE_VEHICLE_GEAR depending on the request type
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

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for the vehicle data streaming configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableVehicleData
(
  slimClientHandleT p_Handle,
  const slimEnableVehicleDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of pedestrian alignment data streaming.

Pedestrian alignment data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data of
the response. If service is started successfully, indications are reported
via registered client callback function.

Final result to sensor data streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_PED_ALIGNMENT
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - Payload: No payload.

Pedestrian alignment data indications are provided to client via registered
callback function.
@see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_PED_ALIGNMENT
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_PED_ALIGNMENT_IND/slimPedestrianAlignmentDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for the pedestrian alignment streaming configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnablePedestrianAlignment
(
  slimClientHandleT p_Handle,
  const slimEnablePedestrianAlignmentRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of magnetic field data streaming.

Magnetic field data streaming can be started or stopped with this function.
Response to this configuration request is sent via registered client callback
function and possible error is indicated in the message header data
of the response. If service is started successfully, indications are
reported periodically via registered client callback function.

Final result to magnetic field data streaming request is provided to client
via registered callback function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_MAG_FIELD_DATA
 - Type:    eSLIM_MESSAGE_TYPE_RESPONSE
 - Error:   eSLIM_SUCCESS if request was successful, otherwise SLIM error code.
 - ID/Payload: eSLIM_MESSAGE_ID_MAG_FIELD_DATA_ENABLE_RESP/no payload

Magnetic field data indications are provided to client via registered callback
function. @see slimNotifyCallbackFunctionT
 - Service: eSLIM_SERVICE_MAG_FIELD_DATA
 - Type:    eSLIM_MESSAGE_TYPE_INDICATION
 - Error:   SLIM error code if error was detected in indication.
            eSLIM_SUCCESS otherwise.
 - ID/Payload:
    - eSLIM_MESSAGE_ID_MAG_FIELD_DATA_IND/slimMagneticFieldDataStructT

@param[in]  p_Handle   Handle to identify client.
@param[in]  pz_Request Data for magnetic field data configuration.
@param[in]  u_TxnId    A transaction ID for the sensor service request. This id
                       is provided to client when a response to this request is
                       sent.
@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
SLIM_API slimErrorEnumT slim_EnableMagneticFieldData
(
  slimClientHandleT p_Handle,
  const slimEnableMagneticFieldDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* #ifndef SLIM_API_H */
