/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "modules/uim/qcril_uim_lpa.h"

class LpaUimReqMessageCallback : public GenericCallback<qcril_uim_lpa_response_type> {
 public:
  inline LpaUimReqMessageCallback(string clientToken)
      : GenericCallback(clientToken){};

  inline ~LpaUimReqMessageCallback(){};

  Message::Callback *clone() override;

  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                  std::shared_ptr<qcril_uim_lpa_response_type> responseDataPtr) override;
};
