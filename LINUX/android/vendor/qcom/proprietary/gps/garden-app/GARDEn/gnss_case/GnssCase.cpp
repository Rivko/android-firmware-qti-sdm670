/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GardenUtil.h"
#include "GnssCase.h"
#include "GnssLocationAPI.h"

#ifdef __ANDROID__
#include "HidlAPI.h"
#endif

namespace garden {

IGnssAPI* GnssCase::mGnssAPI = nullptr;
int GnssCase::mPrintNmea = 0;
int GnssCase::mSvDetails = 0;
int GnssCase::mMeasurements = 0;
uint64_t GnssCase::mTTFF = 0;
bool GnssCase::mRemoteAPI = false;

GnssCase::GnssCase() :
    mCb(nullptr),
    mTestFlag(GNSS_CASE_NONE),
    mTimeout(0),
    mMinSvs(0),
    mMinSnr(0.0),
    mRecurrence(0),
    mStartTime(0),
    mMsgTask(new MsgTask("Garden MsgTask", false)) {
}

GnssCase::~GnssCase() {
    if (mTimer.isStarted()) {
        mTimer.stop();
    }
    if (mMsgTask) {
        mMsgTask->destroy();
    }
}

GARDEN_RESULT GnssCase::preRun() {
    lock_guard guard(mLock);
    mTestFlag = GNSS_CASE_NONE;
    if (getGnssAPI() == nullptr) {
        GARDEN_ERROR("getGnssAPI() failed.");
    }
    if(!mCb) {
        mCb = new GnssCb(this);
        mGnssAPI->setGnssCbs(mCb);
    }
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssCase::postRun() {
    wait();
    if (mGnssAPI) {
        mGnssAPI->setGnssCbs(nullptr);
    }
    if(mCb) {
        delete mCb;
        mCb = nullptr;
    }
    return GARDEN_RESULT_PASSED;
}

void GnssCase::wait() {
    std::unique_lock<std::mutex> lck(mLock);
    // wait all test flags cleared
    if (mTestFlag != GNSS_CASE_NONE) {
        cv.wait(lck);
    }
}

void GnssCase::setSvInfo(int printNmea, int svDetails,
                         int measurements) {
    mPrintNmea = printNmea;
    mSvDetails = svDetails;
    mMeasurements = measurements;
}

void GnssCase::setStartParam(int timeout, int minSvs, float minSnr, uint32_t recurrence) {
    mTimeout = timeout;
    mMinSvs = minSvs;
    mMinSnr = minSnr;
    mRecurrence = recurrence;
}

void GnssCase::setStartParam(int timeout) {
    setStartParam(timeout, 0, 0.0, 0);
}

void GnssCase::setNiParam(int niCount, int niResPatCount,
        int* networkInitiatedResponse) {
    if (mCb != nullptr) {
        ((GnssCb*)mCb)->setNiParam(niCount, niResPatCount, networkInitiatedResponse);
    }
}

void GnssCase::sessionTimeout() {
    lock_guard guard(mLock);
    unsetFlag(GNSS_CASE_SESSION);
}

int GnssCase::gnssStart() {
    {
        lock_guard guard(mLock);
        if (mTimeout > 0) {
            setFlag(GNSS_CASE_SESSION);
            // this Runnable will be called from timer thread
            Runnable timerRunnable = [this] {
                sessionTimeout();
            };
            mTimer.set(mTimeout, timerRunnable);
            mTimer.start();
        }
        if (mMinSvs > 0 && mMinSnr > 0.0) {
            setFlag(GNSS_CASE_SESSION);
        }
    }

    mStartTime = getUpTimeSec();

    return getGnssAPI()->gnssStart();
}

int GnssCase::gnssStop() {
    {
        lock_guard guard(mLock);
        setFlag(GNSS_CASE_ENGINEOFF);
    }

    return getGnssAPI()->gnssStop();
}

void GnssCase::niBackToBack(int niCount, int niResPatCount,
        int* networkInitiatedResponse) {
    lock_guard guard(mLock);
    setNiParam(niCount, niResPatCount, networkInitiatedResponse);
    setFlag(GNSS_CASE_NIBACKTOBACK);
}

void GnssCase::sendMsg(LocMsg* msg) const {
    if (msg != nullptr) {
        mMsgTask->sendMsg(msg);
    }
}

void GnssCase::sendMsg(Runnable& runnable) const {
    mMsgTask->sendMsg(new LocMsgWrapper(runnable));
}

GARDEN_RESULT GnssCase::verifyMenuCase() {
    char buf[16], *p;
    mTimeout = 0;
    mMinSvs = 0;
    mMinSnr = 0;

    menuPrint("\nComplete test with SVs SNR instead of position? (y/n) (default n) :");
    fflush (stdout);
    p = fgets(buf, 16, stdin);
    if (p == nullptr) {
        GARDEN_ERROR("Error: fgets returned nullptr !!");
    }
    if (p[0] == 'y') {
        mMinSvs = 1;
        mMinSnr = 28;
        menuPrint("\nEnter minimum SVs needed to complete test? (default %d):", mMinSvs);
        fflush (stdout);
        p = fgets(buf, 16, stdin);
        if (p == nullptr) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }
        if (atoi(p) != 0)
            mMinSvs = atoi(p);
        menuPrint("\nEnter minimum SNR for SVs to complete test (default %f):", mMinSnr);
        fflush (stdout);
        p = fgets(buf, 16, stdin);
        if (p == nullptr) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }
        if (atof(p) != 0)
            mMinSnr = atof(p);
    }

    menuPrint("\nComplete test with timeout? (y/n) (default n) :");
    fflush (stdout);
    p = fgets(buf, 16, stdin);
    if (p == nullptr) {
        GARDEN_ERROR("Error: fgets returned nullptr !!");
    }
    if (p[0] == 'y') {
        mTimeout = 60;
        menuPrint("\nEnter timeout in second? (default %d):", mTimeout);
        fflush (stdout);
        p = fgets(buf, 16, stdin);
        if (p == nullptr) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }
        if (atoi(p) != 0) {
            mTimeout = atoi(p);
        }
    }

    menuPrint("\ncalling gps_start_tracking with StopOnMinSvs=%d StopOnMinSnr=%f Timeout=%d...\n",
              mMinSvs, mMinSnr, mTimeout);
    gnssStart();
    wait();
    gnssStop();

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssCase::positioningMenuCase() {
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop) {
        menuPrint("\n\n"
            "1: start tracking\n"
            "2: stop tracking\n"
            "b: back\n"
            "q: quit\n"
            "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }

        switch (p[0]) {
        case '1':
            getGnssAPI()->gnssStart();
            break;
        case '2':
            getGnssAPI()->gnssStop();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            return GARDEN_RESULT_ABORT;
            break;
        default:
            gardenPrint("\ninvalid command\n");
        }
    }

    return GARDEN_RESULT_PASSED;
}

IGnssAPI* GnssCase::getGnssAPI() {
    if(!mGnssAPI) {
        if (mRemoteAPI) {
            gardenPrint("create remote api.");
#ifdef __ANDROID__
            mGnssAPI = new HidlAPI();
#endif
            if (!mGnssAPI) {
                gardenPrint("failed to create remote api.");
            }
        }
        if (!mGnssAPI) {
            gardenPrint("create local api.");
            mGnssAPI = new GnssLocationAPI();
            if (!mGnssAPI) {
                gardenPrint("failed to create local api.");
            }
        }
    }
    return mGnssAPI;
}

void GnssCase::destroyGnssAPI() {
    if(mGnssAPI) {
        mGnssAPI->setGnssCbs(nullptr);
        delete mGnssAPI;
        mGnssAPI = nullptr;
    }
}

} // namespace garden
