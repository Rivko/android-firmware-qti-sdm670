/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "telephony/ril.h"
#include "modules/qmi/ModemEndPoint.h"

class EndpointStatusIndMessage : public UnSolicitedMessage {
 private:
  ModemEndPoint::State mState;

 public:
  static constexpr const char *MESSAGE_NAME =
        "com.qualcomm.qti.qcril.core.endpoint_status_message";
  EndpointStatusIndMessage() = delete;
  ~EndpointStatusIndMessage();

  inline EndpointStatusIndMessage(ModemEndPoint::State state,
         message_id_t msg_id) : UnSolicitedMessage(msg_id) {
    mState = state;
    mName = MESSAGE_NAME;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<EndpointStatusIndMessage>(
        mState, get_message_id());
    return msg;
  }

  ModemEndPoint::State getState();

  string dump();
};
