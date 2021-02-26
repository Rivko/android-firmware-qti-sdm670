/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_CALLBACKS_H
#define TEST_CASE_CALLBACKS_H

#include <vector>
#include <algorithm>
#include "logfile.h"
#include "IQvopCallback.h"

enum CallbackStatus {CBSuccess, CBFailure};
using namespace qvop;

/*
* Base class for all callback classes. The QvopService API contains
* methods which reqiure a callback to be implemented by the client. The
* callback class has API that the service will call with results of the
* requested operation.
*/
class TestCaseCallback : public IQvopCallback {
public:
    TestCaseCallback() : status(CBFailure) {};
    virtual ~TestCaseCallback() {};
    //default, just return status
    virtual CallbackStatus GetStatus() { return status; }
    virtual void onError(int32_t error) {
        logfile_print_d("Unexpected callback onError");
    }
    virtual void onMatchFound(int32_t error,
        const char* user_id,
        const char* identifier,
        int32_t index,
        double sentence_score,
        double ver_score,
        float spoofScore) {
        logfile_print_d("Unexpected callback onMatchFound");
    }
    virtual void onMatchNotFound(int32_t error,
        int32_t index,
        double sentence_score,
        double ver_score,
        float spoofScore) {
        logfile_print_d("Unexpected callback onMatchNotFound");
    }
    virtual void onEnrollStatus(int32_t code) {
        logfile_print_d("Unexpected callback onEnrollStatus");
    }
    virtual void onDeleteStatus(int32_t code,
        const char* user_id,
        const char* identifier) {
        logfile_print_d("Unexpected callback onDeleteStatus");
    }
    virtual void onDeleteAllStatus(int32_t code) {
        logfile_print_d("Unexpected callback onDeleteAllStatus");
    }
    virtual void onVersionInfo(long major, long minor) {
        logfile_print_d("Unexpected callback onVersionInfo");
    }
    virtual void onConfigStatus(int32_t code) {
        logfile_print_d("Unexpected callback onConfigStatus");
    }
    virtual void onRenameStatus(int32_t code) {
        logfile_print_d("Unexpected callback onRenameStatus");
    }

protected:
    CallbackStatus status;
};

/*
* TestCaseCallbackEnroll - used for the enroll actions - regardless of
* mode - live or file based enrollment. Overridden API onErorr and onEnrollStatus.
* onError should only be called if there was a failure to call the secure env.
* Otherwise onEnrollStatus should be called
*/
class TestCaseCallbackEnroll : public TestCaseCallback {
public:
    TestCaseCallbackEnroll(vector<int32_t> codes) : expected(codes) {
        //default=Success. QvopCommandEnroll is success by default
        //unless call back occurs
        status = CBSuccess;
    };
    virtual ~TestCaseCallbackEnroll() {};

    virtual void onError(int32_t error) {
        /*an error has occured. this is abnormal program flow. result: failure*/
        logfile_print_e("TestCaseCallbackEnroll callback onError: %d", error);
        status = CBFailure;
    };

    virtual void onEnrollStatus(int32_t code) {
        logfile_print_d("TestCaseCallbackEnroll callback onEnrollStatus: %d", code);
        //using std::find from <algorithm>
        if (find(expected.begin(), expected.end(), code) == expected.end()) {
            // default is success, so just set failure, test ends
            status = CBFailure;
        }
    };

protected:
    vector<int32_t> expected;
};

/*
* TestCaseCallbackMatch - callback for all match cases. - regardless of
* mode - live or file based enrollment. Overridden API are onError, onMatchFound
* and onMatchNotFound. onError is abnormal program flow... should't be called
* under normal circumstances. onMatchFound and onMatchNotFound are self
* explanatory.
*/
class TestCaseCallbackMatch : public TestCaseCallback {
public:
    TestCaseCallbackMatch(bool expected_result) : expected(expected_result) {
        //default=Success
        status = CBSuccess;
    };
    virtual ~TestCaseCallbackMatch() {};

    virtual void onError(int32_t error) {
        /*an error has occured. this is abnormal program flow. result: failure*/
        logfile_print_e("TestCaseCallbackMatch callback onError: %d", error);
        status = CBFailure;
    };

    virtual void onMatchFound(const char* user_id,
                                const char* identifier,
                                int32_t index,
                                double sentence_score,
                                double ver_score,
                                float spoofScore) {
        /*success of test is dependent on expected result*/
        logfile_print_d("%s: expected: %s", QVOP_FN, expected ? "Match" : "NoMatch");
        status = (expected ? CBSuccess : CBFailure);
    }

    virtual void onMatchNotFound(int32_t error,
                                    int32_t index,
                                    double sentence_score,
                                    double ver_score,
                                    float spoofScore) {
        /*success of test is dependent on expected result*/
        logfile_print_d("%s: expected: %s", QVOP_FN, expected ? "Match" : "NoMatch");
        status = (!expected ? CBSuccess : CBFailure);
    }
protected:
    bool expected;
};


/*
* TestCaseCallbackProcessFrame - callback for all match cases. - regardless of
* mode - live or file based enrollment. Overridden API are onError, onMatchFound
* and onMatchNotFound. onError is abnormal program flow... should't be called
* under normal circumstances. onMatchFound and onMatchNotFound are self
* explanatory. Also note that a match can occur while processing frames
*/
class TestCaseCallbackProcessFrame : public TestCaseCallback {
public:
    TestCaseCallbackProcessFrame(bool expected_result) :
        expected(expected_result), complete(false) {
        //for this case, default status is success
        logfile_print_v("%s: TestCaseCallbackProcessFrame CTOR: %s",
            QVOP_FN,
            expected_result ? "TRUE":"FALSE");
        status = CBSuccess;
    };

    virtual ~TestCaseCallbackProcessFrame() {};

    //returns flag indicating whether the match was found yet.
    virtual bool IsComplete() {
        return complete;
    };

    //returns match status - only relevant if complete = true
    virtual QvopStatus GetMatch() {
        return match;
    }

    virtual void onError(int32_t error) {
        /*an error has occured. this is abnormal program flow. result: failure*/
        logfile_print_e("%s: callback onError: %d", QVOP_FN, error);
        complete = true;
        status = CBFailure;
    };

    virtual void onMatchFound(const char* user_id,
                                    const char* identifier,
                                    int32_t index,
                                    double sentence_score,
                                    double ver_score,
                                    float spoofScore) {
        /*success of test is dependent on expected result. either way test is complete*/
        logfile_print_d("%s: expected: %s", QVOP_FN, expected?"Match":"NoMatch");
        complete = true;
        status = expected ? CBSuccess : CBFailure;
        match = QVOP_MATCH_SUCCESS;
    };

    virtual void onMatchNotFound(int32_t error,
                                    int32_t index,
                                    double sentence_score,
                                    double ver_score,
                                    float spoofScore) {
        /*success of test is dependent on expected result. either way test is complete*/
        logfile_print_d("%s: expected: %s", QVOP_FN, expected ? "Match" : "NoMatch");
        complete = true;
        status = !expected ? CBSuccess : CBFailure;
        match = QVOP_MATCH_FAILURE;
    };

protected:
    /*expected results of process frame request -
    * FYI the processFrame call can have a match result
    */
    bool expected;
    /*flag incidating completeness of processing frames
    * i.e. if the process frame call results in a match success
    * or match failure, the processing should stop and test is over.
    */
    bool complete;

    //match status - which can occur while processing frames.
    QvopStatus match;
};

/*
* TestCaseCallbackDelete - used for delete cases... Gets callback to
* onDeleteStatus or onDeleteAllStatus with code. OnError is abnormal
* flow.
*/
class TestCaseCallbackDelete: public TestCaseCallback {
public:
    TestCaseCallbackDelete(int32_t code) : expected(code) {};
    virtual ~TestCaseCallbackDelete() {
        //for this case, default status is success
        status = CBSuccess;
    };

    virtual void onError(int32_t error) {
        /*an error has occured. this is abnormal program flow. result: failure*/
        logfile_print_e("TestCaseCallbackEnroll callback onError: %d", error);
        status = CBFailure;
    };

    virtual void onDeleteStatus(int32_t code,
                                const char* user_id,
                                const char* identifier) {
        /*success of test is dependent on expected result. either way test is complete*/
        logfile_print_d("onDeleteStatus user_id: %s, identifier: %s code: %d expected: %d",
            user_id,
            identifier,
            code,
            expected);
        status = code == expected ? CBSuccess : CBFailure;
    };

    virtual void onDeleteAllStatus(int32_t code) {
        /*success of test is dependent on expected result. either way test is complete*/
        logfile_print_d("onDeleteAllStatus code: %d expected: %d", code, expected);
        status = code == expected ? CBSuccess : CBFailure;
    };
protected:
    int32_t expected;
};


/*
* TestCaseCallbackGetVersion - used for GetVersion case... Not concerned
* at this point what the version is... just that we get a callback to
* onVersionInfo.
*/
class TestCaseCallbackGetVersion : public TestCaseCallback {
public:
    TestCaseCallbackGetVersion() {};
    virtual ~TestCaseCallbackGetVersion() {};

    virtual void onError(int32_t error) {
        /*an error has occured. this is abnormal program flow. result: failure*/
        logfile_print_e("TestCaseCallbackGetVersion callback onError: %d", error);
        status = CBFailure;
    };

    virtual void onVersionInfo(long major, long minor) {
        logfile_print_d("...callback onVersionInfo: %l:%l");
        status = CBSuccess;
    };
};

#endif //TEST_CASE_CALLBACKS_H
