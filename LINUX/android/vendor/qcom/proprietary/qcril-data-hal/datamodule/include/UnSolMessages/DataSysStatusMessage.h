/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "wireless_data_service_v01.h"

namespace rildata {

/********************** Class Definitions *************************/
class DataSysStatusMessage: public UnSolicitedMessage,
                            public add_message_id<DataSysStatusMessage> {

private:
  wds_data_system_status_type_v01 mWdsSysStatus;
  int mInstanceId;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SYS_STATUS";

  DataSysStatusMessage() = delete;
  ~DataSysStatusMessage();

  DataSysStatusMessage(int instance_id, wds_data_system_status_type_v01 const &p);

  std::shared_ptr<UnSolicitedMessage> clone();

  const wds_data_system_status_type_v01& getParams();

  string dump();

  int getInstanceId();
};

} //namespace
