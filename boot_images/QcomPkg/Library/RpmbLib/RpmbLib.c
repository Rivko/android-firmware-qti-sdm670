/** @file RpmbLib.c
   
  Implements generic library functions for RPMB. 

  Copyright (c) 2011-2018, Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 2018-09-11   gp      check buffer limit before copying to AppName
 2017-11-16   jt      Skip partition config for LA
 2015-04-17   jt/rm   Add support for ufs rpmb
 2014-08-15   sm      Switched to new SCM API
 2014-07-23   jt      Cleanup RpmbLib
 2014-03-27   bn      Fixed 64-bit pointer size
 2014/03/17   bn      Removed __align() for UEFI 64-bit build
 2013/07/10   jt      Clean up partition config 
 2013/05/30   jt      Guid check in ConfigKeyValCb applies only for GPT partitions
 2013/02/22   shl     ScmDxe cleanup
 2013/02/19   jt      Added support to configure RPMB/GPT partitions 
 2013/02/13   bn      Fixed klockwork warnings
 2013/01/17   jt      (Temporary) Do key provisioning entirely in UEFI 
 2011/12/19   bn      Moved Rpmb Provision code to a separate API
 2011/11/12   bn      Initial revision.

=============================================================================*/

#include <string.h>
#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIRpmb.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_qsee_interface.h>
#include <Protocol/BlockIo.h>

#include <Library/ParserLib.h>
#include <Library/QcomLib.h>
#include "SDMgrMsgs.h"

#include <Library/RpmbCg.h>
#include <Library/MemoryAllocationLib.h>
extern EFI_GUID gEfiSdccRpmbProtocolGuid;

#define  BLOCK_SIZE     512
#define  KEY_INDEX      (BLOCK_SIZE - 316) 
#define  REQ_INDEX      (BLOCK_SIZE - 2)  
#define  KEY_FIELD_LEN  0x20   
#define  REQ_FIELD_LEN  0x02   

typedef enum
{
  RPMB_PROVISION_KEY_SECURE = 0,
  RPMB_PROVISION_KEY_TEST
} RpmbProvisionType;

#define OS_TYPE_LEN     4
#define MAX_PARTITIONS 15
#define PARTITION_CONFIG_FILE L"SecParti.cfg"
#define GPT_SECTION_LEN 37 /* 32 for GUID, 4 for "GPT.", 1 for NULL terminator */
#define DIGIT(x) ( (x > 0) && (x <= 9) )

/* Partition Data structure */
typedef struct 
{
   UINT8    AppName[32]; 
   UINT32   PartitionId; 
   UINT32   NumSectors; 
} SdMgrPartiAdditionData;

static BOOLEAN CallbackEntered = FALSE; 
static BOOLEAN GuidFound = FALSE; 
static CHAR8 *OsType = NULL; 
static EFI_BLOCK_IO_MEDIA *CurrentBlockIoMedia; 
static UINT32 CurrentConfigVersion; 
static UINT8 PartitionsToAdd = 0;
static SdMgrPartiAdditionData PartiAdditionData[MAX_PARTITIONS]; 
static SDMgrDeviceType CurrentDevId; 
static UINT8 CurrentPartitionGuid[16]; 
static UINT8 *CfgBuffer = NULL;
static UINTN FileSize = 0;

static UINT8 *RpmbSectionNames[4] =
{
   (UINT8 *)"RPMB.128KB", 
   (UINT8 *)"RPMB.256KB",
   (UINT8 *)"RPMB.512KB",
   (UINT8 *)"RPMB.1MB" 
};

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/**
  Function: ProvisionRpmbTestKey
 
  Description:
  This function sends a QSEE command to TZ in order to provision
  the RPMB with the test key.
                      
  @return EFI_SUCCESS               Initialization successful.
  @return EFI_INVALID_PARAMETER     Invalid parameter. 
**/
EFI_STATUS
EFIAPI
ProvisionRpmbTestKey (UINT32 *Result)
{
   EFI_STATUS Status; 
   QCOM_SCM_PROTOCOL *ScmProtocol = NULL; 
   qsee_provision_rpmb_key_req_t provision_key_req; 
   UINT8* ScmBuffer = NULL;

   if (NULL == Result)
   {
      return EFI_INVALID_PARAMETER; 
   }

   provision_key_req.key_type = RPMB_PROVISION_KEY_TEST; 

   ScmBuffer = (UINT8 *) UncachedAllocateZeroPool(sizeof(qsee_provision_rpmb_key_req_t)); 
   if (NULL == ScmBuffer)
   {
      DEBUG((EFI_D_ERROR, "ProvisionRpmbTestKey: UncachedAllocateZeroPool failed!\r\n")); 
      return EFI_DEVICE_ERROR; 
   }

   Status = gBS->LocateProtocol(&gQcomScmProtocolGuid, NULL,
                                (void**)&ScmProtocol);

   if ((EFI_SUCCESS != Status) || (NULL == ScmProtocol))
   {
      DEBUG((EFI_D_ERROR, "ProvisionRpmbTestKey: Failed to find ScmProtocol!!"
                          "Status = (0x%x)\r\n", Status));
      return EFI_NOT_FOUND;
   }

   CopyMem(ScmBuffer, (UINT8 *) &provision_key_req, 
           sizeof(qsee_provision_rpmb_key_req_t)); 

   return ScmProtocol->ScmSendCommand(ScmProtocol, 
                                      APP_PROVISION_RPMB_KEY_COMMAND, NULL,
                                      ScmBuffer, 
                                      sizeof(qsee_provision_rpmb_key_req_t), 
                                      Result, sizeof(UINT32)); 
}

/**
  Function: ConfigKeyValCb
 
  Description:
  This callback function parses the config file and returns the 
  key and value for use. 
**/
STATIC VOID
ConfigKeyValCb (UINT8* Section, UINT8* Key, UINT8* Value)
{
   GUID PartitionGuid; 
   UINT32 ByteSize = 0, SectorSize = 0; 

   CallbackEntered = TRUE; 
   
   if (AsciiStriCmp ((CHAR8*)Key, "Version") == 0)
   {
      CurrentConfigVersion = AsciiStrHexToUintn((CHAR8*)Value);  
   }

   else if (AsciiStriCmp ((CHAR8*)Key, "DevId") == 0)
   {
      CurrentDevId = (SDMgrDeviceType) AsciiStrDecimalToUintn((CHAR8*)Value);  
   }

   else if (AsciiStriCmp ((CHAR8*)Key, "PartitionGuid") == 0)
   {
      if (HexStrToBin (Value, AsciiStrLen((CHAR8*)Value), (UINT32 *) &PartitionGuid, 
                       sizeof(PartitionGuid)))
      {
         if (CompareMem(&PartitionGuid, CurrentPartitionGuid, 16))
         {
            return; 
         }
         GuidFound = TRUE; 
      }
   }

   if ((FALSE == GuidFound) && (SD_MGR_DEVICE_EMMC_RPMB != CurrentDevId))
   {
      return; 
   }

   if (AsciiStriCmp ((CHAR8*)Key, "AppNameId") == 0)
   {
      memset(PartiAdditionData[PartitionsToAdd].AppName, 0, 
	         sizeof(PartiAdditionData[PartitionsToAdd].AppName));
	  CopyMemS( PartiAdditionData[PartitionsToAdd].AppName,
				(sizeof(PartiAdditionData[PartitionsToAdd].AppName)-1),
				Value, AsciiStrLen((CHAR8*)Value));
   }

   else if (AsciiStriCmp ((CHAR8*)Key, "AppPartitionId") == 0)
   {
      PartiAdditionData[PartitionsToAdd].PartitionId = AsciiStrDecimalToUintn((CHAR8*)Value);  
   }

   else if (AsciiStriCmp ((CHAR8*)Key, "SizeInSectors") == 0)
   {
      PartiAdditionData[PartitionsToAdd].NumSectors = AsciiStrDecimalToUintn((CHAR8*)Value); 
      PartitionsToAdd++; 
   }
   else if (AsciiStriCmp ((CHAR8*)Key, "SizeInBytes") == 0)
   {
      ByteSize = AsciiStrDecimalToUintn((CHAR8*)Value); 
      SectorSize = ByteSize / CurrentBlockIoMedia->BlockSize; 

      if (ByteSize % CurrentBlockIoMedia->BlockSize)
      {
         SectorSize++; 
      }

      PartiAdditionData[PartitionsToAdd].NumSectors = SectorSize;  

      PartitionsToAdd++; 
   }
}

/**
  Function: ConfigRpmbStorPartitions
 
  Description:
  This function parses the config file for RPMB entries and updates the BufferPtr with the 
  partition addition information. 
                      
  @return EFI_SUCCESS               Initialization successful.    
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_LOAD_ERROR            Failed to open the parser. 
  @return EFI_UNSUPPORTED           Failed to enumerate the key values 
**/
EFI_STATUS
ConfigRpmbStorPartitions (UINT32 *NumPartitions, UINT8 *BufferPtr)
{
   EFI_SDCC_RPMB_PROTOCOL *RPMBProtocol;
   EFI_STATUS Status;
   INT32 SectionIndex = 0;  
   INTN Pd; 

   /* Find the RPMB protocol to retrieve the RPMB size */
   Status = gBS->LocateProtocol(&gEfiSdccRpmbProtocolGuid, NULL,
                                (void**)&RPMBProtocol);
   if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "ConfigRpmbStorPartitions: Failed to find RPMB Protocol\n")); 
      return Status;
   }

   /* Pick the initial section based on the RPMB size */
   if (RPMBProtocol->RPMBPartitionSizeInBytes >= (1024 * 1024))
   {
      SectionIndex = 3; 
   }
   else if (RPMBProtocol->RPMBPartitionSizeInBytes >= (512 * 1024))
   {
      SectionIndex = 2; 
   }
   else if (RPMBProtocol->RPMBPartitionSizeInBytes >= (256 * 1024))
   {
      SectionIndex = 1; 
   }
   else if (RPMBProtocol->RPMBPartitionSizeInBytes >= (128 * 1024))
   {
      SectionIndex = 0; 
   }
   else
   {
      SectionIndex = -1; 
   }

   /* Enumerate Key Values for RPMB, starting with the initial
      section specified by the RPMB size. If the initial section
      is not found, we will use the section of the next greatest
      size. */
   while (SectionIndex >= 0)
   {
      /* Open the parser for the config file */
      Pd = OpenParser (CfgBuffer, (UINT32)FileSize, NULL);
      if (Pd < 0)
      {
         DEBUG((EFI_D_ERROR, "ConfigRpmbStorPartitions: OpenParser failed\n")); 
         return EFI_LOAD_ERROR;
      }

      EnumKeyValues (Pd, RpmbSectionNames[SectionIndex], ConfigKeyValCb); 

      CloseParser(Pd); 

      if (FALSE == CallbackEntered)
      {
         /* Did not find the section, so try again looking for a smaller
            RPMB section */
         SectionIndex--;  
      }
      else
      {
         /* Otherwise, we are done */
         break; 
      }
   }

   if (SectionIndex == -1)
   {
      return EFI_UNSUPPORTED; 
   }

   /* Reset CallbackEntered */
   CallbackEntered = FALSE; 

   /* Check the DevId */
   if (SD_MGR_DEVICE_EMMC_RPMB != CurrentDevId)
   {
      return EFI_INVALID_PARAMETER; 
   }

   CopyMem(BufferPtr, PartiAdditionData, sizeof(SdMgrPartiAdditionData) * PartitionsToAdd); 
   *NumPartitions = PartitionsToAdd; 

   return EFI_SUCCESS; 
}

/**
  Function: ConfigGptStorPartitions
 
  Description:
  This function parses the config file for GPT entries and updates the BufferPtr with the 
  partition addition information. 
                      
  @return EFI_SUCCESS               Initialization successful.    
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_LOAD_ERROR            Failed to open the parser. 
  @return EFI_UNSUPPORTED           Failed to enumerate the key values 
**/
EFI_STATUS
ConfigGptStorPartitions (UINT8 *PartitionGuid, UINT32 *NumPartitions, UINT8 *BufferPtr, 
                         EFI_BLOCK_IO_MEDIA *BlkIoMedia)
{ 
   EFI_STATUS Status = EFI_SUCCESS;
   INTN Pd; 
   
   if ((NULL == PartitionGuid) || (NULL == NumPartitions) || 
       (NULL == BufferPtr) || (NULL == BlkIoMedia))
   {
      return EFI_INVALID_PARAMETER;
   } 
   
   /* Open the parser for the config file */
   Pd = OpenParser (CfgBuffer, (UINT32)FileSize, NULL);
   if (Pd < 0)
   {
      DEBUG((EFI_D_ERROR, "ConfigGptStorPartitions: OpenParser failed\n")); 
      return EFI_LOAD_ERROR;
   }

   /* Save the PartitionGuid */
   CopyMem(CurrentPartitionGuid, PartitionGuid, 16); 

   /* Save the BlockIoMedia */
   CurrentBlockIoMedia = BlkIoMedia; 

   /* Enumerate Key Values for GPT */
   EnumKeyValues(Pd, (UINT8 *)("GPT"), ConfigKeyValCb); 

   /* If we did not enter the callback, return error */
   if (FALSE == CallbackEntered)
   {
      Status = EFI_UNSUPPORTED; 
      goto ConfigGptExit; 
   }

   /* Reset CallbackEntered */
   CallbackEntered = FALSE; 

   /* Check the DevId */
   if ((SD_MGR_DEVICE_EMMC_RPMB == CurrentDevId) ||
       (SD_MGR_DEVICE_EMMC_ALL < CurrentDevId))
   {
      Status = EFI_INVALID_PARAMETER; 
      goto ConfigGptExit; 
   }

   CopyMem(BufferPtr, PartiAdditionData, sizeof(SdMgrPartiAdditionData) * PartitionsToAdd); 
   *NumPartitions = PartitionsToAdd; 

ConfigGptExit: 
   CloseParser(Pd); 
   return Status; 
}

/**
  Function: ConfigStorPartitions
 
  Description:
  This function parses the config file and updates the BufferPtr with the 
  partition addition information. 
                      
  @return EFI_SUCCESS               Initialization successful.    
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_LOAD_ERROR            Failed to open the parser. 
  @return EFI_UNSUPPORTED           Failed to enumerate the key values 
**/
EFI_STATUS
EFIAPI
ConfigStorPartitions (UINT32 DevId, UINT8 *PartitionGuid, 
                      UINT32 Version, UINT32 *NumPartitions, UINT8 *BufferPtr, 
                      VOID *BlkIoMedia)
{
   EFI_STATUS Status;
   INTN Pd;
   
   if ((NULL == NumPartitions) || (NULL == BufferPtr))
   {
      return EFI_INVALID_PARAMETER; 
   }

   /* LA doesn't require partition configuration */
   if (0 == AsciiStrCmp(OsType, "LA"))
   {
      return EFI_UNSUPPORTED;
   }

   /* Open the parser for the config file */
   Pd = OpenParser (CfgBuffer, (UINT32)FileSize, NULL);
   if (Pd < 0)
   {
      DEBUG((EFI_D_ERROR, "ConfigStorPartitions: OpenParser failed\n")); 
      return EFI_LOAD_ERROR;
   }
   
   /* Enumerate Key Values for Version */
   EnumKeyValues (Pd, (UINT8 *)("VersionNum"), ConfigKeyValCb);

   /* If we did not enter the callback, return error */
   if (FALSE == CallbackEntered)
   {
      Status = EFI_UNSUPPORTED; 
      goto ConfigExit; 
   }

   /* Reset CallbackEntered */
   CallbackEntered = FALSE; 
   
   /* Check the version of the config file */
   if (Version != CurrentConfigVersion)
   {
      DEBUG((EFI_D_ERROR, "ConfigStorPartitions: Config file version mismatch\n"));
      Status = EFI_INVALID_PARAMETER; 
      goto ConfigExit; 
   }
   
   SetMem((UINT8 *) PartiAdditionData, sizeof(SdMgrPartiAdditionData) * MAX_PARTITIONS, 0x00);
   PartitionsToAdd = 0; 

   if (SD_MGR_DEVICE_EMMC_RPMB == (SDMgrDeviceType) DevId)
   {
      Status = ConfigRpmbStorPartitions(NumPartitions, BufferPtr); 
   }
   else
   {
      Status = ConfigGptStorPartitions(PartitionGuid, NumPartitions, BufferPtr, 
                                       (EFI_BLOCK_IO_MEDIA *) BlkIoMedia); 
   }

ConfigExit: 
   CloseParser(Pd); 
   return Status; 
}

/**
  Function: InitPartitionConfig
 
  Description:
  This function loads the partition config file from the FV.
                      
  @return EFI_SUCCESS               Initialization successful.        
**/
EFI_STATUS
EFIAPI
InitPartitionConfig(VOID)
{
   EFI_STATUS Status; 
   UINTN OsTypeStringLen = OS_TYPE_LEN;

   /* Determine OS Type */
   if (NULL == OsType)
   {
      OsType = AllocatePool (OsTypeStringLen * sizeof(CHAR8));
      if (NULL == OsType)
      {
         DEBUG ((EFI_D_ERROR, "Failed to allocate OsType\n"));
         return EFI_OUT_OF_RESOURCES;
      }

      Status = GetConfigString ("OsTypeString", OsType, &OsTypeStringLen);
      if ((EFI_SUCCESS != Status) || (0 == OsTypeStringLen))
      {
         DEBUG ((EFI_D_ERROR, "Error reading OsTypeString\n"));
         return Status; 
      }
   }

   /* Load the partiapp.cfg file, if needed */
   if (0 == AsciiStrCmp(OsType, "WP"))
   {
      Status = ReadFromFV(PARTITION_CONFIG_FILE, (void **) &CfgBuffer, &FileSize);
      if (EFI_SUCCESS != Status)
      {
         DEBUG((EFI_D_ERROR, "Failed to load partiapp.cfg!"
                             "Status = (0x%x)\r\n", Status));
         return Status; 
      }
   }

   return EFI_SUCCESS; 
}

/**
  Function: RPMBCreateReadPacket
 
  Description:
  This function creates the RPMB Read packet
                      
  @return EFI_SUCCESS               RPMB read frame created. 
  @return EFI_INVALID_PARAMETER     Invalid parameter. 
**/
EFI_STATUS
EFIAPI
RPMBCreateReadPacket (UINT32 StartSector, UINT8* PktBuffer)
{
   RPMB_DATA_FRAME *RpmbDataFrame = (RPMB_DATA_FRAME *)PktBuffer;
   UINT16 RequestResponse = 0, TempStartSector = 0;

   StartSector = StartSector * 2;

   if (NULL == RpmbDataFrame)
   {
      return EFI_INVALID_PARAMETER;
   }

   SetMem(RpmbDataFrame, sizeof(RPMB_DATA_FRAME), 0x00);

   /* Set the Request type to Data Read */ 
   RequestResponse = READ_DATA_REQ;
      
   /* RPMB data frame is Big Endian. Reverse the byte order if the
      system is Little Endian */
   RequestResponse = IS_BIG_ENDIAN() ? RequestResponse : REVERSE_WORD(RequestResponse);

   CopyMem(RpmbDataFrame->RequestResponse, &RequestResponse, 
           sizeof(RpmbDataFrame->RequestResponse));

   /* Format the 2-byte Address field of the RPMB Data Frame */
   TempStartSector = IS_BIG_ENDIAN() ? StartSector : REVERSE_WORD(StartSector);
   CopyMem(RpmbDataFrame->Address, &TempStartSector, 
              sizeof(RpmbDataFrame->Address));
   
   return EFI_SUCCESS;
}

/**
  Function: RPMBProvisioned
 
  Description:
  This function checks if RPMB is already provisioned or not.  
                      
  @return EFI_SUCCESS               RPMB already provisioned. 
  @return EFI_UNSUPPORTED           RPMB not yet provisioned. 
  @return EFI_DEVICE_ERROR          CG/RPMB read failure 
**/
EFI_STATUS
EFIAPI
RPMBProvisioned (VOID)
{
   EFI_SDCC_RPMB_PROTOCOL *RpmbProtocol = NULL;
   EFI_STATUS Status = EFI_SUCCESS;  
   RPMB_DATA_FRAME RpmbResponsePkt; 
   INT32 Result; 

   Status = gBS->LocateProtocol(&gEfiSdccRpmbProtocolGuid, NULL,
                                (void**)&RpmbProtocol);

   if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "Locate RPMB Protocol failed, status = 0x%x\n", Status));
      return Status;
   }

   /* Read the eMMC's Write Counter */
   Result = RpmbProtocol->RPMBReadCounterPkt(RpmbProtocol, (UINT32 *)&RpmbResponsePkt); 
   if (RPMB_ERR_KEY_NOT_PROGRAM == Result)
   {
      DEBUG((EFI_D_ERROR, "RPMB not provisioned\n"));
      return EFI_UNSUPPORTED;
   }
   else if (RPMB_NO_ERROR != Result)
   {    
      DEBUG((EFI_D_ERROR, "RPMB ReadCounterPkt failed, Result = %d\n", Result));
      return EFI_DEVICE_ERROR;
   }

   return EFI_SUCCESS; 
}

/**
  Function: RPMBReadData
 
  Description:
  This function reads BlockCount sectors starting at Lba from 
  the RPMB partition. 
                      
  @return EFI_SUCCESS               Data read successfully. 
  @return EFI_INVALID_PARAMETER     Invalid parameter. 
  @return EFI_DEVICE_ERROR          CG/RPMB read failure 
  @return EFI_OUT_OF_RESOURCES      Insufficient memory 
  @return EFI_UNSUPPORTED           RPMB is not provisioned  
**/
EFI_STATUS
EFIAPI
RPMBReadData (
   UINT32                Lba, 
   UINT32                BlockCount, 
   UINT8                 *ReadBuffer
   )
{
   EFI_SDCC_RPMB_PROTOCOL *RpmbProtocol = NULL;
   EFI_STATUS Status = EFI_SUCCESS; 
   UINT8 ReadPktBuffer[BLOCK_SIZE];
   UINT8 *RpmbReadBuffer = NULL; 
   UINT32 i = 0; 

   if (NULL == ReadBuffer)
   {
      return EFI_INVALID_PARAMETER;
   }

   Status = gBS->LocateProtocol(&gEfiSdccRpmbProtocolGuid, NULL,
                                (void**)&RpmbProtocol);

   if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "Locate RPMB Protocol failed, status = 0x%x\n", Status));
      return Status;
   }

   Status = RPMBProvisioned(); 
   if (EFI_UNSUPPORTED == Status)
   {
      DEBUG((EFI_D_ERROR, "Cannot read the data: RPMB is not provisioned, status = 0x%x\n", Status));
      return Status;
   }
   else if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "RPMBProvisioned failed, status 0x%x\n", Status));
      return Status;
   }

   Status = RPMBCreateReadPacket (Lba, ReadPktBuffer);
   if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "RPMBCreateReadPacket failed, status = 0x%x\n", Status));
      return Status;
   }

   RpmbReadBuffer = (UINT8 *)AllocateZeroPool(2 * BlockCount * BLOCK_SIZE); 
   if (NULL == RpmbReadBuffer)
   {
      DEBUG((EFI_D_ERROR, "AllocateZeroPool failed\n"));
      return EFI_OUT_OF_RESOURCES;
   }
  
   /* Read the RPMB partition */
   Status = RpmbProtocol->RPMBReadBlocks(RpmbProtocol, (UINT32 *)ReadPktBuffer, 
                                         2 * BlockCount, (UINT32 *) RpmbReadBuffer);
   if (RPMB_NO_ERROR != Status)
   {
      DEBUG((EFI_D_ERROR, "RPMBReadBlocks Failed, status = 0x%x\n", Status));
      Status = EFI_DEVICE_ERROR;   
      goto ReadEnd;  
   }

   /* Extract the data from the RPMB response packets */
   for (i = 0; i < (2 * BlockCount); i++)
   {
      gBS->CopyMem(ReadBuffer + (DATA_FIELD_LEN * i), 
                   RpmbReadBuffer + DATA_INDEX + (sizeof(RPMB_DATA_FRAME) * i), DATA_FIELD_LEN); 
   }

ReadEnd: 
   if (RpmbReadBuffer)
   {
      gBS->FreePool(RpmbReadBuffer);
   }

   return Status; 
}


