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
class RilPCODataMessage : public UnSolicitedMessage,public add_message_id<RilPCODataMessage>  {
private:
  RIL_PCO_Data pco;

public:
  static constexpr const char *MESSAGE_NAME = "RIL_UNSOL_PCO_DATA";

  RilPCODataMessage() = delete;
  ~RilPCODataMessage();

  inline RilPCODataMessage(RIL_PCO_Data const &p):UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    pco = p; 
  }

  const RIL_PCO_Data& getDataParams() {
    return pco;
  }

  string dump() {
    return RilPCODataMessage::MESSAGE_NAME;
  }
};
} //namespace
