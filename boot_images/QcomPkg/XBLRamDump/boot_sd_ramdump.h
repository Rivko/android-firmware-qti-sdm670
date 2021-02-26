#ifndef BOOT_SD_RAMDUMP_H
#define BOOT_SD_RAMDUMP_H

/*===========================================================================

                         Boot SD RAM Dump Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions  for  functions for taking memory
  dumps to SD card.

Copyright 2010,2011,2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/14   ck      Removed bl_shared_data from parameter list
04/05/11   dh      do not include customer.h
7/22/10   Kedar        Initial version . 

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

#include "boot_sdcc.h"
#include "boot_sbl_shared.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


typedef struct boot_ramdump_errlog_data_type
{
   int err_no; /* EFS error returned by efs routines */
   char filename[MAX_FILE_NAME]; /* File in which error has occured */

} boot_ramdump_errlog_data_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_ram_dumps_to_sd_card

** ==========================================================================
*/
/*!
* 
* @brief  
*   This routine initiates the Ramdumps to SD card. Checks for the Ramdump 
*   cookie file and go ahead with Ramdumps to SD card if cookie file is present.
*   Setup the Ram regions to be taken as dumps.
*
* @param[in]
*   None
*
* @par Dependencies:
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_ram_dumps_to_sd_card(void);

#endif  /* BOOT_SD_RAMDUMP_H */

