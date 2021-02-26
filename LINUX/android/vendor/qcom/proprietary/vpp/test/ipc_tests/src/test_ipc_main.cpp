/*!
 * @file test_ipc_main.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <hidl/LegacySupport.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"
#include "vpp_dbg.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/
TEST_SUITE_DECLARE(vppHidlUtilsSuite);
TEST_SUITE_DECLARE(vppClientSuite);
TEST_SUITE_DECLARE(vppSvcSuite);

/************************************************************************
 * Local Functions
 ***********************************************************************/

 /************************************************************************
 * Global Functions
 ***********************************************************************/
int main(int argc, char **argv)
{
    // Initialize the test bench
    vDvpTb_Init();

    VPP_UNUSED(argc);
    VPP_UNUSED(argv);

    // Add the test suites to the framework
    TEST_SUITE_INSTALL(vppHidlUtilsSuite);
    TEST_SUITE_INSTALL(vppClientSuite);
    TEST_SUITE_INSTALL(vppSvcSuite);

    configureRpcThreadpool(10, false /* callerWillJoin */);
    // otherwise, incoming callbacks from the vppService would block the
    // main thread (we have to do this as we are a "Binder server").

    // Start running the test bench
    vDvpTb_RunTests();

    // Terminate the test bench
    vDvpTb_Term();

    return 0;
}