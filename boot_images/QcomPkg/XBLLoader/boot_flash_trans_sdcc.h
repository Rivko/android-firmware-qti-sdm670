#ifndef BOOT_FLASH_TRANS_SDCC_H
#define BOOT_FLASH_TRANS_SDCC_H

/*=============================================================================
                           
                Boot SD and MMC Flash Translation Module

GENERAL DESCRIPTION
  This header file contains the implementation for the SDCC/eMMC/MicroSD
  flash translation module/layer.

Copyright 2009, 2010, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
08/04/10   aus     Clean-up for secboot 3
08/31/09   VTW     Created.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_trans_if.h"
#include "boot_module_base.h"
#include "boot_msm.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_TRANS_SDCC_VERSION 1
#define BOOT_FLASH_TRANS_SDCC_NAME "boot_flash_trans_sdcc"

#define BOOT_FLASH_TRANS_FACTORY_SDCC_VERSION 1
#define BOOT_FLASH_TRANS_FACTORY_SDCC_NAME "boot_flash_trans_sdcc_factory"

/*=========================================================================

**  Macro :  bfa_sdcc_verify

** ==========================================================================
*/
/*!
* 
* @brief
*   Macro to verify a condition and transition to an error state if the
*   condition is not met.  Default error bahavior is to return FALSE to
*   the caller.  This behavior can be over-ridden by re-defining the macro
*   in BOOT_MSM_H.
* 
* @par Dependencies
*   None
*   
* @retval
*   FALSE if the condition was not met.
* 
* @par Side Effects
*   None
* 
*/
#ifndef BFA_SDCC_VERIFY
#define BFA_SDCC_VERIFY( cond ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return FALSE; \
    } \
  } while(0)
#endif /* BFA_SDCC_VERIFY */

/*=========================================================================

**  Macro :  bfa_sdcc_verify2

** ==========================================================================
*/
/*!
* 
* @brief
*   Macro to verify a condition and transition to an error state if the
*   condition is not met.  Default bahavior is to return the value "retval"
*   given as a parameter to the caller.  This form of the macro is used
*   by routines that do not return boolean.  The default behavior can
*   be over-ridden by re-defining the macro in BOOT_MSM_H.
* 
* @par Dependencies
*   None
*   
* @retval
*   The error value passed as parameter if the condition was not met.
* 
* @par Side Effects
*   None
* 
*/
#ifndef BFA_SDCC_VERIFY2
#define BFA_SDCC_VERIFY2( cond, ret_val ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return (ret_val); \
    } \
  } while(0)
#endif /* BFA_SDCC_VERIFY2 */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_trans_sdcc_factory_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the SDCC boot translation factory.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the SDCC translation factory interface
* 
* @par Side Effects
*   None
* 
*/
boot_flash_trans_factory_if_type* boot_flash_trans_sdcc_factory_init
(
  void
);

#endif /* BOOT_FLASH_TRANS_SDCC_H */

