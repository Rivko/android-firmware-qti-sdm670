/******************************************************************************
 *   @file    lmtests.cpp
 *   @brief   Implementation of Android Framework Learning Module
 *
 *   DESCRIPTION
 *
 * ---------------------------------------------------------------------------
 *          Copyright (c) 2017 Qualcomm Technologies, Inc.
 *          All Rights Reserved.
 *          Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 ********************************************************************************/

#include <iostream>
#include <string>
#include "Debug.h"
#include "LMDBTest.h"
#include "LMCore.h"

int main(int argc, char* argv[]){
    bool runAllTests = false;
    /* No option specified, run all tests */
    if(argc == 1) {
        runAllTests = true;
        DEBUGTEST("Running All LM Tests()\n");
    }

    std::string option;
    if(argc == 2) {
        /* Option is specified, run test accordingly */
        option = argv[1];
    }

    /*
     * Idle is not a test - it's a debug feature,
     * so it shouldn't be run during runAllTests
     */
    if (option == "idle") {
        DEBUGTEST("Calling lmSendIdle()\n");
        lmSendIdle();
        DEBUGTEST("lmSendIdle() returned\n");
    }

    if (option == "core" || runAllTests) {
        DEBUGTEST("Calling LMCoreTest()\n");
        LMCoreTest();
        DEBUGTEST("LMCoreTest() returned\n");
    }

    if(option == "lmdb" || runAllTests) {
        DEBUGTEST("Calling LMDBTest()\n");
        LMDBTest();
        DEBUGTEST("LMDBTest() returned\n");
    }
    return 0;
}
