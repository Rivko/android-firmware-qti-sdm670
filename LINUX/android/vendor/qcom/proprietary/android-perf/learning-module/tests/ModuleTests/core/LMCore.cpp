/******************************************************************************
 *   @file    LMCore.cpp
 *   @brief   Implementation of Learning Module Core test
 *
 *   DESCRIPTION
 *      Implements simple module level LM Core test which uses perf-hal hints
 *   to send a sequence of hints to test LM Core.
 *      It also implements api to send debug idle trigger which helps in
 *   manually triggering algorithms.
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 ********************************************************************************/

#include "AsyncData.h"
#include "PerfHintHelper.h"
#include "Debug.h"
#include "LMCore.h"

#define SLEEP_TIME 5
int LMCoreTest() {
    PerfHintHelper perflib;
    DEBUGTEST("Calling perf Hint (VENDOR_HINT_FIRST_LAUNCH_BOOST)\n");
    perflib.perfHint(VENDOR_HINT_FIRST_LAUNCH_BOOST, "com.dummy.app1");
    perflib.perfHint(VENDOR_HINT_FIRST_LAUNCH_BOOST, "com.dummy.app2");
    perflib.perfHint(VENDOR_HINT_FIRST_LAUNCH_BOOST, "com.dummy.app3");
    DEBUGTEST("perf hint (VENDOR_HINT_FIRST_LAUNCH_BOOST) returned\n");
    sleep(SLEEP_TIME);
    DEBUGTEST("Calling perf hint (VENDOR_HINT_FIRST_DRAW)\n");
    perflib.perfHint(VENDOR_HINT_FIRST_DRAW, "com.dummy.app1");
    perflib.perfHint(VENDOR_HINT_FIRST_DRAW, "com.dummy.app2");
    perflib.perfHint(VENDOR_HINT_FIRST_DRAW, "com.dummy.app3");
    DEBUGTEST("perf hint (VENDOR_HINT_FIRST_DRAW) returned\n");
    sleep(SLEEP_TIME);
    DEBUGTEST("Calling perf hint (VENDOR_HINT_DISPLAY_OFF)\n");
    perflib.perfHint(VENDOR_HINT_DISPLAY_OFF, "", 0, LMTESTS_HINTTYPE);
    DEBUGTEST("perf hint VENDOR_HINT_DISPLAY_OFF returned\n");
    sleep(SLEEP_TIME);
    return 0;
}

void lmSendIdle() {
    PerfHintHelper perflib;
    DEBUGTEST("Calling perf_hint(VENDOR_HINT_DISPLAY_OFF)\n");
    perflib.perfHint(VENDOR_HINT_DISPLAY_OFF, "", 0, LMTESTS_HINTTYPE);
    DEBUGTEST("perf_hint() returned\n");
}
