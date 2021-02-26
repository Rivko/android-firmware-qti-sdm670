/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_ENROLL_MIC_INPUT_H
#define TEST_CASE_ENROLL_MIC_INPUT_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "TestCaseUtility.h"
#include "logfile.h"

using namespace qvop;

class TestCaseEnrollMicInput : public TestCase {
public:
    TestCaseEnrollMicInput() : user_id(DEFAULT_ENROLL_USER_ID),
        identifier(DEFAULT_ENROLL_IDENTIFIER),
        keyphrase(DEFAULT_ENROLL_KWPHRASE),
        number_collections(DEFAULT_ENROLL_MIC_COLLECTIONS),
        user_input(DEFAULT_SRC_TRACKING,
                DEFAULT_INPUT_DEVICE,
                DEFAULT_NUM_CHANNELS,
                DEFAULT_FORMAT,
                DEFAULT_SAMPLE_RATE,
                DEFAUT_MIC_SOURCE,
                DEFAULT_LOOPTIME) {};
    virtual ~TestCaseEnrollMicInput() { }

    virtual TestCaseResult Run(QvopService& service, vector<std::string> const& params) {
        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        /*
        * Enrollment flow
        - call enroll with meta data e.g. user id identifier & keyphrase
        - for each file provided
            - enrollCaptureStart
            - beginVoiceInput
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

        /*loop until max recordings achieved
        */
        for (size_t index = 0; index < number_collections; index++) {
            if (service.enrollCaptureStart(cb) != QVOP_SUCCESS ||
                cb.GetStatus() == CBFailure) {
                logfile_print_e("%s: failed to start capture", QVOP_FN);
                return TCFailure;
            }

            QvopStatus capStatus = TestCaseUtility::beginVoiceInput(service, pfcb, user_input);
            if (capStatus != QVOP_SUCCESS) {
                logfile_print_e("%s: voice capture returned status: %d", QVOP_FN, capStatus);
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

        logfile_print_i("%s: End Test Case...", QVOP_FN);
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
            if (it->compare(KEY_IDENTIFIER) == 0) {
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

        logfile_print_v("%s: user:%s id:%s kw:%s num_coll: %d chan:%d fmt:%d device:%d loop:%d samp-rate:%d src:%d src_trk:%d",
            QVOP_FN,
            user_id.c_str(),
            identifier.c_str(),
            keyphrase.c_str(),
            number_collections,
            user_input.channels,
            user_input.format,
            user_input.input_device,
            user_input.looptime,
            user_input.sample_rate,
            user_input.source,
            user_input.source_tracking);

        //allowing for error condition
        if (user_id == string(DEFAULT_ENROLL_MIC_USER_ID) ||
            identifier == string(DEFAULT_ENROLL_MIC_IDENTIFIER) ||
            keyphrase == string(DEFAULT_ENROLL_MIC_IDENTIFIER)) {

            logfile_print_e("%s: need to init params", QVOP_FN);
            return TCBadParams;
        }
        return true;
    }

protected:
    string user_id;
    string identifier;
    string keyphrase;
    size_t number_collections;
    qvop_user_input user_input;
};

#endif //TEST_CASE_ENROLL_MIC_INPUT_H
