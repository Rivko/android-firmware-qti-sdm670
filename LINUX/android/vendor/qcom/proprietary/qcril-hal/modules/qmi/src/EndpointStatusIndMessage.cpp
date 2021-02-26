/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/qmi/EndpointStatusIndMessage.h"

EndpointStatusIndMessage::~EndpointStatusIndMessage() {
}

ModemEndPoint::State EndpointStatusIndMessage::getState() { return mState; }

string EndpointStatusIndMessage::dump() {
  return mName;
}
