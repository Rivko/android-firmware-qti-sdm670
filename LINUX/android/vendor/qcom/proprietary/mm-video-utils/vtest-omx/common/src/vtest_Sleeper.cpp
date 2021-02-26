/*-------------------------------------------------------------------
Copyright (c) 2013-2016 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------*/

#include <unistd.h>
#include "vtest_Sleeper.h"
#include "vtest_Debug.h"

namespace vtest {

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int vt_sleep(int time_millis)
{
   usleep(time_millis * 1000);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Sleeper::Sleep(OMX_S32 nTimeMillis) {
    OMX_ERRORTYPE result = OMX_ErrorNone;

    if (vt_sleep((int)nTimeMillis) != 0) {
        VTEST_MSG_ERROR("error sleeping");
        result = OMX_ErrorUndefined;
    }

    return result;
}

int Sleeper::SleepUs(OMX_S64 nTimeMicroUs) {
    struct timespec reqt;

    reqt.tv_sec  = 0;
    reqt.tv_nsec = nTimeMicroUs * 1000;
    nanosleep(&reqt, NULL);

    return 0;
}

} // namespace vtest
