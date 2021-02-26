/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_GET_VERSION_H
#define TEST_CASE_GET_VERSION_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "logfile.h"


using namespace qvop;
using namespace std;

class TestCaseGetVersion : public TestCase {
public:
    TestCaseGetVersion() {};
    virtual ~TestCaseGetVersion() {};

    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
        //expected = 0; should add to parameter list
        TestCaseCallbackGetVersion cb;

        if (service.getVersion(cb) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call getVersion", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }
};

#endif //TEST_CASE_GET_VERSION_H