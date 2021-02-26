/******************************************************************************
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Dispatcher.h"
#include "framework/Message.h"
template <typename T>
class GenericCallback : public Message::Callback {
 public:
  inline GenericCallback() : Callback(""){};
  inline GenericCallback(string str) : Callback(str){};
  using cb_t = std::function<void(std::shared_ptr<Message>, Status, std::shared_ptr<T>)>;
  inline GenericCallback(cb_t cb): Callback(""), mCallback(cb) {};
 protected:
  cb_t mCallback;
 public:
  inline virtual ~GenericCallback(){};
  virtual void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                          std::shared_ptr<T> responseDataPtr) {
    (void)solicitedMsg;
    (void)status;
    (void)responseDataPtr;
  };
  virtual Message::Callback *clone() {
      if(mCallback != nullptr) {
          return new GenericCallback(mCallback);
      } else {
          return nullptr;
      }
  }

  inline void handleAsyncResponse(std::shared_ptr<Message> solicitedMsg,
                                  Message::Callback::Status status,
                                  std::shared_ptr<void> responseDataPtr) {
      if (mCallback) {
          mCallback(solicitedMsg, status, std::static_pointer_cast<T>(responseDataPtr));
      } else {
          onResponse(solicitedMsg, status,
            std::static_pointer_cast<T>(responseDataPtr));
      }
  }
};
