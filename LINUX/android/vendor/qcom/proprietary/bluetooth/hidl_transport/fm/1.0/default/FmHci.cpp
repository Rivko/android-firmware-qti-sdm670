/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "FmHci.h"

#define LOG_TAG "vendor.qti.hardware.fm@1.0-FmHci"

namespace vendor {
namespace qti {
namespace hardware {
namespace fm {
namespace V1_0 {
namespace implementation {


class FmDeathRecipient : public hidl_death_recipient {
    public:
        FmDeathRecipient(const sp<IFmHci> hci) : mHci(hci) {}

        virtual void serviceDied(
            uint64_t /*cookie*/,
            const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
                ALOGE("FmDeathRecipient::serviceDied - Fm service died");
                has_died_ = true;
                mHci->close();
            }
        sp<IFmHci> mHci;
        bool getHasDied() const { return has_died_; }
        void setHasDied(bool has_died) { has_died_ = has_died; }

    private:
        bool has_died_;
};

FmHci::FmHci()
    : death_recipient_(new FmDeathRecipient(this)) {}


// Methods from ::vendor::qti::hardware::fm::V1_0::IFmHci follow.
Return<void> FmHci::initialize(const sp<IFmHciCallbacks>& cb) {
    ALOGI("initialize");
    if (cb == nullptr) {
        ALOGE("cb == nullptr! -> Unable to call initializationComplete(ERR)");
        return Void();
    }

    event_cb_ = cb;

    death_recipient_->setHasDied(false);
    event_cb_->linkToDeath(death_recipient_, 0);

    bool rc = DataHandler::Init( TYPE_FM,
        [this](bool status) {
            auto hidl_status = event_cb_->initializationComplete(
                status ? Status::SUCCESS : Status::INITIALIZATION_ERROR);
            if (!hidl_status.isOk()) {
                ALOGE("Unable to call initializationComplete()");
            }
        },
        [this](HciPacketType type, const hidl_vec<uint8_t> *packet) {
            switch (type) {
                case HCI_PACKET_TYPE_FM_EVENT:
                {
                    auto hidl_status = event_cb_->hciEventReceived(*packet);
                    if (!hidl_status.isOk()) {
                        ALOGE(" Unable to call hciEventReceived()");
                    }
                    break;
                }
                default:
                {
                    ALOGE("%s Unexpected event type %d", __func__, type);
                    break;
                }
            }
        });
    if (!rc) {
        auto hidl_status = event_cb_->initializationComplete(Status::INITIALIZATION_ERROR);
        if (!hidl_status.isOk()) {
            ALOGE("Unable to call initializationComplete(ERR)");
        }
    }

    unlink_cb_ = [this](sp<FmDeathRecipient>& death_recipient) {
        if (death_recipient->getHasDied()){
            ALOGI("Skipping unlink call, service died.");
        } else {
            event_cb_->unlinkToDeath(death_recipient);
            ALOGI(" unlink  to death recipient ");
        }
    };

    return Void();
}

Return<void> FmHci::sendHciCommand(const hidl_vec<uint8_t>& command) {
    ALOGI("sendFmCommand");
    sendDataToController(HCI_PACKET_TYPE_FM_CMD, command);
    return Void();
}

Return<void> FmHci::close() {
    ALOGI("close");
    DataHandler::CleanUp(TYPE_FM);
    unlink_cb_(death_recipient_);
    return Void();
}

void FmHci::sendDataToController(HciPacketType type,
                                        const hidl_vec<uint8_t>& data) {
    DataHandler *data_handler = DataHandler::Get();

    if (data_handler != nullptr) {
        data_handler->SendData(TYPE_FM, type, data.data(), data.size());
    } else {
        ALOGI("sendDataToController: data_handler is null");
    }

}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IFmHci* HIDL_FETCH_IFmHci(const char* /* name */) {
    return new FmHci();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace fm
}  // namespace hardware
}  //namespace qti
}  // namespace vendor
