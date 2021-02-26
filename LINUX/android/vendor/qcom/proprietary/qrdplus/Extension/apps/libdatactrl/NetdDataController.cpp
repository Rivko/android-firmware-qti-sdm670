/*************************************************
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*********************************************************/

#include <DataController.h>
#include <cutils/log.h>

static DataController *dc = NULL ;

void initDataControllerInstance()
{
    ALOGD("initDataController cons ");
    if (dc == NULL) {
        dc = new DataController();
    }
}

extern "C" void initDataController()
{
    initDataControllerInstance();
}

extern "C" int blockAllData()
{
    int ret = -1;
    if (dc) {
        ret = dc->blockAllData();
    }
    return ret;
}

extern "C" int unblockAllData()
{
    int ret = -1;
    if (dc) {
        ret = dc->unblockAllData();
    }
    return ret;
}

extern "C" unsigned checkAppInWhitelist(SocketClient *cli)
{
    int ret = -1;
    if (dc) {
        ret = dc->checkAppInWhitelist(cli);
    }
    return ret;
}
