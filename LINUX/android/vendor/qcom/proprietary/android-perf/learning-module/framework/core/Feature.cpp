/******************************************************************************
 *   @file    Feature.cpp
 *   @brief   Feature default implementation
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the Base implementation
 *   for Feature implementers to derive.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "Feature.h"
#include "DebugLog.h"

#define LOG_TAG_FEATURE "Feature"

void Feature::runAsync(AsyncTriggerPayload payload) {
    DEBUGV(LOG_TAG_FEATURE, "runAsync %s", mFeatureName.c_str());
    if (mMetaMeter) {
        /*
         * Check if the metameter is interested in this trigger:
         * We need to send the trigger if DEFAULT_HINTTYPE is present
         * for this hintID (metameter is interested in all hintTypes)
         * or if current hintType is present for this hintID in the
         * FeatureInfo
         *  */
        auto currHintIDTypes = mfeatureInfo.asyncIdList[payload.hintID];
        if (currHintIDTypes.count(DEFAULT_HINTTYPE) == 1
                || currHintIDTypes.count(payload.hintType) == 1) {
            DEBUGV(LOG_TAG_FEATURE, "calling handleAsync() %s", mFeatureName.c_str());
            mMetaMeter->handleAsync(payload);
        }
    }
}

void Feature::runAsyncIdle(const atomic_bool& interrupt) {
    DEBUGV(LOG_TAG_FEATURE, "runAsyncIdle %s", mFeatureName.c_str());
    if(mAlgorithm) {
        DEBUGV(LOG_TAG_FEATURE, "calling runAlgo() %s", mFeatureName.c_str());
        mAlgorithm->runAlgo(interrupt);
    }
}
