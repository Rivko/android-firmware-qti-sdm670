/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <GardenFramework.h>
#include <GardenUtil.h>
#include "ZppCase.h"

namespace garden {

ZppCase::ZppCase() {
}

ZppCase::~ZppCase() {
}

GARDEN_RESULT ZppCase::preRun() {
    GARDEN_RESULT ret = UlpCase::preRun();
    GARDEN_ASSERT_NEQ(pUlpEngineInterface, nullptr, "pUlpEngineInterface is nullptr.");
    return ret;
}

GARDEN_RESULT ZppCase::postRun() {
    GARDEN_RESULT ret = UlpCase::postRun();
    return ret;
}

GARDEN_RESULT ZppCase::zppCaseScreenOn() {
    {
        std::lock_guard<mutex> guard(mLock);
        expectLocation();
        pUlpEngineInterface->system_update(ULP_LOC_SCREEN_ON);
    }
    wait();
    gardenPrint("ZPP TestCase: System event screen on passed");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT ZppCase::zppCaseTimeZoneChange() {
    {
        std::lock_guard<mutex> guard(mLock);
        expectLocation();
        pUlpEngineInterface->system_update(ULP_LOC_TIMEZONE_CHANGE);
    }
    wait();
    gardenPrint ("ZPP TestCase:System event time zone change passed");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT ZppCase::zppCasePowerDisconnect() {
    {
        std::lock_guard<mutex> guard(mLock);
        expectLocation();
        pUlpEngineInterface->system_update(ULP_LOC_POWER_CONNECTED);
    }
    wait();

    gardenPrint ("ZPP TestCase:System event Power connected. got 1st loc report");

    {
        std::lock_guard<mutex> guard(mLock);
        expectLocation();
    }
    wait();

    pUlpEngineInterface->system_update(ULP_LOC_POWER_DISCONNECTED);
    gardenPrint ("ZPP TestCase:System event Power connected passed");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT ZppCase::zppCaseThrotting() {
    {
        std::lock_guard<mutex> guard(mLock);
        expectLocation(10);
        pUlpEngineInterface->system_update(ULP_LOC_SCREEN_ON);
        sleep(1);
        pUlpEngineInterface->system_update(ULP_LOC_TIMEZONE_CHANGE);
    }
    wait();

    pUlpEngineInterface->system_update(ULP_LOC_POWER_DISCONNECTED);
    gardenPrint ("ZPP TestCase:verify ZPP throtting passed");
    return GARDEN_RESULT_PASSED;
}

void ZppCase::addCase(int idx) {
    switch (idx) {
        case 1:
            GARDEN_ADD(ZppCase, "zppCaseScreenOn",
                {
                    ZppCase* zppCase = (ZppCase*)icase;
                    return zppCase->zppCaseScreenOn();
                });
            break;
        case 2:
            GARDEN_ADD(ZppCase, "zppCaseTimeZoneChange",
                {
                    ZppCase* zppCase = (ZppCase*)icase;
                    return zppCase->zppCaseTimeZoneChange();
                });
            break;
        case 3:
            GARDEN_ADD(ZppCase, "zppCasePowerDisconnect",
                {
                    ZppCase* zppCase = (ZppCase*)icase;
                    return zppCase->zppCasePowerDisconnect();
                });
            break;
        case 4:
            GARDEN_ADD(ZppCase, "zppCaseThrotting",
                {
                    ZppCase* zppCase = (ZppCase*)icase;
                    return zppCase->zppCaseThrotting();
                });
            break;
        default:
            gardenPrint("Invalid Zpp case number: %d", idx);
            break;
    }
}

extern "C" void GARDEN_Plugin(std::string args) {
    gardenPrint("Adding ZppCase with args: %s", args.c_str());
    ZppCase::addCase(atoi(args.c_str()));
}

} // namespace garden
