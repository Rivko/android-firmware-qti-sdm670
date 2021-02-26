#ifndef BOOT_MSM_H
#define BOOT_MSM_H

/*===========================================================================

                      MSM-specific Common Boot Header File

GENERAL DESCRIPTION
  Contains target-specific defines related to common boot code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2007, 2009-2010, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/05/15   ck      Corrected copyright
05/02/14   kedara  Include HALbootHWIO.h instead of msmhwioreg.h 
08/04/10   aus     Clean-up for secboot 3
05/26/10   plc     Initial port to secboot 3.0 common code  
12/28/09   ty      Initial port to BOOT
11/07/07   ly      Create

============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "HALbootHWIO.h"
#include "msmhwio.h"
#include "boot_error_handler.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Map the ELF loader error handler to the SBL error handler */
#define BOOT_ELF_VERIFY( cond, err ) \
  BL_VERIFY( cond, err + BL_ERR_ELF_START )
#define BOOT_ELF_VERIFY2( cond, err, ret_val )  BOOT_ELF_VERIFY( cond, err )

/* Map the boot ELF loader error handler to the SBL error handler */
#define BOOT_SEC_ELF_VERIFY( cond, err )  BOOT_ELF_VERIFY( cond, err )
#define BOOT_SEC_ELF_VERIFY2( cond, err, ret_val )  BOOT_ELF_VERIFY2( cond, err, ret_val )

/* Map the NAND boot flash access error handler to the SBL error handler */
#define BFA_NAND_VERIFY( cond ) \
  BL_VERIFY( cond, BL_ERR_BOOT_FLASH_ACCESS|BL_ERROR_GROUP_BOOT )
#define BFA_NAND_VERIFY2( cond, ret_val )  BFA_NAND_VERIFY( cond )

/* Map the NOR boot flash access error handler to the SBL error handler */
#define BFA_NOR_VERIFY( cond )  BFA_NAND_VERIFY( cond )
#define BFA_NOR_VERIFY2( cond, ret_val )  BFA_NAND_VERIFY2( cond, ret_val )

/* Map the boot clobber error handler to the SBL error handler */
#ifndef BOOT_CLOBBER_VERIFY
#define BOOT_CLOBBER_VERIFY( cond ) \
  BL_VERIFY( cond, BL_ERR_CLOBBER_PROTECTION_VIOLATION|BL_ERROR_GROUP_BOOT )
#endif /* BOOT_CLOBBER_VERIFY */

/* Map boot clobber global address check to BOOT macro */
#ifndef BOOT_CLOBBER_GLOBAL_ADDR_CHECK
#define BOOT_CLOBBER_GLOBAL_ADDR_CHECK( addr ) \
  BOOT_VALID_ADDR_INC_IMEM( (addr), BL_ERR_IMG_DEST_PTR_INVALID )
#endif /* BOOT_CLOBBER_GLOBAL_ADDR_CHECK */

#ifdef BUILD_BOOT_CHAIN

#ifdef HWIO_LOCK
#undef HWIO_LOCK
#endif
#define HWIO_LOCK()

#ifdef HWIO_UNLOCK
#undef HWIO_UNLOCK
#endif
#define HWIO_UNLOCK()

#ifdef HWIO_INTLOCK
#undef HWIO_INTLOCK
#endif
#define HWIO_INTLOCK()

#ifdef HWIO_INTFREE
#undef HWIO_INTFREE
#endif
#define HWIO_INTFREE()

#endif /* BUILD_BOOT_CHAIN */

#endif /* BOOT_MSM_H*/
