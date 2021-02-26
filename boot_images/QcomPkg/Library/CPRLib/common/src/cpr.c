/**
 * @file:  cpr.c
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr.c#1 $
 * $Change: 14402555 $
 */
#include "cpr.h"
#include "cpr_logs.h"
#include "cpr_cfg.h"
#include "cpr_hal.h"
#include "cpr_data.h"
#include "cpr_utils.h"
#include "cpr_measurements.h"
#include "cpr_rail.h"
#include "cpr_image.h"
#include "cpr_voltage_plan.h"

//******************************************************************************
// Global Data
//******************************************************************************

cpr_information cpr_info = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

/**
*
* <!-- calc_safe_voltage -->
*
*/
static uint32 calc_safe_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    cpr_rail             *rail = cpr_utils_get_rail(railId);
    cpr_submode_settings *submodeState;
    uint32                min_plus_two_pmic_steps;

    cpr_utils_get_mode_settings( railId, info, NULL, &submodeState );

    /*
     * Safe voltage is the lowest voltage plus two PMIC steps or the open loop voltage
     * depending on which is lower.
     */

    min_plus_two_pmic_steps = submodeState->min + (rail->enablement->stepSize * 2);

    return MIN( min_plus_two_pmic_steps, submodeState->ceiling );
}

/**
*
* <!-- init_cpr_config -->
*
* Initialize rail config data.
* Should not access to CPR controller.
*
*/
static void init_cpr_config(void)
{
    CPR_LOG_INIT();

    CPR_LOG_INFO( "Init CPR cfg data" );

    /*
     * First get Enablement Cfg Info and Voltage Plan Info
     */
    cpr_info.settings    = cpr_utils_get_settings();

    cpr_info.chipVersion = cpr_image_get_chip_version();
    cpr_info.foundry     = cpr_image_get_foundry();
    cpr_info.cprRev      = cpr_utils_decode_fuse_value(&cpr_info.settings->miscCfg->cprRev, 1, false);

    cpr_info.railStates  = cpr_image_malloc(sizeof(cpr_rail_state) * cpr_info.settings->railsCount);

    /*
     * initialize CPR config data
     */
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        cpr_rail* rail = cpr_info.settings->rails[i];

        cpr_rail_init( rail );
    }

    /*
     * must be after cpr info was populated in order to calculate number of slots
     */
    CPR_STATIC_LOG_INIT();
    CPR_LOG_INFO( "--- Done with CPR cfg data init ---" );
}

/**
*
* <!-- configure_cpr -->
*
* Initialize CPR controller and call configurable init func defined in cpr_image_init.c
* Configure mode voltages based on the rail mode (OpenLoop/ClosedLoop)
* and configure HW if ClosedLoop.
*
*/
static void configure_cpr(void)
{
    CPR_LOG_INFO( "Config CPR rails" );

    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        cpr_rail* rail = cpr_info.settings->rails[i];

        cpr_cfg_funcs* funcs =
            cpr_image_target_get_init_func(rail->enablement->funcId);

        while(funcs != NULL && funcs->init != NULL)
        {
            cpr_info.railStates[rail->railIdx].cMode = funcs->cMode;

            funcs = funcs->init( rail );
        }
    }

    CPR_STATIC_LOG_UPDATE_ALL();
    CPR_LOG_INFO( "--- Done with CPR rail config ---" );
}

/**
*
* <!-- lock_and_init_data -->
*
* Wrapper function for lock, init, and config.
* to avoid repeating the same code again and agin.
*
*/
static void lock_and_init_data(boolean requireInit, boolean requireCfg)
{
    CPR_IMAGE_LOCK_CORE();

    if(requireInit && !cpr_info.dataInit)
    {
        cpr_info.dataInit = true; // set here to prevent recursive call

        init_cpr_config();
    }

    if(requireCfg && !cpr_info.cfgInit)
    {
        /*
         * CPR config data must be initialized first.
         */
        CPR_ASSERT(cpr_info.dataInit);

        cpr_info.cfgInit = true; // set here to prevent recursive call

        configure_cpr();
    }
}

/**
*
* <!-- resume_control -->
*
*/
static void resume_control(cpr_domain_id railId, cpr_domain_info* info, uint32 voltage)
{
    cpr_mode_settings* newModeState;
    cpr_submode_settings* newSubmodeState;
    cpr_rail* rail = cpr_utils_get_rail( railId );

    /*
     * Only allow resuming control:
     * - if this is called by ISR (since CPR IRQ fired means CPR CL has been enabled)
     * - if not called by ISR, decision is specific to image.
     *   On Modem, it can resume only if nobody has voted for disabling CPR.
     */
    if(!cpr_info.processingIsr && !cpr_image_can_resume_control(railId))
    {
        CPR_LOG_WARNING( "Cannot resume control on %s", rail->name );
        return;
    }

    CPR_LOG_TRACE( "Resume control on %s, mode 0x%x, voltage %u", rail->name, info->u.mode, voltage );

    if(info->u.mode == CPR_VOLTAGE_MODE_OFF)
    {
        cpr_rail_disable( rail );
        return;
    }

    cpr_utils_get_mode_settings( railId, info, &newModeState, &newSubmodeState );

    if(voltage > newSubmodeState->ceiling || voltage < newSubmodeState->floor)
    {
        uint32 v = MIN( newSubmodeState->ceiling, MAX( voltage, newSubmodeState->floor ) );
        CPR_LOG_WARNING( "Clipping out of bounds voltage on resume: %u -> %u", voltage, v );
        voltage = v;
    }

    if(newSubmodeState->current != voltage)
    {
        CPR_LOG_WARNING( "Voltage change on resume: %u -> %u", newSubmodeState->current, voltage );

        newSubmodeState->current = voltage;
        newSubmodeState->min = MIN( voltage, newSubmodeState->min );
    }

    cpr_rail_enable( rail, newModeState, newSubmodeState, false );
}

//******************************************************************************
// Public API Functions
//******************************************************************************

/**
*
* <!-- cpr_init -->
*
*/
void cpr_init()
{
    lock_and_init_data(true, true);

    CPR_LOG_INFO( "Enabling CPR rails" );

    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        cpr_rail* rail = cpr_info.settings->rails[i];
        cpr_cfg_funcs* funcs = cpr_image_target_get_init_func(rail->enablement->funcId);

        // Call configurable enable func defined in cpr_init.c
        while(funcs != NULL && funcs->enable != NULL)
        {
            funcs = funcs->enable( rail );
        }
    }

#if 0 /* The thermal callback is not used in XBL CPR but it's not compiled out due to this code.
         Disable this code to compiler out the thermal callback in order to save memory. */
    /*
     * register thermal event callback if there is closed loop rail
     */
    for(int i = 0; i < cpr_info.settings->railsCount; i++)
    {
        cpr_rail* rail = cpr_info.settings->rails[i];

        if(!cpr_utils_is_closed_loop_mode(rail->id))
        {
            continue;
        }

        cpr_image_register_thermal_cb( cpr_measurements_thermal_update );
        break;
    }
#endif

    cpr_info.fullInit = true;

    CPR_STATIC_LOG_CPR_INFO(&cpr_info);

    CPR_LOG_INFO( "--- Done with full init ---" );

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_get_supported_modes -->
*
*/
void cpr_get_supported_modes(cpr_domain_id railId, const cpr_voltage_mode** modes, uint32* modesCount)
{
    /*
     * need only data init.
     */
    lock_and_init_data(true, false);

    cpr_rail* rail = cpr_utils_get_rail( railId );
    *modes = rail->vp->supportedModes;
    *modesCount = rail->vp->modesCount;

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_get_voltage -->
*
*/
uint32 cpr_get_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    cpr_submode_settings* modeState;

    lock_and_init_data(true, false);

    cpr_utils_get_mode_settings( railId, info, NULL, &modeState );

    CPR_IMAGE_UNLOCK_CORE();

    return modeState->current;
}

/**
*
* <!-- cpr_get_safe_voltage -->
*
*/
uint32 cpr_get_safe_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    lock_and_init_data(true, false);

    uint32 rtn = calc_safe_voltage( railId, info );

    CPR_IMAGE_UNLOCK_CORE();

    return rtn;
}

/**
*
* <!-- cpr_pre_state_switch -->
*
*/
void cpr_pre_state_switch(cpr_domain_id railId)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    CPR_IMAGE_LOCK_CORE();

    /*
     * Should be called only when the rail is closed loop
     * If rail is not closed loop, pre/post callback shouldn't be registered
     */
    CPR_ASSERT(cpr_utils_is_closed_loop_mode(railId));

    cpr_utils_set_disable_vote(railId, CPR_DISABLE_VOTE_MODE_SWITCH);

    cpr_rail* rail = cpr_utils_get_rail( railId );

    cpr_rail_disable( rail );

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_post_state_switch -->
*
*/
void cpr_post_state_switch(cpr_domain_id railId, cpr_domain_info* info, uint32 voltage)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    CPR_IMAGE_LOCK_CORE();

    /*
     * Should be called only when the rail is closed loop
     * If rail is not closed loop, pre/post callback shouldn't be registered
     */
    CPR_ASSERT(cpr_utils_is_closed_loop_mode(railId));

    /*
     * Should be called only after cpr_pre_state_switch() is called.
     */
    CPR_ASSERT(cpr_utils_has_disable_vote(railId, CPR_DISABLE_VOTE_MODE_SWITCH));

    cpr_utils_clear_disable_vote(railId, CPR_DISABLE_VOTE_MODE_SWITCH);

    resume_control(railId, info, voltage);

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_relinquish_control -->
*
*/
void cpr_relinquish_control(cpr_domain_id railId)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    /*
     * Cannot assert since this could be called even if the rail is not closed loop.
     */
    if(!cpr_utils_is_closed_loop_mode(railId))
    {
        return;
    }

    CPR_IMAGE_LOCK_CORE();

    cpr_utils_set_disable_vote(railId, CPR_DISABLE_VOTE_RELINQUISH);

    cpr_rail* rail = cpr_utils_get_rail( railId );
    cpr_rail_state* railState = cpr_utils_get_rail_state( railId );

    if(railState->activeMode == NULL) {
        CPR_IMAGE_UNLOCK_CORE();
        return;
    }

    /*
     * Reset to use safe voltage when we resume
     * VCS should call cpr_get_voltage() to get the safe voltage or
     * call cpr_get_safe_voltage() directly before calling cpr_resume_control()
     */
    cpr_domain_info info = {CPR_DOMAIN_TYPE_MODE_BASED, {railState->activeMode->mode}};
    railState->activeMode->activeSubMode->current = calc_safe_voltage(railId, &info);

    CPR_LOG_TRACE( "Relinquish control on %s (reset voltage to: %u)",
                   rail->name, railState->activeMode->activeSubMode->current );

    cpr_rail_disable( rail );

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_resume_control -->
*
*/
void cpr_resume_control(cpr_domain_id railId, cpr_domain_info* info, uint32 voltage)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    /*
     * Cannot assert since this could be called even if the rail is not closed loop.
     */
    if(!cpr_utils_is_closed_loop_mode(railId))
    {
        return;
    }

    CPR_IMAGE_LOCK_CORE();

    /*
     * Should be called only after cpr_relinquish_control() is called.
     */
    CPR_ASSERT(cpr_utils_has_disable_vote(railId, CPR_DISABLE_VOTE_RELINQUISH));

    cpr_utils_clear_disable_vote(railId, CPR_DISABLE_VOTE_RELINQUISH);

    resume_control(railId, info, voltage);

    CPR_IMAGE_UNLOCK_CORE();
}

/**
*
* <!-- cpr_get_ceiling_voltage -->
* TODO: delete when VCS driver stops managing voltage range.
*
*/
uint32 cpr_get_ceiling_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    cpr_submode_settings* modeState;

    lock_and_init_data(true, false);

    cpr_utils_get_mode_settings( railId, info, NULL, &modeState );

    CPR_IMAGE_UNLOCK_CORE();

    return modeState->ceiling;
}

/**
*
* <!-- cpr_get_floor_voltage -->
* TODO: delete when VCS driver stops managing voltage range.
*
*/
uint32 cpr_get_floor_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    cpr_submode_settings* modeState;

    lock_and_init_data(true, false);

    cpr_utils_get_mode_settings( railId, info, NULL, &modeState );

    CPR_IMAGE_UNLOCK_CORE();

    return modeState->floor;
}

/**
*
* <!-- cpr_get_eldo_voltage -->
*
*/
uint32 cpr_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info)
{
    lock_and_init_data(true, false);

    uint32 rtn = cpr_image_get_eldo_voltage( railId, info );

    CPR_IMAGE_UNLOCK_CORE();

    return rtn;
}

/**
*
* <!-- cpr_enter_sleep -->
*
*/
void cpr_enter_sleep(void)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    cpr_image_enter_sleep();
}

/**
*
* <!-- cpr_exit_sleep -->
*
*/
void cpr_exit_sleep(void)
{
    /*
     * Do not want to modify CPR controller state before de-aging completes.
     * Also don't see a reason why we want to enable/disable CPR before full init done.
     */
    if(!cpr_info.fullInit)
    {
        return;
    }

    cpr_image_exit_sleep();
}

