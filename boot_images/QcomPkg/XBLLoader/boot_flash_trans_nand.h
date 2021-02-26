#ifndef BOOT_FLASH_TRANS_NAND_H
#define BOOT_FLASH_TRANS_NAND_H

/*===========================================================================

                                Boot Loader
                      NAND Flash Translation Module
                                Header File

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot-specific NAND flash translation module/layer, and declarations
  for the public functions in the module.

Copyright 2010, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
09/21/10   plc     Initial port in to sb3.0

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

#define BOOT_FLASH_TRANS_NAND_VERSION 1
#define BOOT_FLASH_TRANS_NAND_NAME "boot_flash_trans_nand"

#define BOOT_FLASH_TRANS_NAND_FACTORY_VERSION 1
#define BOOT_FLASH_TRANS_NAND_FACTORY_NAME "boot_flash_trans_nand_factory"

/*=========================================================================

**  Macro :  bfa_nand_verify

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
#ifndef BFA_NAND_VERIFY
#define BFA_NAND_VERIFY( cond ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return FALSE; \
    } \
  } while(0)
#endif /* BFA_NAND_VERIFY */

/*=========================================================================

**  Macro :  bfa_nand_verify2

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
#ifndef BFA_NAND_VERIFY2
#define BFA_NAND_VERIFY2( cond, ret_val ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return (ret_val); \
    } \
  } while(0)
#endif /* BFA_NAND_VERIFY2 */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_trans_nand_factory_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the NAND boot translation factory.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the NAND translation factory interface
* 
* @par Side Effects
*   None
* 
*/
boot_flash_trans_factory_if_type* boot_flash_trans_nand_factory_init
(
  void
);

#endif /* BOOT_FLASH_TRANS_NAND_H */

