#ifndef RPMH_ARC_MOL_H
#define RPMH_ARC_MOL_H
/*===========================================================================
                             rpmh_arc_mol.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

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
#include <string.h>
#include "rpmh_arc.h"
#include "pwr_utils_lvl.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================
/**
 * <!-- rpmh_arc_mol_get -->
 *
 * @brief Returns the MOL for the given arc. This MOL is typically dependent
 *        on the target.
 *
 * @param arc - the ARC 
 *
 * @return the MOL value, as a VLVL
 */

uint32 rpmh_arc_mol_get (rpmh_arc_enum_t arc);

#endif // RPMH_ARC_MOL_H
