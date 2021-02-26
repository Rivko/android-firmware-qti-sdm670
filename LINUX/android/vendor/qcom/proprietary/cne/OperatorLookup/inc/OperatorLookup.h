/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef OPERATOR_LOOKUP_H
#define OPERATOR_LOOKUP_H

#include <string>
#include <cutils/log.h>

using namespace std;

const char* default_operator = "ROW";
const string ERROR_STR = "ERROR";

typedef struct _IINInfo {
    string name;
    string iin;
    _IINInfo(string n="",string i=""){
        name = n;
        iin = i;
    };
} IINInfo;

class OperatorLookup {
    public:
        OperatorLookup() {};
        string lookupOperator(const string&);

    private:
        static const IINInfo IINInfoArray[];
};
#endif
