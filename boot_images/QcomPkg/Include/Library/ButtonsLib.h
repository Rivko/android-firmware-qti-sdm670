#ifndef __BUTTONSLIB_H__
#define __BUTTONSLIB_H__

/** @file
                              ButtonsLib.h

  ButtonsDxe driver is responsible for detecting key press/release. It is composed
  of two parts: platform(target) independent and platform dependent. The platform
  independent part is same across different platforms, and platform dependent
  part vary on different platform.

  This file defines the interfaces both parts will follow.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/14   dm     Add pwr key to button array
04/09/13   ma     Added IsNotificationSupported() API
11/07/12   ma     Adding IsHomeKeyDetected() API
05/05/12   sr     Changes for Reset based solution to avoid flooding issue.
02/10/12   ma     Updating return type from void to VOID
02/09/12   ma     Added support for more keys, porting changes from KeypadLib.h
01/04/12   hkm    Initial version.
===========================================================================*/

// key types
typedef enum {
    NONE = 0,
	PWR,
    VOL_UP,
    VOL_DOWN,
    FOCUS,
    CAMERA,
    ENTER,
    HOME,
    ROTATE_LOCK,
    KEY_LAST
}KEY_TYPE;

/**
  Translate local keys into EFI keys

  @param  pKeysPressed                Pointer to currently pressed keys array.
  @param  numOfKeysPressed            Number of keys pressed in pKeysPressed
  @param  pKeysReleased               Pointer to currently released keys array.
  @param  numOfKeysReleased           Number of keys released in pKeysReleased
  @param  sizeOfPressedReleasedArray  Size of pKeysPresed/pKeysPressed arrays.
  @param  pEfiKeys                    Pointer to Uefi keys array.
  @param  pNumOfEfiKeys               Number of Uefi keys detected.

  @retval EFI_SUCCESS                 UEFI key detected
  @retval EFI_NOT_READY               No UEFI key detected

**/
EFI_STATUS ConvertEfiKeyCode( KEY_TYPE      *pKeysPressed,
                              UINT8          numOfKeysPressed,
                              KEY_TYPE      *pKeysReleased,
                              UINT8          numOfKeysReleased,
                              UINT8          sizeOfPressedReleasedArray,
                              EFI_INPUT_KEY *pEfiKeys,
                              UINT8         *pNumOfEfiKeys
                            );

/**
Initialize all PMIC GPIOs as input based on platform.
Also configure the power key

@param  pNumberOfKeys            Pointer to number of keys
@param  pKeyMap                  Pointer to key map.
@retval EFI_SUCCESS              Initialization successful
@retval non EFI_SUCCESS          Initialization failed

**/
EFI_STATUS ButtonsInit(
   UINT8         *pNumberofKeys,
   KEY_TYPE      *pKeyMap
   );

/**
  Poll GPIOs button array from PMIC.

  @param  pButtonArray           Pointer to button array.

  @retval EFI_SUCCESS              retrieve matirx successfully
  @retval non EFI_SUCCESS          retrieve matirx failed

**/
EFI_STATUS PollButtonArray( UINT8 *pButtonArray);

/**
  Poll power key state from PMIC.

  @param  pPowerKey                Pointer to power key state.

  @retval EFI_SUCCESS              Retrieve matrix successfully
  @retval non EFI_SUCCESS          Retrieve matrix failed

**/
EFI_STATUS PollPowerKey(BOOLEAN *pPowerKey);

/**
  set the value of isEfiKeyDetected .

  @param  flag  Boolean value .

  @retval none

**/
VOID SetEfiKeyDetection(BOOLEAN flag);

/**
  return the value of isHomeKeyDetected .

  @param  none .

  @retval Bollean value.

**/
BOOLEAN IsHomeKeyDetected(VOID);

/**
  return TRUE if Key press notification is supported.

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsNotificationSupported(VOID);

#endif /* __BUTTONSLIB_H__ */
