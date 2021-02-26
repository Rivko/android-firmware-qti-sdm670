#ifndef RPMH_COMMON_H
#define RPMH_COMMON_H

/*===========================================================================
                             rpmh_common.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================
#include "comdef.h"
#include "rpmh_drv.h"

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/*
 *  The "set" concept associated with making RPMh requests.  
 */
typedef enum
{
  RPMH_SET_ACTIVE = 0, // Commands for this set are applied when the subsystem is active
  RPMH_NUM_SETS
  //SLEEP and WAKE are not supported on AOP	  
  //RPMH_SET_SLEEP = 1,  // Commands for this set are applied when the subsystem goes to sleep
  //RPMH_SET_WAKE = 2,   // Commands for this set are applied when the subsystem wakes up
} rpmh_set_enum;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

#endif /* RPMH_COMMON_H */

