/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#ifndef _LOOPER_H__
#define _LOOPER_H__

#include <thread>
#include "framework/Message.h"
#include "framework/MessageQueue.h"

/*Forward declaration*/
class Module;

class Looper {
 protected:
  string mName;
  Module *mModule;
  std::unique_ptr<MessageQueue> mMessageQueue;
  std::thread* mLooperThread;

 public:
  static constexpr bool FEATURE_DYNAMIC_THREAD = false;
  std::mutex looperMutex;
  std::condition_variable looperCv;
  bool looperReady;
  bool mExit;
  bool mReleaseThread;

  Looper();
  virtual ~Looper();

  void init(Module *module);
  void dispatch(std::shared_ptr<Message> msg);
  void dispatchSync(std::shared_ptr<Message> msg);
  virtual void handleMessage(std::shared_ptr<Message> msg) = 0;
  virtual void handleMessageSync(std::shared_ptr<Message> msg) = 0;

  size_t getSize();
  bool isEmptyQueue();
  void dumpMessageQueue();
  std::shared_ptr<Message> getFirstMessage();

  std::thread *getLooperThread();
  void acquireThread();
  void releaseThread();
  string to_string();

  /* Only for simulation */
  void waitForLooperToConsumeAllMsgs();
  void killLooper();
};

inline Looper::Looper(){
    mExit = false;
    mLooperThread = nullptr;
    mMessageQueue = std::unique_ptr<MessageQueue>(new MessageQueue);
};

inline Looper::~Looper() {
  if (mMessageQueue) {
    mMessageQueue->clear();
  }
  killLooper();
  if (mLooperThread != nullptr) {
    mLooperThread->join();
    delete mLooperThread;
    mLooperThread = nullptr;
  }
  mMessageQueue = nullptr;
};

inline std::thread *Looper::getLooperThread() { return mLooperThread; }
inline string Looper::to_string() { return mName; }

#endif
