/** @file SecurityToggleApp.c
   
  Security Toggle Application

  Arguments passed in as Unicode Wide-characters: 

     /SecureBootEnable    :   Enables UEFI secure boot
     /SecureBootDisable   :   Disables UEFI secure boot

  If no arguments are provided, then enters interactive menu to
  perform the same functions through key presses.
  
  Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.  All Rights
  Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  2017-02-16   pr      Updated function to read certs based on Partition name
  2015-05-15   sm      Branched away debug policy to seperate app  
  2015-05-12   sm      Removed OS type dep and changed to read certs from PLAT partition
  2014-03-18   sm      Removed ReadFromFV as it migrated into a library
  2013-09-10   shl     Supported PPI display configuration
  2013-02-27   mic     Fixed KW warnings
  2012-09-17   yg      Use ReadAnyKey API
  2012-09-10   rsb     Support for DB, KEK, DBX, and Debug Policy for WP
  2012-08-10   yg      Use GetAnyKey API from QcomLib
  2012-07-31   mic     Support for setting DBX when enabling UEFI secure boot
  2012-07-25   rsb     Support for setting KEK and DB when enabling UEFI secure boot
  2012-06-28   rsb     Initial revision
 
=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BootConfig.h>
#include <Pi/PiPeiCis.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>
#include <Guid/ImageAuthentication.h>

STATIC CHAR16* SecBootKeysFileLists[] = {
  L"\\secboot\\KEK_SET.bin",
  L"\\secboot\\DB_SET.bin",
  L"\\secboot\\DBX_SET.bin",
  L"\\secboot\\TEST_PK_SET.bin",
  L"\\secboot\\TEST_PK_CLEAR.bin"
};

typedef enum _SECBOOT_VALUE_TYPE {
  TYPE_KEKPUB = 0,
  TYPE_SIGNATUREDB,
  TYPE_SIGNATUREDBX,
  TYPE_PKPUB,
  TYPE_PKPUB_CLEAR  
} SECBOOT_VALUE_TYPE;

STATIC EFI_STATUS EnterMenuInteractiveMode(
   VOID
   );

STATIC EFI_STATUS PromptForExit (VOID);

STATIC EFI_STATUS PerformSecureBootEnable(
  VOID
  );

STATIC EFI_STATUS PerformSecureBootDisable(
  VOID
  );

STATIC EFI_STATUS WriteSecureBootValue(
  IN SECBOOT_VALUE_TYPE ValueType,
  IN BOOLEAN DoFlushVariableServices
  );

EFI_STATUS IsSecureVariableSet(
  IN SECBOOT_VALUE_TYPE ValueType,
  OUT BOOLEAN *VariableIsSet
  );

/**
  Entry point for Security Toggle Application. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       Application performed the requested function
                            successfully
  @retval EFI_INVALID_PARAMETER  Invalid command argument
  @retval EFI_DEVICE_ERROR  Application failed
**/
EFI_STATUS
EFIAPI
SecurityToggleAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                 Status = EFI_SUCCESS;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage  = NULL;
  CHAR16                    *ArgStr = NULL;

  Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID *)&LoadedImage);
  if (EFI_ERROR(Status)) {
        return Status;
  }
  
  if ( (LoadedImage->LoadOptions == NULL) || (LoadedImage->LoadOptionsSize == 0) )
  {
    /* If no arguments are passed to the application, go into menu interactive mode. */
    return EnterMenuInteractiveMode();
  }

  ArgStr = (CHAR16 *)LoadedImage->LoadOptions;
  
  /* Parse command-line arguments */
  if (StrCmp(ArgStr, L"/SecureBootEnable") == 0)
  {
    Status = PerformSecureBootEnable();
  }
  else if (StrCmp(ArgStr, L"/SecureBootDisable") == 0)
  {
    Status = PerformSecureBootDisable();
  }
  else
  {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
  Enter in the interative menu mode of operation for updating security policy settings

  @retval EFI_SUCCESS 
**/
EFI_STATUS EnterMenuInteractiveMode(
   VOID
   )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  EFI_INPUT_KEY    Key;
  BOOLEAN          bUEFISecureBootEnabled = FALSE;
  BOOLEAN          bFoundKey = FALSE;
  BOOLEAN          bExitMenu = FALSE;

  do
  {
    Status = gST->ConOut->ClearScreen(gST->ConOut);

    AsciiPrint ("Security Policy Settings Menu\n"
                "-----------------------------\n\n");

    Status = IsSecureVariableSet(TYPE_PKPUB, &bUEFISecureBootEnabled);
    if (EFI_ERROR(Status))
    {
      bUEFISecureBootEnabled = FALSE;
    }

    if (bUEFISecureBootEnabled)
    {
      AsciiPrint ("UEFI Secure Boot is currently enabled\n");
    }
    else
    {
      AsciiPrint ("UEFI Secure Boot is currently disabled\n");
    }

    AsciiPrint ("\nPress [Volume-Up]   to Enable UEFI Secure Boot\n"
                  "Press [Volume-Down] to Disable UEFI Secure Boot\n\n"
                  "Press [Home]        to Exit\n\n");

    bFoundKey = FALSE;

    do 
    {
      Status = ReadAnyKey (&Key, 0);
      if (EFI_ERROR(Status))
      {
        goto clean_up;
      }

      if ((Key.ScanCode == SCAN_UP) || (Key.ScanCode == SCAN_DOWN) ||
          (Key.ScanCode == SCAN_HOME))
      {
        bFoundKey = TRUE;
      }
    }
    while (bFoundKey == FALSE);

    switch(Key.ScanCode)
    {
       case SCAN_UP:
         (VOID)PerformSecureBootEnable();
         break;

       case SCAN_DOWN:
         (VOID)PerformSecureBootDisable();
         break;
         
       case SCAN_HOME:
         bExitMenu = TRUE;
         break;

       default:
         break;
    }

  } while (bExitMenu == FALSE);

clean_up:

  return Status;
}


/**
  This function prompts the user to press any button to exit the application.
  
  @retval  EFI_SUCCESS 
**/
EFI_STATUS PromptForExit (VOID)
{
  AsciiPrint ("\nPress any key to return...\n");

  (VOID)ReadAnyKey (NULL, 0);

  return EFI_SUCCESS;
}

/**
  This function prompts the user to confirm or cancel enabling UEFI secure boot,
  and performs the action if requested.
  
  @retval  EFI_SUCCESS Successfully enabled or canceled
**/
EFI_STATUS
PerformSecureBootEnable(
  VOID
  )
{
   EFI_STATUS     Status;
   BOOLEAN        bSecureBootEnabled;
   EFI_INPUT_KEY  Key;
   EFI_STATUS     VarSetStatus;
   BOOLEAN        VariableIsSet = FALSE;

   Status = gST->ConOut->ClearScreen(gST->ConOut);
   
   Status = IsSecureVariableSet(TYPE_PKPUB, &bSecureBootEnabled);
   if (EFI_ERROR(Status))
   {
     bSecureBootEnabled = FALSE;
   }
   
   AsciiPrint ("Enable UEFI Secure Boot\n"
               "-----------------------\n\n"
               "This operation enables UEFI secure boot by programming the PK, KEK, DB and DBX\n"
               "UEFI secure boot is currently ");

   if (bSecureBootEnabled)
   {
     AsciiPrint ("enabled\n\n");
   }
   else
   {
     AsciiPrint ("disabled\n\n");
   }
   
   AsciiPrint ("Press [Volume-Up] to confirm\n"
               "Press [Any other key] to cancel\n");
   
   Status = ReadAnyKey (&Key, 0);
   if (EFI_ERROR(Status))
   {
     AsciiPrint ("Error: Failed key detection, return code %r\n", Status);
     return Status;
   }
    
   if (Key.ScanCode == SCAN_UP)
   {
     Status = WriteSecureBootValue(TYPE_KEKPUB, FALSE);
     if (Status == EFI_SECURITY_VIOLATION)
     {
        /* If we fail writing KEK due to security violation, then it is likely that we failed because
           the same KEK is already programmed and the timestamp validation check fails. */

        /* Check to see if KEK is already set. */
        VarSetStatus = IsSecureVariableSet(TYPE_KEKPUB, &VariableIsSet);
        if ((EFI_ERROR(VarSetStatus) == FALSE) && (VariableIsSet == TRUE))
        {

          /* Since it is already set, treat the security violation as a soft error and continue
             using the existing KEK already programmed in the device. */
          DEBUG((EFI_D_ERROR,"SecurityToggleApp WARNING: KEK is already set and cannot be overwritten, using existing KEK\n\n"));
          Status = EFI_SUCCESS;
        }
        else
        {
           AsciiPrint ("\nRESULT: Error: Failed writing KEK secure boot variable, return code %r\n", Status);
           goto clean_up;
        }
     }
     else if (EFI_ERROR(Status))
     {
       AsciiPrint ("\nRESULT: Error: Failed writing KEK secure boot variable, return code %r\n", Status);
       goto clean_up;
     }

     Status = WriteSecureBootValue(TYPE_SIGNATUREDB, FALSE);
     if (Status == EFI_SECURITY_VIOLATION)
     {
        /* If we fail writing DB due to security violation, then it is likely that we failed because
           the same DB is already programmed and the timestamp validation check fails. */

        /* Check to see if DB is already set. */
        VarSetStatus = IsSecureVariableSet(TYPE_SIGNATUREDB, &VariableIsSet);
        if ((EFI_ERROR(VarSetStatus) == FALSE) && (VariableIsSet == TRUE))
        {

          /* Since it is already set, treat the security violation as a soft error and continue
             using the existing DB already programmed in the device. */
          DEBUG((EFI_D_ERROR,"SecurityToggleApp WARNING: DB is already set and cannot be overwritten, using existing DB\n\n"));
          Status = EFI_SUCCESS;
        }
        else
        {
           AsciiPrint ("\nRESULT: Error: Failed writing DB secure boot variable, return code %r\n", Status);
           goto clean_up;
        }
     }
     else if (EFI_ERROR(Status))
     {
       AsciiPrint ("\nRESULT: Error: Failed writing DB secure boot variable, return code %r\n", Status);
       goto clean_up;
     }

     Status = WriteSecureBootValue(TYPE_SIGNATUREDBX, FALSE);
     if (Status == EFI_SECURITY_VIOLATION)
     {
        /* If we fail writing DBX due to security violation, then it is likely that we failed because
           the same DBX is already programmed and the timestamp validation check fails. */

        /* Check to see if DBX is already set. */
        VarSetStatus = IsSecureVariableSet(TYPE_SIGNATUREDBX, &VariableIsSet);
        if ((EFI_ERROR(VarSetStatus) == FALSE) && (VariableIsSet == TRUE))
        {

          /* Since it is already set, treat the security violation as a soft error and continue
             using the existing DBX already programmed in the device. */
          DEBUG((EFI_D_ERROR,"SecurityToggleApp WARNING: DBX is already set and cannot be overwritten, using existing DBX\n\n"));
          Status = EFI_SUCCESS;
        }
        else
        {
           AsciiPrint ("\nRESULT: Error: Failed writing DBX secure boot variable, return code %r\n", Status);
           goto clean_up;
        }
     }
     else if (EFI_ERROR(Status))
     {
       AsciiPrint ("\nRESULT: Error: Failed writing DBX secure boot variable, return code %r\n", Status);
       goto clean_up;
     }

     /* PKpub variable needs to be written last to ensure that KEK and DB can be written without being signed.
        Flush the variable services so that the change takes effect immediately in case the device is
        power cycled instead of properly soft reset. */
     Status = WriteSecureBootValue(TYPE_PKPUB, TRUE);
     if (EFI_ERROR(Status))
     {
       AsciiPrint ("\nRESULT: Error: Failed writing PK secure boot variable, return code %r\n", Status);
       goto clean_up;
     }
     else
     {
       AsciiPrint ("\nRESULT: Successfully enabled UEFI secure boot\n");
     }
    
   }
   else
   {
     AsciiPrint ("\nRESULT: Cancel\n");
   }

clean_up:

   (VOID)PromptForExit();

   return Status; 
}

/**
  This function prompts the user to confirm or cancel disabling UEFI secure boot,
  and performs the action if requested.
  
  @retval  EFI_SUCCESS Successfully disabled or canceled
**/
EFI_STATUS
PerformSecureBootDisable(
  VOID
  )
{
   EFI_STATUS     Status;
   BOOLEAN        bSecureBootEnabled;
   EFI_INPUT_KEY  Key;

   Status = gST->ConOut->ClearScreen(gST->ConOut);
    
   Status = IsSecureVariableSet(TYPE_PKPUB, &bSecureBootEnabled);
   if (EFI_ERROR(Status))
   {
     bSecureBootEnabled = FALSE;
   }

   AsciiPrint ("Disable UEFI Secure Boot\n"
               "-----------------------\n\n"
               "This operation disables UEFI secure boot by clearing the Platform Key\n"
               "UEFI secure boot is currently ");
   
   if (bSecureBootEnabled)
   {
     AsciiPrint ("enabled\n\n");
   }
   else
   {
     AsciiPrint ("disabled\n\n");
   }
   
   AsciiPrint ("Press [Volume-Up] to confirm\n"
               "Press [Any other key] to cancel\n");
   
   Status = ReadAnyKey (&Key, 0);
   if (EFI_ERROR(Status))
   {
     AsciiPrint ("Error: Failed key detection, return code %r\n", Status);
     return Status;
   }
    
   if (Key.ScanCode == SCAN_UP)
   {
     Status = WriteSecureBootValue(TYPE_PKPUB_CLEAR, TRUE);
     if (EFI_ERROR(Status))
     {
       AsciiPrint ("\nRESULT: Error: Failed writing PK secure boot variable, return code %r\n", Status);
     }
     else
     {
       AsciiPrint ("\nRESULT: Successfully disabled UEFI secure boot\n");
     }
   }
   else
   {
     AsciiPrint ("\nRESULT: Cancel\n");
   }

   (VOID)PromptForExit();

   return Status; 
}

/**
  This function writes a given UEFI secure boot key value to the corresponding UEFI variable by
  locating the data from the firmware volume.

  Currently only TYPE_PKPUB and TYPE_PKPUB_CLEAR are supported.

  @param[in]  ValueType                 The type of UEFI secure boot key to write. 
  @param[in]  DoFlushVariableServices   TRUE to flush the UEFI NV variable services RAM
                                        so change is effective in case of power cycle
   
  @retval EFI_SUCCESS     The data is found from the firmware volume and written
                          to the UEFI NV variable.
  @retval EFI_UNSUPPORTED The given key type is not supported.
  @retval EFI_NOT_FOUND        The key data cannot be found in the firmware volume
  @retval EFI_OUT_OF_RESOURCES There were not enough resources to allocate a 
                                data buffer or complete the operations
  @retval EFI_ACCESS_DENIED The operation is not allowed.
  @retval EFI_SECURITY_VIOLATION The operation is a security violation.
**/
EFI_STATUS
WriteSecureBootValue(
  IN SECBOOT_VALUE_TYPE ValueType,
  IN BOOLEAN DoFlushVariableServices
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT8            *DataBuffer = NULL;
  UINTN             DataSize = 0;
  CHAR16           *VarName = NULL;
  EFI_GUID         *VarGuid = NULL;
  EFI_GUID   *RootDeviceType = NULL;
  EFI_GUID   *PartitionType  = NULL;
  STATIC EFI_GUID   GlobalVariableGuid = EFI_GLOBAL_VARIABLE;
  STATIC EFI_GUID   DBGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;
  UINT32            Attributes = (EFI_VARIABLE_RUNTIME_ACCESS | 
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                  EFI_VARIABLE_NON_VOLATILE |
                                  EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS);
  UINT32            FileAttributes = (BLK_IO_SEL_PARTITIONED_GPT | 
                                      BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE |
                                      BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM | 
                                      BLK_IO_SEL_MATCH_PARTITION_LABEL |
									  BLK_IO_SEL_STRING_CASE_INSENSITIVE);
  
  switch (ValueType)
  {
    case TYPE_KEKPUB:

      VarName = EFI_KEY_EXCHANGE_KEY_NAME;
      VarGuid = &GlobalVariableGuid;
      break;
      
    case TYPE_SIGNATUREDB:
      VarName = EFI_IMAGE_SECURITY_DATABASE;
      VarGuid = &DBGuid;
      break;
      
    case TYPE_SIGNATUREDBX:
      VarName = EFI_IMAGE_SECURITY_DATABASE1;
      VarGuid = &DBGuid;
      break;
  
    case TYPE_PKPUB:
    case TYPE_PKPUB_CLEAR:
      VarName = EFI_PLATFORM_KEY_NAME;
      VarGuid = &GlobalVariableGuid;
      break;

    default:
      return EFI_UNSUPPORTED;
  }

  /*Set the filename from keysFileLists structure according to ValueType(PK/PKPUB/DB/DBX) parameter*/
  Status = LoadFileFromPartition(FileAttributes, SecBootKeysFileLists[ValueType], RootDeviceType, PartitionType, L"PLAT", &DataBuffer, &DataSize);
  if (EFI_ERROR(Status))
  {
    goto clean_up;
  }

  /* Attempt to write the data to the specified variable. */
  Status = gRT->SetVariable(VarName, VarGuid, Attributes, DataSize, DataBuffer);
  if (Status == EFI_NOT_FOUND)
  {
    Status = EFI_SUCCESS;
  }
  else if (EFI_ERROR(Status))
  {
    goto clean_up;
  }
  else
  {
    if (DoFlushVariableServices == TRUE)
    {
      /* On successful write to variable services, trigger a flush to ensure it is written to disk without
         requring a reset. */
      (VOID)FlushVariableServices();
    }
  }
  
clean_up:

  /* Free up the key buffer */
  if (DataBuffer != NULL)
  {
      gBS->FreePool(DataBuffer);
      DataBuffer = NULL;
  }

  return Status;
}

/**
  This function reads 1 byte from the given secure boot UEFI NV variable to return whether or not the variable exists
  and has data.

  If VariableIsSet is NULL, then ASSERT()

  @param[in]   ValueType               The type of secure boot variable to read.
  @param[out]  VariableIsSet           TRUE if the variable exists, FALSE otherwise. 
   
  @retval EFI_SUCCESS
  @retval EFI_OUT_OF_RESOURCES There were not enough resources to allocate a 
                                data buffer or complete the operations
**/
EFI_STATUS
IsSecureVariableSet(
  IN SECBOOT_VALUE_TYPE ValueType,
  OUT BOOLEAN *VariableIsSet
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  UINT8                            VarData = 0;
  UINTN                            DataSize = sizeof(VarData);
  CHAR16                          *VarName = NULL;
  EFI_GUID                        *VarGuid = NULL;
  STATIC EFI_GUID GlobalVariableGuid = EFI_GLOBAL_VARIABLE;
  STATIC EFI_GUID DBGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;

  ASSERT(VariableIsSet != NULL);

  switch (ValueType)
  {
    case TYPE_KEKPUB:
      VarName = EFI_KEY_EXCHANGE_KEY_NAME;
      VarGuid = &GlobalVariableGuid;

      break;
      
    case TYPE_SIGNATUREDB:
      VarName = EFI_IMAGE_SECURITY_DATABASE;
      VarGuid = &DBGuid;

      break;
      
    case TYPE_SIGNATUREDBX:
      VarName = EFI_IMAGE_SECURITY_DATABASE1;
      VarGuid = &DBGuid;

      break;
      
    case TYPE_PKPUB:
      VarName = EFI_PLATFORM_KEY_NAME;
      VarGuid = &GlobalVariableGuid;

      break;

    case TYPE_PKPUB_CLEAR:
      VarName = EFI_PLATFORM_KEY_NAME;
      VarGuid = &GlobalVariableGuid;
      
      break;

    default:
      return EFI_UNSUPPORTED;
  }

  Status = gRT->GetVariable (VarName, VarGuid,
                             NULL, &DataSize, &VarData);

  /* Indicate that secure boot is enabled if the size of PK is 1 byte or larger. */
  if ((EFI_ERROR(Status) == FALSE) || (Status == EFI_BUFFER_TOO_SMALL))
  {
    Status = EFI_SUCCESS;
    if (VariableIsSet != NULL)
    {
      *VariableIsSet = TRUE;
    }
  }
  else
  {
    /* Indicate that secure boot is not enabled if the PK variable does not exist
       or encountered some other error reading it. */
    
    Status = EFI_SUCCESS;
    if (VariableIsSet != NULL)
    {
      *VariableIsSet = FALSE;
    }
  }
  
  return Status;
}
