/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __DISPATCHER_MODULE_H
#define __DISPATCHER_MODULE_H
#include "framework/Module.h"
class DispatcherModule : public Module {
 public:
  DispatcherModule();

  ~DispatcherModule();

  void handleMessage(std::shared_ptr<Message> msg);
  void handleMessageSync(std::shared_ptr<Message> msg);
};
#endif
