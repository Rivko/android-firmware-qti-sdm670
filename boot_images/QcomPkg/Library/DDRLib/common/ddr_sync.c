/*
===========================================================================

FILE:         ddr_sync.c

DESCRIPTION:  
  This is the ddr driver wrapper for syncronization mechanisms

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
02/18/10   tw     Initial version

===========================================================================
             Copyright © 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "ddr_sync.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
ddr_dalsys_type ddr_dalsys;

/* -----------------------------------------------------------------------      
**                           FUNCTIONS                                          
** ----------------------------------------------------------------------- */   

/* ============================================================================
**  Function : DDR_SYNC_INIT
** ============================================================================
*/
/*!
*   @brief
*   Initializes the DDR Syncronization mechanism
*   
*   @details
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval 
*   
*   @sa None
*/

void DDR_SYNC_INIT(void)
{
#if defined( BUILD_EHOSTDL ) || defined( BUILD_BOOT_CHAIN )

#else
  DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION, 
        &(ddr_dalsys.ddr_sync_handle), 
        &(ddr_dalsys.ddr_sync_obj));
#endif
}

/* ============================================================================
**  Function : DDR_SYNC_LOCK
** ============================================================================
*/
/*!
*   @brief
*   Issues a lock for ddr drivers
*   
*   @details
*   This function will issue a lock for ddr and ensure no further calls will be
*   executed until current operation is done.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval 
*   
*   @sa None
*/

void DDR_SYNC_LOCK(void)
{
#if defined( BUILD_EHOSTDL ) || defined( BUILD_BOOT_CHAIN )

#else
  DALSYS_SyncEnter(ddr_dalsys.ddr_sync_handle);
#endif  
}

/* ============================================================================
**  Function : DDR_SYNC_LOCK
** ============================================================================
*/
/*!
*   @brief
*   Issues a unlock for ddr drivers
*   
*   @details
*   This function will issue a unlock for ddr and so that other code can execute
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval 
*   
*   @sa None
*/

void DDR_SYNC_UNLOCK(void)
{
#if defined( BUILD_EHOSTDL ) || defined( BUILD_BOOT_CHAIN )

#else
  DALSYS_SyncLeave(ddr_dalsys.ddr_sync_handle);
#endif  
}
