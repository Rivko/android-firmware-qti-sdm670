/******************************************************************************
   @file    MetaMeter.cpp
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION
      Each feature has one metameter. Metameter creates and holds its meter objects.

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "MetaMeter.h"
#include "ThreadHelper.h"
#include "inc/MeterFwk.h"
#include "inc/MeterMgr.h"
#include "Error.h"

#include <cutils/properties.h>

#define LOG_TAG_MM "MetaMeter"
using namespace std;

MetaMeter::MetaMeter(const FeatureInfo& fInfo, FeatureState& featureState)
    : mFeatureName(fInfo.name),
    mFeatureState(featureState),
    mFeatureInfo (fInfo){
    for(unsigned int i=0;i<METER_MGR_MAX;i++) {
        metaMgrList[i] = new MeterMgr(fInfo, this, i);
        DEBUGD(LOG_TAG_MM,"Created MeterMgrs (feature, index , meterMgr) = (%s %u %p) \n",mFeatureName.c_str(), i, metaMgrList[i]);
    }
}

MetaMeter::~MetaMeter() {
    // TODO:: Lets handle this in a separate thread to avoid pthread_join() latency
    DEBUGV(LOG_TAG_MM,"%s MetaMeter::~MetaMeter() start for feature \n", mFeatureName.c_str());
    for(int i=0;i<METER_MGR_MAX;i++) {
        DEBUGD(LOG_TAG_MM,"%s Deleting MeterMgr %d \n",mFeatureName.c_str(), i);
        if(metaMgrList[i]) {
            delete metaMgrList[i];
        }
    }
    DEBUGV(LOG_TAG_MM,"%s MetaMeter::~MetaMeter() end \n", mFeatureName.c_str());
}

void MetaMeter::restoreMeterMgr(int meterMgrindex) {
    MeterMgr* meterMgr = NULL;
    MeterMgrState mtrMgrState = IDLE;
    unsigned int startedMeters = 0;
    unsigned int returnedMeters = 0;

    meterMgr = metaMgrList[meterMgrindex];
    if(!meterMgr) {
        return;
    }

    meterMgr->lock();

    mtrMgrState = meterMgr->getState();
    startedMeters = meterMgr->getStartedMetersSize();
    returnedMeters = meterMgr->getReturnedMetersSize();

    if((ACTIVE == mtrMgrState)&&( returnedMeters < startedMeters)) {
        DEBUGD(LOG_TAG_MM,"%s Setting Stop Notification. (index,state)=(%d,%d)", mFeatureName.c_str(), meterMgrindex,(int)mtrMgrState);
        meterMgr->setState(INACTIVE);
        DEBUGD(LOG_TAG_MM,"%s Setting Stop Notification  (index,state)=(%d,%d)", mFeatureName.c_str(), meterMgrindex, (int)meterMgr->getState());
        meterMgr->unlock();
        meterMgr->stopMeters();
    } else if((ACTIVE == mtrMgrState) && ( startedMeters == returnedMeters )) {
        DEBUGD(LOG_TAG_MM,"%s Setting Stop Notification  (index,state)=(%d,%d)", mFeatureName.c_str(), meterMgrindex, (int)mtrMgrState);
        meterMgr->reset();
        meterMgr->setState(IDLE);
        DEBUGD(LOG_TAG_MM,"%s Setting Stop Notification  (index,state)=(%d,%d)", mFeatureName.c_str(), meterMgrindex, (int)meterMgr->getState());
        meterMgr->unlock();
        meterMgr->stopMeters();
    } else {
        DEBUGD(LOG_TAG_MM,"%s No State Change needed!  (index,state)=(%d,%d)", mFeatureName.c_str(), meterMgrindex, (int)mtrMgrState);
        meterMgr->unlock();
    }
}

int MetaMeter::handlePrimaryAsync(const AsyncTriggerPayload& asyncData) {
    int meterMgrIndex = 0;
    MeterMgr* meterMgr = NULL;
    MeterMgrState mtrMgrState = IDLE;
    int retValue = SUCCESS;
    int meterMgrIndexToStart = -1;

    do {
        meterMgr = metaMgrList[meterMgrIndex];
        if(!meterMgr) {
            return METAMETER_NULL_METERMGR;
        }
        meterMgr->lock();
        mtrMgrState = meterMgr->getState();
        if(IDLE == mtrMgrState) {
            meterMgr->setState(ACTIVE);
            meterMgrIndexToStart = meterMgrIndex;
            DEBUGD(LOG_TAG_MM,"%s MeterMgr-%d oldState = %d, newState = %d, asyncId = %u",
                mFeatureName.c_str(),meterMgrIndex,mtrMgrState,meterMgr->getState(), asyncData.hintID);
        }
        meterMgr->unlock();
        meterMgrIndex++;
    } while((meterMgrIndexToStart == -1) && (meterMgrIndex != METER_MGR_MAX));

    if(meterMgrIndexToStart != -1) {
        DEBUGD(LOG_TAG_MM,"%s starting meters %d", mFeatureName.c_str(), meterMgrIndexToStart);
        metaMgrList[meterMgrIndexToStart]->startMeters(asyncData, true);
        for(int restoreIndex = 0; (restoreIndex < METER_MGR_MAX)&&(restoreIndex!=meterMgrIndexToStart) ; restoreIndex++) {
            restoreMeterMgr(restoreIndex);
        }
    } else {
        retValue = METAMETER_BUSY;
        DEBUGW(LOG_TAG_MM,"%s MetaMeter. No MeterMgr Available to handle this Primary Async. !!!!", mFeatureName.c_str());
    }
    return retValue;
}

int MetaMeter::handleSecondaryAsync(const AsyncTriggerPayload& asyncData) {
    int meterMgrIndex = 0;
    MeterMgr* meterMgr = NULL;
    MeterMgrState mtrMgrState = IDLE;
    int retValue = SUCCESS;
    int meterMgrIndexToStart = -1;

    do {
        meterMgr = metaMgrList[meterMgrIndex];
        if(!meterMgr) {
            return METAMETER_NULL_METERMGR;
        }
        meterMgr->lock();
        MeterMgrState mgrState = meterMgr->getState();
        if(ACTIVE==mgrState) {
            meterMgrIndexToStart = meterMgrIndex;
            DEBUGD(LOG_TAG_MM,"%s MeterMgr-%d oldState = %d, newState = %d, asyncId = %u\n",
                mFeatureName.c_str(),meterMgrIndex,mgrState,meterMgr->getState(), asyncData.hintID);
        }
        meterMgr->unlock();
        meterMgrIndex++;
    } while((meterMgrIndexToStart == -1) && (meterMgrIndex != METER_MGR_MAX));

    if(meterMgrIndexToStart != -1) {
        DEBUGD(LOG_TAG_MM,"%s starting meters on %d", mFeatureName.c_str(), meterMgrIndexToStart);
        metaMgrList[meterMgrIndexToStart]->startMeters(asyncData, false);
    } else {
        retValue = METAMETER_INVALID_SECONADARY_ASYNC;
        DEBUGW(LOG_TAG_MM,"%s MetaMeterThread start(). No MeterMgr Available to handle this Secondary/Data Async. !!!! \n", mFeatureName.c_str());
    }

    return retValue;
}


int MetaMeter::handleAsync(const AsyncTriggerPayload& asyncData) {
    if(!metaMgrList[0] || !metaMgrList[1]) {
        DEBUGE(LOG_TAG_MM,"%s No Meter Mgr available", mFeatureName.c_str());
        return METAMETER_NULL_METERMGR;
    }

    // Both metaMgrList[0] & metaMgrList[0] will have same meterInfo. Hence validating based on metaMgrList[0]
    if(!metaMgrList[0]->isValidAsync(asyncData)) {
        DEBUGE(LOG_TAG_MM,"%s Invalid Async", mFeatureName.c_str());
        return METAMETER_INVALID_ASYNC;
    }

    if(!isValidMetameterState(asyncData.appName)) {
        DEBUGW(LOG_TAG_MM,"%s Invalid state for appName %s ", mFeatureName.c_str(), asyncData.appName.c_str());
        return METAMETER_INVALID_APP_FEATURE_STATE;
    }

    DEBUGV(LOG_TAG_MM,"%s MetaMeter handleAsync start() Entry", mFeatureName.c_str());

    int meterMgrIndex = 0;
    MeterMgr* meterMgr = NULL;
    MeterMgrState mtrMgrState = IDLE;
    int pendingAsync = 0;
    int retValue = SUCCESS;

    //Order of Asyncs in Asynclist is not being handled here currently
    // Both metaMgrList[0] & metaMgrList[0] will have same meterInfo. Hence validating based on metaMgrList[0]
    if(metaMgrList[0]->isPrimaryASync(asyncData)) {
        retValue = handlePrimaryAsync(asyncData);
    } else {
        retValue = handleSecondaryAsync(asyncData);
    }

    DEBUGV(LOG_TAG_MM,"%s MetaMeter handleAsync End() Exit", mFeatureName.c_str());

    return retValue;
}
