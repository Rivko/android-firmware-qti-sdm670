/******************************************************************************
 *   @file    Algorithm.h
 *   @brief   Abstract base class for "Algorithm"s
 *
 *   DESCRIPTION
 *      Algorithm class provides an abstract base class for Algorithms to
 *   derive from. It contains the API required to be implemented for
 *   algorithms.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "FeatureState.h"
#include <atomic>

class Algorithm {
private:
    FeatureState& mFeatureState;
protected:
    void evalRequired(std::string appName) {
        mFeatureState.setState(appName, AppState::EVAL_REQUIRED);
    }

    void dataRequired(std::string appName) {
        mFeatureState.setState(appName, AppState::DATA_REQUIRED);
    }

    void evalDone(std::string appName) {
        mFeatureState.setState(appName, AppState::LEARNING_COMPLETE);
    }
    void disableLearning(std::string appName) {
        mFeatureState.setState(appName, AppState::DISABLE_LEARNING);
    }
    bool isValidAlgoState(std::string appName) {
        AppState appState = mFeatureState.getState(appName);
        return (appState == AppState::DATA_COMPLETE || appState == AppState::EVAL_DATA_COMPLETE);
    }
    bool isDataComplete(std::string appName) {
        return (mFeatureState.getState(appName) == AppState::DATA_COMPLETE);
    }
    bool isEvalDataComplete(std::string appName) {
        return (mFeatureState.getState(appName) == AppState::EVAL_DATA_COMPLETE);
    }
    /*
     * TODO: Remove this - useful in debug statements, but shouldn't be required
     * in normal flow
     */
    AppState getAppState(std::string appName) {
        return mFeatureState.getState(appName);
    }

public:
    Algorithm(FeatureState& featureState) :
            mFeatureState(featureState) {
    }
    virtual void runAlgo(const std::atomic_bool& interrupt) = 0;
    virtual ~Algorithm(){}
};

#endif /* ALGORITHM_H */
