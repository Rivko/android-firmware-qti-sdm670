/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/legacy.h"
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>
#include "DataCommon.h"
#include "legacy/qcril_data.h"

namespace rildata {

/********************** Request Definitions **********************/


/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventAsyncCallback : public UnSolicitedMessage,public add_message_id<RilEventAsyncCallback> {
private:
  qcril_data_cb_struct p;
protected:
  void deepCopy(qcril_data_cb_struct const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_ASYNC_CALLBACK";

  RilEventAsyncCallback() = default;
  ~RilEventAsyncCallback();

  inline RilEventAsyncCallback(qcril_data_cb_struct const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const qcril_data_cb_struct& getCallParams();
  string dump();
};

} //namespace
