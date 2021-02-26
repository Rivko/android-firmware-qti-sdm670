/*===========================================================================
@file npa_plugins.c

NPA standard plugin library

Copyright © 2009-2014 QUALCOMM Technologies Incorporated
All Rights Reserved.
QUALCOMM Proprietary/GTDR

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

/*
 * IMPORTANT: If you add a new update function to this file and that function
 * supports SUPPRESSIBLE requests, make sure that you add the function to
 * the npa_update_fcn_list in npa_graph.c
 */

static NPA_INLINE
unsigned int _internal_index( npa_client_handle client )
{
  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:     return NPA_REQUIRED_INDEX;
  case NPA_CLIENT_SUPPRESSIBLE: return NPA_SUPPRESSIBLE_INDEX;
  default: CORE_VERIFY( 0 );
  }
  return 0;
}

npa_resource_state npa_min_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client)
{
  npa_resource_state   request, active_min, pending_min;
  unsigned int         index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
    index = _internal_index( client );
    pending_min = active_min = resource->internal_state[index];

    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;

    if ( ( request != 0 ) &&
         ( ( pending_min == 0 ) || request < pending_min ) )
    {
      pending_min = request;
    } 
    else if ( active_min == NPA_ACTIVE_REQUEST(client).state &&
              !( client->request_attr & NPA_REQUEST_CHANGED_TYPE ) )
    {
      npa_client_handle c = resource->clients;
      pending_min = request;
    
      /* Scan the pending list for a new minimum */
      while ( NULL != c )
      {
        if ( c != client && client->type == c->type ) 
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( ( request != 0 ) &&
               ( ( pending_min == 0 ) || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        c = c->next;
      }
    }
    resource->internal_state[index] = pending_min;

    /* If the client changed type and its active request is the resource's
     * min_aggregation for the client's previous type, we must re-aggregate
     * across clients of the old type to determine a new min_aggregation for
     * that type.
     */
    if ( ( client->request_attr & NPA_REQUEST_CHANGED_TYPE ) &&
        resource->internal_state[index^1] == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle c = resource->clients;
      npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                           NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;
      
      pending_min = 0;
      /* Scan the pending list for a new minimum */
      while ( NULL != c )
      {
        if ( prev_type == c->type ) 
        {
          request = NPA_ACTIVE_REQUEST(c).state;
          if ( ( request != 0 ) &&
               ( ( pending_min == 0 ) || request < pending_min ) )
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

  if ( 0 == resource->internal_state[NPA_SUPPRESSIBLE_INDEX] )
  {
    return resource->internal_state[NPA_REQUIRED_INDEX];
  }
  else if ( 0 == resource->internal_state[NPA_REQUIRED_INDEX] )
  {
    return resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
  }
  else 
  {
    return MIN( resource->internal_state[NPA_REQUIRED_INDEX],
                resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
  }
}

npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  npa_resource_state request, active_max, pending_max;
  unsigned int index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
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
      while ( NULL != c )
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

    /* If the client changed type and its active request is the resource's
     * min_aggregation for the client's previous type, we must re-aggregate
     * across clients of the old type to determine a new min_aggregation for
     * that type.
     */
    if ( ( client->request_attr & NPA_REQUEST_CHANGED_TYPE ) &&
        resource->internal_state[index^1] == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle c = resource->clients;
      npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                           NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;

      pending_max = 0;
      /* Scan the pending list for a new maximum */
      while ( NULL != c )
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

  return MAX( resource->internal_state[NPA_REQUIRED_INDEX],
              resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_sum_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      NPA_REMAP_MAX_REQUEST( resource, client );
      if( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
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
  }
  return resource->internal_state[NPA_REQUIRED_INDEX] +
         resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
}

npa_resource_state npa_binary_update_fcn( npa_resource      *resource,
                                          npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
      {
        resource->internal_state[(index^1)+2] -= 
          ( NPA_ACTIVE_REQUEST(client).state == 0 ) ? 0 : 1;
        resource->internal_state[index^1] = 
          ( resource->internal_state[(index^1)+2] ) ? resource->active_max : 0;
      }
      else
      {
        resource->internal_state[index+2] -= 
          ( NPA_ACTIVE_REQUEST(client).state == 0 ) ? 0 : 1;
      }
      resource->internal_state[index+2] += 
        ( NPA_PENDING_REQUEST(client).state == 0 ) ? 0 : 1;
      resource->internal_state[index] = 
        ( resource->internal_state[index+2] ) ? resource->active_max : 0;
      break;
    default:
      break;
    }
  }
  
  return MAX( resource->internal_state[NPA_REQUIRED_INDEX],
              resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_or_update_fcn( npa_resource       *resource,
                                      npa_client_handle   client)
{
  if ( NULL != client )
  {
    npa_resource_state request = NPA_PENDING_REQUEST(client).state;
    npa_client_handle  c  = resource->clients;
    unsigned int       index   = _internal_index( client );

    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      while ( NULL != c )
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
        npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                             NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;
        c  = resource->clients;
        request = 0;
        while ( NULL != c )
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
  }
  
  return ( resource->internal_state[NPA_REQUIRED_INDEX] |
           resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_binary_and_update_fcn( npa_resource       *resource,
                                              npa_client_handle   client)
{
  if ( NULL != client )
  {
    npa_resource_state request = NPA_PENDING_REQUEST(client).state;
    npa_client_handle  c  = resource->clients;
    unsigned int       index   = _internal_index( client );
    
    /* Initialize internal aggregations upon first request to this
       update function */
    if ( resource->sequence == 0x0 )
    {
      resource->internal_state[NPA_REQUIRED_INDEX] = 0x1;
      resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = 0x1;
    }

    switch ( client->type ) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      while ( NULL != c )
      {
        if ( c != client && client->type == c->type )
        {
          request &= ( NPA_ACTIVE_REQUEST(c).state == 0 ) ? 0 : 1;
        }
        c = c->next;
      }
      resource->internal_state[index] = request;

      if ( client->request_attr & NPA_REQUEST_CHANGED_TYPE )
      {
        npa_client_type prev_type = ( client->type == NPA_CLIENT_REQUIRED ) ?
                             NPA_CLIENT_SUPPRESSIBLE : NPA_CLIENT_REQUIRED;
        c  = resource->clients;
        request = 0;
        while ( NULL != c )
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
  }
  
  return ( resource->internal_state[NPA_REQUIRED_INDEX] &
           resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                            npa_client_handle  client )
{
  if ( client->type & NPA_CLIENT_VECTOR &&
       npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int vector_size;

    CORE_VERIFY( 
         NPA_PENDING_REQUEST(client).state <= resource->definition->max );
    resource->active_max = NPA_PENDING_REQUEST(client).state;
    
    resource->request_state = NPA_PENDING_REQUEST(client).state;
    vector_size = resource->request_state * sizeof( npa_resource_state );
      
    core_memcpy( resource->state_vector, sizeof(*(resource->state_vector)),
      NPA_PENDING_REQUEST(client).pointer.vector,
      vector_size );

    /* If the resource supports suppressible requests, also copy the request
       into the appropriate buffer */
    if ( client->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR )
    {
      if ( npa_resource_has_attribute( resource, 
                            NPA_RESOURCE_ALLOC_SUPPRESSIBLE_VECTOR ) )
      {
        core_memcpy( resource->suppressible_state_vector, sizeof(*(resource->suppressible_state_vector)),
                     NPA_PENDING_REQUEST(client).pointer.vector,
                     vector_size );
      }
    }
    else 
    {
      if ( npa_resource_has_attribute( resource, 
                                       NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) )
      {
        core_memcpy( resource->required_state_vector,sizeof(*(resource->required_state_vector)),  
                     NPA_PENDING_REQUEST(client).pointer.vector,
                     vector_size );
      }
    }
      
    return resource->request_state;
  }
  else if ( client->type == NPA_CLIENT_SUPPRESSIBLE )
  {
    resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = 
								NPA_PENDING_REQUEST(client).state;
  }
  else if ( client->type == NPA_CLIENT_REQUIRED )
  {
    resource->internal_state[NPA_REQUIRED_INDEX] = 
								NPA_PENDING_REQUEST(client).state;
  }
    return NPA_PENDING_REQUEST(client).state;
}

npa_resource_state npa_always_on_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  resource->internal_state[NPA_REQUIRED_INDEX] = \
    resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = resource->active_max;
  return resource->active_max;
}

npa_resource_state npa_impulse_update_fcn( npa_resource      *resource,
                                           npa_client_handle  client )
{
  return resource->request_state;
}

npa_resource_state npa_no_client_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  /* This function should never be invoked */
  CORE_VERIFY(0);
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
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_min_plugin = 
{
  npa_min_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                         /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_sum_plugin = 
{
  npa_sum_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_identity_plugin = 
{
  npa_identity_update_fcn,
  0xffffffff,                   /* All client types supported */
  NULL,                         /* Create client function */
  NULL,                         /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_always_on_plugin = 
{
  npa_always_on_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};


const npa_resource_plugin npa_impulse_plugin = 
{
  npa_impulse_update_fcn,
  /* Supported client types */
  NPA_CLIENT_IMPULSE,
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_or_plugin = 
{
  npa_or_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                         /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

const npa_resource_plugin npa_binary_and_plugin = 
{
  npa_binary_and_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
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
  NULL,                          /* Destroy client function */
  NULL,                         /* Extended client create function */
  NULL                          /* Cancel Client Function */
};

#ifdef __cplusplus
}
#endif
