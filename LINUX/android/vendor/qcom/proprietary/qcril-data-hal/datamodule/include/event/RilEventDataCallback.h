/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>
#include "dsi_netctrl.h"
namespace rildata {

/********************** Request Definitions **********************/

typedef struct
{
  dsi_net_evt_t      evt;
  void              *data;
  int                data_len;
  void              *self;
  dsi_evt_payload_t  payload;
} EventData;

/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventDataCallback : public UnSolicitedMessage,public add_message_id<RilEventDataCallback> {
private:
  EventData p;
protected:
  void deepCopy(EventData const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_EVENT_CALLBACK";

  RilEventDataCallback() = default;
  ~RilEventDataCallback();

  inline RilEventDataCallback(EventData const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const EventData& getEventData();
  string dump();
};

} //namespace
