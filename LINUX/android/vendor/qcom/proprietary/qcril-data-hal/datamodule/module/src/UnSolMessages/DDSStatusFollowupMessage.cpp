/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <UnSolMessages/DDSStatusFollowupMessage.h>

using namespace rildata;

DDSStatusFollowupMessage::DDSStatusFollowupMessage
(
  int instance_id,
  const DDSStatusInfo& dds_status
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
  mDDSStatusInfo = dds_status;
}

DDSStatusFollowupMessage::~DDSStatusFollowupMessage()
{
}

std::shared_ptr<UnSolicitedMessage> DDSStatusFollowupMessage::clone()
{
  return std::make_shared<DDSStatusFollowupMessage>
                            (mInstanceId, mDDSStatusInfo);
}

const DDSStatusInfo& DDSStatusFollowupMessage::getDDSStatusInfo()
{
  return mDDSStatusInfo;
}

string DDSStatusFollowupMessage::dump()
{
  return MESSAGE_NAME;
}

int DDSStatusFollowupMessage::getInstanceId()
{
  return mInstanceId;
}

