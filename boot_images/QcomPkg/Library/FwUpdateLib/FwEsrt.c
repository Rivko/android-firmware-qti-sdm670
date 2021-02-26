/** @file FwESRT.c
   
   Functions to maintain the ESRT , including ESRT header and ESRT entry . 
  
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2/15/04/16     mic	       Use updated DppProtocol 
 2015/03/31     mic        Added 8994 platform types
 2013/06/11     rsb        Added 8074 platform types
 2013/04/11     rsb        Added 8084 platform types
 2012/12/11     mic        Fixed KW warnings
 2012/11/30     mic        Added ESRT entry print function
 2012/11/09     jmb        Adding CBH platform
 2012/10/21     mic        Code review
 2012/10/03     ash        Fixed version matching for 8064 platforms.
 2012/09/19     rsb        Added 8974 platform types
 2012/08/14     ash        Added 8064 WB292 and V1 variants.
 2012/06/21     rs         Adding support for FPDT ACPI table  
 2012/06/21     mic        Set the default ESRT FW ver/lowest support version/last attempt version to 0
 2012/06/05     rs         Enhanced lowest supported version check
 2012/05/09     rsb        Platform run-time type detection for FW Resource GUID
 2012/05/07     jd         Migrated to DPP Protocol.
 2012/02/01     mic        Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Include/Library/FwCommonLib.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIDpp.h>
#include "FwEsrt.h"
#include "FwCapsule.h"
#include "FwPayloadEntry.h"

#define DEFAULT_ESRT_CAPSULE_FLAGS 0x0
#define ESRT_PROVISIONING_FILE_NAME L"ESRT_SYS.PROVISION"
#define ESRT_PROVISIONING_DATA_TYPE L"QCOM"

/*extern variable*/
extern QPAYLOAD_METADATA_HEADER_V3 PayLoadHeader;

/*global variable*/
UINT16                  *esrtfilename = ESRT_PROVISIONING_FILE_NAME;
UINT16                  *ProvisiondataType = ESRT_PROVISIONING_DATA_TYPE;
ESRT_ENTRY   esrt_ent          = {QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_GUID,
                                  FW_TYPE_SYSTEM, 
                                  0x0, 
                                  0x0, 
                                  DEFAULT_ESRT_CAPSULE_FLAGS, 
                                  0x0, 
                                  FW_LAST_ATMPT_STATUS_SUCCESS
                                 };
EFI_GUID          ESRT_guid = QCOM_FW_UPDATE_ESRT_GUID;
EFI_GUID          UXCapGuid = QCOM_FW_UPDATE_UX_CAPSULE_GUID;

/* The list of System Firmware Resource GUIDs that apply for each type of platform, the order must
   match the QCOM_FW_RESOURCE_TYPE. */
const EFI_GUID qcomFwResourceGUIDs[] = 
{
    QCOM_FW_UPDATE_SYSTEM_FW_8996_MTP_GUID,
    QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_GUID
};

/**
 * Function to print ESRT entry 
 * 
 * @param ent  - ESRT entry
 * @DebugLevel - Debug level 
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
QcomPrintEsrtEntry(
  IN ESRT_ENTRY *ent, 
  IN UINTN      DebugLevel
  )
{

  DEBUG((DebugLevel,"    Esrt info:\n"));
  DEBUG((DebugLevel,"    ====================================\n"));
  
  DEBUG((DebugLevel,"    GUID                     :"));
  FwCommonPrintGuid(ent->FWClass,DebugLevel);
  DEBUG((DebugLevel,"\n"));

  DEBUG((DebugLevel,"    FWType                   : 0x%x\n", ent->FWType));
  DEBUG((DebugLevel,"    FWVer                    : 0x%x\n", ent->FWVer));
  DEBUG((DebugLevel,"    Lowest supported version : 0x%x\n", ent->FwLowestSupportedVer));
  DEBUG((DebugLevel,"    Capsuleflag              : 0x%x\n", ent->CapsuleFlag));
  DEBUG((DebugLevel,"    Last attempt ver         : 0x%x\n", ent->LastAttemptVer));  
  DEBUG((DebugLevel,"    Last attempt status      : 0x%x\n", ent->LastAttemptStatus));

  DEBUG((DebugLevel,"    ====================================\n"));

  return EFI_SUCCESS;
}

/**
 *  Read ESRT entry
 *   
 * @param ent - esrt entry 
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
QcomGetEsrtEntry(
  IN OUT ESRT_ENTRY  **ent
  )
{
  if( ent == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  *ent = &esrt_ent;

  return EFI_SUCCESS;
}

/**
 *  Check if ESRT header is installed in UEFI System table 
 *   
 * @param SystemTable -UEFI system table
 *   
 * @return BOOLEAN 
 */
BOOLEAN
QcomIsEsrtHeaderInstalledInSysTbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_CONFIGURATION_TABLE *pcfg_table = NULL;
  UINTN                    cs_cnt     = 0;
  UINTN                    i          = 0;

  if (SystemTable == NULL)
  {
    return FALSE;
  }
  
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;

  for ( i = 0; i < cs_cnt; i++)
  {
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      return TRUE;
    }
    pcfg_table++;
  }
    
    return FALSE;
}

/**
 * Read ESRT entry info from storage
 * 
 * @param buf       - out buffer
 * @param esrtSize  - out buffer size 
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
QcomRead_EsrtEntryFromStorage(
  IN OUT ESRT_ENTRY **buf, 
  IN OUT UINTN       *esrtSize
  )
{
  EFI_STATUS         Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL  *DppProtocol = NULL;

  if((buf == NULL) ||
     (esrtSize == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *buf = NULL;
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,QcomRead_EsrtEntryFromStorageExit);

  Status = DppProtocol->GetDPP(DppProtocol,ProvisiondataType,esrtfilename,NULL,esrtSize);
  HANDLE_ERROR_LABEL(Status,DppProtocol->GetDPP,QcomRead_EsrtEntryFromStorageExit);
  
  Status = gBS->AllocatePool(EfiBootServicesData,*esrtSize,(VOID **)buf);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,QcomRead_EsrtEntryFromStorageExit);

  Status = DppProtocol->GetDPP(DppProtocol,ProvisiondataType,esrtfilename,*buf,esrtSize);
  HANDLE_ERROR_LABEL(Status,DppProtocol->GetDPP,QcomRead_EsrtEntryFromStorageExit);

  return EFI_SUCCESS;

QcomRead_EsrtEntryFromStorageExit:

  if(*buf != NULL)
  {
    gBS->FreePool(*buf);
    *buf = NULL;
  }
  return Status;
}

/**
 * Write ESRT entry info into storage.
 * 
 * @param inBuf    - buffer
 * @param esrtSize - buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
QcomWrite_EsrtEntryToStorage(
  IN ESRT_ENTRY *inBuf, 
  IN UINTN       esrtSize
  )
{
  EFI_STATUS         Status      = EFI_SUCCESS;
  VOID              *EsrtBuffer  = NULL;
  EFI_DPP_PROTOCOL  *DppProtocol = NULL;

  if(inBuf == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
    
  /*Create the new ESRT entry*/
  Status=gBS->AllocatePool(EfiBootServicesData,esrtSize,&EsrtBuffer);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,QcomWrite_EsrtEntryToStorageExit);

  /*Initialize the ESRT entry*/
  ZeroMem(EsrtBuffer,esrtSize);
  CopyMem(EsrtBuffer,(VOID*)inBuf,esrtSize);

  /*write the ESRT entry to DPP*/
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,QcomWrite_EsrtEntryToStorageExit);

  Status = DppProtocol->SetDPP(DppProtocol,ProvisiondataType,esrtfilename,EsrtBuffer,esrtSize);
  HANDLE_ERROR_LABEL(Status,DppProtocol->SetDPP,QcomWrite_EsrtEntryToStorageExit);
  
QcomWrite_EsrtEntryToStorageExit:

  if(EsrtBuffer != NULL)
  {
    gBS->FreePool(EsrtBuffer);
    EsrtBuffer = NULL;
  }

  return Status;

}

/**
 * Install ESRT header in system table
 *
 * @param SystemTable - UEFI System Table
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
QcomInstallEsrtHeaderInSysTbl(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS         Status           = EFI_SUCCESS;
  ESRT_TABLE_HEADER *pesrt_tbl_header = NULL;

  if (SystemTable == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /*Create a new ESRT header*/
  Status=gBS->AllocatePool(EfiRuntimeServicesData,sizeof(ESRT_TABLE_HEADER),(VOID*)&pesrt_tbl_header);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,QcomInstallEsrtHeaderInSysTblExit);

  /*Initialize the header*/
  pesrt_tbl_header->FWResrcCnt=0;
  pesrt_tbl_header->FWResrcMax=0;
  pesrt_tbl_header->FWResrcVer=1;
  
  /*Install the config table*/
  Status = gBS->InstallConfigurationTable (&ESRT_guid, pesrt_tbl_header);
  HANDLE_ERROR_LABEL(Status,gBS->InstallConfigurationTable,QcomInstallEsrtHeaderInSysTblExit); 

QcomInstallEsrtHeaderInSysTblExit:
  
  return Status;
};

/**
 * Install ESRT entry in ESRT in System table
 *
 * @param SystemTable - the UEFI System Table
 *
 * @return EFI_STATUS 
 */

EFI_STATUS
QcomInstallEsrtEntryInSysTbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status          = EFI_SUCCESS;
  EFI_CONFIGURATION_TABLE *pcfg_table      = NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header = NULL;
  ESRT_TABLE_HEADER       *pNew_tbl_header = NULL;
  ESRT_ENTRY              *pNew_esrt_ent   = NULL;
  ESRT_ENTRY              *pTemp           = NULL;
  UINTN                    old_ent_cnt     = 0;
  UINTN                    new_ent_cnt     = 0;
  UINTN                    old_tbl_size    = 0;
  UINTN                    new_tbl_size    = 0;
  UINTN                    cs_cnt          = 0;
  UINTN                    i               = 0;

  if (SystemTable == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /*check if ESRT table exist?*/
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
  
  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      pCur_tbl_header = (ESRT_TABLE_HEADER*)pcfg_table->VendorTable;

      if (pCur_tbl_header == NULL)
      {
         Status = EFI_INVALID_PARAMETER;
         HANDLE_ERROR_LABEL(Status,QcomInstallEsrtEntryInSysTbl,QcomInstallEsrtEntryInSysTblExit); 
      }
      else
      {
        /*calc the new count of ESRT entries*/
        old_ent_cnt = pCur_tbl_header->FWResrcCnt;
        new_ent_cnt = pCur_tbl_header->FWResrcCnt+1;
        old_tbl_size = pCur_tbl_header->FWResrcCnt*sizeof(ESRT_ENTRY)+sizeof(ESRT_TABLE_HEADER);
        new_tbl_size = new_ent_cnt*sizeof(ESRT_ENTRY)+ sizeof(ESRT_TABLE_HEADER);
        
        /* Allocate memory for new ESRT entries */
        Status=gBS->AllocatePool(EfiRuntimeServicesData,new_tbl_size,(VOID*)&pNew_tbl_header);
        HANDLE_ERROR_LABEL(Status,gBS->InstallConfigurationTable,QcomInstallEsrtEntryInSysTblExit); 
              
        /*copy the original ESRT table to new ESRT*/
        CopyMem(pNew_tbl_header,(VOID*)pCur_tbl_header,old_tbl_size);
                    
        /*adjust the pointer to last entry*/
        pTemp = (ESRT_ENTRY*)(pNew_tbl_header+1);
        pNew_esrt_ent = (ESRT_ENTRY*)(pTemp + old_ent_cnt);
                    
        /*copy the last entry */
        CopyMem(pNew_esrt_ent,(VOID*)&esrt_ent,sizeof(ESRT_ENTRY));
                    
        /*update the esrt table header*/
        pNew_tbl_header->FWResrcCnt++;
        pNew_tbl_header->FWResrcMax++;
        
        /*swap the ESRT table*/
        pcfg_table->VendorTable =(VOID*) pNew_tbl_header;
                    
        /*Free the original ESRT table*/
        gBS->FreePool(pCur_tbl_header);
        pCur_tbl_header = NULL;
              
        return EFI_SUCCESS;
      }      
    }
    pcfg_table++;
  }

  Status = EFI_NOT_FOUND;

QcomInstallEsrtEntryInSysTblExit:

  if(pNew_tbl_header != NULL)
  {
    gBS->FreePool(pNew_tbl_header);
    pNew_tbl_header = NULL;
  }
  return Status;
}

/**
 * Update ESRT entry in System table.
 * 
 * @param SystemTable - the UEFI System Table
 * 
 * @return BOOLEAN 
 */

BOOLEAN
QcomUpdateEsrtInSystbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_CONFIGURATION_TABLE *pcfg_table      = NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header = NULL;
  ESRT_TABLE_HEADER       *pEntStart       = NULL;
  ESRT_ENTRY              *pCurEnt         = NULL;
  UINTN                    ent_cnt         = 0;
  UINTN                    cs_cnt          = 0;
  UINTN                    i               = 0;
  UINTN                    j               = 0;

  if (SystemTable == NULL)
    return FALSE;

  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
  
  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      pCur_tbl_header=(ESRT_TABLE_HEADER*)pcfg_table->VendorTable;    
      ent_cnt = pCur_tbl_header->FWResrcCnt;
      pEntStart= pCur_tbl_header+1;
      pCurEnt = (ESRT_ENTRY*)(pEntStart);
      for (j=0;j<ent_cnt; j++)
      {
        if(CompareGuid((GUID*)&esrt_ent.FWClass,(GUID*)&pCurEnt->FWClass))
        {
          CopyMem(pCurEnt,(VOID*)&esrt_ent,sizeof(ESRT_ENTRY));
          return TRUE;
        }
        pCurEnt++;
      }      
    }
    pcfg_table++;
  }  
  
  return FALSE;
}

/**
 *  Update the ESRT table to DPP and System Table.
 * 
 * @param SystemTable - the UEFI System Table
 * @param attemptatus - the update attempt status. 
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
QcomUpdateESRT(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS attemptStatus)
{

  EFI_STATUS Status   = EFI_SUCCESS;
  UINTN      esrtSize = 0;

  if (SystemTable == NULL)
    return EFI_INVALID_PARAMETER;

  switch(attemptStatus)
  {
  case FW_LAST_ATMPT_STATUS_SUCCESS:
    esrt_ent.FWVer = PayLoadHeader.FirmwareVersion;
    //Update the lowest supported version only if it's greater than what is currently set
    if(PayLoadHeader.LowestSupportedVersion > esrt_ent.FwLowestSupportedVer )
    {
      esrt_ent.FwLowestSupportedVer = PayLoadHeader.LowestSupportedVersion;
    }
    esrt_ent.LastAttemptVer = PayLoadHeader.FirmwareVersion;
  break;

  case FW_LAST_ATMPT_STATUS_UNSUCCESSFUL:
  case FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE:
  case FW_LAST_ATMPT_STATUS_INCORRECT_VERSION:
  case FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT:
  case FW_LAST_ATMPT_STATUS_AUTHEN_ERROR:
  default:
    esrt_ent.LastAttemptVer = PayLoadHeader.FirmwareVersion;
  break;
  }

  esrt_ent.LastAttemptStatus = (UINT32)attemptStatus;
   
  /*update Systemtable*/
  if(!QcomUpdateEsrtInSystbl(SystemTable))
  {
    return EFI_ABORTED;
  }
      
  /*update ESRT in DPP*/
  esrtSize = sizeof(ESRT_ENTRY);
  Status = QcomWrite_EsrtEntryToStorage(&esrt_ent, esrtSize);

  return Status;
}

/**
 * Check if ESRT Entry is installed
 *
 * @param SystemTable - UEFI System Table
 *
 * @return BOOLEAN
 */  
BOOLEAN
QcomIsEsrtEntryInstalledInSysTbl(
   IN OUT EFI_SYSTEM_TABLE  *SystemTable
   )
{
  EFI_CONFIGURATION_TABLE *pcfg_table      = NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header = NULL;
  ESRT_TABLE_HEADER       *pEntStart       = NULL;
  ESRT_ENTRY              *pCurEnt         = NULL;
  UINTN                    ent_cnt         = 0;
  UINTN                    cs_cnt          = 0;
  UINTN                    i               = 0;
  UINTN                    j               = 0;

  if (SystemTable == NULL)
    return FALSE;
  
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;

  /*Search the ESRT table GUID from System table*/
  for (i=0;i<cs_cnt; i++){
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      pCur_tbl_header=(ESRT_TABLE_HEADER*)pcfg_table->VendorTable;
      ent_cnt = pCur_tbl_header->FWResrcCnt;
      pEntStart = pCur_tbl_header+1;
      pCurEnt = (ESRT_ENTRY*)(pEntStart);

      /*Search the  ESRT entry GUID from ESRT table*/
      for (j=0;j<ent_cnt; j++){
        if(CompareGuid((GUID*)&esrt_ent.FWClass,(GUID*)&pCurEnt->FWClass))
        {
          return TRUE;
        }
        pCurEnt++;
      }      
    }
    pcfg_table++;
  }  
  return FALSE;
}

/**
 *  Find if the given Firmware Resource GUID matches a known type
 *  used for Qualcomm platforms.
 * 
 * @param FwResourceGuid - Pointer to GUID to match against known platform types
 * @param bFound         - Pointer to output boolean set to TRUE if a match is found,
 *                         or FALSE otherwise.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
QcomIsFwGuidKnownPlatformType(
  IN EFI_GUID *FwResourceGuid,
  OUT BOOLEAN *bFound
  )
{
   EFI_STATUS Status     = EFI_SUCCESS;
   UINTN      numFwGuids = 0;
   UINTN      i          = 0;

   if ((FwResourceGuid == NULL) || (bFound == NULL))
   {
      return EFI_INVALID_PARAMETER;
   }

   numFwGuids = sizeof(qcomFwResourceGUIDs)/sizeof(qcomFwResourceGUIDs[0]);

   *bFound = FALSE;

   for (i = 0; i < numFwGuids; i++)
   {
      if (CompareGuid((GUID *)FwResourceGuid, (GUID *)&qcomFwResourceGUIDs[i]) == TRUE)
      {
         *bFound = TRUE;
         break;
      }
   }

   return Status;
}

/**
 *  Find the Firmware Resource GUID for the device platform.
 * 
 * @param FwResourceGuid - Pointer to GUID that is modified to correspond to the current
 *                         platform type.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
QcomGetFwGuidForPlatform(
  OUT EFI_GUID *FwResourceGuid
  )
{
    EFI_STATUS                           Status                = EFI_SUCCESS;
    EFI_CHIPINFO_PROTOCOL               *mChipInfoProtocol     = NULL;
    EFI_PLATFORMINFO_PROTOCOL           *pPlatformInfoProtocol = NULL;
    EFI_PLATFORMINFO_PLATFORM_INFO_TYPE  platformInfo;
    EFIChipInfoIdType                    eId                   = EFICHIPINFO_ID_UNKNOWN;
    EFIChipInfoVersionType               nChipVersion          = 0;

    if (FwResourceGuid == NULL)
    {
       return EFI_INVALID_PARAMETER;
    }

    Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid,
                                  NULL,
                                  (VOID **) &mChipInfoProtocol
                                 );
    HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,QcomGetFwGuidForPlatformExit);

    
    Status = mChipInfoProtocol->GetChipId( mChipInfoProtocol, 
                                                       &eId);
    HANDLE_ERROR_LABEL(Status,mChipInfoProtocol->GetChipId,QcomGetFwGuidForPlatformExit);

    Status = mChipInfoProtocol->GetChipVersion( mChipInfoProtocol, 
                                                   &nChipVersion);
    HANDLE_ERROR_LABEL(Status,mChipInfoProtocol->GetChipVersion,QcomGetFwGuidForPlatformExit);

    // Get the platform info protocol.  It is used to map rows and scan columns to 
    // specific keys which can vary per platform.
    Status = gBS->LocateProtocol ( &gEfiPlatformInfoProtocolGuid, 
                                   NULL, 
                                   (VOID **)&pPlatformInfoProtocol
                                  );
    HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,QcomGetFwGuidForPlatformExit);
    
    ZeroMem(&platformInfo, sizeof(platformInfo));

    Status = pPlatformInfoProtocol->GetPlatformInfo( pPlatformInfoProtocol,
                                                    &platformInfo);
    HANDLE_ERROR_LABEL(Status,pPlatformInfoProtocol->GetPlatformInfo,QcomGetFwGuidForPlatformExit);    
 
	if (eId == EFICHIPINFO_ID_MSM8996)
    {
       /* Detected 8996 */
       if (platformInfo.platform == EFI_PLATFORMINFO_TYPE_MTP_MSM)
       {
          /* 8996 MTP platform */
          DEBUG((EFI_D_WARN,"    Platform: 8996 MTP\n"));
          CopyMem(FwResourceGuid, &qcomFwResourceGUIDs[QCOM_FW_UPDATE_SYSTEM_FW_8996_MTP_TYPE], sizeof(EFI_GUID));
       }
	   else
       {
         /* Unknown 8996 platform */
         DEBUG((EFI_D_WARN,"    Platform: Unknown 8996 platform\n"));
         CopyMem(FwResourceGuid, &qcomFwResourceGUIDs[QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_TYPE], sizeof(EFI_GUID));
       }
    } 
    else
    {
       /* Unknown chipset */
       DEBUG((EFI_D_WARN,"    Platform: Unknown chipset\n"));
       CopyMem(FwResourceGuid, &qcomFwResourceGUIDs[QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_TYPE], sizeof(EFI_GUID));
    }

QcomGetFwGuidForPlatformExit:

    return Status;
}

/**
 * Entry function for ESRT maintainence.
 *
 * @param SystemTable 
 * 
 * @return EFI_STATUS 
 **/
EFI_STATUS
QcomESRTPhaseMain(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS   Status             = EFI_SUCCESS;
  ESRT_ENTRY  *buf                = NULL;
  UINTN        esrtSize           = 0;
  BOOLEAN      bIsQcomFwGuid      = FALSE;
  EFI_GUID     QcomFwPlatformGuid;

  if (SystemTable == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /*Read ESRT entry from DPP*/
  esrtSize = sizeof(ESRT_ENTRY);
  DEBUG((EFI_D_WARN,"\n  Reading System ESRE from DPP...\n"));
  Status = QcomRead_EsrtEntryFromStorage(&buf, &esrtSize);

  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN,"    System ESRT not found.\n"));
    if(buf != NULL)
    {
      gBS->FreePool(buf);
      buf = NULL;
    }
    /* Write the initial DPP ESRT entry using the correct Firmware Resource GUID assigned to the
       given platform type. If it fails getting the platform type, the esrt_ent already contains
       UNKNOWN platform GUID. */
    DEBUG((EFI_D_WARN,"    Detecting platform type\n"));
    if (QcomGetFwGuidForPlatform(&QcomFwPlatformGuid) == EFI_SUCCESS)
    {
       CopyMem(&(esrt_ent.FWClass), &QcomFwPlatformGuid, sizeof(EFI_GUID));
    }
    else
    {
      DEBUG((EFI_D_WARN,"    Set platform type to default\n"));
    }

    Status = QcomWrite_EsrtEntryToStorage(&esrt_ent, esrtSize);
    HANDLE_ERROR_LABEL(Status,QcomWrite_EsrtEntryToStorage,QcomESRTPhaseMainExit);
  }
  else
  {
    /* Check if the Firmware Resource GUID in storage is a known Qualcomm firmware resource GUID. */
    if (QcomIsFwGuidKnownPlatformType(&(buf->FWClass), &bIsQcomFwGuid) == EFI_SUCCESS)
    {
      if (bIsQcomFwGuid == TRUE)
      {
         /* Verify that the Firmware Resource GUID is correct for the platform. */
         if (QcomGetFwGuidForPlatform(&QcomFwPlatformGuid) == EFI_SUCCESS)
         {
            if (CompareGuid((GUID *)&(buf->FWClass), (GUID *)&QcomFwPlatformGuid) == FALSE)
            {
               /* If the storage contains an ESRT firmware resource GUID that is a Qualcomm type,
                  but the type doesn't match the current platform, then reset the GUID to the
                  correct GUID for the platform and write it back to storage. */
               CopyMem(&(buf->FWClass), &QcomFwPlatformGuid, sizeof(EFI_GUID));
               Status = QcomWrite_EsrtEntryToStorage(buf, esrtSize);
               HANDLE_ERROR_LABEL(Status,QcomWrite_EsrtEntryToStorage,QcomESRTPhaseMainExit);
            }
         }
      }
    }
    CopyMem(&esrt_ent,(VOID*)buf,esrtSize);
  }
  
  /*Create the ESRT header if it doesn't exist in System table*/
  if(!QcomIsEsrtHeaderInstalledInSysTbl(SystemTable))
  {
    DEBUG((EFI_D_WARN,"    Installing ESRT header"));
    Status = QcomInstallEsrtHeaderInSysTbl(SystemTable);    
    HANDLE_ERROR_LABEL(Status,QcomInstallEsrtHeaderInSysTbl,QcomESRTPhaseMainExit);
    DEBUG((EFI_D_WARN,"...Done\n"));

  }

  /*Install the ESRT entry if it doesn't exist in ESRT table*/
  if(!QcomIsEsrtEntryInstalledInSysTbl(SystemTable))
  {
    DEBUG((EFI_D_WARN,"    Installing System ESRE"));
    Status = QcomInstallEsrtEntryInSysTbl(SystemTable);
    HANDLE_ERROR_LABEL(Status,QcomInstallEsrtEntryInSysTbl,QcomESRTPhaseMainExit);
    DEBUG((EFI_D_WARN,"...Done\n"));

  }else
  {
  /*Populate updated ESRT table to System table*/
    DEBUG((EFI_D_WARN,"    Updating System ESRE"));
    Status = QcomUpdateEsrtInSystbl(SystemTable);
    HANDLE_ERROR_LABEL(Status,QcomUpdateEsrtInSystbl,QcomESRTPhaseMainExit);
    DEBUG((EFI_D_WARN,"...Done\n"));

  }
 
  
QcomESRTPhaseMainExit:

  if(buf != NULL)
  {
    gBS->FreePool(buf);
    buf = NULL;
  }
  
  return Status;
}



