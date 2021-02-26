/**
 * @file:  cpr_defs.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/inc/cpr_defs.h#1 $
 * $Change: 15485400 $
 */
#ifndef CPR_DEFS_H
#define CPR_DEFS_H

#include "comdef.h"

typedef enum
{
    CPR_VOLTAGE_MODE_OFF,
    CPR_VOLTAGE_MODE_RETENTION,
    CPR_VOLTAGE_MODE_MIN_SVS,
    CPR_VOLTAGE_MODE_LOW_SVS,
    CPR_VOLTAGE_MODE_SVS,
    CPR_VOLTAGE_MODE_SVS_L1,
    CPR_VOLTAGE_MODE_NOMINAL,
    CPR_VOLTAGE_MODE_NOMINAL_L1,
    CPR_VOLTAGE_MODE_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR,

    CPR_VOLTAGE_MODE_COUNT
} cpr_voltage_mode;

typedef enum
{
    // Core rails
    CPR_RAIL_MX     = 0x100,
    CPR_RAIL_CX,
    CPR_RAIL_MSS,
    CPR_RAIL_GFX,
    CPR_RAIL_VDDA,

    // APC rails
    CPR_RAIL_APC0   = 0x200,
    CPR_RAIL_APC1,
    CPR_RAIL_APC2,
    CPR_RAIL_APC3,
    CPR_RAIL_APC4,
    CPR_RAIL_APC5,
    CPR_RAIL_CBF,
    CPR_RAIL_APCC_LVT,
    CPR_RAIL_APCC_SLVT,
    CPR_RAIL_APCC_M4M,

    // SSC rails
    CPR_RAIL_SSC_MX = 0x300,
    CPR_RAIL_SSC_CX,

    CPR_RAIL_LPI_MX = 0x400,
    CPR_RAIL_LPI_CX
} cpr_domain_id;

typedef enum {
    CPR_DOMAIN_TYPE_MODE_BASED,
    CPR_DOMAIN_TYPE_FREQ_BASED
} cpr_domain_type;

typedef struct
{
    cpr_domain_type type;
    union
    {
        cpr_voltage_mode mode;
        uint32 freq;
    } u;
} cpr_domain_info;

typedef cpr_domain_id cpr_rail_id_t;
typedef cpr_voltage_mode cpr_voltage_mode_t;

#endif //CPR_DEFS_H
