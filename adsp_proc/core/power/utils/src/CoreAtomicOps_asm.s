/*============================================================================
@file CoreAtomicOps_asm.s

Implements an atomic increment or decrement in ARM assembly

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreAtomicOps_asm.s#1 $
============================================================================*/

.global Core_AtomicInc
.global Core_AtomicDec

#------------------------------------------------------
#               Atomic Increment Function
#------------------------------------------------------
Core_AtomicInc:
    PUSH {r1-r2, LR}

incretry_l:
	LDREX r1, [r0]
	ADD r1, r1, #1
	STREX r2, r1, [r0]
	CMP r2, #0
	BNE incretry_l
	MOV r0, r1
#	DMB

	POP    	{r1-r2, PC}

#------------------------------------------------------
#               Atomic Decrement Function
#------------------------------------------------------
Core_AtomicDec:
	PUSH {r1-r2, LR}

decretry_l:
	LDREX r1, [r0]
	SUB r1, r1, #1
	STREX r2, r1, [r0]
	CMP r2, #0
	BNE decretry_l
	MOV r0, r1
#	DMB

	POP {r1-r2, PC}

#------------------------------------------------------
