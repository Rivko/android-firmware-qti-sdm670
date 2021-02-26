/*============================================================================
@file npa_target_routines.c

NPA target specific routines

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/hw/npa_target_routines.c#1 $
============================================================================*/
#include "npa_internal.h"

/* RPM available callback, defined in npa_remote_rpm_protocol.c */
extern void npa_rpm_available_cb( void*, unsigned int, void*, unsigned int );


void npa_target_init( void )
{
  /* The RPM driver on 8974 does not define the "/init/rpm" marker (yet?)
  and, per AJ, lazy initializes, so this call should be safe. */
  npa_resource_available_cb( "/init/rpm", npa_rpm_available_cb,  NULL ); 
}

#ifdef NPA_USES_RCINIT
#include "rcinit.h"

/* Create the NPA Scheduler Thread */
void npa_scheduler_init_rctask( void )
{
  RCINIT_INFO handle = rcinit_lookup("npaScheduler");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, npaScheduler_task);
  }
}

#endif
