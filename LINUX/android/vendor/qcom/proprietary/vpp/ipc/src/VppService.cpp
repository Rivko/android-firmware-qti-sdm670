/*!
 * @file VppService.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <vendor/qti/hardware/vpp/1.1/IHidlVppService.h>
#include <hidl/LegacySupport.h>

#define VPP_LOG_TAG     VPP_LOG_IPC_SERVICE_TAG
#define VPP_LOG_MODULE  VPP_LOG_IPC_SERVICE
#include "vpp_dbg.h"
#include "vpp.h"
#include "HidlVppService.h"

using vendor::qti::hardware::vpp::V1_1::IHidlVppService;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

int main(int /* argc */, char**)
{
    uint32_t u32;
    configureRpcThreadpool(10, true /* callerWillJoin */);

    // Setup hwbinder service
    android::sp<IHidlVppService> service = new vendor::qti::hardware::vpp::V1_1::implementation::HidlVppService();
    if (service == NULL)
    {
        LOGE("HidlVppService is null");
        return 0;
    }

    u32 = service->registerAsService("vppService");
    if (u32 != 0)
    {
        LOGE("failed to register VppService, u32=%u", u32);
        return VPP_ERR;
    }

    LOGE("Starting the vppService....");

    u32 = vpp_boot();
    LOGE_IF(u32 != VPP_OK, "vpp_boot failed, u32=%u", u32);

    joinRpcThreadpool();

    if (u32 == VPP_OK)
    {
        u32 = vpp_shutdown();
        LOGE_IF(u32 != VPP_OK, "vpp_shutdown failed, u32=%u", u32);
    }

    return 0;
}
