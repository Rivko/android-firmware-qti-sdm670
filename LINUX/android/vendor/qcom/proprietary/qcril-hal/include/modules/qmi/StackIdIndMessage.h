/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"
#include "qmi_client.h"

// This message is used to broadcast the stack id info. It may be broadcasted when
// bootup, or cross-mapping happens
class StackIdIndMessage : public UnSolicitedMessage,
                          public add_message_id<StackIdIndMessage>
{
 public:
  static constexpr const char* MESSAGE_NAME = "QMI_STACK_ID_IND";
  StackIdIndMessage() = delete;
  inline StackIdIndMessage(qcril_instance_id_e_type instance, int8_t stack):
      UnSolicitedMessage(get_class_message_id())
  {
    mInstanceId = instance;
    mStackId = stack;
  }
  ~StackIdIndMessage();

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<StackIdIndMessage>(mInstanceId, mStackId));
  }

  string dump();
  int8_t getStackId();
  qcril_instance_id_e_type getInstanceId();

 private:
  int8_t mStackId;
  qcril_instance_id_e_type mInstanceId;
};
