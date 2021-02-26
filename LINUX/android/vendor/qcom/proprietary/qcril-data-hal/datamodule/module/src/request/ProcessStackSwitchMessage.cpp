/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/ProcessStackSwitchMessage.h"

using namespace rildata;

ProcessStackSwitchMessage::ProcessStackSwitchMessage
(
  StackSwitchReq & params
) :SolicitedMessage<int>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mData.oldStackId = params.oldStackId;
  mData.newStackId = params.newStackId;
  mData.instanceId = params.instanceId;
}

ProcessStackSwitchMessage::ProcessStackSwitchMessage
(
  qcril_modem_stack_id_e_type old_stack_id,
  qcril_modem_stack_id_e_type new_stack_id,
  qcril_instance_id_e_type instance_id
) : SolicitedMessage<int>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mData.oldStackId = old_stack_id;
  mData.newStackId = new_stack_id;
  mData.instanceId = instance_id;
}

ProcessStackSwitchMessage::StackSwitchReq ProcessStackSwitchMessage::getParams()
{
  return mData;
}

ProcessStackSwitchMessage::~ProcessStackSwitchMessage()
{
  if (mCallback) {
      delete mCallback;
      mCallback = nullptr;
  }
}

string ProcessStackSwitchMessage::dump()
{
  return mName + "{ }";
}

