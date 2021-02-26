#ifndef __RPMCOMDEF_H__
#define __RPMCOMDEF_H__

/*=========================================================================

                        RPM COMMON   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains types and definitions used by the RPM.

      Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                       All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/12/15   rli      Remove duplicate typedef of offsetof
01/01/15   bh       Removed duplicate typdef of uint64_t
06/13/14   rli      Fix for LLVM 3.5
05/30/14   rli      Add LLVM support
03/21/14   rli      Fix 64bit compiler warnings.
03/13/14   rli      Updated for 64bit support.
04/11/13   rli      Initial release.

==========================================================================*/

#include <bits/alltypes.h>
#include <stddef.h>

#ifndef true
  #define true 1
#endif


#ifndef false
  #define false 0
#endif

#ifndef _BOOL_DEFINED
    typedef  unsigned char      bool;
    #define _BOOL_DEFINED
#endif
 
static inline unsigned long PtrToUlong( const void  *p )
{
    return((unsigned long) p );
}

#endif  /* __RPMCOMDEF_H__ */

