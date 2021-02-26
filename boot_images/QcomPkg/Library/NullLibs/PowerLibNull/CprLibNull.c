/** @file CprLibNull.c
  
  Stub functions for CprLib

  Copyright (c) 2014-2016, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/17/15   kpa     Added cpr_cx_mx_settings_checksum
 05/14/15   plc     Update with newer apis post 8996
 09/24/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "railway.h"



/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

//Function to be called to initialize the CPR driver.
//Must be called after railway_init.
//Will do the necessary settling of voltages as required for any DDR-related
//training on the target.
void rbcpr_init(void)
{
}


//Function to be called after SMEM is initialized to push out the CPR settings
//to SMEM. These settings are to be picked up by the RPM CPR driver during boot.
//Must be done before the RPM FW execution begins.
void rbcpr_externalize_state(void)
{
}

void cpr_init(void)
{
}

void cpr_externalize_state(void)
{
}

uint32 cpr_railway_get_voltage(int railwayRailId, railway_corner railwayCorner)
{
    return 952000; /* return Turbo voltage */
}

void cpr_railway_get_supported_corners(int railwayRailId, railway_corner *corners, unsigned int *cornerCount )
{
    *cornerCount = 1;
    corners[0]   = RAILWAY_SUPER_TURBO;
}

uint32 cpr_cx_mx_settings_checksum(void)
{
  return 0;
}
