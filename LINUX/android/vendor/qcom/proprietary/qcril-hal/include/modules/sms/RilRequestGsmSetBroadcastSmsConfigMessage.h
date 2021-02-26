/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "framework/legacy.h"
#include "modules/android/ril_request_info.h"

/* Request to set GSM/WCDMA cell broadcast SMS config.
   @Sender: Android
   @Receiver: SmsModule
*/
class RilRequestGsmSetBroadcastSmsConfigMessage : public SolicitedMessage<generic_callback_payload>,
                                                  public add_message_id<RilRequestGsmSetBroadcastSmsConfigMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG";
    RilRequestGsmSetBroadcastSmsConfigMessage() = delete;
    ~RilRequestGsmSetBroadcastSmsConfigMessage();

    inline RilRequestGsmSetBroadcastSmsConfigMessage(const ril_request_type &request):
        SolicitedMessage<generic_callback_payload>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      deepCopy(request);
    }

    inline const qcril_request_params_type& get_request_params() {
      return params;
    }

    string dump();

  private:
    qcril_request_params_type params;
    RIL_GSM_BroadcastSmsConfigInfo *broadcast_sms_config_info = nullptr;
    void deepCopy(const ril_request_type &request);
};
