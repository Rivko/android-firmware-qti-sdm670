/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_MATCH_PCM_FILE_H
#define TEST_CASE_MATCH_PCM_FILE_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "TestCaseUtility.h"
#include "logfile.h"

using namespace qvop;
using namespace std;

class TestCaseMatchPcmFile : public TestCase {
public:
    TestCaseMatchPcmFile() {};
    virtual ~TestCaseMatchPcmFile() { }

    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {

        //convert list of params to mem vars
        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        /* calling match initializes the state of the secure module to accept
        *   the match request. A failed call to match is a test case failure
        *   because the secure environment would not be ready to accept the
        *   audio data for the match processing.
        *
        *   The secure module is typically an environment like Trustzone, but could be
        *   something else.
        */
        TestCaseCallbackMatch cb(expected);
        if (service.match(cb, user_id.c_str()) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call match", QVOP_FN);
            return TCFailure;
        }

        /* send the file to the secure environment for match processing. Only
        *   one audio stream is matched at a time.
        */
        TestCaseCallbackProcessFrame pfcb(expected); //used below when calling process frame
        QvopStatus sendStatus = TestCaseUtility::send_pcm_file(service, pfcb, srcfile.c_str());

        /* It is possible that the secure environment matched/failed match
        *   prior to the end of the stream, so a successful "send" would include
        *   return values QVOP_SUCCESS or QVOP_MATCH_SUCCESS or QVOP_MATCH_FAILURE
        */
        if (sendStatus != QVOP_SUCCESS && sendStatus != QVOP_MATCH_SUCCESS &&  sendStatus != QVOP_MATCH_FAILURE) {
            logfile_print_e("%s: failed to send file", QVOP_FN);
            return TCFailure;
        }

        /* If the secure environment match'd prior to the end of the stream,
        *   the test is complete, but need to check response against expected
        *   response value.
        */
        if (sendStatus == QVOP_MATCH_SUCCESS || sendStatus == QVOP_MATCH_FAILURE) {
            logfile_print_d("%s: match Complete - result: %s expected: %s", QVOP_FN,
                sendStatus == QVOP_MATCH_SUCCESS ? "QVOP_MATCH_SUCCESS" : "QVOP_MATCH_FAILURE",
                expected ? "Match" : "NoMatch");
            if ((expected && (sendStatus == QVOP_MATCH_SUCCESS)) ||
                (!expected && (sendStatus == QVOP_MATCH_FAILURE))) {
                return TCSuccess;
            }
            else {
                return TCFailure;
            }
        }

        /* Calling matchCaptureComplete resets the state of the secure environment
        *   to accept new requests.
        */
        if (service.matchCaptureComplete(cb) != QVOP_SUCCESS) {
            logfile_print_e("%s: failed to call matchCaptureComplete", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }

protected:
    /*
    * Helper function to parse thru the parameters provided
    * Parameters required
    * KEY_MATCH_FILES       : --pcm_files
    *                       : files - typically more than 1
    * KEY_MATCH_IDENTIFIER: --identifier
    *                       : name of identifier
    * KEY_MATCH_USERID      : --user_id
    *                       : name of user
    */
    virtual bool ConvertParams(vector<string> const& params) {
        size_t found = string::npos;

        for (auto it = begin(params); it != end(params); ++it) {
            if (it->compare(KEY_FILES) == 0) {
                //move to next item
                //after KEY_FILES, at least one file should be found
                if (++it != end(params)) {
                    if (it->find(PARAM_INDICATOR) == string::npos) {
                        string temp(QVOP_DIRECTORY);
                        temp.append(it->c_str());
                        srcfile = (temp.c_str());
                    }
                }
            }
            else if (it->compare(KEY_IDENTIFIER) == 0) {
                //value is in next item
                if (++it != end(params)) {
                    if (it->find(PARAM_INDICATOR) == string::npos) {
                        identifier = (it->c_str());
                    }
                }
            }
            else if (it->compare(KEY_USERID) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_id = (it->c_str());
                }
            }
            else if (it->compare(KEY_MATCH_EXPECTED) == 0) {
                logfile_print_v("%s: expected match result:KEY_EXPECTED", QVOP_FN);
                expected = true;
            }
            else if (it->compare(KEY_MATCH_NOT_EXPECTED) == 0) {
                logfile_print_v("%s: expected match result:KEY_NOT_EXPECTED", QVOP_FN);
                expected = false;
            }
            else {
                logfile_print_v("%s: unknown parameter: %s", QVOP_FN, it->c_str());
            }

            //since the code above inc/dec the it pointer, there is a need
            //for an end check here before the for loop increments it again.
            if (it == end(params)){
                break;
            }
        }

        logfile_print_v("%s: user:%s id:%s srcfile:%s",
            QVOP_FN,
            user_id.c_str(),
            identifier.c_str(),
            srcfile.c_str());

        //allowing for error condition
        if (user_id == string(DEFAULT_MATCH_USER_ID) ||
            srcfile == string(DEFAULT_MATCH_FILE) ) {

            logfile_print_e("%s: need to init params", QVOP_FN);
            return false;
        }

        return true;
    }

protected:
    string user_id;
    string identifier;
    string srcfile;
    bool expected;
};


class TestCaseMatchPcmFileWithId : public TestCaseMatchPcmFile {
public:
    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {

        //convert list of params to mem vars
        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        //error in "withid" case, but not in other cases
        if (identifier == string(DEFAULT_MATCH_IDENTIFIER)) {
            logfile_print_e("%s: need to init params", QVOP_FN);
            return TCBadParams;
        }

        /* calling matchWithId initializes the state of the secure module to accept
        *   the match request against a particular identifier for the user.
        *   A failed call to matchWithId is a test case failure
        *   because the secure environment would not be ready to accept the
        *   audio data for the match processing.
        *
        *   The secure module is typically an environment like Trustzone, but could be
        *   something else.
        */
        TestCaseCallbackMatch cb(expected);
        if (service.matchWithId(cb, user_id.c_str(), identifier.c_str()) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call matchWithId", QVOP_FN);
            return TCFailure;
        }

        /* send the file to the secure environment for match processing. Only
        *   one audio stream is matched at a time.
        */
        TestCaseCallbackProcessFrame pfcb(expected); //used below when calling process frame
        QvopStatus sendStatus = TestCaseUtility::send_pcm_file(service, pfcb, srcfile.c_str());

        /* It is possible that the secure environment matched/failed match
        *   prior to the end of the stream, so a successful "send" would include
        *   return values QVOP_SUCCESS or QVOP_MATCH_SUCCESS or QVOP_MATCH_FAILURE
        */
        if (sendStatus != QVOP_SUCCESS && sendStatus != QVOP_MATCH_SUCCESS &&  sendStatus != QVOP_MATCH_FAILURE) {
            logfile_print_e("%s: failed to send file", QVOP_FN);
            return TCFailure;
        }

        /* If the secure environment match'd prior to the end of the stream,
        *   the test is complete, but need to check response against expected
        *   response value.
        */
        if (sendStatus == QVOP_MATCH_SUCCESS || sendStatus == QVOP_MATCH_FAILURE) {
            logfile_print_d("%s: match Complete - result: %s expected: %s", QVOP_FN,
                sendStatus == QVOP_MATCH_SUCCESS ? "QVOP_MATCH_SUCCESS": "QVOP_MATCH_FAILURE",
                expected ? "Match" : "NoMatch");
            if ((expected && (sendStatus == QVOP_MATCH_SUCCESS)) ||
                (!expected && (sendStatus == QVOP_MATCH_FAILURE))) {
                return TCSuccess;
            }
            else {
                return TCFailure;
            }
        }

        /* Calling matchCaptureComplete resets the state of the secure environment
        *   to accept new requests.
        */
        if (service.matchCaptureComplete(cb) != QVOP_SUCCESS) {
            logfile_print_e("%s: failed to call matchCaptureComplete", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }
};

#endif //TEST_CASE_MATCH_PCM_FILE_H
