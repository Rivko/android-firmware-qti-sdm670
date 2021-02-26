/**
 * @file:  cpr_image_logs.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/xbl/src/cpr_image_logs.c#1 $
 * $Change: 14402555 $
 */

#include "cpr_image_logs.h"
#include "cpr_data.h"
#include "cpr_cfg.h"

#include <stdio.h>
#include <stdarg.h>
#include "boot_logger.h"

#define CPR_LOG_BUF_SIZE 256


void cpr_image_log_message(const char * restrict msg, ...)
{
    char buf[CPR_LOG_BUF_SIZE] = {0};
    va_list ap;
    int len;

    va_start(ap, msg);
    len = vsnprintf(buf, CPR_LOG_BUF_SIZE-1, msg, ap);
    va_end(ap);

    buf[CPR_LOG_BUF_SIZE-1] = 0; /* make sure NULL terminated */

    boot_log_message(buf);
}

#if 0
void cpr_image_static_log_update_all(void)
{
    cpr_rail *rail = cpr_get_rail(CPR_RAIL_MSS);

    CPR_STATIC_LOG_CPR_INFO(&cpr_info);

    CPR_STATIC_LOG_RAIL_INFO(&cpr_info.railStates[rail->railIdx]);

    for(int modeIdx = 0; modeIdx < rail->vp->modesCount; modeIdx++)
    {
        cpr_mode_settings *modeSetting = &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];

        for(int submodeIdx = 0; submodeIdx < modeSetting->subModesCount; submodeIdx++)
        {
            cpr_submode_settings* submode = &modeSetting->subModes[submodeIdx];

            CPR_STATIC_LOG_MODE_INFO(&cpr_info.railStates[rail->railIdx], modeSetting, submode);
            CPR_STATIC_LOG_ISR_INFO(&cpr_info.railStates[rail->railIdx], modeSetting, submode);
        }
    }
}
#endif

