/*===========================================================================
                              rpmh_arc_mol.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rpmh_arc_mol.h"
#include "CoreVerify.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static const uint32 arc_to_mol[RPMH_ARC_NUM] =
  {
    RAIL_VOLTAGE_LEVEL_MIN_SVS,     // CX
    RAIL_VOLTAGE_LEVEL_NOM,         // MX
    RAIL_VOLTAGE_LEVEL_MIN_SVS,     // EBI
    RAIL_VOLTAGE_LEVEL_MIN_SVS,     // LPI_CX
    RAIL_VOLTAGE_LEVEL_SVS,         // LPI_MX
    RAIL_VOLTAGE_LEVEL_MIN_SVS,     // GFX
    RAIL_VOLTAGE_LEVEL_MIN_SVS,     // MSS
    DDRSS_LEVEL_ACTIVE,             // DDR_SS
    XO_LEVEL_PMIC_BUFFER_OFF        // XO
  };
    
    
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

uint32 rpmh_arc_mol_get (rpmh_arc_enum_t arc)
{
  CORE_VERIFY (arc < RPMH_ARC_NUM);
  
  uint32 mol = arc_to_mol[arc];
  
  return mol;
}
