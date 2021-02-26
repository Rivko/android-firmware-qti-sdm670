/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include <deque>
#include <unordered_set>

#include "framework/Module.h"
#include "framework/TaskCompleteMessage.h"
#include "framework/DedicatedThreadMessage.h"
#include "framework/DedicatedThreadModule.h"

class ThreadPoolManager : public Module {
 public:
  /* -1 means no limitation */
  static const int MAXIMUM_THREAD = -1;
  static ThreadPoolManager& getInstance();

  ThreadPoolManager();
  ~ThreadPoolManager();

  void init();
  /* public API for using thread pool */
  bool scheduleExecution(DedicatedThreadMessage::HandlerType func, void* cbData);
  void cleanupThreads();

  /* GTEST APIS */
  Module* getIdleModule();
  int getTotalThreads();

  string to_string();

 private:
  /* Message handlers */
  void handleTaskCompleteMessage(std::shared_ptr<TaskCompleteMessage> msg);
  void handleDedicatedThreadMessage(std::shared_ptr<DedicatedThreadMessage> msg);
  /* logic for decision for new thread creation*/
  bool isNewThreadCreationAllowed();
  /* Logic for decision of idle thread disposal */
  bool ifIdleThreadNeedDisposal();

  std::mutex mMutex;
  int mNumModules;
  // list of current active modules
  std::unordered_set<Module*> mThreadModules;
  // Messages pending on execution in dedicated thread
  std::deque<std::shared_ptr<Message>> mMessagesQueue;
  // Keep one idle module when clean up modules after task completes
  Module* mIdleThreadModule;
};
