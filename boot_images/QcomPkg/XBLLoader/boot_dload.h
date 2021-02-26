#ifndef BOOT_DLOAD_H
#define BOOT_DLOAD_H

/*===========================================================================

                        Boot Downloader
                          Header File 

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2012, 2014-2015 by Qualcomm Technologies Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/17   naa     Moved boot_dload_transition_pbl_forced_dload to its own file
09/05/17   yps     Added boot_backup_sp and boot_restore_sp
10/05/15   ck      Corrected copyright
02/19/15   kedara  Update dload cookie masks for version 1 
05/07/14   kedara  Include boot_comdef.h instead of comdef.h
01/08/14   ck      Added SBL_BOOT_MISC_DETECT_MASK to identify which bits SBL
                   owns in BOOT_MISC_DETECT register.
01/08/14   ck      Added SBL_DLOAD_MODE_BIT_MASK which is new download cookie
01/06/14   ck      Removed SBL_DLOAD_MAGIC_NUM_2 as Bear family uses 32 bit
                   register to hold dload magic number.
11/16/12   dh      Add boot_dload_set_cookie
09/29/11   dh      Add boot_dload_transition_enter_sahara
06/01/11   aus     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_emergency_dload.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                     PUBLIC TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* In Bear family SBL owns four bits in the BOOT_MISC_DETECT register.
   In this register one bit tells SBL to enter download mode.  The following
   mask and bit field definition dictate what bits SBL owns and which one
   enables download mode. */
#define SBL_BOOT_MISC_DETECT_MASK        0x000000F0
#define SBL_DLOAD_MODE_BIT_MASK          0x00000010
#define SBL_MINIDUMP_MODE_BIT_MASK       0x00000020

#define SBL_BOOT_MISC_DETECT_MASK_V1     0x00F00000
#define SBL_DLOAD_MODE_BIT_MASK_V1       0x00100000
#define SBL_DLOAD_MODE_BIT_MASK_V1_SHFT  20

/*!
 *  This function pointer is defined in each SBL* Bootloader to handle SBL-specific
 *  requirements to enter a download routine. It serves as an entry point of dload mode
 */
extern void (*sbl_dload_entry)(void);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  BOOT_DLOAD_ENTRY

DESCRIPTION
  Determines if identifier is present in IRAM to tell OEMSBL to enter
  the boot downloader, instead on continuing the normal boot process.
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates downloader should be entered.

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_entry( void );

/*===========================================================================

FUNCTION  BOOT_DLOAD_IS_DLOAD_MODE_SET

DESCRIPTION
This function shall return if the dload cookie is set or not for the
phone to go in download mode.

DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates cookies are set
  FALSE cookies are not set
  
SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_is_dload_mode_set( void );

/*===========================================================================

**  Function :  boot_init_for_dload

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls smem_boot_debug_init().
*  
* @param[in] bl_shared_data Pointer to shared data structure
*  
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_init_for_dload( bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  boot_dload_check

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will check to see if the downloader should be entered
*   for QPST download, and enter the downloader if directed to.
*  
* @param[in] bl_shared_data Pointer to the shared data 
*
* @par Dependencies
*   Download ID must be present in IRAM if downloader is to be entered.
* 
* @retval
*   None
* 
* @par Side Effects
*   Boot may be halted and QPST downloader entered.
* 
*/
void boot_dload_check( bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  boot_dload_transition_enter_sahara

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is implemented in each SBL to enter sahara dload mode
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_dload_transition_enter_sahara(void);


/*===========================================================================
**  Function :  boot_dload_set_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*   Set the SBL dload mode cookie
**        
* @par Dependencies
*  None
*   
*/
void boot_dload_set_cookie(void);

/*===========================================================================
**  Function :  boot_backup_sp
** ==========================================================================
*/
/*!
* 
* @brief
*   back up ramdump stack pointer
**    
* @param[in]pstack_sp point to backup address of stack pointer
*
* @par Dependencies
*  None
*   
*/
void boot_backup_sp(void* pstack_sp);

/*===========================================================================
**  Function :  boot_restore_sp
** ==========================================================================
*/
/*!
* 
* @brief
*   restore ramdump stack pointer
**        
* @param[in]pstack_sp  point to backup address of stack pointer
*
* @par Dependencies
*  None
*   
*/
void boot_restore_sp(void* pstack_sp);


/*===========================================================================
**  Function :  boot_smem_alloc_for_minidump
** ==========================================================================
*/
/*!
* 
* @brief
*   Allocate SMEM memory for minidump usage
**        
* @param[in] bl_shared_data Pointer to the shared data 
*
* @par Dependencies
*  None
*   
*/
void boot_smem_alloc_for_minidump(bl_shared_data_type* bl_shared_data);

/*===========================================================================
**  Function :  boot_dload_read_saved_cookie
** ==========================================================================
*/
/*!
* 
* @brief
*   Return the saved cookie value
**        
* @param[out] saved cookie value
*
* @par Dependencies
*  None
*   
*/
uint32 boot_dload_read_saved_cookie(void);


#endif /* BOOT_DLOAD_H */
