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
class SetPrefDataTechMessage : public UnSolicitedMessage,
                               public add_message_id<SetPrefDataTechMessage> {

private:
  qcril_data_pref_data_tech_e_type mTech;
  int mInstanceId;

public:

  static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_PREF_DATA_TECH";

  SetPrefDataTechMessage(int instance_id, qcril_data_pref_data_tech_e_type t);
  ~SetPrefDataTechMessage();

  std::shared_ptr<UnSolicitedMessage> clone();

  string dump();

  qcril_data_pref_data_tech_e_type get();

  int getInstanceId();
};

} //namespace
