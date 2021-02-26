/** @file TouchApiDefs.h
*/

/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

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

/** Maximum number of fingers. */
#define MAX_FINGER_NUM  10

/** No touch event. */
#define TLAPI_TOUCH_EVENT_NONE  0
/** Down touch event. */
#define TLAPI_TOUCH_EVENT_DOWN  1
/** Move touch event. */
#define TLAPI_TOUCH_EVENT_MOVE  2
/** Up touch event. */
#define TLAPI_TOUCH_EVENT_UP    4


/** Structure for the details of a single finger touch event. */
struct tsTouchData {
  uint32_t event;      /**< Event: \n
                            - 0 -- None
                            - 1 -- Down
                            - 2 -- Move
                            - 4 -- Up   @tablebulletend */
  uint32_t x;          /**< X-coordinate of the touch event. */
  uint32_t y;          /**< Y-coordinate of the touch event. */
  uint64_t timestamp;  /**< Unused. */
  uint32_t dirty;      /**< TRUE if changed since the last read operation.
                            @newpagetable */
};

/** Structure for the details of a touch event. */
struct tsFingerData {
  struct tsTouchData finger[MAX_FINGER_NUM];
    /**< An array holding the touch event details for up to 10 fingers. */
};

/** @} */ /* end_addtogroup qsee_ui_library */

/** Invalid session ID. Returned in the case of an error. */
#define DR_SID_INVALID      0xffffffff



