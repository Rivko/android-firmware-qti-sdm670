/*
===========================================================================

FILE:         PIMEMTargetLibNull.c

DESCRIPTION:
  Stubbed XBL driver for pimem initialization

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/PIMEMTargetLibNull/PIMEMTargetLibNull.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/08/15   plc     Initial revision.

===========================================================================
                   Copyright (c) 2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/
#include "pimem.h"


/*==============================================================================
                                  DATA
==============================================================================*/
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

void pimem_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry)
{
}


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

void pimem_debug_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry)
{
}


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

PIMEM_STATUS pimem_get_status(uint32 * size_in_mb)
{
  return PIMEM_OK;
}

