/*===========================================================================

FILE:  secdbg_oem.c

DESCRIPTION:
  debugpolicy util functionality to skip serial num

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who                what, where, why
--------   ----               ------------------------------------------- 
10/19/15                       Init version.
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include <comdef.h>

/* Serial Number Flag */
//#define SKIP_SERIAL_NUMBER_CHECK 1

boolean is_sec_dbg_skip_serial_number(void)
{

#ifdef SKIP_SERIAL_NUMBER_CHECK
  return TRUE;
#else
 return FALSE;
#endif
}