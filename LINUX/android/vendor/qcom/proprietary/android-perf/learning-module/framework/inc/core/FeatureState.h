/******************************************************************************
 *   @file    FeatureState.h
 *   @brief   Feature State Machine class
 *
 *   DESCRIPTION
 *      FeatureState class provides a state machine implementation for the
 *   Features to use per app. The state machine forces some restrictions on the
 *   allowed state transitions and FeatureState can operate the states of apps
 *   within the allowed transitions.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef FEATURESTATE_H
#define FEATURESTATE_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <mutex>
#include "lmDB.h"

/*
 * AppState is an enum to hold the state of the feature per app
 *
 * DATA_REQUIRED - Default Value: Features start each app in this Feature state
 *      it specifies meters have not run for this app and collected data required
 * DATA_COMPLETE - An app is moved to this Feature state when required
 *      data collection is completed. This state transition is generally expected
 *      to be done by the Metameter. This state is a prerequisite for Algorithm to
 *      start working on the data
 * EVAL_REQUIRED - Algorithm moves the app to this Feature State once it completes
 *      its run. This denotes the algorithm has completed but the output data
 *      needs to be evaluated through another set of evaluation data
 * EVAL_DATA_COMPLETE - Metameter is expected to provide the data for evaluation
 *      also. Metameter moves the app to this Feature State once the evaluation
 *      data collection is complete.
 * LEARNING_COMPLETE - Algorithm starts running again for evaluation of the output
 *      using the Evaluation data and moves to the app to this feature state once
 *      it decides that evaluation is success intending that learning is complete.
 * DISABLE_LEARNING - This state is used to disable learning for particular apps.
 *
 */

enum class AppState : unsigned int {
    DATA_REQUIRED = 0,
    DATA_COMPLETE = 1,
    EVAL_REQUIRED = 2,
    EVAL_DATA_COMPLETE = 3,
    LEARNING_COMPLETE = 4,
    DISABLE_LEARNING = 99
};

/*
 * FeatureState contains a map / db of app to the AppState - for each app
 * the current Feature state is stored in the map / db.
 */

template class LMDatabase<AppState>;
using AppStateDb = LMDatabase<AppState>;
using DbRowVect = AppStateDb::vecT<AppState>;

class FeatureState {
private:
    std::string mFeatureName;
    std::mutex mMutex;
    AppStateDb* mStateDb;
    /* Data structure used to enforce allowed state transitions */
    /*
     * Map of allowed transitions:
     * Key is the target state
     * Value is an unordered_set (hashmap) of allowed source states from which
     * the transition to the target state are allowed
     */
    std::unordered_map<AppState,std::unordered_set<AppState>> mAllowedTransitions = {
            {AppState::DATA_REQUIRED,
                    {AppState::DATA_COMPLETE,
                     AppState::EVAL_DATA_COMPLETE}
            },
            {AppState::DATA_COMPLETE,
                    {AppState::DATA_REQUIRED}
            },
            {AppState::EVAL_REQUIRED,
                    {AppState::DATA_COMPLETE,
                     AppState::EVAL_DATA_COMPLETE}
            },
            {AppState::EVAL_DATA_COMPLETE,
                    {AppState::EVAL_REQUIRED}
            },
            {AppState::LEARNING_COMPLETE,
                    {AppState::EVAL_DATA_COMPLETE}
            },
            {AppState::DISABLE_LEARNING,
                    {AppState::DATA_REQUIRED,
                     AppState::DATA_COMPLETE,
                     AppState::EVAL_REQUIRED,
                     AppState::EVAL_DATA_COMPLETE,
                     AppState::LEARNING_COMPLETE}
            },
    };

    DbRowVect createGetUniqueDR(std::string appName, bool readonly = false);
    int createRow(LMDBDataRowKey key);
    int makeRowUnique(const DbRowVect& dbRows, LMDBDataRowKey key);

    AppState readState(std::string appName);
    int writeState(std::string appName, AppState target);

public:
    FeatureState(std::string featureName);
    ~FeatureState();
    void setState(std::string appName, AppState state) {
        writeState(appName, state);
    }
    AppState getState(std::string appName) {
        return readState(appName);
    }
};

#endif /* FEATURESTATE_H */
