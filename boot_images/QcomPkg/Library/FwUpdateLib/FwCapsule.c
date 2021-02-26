/** @file FwCapsule.c
   
  Capsule related functions
   
  Maintains a registry of the partitions needed for FwUpdate 
    and provides lookup functions for these partitions and
    their appropriate read/write functions.

  Copyright (c) 2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/02/05    al          Changing TPL_NOTIFY to TPL_CALLBACK
 2012/10/05    mic         Added Data-Driven update support
 2012/08/10    mic         Changed debug message level for critical errors and 
                           updated update attempt status error code
 2012/08/03    mic         Added maximum update attempts support
 2012/07/26    mic         Added configuration table pointer check before capsule
                           processing
 2012/07/25    rs          Added debug message for low battery level
 2012/07/02    Mic         Added debug message support for file node
 2012/06/12    rs          Added support for BreakChangeNumber
 2012/06/05    rs          Added support for displaying UXCapsule 
 2012/05/15    Mic         Added robust capsule/Flag clean logic 
 2012/05/07    rs          Added support for decryption of Boot Firmware
 2012/3/01     MiC         Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Library/FwUpdateLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIEncryption.h>
#include <Pi/PiPeiCis.h>
#include <Library/SecBootSigLib.h>

#include "FwPowerCheck.h"
#include "FwPayloadEntry.h"
#include "FwResourceCheck.h"

extern QPAYLOAD_METADATA_HEADER_V3 PayLoadHeader;
extern FW_LINKED_LIST_TYPE         FwEntryList;

EFI_EVENT QcomSysCapsuleDeleteEvent = NULL;
EFI_EVENT QcomUxCapsuleDeleteEvent = NULL;
extern EFI_GUID  ESRT_guid;


/**
 * This function creates update attempt count file 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS 
FwUpdateCreateAttemptCountFile(
    VOID
)
{
  EFI_HANDLE PartHandle = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  VOID *Buffer = NULL;
  UINTN FileSize = 0;
  
  DEBUG((EFI_D_WARN,"Creating attempt count file in FS\n"));
  Status = FwUpdateGetHandleForPartition( QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &PartHandle);
  if(EFI_ERROR(Status))
  {
    goto FwUpdateCreateAttemptCountFileExit;
  }
  
  FileSize = sizeof(UINT8);

  Status = gBS->AllocatePool (EfiBootServicesData , FileSize ,&Buffer ); 
  if(EFI_ERROR(Status))
  {
    goto FwUpdateCreateAttemptCountFileExit;
  }
  
  gBS->SetMem(Buffer, FileSize, 0);

  Status = FwUpdateWriteFileToFat(PartHandle, &Buffer, QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE, &FileSize );
  if(EFI_ERROR(Status))
  {
    goto FwUpdateCreateAttemptCountFileExit;
  }

FwUpdateCreateAttemptCountFileExit:
  if (Buffer != NULL)
  {
    gBS->FreePool (Buffer);
    Buffer = NULL;
  }
  
  return Status;
}

/** 
* Check if the FwVer from the payload is higher then the lowest supported version
*
* @param	bpass              the check result 
* 
* @return EFI_STATUS
**/

EFI_STATUS
FwUpdateCheckLowestSupportedVersion(
   OUT BOOLEAN  *bPass,
   OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
   OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  ESRT_ENTRY  *ent   = NULL;
    
  if((bPass == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;
  *bPass = FALSE;

  Status = QcomGetEsrtEntry(&ent);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_ESRT_GET_ENTRY;
    HANDLE_ERROR_LABEL(Status,QcomGetEsrtEntry,FwUpdateCheckLowestSupportedVersionExit);
  }

  if (ent != NULL)
  {
    if (PayLoadHeader.FirmwareVersion >= ent->FwLowestSupportedVer)
    {
      *bPass = TRUE;
    }
    else
    {
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INCORRECT_VERSION;
      *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_LOWEST_SUPPORT_VERSION;

      *bPass = FALSE;
    }
  }
  else
  {
    Status = EFI_ABORTED;
  }

FwUpdateCheckLowestSupportedVersionExit:

  return Status;

}

/** 
* Load and run the payload file
*
* @param	Image handle  :   UEFI Image Handle
* @param	SystemTable   :   UEFI system table
* @param	pos           :   the capsule index 
*	
* @return EFI_STATUS
**/

EFI_STATUS
FwUpdateCapsuleLoadPayload(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN UINTN              pos
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_HANDLE              LoadedImageHandle = NULL;
  EFI_CAPSULE_TABLE       *pcst = NULL;
  EFI_CAPSULE_HEADER      *pcsh = NULL;
  EFI_CONFIGURATION_TABLE *pcfg_table = NULL;
  VOID                    *payloadbuf = NULL;
  UINTN                   bufsize = 0;
    
  pcfg_table = SystemTable->ConfigurationTable + pos;	
  pcst =(EFI_CAPSULE_TABLE*)pcfg_table->VendorTable;

  if((pcst != NULL) && (pcst->CapsuleArrayNumber > 0) && (pcst->CapsulePtr[0] != NULL))
  {
    pcsh = pcst->CapsulePtr[0];
    bufsize = pcsh->CapsuleImageSize - pcsh->HeaderSize;
    payloadbuf = (UINT8*)pcsh + pcsh->HeaderSize;
    
    //Switch the LoadImage/StartImage UEFI security library to authenticate with the Platform Key rather than the Allowed DB
    SetOemFwUpdateSignal();
    
    Status = gBS->LoadImage(FALSE,
              ImageHandle,
              NULL,
              payloadbuf,
              bufsize,
              &LoadedImageHandle);
               
    ERR_HANDLE(Status);
         
    Status = gBS->StartImage(LoadedImageHandle,
                  NULL,
                  NULL);

  }
  else
  {
    DEBUG((EFI_D_ERROR,"\nUnable to find system capsule\n"));  
    Status = EFI_NOT_FOUND; 
  }
 
  return Status;
}


/** 
* searching the system table to see if fw capsule is installed
*	
* @param	SystemTable   :  UEFI system table
* @param	pos           :  the capsule index 
*	
* @return BOOLEAN
**/

BOOLEAN
FwUpdateFindCapsule(
  IN EFI_GUID *CapGUID,
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT UINTN             *pos
  )
{

  EFI_CONFIGURATION_TABLE * pcfg_table;
  UINTN i=0;
  UINTN cs_cnt;  

  if (!SystemTable || !CapGUID || !pos)
    return FALSE;
    
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;

  
  /*check GUID installed in system table*/

  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid(CapGUID,(GUID*)&pcfg_table->VendorGuid))
    {
      *pos=i;
      return TRUE;
    }
    pcfg_table++;
  }
 
  return FALSE;
}

/** 
*Does nothing. Just a placeholder
*	
* @param	Event	: Event whose notification function is
*                     being invoked.
* @param	Context : Pointer to the notification function's
*                     context, which is always zero in current
*                     implementation.
* @return   MONE
**/

VOID
EFIAPI
FwUpdateCapsuleEventHandler (
	IN EFI_EVENT		  Event,
	IN VOID 		      *ContextGuardPredicateList
	)
{
	//AsciiPrint("Received capsule event...\n");
}

/** 
*Does nothing. Just a placeholder
*	
* @param	Event	: Event whose notification function is
*                     being invoked.
* @param	Context : Pointer to the notification function's
*                     context, which is always zero in current
*                     implementation.
* @return   MONE
**/

VOID
EFIAPI
FwUpdateUXCapsuleEventHandler (
	IN EFI_EVENT		  Event,
	IN VOID 		      *ContextGuardPredicateList
	)
{
	//AsciiPrint("Received capsule event...\n");
}

/** 
* Register the capsule ID for capsule deletion
*	
* @return EFI_STATUS
**/
 
EFI_STATUS
FwUpdateRegisterCapsuleGUID(
  IN EFI_GUID *CapGUID,
  IN EFI_EVENT *DelEvent
  )
{

  EFI_STATUS Status=EFI_SUCCESS;

  if(!CapGUID || !DelEvent)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->CreateEventEx (
        EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        FwUpdateCapsuleEventHandler,
        NULL,
        CapGUID,
        DelEvent
        );    
  return Status;
  
}
/** 
* Main function for update capsule 
*
* @param ImageHandle        -UEFI imagehandle
* @param SystemTable        -UEFI system table
* @param pos                -Capsule index 
* @param MsftAttemptStatus  -Microsoft last attempt status 
* @param QcResultFlag       -QCFirmwareUpdate result flag
*
* @return EFI_STATUS
**/

EFI_STATUS
FwUpdateCapsulePhaseMain(
  IN  EFI_HANDLE               ImageHandle,
  IN  EFI_SYSTEM_TABLE         *SystemTable,
  IN  UINTN                    pos,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

  )
{
  EFI_STATUS                      Status             = EFI_SUCCESS;
  BOOLEAN                         bVersionGood       = FALSE;
  BOOLEAN                         bPowerGood         = FALSE;
  QCOM_FW_UPDATE_STATE_VARIABLES  currentUpdateState = FW_STATE_NOT_IN_PROGRESS;
  ESRT_ENTRY                     *ent                = NULL;
  UINT8                           AttemptCnt         = 0;

  if((MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = QcomGetEsrtEntry(&ent);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_ESRT_GET_ENTRY;
    HANDLE_ERROR_LABEL(Status,QcomGetEsrtEntry,FwUpdateCapsulePhaseMainExit);
  }

  /*register capsule GUID for capsule deletion */
  Status =FwUpdateRegisterCapsuleGUID(&(ent->FWClass),&QcomSysCapsuleDeleteEvent);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_CAPSULE_REGISTER_SYSTEM_CAPSULE;
    HANDLE_ERROR_LABEL(Status,FwUpdateRegisterCapsuleGUID,FwUpdateCapsulePhaseMainExit);
  }

  /*load payload to memory*/
  Status = FwUpdateCapsuleLoadPayload(ImageHandle,SystemTable,pos);
  if (EFI_ERROR(Status))
  {
    if( Status == EFI_SECURITY_VIOLATION)
    {    
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_AUTHEN_ERROR;
      *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_AUTHENTICATION;
      DEBUG_ERR_LABEL("\nFwUpdate Failed: Payload authentication error\n",FwUpdateCapsulePhaseMainExit);
    }
    else if ( (Status == EFI_LOAD_ERROR) || (Status == EFI_UNSUPPORTED))
    {
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
      *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_IMG_FORMAT;
      DEBUG_ERR_LABEL("\nFwUpdate Failed: Invalid payload image format error\n",FwUpdateCapsulePhaseMainExit);
    }
    else
    {    
      *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_LOADING;
      DEBUG_ERR_LABEL("\nFwUpdate Failed: Load payload error\n",FwUpdateCapsulePhaseMainExit);

    }
  }

  /*Update handle registry with loaded payload infomation*/
  Status = UpdateHandleRegistry(SystemTable);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_HANDLE_REGISTRY;
    HANDLE_ERROR_LABEL(Status,UpdateHandleRegistry,FwUpdateCapsulePhaseMainExit);

  }

  /*Parsing payload metadata*/
  Status = FwPayloadParserMain(MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwUpdateCapsulePhaseMainExit);
  
  /*If no FwEntry found, return success*/
  if (FwEntryList.Count== 0)
  {
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;
    *QcResultFlag = QCFW_UPDATE_SUCCESS;
      return EFI_SUCCESS;
  }

  /*Resource availability checking*/
  Status = FwUpdateResourceCheckMain(MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwUpdateResourceCheckMain,FwUpdateCapsulePhaseMainExit);

  /*Check if the payload version is in the boundary of lowest supported version*/
  Status = FwUpdateCheckLowestSupportedVersion(&bVersionGood,MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwUpdateCheckLowestSupportedVersion,FwUpdateCapsulePhaseMainExit);

  if(!bVersionGood) 
  {     
    Status = EFI_UNSUPPORTED;
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INCORRECT_VERSION;
    *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_LOWEST_SUPPORT_VERSION;
    DEBUG_ERR_LABEL("\nFwUpdate Failed: Incorrect version error\n",FwUpdateCapsulePhaseMainExit);
  }

  /*Check if the battery power level is sufficient for capsule update only
     if update is not already in-progress.  This allows an in-progress
     update to complete if the power level was on the boundary at the beginning
     of the update and a power cycle occurred which could prevent the
     update from completing. */
     
  Status = GetState(&currentUpdateState);
  if (EFI_ERROR(Status)) 
  {
    *QcResultFlag = QCFW_UPDATE_ERR_FLAG_GET_STAMACHINE_STATE;
    HANDLE_ERROR_LABEL(Status,GetState,FwUpdateCapsulePhaseMainExit);
  }
  
  if (currentUpdateState == FW_STATE_NOT_IN_PROGRESS)
  {
    /* LastAttemptStatus is set to an appropriate power event type if 
       bPowerGood is FALSE and the function returns success. */
    Status = FwUpdateCheckPowerLevel(&bPowerGood, MsftAttemptStatus,QcResultFlag);
    if (EFI_ERROR(Status))
    {
      /* Hard-code an error value of insufficient battery if failure retrieving power level. */
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY;
      *QcResultFlag = QCFW_UPDATE_ERR_RESOUCE_POWER_INSUFFICEIENT_BATTERY; 
      HANDLE_ERROR_LABEL(Status,FwUpdateCheckPowerLevel,FwUpdateCapsulePhaseMainExit);
 
    }
    else if (bPowerGood == FALSE)
    { 
      DEBUG_ERR_LABEL("\nFwUpdate Failed: Insufficient battery.\n",FwUpdateCapsulePhaseMainExit);
    }
  }

  Status = GetAttemptCount(&AttemptCnt);
  if (EFI_ERROR(Status)) 
  {
    DEBUG((EFI_D_WARN,"Failed to get attempt count in FS\n"));
    if (Status == EFI_NOT_FOUND)
    {
      Status = FwUpdateCreateAttemptCountFile();
      //We want update to continue if FwUpdateCreateAttemptCountFile failed.
      HANDLE_ERR_CONTINUE(Status,FwUpdateCreateAttemptCountFile);
    }
    AttemptCnt = 0;
  }
  if (AttemptCnt <= UPDATE_ATTEMPT_COUNT_MAX )
  {  
    AttemptCnt++;
  } 
  DEBUG((EFI_D_WARN,"Current update attempt count is %d\n", AttemptCnt));
  Status = UpdateAttemptCountInFS(AttemptCnt);
  HANDLE_ERR_CONTINUE(Status,UpdateAttemptCountInFS);
 
  /*State machine function for firmware update*/
  Status = FwUpdatePhaseMain(SystemTable,MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwUpdatePhaseMain,FwUpdateCapsulePhaseMainExit);
  
  return EFI_SUCCESS;

FwUpdateCapsulePhaseMainExit:

  
  return Status;
}

/** 
* Search the system table to see if any pending capsule exists
*	
* @param	SystemTable   :  UEFI system table
* @param	CapsuleFound  :  Pending capsule found or not
*	
* @return EFI_STATUS
**/

EFI_STATUS
FwUpdateFindPendingCapsule(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT BOOLEAN          *CapsuleFound
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_CONFIGURATION_TABLE *pcfg_table=NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header=NULL,*pEntStart=NULL;
  ESRT_ENTRY              *pCurEnt=NULL;
  UINTN                   ent_cnt=0, cs_cnt=0,i=0,j=0;
  UINTN                   pos=0;

  if (!SystemTable || !CapsuleFound)
    return EFI_INVALID_PARAMETER;

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
          if(FwUpdateFindCapsule((GUID*)&(pCurEnt->FWClass),SystemTable,&pos))
          {
              //Capsule GUID from ESRT was found in SystemTable
              //Pending update capsule exists
              *CapsuleFound = TRUE;
              return EFI_SUCCESS;
          }
          pCurEnt++;
      }			
    }
    pcfg_table++;
  }	

  *CapsuleFound = FALSE;
  return Status;

}
