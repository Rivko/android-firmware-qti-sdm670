/*==============================================================================
  FILE:         unpa_resource.c
  
  OVERVIEW:     Resource routines in the UNPA framework

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/framework/unpa_resource.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "unpa_internal.h"

/*==============================================================================
                                  FUNCTIONS
 =============================================================================*/
/* 
 * unpa_updateResource 
 */
void unpa_updateResource(unpa_client *client)
{
  unpa_resource_state active_agg;
  unpa_resource_state old_sleep_state;
  unpa_resource       *resource = client->resource;

  old_sleep_state = resource->sleep_state;
  
  active_agg = resource->definition->update_fcn(resource, client);
  
  if (active_agg > resource->active_max)
  {
    active_agg = resource->active_max;
  }
  
  if (resource->sleep_state > resource->active_max)
  {
    resource->sleep_state = resource->active_max;
  }
  
  if (unpa_resourceHasAttribute(resource, UNPA_RESOURCE_DRIVER_UNCONDITIONAL) ||
      (active_agg != resource->active_state) ||
      (old_sleep_state != resource->sleep_state))
  {
    unpa_resource_state old_active_state = resource->active_state;

    resource->active_state = resource->definition->driver_fcn(resource, client, active_agg);
    client->active_request = client->pending_request;
    
    if (old_active_state != resource->active_state)
    {
      unpaEvent_dispatch(resource, UNPA_POST_CHANGE_EVENT, (void *)resource->active_state);
    }
  }
  else
  {
    client->active_request = client->pending_request;
  }
  
  //UNPA_LOG( &unpa.log, "\trequest processed (client: XXX) (active_state: XXX) (sleep_state: XXX)" );
  
  UNPA_MUTEX_UNLOCK(&resource->lock);
  return;
}

/* 
 * unpa_getAggIndex 
 */
uint32 unpa_getAggIndex(unpa_client *client)
{
  switch (client->type)
  {
    case UNPA_CLIENT_REQUIRED:
      return UNPA_REQUIRED_INDEX;

    case UNPA_CLIENT_SUPPRESSIBLE:
      return UNPA_SUPPRESSIBLE_INDEX;

    case UNPA_CLIENT_SLEEP:
      return UNPA_SLEEP_INDEX;

    default:
      CORE_VERIFY(0);
  }
  
  return 0; /* unused; prevents warnings */
}


/* 
 * unpa_getSupportedClientTypes 
 */
uint32 unpa_getSupportedClientTypes(unpa_resource_update_fcn updateFcn)
{
  if (updateFcn == unpa_minUpdateFcn || updateFcn == unpa_maxUpdateFcn ||
      updateFcn == unpa_binaryUpdateFcn || updateFcn == unpa_orUpdateFcn)
  {
    return UNPA_CLIENT_REQUIRED | UNPA_CLIENT_SUPPRESSIBLE | UNPA_CLIENT_SLEEP;
  }
  
  return 0;
}

/* 
 * unpa_minUpdateFcn 
 */
unpa_resource_state unpa_minUpdateFcn(unpa_resource *resource, unpa_client *client)
{
  uint32              index;
  unpa_resource_state active_min, pending_min, request;
  
  switch (client->type)
  {
    case UNPA_CLIENT_REQUIRED:
    case UNPA_CLIENT_SUPPRESSIBLE: 
    case UNPA_CLIENT_SLEEP:
    {
      index = unpa_getAggIndex(client);
      
      pending_min = active_min = resource->agg_state[index];
      
      request = UNPA_PENDING_REQUEST(client).val;
      
      if (request != 0 && (pending_min == 0 || request < pending_min))
      {
        pending_min = request;
      }
      else if (active_min == UNPA_ACTIVE_REQUEST(client).val)
      {
        /* Walk the client list for a new min */
        unpa_client *c  = resource->clients;
        pending_min     = request;
        
        do
        {
          if (c != client && c->type == client->type)
          {
            request = UNPA_ACTIVE_REQUEST(c).val;
            if (request != 0 && (pending_min == 0 || request < pending_min))
            {
              pending_min = request;
            }
          }
          c = c->next;
        } while(c);
      }
      
      resource->agg_state[index] = pending_min;
      break;
    }

    default:
      break;
  }
  
  /* Compute sleep_state */
  if (resource->agg_state[UNPA_SLEEP_INDEX] == 0)
  {
    resource->sleep_state = resource->agg_state[UNPA_REQUIRED_INDEX];
  }
  else if (resource->agg_state[UNPA_REQUIRED_INDEX] == 0)
  {
    resource->sleep_state = resource->agg_state[UNPA_SLEEP_INDEX];
  }
  else
  {
    resource->sleep_state = MIN(resource->agg_state[UNPA_REQUIRED_INDEX],
                                resource->agg_state[UNPA_SLEEP_INDEX]);
  }
  
  /* Return active_agg */
  if (resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] == 0)
  {
    return resource->agg_state[UNPA_REQUIRED_INDEX];
  }
  
  if (resource->agg_state[UNPA_REQUIRED_INDEX] == 0)
  {
    return resource->agg_state[UNPA_SUPPRESSIBLE_INDEX];
  }
  
  return MIN(resource->agg_state[UNPA_REQUIRED_INDEX],
             resource->agg_state[UNPA_SUPPRESSIBLE_INDEX]);
}

/* 
 * unpa_maxUpdateFcn
 */
unpa_resource_state unpa_maxUpdateFcn(unpa_resource *resource, unpa_client *client)
{
  uint32              index;
  unpa_resource_state active_max, pending_max, request;
  
  switch (client->type)
  {
    case UNPA_CLIENT_REQUIRED:
    case UNPA_CLIENT_SUPPRESSIBLE: 
    case UNPA_CLIENT_SLEEP:
    {
      index = unpa_getAggIndex(client);
      
      pending_max = active_max = resource->agg_state[index];
      
      request = UNPA_PENDING_REQUEST(client).val;
      
      if (request > pending_max)
      {
        pending_max = request;
      }
      else if (active_max == UNPA_ACTIVE_REQUEST(client).val)
      {
        /* Walk the client list for a new max */
        unpa_client *c  = resource->clients;
        pending_max     = request;
        
        do
        {
          if (c != client && c->type == client->type)
          {
            request = UNPA_ACTIVE_REQUEST(c).val;
            if (request > pending_max)
            {
              pending_max = request;
            }
          }
          c = c->next;
        } while(c);
      }
      
      resource->agg_state[index] = pending_max;
      break;
    }
    
    default:
      break;
  }
  
  /* Compute sleep_state */
  resource->sleep_state = MAX(resource->agg_state[UNPA_REQUIRED_INDEX],
                              resource->agg_state[UNPA_SLEEP_INDEX]);

  /* Return active_agg */
  return MAX(resource->agg_state[UNPA_REQUIRED_INDEX], 
             resource->agg_state[UNPA_SUPPRESSIBLE_INDEX]);
}

/* 
 * unpa_binaryUpdateFcn 
 */
unpa_resource_state unpa_binaryUpdateFcn(unpa_resource *resource, unpa_client *client)
{
  uint32 index;
  
  switch (client->type)
  {
    case UNPA_CLIENT_REQUIRED:
    case UNPA_CLIENT_SUPPRESSIBLE:
    case UNPA_CLIENT_SLEEP:
    {
      index = unpa_getAggIndex(client);
       
      resource->agg_state[index] -= (UNPA_ACTIVE_REQUEST(client).val == 0 ? 0 : 1);
      resource->agg_state[index] += (UNPA_PENDING_REQUEST(client).val == 0 ? 0 : 1);
      break;
    }

    default:
      break;
  }
  
  /* Compute sleep_state */
  resource->sleep_state = (MAX(resource->agg_state[UNPA_REQUIRED_INDEX],
                               resource->agg_state[UNPA_SLEEP_INDEX] ) > 0);
  
  /* Return active_agg */
  return (MAX(resource->agg_state[UNPA_REQUIRED_INDEX], 
              resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] ) > 0);
}

/* 
 * unpa_orUpdateFcn 
 */
unpa_resource_state unpa_orUpdateFcn(unpa_resource *resource, unpa_client *client)
{
  unpa_resource_state agg = UNPA_PENDING_REQUEST(client).val;
  unpa_client *c = resource->clients;
  
  switch (client->type)
  {
    case UNPA_CLIENT_REQUIRED:
    case UNPA_CLIENT_SUPPRESSIBLE:
    case UNPA_CLIENT_SLEEP:
    {
      while ( c )
      {
        if ( c != client && client->type == c->type )
        {
          agg |= UNPA_ACTIVE_REQUEST(c).val;
        }
        c = c->next;
      }
      
      resource->agg_state[unpa_getAggIndex(client)] = agg;
      break;
    }

    default:
      break;
  }
  
  /* Compute sleep_state */
  resource->sleep_state = resource->agg_state[UNPA_REQUIRED_INDEX] |
                          resource->agg_state[UNPA_SLEEP_INDEX];
  
  /* Return active_agg */
  return (resource->agg_state[UNPA_REQUIRED_INDEX] | 
          resource->agg_state[UNPA_SUPPRESSIBLE_INDEX]);
}

