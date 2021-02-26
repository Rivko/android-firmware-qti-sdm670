/*==============================================================================
@file CoreIntrinsics.h

Intrinsics for common assembly operations

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreIntrinsics.h#1 $
==============================================================================*/
#ifndef COREINTRINSICS_H
#define COREINTRINSICS_H

/* Count leading zeros:
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0204i/Cihjgjed.html
 */

/* guard against RVCT versions that already have this intrinsic, or other known definers */
#if defined(__arm__) && ( __ARMCC_VERSION < 300000 ) && !defined( __RVCT_CLZ_H__ )
  /* define the other header's name, just in case they try to follow us */
  #define __RVCT_CLZ_H__
  unsigned char __clz(unsigned int val);
  #define CoreCountLeadingZeros32 __clz
#else
  /* __builtin_clz is a GNU extension which is provided on some ARM and Q6
     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0348a/CJAEJGJD.html
     Hexagon GNU C/C++ Compiler - A GNU Manual http://www.codeaurora.org
  */
  #define CoreCountLeadingZeros32 __builtin_clz
#endif 

/* Load word exclusive:
 * http://www.keil.com/support/man/docs/armasm/armasm_cihbghef.htm
 *
 * This version of the intrinsic isn't as cool as the compiler-native one, and
 * can only generate word-sized loads. */
unsigned int  __ldrex(volatile void *ptr);

/* Store word exclusive:
 * http://www.keil.com/support/man/docs/armasm/armasm_cihbghef.htm
 *
 * This version of the intrinsic isn't as cool as the compiler-native one, and
 * can only generate word-sized stores. */
int __strex(unsigned int val, volatile void *ptr);

/* returns nonzero if irq-level interrupts are locked */
unsigned int __irqs_locked(void);

#endif /* COREINTRINSICS_H */
