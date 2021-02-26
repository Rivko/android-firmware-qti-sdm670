/** @file npa_api_stub.c
   
  Null Lib for NPA

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/25/14   vk      Initial revision
=============================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>

typedef struct npa_client * npa_client_handle;
typedef unsigned int npa_resource_state;
typedef unsigned int npa_client_type;
typedef void ( *npa_callback )( void         *context,
                                unsigned int  event_type, 
                                void         *data,       
                                unsigned int  data_size );


void npa_issue_scalar_request( npa_client_handle   client, 
                               npa_resource_state  state )
{
  DEBUG ((EFI_D_WARN, "WARNING: Using NpaLibNull\n"));
  return;
}

npa_client_handle npa_create_sync_client_ex( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref )
{

  DEBUG ((EFI_D_WARN, "WARNING: Using NpaLibNull\n"));
  return NULL;
}

void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                void   *context )
{
  DEBUG ((EFI_D_WARN, "WARNING: Using NpaLibNull\n"));
  return;
}

typedef struct npa_resource npa_resource;

void npa_resource_lock( npa_resource *resource )
{
  return;
}

void npa_resource_unlock( npa_resource *resource )
{
  return;
}

void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  state )
{
  return;
}

