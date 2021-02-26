/** @file SecureUILibDefs.h
*/

/*===========================================================================
 * Copyright(c) 2013-2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

					EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/15    kl      (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
===========================================================================*/

#pragma once

#include <stdint.h>

/** @addtogroup qsee_ui_library
@{ */

/** Command indication from the QSEE application to the Android library layer. */
typedef enum {
  SEC_TOUCH_CMD_CONTINUE      = 0,
    /**< Request to proceed and wait for the next event. */
  SEC_TOUCH_CMD_CANCELLED     = 1,
    /**< Request to cancel the current secure touch session because of an error. */
  SEC_TOUCH_CMD_COMPLETED     = 2,
    /**< Request to terminate the current secure touch session since all data
         requested has been acquired. */
  SEC_TOUCH_CMD_MAX           = SEC_TOUCH_CMD_COMPLETED,
  SEC_TOUCH_CMD_INVALID       = 0x7FFFFFFF
} sec_touch_cmd_t;

/** Error indication from the Android library layer to the QSEE application.
    Note that these indications do not necessarily indicate errors. */
typedef enum {
  SEC_TOUCH_ERR_STARTED       = 0,  /**< Secure touch was started. */
  SEC_TOUCH_ERR_STOPPED       = 1,  /**< Secure touch was stopped. */
  SEC_TOUCH_ERR_DATA          = 2,  /**< Touch interrupt fired; data is available. */
  SEC_TOUCH_ERR_TIMEOUT       = 3,  /**< Waiting for a touch event timed out. */
  SEC_TOUCH_ERR_POWER         = 4,  /**< Abort request to service a power event. */
  SEC_TOUCH_ERR_ABORT         = 5,  /**< Abort request. */
  SEC_TOUCH_ERR_MAX           = SEC_TOUCH_ERR_ABORT,
  SEC_TOUCH_ERR_INVALID       = 0x7FFFFFFF
} sec_touch_err_t;

/** @} */ /* end_addtogroup qsee_ui_library */
