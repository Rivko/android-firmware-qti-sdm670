/*=========================================================================
@file npa_log.c

NPA Log manipulation API

Copyright © 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/framework/npa_log.c#1 $
==========================================================================*/

#include "npa_log.h"
#include "npa_internal.h"
#include "npa_inlines.h"

#include "ULog.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <!-- npa_update_resource_log_by_handle -->
 */
static npa_status 
npa_update_resource_log_by_handle( char *log_name, npa_resource *resource,
                                   ULogHandle ( *ulog_fcn )
                                       ( ULogHandle, ULogHandle ) )
{
  ULogHandle  log, new_log;
  npa_client *client;

  if ( !log_name || !resource || !resource->definition ||
       ( ULogCore_TeeAdd != ulog_fcn && ULogCore_TeeRemove != ulog_fcn ) )
  {
    return NPA_ERROR;
  }

  if ( DAL_SUCCESS != ULogCore_Connect( &log, log_name ) )
  {
    return NPA_ERROR;
  }

  /* Not checking the returned handle, as NULL is a valid return value */
  new_log = ulog_fcn( resource->log, log );

  Core_MutexLock( resource->node_lock );

  /* All clients of this resource inherit the log change */
  client = resource->clients;
  while ( client )
  {
    /* All this client's log(s) are same as its resource */
    if ( client->log == resource->log )
    {
      client->log = new_log;
    }
    /* One or more logs are associated with this client but not its resource */
    else
    {
      /* Not checking the returned handle, as NULL is a valid return value */
      client->log = ulog_fcn( client->log, log );
    }
    client = client->next;
  }

  /* Update resource log handle after all its clients */
  resource->log = new_log;

  Core_MutexUnlock( resource->node_lock );
  return NPA_SUCCESS;
}


/**
 * <!-- npa_update_resource_log_by_name -->
 */
static npa_status 
npa_update_resource_log_by_name( char *log_name, char *resource_name, 
                                 ULogHandle ( *ulog_fcn )
                                     ( ULogHandle, ULogHandle ) )
{
  char         *cptr;
  unsigned int  root_len = 0;
  npa_link     *link;
  npa_status    status;

  if ( !log_name || !resource_name ||
       ( ULogCore_TeeAdd != ulog_fcn && ULogCore_TeeRemove != ulog_fcn ) )
  {
    return NPA_ERROR;
  }

  /* Search for the wildcard ... */
  cptr = strstr( resource_name, "..." );
  if ( cptr )
  {
    /* There is a wildcard suffix */
    if ( ( '/' != *( cptr - 1 ) && '\\' != *( cptr - 1 ) ) || 
         ( 0 != *( cptr + 3 ) ) )
    {
      return NPA_ERROR;
    }

    /* Calculate the length of the root name */
    root_len = ( cptr - resource_name ) / sizeof(char);
  }

  npa_lock_graph();

  link = npa_get_resources();
  while ( link )
  {
    if ( cptr ? ( 0 == strncmp( link->name, resource_name, root_len ) ) :
                ( 0 == strncmp( link->name, resource_name, 
                                NPA_MAX_STR_LEN + 1 ) ) )
    {
      status = npa_update_resource_log_by_handle( log_name, link->resource,
                                                  ulog_fcn );
      if ( NPA_SUCCESS != status )
      {
        npa_unlock_graph();
        return status;
      }
 
      /* Break if there is no wildcard suffix, full match */
      if ( !cptr ) 
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
  return npa_update_resource_log_by_name( log_name, resource_name, 
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
npa_status npa_remove_resource_log_by_name( char *log_name, 
                                            char *resource_name )
{
  if ( 0 == strncmp( log_name, NPA_DEFAULT_LOG_NAME, NPA_MAX_STR_LEN + 1 ) )
  {
    return NPA_ERROR;
  }

  return npa_update_resource_log_by_name( log_name, resource_name, 
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
                                           npa_resource *resource )
{
  return npa_update_resource_log_by_handle( log_name, resource,
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
                                              npa_resource *resource )
{
  return npa_update_resource_log_by_handle( log_name, resource,
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
npa_update_client_log_by_handle( char *log_name, npa_client *client,
                                 ULogHandle ( *ulog_fcn )
                                     ( ULogHandle, ULogHandle ) )
{
  ULogHandle log = NULL; /* Klocworks */
  ULogHandle new_log = NULL;
  npa_resource *resource;

  if ( !log_name || !client || 
       ( ULogCore_TeeAdd != ulog_fcn && ULogCore_TeeRemove != ulog_fcn ) )
  {
    return NPA_ERROR;
  }

  CORE_VERIFY_PTR( resource = client->resource );

  if ( DAL_SUCCESS != ULogCore_Connect( &log, log_name ) )
  {
    return NPA_ERROR;
  }

  Core_MutexLock( resource->node_lock );

  if ( client->log == resource->log )
  {
    /* The trick below is to duplicate the existing ulog tee (in case that 
     * client->log is a tee handle ), but with an extra "log_handle".
     * This is all because the ULog owner doesn't want to support a public
     *  API function to copy a ulog tee.
     */
    new_log = ULogCore_TeeAdd( log, client->log );
  }

  if ( new_log )
  {
    client->log = ulog_fcn( new_log, log );
  }
  else
  {
    client->log = ulog_fcn( client->log, log );

    /* If a client log handle is equivalent to its resource log handle, we 
     * could release the client one, and reuse the existing resource log 
     * handle. We need a function to compare two ulog handles to achieve 
     * this. We punt on the optimization unless it becomes a significant
     * memory issue. 
     */
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
npa_status npa_add_client_log_by_handle( char *log_name, npa_client *client )
{
  return npa_update_client_log_by_handle( log_name, client, ULogCore_TeeAdd );
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
                                            npa_client *client )
{
  /* Users are not allowed to call this function to stop outputting logging 
     messages to the resource log */
  if ( ULogCore_IsLogPresent( 
                (ULogHandle *)client->resource->log, log_name ) )
  {
    return NPA_ERROR;
  }

  return npa_update_client_log_by_handle( log_name, client,
                                          ULogCore_TeeRemove );
}

/**
 * <!-- npa_log_to_qdss -->
 *
 * @brief Setup a tee such that the NPA Log is written out over QDSS as well
 */
void npa_log_to_qdss( void )
{
  ULogResult ulog_result;
  ULogHandle qdssLog;

  ulog_result = ULogFront_RealTimeInit( &qdssLog, "NPALogToQDSS",
                                        64, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS );
  CORE_VERIFY( DAL_SUCCESS == ulog_result );
  
  CORE_VERIFY( DAL_SUCCESS == 
               ULogCore_SetTransportToStmAscii( qdssLog, 13 ) );
               
  npa.log = ULogCore_TeeAdd( npa.log, qdssLog );
}

#ifdef __cplusplus
}
#endif
