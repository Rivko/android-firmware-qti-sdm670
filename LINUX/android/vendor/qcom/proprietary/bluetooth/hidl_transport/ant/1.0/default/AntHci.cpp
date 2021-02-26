/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "AntHci.h"
#define LOG_TAG "com.qualcomm.qti.ant@1.0-AntHci"

namespace com {
namespace qualcomm {
namespace qti {
namespace ant {
namespace V1_0 {
namespace implementation {


class AntDeathRecipient : public hidl_death_recipient {
    public:
       AntDeathRecipient(const sp<IAntHci> hci) : mHci(hci) {}
       virtual void serviceDied(
            uint64_t /*cookie*/,
            const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
                ALOGE("AntDeathRecipient::serviceDied - Ant service died");
                has_died_ = true;
                mHci->close();
            }
       sp<IAntHci> mHci;
       bool getHasDied() const { return has_died_; }
       void setHasDied(bool has_died) { has_died_ = has_died; }
    private:
       bool has_died_;
};

AntHci::AntHci()
    : deathRecipient(new AntDeathRecipient(this)) {}


// Methods from ::com::qualcomm::qti::ant::V1_0::IAntHci follow.
Return<void> AntHci::initialize(const sp<IAntHciCallbacks>& cb) {
    ALOGI("AntHci::initialize()");
     if (cb == nullptr) {
        ALOGE("cb == nullptr! -> Unable to call initializationComplete(ERR)");
        return Void();
     }
     event_cb_ = cb;
     deathRecipient->setHasDied(false);
     event_cb_->linkToDeath(deathRecipient, 0);
     bool rc = DataHandler::Init( TYPE_ANT,
         [this](bool status) {
         auto hidl_status = event_cb_->initializationComplete(
              status ? Status::SUCCESS : Status::INITIALIZATION_ERROR);
             ALOGI("AntHci::init status :%d",status);
             if (!hidl_status.isOk()) {
                 ALOGE("Unable to call initializationComplete()");
             }
         },
         [this](HciPacketType type, const hidl_vec<uint8_t> *packet) {
           if(event_cb_ == nullptr) return;
           switch (type) {
               case HCI_PACKET_TYPE_ANT_CTRL:
                  ALOGD("%s event type: %d", __func__, type);
                  event_cb_->antControlReceived(*packet);
                  break;
               case HCI_PACKET_TYPE_ANT_DATA:
                  event_cb_->antDataReceived(*packet);
                  break;
               default:
                  ALOGE("%s Unexpected event type %d", __func__, type);
                  break;
           }
     });
     if (!rc) {
        auto hidl_status = event_cb_->initializationComplete(Status::INITIALIZATION_ERROR);
        if (!hidl_status.isOk()) {
            ALOGE("Unable to call initializationComplete(ERR)");
        }
     }
     unlink_cb_ = [this](sp<AntDeathRecipient>& deathRecipient) {
        if (deathRecipient->getHasDied()){
            ALOGI("Skipping unlink call, service died.");
        } else {
            event_cb_->unlinkToDeath(deathRecipient);
            ALOGI(" unlink  to death recipient ");
        }
     };

    return Void();
}

Return<void> AntHci::sendAntControl(const hidl_vec<uint8_t>& command) {
    sendDataToController(HCI_PACKET_TYPE_ANT_CTRL,command);
    return Void();
}

Return<void> AntHci::sendAntData(const hidl_vec<uint8_t>& command) {
    sendDataToController(HCI_PACKET_TYPE_ANT_DATA,command);
    return Void();
}

Return<void> AntHci::close() {
  ALOGW("AntHci::close()");
  //if(event_cb_ != nullptr) event_cb_->unlinkToDeath(deathRecipient);
   DataHandler::CleanUp(TYPE_ANT);
   unlink_cb_(deathRecipient);
   return Void();
}

void AntHci::sendDataToController(HciPacketType type,
                                        const hidl_vec<uint8_t>& data)
{
  DataHandler *data_handler = DataHandler::Get();
  if(data_handler != nullptr) {
    data_handler->SendData(TYPE_ANT, type, data.data(), data.size());
  }
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IAntHci* HIDL_FETCH_IAntHci(const char* /* name */) {
    ALOGV("HIDL_FETCH_IAntHci");
    return new AntHci();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace ant
}  // namespace qti
}  // namespace qualcomm
}  // namespace com
