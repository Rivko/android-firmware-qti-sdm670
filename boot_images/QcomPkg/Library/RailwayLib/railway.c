// railway.c - railway top-level implementation
//
// Copyright 2011 - 2012 by QUALCOMM Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header: $
// $ Author: $
// $ DateTime: $
//

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"
#include "alloca.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include <string.h>
#include <stdbool.h>
#include "CoreVerify.h"
#include "stringl/stringl.h"
#include "LoaderUtils.h"
#include "ChipInfo.h"
#include "cpr.h"


static const railway_internal_cb_type railway_callback_remap[RAILWAY_NUM_CB_TYPES] =
{
    RAILWAY_INTERNAL_PRECHANGE_CB,      //RAILWAY_PRECHANGE_CB
    RAILWAY_INTERNAL_POSTCHANGE_CB,     //RAILWAY_POSTCHANGE_CB
    RAILWAY_INTERNAL_PRECHANGE_CB,      //RAILWAY_LAST_PRECHANGE_CB
    RAILWAY_INTERNAL_POSTCHANGE_CB,     //RAILWAY_FIRST_POSTCHANGE_CB
};


static void initialize_rail(uint32 rail_num)
{
    const railway_rail_config_t *rail_config = &RAILWAY_CONFIG_DATA->rails[rail_num];

    railway_rail_state_t* rail_state = &railway.rail_state[rail_num];

    // Compute what our initial request should be.
    // Update our initial state.
    rail_state->current_active.mode       = rail_config->initial_corner;
    rail_state->current_active.microvolts = railway_get_corner_voltage(rail_num, rail_config->initial_corner);
    rail_state->retention_voltage_uv      = rail_config->default_retention_uv;
}

static void railway_walk_cbs(const railway_rail_state_t *rail_data, railway_internal_cb_type type, const railway_settings *proposal)
{
    //Must be done from 0 upwards as we have some callbacks which must be called first which are in the 0th slot.
    for(int i=0; i<RAILWAY_MAX_CBS; i++)
    {
        if(rail_data->cbs[type][i].callback_fn)
        {
            rail_data->cbs[type][i].callback_fn(proposal, rail_data->cbs[type][i].callback_cookie);
        }
    }
}

static void railway_transition_individual_rail(int rail_num)
{
    railway_rail_state_t   *rail_data = &railway.rail_state[rail_num];
    railway_rail_config_t  *rail_cfg  = &RAILWAY_CONFIG_DATA->rails[rail_num];
    const railway_settings *target    = &rail_data->constrained_target;

    if(rail_cfg->pmic_volt_func == NULL)
    {
        CORE_VERIFY(sizeof(railway_settings) == memscpy(&rail_data->current_active, sizeof(railway_settings), target, sizeof(railway_settings)));
        return;
    }

    railway_walk_cbs(rail_data, RAILWAY_INTERNAL_PRECHANGE_CB, target);

    // Only set the actual voltage if it's changing. It could be that we're just changing modes.
    // E.g. if CPR has taken the voltage down in one mode to the point that it is the same voltage as another mode.
    //
    // CR 996920: Call PMIC API always on XBL to make sure Railway/CPR is in sync with actual PMIC voltage.
    //if(rail_data->constrained_target.microvolts!=rail_data->current_active.microvolts)
    {
        rail_cfg->pmic_volt_func(rail_cfg->pmic_chip_id, rail_cfg->pmic_peripheral_index, target->microvolts);
    }

    CORE_VERIFY(sizeof(railway_settings) == memscpy(&rail_data->current_active, sizeof(railway_settings), target, sizeof(railway_settings)));
    railway_walk_cbs(rail_data, RAILWAY_INTERNAL_POSTCHANGE_CB, target);
}

static void railway_quantize_constrained_target(int rail)
{
    railway_corners_t* rail_corners = railway_get_supported_corners(rail);
    const railway_corner highest_supported_corner = rail_corners->corners[rail_corners->corners_count-1];
    railway_corner quantized_corner = highest_supported_corner;

    /*
     * If the requested corner is higher than the highest supported corner, just use the highest supported corner
     * instead of abort.
     * CPR driver uses TURBO instead of SUPER_TURBO since Voltage Plan uses Turbo, and
     * SUPER_TURBO enum value is higher than TURBO enum value * even though they are same.
     * We should just cap the requested corner to the highest supported corner.
     */
    if (railway.rail_state[rail].constrained_target.mode < highest_supported_corner)
    {
        for(int i = rail_corners->corners_count-1; i>=0; i--)
        {
            //Check that we're not going lower than the constrained target's mode.
            if(rail_corners->corners[i] < railway.rail_state[rail].constrained_target.mode)
            {
                break;
            }

            quantized_corner = rail_corners->corners[i];
        }
    }

    railway.rail_state[rail].constrained_target.microvolts = railway_get_corner_voltage(rail, quantized_corner);

    /*
     * CR#2044731: Add support for Cx Turbo_L1 with Mx @ Turbo. Applicable to Napali V1 Only.
     * Do not update the constrained_target.mode for Mx rail when Cx is @Turbo_L1.
     */
    if ((rail == mx_rail_id) &&
            (ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SDM845) &&
            (ChipInfo_GetChipVersion() < CHIPINFO_VERSION(2,0)) &&
            (railway.rail_state[cx_rail_id].constrained_target.mode == RAILWAY_TURBO_L1) &&
            (railway.rail_state[mx_rail_id].constrained_target.mode == RAILWAY_TURBO_L1))
        return;

    railway.rail_state[rail].constrained_target.mode       = quantized_corner;
}

static void railway_update_constrained_targets(int rail, railway_rail_state_t* rail_state, railway_corner floor)
{
    //Start from the unconstrained target.
    rail_state->constrained_target.mode       = MAX(rail_state->unconstrained_target, floor);
    rail_state->constrained_target.microvolts = 0;

    railway_quantize_constrained_target(rail);
}

static void railway_do_update_constrained_targets( void )
{
    //Go through the rails and calculate their constrained voltage target.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if(i == mx_rail_id)   //Skip Mx, we do that later.
            continue;

        railway_update_constrained_targets(i, &railway.rail_state[i], RAILWAY_RETENTION);
    }

    //Next, bump up MX to the max of the logical rails.
    railway_update_constrained_targets(mx_rail_id, &railway.rail_state[mx_rail_id], railway.rail_state[cx_rail_id].constrained_target.mode);
}

static void railway_do_transition_rails(void)
{
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        //To do - move this to when we update votes on rails - I'm sure that will be more efficient.
        //Need to figure out how we deal with CPR changing corner voltages under our feet though.
        railway.rail_state[i].unconstrained_target = railway_aggregated_voltage_target(i);
    }

    //Now update the constrained targets for the rails.
    railway_do_update_constrained_targets();

    //Now do the transitions.
    //If MX is going up, do that first.
    if((railway.rail_state[mx_rail_id].constrained_target.microvolts>railway.rail_state[mx_rail_id].current_active.microvolts) ||
        (railway.rail_state[mx_rail_id].constrained_target.mode>railway.rail_state[mx_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(mx_rail_id);
    }

    //Transition EBI if it's going up.
    if((railway.rail_state[ebi_rail_id].constrained_target.microvolts > railway.rail_state[ebi_rail_id].current_active.microvolts) ||
       (railway.rail_state[ebi_rail_id].constrained_target.mode > railway.rail_state[ebi_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(ebi_rail_id);
    }

    //Now transition the logical rails
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        if( (i == mx_rail_id) || (i == ebi_rail_id) )  // Skip MX & EBI
            continue;

        if((railway.rail_state[i].constrained_target.microvolts!=railway.rail_state[i].current_active.microvolts) ||
            (railway.rail_state[i].constrained_target.mode!=railway.rail_state[i].current_active.mode))
        {
            railway_transition_individual_rail(i);
        }
    }

    //Transition EBI if it's going down.
    if((railway.rail_state[ebi_rail_id].constrained_target.microvolts < railway.rail_state[ebi_rail_id].current_active.microvolts) ||
       (railway.rail_state[ebi_rail_id].constrained_target.mode < railway.rail_state[ebi_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(ebi_rail_id);
    }

    //Now transition Mx if it's going down.
    if((railway.rail_state[mx_rail_id].constrained_target.microvolts<railway.rail_state[mx_rail_id].current_active.microvolts) ||
        (railway.rail_state[mx_rail_id].constrained_target.mode<railway.rail_state[mx_rail_id].current_active.mode))
    {
        railway_transition_individual_rail(mx_rail_id);
    }

    //Assertion that current_active == constrained_target for all rails.
    for(int i=0; i<RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        CORE_VERIFY(railway.rail_state[i].constrained_target.microvolts == railway.rail_state[i].current_active.microvolts);
        CORE_VERIFY(railway.rail_state[i].constrained_target.mode == railway.rail_state[i].current_active.mode);
    }

    //Assertion that MX >= logical rails
    CORE_VERIFY(railway.rail_state[mx_rail_id].current_active.mode>=railway.rail_state[cx_rail_id].current_active.mode);
}

static bool railway_transition_in_progress = false;

//Top level fn for transitioning rails.
void railway_transition_rails()
{
    CORE_VERIFY(!railway_transition_in_progress);    //We don't support this fn becoming re-entrant for now. Guard against it.
    railway_transition_in_progress = true;

    railway_do_transition_rails();
    
    railway_transition_in_progress = false;
}

void railway_transitioner_init(void)
{
    for(uint32 i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        initialize_rail(i);
    }
}

static void railway_set_pre_callback(railway_rail_state_t *rail_state, railway_cb_type type, railway_cb cb, void* cookie)
{
    const railway_internal_cb_type cb_type = railway_callback_remap[type];

    //Deal with special callbacks first.
    if(type!=RAILWAY_PRECHANGE_CB)
    {
        //If we ever extend this, implement another lookup to map the enum railway_cb_type to the array index.
        //For now, support the last entry only.
        CORE_VERIFY(type==RAILWAY_LAST_PRECHANGE_CB);
        
        CORE_VERIFY(rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn==NULL);
        rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn = cb;
        rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_cookie = cookie;
        return;
    }
    
    // If this check fails then we need to increase RAILWAY_MAX_CBS by 1.
    // The last CB is reserved for RAILWAY_LAST_PRECHANGE_CB, so check the "RAILWAY_MAX_CBS-2"-th entry
    CORE_VERIFY(!rail_state->cbs[cb_type][RAILWAY_MAX_CBS-2].callback_fn);
    
    // The last CB is reserved for RAILWAY_LAST_PRECHANGE_CB, so search up to the "RAILWAY_MAX_CBS-1"-th entry
    for(int i = 0; i < RAILWAY_MAX_CBS-2; ++i)
    {
        if(!rail_state->cbs[cb_type][i].callback_fn)
        {
            rail_state->cbs[cb_type][i].callback_fn = cb;
            rail_state->cbs[cb_type][i].callback_cookie = cookie;
            break;
        }
    }
}

static void railway_set_post_callback(railway_rail_state_t *rail_state, railway_cb_type type, railway_cb cb, void* cookie)
{
    const railway_internal_cb_type cb_type = railway_callback_remap[type];

    //Deal with special callbacks first.
    if(type!=RAILWAY_POSTCHANGE_CB)
    {
        //If we ever extend this, implement another lookup to map the enum railway_cb_type to the array index.
        //For now, support the 0th entry only.
        CORE_VERIFY(type==RAILWAY_FIRST_POSTCHANGE_CB);
        
        CORE_VERIFY(rail_state->cbs[cb_type][0].callback_fn==NULL);
        rail_state->cbs[cb_type][0].callback_fn = cb;
        rail_state->cbs[cb_type][0].callback_cookie = cookie;
        return;
    }

    // If this check fails then we need to increase RAILWAY_MAX_CBS by 1.
    CORE_VERIFY(!rail_state->cbs[cb_type][RAILWAY_MAX_CBS-1].callback_fn);
    
    //Skip the first entry as that's for RAILWAY_FIRST_POSTCHANGE_CB
    for(int i = 1; i < RAILWAY_MAX_CBS; ++i)
    {
        if(!rail_state->cbs[cb_type][i].callback_fn)
        {
            rail_state->cbs[cb_type][i].callback_fn = cb;
            rail_state->cbs[cb_type][i].callback_cookie = cookie;
            break;
        }
    }
}

void railway_set_callback(int rail, railway_cb_type type, railway_cb cb, void* cookie)
{
    CORE_VERIFY(type<RAILWAY_NUM_CB_TYPES);
    CORE_VERIFY(cb);
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);

    railway_rail_state_t *rail_state = &railway.rail_state[rail];
    const railway_internal_cb_type cb_type = railway_callback_remap[type];
    
    if(cb_type==RAILWAY_INTERNAL_PRECHANGE_CB)
    {
        railway_set_pre_callback(rail_state, type, cb, cookie);
    }
    else if(cb_type==RAILWAY_INTERNAL_POSTCHANGE_CB)
    {
        railway_set_post_callback(rail_state, type, cb, cookie);
    }
    else
    {
        CORE_VERIFY(0);
    }
}

