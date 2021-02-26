/*!
 * @file dvpTest.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Defines the assert macros, public structures, and helper
 *              macros that are required to interface with the testing
 *              framework.
 */

#ifndef _DVP_TEST_H_
#define _DVP_TEST_H_

#ifdef EXTERN
#undef EXTERN
#endif

#ifdef _DVP_TEST_C_
#undef _DVP_TEST_C_
#define EXTERN
#else
#define EXTERN EXTERN_C
#endif
#include "vpp.h"
#include "vpp_def.h"

/*! Function pointer to a suite initializiation function */
typedef void(*t_fpSuiteInitFunc)(void);
/*! Function pointer to a suite termination function */
typedef void(*t_fpSuiteTermFunc)(void);
/*! Function pointer to a test function */
typedef void(*t_fpTestFunc)(void);
/*! Function pointer to a test setup function */
typedef void(*t_fpTestSetupFunc)(void);
/*! Function pointer to a test cleanup fucntion */
typedef void(*t_fpTestCleanupFunc)(void);

/*!
 * Represents an individual test case.
 */
typedef struct TestCase {
    const char * pName;     /*! Name of the test function */
    t_fpTestFunc fpTest;    /*! Function pointer to the test function */
} t_TestCase;

/*!
 * Represents a test suite.
 */
typedef struct TestSuite {
    /*! Name of the test suite */
    const char * pName;
    /*! Suite init function */
    t_fpSuiteInitFunc fpInit;
    /*! Suite term function */
    t_fpSuiteTermFunc fpTerm;
    /*! Test setup function */
    t_fpTestSetupFunc fpTestSetup;
    /*! Test term function */
    t_fpTestCleanupFunc fpTestCleanup;
    /*! Pointer to an array of test cases */
    t_TestCase * pTests;
} t_TestSuite;

#define DVP_ASSERT_PASS()           vDvpTb_Assert(VPP_TRUE, __LINE__, "PASS", __FILE__, __func__)
#define DVP_ASSERT_FAIL()           vDvpTb_Assert(VPP_FALSE, __LINE__, "FAIL", __FILE__, __func__)
#define DVP_ASSERT_TEST_NOT_IMPL()  vDvpTb_Assert(VPP_FALSE, __LINE__, "TEST_NOT_IMPLEMENTED", __FILE__, __func__)
#define DVP_ASSERT_TRUE(cond)       vDvpTb_Assert((cond), __LINE__, "DVP_ASSERT_TRUE(" #cond ")", __FILE__, __func__)
#define DVP_ASSERT_FALSE(cond)      vDvpTb_Assert(!(cond), __LINE__, "DVP_ASSERT_FALSE(" #cond ")", __FILE__, __func__)

#define DVP_ASSERT_EQUAL(x, y)      ({ \
    unsigned long long myX = (unsigned long long)(x); \
    unsigned long long myY = (unsigned long long)(y); \
    uint32_t u32Res = (myX == myY); \
    char pc[256]; \
    pc[0] = '\0'; \
    if (!u32Res) \
        snprintf(pc, sizeof(pc), "DVP_ASSERT_EQUAL(" #x "=<%llu>, " #y "=<%llu>)", myX, myY); \
    vDvpTb_Assert(u32Res, __LINE__, pc, __FILE__, __func__); \
})

#define DVP_ASSERT_NEQUAL(x, y)     ({ \
    unsigned long long myX = (unsigned long long)(x); \
    unsigned long long myY = (unsigned long long)(y); \
    uint32_t u32Res = (myX != myY); \
    char pc[256]; \
    pc[0] = '\0'; \
    if (!u32Res) \
        snprintf(pc, sizeof(pc), "DVP_ASSERT_NEQUAL(" #x "=<%llu>, " #y "=<%llu>)", myX, myY); \
    vDvpTb_Assert(u32Res, __LINE__, pc, __FILE__, __func__); \
})

#define DVP_ASSERT_PTR_EQUAL(x, y)  ({ \
    void *myX = (void *)(x); \
    void *myY = (void *)(y); \
    uint32_t u32Res = (myX == myY); \
    char pc[256]; \
    pc[0] = '\0'; \
    if (!u32Res) \
        snprintf(pc, sizeof(pc), "DVP_ASSERT_PTR_EQUAL(" #x "=<%p>, " #y "=<%p>)", myX, myY); \
    vDvpTb_Assert(u32Res, __LINE__, pc, __FILE__, __func__); \
})

#define DVP_ASSERT_PTR_NEQUAL(x, y) ({ \
    void *myX = (void *)(x); \
    void *myY = (void *)(y); \
    uint32_t u32Res = (myX != myY); \
    char pc[256]; \
    pc[0] = '\0'; \
    if (!u32Res) \
        snprintf(pc, sizeof(pc), "DVP_ASSERT_PTR_NEQUAL(" #x "=<%p>, " #y "=<%p>)", myX, myY); \
    vDvpTb_Assert(u32Res, __LINE__, pc, __FILE__, __func__); \
})

#define DVP_ASSERT_PTR_NULL(x)      vDvpTb_Assert((((void *)x) == NULL), __LINE__, "DVP_ASSERT_PTR_NULL(" #x ")", __FILE__, __func__)
#define DVP_ASSERT_PTR_NNULL(x)     vDvpTb_Assert((((void *)x) != NULL), __LINE__, "DVP_ASSERT_PTR_NNULL(" #x ")", __FILE__, __func__)

#define TEST_NAME()                 pGetCurrentTestName()

#define TEST(name)                  static void name()
#define TEST_SUITE_INIT(name)       static void name()
#define TEST_SUITE_TERM(name)       static void name()
#define TEST_SETUP(name)            static void name()
#define TEST_CLEANUP(name)          static void name()

#define TEST_CASES                  t_TestCase
#define TEST_CASE(name)             { #name, name }
#define TEST_CASE_NULL()            { NULL, NULL }

#define TEST_SUITE(var, name, suiteInit, suiteTerm, testInit, testTerm, tests) \
    t_TestSuite var = { \
        name, \
        suiteInit, \
        suiteTerm, \
        testInit, \
        testTerm, \
        tests \
    }; \
    t_TestSuite * GET_INSTALLER_##var(void) { return &var; }


#define TEST_SUITE_DECLARE(suite) t_TestSuite * GET_INSTALLER_##suite(void)
#define TEST_SUITE_INSTALL(suite) vDvpTb_AddSuite(GET_INSTALLER_##suite())

#endif /* _DVP_TEST_H_ */
