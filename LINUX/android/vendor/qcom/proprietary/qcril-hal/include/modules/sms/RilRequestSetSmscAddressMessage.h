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

/* Request to set the default Short Message Service Center address on the device.
   @Sender: Android
   @Receiver: SmsModule
*/
class RilRequestSetSmscAddressMessage : public SolicitedMessage<generic_callback_payload>,
                                        public add_message_id<RilRequestSetSmscAddressMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_SET_SMSC_ADDRESS";
    RilRequestSetSmscAddressMessage() = delete;
    ~RilRequestSetSmscAddressMessage();

    inline RilRequestSetSmscAddressMessage(const ril_request_type &request):
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
    void deepCopy(const ril_request_type &request);
};
