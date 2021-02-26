/******************************************************************************
  @file  TinyTestFramework.cpp
  @brief Socket communication with gamed

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "ttf.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

UnitTest TinyTF::mTests[MAX_UNIT_TESTS_MODULES];
int TinyTF::mNumRegistered = 0;
static pthread_mutex_t perf_veri_mutex = PTHREAD_MUTEX_INITIALIZER;

TinyTF::TinyTF() {
    for (int i = 0; i < mNumRegistered; i++) {
        printf ("Module-%d %s Fn %p registered\n", i, mTests[i].mTestName, mTests[i].mTestFn);
    }
}

TinyTF::TinyTF(const char *name, TestFn fn) {
    pthread_mutex_lock(&perf_veri_mutex);
    if ((NULL != name) && (mNumRegistered < MAX_UNIT_TESTS_MODULES)) {
        strlcpy(mTests[mNumRegistered].mTestName, name, MAX_TEST_NAME_LEN);
        mTests[mNumRegistered].mTestFn = fn;
        mNumRegistered++;
    } else {
        printf("name null or reached max limit, can not registered any more modules\n");
    }
    pthread_mutex_unlock(&perf_veri_mutex);
}

int TinyTF::Run(int flag) {
    int ret = -1;

    printf ("Number of test modules registered: %d flags: %d\n", mNumRegistered, flag);

    for (int i = 0; i < mNumRegistered; i++) {
        if (NULL == mTests[i].mTestFn) {
            continue;
        }
        printf ("Running module %s\n", mTests[i].mTestName);
        ret = (*(mTests[i].mTestFn))(flag);
        if (ret >= 0) {
            printf("Module %s: [SUCCESS]\n", mTests[i].mTestName);
        } else {
            printf("Module %s: [FAIL]\n", mTests[i].mTestName);
        }
    }

    return 0;
}

static void show_usage(string name)
{
    printf("Usage: %s <option(s)>"
           "Options:\n"
           "\t-h,--help\t\tShow this help message\n"
           "-a,--all\texecute all tests\n"
           "-s,--stress\tstress tests\n"
           "-p,--simple\tsimple tests\n"
           "-n, --nightstress\tover night stress\n"
           "-m,--multithreaded\tmultithreaded tests\n", name.c_str());
    return;
}

int main(int argc, char *argv[]) {
    int flags = 0;

    if (argc < 1) {
        show_usage(argv[0]);
        return 1;
    }
    else if (argc == 1) {
        flags = TESTS_ALL;
    }

    vector <string> sources;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-s") || (arg == "--stress")) {
            flags |= TESTS_STRESS;
        } else if ((arg == "-p") || (arg == "--simple")) {
            flags |= TESTS_SIMPLE;
        } else if ((arg == "-m") || (arg == "--multithreaded")) {
            flags |= TESTS_MULTITHREADED;
        } else if ((arg == "-n") || (arg == "--nightstress")) {
            flags |= TESTS_MULTITHREADED;
        } else if ((arg == "-a") || (arg == "--all")) {
            flags |= TESTS_ALL;
        } else {
            show_usage(argv[0]);
            return 0;
        }
    }

    TinyTF tf;
    tf.Run(flags);
    return 0;
}

