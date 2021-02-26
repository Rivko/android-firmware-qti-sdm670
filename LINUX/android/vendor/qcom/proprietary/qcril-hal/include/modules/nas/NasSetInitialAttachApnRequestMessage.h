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

class NasSetInitialAttachApnRequestMessage : public SolicitedMessage<generic_callback_payload>,
                              public add_message_id<NasSetInitialAttachApnRequestMessage>
{
 private:
  qcril_request_params_type *params;
  void deepCopy(const ril_request_type &request);
  bool allocate_and_copy_data(const char *src, char **des);

 public:
 static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_initial_attach_apn";
 NasSetInitialAttachApnRequestMessage() = delete;
  ~NasSetInitialAttachApnRequestMessage();

  inline NasSetInitialAttachApnRequestMessage(const ril_request_type &request):
      SolicitedMessage<generic_callback_payload>(get_class_message_id())
  {
    mName = MESSAGE_NAME;
    params = nullptr;
    deepCopy(request);
  }

  inline const qcril_request_params_type *get_params() {
    return params;
  }

  string dump();
};

