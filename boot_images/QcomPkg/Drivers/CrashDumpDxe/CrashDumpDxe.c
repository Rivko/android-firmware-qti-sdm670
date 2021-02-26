/** @file CrashDumpDxe.c
   
  Code for reset reason feature for retail devices

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc. 
  All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  when        who       what, where, why
  --------    ---       ----------------------------------------------------------
  10/04/2016  ly        Update DBI dump table for additional entries
  08/24/2016  ly        Update offline crash dump and reset buffer  
  02/10/2016  na        check DbiDumpTable DDR Base from cfg file rather than FdRegion
  07/29/2015  ao        bring in crash Dump related code from Sec.c
  12/11/2014  ao        update with new SPMI API
  08/15/2014  sm        Switched to new SCM API
  07/09/2014  shireesh  Added pmic PON_PBL_STATUS register to reset data buffer
  06/06/2014  vk        Fix crash in 64 bit
  04/29/2014  shireesh  Added support for h/w button trigerd reset detection
  03/26/2014  shireesh  Initial version
  
=============================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Include/UefiInfoBlk.h>
#include <Library/QcomLib.h>
#include <Library/DbiDumpLib.h>
#include <Library/OfflineCrashDump.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiCfgLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPmicPwrOn.h>
#include <Protocol/EFISPMI.h>
#include <Library/HobLib.h>
#include <Library/RamPartitionTableLib.h>
#include <Library/IoLib.h>
#include <string.h>
#include <boot_shared_imem_cookie.h>

/*Define a global pointer which points to the boot shared imem cookie structure */
struct boot_shared_imem_cookie_type *boot_shared_imem_cookie_ptr = NULL;

STATIC EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE * OfflineCrashDumpConfigTable = NULL;

/**
  Function make syscall to TZ to provide buffer address to copy diag data

**/
STATIC
BOOLEAN
EFIAPI
TZResetDiagDataBufferAddressSysCall(VOID* address, UINTN size)
{ 
  EFI_STATUS        Status;
  BOOLEAN           SysCallStatus     = FALSE;
  QCOM_SCM_PROTOCOL *pQcomScmProtocol = NULL;
  UINT64            Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64            Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_set_address_to_dump_tz_diag_for_uefi_req_t *SysCallReq = (tz_set_address_to_dump_tz_diag_for_uefi_req_t*)Parameters;
  tz_syscall_rsp_t *SysCallRsp = (tz_syscall_rsp_t*)Results;
  
  
  /* Initializing cmd structure for SCM sys call */
  SysCallReq->size        = size;
  SysCallReq->addr        = (UINTN)address;

  /* Locate QCOM_SCM_PROTOCOL */
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, NULL, (VOID **)&pQcomScmProtocol);
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_ERROR, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  /* Make a SCM Sys call */
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_DUMP_SET_ADDRESS_TO_DUMP_TZ_DIAG_FOR_UEFI,
                                            TZ_DUMP_SET_ADDRESS_TO_DUMP_TZ_DIAG_FOR_UEFI_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->status != 1)
  {    
    DEBUG(( EFI_D_ERROR, "TZ_DUMP_SET_ADDRESS_TO_DUMP_TZ_DIAG_FOR_UEFI failed, Status = (0x%x)\r\n", SysCallRsp->status));
    goto ErrorExit;
  }

  SysCallStatus = TRUE;

ErrorExit:
  return SysCallStatus;
}

/**
  Function to get offlinedump Config table

**/
EFIAPI
EFI_STATUS
GetCrashDumpConfigTable( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = EfiGetSystemConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid, (VOID**)&OfflineCrashDumpConfigTable);
  if (Status != EFI_SUCCESS) 
  {
    DEBUG(( EFI_D_ERROR, "Offline Crash Dump Config Table does not exist in System Configuration Table\r\n"));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 This function read a PMIC register and populate it to UEFI RESET BUFFER
**/
CHAR8* Populate_PMIC_POWER_ON_OFF_Reasons(
                                       EFI_QCOM_SPMI_PROTOCOL *spmi_protocol,
									   UINT32 pmic_register_address,
                                       const CHAR8* pmic_register_name_string,
									   CHAR8* buffer_address
									   )
{
  Spmi_Result result = SPMI_SUCCESS;
  volatile UINT32 RegDump = 0x0;
  
  /* Read PMIC register value */
  result = spmi_protocol->ReadLong(spmi_protocol, 0, SPMI_BUS_ACCESS_PRIORITY_LOW, pmic_register_address, (UINT8*)&RegDump, (UINT32)1,0);
  if ( result != SPMI_SUCCESS )
  {
      DEBUG(( EFI_D_ERROR, "Read to %s register failed, Status = (0x%x)\r\n", pmic_register_name_string,result));
  }
   
  /* Populate the PMIC register name and value to designated buffer location */   
  AsciiStrCpy(buffer_address, pmic_register_name_string);    
  buffer_address = (CHAR8*)( (UINTN)buffer_address + AsciiStrLen(pmic_register_name_string));
  AsciiValueToString ((buffer_address), 0, (UINT64)RegDump, 8);
  buffer_address=buffer_address+sizeof(UINT64);
  return buffer_address;
}

/**
 This function intializes Offline Crash Dump Config table fields for reset reason

**/
STATIC
EFI_STATUS
EFIAPI
InitializeResetDataBuffers ( EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *mOfflineCrashDumpConfigTable )
{
  EFI_STATUS    Status;
  MemRegionInfo ResetDataRegionInfo;
  volatile UINT32 RegDump = 0x0;
  
  CHAR8* UefiResetBufferAddress = NULL;
  VOID* tzBufferAddress = 0;
  RESET_DATA_HEADER *mResetReasonData = NULL;
  EFI_QCOM_SPMI_PROTOCOL *pSPMI = NULL;
  EFI_CHIPINFO_PROTOCOL *pChipInf;

  const CHAR8 GccStr[]               = "GCC_RESET_STATUS=";
  const CHAR8 PONPBLStatusStr[]      = "PON_PBL_STATUS=";
  const CHAR8 PONPONReason1Str[]     = "PON_PON_REASON1=";	 
  const CHAR8 PONONReasonStr[]       = "PON_ON_REASON=";	
  const CHAR8 PONWarmReset1Str[]     = "PON_WARM_RESET_REASON1=";  
  const CHAR8 PONFault1Str[]         = "PON_FAULT_REASON1=";
  const CHAR8 PONFault2Str[]         = "PON_FAULT_REASON2=";
  const CHAR8 PONS3ResetReasonStr[]  = "PON_S3_RESET_REASON=";
  const CHAR8 PONSoftReset1Str[]     = "PON_SOFT_RESET_REASON1=";
  const CHAR8 PONPOFF1Str[]          = "PON_POFF_REASON1=";
  const CHAR8 PONOFFStr[]            = "PON_OFF_REASON=";	

  /* UEFI total reset data size in bytes 
     - Version
	 - GCC_RESET_STATUS
	 - PMIC registers - total 10 registers
   */
  UINT64 UefiResetDataSize = sizeof(UINT64)+
                             AsciiStrLen(GccStr)+
                             AsciiStrLen(PONPBLStatusStr)+
                             AsciiStrLen(PONPONReason1Str)+
                             AsciiStrLen(PONONReasonStr)+
                             AsciiStrLen(PONWarmReset1Str)+
                             AsciiStrLen(PONFault1Str)+
                             AsciiStrLen(PONFault2Str)+
                             AsciiStrLen(PONS3ResetReasonStr)+
                             AsciiStrLen(PONSoftReset1Str)+
                             AsciiStrLen(PONPOFF1Str)+
                             AsciiStrLen(PONOFFStr)+
                             11* sizeof(UINT64);

  /* Get the Reset Data memory region. */
  Status = GetMemRegionInfoByName("Reset Data", &ResetDataRegionInfo);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, " Could not find the Reset Data region .\r\n"));
    return Status;
  }
  
  mResetReasonData = (RESET_DATA_HEADER*) ((UINTN)ResetDataRegionInfo.MemBase);

  /* Initialize reset reason data structure*/
  mResetReasonData->Version = RESET_DATA_HEADER_VERSION;
  
  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL, (VOID **) &pChipInf);
  if (EFI_SUCCESS != Status)
  { 
     return Status;
  }

  if (EFI_SUCCESS != pChipInf->GetChipIdString(pChipInf, mResetReasonData->PlatformID, PLATFORMID_MAX_LENGTH))
  {
    DEBUG ((EFI_D_WARN, "gEfiChipInfoProtocolGuid->GetChipIdString() Failed. \n"));
    Status = EFI_UNSUPPORTED;
    return Status;
  }
  
  mResetReasonData->Guid                        = gQcomResetReasonDataCaptureGuid;
  /* Size of datbuffer+header */
  mResetReasonData->ResetDataBufferSize         = sizeof(RESET_DATA_BUFFER);       
  mResetReasonData->ResetDataBuffer.Version     = RESET_DATA_BUFFER_VERSION;
  
  /* Offset from reset data header to tz data buffer */
  mResetReasonData->ResetDataBuffer.DataOffset1 = sizeof(RESET_DATA_HEADER) - QCDATA_1_SIZE - QCDATA_2_SIZE;
  mResetReasonData->ResetDataBuffer.DataSize1   = QCDATA_1_SIZE;

  /* Offset from reset data header to UEFI data buffer */
  mResetReasonData->ResetDataBuffer.DataOffset2 = mResetReasonData->ResetDataBuffer.DataOffset1 + QCDATA_1_SIZE;
  mResetReasonData->ResetDataBuffer.DataSize2   = QCDATA_2_SIZE;


  /* Setup buffer address for TZ to dump to within Reset Data region */
  tzBufferAddress = (UINT8*)mResetReasonData + (UINTN)mResetReasonData->ResetDataBuffer.DataOffset1;
  TZResetDiagDataBufferAddressSysCall(tzBufferAddress, QCDATA_1_SIZE);

  /* Check if Abnormal Reset occurred. If not, TZ Diag Buffer contents are from previous abnormal boot
     and will be mark by "NORMAL Boot".
   */
  if(mOfflineCrashDumpConfigTable->AbnormalResetOccurred == ABNORMAL_RESET_DISABLE)
  {
	  AsciiStrCpy((CHAR8 *)tzBufferAddress, "NORMAL BOOT");  
  }
  
  mOfflineCrashDumpConfigTable->ResetDataAddress = (UINT64)mResetReasonData;
  mOfflineCrashDumpConfigTable->ResetDataSize    = sizeof(RESET_DATA_HEADER);
  
  /* Return if there is not enough room for all data */
  if ( UefiResetDataSize > QCDATA_2_SIZE )
  { 
     return EFI_OUT_OF_RESOURCES;
  }
  
  /* Sets up UefiResetBufferAddress to point to just after TZ buffer */
  UefiResetBufferAddress = (CHAR8*)((UINTN)tzBufferAddress + QCDATA_1_SIZE);
    
  /* Clear UEFI reset buffer region */
  memset(UefiResetBufferAddress, 0x0, UefiResetDataSize); 
    
  /* Fill in version as first field in UEFI buffer */
  *((UINT64*)UefiResetBufferAddress) = RESET_DATA2_VERSION;
    
  /* Copy GCC reset string to UEFI data buffer */
  UefiResetBufferAddress = UefiResetBufferAddress + sizeof(UINT64);
  AsciiStrCpy(UefiResetBufferAddress, GccStr);
  UefiResetBufferAddress = (CHAR8*)( (UINTN)UefiResetBufferAddress + AsciiStrLen(GccStr));
  
  boot_shared_imem_cookie_ptr = (struct boot_shared_imem_cookie_type *)((UINT64)PcdGet32(PcdIMemCookiesBase));
  if(boot_shared_imem_cookie_ptr == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Copy GCC reset from SMEM to UEFI data buffer 
     Note: GCC_RESET_STATUS is read in XBL loader, saved to shared IMEM and then cleared.
  */
  RegDump = boot_shared_imem_cookie_ptr->reset_status_register;
  
  AsciiValueToString ((UefiResetBufferAddress), 0, (UINT64)RegDump, 8);
  UefiResetBufferAddress = UefiResetBufferAddress + sizeof(UINT64);

  /* Populate PMIC registers to reset data buffer */

  /* Get SPMI Protocol */
  Status = gBS->LocateProtocol(&gQcomSPMIProtocolGuid, NULL, (VOID **) &pSPMI);
  if ( EFI_ERROR (Status) )
  {
      DEBUG(( EFI_D_ERROR, "MemMapDump: failed to locate SPMIProtocol, Status = (0x%x)\r\n", Status));
      return Status;
  }

  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_PBL_STATUS,PONPBLStatusStr,UefiResetBufferAddress);  
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_PON_REASON1,PONPONReason1Str,UefiResetBufferAddress);  
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_ON_REASON,PONONReasonStr,UefiResetBufferAddress);  
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_WARM_RESET_REASON1,PONWarmReset1Str,UefiResetBufferAddress);  
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_FAULT_REASON1,PONFault1Str,UefiResetBufferAddress);  
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_FAULT_REASON2,PONFault2Str,UefiResetBufferAddress); 
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_S3_RESET_REASON,PONS3ResetReasonStr,UefiResetBufferAddress); 
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_SOFT_RESET_REASON1,PONSoftReset1Str,UefiResetBufferAddress); 
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_POFF_REASON1,PONPOFF1Str,UefiResetBufferAddress); 
  UefiResetBufferAddress = Populate_PMIC_POWER_ON_OFF_Reasons(pSPMI,PON_OFF_REASON,PONOFFStr,UefiResetBufferAddress);   

  return EFI_SUCCESS; 
}

EFI_STATUS
EFIAPI
CrashDumpDxeInitialize(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
   EFI_STATUS Status = EFI_SUCCESS;
   UINT64 UefiFdBase = PcdGet64(PcdEmbeddedFdBaseAddress);
   MemRegionInfo FdRegion;
   
   /* Get Available memory from partition table */  
   InitRamPartitionTableLib();
 
   Status = GetPartitionEntryByAddr (UefiFdBase, &FdRegion);
   if (Status != EFI_SUCCESS)
   {
      DEBUG ((EFI_D_ERROR, "ERROR: Unable to Get Available memory from partition table!!!\r\n"));
	  DEBUG ((EFI_D_ERROR, "ERROR: CrashDump setup failed!!!\r\n"));
      return Status;
   }
   
   SetupDBIDumpTable();
   HandleCrashDump();
   UpdateOfflineCrashDumpConfigTable();
   GetCrashDumpConfigTable();
   InitializeResetDataBuffers(OfflineCrashDumpConfigTable);
   
   return Status;
}

