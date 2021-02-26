/**
@file debug_policy_private.h
@brief Debug policy implementation definitions/routines
* Copyright (c) 2010-2014 by Qualcomm, Technologies, Inc.  All Rights Reserved.

This file defines a private definition of debug policy implementation

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/07/08     st          Initial version
=============================================================================*/

#ifndef DEBUG_POLICY_H
#define DEBUG_POLICY_H

/**
 * @brief 
 *        Check whether the current image should be authenticated against the debug policy roots
 *
 * @param[in] sw_type is the current image ID being authenticated.
 *                @retval TRUE if the image should be authenticated against the debug policy, FALSE otherwise
 *
 */
boolean is_dbg_policy_rot_for_image( uint32 sw_type );

/**
 * @brief
 * When TZ is initialized we can no longer access the policy
 * from the shared region. We therefore need to keep a local
 * copy. Before copying, verify it.
* 
 * @param[in] dp The debug policy
 * @retval TRUE if the policy is valid, bound and was copied locally
*/
boolean copy_debug_policy(dbg_policy_t* dp);



#endif
