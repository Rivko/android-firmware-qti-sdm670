/**
 * @file:  cpr_smem.c
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/src/cpr_smem.c#1 $
 * $Change: 16759227 $
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
 *   cpr_smem_aging_cfg (Writen and Read for MSS and GFX Only)
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
 *   cpr_smem_aging_cfg (Writen and Read for MSS and GFX Only)
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

#include "smem.h"
#include "cpr_smem.h"
#include "cpr_types.h"
#include "cpr_image.h"
#include <string.h>
#include <stdlib.h>

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
// V2 Defines
//******************************************************************************
typedef struct __attribute__ ((__packed__))
{
    uint32             modeToRun;
    uint8              sensorID;
    uint8              bypassSensorIDsCount;
    uint8              fusePackingFactor;
    uint32             kv;
    uint32             scalingFactor;
    int32              marginLimit;
    uint8              bypassSensorMap[32];
} cpr_smem_aging_cfg;

typedef struct __attribute__ ((__packed__))
{
    uint32 mode;  // cpr_voltage_mode
    int16  decodedFuseSteps;
    int16  decodedFuseOffset;
    uint16 enableCount;
    uint16 targetsCount;
    uint16 submodesCount;
    int16  totalMarginMV;
} cpr_smem_mode_state;

typedef struct __attribute__ ((__packed__))
{
    uint16  size;              // in bytes
    uint32  id;                // cpr_rail_id
    uint32  controlMode;       // cpr_control_mode
    int32   marginAdjustments[CPR_MARGIN_ADJUSTMENT_MAX];
    int16   decodedFuseAging;
    int8    hasAgingInfo;      // Flag to indicate additional Aging info
    uint32  activeVoltageMode; // cpr_voltage_mode
    uint8   modeStateCount;
} cpr_smem_rail_hdr;

typedef struct __attribute__ ((__packed__))
{
    uint8   version;
    uint8   railCount;
    uint16  dataSize;       // Data size written to SMEM to get offset.
    uint16  smemSize;       // SMEM buffer size
} cpr_smem_hdr;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

/*==============================================================================
 *
 * Writes SMEM header and increments offset.
 *
 */
static cpr_smem_hdr* cpr_smem_write_header(void* buf, uint32 bufSize, uint32 *currOffset)
{
    cpr_smem_hdr* hdr = (cpr_smem_hdr*)buf;

    hdr->version   = CURRENT_SMEM_CFG_VERSION;
    hdr->railCount = 0;
    hdr->smemSize  = bufSize;
    hdr->dataSize  = 0;

    INCREMENT_OFFSET(sizeof(cpr_smem_hdr));

    return hdr;
}

/*==============================================================================
 *
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
 *
 * Writes rail header and increments offset.
 *
 */
static cpr_smem_rail_hdr* cpr_smem_write_rail_header(cpr_smem_hdr* hdr, uint32 *currOffset, cpr_rail_state* state)
{
    cpr_smem_rail_hdr* railHdr = (cpr_smem_rail_hdr*) GET_BUFFER();

    hdr->railCount++;

    INCREMENT_OFFSET(sizeof(cpr_smem_rail_hdr));

    railHdr->id          = state->id;
    railHdr->controlMode = state->cMode;

    for(int i = 0; i < CPR_MARGIN_ADJUSTMENT_MAX; i++)
    {
        railHdr->marginAdjustments[i] = state->marginAdjustments[i];
    }

    railHdr->decodedFuseAging = state->decodedFuseAging;
    railHdr->hasAgingInfo = 0;

    if(state->activeMode)
    {
        railHdr->activeVoltageMode = state->activeMode->mode;
    }
    else
    {
        railHdr->activeVoltageMode = 0;
    }

    railHdr->modeStateCount = 0;

    return railHdr;
}

/*==============================================================================
 *
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
         * Return NULL to indicate the rail config was not for the given
         * rail.
         */
        railHdr = NULL;
    }

    return railHdr;
}

/*==============================================================================
 *
 * Writes target quotients and increments offset.
 *
 */
static void cpr_smem_write_target_quotients(cpr_smem_hdr* hdr, uint32 *currOffset, cpr_mode_settings* modeSetting)
{
    int i = 0;
    for(i = 0; i < modeSetting->targetsCount; i++)
    {
        cpr_quotient* target = (cpr_quotient*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_quotient));

        cpr_image_memscpy(target, &(modeSetting->targets[i]), sizeof(cpr_quotient));
    }
}

/*
 *
 * Reads target quotients and increments offset.
 *
 */
static void cpr_smem_read_target_quotients(cpr_smem_hdr* hdr,
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
 * Reads aging config and increments offset.
 *
 */
static void cpr_smem_read_rail_aging(cpr_smem_hdr* hdr, uint32 *currOffset,
        cpr_rail* rail, cpr_rail_state* state, cpr_aging_cfg* agingCfg)
{
    int i = 0, j = 0, k = 0;

    cpr_smem_aging_cfg* smemAgingCfg = (cpr_smem_aging_cfg*) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_aging_cfg));

    agingCfg->modeToRun            = smemAgingCfg->modeToRun;
    agingCfg->sensorID             = smemAgingCfg->sensorID;
    agingCfg->bypassSensorIDsCount = smemAgingCfg->bypassSensorIDsCount;
    agingCfg->fusePackingFactor    = smemAgingCfg->fusePackingFactor;
    agingCfg->kv                   = smemAgingCfg->kv;
    agingCfg->scalingFactor        = smemAgingCfg->scalingFactor;
    agingCfg->marginLimit          = smemAgingCfg->marginLimit;

    if (!agingCfg->bypassSensorIDsCount)
        return;

    agingCfg->bypassSensorIDs      =
        cpr_image_malloc(agingCfg->bypassSensorIDsCount * sizeof(uint8));

    for (i = 0; i < 32; i++) {
        if (!smemAgingCfg->bypassSensorMap[i])
            continue;
        for (j = 0; j < 8; j++) {
            if (smemAgingCfg->bypassSensorMap[i] & (1 << j))
                agingCfg->bypassSensorIDs[k++] = i * 8 + j;
        }
    }
}

/*==============================================================================
 *
 * Writes submode settings and increments offset.
 *
 */
static void cpr_smem_write_sub_modes(cpr_smem_hdr* hdr, uint32 *currOffset,
        cpr_mode_settings* modeSetting)
{
    for(int i = 0; i < modeSetting->subModesCount; i++)
    {
        cpr_submode_settings *submode = (cpr_submode_settings*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_submode_settings));

        cpr_image_memscpy(submode, &(modeSetting->subModes[i]),
                sizeof(cpr_submode_settings));
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
 *
 * Writes rail mode settings and increments offset.
 * Stores the byte size of the rail config in the rail header in order to search
 * rail config easily.
 * Stores the byte size of the entire data written to the SMEM in order to get
 * offset for the next rail config.
 *
 */
static void cpr_smem_write_rail_state(cpr_smem_hdr* hdr,
        cpr_smem_rail_hdr* railHdr, uint32 *currOffset,
        cpr_rail_state* state)
{
    for(int i = 0; i < state->modeSettingsCount; i++)
    {
        cpr_mode_settings* modeSetting = &(state->modeSettings[i]);
        cpr_smem_mode_state* modeState = (cpr_smem_mode_state*) GET_BUFFER();

        railHdr->modeStateCount++;

        INCREMENT_OFFSET(sizeof(cpr_smem_mode_state));

        modeState->mode              = modeSetting->mode;
        modeState->decodedFuseSteps  = modeSetting->decodedFuseSteps;
        modeState->decodedFuseOffset = modeSetting->decodedFuseOffset;
        modeState->enableCount       = modeSetting->enableCount;
        modeState->targetsCount      = modeSetting->targetsCount;
        modeState->submodesCount     = modeSetting->subModesCount;
        modeState->totalMarginMV     = modeSetting->totalMarginMV;

        cpr_smem_write_target_quotients(hdr, currOffset, modeSetting);
        cpr_smem_write_sub_modes(hdr, currOffset, modeSetting);
    }

    railHdr->size = (uint16)((uint8*)hdr + *currOffset - (uint8*)railHdr);
    hdr->dataSize = *currOffset;
}

/*==============================================================================
 *
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
                    modeState->targetsCount * sizeof(cpr_quotient));
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
 *   cpr_smem_aging_cfg (Writen and Read for MSS and GFX Only)
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
 *   cpr_smem_aging_cfg (Writen and Read for MSS and GFX Only)
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

void deserialize_v3(cpr_rail* rail, cpr_rail_state* state,
        cpr_aging_cfg *agingCfg, void* buf, uint32 size)
{
    uint32 currOffset = 0;
    cpr_smem_hdr* hdr = cpr_smem_read_header(buf, &currOffset);
    boolean foundRail = false;
    int i = 0;

    for(i = 0; i < hdr->railCount; i++)
    {
        cpr_smem_rail_hdr* railHdr =
            cpr_smem_read_rail_header(hdr, &currOffset, rail, state);

        if(railHdr)
        {
            foundRail = true;

            if (railHdr->hasAgingInfo)
                cpr_smem_read_rail_aging(hdr, &currOffset, rail, state,
                        agingCfg);

            cpr_smem_read_rail_state(hdr, railHdr, &currOffset, rail, state);

            break;
        }
    }

    CPR_ASSERT( foundRail );
}

//******************************************************************************
// Public API Functions
//******************************************************************************

/*
 *
 * Reads CPR config for the rail in SMEM and update config data.
 *
 */
void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state,
        cpr_aging_cfg *agingCfg)
{
    uint32 size;
    cpr_smem_hdr* hdr = NULL;

    cpr_image_open_remote_cfg( (void**)&hdr, &size );
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
            deserialize_v3( rail, state, agingCfg, hdr, size );
            break;
        default:
            CPR_LOG_FATAL("Unsupported serialization version: %u",
                    hdr->version);
    }

    cpr_image_close_remote_cfg();
}
