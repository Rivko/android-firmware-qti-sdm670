/******************************************************************************
#  @file    PdcModemEndPoint.h
#  @brief   Header file for PdcModemEndPoint Class. provides interface to
#           communicate with QMI PDC service.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/


#pragma once
#include <vector>

#include "qmi_client.h"

#include "modules/qmi/PdcModemEndPointModule.h"
#include "modules/qmi/ModemEndPoint.h"
#include "framework/Log.h"

class PdcModemEndPoint : public ModemEndPoint {
public:
    static constexpr const char *NAME = "PDC";
    PdcModemEndPoint() :ModemEndPoint("PDC") {
        mModule = new PdcModemEndPointModule("PdcModemEndPointModule", *this);
        mModule->init();
    }
    ~PdcModemEndPoint() {
        Log::getInstance().d("[PdcModemEndPoint]: destructor");
        //mModule->killLooper();
        delete mModule;
        mModule = nullptr;
    }

    void requestSetup(string clientToken, GenericCallback<string>* cb);
};
