/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdlib.h>
#include <string.h>

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"
#include "DALSys.h"
#include "CoreVerify.h"

void acc_init(void);

railway_internals_t railway;
int mx_rail_id     = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int cx_rail_id     = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int ebi_rail_id    = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int lpi_mx_rail_id = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int lpi_cx_rail_id = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int wlan_rail_id   = RAIL_NOT_SUPPORTED_BY_RAILWAY;


static int railway_get_rail_id(const char *name)
{
    int id = rail_id(name);
    CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY!=id);
    return id;
}

static void railway_init_voter(int rail_id, railway_voter_id voter_id, railway_corner corner)
{
    railway_voter_t voter = railway_create_voter(rail_id, voter_id);
    railway_corner_vote(voter, corner);
}

void railway_init(void)
{
    uint32 num_rails = RAILWAY_CONFIG_DATA->num_rails;

    uint32 rail_state_size = num_rails * sizeof(railway_rail_state_t);

    DALResult dalResult = DALSYS_Malloc(rail_state_size, (void**)&railway.rail_state);
    CORE_VERIFY_PTR(railway.rail_state);
    CORE_VERIFY(dalResult==DAL_SUCCESS);

    memset(railway.rail_state, 0, rail_state_size);

    cx_rail_id     = railway_get_rail_id("vddcx");
    mx_rail_id     = railway_get_rail_id("vddmx");
    ebi_rail_id    = railway_get_rail_id("vdda_ebi");
    lpi_mx_rail_id = railway_get_rail_id("vdd_lpi_mx");
    lpi_cx_rail_id = railway_get_rail_id("vdd_lpi_cx");
    wlan_rail_id   = railway_get_rail_id("vdda_wlan");

    railway_transitioner_init(); // initialize all rails
    railway_target_init(); /* should be called after railway_transitioner_init
                            * since railway.rail_state[] are initialized with default values.
                            */

    //init time request for lowest active level
    railway_init_voter( cx_rail_id , RAILWAY_RPM_CX_VOTER_ID , RAILWAY_NOMINAL );
    railway_init_voter( mx_rail_id , RAILWAY_RPM_MX_VOTER_ID , RAILWAY_NOMINAL );
    /* 
     * EBI Needs to be able to scale to different voltages for DDR Training.
     * Remove the inital vote for EBI.
     */
    //railway_init_voter( ebi_rail_id, RAILWAY_RPM_EBI_VOTER_ID, RAILWAY_NOMINAL );

    //These rails are just to support PMIC driver to query corner voltage, and
    //Railway driver does not control voltage.
    //Don't expect to receive any vote on these rails.
    //railway_init_voter( ssc_mx_rail_id, RAILWAY_RPM_SSC_MX_VOTER_ID, RAILWAY_NO_REQUEST );
    //railway_init_voter( ssc_cx_rail_id, RAILWAY_RPM_SSC_CX_VOTER_ID, RAILWAY_NO_REQUEST );
    //railway_init_voter( wlan_rail_id  , RAILWAY_RPM_WLAN_VOTER_ID  , RAILWAY_NO_REQUEST );

//    acc_init();
}
