/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>
#include "telephony/ril.h"

namespace rildata {

/********************** Request Definitions **********************/

/********************** Class Definitions *************************/
class RilLceDataRecvMessage : public UnSolicitedMessage ,public add_message_id<RilLceDataRecvMessage>  {
private:
  RIL_LceDataInfo lce;

public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_LCEDATA_RECV";

  RilLceDataRecvMessage() = delete;
  ~RilLceDataRecvMessage();

  inline RilLceDataRecvMessage(RIL_LceDataInfo const &p):UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    lce = p;
  }

  const RIL_LceDataInfo& getLceDataParams() {
    return lce;
  }

  string dump() {
    return RilLceDataRecvMessage::MESSAGE_NAME;
  }
};
} //namespace
