/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VENDOR_QTI_HARDWARE_FM_V1_0_FMHCI_H
#define VENDOR_QTI_HARDWARE_FM_V1_0_FMHCI_H

#include <vendor/qti/hardware/fm/1.0/IFmHci.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "controller.h"
#include "data_handler.h"
#include <utils/Log.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace fm {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using vendor::qti::hardware::fm::V1_0::IFmHci;
using vendor::qti::hardware::fm::V1_0::IFmHciCallbacks;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::sp;
using ::android::wp;

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

class FmDeathRecipient;

struct FmHci : public IFmHci {
    // Methods from ::vendor::qti::hardware::fm::V1_0::IFmHci follow.
    Return<void> initialize(const sp<IFmHciCallbacks>& callback) override;
    Return<void> sendHciCommand(const hidl_vec<uint8_t>& command) override;
    Return<void> close() override;
    FmHci();

    private:
        void sendDataToController(HciPacketType type, const hidl_vec<uint8_t>& data);
        ::android::sp<IFmHciCallbacks> event_cb_;
        ::android::sp<FmDeathRecipient> death_recipient_;
        std::function<void(sp<FmDeathRecipient>&)> unlink_cb_;
    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

extern "C" IFmHci* HIDL_FETCH_IFmHci(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace fm
}  // namespace hardware
}  //namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_FM_V1_0_FMHCI_H