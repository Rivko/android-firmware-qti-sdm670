/**
 * @file:  cpr_checksum.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/src/cpr_checksum.c#1 $
 * $Change: 16759227 $
 */
#include "railway.h"
#include "CoreVerify.h"
#include "cpr_voltage_ranges.h"
#include "cpr_enablement.h"

typedef enum
{
    CPR_CHECKSUM_PART_VOLTAGE_LEVEL,
    CPR_CHECKSUM_PART_SUPPORTED_LEVEL,
    CPR_CHECKSUM_PART_MODE_TO_SETTLE,
    CPR_CHECKSUM_PART_ENABLEMENT_TYPE,
    CPR_CHECKSUM_PART_COUNT,
} cpr_checksum_part_t;

static uint32 checksum32(const uint8 *data, uint32 size)
{
    uint32 sum1 = 0;
    uint32 sum2 = 0;

    while(size--)
    {
        sum1 = 0xffff & (sum1 + *data++);
        sum2 = 0xffff & (sum1 + sum2);
    }

    return  (sum2 << 16) | sum1;
}

static uint32 cpr_rail_checksum(cpr_rail_id_t rail_id)
{
    uint32 checksum_array[CPR_CHECKSUM_PART_COUNT];

    const cpr_config_versioned_voltage_ranges_t* voltage_ranges = cpr_config_find_versioned_voltage_ranges(rail_id);
    if(!voltage_ranges)
    {
        return 0; // voltage_ranges was null
    }

    const cpr_enablement_versioned_rail_config_t* rail_config = cpr_enablement_find_versioned_rail_config(rail_id);
    if(!rail_config)
    {
        return 0; // rail_config was null
    }
    CPR_ASSERT(!rail_config->enablement_init_params); // or should we just return 0?

    // checksum for all voltage level info/rail_config supported levels/rail_config mode to settle
    checksum_array[CPR_CHECKSUM_PART_VOLTAGE_LEVEL]   = checksum32((uint8*)(voltage_ranges->voltage_level), voltage_ranges->voltage_level_count * sizeof(cpr_config_voltage_level_t) );
    checksum_array[CPR_CHECKSUM_PART_SUPPORTED_LEVEL] = checksum32((uint8*)(rail_config->supported_level), rail_config->supported_level_count * sizeof(cpr_enablement_supported_level_t) );
    checksum_array[CPR_CHECKSUM_PART_MODE_TO_SETTLE]  = checksum32((uint8*)(rail_config->mode_to_settle), rail_config->mode_to_settle_count * sizeof(cpr_voltage_mode_t) );
    checksum_array[CPR_CHECKSUM_PART_ENABLEMENT_TYPE] = checksum32((uint8*)(rail_config->enablement_init_params->enablement_type), sizeof(cpr_enablement_type_t) );

    return checksum32((uint8*)checksum_array, sizeof(checksum_array));
}

uint32 cpr_cx_mx_settings_checksum(void)
{
    cpr_rail_id_t checksum_rails[] = {CPR_RAIL_MX, CPR_RAIL_CX, CPR_RAIL_VDDA_EBI};
    uint32 checksum_rails_count = sizeof(checksum_rails)/sizeof(cpr_rail_id_t);
    uint32 checksum_data[checksum_rails_count];

    for(int i = 0; i < checksum_rails_count; i++)
    {
        checksum_data[i] = cpr_rail_checksum(checksum_rails[i]);
    }

    return checksum32((uint8*)checksum_data, sizeof(checksum_data)); //expecting data in 16 bit increments, but checksum_data is already in 32 bit
}
