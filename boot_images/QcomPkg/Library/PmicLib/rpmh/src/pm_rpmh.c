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

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/rpmh/src/pm_rpmh.c#1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "pm_target_information.h"
#include "pm_log_utils.h"
#include "pm_utils.h"
#include "CoreVerify.h"
#include "pm_rpmh.h"

/*===========================================================================

                     GLOBAL VARIABLES FOR MODULE 

===========================================================================*/

static rpmh_client_handle pm_rpmh_h = NULL; 



/*===========================================================================

                     FUNCTION DEFINITIONS 

===========================================================================*/

void 
pm_rpmh_init(void)
{
  uint32 rpmh_drv_id = 0;
  
  rpmh_drv_id = 
        pm_target_information_get_periph_count_info(PM_PROP_RPMH_DRV_ID, 0);
 
  pm_rpmh_h = rpmh_create_handle((RSCSW_DRV_MAPPING)rpmh_drv_id, "pmic");
  
  if(pm_rpmh_h == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Unable to obtain rpmh handle");
  }
  
  return;
}


rpmh_client_handle 
pm_rpmh_get_handle(void)
{
    return pm_rpmh_h;
}


void 
pm_rpmh_create_cmd_set(rpmh_command_set_t *active_set, 
                       rpmh_command_set_t *sleep_set,
                       pm_rpmh_data_type  *rpmh_data,
                       boolean             is_nas_req)
{
  uint8 count = 0;

  if(rpmh_data->dirty_active)
  {   
    count = active_set->num_commands;
    
    active_set->commands[count].address = rpmh_data->address + 
                                          rpmh_data->offset;
    
    active_set->commands[count].data = rpmh_data->active_set;
    
    active_set->commands[count].completion = is_nas_req ? FALSE : TRUE;
    
    active_set->set = is_nas_req ? RPMH_SET_WAKE : RPMH_SET_ACTIVE;
    
    active_set->num_commands += 1;
  }
  
  if(rpmh_data->dirty_sleep && is_nas_req==FALSE)
  {   
    count = sleep_set->num_commands;
    
    sleep_set->commands[count].address = rpmh_data->address + 
                                         rpmh_data->offset;
    
    sleep_set->commands[count].data = rpmh_data->sleep_set;
    
    sleep_set->commands[count].completion = FALSE;
    
    sleep_set->set = RPMH_SET_SLEEP;
    
    sleep_set->num_commands += 1;
  }
  
  return;
}


void  
pm_rpmh_post_set(rpmh_command_set_t *active_set, 
                 rpmh_command_set_t *sleep_set, 
                 pm_rpmh_data_type   *rpmh_data,
                 uint32             *curr_msg_id,
                 boolean             is_nas_req)
{
  uint32 msg_id=0;
  /* Post active set if commands are present for both
   * active and nas requests 
   */
  if(active_set->num_commands > 0)
  {
    msg_id = rpmh_issue_command_set(pm_rpmh_h, active_set);
  }
  
  /* Post sleep set if commands are present */
  if(sleep_set->num_commands > 0 && is_nas_req == FALSE)
  {
    rpmh_issue_command_set(pm_rpmh_h, sleep_set);
  }
    
  /* Update only if non-zero msg_id */
  if (msg_id)
  {
    *curr_msg_id = msg_id;
  }
  
  /* Make dirty keys of resource false after sending request */
  rpmh_data->dirty_active = FALSE;
  rpmh_data->dirty_sleep  = FALSE;

  active_set->num_commands = 0;
  sleep_set->num_commands  = 0; 
  
  return;
}

void 
pm_rpmh_barrier_all(uint32 msg_id)
{
  if(msg_id)
  {
    rpmh_barrier_all(pm_rpmh_h, msg_id);
  }
  
  return;
}

void 
pm_rpmh_barrier_single(uint32 msg_id)
{
  if(msg_id)
  {
    rpmh_barrier_single(pm_rpmh_h, msg_id);
  }
}

