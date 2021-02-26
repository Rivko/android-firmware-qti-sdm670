/*!
 * @file dvpTest_tb.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Public interface to the testing framework's testbench
 */

#ifndef _DVP_TEST_TB_H_
#define _DVP_TEST_TB_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t suitesRun;           // The number of suites run
    uint32_t suitesPassed;        // The number of suites passed
    uint32_t suitesFailed;        // The number of suites failed
    uint32_t testsRun;            // The number of tests run
    uint32_t testsPassed;         // The number of tests passed
    uint32_t testsFailed;         // The number of tests failed
    uint32_t timeStart;           // Start time of the testbench
    uint32_t timeEnd;             // End time of the testbench
} t_TbSummary;

typedef struct {
    uint32_t testsRun;            // Number of tests run in the suite
    uint32_t testsPassed;         // Number of tests that passed
    uint32_t testsFailed;         // Number of tests that failed
    uint32_t suiteStart;          // The time at which the suite started running
    uint32_t suiteEnd;            // The time at which the suite stopped running
} t_TestSuiteSummary;

typedef struct {
    uint32_t assertCnt;           // The number of asserts for the test
    uint32_t assertPass;          // The number of passed asserts
    uint32_t assertFail;          // The number of failed asserts
    uint32_t testStart;           // The time at which the test started running
    uint32_t testEnd;             // The time at which the test stopped running
} t_TestSummary;

struct TbTestSuite;

typedef struct TbTest {
    const char * pName;         // Name of the test
    struct TbTestSuite *pSuite; // Suite that this test belongs to
    struct TbTest * pNext;      // Next test in the suite
    struct TbTest * pPrev;      // Previous test in the suite
    t_fpTestFunc testFunc;      // Function pointer to the test function
    t_TestSummary summary;      // The summary report for this test
} t_TbTest;

typedef struct TbTestSuite {
    const char * pName;         // Name of the test suite
    t_fpSuiteInitFunc initFnc;  // Suite init function poiter
    t_fpSuiteTermFunc termFnc;  // Suite term function pointer
    t_fpTestSetupFunc setupFnc; // Test init func; called before every test
    t_fpTestCleanupFunc clnFnc; // Test cleanup func; called after every test
    struct TbTestSuite * pNext; // Next suite in the testbench
    struct TbTestSuite * pPrev; // Previous suite in the testbench
    struct TbTest * pFirstTest; // First test in the suite
    t_TestSuiteSummary summary; // The summary report for the suite
} t_TbTestSuite;

typedef struct {
    struct TbTestSuite * pHead; // Head of the test suite
    struct TbTestSuite * pTail; // Tail of the test suite
    struct TbTestSuite * pSuiteCur;  // Currently executing test suite
    struct TbTest* pTestCur;    // Currently executing test suite
    t_TbSummary summary;        // The summary report for the suite
} t_TestBenchCb;

/*!
 * @brief       Initializes the testbench.
 * @caveats     Must be called prior to calling anything else in the testbench
 * @return      None
 */
void vDvpTb_Init();

/*!
 * @brief       Terminates the testbench.
 * @caveats     Must be called when the testbench is no longer in use.
 * @return      None
 */
void vDvpTb_Term();

/*!
 * @brief       Adds a test suite to the testbench.
 * @return      None
 */
void vDvpTb_AddSuite(t_TestSuite * pSuite);

/*!
 * @brief       Runs all of the tests in the testbench
 * @return      None
 */
void vDvpTb_RunTests();

/*!
 * @brief       Runs all of the tests in the specified test suite
 * @return      None
 */
void vDvpTb_RunSuite(const char *pcSuiteName);

/*!
 * @brief       Runs the test by name. If the suite is defined, try to find
 *              the test in that test suite.
 * @return      None
 */
void vDvpTb_RunTest(const char *pcTestSuite, const char *pcTestName);

/*!
 * @brief       Gets the name of the currently running test
 * @return      Name of the current running test
 */
const char *pGetCurrentTestName();

/*!
 * @brief       Implementation of an assert. This should be called through
 *              the helper assert macros.
 * @input       cond    The condition to test. 0 = False, else True
 * @input       line    The line that this assert is on
 * @input       strCond String name of cond
 * @input       strFile The file that this assert is in
 * @input       strFunc The function that this assert is in
 *
 * @return      None
 */
void vDvpTb_Assert(int cond,
        uint32_t line,
        const char * strCond,
        const char * strFile,
        const char * strFunc);

#ifdef __cplusplus
 }
#endif

#endif /* _DVP_TEST_TB_H_ */
