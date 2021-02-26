/** @file npa_target_init.c
   
  UEFI target NPA initialization

  Copyright (c) 2013, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/05/13   rli      Added polling mode ON.
03/27/13   rli      Initial revision.
=============================================================================*/

#include "npa.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NPA_REMOTING_SUPPORTED
/* RPM available callback, defined in npa_remote_rpm_protocol.c */
extern void npa_rpm_available_cb( void*, unsigned int, void*, unsigned int );

/*----------------------------------------------------------------------------
 pollingmode_resource_avail_cb
 Create the npa client once the resource is available and vote for polling mode. 
----------------------------------------------------------------------------*/
static void pollingmode_resource_avail_cb( void *context,    //The context passed into this npa_resource_available_cb
                                    unsigned int event_type,  //The event identifier - 0 for resource created
                                    void *resource_name,      //The name of the resource to become available
                                    unsigned int rname_len )  //The length of the resource name
{
  npa_client_handle client = NULL;

  // Initialize the client by registering it with the /protocol/rpm/polling_mode
  client =  npa_create_sync_client( resource_name,  "npa_uefi_init", NPA_CLIENT_REQUIRED); 
  if ( client )
  {
    npa_issue_required_request(client, 1);   //Polling mode ON
  }
}
#endif

void npa_target_init( void )
{
#ifdef NPA_REMOTING_SUPPORTED
  /* npa_rpm_available_cb is only defined when remoting is supported. */
  npa_resource_available_cb( "/init/rpm", npa_rpm_available_cb,  NULL );
  npa_resource_available_cb( "/protocol/rpm/polling_mode", pollingmode_resource_avail_cb, NULL );
#endif
}

#ifdef __cplusplus
}
#endif
