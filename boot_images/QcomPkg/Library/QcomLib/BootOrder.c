/** @file BootOrder.c
   
  Library to manage Additions/Updates of BootOrder related entries.
  
  Copyright 2012,2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  09/02/2014   vk      KW fixes
  04/04/2014   vk      Cleanup warning    
  01/13/2014   niting  Changes to enumerate boot options when non-removable boot option does not exist
  01/17/2012   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Library/QcomLib.h>

#define BOOTORDER_VARIABLE_ATTRIBUTES    (EFI_VARIABLE_BOOTSERVICE_ACCESS  | \
                                          EFI_VARIABLE_RUNTIME_ACCESS      | \
                                          EFI_VARIABLE_NON_VOLATILE)

static UINTN DEBUG_ENABLED = FALSE;

/*
 *  Returns TRUE if the device path searching is found, FALSE if NOT found
 * */
BOOLEAN
FindDevicePath (EFI_DEVICE_PATH_PROTOCOL   *DevPathToFind,
                EFI_DEVICE_PATH_PROTOCOL   *DevPathToSrchIn)
{
  EFI_DEVICE_PATH_PROTOCOL   *TempDevPath;

  TempDevPath = DevPathToSrchIn;
  while (!IsDevicePathEndType (TempDevPath)) {
    /* Check if the dev path matches then return TRUE */
    if (CompareMem(TempDevPath, DevPathToFind, GetDevicePathSize(TempDevPath)) == 0)
      return TRUE;

    TempDevPath = NextDevicePathNode (TempDevPath);
  }

  /* Not found */
  return FALSE;
}

extern EFI_GUID gEfiGlobalVariableGuid;

#define MAX_BOOT_ENTRY_DATA_SIZE    2048
#define MAX_BOOT_ORDER_DATA_SIZE    512


/* Actual buffer passed should have more space for additional entry to add
 * ie should be atleast +2 bytes should be accomodable */
EFI_STATUS
AddToBootOrder (IN OUT UINT16* BootOrderData, 
                IN OUT UINTN*  VarSize,
                IN     UINT16  BootVarOffset,
                IN     UINTN   Flags)
{
  UINTN i;

  for (i = 0; i < *VarSize / 2; ++i)
  {
    if (BootOrderData[i] == BootVarOffset)
      return EFI_ALREADY_STARTED;
  }

  if ((Flags & ADD_FLAG_ADD_FIRST) == 0)
  {
    BootOrderData[i] = BootVarOffset;
  }
  else
  {
    CopyMem (&BootOrderData[1], &BootOrderData[0], *VarSize);
    BootOrderData[0] = BootVarOffset;
  }
  *VarSize += 2;

  return EFI_SUCCESS;
}

EFI_STATUS
WriteBootOrderVariables (IN UINT16    BootVarOffset,
                         IN UINT16   *BootEntryData,
                         IN UINTN     VarSize,
                         IN UINTN     AddFlags)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR16     BootVarName[12];
  UINT16     *BootOrderData;

  UnicodeSPrint (BootVarName, sizeof(BootVarName), L"Boot%04X", BootVarOffset);

  Status = gRT->SetVariable (BootVarName, &gEfiGlobalVariableGuid,
                             BOOTORDER_VARIABLE_ATTRIBUTES,
                             VarSize, BootEntryData);

  if (Status != EFI_SUCCESS)
  {
    AsciiPrint ("SetVariable Failed\r\n");
    return Status;
  }

  VarSize = MAX_BOOT_ORDER_DATA_SIZE;
  BootOrderData = AllocatePool (VarSize);
  if (BootOrderData == NULL)
    return EFI_OUT_OF_RESOURCES;

  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid,
                             NULL, &VarSize, BootOrderData);
  if (Status != EFI_SUCCESS)
    VarSize = 0;

  if (AddToBootOrder (BootOrderData, &VarSize, BootVarOffset, AddFlags) == EFI_SUCCESS)
    Status = gRT->SetVariable (L"BootOrder", &gEfiGlobalVariableGuid,
                               BOOTORDER_VARIABLE_ATTRIBUTES,
                               VarSize, BootOrderData);

  if (DEBUG_ENABLED)
  {
    UINTN i;

    AsciiPrint ("BootOrder : ");
    for (i = 0; i < (VarSize / 2); ++i)
      AsciiPrint ("%04x  ", BootOrderData[i]);
    AsciiPrint ("\r\n");
  }

  if (BootOrderData)
    FreePool (BootOrderData);

  return Status;
}

EFI_STATUS
AddBootOrderEntry (IN UINT16                      BootVarOffset,
                   IN CHAR16                     *Description, 
                   IN EFI_DEVICE_PATH_PROTOCOL   *ImageDevPath,
                   IN UINT8                      *OptionalData OPTIONAL,
                   IN UINT16                      OptionDataSize,
                   IN UINTN                       AddFlags)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT16     *BootEntryData, *Ptr;
  UINTN      VarSize, DevPathLen;
  CHAR16    *DescPtr;

  BootEntryData = AllocatePool (MAX_BOOT_ENTRY_DATA_SIZE);
  if (BootEntryData == NULL)
    return EFI_OUT_OF_RESOURCES;

  /* Prepare BootOrder data per UEFI Spec using the params passed in */
  VarSize = 0;
  Ptr = BootEntryData;

  /* Attributes */
  *((UINT32*)Ptr) = LOAD_OPTION_ACTIVE | LOAD_OPTION_CATEGORY_APP;
  Ptr += 2;
  VarSize += 4;

  /*  FilePathListLength  */
  DevPathLen = GetDevicePathSize (ImageDevPath);
  *Ptr = DevPathLen;
  ++Ptr;
  VarSize += 2;

  DescPtr = Description;
  /* Description  Nul terminated */
  while (1)
  {
    *Ptr = *DescPtr;
    ++Ptr;
    VarSize += 2;

    if (*DescPtr == 0)
      break;

    if (VarSize >= MAX_BOOT_ENTRY_DATA_SIZE)
    {
      AsciiPrint ("Description is too huge %d\r\n", VarSize);
      goto Exit;
    }
    ++DescPtr;
  }

  /*  FilePathList  */
  VarSize += DevPathLen;
  if (VarSize >= MAX_BOOT_ENTRY_DATA_SIZE)
  {
    AsciiPrint ("DevPath Len is too huge %d\r\n", VarSize);
    goto Exit;
  }
  CopyMem (Ptr, ImageDevPath, DevPathLen);
  Ptr += (DevPathLen / 2);

  /* Optional Data */
  if (OptionalData && (OptionDataSize > 0))
  {
    VarSize += OptionDataSize;
    if (VarSize >= MAX_BOOT_ENTRY_DATA_SIZE)
    {
      AsciiPrint ("Optional Data Len is too huge %d\r\n", VarSize);
      goto Exit;
    }
    CopyMem (Ptr, OptionalData, OptionDataSize);
  }

  /* Now Boot order data is ready in the way UEFI Spec specifies
   * Write the variables */
  Status = WriteBootOrderVariables (BootVarOffset, BootEntryData, VarSize, AddFlags);

  if (DEBUG_ENABLED)
    Print (L"Added %s to %04x\r\n", Description, BootVarOffset);

Exit:
  if (BootEntryData)
    FreePool (BootEntryData);

  return Status;
}

EFI_STATUS
GetBootOrderEntryFields (IN  UINT32                    *BootOrderData,
                         IN  UINT32                     Size,
                         OUT UINT16                   **DescriptionPtr OPTIONAL,
                         OUT EFI_DEVICE_PATH_PROTOCOL **FilePathListPtr OPTIONAL)
{
  CHAR16* Ptr;
  EFI_DEVICE_PATH_PROTOCOL *DevPath;
  UINT16 ListLen, PathSize;

  /* Minimum 12 bytes needed for empty Load Options */
  if (Size <= 12)
    return EFI_INVALID_PARAMETER;

  Ptr = (UINT16*)BootOrderData;

  /* Skip 4 bytes Attributes */
  Ptr += 2; Size -= 4;

  /* FilePathListLen */
  ListLen = *Ptr;
  Ptr += 1;
  Size -= 2;

  if (DescriptionPtr)
    *DescriptionPtr = Ptr;

  /* Description */
  while (*Ptr)
  {
    ++Ptr;
    Size -= 2;
    if (Size == 0)
      return EFI_INVALID_PARAMETER;
  }

  /* Past nul termination */
  ++Ptr;
  Size -= 2;

  /* FilePathList */
  if (FilePathListPtr)
    *FilePathListPtr = (EFI_DEVICE_PATH_PROTOCOL*)Ptr;

  DevPath = (EFI_DEVICE_PATH_PROTOCOL *)Ptr;
  PathSize = GetDevicePathSize(DevPath);

  if ((Size < PathSize) || (PathSize != ListLen))
    return EFI_INVALID_PARAMETER;

  return EFI_SUCCESS;
}

EFI_STATUS
GetSelfDevicePath (IN  EFI_HANDLE                  ImageHandle,
                   OUT EFI_DEVICE_PATH_PROTOCOL  **ImgDevPathPtr)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath, *DevicePath;

  if (ImgDevPathPtr == NULL)
    return EFI_INVALID_PARAMETER;

  LoadedImage = NULL;
  Status = gBS->HandleProtocol (ImageHandle,
                                &gEfiLoadedImageProtocolGuid,
                                (VOID *)&LoadedImage);
  if (EFI_ERROR(Status))
    return Status;

  FilePath = LoadedImage->FilePath;

  Status = gBS->HandleProtocol (LoadedImage->DeviceHandle,
                                &gEfiDevicePathProtocolGuid,
                                (VOID *)&DevicePath);

  /* This buffer need to be freed */
  *ImgDevPathPtr = AppendDevicePath(DevicePath, FilePath);
  return Status;
}

/* Returns TRUE if the entry is in one of the BootOrders, returns the
 * offset to enable it as bootnext
 * FALSE if it doesn't even exist in BootOrder 
 * Pass pointers to get the BootOrder Variable and Boot#### Variables back 
 * If the variables are passed back then those need to be freed using FreePool
 * BootOrder buffer should be atleast of size MAX_BOOT_ORDER_DATA_SIZE
 * BootEntryData should be atleast of size MAX_BOOT_ENTRY_DATA_SIZE */
BOOLEAN
TestIfInBootOrder (IN  EFI_DEVICE_PATH_PROTOCOL  *ImgDevPathPtr,
                   OUT UINT16   *Offset,
                   IN  UINT16   *BootOrderPtr,
                   IN  UINT32    BootOrderDataSize)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            *BootEntryData;
  UINTN             i;
  CHAR16            BootVarName[12];
  UINTN             VarSize;
  EFI_DEVICE_PATH_PROTOCOL    *BootEntryDevPath;

  if (Offset == NULL)
    return FALSE;

  *Offset = 0;
  BootEntryData = AllocatePool (MAX_BOOT_ENTRY_DATA_SIZE);

  for (i = 0; i < (BootOrderDataSize / 2); ++i)
  {
    /* Check if the entry already exists */
    UnicodeSPrint (BootVarName, sizeof(BootVarName), L"Boot%04X", BootOrderPtr[i]);
    VarSize = MAX_BOOT_ENTRY_DATA_SIZE;

    Status = gRT->GetVariable (BootVarName, &gEfiGlobalVariableGuid,
                               NULL, &VarSize, BootEntryData);

    if (Status != EFI_SUCCESS)
      continue;

    /* See if in the BootOrder we already have our entry */
    Status = GetBootOrderEntryFields (BootEntryData, VarSize, NULL, &BootEntryDevPath);
    if ((Status == EFI_SUCCESS) && BootEntryDevPath)
    {
      if (FindDevicePath (ImgDevPathPtr, BootEntryDevPath))
      {
        /* Found the BootOrder Entry with our device path */
        *Offset = BootOrderPtr[i];

        if (DEBUG_ENABLED)
          AsciiPrint ("Found Image Devpath in BootOrder Boot%04X\r\n", BootOrderPtr[i]);

        FreePool (BootEntryData);
        return TRUE;
      }
    }
  }

  if (BootEntryData)
    FreePool (BootEntryData);
        
  if (DEBUG_ENABLED)
    AsciiPrint ("Image Devpath Not Found in BootOrder\r\n");

  return FALSE;
}

/* For now just assume that we came here since didn't find an entry */
EFI_STATUS
MakeBootOrderOffset (IN UINT16* CurrentBootOrder, IN UINTN VarSize,
                     OUT  UINT16* BootDPOffset)
{
  UINTN i;
  UINT16 Offset = 0;

  if ((BootDPOffset == NULL) || (CurrentBootOrder == NULL) || (VarSize == 0))
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < VarSize / 2; ++i)
  {
    if (CurrentBootOrder[i] == Offset)
      ++Offset;
  }

  if (BootDPOffset)
    *BootDPOffset = Offset;
  
  return EFI_SUCCESS;
}

EFI_STATUS
ActivateBootNext (UINTN BootDPOffset)
{
  UINT16 BootNextOffset;

  if (DEBUG_ENABLED)
    AsciiPrint ("Configure BootNext to %d\r\n", BootDPOffset);

  BootNextOffset = BootDPOffset;
  return gRT->SetVariable (L"BootNext", &gEfiGlobalVariableGuid,
                           BOOTORDER_VARIABLE_ATTRIBUTES,
                           2, &BootNextOffset);
}

EFI_STATUS
RemoveFromBootOrder (IN CHAR16 *Description)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            *BootEntryData;
  UINTN             i, DescLen;
  CHAR16            BootVarName[12];
  UINTN             VarSize;
  UINTN             BootOrderDataSize, BakBootOrderDataSize;
  UINT16            *BootOrderData, *BackupBootOrder, *BakPtr;
  UINT16            *DescrPtr;

  if (Description == NULL)
    return EFI_INVALID_PARAMETER;

  DescLen = StrLen (Description);

  BootOrderData = AllocatePool (MAX_BOOT_ORDER_DATA_SIZE);
  if (BootOrderData == NULL)
  {
    AsciiPrint ("ERROR: Memory Allocation failed !\n");
    return EFI_OUT_OF_RESOURCES;
  }

  BootOrderDataSize = MAX_BOOT_ORDER_DATA_SIZE;
  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid,
                             NULL, &BootOrderDataSize, BootOrderData);

  if ((Status != EFI_SUCCESS) || (BootOrderDataSize < sizeof(UINT16)))
    return EFI_NOT_FOUND;

  BakPtr = BackupBootOrder = AllocatePool (MAX_BOOT_ORDER_DATA_SIZE);
  if (BakPtr == NULL)
  {
    AsciiPrint ("ERROR: Memory Allocation failed !\n");
    return EFI_OUT_OF_RESOURCES;
  }

  BakBootOrderDataSize = 0;

  BootEntryData = AllocatePool (MAX_BOOT_ENTRY_DATA_SIZE);
  if (BootEntryData == NULL)
  {
    AsciiPrint ("ERROR: Memory Allocation failed !\n");
    return EFI_OUT_OF_RESOURCES;
  }

  for (i = 0; i < (BootOrderDataSize / 2); ++i)
  {
    /* Check if the entry already exists */
    UnicodeSPrint (BootVarName, sizeof(BootVarName), L"Boot%04X", BootOrderData[i]);
    VarSize = MAX_BOOT_ENTRY_DATA_SIZE;

    Status = gRT->GetVariable (BootVarName, &gEfiGlobalVariableGuid,
                               NULL, &VarSize, BootEntryData);

    if (Status != EFI_SUCCESS)
    {
      Print (L"Could not find %s\r\n", BootVarName);
      continue;
    }

    DescrPtr = NULL;

    /* See if in the BootOrder we already have our entry */
    Status = GetBootOrderEntryFields (BootEntryData, VarSize, &DescrPtr, NULL);
    if ((Status == EFI_SUCCESS) && DescrPtr)
    {
      if (StrnCmp (Description, DescrPtr, DescLen) == 0)
      {
        /* Found the BootOrder Entry we were looking for */

        if (DEBUG_ENABLED)
          AsciiPrint ("Found BootOrder %s in %s\r\n", DescrPtr, BootVarName);

        /* Delete the variable */
        gRT->SetVariable (BootVarName, &gEfiGlobalVariableGuid,
                          BOOTORDER_VARIABLE_ATTRIBUTES, 0, BootEntryData);
        continue;
      }
    }
    *BakPtr = BootOrderData[i];
    ++BakPtr;
    BakBootOrderDataSize += 2;
  }

  /* Delete BootOrder and create new one with backup we have */
  gRT->SetVariable (L"BootOrder", &gEfiGlobalVariableGuid, BOOTORDER_VARIABLE_ATTRIBUTES, 0, BootEntryData);

  Status = gRT->SetVariable (L"BootOrder", &gEfiGlobalVariableGuid, BOOTORDER_VARIABLE_ATTRIBUTES, BakBootOrderDataSize, BackupBootOrder);

  if (BootEntryData)
    FreePool (BootEntryData);
  if (BackupBootOrder)
    FreePool (BackupBootOrder);

  return FALSE;
}

/* If Devpath is given then, the given dev path is added to BootOrder
 * If DevPath is NOT given, then the image's current location is added
 * to the bootOrder
 *
 * Even if the boot order exists for the current image, its Description,
 * Optional Data (which would be command line arguments) can be updated */
EFI_STATUS
AddDevPathToBootOrder (IN EFI_HANDLE                 ImageHandle,
                       IN EFI_DEVICE_PATH_PROTOCOL  *DevPath        OPTIONAL,
                       IN CHAR16                    *Description,
                       IN UINT8                     *OptionalData   OPTIONAL,
                       IN UINTN                      OptionDataSize,
                       IN UINTN                      AddFlags)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_DEVICE_PATH_PROTOCOL    *ImageDevPath, *DevPathToFree;
  UINTN                       BootOrderDataSize;
  UINT16                      *BootOrderData;
  UINT16                      BootDPOffset;

  DevPathToFree = NULL;
  if (DevPath == NULL)
  {
    Status = GetSelfDevicePath (ImageHandle, &ImageDevPath);
    if (Status != EFI_SUCCESS)
      return Status;
    DevPathToFree = ImageDevPath;
    if (DEBUG_ENABLED)
      AsciiPrint ("No device path given, adding self image to BootOrder\r\n");
  }
  else
    ImageDevPath = DevPath;

  /* Read BootOrder variable */
  BootOrderData = AllocatePool (MAX_BOOT_ORDER_DATA_SIZE);
  if (BootOrderData == NULL)
  {
    AsciiPrint ("ERROR: Memory Allocation failed !\n");
    return EFI_OUT_OF_RESOURCES;
  }

  BootOrderDataSize = MAX_BOOT_ORDER_DATA_SIZE;
  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid,
                             NULL, &BootOrderDataSize, BootOrderData);

  if ((Status == EFI_SUCCESS) && (BootOrderDataSize > 0))
  {
    if (TestIfInBootOrder (ImageDevPath, &BootDPOffset, BootOrderData, BootOrderDataSize) != TRUE)
      MakeBootOrderOffset (BootOrderData, BootOrderDataSize, &BootDPOffset);
  }
  else
  {
    BootDPOffset = 0;
    if (DEBUG_ENABLED)
      AsciiPrint ("No BootOrder present\r\n");
  }

  if (AddBootOrderEntry (BootDPOffset, Description, ImageDevPath, OptionalData, OptionDataSize, AddFlags) == EFI_SUCCESS)
    if ((AddFlags & ADD_FLAG_NO_BOOT_NEXT) == 0)
      Status = ActivateBootNext (BootDPOffset);
  
  if (BootOrderData)
    FreePool (BootOrderData);

  if (DevPathToFree)
    FreePool (DevPathToFree);

  return Status;
}

#if TEST_CODE
static CONST UINT8   DPath[] = 
{
  0x01, 0x03, 0x18, 0x00, 
  0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0x83, 0x00, 0x00, 0x00, 0x00, 0xBF, 0x23, 0x1C, 0x84, 0x00, 0x00, 0x00, 0x00,

  0x04, 0x06, 0x14, 0x00, 
  0xA5, 0x23, 0x45, 0x48, 0x18, 0x6A, 0x3F, 0x46, 0xEA, 0x1D, 0xDE, 0x3D, 0x74, 0x0A, 0xBA, 0xD5, 
  0x7F, 0xFF, 0x04, 0x00

};

/**
  Entry point for Test App

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                the buffers
**/

EFI_STATUS
EFIAPI
TestAppMain (IN EFI_HANDLE         ImageHandle,
             IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS                  Status = EFI_SUCCESS;

  Status = AddDevPathToBootOrder (ImageHandle, NULL,
                                  L"Test App Self", NULL, 0, 0);

  Status = AddDevPathToBootOrder (NULL, (EFI_DEVICE_PATH_PROTOCOL *)DPath,
                                  L"Cmd App Pre Defined", NULL, 0, 0);

  RemoveFromBootOrder (L"Test App");
  RemoveFromBootOrder (L"Cmd App");
  return Status;
}
#endif

/*
 *  Update the in RAM array that stores the BootOrder data. This function will take the BootOptionNumber
 *  that corresponds to a Boot#### option, and the BootOrderIndex that indicates what Index that option
 *  is to be placed in the BootOrderData, and proceed to place that option at that index. This function
 *  will not actually write to the BootOrder variable. That is the responsibility of the caller.
 *
 *  @param  BootOrderData       The BootOrder variable data
 *  @param  BootOrderDataSize   Size in bytes of the BootOrder array
 *  @param  BootOptionNumber    Boot#### option number to move
 *  @param  BootOrderIndex      Index into BootOrder array to move the
 *                              BootOptionNumber to.
 * 
 *  @retval  EFI_SUCCESS            Successfully updated the BootOrder
 *  @retval  EFI_INVALID_PARAMETER  BootOptionNumber not found in BootOrder
 *  @retval  EFI_INVALID_PARAMETER  Invalid BootOrderIndex or other parameters
 */
EFI_STATUS
ReorderBootOrderIndex(IN OUT UINT16* BootOrderData, 
                      IN     UINTN   BootOrderDataSize,
                      IN     UINT16  BootOptionNumber,
                      IN     UINTN   BootOrderIndex)
{
  UINTN i;
  INTN j;
  BOOLEAN IndexFound = FALSE;
  UINTN Index = 0;
  UINT32 BootOrderCount = BootOrderDataSize / sizeof(UINT16);
  
  /* Check parameters to make sure index is in bounds and size of array is greater than one */
  if ((BootOrderIndex >= BootOrderCount) || 
      (BootOrderData == NULL) || 
      (BootOrderDataSize <= sizeof(UINT16)))
  {
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; (i < BootOrderCount) && (!IndexFound); i++)
  {
    if (BootOrderData[i] == BootOptionNumber)
    {
      IndexFound = TRUE;
      Index = i;
      break;
    }
  }

  if (!IndexFound)
  {
    DEBUG ((DEBUG_WARN, "BootOptionNumber not found in BootOrder\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderIndex == Index)
  {
    DEBUG ((DEBUG_WARN, "BootOptionNumber already in correct position in BootOrder\n"));
    return EFI_SUCCESS;
  }

  if (BootOrderIndex > Index)
  {
    /* Final index is greater than current position - shift array left */
    /* Copy from i+1 to BootOrderIndex into positions starting at i */
    for (j = Index+1; (UINTN)j <= BootOrderIndex; j++)
    {
      /* Copy one at a time */
      BootOrderData[j-1] = BootOrderData[j];
    }
    BootOrderData[j-1] = BootOptionNumber;
  }
  else
  {
    /* Final index is less than current position - shift array right */
    /* Copy from i-1 down to BootOrderIndex into positions starting at i */
    for (j = Index-1; (((UINTN)j >= BootOrderIndex) && (j >= 0)); j--)
    {
      /* Copy one at a time */
      BootOrderData[j+1] = BootOrderData[j];
    }
    BootOrderData[j+1] = BootOptionNumber;
  }

  return EFI_SUCCESS;
}

