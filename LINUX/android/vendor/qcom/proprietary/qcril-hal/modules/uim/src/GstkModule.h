/******************************************************************************
#  @file    UimModem.h
#  @brief   Base class header file for UIM module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/uim/GstkRilRequestMessage.h"
#include "modules/uim/GstkQmiCatIndicationMsg.h"
#include "GstkQmiCatResponseMsg.h"
#include "UimGstkCardStatusMsg.h"
#include "framework/QcrilInitMessage.h"

class GstkModule : public Module
{
  private:
    bool       mReady;

  public:
    GstkModule();
    ~GstkModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalGstkModuleCleanup();
#endif

  private:
    void GstkProcessRilInit(std::shared_ptr<QcrilInitMessage> msg_ptr);
    void GstkProcessRilRequest(std::shared_ptr<GstkRilRequestMessage> msg);
    void GstkProcessQmiCatCallback(std::shared_ptr<GstkQmiCatResponseMsg> msg);
    void GstkProcessQmiCatInd(std::shared_ptr<GstkQmiCatIndicationMsg> msg);
    void GstkProcessCardStatusMsg(std::shared_ptr<UimGstkCardStatusMsg> msg);
};

GstkModule *getGstkModule();
