#ifndef SLIM_PROVIDER_DATA_H
#define SLIM_PROVIDER_DATA_H
/**
@file
@brief Declaration of SLIM service provider interface.

This file declares types required for implementing SLIM service providers.

@sa slim_QSProvider slim_SSCProvider slim_PEProvider slim_QMILOCProvider
@ingroup slim_CoreProviderApi
*/
/*============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_provider_data.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_utils.h"
#include "slim_timesync.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreProviderApi
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*!
@brief SLIM Service Provider Interface.

SLIM service provider is an object with function table reference. The reference
may be the first member, but it is not required.

Implementations shall handle conversion of the interface into own structure
by computing function table offset from the interface pointer.

When function table is the first member inside provider's object, the address
of object matches interface address.

@sa slim_ServiceProviderInterfaceStruct
*/
typedef const struct slim_ServiceProviderInterfaceStruct *const
  slim_ServiceProviderInterfaceType;

/** Enum for provider setting. Preferred provider can be set from APSS using
    QMI_LOC. If preferred provider is native, SSC should not be used.
*/
typedef enum
{
  SLIM_PROVIDER_SETTING_NONE = -1,/**< Error value */
  SLIM_PROVIDER_SETTING_SSC  = 0, /**< Sensors data provider is Snapdragon Sensor
                                       Core (SSC). This is the default. */
  SLIM_PROVIDER_SETTING_NATIVE,   /**< Sensors data provider is on the host
                                       processor. */
  SLIM_PROVIDER_SETTING_COUNT,
  //! @cond
  SLIM_PROVIDER_SETTING_MAX = 2147483647 /* To force a 32 bit signed enum.
                                            Do not change or use */
  //! @endcond
} slim_ProviderSettingEnumType;

/*! Struct for time sync service monotonous data */
typedef struct slim_ServiceMtDataStructType
{
  uint64 t_LastSampleTime; /*!< Time of latest sample */
  /**
     @brief Monotonous offset.

     Offset changes at a rate of maximum of passed millisecond time.
     Can be used to convert from remote to local times so that monotonous
     counter in remote times is also monotonous in local times. */
  slim_TimeSyncMtOffsetStructType z_MtOffset;
  boolean b_OffsetReset; /*!< TRUE if offset has been reset */
} slim_ServiceMtDataStructType;

/* Function prototypes for sensor provider "interface" */
/**
@brief Returns TRUE if service is supported by the provider.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function returns TRUE if service is supported by the provider.

@param[in] pz_Interface Provider object
@param[in] e_Service    SLIM service

@return TRUE if service is supported. FALSE otherwise.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_CoreNotifyConfigurationChange
*/
typedef boolean slim_ServiceProvider_SupportsServiceFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  slimServiceEnumT e_Service
);

/**
@brief Opens provider connection for specific client.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function opens provider connection for a specific client.

@param[in] pz_Interface Provider object.
@param[in] p_Handle     Client handle.

@return eSLIM_SUCCESS if connection is opened successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_Open
*/
typedef slimErrorEnumT slim_ServiceProvider_OpenClientConnectionFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  slimClientHandleT p_Handle
);

/**
@brief Closes provider connection for specific client.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function closes service provider connection for a specific client and frees all
allocated resources.

@param[in] pz_Interface Provider object.
@param[in] p_Handle     Client handle.

@return eSLIM_SUCCESS if connection is closed successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_Close
*/
typedef slimErrorEnumT slim_ServiceProvider_CloseClientConnectionFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  slimClientHandleT p_Handle
);

/**
@brief Provides timesync mt offset for the SLIM service.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function provides timesync mt offset for the SLIM service.

@param[in] pz_Interface Provider object.
@param[in] p_Handle     Client handle.
@param[in] e_Service    SLIM service.
@param[out] pz_MtOffset Pointer to mt offset. Data is stored to this variable if
                        found.

@return TRUE if mt data was found. FALSE otherwise.

@sa slim_ServiceProviderInterfaceStructType
*/
typedef boolean slim_ServiceProvider_GetMtOffsetForServiceFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  slim_ServiceMtDataStructType *pz_MtOffset
);

/**
@brief Sets timesync mt offset for the SLIM service.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function sets timesync mt offset for the SLIM service.

@param[in] pz_Interface Provider object.
@param[in] p_Handle     Client handle.
@param[in] e_Service    SLIM service. If eSLIM_SERVICE_NONE, data is applied to
                        all services.
@param[in] pz_MtOffset  Pointer to mt offset to be set.
@param[in] b_SetForAllClients TRUE if data should be applied to all clients. In
                              this case parameter p_Handle is ignored. Otherwise
                              FALSE.

@sa slim_ServiceProviderInterfaceStructType
*/
typedef void slim_ServiceProvider_SetMtOffsetForServiceFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  slimClientHandleT p_Handle,
  slimServiceEnumT e_Service,
  const slim_ServiceMtDataStructType *pz_MtOffset,
  boolean b_ForAllClients
);

/**
@brief Handler for messages targeted for provider.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function handles messages sent via SLIM core to provider. The provider can
send messages to self using #slim_SendProviderIpcToTask call.

@param[in] pz_Interface     Provider object.
@param[in] q_MessageId      Message id.
@param[in] q_MsgPayloadSize Message size.
@param[in] ca_MsgPayload    Pointer to the message payload.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_SendProviderIpcToTask
*/
typedef void slim_ServiceProvider_HandleProviderMessageFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  uint32                             q_MessageId,
  uint32                             q_MsgPayloadSize,
  const void                        *ca_MsgPayload
);

/**
@brief Initiates time offset request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function for making the time request. Successful response enable SLIM to
calculate the offset between modem time and sensor time.

This function shall be implemented when provider time differs from SLIM core
time. By API contract, the method can be called at any time when provider
reports at least one service available.

The provider shall reply time response with #slim_CoreNotifyTimeResponse64 call.

@param[in] pz_Interface   Provider object.
@param[in] l_ServiceTxnId Service transaction id.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_CoreNotifyTimeResponse64
*/
typedef slimErrorEnumT slim_ServiceProvider_GetTimeRequestFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  int32 l_ServiceTxnId
);

/**
@brief Initiates sensor data request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates sensor data request to provider. If request is accepted for
processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableSensorData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableSensorDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableSensorDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates motion data request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates motion data request to provider. If request is accepted for
processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableMotionData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableMotionDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableMotionDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates pedometer request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates pedometer request to provider. If request is accepted for
processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnablePedometer
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnablePedometerFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnablePedometerTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates QMD data request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates QMD data request to provider. If request is accepted for
processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableQmdData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableQmdDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableQmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates SMD data request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates SMD data request to provider. If request is accepted for
processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableSmdData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableSmdDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableSmdDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates distance bound request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates distance bound request to provider. If request is accepted
for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableDistanceBound
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableDistanceBoundFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates distance bound set request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates distance bound update request to provider. If request is
accepted for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_SetDistanceBound
@sa slim_CoreNotifyResponseNoPayload
*/
typedef slimErrorEnumT slim_ServiceProvider_SetDistanceBoundFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_SetDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates distance bound get request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates distance bound query request to provider. If request is
accepted for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponse with given transaction identifier and operation status.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_GetDistanceBoundReport
@sa slim_CoreNotifyResponse
*/
typedef slimErrorEnumT slim_ServiceProvider_GetDistanceBoundFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_GetDistanceBoundTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates vehicle data request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates vehicle service request to provider. If request is accepted
for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableVehicleData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableVehicleDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableVehicleDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Initiates pedestrian alignment request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates pedestrian alignment service request to provider. If request
is accepted for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnablePedestrianAlignment
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnablePedestrianAlignmentFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnablePedestrianAlignmentTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/*!
@brief Initiates magnetic field request.

Service provider interface function (#slim_ServiceProviderInterfaceStructType).

Function initiates magnetic field service request to provider. If request is
accepted for processing, #eSLIM_SUCCESS shall be returned.

After the request is completed, the provider shall call
#slim_CoreNotifyResponseNoPayload with given transaction identifier and
operation status.

When service is enabled, provider shall dispatch data using
#slim_CoreDispatchData interface. It is strongly recommended that the data
dispatching is performed from a SLIM task context, so when a different thread
generates data, it shall be first routed to SLIM task context using
#slim_SendProviderIpcToTask method.

@param[in] pz_Interface   Provider interface.
@param[in] pz_Txn         Request payload.
@param[in] l_ServiceTxnId Request transaction identifier.

@return eSLIM_SUCCESS if request is made successfully.

@sa slim_ServiceProviderInterfaceStructType
@sa slim_EnableMagneticFieldData
@sa slim_CoreNotifyResponseNoPayload
@sa slim_CoreDispatchData
*/
typedef slimErrorEnumT slim_ServiceProvider_EnableMagneticFieldDataFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  const slim_EnableMagneticFieldDataTxnStructType *pz_Txn,
  int32 l_ServiceTxnId
);

/**
@brief Tells the service to cancel transaction.

This method is optional. If the provider doesn't track transaction status list,
the method can be omitted.

When SLIM core decides to terminate the transaction prior to completition, it
forwards the call to the provider.

If provider doesn't implement this method, the call result assumed to be
eSLIM_ERROR_UNSUPPORTED.

If the provider supports this method, it can confirm transaction abort or
cancel the operation.

@param[in] pz_Interface   Provider interface.
@param[in] l_ServiceTxnId Request transaction identifier.

@retval eSLIM_SUCCESS  The transaction has been aborted.
@retval eSLIM_ERROR... The transaction can't be aborted.

@sa slim_ServiceProviderInterfaceStructType
*/
typedef slimErrorEnumT slim_ServiceProvider_CancelTransactionRequestFnT
(
  slim_ServiceProviderInterfaceType *pz_Interface,
  int32                              l_ServiceTxnId
);

/*!
@brief Service provider interface function table.

Structure for service provider "interface" to be implemented by provider
modules.

@sa slim_ServiceProviderInterfaceType
*/
typedef struct slim_ServiceProviderInterfaceStruct
{
  boolean b_TimeCommonWithSlim;
  /**< If TRUE, provider has common time with SLIM and no time sync is
       needed.*/

  slim_ServiceProvider_SupportsServiceFnT *fn_SupportsService;
  /**< Returns TRUE, if provider supports the specified service. */

  slim_ServiceProvider_OpenClientConnectionFnT *fn_OpenClientConnection;
  /**< Called when new SLIM client opens a connection. */

  slim_ServiceProvider_CloseClientConnectionFnT *fn_CloseClientConnection;
  /**< Called when existing SLIM client closes a connection. */

  slim_ServiceProvider_GetMtOffsetForServiceFnT *fn_GetMtOffset;
  /**< Called when SLIM core needs a monotonous offset for the specified
       service. */

  slim_ServiceProvider_SetMtOffsetForServiceFnT *fn_SetMtOffset;
  /**< Called when SLIM core set a monotonous offset for the specified
       service. */

  slim_ServiceProvider_HandleProviderMessageFnT *fn_HandleMessage;
  /**< Called when SLIM core gets a message targeted for provider. */

  slim_ServiceProvider_GetTimeRequestFnT *fn_GetTimeRequest;
  /**< Get request for provider time. */

  slim_ServiceProvider_EnableSensorDataFnT *fn_EnableSensorDataRequest;
  /**< Enable request for device sensor data. */

  slim_ServiceProvider_EnableMotionDataFnT *fn_EnableMotionDataRequest;
  /**< Enable request for motion data. */

  slim_ServiceProvider_EnablePedometerFnT *fn_EnablePedometerRequest;
  /**< Enable request for pedometer data. */

  slim_ServiceProvider_EnableQmdDataFnT *fn_EnableQmdDataRequest;
  /**< Enable request for AMD/RMD data. */

  slim_ServiceProvider_EnableSmdDataFnT *fn_EnableSmdDataRequest;
  /**< Enable request for SMD data. */

  slim_ServiceProvider_EnableDistanceBoundFnT *fn_EnableDistanceBoundRequest;
  /**< Enable request for distance bound data. */

  slim_ServiceProvider_SetDistanceBoundFnT *fn_SetDistanceBoundRequest;
  /**< Set request for distance bound data. */

  slim_ServiceProvider_GetDistanceBoundFnT *fn_GetDistanceBoundRequest;
  /**< Get request for distance bound data. */

  slim_ServiceProvider_EnableVehicleDataFnT *fn_EnableVehicleDataRequest;
  /**< Enable request for vehicle data. */

  slim_ServiceProvider_EnablePedestrianAlignmentFnT *
    fn_EnablePedestrianAlignmentRequest;
  /**< Enable request for pedestrian alignment data. */

  slim_ServiceProvider_EnableMagneticFieldDataFnT *
    fn_EnableMagneticFieldDataRequest;
  /**< Enable request for magnetic field data. */

  slim_ServiceProvider_CancelTransactionRequestFnT *fn_CancelTransactionRequest;
  /**< Cancel transaction request */
} slim_ServiceProviderInterfaceStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** @brief Default implementation for transaction cancel: returns eSLIM_SUCCESS */
slim_ServiceProvider_CancelTransactionRequestFnT slim_ServiceProvider_CTRSuccess;
/** @brief Default implementation for transaction cancel: returns eSLIM_ERROR_UNSUPPORTED */
slim_ServiceProvider_CancelTransactionRequestFnT slim_ServiceProvider_CTRUnsupported;

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_PROVIDER_DATA_H */
