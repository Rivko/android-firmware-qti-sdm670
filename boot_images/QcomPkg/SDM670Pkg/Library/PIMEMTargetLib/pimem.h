#ifndef __PIMEM_H__
#define __PIMEM_H__

/**
 * @file pimem.h
 * @brief
 * Header file for pimem driver
 */
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/05/15   tw      added api parameter to pass in mmu page table address 
                   and descriptor
10/26/14   tw      Initial version.
================================================================================
                      Copyright 2014-2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
#include  "msmhwiobase.h"
#include  "pimem_hwio.h"
#include  "HALhwio.h"

/*==============================================================================
                                  MACROS
==============================================================================*/

/*==============================================================================
                                   DATA
==============================================================================*/
typedef enum {
    PIMEM_OK = 0,
    PIMEM_DISABLE,
    PIMEM_ERROR,
    PIMEM_UNKNOWN_ERROR=0x7F
} PIMEM_STATUS;


/*==============================================================================
                                   FUNCTIONS
==============================================================================*/

/* ============================================================================
**  Function : pimem_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize pimem for operation
*
*   @details
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void pimem_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry);

/* ============================================================================
**  Function : pimem_debug_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize pimem for debug operation
*
*   @details
*   turn off authentication error fatal so we can continue debug on pimem
*   even when we see data corruptions
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void pimem_debug_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry);

/* ============================================================================
**  Function : pimem_get_status
** ============================================================================
*/
/*!
*   @brief
*   Return current pimem status
*
*   @details
*
*   @param size_in_mb   -   [OUT] pointer to fill with actual pimem vault size
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  PIMEM_STATUS - return the current status of pimem
*
*   @sa None
*/

PIMEM_STATUS pimem_get_status(uint32 * size_in_mb);

#endif /* __PIMEM_H__ */
