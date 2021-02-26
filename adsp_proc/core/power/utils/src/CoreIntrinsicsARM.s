/*============================================================================
@file CoreIntrinsicsARM.s

This file provides the intrinsic function clz for counting
leading zeros.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreIntrinsicsARM.s#1 $
============================================================================*/
  EXPORT __ldrex
  EXPORT __strex
  EXPORT CoreCountLeadingZeros32
  EXPORT __irqs_locked

  AREA  CORE_INTRINSICS, CODE, READONLY
  CODE32

__ldrex
  ldrex   r0, [r0]
  bx      lr

__strex
  strex   r2, r0, [r1]
  mov     r0, r2
  bx      lr

CoreCountLeadingZeros32
  clz     r0, r0
  bx      lr

__irqs_locked
  mrs r0, cpsr
  and r0, r0, #0x80
  bx  lr

  END

