/*===========================================================================

FILE:  SecDbgHw.c

DESCRIPTION:
  Debug Policy HW dependent functionality

EXTERNALIZED FUNCTIONS


Copyright (c) 2015 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who                what, where, why
--------   ----               ------------------------------------------- 
05/18/15   plc                Initial version
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include <comdef.h>


/* Check if the debug policy fuse bit is set */
boolean sec_dbg_is_disable_fuse_bit_set()
{
  return FALSE;
}

/* Set the override bits */
void sec_dbg_override_jtag()
{
}

