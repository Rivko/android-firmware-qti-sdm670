#ifndef BOOT_COMDEF_H
#define BOOT_COMDEF_H

/*===========================================================================

                 Boot Common Definition Header File

GENERAL DESCRIPTION
  This header file contains common declarations and definitions used
  throughout the boot software.
   
Copyright 2004-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/14   kedar   Use com_dtypes.h instead of comdef.h
11/06/13   plc     Add SECTION() macro supporting linker section assignment
01/29/10   as      Removed definitions as they are defined in comdef.h
12/04/09   as      Added boot_boolean in !BUILD_BOOT_CHAIN 
11/20/09   as      Add boot_boolean as uint32 boolean data type
07/29/09   dk      Added #define _BOOLEAN_DEFINED to avoid mismatch with 
                   other comdef file inclusions.
01/07/08   MJS     Merged 6xxx and 7xxx versions.
04/11/05   SJD     Changed to only define LOCAL if it isn't already defined.
08/25/04   SJD     Initial revision.

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "com_dtypes.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* boot_boolean data type used for boolean variables shared with pbl*/
typedef  unsigned int       boot_boolean;     /* uint32 Boolean value type. */

#define SECTION(__name) __attribute__((section(__name)))

#endif /* BOOT_COMDEF_H */
