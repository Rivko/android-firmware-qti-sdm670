/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/legacy.h"
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "DataCommon.h"
#include <framework/add_message_id.h>
//#include "framework/qcrili.h"
#include "legacy/qcril_data.h"

namespace rildata {

/********************** Request Definitions **********************/

typedef enum
{
  IS_DATA_ENABLED,
  IS_DATA_ROAMING_ENABLED,
  SET_RAT_PREF,
  GET_RAT_PREF
}AsyncCallingFunc;

/********************** Class Definitions *************************/
// FIXME remove template param once framework fixes it...this is a NO-OP
class RilEventSetRatPrefCallback : public UnSolicitedMessage, public add_message_id<RilEventSetRatPrefCallback> {

private:
  qcril_data_cb_struct p;
protected: 	
  void deepCopy(qcril_data_cb_struct const &p);
public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_SET_RAT_PREF_CALLBACK";

  RilEventSetRatPrefCallback() = default;
  ~RilEventSetRatPrefCallback();

  inline RilEventSetRatPrefCallback(qcril_data_cb_struct const &p):UnSolicitedMessage(get_class_message_id()) {
    deepCopy(p);
  }
  const qcril_data_cb_struct& getCallParams();
  string dump();
};

} //namespace
