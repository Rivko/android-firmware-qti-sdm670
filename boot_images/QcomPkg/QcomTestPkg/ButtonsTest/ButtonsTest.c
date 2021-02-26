/** @file
                    ButtonsTest.c

  Buttons test code.

  Portions copyright (c) 2012 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 01/29/12   ma      Reducing key press count to 10
 10/22/12   ma      Updating copyrights
 08/16/12   ma      Log KeyToggleState, KeyShiftState
 02/09/12   ma      Porting Changes from KeypadTest
 01/17/12   ma      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/SimpleTextInEx.h>

// Key Presses Count
#define CHAR_COUNT 10

typedef struct _KEY_REGISTRATION_INFO
{
	EFI_KEY_DATA   EfiKeyData;
	EFI_HANDLE     NotificationHandle;
}KEY_REGISTRATION_INFO;

// Allocate an array to store Key info
KEY_REGISTRATION_INFO keyRegistrationArray[CHAR_COUNT];

/**
  Store the Key which has been registered with the Keypad Driver
  in an array along with its Handle.

  @param  storeKeyData        Information about the key which has been registered.
  @param  storeNotifyHandle   Unique handle assigned to the registered notification

  @retval none

**/

VOID StoreKeyInfo (
  EFI_KEY_DATA storeKeyData,
  EFI_HANDLE storeNotifyHandle
  )
{
   UINT8 i;

   for( i = 0; i < CHAR_COUNT; i++ )
   {
       if ( (keyRegistrationArray[i].EfiKeyData.Key.ScanCode == storeKeyData.Key.ScanCode) &&
            (keyRegistrationArray[i].EfiKeyData.Key.UnicodeChar == storeKeyData.Key.UnicodeChar) &&
            (keyRegistrationArray[i].NotificationHandle == storeNotifyHandle) )
       {
         // Key Info already present
         break;
       }
       else if ( keyRegistrationArray[i].NotificationHandle == NULL )
       {
         // Store the Key Info
         keyRegistrationArray[i].EfiKeyData.Key.ScanCode = storeKeyData.Key.ScanCode;
         keyRegistrationArray[i].EfiKeyData.Key.UnicodeChar = storeKeyData.Key.UnicodeChar;
         keyRegistrationArray[i].NotificationHandle = storeNotifyHandle;
         break;
       }
   }
}

/**
  This is a test notification function for all keys except Pwr Key, it will be registered
  with SimpleTextInEx protocol interface - RegisterKeyNotify() of Keypad Driver

  @param   KeyData                A pointer to a buffer that is filled
                                  in with the keystroke
                                  information for the key that was pressed.
  @retval  EFI_SUCCESS            KeyData is successfully processed.
  @return  EFI_INVALID_PARAMETER  KeyData is NULL.
**/

EFI_STATUS EFIAPI TestNotificationFn (
  IN EFI_KEY_DATA     *KeyData
)
{

 EFI_STATUS ret = EFI_SUCCESS;

   if(KeyData == NULL )
   {
     ret = EFI_INVALID_PARAMETER ;
   }
   else
   {
	AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");
        AsciiPrint("A CallBack was Registered for the Key Pressed:\n");
        AsciiPrint("ScanCode = (0x%x), UnicodeChar =(0x%x)\n",
                    KeyData->Key.ScanCode, KeyData->Key.UnicodeChar);
        AsciiPrint("ShiftState=(0x%x), ToggleState==(0x%x)\n",
                    KeyData->KeyState.KeyShiftState, KeyData->KeyState.KeyToggleState );
        AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");

   }
  return ret;
}



/**
  This is a test notification function for Pwr Key, it will be registered
  with SimpleTextInEx protocol interface - RegisterKeyNotify() of Keypad Driver

  @param   KeyData                A pointer to a buffer that is filled
                                  in with the keystroke
                                  information for the key that was pressed.
  @retval  EFI_SUCCESS            KeyData is successfully processed.
  @return  EFI_INVALID_PARAMETER  KeyData is NULL.
**/

EFI_STATUS EFIAPI TestNotificationFnPwrKey (
  IN EFI_KEY_DATA     *KeyData
)
{

 EFI_STATUS ret = EFI_SUCCESS;

   if(KeyData == NULL )
   {
     ret = EFI_INVALID_PARAMETER ;
   }
   else
   {
        AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");
        AsciiPrint("A CallBack was Registered for the Power Key Pressed:");
        AsciiPrint("ScanCode = (0x%x), UnicodeChar =(0x%x)\n",
                    KeyData->Key.ScanCode, KeyData->Key.UnicodeChar);
        AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");
   }
  return ret;
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS EFIAPI ButtonsTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  UINTN         WaitCount;
  UINTN         WaitIndex;
  EFI_EVENT     WaitList[2];
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleEx;
  EFI_HANDLE   testNotifyHandle;
  EFI_KEY_DATA testKeyData;
  UINT8 index;
  UINT8 CountOfCharReceived = 0;

  WaitCount   = 1;
  WaitList[0] = gST->ConIn->WaitForKey;

  Status = gBS->OpenProtocol (
                  gST->ConsoleInHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID**)&SimpleEx,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  ASSERT_EFI_ERROR (Status);

  AsciiPrint("Buttons Test : Max 10 keys to exit\n");
  AsciiPrint("             : To test keypad extended protocol, keep the key in pressed state\n");

  TEST_START("ButtonsTest");
  for (;;)
  {
    Status = gBS->WaitForEvent (WaitCount, WaitList, &WaitIndex);
    ASSERT_EFI_ERROR (Status);

    // Key event signaled
    Status = SimpleEx->ReadKeyStrokeEx (SimpleEx, &testKeyData);

    if(!EFI_ERROR (Status))
    {
      AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");
      AsciiPrint("Key Stroke Read\n");
      AsciiPrint("ScanCode = (0x%x), UnicodeChar =(0x%x)\n",
                  testKeyData.Key.ScanCode, testKeyData.Key.UnicodeChar);
      AsciiPrint("ShiftState=(0x%x), ToggleState==(0x%x)\n",
                  testKeyData.KeyState.KeyShiftState, testKeyData.KeyState.KeyToggleState );
      AsciiPrint("++++++++++++++++++++++++++++++++++++++++++++++\n");

      //Register the notification function with keypad driver for testKeyData
      // Pwr key
      if( (testKeyData.Key.ScanCode == SCAN_SUSPEND) && (testKeyData.Key.UnicodeChar == 0) )
      {
        Status = SimpleEx->RegisterKeyNotify (
                             SimpleEx,
                             &testKeyData,
                             TestNotificationFnPwrKey,
                             &testNotifyHandle
                             );
      }
      // All keys except Pwr key
      else
      {
        Status = SimpleEx->RegisterKeyNotify (
                             SimpleEx,
                             &testKeyData,
                             TestNotificationFn,
                             &testNotifyHandle
                             );
      }


      if(EFI_ERROR(Status) || !(testNotifyHandle))
      {
        AsciiPrint("Key Registration failed,Status:%x\n",Status);
      }
      else
      {
        AsciiPrint("Key Registration successfull with keypad driver, Handle =(0x%x)\n",testNotifyHandle);

        //Store the handle and the key information in an array
        StoreKeyInfo (testKeyData,testNotifyHandle);
      }

      if( ++CountOfCharReceived == CHAR_COUNT )
      {
        break;
      }
    }
  }

  // Unregister the notification functions for all the keys
  AsciiPrint("*******Key Unregistration*******\n");
  for( index = 0; index < CHAR_COUNT; index++ )
  {
    if( keyRegistrationArray[index].NotificationHandle != NULL )
    {
      Status = SimpleEx->UnregisterKeyNotify(SimpleEx,keyRegistrationArray[index].NotificationHandle);

      if( !EFI_ERROR (Status) )
      {
        AsciiPrint("Unregistration successfull for key ScanCode = (0x%x), UnicodeChar =(0x%x)\n",
                    keyRegistrationArray[index].EfiKeyData.Key.ScanCode,
                    keyRegistrationArray[index].EfiKeyData.Key.UnicodeChar);
      }
      else
      {
        AsciiPrint("Unregistration failed for key ScanCode = (0x%x), UnicodeChar =(0x%x)\n",
                    keyRegistrationArray[index].EfiKeyData.Key.ScanCode,
                    keyRegistrationArray[index].EfiKeyData.Key.UnicodeChar);
      }
      //clear the key handle
      keyRegistrationArray[index].NotificationHandle = NULL;
    }
  }
  TestStatus("ButtonsTest",Status);
  TEST_STOP("ButtonsTest");
  return Status;
}
