/*============================================================================
@file unpa_client.c

Client routines in the UNPA framework

Copyright (c) 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/

#include "unpa_internal.h"

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/** 
 * <!-- unpa_create_client -->
 *
 * @brief Create a client to a resource; you cannot issue requests to resources
 * without first creating a client to it. If a resource with the given name
 * is defined and accepts clients of "type", a new unpa_client structure
 * will be created and and a pointer to it returned to caller.
 * If the client cannot be created, NULL is returned.
 *
 * @param client_name: Name of the client; length, including the '\0',
 * must be < UNPA_MAX_NAME_LEN.
 * @param client_type: Type of client; one of the UNPA_CLIENT_* defines
 * @param resource_name: Name of the resource to create a client to
 *
 * @return If successful, a pointer to a unpa_client structure; else, NULL
 */
unpa_client* unpa_create_client( const char *client_name, uint32_t client_type,
                                 const char *resource_name )
{
  unpa_resource *resource;
  unpa_client *client;
  uint32_t supported_types;
  
  CORE_VERIFY_PTR( client_name );
  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY( strlen( client_name ) < UNPA_MAX_NAME_LEN );
  
  resource = unpa_get_resource( resource_name );
  if ( !resource )
  {
    return NULL;
  }
  
  supported_types = resource->definition->client_types;
  if ( supported_types == UNPA_UPDATE_CLIENT_TYPES )
  {
    supported_types = 
      unpa_get_supported_client_types( resource->definition->update_fcn );
    CORE_VERIFY( supported_types != 0x0 );
  }
  
  if ( !( supported_types & client_type ) )
  { 
    /* The resource does not support clients of the given type */
    return NULL;
  }
  
  client = (unpa_client *) malloc( sizeof(unpa_client) );
  CORE_VERIFY_PTR( client );
  memset( client, 0, sizeof(unpa_client) );
  
  client->name = client_name;
  client->type = client_type;
  client->resource = resource;
  
  rex_enter_crit_sect( &resource->lock);
  
  client->next = resource->clients;
  resource->clients = client;
  
  client->request_attr = UNPA_REQUEST_ADD_CLIENT;
  
  if ( unpa_resource_has_attribute( resource, 
                                    UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY ) )
  {
    resource->definition->driver_fcn( resource, client, 0 );
  }
  
  rex_leave_crit_sect( &resource->lock );

  return client;  
}

/** 
 * <!-- unpa_destroy_client -->
 * @brief Destroy a client. The current request from client is cancelled.
 *
 * @param client: Pointer to the unpa_client to destroy.
 */
void unpa_destroy_client( unpa_client *client )
{
  unpa_client *c, *prevc;
  unpa_resource *resource;
  
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( resource = client->resource );
  
  /* Cancel any active request from client */
  unpa_cancel_request( client );
  
  rex_enter_crit_sect( &resource->lock);
  
  for ( prevc = NULL, c = client->resource->clients; c != NULL;
        prevc = c, c = c->next )
  {
    if ( c == client )
    {
      if ( prevc == NULL )
      {
        client->resource->clients = c->next;
      }
      else
      {
        prevc->next = c->next;
      }
      break;
    }
  }
  
  /* Verify that the client was indeed associated with resource
     and is now unlinked */  
  CORE_VERIFY( c != NULL );
  
  if ( unpa_resource_has_attribute( resource, 
                                    UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY ) )
  {
    client->request_attr = UNPA_REQUEST_REMOVE_CLIENT;
    resource->definition->driver_fcn( resource, client, 0 );
  }
  
  rex_leave_crit_sect( &resource->lock );
  
  memset( client, 0, sizeof(unpa_client) );
  free( client );
}

/** 
 * <!-- unpa_set_request_attribute -->
 *
 * @brief Sets the attribute for the specific request
 * The attribute is "one shot" i.e. will be set for the duration of the request and will be
 * cleared out when the request is complete.
 *
 * @param client: The client
 * @param attribute: The attribute to set (usually a mask).
 */
void unpa_set_request_attribute( unpa_client* client, uint32_t attribute)
{
  CORE_VERIFY_PTR( client );
  client->request_attr |= attribute;
}

/** 
 * <!-- unpa_issue_request -->
 *
 * @brief Do not use this function directly. Instead, use one of the 
 * issue_required_request or issue_suppressible_request macros, depending on
 * the type of client you are issuing with.
 */
void unpa_issue_request( unpa_client* client, unpa_resource_state request )
{
  uint32_t pass_thru = 0;
  
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( client->resource );
  
  /* If this is a sync request following previous FnF request(s) from client */
  if ( !( client->request_attr & UNPA_REQUEST_FIRE_AND_FORGET ) &&
       ( client->internal_attr & UNPA_CLIENT_LAST_FNF ) )
  {
    client->internal_attr &= ~UNPA_CLIENT_LAST_FNF;
    pass_thru = 1;
  }

  /* If client's request is different from its previous/active request OR
   * If client is new and issuing its first request OR
   * If pass_thru is TRUE, send it through
   */
  if ( request != client->active_request.val ||
       ( client->request_attr & UNPA_REQUEST_ADD_CLIENT ) || pass_thru )
  {
    client->pending_request.val = request;
    
    //UNPA_LOG( &unpa.log, "issue_request (client: XXX) (type: XXX) (resource: XXX) (request: XXX) (req_attr: XXX)" );
    
    rex_enter_crit_sect( &client->resource->lock );
    
    unpa_update_resource( client );
    /* Resource is unlocked in above routine */
  }
  else
  {
    //UNPA_LOG( &unpa.log, "redundant_request (client: XXX) (request: XXX)" );
  }

  client->request_attr = UNPA_REQUEST_DEFAULT;
}

/** 
 * <!-- unpa_issue_request_async -->
 *
 * @brief Issue a request from client such that the resource processes the
 * request asynchronously. Resource will dispatch "done_cb" after the
 * request is processed. See note on return value.
 * Clients are non-reentrant and use must be serialised by caller. 
 * For readability, use one of the below, issue_required_request_async or
 * issue_suppressible_request_async macros, depending on your client type.
 *
 * @return A "request id" for the async request that allows the caller
 * to map this request to the eventual callback. A return value of 0 indicates
 * that the request is done (was executed synchronously or was short-circuited)
 * and no callback will arrive.
 */
uint32_t unpa_issue_request_async( unpa_client* client, 
                                   unpa_resource_state request,
                                   unpa_request_done_cb done_cb )
{
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( done_cb );
  CORE_VERIFY_PTR( client->resource );
  CORE_VERIFY( unpa_resource_has_attribute( client->resource, 
                                            UNPA_RESOURCE_ASYNC_REQUESTS ) );
  
  client->request_attr |= UNPA_REQUEST_ASYNC;
  client->done_cb = done_cb;
  client->async_data = 0;
  
  unpa_issue_request( client, request );
  return client->async_data;
}

/** 
 * <!-- unpa_cancel_request -->
 *
 * @brief Cancel (or drop) the previous request from client.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_complete_request
 *
 * @param client: The client
 */
void unpa_cancel_request( unpa_client* client )
{
  CORE_VERIFY_PTR( client );
  
  client->request_attr |= UNPA_REQUEST_DROP_VOTE;
  unpa_issue_request( client, 0 );
}

/** 
 * <!-- unpa_cancel_request_async -->
 *
 * @brief Cancel (or drop) the previous request from client, asynchronously.
 * The resource will dispatch "done_cb" after the cancel vote is processed.
 * See note on return value.
 *
 * Equivalent to unpa_complete_request_async
 *
 * @return A "request id" for the async request that allows the caller
 * to map this request to the eventual callback. A return value of 0 indicates
 * that the request is done (was executed synchronously or was short-circuited)
 * and no callback will arrive.
 */
uint32_t unpa_cancel_request_async( unpa_client* client, 
                                    unpa_request_done_cb done_cb )
{
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( done_cb );
  CORE_VERIFY_PTR( client->resource );
  CORE_VERIFY( unpa_resource_has_attribute( client->resource, 
                                            UNPA_RESOURCE_ASYNC_REQUESTS ) );
  
  client->request_attr |= ( UNPA_REQUEST_DROP_VOTE | UNPA_REQUEST_ASYNC );
  client->done_cb = done_cb;
  client->async_data = 0;
  
  unpa_issue_request( client, 0 );
  return client->async_data;
}

/** 
 * <!-- unpa_try_issue_request -->
 *
 * @brief Do not use this function directly. Instead, use one of the 
 * try_issue_required_request or try_issue_suppressible_request macros, 
 * depending on the type of client you are issuing with.
 * Attempts to issue a request using a try_lock on the resource; if the lock
 * succeeds, processes the request and returns 0. Else, returns -1.
 */
int32_t unpa_try_issue_request( unpa_client* client, 
                                unpa_resource_state request )
{
  int err = 0;
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( client->resource );

  /* If the client's request is different from its previous/active request OR
     if the client is new and issuing its first request, send it through */
  if ( request != client->active_request.val ||
       ( client->request_attr & UNPA_REQUEST_ADD_CLIENT ) )
  {
    client->pending_request.val = request;
    
    //UNPA_LOG( &unpa.log, "try_issue_request (client: XXX) (type: XXX) (resource: XXX) (request: XXX) (req_attr: XXX)" );
    
    err = rex_try_enter_crit_sect( &client->resource->lock );
    if ( err == 0 )
    {
      unpa_update_resource( client );
      /* Resource is unlocked in above routine */
    }
  }
  else
  {
    //UNPA_LOG( &unpa.log, "redundant_request (client: XXX) (request: XXX)" );
  }

  client->request_attr = UNPA_REQUEST_DEFAULT;
  return err;
}

/** 
 * <!-- unpa_try_cancel_request -->
 *
 * @brief Attempts to cancel (or drop) the previous request from client,
 * using a try_lock on the resource; if the lock succeeds, processes the
 * cancel and returns 0. Else, returns -1.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_try_complete_request
 *
 * @param client: The client
 */
int32_t unpa_try_cancel_request( unpa_client* client )
{
  CORE_VERIFY_PTR( client );
  
  client->request_attr |= UNPA_REQUEST_DROP_VOTE;
  return unpa_try_issue_request( client, 0 );
}