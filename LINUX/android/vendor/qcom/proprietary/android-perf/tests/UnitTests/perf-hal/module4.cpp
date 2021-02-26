/******************************************************************************
  @file  module4.cpp
  @brief test module to load into hal for hal testing

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "mp-ctl.h"
#include "PerfGlueLayer.h"
#include "ttf.h"
#include <stdio.h>

static int mod4Events[] = {
   VENDOR_ACT_TRIGGER_HINT_BEGIN,
   VENDOR_ACT_TRIGGER_HINT_END,
};

static PerfGlueLayer mod4glue = {
   "libqti-tests-mod4.so",
   mod4Events, sizeof(mod4Events)/sizeof(int),
};

//interface implementation
int perfmodule_init() {
    int rc = 0, ret = SUCCESS;

    return ret;
}

void perfmodule_exit() {
}

int perfmodule_submit_request(mpctl_msg_t *msg) {
    int ret = FAILED;

    if (NULL == msg) {
        return ret;
    }
    printf("mod4 - run()");
    ret = SUCCESS;

    return ret;
}
