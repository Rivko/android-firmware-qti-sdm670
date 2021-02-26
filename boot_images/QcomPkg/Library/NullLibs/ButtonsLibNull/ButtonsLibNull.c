/** @file ButtonsLibNull.c
  
  Stub functions for ButtonsLib

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/08/15   plc     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/ButtonsLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/OfflineCrashDump.h>

//#include "ButtonsLibPrivate.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/**
  return the value of isHomeKeyDetected .

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsHomeKeyDetected(VOID)
{
    return FALSE;
}

/**
  return TRUE if Key press notification is supported .

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsNotificationSupported(VOID)
{
  return TRUE;
}

/**
Poll button array.

@param  pButtonArray             Pointer to buttons array.

@retval EFI_SUCCESS              Retrieve matrix successfully
@retval non EFI_SUCCESS          Retrieve matrix failed

**/
EFI_STATUS PollButtonArray( UINT8 *pButtonArray )
{
  return EFI_SUCCESS;
}

/**
Translate local keys into EFI keys.

The press session starts with any key is pressed and ends with no key is
pressed.

Only generate one EFI key per press session and this EFI key is generated
only based on the above table.

For the case of multiple keys pressed, check the possible combo keys first,
first detected combo keys will be used to generate the EFI key. If there is
no valid combo keys detected, then check the single key case, first detected
single key will be used to generate the EFI key.

Once an EFI key is generated in a session, even though there is still other
key currently pressed, no more EFI key will be generated in this session and
just wait for user to release those pressed keys.

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
EFI_STATUS ConvertEfiKeyCode (
   KEY_TYPE       *pKeysPressed,
   UINT8           numOfKeysPressed,
   KEY_TYPE       *pKeysReleased,
   UINT8           numOfKeysReleased,
   UINT8           sizeOfPressedReleasedArray,
   EFI_INPUT_KEY  *pEfiKeys,
   UINT8          *pNumOfEfiKeys
   )
{
  return EFI_SUCCESS;
}

/**
  Set the value of isEfiKeyDetected .

  @param  flag  Boolean value .

  @retval none

**/
VOID SetEfiKeyDetection(BOOLEAN flag)
{
  return;
}

/**
Initialize all PMIC GPIOs as input based on platform.
Also configure the power key

@param  pNumberOfKeys            Pointer to number of keys
@param  pKeyMap                  Pointer to key map.
@retval EFI_SUCCESS              Initialization successful
@retval non EFI_SUCCESS          Initialization failed

**/
EFI_STATUS ButtonsInit (
   UINT8         *pNumberofKeys,
   KEY_TYPE      *pKeyMap
   )
{
  return EFI_SUCCESS;
}

