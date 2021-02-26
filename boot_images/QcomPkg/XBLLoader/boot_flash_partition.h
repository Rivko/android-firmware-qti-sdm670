#ifndef BOOT_FLASH_PARTITION_H
#define BOOT_FLASH_PARTITION_H

/*=============================================================================

                       Boot Loader Flash Partition

GENERAL DESCRIPTION
  This file implements functions and data used to access partitions 
  for flash parts

Copyright 2007, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
12/04/07   MJS     Ported from MSM7800.

=============================================================================*/

/*=============================================================================
 
                           INCLUDE FILES

=============================================================================*/
#include "boot_comdef.h"
#include "miparti.h"

/*=============================================================================

                      PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_flash_find_partition_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function finds the requested partition table entry within
*   the partition table and returns a pointer to it. 
* 
* @param[in] parti_tbl_ptr Partition table pointer
* @param[in] parti_entry_name Name of partition table entry to find
*											   
* @par Dependencies
*   Partition table must be loaded in RAM.
* 
* @retval
*   Pointer to partition table entry or NULL if not found.
* 
* @par Side Effects
*   Executes the error handler and never returns if an error is detected.
* 
*/
flash_partentry_ptr_type boot_flash_find_partition_entry
(
  flash_partable_t parti_tbl_ptr,           /* Partition table pointer */
  const char *parti_entry_name              /* Name of partition table entry
                                               to find */
);

#endif /* BOOT_FLASH_PARTITION_H */

