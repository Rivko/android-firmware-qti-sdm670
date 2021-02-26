/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>

#include <framework/Log.h>

#include "ModemEndPoint.h"

using std::mutex;
using std::lock_guard;

template <typename T>
class ModemEndPointFactory {
 public:
  inline virtual ~ModemEndPointFactory(){};
  inline static ModemEndPointFactory<T> &getInstance() {
    static ModemEndPointFactory<T> sInstance;
    return sInstance;
  }

  ModemEndPointFactory(ModemEndPointFactory const &) =
      delete;                                              // Copy construct
  ModemEndPointFactory(ModemEndPointFactory &&) = delete;  // Move construct
  ModemEndPointFactory &operator=(ModemEndPointFactory const &) =
      delete;  // Copy assign
  ModemEndPointFactory &operator=(ModemEndPointFactory &&) =
      delete;  // Move assign

  inline std::shared_ptr<T> buildEndPoint() {
    lock_guard<mutex> lock(mMutex);
    if (mModemEndPoint == nullptr) {
      mModemEndPoint = std::shared_ptr<T>(new T);
    }
    return mModemEndPoint;
  }

  inline void reset() {
    lock_guard<mutex> lock(mMutex);
    if (mModemEndPoint != nullptr) {
      Log::getInstance().d("[DEBUG]: " + mModemEndPoint->to_string() + " refcount = "  + std::to_string(mModemEndPoint.use_count()));
      /* No other known client active.*/
      if (mModemEndPoint.use_count() > 1) {
        Log::getInstance().d("[DEBUG]: some client still holding reference to " + mModemEndPoint->to_string());
      }
      mModemEndPoint = nullptr;
    }
  }
  inline int getRefCount() {
      if (mModemEndPoint != nullptr) {
          return mModemEndPoint.use_count();
      }
      else {
          return 0;
      }
  }

 private:
  mutex mMutex;
  ModemEndPointFactory() {}
  std::shared_ptr<T> mModemEndPoint;
};
