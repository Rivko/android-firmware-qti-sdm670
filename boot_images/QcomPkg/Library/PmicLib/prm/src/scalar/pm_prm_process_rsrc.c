/*! \file
 *  
 *  \brief  pm_prm_device_init.c ----This file contains initialization functions
 *                                   prm device layer
 *  
 * Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
 * QUALCOMM Proprietary.  Export of this technology or software is regulated 
 * by the U.S. Government. Diversion contrary to U.S. law prohibited.
 *
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/src/scalar/pm_prm_process_rsrc.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   rl      created
===========================================================================*/


#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "pm_prm_device_update.h"
#include "pm_prm_driver.h"
#include "pm_log_utils.h"
#include "pm_utils.h"
#include "CoreVerify.h"
#include "pm_rpmh.h"



#define PM_PRM_GET_RSRC_STATE(rsrc_type, rsrc, num_keys)                      \
  case rsrc_type:                                                             \
    if (n_rsrcs == 0) n_rsrcs = num_keys;                                     \
    data_ptr = (pm_prm_##rsrc##_data_type *)pam_data[index].rail_data;        \
    data_ptr = ((pm_prm_##rsrc##_data_type *)data_ptr + mode_id);             \
    state    =  pm_prm_get_##rsrc##_rsrc_state(client_dep_h, data_ptr);       \
    break
    
/*===========================================================================

                     FUNCTION DEFINITIONS 

===========================================================================*/

//Helper function to process dependencies and post device layer requests
uint32
pm_prm_process_rsrc (npa_client_handle   client,
                     npa_resource       *resource,
                     npa_resource_state  mode_id,
                     uint32              index, 
                     uint32              dep_idx,
                     uint32             *curr_msg_id)
{
  pm_prm_rsrc_data_type       *rsrc_key_data    = NULL;
  npa_client_handle            client_dep_h     = NULL;
  void                        *data_ptr         = NULL; 
  npa_resource                *res_lock         = NULL;
  pm_prm_pam_client_cfg_type  *pam_data         = NULL;
  uint8                        n_rsrcs = 0;
  npa_resource_state           state             = 0; 
  uint32                       dep_inc           = 0;
  boolean                      is_nas_req        = FALSE;
  rpmh_command_set_t           active_set; 
  rpmh_command_set_t           sleep_set;

  
  pam_data = (pm_prm_pam_client_cfg_type *) resource->definition->data;
  
  if(client->request_attr & NPA_REQUEST_NEXT_AWAKE) 
  {
    is_nas_req = TRUE;
  }
  
  DALSYS_memset(&active_set, 0, sizeof(rpmh_command_set_t));
  DALSYS_memset(&sleep_set,  0, sizeof(rpmh_command_set_t));

  /* Get initial dep to get a lock on the resource node */
  client_dep_h = NPA_DEPENDENCY( resource, dep_idx );

  res_lock = client_dep_h->resource;

  npa_resource_lock(res_lock);

  do
  {
    client_dep_h = NPA_DEPENDENCY( resource, dep_idx );
    
    rsrc_key_data = 
    (pm_prm_rsrc_data_type*)client_dep_h->resource->definition->data;
    
    switch(pam_data[index].rail_type)
    {
      PM_PRM_GET_RSRC_STATE(PM_PRM_RSRC_VRM_VREG, vreg, NUM_OF_VREG_KEYS);
      PM_PRM_GET_RSRC_STATE(PM_PRM_RSRC_VRM_CLK,  clk,  NUM_OF_CLK_KEYS);
      default:
        CORE_VERIFY(0);
    }
    
    rsrc_key_data->pam_req = TRUE;
 
    npa_pass_request_attributes( client, client_dep_h);
 
    npa_issue_scalar_request(client_dep_h, state);
 
    pm_rpmh_create_cmd_set(&active_set, &sleep_set, 
                           &rsrc_key_data->rpmh_data,
                           is_nas_req);
 
    /* Clear dirty active & sleep for PAM Req 
    *  after adding them to rpmh cmd set 
    */
    rsrc_key_data->rpmh_data.dirty_active=FALSE;
    rsrc_key_data->rpmh_data.dirty_sleep=FALSE;
 
    rsrc_key_data->pam_req = FALSE;
 
    dep_inc += DEP_OFFSET;
    dep_idx += DEP_OFFSET;
    n_rsrcs--;
 
  }while(n_rsrcs);
  
  pm_rpmh_post_set(&active_set, &sleep_set, &rsrc_key_data->rpmh_data, 
                   curr_msg_id, is_nas_req);
  
  npa_resource_unlock(res_lock);

  return dep_inc;
}
