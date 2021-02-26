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

/* Request to send a SMS message over IMS.
   @Sender: Android
   @Receiver: SmsModule
*/
class RilRequestImsSendSmsMessage : public SolicitedMessage<generic_callback_payload>,
                                    public add_message_id<RilRequestImsSendSmsMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_IMS_SEND_SMS";
    /*setting Message timeout as 245 sec as the maximum allowed value of the NV item
     * in MPSS for message retry is 240 secs
    */
    static constexpr int MESSAGE_EXPIRY_TIMEOUT{ 1000 * 245 };
    RilRequestImsSendSmsMessage() = delete;
    ~RilRequestImsSendSmsMessage();

    inline RilRequestImsSendSmsMessage(const ril_request_type &request):
        SolicitedMessage<generic_callback_payload>(get_class_message_id(), MESSAGE_EXPIRY_TIMEOUT) {
      mName = MESSAGE_NAME;
      deepCopy(request);
    }

    inline const qcril_request_params_type& get_request_params() {
      return params;
    }

    string dump();

  private:
    qcril_request_params_type params;
    char **gsm_message = nullptr;
    char *smsc_address = nullptr;
    char *gw_pdu = nullptr;
    RIL_CDMA_SMS_Message *cdma_sms_message = nullptr;
    void deepCopy(const ril_request_type &request);
};
