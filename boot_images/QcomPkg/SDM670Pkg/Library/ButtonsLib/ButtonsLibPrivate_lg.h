#ifndef __BUTTONSLIBPRIVATE_H__
#define __BUTTONSLIBPRIVATE_H__

/** @file
ButtonsLibPrivate.h

  Buttons library internal defines

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/17   sv      Add support for SDM670.
02/04/16   ma      Port to 8998
===========================================================================*/

#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFIPmicPwrOn.h>

//GPIO Numbers
#define VOLUME_UP_BUTTON_GPIO              2     // GPIO2C
//#define HOME_BUTTON_GPIO                 11    // Only For LIQUID or CDP
// #define CAMERA_SNAPSHOT_BUTTON_GPIO      7  //-> CAMERA Button moved to MSM for SDM670


#endif /* __BUTTONSLIBPRIVATE_H__ */
