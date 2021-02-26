/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/SetQualityMeasurementMessage.h"

using namespace rildata;

SetQualityMeasurementMessage::SetQualityMeasurementMessage
(
  const dsd_set_quality_measurement_info_req_msg_v01& reqData
) : SolicitedMessage<qmi_response_type_v01>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mInfo = reqData;
}

SetQualityMeasurementMessage::~SetQualityMeasurementMessage()
{
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

string SetQualityMeasurementMessage::dump()
{
  return mName + "{ }";
}

dsd_set_quality_measurement_info_req_msg_v01 SetQualityMeasurementMessage::getInfo()
{
  return mInfo;
}

