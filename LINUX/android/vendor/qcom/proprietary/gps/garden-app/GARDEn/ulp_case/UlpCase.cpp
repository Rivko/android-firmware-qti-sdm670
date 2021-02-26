/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <GardenFramework.h>
#include <GardenUtil.h>
#include "UlpCase.h"

#ifdef USE_GLIB
#include "IIzatManager.h"
#include "OSFramework.h"
#endif

namespace garden {

typedef enum
{
    PC_GPS      = (1 << 0),
    PC_NETWORK  = (1 << 1),
    PC_WIFI     = (1 << 2),
    PC_AGPS     = (1 << 3),
    PC_ALL      = (~0L),
} PHONE_CONTEXT;

UlpCase* UlpCase::mInstance = nullptr;

const UlpEngineInterface* UlpCase::pUlpEngineInterface = nullptr;
const UlpNetworkInterface* UlpCase::pUlpNetworkInterface = nullptr;
const UlpPhoneContextInterface* UlpCase::pUlpPhoneContextInterface = nullptr;

static UlpEngineCallbacks sUlpEngineCallbacks = {
    sizeof(UlpEngineCallbacks),
    UlpCase::locationCb,
    UlpCase::statusCb,
    UlpCase::createThreadCb,
    UlpCase::nmeaCb,
    UlpCase::svInfoCb,
};

static UlpPhoneContextCallbacks pUlpPhoneContextCallbacks = {
    UlpCase::requestPhoneContextCb,
};

static UlpNetworkLocationCallbacks pUlpNetworkLocationCallbacks = {
    UlpCase::networkLocationRequestCb,
};

typedef void (*ThreadStart)(void *);
typedef struct _tcreatorData {
    ThreadStart pfnThreadStart;
    void* arg;
} tcreatorData;

static void *gThreadFn (void *tcd) {
    tcreatorData* local_tcd = (tcreatorData*)tcd;
    if (nullptr != local_tcd) {
        local_tcd->pfnThreadStart (local_tcd->arg);
        free(local_tcd);
    }
    return nullptr;
}

UlpCase::UlpCase() : mPhoneContext(PC_ALL) {
}

UlpCase::~UlpCase() {
    if (mTimer.isStarted()) {
        mTimer.stop();
    }
}

GARDEN_RESULT UlpCase::preRun() {
    mInstance = this;
    return init();
}

GARDEN_RESULT UlpCase::postRun() {
    mInstance = nullptr;
    wait();
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::init() {
    if (pUlpEngineInterface != nullptr &&
            pUlpNetworkInterface != nullptr &&
            pUlpPhoneContextInterface != nullptr) {
        return GARDEN_RESULT_PASSED;
    }
#ifdef USE_GLIB
    IIzatManager * pIzatManager = getIzatManager (OSFramework::getOSFramework());
    GARDEN_ASSERT_NEQ(pIzatManager, nullptr, "getIzatManager returned nullptr!!");
    pIzatManager->enableProvider(IZAT_STREAM_NETWORK);
#endif
#ifdef __ANDROID__
    ulp_init(&sUlpEngineCallbacks, &pUlpNetworkLocationCallbacks, &pUlpPhoneContextCallbacks);
#endif
    pUlpEngineInterface =
        (const UlpEngineInterface*)ulp_get_extension(ULP_ENGINE_INTERFACE);
    pUlpNetworkInterface =
        (UlpNetworkInterface*)ulp_get_extension(ULP_NETWORK_INTERFACE);
    pUlpPhoneContextInterface =
        (const UlpPhoneContextInterface*)ulp_get_extension(ULP_PHONE_CONTEXT_INTERFACE);
    GARDEN_ASSERT_NEQ(pUlpEngineInterface, nullptr, "pUlpEngineInterface is nullptr.");
    GARDEN_ASSERT_NEQ(pUlpNetworkInterface, nullptr, "pUlpNetworkInterface is nullptr.");
    GARDEN_ASSERT_NEQ(pUlpPhoneContextInterface, nullptr, "pUlpPhoneContextInterface is nullptr.");
#ifdef __ANDROID__
    pUlpEngineInterface->system_update(ULP_LOC_EVENT_OBSERVER_STOP_EVENT_TX);
#endif
    return GARDEN_RESULT_PASSED;
}

pthread_t UlpCase::createThreadCb(const char *name, void (*start)(void *), void *arg) {
    gardenPrint("## %s ##:", __func__);
    pthread_t thread_id = -1;
    gardenPrint("%s", name);

    tcreatorData* tcd = (tcreatorData*)malloc(sizeof(*tcd));

    if (nullptr != tcd) {
        tcd->pfnThreadStart = start;
        tcd->arg = arg;

        if (0 > pthread_create (&thread_id, nullptr, gThreadFn, (void*)tcd)) {
            gardenPrint("error creating thread");
            free(tcd);
        } else {
            gardenPrint("created thread");
        }
    }

    return thread_id;
}

void UlpCase::locationCb(UlpLocation* location,
        GpsLocationExtended* /*locationExtended*/, enum loc_sess_status status) {
    gardenPrint("======================================================");
    gardenPrint("locationCb:");
    gardenPrint("LAT: %f, LON: %f, ACC: %f, TIME: %llu, status: %d",
                 location->gpsLocation.latitude, location->gpsLocation.longitude,
                 location->gpsLocation.accuracy,(long long) location->gpsLocation.timestamp,
                 status);
    gardenPrint("======================================================");
    if (mInstance != nullptr) {
        std::lock_guard<mutex> guard(mInstance->mLock);
        mInstance->unsetFlag(ULP_CASE_LOCATION);
    }
}

void UlpCase::statusCb(const LocGpsStatusValue /*status*/) {
}

void UlpCase::nmeaCb(const UlpNmea *pNmeaStr) {
    if(pNmeaStr) {
        gardenPrint("NMEA: %s", pNmeaStr->nmea_str);
    }
}

void UlpCase::svInfoCb(const GnssSvNotification* svNotify, uint16_t /*source*/) {
    if(svNotify) {
        gardenPrint("SvNotify");
    }
}

void UlpCase::requestPhoneContextCb(UlpPhoneContextRequest *req) {
    gardenPrint("requestPhoneContextCb with context_type: %x, request_type: %d",
            req->context_type, req->request_type );
    if (mInstance != nullptr) {
        mInstance->updatePhoneContext();
    }
}

void UlpCase::networkLocationRequestCb(UlpNetworkRequestPos* req) {
    gardenPrint("networkLocationRequestCb type: %d, interval: %d, desired_position_source: %d",
            req->request_type , req->interval_ms , req->desired_position_source );

    if (req->request_type == ULP_NETWORK_POS_STOP_REQUEST ) {
        gardenPrint("received network provider stop request\n");
    } else {
        if (mInstance != nullptr) {
            mInstance->nlpResponse();
        }
    }
}

void UlpCase::setupPhoneContext(uint32_t phoneContext) {
    mPhoneContext = phoneContext;
}

void UlpCase::updatePhoneContext() {
    gardenPrint("updatePhoneContext: Simulating AFW injection of phone context info");
    Runnable runnable = [this] {
        if(pUlpPhoneContextInterface != NULL) {
            UlpPhoneContextSettings settings;
            settings.context_type = ULP_PHONE_CONTEXT_GPS_SETTING |
                ULP_PHONE_CONTEXT_NETWORK_POSITION_SETTING |
                ULP_PHONE_CONTEXT_WIFI_SETTING |
                ULP_PHONE_CONTEXT_AGPS_SETTING;
            settings.is_gps_enabled = (mPhoneContext & PC_GPS);
            settings.is_agps_enabled = (mPhoneContext & PC_AGPS);
            settings.is_network_position_available = (mPhoneContext & PC_NETWORK);
            settings.is_wifi_setting_enabled = (mPhoneContext & PC_WIFI);
            pUlpPhoneContextInterface->ulp_phone_context_settings_update(&settings);
        }
    };
    sendMsg(runnable);
}

void UlpCase::nlpResponse() {
    gardenPrint("nlpResponse: Simulating AFW injection of network location after 5 secs");
    sleep(5);
    Runnable runnable = [this] {
        if((pUlpNetworkInterface != NULL) &&
                (pUlpNetworkInterface->ulp_send_network_position != NULL)){
            UlpNetworkPositionReport position_report;
            position_report.valid_flag = ULP_NETWORK_POSITION_REPORT_HAS_POSITION;
            position_report.position.latitude = 32.7;
            position_report.position.longitude = -119;
            position_report.position.HEPE = 1000;
            position_report.position.pos_source = ULP_NETWORK_POSITION_SRC_UNKNOWN;
            pUlpNetworkInterface->ulp_send_network_position(&position_report);
        }
    };
    sendMsg(runnable);
}

void UlpCase::updateCriteriaAndSleep(UlpLocationCriteria& criteria,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    criteria.valid_mask |= ULP_CRITERIA_HAS_RECURRENCE_TYPE;
    criteria.recurrence_type = (UlpRecurrenceCriteria)type;
    criteria.valid_mask |= ULP_CRITERIA_HAS_MIN_INTERVAL;
    criteria.min_interval = interval;
    pUlpEngineInterface->update_criteria(criteria);
    if (timeout > 0) {
        sleep(timeout);
    }
}

void UlpCase::updateCriteriaWithSource(uint32_t action, uint32_t source,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    if (pUlpEngineInterface == nullptr) return;

    UlpLocationCriteria criteria;
    memset(&criteria, 0, sizeof(UlpLocationCriteria));

    criteria.valid_mask = 0;
    criteria.valid_mask |= ULP_CRITERIA_HAS_ACTION;
    criteria.action = action;
    criteria.valid_mask |= ULP_CRITERIA_HAS_PROVIDER_SOURCE;
    criteria.provider_source = source;
    updateCriteriaAndSleep(criteria, type, interval, timeout);
}

void UlpCase::updateCriteriaWithAccuracy(uint32_t action, uint32_t accuracy,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    if (pUlpEngineInterface == nullptr) return;

    UlpLocationCriteria criteria;
    memset(&criteria, 0, sizeof(UlpLocationCriteria));

    criteria.valid_mask = 0;
    criteria.valid_mask |= ULP_CRITERIA_HAS_ACTION;
    criteria.action = action;
    criteria.valid_mask |= ULP_CRITERIA_HAS_PREFERRED_HORIZONTAL_ACCURACY;
    criteria.preferred_horizontal_accuracy = (UlpHorzAccuracyCriteria)accuracy;
    updateCriteriaAndSleep(criteria, type, interval, timeout);
}

void UlpCase::addCriteriaWithSource(uint32_t source,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    updateCriteriaWithSource(ULP_ADD_CRITERIA, source, type, interval, timeout);
}

void UlpCase::removeCriteriaWithSource(uint32_t source,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    updateCriteriaWithSource(ULP_REMOVE_CRITERIA, source, type, interval, timeout);
}

void UlpCase::addCriteriaWithAccuracy(uint32_t accuracy,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    updateCriteriaWithAccuracy(ULP_ADD_CRITERIA, accuracy, type, interval, timeout);
}

void UlpCase::removeCriteriaWithAccuracy(uint32_t accuracy,
        uint32_t type, uint32_t interval, uint32_t timeout) {
    updateCriteriaWithAccuracy(ULP_REMOVE_CRITERIA, accuracy, type, interval, timeout);
}

void UlpCase::startAndSleep(uint32_t timeout) {
    getGnssAPI()->gnssStart();
    if (timeout > 0) {
        sleep(timeout);
    }
}

void UlpCase::stopAndSleep(uint32_t timeout) {
    getGnssAPI()->gnssStop();
    if (timeout > 0) {
        sleep(timeout);
    }
}

void UlpCase::expectLocation(uint32_t timeout) {
    setFlag(ULP_CASE_LOCATION);
    if (timeout > 0) {
        // this Runnable will be called from timer thread
        Runnable timerRunnable = [this] {
            std::lock_guard<mutex> guard(mInstance->mLock);
            unsetFlag(ULP_CASE_LOCATION);
        };
        mTimer.set(timeout, timerRunnable);
        mTimer.start();
    }
}

void UlpCase::expectLocation() {
    expectLocation(0);
}

GARDEN_RESULT UlpCase::ulpCase1() {
    gardenPrint("UlpCase::testCase1");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 1);
    startAndSleep(5);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase2() {
    gardenPrint("UlpCase::testCase2");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_PERIODIC, 5, 1);
    startAndSleep(10);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(10);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase3() {
    gardenPrint("UlpCase::testCase3");
    setupPhoneContext(PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 1);
    startAndSleep(5);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase4() {
    gardenPrint("UlpCase::testCase4");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 1);
    startAndSleep(4);
    setupPhoneContext(PC_NETWORK|PC_WIFI);
    sleep(1);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase5() {
    gardenPrint("UlpCase::testCase5");
    setupPhoneContext(PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 1);
    startAndSleep(4);
    setupPhoneContext(PC_GPS|PC_WIFI);
    sleep(1);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase6() {
    gardenPrint("UlpCase::testCase6");
    setupPhoneContext(PC_ALL);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_SINGLE, 0, 1);
    startAndSleep(4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 5, 2);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_SINGLE, 5, 0);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 1, 5);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 1, 3);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 5, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase7() {
    gardenPrint("UlpCase::testCase7");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 0, 1);
    startAndSleep(4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 30, 4);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    stopAndSleep(4);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase8() {
    gardenPrint("UlpCase::testCase8");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 0, 1);
    startAndSleep(4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 10, 2);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 10, 4);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 10, 2);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    stopAndSleep(4);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase9() {
    gardenPrint("UlpCase::testCase9");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 0, 1);
    startAndSleep(4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 10, 2);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_SINGLE, 10, 4);
    setupPhoneContext(PC_NETWORK|PC_WIFI);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 10, 2);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 4);
    stopAndSleep(4);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase10() {
    gardenPrint("UlpCase::testCase10");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_SINGLE, 0, 1);
    startAndSleep(5);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_GNSS, ULP_LOC_RECURRENCE_SINGLE, 0, 0);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase11() {
    gardenPrint("UlpCase::testCase11");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 30, 0);
    startAndSleep(10);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 30, 10);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 30, 2);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_HIGH, ULP_LOC_RECURRENCE_PERIODIC, 30, 2);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT UlpCase::ulpCase12() {
    gardenPrint("UlpCase::testCase12");
    setupPhoneContext(PC_GPS|PC_NETWORK|PC_WIFI);
    addCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_PERIODIC, 30, 0);
    startAndSleep(10);
    addCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 10);
    removeCriteriaWithSource(ULP_PROVIDER_SOURCE_HYBRID, ULP_LOC_RECURRENCE_PERIODIC, 30, 2);
    removeCriteriaWithAccuracy(ULP_HORZ_ACCURACY_LOW, ULP_LOC_RECURRENCE_PERIODIC, 30, 2);
    stopAndSleep(3);
    return GARDEN_RESULT_PASSED;
}


void UlpCase::addCase(int idx) {
    switch (idx) {
        // Run test scenario 1
        // GPS tracking session when GPS is enabled
        // Remove GPS tracking session
        case 1:
            GARDEN_ADD(UlpCase, "UlpCase 1",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase1();
                });
            break;
        // Network tracking session when network is enabled
        // Remove network tracking session
        case 2:
            GARDEN_ADD(UlpCase, "UlpCase 2",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase2();
                });
            break;
        // Run test scenario 3
        // GPS tracking session when GPS is disabled
        // Remove GPS tracking session
        //
        // Result: No GPS session should be started
        case 3:
            GARDEN_ADD(UlpCase, "UlpCase 3",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase3();
                });
            break;
        // Run test scenario 4
        // GPS tracking session when GPS is enabled
        // While tracking session is on progress, GPS is disabled
        // Remove GPS tracking session
        //
        // Result: GPS tracking session is started, and then stopped
        // when GPS is disabled
        case 4:
            GARDEN_ADD(UlpCase, "UlpCase 4",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase4();
                });
            break;
        // Run test scenario 5
        // GPS tracking session when GPS is disabled
        // later on, user enables GPS setting
        // later on, GPS tracking session is removed
        //
        // Result: GPS tracking session is only started when gps setting is enabled,
        // and then stopped when the request is removed
        case 5:
            GARDEN_ADD(UlpCase, "UlpCase 5",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase5();
                });
            break;
        // Run test scenario 6
        // GPS single shot session when GPS is ensabled
        // later on, high accuracy tracking (5 second interval) session come in
        // later on, single shot fix request removed (potentially due to position has been satisfied)
        // later on, GPS session (1hz) come in
        // later on, 1hz tracking criteria removed
        // later on, 5hz tracking criteria removed
        // later on, stop request received
        //
        // ulp_brain_choose_providers:|position mode|GPS start| GPS engine
        case 6:
            GARDEN_ADD(UlpCase, "UlpCase 6",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase6();
                });
            break;
        // Run test scenario 7
        // Network provider is enabled
        // First comes network single shot
        // Secondly comes network tracking session (30 seconds)
        // Secondly comes network tracking session (10 seconds)
        // Single shot removed
        // 30 seconds network tracking session removed
        // 10 seconds network tracking session removed
        // ulp stop received
        //
        // Search "ulp_brain_choose_providers:|request type: " for output
        case 7:
            GARDEN_ADD(UlpCase, "UlpCase 7",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase7();
                });
            break;
        // Run test scenario 8
        // GPS and Network provider is enabled
        // First comes network single shot
        // Secondly comes network tracking session (30 seconds)
        // Then comes GPS tracking session (10 seconds)
        // Single shot network is removed
        // GPS tracking session (10 seconds) is removed
        // Network tracking session (30 seconds) is removed
        // ulp stop received
        // Search log for "ulp_brain_choose_providers:|position mode|GPS engine|request type: "
        case 8:
            GARDEN_ADD(UlpCase, "UlpCase 8",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase8();
                });
            break;
        // Run test scenario 9
        // GPS and Network provider is enabled
        // First comes network single shot
        // Secondly comes network tracking session (30 seconds)
        // Then comes GPS tracking session (10 seconds)
        // Single shot network is removed
        // GPS disabled
        // GPS tracking session (10 seconds) is removed
        // Network tracking session (30 seconds) is removed
        // ulp stop received
        //
        // Search log for "ulp_brain_choose_providers:|position mode|GPS engine|request type: "
        case 9:
            GARDEN_ADD(UlpCase, "UlpCase 9",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase9();
                });
            break;
        // Run test scenario 10
        // GPS single shot when GPS is enabled, GPS_CAPABLITY support MSA
        // Remove single shot
        // Output: single shot will be set to tracking with long interval, ,MSA
        case 10:
            GARDEN_ADD(UlpCase, "UlpCase 10",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase10();
                });
            break;
        // Run test scenario 11
        // high accuracy tracking (30 second interval) session come in
        // 10 seconds later, another high accuracy tracking (30 second interval) session come in
        // later on, one 30 second tracking criteria removed
        // later on, the other 30 second tracking criteria removed
        // later on, stop request received
        //
        // ulp_brain_choose_providers:|position mode|GPS start| GPS engine
        case 11:
            GARDEN_ADD(UlpCase, "UlpCase 11",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase11();
                });
            break;
        // Run test scenario 12
        // First, hybrid provider tracking (30 second interval) session come in
        // 10 seconds later, another low accuracy (30 second interval) session come in
        // later on, the 30 second hybrid provider tracking criteria removed
        // later on, the other 30 second low accuracy tracking criteria removed
        // later on, stop request received
        //
        // Search log for "ulp_brain_choose_providers:|position mode|GPS engine|request type: "
        // set up global phone context
        case 12:
            GARDEN_ADD(UlpCase, "UlpCase 12",
                {
                    UlpCase* ulpCase = (UlpCase*)icase;
                    return ulpCase->ulpCase12();
                });
            break;
        default:
            gardenPrint("Invalid Ulp case number: %d", idx);
            break;
    }
}

extern "C" void* getSetPhoneContextMsg() {
    UlpCase::init();
    if (UlpCase::pUlpPhoneContextInterface == nullptr) {
        gardenPrint("Cannot get SetPhoneContextMsg");
        return nullptr;
    }
    Runnable runnable = [] {
            UlpPhoneContextSettings settings;
            settings.context_type = ULP_PHONE_CONTEXT_GPS_SETTING |
                ULP_PHONE_CONTEXT_NETWORK_POSITION_SETTING |
                ULP_PHONE_CONTEXT_WIFI_SETTING |
                ULP_PHONE_CONTEXT_AGPS_SETTING;
            settings.is_gps_enabled = true;
            settings.is_agps_enabled = true;
            settings.is_network_position_available = true;
            settings.is_wifi_setting_enabled = true;
            UlpCase::pUlpPhoneContextInterface->ulp_phone_context_settings_update(&settings);
    };
    gardenPrint("Got SetPhoneContextMsg");
    return (void*)(new LocMsgWrapper(runnable));
}

extern "C" void GARDEN_Plugin(std::string args) {
    gardenPrint("Adding UlpCase with args: %s", args.c_str());
    UlpCase::addCase(atoi(args.c_str()));
}

} // namespace garden
