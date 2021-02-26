/**
 * @file:  cpr_rail.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_rail.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_RAIL_H
#define	CPR_RAIL_H

#include "cpr_data.h"
#include "cpr_cfg.h"


void cpr_rail_init(cpr_rail* rail);
void cpr_rail_init_rail_hw(cpr_rail* rail, boolean configureHW);
boolean cpr_rail_process_results(cpr_rail* rail, cpr_results* rslts);
void cpr_rail_calculate_target_quotients(cpr_rail* rail,
        cpr_rail_state* state);
void cpr_rail_set_initial_voltages(cpr_rail* rail, boolean useGlobalCeiling,
        boolean useGlobalFloor);
void cpr_rail_disable_lower_modes(cpr_rail* rail, cpr_rail* fuse_rail,
        boolean updateOpenLoop, boolean updateClosedLoop,
        boolean forceDisable, boolean turboOnly);
void cpr_rail_enable(cpr_rail* rail, cpr_mode_settings* modeState,
        cpr_submode_settings* submodeState, boolean changeOveride);
void cpr_rail_disable(cpr_rail* rail);
void cpr_rail_register_isr(cpr_rail* rail);
void cpr_rail_isr(void* ctx);
void cpr_rail_write_all_voltages(cpr_rail* rail);
void cpr_rail_write_all_target_quotients(cpr_rail* rail);
void cpr_rail_enable_hw_closed_loop( cpr_rail *rail );
void cpr_rail_write_cprf_settings( cpr_rail *rail, uint8 cprf_id );

#endif

