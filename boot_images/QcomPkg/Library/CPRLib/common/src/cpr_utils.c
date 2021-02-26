/**
 * @file:  cpr_utils.c
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr_utils.c#1 $
 * $Change: 14402555 $
 */

#include "cpr_utils.h"


const cpr_versioned_voltage_plan* cpr_utils_get_versioned_voltage_plan(cpr_domain_id railId)
{
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        if(cpr_info.settings->railVVPs[i]->rail == railId)
        {
            return cpr_info.settings->railVVPs[i];
        }
    }

    CPR_ASSERT(0);

    return NULL;
}

cpr_hal_controller* cpr_utils_get_hal_controller(cpr_domain_id railId)
{
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        if(cpr_info.settings->rails[i]->id == railId)
        {
            cpr_rail *rail = cpr_info.settings->rails[i];
            cpr_hal_handle *hal = &(rail->hal);
            return hal->controller;
        }
    }

    CPR_ASSERT(0);

    return NULL;
}

cpr_misc_cfg* cpr_utils_get_misc_cfg(void)
{
    cpr_misc_cfg* miscCfg = cpr_info.settings->miscCfg;

    CPR_ASSERT(miscCfg);
    return miscCfg;
}

cpr_rail* cpr_utils_get_rail(cpr_domain_id railId)
{
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        if(cpr_info.settings->rails[i]->id == railId)
        {
            return cpr_info.settings->rails[i];
        }
    }

    CPR_ASSERT(0);

    return NULL;
}

uint32 cpr_utils_get_rail_idx(cpr_domain_id railId)
{
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        if(cpr_info.settings->rails[i]->id == railId)
        {
            return i;
        }
    }

    CPR_ASSERT(0);

    return 0;
}

/*
 * Round up to the nearest mode. Should not abort.
 */
uint32 cpr_utils_get_mode_idx(cpr_rail* rail, cpr_voltage_mode mode)
{
    for(int i = 0; i < rail->vp->modesCount; i++)
    {
        if(mode <= rail->vp->supportedModes[i])
        {
            return i;
        }
    }

    return rail->vp->modesCount - 1;
}

void cpr_utils_set_disable_vote(cpr_domain_id railId, uint32 vote)
{
    uint32 idx = cpr_utils_get_rail_idx( railId );

    cpr_info.railStates[idx].disableVotes |= vote;
    CPR_LOG_TRACE(" DisableVotes: 0x%x (set voter:0x%x)", cpr_info.railStates[idx].disableVotes, vote);
    CPR_STATIC_LOG_RAIL_INFO(&cpr_info.railStates[idx]);
}

void cpr_utils_clear_disable_vote(cpr_domain_id railId, uint32 vote)
{
    uint32 idx = cpr_utils_get_rail_idx( railId );

    cpr_info.railStates[idx].disableVotes &= ~vote;
    CPR_LOG_TRACE(" DisableVotes: 0x%x (cleared voter:0x%x)", cpr_info.railStates[idx].disableVotes, vote);
    CPR_STATIC_LOG_RAIL_INFO(&cpr_info.railStates[idx]);
}

const cpr_margin_data* cpr_utils_get_margins(const uint8 railCPRRev, const cpr_margin_cfg* marginList)
{
    if(marginList != NULL)
    {
        for(int i = 0; i < marginList->count; i++)
        {
            if (railCPRRev >= marginList->data[i].cprRevMin && railCPRRev <= marginList->data[i].cprRevMax) {
                return &marginList->data[i];
            }
        }
    }

    return NULL;
}

void cpr_utils_get_mode_settings(cpr_domain_id railId, cpr_domain_info* info, cpr_mode_settings** mode, cpr_submode_settings** submode)
{
    cpr_rail* rail = cpr_utils_get_rail( railId );
    cpr_rail_state* railState = &cpr_info.railStates[rail->railIdx];

    if(info->type == CPR_DOMAIN_TYPE_MODE_BASED)
    {
        cpr_mode_settings* ms = &railState->modeSettings[cpr_utils_get_mode_idx(rail, info->u.mode)];
        if(mode) *mode = ms;
        if(submode) *submode = &ms->subModes[ms->subModesCount - 1];
    }
    else
    {
        for(int i = 0; i < railState->modeSettingsCount; i++)
        {
            cpr_mode_settings* ms = &railState->modeSettings[i];

            if(info->u.freq <= ms->subModes[ms->subModesCount-1].freq)
            {
                for (int k = 0; k < ms->subModesCount; k++)
                {
                    if(info->u.freq == ms->subModes[k].freq) {
                        if(mode) *mode = ms;
                        if(submode) *submode = &ms->subModes[k];
                        return;
                    }
                }
            }
        }

        CPR_LOG_FATAL("Virtual corner (freq %u) not supported on rail 0x%x", info->u.freq, railId);
    }
}

int16 cpr_utils_decode_fuse_value(const cpr_fuse* fuse, uint8 packingFactor, boolean isSigned)
{
    int bits_assigned = 0;
    uint32 cpr_fuse_bits = 0;

    CPR_ASSERT(fuse != NULL);

    if(!fuse->count)
    {
        return 0;
    }

    /*
     * NOTE:
     * First entry is MSB of raw fuse value.
     * Last entry is LSB (bit0) of raw fuse value.
     */
    for(int i = 0; i < fuse->count; i++)
    {
        uint32 raw_fuse_value = CPR_HWIO_IN( fuse->data[i].address );

        //This will give us the bits we care about from the fuse in the lowest bits of bits_we_care_about.
        const uint32 bits_we_care_about = (raw_fuse_value & fuse->data[i].mask) >> fuse->data[i].offset;

        //Now need to know how many bits of the fuse this accounts for. Could use __clz if only I knew what it did on ARM64/Q6?
        uint32 number_of_bits_we_care_about = 0;
        uint32 mask_for_counting = fuse->data[i].mask >> fuse->data[i].offset;

        while(mask_for_counting) {
            number_of_bits_we_care_about++;
            mask_for_counting = mask_for_counting >> 1;
        }

        //Move up the bits we've got previously to make way for the new bits we've parsed.
        cpr_fuse_bits = cpr_fuse_bits << number_of_bits_we_care_about;

        //Now pull in the new bits we just parsed.
        cpr_fuse_bits |= bits_we_care_about;

        //Record the total number of bits in the fuse.
        bits_assigned += number_of_bits_we_care_about;
    }

    //We have the CPR fuse as a raw bitfied in cpr_fuse_bits. Now need to convert to a signed int based on the
    //highest bit of the fuse. If the highest bit is set then the fuse value is -ve, otherwise +ve

    //Create a mask for the value part of the fuse. This mask is to pull out the bottom (rail_fuse_config->fuse_total_bits-1) bits.
    const uint32 value_mask = (1 << (isSigned ? bits_assigned - 1 : bits_assigned)) - 1;

    int16 value = (int16) ((cpr_fuse_bits & value_mask) * packingFactor);

    //Now check the top bit to see if we need to flip the sign.
    if(isSigned && (cpr_fuse_bits & (1 << (bits_assigned - 1)))) {
        value = -value;
    }

    return value;
}

void cpr_utils_set_active_mode_setting(cpr_domain_id railId, cpr_mode_settings* newModeSetting, cpr_submode_settings* newSubModeSetting)
{
    uint32 railIdx = cpr_utils_get_rail_idx( railId );

    /*
     * Store the current active mode to the previous mode
     * and update the active mode with the new mode
     */
    cpr_info.railStates[railIdx].previousMode = cpr_info.railStates[railIdx].activeMode;
    cpr_info.railStates[railIdx].activeMode   = newModeSetting;

    if(cpr_info.railStates[railIdx].activeMode)
    {
        /*
         * increment mode enablement counter for the new mode
         */
        cpr_info.railStates[railIdx].activeMode->enableCount++;

        /*
         * Set active submode if the new submode is not NULL
         */
        if(newSubModeSetting)
        {
            cpr_info.railStates[railIdx].activeMode->activeSubMode = newSubModeSetting;
        }
    }

    CPR_LOG_TRACE(" Active mode change: %u -> %u",
                  cpr_info.railStates[railIdx].previousMode ? cpr_info.railStates[railIdx].previousMode->mode : 0,
                  cpr_info.railStates[railIdx].activeMode   ? cpr_info.railStates[railIdx].activeMode->mode : 0);
    CPR_STATIC_LOG_RAIL_INFO(&cpr_info.railStates[railIdx]);
}

