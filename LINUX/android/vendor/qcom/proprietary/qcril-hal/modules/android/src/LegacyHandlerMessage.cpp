/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/android/LegacyHandlerMessage.h"

LegacyHandlerMessage::~LegacyHandlerMessage() {
}

Module* LegacyHandlerMessage::getSubModule() {
  return mSubModule;
}

qcril_req_handler_type* LegacyHandlerMessage::getLegacyHandler() {
  return mHandler;
}

qcril_request_params_type* LegacyHandlerMessage::getLegacyParams() {
  return mParams;
}

string LegacyHandlerMessage::dump() {
  return mName;
}
