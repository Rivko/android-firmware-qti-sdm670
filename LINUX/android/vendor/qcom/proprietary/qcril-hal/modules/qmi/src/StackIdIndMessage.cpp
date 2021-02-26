/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/StackIdIndMessage.h"

StackIdIndMessage::~StackIdIndMessage() {
}

qcril_instance_id_e_type StackIdIndMessage::getInstanceId() {
  return mInstanceId;
}

int8_t StackIdIndMessage::getStackId() {
  return mStackId;
}

string StackIdIndMessage::dump() {
  return mName + " instance id: " + std::to_string((int)mInstanceId)
        + " stack id: " + std::to_string(mStackId);
}
