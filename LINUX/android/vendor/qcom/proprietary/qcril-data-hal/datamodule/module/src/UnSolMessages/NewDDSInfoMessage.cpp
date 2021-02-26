/******************************************************************************
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <UnSolMessages/NewDDSInfoMessage.h>

using namespace rildata;

NewDDSInfoMessage::NewDDSInfoMessage
(
  int instance_id,
  DDSSubIdInfo dds_sub_info
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
  mDdsSubIdInfo.dds_sub_id = dds_sub_info.dds_sub_id;
  mDdsSubIdInfo.switch_type = dds_sub_info.switch_type;
}

NewDDSInfoMessage::~NewDDSInfoMessage()
{
}

std::shared_ptr<UnSolicitedMessage> NewDDSInfoMessage::clone()
{
  return std::make_shared<NewDDSInfoMessage>( mInstanceId, mDdsSubIdInfo);
}

DDSSubIdInfo NewDDSInfoMessage::getDDS()
{
  return mDdsSubIdInfo;
}

string NewDDSInfoMessage::dump()
{
  return MESSAGE_NAME;
}

int NewDDSInfoMessage::getInstanceId()
{
  return mInstanceId;
}