/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "data_system_determination_v01.h"
#include <framework/add_message_id.h>

namespace rildata {

/********************** Request Definitions **********************/

/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventDSDCallback : public UnSolicitedMessage,public add_message_id<RilEventDSDCallback> {
private:
  dsd_system_status_ind_msg_v01 p;
protected:
  void deepCopy(dsd_system_status_ind_msg_v01 const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_DSD_EVENT_CALLBACK,";

  RilEventDSDCallback() = default;
  ~RilEventDSDCallback();

  inline RilEventDSDCallback(dsd_system_status_ind_msg_v01 const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const dsd_system_status_ind_msg_v01& getParams();
  string dump();
};

} //namespace
