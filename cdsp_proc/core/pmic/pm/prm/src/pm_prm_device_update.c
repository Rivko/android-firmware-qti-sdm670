/*! \file
*  
*  \brief  pm_prm_device_vreg.c ----This file contains driver functions for PMIC  
*                                  VREG PRM Layer 
*
* Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
* QUALCOMM Proprietary.  Export of this technology or software is regulated 
* by the U.S. Government. Diversion contrary to U.S. law prohibited.  
*  
*/
/*===========================================================================

            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: 
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   rl      Add Scalar and Vector Support for RPMh use PRM convention 
01/16/13   umr     Add Suppressible request support.
03/07/12   hs      Use DAl APIs to replace memory allocation. 
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/
/*===========================================================================

             INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_prm_device_update.h"
#include "pm_prm_driver.h"
#include "pm_ulog.h"
#include "pm_malloc.h"
#include "CoreVerify.h"
#include "pm_rpmh.h"


#define PM_PRM_VREG_POPULATE_KEY(key_type, key_offset)              \
  rsrc_vec_data->rpmh_data.offset = key_offset;                     \
  rsrc_vec_data->rpmh_data.active_set = keys_active->key_type;      \
  rsrc_vec_data->rpmh_data.sleep_set = keys_sleep->key_type;        \
  pm_rpmh_create_cmd_set(&active_set, &sleep_set,                   \
                         &rsrc_vec_data->rpmh_data,                 \
                         is_nas_req)



#define PM_PRM_CLK_POPULATE_KEY(key_type, key_offset)               \
  rsrc_vec_data->rpmh_data.offset = key_offset;                     \
  rsrc_vec_data->rpmh_data.active_set = keys_active->key_type;      \
  rsrc_vec_data->rpmh_data.sleep_set  = keys_sleep->key_type;       \
  pm_rpmh_create_cmd_set(&active_set, &sleep_set,                   \
                         &rsrc_vec_data->rpmh_data,                 \
                         is_nas_req)


/********************Update Functions for VREG***************************/

void
pm_prm_vreg_populate_sets(npa_resource *resource, boolean is_nas_req)
{
  pm_prm_rsrc_data_type  *rsrc_vec_data = NULL;
  pm_prm_vreg_data_type  *keys_active   = NULL;
  pm_prm_vreg_data_type  *keys_sleep    = NULL;
  rpmh_command_set_t      active_set; 
  rpmh_command_set_t      sleep_set;
  
  if(resource == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Resource = %d", resource);
  }
  
  DALSYS_memset(&active_set, 0, sizeof(rpmh_command_set_t));
  DALSYS_memset(&sleep_set,  0, sizeof(rpmh_command_set_t));
  
  keys_active   = (pm_prm_vreg_data_type*)resource->state_vector;
  keys_sleep    = (pm_prm_vreg_data_type*)resource->required_state_vector;
  rsrc_vec_data = (pm_prm_rsrc_data_type*)resource->definition->data;
    
  rsrc_vec_data->rpmh_data.dirty_active = TRUE;
  rsrc_vec_data->rpmh_data.dirty_sleep  = TRUE;
  
  PM_PRM_VREG_POPULATE_KEY(mode,     PM_VREG_MODE_OFFSET);
  PM_PRM_VREG_POPULATE_KEY(voltage,  PM_VREG_VOLTAGE_OFFSET);
  PM_PRM_VREG_POPULATE_KEY(headroom, PM_VREG_HEADROOM_OFFSET);
  PM_PRM_VREG_POPULATE_KEY(enable,   PM_VREG_ENABLE_OFFSET);
  
  pm_rpmh_post_set(&active_set, &sleep_set, &rsrc_vec_data->rpmh_data, 
                   &(rsrc_vec_data->rpmh_data.msg_id), is_nas_req);

  return; 
}


static void
pm_prm_vreg_aggregate(pm_prm_vreg_data_type *keys, 
                      pm_prm_vreg_data_type *request)
{
  if(keys == NULL || request == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("keys = %d, request = %d", keys, request);
  }

  keys->enable   = MAX(keys->enable,   request->enable);
  keys->mode     = MAX(keys->mode,     request->mode);
  keys->voltage  = MAX(keys->voltage,  request->voltage);
  keys->headroom = MAX(keys->headroom, request->headroom);
  return;
}


npa_resource_state 
pm_prm_vreg_update_fcn(npa_resource *resource, npa_client_handle client)
{
  npa_client_handle      active_client;
  npa_client_type        req_type;
  boolean                is_nas_req    = FALSE;
  pm_prm_vreg_data_type  *keys, *reqd_keys, *client_req;
  
  if(resource == NULL || client == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Resource = %d, client = %d", resource, client);
  }

  active_client = resource->clients;
  req_type      = client->type;  
  client_req = (pm_prm_vreg_data_type *)client->resource_data;
  
  DALSYS_memcpy(client_req, NPA_PENDING_REQUEST(client).pointer.vector, 
                sizeof(pm_prm_vreg_data_type));
  
  NPA_PENDING_REQUEST(client).pointer.vector = (npa_resource_state*)client_req;

  keys = (pm_prm_vreg_data_type *)resource->state_vector;
  
  DALSYS_memcpy(keys, client_req, sizeof(pm_prm_vreg_data_type));

  /* Reqd request Only*/
  reqd_keys = (pm_prm_vreg_data_type *)resource->required_state_vector;
  
  if (req_type == NPA_CLIENT_VECTOR)
  {
    DALSYS_memcpy(reqd_keys, client_req, sizeof(pm_prm_vreg_data_type));
  }

  /* Loop through all of the clients, aggregating state */
  while (active_client)
  {
    /* Find the saved request for this client.*/
    pm_prm_vreg_data_type *request = active_client->resource_data;
  
    // If this is the current client, we've already started with his values.
    if (client == active_client)
    {
      active_client = active_client->next;
      continue;
    }
  
    /* Aggregate for both Suppressable and Required */
    pm_prm_vreg_aggregate (keys, request);
  
    if (req_type == NPA_CLIENT_VECTOR && 

    active_client->type == NPA_CLIENT_VECTOR)
    {
      /* Aggregate only Required */
      pm_prm_vreg_aggregate (reqd_keys, request);
    }
  
    active_client = active_client->next;
  }
  
  is_nas_req = npa_request_has_attribute(client, NPA_REQUEST_NEXT_AWAKE);

  pm_prm_vreg_populate_sets(resource, is_nas_req);

  return keys->voltage; 
}


npa_resource_state
pm_prm_get_vreg_rsrc_state(npa_client_handle client_handle, 
                          pm_prm_vreg_data_type *pam_data)
{
  npa_resource_state    state      = 0;
  pm_prm_rsrc_data_type *rsrc_data = NULL;
  
  if(client_handle == NULL || pam_data == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Inputs");
  }
  
  rsrc_data = (pm_prm_rsrc_data_type*)client_handle->resource->definition->data;
  
  switch(rsrc_data->rsrc_key)
  {
    case PM_RSRC_KEY_MODE:
      state = pam_data->mode; 
      break;
      
    case PM_RSRC_KEY_VOLTAGE:
      state = pam_data->voltage;
      break;
      
    case PM_RSRC_KEY_HEADROOM:
      state = pam_data->headroom; 
      break;
      
    case PM_RSRC_KEY_ENABLE:
      state = pam_data->enable;
      break;
      
    default:
      PM_LOG_MSG_ERR_FATAL("Invalid RSRC Key provided for %s", 
                                  client_handle->resource_name);
  }
  return state; 
}



/*************************Update Functions for CLK***************************/
void
pm_prm_clk_populate_sets(npa_resource *resource, boolean is_nas_req)
{
  pm_prm_rsrc_data_type *rsrc_vec_data = NULL;
  pm_prm_clk_data_type  *keys_active   = NULL;
  pm_prm_clk_data_type  *keys_sleep    = NULL;
  rpmh_command_set_t    active_set; 
  rpmh_command_set_t    sleep_set;
  
  if(resource == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Resource = %d", resource);
  }

  DALSYS_memset(&active_set, 0, sizeof(rpmh_command_set_t));
  DALSYS_memset(&sleep_set,  0, sizeof(rpmh_command_set_t));

  keys_active   = (pm_prm_clk_data_type*)resource->state_vector;
  keys_sleep    = (pm_prm_clk_data_type*)resource->required_state_vector;
  rsrc_vec_data = (pm_prm_rsrc_data_type*)resource->definition->data;

  rsrc_vec_data->rpmh_data.dirty_active = TRUE;
  rsrc_vec_data->rpmh_data.dirty_sleep = TRUE;

  PM_PRM_CLK_POPULATE_KEY(enable, PM_CLK_ENABLE_OFFSET);
  
  pm_rpmh_post_set(&active_set, &sleep_set, &rsrc_vec_data->rpmh_data, 
                   &(rsrc_vec_data->rpmh_data.msg_id), is_nas_req);

  return; 
}


static void
pm_prm_clk_aggregate(pm_prm_clk_data_type *keys, 
                     pm_prm_clk_data_type *request)
{
  if(keys == NULL || request == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("keys = %d, request = %d", keys, request);
  }

  keys->enable   = MAX(keys->enable, request->enable);
  return;
}


npa_resource_state 
pm_prm_clk_update_fcn(npa_resource *resource, npa_client_handle client)
{
  npa_client_handle    active_client;
  npa_client_type      req_type;
  boolean              is_nas_req    = FALSE;
  pm_prm_clk_data_type *keys, *reqd_keys, *client_req;

  if(resource == NULL || client == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Resource = %d, client = %d", resource, client);
  }

  active_client = resource->clients;
  req_type      = client->type;
  client_req = (pm_prm_clk_data_type *)client->resource_data;
  
  DALSYS_memcpy(client_req, NPA_PENDING_REQUEST(client).pointer.vector, 
                sizeof(pm_prm_clk_data_type));
                
  NPA_PENDING_REQUEST(client).pointer.vector=(npa_resource_state*)client_req;

  keys = (pm_prm_clk_data_type *)resource->state_vector;
  
  DALSYS_memcpy(keys, client_req, sizeof(pm_prm_clk_data_type));

  /* Reqd request Only */
  reqd_keys = (pm_prm_clk_data_type *)resource->required_state_vector;
  
  if (req_type == NPA_CLIENT_VECTOR)
  {
    DALSYS_memcpy(reqd_keys, client_req, sizeof(pm_prm_clk_data_type));
  }

  /* Loop through all of the clients, aggregating state */ 
  while (active_client)
  {
    /* Find the saved request for this client. */
    pm_prm_clk_data_type *request = active_client->resource_data;

    /* If this is the current client, we've already started with his values. */
    if (client == active_client)
    {
      active_client = active_client->next;
      continue;
    }

    /* Aggregate for both Suppressable and Required */
    pm_prm_clk_aggregate (keys, request);

    if (req_type == NPA_CLIENT_VECTOR && 
        active_client->type == NPA_CLIENT_VECTOR)
    {
      /* Aggregate only Required */
      pm_prm_clk_aggregate (reqd_keys, request);
    }

    active_client = active_client->next;
  }
  
  is_nas_req = npa_request_has_attribute(client, NPA_REQUEST_NEXT_AWAKE);

  pm_prm_clk_populate_sets(resource, is_nas_req);

  return keys->enable;
}


npa_resource_state
pm_prm_get_clk_rsrc_state(npa_client_handle     client_handle, 
                          pm_prm_clk_data_type  *pam_data)
{
  npa_resource_state    state      = 0;
  pm_prm_rsrc_data_type *rsrc_data = NULL;
  
  if(client_handle == NULL || pam_data == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Invalid Inputs");
  }
  
  rsrc_data=(pm_prm_rsrc_data_type*)client_handle->resource->definition->data;
  
  switch(rsrc_data->rsrc_key)
  {
    case PM_RSRC_KEY_ENABLE:
      state = pam_data->enable;
      break;
      
    default:
      PM_LOG_MSG_ERR_FATAL("Invalid RSRC Key provided for %s", 
                           client_handle->resource_name);
  }
  return state; 
}


