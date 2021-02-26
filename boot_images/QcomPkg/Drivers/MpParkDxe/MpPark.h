/** @file
  Contains common definitions for the MpPark Protocol

  Copyright (c) 2011-2012, 2014 Qualcomm Technologies, Inc. All rights reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/08/14   rli     Updated to support AARCH64. Remove timers.
 06/26/14   rli     Removed SCTLR, ACTLR, DACR from MPSTART_ARGS
 11/28/12   aus     Moved timeout definition to .c file
 01/18/12   niting  Added cold boot timer sync.
 06/15/11   cwg     Initial version.

=============================================================================*/
#ifndef _MPPARK_H_
#define _MPPARK_H_

#define MPSTART_RESERVED_SIZE  10-(sizeof(UINTN)-4)/2
//*************************************************************
//  Structures
//*************************************************************
typedef struct
{
    UINT64 ProcessorId;      // Processor ID
    UINT64 JumpAddress;      // Jump Address
    UINT32 OSReserved[508];

    UINT32 MailboxCode[500];
    UINT32 Reserved[MPSTART_RESERVED_SIZE]; ////aarch32: Reserved[10]; aarch64: Reserved[8]
    UINTN Callback;         // Cpu init C code
    UINTN StackTop;         // STACK TOP

} MPSTART_ARGS;


extern 
VOID
StartAuxiliaryCPU (
  VOID
  );

extern 
VOID
StartAuxiliaryCPU_END (
  VOID
  );

extern
VOID
MpParkMailbox (
  VOID
  );

extern
VOID
MpParkMailbox_END (
  VOID
  );

extern 
VOID
MpParkInitAddresses (
  VOID
  );

typedef
VOID
(*MAILBOX) (
  VOID
  );

#endif
