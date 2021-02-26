/** @file FwUpdatePhaseLib.c
   
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/03/22     mic         Added Fwupdate support for MSM8994 LiQUID only
 2015/02/05     al          Changing TPL_NOTIFY to TPL_CALLBACK
 2014/12/08     mic         Removed FwGetOsType API
 2013/06/28     niting      Added WARN macro to prevent message in PRODMODE
 2013/01/26     mic         Added workaround for authentication protocol issue
 2012/11/30     mic         Added ESRT entry print 
 2012/11/15     mic         Fixed a corner case issue on GPT table switching 
 2012/10/10     jd          Handle GPT switch based on firmware entry list  
 2012/10/05     mic         Added Data-Driven update support
 2012/08/10     mic         Changed debug message level for critical errors
 2012/08/03     mic         Added maximum update attempts support
 2012/07/26     mic         Added configuration table pointer check before capsule
                                     processing
 2012/07/17     mic         Added ToolSupportProtocol for OPM handshake and removed 
                                     ToolSupportLib
 2012/07/13     mic         Added WinSecApp OPM handshake support
 2012/06/25     mic         Added support for OPM keys update          
 2012/06/21     rs          Adding support for FPDT ACPI table
 2012/06/18     jd          rename OEM_UPDATE_EVENT_GUID 
 2012/06/12     rs          Added support for BreakChangeNumber 
 2012/06/05     rs          Added support for UXCapsule
 2012/06/04     rs          Replaced AsciiPrint with DEBUG macro 
 2012/05/15     mic         Added robustness of the reset logic
 2012/05/07     rs          Added support for decryption of Boot Firmware
 2012/05/03     mic         Initial version

=============================================================================*/

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Pi/PiPeiCis.h>
#include <Library/QcomLib.h>
#include <Include/Library/FwCommonLib.h>
#include <Include/Library/FwUpdateLib.h>
#include <Library/SecBootSigLib.h>
#include <Protocol/EFIDpp.h>
#include <IndustryStandard/Acpi50.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/EFIEncryption.h>
#include <Protocol/EFIToolSupport.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPlatformInfo.h>
#include "FwCapsule.h"
#include "FwEsrt.h"
#include "FwPayloadEntry.h"
#include "FwResourceCheck.h"
#include "FwEntryIO_helpers.h"

/*extern variable*/
extern EFI_EVENT                      QcomSysCapsuleDeleteEvent;
extern EFI_EVENT                      QcomUxCapsuleDeleteEvent;
extern EFI_GUID                       UXCapGuid;
extern QPAYLOAD_METADATA_HEADER_V3    PayLoadHeader;
extern BOOLEAN                        bDoubleUpdateNeeded;
extern FW_LINKED_LIST_TYPE            FwEntryList;


/*Static variable*/
static QCOM_FW_UPDATE_STATE_VARIABLES State ;

/*global variable*/
QCOM_FW_UPDATE_MODE                   FWupdateMode                  = QCOM_FW_UPDATE_MODE_NOUPDATE;
EFI_GUID                              UXCapOEMGuid                  = QCOM_FW_UPDATE_OEM_UPDATE_EVENT_GUID;
EFI_EVENT                             QcomUxCapsuleOemUpdateEvent;
UX_CAPSULE_PAYLOAD                    *UxCapsulePayload             = NULL;
UINT8                                 *UXCapsuleImageAddress        = NULL;
UINT32                                UXCapsuleImageSize            = 0;

/**
 * Function to delete capsule or Qpayload.efi
 * 
 * @param QcResultFlag  -update result
 *
 * return EFI_STATUS
 */
EFI_STATUS
FwUpdateClearFlagOrCapsule (
  IN QCFW_UPDATE_FLAG_VALUE QcResultFlag
  )
{
  EFI_STATUS Status                = EFI_SUCCESS;
  UINT8      UpdateCommitFlagValue = QCFW_UPDATE_INIT;

  switch(FWupdateMode)
  {

  case QCOM_FW_UPDATE_MODE_QCFW:
  
    UpdateCommitFlagValue = (UINT8) QcResultFlag;
    
    Status = SetFlag(QCOM_FW_UPDATE_LEGACY_UPDATE_COMMIT_FLAG, (VOID*)&UpdateCommitFlagValue );
    HANDLE_ERROR_LABEL(Status,SetFlag,FwUpdateClearFlagOrCapsuleExit);

    Status = DeleteFwFile2(QCOM_FW_UPDATE_PAYLOAD_APP_FILE_PATH,QCOM_FW_UPDATE_PARTITION_USER_ESP);
    HANDLE_ERROR_LABEL(Status,DeleteFwFile2,FwUpdateClearFlagOrCapsuleExit);

    break;

  case QCOM_FW_UPDATE_MODE_CAPSULE:
    
    Status = gBS->SignalEvent(QcomSysCapsuleDeleteEvent);
    HANDLE_ERROR_LABEL(Status,DeleteFwFile2,FwUpdateClearFlagOrCapsuleExit);

    break;

  case QCOM_FW_UPDATE_MODE_NOUPDATE:
  default:
    break;
  }

FwUpdateClearFlagOrCapsuleExit:

  FWupdateMode = QCOM_FW_UPDATE_MODE_NOUPDATE;
  return Status ;
}

/**
 *  Update ESRT  to both DPP and System Table and delete capsule.
 * 
 * @param SystemTable - System table
 * @param attemptatus - the update attempt status. 
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateDone(
  IN EFI_SYSTEM_TABLE         *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS attemptStatus,
  IN QCFW_UPDATE_FLAG_VALUE   QcResultValue
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  Status = QcomUpdateESRT(SystemTable,attemptStatus);
  //We want FupdateDone to continue if updating ESRT failed. 
  HANDLE_ERR_CONTINUE(Status,QcomUpdateESRT);
 
  Status = UpdateAttemptCountInFS(0);
  //We want FupdateDone to continue if UpdateAttemptCountInFS failed.
  HANDLE_ERR_CONTINUE(Status,UpdateAttemptCountInFS);
  
  /*delete capsule or update commint flag and delete Qpayload*/
  Status = FwUpdateClearFlagOrCapsule(QcResultValue);
  HANDLE_ERROR_LABEL(Status,FwUpdateClearFlagOrCapsule,FwUpdateDoneExit);
  
  return EFI_SUCCESS;

FwUpdateDoneExit:

  return Status;
}

/**
 *  Function to backup FW entries in FwEntry linked list
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS 
StateFwEntryBackupStart (
   OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
   OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
   )
{
  EFI_STATUS          Status        = EFI_SUCCESS;
  UINTN               NodeCount     = 0;
  FW_LIST_NODE_TYPE   *Node         = NULL;
  QPAYLOAD_FW_ENTRY   *pFwEntry      = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList  = NULL;
  UINTN               i             = 0;
   
  if((MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,StateFwEntryBackupStartExit);

  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,StateFwEntryBackupStartExit);

  if (Node == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,StateFwEntryBackupStartExit);

  // Backup BOOT and GPP GPT to FAT
  Status = CopyPartitionTabletoFat16(TRUE, TRUE);
  HANDLE_ERROR_LABEL(Status,CopyPartitionTabletoFat16,StateFwEntryBackupStartExit);

  for (i = 0; i < NodeCount; i++)
  {
    DEBUG((EFI_D_WARN,"    Backing up %d/%d FwEntry...\n",i+1,NodeCount));
    if (Node != NULL)
    {
      pFwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;
      if (pFwEntry != NULL)
      {
        Status = FwEntryBackup(pFwEntry);
        if(EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR,"      Backup failed\n\n"));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"      Backup Success\n\n"));
        }

        HANDLE_ERROR_LABEL(Status,FwEntryBackup,StateFwEntryBackupStartExit);
        
      }
      else
      {
        Status = EFI_INVALID_PARAMETER;
        *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_NULL_ENTRY;
        DEBUG_ERR_LABEL("ListNode->Data == NULL ",StateFwEntryBackupStartExit);
      }
      Node = Node->Next;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      *QcResultFlag = QCFW_UPDATE_ERR_LIST_NULL_NODE;
      DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryBackupStartExit);
    }
  }
 
  Status = UpdateStateinFS(FW_STATE_BACKUP_DONE);
  HANDLE_ERROR_LABEL(Status,UpdateStateinFS,StateFwEntryBackupStartExit);

  return EFI_SUCCESS;
 
StateFwEntryBackupStartExit:
 
  FwPrintFwEntry(pFwEntry,EFI_D_ERROR);

 return Status;
}

/**
 *  Function to update FW entries in FwEntry linked list
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS 
StateFwEntryUpdateStart(
   OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
   OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
)
{
  GPT_TABLE_STATE      BOOT_GPT_STATE = ORIGINAL_TABLE;
  GPT_TABLE_STATE      GPP_GPT_STATE  = ORIGINAL_TABLE;
  EFI_STATUS           Status         = EFI_SUCCESS;
  UINTN                NodeCount      = 0;
  FW_LIST_NODE_TYPE   *Node           = NULL;
  QPAYLOAD_FW_ENTRY   *pFwEntry       = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList   = NULL;
  UINTN                i              = 0;
  BOOLEAN              SwitchBootGpt  = FALSE;
  BOOLEAN              SwitchGppGpt   = FALSE;
  EFI_GUID             Emmc_BOOT_GUID = EFI_EMMC_BOOT_PARTITION1_GUID;
  EFI_GUID             Emmc_GPP_GUID  = EFI_EMMC_GPP_PARTITION1_GUID;

  if((MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,StateFwEntryUpdateStartExit);

  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,StateFwEntryUpdateStartExit);

  if (Node == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_NULL_NODE;
    DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryUpdateStartExit);
  }

  Status = LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,StateFwEntryUpdateStartExit);

  // Check if need to backup BOOT/GPP GPT
  Status = GetAllSwitchGptRequestsFromFwEntryList(FwEntryList, &SwitchBootGpt, &SwitchGppGpt);
  HANDLE_ERROR_LABEL(Status,GetAllSwitchGptRequestsFromFwEntryList,StateFwEntryUpdateStartExit);

  // We might have encountered a reset hence first check if we have the right
  // backup GPT table if not correct it and then invalidate it

  // Only do this is GPT switch is needed
  if (SwitchBootGpt)
  {
    // At this point, GPT should be backup to FAT16 already in previous backup phase.
    Status = CmpGPTTable(BACKUP_GPT_TABLE, Emmc_BOOT_GUID, &BOOT_GPT_STATE);
    HANDLE_ERROR_LABEL(Status,CmpGPTTable,StateFwEntryUpdateStartExit);
  }

  if (SwitchGppGpt)
  {
    // At this point, GPT should be backup to FAT16 already in previous backup phase.
    Status = CmpGPTTable(BACKUP_GPT_TABLE, Emmc_GPP_GUID, &GPP_GPT_STATE);
    HANDLE_ERROR_LABEL(Status,CmpGPTTable,StateFwEntryUpdateStartExit);
  }
      
  if((BOOT_GPT_STATE == ORIGINAL_TABLE) && (SwitchBootGpt == TRUE)) 
  {
    Status = ModifyAllBackupGptTablesForFailSafeUpdate(FwEntryList, 
                                                       SwitchBootGpt, 
                                                       FALSE);
    HANDLE_ERROR_LABEL(Status,ModifyAllBackupGptTablesForFailSafeUpdate,StateFwEntryUpdateStartExit);
  }

  if ((GPP_GPT_STATE == ORIGINAL_TABLE) && (SwitchGppGpt == TRUE))
  {
    Status = ModifyAllBackupGptTablesForFailSafeUpdate(FwEntryList, 
                                                       FALSE,
                                                       SwitchGppGpt) ;
    HANDLE_ERROR_LABEL(Status,ModifyAllBackupGptTablesForFailSafeUpdate,StateFwEntryUpdateStartExit);
  }

  // Invalidate primary GPT if needed
  Status = InvalidateAllPrimaryGPT(SwitchBootGpt, SwitchGppGpt);
  HANDLE_ERROR_LABEL(Status,InvalidateAllPrimaryGPT,StateFwEntryUpdateStartExit);

  for (i = 0; i < NodeCount; i++)
  {
    DEBUG((EFI_D_WARN,"    Updating %d/%d FwEntry...\n",i+1,NodeCount));
    if (Node != NULL)
    {
      pFwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;
      if (pFwEntry != NULL)
      { 
        Status = FwEntryUpdate(pFwEntry);
        if(EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR,"      Update failed\n\n"));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"      Update Success\n\n"));
        }
        HANDLE_ERROR_LABEL(Status,FwEntryUpdate,StateFwEntryUpdateStartExit);
      }
      else
      {
        Status = EFI_INVALID_PARAMETER;
        *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_NULL_ENTRY;
        DEBUG_ERR_LABEL("ListNode->Data == NULL ",StateFwEntryUpdateStartExit);
      }
      Node = Node->Next;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      *QcResultFlag = QCFW_UPDATE_ERR_LIST_NULL_NODE;
      DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryUpdateStartExit);
    }
  }
 
  Status = UpdateStateinFS(FW_STATE_UPDATE_SUCCESS);
  HANDLE_ERROR_LABEL(Status,UpdateStateinFS,StateFwEntryUpdateStartExit);

  return EFI_SUCCESS;

StateFwEntryUpdateStartExit:

  FwPrintFwEntry(pFwEntry,EFI_D_ERROR);

  return Status;
}
/**
 *  Function to update Flags/State in post update done state
 * 
 * @Param SystemTable  - System table
 *
 * @return EFI_STATUS 
 */

EFI_STATUS 
StateFwEntryPostUpdateStart(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  )
{
  EFI_STATUS           Status         = EFI_SUCCESS;
  FW_LINKED_LIST_TYPE *FwEntryList    = NULL;
  UINTN                NodeCount      = 0;
  FW_LIST_NODE_TYPE   *Node          = NULL;
  BOOLEAN              RestoreBootGpt = FALSE;
  BOOLEAN              RestoreGppGpt  = FALSE;
  UINTN                i              = 0;
  QPAYLOAD_FW_ENTRY   *pFwEntry       = NULL;


  if((SystemTable == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,StateFwEntryPostUpdateStartExit);

  Status = GetAllSwitchGptRequestsFromFwEntryList(FwEntryList, &RestoreBootGpt, &RestoreGppGpt);
  HANDLE_ERROR_LABEL(Status,GetAllSwitchGptRequestsFromFwEntryList,StateFwEntryPostUpdateStartExit);

  // We could have come back to this state after a reboot, hence we need to verify
  // the state of the primary table and if is switched shows UEFI copied backup 
  // table to primary

  Status = RestoreAllGPTTablesFromFat(PRIMARY_GPT_TABLE, RestoreBootGpt, RestoreGppGpt);
  HANDLE_ERROR_LABEL(Status,RestoreAllGPTTablesFromFat,StateFwEntryPostUpdateStartExit);

  Status = RestoreAllGPTTablesFromFat(BACKUP_GPT_TABLE, RestoreBootGpt, RestoreGppGpt);
  HANDLE_ERROR_LABEL(Status,RestoreAllGPTTablesFromFat,StateFwEntryPostUpdateStartExit);

  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,StateFwEntryPostUpdateStartExit);
  
  if (Node == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_NULL_NODE;
    DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryPostUpdateStartExit);
  }
  
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,StateFwEntryPostUpdateStartExit);

  for (i = 0; i < NodeCount; i++)
  {
    DEBUG((EFI_D_WARN,"    Deleting %d/%d FwEntry in backup device path...\n",i+1,NodeCount));

    if (Node != NULL)
    {
      pFwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;
      if (pFwEntry != NULL)
      { 
        Status = FwEntryDeleteBackup(pFwEntry);
        if(EFI_ERROR(Status))
        {
          FwPrintFwEntry(pFwEntry, EFI_D_WARN);
          DEBUG((EFI_D_ERROR,"      Delete failed...Continue\n\n"));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"      Delete Success\n\n"));
        }
        HANDLE_ERR_CONTINUE(Status,FwEntryDeleteBackup);
      }
      else
      {
        DEBUG((EFI_D_ERROR,"  Can not delete a NULL FwEntry\n\n"));

      }
      Node = Node->Next;
    }
    else
    {
      DEBUG((EFI_D_ERROR,"  Can not delete a NULL Node\n\n"));
      break;
    }
  }

  Status = UpdateStateinFS(FW_STATE_NOT_IN_PROGRESS);
  HANDLE_ERROR_LABEL(Status,UpdateStateinFS,StateFwEntryPostUpdateStartExit);

  //Write the new BreakingChangeNumber to FS
  Status = SetBreakingChangeNumber(PayLoadHeader.BreakingChangeNumber);
  
  if(Status != EFI_SUCCESS)
  {
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;
    *QcResultFlag = QCFW_UPDATE_SUCCESS;
    HANDLE_ERR_CONTINUE(Status,SetBreakingChangeNumber);
  }
  else if(bDoubleUpdateNeeded)
  {
    //This is a Breaking Change Case. A double-update is needed. Do not clear the commit flag
      
    /*System might have up to 6 attempts after resetting the attempt count, but we still want to reset it since
    the new UEFI.mbn in the new payload may interpret the counter differently.*/
    Status = UpdateAttemptCountInFS(0);
    HANDLE_ERR_CONTINUE(Status,UpdateAttemptCountInFS);

    // System Reset is still required if UpdateAttemptCountInFs failed.
    //Force Reset here.
    DEBUG((EFI_D_WARN,"\n\nDouble update required. Resetting System...\n\n"));
    gRT->ResetSystem(EfiResetCold,
                     EFI_SUCCESS,
                     0,
                     NULL) ;
  }
  else
  {
    //This is NOT a Breaking Change Case. A double-update is NOT needed. Clear the commit flag.
    /*Clear flag*/
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;
    *QcResultFlag = QCFW_UPDATE_SUCCESS;
  }

  return EFI_SUCCESS;

StateFwEntryPostUpdateStartExit:

    return Status;
}

/**
 *  Function to Rollback all FW entries
 * 
 * @Param SystemTable  - System table
 *
 * @return EFI_STATUS 
 */
EFI_STATUS 
StateFwEntryRollbackAllStart(  
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS          Status       = EFI_SUCCESS;
  UINTN               i            = 0;
  UINTN               NodeCount    = 0;
  FW_LIST_NODE_TYPE   *Node        = NULL;
  QPAYLOAD_FW_ENTRY   *pFwEntry     = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList = NULL;
  
  if(SystemTable == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,StateFwEntryRollbackAllStartExit);

  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,StateFwEntryRollbackAllStartExit);
 
  if (Node == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryRollbackAllStartExit);
  }

  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,StateFwEntryRollbackAllStartExit);

  for (i = 0; i < NodeCount; i++)
  {
    if (Node != NULL)
    {
      DEBUG((EFI_D_ERROR,"Rollingback %d/%d FwEntry..\n",i+1,NodeCount));
      pFwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;

      if (pFwEntry != NULL)
      {   
        Status = FwEntryRollback(pFwEntry);
        if(EFI_ERROR(Status))
        {
          FwPrintFwEntry(pFwEntry, EFI_D_ERROR);
          DEBUG((EFI_D_ERROR,"  Rollback failed\n\n"));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"  Rollback Success\n\n"));
        }
        HANDLE_ERR_CONTINUE(Status,FwEntryRollback);
      }
      else
      {
        DEBUG((EFI_D_ERROR,"Node->Data == NULL\n"));
      }
      Node = Node->Next;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      DEBUG_ERR_LABEL("ListNode == NULL ",StateFwEntryRollbackAllStartExit);
    }
  }
  Status = UpdateStateinFS(FW_STATE_ROLLBACK_DONE);
  /*We want rollback to continue if UpdateStateinFS failed*/
  HANDLE_ERR_CONTINUE(Status,UpdateStateinFS);

  return EFI_SUCCESS;

StateFwEntryRollbackAllStartExit:  
 
  FwPrintFwList(FwEntryList,EFI_D_ERROR);
  
  return Status;
        
}

/**
 *  Function to Rollback all FW entries
 * 
 * @Param SystemTable  - System table
 *
 * @return EFI_STATUS 
 */
EFI_STATUS 
StateFwEntryPostRollbackStart(  
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS          Status        = EFI_SUCCESS;
  FW_LINKED_LIST_TYPE *FwEntryList  = NULL;
  BOOLEAN             SwitchBootGpt = FALSE;
  BOOLEAN             SwitchGppGpt  = FALSE;
  UINTN               NodeCount     = 0;
  QPAYLOAD_FW_ENTRY   *pFwEntry     = NULL;
  FW_LIST_NODE_TYPE   *Node         = NULL;
  UINTN               i             = 0;

  if(SystemTable == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERR_CONTINUE(Status,FwGetFwEntryList);

  // Check if GPT tables has been switched  
  Status = GetAllSwitchGptRequestsFromFwEntryList(FwEntryList, &SwitchBootGpt, &SwitchGppGpt);
  HANDLE_ERROR_LABEL(Status,GetAllSwitchGptRequestsFromFwEntryList,StateFwEntryPostRollbackStartExit);

  Status = RestoreAllGPTTablesFromFat(PRIMARY_GPT_TABLE, SwitchBootGpt, SwitchGppGpt);
  HANDLE_ERR_CONTINUE(Status,RestoreAllGPTTablesFromFat);

  Status = RestoreAllGPTTablesFromFat(BACKUP_GPT_TABLE, SwitchBootGpt, SwitchGppGpt);
  HANDLE_ERR_CONTINUE(Status,RestoreAllGPTTablesFromFat);

  for (i = 0; i < NodeCount; i++)
  {
    DEBUG((EFI_D_WARN,"  Deleting %d/%d FwEntry in rollback path...\n",i+1,NodeCount));

    if (Node != NULL)
    {
      pFwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;
      if (pFwEntry != NULL)
      { 
        FwPrintFwEntry(pFwEntry, EFI_D_WARN);
        Status = FwEntryDeleteBackup(pFwEntry);
        if(EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR,"    Delete failed...Continue\n\n"));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"    Delete Success\n\n"));
        }
        HANDLE_ERR_CONTINUE(Status,FwEntryDeleteBackup);
      }
      else
      {
        DEBUG((EFI_D_ERROR,"  Can not delete a NULL FwEntry\n\n"));

      }
      Node = Node->Next;
    }
    else
    {
      DEBUG((EFI_D_ERROR,"  Can not delete a NULL Node\n\n"));
      break;
    }
  }

  Status = UpdateStateinFS(FW_STATE_NOT_IN_PROGRESS);
  /*We want post rollback to continue if UpdateStateinFS failed*/
  HANDLE_ERR_CONTINUE(Status,UpdateStateinFS);

StateFwEntryPostRollbackStartExit:  
  
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
FwUpdatePhaseMain (
  IN EFI_SYSTEM_TABLE          *SystemTable,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

  )
{
  EFI_STATUS Status = FW_STATE_NOT_IN_PROGRESS;
  UINT8 AttemptCnt = 0;
          
  if((SystemTable == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = UpdateHandleRegistry (SystemTable); 
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_HANDLE_REGISTRY;
    HANDLE_ERROR_LABEL(Status,UpdateHandleRegistry,FwUpdatePhaseMainExit);
  }
           
  while (FWupdateMode != QCOM_FW_UPDATE_MODE_NOUPDATE) 
  {
    Status = GetState(&State) ;
    if (EFI_ERROR(Status)) 
    {
      *QcResultFlag = QCFW_UPDATE_ERR_FLAG_GET_STAMACHINE_STATE;
      HANDLE_ERROR_LABEL(Status,GetState,FwUpdatePhaseMainExit);
    }
    
    Status = GetAttemptCount(&AttemptCnt);
    if (EFI_ERROR(Status)) 
    {
      AttemptCnt = 0; 
    }

    /* If update is in progress then need to perform a roll-back to abort if too many attempts have been made.*/
    if (AttemptCnt > UPDATE_ATTEMPT_COUNT_MAX) 
    {
      *QcResultFlag = QCFW_UPDATE_ERR_FLAG_STATE_EXCEEDED_UPDATE_ATTEMPT_MAX;
      if (State == FW_STATE_NOT_IN_PROGRESS)
      {
        /*Rollback is not required, */
        DEBUG_ERR_LABEL("\nAborting. Reached update attempt limit.\n",FwUpdatePhaseMainExit);
      }
      else
      {
        /*Needs to rollback all*/
        DEBUG((EFI_D_WARN,"\nAborting. Reached update attempt limit. Current update attempt count is %d\n\n", AttemptCnt));
        State = FW_STATE_UPDATE_FAIL;
        Status = UpdateStateinFS(FW_STATE_UPDATE_FAIL);
        HANDLE_ERROR_LABEL(Status,UpdateStateinFS,FwUpdatePhaseMainExit);
      }      
    }
    
    switch (State) 
    {
    case FW_STATE_NOT_IN_PROGRESS:
      DEBUG((EFI_D_WARN,"\n  Phase 1: FwEntry Backup start\n\n"));
      Status = StateFwEntryBackupStart(MsftAttemptStatus,QcResultFlag);
      HANDLE_ERROR_LABEL(Status,StateFwEntryBackupStart,FwUpdatePhaseMainExit);
      DEBUG((EFI_D_WARN,"\n  Phase 1 Done\n"));
      break;
                
    case FW_STATE_BACKUP_DONE:
      DEBUG((EFI_D_WARN,"\n  Phase 2: FwEntry Update start\n\n"));
      Status = StateFwEntryUpdateStart(MsftAttemptStatus,QcResultFlag);
      HANDLE_ERR_CONTINUE(Status,StateFwEntryUpdateStart);
      if (EFI_ERROR(Status)) 
      {
        Status =UpdateStateinFS(FW_STATE_UPDATE_FAIL);
        HANDLE_ERROR_LABEL(Status,UpdateStateinFS,FwUpdatePhaseMainExit);
      }   
      DEBUG((EFI_D_WARN,"\n  Phase 2 Done\n"));           
      break;

    case FW_STATE_UPDATE_SUCCESS:
      DEBUG((EFI_D_WARN,"\n  Phase 3: FwEntry Post Update start\n\n"));
      Status = StateFwEntryPostUpdateStart(SystemTable,MsftAttemptStatus,QcResultFlag);
      HANDLE_ERR_CONTINUE(Status,StateFwEntryUpdateStart);
      if (EFI_ERROR(Status))
      {
        Status =UpdateStateinFS(FW_STATE_UPDATE_FAIL);
        HANDLE_ERROR_LABEL(Status,UpdateStateinFS,FwUpdatePhaseMainExit);
      }
      DEBUG((EFI_D_WARN,"\n  Phase 3 Done\n"));

      return EFI_SUCCESS;

    case FW_STATE_UPDATE_FAIL: 
      DEBUG((EFI_D_WARN,"\n  Error Phase 1 : FwEntry rollback start\n\n"));
      Status = StateFwEntryRollbackAllStart(SystemTable);
      if (EFI_ERROR(Status))
      {
        Status =UpdateStateinFS(FW_STATE_ROLLBACK_DONE);
        HANDLE_ERROR_LABEL(Status,UpdateStateinFS,FwUpdatePhaseMainExit);
      }
      DEBUG((EFI_D_WARN,"\n  Error Phase1 Done\n"));
      break;

    case FW_STATE_ROLLBACK_DONE:
      DEBUG((EFI_D_WARN,"\n  Error Phase 2:  Post rollback start\n\n"));
      Status = StateFwEntryPostRollbackStart(SystemTable);
      HANDLE_ERROR_LABEL(Status,StateFwEntryPostRollbackStart,FwUpdatePhaseMainExit);
      DEBUG((EFI_D_WARN,"\n  Error Phase 2 Done\n"));

      return EFI_SUCCESS;

    default:
      return EFI_LOAD_ERROR ;
    }
  }

FwUpdatePhaseMainExit:

  return Status;
}

/**
 *  run the payload Deployment App for QCOM WOA Firmware Update.
 *
 * @param ImageHandle        - The firmware allocated handle for the EFI image.  
 * @param SystemTable        - A pointer to the EFI System Table.
 * @param MsftAttemptStatus  - Microsoft last attempt status 
 * @param QcResultFlag       - QCFirmwareUpdate result flag

 * @retval EFI_STATUS 
 */

EFI_STATUS
FwUpdateRunPayloadApp(
  IN EFI_HANDLE                ImageHandle,
  IN EFI_SYSTEM_TABLE          *SystemTable,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  )
{
  EFI_STATUS      Status            = EFI_SUCCESS;
  EFI_HANDLE      LoadedImageHandle = NULL;
  EFI_DEVICE_PATH *DevPath          = NULL;

  if((SystemTable == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  Status = FwUpdateGetFilePathDevPath(QCOM_FW_UPDATE_PAYLOAD_APP_FILE,
                                      QCOM_FW_UPDATE_PARTITION_USER_ESP,
                                      &DevPath
                                      );
  HANDLE_ERROR_LABEL(Status,FwUpdateGetFilePathDevPath,FwUpdateRunPayloadAppExit);

  //Switch the LoadImage/StartImage UEFI security library to authenticate with the Platform Key rather than the Allowed DB
  SetOemFwUpdateSignal();

  Status = gBS->LoadImage(FALSE,
                          ImageHandle,
                          DevPath,
                          NULL,
                          0,
                          &LoadedImageHandle);
  HANDLE_ERROR_LABEL(Status,gBS->LoadImage,FwUpdateRunPayloadAppExit);


  Status = gBS->StartImage(LoadedImageHandle,
                           NULL,
                           NULL);
  HANDLE_ERROR_LABEL(Status,gBS->StartImage,FwUpdateRunPayloadAppExit);

  return EFI_SUCCESS;

FwUpdateRunPayloadAppExit:

    return Status;
}

/**
  FwUpdateDeleteUXCapsule - Delete the UXCapsule

  @retval EFI_STATUS

**/
EFI_STATUS
FwUpdateDeleteUXCapsule()
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  Status =FwUpdateRegisterCapsuleGUID(&UXCapGuid,&QcomUxCapsuleDeleteEvent);      
  if(EFI_ERROR(Status))
  {
  DEBUG((EFI_D_ERROR,"\nError in creating UXCapsule Delete event")); 
  }
  Status = gBS->SignalEvent(QcomUxCapsuleDeleteEvent);
  if(EFI_ERROR(Status))
  {
  DEBUG((EFI_D_ERROR,"\nError in signalling UXCapsule Delete Event")); 
  }
  return Status;

}

/**
  FwUpdateSignalDeviceUpdaters - Signals the event for the OEM 
  device updates

  @retval EFI_STATUS

**/
EFI_STATUS
FwUpdateSignalDeviceUpdaters()
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  Status = gBS->CreateEventEx (
  EVT_NOTIFY_SIGNAL,
  TPL_CALLBACK,
  FwUpdateUXCapsuleEventHandler,
  NULL,
  &UXCapOEMGuid,
  &QcomUxCapsuleOemUpdateEvent
  );

  if(Status != EFI_SUCCESS) 
  {
  DEBUG((EFI_D_ERROR,"\nError in creating OEM Update event")); 
  }
  
  Status = gBS->SignalEvent(QcomUxCapsuleOemUpdateEvent);
  if(Status != EFI_SUCCESS) 
  {
  DEBUG((EFI_D_ERROR,"\nError in signalling OEM Update Event")); 
  }
  return Status;
}
/**
  FwUpdateDisplayUXCapsule - Display the UX Capsule text on 
  the screen 

  @param[in]  SystemTable   
  @param[out] UxCapsuleDisplayed  - Indicates if the UXCapsule
                                  was displayed or not

  @retval EFI_STATUS


**/
EFI_STATUS
FwUpdateDisplayUXCapsule
(
    IN EFI_SYSTEM_TABLE  *SystemTable, 
  IN BOOLEAN           *UxCapsuleDisplayed
)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  UINTN         pos     = 0;
  BOOLEAN CapsuleFound  = FALSE;
  EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE *BGRTTable   = NULL;

  if(FwUpdateFindCapsule(&UXCapGuid,SystemTable,&pos))
  {
    /*If the UXCapsule is found, first check if any pending capsule is present. 
    If there is no pending capsule (FWUpdate or OEM Update) then don't display
    the capsule*/
    Status = FwUpdateFindPendingCapsule(SystemTable,&CapsuleFound);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR,"\nError in finding pending capsule !! \n"));  
    }
    else if(CapsuleFound)
    {
      EFI_CAPSULE_TABLE *pcst = NULL;
      EFI_CAPSULE_HEADER *UxCapsuleHeader = NULL;
      EFI_CONFIGURATION_TABLE * pcfg_table = NULL;
      
      pcfg_table=SystemTable->ConfigurationTable+pos;  
      pcst =(EFI_CAPSULE_TABLE*)pcfg_table->VendorTable;
      if((pcst != NULL) && (pcst->CapsuleArrayNumber > 0) && (pcst->CapsulePtr[0] != NULL))
      {
        UxCapsuleHeader = pcst->CapsulePtr[0];     
        UxCapsulePayload = (UX_CAPSULE_PAYLOAD*)((UINT8*)UxCapsuleHeader + UxCapsuleHeader->HeaderSize);        
        AllocateAndZero(EfiBootServicesData, sizeof(EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE), (VOID**)&BGRTTable);
             
        if(!BGRTTable) 
        {
          DEBUG((EFI_D_ERROR,"\nUnable to allocate memory for the BGRT Table \n"));
          return EFI_OUT_OF_RESOURCES;  
        }
        
        BGRTTable->ImageOffsetX = UxCapsulePayload->ImageOffsetX;
        BGRTTable->ImageOffsetY = UxCapsulePayload->ImageOffsetY;
             
        //The Image Starts right at the end of the header
        UXCapsuleImageAddress = (UINT8*)(UxCapsulePayload + 1);
        UXCapsuleImageSize = UxCapsuleHeader->CapsuleImageSize - UxCapsuleHeader->HeaderSize - sizeof(UX_CAPSULE_PAYLOAD);

		/*TODO: validate again on 8998*/	 
        Status = RenderBGRTImage(UXCapsuleImageAddress, UXCapsuleImageSize);
        if (EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR,"\nUnable to render the image !! \n"));  
        }
        else
        {
          *UxCapsuleDisplayed = TRUE;
        } 
        gBS->FreePool(BGRTTable);   
      }
      else
      {
        DEBUG((EFI_D_ERROR,"\nUnable to find UX capsule\n"));  
        return EFI_NOT_FOUND;
      }
    }
  }

  return Status;
}
/**
  FwUpdateClearUxCapsuleText - Clear the UX Capsule text from 
  the screen 

  @param[in]  ImageOffsetX   
  @param[in]  ImageOffsetY   
  @param[in]  BGRTImage      BGRT Source image buffer
  @param[in]  BGRTImageSize  BGRT image buffer size
  
  @retval EFI_STATUS


**/
EFI_STATUS
FwUpdateClearUxCapsuleText
(
    IN UINTN ImageOffsetX, 
    IN UINTN ImageOffsetY,
    IN UINT8 *BGRTImage, 
    IN UINT32 BGRTImageSize)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput = NULL;
  EFI_STATUS                     Status         = EFI_SUCCESS;
  BMP_IMAGE_HEADER              *ImageHeader    = (BMP_IMAGE_HEADER*)BGRTImage;
  UINT32   ImageHeight;
  UINT32   ImageWidth;
  UINT32   DestStride;
  UINT32  *ConversionBuffer = NULL;      

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);

  if ((EFI_SUCCESS != Status) ||
  (NULL == GraphicsOutput) || 
  (0 == GraphicsOutput->Mode->Info->HorizontalResolution) ||
  (0 == GraphicsOutput->Mode->Info->VerticalResolution))
  {
    // GOP protocol Initialization failed
  }
  else
  {
  // Populate the image width and height
  ImageWidth = ImageHeader->PixelWidth;
      
  // Populate starting address, height and stride information, the source image could be stored
  // upside down, need to handle the case of an inverted image
  if ((INT32)ImageHeader->PixelHeight < 0)
  {
    // Image height is negative, indicating the image is in raster order, setup the pointers so we can
    // copy the image directly
    ImageHeight = (UINT32)- ImageHeader->PixelHeight;
  }
  else
  {
    // by default BMP images are stored upside down, setup the buffers to copy in the reverse direction
    ImageHeight = (UINT32)ImageHeader->PixelHeight;
  }
  
  // Calcualte the destination output stride.
  DestStride  = ImageWidth * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);  

  // Allocate a temporary buffer that is compatible with the GOP protocol
  Status = gBS->AllocatePool(EfiBootServicesData,(ImageHeight * DestStride),(VOID**)&ConversionBuffer);

  if((Status != EFI_SUCCESS) || !ConversionBuffer) 
  {
    DEBUG((EFI_D_ERROR,"\nUnable to allocate buffer to clear UXCapsule\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //Zero out the buffer to make it black
  ZeroMem(ConversionBuffer, (ImageHeight * DestStride)); 
  // 
  // Setup blt of conversion buffer to the screen
  // 
  Status = GraphicsOutput->Blt(GraphicsOutput,
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)ConversionBuffer,
                EfiBltBufferToVideo,
                0,
                0,
                ImageOffsetX,
                ImageOffsetY,
                ImageWidth,
                ImageHeight,
                ImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));          
  
  //
  // Free the conversion buffer
  //
  gBS->FreePool(ConversionBuffer);
  }
  return Status;
}


/**
  Main function for QCFirmwareUpdate. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
FwQcFirmwareUpdatePhaseMain (
  IN EFI_HANDLE                ImageHandle,
  IN EFI_SYSTEM_TABLE          *SystemTable,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

  )
{
  EFI_STATUS           Status        = EFI_SUCCESS;
  BOOLEAN              bVersionGood  = FALSE;

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL,
  *QcResultFlag = QCFW_UPDATE_INIT;

  //Run Payload app
  Status = FwUpdateRunPayloadApp(ImageHandle,
                                 SystemTable,
                                 MsftAttemptStatus,
                                 QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwUpdateRunPayloadApp,FwQcFirmwareUpdatePhaseMainExit);
  
  //Get Block I/O and File Handlesand  adding FwUpdatePayload
  Status = UpdateHandleRegistry(SystemTable);  
  if (EFI_ERROR(Status))
  {
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
    *QcResultFlag = QCFW_UPDATE_ERR_HANDLE_REGISTRY;
    HANDLE_ERROR_LABEL(Status,UpdateHandleRegistry,FwQcFirmwareUpdatePhaseMainExit);  
  }

  /*Parsing payload metadata*/
  Status = FwPayloadParserMain(MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwQcFirmwareUpdatePhaseMainExit);

  /*If no FwEntry found, return success*/
  if (FwEntryList.Count== 0)
  {
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;
    *QcResultFlag = QCFW_UPDATE_SUCCESS;
    return EFI_SUCCESS;
  }

  /**/

  /*Resource availability checking*/
  Status = FwUpdateResourceCheckMain(MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwQcFirmwareUpdatePhaseMainExit);

  /*Check if the payload version is in the boundary of lowest supported version*/
  Status = FwUpdateCheckLowestSupportedVersion(&bVersionGood,MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwQcFirmwareUpdatePhaseMainExit);

  if (!bVersionGood)
  { 
    Status = EFI_UNSUPPORTED;   
    DEBUG_ERR_LABEL("\nLowest Supported Version failed\n",FwQcFirmwareUpdatePhaseMainExit);
  }        
  
  //Run FwUpdatePhase
  Status = FwUpdatePhaseMain(SystemTable,
                             MsftAttemptStatus,
                             QcResultFlag);

  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwQcFirmwareUpdatePhaseMainExit);

  return EFI_SUCCESS;

FwQcFirmwareUpdatePhaseMainExit:

  /* Indicate failure of processing payload to legacy update */
  //Status=FwUpdateClearFlagOrCapsule(FALSE);
  return Status;      
 }

/**
 * The user Entry Point for FirmwareUpdate. 
 *
 * @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
 * @param[in] SystemTable    A pointer to the EFI System Table.
 * 
 * @retval EFI_SUCCESS       The entry point is executed successfully.
 * @retval other             Some error occurs when executing this entry point.
 *
 **/
EFI_STATUS
EFIAPI
RunFwUpdateMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status                 = EFI_SUCCESS;
  UINTN                    pos                    = 0;
  ESRT_ENTRY              *ent                    = NULL;
  BOOLEAN                  UxCapsuleDisplayed     = FALSE;
  BOOLEAN                  SecureBootEnabled      = FALSE;
  BOOLEAN                  DebugModeEnabled       = FALSE;
  BOOLEAN                  bResetSystemOnDeviceFw = FALSE;
  ESRT_LAST_ATTEMPT_STATUS MsftAttemptStatus      = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  QCFW_UPDATE_FLAG_VALUE   QcResultFlag           = QCFW_UPDATE_INIT;
  
  FWupdateMode = QCOM_FW_UPDATE_MODE_NOUPDATE;

  //Get Block I/O and File Handles
  Status = UpdateHandleRegistry(SystemTable);
  HANDLE_ERROR_LABEL(Status,UpdateHandleRegistry,RunFwUpdateMainExit);

  Status = CheckandCreateFilesinFat() ;
  HANDLE_ERROR_LABEL(Status,CheckandCreateFilesinFat,RunFwUpdateMainExit);

  Status =QcomESRTPhaseMain(SystemTable);
  HANDLE_ERROR_LABEL(Status,QcomESRTPhaseMain,RunFwUpdateMainExit);

  //Display UX Capsule Here, the update process should continue even if UxCapsule display doesn't work properly
  Status = FwUpdateDisplayUXCapsule(SystemTable,&UxCapsuleDisplayed);
  
  QcomGetEsrtEntry(&ent);

  if(FwUpdateFindCapsule(&(ent->FWClass),SystemTable,&pos))
  {
    QcomPrintEsrtEntry(ent,EFI_D_WARN);

    FWupdateMode = QCOM_FW_UPDATE_MODE_CAPSULE;
    Status = FwUpdateCapsulePhaseMain(ImageHandle,
                                      SystemTable, 
                                      pos,
                                      &MsftAttemptStatus,
                                      &QcResultFlag);
    HANDLE_ERR_CONTINUE(Status,FwUpdateCapsulePhaseMain);

    Status = FwUpdateDone(SystemTable,MsftAttemptStatus,QcResultFlag);
    HANDLE_ERR_CONTINUE(Status,FwUpdateDone);

    /*If capsule is found, it is guaranteed that LoadImage() will be called. So that system reset is required to avoid PCR7 measurement*/
    goto ResetAfterLoadImageIsCalled;
  }
  else 
  {
    DEBUG((EFI_D_WARN,"\n  No system fw capsule found...Skipped\n"));

    /* bitlocker workaround */
    bResetSystemOnDeviceFw = FALSE;

    /* Check if any device firmware update is pending */
    FwUpdateFindPendingCapsule(SystemTable,&bResetSystemOnDeviceFw);
    /* bitlocker workaround  */

    //Signal the OEMs to do their update
    FwUpdateSignalDeviceUpdaters();

    //Clear out the UXCapsule if it was displayed before deleting the UXCapsule
    if(UxCapsuleDisplayed)
    {
      FwUpdateClearUxCapsuleText(UxCapsulePayload->ImageOffsetX, UxCapsulePayload->ImageOffsetY,UXCapsuleImageAddress, UXCapsuleImageSize);
    }
  
    FwUpdateDeleteUXCapsule();

    /* bitlocker workaround
     * Authenticating payload which is not KEK/DB signed will add boot measurement in PCR7 log,
     * and result in bitlocker issue. Add a reset if device fw update happens.
     */
    if (bResetSystemOnDeviceFw)
    {
      goto ResetAfterLoadImageIsCalled;
    }
    /* bitlocker workaround  */
    
    Status = GetSecureBootState(&SecureBootEnabled);
    if(EFI_ERROR(Status))
    {
       // If failed, assume secure boot is disabled.
       SecureBootEnabled = FALSE;
       DEBUG((EFI_D_WARN,"Warning: Failed to get secure boot state: %r\n", Status));
    }
    Status = GetDebugModeState(&DebugModeEnabled);

    if(SecureBootEnabled && !DebugModeEnabled)
    {
      /*QCFirmwareUpdate should be disabled when SecureBoot is enabled and DebugMode is disabled*/
      DEBUG((EFI_D_WARN,"Warning: SecureBoot is enabled and Windows DebugPolicy is not installed. Firmware update skipped.\n"));
      return EFI_SUCCESS;     
    }
      
    //Check updateflag
    Status = CheckFlag(QCOM_FW_UPDATE_LEGACY_UPDATE_COMMIT_FLAG,
              (VOID*)&QcResultFlag);
    //ERR_HANDLE(Status);   
      
    if(QcResultFlag!=  QCFW_UPDATE_COMMIT_SET) 
    {
      DEBUG((EFI_D_WARN,"\n  No system fw payload found...Skipped\n"));
      return EFI_SUCCESS;
    }

    QcomPrintEsrtEntry(ent,EFI_D_WARN);

    FWupdateMode =QCOM_FW_UPDATE_MODE_QCFW;  
  
    //Dont check the return status here. System reset is required to avoid PCR7 
    //measurement record after called LoadImage() function in FwUpdateRunPayloadApp.  
    Status = FwQcFirmwareUpdatePhaseMain(ImageHandle,SystemTable,&MsftAttemptStatus,&QcResultFlag);
    HANDLE_ERR_CONTINUE(Status,FwQcFirmwareUpdatePhaseMain);

    Status = FwUpdateDone(SystemTable,MsftAttemptStatus,QcResultFlag);
    HANDLE_ERR_CONTINUE(Status,FwUpdateDone);
  } 
//System reset is required to avoid PCR7 measurement record after called LoadImage() function in FwUpdateRunPayloadApp.  
ResetAfterLoadImageIsCalled:

  DEBUG((EFI_D_WARN,"\nSystem firmware update completed...Resetting system\n"));

  gRT->ResetSystem(EfiResetCold,
                      EFI_SUCCESS,
                      0,
                      NULL) ;
  //added return value to avoid compile warning. 
  return EFI_SUCCESS;

RunFwUpdateMainExit:
 /*only SUCCESS here, shouldn't error out*/
  return EFI_SUCCESS;

}

