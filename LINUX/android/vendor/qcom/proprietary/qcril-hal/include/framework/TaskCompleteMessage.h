/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class TaskCompleteMessage : public UnSolicitedMessage,
                            public add_message_id<TaskCompleteMessage> {
 private:
  Module* mModule;

 public:
  static constexpr const char *MESSAGE_NAME = "TASK_COMPLETE_MESSAGE";
  TaskCompleteMessage() = delete;
  ~TaskCompleteMessage();

  inline TaskCompleteMessage(Module* module) :
            UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mModule = module;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<TaskCompleteMessage>(mModule);
    return msg;
  }

  Module* getModule();

  string dump();
};
