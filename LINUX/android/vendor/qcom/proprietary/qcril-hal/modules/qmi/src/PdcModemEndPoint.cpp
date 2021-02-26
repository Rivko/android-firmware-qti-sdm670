/******************************************************************************
#  @file    PdcModemEndPoint.cpp
#  @brief   Definition for PdcModemEndPoint Class. provides interface to
#           communicate with QMI PDC service.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/qmi/PdcModemEndPoint.h"
//TODO add qmi service header files if needed
//#include "qmi.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"
#include "modules/qmi/QmiPdcSetupRequestSync.h"

using std::to_string;
constexpr const char *PdcModemEndPoint::NAME;

void PdcModemEndPoint::requestSetup(string clientToken,
                                    GenericCallback<string>* callback) {
    auto shared_SetupMsg = std::make_shared<QmiSetupRequest>
                            (clientToken,0,nullptr,callback);
    mModule->dispatch(shared_SetupMsg);
}
