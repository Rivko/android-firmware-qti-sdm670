/*! \file pm_prm_driver.c
*  
*  \brief This file contains PAM Layer driver functions for PRM.
*  \n  
*  \n &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/prm/src/pm_prm_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_prm_driver.h"
#include "pm_prm_device_defs.h"
#include "pm_debug.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************

static pm_prm_driver_info_type pm_prm_driver_info = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void 
pm_prm_driver_lock()
{
    pm_lock(&pm_prm_driver_info.lock);
    pm_prm_driver_info.lock_count++;
}

static void 
pm_prm_driver_unlock()
{
  pm_prm_driver_info.lock_count--;
  CORE_VERIFY(pm_prm_driver_info.lock_count >= 0);
  pm_unlock(&pm_prm_driver_info.lock);
}

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type 
pm_prm_driver_init()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS; 
         
    pm_debug_info_type *pmic_debug = pm_get_debug_info();
    CORE_VERIFY_PTR(pmic_debug);
    pmic_debug->pm_prm_driver_info = &pm_prm_driver_info;

    if (pm_prm_driver_info.initialized == FALSE)
    {

        // initialize the pmic app vrm lock
        pm_prm_driver_info.lock_count = 0;
        pm_lock_init(&pm_prm_driver_info.lock);

        pm_prm_driver_info.initialized = TRUE;
    }

    return err_flag;
}
/*
*  Driver function for vector and scalar requests for PAM Layer
*/
unpa_resource_state
pm_prm_pam_driver_fcn (unpa_resource *resource,
                       unpa_client *client, 
                       unpa_resource_state mode_id)
{
    uint32 index = 0;
    uint32 dep = 0;
    uint32 current_msg_id = 0; 
    pm_prm_node_resource_info* rsrc_data = (pm_prm_node_resource_info*)resource->user_data;

    pm_prm_pam_client_cfg_type *pam_data = NULL;
    pm_prm_npa_node_dependency *node_dep = NULL;
    pm_prm_rsrc_data_type *rsrc_key_data = NULL;
    unpa_client *client_handle_dep = NULL;
    pm_rpmh_set_type *rpmh_data_ptr = NULL;
    vrm_rsrc_vote_data *active_set_ptr = NULL;
    unpa_resource_state active_state = 0; 
    unpa_resource_state prev_active_state = 0;
    void *data_ptr = NULL; 
    void *active_data_ptr = NULL; 
    void *prev_active_data_ptr = NULL; 
    uint32 dep_inc = 0;
    uint32 num_of_node_rsrcs = 0;
    boolean is_fnf = FALSE;

    if (client->type == UNPA_CLIENT_INITIALIZE)
    {
        /*
         * Initial state of resources to be set if any, Apart from device initial
         * states
        */
        return mode_id;
    }

    // Only client type supported on AOP is REQUIRED
    if (client->type != UNPA_CLIENT_REQUIRED)
    {
        //abort
        CORE_VERIFY(0);
    }

    // enter crit section
    pm_prm_driver_lock();

    if ((client->request_attr & UNPA_REQUEST_ASYNC) && (client->done_cb != NULL))
    {
        pm_rpmh_register_completion_callback(client->done_cb);
    }

    // If fire and forget request, set the is_fnf flag to make completion FALSE requests
    is_fnf = (client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET) ? TRUE : FALSE;

    pam_data = (pm_prm_pam_client_cfg_type*)rsrc_data->resource_data;

    rpmh_data_ptr = &pm_prm_driver_info.rpmh_data;
    active_set_ptr = &pm_prm_driver_info.active_set;

    //Make all the vote data zero in active set
    memset(rpmh_data_ptr, 0, sizeof(pm_rpmh_set_type));
    memset(active_set_ptr, 0, sizeof(vrm_rsrc_vote_data));

    for (index = 0; dep < rsrc_data->dependency_count; index++)
    {
        num_of_node_rsrcs = 0;

        do
        {
            switch(pam_data[index].rsrc_type)
            {
               case PM_PRM_RSRC_VRM_VREG:
                 if (num_of_node_rsrcs == 0) num_of_node_rsrcs = NUM_OF_VREG_KEYS;
                 data_ptr = (pm_prm_vreg_data_type *)pam_data[index].rail_data;
                 active_data_ptr = ((pm_prm_vreg_data_type *)data_ptr + mode_id);

                 // if previous active state is invalid initial value, then it's the first request
                 // on the resource.
                 if (resource->active_state != PM_PRM_RSRC_INVALID_INIT_STATE)
                 {
                     prev_active_data_ptr = ((pm_prm_vreg_data_type *)data_ptr + resource->active_state);
                 }
                 break;
               default:
                 CORE_VERIFY(0);
            }

            node_dep = &rsrc_data->node_dependencies[dep];

            // All the resources need to be in same category (like REGULATOR or XOB) to combine 
            // multiple resource commands into the same command set. If a different rsrc category
            // is seen, send the previous resource category commands
            if ((active_set_ptr->num_votes > 0) && (node_dep->rsrc_category != active_set_ptr->rsrc_category))
            {
                pm_rpmh_post_set(active_set_ptr, &current_msg_id);
            }

            client_handle_dep = node_dep->client_handle;

            //if the device level resource setting is a shared resource then client_handle_dep is not NULL
            // and the request is sent through unpa to create it's RPMh command set.
            if (client_handle_dep != NULL)
            {
                rsrc_key_data = (pm_prm_rsrc_data_type*)client_handle_dep->resource->user_data;
                active_state = pm_prm_get_vreg_rsrc_state(rsrc_key_data->rsrc_key, active_data_ptr);
                rsrc_key_data->pam_req = TRUE;

                unpa_issue_request(client_handle_dep, active_state);

                pm_rpmh_create_cmd_set(active_set_ptr,
                                       &rsrc_key_data->rpmh_data,
                                       rsrc_key_data->rsrc_category,
                                       rsrc_key_data->rsrc_id,
                                       rsrc_key_data->rsrc_key,
                                       is_fnf);

                //Clear dirty active for PAM Req. 
                rsrc_key_data->rpmh_data.dirty_active=FALSE;
                rsrc_key_data->pam_req = FALSE;
            }
            else
            {
                active_state = pm_prm_get_vreg_rsrc_state(node_dep->rsrc_key, active_data_ptr);

                if (prev_active_data_ptr != NULL)
                {
                    prev_active_state = pm_prm_get_vreg_rsrc_state(node_dep->rsrc_key, prev_active_data_ptr);
                }

                // short-circuit the resource setting current request with the prev request
                // if there is a valid previous request
                if ((active_state != prev_active_state) || (prev_active_data_ptr == NULL))
                {
                   rpmh_data_ptr->active_set = active_state;
                   rpmh_data_ptr->dirty_active = TRUE;
                }

                pm_rpmh_create_cmd_set(active_set_ptr,
                                       rpmh_data_ptr,
                                       node_dep->rsrc_category,
                                       node_dep->rsrc_id,
                                       node_dep->rsrc_key,
                                       is_fnf);

                //Clear dirty active for PAM Req. 
                rpmh_data_ptr->dirty_active=FALSE;
            }

            // Send the multiple resource commands combined into the same command set when the
            // max command set limit is reached
            if (active_set_ptr->num_votes == VRM_MAX_NUM_VOTES)
            {
                pm_rpmh_post_set(active_set_ptr, &current_msg_id);
            }

            dep += DEP_OFFSET;
            num_of_node_rsrcs--;

        }while(num_of_node_rsrcs);
    }

    if (active_set_ptr->num_votes > 0)
    {
        pm_rpmh_post_set(active_set_ptr, &current_msg_id);
    }

    if ((client->request_attr & UNPA_REQUEST_ASYNC) && (client->done_cb != NULL))
    {
        pm_prm_notify_completion(current_msg_id, FALSE);
    }
    else
    {
        pm_prm_barrier_request(current_msg_id, FALSE);
    }

    // leave crit section
    pm_prm_driver_unlock();

    return mode_id;
}

/*
*  Driver function scalar requests on Device Layer Resources
*/
unpa_resource_state
pm_prm_scalar_driver_fcn (unpa_resource *resource,
                          unpa_client *client,
                          unpa_resource_state state)
{

    pm_prm_rsrc_data_type *rsrc_key_data = NULL;
    vrm_rsrc_vote_data active_set; 
    uint32 msg_id = 0;
    boolean is_fnf = FALSE;

    rsrc_key_data=(pm_prm_rsrc_data_type*)resource->user_data;
    
    //Make all the vote data zero in active set
    memset(&active_set, 0, sizeof(vrm_rsrc_vote_data));

    if (client->type == UNPA_CLIENT_INITIALIZE)
    {
        /** If initialization do nothing*/
        return state;
    }

    if(client->type == UNPA_CLIENT_REQUIRED)
    {   
        rsrc_key_data->rpmh_data.active_set = state;
        rsrc_key_data->rpmh_data.dirty_active = TRUE;
    }
    else 
    {
        //abort
        CORE_VERIFY(0);
    }
    
    //If direct resource request is made, send request to RPMh if Scalar config
    //For Vector config requests are sent to RPMh in individual device layer
    //Barrier for both scalar and vector configs
    if(rsrc_key_data->pam_req == FALSE) 
    {
        // If fire and forget request, set the is_fnf flag to make completion FALSE requests
        is_fnf = (client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET) ? TRUE : FALSE;

        pm_rpmh_create_cmd_set(&active_set,
                               &rsrc_key_data->rpmh_data,
                               rsrc_key_data->rsrc_category,
                               rsrc_key_data->rsrc_id,
                               rsrc_key_data->rsrc_key,
                               is_fnf);

        if ((client->request_attr & UNPA_REQUEST_ASYNC) && (client->done_cb != NULL))
        {
            pm_rpmh_register_completion_callback(client->done_cb);
        }
                               
        pm_rpmh_post_set(&active_set, &msg_id); 

        rsrc_key_data->rpmh_data.dirty_active=FALSE;

        if ((client->request_attr & UNPA_REQUEST_ASYNC) && (client->done_cb != NULL))
        {
            pm_prm_notify_completion(msg_id, TRUE);
        }
        else
        {
            pm_prm_barrier_request(msg_id, TRUE);
        }
    }
    
return state;
}

/* 
*  Helper function for posting barrier requests to RPMh
*/
void
pm_prm_barrier_request(uint32 msg_id, 
                       boolean is_single)
{
    if (msg_id != 0)
    {
        if(is_single)
            pm_rpmh_barrier_single(msg_id);
        else 
            pm_rpmh_barrier_all(msg_id);
    }    
}

/* 
*  Helper function for RPMh to notify completion for async requests
*/
void
pm_prm_notify_completion(uint32 msg_id,
                         boolean is_single)
{
    if (is_single) 
        pm_rpmh_notify_completion_single(msg_id);
    else
         pm_rpmh_notify_completion_all(msg_id);
}

