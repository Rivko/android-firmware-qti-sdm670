/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "data_system_determination_v01.h"

namespace rildata {

/********************** Class Definitions *************************/
class DsdSysStatusMessage: public UnSolicitedMessage,
                           public add_message_id<DsdSysStatusMessage> {

private:
  dsd_system_status_ind_msg_v01 mDsdSysStatus;
  int mInstanceId;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_DSD_SYS_STATUS";

  DsdSysStatusMessage() = delete;
  ~DsdSysStatusMessage();

  DsdSysStatusMessage(int instance_id, dsd_system_status_ind_msg_v01 const &p);

  std::shared_ptr<UnSolicitedMessage> clone();

  const dsd_system_status_ind_msg_v01& getParams();

  string dump();

  int getInstanceId();
};

} //namespace
