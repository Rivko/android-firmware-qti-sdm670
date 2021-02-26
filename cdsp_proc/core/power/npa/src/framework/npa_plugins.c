/*===========================================================================
@file npa_plugins.c

NPA standard plugin library

Copyright © 2009-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/framework/npa_plugins.c#1 $
============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"
#include "CoreString.h"

#ifdef WINSIM
#include "npa_offline.h"
#endif /* WINSIM */

#ifdef __cplusplus
extern "C" {
#endif

static NPA_INLINE
unsigned int _internal_index( npa_client_handle client )
{
  switch ( client->type )
  {
  case NPA_CLIENT_REQUIRED:      return NPA_REQUIRED_INDEX;
  case NPA_CLIENT_SUPPRESSIBLE:  return NPA_SUPPRESSIBLE_INDEX;
  case NPA_CLIENT_SUPPRESSIBLE2: return NPA_SUPPRESSIBLE2_INDEX;
  default: CORE_VERIFY( 0 );
  }
  return 0;
}

npa_resource_state npa_min_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client)
{
  npa_resource_state request, active_min, pending_min;
  unsigned int       index;

  switch ( client->type )
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
  case NPA_CLIENT_SUPPRESSIBLE2:
    index = _internal_index( client );
    pending_min = active_min = resource->internal_state[index];

    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;

    if ( request != 0 && ( pending_min == 0 || request < pending_min ) )
    {
      pending_min = request;
    } 
    else if ( active_min == NPA_ACTIVE_REQUEST(client).state &&
              !( client->request_attr & NPA_REQUEST_CHANGED_TYPE ) )
    {
      npa_client_handle c = resource->clients;
      pending_min = request;
    
      /* Scan the pending list for a new minimum */
      while ( c )
      {
        if ( c != client && client->type == c->type ) 
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( request != 0 && ( pending_min == 0 || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        c = c->next;
      }
    }
    resource->internal_state[index] = pending_min;

    /* If the client changed type and its active request is the resource's min
     * aggregation for the client's old type, we must re-aggregate across
     * clients of the old type to obtain a new min for that type.
     */
    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE &&
         resource->internal_state[index^1] == 
                                           NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle c = resource->clients;
      npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                           NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;
      
      pending_min = 0;
      /* Scan the pending list for a new minimum */
      while ( c )
      {
        if ( prev_type == c->type ) 
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( request != 0 && ( pending_min == 0 || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        c = c->next;
      }
      resource->internal_state[index^1] = pending_min;
    }
  default:
    break;
  }

  /* The idea below is to eliminate 0s from consideration as much as possible.
     If all 3 aggs are 0, return 0. Else, return the lowest non-zero value */
  
  if ( 0 == resource->internal_state[NPA_SUPPRESSIBLE_INDEX] )
  {
    resource->internal_state[NPA_SEMIACTIVE_INDEX] =
              resource->internal_state[NPA_REQUIRED_INDEX];
  }
  else if ( 0 == resource->internal_state[NPA_REQUIRED_INDEX] )
  {
    resource->internal_state[NPA_SEMIACTIVE_INDEX] =
              resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
  }
  else 
  {
    resource->internal_state[NPA_SEMIACTIVE_INDEX] =
              MIN( resource->internal_state[NPA_REQUIRED_INDEX],
                   resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
  }

  if ( 0 == resource->internal_state[NPA_SEMIACTIVE_INDEX] )
  {
    return resource->internal_state[NPA_SUPPRESSIBLE2_INDEX];
  }
  else if ( 0 == resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] )
  {
    return resource->internal_state[NPA_SEMIACTIVE_INDEX];
  }
  else
  {
    return MIN( resource->internal_state[NPA_SEMIACTIVE_INDEX],
                resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] );
  }
}

npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  npa_resource_state request, active_max, pending_max;
  unsigned int index;

  switch ( client->type )
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
  case NPA_CLIENT_SUPPRESSIBLE2:
    index = _internal_index( client );
    pending_max = active_max = resource->internal_state[index];
  
    NPA_REMAP_MAX_REQUEST( resource, client );

    request = NPA_PENDING_REQUEST(client).state;
    if ( request > pending_max )
    {
      pending_max = request;
    } 
    else if ( active_max == NPA_ACTIVE_REQUEST(client).state &&
              !( client->request_attr & NPA_REQUEST_CHANGED_TYPE ) )
    {
      npa_client_handle c = resource->clients;
      pending_max = NPA_PENDING_REQUEST(client).state;
    
      /* Scan the pending list for a new maximum */
      while ( c )
      {
        if ( client != c && client->type == c->type )
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( request > pending_max )
          {
            pending_max = request;
          }
        }
        c = c->next;
      }
    }
    resource->internal_state[index] = pending_max;

    /* If the client changed type and its active request is the resource's max
     * aggregation for the client's old type, we must re-aggregate across
     * clients of the old type to obtain a new max for that type.
     */
    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE &&
         resource->internal_state[index^1] == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle c = resource->clients;
      npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                           NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;

      pending_max = 0;
      /* Scan the pending list for a new maximum */
      while ( c )
      {
        if ( prev_type == c->type ) 
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( request > pending_max )
          {
            pending_max = request;
          }
        }
        c = c->next;
      }
      resource->internal_state[index^1] = pending_max;
    }
    break;
  default:
    break;
  }

  resource->internal_state[NPA_SEMIACTIVE_INDEX] = 
            MAX( resource->internal_state[NPA_REQUIRED_INDEX],
                 resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );

  return MAX ( resource->internal_state[NPA_SEMIACTIVE_INDEX],
               resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] );
}

npa_resource_state npa_sum_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  unsigned int index;

  switch ( client->type ) 
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
  case NPA_CLIENT_SUPPRESSIBLE2:
    index = _internal_index( client );

    NPA_REMAP_MAX_REQUEST( resource, client );

    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
    {
      resource->internal_state[index^1] -= NPA_ACTIVE_REQUEST(client).state;
    }
    else
    {
      resource->internal_state[index] -= NPA_ACTIVE_REQUEST(client).state;
    }
    resource->internal_state[index] += NPA_PENDING_REQUEST(client).state;
    break;
  default:
    break;
  }

  resource->internal_state[NPA_SEMIACTIVE_INDEX] = 
            resource->internal_state[NPA_REQUIRED_INDEX] + 
            resource->internal_state[NPA_SUPPRESSIBLE_INDEX];

  return resource->internal_state[NPA_SEMIACTIVE_INDEX] +
         resource->internal_state[NPA_SUPPRESSIBLE2_INDEX];
}

npa_resource_state npa_binary_update_fcn( npa_resource      *resource,
                                          npa_client_handle  client )
{
  unsigned int index;

  switch ( client->type ) 
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
  case NPA_CLIENT_SUPPRESSIBLE2:
    index = _internal_index( client );

    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
    {
      resource->internal_state[(index^1)+4] -= 
        ( NPA_ACTIVE_REQUEST(client).state == 0 ) ? 0 : 1;
      resource->internal_state[index^1] =
        ( resource->internal_state[(index^1)+4] ) ? resource->active_max : 0;
    }
    else
    {
      resource->internal_state[index+4] -= 
        ( NPA_ACTIVE_REQUEST(client).state == 0 ) ? 0 : 1;
    }

    resource->internal_state[index+4] += 
      ( NPA_PENDING_REQUEST(client).state == 0 ) ? 0 : 1;

    resource->internal_state[index] =
      ( resource->internal_state[index+4] ) ? resource->active_max : 0;
    break;
  default:
    break;
  }

  resource->internal_state[NPA_SEMIACTIVE_INDEX] =
            MAX( resource->internal_state[NPA_REQUIRED_INDEX],
                 resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );

  return MAX( resource->internal_state[NPA_SEMIACTIVE_INDEX],
              resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] );
}

npa_resource_state npa_or_update_fcn( npa_resource       *resource,
                                      npa_client_handle   client)
{
  npa_resource_state request = NPA_PENDING_REQUEST(client).state;
  npa_client_handle  c  = resource->clients;
  unsigned int       index   = _internal_index( client );

  switch ( client->type ) 
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
  case NPA_CLIENT_SUPPRESSIBLE2:
    while ( c )
    {
      if ( c != client && client->type == c->type )
      {
        request |= NPA_ACTIVE_REQUEST(c).state;
      }
      c = c->next;
    }
    resource->internal_state[index] = request;

    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
    {
      npa_client_type prev_type = 
        ( client->type == NPA_CLIENT_REQUIRED ) ? NPA_CLIENT_SUPPRESSIBLE : 
                                                  NPA_CLIENT_REQUIRED;
      c  = resource->clients;
      request = 0;
      while ( c )
      {
        if ( prev_type == c->type ) 
        {
          request |= NPA_ACTIVE_REQUEST(c).state;
        }
        c = c->next;
      }
      resource->internal_state[index^1] = request;
    }
    break;
  default:
    break;
  }

  resource->internal_state[NPA_SEMIACTIVE_INDEX] = 
            resource->internal_state[NPA_REQUIRED_INDEX] | 
            resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
  
  return ( resource->internal_state[NPA_SEMIACTIVE_INDEX] |
           resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] );
}

npa_resource_state npa_binary_and_update_fcn( npa_resource       *resource,
                                              npa_client_handle   client)
{
  npa_resource_state request = NPA_PENDING_REQUEST(client).state;
  npa_client_handle c  = resource->clients;
  unsigned int      index = _internal_index( client );
    
  /* Initialize internal aggregations if this is the first request to
     the resource */
  if ( resource->sequence == 0x0 )
  {
    resource->internal_state[NPA_REQUIRED_INDEX] = 0x1;
    resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = 0x1;
    resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] = 0x1;
  }

  switch ( client->type ) 
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
  case NPA_CLIENT_SUPPRESSIBLE2:
    while ( c )
    {
      if ( c != client && c->type == client->type && c->sequence != 0 )
      {
        request &= ( NPA_ACTIVE_REQUEST(c).state == 0 ) ? 0 : 1;
      }
      c = c->next;
    }
    resource->internal_state[index] = request;

    if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
    {
      npa_client_type prev_type = 
        ( client->type == NPA_CLIENT_REQUIRED ) ? NPA_CLIENT_SUPPRESSIBLE : 
                                                  NPA_CLIENT_REQUIRED;
      c = resource->clients;
      request = 0x1;
      while ( c )
      {
        if ( prev_type == c->type ) 
        {
          request &= ( NPA_ACTIVE_REQUEST(c).state == 0 ) ? 0 : 1;
        }
        c = c->next;
      }
      resource->internal_state[index^1] = request;
    }
    break;
  default:
    break;
  }

  resource->internal_state[NPA_SEMIACTIVE_INDEX] =
            resource->internal_state[NPA_REQUIRED_INDEX] &
            resource->internal_state[NPA_SUPPRESSIBLE_INDEX];

  /* Need the below check in this particular update function as it relies
     on internal_state fields being initialized to 0x1 */
  if ( _npa_resource_has_attribute( resource,
                                    NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE2 ) )
  {
    return ( resource->internal_state[NPA_SEMIACTIVE_INDEX] &
             resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] );
  }

  /* Else, return the SEMIACTIVE aggregation */
  return resource->internal_state[NPA_SEMIACTIVE_INDEX];
}

npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                            npa_client_handle  client )
{
  npa_resource_state request = NPA_PENDING_REQUEST(client).state;

  if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) &&
       ( client->type == NPA_CLIENT_VECTOR ||
         client->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR ||
         client->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR ) )
  {
    unsigned int vector_size;

    CORE_VERIFY( 
         NPA_PENDING_REQUEST(client).state <= resource->definition->max );

    resource->active_max = NPA_PENDING_REQUEST(client).state;
    resource->request_state = NPA_PENDING_REQUEST(client).state;

    vector_size = resource->active_max * sizeof( npa_resource_state );

    switch ( client->type )
    {
    case NPA_CLIENT_VECTOR:
      core_memcpy( resource->required_state_vector, vector_size,
                   NPA_PENDING_REQUEST(client).pointer.vector, vector_size );
      resource->state_vector = \
        resource->semiactive_state_vector = resource->required_state_vector;
      break;
    case NPA_CLIENT_SUPPRESSIBLE_VECTOR:
      core_memcpy( resource->suppressible_state_vector, vector_size,
                   NPA_PENDING_REQUEST(client).pointer.vector, vector_size );
      resource->state_vector = \
        resource->semiactive_state_vector = resource->suppressible_state_vector;
      break;
    case NPA_CLIENT_SUPPRESSIBLE2_VECTOR:
      core_memcpy( resource->suppressible2_state_vector, vector_size,
                   NPA_PENDING_REQUEST(client).pointer.vector, vector_size );
      /* SUPPRESSIBLE2 requests don't affect the semi_active aggregation */
      resource->state_vector = resource->suppressible2_state_vector;
      break;
    default:
      break;
    }

    return resource->request_state;
  }
  else if ( client->type == NPA_CLIENT_REQUIRED )
  {
    resource->internal_state[NPA_SEMIACTIVE_INDEX] =
      resource->internal_state[NPA_REQUIRED_INDEX] = request;
  }
  else if ( client->type == NPA_CLIENT_SUPPRESSIBLE )
  {
    resource->internal_state[NPA_SEMIACTIVE_INDEX] =
      resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = request;
  }
  else if ( client->type == NPA_CLIENT_SUPPRESSIBLE2 )
  {
    /* SUPPRESSIBLE2 requests don't affect the SEMIACTIVE aggregation */
    resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] = request;
  }

  return request;
}

npa_resource_state npa_always_on_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  resource->internal_state[NPA_REQUIRED_INDEX] = \
    resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = \
      resource->internal_state[NPA_SUPPRESSIBLE2_INDEX] = \
        resource->internal_state[NPA_SEMIACTIVE_INDEX] = resource->active_max;
  return resource->active_max;
}

npa_resource_state npa_impulse_update_fcn( npa_resource      *resource,
                                           npa_client_handle  client )
{
  /* No aggregation */
  return resource->request_state;
}

npa_resource_state npa_no_client_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  /* This function should never be invoked */
  CORE_VERIFY( 0 );
  return 0;
}

/*
  NPA plugin library
*/

const npa_resource_plugin npa_binary_plugin =
{
  npa_binary_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_min_plugin = 
{
  npa_min_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_sum_plugin = 
{
  npa_sum_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_identity_plugin = 
{
  npa_identity_update_fcn,
  0xffffffff,                   /* All client types supported */
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_always_on_plugin = 
{
  npa_always_on_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};


const npa_resource_plugin npa_impulse_plugin = 
{
  npa_impulse_update_fcn,
  /* Supported client types */
  NPA_CLIENT_IMPULSE,
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_or_plugin = 
{
  npa_or_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_binary_and_plugin = 
{
  npa_binary_and_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE | NPA_CLIENT_SUPPRESSIBLE2, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

/* Resource supports no clients - used for markers */
static void npa_no_client_create_fcn ( npa_client *client )
{
  client->resource = NULL; /* Signal that this client can't be created */
  client->resource_data = 0;    /* Set error code */
}

const npa_resource_plugin npa_no_client_plugin =
{
  npa_no_client_update_fcn,
  /* Supported client types */
  0,
  npa_no_client_create_fcn,     /* Create client function */
  NULL                          /* Destroy client function */
};

#ifdef __cplusplus
}
#endif
