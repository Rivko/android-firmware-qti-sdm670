/*!
 * @file dvpTest_system.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Defines the functions and standard strings that are used and
 *              expected by the testing framework. The functions defined in this
 *              header file must be implemented by a given SYSTEM or PLATFORM.
 */

#ifndef _DVP_TEST_SYSTEM_H_
#define _DVP_TEST_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FAIL_STR "FAIL"
#define PASS_STR "PASS"

#define TEST_START \
    "    Test: %-40s                                                             [START]\n"
#define TEST_RUN \
    "    Test: %-40s                                                              [RUN ]\n"
#define TEST_SUMMARY \
    "    Test: %-40s Asserts: %-5d Passed: %-5d Failed: %-5d Duration: %3ds    [%4s]\n"
#define TEST_ASSERT \
    "        ASSERT FAILED:\n" \
    "            Line: %d    File: %-30s    Function: %-30s\n" \
    "            Cond: %s\n"
#define TEST_SUITE_START "\n[%s]\n"
#define TEST_SUITE_SUMMARY \
    "\n    Suite Summary:\n" \
    "        Total Tests Run:    %d\n" \
    "        Tests Passed:       %d\n" \
    "        Tests Failed:       %d\n" \
    "        Duration:           %ds\n"
#define TEST_BENCH_SUMMARY \
    "\nTest Summary:\n" \
    "    Suites Run:     %d\n" \
    "    Suites Passed:  %d\n" \
    "    Suites Failed:  %d\n" \
    "    Tests Run:      %d\n" \
    "    Tests Passed:   %d\n" \
    "    Tests Failed:   %d\n" \
    "    Total Duration: %ds\n"

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

/*!
 * @description This function provides logging services
 *
 * @return      None
 */
void SYSTEM_LOG(const char * fmt, ...);

/*!
 * @description This function is called whenever the unit testing framework
 *              itself is initialized.
 *
 * @return      None.
 */
void vDvpTestSystem_Init();

/*!
 * @description This function is called whenever the unit testing framework is
 *              terminated.
 *
 * @return      None.
 */
void vDvpTestSystem_Term();

/*!
 * @description This function is called whenever the test bench is started;
 *              i.e. it is called when vDvpTb_RunTests() is called by the user.
 *
 * @return      None
 */
void vDvpTestSystem_TestbenchStart();

/*!
 * @description This function is called whenever the test bench has finished
 *              running all of the tests to completion.
 *
 * @input       pSummary A pointer to the test bench summary report.
 *
 * @return      None
 */
void vDvpTestSystem_TestbenchFinish(t_TbSummary * pSummary);

/*!
 * @description This function is called prior to each test being called.
 *
 * @input       pTest   A pointer to the internal test case that is about to
 *                      be called.
 *
 * @return      None
 */
void vDvpTestSystem_TestStart(t_TbTest * pTest);

/*!
 * @description This function is called after each test case is run.
 *
 * @input       pTest   A pointer to the internal test case that was just run.
 *
 * @return      None
 */
void vDvpTestSystem_TestFinish(t_TbTest * pTest);

/*!
 * @description This function is called everytime a test suite is added
 *
 * @input       pSuite  A pointer to the test suite
 *
 * @return      None
 */
void vDvpTestSystem_SuiteAdd(t_TestSuite *pSuite);

/*!
 * @description This function is called everytime a test case is added
 *
 * @input       pTest   A pointer to the test
 *
 * @return      None
 */
void vDvpTestSystem_TestAdd(t_TestCase *pTest);

/*!
 * @description This function is called prior to a test suite running.
 *
 * @input       pSuite  A pointer to the test suite that is to be run.
 *
 * @return      None
 */
void vDvpTestSystem_SuiteStart(t_TbTestSuite * pSuite);

/*!
 * @description This function is called after a test suite has finished running
 *              all of its test cases.
 *
 * @input       pSuite  A pointer to the test suite that was just run.
 *
 * @return      None
 */
void vDvpTestSystem_SuiteFinish(t_TbTestSuite * pSuite);

/*!
 * @description This function is called whenever an assert fails its condition.
 *
 * @input       line    The line that the assert failed on
 * @input       strCond The stringified failed condition
 * @input       strFile The file that the test failed in
 * @input       strFunc The function that the test failed in
 *
 * @return      None
 */
void vDvpTestSystem_AssertFail(uint32_t line,
        const char * strCond,
        const char * strFile,
        const char * strFunc);

/*!
 * @description This function is expected to return the time in any units that
 *              can be described by an uint32_t. The testbench does nothing with
 *              this value. The value is to be used by the system implementation
 *              to calculate times for test runs.
 *
 * @return      Some time value
 */
uint32_t u32DvpTest_GetTime();

/*!
 * @brief       Get a pointer to a TbTest object.
 *
 * @return      Pointer to a TbTest object.
 */
t_TbTest * pstGetTbTest();

/*!
 * @brief       Free a TbTest object.
 *
 * @return      None
 */
void vFreeTbTest(t_TbTest * pst);

/*!
 * @brief       Get a pointer to a TbTestSuite object.
 *
 * @return      Pointer to a TbTestSuite object.
 */
t_TbTestSuite * pstGetTbTestSuite();

/*!
 * @brief       Free a TbTestSuite object.
 *
 * @return      None
 */
void vFreeTbTestSuite(t_TbTestSuite * pst);

/*!
 * @brief       Get a pointer to a TestBenchCb object.
 *
 * @return      Pointer to a TestBenchCb object.
 */
t_TestBenchCb * pstGetTestBenchCb();

/*!
 * @brief       Free a TestBenchCb object.
 *
 * @return      None
 */
void vFreeTestBenchCb(t_TestBenchCb * pst);

#ifdef __cplusplus
 }
#endif

#endif /* _DVP_TEST_SYSTEM_H_ */
