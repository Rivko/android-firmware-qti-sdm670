/** @file HALsdcc_sdcc5.c

  SDCC(Secure Digital Card Controller) driver HAL Platform
  Specific API.
  This file implements the SDCC driver HAL components that are
  common to all implementations of the SDCC5 core

  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY


when        who     what, where, why
----------  ---     ----------------------------------------------------------- 
2017-05-12  sb      Cleanup  
2016-12-12  jt      Remove legacy code  
2016-01-18  eo      Reduced sdcc code size
2015-02-12  dg      Runtime fixes
2014-06-18  bn      Initial version. Branched from 8994 UEFI

=============================================================================*/

#include "HALhwio.h"
#include "SdccHal.h"
#include "HALsdcc_sdcc5.h"
#include "SdccBsp.h"

#define SDCC_REG_SLOT_1         0
#define SDCC_REG_SLOT_2         1
#define SDCC_REG_SLOT_3         2
#define SDCC_REG_SLOT_4         3

/*
 * Initilize the HWIO address table for access to hwio regs using the
 * drive number as the index:
 */
UINTN sdcc_reg_hc[HAL_SDCC_CONTROLLER_COUNT];
static boolean hwio_init_done   = FALSE;

void
HAL_sdhci_InitializeRegisters()
{
  sdcc_base_addr_type sdcc_base_address = {0, 0, 0, 0}; 

  if (hwio_init_done == TRUE)
  {
     return;
  }

  /* Get the base addresses from the target */
  sdcc_bsp_get_base_addr (&sdcc_base_address);

  /* Start to initialize hwio registers for slots 1-4 */
  sdcc_reg_hc[SDCC_REG_SLOT_1]   = sdcc_base_address.sdc1_base_hc;
  sdcc_reg_hc[SDCC_REG_SLOT_2]   = sdcc_base_address.sdc2_base_hc;
  sdcc_reg_hc[SDCC_REG_SLOT_3]   = sdcc_base_address.sdc3_base_hc;
  sdcc_reg_hc[SDCC_REG_SLOT_4]   = sdcc_base_address.sdc4_base_hc;

  hwio_init_done = TRUE;
}

/**
 * Overwrite SDHCi Capabilties 0 Register (supported from Aragorn v2)
 * In SDHCi mode, certain registers may be hard coded at design time
 * The vendor_capabilities registers allows S/W to overwrite the default values
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param capabilities
 *   The capabilities 0 to overwrite - see SDHCi spec
 *
 * @param value
 *   New capability value
 */
void
HAL_sdhci_OverwriteCaps0 (const uint32 nControllerId, 
                          HAL_capabilties_type Capabilities, uint32 nValue)
{
  return;
}

/**
 * Overwrite SDHCi Capabilties 1 Register (supported from Aragorn v2)
 * In SDHCi mode, certain registers may be hard coded at design time
 * The vendor_capabilities registers allows S/W to overwrite the default values
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param capabilities
 *   The capabilities 1 to overwrite - see SDHCi spec
 *
 * @param value
 *   New capability value
 */
void
HAL_sdhci_OverwriteCaps1 (const uint32 nControllerId, 
                          HAL_capabilties_type Capabilities, uint32 nValue)
{
  return;
}

