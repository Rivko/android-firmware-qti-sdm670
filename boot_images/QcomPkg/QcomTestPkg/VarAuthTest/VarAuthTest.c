/** @file VarAuthTest.c
   
  Tests for Authenticated Variables.

  Copyright (c) 2011-2013, 2019 by Qualcomm Technologies, Inc. All Rights Reserved
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/10/13   aus     Fix KW warnings
 01/17/13   vk      Fix warning
 06/29/12   niting  Updated test.
 12/15/11   niting  Initial Revision.

=============================================================================*/

#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/ImageAuthentication.h>
#include <Library/EfiFileLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Library/UefiLib.h>

STATIC EFI_GUID GlobalVariableGuid = EFI_GLOBAL_VARIABLE;
STATIC EFI_GUID DBGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;
STATIC EFI_GUID DBXGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;

UINT8 DataBuffer[8192];

extern EFI_GUID gQcomTokenSpaceGuid;

BOOLEAN
WaitForKey ( VOID )
{
  EFI_STATUS Status;
  EFI_INPUT_KEY Key;
  Key.ScanCode = SCAN_NULL;
  Key.UnicodeChar = 0;

  AsciiPrint("Press Enter to continue. Press Tab to skip.\n", 
             Key.ScanCode, Key.UnicodeChar);   
  while ((Key.UnicodeChar != CHAR_CARRIAGE_RETURN) && 
         (Key.UnicodeChar != CHAR_TAB))
  {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  }
  AsciiPrint("Key Detected: ScanCode = (0x%x), UnicodeChar =(0x%x)\n", 
             Key.ScanCode, Key.UnicodeChar);

  if (Key.UnicodeChar == CHAR_TAB)
  {
    AsciiPrint("Skipping\n\n");
    Status = FALSE;
  }
  else 
  {
    Status = TRUE;
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetData (
  IN CHAR8     *DataFile,
  IN CHAR16    *VarName,
  IN EFI_GUID  *VendorGuid,
  IN UINT32    Attributes
  )
{
  UINT32 DataSize;
  EFI_OPEN_FILE *File = NULL;
  EFI_STATUS Status = EFI_SUCCESS;

  DataSize = sizeof(DataBuffer);
  AsciiPrint("Opening Data File:\n%a\n", DataFile);
  File = EfiOpen(DataFile, EFI_FILE_MODE_READ, EFI_SECTION_ALL);

  do
  {
    if (File == NULL)
    {
      Status = EFI_NOT_FOUND;
      break;
    }
  
    if (WaitForKey())
    {
      Status = File->FsFileHandle->Read(File->FsFileHandle, &DataSize, DataBuffer);
    
      if (Status == EFI_SUCCESS)
      {
        AsciiPrint("Setting %s.\n", VarName);
        Status = gRT->SetVariable(VarName,
                                  VendorGuid,
                                  Attributes,
                                  DataSize,
                                  DataBuffer);
        if (Status != EFI_SUCCESS)
        {
          AsciiPrint("Could not set variable %s: 0x%08x\n\n", VarName, Status);
          break;
        }
        else
        {
          AsciiPrint("Successfully set variable %s\n\n", VarName);
        }
      }
    }
  } while (0);

  if (File != NULL)
    EfiClose(File);

  return Status;
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
VarAuthTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 *PKDataFile = "fs2:\\authtest\\Example_SetVariable_Data-PK_set_PK.bin";
  CHAR8 *KEKDataFile = "fs2:\\authtest\\Example_SetVariable_Data-Fabrikam_Test_KEK_CA_set_KEK.bin";
  CHAR8 *DBDataFile = "fs2:\\authtest\\Example_SetVariable_Data-signing_signatures_set_db.bin";
  CHAR8 *DBDataFile2 = "fs2:\\authtest\\Example_SetVariable_Data-LostCA_append_db.bin";
  CHAR8 *DBXDataFile = "fs2:\\authtest\\Example_SetVariable_Data-pressAnyKey2_hashes_append_dbx.bin";
  CHAR8 *DBXDataFile2 = "fs2:\\authtest\\Example_SetVariable_Data-LostCertificate_append_dbx.bin";
  CHAR8 *PKDataFile2 = "fs2:\\authtest\\Example_clear_PK.bin";
  CHAR8 *DBXLifetime1 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_1_append_dbx.bin";
  CHAR8 *DBXLifetime2 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_2_append_dbx.bin";
  CHAR8 *DBXLifetime3 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_3_append_dbx.bin";
  CHAR8 *DBXLifetime4 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_4_append_dbx.bin";
  CHAR8 *DBXLifetime5 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_5_append_dbx.bin";
  CHAR8 *DBXLifetime6 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_6_append_dbx.bin";
  CHAR8 *DBXLifetime7 = "fs2:\\authtest\\Example_SetVariable_Data-mixedHash_7_append_dbx.bin";
  CHAR8 *PKWP = "fs2:\\authtest\\MS_TEST_PK_setVariable.bin";
  CHAR8 *KEKWP = "fs2:\\authtest\\MS_TEST_KEK_setVariable.bin";
  CHAR8 *DBWP = "fs2:\\authtest\\WindowsPhoneSelfhost_test_only_db_setVariable.bin";
  CHAR8 *DB2WP = "fs2:\\authtest\\WindowsPhoneSelfhost_phoneprod_only_db_setVariable.bin";
  CHAR8 *DB3WP = "fs2:\\authtest\\WindowsPhoneSelfhost_test_phone_db_setVariable.bin";

  EFI_GUID *VendorGuid = &GlobalVariableGuid;
  UINT32 Attributes = (EFI_VARIABLE_RUNTIME_ACCESS | 
                       EFI_VARIABLE_BOOTSERVICE_ACCESS |
                       EFI_VARIABLE_NON_VOLATILE |
                       EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS);
  UINT32 AppendAttributes = (EFI_VARIABLE_RUNTIME_ACCESS | 
                             EFI_VARIABLE_BOOTSERVICE_ACCESS |
                             EFI_VARIABLE_NON_VOLATILE |
                             EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
                             EFI_VARIABLE_APPEND_WRITE);

  do
  {
    Status = SetData(PKDataFile, EFI_PLATFORM_KEY_NAME, VendorGuid, Attributes);

    Status = SetData(KEKDataFile, EFI_KEY_EXCHANGE_KEY_NAME, VendorGuid, Attributes);

    VendorGuid = &DBGuid;
    Status = SetData(DBDataFile, EFI_IMAGE_SECURITY_DATABASE, VendorGuid, Attributes);

    VendorGuid = &DBGuid;
    Status = SetData(DBDataFile2, EFI_IMAGE_SECURITY_DATABASE, VendorGuid, AppendAttributes);

    VendorGuid = &DBXGuid;
    Status = SetData(DBXDataFile, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

    VendorGuid = &DBXGuid;
    Status = SetData(DBXDataFile2, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

    VendorGuid = &GlobalVariableGuid;
    Status = SetData(PKDataFile2, EFI_PLATFORM_KEY_NAME, VendorGuid, Attributes);

    AsciiPrint("Running 04-LifetimeServicing test?\n");
    if (WaitForKey())
    {
      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime1, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);
     
      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime2, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime3, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime4, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime5, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime6, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);

      VendorGuid = &DBXGuid;
      Status = SetData(DBXLifetime7, EFI_IMAGE_SECURITY_DATABASE1, VendorGuid, AppendAttributes);
    }

    AsciiPrint("Running WP tests?\n");
    if (WaitForKey())
    {
      Status = SetData(PKWP, EFI_PLATFORM_KEY_NAME, VendorGuid, Attributes);
      
      Status = SetData(KEKWP, EFI_KEY_EXCHANGE_KEY_NAME, VendorGuid, Attributes);
      
      VendorGuid = &DBGuid;
      Status = SetData(DBWP, EFI_IMAGE_SECURITY_DATABASE, VendorGuid, Attributes);

      VendorGuid = &DBGuid;
      Status = SetData(DB2WP, EFI_IMAGE_SECURITY_DATABASE, VendorGuid, AppendAttributes);

      VendorGuid = &DBGuid;
      Status = SetData(DB3WP, EFI_IMAGE_SECURITY_DATABASE, VendorGuid, AppendAttributes);
    }

  } while (0);

  return Status;
}

