/*===========================================================================

FILE:  SecDbgHw.c

DESCRIPTION:
  Debug Policy HW dependent functionality

EXTERNALIZED FUNCTIONS


Copyright (c) 2014-2015 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who                what, where, why
--------   ----               ------------------------------------------- 
11/06/13   hw                  Init version.
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include <comdef.h>
#include "HALhwio.h"
#include "secboot_hwio.h"

/* Check if the debug policy fuse bit is set */
boolean sec_dbg_is_disable_fuse_bit_set()
{
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_BMSK 0x100000
  return 0 != HWIO_INM(QFPROM_RAW_OEM_CONFIG_ROW0_LSB, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_BMSK);
}

/* Set the override bits */
void sec_dbg_override_jtag()
{
  HWIO_OUT(OVERRIDE_1, HWIO_OVERRIDE_1_RMSK);
  HWIO_OUT(OVERRIDE_2, HWIO_OVERRIDE_2_RMSK);
  HWIO_OUT(OVERRIDE_3, HWIO_OVERRIDE_3_RMSK);
}

