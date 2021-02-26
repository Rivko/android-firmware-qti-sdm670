/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "modules/data/DataCommon.h"

namespace rildata {

typedef struct
{
  qcril_data_net_evt_t evt;
  unsigned int num_calls;
} CallStatusParams;

/********************** Class Definitions *************************/
class CallStatusMessage : public UnSolicitedMessage,
                          public add_message_id<CallStatusMessage> {
private:
  CallStatusParams mCallStatus;
  int mInstanceId;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_CALL_STATUS";

  CallStatusMessage(int instance_id, qcril_data_net_evt_t evt, unsigned num_calls);
  ~CallStatusMessage();

  std::shared_ptr<UnSolicitedMessage> clone(); 

  const CallStatusParams& getCallParams();

  string dump();

  int getInstanceId();
};

} //namespace
