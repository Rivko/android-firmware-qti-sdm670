/*============================================================================
@file unpa_query.c

Query routines in the UNPA framework

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/

#include "unpa_internal.h"

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/** 
 * <!-- unpa_get_query_handle -->
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 * query certain runtime attributes of the resource, such as active_state.
 *
 * @param resource_name: Name of the resource
 *
 * @return A query handle to the resource or NULL, if the resource was
 * not (yet) defined.
 */
unpa_query_handle unpa_get_query_handle( const char *resource_name )
{
  CORE_VERIFY_PTR( resource_name );
  return (unpa_query_handle)unpa_get_resource( resource_name );
}

/** 
 * <!-- unpa_get_client_query_handle -->
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 * query certain runtime attributes of a particular client to a resource.
 *
 * @param client_name: Name of the client
 * @param resource_name: Name of the resource
 *
 * @return A query handle to the client or NULL, if the client doesn't exist.
 */
unpa_query_handle unpa_get_client_query_handle( const char *client_name,
                                                const char *resource_name )
{
  unpa_resource *resource;
  unpa_client *client = NULL;
  
  CORE_VERIFY_PTR( client_name );
  CORE_VERIFY_PTR( resource_name );
  
  resource = unpa_get_resource( resource_name );
  if ( resource )
  {
    client = resource->clients;
    while ( client )
    {
      if ( 0 == strncmp( client_name, client->name, UNPA_MAX_NAME_LEN+1 ) )
      {
        break;
      }
      client = client->next;
    }
  }
  
  return (unpa_query_handle)client;
}

/** 
 * <!-- unpa_query -->
 *
 * @brief Query certain runtime attributes of the resource or client, 
 * using the given unpa_query_handle.
 *
 * @param handle: Query handle, obtained using unpa_get_query_handle or
 * unpa_get_client_query_handle. Will be interpreted as a query handle
 * to a resource or client, based on the query id passed in.
 * @param id: One of the unpa_query_id enums.
 * @param result: OUT parameter with the results of the query.
 *
 * @return UNPA_QUERY_SUCCESS if successful; else, one of the other
 * unpa_query_status enums.
 */
unpa_query_status unpa_query( unpa_query_handle handle, unpa_query_id id,
                              unpa_query_result *result )
{
  unpa_query_status status = UNPA_QUERY_SUCCESS;
  
  CORE_VERIFY_PTR( handle );
  CORE_VERIFY_PTR( result );
  
  switch ( id )
  {
  /* Resource queries */
  case UNPA_QUERY_ACTIVE_STATE:
    result->val = ((unpa_resource *)handle)->active_state;
    break;
  case UNPA_QUERY_SLEEP_STATE:
    result->val = ((unpa_resource *)handle)->sleep_state;
    break;
  case UNPA_QUERY_ACTIVE_MAX:
    result->val = ((unpa_resource *)handle)->active_max;
    break;

  /* Client queries */
  case UNPA_QUERY_ACTIVE_REQUEST:
    result->val = ((unpa_client *)handle)->active_request.val;
    break;
  default:
    status = UNPA_QUERY_UNSUPPORTED;
  }
  
  return status;
}
