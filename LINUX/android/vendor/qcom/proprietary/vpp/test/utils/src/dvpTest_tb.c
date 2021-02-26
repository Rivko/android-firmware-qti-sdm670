/*!
 * @file dvpTest_tb.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services   Implementation of the testing framework
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dvpTest.h"

#define _DVP_TEST_TB_C_
#include "dvpTest_tb.h"
#include "dvpTest_system.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/
static t_TestBenchCb * pTbCb;
static const char *pCurrentTestName;

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void vRunTestbenchStart()
{
    pTbCb->summary.timeStart = u32DvpTest_GetTime();
    vDvpTestSystem_TestbenchStart();
}

static void vRunTestbenchFinish()
{
    pTbCb->summary.timeEnd = u32DvpTest_GetTime();
    vDvpTestSystem_TestbenchFinish(&pTbCb->summary);
}

static void vRunSuiteStart(t_TbTestSuite * pSuite)
{
    pTbCb->pSuiteCur = pSuite;
    pTbCb->summary.suitesRun++;
    pSuite->summary.suiteStart = u32DvpTest_GetTime();

    vDvpTestSystem_SuiteStart(pSuite);
    if(pSuite->initFnc != NULL)
    {
        (*pSuite->initFnc)();
    }
}

static void vRunSuiteFinish(t_TbTestSuite * pSuite)
{
    if(pSuite->termFnc != NULL)
    {
        (*pSuite->termFnc)();
    }
    pTbCb->pSuiteCur = NULL;
    pSuite->summary.suiteEnd = u32DvpTest_GetTime();

    // Update the testbench summary based on the results of the suite
    if(pSuite->summary.testsFailed)
    {
        pTbCb->summary.suitesFailed++;
    }
    else
    {
        pTbCb->summary.suitesPassed++;
    }

    vDvpTestSystem_SuiteFinish(pSuite);
}

static void vRunTestStart(t_TbTestSuite * pSuite, t_TbTest * pTest)
{
    pTbCb->pTestCur = pTest;
    pTbCb->summary.testsRun++;
    pSuite->summary.testsRun++;
    pTest->summary.testStart = u32DvpTest_GetTime();
    vDvpTestSystem_TestStart(pTest);
    if(pSuite->setupFnc != NULL)
    {
        (*pSuite->setupFnc)();
    }
}

static void vRunTestFinish(t_TbTestSuite * pSuite, t_TbTest * pTest)
{
    if(pSuite->clnFnc != NULL)
    {
        (*pSuite->clnFnc)();
    }
    // Update the test pass or fail count in the test suite summary
    if(pTest->summary.assertFail)
    {
        pTbCb->summary.testsFailed++;
        pSuite->summary.testsFailed++;
    }
    else
    {
        pTbCb->summary.testsPassed++;
        pSuite->summary.testsPassed++;
    }

    pTbCb->pTestCur = NULL;
    pTest->summary.testEnd = u32DvpTest_GetTime();
    vDvpTestSystem_TestFinish(pTest);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
void vDvpTb_Init()
{
    pTbCb = pstGetTestBenchCb();
    if(pTbCb != NULL)
    {
        memset(pTbCb, 0, sizeof(t_TestBenchCb));
    }
    vDvpTestSystem_Init();
}

void vDvpTb_Term()
{
    t_TbTestSuite *pSuite = pTbCb->pHead;
    t_TbTestSuite *pSuiteTmp = NULL;
    t_TbTest *pTest, *pTestTmp = NULL;

    while(pSuite != NULL)
    {
        pTest = pSuite->pFirstTest;
        while(pTest != NULL)
        {
            pTestTmp = pTest;
            pTest = pTest->pNext;
            vFreeTbTest(pTestTmp);
        }
        pSuiteTmp = pSuite;
        pSuite = pSuite->pNext;
        vFreeTbTestSuite(pSuiteTmp);
    }
    vFreeTestBenchCb(pTbCb);
    vDvpTestSystem_Term();
}

void vDvpTb_AddSuite(t_TestSuite * pSuite)
{
    // Allocate memory for a test suite
    // Set the params in the test suite to those that were passed in
    // For each test, create a test node and add it to the linked list.
    // Add the test suite to the top level control block, at the tail

    t_TbTestSuite * pTestSuite = pstGetTbTestSuite();
    if(pTestSuite != NULL)
    {
        memset(pTestSuite, 0, sizeof(t_TbTestSuite));

        vDvpTestSystem_SuiteAdd(pSuite);
        pTestSuite->pName = pSuite->pName;
        pTestSuite->initFnc = pSuite->fpInit;
        pTestSuite->termFnc = pSuite->fpTerm;
        pTestSuite->setupFnc = pSuite->fpTestSetup;
        pTestSuite->clnFnc = pSuite->fpTestCleanup;
        pTestSuite->pNext = NULL;
        pTestSuite->pPrev = pTbCb->pTail;
        t_TestCase * ptr = pSuite->pTests;
        t_TbTest * pTestPrev = NULL;

        while(ptr->pName != NULL && ptr->fpTest != NULL)
        {
            t_TbTest * pTest = pstGetTbTest();
            if (pTest)
            {
                memset(pTest, 0, sizeof(t_TbTest));
                vDvpTestSystem_TestAdd(ptr);
                pTest->pName = ptr->pName;
                pTest->pSuite = pTestSuite;
                pTest->testFunc = ptr->fpTest;
                pTest->pNext = NULL;
                if(pTestPrev != NULL)
                {
                    pTestPrev->pNext = pTest;
                }
                pTest->pPrev = pTestPrev;
                if(pTestSuite->pFirstTest == NULL)
                {
                    pTestSuite->pFirstTest = pTest;
                }
                pTestPrev = pTest;
            }
            ptr++;
        }
        // Add the new test suite to the control block at the tail
        if(pTbCb->pHead == NULL)
        {
            pTbCb->pHead = pTestSuite;
            pTbCb->pTail = pTbCb->pHead;
        }
        else
        {
            pTbCb->pTail->pNext = pTestSuite;
            pTbCb->pTail = pTestSuite;
        }
    }
}

static void vDvpTb_RunSuiteSingle(t_TbTestSuite *pSuite)
{
    t_TbTest * pTest;

    if (!pSuite)
        return;

    vRunSuiteStart(pSuite);
    pTest = pSuite->pFirstTest;
    while(pTest != NULL)
    {
        pCurrentTestName = pTest->pName;

        vRunTestStart(pSuite, pTest);
        (*pTest->testFunc)();
        vRunTestFinish(pSuite, pTest);

        pCurrentTestName = NULL;
        pTest = pTest->pNext;
    }
    vRunSuiteFinish(pSuite);
}

void vDvpTb_RunTests()
{
    // Foreach test suite in the testbench
    //     Call the init function
    //     Foreach test in the test suite
    //          Log the test start
    //          Invoke the test function
    //          Log the test end
    //     Call the term function

    vRunTestbenchStart();
    t_TbTestSuite * pSuite = pTbCb->pHead;

    while(pSuite != NULL)
    {
        vDvpTb_RunSuiteSingle(pSuite);
        pSuite = pSuite->pNext;
    }
    vRunTestbenchFinish();
}

static t_TbTestSuite *pstFindSuite(const char *pcSuiteName)
{
    // int i = 0;
    // printf("searching for suite: %s\n", pcSuiteName);
    t_TbTestSuite *pSuite = pTbCb->pHead;
    while (pSuite)
    {
        // printf("searching suite: %d (%s)... ", i++, pSuite->pName);
        if (!strcmp(pcSuiteName, pSuite->pName))
        {
            // printf("found\n");
            return pSuite;
        }
        // printf("not found\n");
        pSuite = pSuite->pNext;
    }
    return NULL;
}

static t_TbTest *pstFindTest(const char *pcTestName)
{
    t_TbTestSuite * pSuite = pTbCb->pHead;
    t_TbTest * pTest;

    // int i, j;
    // i = 0;
    while(pSuite != NULL)
    {
        pTest = pSuite->pFirstTest;
        // j = 0;
        // printf("searching suite: %d (%s) for test: %s\n", i++, pSuite->pName, pcTestName);
        while(pTest != NULL)
        {
            // printf("comparing test %d (%s)...", j++, pcTestName);
            if (!strcmp(pcTestName, pTest->pName))
            {
                // printf("found!\n");
                return pTest;
            }
            // printf("not found \n");
            pTest = pTest->pNext;
        }
        pSuite = pSuite->pNext;
    }
    return NULL;
}

void vDvpTb_RunSuite(const char *pcSuiteName)
{
    t_TbTestSuite *pSuite = pstFindSuite(pcSuiteName);
    if (pSuite)
    {
        // run the tests
        vRunTestbenchStart();
        vDvpTb_RunSuiteSingle(pSuite);
        vRunTestbenchFinish();
    }
    else
    {
        SYSTEM_LOG("ERROR: unable to find suite: %s", pcSuiteName);
    }
}

void vDvpTb_RunTest(const char *pcTestSuite, const char *pcTestName)
{
    if (!pcTestName)
    {
        if (pcTestSuite)
            vDvpTb_RunSuite(pcTestSuite);
        return;
    }

    t_TbTest *pTest = pstFindTest(pcTestName);
    if (pTest)
    {
        // run the test
        vRunTestbenchStart();
        vRunSuiteStart(pTest->pSuite);

        pCurrentTestName = pTest->pName;
        vRunTestStart(pTest->pSuite, pTest);
        (*pTest->testFunc)();
        vRunTestFinish(pTest->pSuite, pTest);
        pCurrentTestName = NULL;

        vRunSuiteFinish(pTest->pSuite);
        vRunTestbenchFinish();
    }
    else
    {
        SYSTEM_LOG("ERROR: unable to find test: %s", pcTestName);
    }
}

const char *pGetCurrentTestName()
{
    return pCurrentTestName;
}

void vDvpTb_Assert(int cond,
        uint32_t line,
        const char * strCond,
        const char * strFile,
        const char * strFunc)
{
    t_TbTest * pTest = pTbCb->pTestCur;

    if (pTest)
        pTest->summary.assertCnt++;

    if(!cond)
    {
        if (pTest)
            pTest->summary.assertFail++;
        vDvpTestSystem_AssertFail(line, strCond, strFile, strFunc);
    }
    else
    {
        if (pTest)
            pTest->summary.assertPass++;
    }
}
