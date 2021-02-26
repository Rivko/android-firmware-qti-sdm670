/*!
 * @file HidlVpp.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#ifndef VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPP_H
#define VENDOR_QTI_HARDWARE_VPP_V1_1_HIDLVPP_H

#include <vendor/qti/hardware/vpp/1.1/IHidlVpp.h>
#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <list>
#include <utils/Mutex.h>

#include "vpp.h"
#include "vpp_stats.h"
#include "vpp_def.h"

#define VPP_BUF_POOL_MAXSIZE 128

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::vpp::V1_1::HqvControl;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVpp;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks;
using ::vendor::qti::hardware::vpp::V1_1::VideoProperty;
using ::vendor::qti::hardware::vpp::V1_1::VppBuffer;
using ::vendor::qti::hardware::vpp::V1_1::VppPort;
using ::vendor::qti::hardware::vpp::V1_1::VppPortParam;
using ::vendor::qti::hardware::vpp::V1_1::VppRequirements;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;
using ::android::Mutex;

using namespace android;

struct HidlVpp : public IHidlVpp {
     HidlVpp();
    ~HidlVpp();
    // Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVpp follow.
    Return<uint32_t> vppInit(uint32_t flags, const ::android::sp<IHidlVppCallbacks>& cb) override;
    Return<void> vppTerm() override;
    Return<void> vppSetCtrl(uint64_t cookie, const HqvControl& ctrl, vppSetCtrl_cb _hidl_cb) override;
    Return<uint32_t> vppSetParameter(VppPort port, const VppPortParam& param) override;
    Return<uint32_t> vppQueueBuf(VppPort port, const VppBuffer& buf) override;
    Return<uint32_t> vppReconfigure(const VppPortParam& inputParam, const VppPortParam& outputParam) override;
    Return<uint32_t> vppFlush(VppPort port) override;
    Return<uint32_t> vppSetVidProp(const VideoProperty& prop) override;

    static uint32_t vppBufferDone(void *pv, struct vpp_buffer *buf, enum vpp_port ePort);
    static void vppIBD(void *pv, struct vpp_buffer *buf);
    static void vppOBD(void *pv, struct vpp_buffer *buf);
    static void vppEVT(void *pv, struct vpp_event e);

    class HidlVppDeathRecipient : public hidl_death_recipient {
    public:
        HidlVppDeathRecipient(HidlVpp *hidlVpp) : mHidlVpp(hidlVpp) { }
        virtual void serviceDied(uint64_t cookie, const wp<IBase>& who);
    private:
        HidlVpp *mHidlVpp;
    };

    sp<IHidlVppCallbacks> mHidlCb;
    void* pvCtx;
    struct vpp_buffer mVppBuffers[VPP_BUF_POOL_MAXSIZE];
    std::list<struct vpp_buffer *> mBufPool;

    Mutex mMutex;
    // Context for holding the vpp client's stats
    t_StVppCtx mStCtx;
    void *mStatHandle;

    bool mClientDied;
    Mutex mDeathMutex;
    sp<HidlVppDeathRecipient> mClientDeathNotification;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // COM_QUALCOMM_QTI_VPP_V1_0_HIDLVPP_H
