/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "DALSys.h"
#include "rcinit.h"
#include "adsppm_test.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_verify.h"
#include "clientmgr.h"
#include "hal_clkrgm.h"
#include "npa.h"
#include "CoreVerify.h"


/**
 * ADSPPM Test Application
 *
 *   Doxygen is used to create the test documentation. Please
 *   use the following conventions for commenting the test code
 *   in order to auto generate the ADSPPM test documentation.
 *
 *   1. Use @ingroup to group the tests together
 *   2. Use @test to define the MMPM test ID, this must match the ID in the CSV output
 *   3. Use @brief to define a 1 line description of the test case.
 *
 *   The test results are collected in a CSV file that is
 *   imported into an Excel document used for presentation of
 *   weekly results. The CSV output contains the following:
 *
 *   ID, Description, result, data, Feature ID, Details
 *
 *   Where:
 *     ID = the MMPM test ID, this should match @test value
 *     Description = Very short test description, function name should be sufficient
 *     result = "Pass" or "Fail" if run
 *     data = Any relevant test results or data, i.e. Failure reason, clock freq., or execution time
 *     Feature ID = Reference back the ADSPPM Feature ID that this test is verifying
 *     details = 1 line text of test details, should match @brief value.
 */

/**
 * Define the groups that Doxygen will used to group the test cases & functions:
 *
 * @defgroup System         MMPM Common and System related tests.
 * @defgroup CoreClock      Core clock request test cases using MMPM_RSC_ID_CORE_CLK.
 * @defgroup DomainClock    Domain clock source request test cases using MMPM_RSC_ID_CORE_CLK_DOMAIN.
 * @defgroup MIPS           MIPS requests test cases using MMPM_RSC_ID_MIPS.
 * @defgroup Sleep          Sleep latency voting test cases using MMPM_RSC_ID_SLEEP_LATENCY.
 * @defgroup Bandwidth      Bandwidth requests test cases using MMPM_RSC_ID_MEM_BW.
 * @defgroup RegProg        AHB test cases using MMPM_RSC_ID_REG_PROG.
 * @defgroup Power          GDHS test cases using MMPM_RSC_ID_POWER.
 * @defgroup MemoryPower    Memory power test cases using MMPM_RSC_ID_MEM_POWER.
 * @defgroup Stress         Stress test cases.
 * @defgroup Profile        Run basic requests to time API call durations.
 */


 typedef struct
 {
    uint32 testResults;
    uint32 numTests;
 } AdsppmTestResultsType;

typedef struct
{
    AdsppmTestTargetType  testTarget;
    ULogHandle            test_hLog;
    uint32                test_log_buffer_size;
    uint32                testLogLevel;
    AdsppmTestResultsType testResults[AdsppmTestId_Max];
    uint32                testTotal;
    uint32                testPassed;
    uint32                testFailed;
} AdsppmTestContexType;

AdsppmTestContexType gAdsppmTestContex;


AdsppmTestTargetType GetTestTarget(void)
{
    return gAdsppmTestContex.testTarget;
}


uint32 GetTestDebugLevel(void)
{
    return gAdsppmTestContex.testLogLevel;
}


uint32 GetTestTotal(void)
{
    return gAdsppmTestContex.testTotal;
}


uint32 GetTestPassed(void)
{
    return gAdsppmTestContex.testPassed;
}


uint32 GetTestFailed(void)
{
    return gAdsppmTestContex.testFailed;
}


ULogHandle GetTestUlogHandle(void)
{
    return gAdsppmTestContex.test_hLog;
}


void updateTestResults(AdsppmTestIdType adsppmTestId, MMPM_STATUS sts)
{
    if(sts == MMPM_STATUS_SUCCESS)
    {
        gAdsppmTestContex.testResults[adsppmTestId].testResults |=
            (0x1 << gAdsppmTestContex.testResults[adsppmTestId].numTests);
        gAdsppmTestContex.testPassed++;
    }
    else
    {
        gAdsppmTestContex.testFailed++;
    }
    gAdsppmTestContex.testResults[adsppmTestId].numTests++;
    gAdsppmTestContex.testTotal++;
}


void adsppmtest_main(void)
{
    // Start task through RCINIT
    rcinit_handshake_startup();

    uint32 result = DAL_ERROR;
    AdsppmTestIdType testId = AdsppmTestId_System;
    gAdsppmTestContex.testTarget = NUM_MAX_TARGETS;
    gAdsppmTestContex.test_log_buffer_size = ADSPPM_TESTULOG_BUFFER_SIZE;
    gAdsppmTestContex.testLogLevel = ADSPPM_TESTULOG_LEVEL;
    gAdsppmTestContex.testTotal = 0;
    gAdsppmTestContex.testPassed = 0;
    gAdsppmTestContex.testFailed = 0;

    for(testId = AdsppmTestId_System; testId < AdsppmTestId_Max; testId++)
    {
        gAdsppmTestContex.testResults[testId].testResults = 0;
        gAdsppmTestContex.testResults[testId].numTests = 0;
    }

    // Initialize ADSPPMTEST ULOG
    result = ULogFront_RealTimeInit(
        &gAdsppmTestContex.test_hLog,
        "ADSPPMTEST Log",
        gAdsppmTestContex.test_log_buffer_size,
        ULOG_MEMORY_LOCAL,
        ULOG_LOCK_OS);

#ifdef ADSPPM_TEST_8994
    gAdsppmTestContex.testTarget = ADSPPM_8994;
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Target detected: 8994");
#endif
#ifdef ADSPPM_TEST_8992
    gAdsppmTestContex.testTarget = ADSPPM_8992;
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Target detected: 8992");
#endif
#ifdef ADSPPM_TEST_8996
    gAdsppmTestContex.testTarget = ADSPPM_8996;
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Target detected: 8996");
#endif
#ifdef ADSPPM_TEST_845
    gAdsppmTestContex.testTarget = ADSPPM_845;
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Target detected: 845");
#endif
#ifdef ADSPPM_TEST_670
    gAdsppmTestContex.testTarget = ADSPPM_670;
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Target detected: 670");
#endif
    AdsppmUTInit();

    if(DAL_SUCCESS == result)
    {
        if(gAdsppmTestContex.testTarget == NUM_MAX_TARGETS)
        {
            PRINT_ERROR_0("No test target detected");
        }
        else
        {
            // Wait for 1 min for CLients initialization to complete
            // With this in place, waking up the device after this point will
            // most likely cause a crash
            DALSYS_BusyWait(60*10000);

            // Routine from where all the tests are invoked
            RunAllTests();

            ADSPPMTEST_LOG_TEST_RESULT_SUMMARY();
        }
    }

    // Shut down task through RCINIT
    rcinit_handshake_shutdown();

    // Initiate RAM dump through inducing fatal error
    CORE_VERIFY(0);
}

