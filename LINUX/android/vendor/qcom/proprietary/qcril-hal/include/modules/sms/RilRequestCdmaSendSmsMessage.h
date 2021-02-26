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

/* Request to send a CDMA SMS message.
   @Sender: Android
   @Receiver: SmsModule
*/

class RilRequestCdmaSendSmsMessage : public SolicitedMessage<generic_callback_payload>,
                                     public add_message_id<RilRequestCdmaSendSmsMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_CDMA_SEND_SMS";
    /*setting Message timeout as 245 sec as the maximum allowed value of the NV item
     * in MPSS for message retry is 240 secs
    */
    static constexpr int MESSAGE_EXPIRY_TIMEOUT{ 1000 * 245 };

    RilRequestCdmaSendSmsMessage() = delete;
    ~RilRequestCdmaSendSmsMessage();

    inline RilRequestCdmaSendSmsMessage(const ril_request_type &request):
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
    void deepCopy(const ril_request_type &request);
};
