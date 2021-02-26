#ifndef AOP_IPC_H
#define AOP_IPC_H
/*===========================================================================
                             aop_ipc.h

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

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "msmhwio.h"
#include "aop_hwio.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef enum AOP_IPC_SubsystemType
{
  AOP_IPC_APPS_HLOS = HWIO_AOP_IPC_APCS_HLOS_IPC_SHFT,
  AOP_IPC_APPS_TZ   = HWIO_AOP_IPC_APCS_TZ_IPC_SHFT,
  AOP_IPC_APPS_HYP  = HWIO_AOP_IPC_APCS_HV_IPC_SHFT,
  AOP_IPC_LPASS     = HWIO_AOP_IPC_LPASS_IPC_SHFT,
  AOP_IPC_SSC       = HWIO_AOP_IPC_SSC_IPC_SHFT,
  AOP_IPC_MSS       = HWIO_AOP_IPC_MSS_IPC_SHFT,
  AOP_IPC_SPSS      = HWIO_AOP_IPC_SPSS_IPC_SHFT,
  AOP_IPC_GFX       = HWIO_AOP_IPC_GFX_IPC_SHFT,
  AOP_IPC_CAMERA    = HWIO_AOP_IPC_CAMSS_IPC_SHFT,
  AOP_IPC_CDSP      = HWIO_AOP_IPC_CDSP_IPC_SHFT
} AOP_IPC_SubsystemType;

typedef enum
{
  AOP_IPC_PrimaryInterrupt    = 0x1,
  AOP_IPC_SecondaryInterrupt  = 0x2
} AOP_IPC_InterruptType;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

static inline void AOP_IPC_SendInterrupt (AOP_IPC_SubsystemType subsystem, AOP_IPC_InterruptType isr_type)
{
  HWIO_OUT (AOP_IPC, isr_type << subsystem);
}
#endif // AOP_IPC_H
