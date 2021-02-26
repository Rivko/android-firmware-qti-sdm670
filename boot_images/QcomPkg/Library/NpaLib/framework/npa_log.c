/*=========================================================================
@file npa_log.c

NPA Log manipulation API

Copyright (c) 2009-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

==========================================================================*/

#include "npa_log.h"
#include "npa_internal.h"
#include "npa_inlines.h"

#include "ULog.h"
#ifndef TARGET_UEFI
    #include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <!-- npa_update_resource_log_by_handle -->
 */
static npa_status 
npa_update_resource_log_by_handle( char *log_name,
                                   npa_resource *resource,
                                   ULogHandle ( *ulog_fcn )
                                       ( ULogHandle, ULogHandle ) )
{
  ULogHandle  log_handle, new_log_handle;
  npa_client *client;

  if( ( NULL == log_name ) || ( NULL == resource ) ||
      ( ( ULogCore_TeeAdd != ulog_fcn ) && ( ULogCore_TeeRemove != ulog_fcn ) ) ||
      ( NULL == resource->definition ) )
  {
    return NPA_ERROR;
  }

  if( DAL_SUCCESS != ULogCore_Connect( &log_handle, log_name ) )
  {
    return NPA_ERROR;
  }

  /* Not checking the returned handle, as NULL is a valid return value */
  new_log_handle = ulog_fcn( resource->log_handle, log_handle );

  Core_MutexLock( resource->node_lock );

  /* All clients of this resource inherit the log change */
  client = resource->clients;
  while( NULL != client )
  {
    /* All this client's log(s) are same as its resource */
    if( client->log_handle == resource->log_handle )
    {
      client->log_handle = new_log_handle;
    }
    /* One or more logs are associated with this client but not its resource */
    else
    {
      /* Not checking the returned handle, as NULL is a valid return value */
      client->log_handle = ulog_fcn( client->log_handle, log_handle );
    }
    client = client->next;
  }
  /* Update resource log handle after all its clients */
  resource->log_handle = new_log_handle;

  Core_MutexUnlock( resource->node_lock );

  return NPA_SUCCESS;
}


/**
 * <!-- npa_update_resource_log_by_name -->
 */
static npa_status 
npa_update_resource_log_by_name( char         *log_name, 
                                 char         *resource_name, 
                                 ULogHandle ( *ulog_fcn )( ULogHandle, ULogHandle ) )
{
  char         *cptr;
  unsigned int  root_len = 0;
  npa_link     *link;
  npa_status    status;

  if( ( NULL == log_name ) || ( NULL == resource_name ) ||
      ( ( ULogCore_TeeAdd != ulog_fcn ) && ( ULogCore_TeeRemove != ulog_fcn ) ) )
  {
    return NPA_ERROR;
  }

  /* Search for the wildcard ... */
  cptr = strstr(resource_name, "...");
  if(NULL != cptr)
  {
    /* There is a wildcard suffix */
    if( ( ( '/' != *( cptr - 1 ) ) && ( '\\' != *( cptr - 1 ) ) ) || 
        ( 0 != *( cptr + 3 ) ) )
    {
      return NPA_ERROR;
    }

    /* Calculate the length of the root name */
    root_len = ( cptr - resource_name ) / sizeof(char);
  }

  npa_lock_graph();

  link = npa_get_resources();
  while( NULL != link )
  {
    if( cptr ? ( 0 == strncmp( link->name, resource_name, root_len ) ) :
               ( 0 == strncmp( link->name, resource_name, 
                               NPA_MAX_STR_LEN + 1 ) ) )
    {
      status = npa_update_resource_log_by_handle( log_name,
                                                  link->resource,
                                                  ulog_fcn );
      if( NPA_SUCCESS != status )
      {
        npa_unlock_graph();

        return status;
      }
      /* Break if there is no wildcard suffix, full match */
      if( NULL == cptr ) 
      {
        break;
      }
    }

    link = link->next;
  }

  npa_unlock_graph();

  return NPA_SUCCESS;
}


/**
 * <!-- npa_add_resource_log_by_name -->
 *
 * @brief Copies the npa logging messages associated with the specified
 * resource(s) to a separate log.
 *
 *
 * Users could specify a single resource by its full resource name or a 
 * bunch of resources by the common prefix followed by a wildcard (...). 
 * For example,
 * /core/cpu         a single resource with its full name
 * /core/cpu/...     all resources starting with "/core/cpu/"
 * 
 * Note: in the initial implementation, we only support a single trailing
 * wildcard: "..." at the directory level, which means it should follow
 * a slash. The use cases below are not supported right now:
 * /core/.../latency
 * /core/c...  
 *
 * Note: the resource(s) should be created before the call to this function
 * for the associated messages to be copied to the named log.
 *
 * @param ulog_name : Name of the log to which the messages should go, the
 * name must follow the ulog naming restrictions
 * @param resource_name : Name or name root of the resource(s)
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_resource_log_by_name( char *log_name, char *resource_name )
{
  return npa_update_resource_log_by_name( log_name, 
                                          resource_name, 
                                          ULogCore_TeeAdd );
}


/**
 * <!-- npa_remove_resource_log_by_name -->
 *
 * @brief Stops copying messages associated with the specified resources to the named log
 * 
 * Users could specify a single resource by its full resource name or a 
 * bunch of resources by the common prefix followed by a wildcard (...). 
 * For example,
 * /core/cpu         a single resource with its full name
 * /core/cpu/...     all resources starting with "/core/cpu/"
 * 
 * It is not permissable to remove the default NPA log ("NPA Log") by
 * resource name.  This log can only be removed via
 * npa_remove_resource_log_by_handle()
 *
 * Note: in the initial implementation, we only support a single trailing
 * wildcard "..." at the directory level, which means it should follow
 * a slash. The use cases below are not supported right now:
 * /core/.../latency
 * /core/c...  
 *
 * Note: the resource(s) must exist, if not, then the function will return an 
 * error.
 *
 * @param ulog_name : Name of the log, which must follow the ulog naming restrictions
 * @param resource_name : Name or name root of the resource(s)
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_resource_log_by_name( char *log_name, char *resource_name )
{
  if ( 0 == strncmp( log_name, NPA_DEFAULT_LOG_NAME, NPA_MAX_STR_LEN + 1 ) )
  {
    return NPA_ERROR;
  }

  return npa_update_resource_log_by_name( log_name, 
                                          resource_name, 
                                          ULogCore_TeeRemove );
}


/**
 * <!-- npa_add_resource_log_by_handle -->
 *
 * @brief Copies the npa logging messages associated with an npa resource to a separate log
 *
 * @param ulog_name : Name of the log to which the messages should go, the name 
                      must follow the ulog naming restrictions
 * @param resource : Handle to the resource that is of interested
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_resource_log_by_handle( char *log_name, 
                                           struct npa_resource *resource )
{
  return npa_update_resource_log_by_handle( log_name,
                                            resource,
                                            ULogCore_TeeAdd );
}


/**
 * <!-- npa_remove_resource_log_by_handle -->
 *
 * @brief Stops copying messages associated with an npa resource to the named log
 *
 * @param ulog_name : Name of the log, which must follow the ulog naming restrictions
 * @param resource : Handle to the resource that is of interested
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_resource_log_by_handle( char *log_name,
                                              struct npa_resource *resource )
{
  return npa_update_resource_log_by_handle( log_name,
                                            resource,
                                            ULogCore_TeeRemove );
}


/**
 * <!-- npa_add_client_log_by_handle -->
 *
 * @brief Copies the npa logging messages associated with an npa client to a separate log
 *
 * @param ulog_name : Name of the log to which the messages should go, the name
 *                    must follow the ulog naming restrictions
 * @param client : Handle to the client that is of interest
 * @param ulog_fcn : Function to apply.
 *
 * @return success/failure (npa_status)
 */
static npa_status 
npa_update_client_log_by_handle( char *log_name,
                                 npa_client   *client,
                                 ULogHandle ( *ulog_fcn )( ULogHandle, ULogHandle ) )
{
  ULogHandle           log_handle = NULL; /* Klocworks */
  ULogHandle           new_log_handle = NULL;
  struct npa_resource *resource;

  if( ( NULL == log_name ) || ( NULL == client ) || 
      ( ( ULogCore_TeeAdd != ulog_fcn ) && ( ULogCore_TeeRemove != ulog_fcn ) ) )
  {
    return NPA_ERROR;
  }
  CORE_VERIFY_PTR( resource = client->resource );

  if( DAL_SUCCESS != ULogCore_Connect( &log_handle, log_name ) )
  {
    return NPA_ERROR;
  }

  Core_MutexLock( resource->node_lock );

  if( client->log_handle == resource->log_handle )
  {
    /* The trick below is to duplicate the existing ulog tee (in case that 
       client->log_handle is a tee handle ), but with an extra "log_handle".
       This is all because the ULog owner doesn't want to support a public
       API function to copy a ulog tee. */
    new_log_handle = ULogCore_TeeAdd( log_handle, client->log_handle );
  }

  if( NULL != new_log_handle )
  {
    client->log_handle = ulog_fcn( new_log_handle, log_handle );
  }
  else
  {
    client->log_handle = ulog_fcn( client->log_handle, log_handle );

    /* If a client log handle is equivalent to its resource log handle, we 
       could release the client one, and reuse the existing resource log 
       handle. We need a function to compare two ulog handles to achieve 
       this. We punt on the optimization unless it becomes a significant
       memory issue.  */
  }

  Core_MutexUnlock( resource->node_lock );

  return NPA_SUCCESS;
}


/**
 * <!-- npa_add_client_log_by_handle -->
 *
 * @brief Copies the npa logging messages associated with an npa client to a separate log
 *
 * @param ulog_name : Name of the log to which the messages should go, the name
 * must follow the ulog naming restrictions
 * @param client : Handle to the client of interest
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_client_log_by_handle( char *log_name, 
                                         npa_client_handle client )
{
  return npa_update_client_log_by_handle( log_name,
                                          client,
                                          ULogCore_TeeAdd );
}


/**
 * <!-- npa_remove_client_log_by_handle -->
 *
 * @brief Stops copying messages associated with an npa client to the named log
 *
 * @note Client log is a superset of its resoruce log. It is not allowed to 
 * remove a resource log by calling this funtion.
 *
 * @param ulog_name : Name of the log, which must follow the ulog naming restrictions
 * @param client : Handle to the client of interest
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_client_log_by_handle( char *log_name, 
                                            npa_client_handle client )
{
  /* Users are not allowed to call this function to stop outputting logging 
     messages to the resource log */
  if( ULogCore_IsLogPresent( 
               (ULogHandle *)client->resource->log_handle, log_name ) )
  {
    return NPA_ERROR;
  }

  return npa_update_client_log_by_handle( log_name,
                                          client,
                                          ULogCore_TeeRemove );
}


#ifdef __cplusplus
}
#endif
