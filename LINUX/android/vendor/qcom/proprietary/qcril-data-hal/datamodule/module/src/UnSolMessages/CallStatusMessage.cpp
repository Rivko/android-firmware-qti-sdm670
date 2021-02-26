/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "UnSolMessages/CallStatusMessage.h"

using namespace rildata;

CallStatusMessage::CallStatusMessage
(
  int instance_id,
  qcril_data_net_evt_t evt,
  unsigned num_calls
) : UnSolicitedMessage(get_class_message_id())
{
  mName = CallStatusMessage::MESSAGE_NAME;
  mInstanceId = instance_id;
  mCallStatus.evt = evt;
  mCallStatus.num_calls = num_calls;
}

CallStatusMessage::~CallStatusMessage()
{
}

std::shared_ptr<UnSolicitedMessage> CallStatusMessage::clone()
{
  return std::make_shared<CallStatusMessage>(
               mInstanceId, mCallStatus.evt, mCallStatus.num_calls);
}

const CallStatusParams& CallStatusMessage::getCallParams()
{
  return mCallStatus;
}

string CallStatusMessage::dump()
{
  return CallStatusMessage::MESSAGE_NAME;
}

int CallStatusMessage::getInstanceId()
{
  return mInstanceId;
}
