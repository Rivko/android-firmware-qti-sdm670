#ifndef DRV_MAPPING_INTERNAL_H
#define DRV_MAPPING_INTERNAL_H
/*===========================================================================
                             drv_mapping_internal.h

DESCRIPTION:
  DRV MAPPING structure definition. This lists the v1 -> other version drv_mapping
  if it exists

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "comdef.h"
#include "drv_lookup.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define NUM_VERSIONS 0

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
uint32 drv_version_mapping[NUM_VERSIONS][RSC_DRV_GLOBAL_MAX];

//===========================================================================
//                             Function Definitions
//===========================================================================

#endif // DRV_MAPPING_INTERNAL_H

