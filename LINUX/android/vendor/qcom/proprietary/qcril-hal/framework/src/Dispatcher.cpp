/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "framework/Dispatcher.h"
#include "framework/Log.h"
#include "framework/MessageQueue.h"
#include "framework/Module.h"
#include "framework/PolicyManager.h"
#include "framework/message_id.h"

using std::lock_guard;
using Lock = std::recursive_mutex;
using ScopedLock = std::unique_lock<Lock>;

#define TAG "RILQ Dispatcher"
bool Dispatcher::is_handle_valid(message_id_t id)
{
    bool ret = false;

    if (id && id->m_name && !id->m_name->empty() && id->idx > 0 && id->idx < mMessageHandlers.size()) {
        message_id_info &internal = mMessageHandlers[id->idx];
        QCRIL_LOG_DEBUG("%s: internal.m_name %s. message_id: %p, internal: %p", __func__, internal.get_name().c_str(), id, &internal);
        if (internal.idx == id->idx ) {
            ret = true;
            return ret;
        }
    }

    QCRIL_LOG_DEBUG("%s: message_id: %p", __func__, id);
    return ret;
}

void Dispatcher::dump_message_id(message_id_t hdl) {
  if (hdl) {
      message_id_info &internal = mMessageHandlers[hdl->idx];
      std::string module_list_str;
      for (Module *it : internal.module_list) {
          module_list_str += it->to_string()+", ";
      }

      Log::getInstance().d("[Dispatcher]: dump_message_id m_name " + hdl->get_name()+ " regisered modules = " + module_list_str);
      QCRIL_LOG_DEBUG("%s: hdl->m_name %s. message_id: %p internal: %p registered modules = %s", __func__, hdl->get_name().c_str(), hdl, &internal, module_list_str.c_str());
  }
}

void Dispatcher::forwardMsgToModule(std::shared_ptr<Message> msg) {
  ScopedLock lock(mMutex);

  if (is_handle_valid(msg->m_h)) {
    message_id_info *hdl = &mMessageHandlers[msg->m_h->idx];
    /* Allow broadcast only for UnSolicitedMessage.
       Messages having next_hop as a module other than Dispatcher are exception
       to this rule. Message would be forwarded to the next_hop irrespective of multiple
       registered recipients.  TODO: Clients should fix their code to have cleaner interface.*/
    Module* next_hop_module = msg->get_next_hop();
    if (!next_hop_module) {
      Log::getInstance().d("[DispatcherModule]: next_hop_module is nullptr for msg = " + msg->to_string() + "!!");
      next_hop_module = mModule;
    }

    if (hdl->module_list.size() > 0) {
        if (hdl && hdl->module_list.size() > 1) {
          if (next_hop_module == mModule
              && msg->getMessageType() != Message::MessageType::UnSolicitedMessage) {
            //Broadcast not allowed for non-broadcast type messages.
            Log::getInstance().d("[DispatcherModule]: Broadcast not allowed for non-broadcast type msg = " +
              msg->dump());
            informMessageDispatchFailure(msg,
              Message::Callback::Status::BROADCAST_NOT_ALLOWED);
            return;
          }
        }

        for (Module *module : hdl->module_list) {
          bool attempt_delivery = false;

          if (next_hop_module == mModule) {
            /* next_hop is dispatcher module. Deliver the msg to all registered modules */
            attempt_delivery = true;
          } else if (next_hop_module == module) {
            /* next_hop is a pre-identified recepient. Deliver the msg to only that module */
            attempt_delivery = true;
          }

          if (attempt_delivery) {
            if (module && !module->hasValidLooper()) {
                Log::getInstance().d("[DispatcherModule]: Forwarding msg = " +
                                     msg->to_string() + " to module = " +
                                     module->to_string());
                lock.unlock();
                module->onMessageArrival(msg);
                lock.lock();
            }

          }
        }
    } else {
        Log::getInstance().d("[DispatcherModule]: No handler registered for msg = " +
                             msg->to_string());
        informMessageDispatchFailure(msg,
                                     Message::Callback::Status::NO_HANDLER_FOUND);

    }
  } else {
    Log::getInstance().d("[DispatcherModule]: No handler registered for msg = " +
                         msg->to_string());
    informMessageDispatchFailure(msg,
                                 Message::Callback::Status::NO_HANDLER_FOUND);
  }
}

/* this is called from Dispatcher context */
void Dispatcher::informMessageDispatchFailure(
    std::shared_ptr<Message> msg, Message::Callback::Status status) {
  msg->informDispatchFailure(msg, status);
  /* Clear out the restriction.*/
  std::shared_ptr<Restriction> restriction_sharedPtr =
      PolicyManager::getInstance().getMessageRestriction(msg);
  restriction_sharedPtr->onMessageCallbackCompletion(msg);
}

TimeKeeper::timer_id Dispatcher::setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout) {
  Log::getInstance().logTime("[DispatcherModule]: set timeout for " +
                             msg->dump());
  TimeKeeper::timer_id tid = TimeKeeper::getInstance ().set_timer(
      [this, msg](void* user_data) {
        QCRIL_NOTUSED(user_data);
        if (!(msg->isCallbackExecuting() || msg->isCallbackExecuted())) {
          Log::getInstance().logTime("[DispatcherModule]: Timer expired for " +
                                     msg->dump());
          Log::getInstance().d(
              "[Timer]: *************************TIME-OUT for msg = " +
              msg->dump());
          msg->cancelling();
          this->informMessageDispatchFailure(
              msg, Message::Callback::Status::TIMEOUT);

          Log::getInstance().d("Finished");
          /* Clear out the restriction.*/
          std::shared_ptr<Restriction> restriction_sharedPtr =
              PolicyManager::getInstance().getMessageRestriction(msg);
          restriction_sharedPtr->onMessageCallbackCompletion(msg);
          msg->cancelled();
        }
      },
      nullptr,
      maxTimeout);

  Log::getInstance().logTime("[" + msg->to_string() + "]: timer_id = " + std::to_string(tid));
  msg->setTimerId(tid);
  return tid;
}

TimeKeeper::timer_id Dispatcher::setTimeoutForMessage(std::shared_ptr<Message> msg, TimeKeeper::millisec maxTimeout, TimeKeeper::handler_type functor) {
  Log::getInstance().logTime("[DispatcherModule]: Set Client specific timeout for " +
    msg->to_string());
  TimeKeeper::timer_id tid = TimeKeeper::getInstance ().set_timer(functor, nullptr, maxTimeout);
  msg->setTimerId(tid);
  return tid;
}

bool Dispatcher::clearTimeoutForMessage(std::shared_ptr<Message> msg) {
  return TimeKeeper::getInstance ().clear_timer(msg->getTimerId());
}

/* This is called from module context.*/
void Dispatcher::informMessageCallbackFinished(std::shared_ptr<Message> msg) {
    /* Message transaction finishes when callback is executed.*/
    msg->callbackExecuted();
    /* Callback just finished. Cancel the timer.*/
    auto tid = msg->getTimerId();
    TimeKeeper::getInstance ().clear_timer(tid);

    /* Clear out the restriction.*/
    std::shared_ptr<Restriction> restriction_sharedPtr =
      PolicyManager::getInstance().getMessageRestriction(msg);
    restriction_sharedPtr->onMessageCallbackCompletion(msg);
}

void Dispatcher::forwardMsgToModuleSync(std::shared_ptr<Message> msg) {
  ScopedLock lock(mMutex);

  message_id_t hdl = msg->get_message_id();
  string msgStr = msg->to_string();
  if (msg && is_handle_valid(hdl)) {
    hdl = &mMessageHandlers[hdl->idx];
    if (hdl->module_list.size() > 0) {
        if (hdl->module_list.size() > 1) {
          if (msg->getMessageType() != Message::MessageType::UnSolicitedMessage) {
            //Broadcast not allowed for non-broadcast type messages.
            Log::getInstance().d("[DispatcherModule]: Broadcast not allowed for non-broadcast type msg = " +
              msg->dump());
            informMessageDispatchFailure(msg,
              Message::Callback::Status::BROADCAST_NOT_ALLOWED);
            return;
          }
        }
        for (Module *module : hdl->module_list) {
            Log::getInstance().d("[DispatcherModule]: Forwarding SYNC msg = " +
                                 msg->to_string() + " to module = " +
                                 module->to_string());
            lock.unlock();
            module->dispatchSync(msg);
            lock.lock();
        }
    } else {
        Log::getInstance().d("[DispatcherModule]: No handler register for msg = " +
                             msg->to_string());
        informMessageDispatchFailure(msg,
                                     Message::Callback::Status::NO_HANDLER_FOUND);
    }
  } else {
    Log::getInstance().d("[DispatcherModule]: No handler register for msg = " +
                         msg->to_string());
    informMessageDispatchFailure(msg,
                                 Message::Callback::Status::NO_HANDLER_FOUND);
  }
}

void Dispatcher::joinAllModulesLooper() {
  lock_guard<recursive_mutex> lock(mMutex);
  /*Join all known modules loopers*/
  for (const auto &elem : mMessageHandlers) {
    if (elem.get_idx() < 0) {
        continue;
    }
    for (const auto &module : elem.module_list) {
        std::thread *t = module->getLooperThread();
        if (t != nullptr) {
          Log::getInstance().d("[DispatcherModule]: Waiting for looper of " + module->to_string() + " to join.");
          t->join();
          Log::getInstance().d("[DispatcherModule]: looper of " + module->to_string() + " joined.");
        }
    }
  }
}

std::thread::id Dispatcher::getDispatcherLooperThreadId() {
  auto looperThread = mModule->getLooperThread();
  std::thread::id ret;
  if(looperThread){
     ret = looperThread->get_id();
  }
  return ret;
}

void Dispatcher::joinDispatcherLooper() {
  /*Join qcrilMainLooper*/
  std::thread *t = mModule->getLooperThread();
  mModule->killLooper();
  if (t != nullptr) {
      t->join();
  }
}

void Dispatcher::dispatch(std::shared_ptr<Message> msg) {
  bool isNeededToEnqueue = false;
  std::shared_ptr<Restriction> restriction_sharedPtr =
    PolicyManager::getInstance().getMessageRestriction(msg);
  Message::Callback::Status status = Message::Callback::Status::SUCCESS;

  do {
    if (restriction_sharedPtr->isDispatchAllowed(msg)) {
      ScopedLock lock(mMutex);
      if (msg && is_handle_valid(msg->m_h)) {
        message_id_info *hdl = &mMessageHandlers[msg->m_h->idx];
        if (hdl && hdl->module_list.size() > 0) {
          Module* next_hop_module = msg->get_next_hop();
          if (!next_hop_module) {
            Log::getInstance().d("[dispatch function]: next_hop_module is nullptr for msg = " + msg->to_string() + "!!");
            next_hop_module = mModule;
          }

          if((next_hop_module == mModule) && (hdl->module_list.size() > 1) && (msg->getMessageType() !=
              Message::MessageType::UnSolicitedMessage)) {
            //Broadcast not allowed for non-broadcast type messages.
            Log::getInstance().d("[dispatch function]: Broadcast not allowed for non-broadcast type msg = " +
              msg->dump());
            status = Message::Callback::Status::BROADCAST_NOT_ALLOWED;
            break;
          }

          for (Module *module : hdl->module_list) {
            if (module) {
              bool attempt_delivery = false;
              if (next_hop_module == mModule) {
                /* next_hop is dispatcher module. Deliver the msg to all registered modules */
                attempt_delivery = true;
              } else if (next_hop_module == module) {
                /* next_hop is a pre-identified recepient. Deliver the msg to only that module */
                attempt_delivery = true;
              }
              if (attempt_delivery) {
                if(!module->hasValidLooper()) {
                  isNeededToEnqueue = true;
                } else {
                    Log::getInstance().d("[dispatch function]: Forwarding msg = " +
                      msg->to_string() + " to module = " +
                      module->to_string());
                    lock.unlock();
                    module->onMessageArrival(msg);
                    lock.lock();
                }
              }
            }
          }
        } else {
            Log::getInstance().d("[dispatch function]: No handler registered for msg = " +
                             msg->to_string());
            status = Message::Callback::Status::NO_HANDLER_FOUND;
            break;
        }
      } else {
          Log::getInstance().d("[dispatch function]: No handler registered for msg = " +
                              msg->to_string());
          status = Message::Callback::Status::NO_HANDLER_FOUND;
          break;
      }
    }
  }while(0);

  if(Message::Callback::Status::SUCCESS != status) {
    informMessageDispatchFailure(msg, status);
  }

  if(true == isNeededToEnqueue) {
    mModule->dispatch(msg);
  }
}

void Dispatcher::broadcast(std::shared_ptr<Message> msg) {
  mModule->dispatch(msg);
}

void Dispatcher::dispatchSync(std::shared_ptr<Message> msg) {
  /* SyncApiSession manages the timeout for sync calls.*/
  mModule->dispatchSync(msg);
}

Dispatcher &Dispatcher::getInstance() {
  static Dispatcher sInstance;
  // std::cout << "\nDispatcher = " << &sInstance;
  return sInstance;
}

size_t Dispatcher::getUnProcessedMessageCount() {
  return mModule->getUnProcessedMessageCount();
}

void Dispatcher::registerHandler(message_id_t id, Module* module) {
  lock_guard<recursive_mutex> lock(mMutex);
  if (is_handle_valid(id) && module) {
    message_id_info *h = &mMessageHandlers[id->idx];
    h->module_list.push_back(module);
  }
  else {

      if (module == nullptr) {
          Log::getInstance().d("Module is null");
      }
      Log::getInstance().d("Invalid handle passed");
  }

  //();
}

void Dispatcher::dump_registered_handler() {
    Log::getInstance().d("[Dispatcher]: Dump message handler: start.");
    for (auto kv = mMessageHandlers.begin(); kv != mMessageHandlers.end(); kv ++) {
        dump_message_id(&(*kv));
    }
    Log::getInstance().d("[Dispatcher]: Dump message handler: finished.");
}

void Dispatcher::unRegisterHandler(Module *module) {
    //for (auto kv : mMessageHandlers) {
    lock_guard<recursive_mutex> lock(mMutex);

    Log::getInstance().d("[Dispatcher]: unregister module = " + module->to_string());
    for (auto kv = mMessageHandlers.begin(); kv != mMessageHandlers.end(); kv ++) {
        auto& registeredModuleSet = (*kv).module_list;
        registeredModuleSet.erase(std::remove_if(
                    registeredModuleSet.begin(),
                    registeredModuleSet.end(),
                    [&module] (auto &m) {
                        bool ret = module == m;
                        return ret; }
                    ),
                    registeredModuleSet.end());

    }
}

/* Unit test helper, dont use in production code*/
int Dispatcher::getRegisteredModulesCount() {
    lock_guard<recursive_mutex> lock(mMutex);
    int count = 0;
    for (auto &kv : mMessageHandlers) {
        auto& registeredModuleSet = kv.module_list;
        count += registeredModuleSet.size();
    }
    return count;
}
/* Unit test helper, dont use in production code*/
int Dispatcher::getRegisteredMessagesCount() {
    lock_guard<recursive_mutex> lock(mMutex);
    return mMessageHandlers.size();
}
/* Unit test helper, dont use in production code*/
bool Dispatcher::isMessageRegistered(string msgName) {
    lock_guard<recursive_mutex> lock(mMutex);
    for (auto &h : mMessageHandlers) {
        const std::string &name = h.get_name();
        if (name == msgName) {
            return true;
        }
    }
    return false;
}


/* Only for simulation*/
void Dispatcher::waitForLooperToConsumeAllMsgs() {
  mModule->waitForLooperToConsumeAllMsgs();
}

message_id_t Dispatcher::registerMessage(const string &msg_name)
{
    lock_guard<recursive_mutex> lock(mMutex);
    message_id_info *h = nullptr;
    for (auto &mh : mMessageHandlers ) {
        const std::string &name = mh.get_name();
        if (name == msg_name) {
            h = &mh;
            break;
        }
    }
    if (h == nullptr) {
        auto it = mMessageHandlers.insert(mMessageHandlers.end(), message_id_info{msg_name});
        h = &*it;
        h->idx = mMessageHandlers.size() - 1;
    }
    QCRIL_LOG_DEBUG("%s: Registered message %s. message_id: %p (%zu)", __func__, msg_name.c_str(), h, h->idx);
    return new message_id_info(*h);
}

string Dispatcher::getMessageName(message_id_t id)
{
    string ret = "";
    if (is_handle_valid(id)) {
        message_id_info *h = id;
        ret = h->get_name();
    }
    return ret;
}

bool Dispatcher::isAllMessageHandlerModulesWithLooper(std::shared_ptr<Message> msg) {
  bool allRegisteredModulesWithLooper = false;

  if (msg && is_handle_valid(msg->m_h)) {
    message_id_info *hdl = &mMessageHandlers[msg->m_h->idx];
    if (hdl && hdl->module_list.size() > 0) {
      allRegisteredModulesWithLooper = true;
      for (Module *module : hdl->module_list) {
        if (module && !module->hasValidLooper()) {
          allRegisteredModulesWithLooper = false;
          break;
        }
      }
    }
  }

  return allRegisteredModulesWithLooper;
}
