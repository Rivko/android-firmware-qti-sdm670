/** @file PmicDispBacklight.h

  Common definitions for PmicDispBacklight

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/28/12   sm      New file
=============================================================================*/
#ifndef __EFIPMICDISPBACKLIGHT_H_
#define __EFIPMICDISPBACKLIGHT_H_

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/**
  API to enable backlight
*/
EFI_STATUS PmicEnableBacklightSupport( UINT8  Type, BOOLEAN Enable );

/**
  API to change the brightness level of backlight
*/
EFI_STATUS PmicSetBrightness ( UINT8 Type, UINT32 Percent );

#endif // __EFIPMICDISPBACKLIGHT_H_

