/*============================================================================
@file npa_query.c

Routines to create and destroy NPA query handles and query resources using
said handles.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/framework/npa_query.c#1 $
============================================================================*/

#include "npa_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * <!-- npa_create_query_handle --> 
 * 
 * @brief Create a query handle to a resource 
 *  
 * @param resource_name : name of resource being queried
 *
 * @return Query handle to resource
 */
npa_query_handle npa_create_query_handle( const char *resource_name )
{
  npa_link *link = npa_find_resource_link( resource_name );

  if ( link )
  {
    /* If this is an instantiated resource ...*/
    if ( link->resource->definition )
    {
      /* Return the resource link as query handle. Nothing is alloc-d */
      return link;
    }
  }
  return NULL;
}


/** 
 * <!-- npa_destroy_query_handle --> 
 *  
 * @brief Destroy the query handle. Only query handles that have been created
 * created via npa_create_query_handle need to be explicitly destroyed
 *
 * @param query : Query handle to destroy
 */
void npa_destroy_query_handle( npa_query_handle query )
{
  /* Since the query handle returned from npa_create_query_handle is nothing
     but a pointer to the resource link, there is nothing to destroy */
  return;
}


/** 
 * <!-- npa_query_resource -->
 *   
 * @brief Query a resource via the resource handle. Note that due to
 * concurrency, the resource may no longer be in the returned state.
 *
 * @param resource : resource being queried
 * @param query_id : type of query being issued
 * @param result : result from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_resource( npa_resource    *resource, 
                                     uint32           query_id, 
                                     npa_query_type  *result )
{
  npa_query_status status = NPA_QUERY_UNSUPPORTED_QUERY_ID;

  /* Validate resource */
  if ( !resource ) { return NPA_QUERY_UNKNOWN_RESOURCE; }
  if ( NPA_IS_RESOURCE_MARKER(resource) ) { return status; }

  if ( !result ) { return NPA_QUERY_NULL_POINTER; }
  
  /* Give the resource a chance to handle the query */
  if ( resource->definition->query_fcn )
  {
    status = resource->definition->query_fcn( resource, query_id, result );
  }

  /* If the query has not been handled yet, process it */
  if ( NPA_QUERY_UNSUPPORTED_QUERY_ID == status )
  {
    status = NPA_QUERY_SUCCESS;

    switch ( query_id )
    {
    case NPA_QUERY_CURRENT_STATE:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_state;
      break;
    case NPA_QUERY_ACTIVE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_max;
      break;
    case NPA_QUERY_RESOURCE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->definition->max;
      break;
    case NPA_QUERY_RESOURCE_DISABLED:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->sequence & NPA_RESOURCE_DISABLED;
      break;
    case NPA_QUERY_RESOURCE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->definition->attributes;
      break;
    case NPA_QUERY_NODE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->node->attributes;
      break;
    case NPA_QUERY_CURRENT_AGGREGATION:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->request_state;
      break;
    default:
      result->type = NPA_QUERY_TYPE_REFERENCE;
      result->data.reference = NULL;
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
    }
  }

  return status;
}


/** 
 * <!-- npa_query -->
 *   
 * @brief Query a resource via a query handle. Note that due to concurrency, 
 * the resource may no longer be in the returned state.
 *
 * @param handle : Query handle to the resource
 * @param query_id : type of query being issued
 * @param result : result from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query( npa_query_handle handle, 
                            uint32           query_id, 
                            npa_query_type  *result )
{
  npa_query_status  status  = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  npa_resource     *resource = NULL;

  /* Validate handle */
  if ( !handle ) { return NPA_QUERY_UNKNOWN_RESOURCE; }

  CORE_VERIFY_PTR( resource = handle->resource );
  
  /* Validate that resource is not a placeholder */
  if ( resource->definition == NULL ) { return NPA_QUERY_UNKNOWN_RESOURCE; }  

  if ( NPA_IS_RESOURCE_MARKER(resource) ) { return status; }

  if ( !result ) { return NPA_QUERY_NULL_POINTER; }
  
  /* If the resource has a link_query_fcn, give it a chance to handle the
     query. link_query_fcns allow the resource to handle queries by alias */
  if ( resource->definition->link_query_fcn )
  {
    status = resource->definition->link_query_fcn( handle, query_id, result );
  }
  
  /* Else, pass the query to query_resource which will either call the
     resource's query_fcn (if defined) or the framework query handler */
  if ( NPA_QUERY_UNSUPPORTED_QUERY_ID == status )
  {
    status = npa_query_resource( resource, query_id, result );
  }

  return status;
}

/** 
 * <!-- npa_query_by_name -->
 *  
 * @brief Query the resource by name.
 * This is a convenience function that wraps the following:
 *
 * h = npa_create_query_handle(...);
 * npa_query(h, ...);
 * npa_destroy_query_handle(h);
 *
 * If you are making mulitple queries to the same resource, calling this
 * function multiple times will have a poorer performance relative to creating
 * a query handle yourself and issuing multiple npa_query calls calls from it.
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param resource_name : name of resource being queried
 * @param query_id : type of query being issued
 * @param result : result from the query
 * 
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *result )

{
  return npa_query( (npa_query_handle)npa_find_resource_link( resource_name ),
                    query_id, result );
}


/** 
 * <!--  npa_query_by_client -->
 *  
 * @brief Query a resource via a client to that resource. This function will
 * look up the client's resource by traversing a linked list. Due to this, the
 * performance of this function is poor relative to using a query handle.
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param client : client of resource being queried
 * @param query_id : type of query being issued
 * @param result : result from the query
 *
 *  @return success/failure of query (npa_query_status)
 *
 */
npa_query_status npa_query_by_client( npa_client_handle client, 
                                      uint32            query_id, 
                                      npa_query_type   *result )
{
  npa_query_status status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  CORE_VERIFY_PTR( client );
  
  if ( NPA_QUERY_CLIENT_ACTIVE_REQUEST == query_id )
  {
    if ( NPA_CLIENT_VECTOR == client->type || 
         NPA_CLIENT_SUPPRESSIBLE_VECTOR == client->type )
    {
      result->type = NPA_QUERY_TYPE_STATE_VECTOR;
      result->data.state_vector = NPA_ACTIVE_REQUEST(client).pointer.vector;
    }
    else
    {
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = NPA_ACTIVE_REQUEST(client).state;
    }
    status = NPA_QUERY_SUCCESS;
  }
  else
  {
    status = npa_query( 
             (npa_query_handle)npa_find_resource_link( client->resource_name ),
             query_id, result );
  }
  return status;
}


/** 
 * <!-- npa_query_by_event -->
 *  
 * @brief Query a resource via an event of that resource. Note that due to
 * concurrency, the resource may no longer be in the returned state.
 *
 * @param event : event of resource being queried
 * @param query_id : type of query being issued
 * @param result : result from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_event( npa_event_handle event, 
                                     uint32           query_id, 
                                     npa_query_type  *result )
{
  CORE_VERIFY_PTR( event );
  return npa_query_resource( event->resource, query_id, result );
}


/** 
 * <!-- npa_query_get_resource -->
 *  
 * @brief Retrieve the resource associated with the given query handle.
 * This function is intended to be used by resources that wish to define a
 * query API that goes beyond what npa_resource_query_fcn can support
 *  
 * @param query_handle : Handle to resource being queried
 *
 * @return Pointer to resource associated with the query handle
 */
npa_resource *npa_query_get_resource( npa_query_handle query_handle )
{
  /* The query handle is simply a pointer to the resource_link structure */
  CORE_VERIFY_PTR( query_handle );
  return query_handle->resource;
}

/** 
 * <!-- npa_query_resource_available -->
 *  
 * @brief Query if a resource is currently available 
 *
 * Query and return if the given resource is currently available.  If
 * the resource is not currently available, this does not mean that it
 * may not become available at a future time. If you wish to be notified
 * when the resource becomes available, use npa_resource_available_cb() to
 * register a callback to be invoked when the resource becomes available.
 *
 * @param resource_name :  name of the resource that's being queried
 *
 * @return NPA_QUERY_SUCCESS if resource is currently available,
 * NPA_QUERY_UNKNOWN_RESOURCE if resource is not currently available
 */
npa_query_status npa_query_resource_available( const char *resource_name )
{
  if ( npa_find_resource( resource_name ) )
  {
    return NPA_QUERY_SUCCESS;
  }
  else
  {
    return NPA_QUERY_UNKNOWN_RESOURCE;
  }
}

#ifdef __cplusplus
}
#endif
