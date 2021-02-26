/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "telephony/ril.h"
#include "modules/qmi/QmiStruct.h"

class QmiIndMessage : public UnSolicitedMessage {
 private:
  QmiIndMsgDataStruct *mIndDataPtr;

 public:
  QmiIndMessage() = delete;
  ~QmiIndMessage();

  inline QmiIndMessage(QmiIndMsgDataStruct *dataPtr,
         message_id_t msg_id) : UnSolicitedMessage(msg_id) {
    mName = msg_id->get_name();
    deepCopy(dataPtr);
  }
  void deepCopy(QmiIndMsgDataStruct const *dataPtr);

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<QmiIndMessage>(
        getData(), get_message_id());
    return msg;
  }

  QmiIndMsgDataStruct *getData();

  string dump();
};
