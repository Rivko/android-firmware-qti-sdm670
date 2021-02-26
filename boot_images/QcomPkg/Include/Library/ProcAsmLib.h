/** @file ProcAsmLib.h
   
  Contains generic Assembly functions interface.

  Copyright (c) 2012 - 2013, 2015 Qualcomm Technologies, Inc. All rights reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/09/16   bh      Add function to get stack pointer
 09/14/15   bh      Add reg read functions
 10/22/13   vk      Add WriteCNTKCTL
 03/25/13   nk      Add ETM clock enable 
 03/13/13   yg      Rename API
 03/03/13   yg      Access to SMP register
 12/19/11   yg      Add API to get platform type

=============================================================================*/
#ifndef __PROCASMLIB_H__
#define __PROCASMLIB_H__

#include "Uefi.h"

/*
 * Read PMNC Register
 */
UINT32 ReadPMonCtlReg (void);

/*
 * Write PMNC Register
 */
void WritePMonCtlReg(UINT32 Val);

/*
 * Read USEREN Register
 */
UINT32 ReadUserEnReg(void);

/*
 * Write USEREN Register
 */
void WriteUserEnReg(UINT32 Val);

/*
 * Read CNTENS Register
 */
UINT32 ReadCntEnSetReg(void);

/*
 * Write CNTENS Register
 */
void WriteCntEnSetReg(UINT32 Val);

/*
 * Read CNTENC Register
 */
UINT32 ReadCntEnClrReg(void);

/*
 * Write CNTENC Register
 */
void WriteCntEnClrReg(UINT32 Val);

/*
 * Read PMNXSEL Register
 */
UINT32 ReadPerfCntSelReg(void);

/*
 * Write PMNXSEL Register
 */
void WritePerfCntSelReg(UINT32 Val);

/*
 * Read CCNT Register
 */
UINT32 ReadCycleCntReg(void);

/*
 * Write CCNT Register
 */
void WriteCycleCntReg(UINT32 Val);

/*
 * Read EVTSEL Register
 */
UINT32 ReadEvtSelReg(void);

/*
 * Write EVTSEL Register
 */
void WriteEvtSelReg(UINT32 Val);

/*
 * Read TPIDRURW Register
 */
UINT32 ReadTPIDRURWReg(void);

/*
 * Write TPIDRURW Register
 */
void WriteTPIDRURWReg(UINT32 Val);

/*
 * Read TPIDRURO Register
 */
UINT32 ReadTPIDRUROReg(void);

/*
 * Write TPIDRURO Register
 */
void WriteTPIDRUROReg(UINT32 Val);

/* Read ACTLR Register */
UINT32 ReadACTLR(VOID);

/* Write ACTLR Register */
VOID WriteACTLR(UINT32 Val);

/* Read CPMR Register */
UINT32 ReadCPMR(VOID);

/* Write CPMR Register */
VOID WriteCPMR(UINT32 Val);

/* Write ETMCR Register*/
VOID WriteETMCR(UINT32 Val);

/* Write CNTKCTL Register */
VOID WriteCNTKCTL(UINT32 Val);

/* Read CPSR A bit Register*/
UINT32 ReadABit(VOID);

/* Read DACR Register*/
UINT32 ReadDACRReg(VOID);

/* Returns stack pointer */
VOID * GetStackPointer(VOID);

#endif /* __PROCASMLIB_H__ */
