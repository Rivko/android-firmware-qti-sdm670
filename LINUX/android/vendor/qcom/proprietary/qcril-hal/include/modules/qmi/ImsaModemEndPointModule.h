/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/Module.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"

class ImsaModemEndPointModule : public ModemEndPointModule {
  public:
    ImsaModemEndPointModule(string name, ModemEndPoint &owner);
    ~ImsaModemEndPointModule();
    bool handleQmiBinding(
          qcril_instance_id_e_type instanceId, int8_t stackId);
    void init();

  private:
    qmi_idl_service_object_type getServiceObject() override;
};
