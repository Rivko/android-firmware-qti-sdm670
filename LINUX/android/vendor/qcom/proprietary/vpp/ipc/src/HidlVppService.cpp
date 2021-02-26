/*!
 * @file HidlVppService.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include "HidlVppService.h"
#include "HidlVpp.h"
#include "HidlVppUtils.h"

#define VPP_LOG_TAG     VPP_LOG_IPC_SERVICE_TAG
#define VPP_LOG_MODULE  VPP_LOG_IPC_SERVICE
#include "vpp_dbg.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

// Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppService follow.
Return<sp<IHidlVpp>> HidlVppService::getNewVppSession(uint32_t flags)
{
    // flags : for future use
    (void)flags;

    // Create and return the new session
    sp<IHidlVpp> hidlVppSession = new HidlVpp();
    return hidlVppSession;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
