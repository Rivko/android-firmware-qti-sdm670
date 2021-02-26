/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "UnSolMessages/SetPrefDataTechMessage.h"

using namespace rildata;

SetPrefDataTechMessage::SetPrefDataTechMessage
(
  int instance_id,
  qcril_data_pref_data_tech_e_type t
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInstanceId = instance_id;
  mTech = t;
}

SetPrefDataTechMessage::~SetPrefDataTechMessage()
{
}

std::shared_ptr<UnSolicitedMessage> SetPrefDataTechMessage::clone()
{
  return std::make_shared<SetPrefDataTechMessage>(mInstanceId, mTech);
}

string SetPrefDataTechMessage::dump()
{
  return SetPrefDataTechMessage::MESSAGE_NAME;
}

qcril_data_pref_data_tech_e_type SetPrefDataTechMessage::get()
{
  return mTech;
}

int SetPrefDataTechMessage::getInstanceId()
{
  return mInstanceId;
}
