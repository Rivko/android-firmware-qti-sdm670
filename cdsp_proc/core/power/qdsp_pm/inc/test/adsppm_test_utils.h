/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef ADSPPM_TEST_UTILS_H
#define ADSPPM_TEST_UTILS_H

#include "ULog.h"
#include "ULogFront.h"


#define ADSPPMTEST_ULOG 1
#define ADSPPM_TESTULOG_BUFFER_SIZE 32768
#define ADSPPM_TESTULOG_LEVEL ADSPPMTEST_LOG_LEVEL_ERROR

// Logging levels
#define ADSPPMTEST_LOG_LEVEL_RESULT   0 //!< TEST CASE RESULT
#define ADSPPMTEST_LOG_LEVEL_ERROR    1 //!< Error log messages
#define ADSPPMTEST_LOG_LEVEL_WARNING  2 //!< Warnings
#define ADSPPMTEST_LOG_LEVEL_INFO     3 //!< Critical info messages
#define ADSPPMTEST_LOG_LEVEL_INFO_EXT 4 //!< Extended info messages
#define ADSPPMTEST_LOG_LEVEL_FN_ENTRY 5 //!< Function entry/exit messages
#define ADSPPMTEST_LOG_LEVEL_DEBUG    6 //!< All debug messages
#define ADSPPMTEST_LOG_LEVEL_MAX      7


typedef enum
{
    ADSPPM_8994,
    ADSPPM_8992,
    ADSPPM_8996,
    ADSPPM_845,
	ADSPPM_670,
    NUM_MAX_TARGETS
} AdsppmTestTargetType;

typedef enum
{
    AdsppmTestId_System,
    AdsppmTestId_CoreClock,
    AdsppmTestId_DomainClock,
    AdsppmTestId_MIPS,
    AdsppmTestId_Sleep,
    AdsppmTestId_Bandwidth,
    AdsppmTestId_RegProg,
    AdsppmTestId_Power,
    AdsppmTestId_MemoryPower,
    AdsppmTestId_Bundle,
    AdsppmTestId_Stress,
    AdsppmTestId_Profile,
    AdsppmTestId_CachePartitioning,
    AdsppmTestId_SystemCache,
    AdsppmTestId_DcvsAggregation,
    AdsppmTestId_Max,
    AdsppmTestId_force32bits = 0x7FFFFFFF
} AdsppmTestIdType;


AdsppmTestTargetType GetTestTarget(void);
ULogHandle GetTestUlogHandle(void);
uint32 GetTestDebugLevel(void);
uint32 GetTestTotal(void);
uint32 GetTestPassed(void);
uint32 GetTestFailed(void);
void updateTestResults(AdsppmTestIdType adsppmTestId, MMPM_STATUS sts);


#if(ADSPPMTEST_ULOG == 1)
#define ADSPPMTEST_LOG_PRINTF_0(level, formatStr)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_0(GetTestUlogHandle(), formatStr);}
#define ADSPPMTEST_LOG_PRINTF_1(level, formatStr, p1)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_1(GetTestUlogHandle(), formatStr, p1);}
#define ADSPPMTEST_LOG_PRINTF_2(level, formatStr, p1,p2)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_2(GetTestUlogHandle(), formatStr, p1,p2);}
#define ADSPPMTEST_LOG_PRINTF_3(level, formatStr, p1,p2,p3)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_3(GetTestUlogHandle(), formatStr, p1,p2,p3);}
#define ADSPPMTEST_LOG_PRINTF_4(level, formatStr, p1,p2,p3,p4)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_4(GetTestUlogHandle(), formatStr, p1,p2,p3,p4);}
#define ADSPPMTEST_LOG_PRINTF_5(level, formatStr, p1,p2,p3,p4,p5)\
    if(GetTestDebugLevel() >= level)\
        {ULOG_RT_PRINTF_5(GetTestUlogHandle(), formatStr, p1,p2,p3,p4,p5);}
#define ADSPPMTEST_LOG_PROFILESTART(testName)\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(), "[PROFILE]: "testName": ProfileStart")
#define ADSPPMTEST_LOG_PROFILEEND(testName)\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(), "[PROFILE]: "testName": ProfileEnd")

#define ADSPPMTEST_LOG_TEST_HEADER(testName, testDetails)\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(),\
        "==============================================================================================");\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(), "== Test "testName": "testDetails);
#define ADSPPMTEST_LOG_TEST_RESULT(testId, testName, sts)\
    updateTestResults(testId, sts);\
    if(sts == MMPM_STATUS_SUCCESS)\
        {ULOG_RT_PRINTF_0(GetTestUlogHandle(), "== Test "testName" [PASSED]");}\
    else\
        {ULOG_RT_PRINTF_0(GetTestUlogHandle(), "== Test "testName" [FAILED]");}\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(),\
        "==============================================================================================");
#define ADSPPMTEST_LOG_TEST_RESULT_SUMMARY()\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(),\
        "==============================================================================================");\
    ULOG_RT_PRINTF_3(GetTestUlogHandle(),\
        "== Test Result Summary: Total [%u], Passed [%u], Failed [%u]",\
        GetTestTotal(), GetTestPassed(), GetTestFailed());\
    ULOG_RT_PRINTF_0(GetTestUlogHandle(),\
        "==============================================================================================");
#else
#define ADSPPMTEST_LOG_PRINTF_0(level, formatStr)
#define ADSPPMTEST_LOG_PRINTF_1(level, formatStr, p1)
#define ADSPPMTEST_LOG_PRINTF_2(level, formatStr, p1,p2)
#define ADSPPMTEST_LOG_PRINTF_3(level, formatStr, p1,p2,p3)
#define ADSPPMTEST_LOG_PRINTF_4(level, formatStr, p1,p2,p3,p4)
#define ADSPPMTEST_LOG_PRINTF_5(level, formatStr, p1,p2,p3,p4,p5)
#define ADSPPMTEST_LOG_TEST_HEADER(testName, testDetails)
#define ADSPPMTEST_LOG_TEST_RESULT(testName, resultString, sts)
#define ADSPPMTEST_LOG_TEST_RESULT_SUMMARY()
#define ADSPPMTEST_LOG_PROFILESTART(testName)
#define ADSPPMTEST_LOG_PROFILEEND(testName)
#endif


#define PRINT_INFO_0(formatStr)\
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[INFO] %s: "formatStr, __FUNCTION__);
#define PRINT_INFO_1(formatStr, p1)\
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1);
#define PRINT_INFO_2(formatStr, p1,p2)\
    ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2);
#define PRINT_INFO_3(formatStr, p1,p2,p3)\
    ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2,p3);
#define PRINT_INFO_4(formatStr, p1,p2,p3,p4)\
    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2,p3,p4);

#define PRINT_INFOEXT_0(formatStr)\
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO_EXT,\
        "[INFO] %s: "formatStr, __FUNCTION__);
#define PRINT_INFOEXT_1(formatStr, p1)\
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO_EXT,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1);
#define PRINT_INFOEXT_2(formatStr, p1,p2)\
    ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO_EXT,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2);
#define PRINT_INFOEXT_3(formatStr, p1,p2,p3)\
    ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO_EXT,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2,p3);
#define PRINT_INFOEXT_4(formatStr, p1,p2,p3,p4)\
    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO_EXT,\
        "[INFO] %s: "formatStr, __FUNCTION__, p1,p2,p3,p4);

#define PRINT_NA_0(formatStr)\
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_WARNING,\
        "[NA] %s: "formatStr, __FUNCTION__);
#define PRINT_NA_1(formatStr, p1)\
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_WARNING,\
        "[NA] %s: "formatStr, __FUNCTION__, p1);
#define PRINT_NA_2(formatStr, p1,p2)\
    ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_WARNING,\
        "[NA] %s: "formatStr, __FUNCTION__, p1,p2);
#define PRINT_NA_3(formatStr, p1,p2,p3)\
    ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_WARNING,\
        "[NA] %s: "formatStr, __FUNCTION__, p1,p2,p3);
#define PRINT_NA_4(formatStr, p1,p2,p3,p4)\
    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_WARNING,\
        "[NA] %s: "formatStr, __FUNCTION__, p1,p2,p3,p4);

#define PRINT_ERROR_0(formatStr)\
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR,\
        "[ERROR] %s: "formatStr, __FUNCTION__);
#define PRINT_ERROR_1(formatStr, p1)\
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,\
        "[ERROR] %s: "formatStr, __FUNCTION__, p1);
#define PRINT_ERROR_2(formatStr, p1,p2)\
    ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_ERROR,\
        "[ERROR] %s: "formatStr, __FUNCTION__, p1,p2);
#define PRINT_ERROR_3(formatStr, p1,p2,p3)\
    ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_ERROR,\
        "[ERROR] %s: "formatStr, __FUNCTION__, p1,p2,p3);
#define PRINT_ERROR_4(formatStr, p1,p2,p3,p4)\
    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,\
        "[ERROR] %s: "formatStr, __FUNCTION__, p1,p2,p3,p4);

#define PRINT_PASS_0(formatStr)\
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[PASS] %s: "formatStr, __FUNCTION__);
#define PRINT_PASS_1(formatStr, p1)\
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[PASS] %s: "formatStr, __FUNCTION__, p1);
#define PRINT_PASS_2(formatStr, p1,p2)\
    ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[PASS] %s: "formatStr, __FUNCTION__, p1,p2);
#define PRINT_PASS_3(formatStr, p1,p2,p3)\
    ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[PASS] %s: "formatStr, __FUNCTION__, p1,p2,p3);
#define PRINT_PASS_4(formatStr, p1,p2,p3,p4)\
    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO,\
        "[PASS] %s: "formatStr, __FUNCTION__, p1,p2,p3,p4);


#endif /* ADSPPM_TEST_UTILS_H */

