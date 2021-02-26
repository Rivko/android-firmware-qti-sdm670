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

/* XXX TODO
  THIS DEFINITION IS NOT CORRECT
*/

namespace rildata {
/********************** Request Definitions **********************/

/********************** Class Definitions *************************/
class RilDataCallListChangeMessage : public UnSolicitedMessage,public add_message_id<RilDataCallListChangeMessage> {

private:
  RIL_Data_Call_Response_v11 resp;

public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_DATA_CALL_LIST_CHANGED";

  RilDataCallListChangeMessage() = delete;
  ~RilDataCallListChangeMessage();

  inline RilDataCallListChangeMessage(RIL_Data_Call_Response_v11 const &p):UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    resp = p;
  }

  RIL_Data_Call_Response_v11 &getParams() {
    return resp;
  }

  string dump() {
    return RilDataCallListChangeMessage::MESSAGE_NAME;
  }
};

} //namespace
