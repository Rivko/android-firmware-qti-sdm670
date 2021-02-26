/*!
 * @file test_core.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "vpp_dbg.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
enum {
    VPP_ARG_SUITE = 1,
    VPP_ARG_TEST,
};

typedef struct {
    uint32_t bTest;
    char *pcTest;
    uint32_t bSuite;
    char *pcSuite;
} t_StVppTestArgs;

/************************************************************************
 * Local static variables
 ***********************************************************************/
uint64_t u64LogLevelUnit = 0xffffffff;

/************************************************************************
 * Forward Declarations
 ************************************************************************/
#define ENABLE_SUITE_QUEUE      1
#define ENABLE_SUITE_BUF        1
#define ENABLE_SUITE_UC         1
#define ENABLE_SUITE_GPU        1
#define ENABLE_SUITE_PL         1
#define ENABLE_SUITE_VPP        1
#define ENABLE_SUITE_HVX        1
#define ENABLE_SUITE_ME         1
#define ENABLE_SUITE_MC         1
#define ENABLE_SUITE_IP         1
#define ENABLE_SUITE_STATS      1
#define ENABLE_SUITE_HCP        1
#define ENABLE_SUITE_VPPUTILS   1
#define ENABLE_SUITE_REG        1

#if ENABLE_SUITE_QUEUE
TEST_SUITE_DECLARE(QueueSuite);
#endif

#if ENABLE_SUITE_BUF
TEST_SUITE_DECLARE(BufSuite);
#endif

#if ENABLE_SUITE_UC
TEST_SUITE_DECLARE(UsecaseSuite);
#endif

#if ENABLE_SUITE_GPU
#ifdef VPP_TARGET_USES_GPU
TEST_SUITE_DECLARE(IpGpuSuite);
TEST_SUITE_DECLARE(IpGpuNoInitSuite);
#endif
#endif

#if ENABLE_SUITE_PL
TEST_SUITE_DECLARE(PipelineSuite);
#endif

#if ENABLE_SUITE_VPP
TEST_SUITE_DECLARE(VppSuite);
#endif

#if ENABLE_SUITE_HVX
TEST_SUITE_DECLARE(IpHvxSuite);
#endif

#if ENABLE_SUITE_ME
TEST_SUITE_DECLARE(IpMeSuite);
#endif

#if ENABLE_SUITE_MC
TEST_SUITE_DECLARE(IpMcSuite);
#endif

#if ENABLE_SUITE_IP
TEST_SUITE_DECLARE(IpSuite);
#endif

#if ENABLE_SUITE_STATS
TEST_SUITE_DECLARE(StatsSuite);
#endif

#if ENABLE_SUITE_REG
TEST_SUITE_DECLARE(RegSuite);
#endif

#if ENABLE_SUITE_HCP
#ifdef VPP_TARGET_USES_HCP
TEST_SUITE_DECLARE(HcpSuite);
#endif
#endif

#if ENABLE_SUITE_VPPUTILS
TEST_SUITE_DECLARE(VppUtilsSuite);
#endif

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void term_args(t_StVppTestArgs *pstArgs)
{
    if (!pstArgs)
        return;

    if (pstArgs->pcTest)
        free(pstArgs->pcTest);
    pstArgs->pcTest = NULL;


    if (pstArgs->pcSuite)
        free(pstArgs->pcSuite);
    pstArgs->pcSuite = NULL;
}

static void parse_args(t_StVppTestArgs *pstArgs, int argc, char **argv)
{

    int c = 0;
    while (c != -1)
    {
        static struct option long_options[] =
        {
            { "suite", required_argument, NULL, VPP_ARG_SUITE },
            { "test", required_argument, NULL, VPP_ARG_TEST },
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long_only (argc, argv, "",
                              long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case VPP_ARG_SUITE:
                printf("SUITE requested, suite=%s\n", optarg);
                pstArgs->bSuite = VPP_TRUE;
                pstArgs->pcSuite = malloc(strlen(optarg) + 1);
                if (!pstArgs->pcSuite)
                {
                    LOGE("ERROR: Could not allocate memory for pcSuite!");
                    return;
                }
                memcpy(pstArgs->pcSuite, optarg, strlen(optarg) + 1);
                break;
            case VPP_ARG_TEST:
                printf("TEST requested, test=%s\n", optarg);
                pstArgs->bTest = VPP_TRUE;
                pstArgs->pcTest = malloc(strlen(optarg) + 1);
                if (!pstArgs->pcTest)
                {
                    LOGE("ERROR: Could not allocate memory for pcTest!");
                    return;
                }
                memcpy(pstArgs->pcTest, optarg, strlen(optarg) + 1);
                break;
            default:
                LOGE("ERROR: unknown parameter, returned 0%o!", c);
                break;
        }

    }

}

/************************************************************************
 * Global Functions
 ***********************************************************************/
int main(int argc, char **argv)
{
    t_StVppTestArgs stArgs;
    memset(&stArgs, 0, sizeof(t_StVppTestArgs));

    // For tests that require random numbers, seed this once.
    srand(time(NULL));

    parse_args(&stArgs, argc, argv);

    // Initialize the test bench
    vDvpTb_Init();

    // Add the test suites to the framework
#if ENABLE_SUITE_QUEUE
    TEST_SUITE_INSTALL(QueueSuite);
#endif

#if ENABLE_SUITE_BUF
    TEST_SUITE_INSTALL(BufSuite);
#endif

#if ENABLE_SUITE_UC
    TEST_SUITE_INSTALL(UsecaseSuite);
#endif

#if ENABLE_SUITE_GPU
#ifdef VPP_TARGET_USES_GPU
    TEST_SUITE_INSTALL(IpGpuSuite);
    TEST_SUITE_INSTALL(IpGpuNoInitSuite);
#endif
#endif

#if ENABLE_SUITE_HVX
#ifdef VPP_TARGET_USES_HVX
    TEST_SUITE_INSTALL(IpHvxSuite);
#endif
#endif

#if ENABLE_SUITE_PL
    TEST_SUITE_INSTALL(PipelineSuite);
#endif

#if ENABLE_SUITE_VPP
    TEST_SUITE_INSTALL(VppSuite);
#endif

#if ENABLE_SUITE_ME
#ifdef VPP_TARGET_USES_FRC
    TEST_SUITE_INSTALL(IpMeSuite);
#endif
#endif

#if ENABLE_SUITE_MC
#ifdef VPP_TARGET_USES_FRC
    TEST_SUITE_INSTALL(IpMcSuite);
#endif
#endif

#if ENABLE_SUITE_IP
    TEST_SUITE_INSTALL(IpSuite);
#endif

#if ENABLE_SUITE_STATS
    TEST_SUITE_INSTALL(StatsSuite);
#endif

#if ENABLE_SUITE_REG
    TEST_SUITE_INSTALL(RegSuite);
#endif

#if ENABLE_SUITE_HCP
#ifdef VPP_TARGET_USES_HCP
    TEST_SUITE_INSTALL(HcpSuite);
#endif
#endif

#if ENABLE_SUITE_VPPUTILS
    TEST_SUITE_INSTALL(VppUtilsSuite);
#endif

    uint32_t bRun = VPP_FALSE;

    if (stArgs.bSuite)
    {
        vDvpTb_RunSuite(stArgs.pcSuite);
        bRun = VPP_TRUE;
    }

    if (stArgs.bTest)
    {
        vDvpTb_RunTest(NULL, stArgs.pcTest);
        bRun = VPP_TRUE;
    }

    if (!bRun)
    {
        // Start running the test bench
        vDvpTb_RunTests();
    }

    // Terminate the test bench
    vDvpTb_Term();

    term_args(&stArgs);

    return 0;
}
