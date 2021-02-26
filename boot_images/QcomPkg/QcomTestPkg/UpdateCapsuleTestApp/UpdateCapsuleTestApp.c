/** @file UpdateCapsuleTestApp.c
   
  Tests for capsules.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/08/12   mic         Fixed warnings
 12/11/12   mic        Fixed KW warnings
 07/03/12   aus        Replaced DEBUG with AsciiPrint
 11/29/11   jthompso   Initial version
=============================================================================*/

#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/EfiFileLib.h>
#include <Library/BaseMemoryLib.h>
#include "UpdateCapsuleTestApp.h"

#define DEFAULT_NUMBER_OF_ESRT_ENTRY 4
#define DEFAULT_NUMBER_OF_CAPSULES 4
#define DEFAULT_CAPSULE_DATA_ADDRESS 0x10000000
#define DEFAULT_CAPSULE_SIZE 128*1024

#define DEFAULT_CAPSULE_FLAGS CAPSULE_FLAGS_PERSIST_ACROSS_RESET|CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE
#define DEFAULT_MS_CAPSULE_FLAGS1 CAPSULE_FLAGS_PERSIST_ACROSS_RESET|CAPSULE_FLAGS_INITIATE_RESET
#define DEFAULT_MS_CAPSULE_FLAGS2 CAPSULE_FLAGS_PERSIST_ACROSS_RESET|CAPSULE_FLAGS_INITIATE_RESET|CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE



#define DEFAULT_CAPSULE_GUID \
    { \
        0x28D578E6, 0xA736, 0x4A8A, { 0x88, 0xA8, 0x94, 0xC4, 0x7E, 0x8F, 0x80, 0x47 } \
    } 

#define QCOM_FW_UPDATE_TEST_SYSTEM_FW_GUID \
   { \
        0x28D578E6, 0xA736, 0x4A8A, { 0x88, 0xA8, 0x94, 0xC4, 0x7E, 0x8F, 0x80, 0x47 } \
    } 

#define QCOM_FW_UPDATE_NULL_GUID \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }


#define QCOM_FW_SYSTEM_FW_TEST2_GUID \
  { \
    0xb31d4953, 0x342a, 0x44e6, {0x94, 0xad, 0xcb, 0x6d, 0x3d, 0xf8, 0x4d, 0xe0 } \
  }


#define QCOM_FW_DEVICE_FW_TEST1_GUID \
  { \
    0x585322cc, 0xb7a6, 0x4568, {0x97, 0x1a, 0x5a, 0xc2, 0x38, 0x81, 0x9c, 0x52 } \
  }


#define QCOM_FW_DEVICE_FW_TEST2_GUID \
  { \
    0x115795d3, 0x38db, 0x4ba9, {0xb5, 0x1c, 0xd0, 0x86, 0xba, 0xef, 0x9e, 0xa0 } \
  }




#define WRITE_PATTERN 0x1
#define READ_PATTERN 0x2
#define GET_MODE_FROM_FILE 0x0

#define PAGECOUNT(Count,PageCount)\
  do { \
        PageCount = Count + (EFI_PAGE_SIZE - 1);\
        PageCount = PageCount & ~(EFI_PAGE_SIZE -1);\
     \
  } while (0) 

#define CONTINUE_IF_ERROR(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      continue; \
    } \
  } while (0)

#define EXIT_IF_ERROR(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      AsciiPrint(" failed, status: %r\r\n", Status); \
      return Status;\
    } \
  } while (0)



static EFI_PHYSICAL_ADDRESS mScatterGatherList = 0;
static UINT32 mCapsuleCount = 0;
static EFI_CAPSULE_HEADER *mCapsuleHeaderArray[MAX_CAPSULE_COUNT];

static CAPSULE_MAP *mCapsuleMap;

static EFI_FILE_INFO *FileInfo;
static VOID *PayloadBuffer;

//static ESRT_TABLE_HEADER *esrt;

static ESRT_ENTRY default_esrtArr[DEFAULT_NUMBER_OF_ESRT_ENTRY]= {
    {QCOM_FW_UPDATE_TEST_SYSTEM_FW_GUID,FW_TYPE_SYSTEM, 0x1,0x1,0,0x1,FW_LAST_ATMPT_STATUS_SUCCESS},
    {QCOM_FW_SYSTEM_FW_TEST2_GUID,   	FW_TYPE_SYSTEM, 0x1,0x1,0,0x1,FW_LAST_ATMPT_STATUS_SUCCESS},
    {QCOM_FW_DEVICE_FW_TEST1_GUID,   	FW_TYPE_DEVICE, 0x1,0x1,0,0x1,FW_LAST_ATMPT_STATUS_SUCCESS},
	{QCOM_FW_DEVICE_FW_TEST2_GUID,   	FW_TYPE_DEVICE, 0x1,0x1,0,0x1,FW_LAST_ATMPT_STATUS_SUCCESS}
};

 EFI_STATUS
FUAllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    );
EFI_STATUS
CreateCapsulesAndScatterGatherList(
    IN UINT32 NumCapsules
    )
{
    UINTN       CapsuleCount;
    EFI_CAPSULE_HEADER *CurrentCapsuleHeader = NULL;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *CurrentSGList = NULL;

    PAGED_CAPSULE *PagedCapsule;

    UINTN       SGListPageCounter;
    UINTN       SGListPageCount;
    

    ASSERT(mCapsuleMap != NULL);

    for(CapsuleCount = 0; CapsuleCount < NumCapsules; CapsuleCount++)
    {
        if (mCapsuleMap != NULL)
        {
          CurrentCapsuleHeader = &(mCapsuleMap->Capsules[mCapsuleCount].Header);

          if(CurrentCapsuleHeader != NULL)
          {
          
            CurrentCapsuleHeader->HeaderSize = sizeof(EFI_CAPSULE_HEADER);
          
            //CurrentCapsuleHeader->CapsuleImageSize = 128*1024 + sizeof(EFI_CAPSULE_HEADER);
            CurrentCapsuleHeader->CapsuleImageSize =  FileInfo->FileSize + sizeof(EFI_CAPSULE_HEADER);
          
            if(default_esrtArr[CapsuleCount].FWType ==FW_TYPE_SYSTEM )
              CurrentCapsuleHeader->Flags = DEFAULT_MS_CAPSULE_FLAGS1;
            else 
              CurrentCapsuleHeader->Flags = DEFAULT_MS_CAPSULE_FLAGS2;
           
            CopyMem(&(CurrentCapsuleHeader->CapsuleGuid),
                    (VOID*)&default_esrtArr[CapsuleCount].FWClass,
                    sizeof(EFI_GUID));
          
            CopyMem(&(mCapsuleMap->Capsules[mCapsuleCount].data),
                    PayloadBuffer,
                    FileInfo->FileSize);
          
            //Copy Header to our header array
            mCapsuleHeaderArray[mCapsuleCount] = CurrentCapsuleHeader;
          
            //Build ScatterGatherList for this capsule.
            CurrentSGList = &(mCapsuleMap->ScatterGatherLists[mCapsuleCount].List[0]);
                  
            //PAGECOUNT(CurrentCapsuleHeader->CapsuleImageSize,SGListPageCount);
                  
            PagedCapsule = (PAGED_CAPSULE*) &(mCapsuleMap->Capsules[mCapsuleCount]);            
          
            SGListPageCount = EFI_SIZE_TO_PAGES(CurrentCapsuleHeader->CapsuleImageSize);
                                      
                                              
            for(SGListPageCounter = 0; 
                SGListPageCounter < SGListPageCount; 
                SGListPageCounter++)
            {
                CurrentSGList[SGListPageCounter].Length = EFI_PAGE_SIZE;
       
                if(SGListPageCounter == SGListPageCount-1 &&
                   CurrentCapsuleHeader->CapsuleImageSize % EFI_PAGE_SIZE != 0)
                {
                   CurrentSGList[SGListPageCounter].Length = 
                        (UINT64) (CurrentCapsuleHeader->CapsuleImageSize) % EFI_PAGE_SIZE;
                }
                     
                CurrentSGList[SGListPageCounter].Union.DataBlock = (UINT64)&(PagedCapsule->Pages[SGListPageCounter]);
            }
          
            CurrentSGList[SGListPageCounter].Length = 0;
            //Add continuation pointer
            if(CapsuleCount< (NumCapsules - 1))
            {
               CurrentSGList[SGListPageCounter].Union.DataBlock = 
                 (UINT64)&(mCapsuleMap->ScatterGatherLists[mCapsuleCount+1].List);
            }
            else //Add terminator entry
            {
               CurrentSGList[SGListPageCounter].Union.DataBlock = (UINT64)0x0;                
            }
          }
        }
        mCapsuleCount++;        
    }

    return EFI_SUCCESS;

}

EFI_STATUS
RunCapsuleTest()
{

    EFI_STATUS Status;
    UINT64 MaximumCapsuleSize;
    EFI_RESET_TYPE ResetType;

    

    mCapsuleMap = (CAPSULE_MAP*)DEFAULT_CAPSULE_DATA_ADDRESS;


    //Create capsules and ScatterGatherList
    Status = CreateCapsulesAndScatterGatherList(DEFAULT_NUMBER_OF_CAPSULES);

    EXIT_IF_ERROR(Status);

    mScatterGatherList = (EFI_PHYSICAL_ADDRESS) (UINT64*)&(mCapsuleMap->ScatterGatherLists[0].List[0]);

    Status = gRT->QueryCapsuleCapabilities((EFI_CAPSULE_HEADER**)&mCapsuleHeaderArray,
                                           DEFAULT_NUMBER_OF_CAPSULES,
                                           &MaximumCapsuleSize,
                                           &ResetType);
    EXIT_IF_ERROR(Status);
    AsciiPrint("QueryCapsuleCapabilities, MaximumCapsuleSize: 0x%llx, ResetType %d\n",
               MaximumCapsuleSize,
               (UINT32)ResetType);

    Status = gRT->UpdateCapsule((EFI_CAPSULE_HEADER**)&mCapsuleHeaderArray,
                                DEFAULT_NUMBER_OF_CAPSULES,
                                mScatterGatherList);
    EXIT_IF_ERROR(Status);


    //Create Additional Capsule and separate ScatterGatherList
    Status = CreateCapsulesAndScatterGatherList(1);

    EXIT_IF_ERROR(Status);

    Status = gRT->QueryCapsuleCapabilities((EFI_CAPSULE_HEADER**)&(mCapsuleHeaderArray[mCapsuleCount -1]),
                                           1,
                                           &MaximumCapsuleSize,
                                           &ResetType);
    EXIT_IF_ERROR(Status);
    AsciiPrint("QueryCapsuleCapabilities, MaximumCapsuleSize: 0x%llx, ResetType %d\n",
               MaximumCapsuleSize,
               (UINT32)ResetType);

    Status = gRT->UpdateCapsule((EFI_CAPSULE_HEADER**)&(mCapsuleHeaderArray[mCapsuleCount -1]),
                                1,
                                (UINT64)&(mCapsuleMap->ScatterGatherLists[mCapsuleCount-1].List[0]));
    EXIT_IF_ERROR(Status);


    return Status;

}

EFI_STATUS
FUFatGetFileInfo(
    IN EFI_FILE_PROTOCOL    *File
    )
{
    EFI_STATUS Status;
    UINTN       FileInfoSize = 0;

    Status = File->GetInfo(File,
                           &gEfiFileInfoGuid,
                           &FileInfoSize,
                           (VOID*)FileInfo);

    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        

        Status = gBS->AllocatePool(EfiBootServicesData,
                                   FileInfoSize,
                                   (VOID**)&FileInfo);
        //ERR_HANDLE(Status);
 

        Status = File->GetInfo(File,
                               &gEfiFileInfoGuid,
                               &FileInfoSize,
                               (VOID*)FileInfo);
        
    }
    

    return Status;
}
EFI_STATUS LoadPayload()
{
  EFI_STATUS Status = EFI_NOT_FOUND;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
  EFI_FILE_PROTOCOL *FsVolume = NULL;
  EFI_FILE_PROTOCOL *FilePtr = NULL;
  EFI_HANDLE *HandleBuffer = NULL;
  UINTN nHandles,i;
  UINTN                PayloadFileSize;
 
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid,
                                  NULL, &nHandles, &HandleBuffer);
  if(Status != EFI_SUCCESS) 
  {
      AsciiPrint("ERROR: Failed to get handle buffer for simple FS protocol.\n");
      return Status;
  }

  for (i = 0; i < nHandles; i++) {
    //loop through all handles, open volume and check if file is there
    Status = gBS->HandleProtocol(HandleBuffer[i],&gEfiSimpleFileSystemProtocolGuid,(VOID **)&Fs);
//    AsciiPrint(" handle cnt %d, total: %d\n", i,nHandles);

    if(Status != EFI_SUCCESS) 
    {
      AsciiPrint("ERROR: Failed to get FS handles for simple FS protocol.\n");
      continue;
    }

	Status = Fs->OpenVolume (Fs, &FsVolume);

    if (Status != EFI_SUCCESS) 
    {
      AsciiPrint("ERROR: Failed to open FS volume\n");
      continue;
    }

    //Check if file is present
    Status = FsVolume->Open(FsVolume, &FilePtr, L"QPayload.efi", EFI_FILE_MODE_READ, 0);

    if ((Status != EFI_SUCCESS) || !FilePtr) 
    {
      AsciiPrint("QPayload.efi not found\n");

	  if (i ==nHandles -1)
      	AsciiPrint("QPayload.efi not found");
      continue;
    }
    
    AsciiPrint("QPayload.efi found\n");

    Status = FUFatGetFileInfo(FilePtr);

    if(Status != EFI_SUCCESS) 
    {
    
      AsciiPrint("ERROR: Failed to get file info.\n");
      return Status;
    }

    PayloadFileSize =FileInfo->FileSize;
	
    
    AsciiPrint(" Payload size %x bytes\n",  PayloadFileSize);   

     Status = FUAllocateAndZero(EfiBootServicesData,PayloadFileSize,&PayloadBuffer);
    if(Status != EFI_SUCCESS) 
    {
    
      AsciiPrint("ERROR: Failed to allocate buffer for QPayload.efi.\n");
      return Status;
    }
     Status = FilePtr->Read(FilePtr,&PayloadFileSize, PayloadBuffer);
    if(Status != EFI_SUCCESS) 
    {
    
      AsciiPrint("ERROR: Failed to Read.\n");
      return Status;
    }  
   
      Status = FsVolume->Close(FilePtr);
      Status = FsVolume->Close(FsVolume);
      gBS->FreePool(HandleBuffer);
      return Status;
    }
  

  //Looped through all handles and no file found
  gBS->FreePool(HandleBuffer);
  return EFI_NOT_FOUND;

}
        
/**
 * Wrapper to gBS->Allocatepool. 
 *  
 * Clears buffer returned by 
 * gBS->Allocatepool. 
 * 
 * 
 * @param PoolType - PoolType for AllocatPool 
 * @param Size -     Size of buffer
 * @param Buffer -   Buffer 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FUAllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    )
{
    EFI_STATUS Status;
    Status = gBS->AllocatePool(PoolType,Size,Buffer);
//	Status = gBS->AllocatePool(PoolType,Size,Buffer);

	if(Status != EFI_SUCCESS)
    {
  		AsciiPrint("ERROR: Failed to allocate buffer.\n");
        return Status;
    }
    
    SetMem( *Buffer, Size, 0 );

    return EFI_SUCCESS;
}

/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
UefiAppMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{ 
  LoadPayload();

  RunCapsuleTest(); 

  return 0;
}
