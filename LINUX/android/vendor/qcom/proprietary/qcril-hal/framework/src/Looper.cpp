/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/Looper.h"
#include "framework/Module.h"
#include "framework/Util.h"
#include "framework/Log.h"

using std::mutex;
using std::condition_variable;
using std::lock_guard;
using std::unique_lock;
using std::thread;
using std::runtime_error;

void myLoop(Looper *instance) {
  string TAG = instance->to_string();
  setThreadName(TAG.c_str());
  while (!instance->mExit) {
    {
      std::unique_lock<std::mutex> mylock(instance->looperMutex);
      Log::getInstance().d("\n\t[" + TAG + "]: Waiting...");
      instance->looperCv.wait(mylock,
                              [=] { return instance->looperReady == true; });
    }
    Log::getInstance().d("\t[" + TAG + "]: Finished waiting");
    while (instance->isEmptyQueue() == false) {
      Log::getInstance().d("\t[" + TAG + "]: Queue size = " +
                           std::to_string(instance->getSize()));
      std::shared_ptr<Message> msg = instance->getFirstMessage();
      if(msg)
      {
        string msgName = msg->to_string();
        Log::getInstance().d("\t[" + TAG + "]: Handle msg = " + msg->dump());

        instance->handleMessage(msg);

        Log::getInstance().d("\t[" + TAG + "]: Done handling msg = " + msgName);
      }
    }
    {
      std::unique_lock<std::mutex> mylock(instance->looperMutex);
      if (instance->isEmptyQueue()) {
        instance->looperReady = false;
      }
    }
    instance->looperCv.notify_all();
    if (Looper::FEATURE_DYNAMIC_THREAD) {
      instance->releaseThread();
      break;
    }
  }
  Log::getInstance().d("\t[" + TAG + "]: Exiting");
}

void Looper::dispatch(std::shared_ptr<Message> msg) {
  acquireThread();
  if (!mExit && mMessageQueue) {
    mMessageQueue->enqueue(msg);
  }
  {
    lock_guard<mutex> lock(looperMutex);
    looperReady = true;
  }
  looperCv.notify_all();
}

void Looper::dispatchSync(std::shared_ptr<Message> msg) {
  handleMessageSync(msg);
}

size_t Looper::getSize() {
  if (mMessageQueue) {
   return mMessageQueue->getSize();
  }
  return 0;
}

bool Looper::isEmptyQueue() {
  if (mMessageQueue) {
    return mMessageQueue->isEmpty();
  }
  return true;
}

void Looper::dumpMessageQueue() {
  if (mMessageQueue) {
    mMessageQueue->dumpMessageQueue();
  }
}

std::shared_ptr<Message> Looper::getFirstMessage() {
  if (mMessageQueue) {
    return mMessageQueue->pop();
  }
  return nullptr;
}

void Looper::init(Module *module) {
  mModule = module;
  mName = mModule->to_string() + "-Looper";
  if (mMessageQueue) {
    mMessageQueue->init(mModule->to_string());
  }
}

void Looper::acquireThread() {
  std::lock_guard<mutex> lock(looperMutex);
  if (mLooperThread == nullptr && mExit == false) {
    Log::getInstance().d("\t[" + to_string() + "]: AcquireThread***************************************************");
    mLooperThread = new std::thread ( myLoop, this );
  }
}

void Looper::releaseThread() {
  std::lock_guard<mutex> lock(looperMutex);
  if (Looper::FEATURE_DYNAMIC_THREAD) {
    if (mLooperThread != nullptr) {
      Log::getInstance().d("\t[" + to_string() + "]: ReleaseThread***************************************************");
      {
        std::lock_guard<mutex> lock(looperMutex);
        mReleaseThread = true;
        looperReady = true;
      }
      looperCv.notify_all();

      mLooperThread->join();
      delete mLooperThread;
      mLooperThread = nullptr;
    }
  }
}

void Looper::waitForLooperToConsumeAllMsgs() {
  while (true) {
    {
      std::unique_lock<std::mutex> mylock(looperMutex);
      looperCv.wait(mylock, [&] { return looperReady == false; });
    }
    if (isEmptyQueue()) {
      threadSleep(2);
      return;
    }
  }
}

void Looper::killLooper() {
  {
    std::lock_guard<mutex> lock(looperMutex);
    mExit = true;
    looperReady = true;
  }
  looperCv.notify_all();

}
