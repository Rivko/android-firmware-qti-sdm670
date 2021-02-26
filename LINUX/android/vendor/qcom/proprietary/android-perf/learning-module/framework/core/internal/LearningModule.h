/******************************************************************************
 *   @file    LearningModule.h
 *   @brief   Learning Module Framework Core
 *
 *   DESCRIPTION
 *      Learning Module is a framework that enables "Feature"s to be developed
 *    to "learn" at runtime and apply actions based on such learning.
 *    Each "Feature" contains a triplet - "Meters, Algorithm, Action". Feature
 *    takes "Input" from its Meters based on triggers from the system and
 *    stores it in an Input Database. Algorithm takes the stored Input from
 *    Database, "learns" from input and writes output for the Action to an
 *    Output Database. Action uses the Output Database to apply actions.
 *      Learning Module class provides the core functionality of the framework.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef LEARNINGMODULE_H
#define LEARNINGMODULE_H

#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>
#include "WaitingQueue.h"
#include "DebugLog.h"
#include "Feature.h"

#define LOG_TAG_LM "Core"

struct TriggerInfo{
    AsyncTriggerPayload payload;
    bool exitLMThread;
};

class LearningModule {
private:
    std::vector<Feature*> mFeatureList;

    std::atomic_bool lmThreadReady;
    std::thread lmThread;
    std::thread idleAlgoThread;
    std::atomic_bool idleAlgoThrBusy;
    std::atomic_bool algoThreadInterrupt;

    WaitingQueue<TriggerInfo> mTriggerQueue;
    std::unordered_map<type_hintid, std::vector<Feature*>> mAsyncFeatMap;

    LearningModule();
    LearningModule(LearningModule const&);
    LearningModule& operator=(LearningModule const&);
    ~LearningModule();

    void detectAndRunIdle(bool debugIdle);

    void lmMain();
    std::vector<void*> dlLibHandles;
    bool mEnableLMtracing;

public:
    static LearningModule& getInstance() {
/*
 * Meyer's singleton implementation
 * Thread-safe from C++ 11
 *
 * IMPORTANT - first getInstance() to create the singleton object is required
 * before further calls to trigger() - in particular, calling first ever
 * getInstance() and trigger() back to back is not supported and such triggers
 * which come before LM is initialized are dropped by design.
 *
 * All this is automatically taken care if first getInstance() is called from
 * perfmodule_init() and trigger() is called from perfmodule_submit_request()
 * which is the intended design.
 *
 */
        static LearningModule lmInstance;
        DEBUGV(LOG_TAG_LM,"LM getInstance");
        return lmInstance;
    }
    void trigger(AsyncTriggerPayload payload);
};

#endif /* LEARNINGMODULE_H */
