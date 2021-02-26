#ifndef CLOCK_TARGET_H
#define CLOCK_TARGET_H
/*
===========================================================================
*/
/**
  @file clock_target.h
  @brief Public interface include file for accessing the target specific clock driver.

  The clock_target.h file is the target specific public API interface to the clock driver.
*/

/*
  ===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/systemdrivers/clock/sdm670/clock_target.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  02/13/18   bc      Initial version to separate target specific definitions.
  ===========================================================================
*/

#define CLOCK_SMEM_VERSION 1

/**
 * Clock config setting CP data from XBL
 */
typedef struct
{
  uint8 nSHUB;        // System HUB
  uint8 nSNOC;        // System NOC
  uint8 nCNOC;        // Config NOC
  uint8 nCDSP;        // CDSP NOC
  uint8 nCE;          // Crypto Engine
  uint8 nIPA;         // IPA
  uint8 nDDR;         // DDR
  uint8 nReserved;   // Reserved
} ClockCfgCPType;

#endif /* CLOCK_TARGET_H */
