/**
 * @file:  cpr_image_logs.c
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/src/cpr_image_logs.c#1 $
 * $Change: 16759227 $
 */

#include "cpr_image_logs.h"
#include "cpr_data.h"
#include "cpr_cfg.h"
#include "cpr_utils.h"

ULogHandle cprLogHandle;
ULogHandle cprStaticLogHandle;


void cpr_image_static_log_update_all(void)
{
    cpr_rail *rail = cpr_utils_get_rail(CPR_RAIL_MSS);

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

void cpr_image_static_log_init(void)
{
    if(cprStaticLogHandle)
        return;

    cpr_rail *rail = cpr_utils_get_rail(CPR_RAIL_MSS);
    uint32 num_slots = CPR_IMAGE_STATIC_LOG_SLOT_MODE_INFO_OFFSET + 2 * cpr_info.railStates[rail->railIdx].modeSettingsCount;

    ULogFront_RealTimeStaticInit(
            &cprStaticLogHandle,
            CPR_STATIC_LOG_NAME,
            num_slots,
            ULOG_MEMORY_LOCAL,
            ULOG_LOCK_OS);

    cpr_image_static_log_update_all();
}

const char* cpr_image_log_mode_name(cpr_voltage_mode mode)
{
    switch(mode)
    {
        case CPR_VOLTAGE_MODE_MIN_SVS    : return "SVS_MIN";
        case CPR_VOLTAGE_MODE_LOW_SVS    : return "SVS_LOW";
        case CPR_VOLTAGE_MODE_SVS        : return "SVS    ";
        case CPR_VOLTAGE_MODE_SVS_L1     : return "SVS_L1 ";
        case CPR_VOLTAGE_MODE_NOMINAL    : return "NOM    ";
        case CPR_VOLTAGE_MODE_NOMINAL_L1 : return "NOM_L1 ";
        case CPR_VOLTAGE_MODE_TURBO      : return "TUR    ";
        case CPR_VOLTAGE_MODE_SUPER_TURBO: return "STUR   ";
        default: return "?";
    }

    return "?";
}
