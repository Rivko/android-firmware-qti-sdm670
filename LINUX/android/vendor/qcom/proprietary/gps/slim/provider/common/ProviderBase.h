/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
 * @file
 * @brief C++ declaration of base class for SLIM providers.
 *
 * This file defines the implementation of common methods shared
 * between sensor providers.
 *
 * @ingroup slim_ProviderCommon
 */
#ifndef __PROVIDER_BASE_H_INCLUDED__
#define __PROVIDER_BASE_H_INCLUDED__

#include <stdbool.h>
#include <slim_service_status.h>

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
 * @brief Base class for SLIM provider implementations
 *
 * The class encapsulates SLIM Core provider API into C++ interface.
 *
 * @ingroup slim_ProviderCommon
 */
class ProviderBase
{
private:
  /**
  @brief SLIM provider VTBL for providers with common time base.
  @internal
  */
  static const slim_ServiceProviderInterfaceStructType c_zVtblC;
  /**
  @brief SLIM provider VTBL for providers with own time base.
  @internal
  */
  static const slim_ServiceProviderInterfaceStructType c_zVtblU;
protected:
  enum
  {
      eMSG_ID_INVALID = 0,
      /**< Invalid provider message identifier */
      eMSG_ID_CONFIGURATION_CHANGE,
      /**< Service configuration change message identifier */
      eMSG_ID_BASE_COMMON_END,
  };
  /** @brief This provider type */
  const slim_ProviderEnumType m_eProvider;
  /** @brief Provider interface function table for SLIM API*/
  const slim_ServiceProviderInterfaceStructType * const m_pzVtbl;
  /** @brief Supported services mask */
  slimAvailableServiceMaskT m_qServiceMask;

private:
  /* SLIM provider API implementation */
  //! @name SLIM provider API implementation.
  //! @{
  /**
   * @brief Converts SLIM provider interface into object pointer
   *
   * @param[in] pzProvider Provider interface.
   *
   * @return Object pointer.
   */
  static ProviderBase *castSlimInterface
  (
    slim_ServiceProviderInterfaceType *pzProvider
  );
  /**
   * @brief Forwards call to instance method
   *
   * @param[in] pzProvider Provider interface.
   * @param[in] pHandle    SLIM client handle.
   *
   * @return Operation result
   */
  static slimErrorEnumT slimOpenClientConnection
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    slimClientHandleT pHandle
  );
  /**
   * @brief Forwards call to instance method
   *
   * @param[in] pzProvider Provider interface.
   * @param[in] pHandle    SLIM client handle.
   *
   * @return Operation result
   */
  static slimErrorEnumT slimCloseClientConnection
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    slimClientHandleT pHandle
  );
  /**
   * @brief Forwards call to instance method
   *
   * @param[in] pzProvider      Provider interface.
   * @param[in] qMessageId      Message identifier.
   * @param[in] qMsgPayloadSize Message payload size.
   * @param[in] pMsgPayload     Message payload.
   */
  static void slimHandleProviderMessage
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    uint32 qMessageId,
    uint32 qMsgPayloadSize,
    const void *pMsgPayload
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static slimErrorEnumT slimGetTimeUpdate
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    int32 lServiceTxnId
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static slimErrorEnumT slimEnableSensorDataRequest
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slim_EnableSensorDataTxnStructType *pzTxn,
    int32 lServiceTxnId
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static slimErrorEnumT slimEnablePedometerRequest
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slim_EnablePedometerTxnStructType *pzTxn,
    int32 lServiceTxnId
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static slimErrorEnumT slimEnableMotionDataRequest
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slim_EnableMotionDataTxnStructType *pzTxn,
    int32 lServiceTxnId
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static slimErrorEnumT slimEnableVehicleDataRequest
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slim_EnableVehicleDataTxnStructType *pzTxn,
    int32 lServiceTxnId
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static boolean slimSupportsService
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    slimServiceEnumT eService
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static boolean slimGetMtOffsetForService
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slimClientHandleT pHandle,
    slimServiceEnumT eService,
    slim_ServiceMtDataStructType *pzMtOffset
  );
  /**
  @brief Forwards call to instance method
  @return Operation result
  @internal
  */
  static void slimSetMtOffsetForService
  (
    slim_ServiceProviderInterfaceType *pzProvider,
    const slimClientHandleT pHandle,
    slimServiceEnumT eService,
    const slim_ServiceMtDataStructType *pzMtOffset,
    boolean bSetForAllClients
  );
  //! @}

protected:
  /**
  @brief Constructs provider

  Method initializes SLIM API function table and sets provider type and
  initial set of enabled services.

  @param[in] eProvider    Type of the provider.
  @param[in] uCommonTime  When @a true provider reports all data using SLIM
                          Core timer API.
  @param[in] eServiceMask Initial set of available services for this provider.
  */
  ProviderBase
  (
    slim_ProviderEnumType eProvider,
    bool uCommonTime,
    slimAvailableServiceMaskT eServiceMask
  );
  /**
  @brief Terminates provider operation.
  */
  virtual ~ProviderBase();

  /**
  @brief Opens provider connection for specific client.

  Function opens provider connection for a specific client.

  @param[in] pHandle Client handle.
  @return eSLIM_SUCCESS if connection is opened successfully.
  */
  virtual slimErrorEnumT openClientConnection
  (
    slimClientHandleT pHandle
  ) = 0;


  /**
  @brief Closes provider connection for specific client.

  Function closes sensor data provider connection for a specific client and
  frees all allocated resources.

  @param[in] pHandle Client handle.
  @return eSLIM_SUCCESS if connection is closed successfully.
  */
  virtual slimErrorEnumT closeClientConnection
  (
    slimClientHandleT pHandle
  ) = 0;

  /**
  @brief Handler for messages targeted for provider.

  Function handles messages sent via SLIM core to provider.

  @param[in] qMessageId      Message id.
  @param[in] qMsgPayloadSize Message size.
  @param[in] pMsgPayload     Pointer to the message payload.
  */
  virtual void handleProviderMessage
  (
    uint32_t qMessageId,
    uint32_t qMsgPayloadSize,
    const void *pMsgPayload
  );

  /**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in]  lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  ) = 0;

  /**
  @brief Initiates sensor data request.

  Function initiates sensor data request. If request is successful, sensor
  data streaming is either started or stopped.

  @param[in]  zTxn   Transaction data.
  @param[in]  lTxnId Service transaction id.
  @return eSLIM_SUCCESS if sensor data request is made successfully.
  */
  virtual slimErrorEnumT enableSensorDataRequest
  (
    const slim_EnableSensorDataTxnStructType &zTxn,
    int32_t lTxnId
  );
  /**
  @brief Initiates pedometer request.

  Function initiates pedometer request. If request is successful, pedometer
  data streaming is either started or stopped.

  @param[in]  zTxn   Transaction data.
  @param[in]  lTxnId Service transaction id.
  @return eSLIM_SUCCESS if sensor data request is made successfully.
  */
  virtual slimErrorEnumT enablePedometerRequest
  (
    const slim_EnablePedometerTxnStructType &zTxn,
    int32_t lTxnId
  );
  /**
  @brief Initiates motion data request.

  Function initiates motion request. If request is successful, motion data
  streaming is either started or stopped.

  @param[in]  zTxn   Transaction data.
  @param[in]  lTxnId Service transaction id.
  @return eSLIM_SUCCESS if sensor data request is made successfully.
  */
  virtual slimErrorEnumT enableMotionDataRequest
  (
    const slim_EnableMotionDataTxnStructType &zTxn,
    int32_t lTxnId
  );
  /**
  @brief Initiates vehicle data request.

  Function initiates vehicle sensor or odometry request. If request is
  successful, vehicle data streaming is either started or stopped.

  @param[in]  zTxn   Transaction data.
  @param[in]  lTxnId Service transaction id.
  @return eSLIM_SUCCESS if sensor data request is made successfully.
  */
  virtual slimErrorEnumT enableVehicleDataRequest
  (
    const slim_EnableVehicleDataTxnStructType &zTxn,
    int32_t lTxnId
  );

  /**
  @brief Tests if service is supported by the provider.

  Function returns @a true if service is supported by the provider.

  @param[in] eService   SLIM service
  @return true if service is supported. false otherwise.
  */
  virtual bool supportsService
  (
    slimServiceEnumT eService
  );

  /**
  @brief Provides timesync mt offset for the SLIM service.

  Function provides timesync mt offset for the SLIM service.

  @param[in]  pHandle      Pointer to the client handle
  @param[in]  eService     SLIM service.
  @param[out] zMtOffsetOut MT offset reference. Data is stored to this variable
                           if found.
  @return true if mt data was found. false otherwise.
  */
  virtual bool getMtOffsetForService
  (
    slimClientHandleT pHandle,
    slimServiceEnumT eService,
    slim_ServiceMtDataStructType &zMtOffsetOut
  ) = 0;

  /**
  @brief Sets timesync mt offset for the SLIM service.

  Function sets timesync mt offset for the SLIM service.

  @param[in] pHandle     Pointer to the client handle. If NULL, data is applied
                         to all clients.
  @param[in] eService    SLIM service. If eSLIM_SERVICE_NONE, data is applied
                          to all services.
  @param[in] zMtOffset   MT offset to be set.
  @param[in] bSetForAllClients true if data should be applied to all clients.
                               In this case parameter pHandle is ignored.
                               Otherwise false.
  */
  virtual void setMtOffsetForService
  (
    slimClientHandleT pHandle,
    slimServiceEnumT eService,
    const slim_ServiceMtDataStructType &zMtOffset,
    bool bSetForAllClients
  ) = 0;

  /**
  @brief Route function for provider messages

  Provider modules should use this function to route messages to self through
  SLIM-AP daemon task.

  @param[in] qMessageId Provider-specific message identifier.
  @param[in] qSize      Data size.
  @param[in] pData      Data to route (can be 0).
  @retval Operation status; eSLIM_SUCCESS if routed.
  */
  slimErrorEnumT routeMessageToSelf
  (
    uint32_t qMessageId,
    uint32_t qSize,
    const void *pData
  );

  /**
  @brief Forwards configuration change event to SLIM core.

  @param[in] qChangedServiceMask Modified services.
  @sa routeConfigurationChange
  */
  void dispatchConfigurationChange
  (
    slimAvailableServiceMaskT qChangedServiceMask
  );

  /**
  @brief Sends a configuration change message to self.

  Provider modules should use this function to route messages to self through
  SLIM-AP daemon task.

  @param[in] qChangedServiceMask Modified services.

  @return Operation result.
  @retval eSLIM_SUCCESS The message has been successfully routed.

  @sa #dispatchConfigurationChange
  */
  slimErrorEnumT routeConfigurationChange
  (
    slimAvailableServiceMaskT qChangedServiceMask
  );

  /**
   * @brief Dispatch function for time response.
   *
   * AP provider modules should use this function to dispatch time responses
   * to SLIM core.
   *
   * @param[in] lTxId           Transaction number.
   * @param[in] tRemoteRxTimeMs Remote provider request receive time in ms.
   * @param[in] tRemoteTxTimeMs Remote provider response dispatch time in ms.
   */
  void dispatchTimeResponse
  (
    int32_t lTxId,
    uint64_t tRemoteRxTimeMs,
    uint64_t tRemoteTxTimeMs
  );

  /**
   * @brief Dispatch function for error time response.
   *
   * AP provider modules should use this function to dispatch time responses
   * to SLIM core.
   *
   * @param[in] lTxId           Transaction number.
   * @param[in] eError          Error code.
   */
  void dispatchTimeResponse
  (
    int32_t lTxId,
    slimErrorEnumT eError
  );
public:
  /**
  @brief Creates new provider object instance.

  Instantiates and initializes provider object and returns SLIM provider
  interface or 0 on error.

  @return SLIM provider interface or 0 on error.
  */
  slim_ServiceProviderInterfaceType *getSlimInterface() const;
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

/**
@brief Creates new provider object instance.

Instantiates and initializes provider object and returns SLIM provider
interface or 0 on error.

@return SLIM provider interface or 0 on error.
*/
inline slim_ServiceProviderInterfaceType *slim::ProviderBase::getSlimInterface() const
{
  return &m_pzVtbl;
};

//! @}

#endif //__PROVIDER_BASE_H_INCLUDED__
