/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_CASE_H
#define GNSS_MULTI_CLIENT_CASE_H

#include <string>
#include <LocTimer.h>
#include <MsgTask.h>
#include "IGardenCase.h"
#include "IGnssAPI.h"
#include "GnssCase.h"
#include "GnssCbBase.h"
#include "GnssMultiClientLocationAPIClient.h"
#include "GnssMultiClientCb.h"

namespace garden {

class GnssMultiClientCb;

/* Each flag representing a wait condition */
typedef enum {
    GNSS_MULTI_CLIENT_CASE_NONE        = 0,
    GNSS_MULTI_CLIENT_CASE_ALL         = (~0L),
} GNSS_MULTI_CLIENT_CASE_FLAG;

/* Info about each location api client */
typedef struct {
    size_t size;
    GnssMultiClientLocationAPIClient* clientPtr;
    bool isControlClient;
    GnssMultiClientCb* cbPtr;
} LocationAPIClientInfo;

class GnssMultiClientCase : public IGardenCase{

    friend class GnssMultiClientCb;

public:
    GnssMultiClientCase();
    virtual ~GnssMultiClientCase();

    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();

    GARDEN_RESULT menuTest();
    GARDEN_RESULT autoTest(std::string cmdLineArgs);
    GARDEN_RESULT loadFromFileCase(std::string file);

    GARDEN_RESULT cmdLineHelp(const std::vector<std::string>& args);
    GARDEN_RESULT cmdLineSetBlacklist(const std::vector<std::string>& args);
    GARDEN_RESULT cmdLineGetBlacklist(const std::vector<std::string>& args);
    GARDEN_RESULT cmdLineSetConstellation(const std::vector<std::string>& args);
    GARDEN_RESULT cmdLineGetConstellation(const std::vector<std::string>& args);
    GARDEN_RESULT cmdLineResetConstellation(const std::vector<std::string>& args);

    GARDEN_RESULT createLocationAPIClient();
    GARDEN_RESULT createLocationControlAPIClient();
    GARDEN_RESULT listLocationAPIClients();
    GARDEN_RESULT executeLocationAPI();
    GARDEN_RESULT executeLocationControlAPI();
    GARDEN_RESULT executeGnssIfaceAPI();
    GARDEN_RESULT destroyLocationAPIClient();
    GARDEN_RESULT destroyLocationControlAPIClient();

    void wait();

    void sendMsg(LocMsg* msg) const;
    void sendMsg(Runnable& runnable) const;

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
    static bool condPrintEnabled;

private:
    int mTestFlag;
    uint32_t mRecurrence;
    uint64_t mStartTime;
    GardenTimer mTimer;
    MsgTask* mMsgTask;
    int mActiveClientIndex;

    static uint64_t mTTFF;

    typedef std::vector<LocationAPIClientInfo> LocationAPIClientInfoList;
    LocationAPIClientInfoList mLocClientList;
    LocationAPIClientInfo mLocControlClientInfo;

    // For invoking APIs directly on GnssAdapter
    GnssInterface* gnssInterface;

private:
    // Location APIs
    GARDEN_RESULT locAPIStartTracking();
    GARDEN_RESULT locAPIStopTracking();
    GARDEN_RESULT locAPIUpdateTrackingOptions();
    GARDEN_RESULT locAPIStartBatching();
    GARDEN_RESULT locAPIStopBatching();
    GARDEN_RESULT locAPIUpdateBatchingOptions();
    GARDEN_RESULT locAPIGetBatchedLocations();

    // Location Control APIs
    GARDEN_RESULT locControlAPIEnable();
    GARDEN_RESULT locControlAPIDisable();
    GARDEN_RESULT locControlAPIUpdateConfig();
    GARDEN_RESULT locControlAPIGetConfig();
    GARDEN_RESULT locControlAPIDeleteAidingData();

    // Gnss Iface APIs
    GARDEN_RESULT gnssIfaceAPIUpdateSvTypeConfig();
    GARDEN_RESULT gnssIfaceAPIGetSvTypeConfig();
    GARDEN_RESULT gnssIfaceAPIResetSvTypeConfig();
    static void* loadLocationInterface(const char* library, const char* name);

    // SIGNAL utils
    void setupSigQuitForLoggingToggle();
};

} // namespace garden
#endif // GNSS_MULTI_CLIENT_CASE_H
