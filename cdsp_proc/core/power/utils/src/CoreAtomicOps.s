/*============================================================================
@file CoreAtomicOps.c

Implements an atomic increment or decrement

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreAtomicOps.s#1 $
============================================================================*/
  PRESERVE8

  AREA  CORE_ATOMICOPS, CODE, READONLY
  CODE32
  
  ALIGN

  IF ({ARCHITECTURE} /= "7")    :LAND: \
     ({ARCHITECTURE} /= "7-A")  :LAND: \
     ({ARCHITECTURE} /= "7-R")  :LAND: \
     ({ARCHITECTURE} /= "7-M")  :LAND: \
     ({ARCHITECTURE} /= "6-M")  :LAND: \
     ({ARCHITECTURE} /= "6S-M")
    MACRO
    DMemBarrier
    MOV r1, #0
    MCR p15, 0, r1, c7, c10, 5
    MEND
  ELSE
    MACRO
    DMemBarrier
    DMB
    MEND
  ENDIF

Core_AtomicInc PROC
  EXPORT Core_AtomicInc
1 LDREX r1, [r0]
  ADD r1, r1, #1
  STREX r2, r1, [r0]
  CMP r2, #0
  BNE %b1
  MOV r0, r1
  DMemBarrier
  BX lr
  ENDP

Core_AtomicDec PROC
  EXPORT Core_AtomicDec
2 LDREX r1, [r0]
  SUB r1, r1, #1
  STREX r2, r1, [r0]
  CMP r2, #0
  BNE %b2
  MOV r0, r1
  DMemBarrier
  BX lr
  ENDP
  
  END

