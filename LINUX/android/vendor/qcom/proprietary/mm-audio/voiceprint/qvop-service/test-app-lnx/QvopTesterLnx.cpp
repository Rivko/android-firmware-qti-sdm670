/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "logfile.h"
#include "QvopService.h"
#include "QvopCallbackLNX.h"
#include "TestCaseEnrollPcmFile.h"
#include "TestCaseMatchPcmFile.h"
#include "TestCaseDelete.h"
#include "TestCaseGetVersion.h"
#include "TestCaseEnrollMicInput.h"
#include "TestCaseMatchMicInputWithId.h"
#include "TestCaseMatchSVAInputWithId.h"

//helpful defines
#define MIN_NUMBER_CMD_ARGS 3
//test case defs
#define TEST_NOT_IDENTIFIED 0
#define TEST_GET_VERSION 1
#define TEST_ENROLL_PCM_FILE 100
#define TEST_ENROLL_MIC 101
#define TEST_MATCH_PCM_FILE 200
#define TEST_MATCH_WITH_ID_PCM_FILE 201
#define TEST_MATCH_WITH_ID_MIC_INPUT 202
#define TEST_MATCH_WITH_ID_SVA_INPUT 203
#define TEST_DELETE_USER 300
#define TEST_DELETE_USER_WITH_ID 301
#define TEST_DELETE_USER_ALL 302

using namespace qvop;
using namespace std;

static void show_usage(string name)
{
    logfile_print_i("Usage: %s <option(s)> SOURCES", name.c_str());
    logfile_print_i("Options: \n");
    logfile_print_i("\t--help\t\tShow this help message");
    logfile_print_i("\t--test-case TEST_CASE_NUM\tSpecify the test case to run. 1 at a time. Numeric 1 - X");
}

static void show_test_cases(uint8_t testId)
{
    logfile_print_i("Test Case Requested: %d\n\n", testId);
    logfile_print_i("Test Case Options: \n");
    logfile_print_i("\t%d\t%s", TEST_GET_VERSION, "TEST_GET_VERSION");
    logfile_print_i("\t%d\t%s", TEST_ENROLL_PCM_FILE, "TEST_ENROLL_PCM_FILE");
    logfile_print_i("\t%d\t%s", TEST_ENROLL_MIC, "TEST_ENROLL_MIC");
    logfile_print_i("\t%d\t%s", TEST_MATCH_PCM_FILE, "TEST_MATCH_PCM_FILE");
    logfile_print_i("\t%d\t%s", TEST_MATCH_WITH_ID_PCM_FILE, "TEST_MATCH_WITH_ID_PCM_FILE");
    logfile_print_i("\t%d\t%s", TEST_MATCH_WITH_ID_MIC_INPUT, "TEST_MATCH_WITH_ID_MIC_INPUT");
    logfile_print_i("\t%d\t%s", TEST_MATCH_WITH_ID_SVA_INPUT, "TEST_MATCH_WITH_ID_SVA_INPUT");
    logfile_print_i("\t%d\t%s", TEST_DELETE_USER, "TEST_MATCH_PCM_FILE");
    logfile_print_i("\t%d\t%s", TEST_DELETE_USER_WITH_ID, "TEST_DELETE_USER_WITH_ID");
    logfile_print_i("\t%d\t%s", TEST_DELETE_USER_ALL, "TEST_DELETE_USER_ALL");
}

/**
    * @brief daemon used for testing libqvop-lnx-service
    *
    * @param argc
    * @param argv
    *
    * @return
    */
int main(int argc, char* argv[]) {
    uint8_t logLevel = QVOP_LOG_INFO;
    //init logging to = _printToScreen = true, android = false
    logfile_init_platform(true, false);

    //basic sanity check that there is at least one pair of params...e.g. test-case 1
    if (argc < MIN_NUMBER_CMD_ARGS) {
        show_usage(argv[0]);
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);

    // service object - impl in library being tested.
    QvopService service;

    //logfile_print_i("%s: ENTER App run date: %s %s", QVOP_FN, __DATE__, __TIME__);

    vector <string> sources;
    uint16_t testId = TEST_NOT_IDENTIFIED;

    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--help"){
            show_usage(argv[0]);
        }
        else if (string(argv[i]) == "--test-case") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                testId = atoi(argv[++i]); // Increment 'i' to get next arg
            }
            else { // no argument to the option.
                logfile_print_e("%s option requires one argument.", argv[i]);
                return 1;
            }
        }
        else if (string(argv[i]) == "--log-level") {
            if (i + 1 < argc) { // Make sure we aren't at the end of argv!
                logLevel = atoi(argv[++i]); // Increment 'i' to get next arg
                logfile_set(logLevel);
            }
            else { // no argument to the option.
                logfile_print_e("%s option requires one argument.", argv[i]);
                return 1;
            }
        }
        else {
            sources.push_back(argv[i]);
        }
    }

    //if test id not identified, show usage and exit
    TestCaseResult tcr = TCFailure;//default until proven otherwise
    switch (testId) {
    case TEST_NOT_IDENTIFIED:
        show_usage(argv[0]);
        return 1;
    case TEST_GET_VERSION:
    {
        TestCaseGetVersion tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_ENROLL_PCM_FILE:
    {
        TestCaseEnrollPcmFile tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_ENROLL_MIC:
    {
        TestCaseEnrollMicInput tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_MATCH_PCM_FILE:
    {
        TestCaseMatchPcmFile tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_MATCH_WITH_ID_PCM_FILE:
    {
        TestCaseMatchPcmFileWithId tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_MATCH_WITH_ID_MIC_INPUT:
    {
        TestCaseMatchMicInputWithId tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_MATCH_WITH_ID_SVA_INPUT:
    {
        TestCaseMatchSVAInputWithId tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_DELETE_USER:
    {
        TestCaseDeleteUser tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_DELETE_USER_WITH_ID:
    {
        TestCaseDeleteUserWithId tc;
        tcr = tc.Run(service, sources);
        break;
    }
    case TEST_DELETE_USER_ALL:
    {
        TestCaseDeleteAll tc;
        tcr = tc.Run(service, sources);
        break;
    }
    default:
        show_test_cases(testId);
        return 1;
    }

    logfile_print_i("%s: test case: %d - %s  %d", QVOP_FN,
        testId,
        tcr == TCSuccess ? "PASSED" : "FAILED",
        tcr);

    // clear the log file
    FILE* fp = fopen(QVOP_LOG_FILE, "wb");
    if (fp) {
        fclose(fp);
    }

    //logfile_print_i("%s: EXIT App run date: %s %s\n", QVOP_FN, __DATE__, __TIME__);
    return 0;
}
