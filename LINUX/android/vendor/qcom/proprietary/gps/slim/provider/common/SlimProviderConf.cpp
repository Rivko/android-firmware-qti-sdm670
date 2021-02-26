/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

/**
@file
@brief SLIM processor-specific provider configuration implementation.

@ingroup slim_APSS
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <slim_provider_conf.h>
#include <SlimProviderConf.h>
#ifdef FEATURE_LOCTECH_SLIM_DSPS
#include <SlimSensor1Provider.h>
#endif
#ifdef FEATURE_LOCTECH_SLIM_NDK
#include <SlimNDKProvider.h>
#endif
#ifdef FEATURE_LOCTECH_SLIM_VNW
#include <VehicleNetworkProvider.h>
#endif
#ifdef FEATURE_LOCTECH_QMISLIM_PROVIDER
#include <qmislim_task_api.h>
#endif
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL
#include <SlimSensorHalProvider.h>
#endif
#ifdef FEATURE_LOCTECH_SLIM_VNS
#include <VNSProvider.h>
#endif

#ifdef FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
#include <qmislim_task_api.h>
#endif
//! @addtogroup slim_APSS
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*!
 * @brief APSS-specific provider configuration data
 *
 * This structure is used for controlling provider configuration
 */
typedef struct
{
  //! @brief Default provider setting.
  slim_ProviderSettingEnumType e_Setting;
  //! @brief Enabled providers mask.
  uint32_t                     q_EnabledMask;
} slim_ProviderConfDataT;
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
//! @brief Provider configuration data
static slim_ProviderConfDataT z_ProviderConfData =
    { SLIM_PROVIDER_SETTING_NONE, 0 };

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
@brief Sets SLIM provider configuration parameters

@param[in] e_Setting     Configured provider setting.
@param[in] q_EnabledMask Mask with @ref slim_ProviderEnumType provider fields.
*/
void slim_ProviderConfSetConfig
(
  slim_ProviderSettingEnumType e_Setting,
  uint32_t q_EnabledMask
)
{
  slim_ProviderConfDataT *pz_ConfData = &z_ProviderConfData;
  pz_ConfData->e_Setting = e_Setting;
  pz_ConfData->q_EnabledMask = q_EnabledMask;
}

/**
@brief Initializes provider of given type and returns pointer to the provider.

Function initializes provider of given type and returns pointer to the provider.
If provider is not supported, NULL is returned.
Implementation of this function is processor specific.

@param e_Provider: Pointer to the provider registry.
@return Pointer to the initialized provider. NULL if provider is not supported.
*/
slim_ServiceProviderInterfaceType *slim_ProviderConfInitProvider
(
    slim_ProviderEnumType e_Provider
)
{
  const slim_ProviderConfDataT *pz_ConfData = &z_ProviderConfData;
  slim_ServiceProviderInterfaceType *pz_Provider = NULL;
  switch (e_Provider)
  {
  case SLIM_PROVIDER_SENSOR1:
    /* Initialize sensor1 only when it is configured */
#ifdef FEATURE_LOCTECH_SLIM_DSPS
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_SENSOR1))
    {
      SLIM_LOGD("Creating Sensor1 provider");
      pz_Provider = slim::Sensor1Provider::getServiceProvider();
    }
#endif
    break;
  case SLIM_PROVIDER_NDK:
#ifdef FEATURE_LOCTECH_SLIM_NDK
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_NDK))
    {
      SLIM_LOGD("Creating NDK provider");
      android::sp<slim::NDKProvider> pzNDKProvider =
          slim::NDKProvider::getInstance();
      if (0 != pzNDKProvider.get())
      {
        pz_Provider = pzNDKProvider->getSlimInterface();
      }
    }
#endif
    break;
  case SLIM_PROVIDER_VNW:
#ifdef FEATURE_LOCTECH_SLIM_VNW
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_VNW))
    {
      SLIM_LOGD("Creating VNW provider");
      pz_Provider = slim::VehicleNetworkProvider::getProvider();
    }
#endif
    break;
  case SLIM_PROVIDER_SENSOR_HAL:
#ifdef FEATURE_LOCTECH_SLIM_SENSOR_HAL
    SLIM_LOGD("Initializing Sensor HAL provider");
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_SENSOR_HAL))
    {
      SLIM_LOGD("Creating Sensor HAL provider");
      pz_Provider = slim::SensorHalProvider::getProvider();
    }
#endif
    break;
  case SLIM_PROVIDER_VNS:
#ifdef FEATURE_LOCTECH_SLIM_VNS
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_VNS))
    {
      SLIM_LOGD("Creating VNS provider");
      pz_Provider = slim::VNSProvider::getProvider();
    }
#endif
    break;
#ifdef FEATURE_LOCTECH_QMISLIM_PROVIDER_MP
  case SLIM_PROVIDER_QMISLIM_MPSS:
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_QMISLIM_MPSS))
    {
      SLIM_LOGD("Creating MP provider");
      pz_Provider = slim_QSProviderTask_NewProvider(
                       &slimz_QSProviderTask,
                       eSLIM_QS_NODE_MP);
    }
    break;
#endif /* FEATURE_LOCTECH_QMISLIM_PROVIDER_AP */
#ifdef FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
  case SLIM_PROVIDER_QMISLIM_TEST:
    if (SLIM_MASK_IS_SET(pz_ConfData->q_EnabledMask, SLIM_PROVIDER_QMISLIM_TEST))
    {
      SLIM_LOGD("Creating Test provider");
      pz_Provider = slim_QSProviderTask_NewProvider(
                       &slimz_QSProviderTask,
                       eSLIM_QS_NODE_TEST);
    }
    break;
#endif /* FEATURE_LOCTECH_QMISLIM_PROVIDER_AP */

    default:
        break;
    }
    return pz_Provider;
}

/**
@brief Provides initial provider setting for preferred provider.

Function provides initial provider setting for preferred provider.

@return Preferred provider setting
*/
slim_ProviderSettingEnumType slim_ProviderConfGetInitialProviderSetting
(
  void
)
{
  const slim_ProviderConfDataT *pz_ConfData = &z_ProviderConfData;
  return pz_ConfData->e_Setting;
}

/**
@brief Returns available provider for the service requested by client.

Function returns available provider for the service requested by client.
Implementation of this function is processor specific.

@param q_AvailableProviders       Available service providers.
@param q_AllowedProviders         Providers allowed by client.
@param e_ProviderSetting          Preferred provider setting.
@param b_AllowRemoteSlimProviders TRUE if client allows usage of remote
                                  SLIM providers.
@return Provider type
*/
slim_ProviderEnumType slim_ProviderConfGetProviderForClientService
(
  uint32 q_AvailableProviders,
  slimServiceProviderMaskT q_AllowedProviders,
  slim_ProviderSettingEnumType e_ProviderSetting,
  boolean b_AllowRemoteSlimProviders
)
{
    slim_ProviderEnumType e_Provider = SLIM_PROVIDER_NONE;

    SLIM_UNUSED(e_ProviderSetting);

    if (b_AllowRemoteSlimProviders &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_QMISLIM_MPSS) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_SAMLITE))
    {
      e_Provider = SLIM_PROVIDER_QMISLIM_MPSS;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_NDK) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_NATIVE))
    {
      e_Provider = SLIM_PROVIDER_NDK;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_SENSOR_HAL) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_NATIVE))
    {
      e_Provider = SLIM_PROVIDER_SENSOR_HAL;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_VNW) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_NATIVE))
    {
      e_Provider = SLIM_PROVIDER_VNW;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_VNS) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_NATIVE))
    {
      e_Provider = SLIM_PROVIDER_VNS;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        b_AllowRemoteSlimProviders &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_SENSOR1) &&
        SLIM_MASK_IS_SET(q_AllowedProviders, eSLIM_SERVICE_PROVIDER_SSC))
    {
      e_Provider = SLIM_PROVIDER_SENSOR1;
    }

    if (SLIM_PROVIDER_NONE == e_Provider &&
        SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_QMISLIM_TEST))
    {
      e_Provider = SLIM_PROVIDER_QMISLIM_TEST;
    }

    return e_Provider;
}

/**
@brief Returns mask of available providers for client.

Function returns mask of available providers for client.
Implementation of this function is processor specific.

@param q_AvailableProviders        Available service providers.
@param e_ProviderSetting           Preferred provider setting.
@param b_AllowRemoteSlimProviders  TRUE if client allows usage of remote
                                   SLIM providers.
@return Mask of type slimServiceProviderMaskT of available providers to be sent
        to SLIM client.
*/
slimServiceProviderMaskT slim_ProviderConfGetClientProviderMask
(
  uint32 q_AvailableProviders,
  slim_ProviderSettingEnumType e_ProviderSetting,
  boolean b_AllowRemoteSlimProviders
)
{
  slimServiceProviderMaskT q_ClientMask = 0;

  SLIM_UNUSED(e_ProviderSetting);

  if (b_AllowRemoteSlimProviders &&
      SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_QMISLIM_MPSS))
  {
    SLIM_MASK_SET(q_ClientMask, eSLIM_SERVICE_PROVIDER_SAMLITE);
  }
  if (b_AllowRemoteSlimProviders &&
      SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_SENSOR1))
  {
    SLIM_MASK_SET(q_ClientMask, eSLIM_SERVICE_PROVIDER_SSC);
  }
  if (SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_NDK) ||
      SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_VNW) ||
      SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_SENSOR_HAL) ||
      SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_VNS))
  {
    SLIM_MASK_SET(q_ClientMask, eSLIM_SERVICE_PROVIDER_NATIVE);
  }

  /* Special case for testing: if testing is enabled, test provider
   * can act as both native and remote */
#ifdef USE_GLIB
  if (SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_QMISLIM_TEST))
  {
    SLIM_MASK_SET(q_ClientMask, eSLIM_SERVICE_PROVIDER_NATIVE);
  }
#else
  if (SLIM_MASK_IS_SET(q_AvailableProviders, SLIM_PROVIDER_QMISLIM_TEST))
  {
    SLIM_MASK_SET(q_ClientMask, eSLIM_SERVICE_PROVIDER_DEFAULT);
  }

#endif

  return q_ClientMask;
}

/**
@brief Maps SLIM provider type to client service provider type.

Function maps SLIM provider type to client service provider type.
Implementation of this function is processor specific.

@param e_SlimProvider: SLIM provider type.
@return Client provider type.
*/
slimServiceProviderEnumT slim_ProviderConfMapSlimProvider
(
  slim_ProviderEnumType e_SlimProvider
)
{
  switch (e_SlimProvider)
  {
  case SLIM_PROVIDER_SENSOR1:
    return eSLIM_SERVICE_PROVIDER_SSC;
  case SLIM_PROVIDER_QMISLIM_MPSS:
    return eSLIM_SERVICE_PROVIDER_SAMLITE;
  case SLIM_PROVIDER_NDK:
  case SLIM_PROVIDER_VNW:
  case SLIM_PROVIDER_SENSOR_HAL:
  case SLIM_PROVIDER_VNS:
    return eSLIM_SERVICE_PROVIDER_NATIVE;
  case SLIM_PROVIDER_QMISLIM_TEST:
    return eSLIM_SERVICE_PROVIDER_DEFAULT;
  default:
    break;
  }
  return eSLIM_SERVICE_PROVIDER_DEFAULT;
}

/**
@brief Applies provider setting to the given SLIM provider mask.

Function applies provider setting to the given SLIM provider mask.
Implementation of this function is processor specific.

@param q_AvailableProviders  Available service providers.
@param e_ProviderSetting     Preferred provider setting.
@return Mask of SLIM providers.
*/
uint32 slim_ProviderConfApplyProviderSetting
(
  uint32 q_AvailableProviders,
  slim_ProviderSettingEnumType e_ProviderSetting
)
{
  SLIM_UNUSED(e_ProviderSetting);

  return q_AvailableProviders;
}

//! @}
