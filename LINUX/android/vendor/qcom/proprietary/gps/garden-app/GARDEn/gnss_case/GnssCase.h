/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSS_CASE_H
#define GNSS_CASE_H

#include <condition_variable>
#include <LocTimer.h>
#include <MsgTask.h>
#include "IGardenCase.h"
#include "IGnssAPI.h"
#include "GnssCb.h"
#include "GardenUtil.h"

namespace garden {
#define LAT 32.896535
#define LONG -117.201025
#define ACCU 50

typedef enum {
    GNSS_CASE_NONE            = 0,
    GNSS_CASE_SESSION         = (1 << 0),
    GNSS_CASE_TRACKING        = (1 << 1),
    GNSS_CASE_NIBACKTOBACK    = (1 << 2),
    GNSS_CASE_ENGINEOFF       = (1 << 3),
    GNSS_CASE_FOREVER         = (1 << 4),
    GNSS_CASE_MAX             = (1 << 5),
    GNSS_CASE_ALL             = (~0L),
} GNSS_CASE_FLAG;

using Runnable = std::function<void()>;
class LocMsgWrapper : public LocMsg {
public:
    LocMsgWrapper(Runnable& runnable) : mRunnable(move(runnable)) {
    }

    virtual void proc() const {
        mRunnable();
    }
private:
    Runnable mRunnable;
};

class GardenTimer : public LocTimer {
public:
    GardenTimer() : LocTimer(), mStarted(false) {}
    inline ~GardenTimer() { stop(); }
    inline void set(const time_t waitTimeSec, const Runnable& runable) {
        mWaitTimeInMs = waitTimeSec * 1000;
        mRunnable = runable;
    }
    inline void start() {
        mStarted = true;
        LocTimer::start(mWaitTimeInMs, false);
    }
    inline void start(const time_t waitTimeSec) {
        mWaitTimeInMs = waitTimeSec * 1000;
        start();
    }
    inline void stop() {
        if (mStarted) {
            LocTimer::stop();
            mStarted = false;
        }
    }
    inline void restart() { stop(); start(); }
    inline void restart(const time_t waitTimeSec) { stop(); start(waitTimeSec); }
    inline bool isStarted() { return mStarted; }

private:
    // Override
    inline virtual void timeOutCallback() override {
        mStarted = false;
        mRunnable();
    }

private:
    time_t mWaitTimeInMs;
    Runnable mRunnable;
    bool mStarted;

};

class GnssCase : public IGardenCase{
    friend class GnssCb;
public:
    GnssCase();
    virtual ~GnssCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();

    void wait();
    void setStartParam(int timeout, int minSvs, float minSnr, uint32_t recurrence);
    void setStartParam(int timeout);

    void setNiParam(int niCount, int niResPatCount,
            int* networkInitiatedResponse);
    static void setSvInfo(int printNmea, int svDetails, int measurements);
    void sessionTimeout();
    int gnssStart();
    int gnssStop();
    void niBackToBack(int niCount, int niResPatCount, int* networkInitiatedResponse);
    inline uint64_t getTTFF() const { return mTTFF; }
    void sendMsg(LocMsg* msg) const;
    void sendMsg(Runnable& runnable) const;

    GARDEN_RESULT verifyMenuCase();
    GARDEN_RESULT positioningMenuCase();

    static void setAPIMode(bool remoteAPI) { mRemoteAPI = remoteAPI; }
    static IGnssAPI* getGnssAPI();
    static void destroyGnssAPI();

    void setFlag(int flag) {
        mTestFlag |= flag;
    }

    bool hasFlag(int flag) {
        return mTestFlag & flag;
    }

    void unsetFlag(int flag) {
        if (!hasFlag(flag)) return;
        mTestFlag &= (~flag);
        if (!mTestFlag) {
            cv.notify_one();
        }
    }

public:
    mutex mLock;
    std::condition_variable cv;
    static IGnssAPI* mGnssAPI;
protected:
    GnssCbBase* mCb;
private:
    int mTestFlag;
    int mTimeout;
    size_t mMinSvs;
    float mMinSnr;
    uint32_t mRecurrence;
    uint64_t mStartTime;
    GardenTimer mTimer;
    MsgTask* mMsgTask;
    static int mPrintNmea;
    static int mSvDetails;
    static int mMeasurements;
    static uint64_t mTTFF;
    static bool mRemoteAPI;
};

} // namespace garden
#endif // GNSS_CASE_H
