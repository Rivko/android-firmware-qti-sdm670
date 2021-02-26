#ifndef QSEE_SECDISP_H
#define QSEE_SECDISP_H

/**
@file qsee_secdisp.h
@brief Provide Secure Display functionality
*/

/*===========================================================================
   Copyright (c) 2015 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_secdisp.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Get the current secure display session ID
 *
 * @param [out] the current secure display session ID
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sd_get_session(uint32_t *sessionID);

/**
 * Allow/block HLOS from stopping Secure Display through the sd_ctrl syscall
 *
 * @param [in] TRUE to allow; FLASE to block
 *
 * @return \c 0 on success; -1 on failure. \c
 */
int qsee_sd_set_stop_allowed(
  bool allow
);

#endif /* QSEE_SECDISP_H */

