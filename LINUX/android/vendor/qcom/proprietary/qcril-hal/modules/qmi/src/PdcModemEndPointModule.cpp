/******************************************************************************
#  @file    PdcModemEndPointModule.h
#  @brief   Module implementation for PdcModemEndPoint Class. provides interface
#           to communicate with QMI PDC service.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <cstring>
#include "modules/qmi/PdcModemEndPointModule.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiPdcSetupRequestSync.h"


PdcModemEndPointModule::PdcModemEndPointModule(string name,
                    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {
    mServiceObject = nullptr;
    mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

    mMessageHandler = {
    };
}

PdcModemEndPointModule::~PdcModemEndPointModule() {
    mLooper = nullptr;
}

qmi_idl_service_object_type PdcModemEndPointModule::getServiceObject() {
  return pdc_get_service_object_v01();
}

