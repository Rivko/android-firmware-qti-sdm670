/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <deque>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "framework/DispatcherModule.h"
#include "framework/message_id.h"
#include "framework/Message.h"
#include "framework/MessageQueue.h"
#include "framework/Module.h"
#include "framework/TimeKeeper.h"
#include "framework/message_id.h"

using std::deque;
using std::recursive_mutex;
using std::lock_guard;

#define REG_MSG(name) Dispatcher::getInstance().registerMessage(name)

class Dispatcher {

 private:
  Module *mModule;
  recursive_mutex mMutex;


  /*Map, Key = Message string, Value = Set of Module pointers.*/
  std::vector<message_id_info> mMessageHandlers;
  bool is_handle_valid(message_id_t);

  Dispatcher();

 public:
  #ifdef HOST_EXECUTABLE_BUILD
    static constexpr int sIsHostExecutable = 1;
  #else
    static constexpr int sIsHostExecutable = 0;
  #endif
  static constexpr int MESSAGE_EXPIRY_TIMEOUT{1000 * 90};
  static constexpr int SYNC_MESSAGE_EXPIRY_TIMEOUT{ 1000 * 20 };
  ~Dispatcher();

  Dispatcher(Dispatcher const &) = delete;             // Copy construct
  Dispatcher(Dispatcher &&) = delete;                  // Move construct
  Dispatcher &operator=(Dispatcher const &) = delete;  // Copy assign
  Dispatcher &operator=(Dispatcher &&) = delete;       // Move assign

  void dispatch(std::shared_ptr<Message> msg);
  void dispatchSync(std::shared_ptr<Message> msg);
  void broadcast(std::shared_ptr<Message> msg);
  static Dispatcher &getInstance();
  void joinAllModulesLooper();
  void joinDispatcherLooper();
  void waitForLooperToConsumeAllMsgs();
  std::thread::id getDispatcherLooperThreadId();

  message_id_t registerMessage(const string &msg_name);
  void dump_message_id(message_id_t hdl);
  void dump_registered_handler();
  string getMessageName(message_id_t id);
  void registerHandler(message_id_t id, Module* module);
  void unRegisterHandler(Module *module);
  void forwardMsgToModule(std::shared_ptr<Message> msg);
  void forwardMsgToModuleSync(std::shared_ptr<Message> msg);
  void informMessageCallbackFinished(std::shared_ptr<Message> msg);
  size_t getUnProcessedMessageCount();

  TimeKeeper::timer_id setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout);
  TimeKeeper::timer_id setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout, TimeKeeper::handler_type);
  bool clearTimeoutForMessage(std::shared_ptr<Message> msg);
  void informMessageDispatchFailure(std::shared_ptr<Message> msg,
                                    Message::Callback::Status status);

  /* Unit test helper, dont use in production code*/
  int getRegisteredModulesCount();
  int getRegisteredMessagesCount();
  bool isMessageRegistered(string msgName);

  inline void reset() {
      joinAllModulesLooper();
      if (mModule) {
          delete mModule;
          mModule = nullptr;
      }

      //TimeKeeper::getInstance ().wait_for_looper_to_join();
      TimeKeeper::getInstance ().kill();
      {
        lock_guard<recursive_mutex> lock(mMutex);
        mMessageHandlers.clear();
      }
  }
  bool isAllMessageHandlerModulesWithLooper(std::shared_ptr<Message> msg);
};

inline Dispatcher::Dispatcher() {
  mModule = new DispatcherModule;
  mModule->init();
  // In order to have index 0 as an invalid marker
  registerMessage("<Invalid>");
}

/* this is singleton. Destructor would never be called.*/
inline Dispatcher::~Dispatcher() {
}
