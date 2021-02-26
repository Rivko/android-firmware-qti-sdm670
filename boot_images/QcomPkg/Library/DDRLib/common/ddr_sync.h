#ifndef DDR_SYNC_H
#define DDR_SYNC_H

/*=============================================================================

                                  DDR SYNC 
                            H e a d e r  F i l e
GENERAL DESCRIPTION
This file implements the wrapper for syncronization mechanisms
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who   what, where, why
--------   ---   ------------------------------------------------------------
02/18/10   tw    Initial revision.
=============================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "DALSys.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */
/*
 * ddr_dalsys_type
 *  sync_obj     - Synchronization object.
 *  sync_handle  - Synchronization object handle.
 */
typedef struct
{
  DALSYSSyncObj               ddr_sync_obj;
  DALSYSSyncHandle            ddr_sync_handle;
} ddr_dalsys_type;

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

void DDR_SYNC_INIT(void);

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

void DDR_SYNC_LOCK(void);

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

void DDR_SYNC_UNLOCK(void);
#endif /* DDR_SYNC_H */
