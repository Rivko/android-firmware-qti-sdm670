/**
 * @file:  cpr_target.h
 * @brief:
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 */

#ifndef CPR_TARGET_H
#define CPR_TARGET_H

#include "cprf.h"
#include "cpr_types.h"
#include "cpr_hwio.h"
#include "cprf_hwio.h"
#include "cpr_internal.h"
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_internal.h"
#include "rpmh_arc.h"

void cpr_target_init(void);

cpr_return_status cpr_target_set_closed_loop(uint8 id, boolean isClosedLoop);
boolean cpr_target_is_closed_loop(uint8 id);

cpr_return_status cpr_target_cprf_ctrl_intr_en(uint8 id, boolean enable,
        boolean force);

cpr_return_status cpr_target_cprc_loop_en(uint32 base, boolean enable);

cpr_return_status cpr_target_enable_cprf_interface_en(uint8 id, boolean enable);

uint32 cpr_target_get_ret_voltage (uint8 id);

cpr_return_status cpr_target_set_ret_voltage (uint8 id, uint32 vol);

cpr_return_status cpr_target_set_target_quotient(uint32 base, uint8 thread,
        uint8 mode, uint32 ro, uint32 quotient);

cpr_return_status cpr_target_set_submode(uint8 cprf_id, uint8 hlvl,
        uint32 ceiling, uint32 floor);

cpr_return_status cpr_target_update_rail_settings(cpr_rail_state *state,
        uint8 cprf_id, uint32 base, uint32 thread);

cpr_return_status cpr_target_free_rail_settings(cpr_rail_state *state);
#endif
