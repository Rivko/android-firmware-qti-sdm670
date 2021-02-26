/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <loc_pla.h>
#include <log_util.h>
#include <GardenUtil.h>

namespace garden {

uint64_t getRealTimeNsec()
{
    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

uint64_t getUpTimeSec()
{
    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1000000000LL);
}

} // namespace garden
