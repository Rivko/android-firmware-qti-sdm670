/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <DataCtl.h>
#include <cutils/log.h>

static DataCtl *dc = NULL ;

extern "C" void initCsmDataCtl()
{
    ALOGI("DataCtl new obj ");
    if (dc == NULL) {
        dc = new DataCtl();
    }
}

extern "C" bool enableMms(char *blackListUids)
{
    bool ret = false;
    if (dc) {
        ret = dc->enableMms(blackListUids);
    }
    return ret;
}

extern "C" bool enableData(char *blackListUids)
{
    bool ret = false;
    if (dc) {
        ret = dc->enableData(blackListUids);
    }
    return ret;
}
