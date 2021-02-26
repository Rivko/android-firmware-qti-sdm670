/** @file

  KeypadDxe driver is responsible for detecting key press/release through PMIC
  HW and service at boot time. It implements the interfaces of simple text input
  protocol.

  Copyright (c) 2011-2013, 2015 Qualcomm Technologies Inc. All rights reserved.
  Portions Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
 06/17/13   vk       Remove UefiRunTime lib
 09/10/12   ma       Updating license info
 04/09/12   sr       Added Changes for Reset based solution to stop flooding issue.
 02/02/12   sr       Added SimpleTextInputEx protocol's RegisterKeyNotify,
                     UnRegisterKeyNotify Implementation.
 01/24/12   sr       changes to support LIQUID platform.
 01/12/12   sr       Added support to send Scan Code on PWR key press 
                     instead of release .
 01/05/12   sr       Initialize the matrices in Reset Call.
 01/04/12   yg       Add SimpleTextInputEx protocol wrapper
 12/13/11   sr       Added support to send EFI key as long as there is
                     no change in the state of the pressed keys that
                     generated the EFI Scan Code 
 06/21/11   shl      Added combo keys and full power key support.
 05/18/11   shl      Initial version

=============================================================================*/

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/KeypadLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>

// key buffer for passing key to console. 
#define MAX_KEYS_TO_BUFFER   32
#define KEYPAD_TIMER_INTERVAL 200000 // 200 milli seconds

EFI_INPUT_KEY  keyBuffer[MAX_KEYS_TO_BUFFER] = {0};
EFI_INPUT_KEY* keyBufferRead;
EFI_INPUT_KEY* keyBufferWrite;

// Keypad matirx can be allocated at run time, but for
// fast boot, it is pre-set here. 
#define   KEYPAD_MAX_COLUMNS   8
#define   KEYPAD_MAX_ROWS      18 
UINT8     matrixA[KEYPAD_MAX_ROWS];
UINT8     matrixB[KEYPAD_MAX_ROWS];
UINT8    *pCurrMatrix = matrixA;
UINT8    *pPrevMatrix = matrixB;
BOOLEAN   isMatrixChanged = FALSE;
BOOLEAN   TimerRKStroke = FALSE;

// keypad real size
UINT8     numOfColumns;
UINT8     numOfRows;
UINT8     numOfKeyRegistrations;

#define   MAX_KEYS_PRESSED_RELEASED  7

// currently pressed keys storage
KEY_TYPE  keysPressed[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfKeysPressed = 0;

// currently just released key storage
KEY_TYPE  keysReleased[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfKeysReleased = 0;

// currently detected UEFI key storage
EFI_INPUT_KEY EfiKeys[MAX_KEYS_PRESSED_RELEASED];
UINT8     numOfEfiKeys = 0;

// power state, TRUE means pressed, FALSE means not pressed
BOOLEAN   currPowerKeyState = FALSE;
BOOLEAN   prevPowerKeyState = FALSE;
EFI_INPUT_KEY powerKey;

// key map storage
KEY_TYPE  keyMap[KEYPAD_MAX_ROWS*KEYPAD_MAX_COLUMNS];

// To store the NotifyEntry of each notification.
LIST_ENTRY     *NotifyList;

/* Structure to register a notification function for a 
   particular keystroke for the input device.*/
typedef struct _KEYPAD_CONSOLE_IN_EX_NOTIFY {
  EFI_HANDLE                            NotifyHandle;
  EFI_KEY_DATA                          KeyData;
  EFI_KEY_NOTIFY_FUNCTION               KeyNotificationFn;
  LIST_ENTRY                            NotifyEntry;
} KEYPAD_CONSOLE_IN_EX_NOTIFY;

/* call back periodic timer maintained by the driver 
   in order to poll PMIC and inform the clients registered
   with keypad driver about the status of input key. */
EFI_EVENT      CallbackTimer;

// EFI_SIMPLE_TEXT_INPUT_PROTOCOL implementation
EFI_STATUS KeysDxeReset(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
  IN BOOLEAN                        ExtendedVerification );

EFI_STATUS KeysDxeReadKeyStroke(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
  OUT EFI_INPUT_KEY                   *Key);

EFI_SIMPLE_TEXT_INPUT_PROTOCOL KeypadSimpleInputProtocolImplementation = {
   KeysDxeReset,
   KeysDxeReadKeyStroke,
   0    //WaitForKey
};

/* Protos for EfiSimpleTextInputExProtocol implementation */
EFI_STATUS
EFIAPI
KeysDxeResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification);

EFI_STATUS
EFIAPI
KeysDxeReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData);

EFI_STATUS
EFIAPI
KeysDxeSetStateEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState);

EFI_STATUS
EFIAPI
KeysDxeRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN  EFI_KEY_DATA                       *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                         *NotifyHandle);

EFI_STATUS
EFIAPI
KeysDxeUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle);

BOOLEAN
EFIAPI
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegisteredData,
  IN EFI_KEY_DATA  *InputData );

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL KeypadSimpleInputExProtocolImplementation = {
   KeysDxeResetEx,
   KeysDxeReadKeyStrokeEx,
   0,                          //  WaitForKeyEx
   KeysDxeSetStateEx,
   KeysDxeRegisterKeyNotify,
   KeysDxeUnregisterKeyNotify,
};

/* DevicePath definition for Keypad driver */
#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} EFI_KEYPAD_DEVICE_PATH;
#pragma pack()

EFI_KEYPAD_DEVICE_PATH KeysDxeDevicePath = 
{
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    //
    // {D7F58A0E-BED2-4B5A-BB43-8AB23DD0E2B0}
    // This value is defined in gEfiKeypadDeviceGuid in QcomPkg.dec package
    //
    { 0xD7F58A0E, 0xBED2, 0x4B5A, { 0xBB, 0x43, 0x8A, 0xB2, 0x3D, 0xD0, 0xE2, 0xB0}}
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


/**
  Store a local key to an array.

  @param  pArray         An array pointer to which the key will be stored to.
  @param  key            The key will be stored.

  @retval TRUE           Store successfully.
  @retval FALSE          Store failed.

**/
BOOLEAN StoreAKey( 
  KEY_TYPE *pArray,
  KEY_TYPE key
)
{
  UINT8 i;
  for( i=0; i<MAX_KEYS_PRESSED_RELEASED; i++ )
  {
    if( pArray[i] == NONE )
    {
      pArray[i] = key;
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Remove a local key from an array.

  @param  pArray         An array pointer to which the key will be removed from.
  @param  key            The key will be removed.

  @retval TRUE           Remove successfully.
  @retval FALSE          Remove failed.

**/
BOOLEAN RemoveAKey(
  KEY_TYPE *pArray,
  KEY_TYPE key
)
{
  UINT8 i;
  for( i=0; i<MAX_KEYS_PRESSED_RELEASED; i++ )
  {
    if( pArray[i] == key )
    {
      pArray[i] = NONE;
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Compare the matrices to determine whether a key is pressed/released. 
  Update the corresponding array if there is such event detected.

  @param  VOID

  @retval VOID 

**/
VOID ProcessMatrix( VOID )
{
  UINT8      row, col;
  UINT8      rowDiff = 0;
  UINT8     *p;
  KEY_TYPE   key;

  // assume no matrix change first
  isMatrixChanged = FALSE;

  for( row = 0; row < numOfRows; ++row )
  {
    rowDiff = pCurrMatrix[row] ^ pPrevMatrix[row];

    // If the previous and current matrices differ in data in 
    // any given row, it means a key in that row changed state 
    if( rowDiff )
    {
      // matrix change detected   
      isMatrixChanged = TRUE;

      for( col = 0; col < numOfColumns; ++col)
      {
        // Which Column changed state 
        if( ( rowDiff >> col ) & 0x1)
        {
          // get local key code 
          key = keyMap[ row*numOfColumns + col ];
			  
          if( key == NONE )
          {
            continue;
          }

          if( ( pCurrMatrix[row] >> col ) & 0x1 )
          {
            // key is released
            // remove this key from keyPressed array if there is
            if( RemoveAKey( keysPressed, key )) 
            {
              numOfKeysPressed--;
            }
            // store this key in keyReleased array
            if( StoreAKey( keysReleased, key ))
            {
              numOfKeysReleased++;
            }
          }
          else
          {
            // key is pressed
            if( StoreAKey( keysPressed, key )) 
            {
              numOfKeysPressed++;
            }
          }
        } // key state
      } // key code
    }
  }

  // swap pointers for next comparison session
  p = pCurrMatrix;
  pCurrMatrix = pPrevMatrix;
  pPrevMatrix = p;

  return;
}

/**
  Poll to see whether there is a key stroke.

  @param  VOID

  @retval TRUE           Key stroke detected.
  @retval FALSE          No key stroke detected.

**/
BOOLEAN PollForKey()
{
  UINT8 i;
  BOOLEAN keyFound = FALSE;
  EFI_STATUS status;

  // power key
  status = PollPowerKey(&currPowerKeyState);
  if(status == EFI_SUCCESS)
  {
    if( currPowerKeyState !=  prevPowerKeyState  )
    {
      if( currPowerKeyState == TRUE )
      {
         // power key is pressed , queue an EFI key
        *keyBufferWrite = powerKey;
    
        keyBufferWrite++;
        if(keyBufferWrite >= &keyBuffer[MAX_KEYS_TO_BUFFER])
        {
          keyBufferWrite = &keyBuffer[0];
        }    
        keyFound = TRUE;
      }
      // update the current state 
      prevPowerKeyState  = currPowerKeyState;
    }
  }
  else
  {
    DEBUG((EFI_D_ERROR, "KeysDxe - PollPowerKey failed status:0x%08X\r\n",status));
  }

  // get the matrix
  status = PollKeypadMatrix( pCurrMatrix );
  if(status != EFI_SUCCESS)
  {	
    //Keypad API failed!
    DEBUG((EFI_D_ERROR, "KeysDxe - PollKeypadMatrix failed status:0x%08X\r\n",status));
    return FALSE;
  }

  // process the matrix
  ProcessMatrix();

  if( isMatrixChanged == TRUE )
  {
    // There is matrix change detected. 
    //Translate to  EFI keys and put them into the key buffer.
    if(ConvertEfiKeyCode( keysPressed,
                          numOfKeysPressed,
                          keysReleased,
                          numOfKeysReleased,
                          MAX_KEYS_PRESSED_RELEASED, 
                          EfiKeys, 
                          &numOfEfiKeys) == EFI_SUCCESS)
    {
      for( i=0; i<numOfEfiKeys; i++ )
      {
        *keyBufferWrite = EfiKeys[i];
    
        keyBufferWrite++;
        if(keyBufferWrite >= &keyBuffer[MAX_KEYS_TO_BUFFER] )
        {
          keyBufferWrite = &keyBuffer[0];
        }    
        keyFound = TRUE;
      }
    }

    //Clear the keys released.
    SetMem(keysReleased, MAX_KEYS_PRESSED_RELEASED, NONE);
    numOfKeysReleased = 0;
    SetMem(EfiKeys, MAX_KEYS_PRESSED_RELEASED, NONE);
    numOfEfiKeys = 0;
  }

  //Returns TRUE if any key was found.  FALSE if not.
  return keyFound;
}


/**
  Query Key buffer.

  @param  VOID

  @retval TRUE           Key buffer not empty.
  @retval FALSE          Key buffer empty.

**/
BOOLEAN IsKeyInBuffer()
{
  return (keyBufferRead != keyBufferWrite);
}

/**
  This callback is called by a consumer of the simple text input protocol
  when it is waiting on keystrokes to be passed to the console.  It gets
  polled frequently.

  @param  Event         An UEFI event to signal for.
  @param  Context       System required parameter, not used in this function.

  @retval VOID

**/
VOID EFIAPI WaitForKeyCallback (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  if( PollForKey() || IsKeyInBuffer() )
  {
    //Notify the system that a key is pending
    gBS->SignalEvent(Event);
  }
}


/**
  This callback is called by the keypad driver periodically to
  poll the PMIC keypad matrix 
  
  @param  Event        An UEFI event to signal for.
  @param  Context      System required parameter, not used in this function.

  @retval VOID

**/
VOID EFIAPI PollForKeyCallback (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_KEY_DATA         KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This = NULL;

  TimerRKStroke = TRUE;
 ( void )KeysDxeReadKeyStrokeEx( This,
                                  &KeyData );
  TimerRKStroke = FALSE;
}

/**
  This function is a common function called by KeysDxeReset &
  KeysDxeResetEx .

  @param  NONE

  @retval EFI_SUCCESS    Always

**/
EFI_STATUS KeysDxeResetCommon(void)
{
  //Reset the key device. This will reset and clear the key buffer
  keyBufferRead = keyBufferWrite = keyBuffer;
  numOfKeysReleased = numOfKeysPressed = 0;

  SetMem(matrixA, numOfRows, 0xFF);
  SetMem(matrixB, numOfRows, 0xFF);

  SetMem(keysPressed,  MAX_KEYS_PRESSED_RELEASED * sizeof(KEY_TYPE), NONE);
  SetMem(keysReleased, MAX_KEYS_PRESSED_RELEASED * sizeof(KEY_TYPE), NONE);
  SetEfiKeyDetection(FALSE);
  TimerRKStroke  = FALSE;
  currPowerKeyState = FALSE;
  prevPowerKeyState = FALSE;
  return EFI_SUCCESS;
}

/**
  This function is supposed to reset the input device and clear the buffer. 
  We don't have a way to reset the device, so just clear the buffer.

  @param  this                  An EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  ExtendedVerification  TRUE or FALSE for extended verification.

  @retval EFI_SUCCESS           Always 

**/
EFI_STATUS KeysDxeReset(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  IN BOOLEAN                         ExtendedVerification)
{
  return KeysDxeResetCommon();
}

/**
  This function is supposed to reset the input device and clear the buffer. 
  We don't have a way to reset the device, so just clear the buffer.

  @param  This                  An EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance.
  @param  ExtendedVerification  TRUE or FALSE for extended verification.

  @retval EFI_SUCCESS           Always 
**/
EFI_STATUS
EFIAPI
KeysDxeResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  return KeysDxeResetCommon();
}

/**
  This is the callback for the simple text input protocol to receive a keystroke 
  from the device.

  @param  this                  An EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  Key                   A UEFI key pointer to hold the key read from key buffer.

  @retval EFI_SUCCESS           A UEFI key found in key buffer
  @retval EFI_NOT_READY         No key found in key buffer

**/
EFI_STATUS KeysDxeReadKeyStroke(
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  OUT EFI_INPUT_KEY                  *Key )
{
  EFI_TPL  OldTpl;
  OldTpl =   gBS->RaiseTPL(TPL_CALLBACK);
  if( PollForKey() || IsKeyInBuffer() )
  {
    Key->ScanCode = keyBufferRead->ScanCode;
    Key->UnicodeChar = keyBufferRead->UnicodeChar;

    if(TimerRKStroke == FALSE)
    {
        keyBufferRead++;
        if(keyBufferRead >= &keyBuffer[MAX_KEYS_TO_BUFFER] )
        {
            keyBufferRead = &keyBuffer[0];
        }
    }
    gBS->RestoreTPL(OldTpl);	
    return EFI_SUCCESS;
  }
  gBS->RestoreTPL(OldTpl);
  return EFI_NOT_READY;
}

/**
  Reads the next keystroke from the input device.

  @param  This                   EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance pointer.
  @param  KeyData                A pointer to a buffer that is filled in with the
                                 keystroke state data for the key that was pressed.
  @retval EFI_SUCCESS            The keystroke information was returned.
  @retval EFI_NOT_READY          There was no keystroke data available.
  @retval EFI_INVALID_PARAMETER  KeyData is NULL.

**/
EFI_STATUS
EFIAPI
KeysDxeReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  EFI_STATUS ret = EFI_NOT_READY;
  KEYPAD_CONSOLE_IN_EX_NOTIFY *CurrentNotify;  
  LIST_ENTRY  *Link;

  if ( KeyData == NULL ) 
  {
    return EFI_INVALID_PARAMETER;
  }

  KeyData->KeyState.KeyShiftState = 0;
  KeyData->KeyState.KeyToggleState = 0;
  ret = KeysDxeReadKeyStroke ((EFI_SIMPLE_TEXT_INPUT_PROTOCOL*)This, &KeyData->Key);

  if( ret == EFI_SUCCESS )
  {
     /* Invoke notification functions if the key is registered. */
     for ( Link = GetFirstNode ( NotifyList );
           !IsNull ( NotifyList, Link );
           Link = GetNextNode ( NotifyList, Link ) ) 
     {
        CurrentNotify = BASE_CR ( Link,
                                  KEYPAD_CONSOLE_IN_EX_NOTIFY,
                                  NotifyEntry );
        if (IsKeyRegistered ( &CurrentNotify->KeyData, KeyData ) ) 
        { 
           CurrentNotify->KeyNotificationFn ( KeyData );
        }
     }	
  } 
  return ret; 
}

EFI_STATUS
EFIAPI
KeysDxeSetStateEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Check whether the pressed key matches a registered key or not.

  @param  RegsiteredData    A pointer to keystroke data for the key that was registered.
  @param  InputData         A pointer to keystroke data for the key that was pressed.

  @retval TRUE              Key pressed matches a registered key.
  @retval FLASE             Key pressed does not matches a registered key.

**/
BOOLEAN
EFIAPI
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegisteredData,
  IN EFI_KEY_DATA  *InputData
  )
{
  if( RegisteredData != NULL && InputData != NULL )
  {
     if ( ( RegisteredData->Key.ScanCode    != InputData->Key.ScanCode ) ||
          ( RegisteredData->Key.UnicodeChar != InputData->Key.UnicodeChar ) )
     {
       return FALSE;
     }
  }
  return TRUE;
}

/**
  Register a notification function for a particular keystroke .

  @param  This                        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance pointer.
  @param  KeyData                     A pointer to a buffer that is filled in with the keystroke
                                      information data for the key that was pressed.
  @param  KeyNotificationFunction     Points to the function to be called when the key
                                      sequence is typed specified by KeyData.
  @param  NotifyHandle                Points to the unique handle assigned to the registered notification.

  @retval EFI_SUCCESS                 The notification function was registered successfully.
  @retval EFI_OUT_OF_RESOURCES        Unable to allocate resources for necessary data structures.
  @retval EFI_INVALID_PARAMETER       KeyData or NotifyHandle or KeyNotificationFunction is NULL.

**/

EFI_STATUS
EFIAPI
KeysDxeRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN  EFI_KEY_DATA                       *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                         *NotifyHandle
  )
{
  LIST_ENTRY                      *Link;
  KEYPAD_CONSOLE_IN_EX_NOTIFY     *NewNotify;
  KEYPAD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;
  EFI_STATUS  Status;

  if ( KeyData == NULL || NotifyHandle == NULL || KeyNotificationFunction == NULL ) 
  {
    return EFI_INVALID_PARAMETER;
  }
  
  /* Return EFI_SUCCESS if the (KeyData, NotificationFunction) 
     is already registered. */

  for ( Link = GetFirstNode ( NotifyList );
        !IsNull ( NotifyList, Link );
        Link = GetNextNode ( NotifyList, Link ) ) 
  {
    CurrentNotify = BASE_CR ( Link, 
                              KEYPAD_CONSOLE_IN_EX_NOTIFY, 
                              NotifyEntry ); 

    if ( IsKeyRegistered ( &CurrentNotify->KeyData, KeyData ) )
    {
      if ( CurrentNotify->KeyNotificationFn == KeyNotificationFunction )
      {
        *NotifyHandle = CurrentNotify->NotifyHandle;
        return EFI_SUCCESS;
      } 
    }
  }

  /* Allocate resource to save the notification function */

  NewNotify = (KEYPAD_CONSOLE_IN_EX_NOTIFY *) AllocateZeroPool (sizeof (KEYPAD_CONSOLE_IN_EX_NOTIFY));
  if ( NULL == NewNotify )
  {
    return EFI_OUT_OF_RESOURCES;
  }

  NewNotify->KeyNotificationFn = KeyNotificationFunction;
  NewNotify->NotifyHandle      = (EFI_HANDLE) NewNotify;

  CopyMem ( &NewNotify->KeyData, KeyData, sizeof ( EFI_KEY_DATA ) );
  if( NotifyList != NULL )
  { 
    InsertTailList ( NotifyList, &NewNotify->NotifyEntry );
    numOfKeyRegistrations++;
  }

  /* if the numOfKeyRegistrations is 1, and not zero, then
     it is an indication that some client registered with the
     keypad driver, so lets start the periodic timer */
  if( numOfKeyRegistrations == 1)
  {
      Status = gBS->SetTimer (
                  CallbackTimer,
                  TimerPeriodic,
                  KEYPAD_TIMER_INTERVAL
                  );
      if (EFI_ERROR (Status)) {
          return Status; 
      } 
  }

  *NotifyHandle = NewNotify->NotifyHandle; 

  return EFI_SUCCESS;
}

/**
  Remove a registered notification function for a particular keystroke.

  @param  This                     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance pointer.
  @param  NotificationHandle       The handle of the notification function being unregistered.

  @retval EFI_SUCCESS              The notification function was unregistered successfully.
  @retval EFI_INVALID_PARAMETER    The NotificationHandle is invalid

**/
EFI_STATUS
EFIAPI
KeysDxeUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  )
{
  KEYPAD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;
  LIST_ENTRY                      *Link;

  if ( NotificationHandle == NULL ) 
  {
    return EFI_INVALID_PARAMETER;
  }
   
  /* Traverse notify list of keypad and remove 
     the entry of NotificationHandle. */

  for ( Link = GetFirstNode ( NotifyList );
        !IsNull ( NotifyList, Link );
        Link = GetNextNode ( NotifyList, Link ) ) 
  {
    CurrentNotify = BASE_CR ( Link, 
                              KEYPAD_CONSOLE_IN_EX_NOTIFY, 
                              NotifyEntry );       
    if ( CurrentNotify->NotifyHandle == NotificationHandle ) 
    {
      /* Remove the notification function from 
         NotifyList and free resources */
      RemoveEntryList ( &CurrentNotify->NotifyEntry );      

      FreePool ( CurrentNotify );
      numOfKeyRegistrations--;
      
      /* if the numOfKeyRegistrations is 0, then it is an indication
         that no client registered with the keypad driver for 
         notifications, so lets cancel the timer. */
      if( numOfKeyRegistrations == 0 )
      {
         gBS->CloseEvent ( CallbackTimer );
      }	  
      return EFI_SUCCESS;
    }
  }
  
  /* Cannot find the matching entry in database. */
  return EFI_INVALID_PARAMETER;
}

/**
  KeypadDxe driver initialization, it is also the entry point of KeypadDxe driver.

  @param[in] ImageHandle        The firmware allocated handle for the EFI image.  
  @param[in] SystemTable        A pointer to the EFI System Table.

  @retval EFI_SUCCESS           Initialization done successfully
  @retval non EFI_SUCCESS       Initialization failed.

**/
EFI_STATUS KeypadDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
  EFI_STATUS  Status;
  EFI_EVENT   WaitKeyEvt;

  //Initialize the key buffer to be empty
  keyBufferRead = keyBufferWrite = keyBuffer;

  // get the keypad configuration data
  Status = KeypadInit( &numOfRows,
                       &numOfColumns,
                       keyMap,
                       &powerKey
                     );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_INFO, "KeysDxeInitialize: KeypadInit failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // init matrix 
  SetMem(matrixA, numOfRows, 0xFF);
  SetMem(matrixB, numOfRows, 0xFF);

  SetMem(keysPressed,  MAX_KEYS_PRESSED_RELEASED*sizeof(KEY_TYPE), NONE);
  SetMem(keysReleased, MAX_KEYS_PRESSED_RELEASED*sizeof(KEY_TYPE), NONE);

  NotifyList = AllocateZeroPool(sizeof(LIST_ENTRY));
  if ( NULL == NotifyList )
  {
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeListHead ( NotifyList );
  // Initialize the WaitForKey event for the simple/Ex text input protocol.
  // The callback will be called anytime something is waiting on 'WaitForKey' 
  // and the event is not signalled.
  Status = gBS->CreateEvent (
                    EVT_NOTIFY_WAIT,
                    TPL_CALLBACK,
                    WaitForKeyCallback,
                    NULL,
                    &WaitKeyEvt
                    );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_INFO, "KeysDxeInitialize: Create Event failed,  Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  KeypadSimpleInputProtocolImplementation.WaitForKey = WaitKeyEvt;
  KeypadSimpleInputExProtocolImplementation.WaitForKeyEx = WaitKeyEvt;

  // Install our protocols.  The device path protocol is required for this driver
  // to show up to the console.
  Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gEfiSimpleTextInProtocolGuid,
                    &KeypadSimpleInputProtocolImplementation,
                    &gEfiSimpleTextInputExProtocolGuid,
                    &KeypadSimpleInputExProtocolImplementation,
                    &gEfiDevicePathProtocolGuid,
                    &KeysDxeDevicePath,
                    NULL
                    );

  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "KeysDxeInitialize: Install Protocol Interfaces failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // Setup a periodic timer, used for reading keystrokes at a fixed interval
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PollForKeyCallback,
                  NULL,
                  &CallbackTimer
                  );
  
ErrorExit:
  return Status;

}

