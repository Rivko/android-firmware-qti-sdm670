/*
===========================================================================
*/
/**
  @file npa_test_remote_xo.c

  The file contains the resource definitions for XO management on the
  apps processor on MSM8974.
*/
/*
  ====================================================================

  Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  05/14/14   sj       64 bit compilation error fixes.
  08/16/13   rli      Updated to support b2b test runs.
  03/22/13   rli      Initial revision.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

/*
 * External header files.
 */
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>


/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Variables
==========================================================================*/

/*
 * NPA remote XO resource.
 */
static npa_remote_resource_definition Clock_XORemote =
{
  /* .local_resource_name   = */ "/clkregim/apps/xo/cxo",
  /* .remote_resource_name  = */ "clk0\x00\x00\x00\x00",
  /* .protocol_type         = */ "/protocol/rpm/rpm",
  /* .plugin                = */ &npa_max_plugin,
  /* .driver_fcn            = */ npa_remote_resource_local_aggregation_driver_fcn,
  /* .units                 = */ "Enable/Disable",
  /* .max                   = */ 1,
  /* .attributes            = */ 0,
  /* .data                   = */ NULL,
  /* .handle                = */ NULL
};


/* =========================================================================
**  Function : Clock_InitXO
** =========================================================================*/
/*
  See ClockApps.h
*/

int test_Clock_XORemote(void)
{

   npa_client_handle reqClient = NULL;
   reqClient =  npa_create_sync_client("/clkregim/apps/xo/cxo",  "NPA/RPM Test", NPA_CLIENT_REQUIRED); 

   if ( NULL == reqClient)
   {
      /*-----------------------------------------------------------------------*/
      /* Send initial vote of "1" to keep XO in the active set.                */
      /*-----------------------------------------------------------------------*/

      npa_remote_define_resource(&Clock_XORemote, 1, NULL);

   }
   else
   {
      npa_issue_required_request(reqClient, 1);   //Sends the driver a 1
   }

  return 0;

} /* END Clock_InitXO*/

