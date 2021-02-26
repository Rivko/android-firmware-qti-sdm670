/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  This file contains PAM Layer driver functions for PRM

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/prm/src/pm_prm_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   rl      Created
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "pm_prm_device_update.h"
#include "pm_prm_driver.h"
#include "pm_target_information.h"
#include "pm_log_utils.h"
#include "pm_utils.h"
#include "CoreVerify.h"
#include "pm_rpmh.h"
#include "pm_prm_device_defs.h"
#include "pm_prm_stubs.h"

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

void pm_prm_nas_cb_scalar(void *npa_rsrc);
void pm_prm_nas_cb_vector(void *npa_rsrc);

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
/*
*  Driver function for vector and scalar requests for PAM Layer
*/
npa_resource_state
pm_prm_pam_driver_fcn (npa_resource      *resource,
                       npa_client_handle  client,
                       npa_resource_state mode_id)
{
  uint32             index          = 0; 
  uint32             dep            = 0;
  uint32             current_msg_id = 0; 
  npa_resource_state mode_id_req    = 0;

  if (client->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Initial state of resources to be set if any, Apart from device initial
     * states
    */
  } else
  {
    /* If requested state is greater than max, error fatal*/
    if(resource->request_state > resource->active_max)
    {
      PM_LOG_MSG_ERR_FATAL("Invalid ModeId %d resource %s by client %s",
                          resource->request_state, client->resource_name,
                          client->name);
    }
    
    /* Use the aggregation corresponding to the current client->type.
     * The overall agg isn't important here as much as the individual
     * REQUIRED and SUPPRESSIBLE aggregations, because we have 2 client
     * handles to the dependency and it will aggregate across these anyway.
     */
    if (client->type == NPA_CLIENT_SUPPRESSIBLE)
    {
      dep = 1;
      mode_id_req = NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource);
    }
    else if(client->type == NPA_CLIENT_REQUIRED)
    { 
      dep = 0;
      mode_id_req = NPA_RESOURCE_REQUIRED_REQUEST(resource);
    }
    else
    {
      PM_LOG_MSG_ERR_FATAL("Client Type Not Supported");
    }
    
    for (index = 0; dep < resource->node->dependency_count; index++)
    {
      dep += pm_prm_process_rsrc(client, resource, mode_id_req, 
                                 index, dep, &current_msg_id);
    }
    
    pm_prm_barrier_request(client, current_msg_id, FALSE);

    PM_LOG_MSG_INFO("Done Rsrc=%s, Client=%s, MId=%d, Type=0x%x, attr=0x%x",
                     client->resource_name, client->name, mode_id, 
                     client->type, client->request_attr);
  }
  return mode_id;
}

/*
*  Driver function scalar requests on Device Layer Resources
*/
npa_resource_state
pm_prm_scalar_driver_fcn (npa_resource      *resource,
                          npa_client_handle  client,
                          npa_resource_state state)
{

  pm_prm_rsrc_data_type *rsrc_key_data = NULL;
  uint32                msg_id         = 0;
  boolean               is_nas_req     = FALSE;
  rpmh_command_set_t    active_set; 
  rpmh_command_set_t    sleep_set;
 
  rsrc_key_data=(pm_prm_rsrc_data_type*)resource->definition->data;
  
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
    
    rsrc_key_data->rpmh_data.nas_req_cnt += 1; 
    
    npa_notify_next_awake(pm_prm_nas_cb_scalar, (void *)resource);
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
  if(rsrc_key_data->rpmh_data.nas_req_cnt > 0 &&
     is_nas_req != TRUE)
  {
    PM_LOG_MSG_INFO("Imm Req before NAS CB for %s by %s", 
                     client->resource_name, 
                     resource->definition->name);
                     
    /* Make nas request count zero for nas cb to ignore this rsrc */ 
    rsrc_key_data->rpmh_data.nas_req_cnt = 0;
  }
  
  if(client->type == NPA_CLIENT_REQUIRED || 
     client->type == NPA_CLIENT_SUPPRESSIBLE)
  {   
    rsrc_key_data->rpmh_data.active_set   = resource->request_state;
    rsrc_key_data->rpmh_data.sleep_set    = NPA_RESOURCE_REQUIRED_REQUEST(resource);
    rsrc_key_data->rpmh_data.dirty_active = TRUE;
    rsrc_key_data->rpmh_data.dirty_sleep  = TRUE;
  }
  else 
  {
    /** If initialization do nothing*/
    return state;
  }
  
  /* If direct resource request is made, send request to RPMh if Scalar config
   * For Vector config requests are sent to RPMh in device update fcn
   * Barrier for both scalar and vector configs
   */
  if(rsrc_key_data->pam_req == FALSE) 
  {
    pm_rpmh_create_cmd_set(&active_set, &sleep_set,
                           &rsrc_key_data->rpmh_data,
                           is_nas_req);
                           
    pm_rpmh_post_set(&active_set, &sleep_set, 
                     &rsrc_key_data->rpmh_data, 
                     &msg_id, is_nas_req);
    
    //Check if request if FNF 
    pm_prm_barrier_request(client, msg_id, TRUE);
    
    PM_LOG_MSG_INFO("Done Rsrc=%s, Client=%s, state=%d, Type=0x%x, attr=0x%x",
                     client->resource_name, client->name, state, 
                     client->type, client->request_attr);
  }
  
return state;
}

/*
*  Driver function Vector requests on Device Layer Resources
*/
npa_resource_state
pm_prm_vector_driver_fcn(npa_resource      *resource,
                         npa_client_handle  client,
                         npa_resource_state state)
{
  return state;
}                       


/* 
*  Vector plugin function to update device resource state 
*  and post request to RPMh
*/
npa_resource_state
pm_prm_update_vector(npa_resource *resource, 
                     npa_client_handle client)
{
  pm_prm_rsrc_data_type *rsrc_vec_data = NULL;
  npa_resource_state     data          = 0;
  boolean                is_nas_req    = FALSE;
  
  rsrc_vec_data=(pm_prm_rsrc_data_type*)resource->definition->data;
  
  /* For nas requests register call back and update nas count
   * NPA will call-back in LIFO manner, where the last request will
   * be called first, in call back rpmh sets will be updated to reflect 
   * current aggregation per npa
   */
  if(npa_request_has_attribute(client, NPA_REQUEST_NEXT_AWAKE))
  {    
    is_nas_req = TRUE;
    
    rsrc_vec_data->rpmh_data.nas_req_cnt += 1; 
    
    npa_notify_next_awake(pm_prm_nas_cb_vector, (void *)resource);
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
  if(rsrc_vec_data->rpmh_data.nas_req_cnt > 0 &&
     is_nas_req != TRUE)
  {
    /* Make nas request count zero for nas cb to ignore this rsrc */ 
    PM_LOG_MSG_INFO("Imm Req before NAS CB for %s by %s", 
                     client->resource_name, 
                     resource->definition->name);
                     
    rsrc_vec_data->rpmh_data.nas_req_cnt = 0;
  }
  
  switch(rsrc_vec_data->rsrc_vrm_type)
  {
    case PM_PRM_RSRC_VRM_VREG:
      data = pm_prm_vreg_update_fcn(resource, client);
      break;
    
    case PM_PRM_RSRC_VRM_CLK:
      data = pm_prm_clk_update_fcn(resource, client);
      break;
    
    default:
      CORE_VERIFY(0);
      break;
  }
  
  return data;
}

/* 
*  Vector plugin function to create device layer resources
*/
void 
pm_prm_create_vector(npa_client *client)
{
  npa_resource          *resource      = NULL;
  pm_prm_rsrc_data_type *rsrc_vec_data = NULL;
  npa_query_handle query_handle = NULL;

  CORE_VERIFY_PTR(client);

  query_handle = npa_create_query_handle(client->resource_name);

  CORE_VERIFY_PTR(query_handle);

  resource = npa_query_get_resource(query_handle);
 
  CORE_VERIFY_PTR(resource);
                     
  rsrc_vec_data = (pm_prm_rsrc_data_type*)resource->definition->data;

  CORE_VERIFY_PTR(rsrc_vec_data);
  
  switch(rsrc_vec_data->rsrc_vrm_type)
  {
    case PM_PRM_RSRC_VRM_VREG:
      
      pm_malloc(sizeof(pm_prm_vreg_data_type),
                (void **)&client->resource_data);
      
      DALSYS_memset(client->resource_data, 0, 
                    sizeof(pm_prm_vreg_data_type));
      break;
    
    case PM_PRM_RSRC_VRM_CLK:
      
      pm_malloc(sizeof(pm_prm_clk_data_type),
                (void **)&client->resource_data);
      
      DALSYS_memset(client->resource_data, 0, 
                    sizeof(pm_prm_clk_data_type));
      break;
          
    default:
      CORE_VERIFY(0);
      break;
  }
  return;
}

/* 
*  Vector plugin function to delete device layer resources
*/
void
pm_prm_destroy_vector(npa_client *client)
{
  DALSYS_Free(client->resource_data);
}


/* 
*  Vector plugin information structure
*/
npa_resource_plugin pm_prm_vector_plugin =
{
  pm_prm_update_vector,
  NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
  pm_prm_create_vector,
  pm_prm_destroy_vector
};


/*
 * Scalar NAS Call-back
 */
void pm_prm_nas_cb_scalar(void *npa_rsrc)
{
  npa_resource          *resource      = NULL; 
  pm_prm_rsrc_data_type *rsrc_key_data = NULL;
  uint32                 msg_id        = 0;
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
  
  rsrc_key_data=(pm_prm_rsrc_data_type*)resource->definition->data;
 
  if (rsrc_key_data->rpmh_data.nas_req_cnt == 0)
  {
    /* Un-lock Resource */
    npa_resource_unlock(resource);
    return; 
  }
  
  rsrc_key_data->rpmh_data.nas_req_cnt = 0; 
  
  /* Use active state here instead of requst state since active 
   *state is current aggregated value
   */
  rsrc_key_data->rpmh_data.active_set   = resource->active_state;
  rsrc_key_data->rpmh_data.sleep_set    = NPA_RESOURCE_REQUIRED_REQUEST(resource);
  rsrc_key_data->rpmh_data.dirty_active = TRUE;
  rsrc_key_data->rpmh_data.dirty_sleep  = TRUE;
  
  
  /*
   * Create command set based on the previous aggregated active 
   * and sleep set values in rpmh data for the resource, this should only have 
   * sleep set on RPMh drivr since active set is already posted as Next Awake 
   * and RPMh driver would have made it Active set after wake-up from sleep. 
   */
  pm_rpmh_create_cmd_set(&active_set, &sleep_set, &rsrc_key_data->rpmh_data, 0);
  
  /* MSG ID returned here will always be 0 as we will only be updating 
   * sleep set and active set would be short circuited at rpmh layer 
   */
  pm_rpmh_post_set(&active_set, &sleep_set, 
                   &rsrc_key_data->rpmh_data, 
                   &msg_id, 0);
                   
  PM_LOG_MSG_INFO("Done NAS_CB: rsrc=%s, AS=%d, SS=%d, msg_id=%d", 
                  resource->definition->name,
                  rsrc_key_data->rpmh_data.active_set,
                  rsrc_key_data->rpmh_data.sleep_set, msg_id);
  
  /* Un-lock resource */
  npa_resource_unlock(resource);                 
  return;  
}

/*
 * Vector NAS Call-back
 */

void pm_prm_nas_cb_vector(void *npa_rsrc)
{
  npa_resource          *resource      = NULL; 
  pm_prm_rsrc_data_type *rsrc_vec_data = NULL;
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

  rsrc_vec_data=(pm_prm_rsrc_data_type*)resource->definition->data;
 
  if (rsrc_vec_data->rpmh_data.nas_req_cnt == 0)
  {
    /* Un-lock Resource */
    npa_resource_unlock(resource);
    return; 
  }
  
  rsrc_vec_data->rpmh_data.nas_req_cnt = 0; 
  
  switch(rsrc_vec_data->rsrc_vrm_type)
  {
    case PM_PRM_RSRC_VRM_VREG:
      pm_prm_vreg_populate_sets(resource, FALSE);
      break;
      
    case PM_PRM_RSRC_VRM_CLK:
      pm_prm_clk_populate_sets(resource, FALSE);
      break;
      
    default:
      PM_LOG_MSG_ERR_FATAL("Invalid VRM Rsrc Type");
  }

  PM_LOG_MSG_INFO("Done NAS_CB: rsrc=%s", 
                  resource->definition->name);

  /* Un-lock Resource */
  npa_resource_unlock(resource);

  return;  
}


/* 
 *  Helper function for posting barrier requests to RPMh
 */
void
pm_prm_barrier_request(npa_client_handle client, 
                       uint32 msg_id, 
                       boolean is_single)
{
  if (msg_id == 0 || 
      npa_request_has_attribute(client, NPA_REQUEST_FIRE_AND_FORGET))
  {
    return;
  }
  
  if(is_single)
  {
    pm_rpmh_barrier_single(msg_id);
  }
  else 
  {
    pm_rpmh_barrier_all(msg_id);
  }
  
  return;  
}


npa_query_status 
pm_prm_query_fcn ( npa_resource *resource,
                   unsigned int query_id,
                   npa_query_type *query_result )
{
  uint32                     index             = 0;
  uint32                     dep               = 0;
  npa_resource_state         mode_id_req       = 0;
  pm_prm_pam_client_cfg_type *pam_data         = NULL;
  boolean                    rsrc_status       = FALSE;
  void                       *data_ptr         = NULL;
  npa_client_handle          client_handle_dep = NULL;
  
  if(resource==NULL || query_result==NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Inputs");
  }
  
  pam_data = (pm_prm_pam_client_cfg_type *) resource->definition->data;
  
  for (index = 0; dep < resource->node->dependency_count; index++)
  {
    client_handle_dep = NPA_DEPENDENCY( resource, dep );
    
    mode_id_req = resource->active_state;
    
    switch(pam_data[index].rail_type)
    {
      case PM_PRM_RSRC_VRM_VREG:                                            
        data_ptr    = (pm_prm_vreg_data_type *)pam_data[index].rail_data;    
        data_ptr    = ((pm_prm_vreg_data_type *)data_ptr + mode_id_req);     
        rsrc_status = pm_prm_get_vreg_status(client_handle_dep, data_ptr);
        dep        += (DEP_OFFSET * NUM_OF_VREG_KEYS);                             
        break;
          
      case PM_PRM_RSRC_VRM_CLK:                                            
        data_ptr    = (pm_prm_clk_data_type *)pam_data[index].rail_data;
        data_ptr    = ((pm_prm_clk_data_type *)data_ptr + mode_id_req);     
        rsrc_status = pm_prm_get_clk_status(client_handle_dep, data_ptr);
        dep        += (DEP_OFFSET * NUM_OF_CLK_KEYS);                             
        break;

      default:
        CORE_VERIFY(0);
    }
    
    query_result->data.value = (unsigned int)rsrc_status;
    
    if (rsrc_status==FALSE)
    {   
      break;
    }
  }
      
  return  NPA_QUERY_SUCCESS ;  
}


