/******************************************************************************
#  @file    PdcModemEndPointModule.h
#  @brief   Module implementation for PdcModemEndPoint Class. provides interface to
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
#include "framework/Module.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"

//TODO add pdc header files
#include "common_v01.h"
#include "qmi_client.h"
#include "device_management_service_v01.h"
#include "persistent_device_configuration_v01.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

class PdcModemEndPointModule : public ModemEndPointModule {
private:
  qmi_idl_service_object_type getServiceObject() override;

public:
    PdcModemEndPointModule(string name, ModemEndPoint& owner);
    ~PdcModemEndPointModule();

};
