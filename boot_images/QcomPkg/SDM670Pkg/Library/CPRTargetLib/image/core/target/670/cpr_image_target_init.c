/**
 * @file:  cpr_init.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/10/31 00:28:19 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/image/core/target/670/cpr_image_target_init.c#2 $
 * $Change: 14733083 $
 */
#include <string.h>
#include "cpr_logs.h"
#include "cpr_data.h"
#include "cpr_rail.h"
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_measurements.h"
#include "cpr_cfg.h"
#include "cpr_hal.h"
#include "cpr_utils.h"
#include "cpr_image_target_init.h"
#include "cpr_image_target.h"
#include "cprf_hwio.h"

//******************************************************************************
// Local Helper Functions
//******************************************************************************


static cpr_cfg_funcs* init_open_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "Initializing open loop on %s", rail->name );

    /*
     * This is to workaround the fuse reference voltage change from global ceiling to TT voltage.
     *
     * For   CPR Rev0, use the global ceiling voltage.
     * For > CPR Rev0, use the fuse reference voltage.
     */
    cpr_rail_set_initial_voltages( rail, (cpr_info.cprRev == 0), false );

    return NULL;
}

static cpr_cfg_funcs* init_settle(cpr_rail* rail)
{
    CPR_LOG_TRACE( "Settling on %s", rail->name );

    for(int i = 0; i < rail->settleModesCount; i++) {
        cpr_measurements_settle( rail, rail->settleModes[i] );
    }

    CPR_LOG_TRACE( "--- Done with settling ---" );

    return NULL;
}

static cpr_cfg_funcs* init_hw_open_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- CPR HW Open Loop ---" );

    //cpr_rail_init_rail_hw( rail, true );

    init_open_loop(rail);

    cpr_rail_write_all_voltages( rail );

    return NULL;
}

static cpr_cfg_funcs* enable_hw_open_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- Enable CPR HW Open Loop ---" );

    cpr_rail_write_cprf_settings( rail, GFX_CPRF_ID );

    return NULL;
}

static cpr_cfg_funcs* init_hw_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- CPR HW Closed Loop ---" );

    cpr_rail_init_rail_hw( rail, true );

    cpr_measurements_aging( rail );

    init_open_loop(rail);

    CPR_LOG_TRACE( "Configuring closed loop on %s", rail->name );

    cpr_rail_calculate_target_quotients( rail, &cpr_info.railStates[rail->railIdx] );

    cpr_rail_write_all_voltages( rail );

    cpr_rail_write_all_target_quotients( rail );

    return NULL;
}

static cpr_cfg_funcs* enable_hw_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- Enable CPR HW Closed Loop ---" );

    cpr_rail_write_cprf_settings( rail, GFX_CPRF_ID );

    cpr_rail_enable_hw_closed_loop( rail );

    return NULL;
}

static cpr_cfg_funcs* init_closed_loop(cpr_rail* rail)
{
    cpr_rail_init_rail_hw( rail, true );

    cpr_measurements_aging( rail );

    init_open_loop(rail);

    CPR_LOG_TRACE( "Configuring closed loop on %s", rail->name );

    cpr_rail_calculate_target_quotients( rail, &cpr_info.railStates[rail->railIdx] );

    init_settle(rail);

    return NULL;
}

static cpr_cfg_funcs* enable_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "Enabling closed loop on %s", rail->name );

    cpr_rail_register_isr( rail );

    return NULL;
}


//******************************************************************************
// Default Enablement Structures
//******************************************************************************

static cpr_cfg_funcs init_funcs[CPR_INIT_FUNCS_COUNT] =
{
[CPR_INIT_NONE]           = {.cMode = CPR_CONTROL_NONE,           .init = NULL,                .enable = NULL},
[CPR_INIT_OPEN_LOOP]      = {.cMode = CPR_CONTROL_OPEN_LOOP,      .init = init_open_loop,      .enable = NULL},
[CPR_INIT_SW_CLOSED_LOOP] = {.cMode = CPR_CONTROL_SW_CLOSED_LOOP, .init = init_closed_loop,    .enable = NULL},
[CPR_INIT_HW_OPEN_LOOP]   = {.cMode = CPR_CONTROL_HW_OPEN_LOOP,   .init = init_hw_open_loop,   .enable = enable_hw_open_loop},
[CPR_INIT_HW_CLOSED_LOOP] = {.cMode = CPR_CONTROL_HW_CLOSED_LOOP, .init = init_hw_closed_loop, .enable = enable_hw_closed_loop},
};

cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id)
{
    CPR_ASSERT(func_id < CPR_INIT_FUNCS_COUNT);

    return &init_funcs[func_id];
}
