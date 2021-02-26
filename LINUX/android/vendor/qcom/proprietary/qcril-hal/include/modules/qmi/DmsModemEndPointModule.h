/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/ModuleLooper.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"

#include "common_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "voice_service_v02.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/GetOperatingModeSyncMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

class DmsModemEndPointModule : public ModemEndPointModule {
 public:
  DmsModemEndPointModule(string name, ModemEndPoint& owner);
  ~DmsModemEndPointModule();

 private:
  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId) override;

  void handleGetOperatingMode(std::shared_ptr<GetOperatingModeSyncMessage> msg);
};
