/*! \file
 *  
 *  \brief  pm_xo_node.c ----This file contains initialization functions
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

$Header: //components/rel/core.qdsp6/2.1.c4/pmic/pm/xo/src/pm_xo_node.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/17   rl      created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "npa.h"
#include "npa_resource.h"
#include "npa_scheduler.h"
#include "cmd_db.h"
#include "pmapp_npa.h"
#include "pm_rpmh.h"
#include "pm_xo_node.h"
#include "pm_ulog.h"
#include "pm_prm_device.h"

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/


npa_resource_state
pm_xo_driver_fcn (npa_resource      *resource,
                  npa_client_handle  client,
                  npa_resource_state state);


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

extern boolean bPMICStub;

/*
* XO Resource Data
*/
pm_xo_rsrc_data_type pm_xo_data =
{    
  .cmd_db_name = "xo.lvl",        
};

/*
* XO NPA Resource Definiton
*/
npa_resource_definition pm_xo_rsrc = 
{
  .name = PMIC_NPA_GROUP_ID_XO,
  .units = "on_off", 
  .max = PMIC_NPA_MODE_ID_XO_MAX-1, 
  .plugin = &npa_max_plugin,
  .attributes = (NPA_RESOURCE_DEFAULT |  
                 NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST), 
  .data = &pm_xo_data,    
}; 

/*
* XO NPA Node Definiton
*/
npa_node_definition pm_xo_node = 
{
  .name = PMIC_NODE_XO, 
  .driver_fcn = pm_xo_driver_fcn, 
  .attributes = NPA_NODE_DEFAULT,
  .data = NULL, 
  .dependency_count = 0, 
  .dependencies = NULL, 
  .resource_count = 1, 
  .resources = &pm_xo_rsrc,
};

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

void pm_xo_nas_cb(void *npa_rsrc);

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

npa_resource_state
pm_xo_driver_fcn (npa_resource      *resource,
                  npa_client_handle  client,
                  npa_resource_state state)
{

  pm_xo_rsrc_data_type    *rsrc_data = NULL;
  pm_rpmh_data_type       *rpmh_data = NULL; 
  uint32                  msg_id     = 0;
  boolean                 is_nas_req = FALSE;
  rpmh_command_set_t      active_set; 
  rpmh_command_set_t      sleep_set;
  
  rsrc_data = (pm_xo_rsrc_data_type*)resource->definition->data;
  rpmh_data = &rsrc_data->rpmh_data;

  DALSYS_memset(&active_set, 0, sizeof(rpmh_command_set_t));
  DALSYS_memset(&sleep_set,  0, sizeof(rpmh_command_set_t));
  
  /* For nas requests register call back and update nas count
   * NPA will call-back in LIFO manner, where the last request will
   * be called first, in call back rpmh sets will be updated to reflect 
   * current aggregation per npa
   */
  if(npa_request_has_attribute(client, NPA_REQUEST_NEXT_AWAKE)) 
  {
    is_nas_req = TRUE;
    
    rpmh_data->nas_req_cnt += 1; 
    
    npa_notify_next_awake(pm_xo_nas_cb, (void *)resource);
  }

  /*  Immediate request is received but nas requests are still pending,
   *  it means that pmic got a request from IST context on wake-up before
   *  npa serviced the nas callback for the resource. In this case, 
   *  clear out the nas request flag and honor the immediate request since 
   *  the aggregation has changed. 
   *  No need to lock the resource here as its locked by npa already. 
   *  There is no chance of having concurrency issues since resource is 
   *  also locked in the nas callback function before doing any operations
   *  Note: Immediate requests cannot come before system has gone through 
   *        sleep-cycle as NAS requests are scheduled just before entering 
   *        sleep per design
   */
  if(rpmh_data->nas_req_cnt > 0 &&
     is_nas_req != TRUE)
  {
    PM_LOG_MSG_INFO("Imm Req before NAS CB for %s by %s", 
                     client->resource_name, 
                     resource->definition->name);

    /* Make nas request count zero for nas cb to ignore this rsrc */ 
    rpmh_data->nas_req_cnt = 0;
  }

  if(client->type == NPA_CLIENT_REQUIRED ||
     client->type == NPA_CLIENT_SUPPRESSIBLE)
  {   
    rpmh_data->active_set   = resource->request_state;
    rpmh_data->sleep_set    = NPA_RESOURCE_REQUIRED_REQUEST(resource);
    rpmh_data->dirty_active = TRUE;
    rpmh_data->dirty_sleep  = TRUE;
  }
  else 
  {
    //Retrun state during initialization call
    return state;
  }
  

  pm_rpmh_create_cmd_set(&active_set, &sleep_set, rpmh_data, is_nas_req);
                         
  pm_rpmh_post_set(&active_set, &sleep_set, rpmh_data, &msg_id, is_nas_req);

  if (!(msg_id == 0 || 
      npa_request_has_attribute(client, NPA_REQUEST_FIRE_AND_FORGET)))
  {
    pm_rpmh_barrier_single(msg_id);
  }

  PM_LOG_MSG_INFO("Done Rsrc=%s, Client=%s, state=%d, Type=0x%x, attr=0x%x",
                   client->resource_name, client->name, state, 
                   client->type, client->request_attr);
  
return state;
}

void
pm_xo_node_init (void)
{
  pm_xo_data.rpmh_data.address = cmd_db_query_addr(pm_xo_data.cmd_db_name);

  if(pm_xo_data.rpmh_data.address == 0 && bPMICStub == FALSE)
  {
    PM_LOG_MSG_ERR_FATAL("Unable to get VRM address for XO");
  }

  if (bPMICStub == TRUE)
  {
    npa_stub_resource(pm_xo_rsrc.name);
  }
  else
  {
    npa_define_node (&pm_xo_node, NULL, NULL);
  }

} /* end of pm_xo_node_init() */


/*
 * XO NAS Call-back
 */
void pm_xo_nas_cb(void *npa_rsrc)
{
  npa_resource          *resource    = NULL; 
  pm_xo_rsrc_data_type  *rsrc_data   = NULL;
  uint32                 msg_id      = 0;
  rpmh_command_set_t     active_set; 
  rpmh_command_set_t     sleep_set;
 
  DALSYS_memset(&active_set, 0, sizeof(rpmh_command_set_t));
  DALSYS_memset(&sleep_set,  0, sizeof(rpmh_command_set_t));
  
  if (npa_rsrc == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Resource in NAS Callback");
  }
  
  resource = (npa_resource*)npa_rsrc;
  
  /* Lock resource to protect against parallel on-going immediate reqeust */
  npa_resource_lock(resource);
  
  rsrc_data=(pm_xo_rsrc_data_type*)resource->definition->data;
 
  if (rsrc_data->rpmh_data.nas_req_cnt == 0)
  {
    /* Un-lock Resource */
    npa_resource_unlock(resource);
    return; 
  }
  
  rsrc_data->rpmh_data.nas_req_cnt = 0; 

  rsrc_data->rpmh_data.active_set   = resource->active_state;
  rsrc_data->rpmh_data.sleep_set    = NPA_RESOURCE_REQUIRED_REQUEST(resource);
  rsrc_data->rpmh_data.dirty_active = TRUE;
  rsrc_data->rpmh_data.dirty_sleep  = TRUE;
  
  
  /*
   * Create command set based on the previous aggregated active 
   * and sleep set values in rpmh data for the resource, this should only have 
   * sleep set as dirty since active set is already posted as Next Awake 
   * and RPMh driver would have made it Active set after wake-up from sleep. 
   */
  pm_rpmh_create_cmd_set(&active_set, &sleep_set, &rsrc_data->rpmh_data, 0);
  
  /* MSG ID returned here will always be 0 as we will only be updating 
   * sleep set and active set would be short circuited at rpmh layer 
   */
  pm_rpmh_post_set(&active_set, &sleep_set, 
                   &rsrc_data->rpmh_data, 
                   &msg_id, 0);

  PM_LOG_MSG_INFO("Done NAS_CB: rsrc=%s, AS=%d, SS=%d, msg_id=%d", 
                  resource->definition->name,
                  rsrc_data->rpmh_data.active_set,
                  rsrc_data->rpmh_data.sleep_set, msg_id); 

  /* Un-lock Resource */  
  npa_resource_unlock(resource);                  
                   
  return;  
}


