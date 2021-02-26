/**
@file  coreinit.c

@brief Core default resource voting driver.
        
===========================================================================
                             Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/coreinit/image/cdsp/hw/sdm845/coreinit.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/01/06   sds     Initial revision

===========================================================================
           Copyright 2017 QUALCOMM Technologies, Incorporated.
                       All Rights Reserved.
                     QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "icbarb.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static npa_client_handle config_handle = NULL;

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/** 
@copydoc coreinit_init
*/
void coreinit_init( void )
{
  /* Path to CNOC. */
  ICBArb_MasterSlaveType routes[] =
  {
    {
      ICBID_MASTER_CDSP_PROC,
      ICBID_SLAVE_CLK_CTL
    },
  }; 


  /* Create our client. */
  config_handle = npa_create_sync_client_ex( "/icb/arbiter",
                                             "Core Init Default Client",
                                             NPA_CLIENT_SUPPRESSIBLE_VECTOR,
                                             sizeof(routes),
                                             &routes );

  /* Issue the request if we successfully created the handle. */
  if( NULL != config_handle )
  {
    /* Request for IB = 1. */
    ICBArb_RequestType requests[1];

    requests[0].arbType                  = ICBARB_REQUEST_TYPE_3;
    requests[0].arbData.type3.uIb        = 1;
    requests[0].arbData.type3.uAb        = 0;
    requests[0].arbData.type3.uLatencyNs = 0;

    npa_issue_vector_request( config_handle,
                              sizeof(requests)/sizeof(npa_resource_state),
                              (npa_resource_state *)requests );
  }
}

