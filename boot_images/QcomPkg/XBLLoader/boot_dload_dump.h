#ifndef DLOAD_DEBUG_DUMP_H
#define DLOAD_DEBUG_DUMP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              M E M O R Y   D E B U G   I N F O R M A T I O N 

GENERAL DESCRIPTION
  This module dumps security memory region to destinition address

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/XBLLoader/boot_dload_dump.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   digant  definition for boot_dload_is_security_region_dump_allowed
06/24/15   kpa     Initial Creation.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "boot_sbl_shared.h"

/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
                      
===========================================================================*/


/*===========================================================================
**  Function :  boot_dload_dump_security_regions
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will dump backup qsee and qhee regions according to 
*   information passed by qsee/qhee's shared imem cookies, so they can be 
*   ramdumped later.
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   Called only in dload path
* 
*/
void boot_dload_dump_security_regions(struct bl_shared_data_type *bl_shared_data );

/*===========================================================================
**  Function :  boot_dload_is_security_region_dump_allowed
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine if sbl can dump tz in dload mode
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
*/
boot_boolean boot_dload_is_security_region_dump_allowed(bl_shared_data_type *bl_shared_data);

#endif  /* DLOAD_DEBUG_DUMP_H */
