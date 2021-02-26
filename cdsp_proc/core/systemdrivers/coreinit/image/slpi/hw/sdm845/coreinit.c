/**
@file  coreinit.c

@brief Core default resource voting driver.
        
===========================================================================
                             Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/coreinit/image/slpi/hw/sdm845/coreinit.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/04/05   sds     Add additional resources
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
#include "npa.h"
#include "icbarb.h"
#include "pmapp_npa.h"
#include "voltage_level.h"
#include "stdint.h"
#include "rpmh_client.h"
#include "cmd_db.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct {
  npa_client_handle  handle;
  const char *       name;
  npa_client_type    client_type;
  npa_resource_state vote;
} coreinit_npa_vote_t;

typedef struct {
  const char *       name;
  RSCSW_DRV_MAPPING  rsc;
  rpmh_client_handle handle;
  uint32_t           addr;
  uint32_t           offset;
  uint32_t           active_vote;
  uint32_t           sleep_vote;
} coreinit_rpmh_vote_t;

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static npa_client_handle config_handle = NULL;
static const char *resource_deps[] = { "/icb/arbiter",
                                       "/pmic/client/xo",
                                       "/vcs/vdd_cx",
                                       "/vcs/vdd_mx",
                                       "/vcs/vdd_ssc_mx" };
static coreinit_npa_vote_t npa_resources[] = 
{
  [0] =
  {
    .name        = "/pmic/client/xo",
    .client_type = NPA_CLIENT_SUPPRESSIBLE,
    .vote        = PMIC_NPA_MODE_ID_CLK_CXO_XO_ON,
  },
  [1] =
  {
    .name        = "/vcs/vdd_cx",
    .client_type = NPA_CLIENT_SUPPRESSIBLE,
    .vote        = RAIL_VOLTAGE_LEVEL_MIN_SVS,
  },
  [2] =
  {
    .name        = "/vcs/vdd_mx",
    .client_type = NPA_CLIENT_SUPPRESSIBLE,
    .vote        = RAIL_VOLTAGE_LEVEL_SVS,
  },
  [3] =
  {
    .name        = "/vcs/vdd_ssc_mx",
    .client_type = NPA_CLIENT_REQUIRED,
    .vote        = RAIL_VOLTAGE_LEVEL_NOM,
  },
};

static coreinit_rpmh_vote_t rpmh_resources[] = 
{
  [0] =
  {
    .name        = "vrm.soc",
    .rsc         = RSC_DRV_SENSORS,
    .offset      = 0x4, /* VRM enable offset */
    .active_vote = 0x1,
    .sleep_vote  = 0x0,
  },
};

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */
static void coreinit_init_cb
(
  void         *context,
  unsigned int  event_type, 
  void         *data,       
  unsigned int  data_size
)
{
  /* Unused parameters */
  (void)context;
  (void)event_type;
  (void)data;
  (void)data_size;

  /* Path to CNOC. */
  ICBArb_MasterSlaveType routes[] =
  {
    {
      ICBID_MASTER_SENSORS_PROC,
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

  /* Handle npa default requests. */
  for( uint32_t i = 0; i < ARRAY_SIZE(npa_resources); i++ )
  {
    npa_resources[i].handle = npa_create_sync_client( npa_resources[i].name,
                                              "Core Init Default Client",
                                              npa_resources[i].client_type );

    /* Issue the request if we successfully created the handle. */
    if( NULL != npa_resources[i].handle )
    {
      npa_issue_scalar_request( npa_resources[i].handle, npa_resources[i].vote );
    }
  }

  /* Handle rpmh default requests. */
  for( uint32_t i = 0; i < ARRAY_SIZE(rpmh_resources); i++ )
  {
    rpmh_resources[i].handle = rpmh_create_handle(rpmh_resources[i].rsc, "Core Init Default Client");
    rpmh_resources[i].addr = cmd_db_query_addr( rpmh_resources[i].name );

    /* Issue the request if we successfully created the handle and obtained an address. */
    if( NULL != rpmh_resources[i].handle && 
        0    != rpmh_resources[i].addr )
    {
      uint32_t msg_id = 0;
      rpmh_command_set_t command_set;

      /* Fill out rpmh command set. One command per. */
      memset(&command_set, 0, sizeof(command_set));
      command_set.num_commands        = 1;
      command_set.commands[0].address = rpmh_resources[i].addr +
                                        rpmh_resources[i].offset;

      /* Active vote. */
      command_set.set                    = RPMH_SET_ACTIVE;
      command_set.commands[0].completion = TRUE;
      command_set.commands[0].data       = rpmh_resources[i].active_vote;
      msg_id = rpmh_issue_command_set(rpmh_resources[i].handle, &command_set);

      /* Sleep vote */
      command_set.set                    = RPMH_SET_SLEEP;
      command_set.commands[0].completion = FALSE;
      command_set.commands[0].data       = rpmh_resources[i].sleep_vote;
      (void)rpmh_issue_command_set(rpmh_resources[i].handle, &command_set);

      /* Wait for the active vote to complete. */
      rpmh_barrier_all(rpmh_resources[i].handle, msg_id);
    }
  }
}

/** 
@copydoc coreinit_init
*/
void coreinit_init( void )
{
  npa_resources_available_cb( NPA_ARRAY(resource_deps), coreinit_init_cb, NULL );
}


