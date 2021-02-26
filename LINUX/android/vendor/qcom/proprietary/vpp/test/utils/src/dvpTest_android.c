/*!
 * @file dvpTest_android.c
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    The SystemC implementation of the testing framework. This
 *              module adheres to the interface defined in dvpTest_system.h
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <libgen.h>
#include <cutils/properties.h>

#define _DVP_TEST_SYSTEM_C_
#include "dvpTest.h"
#include "dvpTest_tb.h"
#include "dvpTest_system.h"

#include "vpp_def.h"
#include "vpp_dbg.h"

#include <utils/Log.h>

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define FILE_NAME "/data/test/output/log.txt"
#define TEST_ASSERT_STRING_LEN 256

typedef struct AssertFail {
    struct AssertFail * pNext;
    uint32_t line;
    const char * strFile;
    const char * strFunc;
    char strCond[TEST_ASSERT_STRING_LEN];
} t_AssertFail;

typedef struct {
    FILE * fp;
    t_AssertFail * pAssertHead;
} t_SystemCb;

/************************************************************************
 * Local static variables
 ***********************************************************************/
t_SystemCb * pCb;
static uint32_t u32FancyOutput;
/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

void SYSTEM_LOG(const char * fmt, ...)
{
    // Log to console
    if(VPP_TRUE)
    {
        va_list arg_p;
        va_start(arg_p, fmt);
        vprintf(fmt, arg_p);
        va_end(arg_p);
    }
    // Log to file
    if(pCb->fp)
    {
        va_list arg_f;
        va_start(arg_f, fmt);
        vfprintf(pCb->fp, fmt, arg_f);
        va_end(arg_f);
    }
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

void vDvpTestSystem_Init()
{
    char property_value[PROPERTY_VALUE_MAX] = {0};
    property_get("vendor.media.vpp.fancyunittest", property_value, "0");
    u32FancyOutput = strtoull(property_value, NULL, 0);

    pCb = (t_SystemCb *)malloc(sizeof(t_SystemCb));
    if (pCb)
        memset(pCb, 0, sizeof(t_SystemCb));
}

void vDvpTestSystem_Term()
{
    if (pCb)
        free(pCb);
}

void vDvpTestSystem_SuiteAdd(t_TestSuite *pSuite)
{
    SYSTEM_LOG("Adding test suite: %s\n", pSuite->pName);
}

void vDvpTestSystem_TestAdd(t_TestCase *pTest)
{
    SYSTEM_LOG(">> Adding test: %s\n", pTest->pName);
}

void vDvpTestSystem_TestbenchStart()
{
    pCb->fp = fopen(FILE_NAME, "a");

    time_t rawtime;
    time(&rawtime);

    SYSTEM_LOG("Test Report:\n");
    SYSTEM_LOG("Generated: %s\n", ctime(&rawtime));
}

void vDvpTestSystem_TestbenchFinish(t_TbSummary * pSummary)
{
    uint32_t duration = pSummary->timeEnd - pSummary->timeStart;
    SYSTEM_LOG(TEST_BENCH_SUMMARY,
            pSummary->suitesRun,
            pSummary->suitesPassed,
            pSummary->suitesFailed,
            pSummary->testsRun,
            pSummary->testsPassed,
            pSummary->testsFailed,
            duration);

    if (pCb->fp)
        fclose(pCb->fp);
}

void vDvpTestSystem_TestStart(t_TbTest * pTest)
{
    time_t rawtime;
    time(&rawtime);

    SYSTEM_LOG(TEST_RUN, pTest->pName);
    LOGD("*************************************************");
    LOGD("* Test: %s", pTest->pName);
    LOGD("* Time: %s", ctime(&rawtime));
    LOGD("*************************************************");
}

void vDvpTestSystem_TestFinish(t_TbTest * pTest)
{
    const char * n = pTest->pName;
    uint32_t a = pTest->summary.assertCnt;
    uint32_t p = pTest->summary.assertPass;
    uint32_t f = pTest->summary.assertFail;
    uint32_t d = pTest->summary.testEnd - pTest->summary.testStart;
    const char * r = f ? FAIL_STR : PASS_STR;

    if (u32FancyOutput)
    {
        SYSTEM_LOG("\033[1A");
        SYSTEM_LOG("\033[K");
    }

    SYSTEM_LOG(TEST_SUMMARY, n, a, p, f, d, r);

    // Log all the asserts that occurred during the test run
    t_AssertFail * ptr = pCb->pAssertHead;
    t_AssertFail * prev = NULL;
    while(ptr != NULL)
    {
        SYSTEM_LOG(TEST_ASSERT, ptr->line, ptr->strFile, ptr->strFunc, ptr->strCond);
        prev = ptr;
        ptr = ptr->pNext;
        free(prev);
        prev = NULL;
    }
    pCb->pAssertHead = NULL;
}

void vDvpTestSystem_SuiteStart(t_TbTestSuite * pSuite)
{
    SYSTEM_LOG(TEST_SUITE_START, pSuite->pName);
}

void vDvpTestSystem_SuiteFinish(t_TbTestSuite * pSuite)
{
    uint32_t t = pSuite->summary.testsRun;
    uint32_t p = pSuite->summary.testsPassed;
    uint32_t f = pSuite->summary.testsFailed;
    uint32_t d = pSuite->summary.suiteEnd - pSuite->summary.suiteStart;
    SYSTEM_LOG(TEST_SUITE_SUMMARY, t, p, f, d);
}

void vDvpTestSystem_AssertFail(uint32_t line,
        const char * strCond,
        const char * strFile,
        const char * strFunc)
{
    t_AssertFail * ptr = (t_AssertFail *)malloc(sizeof(t_AssertFail));

    if (!ptr)
    {
        LOGE("TEST_BENCH: unable to log assert");
        return;
    }

    memset(ptr, 0, sizeof(t_AssertFail));
    ptr->line = line;
    ptr->strFile = strFile;
    ptr->strFunc = strFunc;
    strlcpy(ptr->strCond, strCond, TEST_ASSERT_STRING_LEN);

    LOGE("ASSERT_FAILED: Line: %u File: %s Function: %s", line, strFile, strFunc);
    LOGE("               Cond: %s", strCond);

    if(pCb->pAssertHead == NULL)
    {
        pCb->pAssertHead = ptr;
    }
    else
    {
        t_AssertFail * cur = pCb->pAssertHead;
        while(cur->pNext != NULL)
        {
            cur = cur->pNext;
        }
        cur->pNext = ptr;
    }
}

uint32_t u32DvpTest_GetTime()
{
    time_t rawtime;
    time(&rawtime);
    return rawtime;
}

t_TbTest * pstGetTbTest()
{
    return (t_TbTest *)malloc(sizeof(t_TbTest));
}

void vFreeTbTest(t_TbTest * pst)
{
    free(pst);
}

t_TbTestSuite * pstGetTbTestSuite()
{
    return (t_TbTestSuite *)malloc(sizeof(t_TbTestSuite));
}

void vFreeTbTestSuite(t_TbTestSuite * pst)
{
    free(pst);
}

t_TestBenchCb * pstGetTestBenchCb()
{
    return (t_TestBenchCb *)malloc(sizeof(t_TestBenchCb));
}

void vFreeTestBenchCb(t_TestBenchCb * pst)
{
    free(pst);
}
