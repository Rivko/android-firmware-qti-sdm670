/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef COM_QUALCOMM_QTI_ANT_V1_0_ANTHCI_H
#define COM_QUALCOMM_QTI_ANT_V1_0_ANTHCI_H

#include <com/qualcomm/qti/ant/1.0/IAntHci.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "controller.h"
#include "data_handler.h"
#include <utils/Log.h>


namespace com {
namespace qualcomm {
namespace qti {
namespace ant {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::com::qualcomm::qti::ant::V1_0::IAntHci;
using ::com::qualcomm::qti::ant::V1_0::IAntHciCallbacks;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;

using android::hardware::hidl_death_recipient;

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

class AntDeathRecipient;

struct AntHci : public IAntHci {
	public:
	AntHci();
    // Methods from ::vendor::qcom::ant::V1_0::IAntHci follow.
    Return<void> initialize(const sp<IAntHciCallbacks>& callback) override;
    Return<void> sendAntControl(const hidl_vec<uint8_t>& command) override;
    Return<void> sendAntData(const hidl_vec<uint8_t>& command) override;
    Return<void> close() override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
 private:
  void sendDataToController(HciPacketType type, const hidl_vec<uint8_t>& data);
  ::android::sp<IAntHciCallbacks> event_cb_;
  ::android::sp<AntDeathRecipient> deathRecipient;
  std::function<void(sp<AntDeathRecipient>&)> unlink_cb_;
 };

    // Methods from ::android::hidl::base::V1_0::IBase follow.
 extern "C" IAntHci* HIDL_FETCH_IAntHci(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace ant
}  // namespace qti
}  // namespace qualcomm
}  // namespace com

#endif  // COM_QUALCOMM_QTI_ANT_V1_0_ANTHCI_H
