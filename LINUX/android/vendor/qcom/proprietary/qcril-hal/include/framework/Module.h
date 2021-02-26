/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef __MODULE_H__
#define __MODULE_H__

#include <memory>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "framework/Looper.h"
#include "framework/Message.h"
#include "framework/MessageQueue.h"

#define HANDLER(MSG_TYPE, FUNC) {REG_MSG(MSG_TYPE::MESSAGE_NAME), ([this](auto msg) { Module::handler<MSG_TYPE>(std::bind(&FUNC, this, std::placeholders::_1), msg); })}
#define LEGACY_HANDLER(ANDROID_REQ_TYPE, FUNC) {ril_request_info_##ANDROID_REQ_TYPE.msg_id, ([this](auto msg) { Module::handler<RilRequestMessage>(std::bind(&FUNC, this, std::placeholders::_1), msg); })}

class Module: public std::enable_shared_from_this<Module> {
 protected:
  std::string mName;
  std::unique_ptr<Looper> mLooper;
  std::unordered_map<message_id_t,
                     std::function<void(std::shared_ptr<Message>)>
                     >
      mMessageHandler;

 public:
  Module();
  virtual ~Module();
  virtual void init();
  void dispatch(std::shared_ptr<Message> msg);
  void dispatchSync(std::shared_ptr<Message> msg);
  virtual void handleMessage(std::shared_ptr<Message> msg);
  virtual void handleMessageSync(std::shared_ptr<Message> msg);

  /* Below method is invoked by Dispather module only. */
  void onMessageArrival(std::shared_ptr<Message> msg);

  string to_string();
  std::thread *getLooperThread();
  size_t getUnProcessedMessageCount();
  bool hasValidLooper();

  template<class M>
  inline void handler(std::function<void(std::shared_ptr<M>)> f, std::shared_ptr<Message> msg) {
    auto event = std::static_pointer_cast<M> (msg);
    f(event);
  }

  /* Only for simulation */
  void waitForLooperToConsumeAllMsgs();
  void killLooper();

  std::recursive_mutex mModuleHandlerExecutionMutex;
};

inline Module::Module() = default;

template <typename M>
class load_module
{
    public:
        load_module() {
            get_module().init();
        }
        M &get_module() {
            static M module{};
            return module;
        }
};

#endif
