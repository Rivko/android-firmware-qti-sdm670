/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/ThreadPoolManager.h"
#include "framework/ModuleLooper.h"
#include <typeinfo>

/* to call module init in constructor execution time */
static load_module<ThreadPoolManager> poolManager_load;

ThreadPoolManager& ThreadPoolManager::getInstance() {
    return (poolManager_load.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
*/
ThreadPoolManager::ThreadPoolManager() {
  mName = "ThreadPoolManager";
  mIdleThreadModule = nullptr;
  mNumModules = 0;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(TaskCompleteMessage, ThreadPoolManager::handleTaskCompleteMessage),
      HANDLER(DedicatedThreadMessage, ThreadPoolManager::handleDedicatedThreadMessage),
  };
}

/* Follow RAII.
*/
ThreadPoolManager::~ThreadPoolManager() {
  cleanupThreads();
}

/*
 * Module specific initialization that does not belong to RAII .
*/
void ThreadPoolManager::init() { Module::init(); }

/* Logic for decision of new thread creation */
bool ThreadPoolManager::isNewThreadCreationAllowed() {
    if (ThreadPoolManager::MAXIMUM_THREAD == -1 ||
            mNumModules < ThreadPoolManager::MAXIMUM_THREAD)
        return true;
    return false;
}

/* Logic for decision of idle thread disposal */
bool ThreadPoolManager::ifIdleThreadNeedDisposal() {
    if (mIdleThreadModule)
        return true;
    return false;
}

/*
 * public API
 */
void ThreadPoolManager::cleanupThreads()
{
    std::lock_guard<std::mutex> lock(mMutex);

    for (auto it = mThreadModules.begin(); it != mThreadModules.end();) {
        Module* module = *it;
        if (module != nullptr)
            delete module;
        it = mThreadModules.erase(it);
    }
    mIdleThreadModule = nullptr;
}

bool ThreadPoolManager::scheduleExecution(DedicatedThreadMessage::HandlerType func, void* cbData) {
    auto msg = std::make_shared<DedicatedThreadMessage>(func, cbData);
    if (!msg) {
        Log::getInstance().d("Failed to create the dedicated thread message");
        return false;
    }

    /* Call module::dispatch to let ThreadPoolManager handle this message first */
    dispatch(msg);
    return true;
}

void ThreadPoolManager::handleDedicatedThreadMessage(
        std::shared_ptr<DedicatedThreadMessage> msg) {

    /* Forward the message to sub modules, or put it in the pending queue */
    std::unique_lock<std::mutex> lock(mMutex);
    /* there is idle thread ready for execution */
    if (mIdleThreadModule) {
        auto module = mIdleThreadModule;
        mIdleThreadModule = nullptr;
        lock.unlock();
        Log::getInstance().d("Ready for execution due to existed idle thread");
        module->dispatch(msg);
        return;
    }
    /* if new thread creation is allowed, create a new module */
    if (isNewThreadCreationAllowed()) {
        auto module = new DedicatedThreadModule();
        if (module) {
            mNumModules++;
            mThreadModules.insert(module);
            lock.unlock();
            Log::getInstance().d("New module is created for dedicated thread execution");
            module->init();
            module->dispatch(msg);
            return;
        }
    }

    // if new thread creation is not allowed or gets failed, put it in the waiting queue
    Log::getInstance().d("Current request for execution is pended");
    mMessagesQueue.push_back(msg);
}

/*
 * List of individual private handlers for the subscribed messages.
*/
void ThreadPoolManager::handleTaskCompleteMessage(
        std::shared_ptr<TaskCompleteMessage> shared_msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + shared_msg->dump());

    auto module = shared_msg->getModule();
    if (!module) return;

    std::unique_lock<std::mutex> lock(mMutex);
    /* module does not exist, and most likely already destoryed as cleanup */
    if (mThreadModules.find(module) == mThreadModules.end())
        return;
    if (!mMessagesQueue.empty()) {
        auto entry = mMessagesQueue.front();
        mMessagesQueue.pop_front();
        lock.unlock();
        Log::getInstance().d("Fetch the pending msg = " + entry->dump() + " for execution");
        module->dispatch(entry);
    } else if (ifIdleThreadNeedDisposal()) {
        mThreadModules.erase(module);
        lock.unlock();
        Log::getInstance().d("Delete the module after execution finishes");
        delete module;
    } else {
        if (!mIdleThreadModule)
            mIdleThreadModule = module;
        lock.unlock();
    }
}

/* --- APIs for GTEST --- */
Module* ThreadPoolManager::getIdleModule() {
    return mIdleThreadModule;
}
int ThreadPoolManager::getTotalThreads() {
    return mThreadModules.size();
}
