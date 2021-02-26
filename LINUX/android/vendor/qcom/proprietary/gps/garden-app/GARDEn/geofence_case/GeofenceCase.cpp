/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <regex>
#include <cstdlib>
#include <fstream>
#include "GardenFramework.h"
#include "GardenUtil.h"
#include "GeofenceCase.h"
#include <GeofenceCb.h>

#define CALL_AT(future, func)\
{\
    uint64_t nowUp = getUpTimeMSec();\
    if (startTime == 0) { startTime = future; startUpTime = nowUp; }\
    if (future - startTime > nowUp - startUpTime) {\
        usleep(((future - startTime) - (nowUp - startUpTime)) * 1000);\
    }\
    func;\
}

namespace garden {

enum GeoFenceCommands {
    ADD_COMMAND = 0,
    PAUSE_COMMAND = 1,
    RESUME_COMMAND = 2,
    MODIFY_COMMAND = 3,
    REMOVE_COMMAND = 4,
};

typedef std::map<int32_t, GeofenceInfo> GeoFencesMap;
static GeoFencesMap sGeoFencesRunning;
static GeoFencesMap sGeoFencesPaused;

static float randomFloat(float low, float high) {
    return (low + (float)rand()/((float)RAND_MAX/(high-low)));
}

static int randomInt(int low, int high) {
    return (low + (rand() % (high-low+1)));
}

static uint32_t getGeofenceId(GeoFencesMap& geoMap, GeoFencesMap::iterator& geoIter) {
    int index = randomInt(0, geoMap.size() - 1);
    for(int i=0; i<index; ++i) {
        ++geoIter;
    }
    return geoIter->first;
}

GeofenceCase::GeofenceCase() {
}

GeofenceCase::~GeofenceCase() {
}

GARDEN_RESULT GeofenceCase::preRun() {
    if (getGnssAPI() == nullptr) {
        GARDEN_ERROR("getGnssAPI() failed.");
    }
    if(!mCb) {
        mCb = new GeofenceCb(this);
        mGnssAPI->setGeofenceCbs(mCb);
    }
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GeofenceCase::postRun() {
    wait();
    if (mGnssAPI) {
        mGnssAPI->setGeofenceCbs(nullptr);
    }
    if(mCb) {
        delete mCb;
        mCb = nullptr;
    }
    return GARDEN_RESULT_PASSED;
}

void GeofenceCase::syncAddGeofence(uint32_t geofenceId, double latitudeDegrees,
        double longitudeDegrees, double radiusMeters,
        GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    gardenPrint("GEOFENCE ADD -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)\n",
            geofenceId, latitudeDegrees, longitudeDegrees,
            radiusMeters, notificationResponsivenessMs);
    getGnssAPI()->addGeofence(geofenceId, latitudeDegrees, longitudeDegrees,
            radiusMeters, monitorTransitions, notificationResponsivenessMs);
}

void GeofenceCase::syncModifyGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs) {
    gardenPrint("GEOFENCE MODIFY -> id=%2u | resp=%d(ms)\n",
            geofenceId, notificationResponsivenessMs);
    getGnssAPI()->modifyGeofence(geofenceId, monitorTransitions,
            notificationResponsivenessMs);
}

void GeofenceCase::syncPauseGeofence(uint32_t geofenceId) {
    gardenPrint("GEOFENCE PAUSE -> id=%2u\n", geofenceId);
    getGnssAPI()->pauseGeofence(geofenceId);
}

void GeofenceCase::syncResumeGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitor_transitions) {
    gardenPrint("GEOFENCE RESUME -> id=%2u monitor_transitions=%d\n",
            geofenceId, monitor_transitions);
    getGnssAPI()->resumeGeofence(geofenceId, monitor_transitions);
}

void GeofenceCase::syncRemoveGeofence(uint32_t geofenceId) {
    gardenPrint("GEOFENCE REMOVE -> id=%2u\n", geofenceId);
    getGnssAPI()->removeGeofence(geofenceId);
}

void GeofenceCase::menuAddGeofence() {
    uint32_t geofence_id = 1;
    double latitude = LAT;
    double longitude = LONG;
    double radius_m = ACCU;
    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
            (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    uint32_t notification_responsivenes_ms = 4000;
    char buf[16], *p;
    menuPrint ("\nEnter id (default %d):", geofence_id);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofence_id = atoi(p);
    }
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
    menuPrint ("\nEnter radius (default %f):", radius_m);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atof(p) != 0) {
        radius_m = atof(p);
    }
    menuPrint ("\nEnter responsiveness in seconds: (default %d):",
            notification_responsivenes_ms / 1000);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        notification_responsivenes_ms = atoi(p) * 1000;
    }
    syncAddGeofence(geofence_id, latitude, longitude, radius_m,
            monitor_transitions, notification_responsivenes_ms);
}

void GeofenceCase::menuModifyGeofence() {
    int32_t geofence_id = 1;
    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
            (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    uint32_t notification_responsivenes_ms = 4000;
    char buf[16], *p;
    menuPrint ("\nEnter id (default %d):", geofence_id);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofence_id = atoi(p);
    }
    menuPrint ("\nEnter responsiveness in seconds: (default %d):",
            notification_responsivenes_ms / 1000);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        notification_responsivenes_ms = atoi(p) * 1000;
    }
    syncModifyGeofence(geofence_id, monitor_transitions, notification_responsivenes_ms);
}

void GeofenceCase::menuPauseGeofence() {
    int32_t geofence_id = 1;
    char buf[16], *p;
    menuPrint ("\nEnter id (default %d):", geofence_id);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofence_id = atoi(p);
    }
    syncPauseGeofence(geofence_id);
}

void GeofenceCase::menuResumeGeofence() {
    int32_t geofence_id = 1;
    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
            (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    char buf[16], *p;
    menuPrint ("\nEnter id (default %d):", geofence_id);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofence_id = atoi(p);
    }
    syncResumeGeofence(geofence_id, monitor_transitions);
}

void GeofenceCase::menuRemoveGeofence() {
    int32_t geofence_id = 1;
    char buf[16], *p;
    menuPrint ("\nEnter id (default %d):", geofence_id);
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        menuPrint("Error: fgets returned nullptr !!");
        return;
    }
    if (atoi(p) != 0) {
        geofence_id = atoi(p);
    }

    syncRemoveGeofence(geofence_id);
}

void GeofenceCase::randomAddGeofence() {
    GeofenceInfo info;
    GeoFencesMap::iterator it;
    uint32_t geofence_id;
    do {
        geofence_id = randomInt(0, 4096);
        it = sGeoFencesRunning.find(geofence_id);
    } while (it != sGeoFencesRunning.end());

    info.size = sizeof(GeofenceInfo);
    info.latitude = randomFloat(-90, 90);
    info.longitude = randomFloat(-180, 180);
    info.radius = randomFloat(1, 1000);
    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
        (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    uint32_t notification_responsivenes_ms = randomInt(1, 0x40000);

    sGeoFencesRunning[geofence_id] = info;
    syncAddGeofence(geofence_id, info.latitude, info.longitude, info.radius,
            monitor_transitions, notification_responsivenes_ms);
}

void GeofenceCase::randomModifyGeofence() {
    bool modifyRunningGeofence = true;
    if (!sGeoFencesRunning.empty() && !sGeoFencesPaused.empty()) {
        modifyRunningGeofence = randomInt(0, 1);
    } else if (!sGeoFencesRunning.empty()) {
        modifyRunningGeofence = true;
    } else if (!sGeoFencesPaused.empty()) {
        modifyRunningGeofence = false;
    } else {
        return;
    }
    GeoFencesMap& modifyMap(modifyRunningGeofence ? sGeoFencesRunning : sGeoFencesPaused);
    GeoFencesMap::iterator geoModifyIter = modifyMap.begin();

    uint32_t geofence_id = getGeofenceId(modifyMap, geoModifyIter);
    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
            (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    uint32_t notification_responsivenes_ms = randomInt(1, 0x40000);

    syncModifyGeofence(geofence_id, monitor_transitions, notification_responsivenes_ms);
}

void GeofenceCase::randomPauseGeofence() {
    if (sGeoFencesRunning.empty()) {
        return;
    }
    GeoFencesMap::iterator geoRunningIter = sGeoFencesRunning.begin();

    uint32_t geofence_id = getGeofenceId(sGeoFencesRunning, geoRunningIter);
    sGeoFencesPaused[geofence_id] = geoRunningIter->second;
    sGeoFencesRunning.erase(geoRunningIter);
    syncPauseGeofence(geofence_id);
}

void GeofenceCase::randomResumeGeofence() {
    if (sGeoFencesPaused.empty()) {
        return;
    }
    GeoFencesMap::iterator geoPausedIter = sGeoFencesPaused.begin();

    uint32_t geofence_id = getGeofenceId(sGeoFencesPaused, geoPausedIter);
    sGeoFencesRunning[geofence_id] = geoPausedIter->second;
    sGeoFencesPaused.erase(geoPausedIter);

    GeofenceBreachTypeMask monitor_transitions = (GeofenceBreachTypeMask)
        (GEOFENCE_BREACH_ENTER_BIT | GEOFENCE_BREACH_EXIT_BIT);
    syncResumeGeofence(geofence_id, monitor_transitions);
}

void GeofenceCase::randomRemoveGeofence() {
    bool removeRunningGeofence = true;
    if (!sGeoFencesRunning.empty() && !sGeoFencesPaused.empty()) {
        removeRunningGeofence = randomInt(0, 1);
    } else if (!sGeoFencesRunning.empty()) {
        removeRunningGeofence = true;
    } else if (!sGeoFencesPaused.empty()) {
        removeRunningGeofence = false;
    } else {
        return;
    }
    GeoFencesMap& removeMap(removeRunningGeofence ? sGeoFencesRunning : sGeoFencesPaused);
    GeoFencesMap::iterator geoRemoveIter = removeMap.begin();

    uint32_t geofence_id = getGeofenceId(removeMap, geoRemoveIter);
    removeMap.erase(geoRemoveIter);
    syncRemoveGeofence(geofence_id);
}

void GeofenceCase::dumpGeofence() {
    gardenPrint("GPS  | id | latitude  | longitude |  radius  | paused");

    for (GeoFencesMap::iterator geoRunningIter = sGeoFencesRunning.begin();
            geoRunningIter != sGeoFencesRunning.end(); geoRunningIter++)
    {
        uint32_t geofence_id = geoRunningIter->first;
        GeofenceInfo info(geoRunningIter->second);
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               geofence_id, info.latitude, info.longitude, info.radius, 0);
    }

    for (GeoFencesMap::iterator geoPausedIter = sGeoFencesPaused.begin();
            geoPausedIter != sGeoFencesPaused.end(); geoPausedIter++)
    {
        uint32_t geofence_id = geoPausedIter->first;
        GeofenceInfo info(geoPausedIter->second);
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               geofence_id, info.latitude, info.longitude, info.radius, 1);
    }
}

void GeofenceCase::clearGeofence() {
    for (auto &gfRunning: sGeoFencesRunning) {
        syncRemoveGeofence(gfRunning.first);
    }
    for (auto &gfPaused: sGeoFencesPaused) {
        syncRemoveGeofence(gfPaused.first);
    }
}

GARDEN_RESULT GeofenceCase::menuCase() {
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
            menuAddGeofence();
            break;
        case '2':
            menuPauseGeofence();
            break;
        case '3':
            menuResumeGeofence();
            break;
        case '4':
            menuModifyGeofence();
            break;
        case '5':
            menuRemoveGeofence();
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

GARDEN_RESULT GeofenceCase:: stressRandomCase() {
    uint32_t testCount = 50;
    uint32_t geofencesToStartWith = 50;
    char buf[16], *p;
    menuPrint ("\nEnter number of initial geofences to add (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        GARDEN_ERROR("Error: fgets returned nullptr !!");
    }
    if (atoi(p) != 0)
        geofencesToStartWith = atoi(p);
    menuPrint ("\nEnter number of random geofences commands to call (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        GARDEN_ERROR("Error: fgets returned nullptr !!");
    }
    int32_t tmp = atoi(p);
    if (tmp > 0) {
        testCount = tmp;
    }
    srand(time(nullptr));

    if (geofencesToStartWith > 10000) {
        GARDEN_ERROR("\nInvalid geofencesToStartWith (max 10000) %u",
                geofencesToStartWith);
    }
    for (uint32_t i = 0; i < geofencesToStartWith; ++i) {
        randomAddGeofence();
    }
    while (testCount-- > 0) {
        GeoFenceCommands commandId = (GeoFenceCommands)randomInt(0, 4);
        switch (commandId) {
            case ADD_COMMAND:
                menuPrint("ADD_COMMAND\n");
                randomAddGeofence();
            break;

            case PAUSE_COMMAND:
                menuPrint("PAUSE_COMMAND\n");
                randomPauseGeofence();
            break;

            case RESUME_COMMAND:
                menuPrint("RESUME_COMMAND\n");
                randomResumeGeofence();
            break;

            case MODIFY_COMMAND:
                menuPrint("MODIFY_COMMAND\n");
                randomModifyGeofence();
            break;

            case REMOVE_COMMAND:
                menuPrint("REMOVE_COMMAND\n");
                randomRemoveGeofence();
            break;
        }

        dumpGeofence();
    }
    clearGeofence();
    menuPrint("CLEARING ALL GEOFENCES\n");
    sGeoFencesRunning.clear();
    sGeoFencesPaused.clear();

    menuPrint("DONE\n");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT GeofenceCase::stressAddRemoveCase() {
    uint32_t testCount = 50;
    char buf[16], *p;
    menuPrint ("\nEnter number of times to add/remove 50 geofences (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p == nullptr) {
        GARDEN_ERROR("Error: fgets returned nullptr !!");
    }
    int32_t tmp = atoi(p);
    if (tmp > 0) {
        testCount = tmp;
    }
    while (testCount-- > 0) {
        for (uint32_t i = 0; i < 50; ++i) {
            randomAddGeofence();
        }
        for (uint32_t i = 0; i < 50; ++i) {
            randomRemoveGeofence();
        }
    }

    menuPrint("CLEARING ALL GEOFENCES\n");
    sGeoFencesRunning.clear();
    sGeoFencesPaused.clear();

    menuPrint("DONE\n");
    return GARDEN_RESULT_PASSED;
}

// This test case will load commands from given file, which can be logcat output,
// and execute the corresponding command at the given time (relatively).

// Format of input file:
// 00:17:52.771 geofenceAdd (1, 37.394662, -121.829398, 80.000000, 4, 7, 9000, 9000)
// 00:17:52.771 geofenceRemove (3)
GARDEN_RESULT GeofenceCase::loadFromFileCase(std::string file) {
    uint64_t startTime = 0;
    uint64_t startUpTime = 0;

    std::ifstream in(file);
    std::string s;
    while(getline(in, s)) {
        const std::regex pt(".*(\\d{2}):(\\d{2}):(\\d{2}).(\\d{3}).*(geofence.*)\\x28(.*)\\x29[\\S\\s]*");
        std:: match_results<std::string::const_iterator> result;
        bool valid = std::regex_match(s, result, pt);
        if (valid) {
            std::string hour = result[1];
            std::string min = result[2];
            std::string sec = result[3];
            std::string msec = result[4];
            std::string func = result[5];
            std::string args = result[6];
            int32_t hourVal = atoi(hour.c_str());
            int32_t minVal = atoi(min.c_str());
            int32_t secVal = atoi(sec.c_str());
            int32_t msecVal = atoi(msec.c_str());
            if (hourVal >= 0 && minVal >= 0 && secVal >= 0 && msecVal >= 0) {
                uint64_t time = (uint64_t)(((hourVal * 60 + minVal) * 60) + secVal) * 1000 +
                        msecVal;
                gardenPrint("%" PRIu64 " %s %s", time, func.c_str(), args.c_str());
            }
            std::string pattern;
            std::regex reg;
            std::match_results<std::string::const_iterator> param;
            if (std::strstr(func.c_str(), "geofenceAdd") != nullptr) {
                pattern = "(.*) *, *(.*) *, *(.*) *, *(.*) *, *(.*) *, *(.*) *, *(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if(valid) {
                    std::string v1 = param[1];
                    std::string v2 = param[2];
                    std::string v3 = param[3];
                    std::string v4 = param[4];
                    std::string v6 = param[6];
                    std::string v7 = param[7];
                    syncAddGeofence(atoi(v1.c_str()), atof(v2.c_str()),
                            atof(v3.c_str()), atof(v4.c_str()),
                            atoi(v6.c_str()), atoi(v7.c_str()));
                }
            } else if (std::strstr(func.c_str(), "geofenceRemove") != nullptr) {
                syncRemoveGeofence(atoi(args.c_str()));
            } else if (std::strstr(func.c_str(), "geofenceModify") != nullptr) {
                pattern = "(.*) *, *(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if(valid) {
                    std::string v1 = param[1];
                    std::string v2 = param[2];
                    std::string v3 = param[3];
                    syncModifyGeofence(atoi(v1.c_str()), (GeofenceBreachTypeMask)atoi(v2.c_str()),
                            atoi(v3.c_str()));
                }
            } else if (std::strstr(func.c_str(), "geofencePause") != nullptr) {
                syncPauseGeofence(atoi(args.c_str()));
            } else if (std::strstr(func.c_str(), "geofenceResume") != nullptr) {
                pattern = "(.*) *, *(.*)";
                reg.assign(pattern);
                valid = std::regex_match(args, param, reg);
                if(valid) {
                    std::string v1 = param[1];
                    std::string v2 = param[2];
                    syncResumeGeofence(atoi(v1.c_str()), (GeofenceBreachTypeMask)atoi(v2.c_str()));
                }
            }
        }
    }
    return GARDEN_RESULT_PASSED;
}

} // namespace garden
