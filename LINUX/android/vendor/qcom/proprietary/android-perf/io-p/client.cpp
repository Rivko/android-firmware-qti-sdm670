/******************************************************************************
  @file    client.cpp
  @brief   Android performance iop library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014-2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#define LOG_TAG         "ANDR-IOP"
#include <cutils/log.h>
#include <cutils/properties.h>

#include "client.h"
#include "io-p.h"
#include <vendor/qti/hardware/iop/2.0/IIop.h>

#include "io-prefetch/list_capture.h"

#if defined(ANDROID_JELLYBEAN)
#include "common_log.h"
#endif

#define FAILED    -1
#define SUCCESS   0

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_vec;
using android::hardware::hidl_string;
using vendor::qti::hardware::iop::V2_0::IIop;
using ::android::sp;



int perf_io_prefetch_start(int pid, const char *pkg_name, const char *code_path)
{
    sp<IIop> gIopHal = IIop::tryGetService();
    if (gIopHal != NULL) {
        gIopHal->iopStart(pid, pkg_name, code_path);
     return SUCCESS;
    }
    return FAILED;
}


int perf_io_prefetch_stop()
{
       sp<IIop> gIopHal = IIop::tryGetService();
    if (gIopHal != NULL) {
        gIopHal->iopStop();
     return SUCCESS;
    }
    return FAILED;
}

int perf_ux_engine_events(int opcode, int pid, const char *pkg_name, int lat)
{
    sp<IIop> gIopHal = IIop::tryGetService();
    if (gIopHal != NULL) {
        gIopHal->uxEngine_events(opcode,pid,pkg_name,lat);
        return SUCCESS;
    }
    return FAILED;
}

char * perf_ux_engine_trigger(int opcode)
{
/* Enable this code for testing purpose using
   native client */
#if 0
    char *ret = NULL;
    sp<IIop> gIopHal = IIop::tryGetService();
    if (gIopHal != NULL) {
        //gIopHal->uxEngine_trigger(opcode, [&](char *res) {strlcpy(ret,res,2048)});
        return ret;
    }
#endif
    return NULL;
}
