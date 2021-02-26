/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <memory>
#include "framework/Dispatcher.h"
#include "framework/Message.h"
#include "framework/GenericCallback.h"
#include "framework/SyncApiSession.h"

template <typename R>
class SolicitedSyncMessage : public Message {
public:
  SolicitedSyncMessage() = delete;
  SolicitedSyncMessage(message_id_t id,
    TimeKeeper::millisec timeout = Dispatcher::SYNC_MESSAGE_EXPIRY_TIMEOUT);
  virtual ~SolicitedSyncMessage();
  Message::Callback::Status dispatchSync(std::shared_ptr<R> &responseData);
  void sendResponse(std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status, std::shared_ptr<R> responseData);
  Message::MessageType getMessageType() override;

private:
  Message::Callback::Status doDispatchSync(std::shared_ptr<R> &responseData);
};

template <typename R>
SolicitedSyncMessage<R>::SolicitedSyncMessage(message_id_t id, TimeKeeper::millisec timeout) :
  Message(id, timeout) {
}

template <typename R>
SolicitedSyncMessage<R>::~SolicitedSyncMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
}

template <typename R>
Message::Callback::Status SolicitedSyncMessage<R>::doDispatchSync(
  std::shared_ptr<R> &responseData) {
  std::shared_ptr<Message> shared_msgPtr(shared_from_this());

  string token = "Dispatcher-Sync-Token";
  SyncApiSession<R> session(token);
  /* Instrument the callback.*/
  mCallback = session.mCallback;
  TimeKeeper::timer_id tid = Dispatcher::getInstance().setTimeoutForMessage(shared_msgPtr,
    shared_msgPtr->getMessageExpiryTimer(),
    [shared_msgPtr, &session](void* user_data) {
      QCRIL_NOTUSED(user_data);
      if (!session.misPeerResponded && !(shared_msgPtr->isCallbackExecuting() || shared_msgPtr->isCallbackExecuted())) {
        Log::getInstance().d(
          "[Timer]: *************************TIME-OUT for sync msg = " +
          shared_msgPtr->dump());
        shared_msgPtr->cancelling();
        session.notifyTimeout();
        shared_msgPtr->cancelled();
      }
    }
  );
  Log::getInstance().d("["+mName+"]: timer_id =  " + std::to_string(tid));
  /* Destination module's handler would be executed now.*/
  Dispatcher::getInstance().dispatchSync(shared_msgPtr);

  /* Handler execution is finished, now wait for the callback to get executed.
     The destined module ideally would have called sendResponse() as part of handler.
     Or if sendResponse is being called out of context then we are going to wait for
     SYNC_MESSAGE_EXPIRY_TIMEOUT. If the callback is not yet executed, client is informed about timeout.*/
  session.waitForSyncApiResponse();

  responseData = session.getSyncApiResult();

  //Log::getInstance().d("["+mName+"]: clear timer_id =  " + std::to_string(tid));
  //TimeKeeper::getInstance().clear_timer(tid);
  /* Remove instrumentation.*/
  delete mCallback;
  mCallback = nullptr;
  return session.getApiStatus();
}

template <typename R>
Message::Callback::Status SolicitedSyncMessage<R>::dispatchSync(std::shared_ptr<R> &responseData) {
  std::shared_ptr<R> sync_shared_response{
    std::static_pointer_cast<R>(responseData) };
  Message::Callback::Status status = doDispatchSync(sync_shared_response);
  responseData = sync_shared_response;
  return status;
}

template <typename R>
void SolicitedSyncMessage<R>::sendResponse(std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
  std::shared_ptr<R> responseData) {

  if (!isExpired()) {
    invokeCallback(solicitedMsg, status, responseData);
    Dispatcher::getInstance().informMessageCallbackFinished(solicitedMsg);
  }
  else {
    Log::getInstance().d(
      "[SolicitedAsyncMessage]: Callback is not invoked, msg is "
      "already cancelled = " +
      solicitedMsg->to_string());
  }
}

template <typename R>
Message::MessageType SolicitedSyncMessage<R>::getMessageType() {
  return (Message::MessageType::SolicitedSyncMessage);
}
