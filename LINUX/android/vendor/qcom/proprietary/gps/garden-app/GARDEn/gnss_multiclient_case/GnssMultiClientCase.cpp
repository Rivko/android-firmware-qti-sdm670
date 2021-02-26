/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC"

#include <regex>
#include <cstdlib>
#include <fstream>
#include <utils/Log.h>
#include "GnssMultiClientCase.h"
#include "GnssMultiClientCaseUtils.h"

#include "GardenUtil.h"
#include "GnssLocationAPI.h"
#include "GnssMultiClientCb.h"

#include <string>
#include <sstream>
#include <iostream>

#include <dlfcn.h>
#include <signal.h>

namespace garden {

#define getUserInputEnterToContinue GmccUtils::get().getUserInputEnterToContinue
#define getUserInputInt GmccUtils::get().getUserInputInt
#define getUserInputMask64Bit GmccUtils::get().getUserInputMask64Bit
#define getUserInputString GmccUtils::get().getUserInputString
#define getUserInputYesNo GmccUtils::get().getUserInputYesNo
#define getUserInputSessionMode GmccUtils::get().getUserInputSessionMode
#define getUserInputTrackingOptions GmccUtils::get().getUserInputTrackingOptions
#define getUserInputLocClientIndex GmccUtils::get().getUserInputLocClientIndex
#define getUserInputGnssConfig GmccUtils::get().getUserInputGnssConfig
#define getUserInputGnssConfigFlagsMask GmccUtils::get().getUserInputGnssConfigFlagsMask
#define strUtilTokenize GmccUtils::get().strUtilTokenize

uint64_t GnssMultiClientCase::mTTFF;
bool GnssMultiClientCase::condPrintEnabled = true;

GnssMultiClientCase::GnssMultiClientCase() :
    mTestFlag(GNSS_CASE_NONE),
    mRecurrence(LOC_GPS_POSITION_RECURRENCE_PERIODIC),
    mStartTime(0),
    mMsgTask(new MsgTask("Garden MsgTask", false)),
    mActiveClientIndex(-1), gnssInterface(nullptr) {
}

GnssMultiClientCase::~GnssMultiClientCase() {
    if (mTimer.isStarted()) {
        mTimer.stop();
    }
    if (mMsgTask) {
        mMsgTask->destroy();
    }
}

GARDEN_RESULT GnssMultiClientCase::preRun() {
    lock_guard guard(mLock);
    mTestFlag = GNSS_CASE_NONE;
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::postRun() {
    wait();
    return GARDEN_RESULT_PASSED;
}

void GnssMultiClientCase::wait() {
    std::unique_lock<std::mutex> lck(mLock);
    // wait all test flags cleared
    if (GNSS_CASE_NONE != mTestFlag) {
        cv.wait(lck);
    }
}

void GnssMultiClientCase::sendMsg(LocMsg* msg) const {
    if (msg != nullptr) {
        mMsgTask->sendMsg(msg);
    }
}

void GnssMultiClientCase::sendMsg(Runnable& runnable) const {
    mMsgTask->sendMsg(new LocMsgWrapper(runnable));
}

GARDEN_RESULT GnssMultiClientCase::autoTest(std::string cmdLineArgs)
{
    PRINTLN("\n--------- [GnssMultiClientCase::cmdLineTest] --------\n"
            "Cmd line Arg string: \"%s\"", cmdLineArgs.c_str());

    std::vector<std::string> args = strUtilTokenize(cmdLineArgs);
    PRINTLN("Arg count: %lu\n", args.size());

    if (0 == args.size()) {
        PRINTERROR("No arguments specified");
        getUserInputEnterToContinue();
        return GARDEN_RESULT_FAILED;
    }

    if (0 == args[0].compare("help")) {
        cmdLineHelp(args);
    } else if (0 == args[0].compare("setblacklist")) {
        cmdLineSetBlacklist(args);
    } else if (0 == args[0].compare("getblacklist")) {
        cmdLineGetBlacklist(args);
    } else if (0 == args[0].compare("setconstellation")) {
        cmdLineSetConstellation(args);
    } else if (0 == args[0].compare("getconstellation")) {
        cmdLineGetConstellation(args);
    } else if (0 == args[0].compare("resetconstellation")) {
        cmdLineResetConstellation(args);
    } else {
        PRINTERROR("Invalid command %s", args[0].c_str());
        getUserInputEnterToContinue();
        return GARDEN_RESULT_FAILED;
    }

    getUserInputEnterToContinue();
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineHelp(const std::vector<std::string>& args)
{
    if (1 == args.size()) {

        PRINTLN("Available commands:");
        PRINTLN("- setblacklist");
        PRINTLN("- getblacklist");
        PRINTLN("- setconstellation");
        PRINTLN("- getconstellation");
        PRINTLN("- resetconstellation");
        PRINTLN("\nFor help on a specific cmd, \"help <command>\"");
    }

    if (args.size() > 1 && 0 == args[1].compare("setblacklist")) {

        PRINTLN("Set Blacklist cmd usage:");
        PRINTLN("setblacklist <constellation> <blacklist_mask>");
        PRINTLN("constellation values: gps / glo / gal / bds / qzss");
        PRINTLN("blacklist_mask value: hex value");
        PRINTLN("e.g.");
        PRINTLN("setblacklist gps 0x1 gal 0xFF bds 0x23");

    } else if (args.size() > 1 && 0 == args[1].compare("getblacklist")) {

        PRINTLN("Get Blacklist cmd usage:");
        PRINTLN("getblacklist");
        PRINTLN("Sends the Get Blacklist command to Modem and waits for response.");

    } else if (args.size() > 1 && 0 == args[1].compare("setconstellation")) {

        PRINTLN("Set Constellation cmd usage:");
        PRINTLN("setconstellation enabled <constellation> ... disabled <constellation> ...");
        PRINTLN("constellation values: gps / glo / gal / bds / qzss");
        PRINTLN("e.g.");
        PRINTLN("setconstellation enabed gal bds disabled qzss");

    } else if (args.size() > 1 && 0 == args[1].compare("getConstellation")) {

        PRINTLN("Get Constellation cmd usage:");
        PRINTLN("getconstellation");
        PRINTLN("Sends the Get Constellation command to Modem and waits for response.");

    } else if (args.size() > 1 && 0 == args[1].compare("resetConstellation")) {

        PRINTLN("Reset Constellation cmd usage:");
        PRINTLN("resetconstellation");
        PRINTLN("Sends the Reset Constellation command to Modem.");

    } else {

        PRINTERROR("Invalid command %s", args[1].c_str());
        return GARDEN_RESULT_FAILED;
    }

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineSetBlacklist(const std::vector<std::string>& args)
{
    uint64_t gpsMask = 0;
    uint64_t galMask = 0;
    uint64_t gloMask = 0;
    uint64_t bdsMask = 0;
    uint64_t qzssMask = 0;
    uint64_t* mask = nullptr;

    bool isNextArgConstellation = true;
    for (int i=1; i<(int)args.size(); i++) {
        if (isNextArgConstellation) {
            // Parse constellation name
            if (0 == args[i].compare("gps")) {
                mask = &gpsMask;
            } else if (0 == args[i].compare("gal")) {
                mask = &galMask;
            } else if (0 == args[i].compare("glo")) {
                mask = &gloMask;
            } else if (0 == args[i].compare("bds")) {
                mask = &bdsMask;
            } else if (0 == args[i].compare("qzss")) {
                mask = &qzssMask;
            } else {
                PRINTERROR("Invalid constellation name: [%s]", args[i].c_str());
                getUserInputEnterToContinue();
                return GARDEN_RESULT_FAILED;
            }
            isNextArgConstellation = false;
        } else {
            // Parse mask value
            std::stringstream inputStream(args[i]);
            if (!(inputStream >> std::hex >> (*mask))) {
                PRINTERROR("Failed to parse mask value [%s]", args[i].c_str());
                getUserInputEnterToContinue();
                return GARDEN_RESULT_FAILED;
            }
            isNextArgConstellation = true;
        }
    }

    // Now we have all mask values
    PRINTLN("Set Blacklist Masks specified:");
    PRINTLN("GPS: 0x%lx", gpsMask);
    PRINTLN("GAL: 0x%lx", galMask);
    PRINTLN("GLO: 0x%lx", gloMask);
    PRINTLN("BDS: 0x%lx", bdsMask);
    PRINTLN("QZSS: 0x%lx\n", qzssMask);

    // Create control client
    PRINTLN("Creating Control Client..");
    createLocationControlAPIClient();

    // Validate control client ptr
    if (mLocControlClientInfo.clientPtr == nullptr ||
            mLocControlClientInfo.clientPtr->mControlClient == nullptr) {
        PRINTERROR("Control client ptr NULL");
        getUserInputEnterToContinue();
        return GARDEN_RESULT_FAILED;
    }

    // Create config
    PRINTLN("\nCreating GnssConfig for SV blacklist..");
    GnssConfig config = {};
    PRINTLN("TBD");

    // Send command to control api
    PRINTLN("\nSubmitting the specified config to Control API..");
    mLocControlClientInfo.clientPtr->mControlClient->locAPIGnssUpdateConfig(config);

    // Clean up control api
    PRINTLN("\nDeleting Location control api instance..");
    destroyLocationControlAPIClient();

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineGetBlacklist(const std::vector<std::string>& /*args*/)
{
    // Create control client
    PRINTLN("Creating Control Client..");
    createLocationControlAPIClient();

    // Validate control client ptr
    if (nullptr == mLocControlClientInfo.clientPtr ||
            nullptr == mLocControlClientInfo.clientPtr->mControlClient) {
        PRINTERROR("Control client ptr NULL");
        getUserInputEnterToContinue();
        return GARDEN_RESULT_FAILED;
    }

    PRINTLN("TBD");
    destroyLocationControlAPIClient();

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineSetConstellation(const std::vector<std::string>& /*args*/)
{
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineGetConstellation(const std::vector<std::string>& /*args*/)
{
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT
GnssMultiClientCase::cmdLineResetConstellation(const std::vector<std::string>& /*args*/)
{
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::menuTest()
{
    bool exit_loop = false;
    GARDEN_RESULT result = GARDEN_RESULT_INVALID;

    setupSigQuitForLoggingToggle();

    while(!exit_loop) {
        PRINTLN("\n\n"
                "1: Create a Location API Client \n"
                "2: Create a Location Control API Client \n"
                "3: List all Location API Clients \n"
                "4: Execute Location API \n"
                "5: Execute Location Control API \n"
                "6: Execute API directly on GNSS Interface \n"
                "7: Destroy Location API Client Instance \n"
                "8: Destroy Location Control API Client Instance \n"
                "9: HIDL API Validation \n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            result = createLocationAPIClient();
            break;
        case 2:
            result = createLocationControlAPIClient();
            break;
        case 3:
            result = listLocationAPIClients();
            break;
        case 4:
            result = executeLocationAPI();
            break;
        case 5:
            result = executeLocationControlAPI();
            break;
        case 6:
            result = executeGnssIfaceAPI();
            break;
        case 7:
            result = destroyLocationAPIClient();
            break;
        case 8:
            result = destroyLocationControlAPIClient();
            break;
        case 9:
            PRINTLN("TBD");
            break;
        case 99:
            continue;
        case 0:
            result = GARDEN_RESULT_ABORT;
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", result);
            getUserInputEnterToContinue();
        }
    }

    return result;
}

GARDEN_RESULT GnssMultiClientCase::createLocationAPIClient()
{
    GnssMultiClientLocationAPIClient* client =
            new GnssMultiClientLocationAPIClient();
    GnssMultiClientCb* cb = new GnssMultiClientCb(this);
    client->setCallbacks(cb);

    LocationAPIClientInfo clientInfo = {};
    clientInfo.size = sizeof(LocationAPIClientInfo);
    clientInfo.isControlClient = false;
    clientInfo.clientPtr = client;
    clientInfo.cbPtr = cb;

    mLocClientList.push_back(clientInfo);
    PRINTLN("Added location API client at index: %lu",
                mLocClientList.size()-1);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::createLocationControlAPIClient()
{
    GnssMultiClientLocationAPIClient* client =
            new GnssMultiClientLocationAPIClient();
    int res = client->createControlClient();
    if (0 != res) {
        PRINTERROR("Failed to create control client, "
                "is there an existing instance already ?");
        delete client;
        return GARDEN_RESULT_FAILED;
    } else {
        PRINTLN("Created Control client %p", client);
    }

    GnssMultiClientCb* cb = new GnssMultiClientCb(this);
    client->setCallbacks(cb);

    mLocControlClientInfo.size = sizeof(LocationAPIClientInfo);
    mLocControlClientInfo.isControlClient = true;
    mLocControlClientInfo.clientPtr = client;
    mLocControlClientInfo.cbPtr = cb;

    PRINTLN("Created location control API instance");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::executeLocationAPI()
{
    bool exit_loop = false;
    GARDEN_RESULT result = GARDEN_RESULT_INVALID;

    int clientIndex = getUserInputLocClientIndex(
            "Which location API instance to use for API execution ?",
            mLocClientList.size());
    if (clientIndex < 0) {
        PRINTLN("Failed to fetch active client index !");
        return GARDEN_RESULT_FAILED;
    }
    mActiveClientIndex = clientIndex;

    while(!exit_loop) {
        PRINTLN("\n\nWhich API would you like to execute:\n\n"
                "1: Start Tracking \n"
                "2: Stop Tracking \n"
                "3: Update Tracking Options \n"
                "4: Start Batching \n"
                "5: Stop Batching \n"
                "6: Update Batching Options \n"
                "7: Get Batched Locations \n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            result = locAPIStartTracking();
            break;
        case 2:
            result = locAPIStopTracking();
            break;
        case 3:
            result = locAPIUpdateTrackingOptions();
            break;
        case 4:
            result = locAPIStartBatching();
            break;
        case 5:
            result = locAPIStopBatching();
            break;
        case 6:
            result = locAPIUpdateBatchingOptions();
            break;
        case 7:
            result = locAPIGetBatchedLocations();
            break;
        case 99:
            continue;
        case 0:
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", result);
            getUserInputEnterToContinue();
        }
    }

    return result;
}

GARDEN_RESULT GnssMultiClientCase::executeLocationControlAPI()
{
    bool exit_loop = false;
    GARDEN_RESULT result = GARDEN_RESULT_INVALID;

    while(!exit_loop) {
        PRINTLN("\n\nWhich Control API would you like to execute:\n\n"
                "1: Enable \n"
                "2: Disable \n"
                "3: Update Config \n"
                "4: Get Config \n"
                "5: Delete Aiding Data \n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            result = locControlAPIEnable();
            break;
        case 2:
            result = locControlAPIDisable();
            break;
        case 3:
            result = locControlAPIUpdateConfig();
            break;
        case 4:
            result = locControlAPIGetConfig();
            break;
        case 5:
            result = locControlAPIDeleteAidingData();
            break;
        case 99:
            continue;
        case 0:
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", result);
            getUserInputEnterToContinue();
        }
    }

    return result;
}

GARDEN_RESULT GnssMultiClientCase::executeGnssIfaceAPI()
{
    bool exit_loop = false;
    GARDEN_RESULT result = GARDEN_RESULT_INVALID;

    while(!exit_loop) {
        PRINTLN("\n\nWhich GNSS Iface API would you like to execute:\n\n"
                "1: Update SV Type (Constellation) Config \n"
                "2: Get SV Type (Constellation) Config \n"
                "3: Reset SV Type (Constellation) Config \n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            result = gnssIfaceAPIUpdateSvTypeConfig();
            break;
        case 2:
            result = gnssIfaceAPIGetSvTypeConfig();
            break;
        case 3:
            result = gnssIfaceAPIResetSvTypeConfig();
            break;
        case 99:
            continue;
        case 0:
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", result);
            getUserInputEnterToContinue();
        }
    }

    return result;
}

GARDEN_RESULT GnssMultiClientCase::listLocationAPIClients()
{
    PRINTLN("Location API Clients count: %lu", mLocClientList.size());
    PRINTLN("Location Control API Client created: %d",
            (mLocControlClientInfo.clientPtr == nullptr)? 0:1 );

    for (int i=0; i < (int)mLocClientList.size(); i++) {
        LocationAPIClientInfo info = mLocClientList[i];
        PRINTLN("Location API Client [%d] client ptr %p cb ptr %p isControlClient %d",
                i, info.clientPtr, info.cbPtr, info.isControlClient);
    }

    PRINTLN("Location Control API Client client ptr %p cb ptr %p isControlClient %d",
            mLocControlClientInfo.clientPtr, mLocControlClientInfo.cbPtr,
            mLocControlClientInfo.isControlClient);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::destroyLocationAPIClient()
{
    int clientIndex = getUserInputLocClientIndex(
            "Which Location API instance to destroy ?", mLocClientList.size());
    if (clientIndex < 0) {
        PRINTERROR("Failed to fetch active client index !");
        return GARDEN_RESULT_FAILED;
    }

    // LocationAPIClientInfo info = mLocClientList[clientIndex];

    // todo : info.clientPtr->destroy
    // todo : delete info.clientPtr / info.cbPtr

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::destroyLocationControlAPIClient()
{
    // todo : info.clientPtr->destroy
    // todo : delete info.clientPtr / info.cbPtr

    return GARDEN_RESULT_PASSED;
}

// Location APIs
GARDEN_RESULT GnssMultiClientCase::locAPIStartTracking()
{
    // validate index
    if (mActiveClientIndex < 0) {
        PRINTLN("Select an active client first.");
        return GARDEN_RESULT_FAILED;
    }

    // Client info
    LocationAPIClientInfo info = mLocClientList[mActiveClientIndex];

    // Get Tracking Options
    LocationOptions options = getUserInputTrackingOptions(
            "Please specify TrackingOptions for tracking session.");

    PRINTLN("Got below tracking options:\n"
            "minInterval %d, minDistance %d, mode %d\n",
            options.LocationOptions::minInterval,
            options.LocationOptions::minDistance,
            options.LocationOptions::mode);

    getUserInputEnterToContinue();

    // Start tracking
    info.clientPtr->locAPIStartTracking(options);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIStopTracking()
{
    // validate index
    if (mActiveClientIndex < 0) {
        PRINTLN("Select an active client first.");
        return GARDEN_RESULT_FAILED;
    }

    // Client info
    LocationAPIClientInfo info = mLocClientList[mActiveClientIndex];
    info.clientPtr->locAPIStopTracking();

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIUpdateTrackingOptions()
{
    // validate index
    if (mActiveClientIndex < 0) {
        PRINTLN("Select an active client first.");
        return GARDEN_RESULT_FAILED;
    }

    // Client info
    LocationAPIClientInfo info = mLocClientList[mActiveClientIndex];

    // Get Tracking Options
    LocationOptions options = getUserInputTrackingOptions(
            "Please specify new TrackingOptions for the tracking session.");

    PRINTLN("Got below tracking options:\n"
            "minInterval %d, minDistance %d, mode %d\n",
            options.LocationOptions::minInterval,
            options.LocationOptions::minDistance,
            options.LocationOptions::mode);

    getUserInputEnterToContinue();

    // Start tracking
    info.clientPtr->locAPIUpdateTrackingOptions(options);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIStartBatching()
{
    // validate index
    if (mActiveClientIndex < 0) {
        PRINTLN("Select an active client first.");
        return GARDEN_RESULT_FAILED;
    }

    // Client info
    LocationAPIClientInfo info = mLocClientList[mActiveClientIndex];

    // Fetch Session ID
    uint32_t sessionId = (uint32_t)getUserInputInt("Enter Session ID for batching session to start: ");

    // Fetch Session Type
    SESSION_MODE sessionType = getUserInputSessionMode("Enter Batching Session Type: ");

    // Get Tracking Options
    LocationOptions options = getUserInputTrackingOptions(
            "Please specify options for batching session.");

    info.clientPtr->locAPIStartSession(sessionId, sessionType, options);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIStopBatching()
{
    // validate index
    if (mActiveClientIndex < 0) {
        PRINTLN("Select an active client first.");
        return GARDEN_RESULT_FAILED;
    }

    // Client info
    LocationAPIClientInfo info = mLocClientList[mActiveClientIndex];

    // Fetch Session ID
    uint32_t sessionId = (uint32_t)getUserInputInt("Enter Session ID for batching session to stop: ");

    info.clientPtr->locAPIStopSession(sessionId);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIUpdateBatchingOptions()
{
    // todo : complete
    PRINTLN("TBD");
    return GARDEN_RESULT_FAILED;
}

GARDEN_RESULT GnssMultiClientCase::locAPIGetBatchedLocations()
{
    // todo : complete
    PRINTLN("TBD");
    return GARDEN_RESULT_FAILED;
}

// Location Control APIs
GARDEN_RESULT GnssMultiClientCase::locControlAPIEnable()
{
    // todo : complete
    PRINTLN("TBD");
    return GARDEN_RESULT_FAILED;
}

GARDEN_RESULT GnssMultiClientCase::locControlAPIDisable()
{
    // todo : complete
    PRINTLN("TBD");
    return GARDEN_RESULT_FAILED;
}

GARDEN_RESULT GnssMultiClientCase::locControlAPIUpdateConfig()
{
    // Validate control client ptr
    if (mLocControlClientInfo.clientPtr == nullptr ||
            mLocControlClientInfo.clientPtr->mControlClient == nullptr) {
        PRINTERROR("Control client ptr NULL");
        return GARDEN_RESULT_FAILED;
    }

    // Fetch config to update
    GnssConfig config = getUserInputGnssConfig(
            "Specify the GNSS Config to send to Control API.");

    // Send command to control api
    PRINTLN("Submitting the specified config to Control API..");
    mLocControlClientInfo.clientPtr->mControlClient->locAPIGnssUpdateConfig(config);

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locControlAPIGetConfig()
{
    // Validate control client ptr
    if (nullptr == mLocControlClientInfo.clientPtr ||
            nullptr == mLocControlClientInfo.clientPtr->mControlClient) {
        PRINTERROR("Control client ptr NULL");
        return GARDEN_RESULT_FAILED;
    }

    // Fetch config mask

    // Send command to control api
    PRINTLN("Fetching the GNSS Config");
    PRINTLN("TBD");

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::locControlAPIDeleteAidingData()
{
    // todo : complete
    PRINTLN("TBD");
    return GARDEN_RESULT_FAILED;
}

void*
GnssMultiClientCase::loadLocationInterface(const char* library, const char* name)
{
    PRINTLN("Loading %s::%s ...", library, name);
    if (NULL == library || NULL == name) {
        return NULL;
    }

    typedef void* (getLocationInterface)();
    getLocationInterface* getter = NULL;
    const char *error = NULL;
    dlerror();
    void *handle = dlopen(library, RTLD_NOW);
    if (NULL == handle) {
        PRINTERROR("dlopen for %s failed", library);
    } else if (NULL != (error = dlerror())) {
        PRINTERROR("dlopen for %s failed, error = %s", library, error);
    } else {
        getter = (getLocationInterface*)dlsym(handle, name);
        if ((error = dlerror()) != NULL)  {
            PRINTERROR("dlsym for %s::%s failed, error = %s", library, name, error);
            getter = NULL;
        }
    }

    if (NULL == getter) {
        return NULL;
    } else {
        return (*getter)();
    }
}

GARDEN_RESULT GnssMultiClientCase::gnssIfaceAPIUpdateSvTypeConfig()
{
    PRINTLN("TBD");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::gnssIfaceAPIGetSvTypeConfig()
{
    PRINTLN("TBD");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GnssMultiClientCase::gnssIfaceAPIResetSvTypeConfig()
{
    PRINTLN("TBD");
    return GARDEN_RESULT_PASSED;
}

extern "C" {
void GMCC_sigQuitHandler(int /*sig*/)
{
    if (GnssMultiClientCase::condPrintEnabled) {
        GnssMultiClientCase::condPrintEnabled = false;
    } else {
        GnssMultiClientCase::condPrintEnabled = true;
    }
    PRINTLN("\n\n[ Toggled Verbose Logging to %s ]\n",
            GnssMultiClientCase::condPrintEnabled? "ENABLED":"DISABLED");
}
}

void GnssMultiClientCase::setupSigQuitForLoggingToggle()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = GMCC_sigQuitHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGQUIT, &sigIntHandler, NULL);
}

// This test case will load commands from given file, which can be logcat output,
// and execute the corresponding command at the given time (relatively).

// Format of input file:
// startTrack (1, 1, 1000, 0)
// startTrack (2, 1, 1000, 0)
// updateTrack (1, 2, 2000, 5)
// stopTrack (1)
// stopTrack (2)

GARDEN_RESULT GnssMultiClientCase::loadFromFileCase(std::string file) {
    uint64_t startTime = 0;
    uint64_t startUpTime = 0;
    int index = -1;

    std::ifstream in(file);
    std::string s;
    while(getline(in, s)) {
        const std::regex pt(" *(.*Track.*)\\x28(.*)\\x29[\\s\\S]*");
        std:: match_results<std::string::const_iterator> result;
        bool valid = std::regex_match(s, result, pt);
        if (valid) {
            std::string func = result[1];
            std::string args = result[2];
            gardenPrint(" %s %s", func.c_str(), args.c_str());
            std::string pattern;
            std::regex reg;
            std::match_results<std::string::const_iterator> param;
            if (std::strstr(func.c_str(), "startTrack") != nullptr) {
                pattern = "(.*) *, *(.*) *, *(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if (valid) {
                    std::string v1 = param[1]; //session Id
                    std::string v2 = param[2]; //session Mode
                    std::string v3 = param[3]; //minInterval
                    std::string v4 = param[4]; //minDistance
                    LocationOptions options;
                    memset(&options, 0, sizeof(LocationOptions));
                    options.size = sizeof(LocationOptions);
                    options.mode = static_cast<GnssSuplMode>(atoi(v2.c_str()));
                    options.minInterval = atoi(v3.c_str());
                    options.minDistance = atoi(v4.c_str());
                    createLocationAPIClient();
                    mLocClientList[atoi(v1.c_str())-1].clientPtr->locAPIStartTracking(options);
                }
            } else if (std::strstr(func.c_str(), "updateTrack") != nullptr) {
                pattern = "(.*) *, *(.*) *, *(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if (valid) {
                    std::string v1 = param[1]; //session Id
                    std::string v2 = param[2]; //session Mode
                    std::string v3 = param[3]; //minInterval
                    std::string v4 = param[4]; //minDistance
                    LocationOptions options;
                    memset(&options, 0, sizeof(LocationOptions));
                    options.size = sizeof(LocationOptions);
                    options.mode = static_cast<GnssSuplMode>(atoi(v2.c_str()));
                    options.minInterval = atoi(v3.c_str());
                    options.minDistance = atoi(v4.c_str());
                    mLocClientList[atoi(v1.c_str())-1].clientPtr->locAPIUpdateTrackingOptions( \
                            options);
                }
            } else if (std::strstr(func.c_str(), "stopTrack") != nullptr) {
                mLocClientList[atoi(args.c_str())-1].clientPtr->locAPIStopTracking();
            }
        }
    }
    return GARDEN_RESULT_PASSED;
}
} // namespace garden
