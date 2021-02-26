/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "modules/data/DataCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class DDSStatusFollowupMessage: public UnSolicitedMessage,
                         public add_message_id<DDSStatusFollowupMessage> {

private:
  DDSStatusInfo mDDSStatusInfo;
  int mInstanceId;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_DDS_STATUS_FOLLOWUP";

  DDSStatusFollowupMessage() = delete;
  ~DDSStatusFollowupMessage();

  DDSStatusFollowupMessage(int instance_id, const DDSStatusInfo& dds_status);

  std::shared_ptr<UnSolicitedMessage> clone();
  const DDSStatusInfo& getDDSStatusInfo();
  string dump();
  int getInstanceId();
};

} //namespace
