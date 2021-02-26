/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_MATCH_MIC_INPUT_H
#define TEST_CASE_MATCH_MIC_INPUT_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "TestCaseUtility.h"
#include "logfile.h"

using namespace qvop;
using namespace std;

class TestCaseMatchMicInputWithId : public TestCase {
public:
    TestCaseMatchMicInputWithId() : user_id(DEFAULT_MATCH_USER_ID),
        identifier(DEFAULT_MATCH_IDENTIFIER),
        number_collections(DEFAULT_MATCH_MIC_COLLECTIONS),
        user_input(DEFAULT_SRC_TRACKING,
        DEFAULT_INPUT_DEVICE,
        DEFAULT_NUM_CHANNELS,
        DEFAULT_FORMAT,
        DEFAULT_SAMPLE_RATE,
        DEFAUT_MIC_SOURCE,
        DEFAULT_LOOPTIME) {};
    virtual ~TestCaseMatchMicInputWithId() { }

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

        /* begin audio capture
        */
        logfile_print_v("%s: instantiating TestCaseCallbackProcessFrame CTOR: %s", QVOP_FN, expected ? "TRUE" : "FALSE");
        TestCaseCallbackProcessFrame pfcb(expected); //used below when calling process frame
        QvopStatus capStatus = TestCaseUtility::beginVoiceInput(service, pfcb, user_input);
        /* It is possible that the secure environment matched/failed match
        *   prior to the end of the stream, so a successful "send" would include
        *   return values QVOP_SUCCESS or QVOP_MATCH_SUCCESS or QVOP_MATCH_FAILURE
        */
        if (capStatus != QVOP_SUCCESS && capStatus != QVOP_MATCH_SUCCESS &&  capStatus != QVOP_MATCH_FAILURE) {
            logfile_print_i("%s: voice capture returned status: %d", QVOP_FN, capStatus);
            return TCFailure;
        }

        /* If the secure environment match'd prior to the end of the stream,
        *   the test is complete, but need to check response against expected
        *   response value.
        */
        if (capStatus == QVOP_MATCH_SUCCESS || capStatus == QVOP_MATCH_FAILURE) {
            logfile_print_i("%s: match Complete - result: %s expected: %s", QVOP_FN,
                capStatus == QVOP_MATCH_SUCCESS ? "QVOP_MATCH_SUCCESS" : "QVOP_MATCH_FAILURE",
                expected ? "Match" : "NoMatch");
            if ((expected && (capStatus == QVOP_MATCH_SUCCESS)) ||
                (!expected && (capStatus == QVOP_MATCH_FAILURE))) {
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
    * KEY_MATCH_EXPECTED || KEY_MATCH_NOT_EXPECTED
    *
    * KEY_INPUT_SOURCE_TRACKING_ENABLED
    *                       : flag enabling feature
    *
    * KEY_INPUT_NUM_COLLECTIONS
    *                       : number of collections to sample. Currently 1
    *
    * KEY_INPUT_DEVICE
    *                       : {4->built-in mic, 16->wired_headset}

    * KEY_INPUT_CHANNELS
    *                       : {MONO=1 STEREO=2 QUAD=4}
    *
    * KEY_INPUT_FORMAT
    *                       : {16 - 16 bit recording, 24 - 24 bit recording}
    *
    * KEY_INPUT_SAMPLE_RATE
    *                       : {48000, 16000 etc}
    *
    * KEY_INPUT_SOURCE
    *                       : {ref: system/media/audio/include/system/audio.h}
    *
    * KEY_INPUT_LOOPTIME
    *                       : number of milliseconds to record
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
                logfile_print_i("%s: expected result:KEY_EXPECTED", QVOP_FN);
                expected = true;
            }
            else if (it->compare(KEY_MATCH_NOT_EXPECTED) == 0) {
                logfile_print_i("%s: expected result:KEY_NOT_EXPECTED", QVOP_FN);
                expected = false;
            }
            else if (it->compare(KEY_INPUT_SOURCE_TRACKING_ENABLED) == 0) {
                //value is inferred
                user_input.source_tracking = 1;
            }
            else if (it->compare(KEY_INPUT_SOURCE_TRACKING_DISABLED) == 0) {
                //value is inferred
                user_input.source_tracking = 0;
            }
            else if (it->compare(KEY_INPUT_NUM_COLLECTIONS) == 0) {
                //value is in next item
                if (++it != end(params)){
                    number_collections = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_DEVICE) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.input_device = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_CHANNELS) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.channels = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_FORMAT) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.format = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_SAMPLE_RATE) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.sample_rate = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_SOURCE) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.source = atoi(it->c_str());
                }
            }
            else if (it->compare(KEY_INPUT_LOOPTIME) == 0) {
                //value is in next item
                if (++it != end(params)){
                    user_input.looptime = atoi(it->c_str());
                }
            }
            else {
                logfile_print_w("%s: unknown parameter: %s", QVOP_FN, it->c_str());
            }

            //since the code above inc/dec the it pointer, there is a need
            //for an end check here before the for loop increments it again.
            if (it == end(params)){
                break;
            }
        }

        logfile_print_d("%s: user:%s id:%s num_coll: %d chan:%d fmt:%d device:%d loop:%d samp-rate:%d src:%d src_trk:%d",
            QVOP_FN,
            user_id.c_str(),
            identifier.c_str(),
            number_collections,
            user_input.channels,
            user_input.format,
            user_input.input_device,
            user_input.looptime,
            user_input.sample_rate,
            user_input.source,
            user_input.source_tracking);

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
    size_t number_collections;
    qvop_user_input user_input;
};

#endif //TEST_CASE_MATCH_MIC_INPUT_H
