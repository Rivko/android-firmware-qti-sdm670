/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/* This file contains dummy provider config functions to avoid symbol resolution
 * issue with DEP feature.
 */

#include "slim_provider_data.h"

extern "C"
{

SLIM_API slim_ServiceProviderInterfaceType *slim_ProviderConfInitProvider
(
    slim_ProviderEnumType /*e_Provider*/
)
{
    SLIM_LOGD("Dummy slim_ProviderConfInitProvider, it shouldn't come here");
    return (slim_ServiceProviderInterfaceType *) NULL;
}

SLIM_API slim_ProviderSettingEnumType slim_ProviderConfGetInitialProviderSetting(void)
{
    SLIM_LOGD("Dummy slim_ProviderConfGetInitialProviderSetting, it shouldn't come here");
    return SLIM_PROVIDER_SETTING_NONE;
}

SLIM_API slim_ProviderEnumType slim_ProviderConfGetProviderForClientService
(
    uint32 /*q_AvailableProviders*/,
    slimServiceProviderMaskT /*q_AllowedProviders*/,
    slim_ProviderSettingEnumType /*e_ProviderSetting*/,
    boolean /*b_AllowRemoteSlimProviders*/
)
{
    SLIM_LOGD("Dummy slim_ProviderConfGetProviderForClientService, it shouldn't come here");
    return SLIM_PROVIDER_NONE;
}

SLIM_API slimServiceProviderMaskT slim_ProviderConfGetClientProviderMask
(
    uint32 /*q_AvailableProviders*/,
    slim_ProviderSettingEnumType /*e_ProviderSetting*/,
    boolean /*b_AllowRemoteSlimProviders*/
)
{
    SLIM_LOGD("Dummy slim_ProviderConfGetClientProviderMask, it shouldn't come here");
    return eSLIM_SENSOR_TYPE_NONE;
}

SLIM_API slimServiceProviderEnumT slim_ProviderConfMapSlimProvider(slim_ProviderEnumType /*e_SlimProvider*/)
{
    SLIM_LOGD("Dummy slim_ProviderConfMapSlimProvider, it shouldn't come here");
    return eSLIM_SERVICE_PROVIDER_DEFAULT;
}

SLIM_API uint32 slim_ProviderConfApplyProviderSetting
(
    uint32 /*q_AvailableProviders*/,
    slim_ProviderSettingEnumType /*e_ProviderSetting*/
)
{
    SLIM_LOGD("Dummy slim_ProviderConfApplyProviderSetting, it shouldn't come here");
    return 0;
}

SLIM_API boolean slim_ProviderConfDisableProviders(uint32 /*q_DisabledProviders*/)
{
    SLIM_LOGD("Dummy slim_ProviderConfDisableProviders, it shouldn't come here");
    return false;
}
} /* extern C */
