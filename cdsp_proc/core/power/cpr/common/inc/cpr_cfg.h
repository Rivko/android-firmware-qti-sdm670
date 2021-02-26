/**
 * @file:  cpr_cfg.h
 * @brief:
 *
 * Copyright (c) 2015-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/inc/cpr_cfg.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_CFG_H
#define	CPR_CFG_H

#include "cpr_types.h"
#include "cpr_voltage_plan.h"
#include "cpr_hal.h"
#include "cpr_image_target_init.h"

struct cpr_rail;

typedef struct cpr_cfg_funcs
{
    cpr_control_mode cMode;
    struct cpr_cfg_funcs* (*init)(struct cpr_rail* rail);
    struct cpr_cfg_funcs* (*enable)(struct cpr_rail* rail);
} cpr_cfg_funcs;

typedef struct
{
    cpr_domain_id id;
    cpr_init_funcs_id funcId;

    const cpr_version *version;
    cpr_fuse railCPRRevFuse;

    // parameters not in the voltage plan
    uint16 stepSize;
    uint32 fuseMultiplier;
    int32 thermalAdjustment[CPR_THERMAL_REGION_MAX];
} cpr_enablement;

/*
 * Static read-only config for rails
 */
typedef struct cpr_rail
{
    cpr_domain_id id;
    uint8 railIdx;
    uint8 settleModesCount;

    const char* name;

    // HW rail settings
    cpr_hal_handle hal;

    uint32 interruptId;

    const cpr_voltage_mode *settleModes;

    // versioned external configuration
    const cpr_voltage_plan* vp;

    // versioned internal configuration
    const cpr_enablement* enablement;
} cpr_rail;

typedef struct {
    cpr_fuse cprRev;
} cpr_misc_cfg;

#endif
