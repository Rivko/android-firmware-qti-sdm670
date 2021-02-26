/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GardenFramework.h"
#include "GardenUtil.h"
#include "SampleCase.h"

namespace garden {

SampleCase::SampleCase() {
}

SampleCase::~SampleCase() {
}

bool SampleCase::isLeapYear(int32_t year) {
    if (year == 0) {
        // throw an exception when yeas == 0
        // there is no such year
        throw year;
    }

    /* comment this out to trigger failed case
    if (year % 3200 == 0)
        return false;
    */
    if ((year % 100) == 0 && (year % 400 != 0))
        return false;
    if (year % 4 != 0) {
        return false;
    }
    return true;
}

GARDEN_RESULT SampleCase::preRun() {
    // this function will be called before every case.
    gardenPrint("preRun");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT SampleCase::postRun() {
    // this function will be called after every case.
    gardenPrint("postRun");
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT SampleCase::failedCase() {
    GARDEN_ASSERT_EQ(isLeapYear(3200), false, "Year 3200 is NOT leap year.");
    return GARDEN_RESULT_PASSED;
}

extern "C" void GARDEN_Plugin(std::string args) {
    gardenPrint("Adding SampleCase with args: %s", args.c_str());
    GARDEN_ADD(SampleCase, "PassedCase",
        {
            // There is alway a variable 'icase' we can use inside GARDEN_ADD.
            SampleCase* sampleCase = (SampleCase*)icase;
            GARDEN_ASSERT_EQ(sampleCase->isLeapYear(1900), false, "Year 1900 is NOT leap year.");
            GARDEN_ASSERT_EQ(sampleCase->isLeapYear(2000), true, "Year 2000 is leap year.");
            // We can simply put code blocks into GARDEN_ADD.
            // if we don't return any value, it will return GARDEN_RESULT_PASSED automatically.
        });
    GARDEN_ADD(SampleCase, "FailedCase",
        {
            // We can return a value explictly.
            SampleCase* sampleCase = (SampleCase*)icase;
            return sampleCase->failedCase();
        });
    GARDEN_ADD(SampleCase, "ExceptionCase",
        {
            // This will throw an exception.
            // Cases after this one should not be effected.
            SampleCase* sampleCase = (SampleCase*)icase;
            GARDEN_ASSERT_EQ(sampleCase->isLeapYear(0), false, "Year 0 is NOT a valid year.");
        });
    GARDEN_ADD(SampleCase, "AbortCase",
        {
            // This will abort the entire GardenFramework.
            // Cases after this one will be ignored.
            gardenPrint("You shall not pass.");
            return GARDEN_RESULT_ABORT;
        });
    GARDEN_ADD(SampleCase, "IgnoredCase",
        {
            // We will never be here.
            gardenPrint("You weren't supposed to see me.");
            return GARDEN_RESULT_PASSED;
        });
}

} // namespace garden
