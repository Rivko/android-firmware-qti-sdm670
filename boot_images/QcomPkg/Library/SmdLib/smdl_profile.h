#ifndef SMDL_PROFILE_H
#define SMDL_PROFILE_H

/**
 * @file smdl_profile.h
 *
 * Header file for SMD Lite profiling.
 *
 * See smdl_profile.c for common profiling code and smdl_profile_rex.c for
 * os-specific profiling code.
 */

/*==============================================================================
     Copyright (c) 2009-2010 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/10   tl      Updated to SMD Lite v2
04/17/09   tl      Initial SMD Lite test code
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"

/*=========================================================================== 
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/**
 * Initializes the SMD Lite profiling system.
 *
 * The profiling system will remain dormant until triggered by the debugger.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       @li 0 if the port is closed
 *               @li A negative error code if an error occurred.
 * @sideeffects  Creates a thread for SMD Lite profiling, which remains
 *               dormant until triggered by the debugger.
 */
int smdl_profile_init
(
  uint32 reserved
);

/**
 * Executes the SMD Lite profiling from this thread's context.
 *
 * This function never returns. It waits until triggered by the debugger, then
 * executes the requested profiling tests, and resumes waiting until triggered
 * by the debugger again.
 *
 * @return       None
 */
void smdl_profile(void);

/**
 * Blocks the running thread.
 *
 * This is OS-specific and should be called only by the profiling code.
 *
 * @param[in]    ms  The number of milliseconds to suspend execution
 *
 * @return       None
 * @sideeffects  Temporarily suspends the calling thread
 */
void smdl_profile_wait
(
  uint32 ms
);

#endif /* SMDL_PROFILE_H */
