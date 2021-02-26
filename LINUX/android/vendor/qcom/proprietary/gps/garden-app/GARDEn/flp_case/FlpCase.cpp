/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <regex>
#include <cstdlib>
#include <fstream>
#include "FlpCase.h"
#include "GardenUtil.h"
#include "FlpCb.h"
#include <GardenFramework.h>

namespace garden {
//Minimum time period in seconds
#define MIN_PERIOD 1

GARDEN_RESULT FlpCase::preRun() {
    if (getGnssAPI() == nullptr) {
        GARDEN_ERROR("getGnssAPI() failed.");
    }
    if(!mCb) {
        mCb = new FlpCb(this);
        mGnssAPI->setFlpCbs(mCb);
    }
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT FlpCase::postRun() {
    wait();
    if (mGnssAPI) {
        mGnssAPI->setFlpCbs(nullptr);
    }
    if(mCb) {
        delete mCb;
        mCb = nullptr;
    }
    return GARDEN_RESULT_PASSED;
}

void FlpCase::getBatchSize() {
    int batchSize = 0;
    batchSize = getGnssAPI()->flpGetBatchSize();
    menuPrint("\ngetBatchSize returned %d", batchSize);
}

void FlpCase::startBatching() {
    char buf[16], *p;
    LocationOptions options;
    memset(&options, 0, sizeof(LocationOptions));
    options.size = sizeof(LocationOptions);
    options.minInterval = 0;
    options.minDistance = 0;
    uint32_t mode = 0;
    uint32_t id = 1;

    menuPrint("\nEnter Session Id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) > 1) {
        id = atoi(p);
    }

    menuPrint ("\n"
               "1: report locations ON FULL buffer\n"
               "2: report locations on TRIP completed\n"
               "3: report locations LIVE ON FIX\n"
               "4: no automatic location reporting\n"
               "Enter Command:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }

    if (p[0] == '1') {
        mode = SESSION_MODE_ON_FULL;
    } else if (p[0] == '2') {
        mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else if (p[0] == '3') {
        mode = SESSION_MODE_ON_FIX;
    }

    menuPrint("\nEnter period in seconds:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    int period = atoi(p);
    menuPrint("\nseconds entered = %i:", period);
    fflush (stdout);
    period = std::max(period, MIN_PERIOD);
    options.minInterval = period * 1000;

    menuPrint("\nEnter Smallest Displacement in meters (default 0):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atof(p) != 0) {
        options.minDistance = atof(p);
    }
    menuPrint("\ncalling flpStartSession with id=%u mode=%i minInterval=%d minDistance=%d...\n",
            id, mode, options.minInterval, options.minDistance);
    uint32_t ret = 0;
    fflush (stdout);
    ret = getGnssAPI()->flpStartSession(mode, options, id);
    if (ret == 0) {
        gardenPrint("Waiting on Callback...\n");
    } else {
        gardenPrint("Failed\n");
    }
}

void FlpCase::startFlpSession(uint32_t mode, LocationOptions &options, int timeout) {
    // Starts FLP session for autocase and stops it after timeout.
    uint32_t ret = 0;
    GardenTimer mTimer;
    ret = getGnssAPI()->flpStartSession(mode, options);
    if (0 != ret) {
        gardenPrint("flpStartSession Failed\n");
    }
    if (0 == ret) {
#ifdef USE_GLIB
        // In LE, push the phone settings after starting flp session
        sendMsg((LocMsg*)GARDEN_GET_PHONE_CONTEXT_MSG());
#endif
        Runnable timerRunnable = [this] {
            //stop flp session when timer expires
            int rc = 0;
            rc = getGnssAPI()->flpStopSession();
            gardenPrint ("stop Flp interface returned %d", rc);
            if (rc != 0) {
                gardenPrint("flpStopSession Failed\n");
            }
        };
        // set timer for flp session to run
        mTimer.set(timeout, timerRunnable);
        mTimer.start();
        gardenPrint("Waiting %dsecs for stop batching callback for flp session", timeout);
    } else {
        gardenPrint("flpStartSession Failed\n");
    }
}

void FlpCase::updateBatching() {
    char buf[16], *p;
    LocationOptions options;
    memset(&options, 0, sizeof(LocationOptions));
    options.size = sizeof(LocationOptions);
    options.minInterval = 0;
    options.minDistance = 0;
    uint32_t mode = 0;
    uint32_t id = 1;

    menuPrint("\nEnter Session Id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) > 1) {
        id = atoi(p);
    }

    menuPrint ("\n"
               "1: report locations ON FULL buffer\n"
               "2: report locations on TRIP completed\n"
               "3: report locations LIVE ON FIX\n"
               "4: no automatic location reporting\n"
               "Enter Command:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }

    if (p[0] == '1') {
        mode = SESSION_MODE_ON_FULL;
    } else if (p[0] == '2') {
        mode = SESSION_MODE_ON_TRIP_COMPLETED;
    } else if (p[0] == '3') {
        mode = SESSION_MODE_ON_FIX;
    }

    menuPrint("\nEnter period in seconds:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    int period = atoi(p);
    menuPrint("\nseconds entered = %i:", period);
    fflush (stdout);
    period = std::max(period, MIN_PERIOD);
    options.minInterval = period*1000;

    menuPrint("\ncalling flpUpdateSession with id=%u mode=%i minInterval=%" PRId32 " ...",
            id, mode, options.minInterval);
    uint32_t ret = 0;
    fflush (stdout);
    ret = getGnssAPI()->flpUpdateSession(mode, options, id);
    if (ret == 0) {
        gardenPrint("Waiting on Callback...\n");
    } else {
        gardenPrint("Failed\n");
    }
}

void FlpCase::stopBatching() {
    char buf[16], *p;
    uint32_t ret = 0;
    uint32_t id = 1;

    menuPrint("\nEnter Session Id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) > 1) {
        id = atoi(p);
    }

    menuPrint("\ncalling stopBatching with id=%u...",
            id);
    fflush (stdout);
    ret = getGnssAPI()->flpStopSession(id);
    if (ret == 0) {
        gardenPrint("Waiting on Callback...\n");
    } else {
        gardenPrint("Failed\n");
    }
}

void FlpCase::getBatchedLocation() {
    char buf[16], *p;
    uint32_t ret = 0;
    uint32_t id = 1;

    menuPrint("\nEnter Session Id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) > 1) {
        id = atoi(p);
    }

    menuPrint("\nEnter number of locations:");
    fflush(stdout);
    p = fgets(buf, 16, stdin);
    if(p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    int n = atoi(p);
    menuPrint("\ncalling getBatchedLocation..n=%i.", n);
    fflush (stdout);
    ret = getGnssAPI()->flpGetBatchedLocation(n, id);
    if (ret == 0) {
        gardenPrint("Waiting on Callback...\n");
    } else {
        gardenPrint("Failed\n");
    }
}

void FlpCase::batchingCleanUp() {
    menuPrint("\ncalling batchingCleanUp()...");
    fflush (stdout);
    getGnssAPI()->flpCleanUp();
}

void FlpCase::injectLocation() {
    double latitude = LAT;
    double longitude = LONG;
    float accuracy = ACCU;
    getGnssAPI()->gnssInjectLocation(latitude, longitude, accuracy);
}




GARDEN_RESULT FlpCase::menuCase() {
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
       menuPrint ("\n\n"
               "1: get batch size\n"
               "2: start batching\n"
               "3: update batching\n"
               "4: stop batching\n"
               "5: get batched location\n"
               "6: batching clean up\n"
               "7: inject location\n"
               "b: back\n"
               "q: quit\n\n"
               "Enter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }

        switch (p[0]) {
        case '1':
            getBatchSize();
            break;
        case '2':
            startBatching();
            break;
        case '3':
            updateBatching();
            break;
        case '4':
            stopBatching();
            break;
        case '5':
            getBatchedLocation();
            break;
        case '6':
            batchingCleanUp();
            break;
        case '7':
            injectLocation();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            return GARDEN_RESULT_ABORT;
        default:
            menuPrint("\ninvalid command\n");
        }
    }
    return GARDEN_RESULT_PASSED;
}

// This test case will load commands from given file, which can be logcat output,
// and execute the corresponding command at the given time (relatively).

// Format of input file:
// startSession (1, 1, 1000, 0)
// startSession (2, 1, 1000, 0)
// updateSession (1, 2, 2000, 5)
// stopSession (1)
// stopSession (2)

GARDEN_RESULT FlpCase::loadFromFileCase(std::string file) {
    uint64_t startTime = 0;
    uint64_t startUpTime = 0;

    std::ifstream in(file);
    std::string s;
    while(getline(in, s)) {
        const std::regex pt(" *(.*Session.*)\\x28(.*)\\x29[\\s\\S]*");
        std:: match_results<std::string::const_iterator> result;
        bool valid = std::regex_match(s, result, pt);
        if (valid) {
            std::string func = result[1];
            std::string args = result[2];
            gardenPrint(" %s %s", func.c_str(), args.c_str());
            std::string pattern;
            std::regex reg;
            std::match_results<std::string::const_iterator> param;
            if (std::strstr(func.c_str(), "startSession") != nullptr) {
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
                    options.minInterval = atoi(v3.c_str());
                    options.minDistance = atoi(v4.c_str());
                    getGnssAPI()->flpStartSession(atoi(v2.c_str()), options, atoi(v1.c_str()));
                }
            } else if (std::strstr(func.c_str(), "updateSession") != nullptr) {
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
                    options.minInterval = atoi(v3.c_str());
                    options.minDistance = atoi(v4.c_str());
                    getGnssAPI()->flpUpdateSession(atoi(v2.c_str()), options, atoi(v1.c_str()));
                }
            } else if (std::strstr(func.c_str(), "stopSession") != nullptr) {
                getGnssAPI()->flpStopSession(atoi(args.c_str()));
            } else if (std::strstr(func.c_str(), "getSessionLocation") != nullptr) {
                pattern = "(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if (valid) {
                    std::string v1 = param[1]; //batch size
                    std::string v2 = param[2]; //session Id
                    getGnssAPI()->flpGetBatchedLocation(atoi(v1.c_str()), atoi(v2.c_str()));
                }
            }
        }
    }
    return GARDEN_RESULT_PASSED;
}
} // namespace garden

