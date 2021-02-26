#ifndef BOOT_MODULE_BASE_H
#define BOOT_MODULE_BASE_H

/*===========================================================================

                Boot Base Module Interface Header File

GENERAL DESCRIPTION
  This header file contains the definition of the base module interface.

Copyright 2008-2009, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
10/30/09   tnk     Defining the Boot_module_build_version if not given in build command
01/04/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#ifndef BOOT_MODULE_BUILD_VERSION
#define BOOT_MODULE_BUILD_VERSION 1100
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Constructs a uint32 with the module revision number */
#define BOOT_MODULE_VERSION( major, minor ) \
  ( ( ( (major) & 0xFF ) << 24 ) | \
    ( ( (minor) & 0xFF ) << 16 ) | \
    ( (BOOT_MODULE_BUILD_VERSION) & 0xFFFF ) )


/* The definition of the module versioning data structure */
typedef struct boot_module_version_type
{
  /* Pointer to a string representing the module name */
  const char *name;

  /* A uint32 representing the module version, contains a major, minor and
   * release number similar to the HAL version number */
  uint32 ver;

} boot_module_version_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* BOOT_MODULE_BASE_H */

