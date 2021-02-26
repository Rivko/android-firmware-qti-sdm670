/******************************************************************************
  @file  ttf.h
  @brief tiny test framework

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#ifndef __TTF_H__
#define __TTF_H__

#define MAX_UNIT_TESTS_MODULES 128
#define MAX_TEST_NAME_LEN 128
#define MAX_THREADS 256

#define SUCCESS 0
#define FAILED -1

enum {
    TESTS_SIMPLE = 0x01,
    TESTS_MULTITHREADED = 0x02,
    TESTS_STRESS = 0x04,
    TESTS_NIGHT_STRESS = 0x08,
    TESTS_ALL = TESTS_SIMPLE|TESTS_MULTITHREADED|TESTS_STRESS,
};

typedef int (*TestFn)(int);

typedef struct UnitTest {
    TestFn mTestFn;
    char mTestName[MAX_TEST_NAME_LEN];
}UnitTest;

class TinyTF {
    static UnitTest mTests[MAX_UNIT_TESTS_MODULES];
    static int mNumRegistered;

public:
    TinyTF();
    TinyTF(const char *name, TestFn fn);

    int Run(int flags);
};

#endif
