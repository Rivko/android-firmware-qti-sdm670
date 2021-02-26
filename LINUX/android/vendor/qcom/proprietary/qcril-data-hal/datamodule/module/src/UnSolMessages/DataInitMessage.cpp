/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include<string>
#include "UnSolMessages/DataInitMessage.h"

using namespace rildata;

DataInitMessage::DataInitMessage
(
  int instance_id
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
}

DataInitMessage::~DataInitMessage()
{
}

std::shared_ptr<UnSolicitedMessage> DataInitMessage::clone()
{
  return std::make_shared<DataInitMessage>(mInstanceId);
}

string DataInitMessage::dump()
{
  return DataInitMessage::MESSAGE_NAME;
}

int DataInitMessage::getInstance()
{
  return mInstanceId;
}
