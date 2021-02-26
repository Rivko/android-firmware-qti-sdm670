/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
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
class NewDDSInfoMessage: public UnSolicitedMessage,
                         public add_message_id<NewDDSInfoMessage> {

private:
  DDSSubIdInfo mDdsSubIdInfo;
  int mInstanceId;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_NEW_DDS_INFO";

  NewDDSInfoMessage() = delete;
  ~NewDDSInfoMessage();

  NewDDSInfoMessage(int instance_id, DDSSubIdInfo dds_sub_info);

  std::shared_ptr<UnSolicitedMessage> clone();
  DDSSubIdInfo getDDS();
  string dump();
  int getInstanceId();
};

} //namespace
