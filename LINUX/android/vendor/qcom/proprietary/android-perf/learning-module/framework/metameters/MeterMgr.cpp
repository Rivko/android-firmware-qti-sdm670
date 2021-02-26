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

#include "inc/MeterMgr.h"
#include "ThreadHelper.h"
#include "inc/MeterFwk.h"
#include "MeterRegistry.h"
#include "MetaMeter.h"

#define LOG_TAG_MM "MetaMeter"
using namespace std;

MeterMgr::MeterMgr(const FeatureInfo& fInfo , MetaMeter* metaMeter, const unsigned int id)
    : mMetaMeter(metaMeter),
      mFInfo(fInfo),
      totalMeters(mFInfo.meterInfoList.size()),
      startedMeters(0),
      returnedMeters(0),
      mId(id){

    pthread_mutex_init(&meter_mutex, NULL);
    lock();
    setState(IDLE);
    unlock();

    for(MeterInfo meterInfo : mFInfo.meterInfoList) {
        createMeterHelper(meterInfo);
        for(auto asyncInfo : meterInfo.triggersList) {
            if(asyncInfo.isPrimary) {
                primaryAsyncSet.emplace(asyncInfo);
            } else {
                secondaryAsyncSet.emplace(asyncInfo);
            }
        }
    }

    // Debug only
    for(auto asyncInfo : primaryAsyncSet) {
        DEBUGV(LOG_TAG_MM,"primaryAsyncSet : (hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, (int)asyncInfo.isPrimary);
    }
    for(auto asyncInfo : secondaryAsyncSet) {
        DEBUGV(LOG_TAG_MM,"secondaryAsyncSet : (hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, (int)asyncInfo.isPrimary);
    }
    for(map<AsyncInfo,vector<MeterHelper*>, CompareAsyncInfo>::iterator iter=asyncMeterMap.begin(); iter!=asyncMeterMap.end(); iter++) {
        AsyncInfo asyncInfo = iter->first;
        unsigned vectSize = iter->second.size();
        DEBUGV(LOG_TAG_MM,"asyncMeterMap - AsyncInfo : (hintID,hintType,Primary, size) = (%u, %d, %d, %u)",
            asyncInfo.hintID, asyncInfo.hintType, (int)asyncInfo.isPrimary,vectSize);
    }

}

void MeterMgr::createMeterHelper(MeterInfo& meterInfo) {
    MeterHelper *meterHelper = new MeterHelper();
    if(!meterHelper) {
        DEBUGE(LOG_TAG_MM,"Failed to create Meter Helper");
        return;
    }

    meterHelper->meterInfo = meterInfo;
    meterHelper->meterThread = new ThreadHelper(mFInfo.name + " " + meterInfo.name.c_str(),false);
    if(!meterHelper->meterThread) {
        delete meterHelper;
        return;
    }
    meterHelper->meterInstance = MeterRegistryBase::get(meterInfo);
    meterHelper->isStarted = false;

    if(!meterHelper->meterInstance) {
        delete meterHelper->meterThread;
        delete meterHelper;
        return;
    }

    MeterFwk* meterFwk = meterHelper->meterInstance->getMeterFwk();
    if(meterFwk) {
        meterFwk->setThreadHelper(meterHelper->meterThread);
        meterFwk->setReceiver(this);
    } else {
        delete meterHelper->meterInstance;
        delete meterHelper->meterThread;
        delete meterHelper;
        return;
    }

    meterHelper->meterThread->createThread(meterHelper->meterInstance);
    mMeterHelperList.push_back(meterHelper);
    mMeterInstanceList.push_back(meterHelper->meterInstance);

    for(auto asyncInfo : meterInfo.triggersList) {
        asyncMeterMap[asyncInfo].emplace_back(meterHelper);
    }
    reset();
    return;
}

void MeterMgr::deleteMeterHelpers() {
    for(MeterHelper* meterHelper : mMeterHelperList) {
        if(meterHelper->meterThread) {
           meterHelper->meterThread->destroyThread();
           delete meterHelper->meterThread;
        }
        if(meterHelper->meterInstance) {
            delete meterHelper->meterInstance;
        }
        delete meterHelper;
    }
    mMeterHelperList.clear();
    mMeterInstanceList.clear();

    for(map<AsyncInfo,vector<MeterHelper*>,CompareAsyncInfo>::iterator iter=asyncMeterMap.begin(); iter!=asyncMeterMap.end(); iter++) {
        AsyncInfo asyncInfo = iter->first;
        vector<MeterHelper*> vectPtr = iter->second;
        vectPtr.clear();
    }
    asyncMeterMap.clear();
}

MeterMgr::~MeterMgr() {
    DEBUGV(LOG_TAG_MM,"MeterMgr Destructor");
    deleteMeterHelpers();
    primaryAsyncSet.clear();
    secondaryAsyncSet.clear();
    pthread_mutex_destroy(&meter_mutex);
}

void MeterMgr::notifyReceiver(Meter *m) {
    lock();
    returnedMeters++;
    DEBUGD(LOG_TAG_MM,"%s : (Id : total,started,returned) = (%u,%u,%u,%u) ",mFInfo.name.c_str(),mId,totalMeters,startedMeters,returnedMeters);
    if(startedMeters == returnedMeters) {
        if((getState() == INACTIVE)) {
            startedMeters = returnedMeters = 0;
            setState(IDLE);
            reset();
            DEBUGD(LOG_TAG_MM,"%s : (Id : total,started,returned,state) = (%u,%u,%u,%u,%d) ",
                mFInfo.name.c_str(),mId,totalMeters,startedMeters,returnedMeters,(int)getState());
            unlock();
            return;
        } else if(getState() == ACTIVE) {
            if(returnedMeters == totalMeters) {
                unlock();
                if(m && !m->hasStopNotification() && mMetaMeter) {
                    mMetaMeter->writeToDataBase(mMeterInstanceList);
                }
                lock();
                startedMeters = returnedMeters = 0;
                setState(IDLE);
                reset();
                DEBUGD(LOG_TAG_MM,"%s : (Id : total,started,returned,state) = (%u,%u,%u,%u,%d) ",
                    mFInfo.name.c_str(),mId,totalMeters,startedMeters,returnedMeters,(int)getState());
                unlock();
                return;
            }
        }
    }
    DEBUGD(LOG_TAG_MM,"%s : (Id : total,started,returned,state) = (%u,%u,%u,%u,%d) ",
        mFInfo.name.c_str(),mId,totalMeters,startedMeters,returnedMeters,(int)getState());
    unlock();
    return;
}

bool MeterMgr::isPrimaryASync(const AsyncTriggerPayload& asyncData) {
    AsyncInfo asyncInfo(false,asyncData.hintID, asyncData.hintType);
    if(primaryAsyncSet.find(asyncInfo)!=primaryAsyncSet.end()) {
        DEBUGV(LOG_TAG_MM,"(hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, 1);
        return true;
    }
    DEBUGV(LOG_TAG_MM,"(hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, 0);
    return false;
}

bool MeterMgr::isValidAsync(const AsyncTriggerPayload& asyncData) {
    AsyncInfo asyncInfo(false,asyncData.hintID, asyncData.hintType);
    DEBUGV(LOG_TAG_MM, "%s", asyncInfo.toString().c_str());
    if(primaryAsyncSet.find(asyncInfo)!=primaryAsyncSet.end()) {
        DEBUGV(LOG_TAG_MM,"(hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, 1);
        return true;
    } else if(secondaryAsyncSet.find(asyncInfo)!=secondaryAsyncSet.end()) {
        DEBUGV(LOG_TAG_MM,"(hintID,hintType,Primary) = (%u, %d, %d)", asyncInfo.hintID, asyncInfo.hintType, 0);
        return true;
    } else {
        DEBUGV(LOG_TAG_MM,"Invalid (hintID,hintType,appName,duration,appPID,appThreadTID) = (%u, %d,%s,%d,%u,%u)",
            asyncInfo.hintID, asyncInfo.hintType,asyncData.appName.c_str(), asyncData.duration, asyncData.appPID,asyncData.appThreadTID);
        return false;
    }

    return false;
}

void MeterMgr::startMeters(const AsyncTriggerPayload& asyncData , bool isPrimary) {
    map<AsyncInfo,vector<MeterHelper*>, CompareAsyncInfo>::iterator iter;
    AsyncInfo asyncInfo(false,asyncData.hintID, asyncData.hintType);
    iter = asyncMeterMap.find(asyncInfo);
    if(iter != asyncMeterMap.end()) {
        /*
        Note: Null checks are avoided as every entry in asyncMeterMap is valid.
        This is taken care in constructor. Refer to the function createMeterHelper() for all valid Null checks.
        */
        for(MeterHelper* meterHelper : (iter->second)) {
            DEBUGV(LOG_TAG_MM,"AsyncInfo:(hintID,hintType,Primary,MeterHelper,started)=(%u,%d,%d,%p,%d)",
                asyncInfo.hintID, asyncInfo.hintType, (int)isPrimary,meterHelper, (int)meterHelper->isStarted);

            meterHelper->meterInstance->asyncTriggerUpdate(asyncData);

            lock();
            if(!meterHelper->isStarted) {
                startedMeters++;
                meterHelper->isStarted = true;
                meterHelper->meterInstance->getMeterFwk()->wakeUp(NULL);
            }
            unlock();
        }
    }
}

void MeterMgr::stopMeters() {
    for(MeterHelper* meterHelper : mMeterHelperList) {
        /*
        Note: Null checks are avoided as every entry in mMeterHelperList is valid.
        This is taken care in constructor. Refer to the function createMeterHelper() for all valid Null checks.
        */
        meterHelper->meterInstance->asyncTriggerUpdate(AsyncTriggerPayload());
    }
}

void MeterMgr::reset() {
    for(MeterHelper* meterHelper : mMeterHelperList) {
        /*
        Note: Null checks are avoided as every entry in mMeterHelperList is valid.
        This is taken care in constructor. Refer to the function createMeterHelper() for all valid Null checks.
        */
        meterHelper->meterInstance->asyncTriggerUpdate(AsyncTriggerPayload());
        meterHelper->meterInstance->getAsyncUpdates().clear();
        meterHelper->isStarted = false;
    }
    startedMeters = 0;
    returnedMeters = 0;
}
bool MeterMgr::isReady() {
    static bool didMeterThreadsCreated = false;
    if(didMeterThreadsCreated) {
        return true;
    } else {
        for(MeterHelper* meterHelper : mMeterHelperList) {
            /*
            Note: Null checks are avoided as every entry in mMeterHelperList is valid.
            This is taken care in constructor. Refer to the function createMeterHelper() for all valid Null checks.
            */
            didMeterThreadsCreated = meterHelper->meterThread->isReady();
            if(!didMeterThreadsCreated) {
                break;
            }
        }
    }
    return didMeterThreadsCreated;
}
