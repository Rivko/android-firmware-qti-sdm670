#ifndef BOOT_BUILD_COMPONENT_H
#define BOOT_BUILD_COMPONENT_H

/*===========================================================================

                    Boot Build Component Header File

GENERAL DESCRIPTION
  Contains Boot build component defintion used by BOOT module.

Copyright 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/16   kpa     Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Build flags for image determination */
typedef enum {
    SBL_BUILD_SBL,
    SBL_BUILD_DEVICEPROGRAMMER_LITE,
    SBL_BUILD_DEVICEPROGRAMMER_DDR,
    SBL_BUILD_PMIC,
    SBL_BUILD_JTAG,
    SBL_BUILD_DDRDEBUGIMAGE
} sbl_build_type;


#endif /* BOOT_BUILD_COMPONENT_H */
