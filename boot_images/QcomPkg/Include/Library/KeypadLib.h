#ifndef __KEYPADLIB_H__
#define __KEYPADLIB_H__

/** @file
                              KeypadLib.h

  KeypadDxe driver is responsible for detecting key press/release. It is composed
  of two parts: platform(target) independent and platform dependent. The platform
  independent part is same across different platforms, and platform dependent
  part vary on different platform.

  This file defines the interfaces both parts will follow.

  Copyright (c) 2011,2012 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/12   sr     Changes for Reset based solution to avoid flooding issue.
02/10/12   ma     Updating return type from void to VOID
02/02/12   sr     Declaration of newly introduced function
                  proto types in header file.
01/24/12   sr     Changes to support LIQUID platform.
12/20/11   sr     Added support to send Scan Code on PWR
                  key press instead of release .
06/21/11   shl    Added combo keys and power key support.
05/18/11   shl    Initial version.
===========================================================================*/

// key types
typedef enum {
	NONE = 0,
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
  Initialize keypad driver based on platform.

  @param  pNnumOfColumns           Number of Columes in keypad matrix.
  @param  pNumOfRows               Number of Columes in keypad matrix.
  @param  pKeyMap                  Pointer to key map.
  @param  pPowerKey                Pointer to power key.

  @retval EFI_SUCCESS              Initialization successful
  @retval non EFI_SUCCESS          Initialization failed

**/
EFI_STATUS KeypadInit( UINT8         *pNumOfRows,
                       UINT8         *pNumOfColumns,
                       KEY_TYPE      *pKeyMap,
                       EFI_INPUT_KEY *pPowerKey
                     );

/**
  Poll keypad matirx from PMIC.

  @param  pKeypadMatrix            Pointer to keypad matrix.

  @retval EFI_SUCCESS              retrieve matirx successfully
  @retval non EFI_SUCCESS          retrieve matirx failed

**/
EFI_STATUS PollKeypadMatrix( UINT8 *pKeypadMatrix );

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


#endif /* __KEYPADLIB_H__ */
