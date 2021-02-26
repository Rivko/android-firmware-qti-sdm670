#ifndef BOOT_RMB_H
#define BOOT_RMB_H

/*===========================================================================

                    BOOT EXTERN HSUSB DRIVER DEFINITIONS

DESCRIPTION
  Contains defines, declarations for sbl interactions using RMB registers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
08/13/12   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      DATA DECLARATIONS
===========================================================================*/
#ifdef FEATURE_BOOT_RMB_ENABLE

#define SUBSYS_PBL_SUCCESS 0x1

#define CMD_PMI_META_DATA_READY         0x1
#define CMD_PMI_SEGMENTED_LOAD_READY    0x2

/* MBA status register values */
#define RMB_REG_UNINITIALIZED                       0x0
#define MBA_XPU_UNLOCKED                            0x1
#define MBA_XPU_UNLOCKED_MEM_SCRIBBLED              0x2
#define MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS    0x3
#define MBA_ELF_PROGRAM_SEGMENTS_COMPLETE           0x4
#define MBA_ELF_EXECUTION_COMPLETE                  0x5

/*   RMB Register Defines */
#define RMB_MBA_IMAGE                  MSS_RELAY_MSG_BUFFER_00
#define RMB_PBL_STATUS                 MSS_RELAY_MSG_BUFFER_01
#define RMB_MBA_COMMAND                MSS_RELAY_MSG_BUFFER_02
#define RMB_MBA_STATUS                 MSS_RELAY_MSG_BUFFER_03
#define RMB_PMI_META_DATA              MSS_RELAY_MSG_BUFFER_04
#define RMB_PMI_CODE_START             MSS_RELAY_MSG_BUFFER_05
#define RMB_PMI_CODE_LENGTH            MSS_RELAY_MSG_BUFFER_06

#endif /* FEATURE_BOOT_RMB_ENABLE */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
**  Function :  boot_rmb_get_meta_info_buffer
** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns pointer to meta info buffer. The buffer shall
*  contain the following information about elf image, expected by self
*  authenticating subsystem:
*  "elf header" followed by "program header table" then "hash segment"
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  uint32 *  - Pointer to elf meta information buffer
* 
*/

#ifdef FEATURE_BOOT_RMB_ENABLE
uint8 *boot_rmb_get_meta_info_buffer ( void );
#else
static inline uint8 *boot_rmb_get_meta_info_buffer ( void )
{ 
  return NULL;
}
#endif
/*===========================================================================
**  Function :  boot_rmb_populate_meta_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the location of elf meta information buffer.
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_populate_meta_info ( void );
#else
static inline void boot_rmb_populate_meta_info ( void )
{}
#endif

/*===========================================================================
**  Function :  boot_rmb_set_pmi_code_start_addr
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the start of first elf segment.
*
* @param[in] 
*  uint32  *pmi_code_start_addr  - Pointer start of first elf segment
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_set_pmi_code_start_addr ( uint32  *pmi_code_start_addr );
#else
static inline void boot_rmb_set_pmi_code_start_addr
( 
  uint32  *pmi_code_start_addr
){}
#endif

/*===========================================================================
**  Function :  boot_rmb_set_pmi_code_length
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the loaded elf length.
*
* @param[in] 
*  uint32  pmi_code_length - length of loaded elf
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_set_pmi_code_length ( uint32 pmi_code_length );
#else
static inline void boot_rmb_set_pmi_code_length ( uint32 pmi_code_length )
{}
#endif

/*===========================================================================
**  Function :  boot_rmb_set_pmi_elf_load_ready
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the  start of elf load.
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_set_pmi_elf_load_ready ( void );
#else
static inline void boot_rmb_set_pmi_elf_load_ready ( void )
{}
#endif

/*===========================================================================

**  Function :  boot_rmb_check_subsys_status

** ==========================================================================
*/
/*!
* 
* @brief
*  This function Checks if subsystem PBL and auth(eg MBA) images are able to
*  boot successfuly.
* 
* @param[in] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_check_subsys_status( void );
#else
static inline void boot_rmb_check_subsys_status( void )
{}
#endif

/*===========================================================================
**  Function :  boot_rmb_set_mba_image_addr
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys PBL image, the MBA image start address.
*
* @param[in] 
*  uint32  *mba_image_addr  - Pointer loaded MBA image.
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
#ifdef FEATURE_BOOT_RMB_ENABLE
void boot_rmb_set_mba_image_addr ( uint32  *mba_image_addr );
#else
static inline void boot_rmb_set_mba_image_addr ( uint32  *mba_image_addr )
{}
#endif

#endif /* BOOT_RMB_H */
