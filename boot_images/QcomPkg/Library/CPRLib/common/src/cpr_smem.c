/**
 * @file:  cpr_smem.c
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr_smem.c#1 $
 * $Change: 14402555 $
 */
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_logs.h"
#include "cpr_utils.h"
#include "cpr_smem_internal.h"

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
    cpr_smem_rail_hdr* railHdr = (cpr_smem_rail_hdr*) GET_BUFFER();

    if(railHdr->id == rail->id)
    {
        INCREMENT_OFFSET(sizeof(cpr_smem_rail_hdr));

        state->id    = (cpr_domain_id)railHdr->id;
        state->cMode = (cpr_control_mode)railHdr->controlMode;

        for(int i = 0; i < CPR_MARGIN_ADJUSTMENT_MAX; i++)
        {
            state->marginAdjustments[i] = railHdr->marginAdjustments[i];
        }

        state->decodedFuseAging = railHdr->decodedFuseAging;
        state->modeSettingsCount = railHdr->modeStateCount;

        if(railHdr->activeVoltageMode)
        {
            state->activeMode =
                &state->modeSettings[cpr_utils_get_mode_idx(rail,
                        (cpr_voltage_mode)railHdr->activeVoltageMode )];
        }
        else
        {
            state->activeMode = NULL;
        }
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

/*============================================================================
 *
 * Writes target quotients and increments offset.
 *
 */
static void cpr_smem_write_target_quotients(cpr_smem_hdr* hdr,
        uint32 *currOffset, cpr_mode_settings* modeSetting)
{
    for(int i = 0; i < modeSetting->targetsCount; i++)
    {
        cpr_quotient* target = (cpr_quotient*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_quotient));

        target->ro       = modeSetting->targets[i].ro;
        target->quotient = modeSetting->targets[i].quotient;

        //cpr_image_memscpy(target, &(modeSetting->targets[i]),
        //                              sizeof(cpr_quotient));
    }
}

/*==============================================================================
 *
 * Reads target quotients and increments offset.
 *
 */
static void cpr_smem_read_target_quotients(cpr_smem_hdr* hdr,
        uint32 *currOffset, cpr_mode_settings* modeSetting)
{
    for(int i = 0; i < modeSetting->targetsCount; i++)
    {
        cpr_quotient* target = (cpr_quotient*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_quotient));

        modeSetting->targets[i].ro       = target->ro;
        modeSetting->targets[i].quotient = target->quotient;

        //cpr_image_memscpy(&(modeSetting->targets[i]), target,
        //                          sizeof(cpr_quotient));
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
    for(int i = 0; i < modeSetting->subModesCount; i++)
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
    for(int i = 0; i < railHdr->modeStateCount; i++)
    {
        cpr_mode_settings* modeSetting;
        cpr_smem_mode_state* modeState = (cpr_smem_mode_state*) GET_BUFFER();

        INCREMENT_OFFSET(sizeof(cpr_smem_mode_state));

        modeSetting = &(state->modeSettings[cpr_utils_get_mode_idx(rail,
                    (cpr_voltage_mode)modeState->mode )]);

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

    CPR_ASSERT( foundRail );
}

void deserialize_settings(cpr_rail* rail, cpr_settings* setting,
        void* buf, uint32 size)
{
    uint32 currOffset = 0;
    cpr_smem_hdr* hdr = cpr_smem_read_header(buf, &currOffset);
    boolean foundRail = false;
    cpr_rail_state state;

    for(int i = 0; i < hdr->railCount; i++)
    {
        cpr_smem_rail_hdr* railHdr =
            cpr_smem_read_rail_header(hdr, &currOffset, rail, &state);

        if(railHdr)
        {
            foundRail = true;

            CPR_ASSERT(railHdr->hasVPdata);

            cpr_smem_read_rail_settings(hdr, railHdr, &currOffset, setting);

            break;
        }
    }

    CPR_ASSERT( foundRail );
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
    uint32 size = 0;
    cpr_smem_hdr* hdr;

    cpr_image_open_remote_cfg( (void**)&hdr, &size );

    switch(hdr->version)
    {
        case CURRENT_SMEM_CFG_VERSION: deserialize_v2(rail, state, hdr, size);
            break;
        default:
            CPR_LOG_FATAL( "Unsupported serialization version: %u",
                    hdr->version );
    }

    cpr_image_close_remote_cfg();
}

/*==============================================================================
 *
 * Reads CPR settings for the rail in SMEM.
 *
 */
void cpr_smem_deserialize_settings(cpr_rail* rail, cpr_settings *setting)
{
    uint32 size = 0;
    cpr_smem_hdr* hdr;

    cpr_image_open_remote_cfg( (void**)&hdr, &size );

    switch(hdr->version)
    {
        case CURRENT_SMEM_CFG_VERSION: deserialize_settings(rail, setting, hdr, size);
            break;
        default:
            CPR_LOG_FATAL( "Unsupported serialization version: %u",
                    hdr->version );
    }

    cpr_image_close_remote_cfg();
}
/*===========================================================================
 *
 * Writes rail voltage plan data to SMEM in the below format.
 *
 *  -------------------------------------------------------------------------
 *   cpr_smem_hdr
 *  -------------------------------------------------------------------------
 *   cpr_smem_rail_hdr[]
 *  -------------------------------------------------------------------------
 *   cpr_settings                                   //cpr_settings_845
 *  -------------------------------------------------------------------------
 *      cpr_versioned_voltage_plan[]                //railVVPs
 *  -------------------------------------------------------------------------
 *          voltage_plan_list[]                     //list
 *  -------------------------------------------------------------------------
 *              cpr_version                         //version
 *  -------------------------------------------------------------------------
 *              cpr_voltage_plan                    //cfg
 *  -------------------------------------------------------------------------
 *                  cpr_voltage_mode[]              //supportedModes
 *  -------------------------------------------------------------------------
 *                  cpr_temp_adj_cfg                //tempAdjCfg
 *  -------------------------------------------------------------------------
 *                  cpr_kv_cfg                      //kvCfg
 *  -------------------------------------------------------------------------
 *                      cpr_kv[]                    //kvs
 *  -------------------------------------------------------------------------
 *                  cpr_voltage_data[]              //modes
 *  -------------------------------------------------------------------------
 *                      cpr_margin_cfg              //margins
 *  -------------------------------------------------------------------------
 *                          cpr_margin_data[]       //data
 *  -------------------------------------------------------------------------
 *                      cpr_fuse_data               //fuses
 *  -------------------------------------------------------------------------
 *                          cpr_fuse                //volt
 *  -------------------------------------------------------------------------
 *                              raw_fuse_data[]     //data
 *  -------------------------------------------------------------------------
 *                          cpr_fuse                //quot
 *  -------------------------------------------------------------------------
 *                              raw_fuse_data[]     //data
 *  -------------------------------------------------------------------------
 *                          cpr_fuse                //rosel
 *  -------------------------------------------------------------------------
 *                              raw_fuse_data[]     //data
 *  -------------------------------------------------------------------------
 *                          cpr_fuse                //quotOffset
 *  -------------------------------------------------------------------------
 *                              raw_fuse_data[]     //data
 *  -------------------------------------------------------------------------
 *                          cpr_fuse                //voltOffset
 *  -------------------------------------------------------------------------
 *                              raw_fuse_data[]     //data
 *  -------------------------------------------------------------------------
 *                      cpr_quotient_cfg            //quotients
 *  -------------------------------------------------------------------------
 *                          cpr_quotient[]          //quots
 *  -------------------------------------------------------------------------
 *                      cpr_freq_data[]             //subModes
 *  -------------------------------------------------------------------------
 *          cpr_aging_cfg
 *  -------------------------------------------------------------------------
 *      cpr_rail[]                                  //railVVPs
 *  -------------------------------------------------------------------------
 *          cpr_voltage_mode[]                      //settleModes
 *  -------------------------------------------------------------------------
 *          cpr_voltage_plan                        //vp         
 *  -------------------------------------------------------------------------
 *          cpr_enablement                          //enablement 
 *  -------------------------------------------------------------------------
 *              cpr_version                         //version
 *  -------------------------------------------------------------------------
 *      cpr_hal_controller[]
 *  -------------------------------------------------------------------------
 *          cpr_hal_temp_data                       //tempSensorData
 *  -------------------------------------------------------------------------
 *          cpr_hal_controller_cfg                  //cfg
 *  -------------------------------------------------------------------------
 *              bypassSensors[]                     //bypassSensors
 *  -------------------------------------------------------------------------
 *              disableSensors[]                    //disableSensors
 *  -------------------------------------------------------------------------
 *          cpr_hal_thread                          //threads
 *  -------------------------------------------------------------------------
 *              cpr_hal_thread_cfg                  //cfg
 *  -------------------------------------------------------------------------
 *                  sensors                         //sensors
 *  -------------------------------------------------------------------------
 *      cpr_enablement[]
 *  -------------------------------------------------------------------------
 *              cpr_version                         //version
 *  -------------------------------------------------------------------------
 *      cpr_misc_cfg
 *  -------------------------------------------------------------------------
 *          cpr_fuse                                //cprRev
 *  -------------------------------------------------------------------------
 *              raw_fuse_data[]                     //data
 *  -------------------------------------------------------------------------
 *                   
 */
void cpr_smem_serialize_settings(cpr_rail_state* state, boolean append)
{
    uint32 currOffset = 0;
    uint32 size = 0;
    void* buf;
    cpr_smem_hdr* hdr;
    cpr_rail *rail = cpr_utils_get_rail(state->id);

    CPR_ASSERT(rail);

    cpr_image_open_remote_cfg( &buf, &size );

    if(!append)
    {
        hdr = cpr_smem_write_header(buf, size, &currOffset);
    }
    else
    {
        hdr = (cpr_smem_hdr*)buf;

        currOffset = hdr->dataSize;
    }

    CPR_LOG_INFO(" Exporting %s rail voltage plan (offset: 0x%x)", rail->name,
            currOffset);

    cpr_smem_rail_hdr* railHdr = cpr_smem_write_rail_header(hdr,
            &currOffset, state);

    railHdr->hasVPdata = 1;

    cpr_smem_write_rail_settings(hdr, railHdr, &currOffset, state);
}

/*==============================================================================
 *
 * Writes CPR config for the rail to SMEM in the below format.
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
void cpr_smem_serialize_config(cpr_rail_state* state, boolean append)
{
    uint32 currOffset = 0;
    uint32 size = 0;
    void* buf;
    cpr_smem_hdr* hdr;
    cpr_rail *rail = cpr_utils_get_rail(state->id);

    CPR_ASSERT(rail);

    cpr_image_open_remote_cfg( &buf, &size );

    if(!append)
    {
        hdr = cpr_smem_write_header(buf, size, &currOffset);
    }
    else
    {
        hdr = (cpr_smem_hdr*)buf;

        currOffset = hdr->dataSize;
    }

    CPR_LOG_INFO(" Exporting %s rail state (offset: 0x%x)", rail->name,
            currOffset);

    cpr_smem_rail_hdr* railHdr = cpr_smem_write_rail_header(hdr,
            &currOffset, state);

    cpr_smem_write_rail_state(hdr, railHdr, &currOffset, state);

    cpr_image_close_remote_cfg();
}
