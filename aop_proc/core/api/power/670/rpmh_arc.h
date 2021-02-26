#ifndef RPMH_ARC_H
#define RPMH_ARC_H
/*===========================================================================
                             rpmh_arc.h

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


//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef enum rpmh_arc_enum_t
{
  RPMH_ARC_VDD_CX  =  0,               // SOC Logic
  RPMH_ARC_VDD_MX  =  1,               // SOC Memory
  RPMH_ARC_VDD_EBI =  2,               // DDR Phy Analog
  RPMH_ARC_SSC_CX  =  3,               // Sensors Island Logic
  RPMH_ARC_SSC_MX  =  4,               // Sensors Island Memory
  RPMH_ARC_VDD_GFX =  5,               // Graphics
  RPMH_ARC_VDD_MSS =  6,               // Modem
  RPMH_ARC_DDR_SS  =  7,               // DDR Power State
  RPMH_ARC_XO      =  8,               // Clock Buffer

  RPMH_ARC_LAST = RPMH_ARC_XO,
  RPMH_ARC_NUM
} rpmh_arc_enum_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================


#endif // RPMH_ARC_H
