/** @file
ButtonsLib.c

Button driver is responsible for detecting key press/release through PMIC HW
and service. It implements the interfaces of simple text input protocol.

This file is platform dependent, it supports ButtonsDxe based on the platform.

Copyright (c) 2012-2017 Qualcomm Technologies, Inc. All rights reserved.
Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
EDIT HISTORY

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/19/17   sv       Added support for SDM670
01/22/17   ma       Clamshell support 845
09/13/16   ma       Support QRD845
04/04/16   ma       Change GPIO PULL UP source to VIN0
02/04/16   ma       Port to 845
=============================================================================*/

#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/ButtonsLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/OfflineCrashDump.h>
#include <api/pmic/pm/pm_uefi.h>

#include "ButtonsLibPrivate.h"


EFI_PLATFORMINFO_PLATFORM_TYPE  PlatformType ;

EFI_QCOM_PMIC_GPIO_PROTOCOL *PmicGpioProtocol;
EFI_QCOM_PMIC_PWRON_PROTOCOL *PmicPONProtocol;


BOOLEAN isEfiKeyDetected = FALSE;
BOOLEAN isHomeKeyDetected = FALSE;
extern EFI_GUID gQcomTokenSpaceGuid;

#define NUMBER_OF_KEYS 5

/*** Define the Key Map for all Platforms ***/

//KeyMap for CDP
KEY_TYPE KeyMap_Sdm670_CDP[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE, NONE
};

//KeyMap for MTP and FLUID
KEY_TYPE KeyMap_Sdm670_MTP_FLUID[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE, NONE
};

//KeyMap for LIQUID
KEY_TYPE KeyMap_Sdm670_LIQUID[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE, NONE
};

//KeyMap for QRD
KEY_TYPE KeyMap_Sdm670_QRD[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE, NONE
};

//KeyMap for HDK
KEY_TYPE KeyMap_Sdm670_HDK[ NUMBER_OF_KEYS] =
{
   PWR, VOL_UP, VOL_DOWN, NONE, NONE
};

/**
Find a local key to an array.

@param  pArray         An array pointer to which the key will be found from.
@param  sizeOfArray    The array size.
@param  key            The key will be found.

@retval TRUE           Found successfully.
@retval FALSE          Found failed.

**/

BOOLEAN FoundAKey(
   KEY_TYPE *pArray,
   UINT8     sizeOfArray,
   KEY_TYPE  key
   )
{
   UINT8 i;
   for( i=0; i<sizeOfArray; i++ )
   {
      if( pArray[i] == key )
      {
         return TRUE;
      }
   }
   return FALSE;
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
   EFI_STATUS retVal = EFI_NOT_FOUND;
   EFI_INPUT_KEY EfiKey;

   EfiKey.ScanCode = SCAN_NULL;
   EfiKey.UnicodeChar = 0;
   *pNumOfEfiKeys = 0;
   isHomeKeyDetected = FALSE;

   BOOLEAN bCameraKeyIsPressed;
   BOOLEAN bVolUpKeyIsPressed;
   BOOLEAN bVolDownKeyIsPressed;
   BOOLEAN bCameraKeyIsReleased;
   BOOLEAN bHomeKeyIsPressed;
   BOOLEAN bPwrKeyPressed;

   if( isEfiKeyDetected == FALSE )
   {
      bCameraKeyIsPressed      = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, CAMERA);
      bVolUpKeyIsPressed       = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, VOL_UP);
      bVolDownKeyIsPressed     = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, VOL_DOWN);
      bCameraKeyIsReleased     = FoundAKey(pKeysReleased,sizeOfPressedReleasedArray, CAMERA);
      bHomeKeyIsPressed        = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, HOME);
      bPwrKeyPressed           = FoundAKey(pKeysPressed, sizeOfPressedReleasedArray, PWR);

      // assume EFI key found
      isEfiKeyDetected = TRUE;

	  if( bVolUpKeyIsPressed && bVolDownKeyIsPressed )
	  {
         // combo key found
         EfiKey.ScanCode = SCAN_ESC;
	  }
      else if( bCameraKeyIsPressed && bVolUpKeyIsPressed )
      {
         // combo key found
         EfiKey.ScanCode = SCAN_HOME;
      }
      else if( bCameraKeyIsPressed && bVolDownKeyIsPressed )
      {
         // combo key found
         EfiKey.ScanCode = SCAN_DELETE;
      }
      else if( bVolUpKeyIsPressed )
      {
         if( bPwrKeyPressed )
         {
            // combo key found
            EfiKey.ScanCode = SCAN_HOME;
         }
         else
         {
            // single key found
            EfiKey.ScanCode = SCAN_UP;
         }
      }
      else if( bVolDownKeyIsPressed )
      {
         if( bPwrKeyPressed )
         {
            // combo key found
            EfiKey.ScanCode = SCAN_DELETE;
         }
         else
         {
            // single key found
            EfiKey.ScanCode = SCAN_DOWN;
         }
      }
      else if( bCameraKeyIsReleased )
      {
         // single key found
         EfiKey.UnicodeChar = CHAR_CARRIAGE_RETURN;
      }
      else if( bHomeKeyIsPressed )
      {
         EfiKey.ScanCode = SCAN_NULL;
         isHomeKeyDetected = TRUE;
      }
      else if( bPwrKeyPressed )
      {
        EfiKey.ScanCode = SCAN_SUSPEND;
      }
      else
      {
         // no EFI key found, set the flag to FALSE.
         isEfiKeyDetected = FALSE;
      }


      if( isEfiKeyDetected == TRUE )
      {
         // an EFI key is detected, return it.
         CopyMem( &pEfiKeys[*pNumOfEfiKeys], &EfiKey, sizeof(EFI_INPUT_KEY));
         (*pNumOfEfiKeys)++;

         retVal = EFI_SUCCESS;
      }
   }

   if( numOfKeysPressed == 0)
   {
      // no more key pressed, reset flag for next press session.
      isEfiKeyDetected = FALSE;
   }

   return retVal;
}



/**
Initialize KeyMap based on the platform.
Also initialze Power Key.

@param  pNumberOfKeys            Pointer to number of keys
@param  pKeyMap                  Pointer to key map.
@retval EFI_SUCCESS              Initialization successful
@retval non EFI_SUCCESS          Initialization failed

**/

EFI_STATUS InitializeKeyMap (
   UINT8         *pNumberofKeys,
   KEY_TYPE      *pKeyMap
   )
{
   EFI_PLATFORMINFO_PROTOCOL *pPlatformInfoProtocol;
   EFI_PLATFORMINFO_PLATFORM_INFO_TYPE  platformInfo;
   EFI_STATUS Status;

   Status = gBS->LocateProtocol ( &gEfiPlatformInfoProtocolGuid,
      NULL,
      (VOID **)&pPlatformInfoProtocol
      );

   if ( Status != EFI_SUCCESS )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: Failed to locate PlatformInfo Protocol, Status =  (0x%x)\r\n", Status ));
      goto ErrorExit;
   }

   if( !pPlatformInfoProtocol )
   {
      Status = EFI_INVALID_PARAMETER;
      goto ErrorExit;
   }

   pPlatformInfoProtocol->GetPlatformInfo( pPlatformInfoProtocol, &platformInfo );
   PlatformType = platformInfo.platform;

   switch ( PlatformType )
   {
   case EFI_PLATFORMINFO_TYPE_CDP:
      CopyMem( pKeyMap, KeyMap_Sdm670_CDP, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
      break;
   case EFI_PLATFORMINFO_TYPE_MTP:
   case EFI_PLATFORMINFO_TYPE_FLUID:
      CopyMem( pKeyMap, KeyMap_Sdm670_MTP_FLUID, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
      break;
   case EFI_PLATFORMINFO_TYPE_LIQUID:
      CopyMem( pKeyMap, KeyMap_Sdm670_LIQUID, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
      break;
   case EFI_PLATFORMINFO_TYPE_QRD:
      CopyMem( pKeyMap, KeyMap_Sdm670_QRD, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
      break;
   case EFI_PLATFORMINFO_TYPE_HDK:
      CopyMem( pKeyMap, KeyMap_Sdm670_HDK, NUMBER_OF_KEYS*sizeof(KEY_TYPE) );
      break;
   default:
      Status = EFI_INVALID_PARAMETER;
      goto ErrorExit;
   }

   *pNumberofKeys = NUMBER_OF_KEYS;

ErrorExit:
   return Status;

}

/**
Enable the GPIO for Input

@param  GpioNumber               GPIO Number
@retval EFI_SUCCESS              Enable successful
@retval non EFI_SUCCESS          Enable failed

**/
EFI_STATUS EnableInput ( UINT32 PmicDeviceIndex, UINT16 GpioNumber )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(PmicGpioProtocol)
  {
     Status = PmicGpioProtocol->ConfigDigitalInput( PmicDeviceIndex,
                                                    (EFI_PM_GPIO_WHICH_TYPE)(GpioNumber-1),
                                                    EFI_PM_GPIO_I_SOURCE_PULL_UP_30uA,
                                                    EFI_PM_GPIO_VIN0,
                                                    EFI_PM_GPIO_OUT_BUFFER_OFF,
                                                    EFI_PM_GPIO_SOURCE_GND
                                                  );
     if ( EFI_ERROR (Status) )
     {
         DEBUG(( EFI_D_ERROR, "EnableInput: ConfigDigitalInput failed Status = (0x%x)\r\n", Status));
         goto ErrorExit;
     }
  }

ErrorExit:
   return Status;
}



/**
Initialize all button GPIOs on PMIC for input based
on platform

@param  None
@retval EFI_SUCCESS              Configuration successful
@retval non EFI_SUCCESS          Configuration failed

**/
EFI_STATUS ConfigureButtonGPIOs ( VOID )
{

   EFI_STATUS Status;

   // volume up
   Status = EnableInput(PM_DEVICE_1, VOLUME_UP_BUTTON_GPIO);
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "ConfigureButtonGPIOs: EnableInput failed for VOL+ button, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }

#if 0  //  -> CAMERA SNAPSHOT BUTTON MOVED TO MSM for SDM670
   if( (PlatformType == EFI_PLATFORMINFO_TYPE_MTP)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_FLUID)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_CDP))
   {
     // Camera Snapshot
     Status = EnableInput(PM_DEVICE_1, CAMERA_SNAPSHOT_BUTTON_GPIO);
     if ( EFI_ERROR (Status) )
     {
        DEBUG(( EFI_D_ERROR, "ConfigureButtonGPIOs: EnableInput failed for Snapshot button, Status = (0x%x)\r\n", Status));
        goto ErrorExit;
     }
   }
     // No home button for MTP and CDP, Need to update if LIQUID platform support HOME button.
   if( (PlatformType == EFI_PLATFORMINFO_TYPE_LIQUID)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_CDP) )
   {
     //home
     Status = EnableInput(PM_DEVICE_0, HOME_BUTTON_GPIO);
     if ( EFI_ERROR (Status) )
     {
        DEBUG(( EFI_D_ERROR, "ConfigureButtonGPIOs: EnableInput failed for home button, Status = (0x%x)\r\n", Status));
        goto ErrorExit;
     }
  }
#endif

ErrorExit:
   return Status;

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
   EFI_STATUS Status;

   // get Pmic Gpio protocol handle
   Status = gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (VOID **) &PmicGpioProtocol);
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: failed to locate PmicGpioProtocol, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }

   ASSERT(PmicGpioProtocol != NULL);

   // get Pmic PON protocol handle
   Status = gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, NULL, (VOID **) &PmicPONProtocol);
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: failed to locate PmicPONProtocol, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }

   // Intialize KeyMap
   Status = InitializeKeyMap( pNumberofKeys, pKeyMap );
   if ( EFI_ERROR(Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: InitializeKeyMap() failed, Status =  (0x%x)\r\n", Status ));
      goto ErrorExit;
   }

   // Configure button GPIOs
   Status = ConfigureButtonGPIOs();
   if ( EFI_ERROR(Status) )
   {
      DEBUG(( EFI_D_ERROR, "ButtonsInit: ConfigureButtonGPIOs() failed, Status =  (0x%x)\r\n", Status ));
      goto ErrorExit;
   }

ErrorExit:
   return Status;
}


/**
Read real time interrupt status on PMIC.

@param  IRQType                  PON Real time IRQ type.
@param  pGpioButtonPressed       Pointer storing the value of button activity (press).

@retval EFI_SUCCESS              Retrieve status successfull.
@retval non EFI_SUCCESS          Retrieve status failed.

**/
EFI_STATUS ReadRealTimeIRQStatus( EFI_PM_PON_IRQ_TYPE IRQType, BOOLEAN *pGpioButtonPressed )
{
   EFI_STATUS Status = EFI_INVALID_PARAMETER;

   if(PmicPONProtocol)
   {
     Status = PmicPONProtocol->GetPonRtStatus(0, IRQType, pGpioButtonPressed);
     if ( EFI_ERROR (Status) )
     {
         DEBUG(( EFI_D_ERROR, "ReadRealTimeIRQStatus: GetPonRtStatus failed Status = (0x%x)\r\n", Status));
     }
   }

   return Status;
}


/**
Read gpio status on PMIC.

@param  GpioNumber               GPIO number..
@param  pGpioButtonPressed       Pointer storing the value of button activity (press).

@retval EFI_SUCCESS              Retrieve status successfully
@retval non EFI_SUCCESS          Retrieve status failed

**/
EFI_STATUS ReadGpioStatus( UINT32 PmicDeviceIndex, UINT16 GpioNumber, BOOLEAN *pGpioButtonPressed )
{
   EFI_STATUS Status = EFI_SUCCESS;
   BOOLEAN GpioStatus = FALSE;

   if(PmicGpioProtocol)
   {
     Status = PmicGpioProtocol->IrqStatus( PmicDeviceIndex,
                                           (EFI_PM_GPIO_WHICH_TYPE)(GpioNumber-1),
                                           EFI_PM_IRQ_STATUS_RT,
                                           &GpioStatus
                                         );
     if ( EFI_ERROR (Status) )
     {
         DEBUG(( EFI_D_ERROR, "ReadGpioStatus: IrqStatus failed Status = (0x%x)\r\n", Status));
     }
     else
     {
         //
         // this is because we initially have pull-ups on the GPIOs
         //
         *pGpioButtonPressed =  ( GpioStatus == TRUE ) ? FALSE : TRUE;
     }
   }

   return Status;
}


/**
Poll button array.

@param  pButtonArray             Pointer to buttons array.

@retval EFI_SUCCESS              Retrieve matrix successfully
@retval non EFI_SUCCESS          Retrieve matrix failed

**/
EFI_STATUS PollButtonArray( UINT8 *pButtonArray )
{
   EFI_STATUS Status;
   BOOLEAN ButtonPressed = FALSE;

   //for Power Key
   Status = PollPowerKey(&ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
     DEBUG(( EFI_D_ERROR, "PollButtonArray: PollPowerKey failed Status = (0x%x)\r\n", Status));
     goto ErrorExit;
   }
  *(pButtonArray + 0) = ButtonPressed;

   //for volume up
   ButtonPressed = FALSE;
   Status = ReadGpioStatus(PM_DEVICE_1, VOLUME_UP_BUTTON_GPIO, &ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadGpioStatus failed for VOL+, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }
   *(pButtonArray + 1) = ButtonPressed;

   //for volume down
   ButtonPressed = FALSE;
   Status = ReadRealTimeIRQStatus( EFI_PM_PON_IRQ_RESIN_ON, &ButtonPressed );
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadRealTimeIRQStatus failed for VOL-, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }
   *(pButtonArray + 2) = ButtonPressed;

#if 0  //  -> CAMERA SNAPSHOT BUTTON MOVED TO MSM for SDM660

   if( (PlatformType == EFI_PLATFORMINFO_TYPE_LIQUID)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_CDP) )
   {
      //for home
      ButtonPressed = FALSE;
      Status = ReadGpioStatus( PM_DEVICE_0, HOME_BUTTON_GPIO, &ButtonPressed );
      if ( EFI_ERROR (Status) )
      {
         DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadGpioStatus failed for HOME, Status = (0x%x)\r\n", Status));
         goto ErrorExit;
      }
      *(pButtonArray + 3) = ButtonPressed;
   }
   

   if( (PlatformType == EFI_PLATFORMINFO_TYPE_MTP)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_FLUID)||
       (PlatformType == EFI_PLATFORMINFO_TYPE_CDP))
   {
      //for camera snapshot
      ButtonPressed = FALSE;
      Status = ReadGpioStatus( CAMERA_SNAPSHOT_BUTTON_GPIO, &ButtonPressed );
      if ( EFI_ERROR (Status) )
      {
         DEBUG(( EFI_D_ERROR, "PollButtonArray: ReadGpioStatus failed for CAMERA SNAPSHOT, Status = (0x%x)\r\n", Status));
         goto ErrorExit;
      }
     *(pButtonArray + 4) = ButtonPressed;
   }
#endif

ErrorExit:
   return Status;
}

/**
Poll power key state from PMIC.

@param  pPowerKey                Pointer to power key state.

@retval EFI_SUCCESS              Retrieve power key status successfully
@retval non EFI_SUCCESS          Retrieve power key status failed

**/
EFI_STATUS PollPowerKey(BOOLEAN *pPowerKey)
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;

   Status = ReadRealTimeIRQStatus( EFI_PM_PON_IRQ_KPDPWR_ON, pPowerKey );
   if ( EFI_ERROR (Status) )
   {
      DEBUG(( EFI_D_ERROR, "PollPowerKey: ReadRealTimeIRQStatus failed for Power Button, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
   }

ErrorExit:
  return Status;
}

/**
  Set the value of isEfiKeyDetected .

  @param  flag  Boolean value .

  @retval none

**/
VOID SetEfiKeyDetection(BOOLEAN flag)
{
   isEfiKeyDetected = flag;
}

/**
  return the value of isHomeKeyDetected .

  @param  none .

  @retval Boolean value.

**/
BOOLEAN IsHomeKeyDetected(VOID)
{
    return isHomeKeyDetected;
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

