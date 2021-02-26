#ifndef SLIM_INTERNAL_API_H
#define SLIM_INTERNAL_API_H
/**
  @file
  @brief SLIM internal API header file

  SLIM internal API is an equivalent of SLIM public API for use by internal
  SLIM clients.

  @ingroup slim_CoreClientApi
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_internal_api.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "slim_internal_client_types.h"
#include "slim_processor.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
@addtogroup slim_CoreClientApi
@{
@name Public Functions
@{
*/

/*----------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* SLIM internal API.
* SLIM internal API uses in-process messaging for communicating with SLIM
* Core.
* -------------------------------------------------------------------------*/

/**
@brief Internal method for registering SLIM client with the SLIM service.

This function is a functional equivalent for @ref slim_Open for use with
internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in]  q_ClientId     Client identifier. Shall have client id and client
                           instance.
@param[in]  fn_Callback    Callback function.
@param[in]  q_OpenFlags    Open flags.
@param[in]  t_CallbackData Client cookie for callback function.
@param[out] pz_ContextOut  Client context constructed from input parameters.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_Open
*/
slimErrorEnumT slim_InternalOpen
(
  uint32_t q_ClientId,
  slimNotifyCallbackFunctionT fn_Callback,
  slimOpenFlagsT q_OpenFlags,
  uint64_t t_CallbackData,
  slimClientContextT *pz_ContextOut
);

/**
@brief Internal method for deregistering SLIM client handle.

This function is a functional equivalent for @ref slim_Close for use with
internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_Close
*/
slimErrorEnumT slim_InternalClose
(
  slimClientContextT *pz_Context
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
bool slim_InternalServiceAvailable
(
  const slimServiceStatusEventStructT *pz_Status,
  slimServiceEnumT e_Service
);

/**
@brief Enabling or disabling of sensor data streaming.


This function is a functional equivalent for @ref slim_EnableSensorData for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableSensorData
*/
slimErrorEnumT slim_InternalEnableSensorData
(
  slimClientContextT *pz_Context,
  const slimEnableSensorDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of motion data streaming.


This function is a functional equivalent for @ref slim_EnableMotionData for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableMotionData
*/
slimErrorEnumT slim_InternalEnableMotionData
(
  slimClientContextT *pz_Context,
  const slimEnableMotionDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of pedometer reporting.

This function is a functional equivalent for @ref slim_EnablePedometer for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnablePedometer
*/
slimErrorEnumT slim_InternalEnablePedometer
(
  slimClientContextT *pz_Context,
  const slimEnablePedometerRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of QMD data streaming.

This function is a functional equivalent for @ref slim_EnableQmdData for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableQmdData
*/
slimErrorEnumT slim_InternalEnableQmdData
(
  slimClientContextT *pz_Context,
  const slimEnableQmdDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of SMD data streaming.

This function is a functional equivalent for @ref slim_EnableSmdData for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableSmdData
*/
slimErrorEnumT slim_InternalEnableSmdData
(
  slimClientContextT *pz_Context,
  const slimEnableSmdDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling Distance Bound service.

This function is a functional equivalent for @ref slim_EnableDistanceBound for
use with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableDistanceBound
*/
slimErrorEnumT slim_InternalEnableDistanceBound
(
  slimClientContextT *pz_Context,
  const slimEnableDistanceBoundRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Set request for Distance Bound service.

This function is a functional equivalent for @ref slim_SetDistanceBound for use
with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_SetDistanceBound
*/
slimErrorEnumT slim_InternalSetDistanceBound
(
  slimClientContextT *pz_Context,
  const slimSetDistanceBoundRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Get request for Distance Bound service.

This function is a functional equivalent for @ref slim_GetDistanceBoundReport
for use with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_GetDistanceBoundReport
*/
slimErrorEnumT slim_InternalGetDistanceBoundReport
(
  slimClientContextT *pz_Context,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of vehicle data streaming.

This function is a functional equivalent for @ref slim_EnableVehicleData for
use with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableVehicleData
*/
slimErrorEnumT slim_InternalEnableVehicleData
(
  slimClientContextT *pz_Context,
  const slimEnableVehicleDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of pedestrian alignment data streaming.

This function is a functional equivalent for @ref slim_EnablePedestrianAlignment
for use with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnablePedestrianAlignment
*/
slimErrorEnumT slim_InternalEnablePedestrianAlignment
(
  slimClientContextT *pz_Context,
  const slimEnablePedestrianAlignmentRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Enabling or disabling of magnetic field data streaming.

This function is a functional equivalent for @ref slim_EnableMagneticFieldData
for use with internal SLIM components like QMI-SLIM and QMI-LOC (on AP).

@param[in] pz_Context  Client context.
@param[in] pz_Request  Request parameters.
@param[in] u_TxnId     Transaction number.

@return eSLIM_SUCCESS is operation is successful, error code otherwise.

@sa slim_EnableMagneticFieldData
*/
slimErrorEnumT slim_InternalEnableMagneticFieldData
(
  slimClientContextT *pz_Context,
  const slimEnableMagneticFieldDataRequestStructT *pz_Request,
  uint8_t u_TxnId
);

/**
@brief Request provider configuration change.

This request enqueues request for provider configuration update. It is similar
to slim_CoreNotifyConfigurationChange but uses asynchronous message dispatching
instead of direct call.

@param[in] q_ProviderMask Providers mask.
@param[in] q_ServiceMask  Changed services.

@return eSLIM_SUCCESS if request was sent successfully. Otherwise SLIM error code.
*/
slimErrorEnumT slim_InternalRequestConfigurationChange
(
  slimServiceProviderMaskT q_ProviderMask,
  slimAvailableServiceMaskT q_ServiceMask
);

/**
@}
@}
*/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_INTERNAL_API_H */
