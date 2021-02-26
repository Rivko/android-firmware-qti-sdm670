#ifndef _CIL_RAMDUMP_H
#define _CIL_RAMDUMP_H

/*===========================================================================

                  Crypto Interface Library API's for XBL Ramdump

DESCRIPTION
  Contains wrapper definition for Crypto Interface Library 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
25/04/17   bd      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <boot_dload_debug.h>


/*===========================================================================

**  Function :  cil_md_crypto_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Perform encryption initiliazation
*   
*
* @par Dependencies   
*   
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_crypto_init(void);

/*===========================================================================

**  Function :  cil_md_encrypt_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Perform in-place encryption of the given region
*
* @par Dependencies   
*   
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_encrypt_region(uint64 r_base, uint64 r_len, uint32 r_num);

/*===========================================================================

**  Function :  cil_md_add_unencr_regions

** ==========================================================================
*/
/*!
* 
* @brief
*   Adds the encryption key regions to debug table
*
* @par Dependencies
*   This api Must be called AFTER cil_md_encrypt_region has been called for all regions
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_add_unencr_regions(void);


#endif  /* _CIL_RAMDUMP_H */