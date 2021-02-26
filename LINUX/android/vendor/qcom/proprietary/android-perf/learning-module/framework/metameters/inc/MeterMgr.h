/******************************************************************************
  @file    MetaMeter.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_METERMGR_H
#define FWK_METERMGR_H

#include <list>
#include <string>
#include <set>
#include <map>
#include <vector>

#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include "MeterReceiver.h"
#include "ThreadHelper.h"

#include "DebugLog.h"

enum MeterMgrState{
    IDLE = 0,
    ACTIVE = 1,
    INACTIVE = 2
};


class MetaMeter;

/*
This lock() is being shared by MeterMgr & MetaMeter.
This lock()/unlock() API's in this class is for protecting the following variables:
1. pendingMeters.
    pendingMeters will be reduced as and when each Meter returns via notifyReceiver.
    since notifyReceiver is trigger in respective Meter thread context, we need to protect metersCount.
2. MeterMgrState.
    MeterMgrState is update from MetaMeterThread & last meter of MeterMgr when it returns via notifyReceiver.
3. setStopNotification()

Lot of informtaion coming in to public. These should be moved to a private class.
*/

class CompareAsyncInfo {
public:
    bool operator()(const AsyncInfo& left, const AsyncInfo& right) const {
        // Return true if left and right are not equal and left can go before right.
        return std::tie(left.hintID,left.hintType) < std::tie(right.hintID,right.hintType);
    }
};

class MeterMgr: public MeterReceiver {
    class MeterHelper {
    public:
        MeterInfo meterInfo;
        Meter *meterInstance;
        ThreadHelper *meterThread;
        bool isStarted;
    };

    MetaMeter* mMetaMeter;
    pthread_mutex_t meter_mutex;
    MeterMgrState state;
    const FeatureInfo& mFInfo;

    std::set<AsyncInfo,CompareAsyncInfo> primaryAsyncSet;
    std::set<AsyncInfo,CompareAsyncInfo> secondaryAsyncSet;
    std::map<AsyncInfo,std::vector<MeterHelper*>, CompareAsyncInfo> asyncMeterMap;

    std::vector<MeterHelper*> mMeterHelperList;
    std::vector<Meter*> mMeterInstanceList; // Having this makes it easy to pass arg's for writeToDb() in notifyReceiver()

    // TODO:: try to avoid pointers and use references directly

    const unsigned int totalMeters;
    unsigned int startedMeters;
    unsigned int returnedMeters;
    const unsigned int mId;


    void createMeterHelper(MeterInfo& meterInfo);
    void deleteMeterHelpers();
    void notifyReceiver(Meter *mtr);

public:
    MeterMgr(const FeatureInfo& fInfo, MetaMeter* metaMeter, const unsigned int id);
    ~MeterMgr();

    int lock(){ return pthread_mutex_lock(&meter_mutex); }
    int unlock(){ return pthread_mutex_unlock(&meter_mutex); }

    bool isReady();
    void startMeters(const AsyncTriggerPayload& asyncData , bool isPrimary);
    void stopMeters();
    void reset();

    bool isPrimaryASync(const AsyncTriggerPayload& asyncData);
    bool isValidAsync(const AsyncTriggerPayload& asyncData);

    MeterMgrState getState() {return state;}
    unsigned int getStartedMetersSize() {return startedMeters;}
    unsigned int getReturnedMetersSize() {return returnedMeters;}

    void setState(MeterMgrState mtrMgrState){state = mtrMgrState;}
};
#endif
