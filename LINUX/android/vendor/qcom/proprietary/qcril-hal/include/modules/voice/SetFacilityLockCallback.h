/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <string>

#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "framework/legacy.h"
#include "modules/uim/UimSetFacilityLockRequestMsg.h"

class SetFacilityLockCallback : public GenericCallback<UimSetFacilityLockResponseMsg> {
 private:
  qcril_instance_id_e_type  mInstanceId;
  RIL_Token                 mRilToken;
  qcril_evt_e_type          mEventId;

 public:
  SetFacilityLockCallback() = delete;

  inline SetFacilityLockCallback(qcril_instance_id_e_type instanceId, RIL_Token t,
        qcril_evt_e_type eventId, string clientToken)
      : GenericCallback(clientToken){
    mInstanceId = instanceId;
    mRilToken = t;
    mEventId = eventId;
  }

  ~SetFacilityLockCallback();
  Message::Callback *clone() override;
  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
          std::shared_ptr<UimSetFacilityLockResponseMsg> responseDataPtr) override;
};
