/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"

class QtiRadio : public Module {
  public:
    QtiRadio();
    ~QtiRadio();
    void init();

  private:
      void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
};


