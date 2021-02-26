/******************************************************************************
  @file    MetaMeter.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION
        MetaMeter is an abstsract base class for MetaMeter implementation
        Every LM Feature has to implement this class
        MeterMeter groups required Meters and mansges start / stop via handleAsync() API
        writeToDb() should be implemented by Fature specific Meters.
        Feature will be notified with list of Meter in writeToDb() once the data is avaialble.
        Feature has to validate/sanitize the data and write it to input database which will be
        latter processed by Algo's

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_METAMETER_H
#define FWK_METAMETER_H

#include <vector>
#include <string>

#include "AsyncData.h"
#include "FeatureInfo.h"
#include "FeatureState.h"

// These are required ping/pong implementation.
#define METER_MGR_MAX 2
#define METER_MGR_0 0
#define METER_MGR_1 1

class MeterMgr;
class Meter;
/**
 * MetaMeter is an interface to LM.
 * LM should create and own, one MetaMeter instance per feature.
 * This class provides an API to handleAsync event.
 */
class MetaMeter {
    MeterMgr* metaMgrList[METER_MGR_MAX];
    std::string mFeatureName;
    // TODO:: future enhancements. will be moved out of MetaMeter
    FeatureState& mFeatureState;

    void restoreMeterMgr(int meterMgrindex);
    int handlePrimaryAsync(const AsyncTriggerPayload& data);
    int handleSecondaryAsync(const AsyncTriggerPayload& data);
protected:
    // TODO:: All the API's will be removed along with the FeatureState
    const FeatureInfo& mFeatureInfo;
    void dataComplete(std::string appName) {
        mFeatureState.setState(appName, AppState::DATA_COMPLETE);
    }
    void evalDataComplete(std::string appName) {
        mFeatureState.setState(appName, AppState::EVAL_DATA_COMPLETE);
    }
    bool isDataRequired(std::string appName) {
        return (mFeatureState.getState(appName) == AppState::DATA_REQUIRED);
    }
    bool isEvalDataRequired(std::string appName) {
        return (mFeatureState.getState(appName) == AppState::EVAL_REQUIRED);
    }
    bool isValidMetameterState(std::string appName) {
        AppState appState = mFeatureState.getState(appName);
        return (appState == AppState::DATA_REQUIRED || appState == AppState::EVAL_REQUIRED);
    }
public:
    /**
     * Creates MetaMeter instance for a given feature.
     * @param featureName - Name of the feature.
     */
    MetaMeter(const FeatureInfo& fInfo, FeatureState& featureState);

    /**
     * Destroys MetaMeter instance.
     */
    virtual ~MetaMeter();

    /**
     * Handles Async event. Will start all required meters for this feature.
     * The API returns immediately. Async event will be handled asynchronously.
     * @param data - AsyncTriggerPayload avaialble with Async trigger.
     */
    int handleAsync(const AsyncTriggerPayload& data);
    virtual bool writeToDataBase(std::vector<Meter*> meterList) = 0;
};
#endif
