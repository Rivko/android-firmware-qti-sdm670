/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "framework/Log.h"

// template <typename T> class SyncApiSession;
// template <typename R> class CallbackSync;

/*
T = Callback Type.
R = Return type from callback
*/
template <typename T>
class SyncApiSession {
 public:
  template <typename R>
  class CallbackSync : public GenericCallback<R> {
    SyncApiSession &mOwner;

   public:
    inline CallbackSync(string str, SyncApiSession &owner)
        : GenericCallback<R>(str), mOwner{owner} {}

    inline ~CallbackSync() {}

    inline Message::Callback *clone() {
      CallbackSync *clone =
          new CallbackSync(GenericCallback<R>::mClientToken, mOwner);
      return clone;
    }

    inline void onResponse(std::shared_ptr<Message> solicitedMsg,
                           Message::Callback::Status status,
                           std::shared_ptr<R> responseDataPtr) {
      std::lock_guard<mutex> lock(mOwner.mSyncApiMutex);
      (void)solicitedMsg;
      Log::getInstance().d("[SyncApiSession]: Got trigger to unblock for msg = " + solicitedMsg->to_string());
      Dispatcher::getInstance().informMessageCallbackFinished(solicitedMsg);
      mOwner.mApiStatus = status;
      mOwner.mSyncApiResponse = responseDataPtr;
      mOwner.notifyCallbackExecuted();
    }
  };

  inline void waitForSyncApiResponse() {
    std::unique_lock<std::mutex> mylock(mSyncApiMutex);
    /* Wait only if callback has not been executed.*/
    if (!misPeerResponded) {
        Log::getInstance().d(
            "[SyncApiSession]: Blocking current thread until we "
            "hear back on our request...");
        mSyncApiCv.wait(mylock, [&] { return misPeerResponded == true; });
    }
  }

  /* Make sure mSyncApiMutex is already locked by caller.*/
  inline void notifyCallbackExecuted() {
    misPeerResponded = true;
    mSyncApiCv.notify_all();
  }

  inline void notifyTimeout() {
    std::lock_guard<mutex> lock(mSyncApiMutex);
    misPeerResponded = true;
    mApiStatus = Message::Callback::Status::TIMEOUT;
    mSyncApiResponse = nullptr;
    mSyncApiCv.notify_all();
  }

  inline std::shared_ptr<T> getSyncApiResult() {
    std::lock_guard<mutex> lock(mSyncApiMutex);
    return mSyncApiResponse;
  }

  inline Message::Callback::Status getApiStatus() {
    std::lock_guard<mutex> lock(mSyncApiMutex);
    return mApiStatus;
  }

  inline SyncApiSession(string token):
    mToken(token),
    mCallback(new CallbackSync<T>(token, *this)),
    misPeerResponded(false)
  {
  };

  inline ~SyncApiSession() {
  }

  string mToken;
  Message::Callback *mCallback;
  std::mutex mSyncApiMutex;
  std::condition_variable mSyncApiCv;
  bool misPeerResponded;
  Message::Callback::Status mApiStatus;
  std::shared_ptr<T> mSyncApiResponse;
};
