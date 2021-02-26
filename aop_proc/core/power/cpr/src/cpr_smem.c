/**
 * @file:  cpr_smem.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/src/cpr_smem.c#1 $
 * $Change: 15485400 $
 */

/*
 *
 * CPR config for the rail in SMEM is in the below format.
 *
 *  -----------------------------------------------------
 *   cpr_smem_hdr
 *  -----------------------------------------------------
 *   cpr_smem_rail_hdr[0]
 *  -----------------------------------------------------
 *       cpr_smem_mode_state[0]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *      -------------------------------------------------
 *       ...
 *      -------------------------------------------------
 *       cpr_smem_mode_state[modeSettingsCount-1]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *  -----------------------------------------------------
 *   ...
 *  -----------------------------------------------------
 *   cpr_smem_rail_hdr[cpr_smem_hdr.railCount-1]
 *  -----------------------------------------------------
 *      cpr_smem_mode_state[0]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *      -------------------------------------------------
 *       ...
 *      -------------------------------------------------
 *       cpr_smem_mode_state[modeSettingsCount-1]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *  -----------------------------------------------------
 */

#include "page_select.h"
#include "cpr_smem.h"
#include "cpr_types.h"
#include "cpr_image.h"
#include "msg_ram_layout.h"
#include <string.h>
#include <stdlib.h>

#define CPR_SMEM_SIZE 0x1000
#define CPR_SMEM_PHY_ADDR 0x860031c8

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define CURRENT_SMEM_CFG_VERSION 2

#define INCREMENT_OFFSET(size)                                                    \
do{                                                                               \
    *currOffset += (size);                                                        \
    if(*currOffset >= hdr->smemSize)                                              \
        CPR_LOG_FATAL("Exceeded memory (0x%x/0x%x)", *currOffset, hdr->smemSize); \
}while(0)

#define GET_BUFFER()    (((uint8*)hdr) + *currOffset)

//******************************************************************************
// Local Helper Functions
//******************************************************************************

/*==============================================================================
 * Reads SMEM header and increments offset.
 *
 */
static cpr_smem_hdr* cpr_smem_read_header(void* buf, uint32 *currOffset)
{
    cpr_smem_hdr* hdr = (cpr_smem_hdr*)buf;

    INCREMENT_OFFSET(sizeof(cpr_smem_hdr));

    return hdr;
}

/*==============================================================================
 * Reads rail header and increments offset.
 * If a rail config is not for the given rail, sets offset to the next rail config and
 * returns NULL.
 *
 */
static cpr_smem_rail_hdr* cpr_smem_read_rail_header(cpr_smem_hdr* hdr,
        uint32 *currOffset, cpr_rail* rail, cpr_rail_state* state)
{
    int i = 0;
    cpr_smem_rail_hdr* railHdr = (cpr_smem_rail_hdr*) GET_BUFFER();

    if(railHdr->id == rail->id)
    {
        INCREMENT_OFFSET(sizeof(cpr_smem_rail_hdr));

        state->id    = (cpr_domain_id)railHdr->id;
        state->cMode = (cpr_control_mode)railHdr->controlMode;

        for(i = 0; i < CPR_MARGIN_ADJUSTMENT_MAX; i++)
        {
            state->marginAdjustments[i] = railHdr->marginAdjustments[i];
        }

        state->decodedFuseAging = railHdr->decodedFuseAging;
        state->mode = (cpr_voltage_mode) railHdr->activeVoltageMode;
        state->modeSettingsCount = railHdr->modeStateCount;
    }
    else
    {
        /*
         * Set offset to the next rail.
         */
        *currOffset += railHdr->size;

        /*
         * Return NULL to indicate the rail config was not for
         * the given rail.
         */
        railHdr = NULL;
    }

    return railHdr;
}

/*
 *
 * Reads target quotients and increments offset.
 *
 */
void cpr_smem_read_target_quotients(cpr_smem_hdr* hdr,
        uint32 *currOffset, cpr_mode_settings* modeSetting)
{
    int i = 0;
    for(i = 0; i < modeSetting->targetsCount; i++)
    {
        cpr_quotient* target = (cpr_quotient*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_quotient));

        cpr_image_memscpy(&(modeSetting->targets[i]), target,
                sizeof(cpr_quotient));
    }
}


/*==============================================================================
 *
 * Reads submode settings and increments offset.
 *
 */
static void cpr_smem_read_sub_modes(cpr_smem_hdr* hdr, uint32 *currOffset,
        cpr_mode_settings* modeSetting)
{
    int i;
    for(i = 0; i < modeSetting->subModesCount; i++)
    {
        cpr_submode_settings *submode = (cpr_submode_settings*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_submode_settings));

        cpr_image_memscpy(&(modeSetting->subModes[i]), submode,
                sizeof(cpr_submode_settings));
    }
}

/*==============================================================================
 * Reads rail mode settings and increments offset.
 *
 */
static void cpr_smem_read_rail_state(cpr_smem_hdr* hdr,
        cpr_smem_rail_hdr* railHdr, uint32 *currOffset,
        cpr_rail* rail, cpr_rail_state* state)
{
    int i;
    if (state->modeSettings == NULL)
        state->modeSettings = cpr_image_malloc(railHdr->modeStateCount *
            sizeof(cpr_mode_settings));

    for(i = 0; i < railHdr->modeStateCount; i++)
    {
        cpr_mode_settings* modeSetting;
        cpr_smem_mode_state* modeState =
            (cpr_smem_mode_state*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_smem_mode_state));

        modeSetting = &(state->modeSettings[i]);

        modeSetting->mode              = (cpr_voltage_mode)modeState->mode;
        modeSetting->decodedFuseSteps  = modeState->decodedFuseSteps;
        modeSetting->decodedFuseOffset = modeState->decodedFuseOffset;
        modeSetting->enableCount       = modeState->enableCount;
        modeSetting->totalMarginMV     = modeState->totalMarginMV;

        if(!modeSetting->targets)
        {
            if(modeState->targetsCount)
            {
                modeSetting->targets = cpr_image_malloc(
                        modeState->targetsCount * sizeof(cpr_quotient) );
            }
        }
        else
        {
            /*
             * Do we need to realloc instead of abort?
             */
            CPR_ASSERT( modeSetting->targetsCount >= modeState->targetsCount );

        }

        modeSetting->targetsCount = modeState->targetsCount;

        if(!modeSetting->subModes)
        {
            if(modeState->submodesCount)
            {
                modeSetting->subModes = cpr_image_malloc(
                        modeState->submodesCount *
                        sizeof(cpr_submode_settings));
            }
        }
        else
        {
            /*
             * Do we need to realloc instead of abort?
             */
            CPR_ASSERT(modeSetting->subModesCount >=
                    modeState->submodesCount);
        }

        modeSetting->subModesCount = modeState->submodesCount;

        cpr_smem_read_target_quotients(hdr, currOffset, modeSetting);
        cpr_smem_read_sub_modes(hdr, currOffset, modeSetting);
    }
}

/*==============================================================================
 *
 * Reads CPR config for the rail in SMEM in the below format and updates config data.
 *
 *  -----------------------------------------------------
 *   cpr_smem_hdr
 *  -----------------------------------------------------
 *   cpr_smem_rail_hdr[0]
 *  -----------------------------------------------------
 *       cpr_smem_mode_state[0]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *      -------------------------------------------------
 *       ...
 *      -------------------------------------------------
 *       cpr_smem_mode_state[modeSettingsCount-1]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *  -----------------------------------------------------
 *   ...
 *  -----------------------------------------------------
 *   cpr_smem_rail_hdr[cpr_smem_hdr.railCount-1]
 *  -----------------------------------------------------
 *       cpr_smem_mode_state[0]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *      -------------------------------------------------
 *       ...
 *      -------------------------------------------------
 *       cpr_smem_mode_state[modeSettingsCount-1]
 *      -------------------------------------------------
 *           cpr_quotient[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_quotient[targetsCount-1]
 *          ---------------------------------------------
 *           cpr_submode_settings[0]
 *          ---------------------------------------------
 *           ...
 *          ---------------------------------------------
 *           cpr_submode_settings[subModesCount-1]
 *  -----------------------------------------------------
 */
static void deserialize_v2(cpr_rail* rail, cpr_rail_state* state,
        void* buf, uint32 size)
{
    uint32 currOffset = 0;
    cpr_smem_hdr* hdr = cpr_smem_read_header(buf, &currOffset);
    boolean foundRail = false;

    for(int i = 0; i < hdr->railCount; i++)
    {
        cpr_smem_rail_hdr* railHdr =
            cpr_smem_read_rail_header(hdr, &currOffset, rail, state);

        if(railHdr)
        {
            foundRail = true;

            CPR_ASSERT(!(railHdr->hasVPdata));

            cpr_smem_read_rail_state(hdr, railHdr, &currOffset, rail, state);

            break;
        }
    }

    //ToDo: Re-enable this check.
    //CPR_ASSERT( foundRail );
}

//******************************************************************************
// Public API Functions
//******************************************************************************

/*==============================================================================
 *
 * Reads CPR config for the rail in SMEM and updates config data.
 *
 */
void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state)
{
    uint32 size;
    uint32 addr;
    cpr_smem_hdr* hdr = NULL;
    smem_table_err error;
  
    error = msg_ram_get_smem_addr(CPR_SMEM_CFG, &addr);
    
    if ( error != SMEM_TABLE_SUCCESS )
    {
      addr = cpr_image_get_smem_addr(CPR_SMEM_CFG);
    }

    if (!addr)
        return;

    cpr_image_open_remote_cfg( (void**)&hdr, addr );
    if (!hdr) {
        cpr_image_close_remote_cfg();
        return;
    }

    /* Don't abort to support smem.bin without CPR config for
     * pre-silicon env
     */
    //CPR_ASSERT(hdr);

    switch(hdr->version)
    {
        case 0: /* Don't abort to support smem.bin without CPR config for
                   pre-silicon env */
            break;
        case CURRENT_SMEM_CFG_VERSION:
            deserialize_v2( rail, state, hdr, size );
            break;
        default:
            CPR_LOG_FATAL("Unsupported serialization version: %u",
                    hdr->version);
    }

    cpr_image_close_remote_cfg();
}
