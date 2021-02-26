/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sstream>
#include <semaphore.h>
#include "GardenFramework.h"
#include "GardenUtil.h"
#include "LocationClientApiCase.h"

namespace garden {

using GeofenceBreachTypeMask=location_client::GeofenceBreachTypeMask;
#define BUFFERSIZE 50
#define CLIENTAPI_TEST_DELETEALL "deleteAll"
#define LAT 32.896535
#define LONG -117.201025
#define ACCU 50

enum ActionType {
    TEST_NO_ACTION = 0,
    TEST_GNSS_REPORT,
    TEST_LOCATION_REPORT,
    TEST_NETWORK_UP,
    TEST_NETWORK_DOWN,
    TEST_ROUTINE_BATCHING,
    TEST_TRIP_BATCHING,
    TEST_STOP,
    TEST_STOP_BATCHING,
    TEST_GEOFENCE,
};

enum GnssCallbackOptionsMask {
    GNSS_LOCATION_BIT = (1<<0),
    GNSS_SV_BIT       = (1<<1),
    GNSS_NMEA_BIT     = (1 << 2),
    GNSS_DATA_BIT     = (1 << 3)
};

// debug events counter
static uint32_t numLocationCb = 0;
static uint32_t numGnssLocationCb = 0;
static uint32_t numGnssSvCb = 0;
static uint32_t numGnssNmeaCb = 0;
static uint32_t numBatchingCb = 0;
static uint32_t numGeofenceCb = 0;
static uint32_t numGfBreachCb = 0;
static bool automode = false;
static sem_t sem_locationcbreceived;

/******************************************************************************
Callback functions
******************************************************************************/
static void onCapabilitiesCb(location_client::LocationCapabilitiesMask mask) {
    gardenPrint("<<< onCapabilitiesCb mask=0x%x", mask);
}

static void onResponseCb(location_client::LocationResponse response) {
    gardenPrint("<<< onResponseCb err=%u", response);
}

static void onLocationCb(const location_client::Location& location) {
    numLocationCb++;
    gardenPrint("<<< onLocationCb cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f",
            numLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
}

static void onBatchingCb(const std::vector<location_client::Location>& locations,
        location_client::BatchingStatus status) {
    numBatchingCb++;
    gardenPrint("<<< onBatchingCb batchingStatus=%d", status);
    if (location_client::BATCHING_STATUS_ACTIVE == status) {
        for (auto each: locations) {
            gardenPrint("<<< onBatchingCb cnt=%u time=%" PRIu64 " mask=0x%x lat=%f lon=%f alt=%f",
                    numBatchingCb,
                    each.timestamp,
                    each.flags,
                    each.latitude,
                    each.longitude,
                    each.altitude);
        }
    }
}

static void onGeofenceBreachCb( const std::vector<Geofence>& geofences,
        location_client::Location location, location_client::GeofenceBreachTypeMask type,
        uint64_t timestamp) {
    numGfBreachCb++;
    gardenPrint("<<< onGeofenceBreachCallback, cnt=%u, breachType=%d", numGfBreachCb, type);
    gardenPrint("<<< time=%" PRIu64" lat=%f lon=%f alt=%f",
            timestamp,
            location.latitude,
            location.longitude,
            location.altitude);
}
static void onCollectiveResponseCb(std::vector<pair<Geofence, LocationResponse>>& responses) {
    numGeofenceCb++;
    for (int i=0; i<responses.size(); ++i) {
        gardenPrint("<<< onCollectiveResponseCb cnt=%u lat=%f lon=%f radius=%f, response=%u",
                numGeofenceCb,
                responses[i].first.getLatitude(),
                responses[i].first.getLongitude(),
                responses[i].first.getRadius(),
                responses[i].second);
    }
}

static void onGnssLocationCb(const location_client::GnssLocation& location) {
    numGnssLocationCb++;
    gardenPrint("<<< onGnssLocationCb cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f",
            numGnssLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
    if (automode) {
        sem_post(&sem_locationcbreceived);
    }
}

static void onGnssSvCb(const std::vector<location_client::GnssSv>& gnssSvs) {
    numGnssSvCb++;
    stringstream ss;
    ss << "<<< onGnssSvCb c=" << numGnssSvCb << " s=" << gnssSvs.size();
    for (auto sv : gnssSvs) {
        ss << " " << sv.type << ":" << sv.svId << "/" << (uint32_t)sv.cN0Dbhz;
    }
    string s = ss.str();
    gardenPrint("%s", s.c_str());
}

static void onGnssNmeaCb(uint64_t timestamp, const std::string& nmea) {
    numGnssNmeaCb++;
    gardenPrint("<<< onGnssNmeaCb cnt=%u time=%" PRIu64" nmea=%s",
            numGnssNmeaCb, timestamp, nmea.c_str());
}

static void onGnssDataCb(const GnssData& gnssData) {
    gardenPrint("<<< onGnssDataCb\n");
    for (int sig = 0; sig < GNSS_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
        if (GNSS_DATA_JAMMER_IND_BIT ==
            (gnssData.gnssDataMask[sig] & GNSS_DATA_JAMMER_IND_BIT)) {
            gardenPrint("gnssDataMask[%d]=0x%X\n", sig, gnssData.gnssDataMask[sig]);
            gardenPrint("jammerInd[%d]=%f\n", sig, gnssData.jammerInd[sig]);
        }
        if (GNSS_DATA_AGC_BIT ==
            (gnssData.gnssDataMask[sig] & GNSS_DATA_AGC_BIT)) {
            gardenPrint("gnssDataMask[%d]=0x%X\n", sig, gnssData.gnssDataMask[sig]);
            gardenPrint("agc[%d]=%f\n", sig, gnssData.agc[sig]);
        }
    }
}

LocationClientApiCase::LocationClientApiCase() :
    mPClient_1(nullptr), mPClient_2(nullptr), mPClientImpl(nullptr) {
}

LocationClientApiCase::~LocationClientApiCase() {
    if (mPClient_1) {
        delete mPClient_1;
    }
    if (mPClient_2) {
        delete mPClient_2;
    }
    if (mPClientImpl) {
        delete mPClientImpl;
    }
}

GARDEN_RESULT LocationClientApiCase::preRun() {
    // this function will be called before every case.
    gardenPrint("preRun");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT LocationClientApiCase::postRun() {
    // this function will be called after every case.
    gardenPrint("postRun");
    return GARDEN_RESULT_PASSED;
}

static void printHelp() {
    menuPrint("\n\n"
        "location_client_api_testapp\n"
        "Test command format: client#,actionType(g/l/s/u/d),interval,distance(optional)\n"
        "Start client1 with Gnss Session type, interval 1000 ms, distance 0 meters: 1 g 1000 \n"
        "g: Gnss report session with all 4 types of callbacks\n"
        "g1: Gnss report session with LocationInfo callback\n"
        "g3: Gnss report session with LocationInfo and SV callbacks\n"
        "g5: Gnss report session with LocationInfo and NMEA callbacks\n"
        "g9: Gnss report session with LocationInfo and Data callbacks\n"
        "b: Routine batching\n"
        "t: Outdoor trip batching\n"
        "l: Location session \n"
        "f: Geofence session\n"
        "s: Stop a session \n"
        "k: Stop a batching session \n"
        "u: Set network available \n"
        "d: Set network not available \n"
        "deleteAll: gnssDeleteAidingData \n"
        "q: Quit\n"
        "\nEnter Command:");
        fflush(stdout);
}

void LocationClientApiCase::testDeleteAll() {
    if (!mPClientImpl) {
        mPClientImpl = new LocationClientApiImpl(onCapabilitiesCb);
    }
    GnssAidingData aidingData;
    aidingData.deleteAll = true;
    int ret = mPClientImpl->gnssDeleteAidingData(aidingData);
    gardenPrint("test DeleteAll returns %d\n", ret);
}

void LocationClientApiCase::menuAddGeofence(int clientId) {
    int geofenceNum = 0;
    double latitude = LAT;
    double longitude = LONG;
    double radius = ACCU;
    GeofenceBreachTypeMask type = (GeofenceBreachTypeMask)
            (location_client::GEOFENCE_BREACH_ENTER_BIT |
            location_client::GEOFENCE_BREACH_EXIT_BIT);
    uint32_t responsiveness = 4000;
    uint32_t time = 0;
    char buf[16], *p;
    mGeofences.clear();
    menuPrint ("\nEnter number of geofences:", geofenceNum);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofenceNum = atoi(p);
    }
    for (int i=0; i<geofenceNum; ++i) {
        menuPrint ("\nEnter latitude (default %f):", latitude);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atof(p) != 0) {
            latitude = atof(p);
        }
        menuPrint ("\nEnter longitude (%f):", -117.201025);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atof(p) != 0) {
            longitude = atof(p);
        }
        menuPrint ("\nEnter radius (default %f):", radius);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atof(p) != 0) {
            radius = atof(p);
        }
        menuPrint ("\nEnter breachType (default %u):", type);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            type = static_cast<GeofenceBreachTypeMask>(atoi(p));
        }
        menuPrint ("\nEnter responsiveness in seconds: (default %u):",
                responsiveness / 1000);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            responsiveness = atoi(p) * 1000;
        }
        menuPrint ("\nEnter dwell time (default %u):", time);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atof(p) != 0) {
            time = atof(p);
        }
        Geofence gf(latitude, longitude, radius, type, responsiveness, time);
        mGeofences.push_back(gf);
    }


    if (1 == clientId) {
        mPClient_1->addGeofences(mGeofences, onGeofenceBreachCb, onCollectiveResponseCb);
    } else {
        mPClient_2->addGeofences(mGeofences, onGeofenceBreachCb, onCollectiveResponseCb);
    }
}

void LocationClientApiCase::menuModifyGeofence(int clientId) {
    int32_t geofenceNum = 0;
    location_client::GeofenceBreachTypeMask type = (GeofenceBreachTypeMask)
            (location_client::GEOFENCE_BREACH_ENTER_BIT |
             location_client::GEOFENCE_BREACH_EXIT_BIT);
    uint32_t responsiveness = 4000;
    uint32_t time = 0;
    int32_t geofenceId = 0;
    vector<Geofence> modifyGfs;
    char buf[16], *p;
    menuPrint ("\nEnter number of geofences:", geofenceNum);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofenceNum = atoi(p);
    }
    for (int i=0; i<geofenceNum; ++i) {
        menuPrint ("\nEnter id ");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            geofenceId = atoi(p);
        }
        menuPrint ("\nEnter breachType (default %u):", type);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            type = static_cast<GeofenceBreachTypeMask>(atoi(p));
        }
        menuPrint ("\nEnter responsiveness in seconds: (default %u):",
                responsiveness / 1000);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            responsiveness = atoi(p) * 1000;
        }
        menuPrint ("\nEnter dwell time (default %u):", time);
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atof(p) != 0) {
            time = atof(p);
        }
        mGeofences[geofenceId].setBreachType(type);
        mGeofences[geofenceId].setResponsiveness(responsiveness);
        mGeofences[geofenceId].setDwellTime(time);
        if (geofenceId == 0) {
            return;
        }
        modifyGfs.push_back(mGeofences[geofenceId-1]);
    }
    if (1 == clientId) {
        mPClient_1->modifyGeofences(modifyGfs);
    } else {
        mPClient_2->modifyGeofences(modifyGfs);
    }
}

void LocationClientApiCase::menuPauseGeofence(int clientId) {
    int32_t geofenceNum = 0;
    int32_t geofenceId = 0;
    vector<Geofence> pauseGfs;
    char buf[16], *p;
    menuPrint ("\nEnter number of geofences:", geofenceNum);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofenceNum = atoi(p);
    }
    for (int i=0; i<geofenceNum; ++i) {
        menuPrint ("\nEnter id ");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            geofenceId = atoi(p);
        }
        if (geofenceId > 0 && geofenceId <= mGeofences.size())
            pauseGfs.push_back(mGeofences[geofenceId-1]);
    }
    if (1 == clientId) {
        mPClient_1->pauseGeofences(pauseGfs);
    } else {
        mPClient_2->pauseGeofences(pauseGfs);
    }
}

void LocationClientApiCase::menuResumeGeofence(int clientId) {
    int32_t geofenceNum = 0;
    int32_t geofenceId = 0;
    vector<Geofence> resumeGfs;
    char buf[16], *p;
    menuPrint ("\nEnter number of geofences:", geofenceNum);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofenceNum = atoi(p);
    }
    for (int i=0; i<geofenceNum; ++i) {
        menuPrint ("\nEnter id ");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            geofenceId = atoi(p);
        }
        if (geofenceId > 0 && geofenceId <= mGeofences.size())
            resumeGfs.push_back(mGeofences[geofenceId-1]);
    }
    if (1 == clientId) {
        mPClient_1->resumeGeofences(resumeGfs);
    } else {
        mPClient_2->resumeGeofences(resumeGfs);
    }
}
void LocationClientApiCase::menuRemoveGeofence(int clientId) {
    int32_t geofenceNum = 0;
    int32_t geofenceId = 0;
    vector<Geofence> removeGfs;
    char buf[16], *p;
    menuPrint ("\nEnter number of geofences:", geofenceNum);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofenceNum = atoi(p);
    }
    for (int i=0; i<geofenceNum; ++i) {
        menuPrint ("\nEnter id ");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        if (p == nullptr) {
            menuPrint("Error: fgets returned nullptr !!");
            return;
        }
        if (atoi(p) != 0) {
            geofenceId = atoi(p);
        }
        if (geofenceId >0 && geofenceId <= mGeofences.size()) {
            removeGfs.push_back(mGeofences[geofenceId-1]);
        }
    }

    if (1 == clientId) {
        mPClient_1->removeGeofences(removeGfs);
    } else {
        mPClient_2->removeGeofences(removeGfs);
    }
}

GARDEN_RESULT LocationClientApiCase::testGeofence(int clientId) {
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        menuPrint ("\n\n"
            "1: add_geofence\n"
            "2: pause_geofence\n"
            "3: resume geofence\n"
            "4: modify geofence\n"
            "5: remove geofence\n"
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
            menuAddGeofence(clientId);
            break;
        case '2':
            menuPauseGeofence(clientId);
            break;
        case '3':
            menuResumeGeofence(clientId);
            break;
        case '4':
            menuModifyGeofence(clientId);
            break;
        case '5':
            menuRemoveGeofence(clientId);
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

GARDEN_RESULT LocationClientApiCase::menuCase() {

    automode = false;
    if (nullptr == mPClient_1) {
        mPClient_1 = new LocationClientApi(onCapabilitiesCb);
    }

    bool exit_loop = false;
    while (!exit_loop) {

        printHelp();
        char str[BUFFERSIZE];
        char* pch = fgets(str, BUFFERSIZE, stdin);
        char* argPtr;
        if (nullptr == pch) {
            GARDEN_ERROR("Error: fgets returned nullptr !!");
        }

        /* remove newline, if present */
        int len = strlen(str) - 1;
        if( str[len] == '\n') {
            str[len] = '\0';
        }

        if (str[0] != '1' && str[0] != '2') {
            if (str[0] == 'Q' || str[0] == 'q') {
                exit_loop = true;
                break;
            } else if (!strcmp(str, CLIENTAPI_TEST_DELETEALL)) {
                testDeleteAll();
                continue;
            } else {
                printHelp();
                continue;
            }
        }

        pch = strtok_r(str, " ", &argPtr);
        uint32_t clientId = 0;
        uint32_t intervalInMs = 0;
        uint32_t distanceInMeters = 0;
        ActionType type = TEST_NO_ACTION;
        int gnssReportOptions = GNSS_LOCATION_BIT | GNSS_SV_BIT | GNSS_NMEA_BIT;
        GnssReportCbs reportcbs;
        int i = 0;
        bool parseError = false;

        while (nullptr != pch) {
            switch (i) {
                case 0:
                    clientId = std::stoul(pch, nullptr, 10);
                    if ((2 == clientId) && (!mPClient_2)) {
                        mPClient_2 = new LocationClientApi(onCapabilitiesCb);
                    }
                    break;
                case 1: {
                    if (*pch == 'G' || *pch == 'g') {
                        type = TEST_GNSS_REPORT;
                        if (strlen(pch) > 1) {
                            pch++;
                            gnssReportOptions = std::stoul(pch,nullptr,10);
                        }

                        if (gnssReportOptions < GNSS_LOCATION_BIT) {
                            gardenPrint("Invalid gnss report option, set to default\n");
                            gnssReportOptions = GNSS_LOCATION_BIT | GNSS_SV_BIT |
                                                GNSS_NMEA_BIT | GNSS_DATA_BIT;
                        }
                    } else if (*pch == 'B' || *pch == 'b') {
                        type = TEST_ROUTINE_BATCHING;
                    } else if (*pch == 'T' || *pch == 't') {
                        type = TEST_TRIP_BATCHING;
                    } else if (*pch == 'F' || *pch == 'f') {
                        type = TEST_GEOFENCE;
                    } else if (*pch == 'L' || *pch == 'l') {
                        type = TEST_LOCATION_REPORT;
                    } else if (*pch == 'U' || *pch == 'u') {
                        type = TEST_NETWORK_UP;
                    } else if (*pch == 'D' || *pch == 'd') {
                        type = TEST_NETWORK_DOWN;
                    } else if (*pch == 'S' || *pch == 's') {
                        type = TEST_STOP;
                    } else if (*pch == 'K' || *pch == 'k') {
                        type = TEST_STOP_BATCHING;
                    } else {
                        gardenPrint("Action type is not recognized %c \n",*pch);
                        parseError = true;
                        pch = nullptr;
                    }
                    break;
                }
                case 2:
                    intervalInMs = std::stoul(pch, nullptr, 10);
                    break;
                case 3:
                    distanceInMeters = std::stoul(pch, nullptr, 10);
                    break;
                default:
                    pch = nullptr;
                    break;
            }
            pch = strtok_r(nullptr, " ", &argPtr);
            i++;
        } //while

        if (parseError) {
            printHelp();
            continue;
        }
        LocationClientApi* mPClient = (1 == clientId? mPClient_1: mPClient_2);
        switch (type) {
            case TEST_GNSS_REPORT:
                // clear debug events counter
                numLocationCb = 0;
                numGnssLocationCb = 0;
                numGnssSvCb = 0;
                numGnssNmeaCb = 0;

                if (gnssReportOptions & GNSS_LOCATION_BIT) {
                    reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
                }
                if (gnssReportOptions & GNSS_SV_BIT) {
                    reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
                }
                if (gnssReportOptions & GNSS_NMEA_BIT) {
                    reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);
                }
                if (gnssReportOptions & GNSS_DATA_BIT) {
                    reportcbs.gnssDataCallback = GnssDataCb(onGnssDataCb);
                }

                if (1 == clientId) {
                    mPClient_1->startPositionSession(
                            intervalInMs, reportcbs, onResponseCb);
                } else {
                    mPClient_2->startPositionSession(
                            intervalInMs, reportcbs, onResponseCb);
                }
                break;
            case TEST_LOCATION_REPORT:
                // clear debug events counter
                numLocationCb = 0;
                numGnssLocationCb = 0;
                numGnssSvCb = 0;
                numGnssNmeaCb = 0;

                mPClient->startPositionSession(
                        intervalInMs, distanceInMeters, onLocationCb, onResponseCb);
                break;
            case TEST_NETWORK_UP:
                mPClient->updateNetworkAvailability(true);
                break;
            case TEST_NETWORK_DOWN:
                mPClient->updateNetworkAvailability(false);
                break;
            case TEST_ROUTINE_BATCHING:
                numBatchingCb = 0;
                mPClient->startRoutineBatchingSession(
                        intervalInMs, distanceInMeters, onBatchingCb, onResponseCb);
                break;
            case TEST_TRIP_BATCHING:
                numBatchingCb = 0;
                mPClient->startTripBatchingSession(
                        intervalInMs, distanceInMeters, onBatchingCb, onResponseCb);
                break;
            case TEST_GEOFENCE:
                numGeofenceCb = 0;
                numGfBreachCb = 0;
                testGeofence(clientId);
                break;
            case TEST_STOP:
                mPClient->stopPositionSession();
                break;
            case TEST_STOP_BATCHING:
                mPClient->stopBatchingSession();
                break;
        } //switch(type)
    }//while(!exit_loop)

    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT LocationClientApiCase::autoStartCases(
        uint32_t interval, uint32_t loop, uint32_t option) {

    gardenPrint("autoStartCases interval=%u loop=%u\n", interval, loop);

    // clear debug events counter
    automode = true;
    numLocationCb = 0;
    numGnssLocationCb = 0;
    numGnssSvCb = 0;
    numGnssNmeaCb = 0;
    sem_init(&sem_locationcbreceived, 0, 0);

    if (nullptr == mPClient_1) {
        mPClient_1 = new LocationClientApi(onCapabilitiesCb);
    }

    // set callbacks
    GnssReportCbs reportcbs;
    if (option & GNSS_LOCATION_BIT) {
        reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
    }
    if (option & GNSS_SV_BIT) {
        reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
    }
    if (option & GNSS_NMEA_BIT) {
        reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);
    }
    if (option & GNSS_DATA_BIT) {
        reportcbs.gnssDataCallback = GnssDataCb(onGnssDataCb);
    }

    // start
    mPClient_1->startPositionSession(interval, reportcbs, onResponseCb);

    // wait for location callbacks requested times
    for (int i=0; i<loop; i++) {
        sem_wait(&sem_locationcbreceived);
    }

    // stop session
    mPClient_1->stopPositionSession();
    gardenPrint("autoStartCases done\n");
    return GARDEN_RESULT_PASSED;
}

} // namespace garden
