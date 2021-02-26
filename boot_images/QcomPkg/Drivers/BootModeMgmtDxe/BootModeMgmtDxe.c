/** @file BootModeMgmtDxe.c

  Implements the Boot mode management protocol
  
  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.

**/
/*============================================================================
  FILE:         BootModeMgmtDxe.c

  OVERVIEW:     Implementation of the Boot mode management protocol interface

  DEPENDENCIES: None
 
                Copyright (c) 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary,
============================================================================*/
/*==============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     ----------------------------------------------------------
 11/25/14     lm     Initial revision

==============================================================================*/

/*==============================================================================

                       INCLUDE FILES FOR THIS MODULE

==============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIBootModeMgmt.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseMemoryLib.h>


/*==============================================================================

                      GLOBAL DEFINES

==============================================================================*/

#define BOOT_MODE_SIGNATURE            ('B' | ('M' << 8) | ('o' << 16) | ('d' << 24))
#define BOOT_MODE_CURRENT_VERSION      0x00010001
#define BOOT_MODE_MINOR_VERSION_MASK   0x0000FFFF
#define BOOT_MODE_MAJOR_VERSION_MASK   0xFFFF0000
#define MAX_PROFILE_NAME_ELEMENTS 	   64

typedef struct {
  UINT32 Signature;                					// BOOT_MODE_SIGNATURE
  UINT32 Version;                  					// BOOT_MODE_CURRENT_VERSION
  UINT32 BootMode; 				   					// Boot mode - 0: default, 1: Manufacturing mode
  UINT32 ProfileNameElements;						// Number of elements in the ProfileName array (this is not bytes)
  CHAR16 ProfileName[MAX_PROFILE_NAME_ELEMENTS]; 	// String that holds the ProfileName
} EFI_OS_BOOT_MODE_INFO_TYPE;

/*==============================================================================

                      PROTOTYPES USED IN THIS MODULE

==============================================================================*/
EFI_STATUS
EFI_Get_Boot_Mode_Info(
  IN EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  OUT    UINT32                  *BootMode,
  IN OUT UINT32                  *ProfileNameElements OPTIONAL, 
  OUT    CHAR16                  *ProfileName OPTIONAL
  );

EFI_STATUS
EFI_Set_Boot_Mode_Info(
  IN  EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  IN UINT32                       *BootMode,
  IN UINT32                       *ProfileNameElements OPTIONAL,
  IN CHAR16                       *ProfileName OPTIONAL
  );


/*==============================================================================

                     GLOBAL VARIABLES FOR THIS MODULE

==============================================================================*/

static EFI_BOOT_MODE_MGMT_PROTOCOL BootModeMgmtProtocol = {
   EFI_BOOT_MODE_MGMT_PROTOCOL_REVISION,
   EFI_Set_Boot_Mode_Info,
   EFI_Get_Boot_Mode_Info   
};


static EFI_OS_BOOT_MODE_INFO_TYPE        *BootModePartition = NULL;
static EFI_BLOCK_IO_PROTOCOL             *BlkIo = NULL;  
static EFI_GUID BootModeInfoPartitionType = 
{0xe91ab96, 0x8339, 0x4ae4, {0xa3, 0x10, 0xed, 0x94, 0x35, 0x7f, 0xd7, 0xd0} };

/*==============================================================================

                             Private Functions

==============================================================================*/
EFI_STATUS InitBootModePartitionInfo( VOID )
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  HandleInfo              HandleInfoList[2];
  UINT32                  MaxHandles;
  PartiSelectFilter       HandleFilter;  

  /* Select the BlkIo handle that represents the partition by the
     referenced GUID type in GPT partition on Non removable media
     ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = &BootModeInfoPartitionType;
  HandleFilter.VolumeName = 0;
  
  /* If BootModeSelect partition is not found, return error */
  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);  
  if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
  {
    Status = EFI_NOT_FOUND;
	goto InitBootModePartitionInfoExit;
  }
  
  /* If more than 1 Boot Mode partitions are found, return error */  
  if (MaxHandles > 1)
  {
    Status = EFI_SECURITY_VIOLATION;
	goto InitBootModePartitionInfoExit;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;  

InitBootModePartitionInfoExit:
  return Status;  

}

EFI_STATUS ReadBootModePartition( VOID )
{
  UINT32                  BlkSize = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT8                   *SectorBuff = NULL; 
  
   /* Check the input paramenters */
   if ((BootModePartition == NULL) || (BlkIo == NULL)){
     Status = EFI_INVALID_PARAMETER;
	 goto ReadBootModePartitionExit;   
   }  
   
  BlkSize = BlkIo->Media->BlockSize;
  
  SectorBuff = AllocateZeroPool(BlkSize);
  if (SectorBuff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
	goto ReadBootModePartitionExit;	
  }

  /* Read the partition */
  Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, BlkSize, SectorBuff);
  if (Status != EFI_SUCCESS) {
   	goto ReadBootModePartitionExit;	
  }
  CopyMemS (BootModePartition, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE), SectorBuff, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE));
	
ReadBootModePartitionExit:
  if (SectorBuff != NULL)
    FreePool (SectorBuff);
	
  return Status;
}

EFI_STATUS UpdateBootModePartition
( 
   EFI_OS_BOOT_MODE_INFO_TYPE *BootModePtr
)
{
  UINT32                  BlkSize = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT8                   *SectorBuff = NULL; 
  
  /* Check the input paramenters */
  if ((BootModePtr == NULL) || (BlkIo == NULL)){
    Status = EFI_INVALID_PARAMETER;
    goto UpdateBootModeSelectPartitionExit;   
  }  
   
  BlkSize = BlkIo->Media->BlockSize;
  
  SectorBuff = AllocateZeroPool(BlkSize);
  if (SectorBuff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
	goto UpdateBootModeSelectPartitionExit;	
  }

  /* Write to the partition */
  CopyMemS (SectorBuff, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE), BootModePtr, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE));
  Status = BlkIo->WriteBlocks (BlkIo, BlkIo->Media->MediaId, 0, BlkSize, SectorBuff);
  
  
  if (Status != EFI_SUCCESS) {
   	goto UpdateBootModeSelectPartitionExit;	
  }	
  
UpdateBootModeSelectPartitionExit:
  if (SectorBuff != NULL)
    FreePool (SectorBuff);
	
  return Status;
}


VOID HandleVersionMigration( VOID )
{
   /* Check the input paramenters */
   if (BootModePartition == NULL){
	 return;
   }  
   
   /* If Signature is not valid, return as the partition is not
      programmed */
   if (BootModePartition->Signature != BOOT_MODE_SIGNATURE){
	 return;
   }	
	
   /* If current major version is greater than the version on the disk */
   if ((BootModePartition->Version & BOOT_MODE_MAJOR_VERSION_MASK) < 
       (BOOT_MODE_CURRENT_VERSION & BOOT_MODE_MAJOR_VERSION_MASK))
   {
      /* Update sequentially from lower version to the current version */
	  /* As no lower version is currently supported, we do not update the version
         so that the Get API reports an error. Set API needs to be used to 
		 re-initialize the partition */
   }	  
   /* If current major version is lesser than the version on the disk */   
   else if ((BootModePartition->Version & BOOT_MODE_MAJOR_VERSION_MASK) > 
       (BOOT_MODE_CURRENT_VERSION & BOOT_MODE_MAJOR_VERSION_MASK))
   {
      /* Cannot be handled as software is unaware of the newer version */
	  /* Get API will return an error. Set API needs to be used to
	     re-initialize the partition */
   }	
   /* If current major version is equal to the version on the disk, check
      the minor version and update, if required */      
   else
   {
      /* Update version based on minor version */
      if ((BootModePartition->Version & BOOT_MODE_MINOR_VERSION_MASK) < 
          (BOOT_MODE_CURRENT_VERSION & BOOT_MODE_MINOR_VERSION_MASK))
	  {
         /* Update sequentially from lower version to the current version */
	     /* As no lower version is currently supported, just update it
		    to higher version */	  
	     BootModePartition->Version = BOOT_MODE_CURRENT_VERSION;
	  }
      /* If higher minor version, keep it as is as software is unaware of 
         the newer version */
   }
   
   return;
}

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/

/** 
  @par Summary 
  This function uses the cached copy of the data. Only the fields that are not NULL
  are updated with the information in the cached copy and passed back to the client.
*/
 
EFI_STATUS
EFI_Get_Boot_Mode_Info(
  IN EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  OUT    UINT32                  *BootMode,
  IN OUT UINT32                  *ProfileNameElements OPTIONAL, 
  OUT    CHAR16                  *ProfileName OPTIONAL
  )
{
   EFI_STATUS Status;

   /* Check the input parameters */
   if ((This == NULL) || (BootModePartition == NULL) || (BootMode == NULL)) {
     Status = EFI_NOT_FOUND;
	 goto EFI_Get_Boot_Mode_Info_Exit;   
   }   
   
   /* Use the cached copy of BootModePartition */
   /* If Signature is not valid, return error */
   if (BootModePartition->Signature != BOOT_MODE_SIGNATURE){
      Status = EFI_VOLUME_CORRUPTED;
	  goto EFI_Get_Boot_Mode_Info_Exit;
   }	     
   
   /* If version is not current, then this needs to be marked as an error. 
      All other version updates are handled during initialization */
   if ((BootModePartition->Version & BOOT_MODE_MAJOR_VERSION_MASK) != 
       (BOOT_MODE_CURRENT_VERSION & BOOT_MODE_MAJOR_VERSION_MASK)){
      Status = EFI_VOLUME_CORRUPTED;
	  goto EFI_Get_Boot_Mode_Info_Exit;
   }	   
   
   /* Update the boot mode */
   *BootMode = BootModePartition->BootMode;
      
   if ((ProfileNameElements != NULL) && (ProfileName != NULL)) 
   {
		/* If the emmc elements are greater in size as compared to the ones sent by the user, 
		   then we return EFI_BUFFER_TOO_SMALL */
   		if (BootModePartition->ProfileNameElements > (*ProfileNameElements)) {
			Status = EFI_BUFFER_TOO_SMALL;
			*ProfileNameElements = BootModePartition->ProfileNameElements;
			goto EFI_Get_Boot_Mode_Info_Exit;
		}
		else
		{
			/* Update the size of elemenets that the user has sent to the size from the emmc */
			CopyMemS (ProfileName, (BootModePartition->ProfileNameElements * sizeof(CHAR16)), BootModePartition->ProfileName, (BootModePartition->ProfileNameElements * sizeof(CHAR16)));
			*ProfileNameElements = BootModePartition->ProfileNameElements;
		}
	
   }
      
   Status = EFI_SUCCESS;
   
EFI_Get_Boot_Mode_Info_Exit:
   return Status; 
}

/**
  @par Summary 
  This function updates the fields in the partition. If the partition is uninitialized,
  or the version is a mis-match, it initializes the partition and updates the variables 
  to the ones that are passed to the function. Once the partition is successfully updated, 
  it updates the cached copy.
*/

EFI_STATUS
EFI_Set_Boot_Mode_Info(
  IN  EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  IN UINT32                       *BootMode,
  IN UINT32                       *ProfileNameElements OPTIONAL,
  IN CHAR16                       *ProfileName OPTIONAL
  )
{
   EFI_STATUS Status;
   EFI_OS_BOOT_MODE_INFO_TYPE UpdatedBootModePartition;
   
   /* Check the input paramenters */
   if ((This == NULL) || (BootModePartition == NULL) || (BootMode == NULL)){
     Status = EFI_NOT_FOUND;
	 goto EFI_Set_Boot_Mode_Info_Exit;   
   }

   /* Update the local copy */
   CopyMemS (&UpdatedBootModePartition, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE), BootModePartition, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE));
   
   /* If Signature is not valid, initialize the partition */
   if (UpdatedBootModePartition.Signature != BOOT_MODE_SIGNATURE){
     UpdatedBootModePartition.Signature = BOOT_MODE_SIGNATURE;
     UpdatedBootModePartition.Version   = BOOT_MODE_CURRENT_VERSION;    
	 UpdatedBootModePartition.BootMode = 0x0;
	 UpdatedBootModePartition.ProfileNameElements = 0x0;
   }	     
   
   /* If version is not current, initialize the partition. 
      All other version updates are handled during initialization */
   if ((UpdatedBootModePartition.Version & BOOT_MODE_MAJOR_VERSION_MASK) != 
       (BOOT_MODE_CURRENT_VERSION & BOOT_MODE_MAJOR_VERSION_MASK)){
     UpdatedBootModePartition.Signature = BOOT_MODE_SIGNATURE;
     UpdatedBootModePartition.Version   = BOOT_MODE_CURRENT_VERSION;    
	 UpdatedBootModePartition.ProfileNameElements = 0x0;
   }
    
   /* Boot mode needs to be set */
   UpdatedBootModePartition.BootMode = *BootMode;
      
   if ((ProfileNameElements != NULL) && (ProfileName != NULL)) 
   {
		/* If the elements sent by the user are greater than 64,then we return EFI_BAD_BUFFER_SIZE */
   		if (*ProfileNameElements > MAX_PROFILE_NAME_ELEMENTS) {
			Status = EFI_BAD_BUFFER_SIZE;
			goto EFI_Set_Boot_Mode_Info_Exit;
		}
		else
		{
			/* Update the size of elements that the user has sent to the size from the emmc */
			UpdatedBootModePartition.ProfileNameElements = *ProfileNameElements;
			/* Copy the profile name till the profile name elements sent by the user to the emmc */
			CopyMemS (UpdatedBootModePartition.ProfileName, ((*ProfileNameElements) * sizeof(CHAR16)), ProfileName, ((*ProfileNameElements) * sizeof(CHAR16)));
		}
	
   }
     
   /* Write to the boot select partition */
   Status = UpdateBootModePartition(&UpdatedBootModePartition);
   
   /* Update the cached copy, if the write was a Success */
   if (Status == EFI_SUCCESS) {
      CopyMemS (BootModePartition, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE), &UpdatedBootModePartition, sizeof(EFI_OS_BOOT_MODE_INFO_TYPE));
   } else {
      Status = EFI_DEVICE_ERROR;
   }
   
EFI_Set_Boot_Mode_Info_Exit:   
   return Status; 
}

/*==============================================================================

  FUNCTION      BootModeMgmtEntryPoint

  DESCRIPTION   EFI Entry point for Boot Mode management driver

==============================================================================*/ 

EFI_STATUS
EFIAPI
BootModeMgmtEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
   EFI_HANDLE Handle = NULL;
   EFI_STATUS Status;
   
   /* Initialize the Boot Select partition */
   Status = InitBootModePartitionInfo();
   if (Status != EFI_SUCCESS)
   {
	  goto BootModeMgmtExit;  
   }

   /* Initialize the BootModePartition pointer */   
   BootModePartition = AllocateZeroPool(sizeof(EFI_OS_BOOT_MODE_INFO_TYPE));
   if (BootModePartition == NULL) {
     Status = EFI_OUT_OF_RESOURCES;
	 goto BootModeMgmtExit;
   }

   /* Read the boot select partition and update the cached copy */
   Status = ReadBootModePartition();
   if (Status != EFI_SUCCESS) {
     goto BootModeMgmtExit;
   } 
   
   /* Handle version changes */   
   HandleVersionMigration();
  
   /* Install the protocol only if partition exists */
   Status = gBS->InstallMultipleProtocolInterfaces(&Handle, 
                                                   &gEfiBootModeMgmtProtocolGuid,
                                                   (void **)&BootModeMgmtProtocol, 
                                                   NULL, 
                                                   NULL, 
                                                   NULL);

BootModeMgmtExit: 
   return Status;
}

