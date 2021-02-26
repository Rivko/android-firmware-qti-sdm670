/*===========================================================================
                              railway_aggregator.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "railway.h"
#include "railway_internal.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "railway_config.h"
#include "CoreVerify.h"
#include "DALSys.h"
#include "cpr.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct railway_voter_s {
    railway_corner voltage_corner;
    int id;
    int rail;
    railway_voter_t voter_link;       //Need a better plan for the linked list.
} railway_voter_s;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
static railway_corners_t supported_corners;

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/*===========================================================================
FUNCTION: railway_create_voter

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
railway_voter_t railway_create_voter(int rail, int id)
{
  //Rail ID goes from 0 to (num_rails-1)
  CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
  //Store all the voters in a linked list. To do, make this nicer.
  railway_voter_t voter = NULL;
  
  DALResult dalResult = DALSYS_Malloc(sizeof(railway_voter_s), (void**)&voter);
  CORE_VERIFY_PTR(voter);
  CORE_VERIFY(dalResult==DAL_SUCCESS);
  
  voter->voltage_corner = RAILWAY_NO_REQUEST;
  voter->id = id;
  voter->rail = rail;
  voter->voter_link = railway.rail_state[rail].voter_list_head;   //Will set the first voter's link to NULL, after that it just pushes out the list.
  railway.rail_state[rail].voter_list_head = voter;
  return voter;
}

/*===========================================================================
FUNCTION: railway_corner_vote

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_corner_vote(railway_voter_t voter, railway_corner corner)
{
  CORE_VERIFY(voter);
  CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);
  voter->voltage_corner = corner;
}

/*===========================================================================
FUNCTION: railway_aggregated_voltage_target

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
railway_corner railway_aggregated_voltage_target(int rail)
{
    //Rail ID goes from 0 to (num_rails-1)
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);

    railway_corner target_corner = RAILWAY_NO_REQUEST;

    railway_voter_t voter = railway.rail_state[rail].voter_list_head;

    //Find highest corner vote.
    while (voter)
    {
        if(voter->voltage_corner > target_corner)
        {
            target_corner = voter->voltage_corner;
        }
        voter = (railway_voter_t)voter->voter_link;
    }

    return target_corner;
}

uint32 railway_get_corner_voltage(int rail, railway_corner corner)
{
    uint32 uv = 0;

    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);
    CORE_VERIFY(corner<RAILWAY_CORNERS_COUNT);

    if (corner == RAILWAY_RETENTION)
    {
        uv = railway.rail_state[rail].retention_voltage_uv;
    }
    else if (corner > RAILWAY_RETENTION)
    {
        uv = cpr_railway_get_voltage(rail, corner);
    }

    return uv;
}

railway_corners_t* railway_get_supported_corners(int rail)
{
    uint32 corner_count = 0;

    supported_corners.corners[0] = RAILWAY_NO_REQUEST;
    supported_corners.corners[1] = RAILWAY_RETENTION;

    cpr_railway_get_supported_corners(rail, &(supported_corners.corners[2]), &corner_count);

    supported_corners.corners_count = corner_count + 2;

    return &supported_corners;
}

/*===========================================================================
FUNCTION: railway_get_current_settings

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_get_current_settings(int rail, railway_settings* settings)
{
  CORE_VERIFY(rail < RAILWAY_CONFIG_DATA->num_rails);
  CORE_VERIFY_PTR(settings);

  memcpy(settings, &railway.rail_state[rail].current_active, sizeof(railway_settings));
}

/*===========================================================================
FUNCTION: railway_set_retention_voltage

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void railway_set_retention_voltage(int rail, uint32 microvolts)
{
    CORE_VERIFY(rail<RAILWAY_CONFIG_DATA->num_rails);

    railway.rail_state[rail].retention_voltage_uv = microvolts;
}

/*===========================================================================
FUNCTION: rail_id

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
int rail_id(const char* rail)
{
  const railway_rail_config_t *rail_data = NULL;
    
  for(int i = 0; i < RAILWAY_CONFIG_DATA->num_rails; ++i)
  {
    rail_data = &RAILWAY_CONFIG_DATA->rails[i];
    if(!strcmp(rail, rail_data->vreg_name))
      return i;
  }
    
  return RAIL_NOT_SUPPORTED_BY_RAILWAY;
}