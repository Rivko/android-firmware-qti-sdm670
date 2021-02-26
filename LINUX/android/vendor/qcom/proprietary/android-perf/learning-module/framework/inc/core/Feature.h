/******************************************************************************
 *   @file    Feature.h
 *   @brief   Feature abstract base class
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the abstract base class
 *   for Feature implementers to derive from.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef FEATURE_H
#define FEATURE_H

#include <string>
#include <atomic>
#include "FeatureInfo.h"
#include "FeatureState.h"
#include "MetaMeter.h"
#include "Algorithm.h"
#include "Action.h"

class Feature {
private:
    std::string mFeatureName;
protected:
    /*
     * Features should implement allocation, maintenance and destruction of
     * resources for MetaMeter, Algorithm, Action and InputDb
     * Features should assign the respective pointer to base members
     * LM checks for null and calls the pointer whenever it wants to use
     * a Feature component.
     */
    MetaMeter* mMetaMeter;
    Algorithm* mAlgorithm;
    Action* mAction;
    FeatureInfo mfeatureInfo;
    FeatureState mFeatureState;

public:
    Feature(FeatureInfo featureInfo) :
        mFeatureName(featureInfo.name),
        mMetaMeter(nullptr),
        mAlgorithm(nullptr),
        mAction(nullptr),
        mfeatureInfo(featureInfo),
        mFeatureState(featureInfo.name) {

    }

    virtual ~Feature() {}
    std::string getFeatureName() {
        return mFeatureName;
    }
/*
 * Features should implement the logic for Async Triggers in runAsync()
 * LM calls runAsync() for each AsyncTrigger the Feature is interested in
 */
    virtual void runAsync(AsyncTriggerPayload payload);


/*
 * Features should implement the logic for Idle Triggers in runAsyncIdle()
 * This is generally used to call algorithms when the system is idle
 * LM calls runAsyncIdle() of the Feature when it decides the system is idle
 */
    virtual void runAsyncIdle(const std::atomic_bool& interrupt);

};

#endif /* FEATURE_H */
