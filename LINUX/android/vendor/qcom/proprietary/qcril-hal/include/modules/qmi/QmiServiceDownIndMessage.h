/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "qmi_client.h"

class QmiServiceDownIndMessage : public UnSolicitedMessage,
                                 public add_message_id<QmiServiceDownIndMessage>
{
 public:
  static constexpr const char* MESSAGE_NAME = "QMI_SERVICE_DOWN_IND";
  inline QmiServiceDownIndMessage(qmi_client_error_type errCode):
      UnSolicitedMessage(get_class_message_id())
  {
    mErrCode = errCode;
  }
  ~QmiServiceDownIndMessage();

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<QmiServiceDownIndMessage>(mErrCode));
  }

  string dump();
  qmi_client_error_type getErrorCode();


 private:
  qmi_client_error_type mErrCode;
};
