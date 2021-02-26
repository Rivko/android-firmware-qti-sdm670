/** @file Pgm.c
   
  Application to programm binaries from sdcard to emmc.
  
  Copyright (c) 2012-2014, 2016, Qualcomm Technologies Inc. All rights reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  07/05/16     bh       Update file system call after API change
  06/17/2016   bh       Get block size from blk io protocol instead of hard code
  10/16/2014   vk       LoadFileFromPartition signature update
  06/05/2014   vk       update for 64 bit
  01/09/2013   vk       Fix KlockWork warnings
  09/17/2012   yg       Use ReadAnyKey API
  07/23/2012   yg       Support GUID's in standard form
  07/17/2012   yg       Enhanced to program any image configured via cfg file
  05/17/2012   vishalo  Initial version
 
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiFileLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ParserLib.h>

#define DEFAULT_ALIGN_BOUNDARY   (16 * 1024)

#define FW_FLASH_CONFIG_FILE L"\\PGM\\FW_FLASH.CFG"

STATIC UINTN AlignBoundary;

STATIC EFI_STATUS 
FindFileSize(UINTN *ReadFileSize, EFI_FILE_PROTOCOL *File);

EFI_STATUS 
LoadFSFile(IN CHAR16 *FileName, OUT UINT8 **FileBuffer, OUT UINTN *FileSize);

EFI_STATUS 
ProgPartition(IN EFI_GUID *PartitionTypeGuid, IN UINT8 **FileBuffer, IN UINTN *FileSize);

extern EFI_GUID gEfiSdRemovableGuid;
extern EFI_GUID gEfiUefiPartitionTypeGuid;
UINTN CfgVersion = 0;

STATIC EFI_STATUS 
FindFileSize(OUT UINTN *FileSize, IN EFI_FILE_PROTOCOL *File)
{
  EFI_STATUS     Status;
  EFI_FILE_INFO  *Info    = NULL;
  UINTN          InfoSize = 0;

  if ((FileSize == NULL) || (File == NULL)) 
    return EFI_INVALID_PARAMETER;

  Info = AllocatePool (InfoSize);

  if(Info == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  else
  {
    Status = File->GetInfo (File, &gEfiFileInfoGuid, &InfoSize, Info);

    if(Status == EFI_BUFFER_TOO_SMALL)
    {
      if (Info != NULL)
        FreePool (Info);

      Info = AllocatePool (InfoSize);
      Status = File->GetInfo(File, &gEfiFileInfoGuid, &InfoSize, Info);
    }

    if ((!EFI_ERROR(Status)) && (Info != NULL))
    {
      *FileSize = Info->FileSize;
    }

    if(Info != NULL)
      FreePool (Info);

    return Status;
  }
}

/**
  Program buffer to UEFI partition

  @param[IN]  FileSize          Length of data in FileBuffer
  @param[OUT] FileBuffer        Buffer to be programmed
  @param[IN]  FileSize          Length of data in FileBuffer
  
  @retval EFI_SUCCESS           Partition write suceeded
  @retval EFI_NOT_FOUND         Unable to find partition
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_UNSUPPORTED       File is larger than partition 
                                size
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                buffers
**/

EFI_STATUS 
ProgramPartition(IN EFI_GUID *PartitionTypeGuid, IN UINT8 *DataBuffer, IN UINTN DataSize)
{
  EFI_STATUS               Status;
  UINT32                   MaxHandles;
  STATIC HandleInfo        HandleInfoList[2];
  STATIC PartiSelectFilter HandleFilter;
  UINTN                    Attrib = 0;
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;
  UINTN                    PartitionSize = 0;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  
  if ((DataBuffer == NULL) || (DataSize <= 0))
    return EFI_INVALID_PARAMETER;
	  
  Attrib = BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID | 
           BLK_IO_SEL_PARTITIONED_GPT | 
           BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  
  HandleFilter.PartitionType = PartitionTypeGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attrib, &HandleFilter, HandleInfoList, &MaxHandles);
  if (MaxHandles != 1)
  {
    AsciiPrint ("Partition not found...\r\n");
    return EFI_NOT_FOUND;
  }
  
  BlkIo = HandleInfoList[0].BlkIo;
  
  PartitionSize = (UINTN)(BlkIo->Media->LastBlock + 1) * (UINTN) BlkIo->Media->BlockSize;
  
  if (DataSize > PartitionSize){
    AsciiPrint("File is larger (%d) than partition (%d)\r\n", DataSize, PartitionSize);
    return EFI_UNSUPPORTED;
  }

  Status = BlkIo->WriteBlocks (BlkIo, BlkIo->Media->MediaId, 0, 
                                 DataSize, DataBuffer);
  if (EFI_ERROR (Status))
  {
    AsciiPrint("Flashing Failed...\r\n");
    return EFI_DEVICE_ERROR;
  }

  AsciiPrint("Flash OK\r\n");
  return Status;
}

/**
  Load file to program from SDcard to Buffer

  @param[in]  Filename         Filename to look for
  @param[out] FileBuffer       A pointer to buffer containing 
                               file must be freed by caller
  @param[out] FileSize         Length of data in FileBuffer
  
  @retval EFI_SUCCESS          Erase successfully.
  @retval EFI_NOT_FOUND        Failed to find specified file
  @retval EFI_OUT_OF_RESOURCES Failed to allocate memory for 
                               buffers
**/

EFI_STATUS 
LoadFSFile(IN CHAR16 *FileName, OUT UINT8 **FileBuffer, OUT UINTN *FileSize)
{
  EFI_STATUS                       Status = EFI_NOT_FOUND;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs;
  EFI_FILE_PROTOCOL                *FsVolume = NULL;
  EFI_FILE_PROTOCOL                *FilePtr = NULL;
  EFI_HANDLE                       *HandleBuffer = NULL;
  UINTN                            nHandles,i;

  if ((FileName == NULL) || (FileSize == NULL) || (FileBuffer == NULL))
    return EFI_INVALID_PARAMETER;  

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid,
                                   NULL, &nHandles, &HandleBuffer);
  if(Status != EFI_SUCCESS) 
  {
    DEBUG((EFI_D_ERROR,"Failed to get handle buffer for simple FS protocol.\n"));
    return Status;
  }

  for (i = 0; i < nHandles; i++)
  {
    //loop through all handles, open volume and check if file is there
    Status = gBS->HandleProtocol(HandleBuffer[i],&gEfiSimpleFileSystemProtocolGuid,(VOID **)&Fs);

    if (Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_ERROR,"Failed to get FS handles for simple FS protocol.\n"));
      continue;
    }

    Status = Fs->OpenVolume (Fs, &FsVolume);

    if (Status != EFI_SUCCESS) 
    {
      DEBUG ((EFI_D_ERROR, "Failed to open FS volume\n"));
      continue;
    }

    //Check if file is present
    Status = FsVolume->Open (FsVolume, &FilePtr, FileName, (EFI_FILE_MODE_READ), 0);

    if ((Status != EFI_SUCCESS) || !FilePtr)
    {
      DEBUG ((EFI_D_INFO, "ERRROR: file not found\n"));
      FsVolume->Close (FsVolume);
      Status = EFI_NOT_FOUND;
      continue;
    }
    else
    {
      UINTN  AlignedFileSize;

      DEBUG ((EFI_D_INFO, "ERROR: file found\n"));
      Status = FindFileSize(FileSize, FilePtr);

      if(EFI_ERROR(Status) || (*FileSize <= 0)) {
        DEBUG ((EFI_D_ERROR, "ERROR: file size is zero\n\r"));
        FsVolume->Close(FsVolume);
        continue;
      }
      AlignedFileSize = *FileSize;

      /* Extend size to align boundary */
      AlignedFileSize += (AlignBoundary - 1);
      AlignedFileSize &= (~(AlignBoundary - 1));

      *FileBuffer = AllocateZeroPool (AlignedFileSize);
      if(FileBuffer == NULL)
      {
        FsVolume->Close(FsVolume);
        continue;
      }
       
      Status = FilePtr->Read(FilePtr, FileSize, (VOID*) *FileBuffer);
      if(EFI_ERROR(Status))
      {
        DEBUG ((EFI_D_ERROR, "ERROR: Unable to read file\n\r"));
        FreePool (FileBuffer);
        gBS->FreePool (HandleBuffer);
        continue;
      }
      *FileSize = AlignedFileSize;

      Status = FsVolume->Close (FsVolume);
      gBS->FreePool (HandleBuffer);
      return EFI_SUCCESS;
    }
  }

  //Looped through all handles and no file found
  gBS->FreePool (HandleBuffer);
  return EFI_NOT_FOUND;
}

#define MAX_OPTIONS_SUPPORTED     2
/* Programming options */
STATIC VOID
ProcessOptions (UINT8* Key, UINT8* Value)
{
  UINT32 BinVal;

  if (AsciiStriCmp ((CHAR8*)Key, "BlockAlign") == 0)
  {
    if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), &BinVal, sizeof(BinVal)))
    {
      if (((BinVal & (BinVal - 1)) == 0) && (BinVal >= 512))
        AlignBoundary = BinVal;
    }
  }
  else
  /*  For CfgVersion 1, GUID can be given as actual standard GUID. */
  if (AsciiStriCmp ((CHAR8*)Key, "CfgVersion") == 0)
  {
    BinVal = 0;
    if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), &BinVal, sizeof(BinVal)))
      CfgVersion = BinVal;
  }
}

#define SWAP_BYTES(x,y)  do {           \
                           UINT8 t;     \
                           t = (x);     \
                           (x) = (y);   \
                           (y) = t;     \
                         }while(0)

/* Convert Byte Stream data to GUID format. The parser would have returned the
 * GUID in byte stream format, the cfg will have in guid format, this routine
 * makes sure to put the data into correct format
 * For example:
 * if Cfg has data in below format:
 *    DEA0BA2C CBDD 4805 B4F9 F428251C3E98
 *
 * Parser will return data as follows in stream format
 *    DE A0 BA 2C CB DD 48 05 B4 F9 F4 28 25 1C 3E 98
 *
 * Need to be converted into GUID format (Considering integer and endianness)
 *    DEA0BA2C CBDD 4805 B4F9 F428251C3E98 is equal to stream as follows
 *    2C BA A0 DE DD CB 05 48 B4 F9 F4 28 25 1C 3E 98
 * */
void AdjustGUID (UINT8* Buffer)
{
  /* DWord */
  SWAP_BYTES(Buffer[0], Buffer[3]);
  SWAP_BYTES(Buffer[1], Buffer[2]);

  /* Word */
  SWAP_BYTES(Buffer[4], Buffer[5]);

  /* Word */
  SWAP_BYTES(Buffer[6], Buffer[7]);
}

STATIC UINTN gPartiSelAttrib;
STATIC GUID  gPartitionTypeGUID, *SelectedPartitionTypeGuid;
//STATIC GUID  gRootDeviceGUID, *SelectedRootDeviceGuid;
STATIC UINT8 gPartitionName[64];
STATIC UINT16 gImageFileName[128];

STATIC VOID
CfgKeyValCb (UINT8* Section, UINT8* Key, UINT8* Value)
{
  UINTN Sz;
  STATIC UINTN OptionCbCnt = 0;

  /* options should be first ones, increase the number to as many options we support */
  if (OptionCbCnt < MAX_OPTIONS_SUPPORTED)
  {
    ++OptionCbCnt;
    ProcessOptions (Key, Value);
  }

  if (AsciiStriCmp ((CHAR8*)Key, "PartitionTypeGUID") == 0)
  {
    gPartiSelAttrib = BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE |
                      BLK_IO_SEL_PARTITIONED_GPT;
    Sz = HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), 
                      (UINT32*)&gPartitionTypeGUID, sizeof(gPartitionTypeGUID));

    /* Do the adjustment of GUID if the version is 1 onwards */
    if (CfgVersion > 0)
      AdjustGUID ((UINT8*)&gPartitionTypeGUID);

    if (Sz == sizeof(gPartitionTypeGUID))
    {
      gPartiSelAttrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;
      SelectedPartitionTypeGuid = &gPartitionTypeGUID;
    }
    else
    {
      gPartiSelAttrib &= (~BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID);
      SelectedPartitionTypeGuid = NULL;
    }
  }

  if (AsciiStriCmp ((CHAR8*)Key, "PartitionName") == 0)
    AsciiStrnCpy ((CHAR8*)gPartitionName, (CHAR8*)Value, sizeof(gPartitionName));

  if (AsciiStriCmp ((CHAR8*)Key, "ImageFilePath") == 0)
  {
    if (AsciiStrLen((CHAR8*)Value) < (sizeof(gImageFileName) / sizeof(*gImageFileName)))
      AsciiStrToUnicodeStr ((CHAR8*)Value, gImageFileName);
    else
      AsciiPrint ("Too large file name %a\r\n", Value);
  }

  if (AsciiStriCmp ((CHAR8*)Key, "Program") == 0)
  {
    if (AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0)
    {
      UINT8 *DataBuffPtr = NULL;
      UINTN  DataSize = 0;

      AsciiPrint ("\r\nFlash: %s => \"%a\"\r\n", gImageFileName, gPartitionName);
      if (LoadFSFile (gImageFileName, &DataBuffPtr, &DataSize) == EFI_SUCCESS)
      {
        ProgramPartition (SelectedPartitionTypeGuid, DataBuffPtr, DataSize);
        FreePool (DataBuffPtr);
      }
      else
        AsciiPrint ("Loading file failed.\r\n");
    }
  }
}


/**
  Entry point for FW Flashing app

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                the buffers
**/

EFI_STATUS
EFIAPI
PgmMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status;
  UINT8      *CfgBuffer = NULL;
  UINTN      FileSize = 0;
  INTN       Pd;
  UINT32      BlkIOAttrib = 0;

  gST->ConOut->ClearScreen (gST->ConOut);

  AsciiPrint(" FW Images Flash Utility:\r\n"
             " ------------------------\r\n");

  BlkIOAttrib = BLK_IO_SEL_PARTITIONED_MBR; 
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;
  
  Status = LoadFileFromPartition(BlkIOAttrib,
                                 FW_FLASH_CONFIG_FILE,
                                 (EFI_GUID*)&BlkIOAttrib,
                                 (EFI_GUID*)&BlkIOAttrib,
                                 NULL,
                                 &CfgBuffer, &FileSize);
  
//  Status = LoadFSFile(FW_FLASH_CONFIG_FILE, &CfgBuffer, &FileSize);
  if (EFI_ERROR(Status) || (CfgBuffer == NULL) || (FileSize <= 0) ) {
    AsciiPrint("Error loading config file ...\r\n");
    goto ErrorExit;
  }

  AlignBoundary = DEFAULT_ALIGN_BOUNDARY;

  Pd = OpenParser (CfgBuffer, FileSize, NULL);

  if (Pd < 0)
  {
    AsciiPrint("Parser open failed\r\n");
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

	EnumKeyValues (Pd, (UINT8*)"FW Flash", CfgKeyValCb);

ErrorExit:

  AsciiPrint("Press any key to exit...\r\n");
  ReadAnyKey (NULL, 0);

  gST->ConOut->ClearScreen (gST->ConOut);

  if(CfgBuffer != NULL)
    FreePool(CfgBuffer);

  return Status;
}

