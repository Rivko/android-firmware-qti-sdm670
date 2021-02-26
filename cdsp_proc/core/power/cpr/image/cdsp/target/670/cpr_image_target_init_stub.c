/**
 * @file:  cpr_image_target_init_stub.c
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/target/670/cpr_image_target_init_stub.c#1 $
 * $Change: 16759227 $
 */
#include "cpr_cfg.h"
#include "cpr_image_target_init.h"
#include "cpr_logs.h"

//******************************************************************************
// Default Enablement Structures
//******************************************************************************

static cpr_cfg_funcs init_funcs[CPR_INIT_FUNCS_COUNT] =
{
[CPR_INIT_NONE]           = {.cMode = CPR_CONTROL_NONE, .init = NULL, .enable = NULL},
[CPR_INIT_OPEN_LOOP]      = {.cMode = CPR_CONTROL_NONE, .init = NULL, .enable = NULL},
[CPR_INIT_SW_CLOSED_LOOP] = {.cMode = CPR_CONTROL_NONE, .init = NULL, .enable = NULL},
};

cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id)
{
    CPR_ASSERT(func_id < CPR_INIT_FUNCS_COUNT);

    return &init_funcs[func_id];
}

void cpr_image_enable_closed_loop(void)
{
    return;
}

void cpr_image_disable_closed_loop(void)
{
    return;
}
