/**
 * @file:  cpr_rail.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/12/19 02:59:36 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr_rail.c#3 $
 * $Change: 15088264 $
 */
#include <stdbool.h>
#include <string.h>
#include "cpr_rail.h"
#include "cpr_hal.h"
#include "cpr_data.h"
#include "cpr_utils.h"
#include "cpr_image.h"
#include "cpr_logs.h"
#include "cpr_cfg.h"
#include "cpr.h"
#include "cpr_voltage_plan.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define QUOT_OFFSET_MULTIPLIER 5

typedef struct
{
    boolean turboOnly;
    boolean forceDisable;
    boolean updateOpenLoop;
    boolean updateClosedLoop;
    int     svs2_step;
    int     svs_step;
} disablement_params_t;

//******************************************************************************
// Global Data
//******************************************************************************
static cpr_quotient *new_target_quotients; // used to calculate new target quotients. NOTE: it's shared among all rails
static uint16 max_target_count;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static inline uint32 quantize_up(uint32 quantum, uint32 value)
{
    uint32 remainder = value % quantum;

    if(remainder != 0) {
        value += quantum - remainder;
    }

    return value;
}

static boolean matches_device_version(const cpr_version** vers, uint32 count)
{
    for(int i = 0; i < count; i++)
    {
        const cpr_version* ver = vers[i];

        if((ver->foundry == cpr_info.foundry || cpr_info.foundry == CPR_FOUNDRY_ANY || ver->foundry == CPR_FOUNDRY_ANY) &&
           cpr_info.chipVersion >= ver->min &&
           cpr_info.chipVersion <= ver->max)
        {
            return true;
        }

    }

    return false;
}

/*
 * Derives target quotient for finer grain frequency point in order to optimize power/performance.
 *
 * Reference: HPG - Target Quotient Scaling
 */
static void interpolate_quotients(cpr_rail* rail, cpr_mode_settings* modeState, cpr_submode_settings* submodeState,
                                  cpr_quotient* tgts, boolean cmprToExisting)
{
    uint32 quotScaler_x1000000 = 0;
    uint32 mode_idx = cpr_utils_get_mode_idx(rail, modeState->mode);
    uint32 fdelta = rail->vp->modes[mode_idx].freqDelta;

    /*
     * fuses pointer could be NULL if Voltage Plan does not define yet
     */
    if(!fdelta && (rail->vp->modes[mode_idx].fuses != NULL) && (rail->vp->modes[mode_idx].fuses->quotOffset != NULL))
    {
        uint32 quotOffset = cpr_utils_decode_fuse_value( rail->vp->modes[mode_idx].fuses->quotOffset, 1, false ) * QUOT_OFFSET_MULTIPLIER;
        quotScaler_x1000000 = (quotOffset*1000000) / fdelta;
    }

    CPR_LOG_TRACE( " Interpolate quots. scaler: %u, fmax freq: %u, freq: %u",
                   quotScaler_x1000000, modeState->subModes[modeState->subModesCount - 1].freq, submodeState->freq );

    for(int i = 0; i < modeState->targetsCount; i++)
    {
        /*
         * QUOT(new frequency) = QUOT(fuse) - (Operation mode top frequency - new frequency in MHz)*scaling_factor
         */
        uint32 q = modeState->targets[i].quotient - (quotScaler_x1000000 * (modeState->subModes[modeState->subModesCount - 1].freq - submodeState->freq)) / 1000000;

        tgts[i].ro = modeState->targets[i].ro;
        tgts[i].quotient = cmprToExisting ? MAX( q, tgts[i].quotient ) : q;
    }
}

//******************************************************************************
// ISR Handler
//******************************************************************************

void cpr_rail_isr(void* ctx)
{
    cpr_results rslts;
    cpr_rail* rail = (cpr_rail*) ctx;

    CPR_LOG_TRACE( "=== ISR (rail: %s) ===", rail->name );

    CPR_IMAGE_LOCK_CORE();

    cpr_info.processingIsr = true;
    cpr_utils_set_disable_vote(rail->id, CPR_DISABLE_VOTE_ISR);

    if(cpr_info.railStates[rail->railIdx].activeMode != NULL)
    {
        CPR_ASSERT( cpr_hal_get_results( &rail->hal, &rslts, RESULTS_TIMEOUT_20MS ) );
        cpr_rail_process_results( rail, &rslts );
        cpr_hal_clear_interrupts( &rail->hal );
    }

    cpr_utils_clear_disable_vote(rail->id, CPR_DISABLE_VOTE_ISR);
    cpr_info.processingIsr = false;

    CPR_IMAGE_UNLOCK_CORE();
}

//******************************************************************************
// Public API Functions
//******************************************************************************

static void cpr_rail_init_rail_config(cpr_rail* rail)
{
    rail->railIdx = cpr_utils_get_rail_idx( rail->id );

    const cpr_versioned_voltage_plan* vvp = cpr_utils_get_versioned_voltage_plan( rail->id );

    for(int i = 0; i < vvp->count; i++)
    {
        if(matches_device_version( (const cpr_version**)&vvp->list[i].version, 1 )) {
            rail->vp = vvp->list[i].cfg;
            CPR_LOG_INFO( "Found voltage plan for rail %s: (idx=%d)", rail->name, i );
            break;
        }
    }

    CPR_ASSERT(rail->vp);

    for(int i = 0; i < cpr_info.settings->enablementsCount && rail->enablement == NULL; i++)
    {
        cpr_enablement* e = cpr_info.settings->enablements[i];
        if(e->id == rail->id)
        {
            if(matches_device_version( (const cpr_version**)&(e->version), 1 )) {
                rail->enablement = e;
                CPR_LOG_INFO( "Found enablement config for rail %s: (idx=%d)", rail->name, i );
                break;
            }
        }
    }

    CPR_ASSERT(rail->enablement);
}

/*
 * Initialize config/data pointers and set all modes with global ceiling/floor voltages.
 */
static void cpr_rail_init_rail_data(cpr_rail* rail)
{
    cpr_rail_state         *railState = &cpr_info.railStates[rail->railIdx];
    const cpr_voltage_plan *vp        = rail->vp;
    cpr_enablement         *e         = rail->enablement;

    railState->id                = rail->id;
    railState->modeSettingsCount = vp->modesCount;
    railState->modeSettings      = cpr_image_malloc(railState->modeSettingsCount * sizeof(cpr_mode_settings));
    railState->railCPRRev        = cpr_utils_decode_fuse_value(&(e->railCPRRevFuse), 1, false);

    /*
     * Set to global ceiling/floor first in order to make it easy to run aging algorithm.
     * Open loop voltages are set up later after aging algorihtm in cpr_rail_set_initial_voltages().
     */
    for(int modeIdx = 0; modeIdx < railState->modeSettingsCount; modeIdx++)
    {
        cpr_mode_settings      *modeSettings = &railState->modeSettings[modeIdx];
        const cpr_voltage_data *vpMode       = &(vp->modes[modeIdx]);

        modeSettings->mode = vp->supportedModes[modeIdx];

        /*
         * Allocate memory for target quotient info
         * NOTE: target quotient values are set up in cpr_rail_calculate_target_quotients()
         */
        if(vpMode->fuses != NULL && vpMode->fuses->quot != NULL)
        {
            modeSettings->targetsCount = 1; // for local quots
        }
        else if(vpMode->quotients)
        {
            modeSettings->targetsCount = vpMode->quotients->count;
        }

        /*
         * Read the TargetVolt fuses here to store the value for all rails before rail specific init.
         * since TargetVolt fuse values are special and used to implement HW workaround.
         * MX/SSC MX rails do not have TargetVolt fuse for SVS2, and they need to use CX's/SSC CX 's fuse.
         */
        if(vpMode->fuses != NULL && vpMode->fuses->volt != NULL)
        {
            modeSettings->decodedFuseSteps = cpr_utils_decode_fuse_value( vpMode->fuses->volt, 1, true );
        }

        if(max_target_count < modeSettings->targetsCount)
        {
            max_target_count = modeSettings->targetsCount;
        }

        if(modeSettings->targetsCount)
        {
            modeSettings->targets   = cpr_image_malloc(modeSettings->targetsCount * sizeof(cpr_quotient));
        }

        modeSettings->subModesCount = vpMode->subModesCount;
        modeSettings->subModes      = cpr_image_malloc(modeSettings->subModesCount * sizeof(cpr_submode_settings));

        for(int submodeIdx = 0; submodeIdx < modeSettings->subModesCount; submodeIdx++)
        {
            cpr_submode_settings *submode  = &modeSettings->subModes[submodeIdx];
            const cpr_freq_data *vpSubMode = &(vpMode->subModes[submodeIdx]);

            submode->freq    = vpSubMode->freq;
            submode->ceiling = vpSubMode->ceiling;
            submode->current = vpSubMode->ceiling;
            submode->min     = vpSubMode->ceiling;
            submode->floor   = vpSubMode->floor;

            /*
             * Set active submode to the last one to avoid NULL pointer dereference
             * during cpr_measurements_settle()
             */
            modeSettings->activeSubMode = submode;
        }
    }
}

void cpr_rail_init(cpr_rail* rail)
{
    /*
     * Find Voltage Plan and Enablement config for the rail
     */
    cpr_rail_init_rail_config(rail);

    /*
     * Initialize rail data with global ceiling/floor first
     * based on the Voltage Plan config because
     * Aging algorithm has to run with global ceiling.
     * Open loop voltage is set up in cpr_rail_set_initial_voltages()
     * after running aging algorithm.
     */
    cpr_rail_init_rail_data(rail);
}

void cpr_rail_init_rail_hw(cpr_rail* rail, boolean configureHW)
{
    /*
     * Initialize CPR Controller for the rail
     */
    cpr_hal_init_controller( &rail->hal, configureHW );

    /*
     * Set the SDELTA table up for the specific rail. Assume we are at turbo mode to start.
     */
    // Temperature adjustment feature init for CPR4.
    cpr_hal_init_temp_margin_adj(&rail->hal, rail->vp->modesCount-1, rail->vp);
}

boolean cpr_rail_process_results(cpr_rail* rail, cpr_results* rslts)
{
    boolean changed = false;
    boolean ackResult = false;
    uint32 railIdx = rail->railIdx;
    cpr_mode_settings* currentMode = cpr_info.railStates[railIdx].activeMode;
    cpr_submode_settings* currentSubMode = currentMode->activeSubMode;
    uint32 recommendation = currentSubMode->current;

    /*
     * NOTE:
     * Currently recommended by HW team to limit step size to 1
     * regardless of actual value
     */
    uint32 steps = 1; // rslts->steps

    // It's possible that both the up and dn interrupts are set.
    // In that case, the recommendation from the HW team is that we step-up the voltage.
    // This could happen if a BHS is toggled after the first interrupt triggers.
    if(rslts->up)
    {
        /*
         * Re-enable DOWN interrupt since there is room to take DOWN recommendation.
         */
        cpr_hal_enable_down_interrupt(&rail->hal, true);

        recommendation += steps * rail->enablement->stepSize;
        currentSubMode->debug.up += steps;
    }
    else if(rslts->down)
    {
        /*
         * Re-enable UP interrupt since there is room to take UP recommendation.
         */
        cpr_hal_enable_up_interrupt(&rail->hal, true);

        recommendation -= steps * rail->enablement->stepSize;
        currentSubMode->debug.down += steps;
    }

    CPR_LOG_TRACE( " New voltage recommendation for mode %u: %u (floor:%u) (ceiling:%u)",
                   currentMode->mode,
                   recommendation,
                   currentSubMode->floor,
                   currentSubMode->ceiling);

    // Clamp the recommendation
    if(recommendation >= currentSubMode->ceiling)
    {
        /*
         * Disable UP interrupt since hitting ceiling voltage and no room to take UP recommendation.
         */
        cpr_hal_enable_up_interrupt(&rail->hal, false);

        recommendation = currentSubMode->ceiling;
        currentSubMode->debug.ceiling++;
    }
    else if(recommendation <= currentSubMode->floor)
    {
        /*
         * Disable DOWN interrupt since hitting floor voltage and no room to take DOWN recommendation.
         */
        cpr_hal_enable_down_interrupt(&rail->hal, false);

        recommendation = currentSubMode->floor;
        currentSubMode->debug.floor++;
    }

    if (recommendation != currentSubMode->current)
    {
        currentSubMode->current = recommendation;
        currentSubMode->min = MIN( recommendation, currentSubMode->min );

        /*
         * Transition voltage
         */
        if(cpr_image_rail_transition_voltage( rail->id ))
        {
            changed = true;

            // Don't ack the result if we switched modes.
            if(currentMode == cpr_info.railStates[railIdx].activeMode)
            {
                ackResult = true;
            }
        }
        else
        {
            /*
             * Failed to transition voltage. Increment ignored counter.
             * Don't ack the result since no voltage change.
             */
            currentSubMode->debug.ignored++;
        }
    }
    else
    {
        currentSubMode->debug.ignored++;
    }

    CPR_LOG_TRACE( " Results ack: %u", ackResult );
    CPR_STATIC_LOG_MODE_INFO(&cpr_info.railStates[railIdx], currentMode, currentSubMode);
    CPR_STATIC_LOG_ISR_INFO(&cpr_info.railStates[railIdx], currentMode, currentSubMode);

    cpr_hal_ack_result( &rail->hal, ackResult );

    return changed;
}

static void cpr_rail_set_fused_voltages(cpr_rail* rail, boolean useGlobalCeiling, boolean useGlobalFloor)
{
    const cpr_voltage_plan *vp = rail->vp;
    const cpr_enablement   *e  = rail->enablement;
    cpr_rail_state *railState = cpr_utils_get_rail_state( rail->id );
    uint8 rev = railState ? railState->railCPRRev : 0;

    uint32 fusedCeiling = 0;
    uint32 fusedFloor   = 0;

    for(int modeIdx = vp->modesCount-1; modeIdx >= 0; modeIdx--)
    {
        int32 fuseAdjustment = 0;
        boolean fusedMode = FALSE;
        cpr_mode_settings *modeSettings =
            &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];
        const cpr_voltage_data *vpMode       = &(vp->modes[modeIdx]);

        if(!fusedCeiling)
        {
            /*
             * init with global ceiling voltage of the highest supported mode
             */
            const cpr_freq_data *fmaxVpData =
                &(vpMode->subModes[vpMode->subModesCount-1]);

            fusedCeiling = fmaxVpData->ceiling;
            fusedFloor   = fmaxVpData->floor;
        }

        /*
         * decode the TARG_VOLT fuse for this mode.
         */
        if(vpMode->fuses && vpMode->fuses->volt)
        {
            int32 dFuse = 
                cpr_utils_decode_fuse_value( vpMode->fuses->volt, 1, true );
            modeSettings->decodedFuseSteps = dFuse;
            fusedMode = TRUE;
            CPR_LOG_INFO("Decoded TARG_VOLT fuse = %d for mode %d",
                    dFuse, vp->supportedModes[modeIdx]);
        }
        else
        {
            CPR_LOG_INFO(" TARG_VOLT fuse not found for mode %d",
                    vp->supportedModes[modeIdx]);
        }

        /*
         * decode the TARG_VOLT_OFFSET fuse for this mode.
         */
        if(vpMode->fuses && vpMode->fuses->voltOffset)
        {
            int32 dFuse = 
                cpr_utils_decode_fuse_value( vpMode->fuses->voltOffset, 1, true );
            modeSettings->decodedFuseSteps =
                modeSettings->decodedFuseSteps + dFuse;
            fusedMode = TRUE;
            CPR_LOG_INFO("Decoded TARG_VOLT_OFFSET fuse = %d for mode %d",
                    dFuse, vp->supportedModes[modeIdx]);
        }
        else
        {
            CPR_LOG_INFO(" TARG_VOLT_OFFSET fuse not found for mode %d",
                    vp->supportedModes[modeIdx]);
        }

        if (fusedMode)
        {
            /*
             * Calculate the fuseAdjustment from the decodedFuseSteps.
             * Note: decodedFuseSteps = TARG_VOLT decodedFuseSteps +
             *                              TARG_VOLT_OFFSET decodedFuseSteps
             */
            fuseAdjustment = e->fuseMultiplier * modeSettings->decodedFuseSteps;

            /*
             * calculate fused open loop ceiling voltage
             */
            if(!useGlobalCeiling)
            {
                fusedCeiling = vpMode->fref + fuseAdjustment;
            }

            /*
             * calculate fused open loop floor voltage
             */
            if(!useGlobalFloor)
            {
                const cpr_margin_data* margin = cpr_utils_get_margins( rev, vpMode->margins );

                if(margin && margin->maxFloorToCeil)
                {
                    // max-floor-ceil method
                    fusedFloor = fusedCeiling - margin->maxFloorToCeil;
                }
                else
                {
                    // old school formula
                    fusedFloor = ((vpMode->fref * 90) / 100) + fuseAdjustment + 40000;
                }
            }
        }

        CPR_LOG_INFO(" Fused open loop voltages for mode %d: (floor:%u) (ceiling:%u)",
                     vp->supportedModes[modeIdx], fusedFloor, fusedCeiling);

        for(int submodeIdx = 0; submodeIdx < modeSettings->subModesCount; submodeIdx++)
        {
            cpr_submode_settings* submode  = &modeSettings->subModes[submodeIdx];

            submode->ceiling = fusedCeiling;
            submode->floor   = fusedFloor;
        }
    }
}

static int cpr_rail_find_adj_hi_fused_mode(cpr_rail* rail, int interpModeIdx,
        cpr_mode_settings **subModeHi)
{
    int modeIdx = 0;
    const cpr_voltage_plan *vp = rail->vp;

    for (modeIdx = interpModeIdx+1; modeIdx < vp->modesCount; modeIdx++)
    {
        const cpr_voltage_data *vpMode = &(rail->vp->modes[modeIdx]);

        if (vpMode->fuses && vpMode->fuses->volt)
        {
            *subModeHi =
                &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];
            return modeIdx;
        }
    }

    return -1;
}

static int cpr_rail_find_adj_lo_fused_mode(cpr_rail* rail, int interpModeIdx,
        cpr_mode_settings **subModeLo)
{
    int modeIdx = 0;

    for (modeIdx = interpModeIdx-1; modeIdx >= 0; modeIdx--)
    {
        const cpr_voltage_data *vpMode = &(rail->vp->modes[modeIdx]);

        if (vpMode->fuses && vpMode->fuses->volt)
        {
            *subModeLo =
                &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];
            return modeIdx;
        }
    }

    return -1;
}

/*
 * Interpolation formula for Napali V2 Rails
 *
 * DDR:
 *  LowSVS = MinSVS + (SVS - MinSVS) * 0.50
 *  SVS_L1 = SVS + (NOM - SVS) * 0.57
 *
 * CX:
 *  LowSVS = MinSVS + (SVS - MinSVS) * 0.50
 *  SVS_L1 = SVS + (NOM - SVS) * 0.45
 *
 * MSS:
 *  LowSVS = MinSVS + (SVS - MinSVS) * 0.50
 *  SVS_L1 = SVS + (NOM - SVS) * 0.45
 *
 * LPI_CX:
 *  LowSVS = MinSVS + (SVS - MinSVS) * 0.50
 *  SVS_L1 = SVS + (NOM - SVS) * 0.45
 *
 * GFX:
 *  LowSVS = MinSVS + (SVS - MinSVS) * 0.50
 *  SVS_L1 = SVS + (NOM - SVS) * 0.45
 *  Nom_L1 = Nom + (TurboL1 - Nom) * 0.29
 *  Turbo  = Nom + (TurboL1 - Nom) * 0.61
 *
 *  Formula for finding the interpolated Voltages
 *  ---------------------------------------------
 *
 *  Interpolated Voltage = Lower-Fused-Corner.voltage +
 *      ((Higher-Fused-Corner.voltage - Lower-Fused-Corner.voltage) *
 *          Interpolation-Adjustment-Factor)
 */ 
static void cpr_rail_interpolate_fused_voltages(cpr_rail* rail)
{
    int modeIdx = 0;
    const cpr_voltage_plan *vp = rail->vp;
    cpr_rail_state *railState = cpr_utils_get_rail_state(rail->id);

    for (modeIdx = 0; modeIdx < railState->modeSettingsCount; modeIdx++)
    {
        int subModeIdx = 0;
        const cpr_voltage_data *vpMode = &(vp->modes[modeIdx]);
        cpr_mode_settings *modeSettings = &railState->modeSettings[modeIdx];

        for(subModeIdx = 0; subModeIdx < modeSettings->subModesCount;
                subModeIdx++)
        {
            int modeIdxHi = 0;
            int modeIdxLo = 0;
            
            uint32 interpAdjFactor = 0;

            cpr_mode_settings *modeSettingsHi = NULL;
            cpr_mode_settings *modeSettingsLo = NULL;

            cpr_submode_settings* subMode =  NULL;
            cpr_submode_settings* subModeHi =  NULL;
            cpr_submode_settings* subModeLo =  NULL;

            if(!(vpMode->subModes[subModeIdx].interplEnabled))
                continue;

            interpAdjFactor = vpMode->subModes[subModeIdx].interplEnabled;

            subMode = &modeSettings->subModes[subModeIdx];

            modeIdxHi = cpr_rail_find_adj_hi_fused_mode(rail, modeIdx,
                    &modeSettingsHi);
            subModeHi = &modeSettingsHi->subModes[subModeIdx];

            modeIdxLo = cpr_rail_find_adj_lo_fused_mode(rail, modeIdx,
                    &modeSettingsLo);
            subModeLo = &modeSettingsLo->subModes[subModeIdx];

            subMode->ceiling = subModeLo->ceiling +
                (((subModeHi->ceiling - subModeLo->ceiling) * interpAdjFactor)\
                 / 100);

            subMode->floor = subModeLo->floor +
                (((subModeHi->floor - subModeLo->floor) * interpAdjFactor)\
                 / 100);

            CPR_LOG_INFO("Interpolated rail=%d mode=%d subMode=%d: ceiling:%u \
                    floor:%u", rail->id, modeSettings->mode, subModeIdx, \
                    subMode->ceiling, subMode->floor);
        }
    }
}

static void cpr_rail_adjust_fused_voltages(cpr_rail* rail)
{
    const cpr_voltage_plan *vp = rail->vp;
    cpr_rail_state *railState = cpr_utils_get_rail_state( rail->id );
    uint8 rev = railState ? railState->railCPRRev : 0;

    for(int modeIdx = 0; modeIdx < vp->modesCount; modeIdx++)
    {
        cpr_mode_settings      *modeSettings = &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];
        const cpr_voltage_data *vpMode       = &(vp->modes[modeIdx]);
        const cpr_margin_data  *margin       = cpr_utils_get_margins( rev, vpMode->margins );

        for(int submodeIdx = 0; submodeIdx < modeSettings->subModesCount; submodeIdx++)
        {
            cpr_submode_settings* submode  = &modeSettings->subModes[submodeIdx];
            const uint32          gceiling = vpMode->subModes[submodeIdx].ceiling;
            const uint32          gfloor   = vpMode->subModes[submodeIdx].floor;

            /*
             * Add "CPR Open Loop Voltage Adjustment" from "CPR Fused Corner Adjustment" tab.
             */
            if(margin)
            {
                submode->ceiling += margin->openLoop;
                submode->floor   += margin->openLoop;
            }

            // limit to global limits
            submode->ceiling = MIN( gceiling, MAX( submode->ceiling, gfloor ) );
            submode->floor   = MIN( gceiling, MAX( submode->floor, gfloor ) );

            // round up to PMIC step size.
            submode->ceiling = quantize_up( rail->enablement->stepSize, submode->ceiling );
            submode->floor   = quantize_up( rail->enablement->stepSize, submode->floor );

            submode->current = submode->ceiling;
            submode->min     = submode->ceiling;

            if(!cpr_utils_is_closed_loop_mode(rail->id))
            {
                /*
                 * Set floor to ceiling to make sure CPR controller does not do anything for open loop.
                 * CPR controller is turned on even though it's open loop.
                 */
                submode->floor = submode->ceiling;
            }

            if(submode->floor > submode->ceiling || submode->ceiling > gceiling || submode->floor < gfloor)
            {
                CPR_LOG_FATAL( "Voltages outside allowable settings [rail %s, mode %x] ceiling: %u, floor: %u",
                               rail->name, vp->supportedModes[modeIdx], submode->ceiling, submode->floor );
            }

            CPR_LOG_INFO( " Limited open loop voltage for mode %d submode %d: (floor:%u) (ceiling:%u)",
                         vp->supportedModes[modeIdx], submodeIdx, submode->floor, submode->ceiling );
        }
    }
}

void cpr_rail_set_initial_voltages(cpr_rail* rail, boolean useGlobalCeiling, boolean useGlobalFloor)
{
    /*
     * First, calculate fused open loop voltages for each mode with TARG_VOLT fuse since
     * open loop interpolation requires fused open loop voltages without OL margin adjustment.
     */
    cpr_rail_set_fused_voltages(rail, useGlobalCeiling, useGlobalFloor);

    /*
     * Next, calcualte fused voltage for the mode without TARG_VOLT fuse by Open Loop Interpolation.
     */
    cpr_rail_interpolate_fused_voltages(rail);

    /*
     * Finally, adjust fused voltages with OL margin and to be between global ceiling/floor and round up to PMIC step size.
     */
    cpr_rail_adjust_fused_voltages(rail);
}

static void cpr_rail_copy_openloop_setting(cpr_rail* rail, int dst_mode_idx, int src_mode_idx)
{
    cpr_mode_settings *modeSettings = cpr_info.railStates[rail->railIdx].modeSettings;

    /*
     * Copy open loop settings
     */
    cpr_image_memscpy(&(modeSettings[dst_mode_idx].subModes[0]),
                      &(modeSettings[src_mode_idx].subModes[0]),
                      sizeof(cpr_submode_settings));
}

static void cpr_rail_copy_closedloop_setting(cpr_rail* rail, int dst_mode_idx, int src_mode_idx)
{
    cpr_mode_settings *modeSettings = cpr_info.railStates[rail->railIdx].modeSettings;

    modeSettings[dst_mode_idx].targetsCount = modeSettings[src_mode_idx].targetsCount;
    modeSettings[dst_mode_idx].targets      = modeSettings[src_mode_idx].targets;

    /*
     * Copy closed loop settings
     */
    cpr_image_memscpy(&(rail->vp->modes[dst_mode_idx]),
                      &(rail->vp->modes[src_mode_idx]),
                      sizeof(cpr_voltage_data));
}

/*
 * Find SVS2 and SVS fuse values
 */
__attribute__((noinline))
static void cpr_rail_get_lower_mode_fuse_steps(const cpr_rail* rail, disablement_params_t *params)
{
    params->svs2_step = 0;
    params->svs_step  = 0;

    for(int modeIdx = 0; modeIdx < rail->vp->modesCount; modeIdx++)
    {
        cpr_mode_settings *modeSetting = &cpr_info.railStates[rail->railIdx].modeSettings[modeIdx];

        if(modeSetting->mode < CPR_VOLTAGE_MODE_SVS)
        {
            /*
             * MinSVS / LowSVS
             */
            if(modeSetting->decodedFuseSteps)
            {
                params->svs2_step = modeSetting->decodedFuseSteps;
            }
        }
        else if(modeSetting->mode < CPR_VOLTAGE_MODE_NOMINAL)
        {
            /*
             * SVS / SVS L1
             */
            if(modeSetting->decodedFuseSteps)
            {
                params->svs_step = modeSetting->decodedFuseSteps;
            }
        }
        else
        {
            /*
             * >= Nominal
             */
            break;
        }
    }
}

/*
 * Finds modes for open loop and closed loop based on the table below:
 *
 * Open Loop:
 *
 * TURBO_MODE_ONLY | CPRx_TARG_VOLT_SVS2 | CPRx_TARG_VOLT_SVS || MinSVS  | LowSVS  | SVS     | SVS L1  | Nominal | Turbo
 * ================+=====================+====================++=========+=========+=========+=========+=========+======
 *               0 |              < 0xE  |             < 0xE  || n/a     | n/a     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  || n/a     | n/a     | Nominal | Nominal | n/a     | n/a
 *                 +---------------------+--------------------++---------+---------+---------+---------+---------+------
 *                 |                0xF  |             < 0xE  || SVS     | SVS     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  ||*Nominal*|*Nominal*| Nominal | Nominal | n/a     | n/a
 *                 +---------------------+--------------------++---------+---------+---------+---------+---------+------
 *                 |                0xE  |             < 0xE  || SVS     | SVS     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  || Nominal | Nominal | Nominal | Nominal | n/a     | n/a
 * ----------------+---------------------+--------------------++---------+---------+---------+---------+---------+------
 *               1 |                n/a  |               n/a  || Turbo   | Turbo   | Turbo   | Turbo   | Turbo   | n/a
 * ----------------+---------------------+--------------------++---------+---------+---------+---------+---------+------
 *
 *
 * Closed Loop:
 *
 * TURBO_MODE_ONLY | CPRx_TARG_VOLT_SVS2 | CPRx_TARG_VOLT_SVS || MinSVS  | LowSVS  | SVS     | SVS L1  | Nominal | Turbo
 * ================+=====================+====================++=========+=========+=========+=========+=========+======
 *               0 |              < 0xE  |             < 0xE  || n/a     | n/a     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  || n/a     | n/a     | Nominal | Nominal | n/a     | n/a
 *                 +---------------------+--------------------++---------+---------+---------+---------+---------+------
 *                 |                0xF  |             < 0xE  || SVS     | SVS     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  ||*SVS*    |*SVS*    | Nominal | Nominal | n/a     | n/a
 *                 +---------------------+--------------------++---------+---------+---------+---------+---------+------
 *                 |                0xE  |             < 0xE  || SVS     | SVS     | n/a     | n/a     | n/a     | n/a
 *                 |                     |            >= 0xE  || Nominal | Nominal | Nominal | Nominal | n/a     | n/a
 * ----------------+---------------------+--------------------++---------+---------+---------+---------+---------+------
 *               1 |                n/a  |               n/a  || Turbo   | Turbo   | Turbo   | Turbo   | Turbo   | n/a
 * ----------------+---------------------+--------------------++---------+---------+---------+---------+---------+------
 */
static void cpr_rail_get_restricted_modes(cpr_voltage_mode mode, const disablement_params_t *params, cpr_voltage_mode *openloop_mode, cpr_voltage_mode *closedloop_mode)
{
    if(params->turboOnly)
    {
        *openloop_mode = *closedloop_mode = CPR_VOLTAGE_MODE_TURBO;
        return;
    }

    if(mode < CPR_VOLTAGE_MODE_SVS)
    {
        /*
         * MinSVS / LowSVS
         */
        if(params->svs2_step == 0xF)
        {
            if(params->svs_step < 0xE)
            {
                *openloop_mode = *closedloop_mode = CPR_VOLTAGE_MODE_SVS;
            }
            else
            {
                *openloop_mode   = CPR_VOLTAGE_MODE_NOMINAL;
                *closedloop_mode = CPR_VOLTAGE_MODE_SVS;
            }
        }
        else if (params->svs2_step == 0xE)
        {
            if(params->svs_step < 0xE)
            {
                *openloop_mode = *closedloop_mode = CPR_VOLTAGE_MODE_SVS;
            }
            else
            {
                *openloop_mode = *closedloop_mode = CPR_VOLTAGE_MODE_NOMINAL;
            }
        }
    }
    else if(mode < CPR_VOLTAGE_MODE_NOMINAL)
    {
        /*
         * SVS / SVS L1
         */
        if(params->svs_step >= 0xE)
        {
            *openloop_mode = *closedloop_mode = CPR_VOLTAGE_MODE_NOMINAL;
        }
    }
}

static void cpr_rail_update_lower_modes(cpr_rail* rail, const disablement_params_t *params)
{
    /*
     * Update open loop and closed loop config for lower modes (except Turbo)
     */
    for(int modeIdx = 0; modeIdx < rail->vp->modesCount-1; modeIdx++)
    {
        cpr_voltage_mode mode                       = cpr_info.railStates[rail->railIdx].modeSettings[modeIdx].mode;
        cpr_voltage_mode restricted_openloop_mode   = CPR_VOLTAGE_MODE_OFF;
        cpr_voltage_mode restricted_closedloop_mode = CPR_VOLTAGE_MODE_OFF;

        cpr_rail_get_restricted_modes(mode, params, &restricted_openloop_mode, &restricted_closedloop_mode);

        /*
         * No update
         */
        if(restricted_openloop_mode == CPR_VOLTAGE_MODE_OFF || restricted_closedloop_mode == CPR_VOLTAGE_MODE_OFF)
        {
            continue;
        }

        /*
         * Update mode setting
         */
        if(params->updateOpenLoop)
        {
            cpr_rail_copy_openloop_setting(rail, modeIdx, cpr_utils_get_mode_idx(rail, restricted_openloop_mode));
        }

        if(params->updateClosedLoop)
        {
            cpr_rail_copy_closedloop_setting(rail, modeIdx, cpr_utils_get_mode_idx(rail, restricted_closedloop_mode));
        }
    }
}

/*
 * Disable MinSVS/LowSVS/SVS/SVS_L1 based on fuse values (CR 1008675). The table is in the Fusing Guide.
 * The forceDisable flag is to ignore the table and force to disable all SVS modes (CR 1012794).
 * CR 1050500: disable all modes except Turbo if turboOnly is set.
 */
void cpr_rail_disable_lower_modes(cpr_rail* rail, cpr_rail* fuse_rail, boolean updateOpenLoop, boolean updateClosedLoop, boolean forceDisable, boolean turboOnly)
{
    disablement_params_t params = {
        .turboOnly        = turboOnly,
        .forceDisable     = forceDisable,
        .updateOpenLoop   = updateOpenLoop,
        .updateClosedLoop = updateClosedLoop,
        .svs2_step        = 0xE,  /* 0xE is to force to disable all SVS modes. */
        .svs_step         = 0xE   /* 0xE is to force to disable all SVS modes. */
    };

    if(!turboOnly && !forceDisable && fuse_rail)
    {
        /*
         * Get SVS2 and SVS fuse step value
         */
        cpr_rail_get_lower_mode_fuse_steps(fuse_rail, &params);
    }

    cpr_rail_update_lower_modes(rail, (const disablement_params_t*)&params);
}

/*
 * Returns KV value for the RO.
 */
static uint16 cpr_rail_get_kv(const cpr_kv_cfg* kvs_cfg, uint32 ro)
{
    uint16 kv = 0;

    for(int i = 0; i < kvs_cfg->count; i++)
    {
        if(kvs_cfg->kvs[i].ro == ro)
        {
            kv = kvs_cfg->kvs[i].kv;
            break;
        }
    }

    return kv;
}

/*
 * Calculates target quotients.
 * NOTE: Does not configure HW.
 * NOTE: Until this function is called, target quotients are *NOT* set.
 *       Must be called when initializing closed loop and when some margin changed.
 */
void cpr_rail_calculate_target_quotients(cpr_rail* rail, cpr_rail_state* state)
{
    const cpr_voltage_plan* vp = rail->vp;

    for(int i = 0; i < vp->modesCount; i++)
    {
        const cpr_voltage_data *vpMode = &(vp->modes[i]);
        cpr_quotient       localQuot;
        cpr_quotient_cfg   baseQuots;
        cpr_mode_settings* modeSettings = &state->modeSettings[i];
        const cpr_margin_data* margin =
            cpr_utils_get_margins( state->railCPRRev, vpMode->margins );

        int32  margin_uv = (margin == NULL) ? 0 : margin->closedLoop;

        // Check if we are using local quots
        if(vpMode->fuses != NULL && vpMode->fuses->quot != NULL)
        {
            localQuot.quotient = cpr_utils_decode_fuse_value( vpMode->fuses->quot, 1, false );
            localQuot.ro       = cpr_utils_decode_fuse_value( vpMode->fuses->rosel, 1, false );
            baseQuots.quots    = &localQuot;
            baseQuots.count    = 1;
        }
        else if(vpMode->quotients)
        {
            baseQuots.quots = vpMode->quotients->quots;
            baseQuots.count = vpMode->quotients->count;
        }
        else
        {
            continue;
        }

        for(int k = 0; k < CPR_MARGIN_ADJUSTMENT_TEST; k++) {
            margin_uv += state->marginAdjustments[k];
        }

        // Adding offset fuse value to closed loop margins
        if(vpMode->fuses && vpMode->fuses->voltOffset)
        {
            modeSettings->decodedFuseOffset = cpr_utils_decode_fuse_value( vpMode->fuses->voltOffset, 1, true );

            margin_uv += modeSettings->decodedFuseOffset * rail->enablement->fuseMultiplier;
        }

        modeSettings->totalMarginMV = (int16)(margin_uv/1000);

        CPR_LOG_INFO( "Updating %s quotients with %d mV total margin (%s, mode: %u)",
                      baseQuots.quots == &localQuot ? "LOCAL" : "GLOBAL",
                      margin_uv, rail->name, vp->supportedModes[i] );

        CPR_ASSERT(baseQuots.count == modeSettings->targetsCount);

        for(int k = 0; k < baseQuots.count; k++)
        {
            modeSettings->targets[k] = baseQuots.quots[k];

            if(modeSettings->targets[k].quotient == 0) {
                CPR_LOG_WARNING( " Quotient is 0 for RO %d", baseQuots.quots[k].ro );
                continue;
            }

            uint16 kv = cpr_rail_get_kv(vp->kvCfg, baseQuots.quots[k].ro);

            // Target_Quotient = Base_Quotient + (((static_margin_mV * ro_kv_x_100) + 99) / 100)
            modeSettings->targets[k].quotient += (((int32)((margin_uv/1000) * kv) + 99) / 100);

            CPR_LOG_TRACE( " RO: %u, Base: %u, KV: %u, Adjusted: %u",
                           baseQuots.quots[k].ro,
                           baseQuots.quots[k].quotient,
                           kv,
                           modeSettings->targets[k].quotient );
        }
    }
}

void cpr_rail_disable(cpr_rail* rail)
{
    cpr_rail_state* railState = &cpr_info.railStates[rail->railIdx];

    if(railState->activeMode != NULL)
    {
        CPR_LOG_INFO( " Disabling %s", rail->name );

        // Disable Temperature Adjustments
        cpr_hal_enable_temp_margin_adj(&rail->hal, rail->vp, false);

        cpr_hal_disable_rail( &rail->hal );

        cpr_utils_set_active_mode_setting(rail->id, NULL, NULL);

        CPR_STATIC_LOG_RAIL_INFO(railState);
    }
}

/* 
 * Write all mode/submode ceiling, floor values to CPRF vmax(, vsafe) & vmin
 * regs respectively.
 * This should be called after open loop is initalized to switch from global
 * ceiling, floor to open loop ceiling, floor.
 */
void cpr_rail_write_all_voltages(cpr_rail* rail)
{
    int i = 0;
    cpr_mode_settings* modeState;
    cpr_submode_settings* submodeState = NULL;
    cpr_rail_state* railState = &cpr_info.railStates[rail->railIdx];

    /* ToDo: Need a better way to factor in the right mode with the right hlvl.
     * Set the voltages for modes starting @ index 2, To account for OFF, RET.
     * Assuming here that we will always have modes and there voltages listed in
     * and increasing order. this is required since the enums for modes (cpr_voltage_mode)
     * are common for multiple targets and all targets will not have all the levels.
     */
    for(i = 0; i < railState->modeSettingsCount; i++) {
        modeState = &railState->modeSettings[i];
        submodeState = &modeState->subModes[modeState->subModesCount - 1];

        CPR_LOG_TRACE( "For %s mode=%d: Set vmax=%d vmin=%d", rail->name, i+2,
                submodeState->ceiling, submodeState->floor );
        cpr_hal_write_cprf_voltages(i+2, submodeState->ceiling/1000,
                submodeState->floor/1000);
    }
   
    /* Fix for KW issue: Possible use of un-initialized var submodeState */ 
    if (!submodeState)
        return;

    /* Set the voltages for the modes greater than the highest supported mode,
     * to the voltages of the highest supported mode.
     * Highest supported mode is set in 'i' from the above loop.
     * increment it by 2 to account for OFF, RET.
     */
    for(i+=2; i<16; i++) {
        CPR_LOG_TRACE( "For %s mode=%d: Set vmax=%d vmin=%d", rail->name, i,
                submodeState->ceiling, submodeState->floor );
        cpr_hal_write_cprf_voltages(i, submodeState->ceiling/1000,
                submodeState->floor/1000);
    }
}

void cpr_rail_write_all_target_quotients(cpr_rail* rail)
{
    int i = 0;
    int j = 0;
    cpr_rail_state* railState = &cpr_info.railStates[rail->railIdx];
    cpr_mode_settings* modeState;

    /*
     * HW Workaround: Set quotients for all RO's of Mode 0, 1 to 0xfff
     */
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 16; j++) {
            CPR_LOG_TRACE( "For %s mode=%d: Set ro=%d quot=%d", rail->name,
                    i, j, 0xfff);
            cpr_hal_set_target_quotient(&rail->hal, i, j, 0xfff);
        }
    }

    /* ToDo: Need a better way to factor in the right mode with the right
     * hlvl. Set the voltages for modes starting @ index 2, To account for
     * OFF, RET. Assuming here that we will always have modes and there
     * voltages listed in and increasing order. this is required since the
     * enums for modes (cpr_voltage_mode) are common for multiple targets
     * and all targets will not have all the levels.
     */
    for(i = 0; i < railState->modeSettingsCount; i++) {
        modeState = &railState->modeSettings[i];
        for(j = 0; j < modeState->targetsCount; j++) {
            cpr_quotient *target = &modeState->targets[j];
            CPR_LOG_TRACE( "For %s mode=%d: Set ro=%d quot=%d", rail->name,
                    i+2, target->ro, target->quotient );
            cpr_hal_set_target_quotient(&rail->hal, i+2,
                    target->ro, target->quotient);
        }
    }
}

void cpr_rail_enable_hw_closed_loop( cpr_rail *rail )
{
    CPR_LOG_TRACE( "Prepare and Enable HW Closed Loop for %s", rail->name);
    cpr_hal_enable_hw_closed_loop(&rail->hal);
}

void cpr_rail_write_cprf_settings( cpr_rail *rail, uint8 cprf_id )
{
    CPR_LOG_TRACE( "Prepare and Enable CPRF(%d) HW for %s", cprf_id,
            rail->name);
    cpr_hal_set_cprf_defaults ( rail->name, cprf_id );
    cpr_hal_cprf_enable( rail->name, cprf_id );
}

void cpr_rail_enable(cpr_rail* rail, cpr_mode_settings* modeState,
        cpr_submode_settings* submodeState, boolean changeOveride)
{
    cpr_rail_state* railState = &cpr_info.railStates[rail->railIdx];
    boolean modeChanged = railState->activeMode == NULL ||
        railState->activeMode->activeSubMode != submodeState;

    if(modeChanged || changeOveride)
    {
        if(!new_target_quotients && max_target_count)
        {
            new_target_quotients =
                cpr_image_malloc(max_target_count * sizeof(cpr_quotient));
        }

        CPR_ASSERT(new_target_quotients);

        /*
         * reset values to calculate new target quotients
         */
        memset(new_target_quotients, 0, modeState->targetsCount *
                sizeof(cpr_quotient));

        cpr_rail_disable( rail );

        cpr_utils_set_active_mode_setting(rail->id, modeState, submodeState);

        CPR_LOG_INFO( " Setting %s target quotients to mode %u, freq %u",
                rail->name, modeState->mode, submodeState->freq );

        interpolate_quotients( rail, modeState, submodeState,
                new_target_quotients, false );

        for(int i = 0; i < cpr_info.settings->railsCount; i++)
        {
            cpr_rail* otherRail = cpr_info.settings->rails[i];

            if(rail == otherRail ||
                    rail->hal.controller != otherRail->hal.controller ||
                    rail->hal.thread != otherRail->hal.thread) {
                continue;
            }

            cpr_mode_settings* otherMode =
                cpr_info.railStates[otherRail->railIdx].activeMode;

            if(otherMode && otherMode->activeSubMode &&
                    otherMode->targetsCount > 0) {
                interpolate_quotients( otherRail, otherMode,
                        otherMode->activeSubMode, new_target_quotients,
                        true );
            }
        }

        cpr_hal_set_targets( &rail->hal, modeState->mode, submodeState->freq,
                             new_target_quotients,
                             modeState->targetsCount );

        //called after each mode switch to take care of margin from temp adj
        //feature
        cpr_hal_set_temp_margin(&rail->hal,
                cpr_utils_get_mode_idx(rail, modeState->mode), rail->vp);

        CPR_STATIC_LOG_RAIL_INFO(railState);
        CPR_STATIC_LOG_MODE_INFO(railState, modeState, submodeState);
    }

    uint32 current = submodeState->current;
    boolean up = current < submodeState->ceiling;
    boolean dn = current > submodeState->floor;

    CPR_LOG_INFO( " Enabling %s, enabled (UP_IRQ_EN: %u, DOWN_IRQ_EN: %u)",
            rail->name, up, dn );

    // enable the temperature adj feature
    cpr_hal_enable_temp_margin_adj(&rail->hal, rail->vp, true);

    cpr_hal_enable_rail( &rail->hal, up, dn, false,
            railState->cMode == CPR_CONTROL_SW_CLOSED_LOOP );
}

void cpr_rail_register_isr(cpr_rail* rail)
{
    cpr_image_register_isr( rail->id, rail->interruptId, cpr_rail_isr, rail );
}
