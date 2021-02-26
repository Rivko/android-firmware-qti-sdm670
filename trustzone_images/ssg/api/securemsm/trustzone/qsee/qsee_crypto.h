#ifndef TZBSP_CRYPTO_H
#define TZBSP_CRYPTO_H

/**
@file qsee_crypto.h
@brief Lock the hardware so that only TZ can use it
       The crypto hardware is shared by the HLOS. This means that
       there are all kinds of race conditions if the hardware access
       is not properly managed. The HLOS must ask TZ to use the hardware
       before it can be used. TZ must also lock access to this resource
       if the hardware is used.
*/

/*===========================================================================
   Copyright (c) 2010-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_crypto.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/04/11   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>

/**
 * @brief Lock or unlock the crypto hardware by XPU protecting the resource.
 *        This function may do nothing based if the chipset supports dedicated
 *        TZ crypto engine.
 *
 * @param[in] lock  Set to TRUE to lock and FALSE to unlock
 *
 * @return TRUE if hardware can be locked; otherwise FALSE is returned.
 */
bool qsee_crypto_lock_engine(bool lock);

#endif
