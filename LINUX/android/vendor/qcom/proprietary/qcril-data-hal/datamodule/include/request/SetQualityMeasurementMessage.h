/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "data_system_determination_v01.h"

namespace rildata {

class SetQualityMeasurementMessage : public SolicitedMessage<qmi_response_type_v01>,
                                     public add_message_id<SetQualityMeasurementMessage> {
    private:
      dsd_set_quality_measurement_info_req_msg_v01 mInfo;

    public:
      static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_QUALITY_MEASUREMENT";

      SetQualityMeasurementMessage() = delete;
      SetQualityMeasurementMessage(const dsd_set_quality_measurement_info_req_msg_v01& reqData);

      ~SetQualityMeasurementMessage();

      string dump();

      dsd_set_quality_measurement_info_req_msg_v01 getInfo();
};

}
