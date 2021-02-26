/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_DELETE_H
#define TEST_CASE_DELETE_H

#include "QvopService.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "logfile.h"

using namespace qvop;
using namespace std;

/*
Base class for delete test cases. See below for specific cases.
Base class converts params into mem vars.. validates them as well.
*/
class TestCaseDelete : public TestCase {
public:
    TestCaseDelete() : user_id(DEFAULT_DEL_USER_ID), identifier(DEFAULT_DEL_IDENTIFIER) {};
    virtual ~TestCaseDelete() { }

    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
        //base testcase is a failure. see specific test cases below
        return TCFailure;
    }

protected:
    /*
    * Helper function to parse thru the parameters provided
    * Parameters required
    * KEY_USERID            : --user-id
    *                       : name of user
    * KEY_IDENTIFIER        : --identifier
    *                       : name of identifier
    */
    virtual bool ConvertParams(vector<string> const& params) {
        size_t found = string::npos;

        for (auto it = begin(params); it != end(params); ++it) {
            if (it->compare(KEY_USERID) == 0) {
                //move to next item
                //after KEY_USERID, only one user name should be found
                if (++it != end(params)) {
                    //found variable indicates whether PARAM_INDICATOR is in the next string
                    found = it->find(PARAM_INDICATOR);
                    if (found == string::npos) {
                        user_id = it->c_str();
                        logfile_print_d("%s: user_id %s ", QVOP_FN, user_id.c_str());
                    }
                    else {
                        //in the case where the key was provided e.g. --user-id
                        //but no user name was provided, move the pointer back
                        //to look at the next key
                        //e.g. --user-id --identfier myid
                        //which will cause the Run() to fail due to a lack of a
                        //user id
                        --it;
                        break;
                    }
                }
            }
            else if (it->compare(KEY_IDENTIFIER) == 0) {
                //move to next item
                //after KEY_IDENTIFIER, one id should be found
                if (++it != end(params)) {
                    //found variable indicates whether PARAM_INDICATOR is in the next string
                    found = it->find(PARAM_INDICATOR);
                    if (found == string::npos) {
                        identifier = it->c_str();
                        logfile_print_d("%s: identifier %s ", QVOP_FN, identifier.c_str());
                    }
                    else {
                        //in the case where the key was provided e.g. --identfier
                        //but no identfier was provided, move the pointer back
                        //to look at the next key
                        //e.g. --identfier --user-id myid
                        //which will cause the Run() to fail due to a lack of an
                        //identifier
                        --it;
                        break;
                    }
                }
            }
            else {
                logfile_print_e("%s: unknown parameter: %s", QVOP_FN, it->c_str());
            }
        }

        //boolean return value allows for failure case
        if (user_id == string(DEFAULT_DEL_USER_ID)) {
            logfile_print_e("%s: need to init user id", QVOP_FN);
            return TCBadParams;
        }
        //default
        return true;
    }

protected:
    string user_id;
    string identifier;
};

class TestCaseDeleteAll : public TestCaseDelete {
public:
    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
        //expected = 0; should add to parameter list
        TestCaseCallbackDelete cb(0);

        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        if (service.deleteAll(cb, user_id.c_str()) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call delete all", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }
};

class TestCaseDeleteUser : public TestCaseDelete {
public:
    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
        //expected = 0; should add to parameter list
        TestCaseCallbackDelete cb(0);
        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        if (service.deleteUser(cb, user_id.c_str()) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call deleteuser", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }
};

class TestCaseDeleteUserWithId : public TestCaseDelete {
public:
    virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
        //expected = 0; should add to parameter list
        TestCaseCallbackDelete cb(0);

        if (ConvertParams(params) == false) {
            logfile_print_e("%s: failed to convert params ", QVOP_FN);
            return TCBadParams;
        }

        //error in "withid" case, but not in other cases
        if (identifier == string(DEFAULT_DEL_IDENTIFIER)) {
            logfile_print_e("%s: need to init identifier", QVOP_FN);
            return TCBadParams;
        }

        if (service.deleteWithId(cb, user_id.c_str(), identifier.c_str()) != QVOP_SUCCESS ||
            cb.GetStatus() == CBFailure) {
            logfile_print_e("%s: failed to call deleteWithId", QVOP_FN);
            return TCFailure;
        }

        return TCSuccess;
    }
};
#endif //TEST_CASE_DELETE_H
