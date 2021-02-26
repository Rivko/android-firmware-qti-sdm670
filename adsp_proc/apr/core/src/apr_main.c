/*
  Copyright (C) 2009-2012, 2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/src/apr_main.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include "mmstd.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_atomic.h"
#include "apr_log.h"
#include "apr_api_i.h"
#include "aprv2_api_i.h"



/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static const char_t apr_version_string[] = "2.0.0";

/* Flag indicate whether apr driver is initialized. This is added temporarily */
/* since CSD and AVP are both calling apr_init. We need to file a CR against  */
/* to remove apr_init call in their code. */
bool_t apr_init_flag=FALSE;

/*****************************************************************************
 * Core Routine Implementations                                              *
 ****************************************************************************/

APR_EXTERNAL int32_t apr_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;

  /* This is added temporarily, since CSD and AVP are both calling apr_init.  */
  /* We need to file a CR against them to remove apr_init call in their code. */
  if ( apr_init_flag == TRUE )
  {
    return APR_EOK;
  }
  apr_init_flag = TRUE;

  for ( ;; )
  {
    rc = apr_atomic_init( );
    if ( rc ) break;
    checkpoint = 1;

    rc = apr_log_init( );
    if ( rc ) break;
    checkpoint = 2;

    rc = aprv2_drv_init( );
    if ( rc ) break;
    checkpoint = 3;
    apr_init_flag = TRUE;
    
    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 3:
    ( void ) aprv2_drv_deinit( );
    /*-fallthru */
  case 2:
    ( void ) apr_log_deinit( );
    /*-fallthru */
  case 1:
    ( void ) apr_atomic_deinit( );
    /*-fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

APR_EXTERNAL int32_t apr_deinit ( void )
{
  ( void ) aprv2_drv_deinit( );
  ( void ) apr_log_deinit( );
  ( void ) apr_atomic_deinit( );

  apr_init_flag = FALSE;

  return APR_EOK;
}

/* The apr_call() allows newer libraries to link to older libraries without
 * the C API prototypes. The current CMI method of never deprecating is only
 * backward compatible and is not forward compatible.
 */
APR_EXTERNAL int32_t apr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( cmd_id )
  {
  case APR_CMDID_GET_VERSION:
    rc = apr_cmd_get_version( ( apr_cmd_get_version_t* ) params );
    break;

  case APRV2_CMDID_REGISTER:
    rc = aprv2_cmd_register( ( aprv2_cmd_register_t* ) params );
    break;

  case APRV2_CMDID_REGISTER2:
    rc = aprv2_cmd_register2( ( aprv2_cmd_register2_t* ) params );
    break;

  case APRV2_CMDID_DEREGISTER:
    rc = aprv2_cmd_deregister( ( aprv2_cmd_deregister_t* ) params );
    break;

  case APRV2_CMDID_ASYNC_SEND:
    rc = aprv2_cmd_async_send( ( aprv2_cmd_async_send_t* ) params );
    break;

  case APRV2_CMDID_ALLOC:
    rc = aprv2_cmd_alloc( ( aprv2_cmd_alloc_t* ) params );
    break;

  case APRV2_CMDID_FREE:
    rc = aprv2_cmd_free( ( aprv2_cmd_free_t* ) params );
    break;

  case APRV2_CMDID_QUERY_ADDRESS:
    rc = aprv2_cmd_query_address( ( aprv2_cmd_query_address_t* ) params );
    break;

  case APRV2_CMDID_LOCAL_DNS_LOOKUP:
    rc = aprv2_cmd_local_dns_lookup( ( aprv2_cmd_local_dns_lookup_t* ) params );
    break;

  case APRV2_CMDID_FORWARD:
    rc = aprv2_cmd_forward( ( aprv2_cmd_forward_t* ) params );
    break;

  case APRV2_CMDID_ALLOC_EXT:
    rc = aprv2_cmd_alloc_ext( ( aprv2_cmd_alloc_ext_t* ) params );
    break;

  case APRV2_CMDID_ALLOC_SEND:
    rc = aprv2_cmd_alloc_send( ( aprv2_cmd_alloc_send_t* ) params );
    break;

  case APRV2_CMDID_ACCEPT_COMMAND:
    rc = aprv2_cmd_accept_command( ( aprv2_cmd_accept_command_t* ) params );
    break;

  case APRV2_CMDID_END_COMMAND:
    rc = aprv2_cmd_end_command( ( aprv2_cmd_end_command_t* ) params );
    break;

  default:
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

/*****************************************************************************
 * Core Control Implementations                                              *
 ****************************************************************************/

APR_INTERNAL int32_t apr_cmd_get_version (
  apr_cmd_get_version_t* params
)
{
  if ( ( params == NULL ) || ( params->ret_size == NULL ) )
  {
    return APR_EBADPARAM;
  }

  *params->ret_size = sizeof( apr_version_string );

  if ( params->buffer == NULL )
  {
    return APR_EOK;
  }

  if ( params->buf_size < sizeof( apr_version_string ) )
  {
    return APR_ENEEDMORE;
  }

  ( void ) mmstd_strlcpy( params->buffer, apr_version_string,
                          MMSTD_MIN( sizeof( apr_version_string ), params->buf_size ) );

  return APR_EOK;
}

