/******************************************************************************
  @file  module3.cpp
  @brief test module to load into hal for hal testing

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "mp-ctl.h"
#include "EventQueue.h"
#include "PerfGlueLayer.h"
#include "ttf.h"
#include <stdio.h>

static int mod3Events[] = {
   VENDOR_ACT_TRIGGER_HINT_BEGIN,
   VENDOR_ACT_TRIGGER_HINT_END,
};

static PerfGlueLayer mod3glue = {
   "libqti-tests-mod3.so",
   mod3Events, sizeof(mod3Events)/sizeof(int),
};

//interface implementation
int perfmodule_init() {
    int rc = 0, ret = SUCCESS;

    return ret;
}

void perfmodule_exit() {
}

int perfmodule_submit_request(mpctl_msg_t *msg) {
    EventData *evData;
    int ret = FAILED;

    if (NULL == msg) {
        return ret;
    }

    printf("mod3 - run\n");

    ret = SUCCESS;

    return ret;
}
