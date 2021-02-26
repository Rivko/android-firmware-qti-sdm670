/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <UnSolMessages/DsdSysStatusMessage.h>

using namespace rildata;

DsdSysStatusMessage::DsdSysStatusMessage
(
  int instance_id,
  dsd_system_status_ind_msg_v01 const &p
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
  mDsdSysStatus = p;
}

DsdSysStatusMessage::~DsdSysStatusMessage()
{
}

std::shared_ptr<UnSolicitedMessage> DsdSysStatusMessage::clone()
{
  return std::make_shared<DsdSysStatusMessage>
                      (mInstanceId, mDsdSysStatus);
}

const dsd_system_status_ind_msg_v01& DsdSysStatusMessage::getParams()
{
  return mDsdSysStatus;
}

string DsdSysStatusMessage::dump()
{
  return DsdSysStatusMessage::MESSAGE_NAME;
}
