#ifndef RPMH_DRV_H
#define RPMH_DRV_H

/*===========================================================================
                             rpmh_drv.h

DESCRIPTION:
  Target specific DRV mapping

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================
#include "comdef.h"

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/**
* RPMh driver<-->RSC Hal numbering scheme, RSCSW_DRV_MAPPING, is used for communication between RPMh driver and Hal
* This mapping is a software concept and not same as Drv Id used in SWI. 
* However, this mapping scheme matches the AOSS DRV Id scheme.
* This is defined by HW, and must match the HW implementation.
*
**/

typedef enum {
    RSC_DRV_AUDIO         = 0,
    RSC_DRV_MAX,



    //virtual DRVs
    RSC_DRV_VIRTUAL_DRVS = 0x3FFFFF00,
    RSC_DRV_VIRTUAL_MAX = 0x3FFFFFFF,
} RSCSW_DRV_MAPPING;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

#endif /* RPMH_DRV_H */

