/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Module.h"
#include "modules/qmi/QmiStruct.h"

class ModemEndPoint {
 public:
  string mName;
  Module *mModule;
  static const int DEFAULT_SYNC_TIMEOUT = 500;
  static const int SYNC_REQ_UNRESTRICTED_TIMEOUT = 30000;

  enum class State {
    NON_OPERATIONAL,
    WAITING_SERVICE_UP,
    CLIENT_INIT_REQUESTED,
    OPERATIONAL,
  };

  ModemEndPoint() = default;

  ModemEndPoint(string name);
  inline std::string to_string() {
      return mName;
  }

  ModemEndPoint::State getState();
  void setState(ModemEndPoint::State state);
  // for legacy support
  qmi_client_error_type
  sendRawSync(unsigned int msgId, void* reqData, unsigned int reqSize,
    void* respData, unsigned int respSize, int timeout = ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
  qmi_client_error_type
  sendRawAsync(unsigned int msgId, void* reqData, unsigned int reqSize,
    unsigned int respSize, qmiAsyncCbType cb, void *cbData, Module *clientModule);
  virtual ~ModemEndPoint();

 private:
  State mState;
};

inline ModemEndPoint::ModemEndPoint(string name) {
  mName = name;
  mState = State::NON_OPERATIONAL;
}

inline void ModemEndPoint::setState(ModemEndPoint::State state) {
  mState = state;
}

inline ModemEndPoint::State ModemEndPoint::getState() { return mState; }

inline ModemEndPoint::~ModemEndPoint() {}
