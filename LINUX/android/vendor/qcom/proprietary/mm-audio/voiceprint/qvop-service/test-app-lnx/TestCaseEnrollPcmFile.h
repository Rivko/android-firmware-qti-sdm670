/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_ENROLL_PCM_FILE_H
#define TEST_CASE_ENROLL_PCM_FILE_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "TestCaseUtility.h"
#include "logfile.h"

using namespace qvop;
using namespace std;

class TestCaseEnrollPcmFile : public TestCase {
public:
    TestCaseEnrollPcmFile() : user_id(DEFAULT_ENROLL_USER_ID),
        identifier(DEFAULT_ENROLL_IDENTIFIER),
        keyphrase(DEFAULT_ENROLL_KWPHRASE) {};
    virtual ~TestCaseEnrollPcmFile() { }

    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {

        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        /*
        * Enrollment flow
            - call enroll with meta data e.g. user id identifier & keyphrase
            - for each file provided
                - enrollCaptureStart
                - sendPcmFile
                - enrollCaptureStop
            - enrollCommit
        */
        //TODO: add this to the params//0=SUCCESS, 4=MORE DATA
        vector<int32_t> codes = { 0, 4 };
        TestCaseCallbackEnroll cb(codes);
        //used below when calling process frame
        //arbitrarily using true response, cuz there isn't a response
        //when sending file during enrollment.
        TestCaseCallbackProcessFrame pfcb(true);

        /* calling enroll initializes the state of the secure module to accept
        *   the enrollment. A failed call to enroll is a test case failure
        *   because the secure environment would not be ready to accept the
        *   audio data for the enrollment processing.
        *
        *   The secure module is typically an environment like Trustzone, but could be
        *   something else.
        */
        if (service.enroll(cb, user_id.c_str(), identifier.c_str(), keyphrase.c_str()) != QVOP_SUCCESS ||
                cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call enroll begin", QVOP_FN);
            return TCFailure;
        }

        /*loop thru each file and send to secure module for processing
        *  wrapping each file transmit with enrollCaptureStart and
        *  enrollCaptureComplete. This process simulates the normal voice
        *  capture process.
        */
        for (auto it = begin(files); it != end(files); ++it) {
            if (service.enrollCaptureStart(cb) != QVOP_SUCCESS ||
                cb.GetStatus() == CBFailure) {
                logfile_print_e("%s: failed to start capture", QVOP_FN);
                return TCFailure;
            }

            if (TestCaseUtility::send_pcm_file(service, pfcb, it->c_str()) != QVOP_SUCCESS) {
                logfile_print_e("%s: failed to send file", QVOP_FN);
                return TCFailure;
            }

            if (service.enrollCaptureComplete(cb) != QVOP_SUCCESS ||
                cb.GetStatus() == CBFailure) {
                logfile_print_e("%s: failed to stop capture", QVOP_FN);
                return TCFailure;
            }
        }

        /* call enrollCommit to serialize the data in the secure module.
        *   failure to serialize would be an error condition as subsequent
        *   calls to match would fail.
        */
        if (service.enrollCommit(cb) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call enrollCommit", QVOP_FN);
            return TCFailure;
        }
        /*end enrollment flow*/

        return TCSuccess;
    }

protected:
    /*
    * Helper function to parse thru the parameters provided
    * Parameters required
    * KEY_FILES     : --pcm_files
    *               : files - typically more than 1
    * KEY_IDENTIFIER: --identifier
    *               : name of identifier
    * KEY_USERID    : --user_id
    *               : name of user
    * KEY_KWPHRASE  : --keyphrase
    *               : the phrase
    */
    virtual bool ConvertParams(vector<string> const& params) {
        size_t found = string::npos;

        for (auto it = begin(params); it != end(params); ++it) {
            if (it->compare(KEY_FILES) == 0) {
                //move to next item
                //after KEY_FILES, at least one file should be found
                while (++it != end(params)) {
                    //found variable indicates whether PARAM_INDICATOR is in the next string
                    found = (it)->find(PARAM_INDICATOR);
                    if (found == string::npos) {
                        //create path to audio
                        string temp(QVOP_DIRECTORY);
                        temp.append(it->c_str());

                        files.push_back(temp.c_str());
                        logfile_print_d("%s: adding %s to the file list", QVOP_FN, temp.c_str());
                    }
                    else {
                        --it; //move iterator back for next loop
                        break;
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
            else if (it->compare(KEY_KWPHRASE) == 0) {
                //clear out the string, going to append below
                keyphrase.clear();
                //move to next item
                //after KEY_KWPHRASE, at least one word (sep by space) should be there
                while (++it != end(params)) {
                    //found variable indicates whether PARAM_INDICATOR is in the next string
                    found = (it)->find(PARAM_INDICATOR);
                    if (found == string::npos){
                        keyphrase.append(it->c_str());
                        keyphrase.append(" ");//pad next word
                }
                    else {
                        --it; //move iterator back for next loop
                        break;
                    }
                }
            }
            else if (it->compare(KEY_USERID) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_id = (it->c_str());
                }
            }
            else {
                logfile_print_e("%s: unknown parameter: %s", QVOP_FN, it->c_str());
            }

            //since the code above inc/dec the it pointer, there is a need
            //for an end check here before the for loop increments it again.
            if (it == end(params)){
                break;
            }
        }

        logfile_print_v("%s: user:%s id:%s kw:%s",
            QVOP_FN,
            user_id.c_str(),
            identifier.c_str(),
            keyphrase.c_str());

        //allowing for error condition
        if (user_id == string(DEFAULT_ENROLL_USER_ID) ||
            identifier == string(DEFAULT_ENROLL_IDENTIFIER) ||
            keyphrase == string(DEFAULT_ENROLL_IDENTIFIER) ||
            files.empty()) {

            logfile_print_e("%s: need to init params", QVOP_FN);
            return TCBadParams;
        }
        return true;
    }

protected:
    string user_id;
    string identifier;
    string keyphrase;
    vector<string> files;
};

#endif //TEST_CASE_ENROLL_PCM_FILE_H
