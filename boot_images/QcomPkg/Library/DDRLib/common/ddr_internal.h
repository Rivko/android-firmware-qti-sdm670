#ifndef DDR_INTERNAL_H
#define DDR_INTERNAL_H

/**
 * @file ddr_internal.h
 * @brief
 * Function headers of DDR drivers internal private API
 */

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/08/11   tw      Initial revision.
================================================================================
                  Copyright 2011 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
==============================================================================*/
#include "ddr_common.h"

/* -----------------------------------------------------------------------
**                           DEFINES
** ----------------------------------------------------------------------- */

#define DDR_TRAINING_REQUIRED         0x6188709B
#define DDR_TRAINING_NOT_REQUIRED     0xACBE001F
#define DDR_TRAINING_DATA_UPDATED     0xDAAA11E1
#define DDR_TRAINING_DATA_NOT_UPDATED 0xDE8D526A


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ============================================================================
**  Function : ddr_target_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called at the end of ddr init to initialize any 
*   target specific configurations 
*   
*   @details
*   Target specific configurations such as override of timing registers and 
*   calling target specific init functions will be captured in this api
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

void ddr_target_init(void);


#endif
