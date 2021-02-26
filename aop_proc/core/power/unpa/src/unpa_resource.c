/*============================================================================
@file unpa_resource.c

Resource routines in the UNPA framework

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/

#include "unpa_internal.h"

/*----------------------------------------------------------------------------
 * Request pipeline
 * -------------------------------------------------------------------------*/

/**
 * <!-- unpa_update_resource -->
 * @brief Process the request from client
 */
void unpa_update_resource( unpa_client *client )
{
  unpa_resource *resource = client->resource;
  unpa_resource_state active_agg;
  unpa_resource_state old_sleep_state;
  uint32_t force_sync = 0;
  
  old_sleep_state = resource->sleep_state;
  
  active_agg = resource->definition->update_fcn( resource, client );
  
  if ( active_agg > resource->active_max )
  {
    active_agg = resource->active_max;
  }
  
  if ( resource->sleep_state > resource->active_max )
  {
    resource->sleep_state = resource->active_max;
  }
  
  if ( !( client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET ) && 
       resource->internal_attr & UNPA_RESOURCE_FLUX_STATE )
  {
    resource->internal_attr &= ~UNPA_RESOURCE_FLUX_STATE;
    force_sync = 1;
  }
  
  if ( unpa_resource_has_attribute( resource, 
                                    UNPA_RESOURCE_DRIVER_UNCONDITIONAL ) ||
       active_agg != resource->active_state ||
       old_sleep_state != resource->sleep_state || force_sync )
  {
    resource->active_state = 
      resource->definition->driver_fcn( resource, client, active_agg );

    // TODO: Can be optimized slightly
    if ( client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET )
    {
      resource->internal_attr |= UNPA_RESOURCE_FLUX_STATE;
      client->internal_attr |= UNPA_CLIENT_LAST_FNF;
    }
  }
    
  client->active_request = client->pending_request;
  
  //UNPA_LOG( &unpa.log, "\trequest processed (client: XXX) (active_state: XXX) (sleep_state: XXX)" );
  
  rex_leave_crit_sect( &resource->lock); 
}

/*----------------------------------------------------------------------------
 * UNPA provided update functions
 * -------------------------------------------------------------------------*/ 
 
/**
 * @brief Map client->type (one of REQUIRED, SUPPRESSIBLE or SLEEP_ONLY)
 * to index in the resource->agg_state array. Err-fatals if invoked with
 * any other client type.
 */
uint32_t unpa_get_agg_index( unpa_client *client )
{
  switch ( client->type )
  {
  case UNPA_CLIENT_REQUIRED: return UNPA_REQUIRED_INDEX;
  case UNPA_CLIENT_SUPPRESSIBLE: return UNPA_SUPPRESSIBLE_INDEX;
  case UNPA_CLIENT_SLEEP: return UNPA_SLEEP_INDEX;
  default: CORE_VERIFY( 0 );
  }
  
  return 0; /* unused; prevents warnings */
}


/**
 * @brief Returns a mask with client types supported by the given 
 * (UNPA-provided) update function
 */
uint32_t
unpa_get_supported_client_types( unpa_resource_update_fcn update_fcn )
{
  if ( update_fcn == unpa_min_update_fcn ||
       update_fcn == unpa_max_update_fcn ||
       update_fcn == unpa_sum_update_fcn ||
       update_fcn == unpa_binary_update_fcn )
  {
    return UNPA_CLIENT_REQUIRED|UNPA_CLIENT_SUPPRESSIBLE|UNPA_CLIENT_SLEEP;
  }
  
  return 0;
}

/** 
 * <!-- unpa_min_update_fcn -->
 * @brief Returns the minimum of all active non-zero requests or 0, if all
 * active_requests are 0.
 */
unpa_resource_state
unpa_min_update_fcn( unpa_resource *resource, unpa_client *client )
{
  uint32_t index;
  unpa_resource_state active_min, pending_min, request;
  
  switch ( client->type )
  {
  case UNPA_CLIENT_REQUIRED:
  case UNPA_CLIENT_SUPPRESSIBLE: 
  case UNPA_CLIENT_SLEEP:
    index = unpa_get_agg_index( client );
    
    pending_min = active_min = resource->agg_state[index];
    
    request = UNPA_PENDING_REQUEST(client).val;
    
    if ( request != 0 && ( pending_min == 0 || request < pending_min ) )
    {
      pending_min = request;
    }
    else if ( active_min == UNPA_ACTIVE_REQUEST(client).val )
    {
      /* Walk the client list for a new min */
      unpa_client *c = resource->clients;
      pending_min = request;
      
      do
      {
        if ( c != client && c->type == client->type )
        {
          request = UNPA_ACTIVE_REQUEST(c).val;
          if ( request != 0 && ( pending_min == 0 || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        c = c->next;
      } while ( c );
    }
    
    resource->agg_state[index] = pending_min;
    break;

  default:
    break;
  }
  
  /* Compute sleep_state */
  if ( resource->agg_state[UNPA_SLEEP_INDEX] == 0 )
  {
    resource->sleep_state = resource->agg_state[UNPA_REQUIRED_INDEX];
  }
  else if ( resource->agg_state[UNPA_REQUIRED_INDEX] == 0 )
  {
    resource->sleep_state = resource->agg_state[UNPA_SLEEP_INDEX];
  }
  else
  {
    resource->sleep_state = MIN( resource->agg_state[UNPA_REQUIRED_INDEX],
                                 resource->agg_state[UNPA_SLEEP_INDEX] );
  }
  
  /* Return active_agg */
  if ( resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] == 0 )
  {
    return resource->agg_state[UNPA_REQUIRED_INDEX];
  }
  
  if ( resource->agg_state[UNPA_REQUIRED_INDEX] == 0 )
  {
    return resource->agg_state[UNPA_SUPPRESSIBLE_INDEX];
  }
  
  return MIN( resource->agg_state[UNPA_REQUIRED_INDEX],
              resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] );  
}

/** 
 * <!-- unpa_max_update_fcn -->
 * @brief Returns the maximum of all active requests.
 */
unpa_resource_state
unpa_max_update_fcn( unpa_resource *resource, unpa_client *client )
{
  uint32_t index;
  unpa_resource_state active_max, pending_max, request;
  
  switch ( client->type )
  {
  case UNPA_CLIENT_REQUIRED:
  case UNPA_CLIENT_SUPPRESSIBLE: 
  case UNPA_CLIENT_SLEEP:
    index = unpa_get_agg_index( client );
    
    pending_max = active_max = resource->agg_state[index];
    
    request = UNPA_PENDING_REQUEST(client).val;
    
    if ( request > pending_max )
    {
      pending_max = request;
    }
    else if ( active_max == UNPA_ACTIVE_REQUEST(client).val )
    {
      /* Walk the client list for a new max */
      unpa_client *c = resource->clients;
      pending_max = request;
      
      do
      {
        if ( c != client && c->type == client->type )
        {
          request = UNPA_ACTIVE_REQUEST(c).val;
          if ( request > pending_max )
          {
            pending_max = request;
          }
        }
        c = c->next;
      } while ( c );
    }
    
    resource->agg_state[index] = pending_max;
    break;
    
  default:
    break;
  }
  
  /* Compute sleep_state */
  resource->sleep_state = MAX( resource->agg_state[UNPA_REQUIRED_INDEX],
                               resource->agg_state[UNPA_SLEEP_INDEX] );

  /* Return active_agg */
  return MAX( resource->agg_state[UNPA_REQUIRED_INDEX], 
              resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] );
}

/**
 * <!-- unpa_sum_update_fcn -->
 * @brief Returns the sum of all active requests.
 */
unpa_resource_state
unpa_sum_update_fcn( unpa_resource *resource, unpa_client *client )
{
  uint32_t index;

  switch ( client->type ) {
    case UNPA_CLIENT_REQUIRED:
    case UNPA_CLIENT_SUPPRESSIBLE:
    case UNPA_CLIENT_SLEEP:
        index = unpa_get_agg_index(client);
       
        resource->agg_state[index] -= (UNPA_ACTIVE_REQUEST(client).val);
        resource->agg_state[index] += (UNPA_PENDING_REQUEST(client).val);
        break;

    default:
        break;
  }

  /* Compute sleep_state */
  resource->sleep_state = resource->agg_state[UNPA_REQUIRED_INDEX] +
                            resource->agg_state[UNPA_SLEEP_INDEX];

  /* Return active_agg */
  return resource->agg_state[UNPA_REQUIRED_INDEX] + 
            resource->agg_state[UNPA_SUPPRESSIBLE_INDEX];
}

/**
 * <!-- unpa_binary_update_fcn -->
 * @brief Returns 1 if there is atleast one active non-zero request, or 0.
 * IMPORTANT: This is different from how the plugin/update_fcn is defined
 * in full NPA, where we return active_max if there is atleast one
 * non-zero vote. Modifying this for uNPA saves 2 words per resource.
 */
unpa_resource_state
unpa_binary_update_fcn( unpa_resource *resource, unpa_client *client )
{
  uint32_t index;
  
  switch ( client->type )
  {
  case UNPA_CLIENT_REQUIRED:
  case UNPA_CLIENT_SUPPRESSIBLE:
  case UNPA_CLIENT_SLEEP:
    index = unpa_get_agg_index( client );
    
    resource->agg_state[index] -= 
      ( UNPA_ACTIVE_REQUEST(client).val == 0 ? 0 : 1 );
    
    resource->agg_state[index] +=
      ( UNPA_PENDING_REQUEST(client).val == 0 ? 0 : 1 );
    break;
  default:
    break;
  }
  
  /* Compute sleep_state */
  resource->sleep_state = ( MAX( resource->agg_state[UNPA_REQUIRED_INDEX],
                                 resource->agg_state[UNPA_SLEEP_INDEX] ) > 0 );
  
  /* Return active_agg */
  return ( MAX( resource->agg_state[UNPA_REQUIRED_INDEX], 
                resource->agg_state[UNPA_SUPPRESSIBLE_INDEX] ) > 0 );
}