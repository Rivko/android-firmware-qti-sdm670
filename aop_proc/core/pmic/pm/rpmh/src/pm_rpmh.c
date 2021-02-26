/*! \file
*  
*  \brief  pm_rpmh.c ----This file contains driver functions for rpmh 
*                        communication 
*
* Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
* QUALCOMM Proprietary.  Export of this technology or software is regulated 
* by the U.S. Government. Diversion contrary to U.S. law prohibited.  
*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/rpmh/src/pm_rpmh.c#1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_rpmh.h"

/*===========================================================================

                     GLOBAL VARIABLES FOR MODULE 

===========================================================================*/

static rpmh_client_handle pm_rpmh_h = NULL; 

/*===========================================================================

                     FUNCTION DEFINITIONS 

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void 
pm_rpmh_init(void)
{
    uint32* rpmh_drv_id = NULL;
    
    //rpmh_client_init ();

    rpmh_drv_id = pm_target_information_get_specific_info(PM_PROP_RPMH_DRV_ID);
    CORE_VERIFY_PTR(rpmh_drv_id);
 
    pm_rpmh_h = rpmh_create_handle((RSCSW_DRV_MAPPING)*rpmh_drv_id, "pmic");
    
    if(pm_rpmh_h == NULL)
        CORE_VERIFY(0);
        //PM_LOG_MSG_ERR_FATAL("Unable to obtain rpmh handle");
        
    return;
}

rpmh_client_handle 
pm_rpmh_get_handle(void)
{
    return pm_rpmh_h;
}

void 
pm_rpmh_create_cmd_set(vrm_rsrc_vote_data *active_set, 
                       pm_rpmh_set_type *rsrc_rpmh_data,
                       vrm_rsrc_category rsrc_category,
                       uint32 rsrc_id,
                       vrm_settings rsrc_key,
                       boolean is_fnf)
{
    uint8 count = 0;
    if(rsrc_rpmh_data->dirty_active)
    {   
        count = active_set->num_votes;
        // abort if num commands is greater than num of supported vrm votes per resource
        if(count >= VRM_MAX_NUM_VOTES)
        {
            CORE_VERIFY(0);
        }
        active_set->rpmh_handle = pm_rpmh_h;
        active_set->rsrc_category = rsrc_category;
        active_set->vote_data[count].rsrc_id = rsrc_id;
        active_set->vote_data[count].setting = rsrc_key;
        active_set->vote_data[count].data = rsrc_rpmh_data->active_set;
        active_set->completion = (is_fnf) ? FALSE : TRUE;
        active_set->set_type = RPMH_SET_ACTIVE;
        active_set->num_votes += 1;
    }
   
    return;
}


void  
pm_rpmh_post_set(vrm_rsrc_vote_data *active_set,
                 uint32 *current_msg_id)
{
    uint32 msg_id = 0;

    //Post active set if commands are present
    if(active_set->num_votes > 0)
    {
        vrm_send_rpmh_vote (active_set, &msg_id);
    }

    if(msg_id > 0)
    {
        *current_msg_id = msg_id;
    }
    
    // Make number of votes 0 after sending the votes
    active_set->num_votes = 0;
    
    return;
}

void 
pm_rpmh_barrier_all(uint32 msg_id)
{
    if(msg_id)
        rpmh_barrier_all(pm_rpmh_h, msg_id);
    
    return;
}

void 
pm_rpmh_barrier_single(uint32 msg_id)
{
    if(msg_id)
        rpmh_barrier_single(pm_rpmh_h, msg_id);

    return;
}

void
pm_rpmh_register_completion_callback(rpmh_completion_cb callback)
{
    if(callback != NULL)
    {
        rpmh_client_register_completion_callback(pm_rpmh_h, callback);
    }

    return;
}

void 
pm_rpmh_notify_completion_all(uint32 msg_id)
{
    // No msg_id check here so that call back is always called (even for msg_id 0)
    rpmh_notify_completion_all(pm_rpmh_h, msg_id);
    
    return;
}

void 
pm_rpmh_notify_completion_single(uint32 msg_id)
{
    // No msg_id check here so that call back is always called (even for msg_id 0)
    rpmh_notify_completion_single(pm_rpmh_h, msg_id);

    return;
}

