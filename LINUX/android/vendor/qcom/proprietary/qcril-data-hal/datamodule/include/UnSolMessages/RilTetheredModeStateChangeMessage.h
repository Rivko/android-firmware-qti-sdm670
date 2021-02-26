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

typedef enum RIL_tethered_mode_states_e
{
  QCRIL_TETHERED_MODE_OFF = 0,
  QCRIL_TETHERED_MODE_ON = 1
} RIL_tethered_mode_state;

namespace rildata {

/********************** Class Definitions *************************/
class RilTetheredModeStateChangeMessage : public UnSolicitedMessage,public add_message_id<RilTetheredModeStateChangeMessage> {
private:
  RIL_tethered_mode_state state;

public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_RESPONSE_TETHERED_MODE_STATE_CHANGED";

  RilTetheredModeStateChangeMessage() = delete;
  ~RilTetheredModeStateChangeMessage();

  inline RilTetheredModeStateChangeMessage(RIL_tethered_mode_state const &p):UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    state = p;
  }

  const RIL_tethered_mode_state& getTetheredModeState() {
    return state;
  }

  string dump() {
    return RilTetheredModeStateChangeMessage::MESSAGE_NAME;
  }
};

} //namespace
