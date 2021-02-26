/** @file DeviceUpdaterSkeleton.c

  A skeleton device firmware updater driver that demonstrates 
  device firmeare update solution using UpdateCapsule. 
  
  Copyright (c) 2012,2015 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     --------------------------------------------------------
 10/12/15     mic     Support updated Dpp protocol
 07/13/15     al      Changing TPL_NOTIFY to TPL_CALLBACK
 02/13/13     Mic     Added payload authentication 
 07/04/12     Mic     Updated DummyFindCapsule function to return the payload
                      pointer and payload size
 07/04/12     Mic     Removed dependency on FwCommonLib and FwUpdateLib
 06/14/12     jd/MiC  Initial version
=============================================================================*/
#include <Uefi.h>

/**
  EFI interfaces
 */
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/PcdLib.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/EFIAuthentication.h>

#include "DeviceUpdaterSkeleton.h"

/*===========================================================================
                        CONST and MACRO DEFINITIONS
===========================================================================*/

#define DEFAULT_ESRT_CAPSULE_FLAGS      0x0
#define DUMMY_ESRT_PROVISION_DATA_NAME  L"ESRT_DUMMY.PROVISION"
#define DUMMY_ESRT_PROVISION_DATA_TYPE  L"OEM"
#define ERR_HANDLE(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      return Status; \
    } \
  } while (0)

/*===========================================================================
                        GLOBAL DEFINITIONS
===========================================================================*/

/* Type/Name pair for DPP item of dummy device ESRT entry */
static CHAR16 *gDummyDeviceEsrtDppName = DUMMY_ESRT_PROVISION_DATA_NAME;
static CHAR16 *gDummyDeviceEsrtDppType = DUMMY_ESRT_PROVISION_DATA_TYPE;

/* Default ESRT entry for dummy device */
static ESRT_ENTRY esrt_ent = 
{
  DUMMY_DEVICE_FW_CLASS_GUID,   /* Firmware class */
  FW_TYPE_DEVICE,               /* Firmware type */
  0x0,                          /* Current firmware version */
  0x0,                          /* The lowest firmware version that can be rolled back to */
  DEFAULT_ESRT_CAPSULE_FLAGS,   /* Capsule flag */
  0x0,                          /* Last attempt version */
  FW_LAST_ATMPT_STATUS_SUCCESS  /* Last attempt status */
};

/* GUID to identify ESRT table in EFI configuration table */
static EFI_GUID ESRT_guid = ESRT_GUID;
/* GUID to identify the event group that dummy device updater should register for */
static EFI_GUID OEMUpdateEventGuid = QCOM_FW_UPDATE_OEM_UPDATE_EVENT_GUID;
/* Payload info that contained in dummy device FW update payload */
static DUMMY_DEVICE_PAYLOAD_VERSION  DummyDevicePayloadVer = {0};
/* Event the dummy device updater should register for device firmware update */
static EFI_EVENT DummyDeviceUpdateEvent = NULL;
/* Event the dummy device updater should register and signal for capsule deletion */
static EFI_EVENT DummyDeviceCapsuleDeleteEvent = NULL;
/* Saved copy of ImageHandle */
static EFI_HANDLE gImageHandle;
/* Saved copy of SystemTable */
static EFI_SYSTEM_TABLE *gSystemTable = NULL;


/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Read ESRT entry info from storage
 * 
 * @param buf - the address of pointer  pointing to the buffer
 * @param esrtSize - size to read
 * @return EFI_STATUS 
 */

EFI_STATUS
ReadEsrtEntryFromStorage(
  IN OUT ESRT_ENTRY **Buf, 
  IN OUT UINTN     *Size
  )
{
  EFI_STATUS        Status=EFI_SUCCESS;
  EFI_DPP_PROTOCOL  *DppProtocol = NULL;

  if(Buf == NULL || Size == NULL)
  {  
    Status = EFI_INVALID_PARAMETER;
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to locate DPP Protocol\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Get ESRT entry size */
  Status = DppProtocol->GetDPP(DppProtocol, 
                               gDummyDeviceEsrtDppType,
                               gDummyDeviceEsrtDppName,
                               NULL,
                               Size);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to get ESRT entry size\n"));
    goto ReadEsrtEntryFromStorageExit;
  }
  
  /* Allocate buffer */
  Status = gBS->AllocatePool(EfiBootServicesData,
                             *Size,
                             (VOID **)Buf);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to allocate buffer for ESRT entry\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Read ESRT entry */
  Status = DppProtocol->GetDPP(DppProtocol,
                               gDummyDeviceEsrtDppType,
                               gDummyDeviceEsrtDppName,
                               *Buf,
                               Size);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to get ESRT entry data\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

ReadEsrtEntryFromStorageExit:

  return Status;
}


/**
 * Write ESRT entry info into storage.
 *  Currently 
 * 
 * @param PoolType - PoolType for AllocatPool 
 * @param 
 * @param 
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
WriteEsrtEntryToStorage(
  IN ESRT_ENTRY *Buf, 
  IN UINTN     Size
  )
{
  EFI_STATUS  Status=EFI_SUCCESS;
  EFI_DPP_PROTOCOL  *DppProtocol = NULL;

  if(Buf == NULL)
  {  
    Status = EFI_INVALID_PARAMETER;
    goto WriteEsrtEntryToStorageExit;
  }
    
  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to locate DPP Protocol\n"));
    goto WriteEsrtEntryToStorageExit;
  }

  /* Write ESRT data into DPP */
  Status = DppProtocol->SetDPP(DppProtocol,
                               gDummyDeviceEsrtDppType,
                               gDummyDeviceEsrtDppName,
                               Buf,
                               Size);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to set ESRT entry data\n"));
    goto WriteEsrtEntryToStorageExit;
  }  
  
WriteEsrtEntryToStorageExit:
  
  return Status;

}


/**
 *  Check if ESRT header is installed in EFI System table 
 *   
 * @param SystemTable -
 * @return BOOLEAN 
 */

BOOLEAN
IsEsrtHeaderInstalledInSysTbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_CONFIGURATION_TABLE * pcfg_table;
  UINTN cs_cnt;
  UINTN i=0;

  if (!SystemTable)
    return FALSE;
  
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
  
  for (i=0;i<cs_cnt; i++){
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid)){
      return TRUE;
    }
    pcfg_table++;
  }  

  return FALSE;
}


/**
 * Install ESRT header in system table
 * @param SystemTable - UEFI System Table
 * @return EFI_STATUS 
 */

EFI_STATUS
InstallEsrtHeaderInSysTbl(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS        Status=EFI_SUCCESS;
  ESRT_TABLE_HEADER *pesrt_tbl_header=NULL;

  if (!SystemTable)
    return EFI_ABORTED;

  /*Create a new ESRT header*/
  Status=gBS->AllocatePool(EfiRuntimeServicesData,sizeof(ESRT_TABLE_HEADER),(VOID*)&pesrt_tbl_header);
  ERR_HANDLE(Status);

  /*Initialize the header*/
  pesrt_tbl_header->FWResrcCnt=0;
  pesrt_tbl_header->FWResrcMax=0;
  pesrt_tbl_header->FWResrcVer=1;
  
  /*Install the config table*/
  Status = gBS->InstallConfigurationTable (&ESRT_guid, pesrt_tbl_header);
  
  return Status;
};


/**
 * Check if ESRT Entry is installed.
 * @param SystemTable - UEFI System Table
 * @return BOOLEAN
 */  
 
BOOLEAN
IsEsrtEntryInstalledInSysTbl(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_CONFIGURATION_TABLE *pcfg_table=NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header=NULL,*pEntStart=NULL;
  ESRT_ENTRY              *pCurEnt=NULL;
  UINTN                   ent_cnt=0;
  UINTN                   cs_cnt;
  UINTN                   i=0,j=0;

  if (!SystemTable)
    return FALSE;
  
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;

  /*Search the ESRT table GUID from System table*/
  for (i=0;i<cs_cnt; i++){
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid)){

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
 * Install ESRT entry in ESRT in System table.
 * @param SystemTable - the UEFI System Table
 * @return EFI_STATUS 
 */

EFI_STATUS
InstallEsrtEntryInSysTbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status=EFI_SUCCESS;
  EFI_CONFIGURATION_TABLE *pcfg_table=NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header=NULL,*pNew_tbl_header=NULL;
  ESRT_ENTRY              *pNew_esrt_ent=NULL,*pTemp=NULL;
  UINTN                   old_ent_cnt=0,new_ent_cnt=0;
  UINTN                   old_tbl_size=0,new_tbl_size=0;
  UINTN                   cs_cnt=0,i=0;

  if (!SystemTable)
    return EFI_ABORTED;

  /*check if ESRT table exist?*/
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
  
  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid((GUID*)&ESRT_guid,(GUID*)&pcfg_table->VendorGuid))
    {

      pCur_tbl_header=(ESRT_TABLE_HEADER*)pcfg_table->VendorTable;
      /*calc the new count of ESRT entries*/
      old_ent_cnt = pCur_tbl_header->FWResrcCnt;
      new_ent_cnt = pCur_tbl_header->FWResrcCnt+1;
      old_tbl_size = pCur_tbl_header->FWResrcCnt*sizeof(ESRT_ENTRY)+sizeof(ESRT_TABLE_HEADER);
      new_tbl_size = new_ent_cnt*sizeof(ESRT_ENTRY)+ sizeof(ESRT_TABLE_HEADER);

      /* Allocate memory for new ESRT entries */
      Status=gBS->AllocatePool(EfiRuntimeServicesData,new_tbl_size,(VOID*)&pNew_tbl_header);
      ERR_HANDLE(Status);
      
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
      Status = gBS->FreePool(pCur_tbl_header);
            
      return Status;
    }
    pcfg_table++;
  }  
  return EFI_ABORTED;
}


/**
 * Update ESRT entry in System table.
 *  Currently 
 * @param SystemTable - the UEFI System Table
 * 
 * @return EFI_STATUS 
 */

BOOLEAN
UpdateEsrtInSystbl(
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_CONFIGURATION_TABLE *pcfg_table=NULL;
  ESRT_TABLE_HEADER       *pCur_tbl_header=NULL,*pEntStart=NULL;
  ESRT_ENTRY              *pCurEnt=NULL;
  UINTN                   ent_cnt=0, cs_cnt=0,i=0,j=0;

  if (!SystemTable)
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
 *  Get FW update payload info
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
DummyDeviceGetPayloadInfo()
{
  EFI_STATUS Status = EFI_SUCCESS;

  /*
   For demonstration purpose, FW version is hardcoded here, which
   should match the FW version specified in FW update driver package
   INF file. Lowest supported version is also hardcoded to 0x00.
  */
  DummyDevicePayloadVer.FwVer = 0x04640005;
  DummyDevicePayloadVer.FwLowestSupportedVer = 0x00;

  return Status;
}


/**
 *  Write updated ESRT entry into DPP and System Table.
 * 
 * @param SystemTable - 
 * @param attemptatus - the update attempt status. 
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
UpdateESRT(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS attemptStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN esrtSize=0;

  if (!SystemTable)
    return EFI_ABORTED;

  DummyDeviceGetPayloadInfo();

  switch(attemptStatus)
  {
  case FW_LAST_ATMPT_STATUS_SUCCESS:
    esrt_ent.FWVer = DummyDevicePayloadVer.FwVer;
    esrt_ent.LastAttemptVer = DummyDevicePayloadVer.FwVer;
    /* Update the lowest supported version only if it's greater than what is currently set */
    if(DummyDevicePayloadVer.FwLowestSupportedVer > esrt_ent.FwLowestSupportedVer)
    {
      esrt_ent.FwLowestSupportedVer = DummyDevicePayloadVer.FwLowestSupportedVer;
    }
    break;

  case FW_LAST_ATMPT_STATUS_UNSUCCESSFUL:
  case FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE:
  case FW_LAST_ATMPT_STATUS_INCORRECT_VERSION:
  case FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT:
  case FW_LAST_ATMPT_STATUS_AUTHEN_ERROR:
  default:
    esrt_ent.LastAttemptVer = DummyDevicePayloadVer.FwVer;
    break;
  }

  esrt_ent.LastAttemptStatus = (UINT32)attemptStatus;
   
  /* update Systemtable */
  if(!UpdateEsrtInSystbl(SystemTable))
  {
    return EFI_ABORTED;
  }
      
  /* update ESRT in DPP */
  esrtSize = sizeof(ESRT_ENTRY);
  Status = WriteEsrtEntryToStorage(&esrt_ent, esrtSize);

  return Status;
}


/**
 * ESRT phase of FW update
 *
 * @param SystemTable 
 * @return EFI_STATUS 
 */

EFI_STATUS
InstallEsrtEntry(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{

  EFI_STATUS Status=EFI_SUCCESS;
  ESRT_ENTRY  *buf=NULL;
  UINTN esrtSize;

  if (!SystemTable)
    return EFI_ABORTED;

  /*Read ESRT entry from DPP*/
  esrtSize = sizeof(ESRT_ENTRY);
  Status = ReadEsrtEntryFromStorage(&buf, &esrtSize);

  if(Status != EFI_SUCCESS)
  {
    /* Write the initial DPP ESRT entry using the default */
    Status = WriteEsrtEntryToStorage(&esrt_ent, esrtSize);
    ERR_HANDLE(Status); 
  }
  else
  {
    CopyMem(&esrt_ent,(VOID*)buf,esrtSize);
  }
  
  /*Create the ESRT header if it doesn't exist in System table*/
  if(!IsEsrtHeaderInstalledInSysTbl(SystemTable))
  {
    Status = InstallEsrtHeaderInSysTbl(SystemTable);    
    ERR_HANDLE(Status);
  }

  /*Install the ESRT entry if it doesn't exist in ESRT table*/
  if(!IsEsrtEntryInstalledInSysTbl(SystemTable))
  {
    Status = InstallEsrtEntryInSysTbl(SystemTable);
    ERR_HANDLE(Status);
  }
  else
  {
    /*Populate updated ESRT table to System table*/
    Status = UpdateEsrtInSystbl(SystemTable);
    ERR_HANDLE(Status);
  }
  
  return Status;
}


/** 
* searching the system table to see if fw capsule is installed
*  
* @param  SystemTable   :  UEFI system table
* @param  PayloadSize   :  The Payload Size 
* @param  pPayload      :  The payload pointer  
* @return BOOLEAN
**/

BOOLEAN
DummyFindCapsule(
  IN EFI_GUID *CapGUID,
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT UINTN             *PayloadSize,
  OUT VOID              **pPayload
  )
{

  UINTN i = 0;
  UINTN cs_cnt = 0;  
  EFI_CAPSULE_TABLE *pcst = NULL;
  EFI_CAPSULE_HEADER *pcsh = NULL;
  EFI_CONFIGURATION_TABLE *pcfg_table = NULL;

  if (!SystemTable || !CapGUID || !PayloadSize)
    return FALSE;
    
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
  
  /*check GUID installed in system table*/
  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid(CapGUID,(GUID*)&pcfg_table->VendorGuid))
    {
      pcst =(EFI_CAPSULE_TABLE*)pcfg_table->VendorTable;
      if( (pcst != NULL) && (pcst->CapsuleArrayNumber > 0) && (pcst->CapsulePtr[0] != NULL) )
      {
        pcsh =pcst->CapsulePtr[0];
        *PayloadSize = pcsh->CapsuleImageSize - pcsh->HeaderSize;
        *pPayload = (UINT8*)pcsh+ pcsh->HeaderSize;
        return TRUE;
      }
      return FALSE;
    }
    pcfg_table++;
  }
 
  return FALSE;
}


/** 
*Does nothing. Just a placeholder
*  
* @param  Event   :  Event whose notification function is
*                    being invoked.
* @param  Context :  Pointer to the notification function's
*                    context, which is always zero in current
*                    implementation.
* @return   NONE
**/

VOID
EFIAPI
DummyDeviceDummyEventHandler(
  IN EFI_EVENT      Event,
  IN VOID           *ContextGuardPredicateList
  )
{
  /*AsciiPrint("Received capsule event...\n");*/
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
        DummyDeviceDummyEventHandler,
        NULL,
        CapGUID,
        DelEvent
        );    
  return Status;
}


/** 
* Capsule phase of FW update
*  
* @param ImageHandle :  UEFI imagehandle
* @param SystemTable :  UEFI system table
* @param PayloadSize :  The payload size
* @param pPayload    :  The payload pointer  
* @return EFI_STATUS
**/

EFI_STATUS
PerformCapsuleUpdate(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable,
  IN UINTN            PayloadSize,
  IN VOID             *pPayload
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  ESRT_LAST_ATTEMPT_STATUS attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  EFI_QCOM_AUTHENTICATION_PROTOCOL *PeAuthProtocol = NULL;

  /* Check if the FW update payload version is in the range of lowest supported version */
  /* ...... */
  
  /* Check if the battery power level is sufficient for capsule update only
   if update is not already in-progress.  This allows an in-progress
   update to complete if the power level was on the boundary at the beginning
   of the update and a power cycle occurred which could prevent the
   update from completing */
  /* ...... */

  /* Payload Authentication*/
  Status = gBS->LocateProtocol(&gQcomAuthenticationProtocolGuid, 
                               NULL, 
                               (VOID**)&PeAuthProtocol); 
  if(EFI_ERROR(Status)) 
  { 
    DEBUG((EFI_D_ERROR, "Device: Failed to locate PE authentication 20 Protocol\n")); 
    attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
    goto PerformCapsuleUpdateExit; 
  } 
  
  DEBUG((EFI_D_ERROR, "Device: Authenticating device payload\n")); 

  Status = PeAuthProtocol->AuthenticatePeImage(PeAuthProtocol, 
                                               pPayload, 
                                               PayloadSize); 
  if (EFI_ERROR(Status)) 
  { 
    DEBUG((EFI_D_ERROR, "Authentication failed with status = 0x%x\n", Status)); 

    if (Status == EFI_SECURITY_VIOLATION) 
    { 
      attemptStatus = FW_LAST_ATMPT_STATUS_AUTHEN_ERROR; 
    } 
    else if ((Status == EFI_LOAD_ERROR) || (Status == EFI_UNSUPPORTED)) 
    { 
      attemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT; 
    } 
    else 
    { 
      attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL; 
    } 
    goto PerformCapsuleUpdateExit;
  }
  else
  {
    /* Performing device firmware update */
    /* For demo purpose, just put a 10 seconds delay to display the UX capsule */
    /* It is up to device vendor's implementation to update the device firmware */
    DEBUG((EFI_D_WARN,"DummyDevice: Updating device firmware...\n")); 
    gBS->Stall(10000000);
    DEBUG((EFI_D_WARN,"DummyDevice: Firmware update completed.\n")); 
    attemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;  
  }

PerformCapsuleUpdateExit:  
  /* Update ESRT entry with firmware update status */
  Status=UpdateESRT(gSystemTable, attemptStatus);
  
  /* Delete capsule */
  Status = gBS->SignalEvent(DummyDeviceCapsuleDeleteEvent);
  
  return Status;
}


/**
* Callback to perform device firmware update.
*
* @param  Event         Event whose notification function is being invoked.
* @param  Context       Pointer to the notification function's context, which is
*                       always zero in current implementation.
**/
VOID
EFIAPI
DummyDeviceUpdateFirmware(
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  ESRT_ENTRY *ent = NULL;
  UINTN payloadSize = 0;
  VOID *pPayload =NULL;

  DEBUG((EFI_D_WARN,"DummyDevice: signal received to update device firmware...\n")); 

  ent = &esrt_ent;

  if(DummyFindCapsule(&(ent->FWClass), gSystemTable, &payloadSize,&pPayload ))
  {
    PerformCapsuleUpdate(gImageHandle,
                         gSystemTable, 
                         payloadSize,pPayload);
    
    /* If LoadImage() is used to load FW payload, system reset is required to avoid PCR7 measurement */
  }
  else
  {
    DEBUG((EFI_D_WARN,"DummyDevice: didn't find capsule.\n")); 
  }
}


/**
* DummyDeviceRegisterUpdateEvent   
* Register the event for device firmware update
*  @retval EFI_STATUS
**/
EFI_STATUS
DummyDeviceRegisterUpdateEvent()
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                              TPL_CALLBACK,
                              DummyDeviceUpdateFirmware,
                              NULL,
                              &OEMUpdateEventGuid,
                              &DummyDeviceUpdateEvent
  );

  if(Status != EFI_SUCCESS) 
  {
    DEBUG((EFI_D_ERROR,"DummyDevice: Error in creating OEM Update event")); 
  }
  
  return Status;
}


/**
*  DeviceUpdaterInitialize ()
*  @brief
*  Entry function of updater
*/
EFI_STATUS
DeviceUpdaterInitialize(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;  
  ESRT_ENTRY *ent = NULL;
  UINTN pos = 0;
  VOID *pPayload =NULL;

  /*
   Save a local copies of ImageHandle and SystemTable
  */

  gImageHandle = ImageHandle;
  gSystemTable = SystemTable;

  /*
   ESRT phase
  */

  Status = InstallEsrtEntry(SystemTable);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "DummyDevice: Failed to write default ESRT entry\n"));
    return Status;
  }

  ent = &esrt_ent;
  if(DummyFindCapsule(&(ent->FWClass), gSystemTable, &pos,&pPayload ))
  {
    /* Register capsule GUID for capsule deletion */
    Status =FwUpdateRegisterCapsuleGUID(&(ent->FWClass),&DummyDeviceCapsuleDeleteEvent);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR,"DummyDevice: FwUpdateRegisterCapsuleGUID failed\n")); 
      return Status;
    }
  
    /*
       Register event for firmware update. The event will be signaled by QCOM
       system firmware updater at BDS phase.
      */
    Status = DummyDeviceRegisterUpdateEvent();
  }
  return Status;
}


