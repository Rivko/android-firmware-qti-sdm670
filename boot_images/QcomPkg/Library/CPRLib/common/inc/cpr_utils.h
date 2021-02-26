/**
 * @file:  cpr_utils.h
 * @brief:
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_utils.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_UTILS_H
#define	CPR_UTILS_H

#include "cpr_data.h"
#include "cpr_image.h"


static inline const cpr_settings* cpr_utils_get_settings(void)
{
    return (cpr_info.settings) ? cpr_info.settings : cpr_image_get_settings();
}

static inline uint32 cpr_utils_get_num_rails(void)
{
    return cpr_info.settings->railsCount;
}

const cpr_versioned_voltage_plan* cpr_utils_get_versioned_voltage_plan(cpr_domain_id railId);
cpr_hal_controller* cpr_utils_get_hal_controller(cpr_domain_id railId);
cpr_misc_cfg* cpr_utils_get_misc_cfg(void);
cpr_rail* cpr_utils_get_rail(cpr_domain_id railId);
uint32 cpr_utils_get_rail_idx(cpr_domain_id railId);
uint32 cpr_utils_get_mode_idx(cpr_rail* rail, cpr_voltage_mode mode);

static inline cpr_rail_state* cpr_utils_get_rail_state(cpr_domain_id railId)
{
    return &cpr_info.railStates[cpr_utils_get_rail_idx( railId )];
}

void cpr_utils_set_disable_vote(cpr_domain_id railId, uint32 vote);
void cpr_utils_clear_disable_vote(cpr_domain_id railId, uint32 vote);

static inline boolean cpr_utils_has_disable_vote(cpr_domain_id railId, uint32 vote)
{
    return (0 != (cpr_info.railStates[cpr_utils_get_rail_idx( railId )].disableVotes & vote));
}

static inline cpr_mode_settings* cpr_utils_get_active_mode_setting(cpr_domain_id railId)
{
    return cpr_info.railStates[cpr_utils_get_rail_idx( railId )].activeMode;
}

static inline cpr_mode_settings* cpr_utils_get_previous_mode_setting(cpr_domain_id railId)
{
    return cpr_info.railStates[cpr_utils_get_rail_idx( railId )].previousMode;
}

static inline boolean cpr_utils_is_closed_loop_mode(cpr_domain_id railId)
{
    uint32 idx = cpr_utils_get_rail_idx( railId );

    return ((cpr_info.railStates[idx].cMode == CPR_CONTROL_SW_CLOSED_LOOP) ||
            (cpr_info.railStates[idx].cMode == CPR_CONTROL_HW_CLOSED_LOOP));
}

const cpr_margin_data* cpr_utils_get_margins(const uint8 railCPRRev, const cpr_margin_cfg* marginList);
void cpr_utils_get_mode_settings(cpr_domain_id railId, cpr_domain_info* info, cpr_mode_settings** mode, cpr_submode_settings** submode);
int16 cpr_utils_decode_fuse_value(const cpr_fuse* fuse, uint8 packingFactor, boolean isSigned);
void cpr_utils_set_active_mode_setting(cpr_domain_id railId, cpr_mode_settings* newModeSetting, cpr_submode_settings* newSubModeSetting);

#endif

