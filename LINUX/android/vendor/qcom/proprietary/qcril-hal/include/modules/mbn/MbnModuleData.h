/******************************************************************************
#  @file    MbnModuleData.h
#  @brief   this provides provides the implementation for all the Data structures
    related to MBN module.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include <framework/Message.h>
#include <framework/Log.h>
#include "ril.h"
#include <string>
//TODO need to re-check include files

using std::string;

/*
 * Class definition for hwMbnInitRequest
*/
class hwMbnInitRequest {
private:
    int mModemMaxSub;
    int mApssMaxSub;
public:
    inline hwMbnInitRequest(){
        mModemMaxSub = 0;
        mApssMaxSub = 0;
    }
    inline ~hwMbnInitRequest(){};
    inline hwMbnInitRequest(int modemSub, int apssSub) :
        mModemMaxSub(modemSub), mApssMaxSub(apssSub) {};

    inline void setModemSub(int modemSub) {
        mModemMaxSub = modemSub;
    }

    inline void setApssSub(int appsSub) {
        mApssMaxSub = appsSub;
    }

    inline int getModemSub() {
        return mModemMaxSub;
    }
    inline int getApssSub() {
        return mApssMaxSub;
    }
    void dump() {
        Log::getInstance().d("[hwMbnInitRequest] modemsub " +
                            std::to_string(mModemMaxSub) + "appsSub "+ std::to_string(mApssMaxSub));
    }

};


