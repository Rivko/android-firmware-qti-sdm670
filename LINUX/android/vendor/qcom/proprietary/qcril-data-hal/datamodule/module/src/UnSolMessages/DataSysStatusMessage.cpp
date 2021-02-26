/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <UnSolMessages/DataSysStatusMessage.h>

using namespace rildata;

DataSysStatusMessage::DataSysStatusMessage
(
  int instance_id,
  wds_data_system_status_type_v01 const &p
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
  mWdsSysStatus = p;
}

DataSysStatusMessage::~DataSysStatusMessage()
{
}

std::shared_ptr<UnSolicitedMessage> DataSysStatusMessage::clone()
{
  return std::make_shared<DataSysStatusMessage>
                          (mInstanceId, mWdsSysStatus);
}

const wds_data_system_status_type_v01& DataSysStatusMessage::getParams()
{
  return mWdsSysStatus;
}

string DataSysStatusMessage::dump()
{
  return DataSysStatusMessage::MESSAGE_NAME;
}

int DataSysStatusMessage::getInstanceId()
{
  return mInstanceId;
}
