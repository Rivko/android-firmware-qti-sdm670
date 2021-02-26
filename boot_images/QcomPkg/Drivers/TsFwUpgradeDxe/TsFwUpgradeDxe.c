/** @file TsFwUpgradeDxe.c

  Dxe file to do firmware update for touch screen controller using UpdateCapsule .

  Copyright (c) 2012-2015,2017 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     --------------------------------------------------------
 01/27/17     ai      Fix use of retail
 10/12/15     mic     Support updated Dpp protocol
 07/13/15     al      Changing TPL_NOTIFY to TPL_CALLBACK
 04/09/14     dm      Changing gImageHandle name to avoid GCC compiler Warning
 05/17/13     dm      Changes in logic finding Firmware/CFG in payload
 04/24/13     dm      Added support for HIDI2C Communication
 03/22/13     dm      Added OS Type checking and battery level checking
 03/06/13     dm      Added UEFI Capsule Authentication code
 02/07/13     dm      Fixed Warnings
 09/11/12     dm      Added I2CModeOnAndoff function
 08/09/12     dm      Added code for Payload parsing,
                      battery checking, FW, CFG functions
 06/20/12     sr      Initial version
=============================================================================*/

#include "string.h"
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
#include <Library/FwCommonLib.h>
#include <Library/FwUpdateLib.h>
#include <Library/PcdLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIAuthentication.h>

#include <Protocol/EFIDpp.h>

/**
  Include Charger DXE Dependencies
  */
#include <Protocol/EFICharger.h>
/**
  Include Charger Extension Dependencies
  */
#include <Protocol/EFIChargerExtn.h>

/**
  Include PMIC Dependencies
  */
//#include <Protocol/EFIPmicCharger.h>
#include "TsFwUpgradeDxe.h"


/*===========================================================================
                        CONST and MACRO DEFINITIONS
===========================================================================*/

#define DEFAULT_ESRT_CAPSULE_FLAGS      0x0
#define TS_ESRT_PROVISION_DATA_NAME  L"ESRT_TS.PROVISION"
#define TS_ESRT_PROVISION_DATA_TYPE   L"OEM"
#define MINIMUM_BATTERY_CHARGE_PERCENT 25
#define ALLOW_NO_BATTERY_FIRMWARE_UPDATE

/*===========================================================================
                        GLOBAL DEFINITIONS
===========================================================================*/

/* Type/Name pair for DPP item of Ts device ESRT entry */
static CHAR16 *gTsDeviceEsrtDppName = TS_ESRT_PROVISION_DATA_NAME;
static CHAR16 *gTsDeviceEsrtDppType = TS_ESRT_PROVISION_DATA_TYPE;

/* Default ESRT entry for Ts device */
static ESRT_ENTRY esrt_ent =
{
  TS_DEVICE_FW_CLASS_GUID,      /* Firmware class */
  FW_TYPE_DEVICE,               /* Firmware type */
  0x00,                          /* Current firmware version */
  0x00,                          /* The lowest firmware version that can be rolled back to */
  DEFAULT_ESRT_CAPSULE_FLAGS,   /* Capsule flag */
  0x00,                          /* Last attempt version */
  FW_LAST_ATMPT_STATUS_SUCCESS  /* Last attempt status */
};

/* GUID to identify ESRT table in EFI configuration table */
static EFI_GUID ESRT_guid = ESRT_GUID;
/* GUID to identify the event group that Ts device updater should register for */
static EFI_GUID UXCapOEMGuid = QCOM_FW_UPDATE_UX_CAPSULE_OEM_GUID;
/* Payload info that contained in Ts device FW update payload */
static TS_DEVICE_PAYLOAD_VERSION  TsDevicePayloadVer = {0};
/* Event: the Ts device updater should register for device firmware update */
static EFI_EVENT TsDeviceUpdateEvent = NULL;
/* Event: the Ts device updater should register and signal for capsule deletion */
static EFI_EVENT TsDeviceCapsuleDeleteEvent = NULL;
/* Saved copy of ImageHandle */
static EFI_HANDLE gImageHandle_TsFw;
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
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to locate DPP Protocol\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Get ESRT entry size */
  Status = DppProtocol->GetDPP(DppProtocol,
                               gTsDeviceEsrtDppType,
                               gTsDeviceEsrtDppName,
                               NULL,
                               Size);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to get ESRT entry size\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Allocate buffer */
  Status = gBS->AllocatePool(EfiBootServicesData,
                             *Size,
                             (VOID **)Buf);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to allocate buffer for ESRT entry\n"));
    goto ReadEsrtEntryFromStorageExit;
  }

  /* Read ESRT entry */
  Status = DppProtocol->GetDPP(DppProtocol,
                               gTsDeviceEsrtDppType,
                               gTsDeviceEsrtDppName,
                               *Buf,
                               Size);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to get ESRT entry data\n"));
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
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to locate DPP Protocol\n"));
    goto WriteEsrtEntryToStorageExit;
  }

  /* Write ESRT data into DPP */
  Status = DppProtocol->SetDPP(DppProtocol,
                               gTsDeviceEsrtDppType,
                               gTsDeviceEsrtDppName,
                               Buf,
                               Size);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "TsDevice: Failed to set ESRT entry data\n"));
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
 *  Write updated ESRT entry into DPP and System Table.
 *
 * @param SystemTable -
 * @param attemptStatus - the update attempt status.
 *
 * @return EFI_STATUS
 */

EFI_STATUS
UpdateESRT(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS attemptStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 esrtSize=0;

  if (!SystemTable)
    return EFI_ABORTED;

    DEBUG ((EFI_D_INFO, "TSFW:Entering updateESRT\n"));

  switch(attemptStatus)
  {
  case FW_LAST_ATMPT_STATUS_SUCCESS:
    DEBUG ((EFI_D_INFO, "TSFW:last attempt success\n"));
    esrt_ent.FWVer = TsDevicePayloadVer.FwVer;
    esrt_ent.LastAttemptVer = TsDevicePayloadVer.FwVer;
    break;

  case FW_LAST_ATMPT_STATUS_UNSUCCESSFUL:
  case FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE:
  case FW_LAST_ATMPT_STATUS_INCORRECT_VERSION:
  case FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT:
  case FW_LAST_ATMPT_STATUS_AUTHEN_ERROR:
  default:
    esrt_ent.LastAttemptVer = TsDevicePayloadVer.FwVer;
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
* Locate the firmware image within a PE/COFF format payload
*
* @param ppPayload   :
* @return EFI_STATUS
**/
EFI_STATUS
LocateFirmwareImageInPayload(
  IN OUT VOID **ppPayload,
  IN     UINTN  PayloadSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 *pDeviceHID = NULL;
  UINTN i = 0;
  UINTN LoopCount = 0;
  BOOLEAN bFound = FALSE;

  pDeviceHID = *ppPayload;

  LoopCount = PayloadSize - sizeof(GUID) + 1;

  for (i = 0; i < LoopCount; i++)
  {
    if(CompareGuid((GUID*)&esrt_ent.FWClass,(GUID*) pDeviceHID))
    {
      bFound = TRUE;
      break;
    }
    else
    {
      pDeviceHID++;
    }
  }

  if (bFound)
  {
    *ppPayload = (UINT8*)pDeviceHID + 16 + 8; // 16 - GUID size + 8 zeros
  }
  else
  {
    Status = RETURN_NOT_FOUND;
  }

  return Status;
}

/**
 * ESRT phase of FW update
 *
 * @param SystemTable
 * @return EFI_STATUS
 */

EFI_STATUS
EsrtPhase(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  )
{

  EFI_STATUS Status=EFI_SUCCESS;
  ESRT_ENTRY  *buf=NULL;
  UINT32 esrtSize;

  if (!SystemTable)
    return EFI_ABORTED;

    DEBUG ((EFI_D_INFO, "TSFW:entering esrt phase\n"));

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
    /* Check if the Firmware Resource GUID in storage is a known Qualcomm firmware resource GUID. */
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
  DEBUG ((EFI_D_INFO, "TSFW:leaving esrt phase\n"));

  return Status;
}


/**
* searching the system table to see if fw capsule is installed
*
* @param    SystemTable   :  UEFI system table
* @param    pos           :  the capsule index
*
* @return BOOLEAN
**/

BOOLEAN
Fw_UpdateFindCapsule(
  IN EFI_GUID *CapGUID,
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT UINTN             *PayloadSize,
  OUT VOID              **pPayload
  )
{

  EFI_CONFIGURATION_TABLE * pcfg_table;
  UINTN i=0;
  EFI_CAPSULE_TABLE *pcst = NULL;
  EFI_CAPSULE_HEADER *pcsh = NULL;
  UINTN cs_cnt;

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
* @param    Event    : Event whose notification function is
*                     being invoked.
* @param    Context : Pointer to the notification function's
*                     context, which is always zero in current
*                     implementation.
* @return   NONE
**/

VOID
EFIAPI
TsDeviceDummyEventHandler(
  IN EFI_EVENT          Event,
  IN VOID               *ContextGuardPredicateList
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
  DEBUG ((EFI_D_INFO, "TSFW:registering for capsule deletion\n"));

  if(!CapGUID || !DelEvent)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->CreateEventEx (
        EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        TsDeviceDummyEventHandler,
        NULL,
        CapGUID,
        DelEvent
        );
  return Status;
}

/**
* Capsule phase of FW update
*
* @param    ImageHandle    :  UEFI imagehandle
* @param    SystemTable :  UEFI system table
* @param    pos         :  the capsule index
*
* @return EFI_STATUS
**/

EFI_STATUS
CapsulePhase(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable,
  IN UINTN            PayloadSize,
  IN VOID             *pPayload
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_STATUS StatusCFG = EFI_SUCCESS;
  ESRT_LAST_ATTEMPT_STATUS attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  ESRT_LAST_ATTEMPT_STATUS lastAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  BOOLEAN bPowerGood = FALSE;
  QCFW_UPDATE_FLAG_VALUE QcResultFlag = QCFW_UPDATE_ERR_RESOUCE_POWER_INSUFFICEIENT_BATTERY;
  UINT8 *pFWBuffer = NULL;
  UINT8 *pCFGBuffer = NULL;
  UINTN FWSize = 0;
  UINTN CFGSize = 0;
  UINTN Version = 0;
  EFI_QCOM_AUTHENTICATION_PROTOCOL *PeAuthProtocol = NULL;

  DEBUG ((EFI_D_INFO, "TSFW:entering capsule phase\n"));

   /**
   * Now payload is in PE/COFF format and signed with OEM platform
   * key. Need to authenticate the payload first
   */
  Status = gBS->LocateProtocol(&gQcomAuthenticationProtocolGuid,
                               NULL,
                               (VOID**)&PeAuthProtocol);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Device: Failed to locate PE authentication Protocol\n"));
    goto CapsulePhaseExit;
  }

  Status = PeAuthProtocol->AuthenticatePeImage(PeAuthProtocol,
                                               pPayload,
                                               PayloadSize);
  if (EFI_ERROR(Status))
  {
    if (!RETAIL)
    AsciiPrint("Authentication failed with status = 0x%x\n", Status);
    DEBUG((EFI_D_ERROR, "Authentication failed with status = 0x%x\n", Status));

    if (Status == EFI_SECURITY_VIOLATION)
    {
      attemptStatus = FW_LAST_ATMPT_STATUS_AUTHEN_ERROR;
      goto CapsulePhaseExit;
    }
    else if ((Status == EFI_LOAD_ERROR) || (Status == EFI_UNSUPPORTED))
    {
      attemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
      goto CapsulePhaseExit;
    }
    else
    {
      attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
      goto CapsulePhaseExit;
    }

  }

  /**
   * Extract the firmware image from the payload
   */
  Status = LocateFirmwareImageInPayload(&pPayload, PayloadSize);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Firmware image not found in payload\n"));
    attemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
    goto CapsulePhaseExit;
  }

  /* Check if the battery power level is sufficient for capsule update only
   if update is not already in-progress.  This allows an in-progress
   update to complete if the power level was on the boundary at the beginning
   of the update and a power cycle occurred which could prevent the
   update from completing */
  Status = FwUpdateCheckPowerLevel(&bPowerGood, &lastAttemptStatus, &QcResultFlag);
  DEBUG ((EFI_D_INFO, "TSFW:bPowerGood is %d\n", bPowerGood));
  DEBUG ((EFI_D_INFO, "TSFW:Status is %d\n", Status));

  if(Status == EFI_SUCCESS && bPowerGood == TRUE)
  {
     ParsePayload(pPayload, PayloadSize, &pFWBuffer, &FWSize, &pCFGBuffer, &CFGSize, &Version);

     TsDevicePayloadVer.FwVer = Version;
      if(TsDevicePayloadVer.FwVer >= esrt_ent.FwLowestSupportedVer)
     {
       DEBUG ((EFI_D_WARN, "TSFW:Starting firmware update\n"));
       DEBUG((EFI_D_WARN,"TsDevice: FWSize is %d \n", FWSize));
       DEBUG((EFI_D_WARN,"TsDevice: CFGSize is %d \n", CFGSize));

       PowerUpAndInitController();
       gBS->Stall(1000000);

       if(FWSize >= 1)
       {
         Status = PerformFWUpgradeProc((PUCHAR) pFWBuffer, FWSize);
          gBS->Stall(1000000);
       }

       ResetControllerForTouchOpMode();
       if(CFGSize >= 1)
       {
         StatusCFG = PerformTsCfgUpd((PUCHAR) pCFGBuffer, CFGSize);
          gBS->Stall(1000000);
       }
     }
     else
     {
       DEBUG((EFI_D_WARN,"Device: FW version is not within range of supported versions, skip update\n"));
       attemptStatus = FW_LAST_ATMPT_STATUS_INCORRECT_VERSION;
       goto CapsulePhaseExit;
     }
  }
  else
  {
       DEBUG(( EFI_D_ERROR,"\nFwUpdate Failed: Insufficient battery.\n"));
       attemptStatus  = FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY;
       goto CapsulePhaseExit;
  }


  // turn off power for TS, so HLOS can start in HID-I2C protocol mode
  // PowerOffTSGPIO();

  /* Update ESRT entry with firmware update status */
  if(Status == EFI_SUCCESS && StatusCFG == EFI_SUCCESS )
  {
     attemptStatus = FW_LAST_ATMPT_STATUS_SUCCESS;
     DEBUG((EFI_D_WARN,"TsDevice: Both FW and CFG success.\n"));
  }
  else
  {
     attemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
     DEBUG((EFI_D_WARN,"TsDevice: Both FW and CFG are not success.\n"));
  }


CapsulePhaseExit:
  Status=UpdateESRT(gSystemTable, attemptStatus);

  /* Delete capsule */
  Status = gBS->SignalEvent(TsDeviceCapsuleDeleteEvent);

  return Status;
}

/**
* parses payload - identifies payload version;FW,CFG info
*
* @param    pPayload       :  Pointer to payload
* @param    PayloadSize :  Payload size
* @param    pFWBuffer    :  pointer of FW buffer pointer
* @param    FWSize      :  pointer to FW size variable
* @param    pCFGBuffer  :  pointer to CFG buffer pointer
* @param    CFGSize       :  pointer to CFG size variable
* @param    Version     :  pointer to payload version
*
* @return None
**/

VOID
ParsePayload(
IN VOID *pPayload,
IN UINTN PayloadSize,
OUT UINT8 **pFWBuffer,
OUT UINTN *FWSize,
OUT UINT8 **pCFGBuffer,
OUT UINTN *CFGSize,
OUT UINTN *Version
)
{
   //UINTN bytes = 0;
    UINT8 *currentPointer = NULL;

   CalculateHexValue(pPayload, Version);
   currentPointer = (UINT8*)pPayload + 8;

   CalculateHexValue(currentPointer, FWSize);
    currentPointer = (UINT8*)pPayload + 16;

    CalculateHexValue(currentPointer, CFGSize);
    *pFWBuffer = (UINT8*)pPayload + 26;

    *pCFGBuffer = (UINT8*)(*pFWBuffer) + (*FWSize) + 2;
}

/**
* parses payload - identifies payload version;FW,CFG info
*
* @param    BufferPointer : Buffer pointer of Ascii data
* @param    HexValue      : Hex value of ascii data buffer
*
* @return None
**/
VOID
CalculateHexValue(
IN VOID *BufferPointer,
OUT UINTN *HexValue
)
{
   long Index = 0;
    UINT8 Value = 0;
    long result = 0;
    UINT8 Multiplyingfactor = 16;
    for(Index = 0; Index < 8; Index++)
    {
        if((*(UINT8*)((UINT8*)BufferPointer + Index)) >= '0' && (*(UINT8*)((UINT8*)BufferPointer + Index)) <= '9')
        {
            Value = (*(UINT8*)((UINT8*)BufferPointer + Index)) - '0';
           result = (result + Value) * Multiplyingfactor;
        }
        else if((*(UINT8*)((UINT8*)BufferPointer + Index)) >= 'A' && (*(UINT8*)((UINT8*)BufferPointer + Index)) <= 'F')
        {
           Value = (*(UINT8*)((UINT8*)BufferPointer + Index)) - 'A' + 10;
           result = (result + Value) * Multiplyingfactor;
        }
        if(Index == 6)
        {
           Multiplyingfactor = 1;
        }
    }
    *HexValue = result;
}


/**
  Callback to perform device firmware update.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
TsDeviceUpdateFirmware(
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  ESRT_ENTRY *ent = NULL;
  UINTN PayloadSize = 0;
  VOID *pPayload =NULL;

  DEBUG((EFI_D_INFO,"TsDevice: signal received to update device firmware...\n"));

  ent = &esrt_ent;

  if(Fw_UpdateFindCapsule(&(ent->FWClass), gSystemTable, &PayloadSize,&pPayload))
  {
    CapsulePhase(gImageHandle_TsFw,
                 gSystemTable,
                 PayloadSize,pPayload);

    /* If LoadImage() is used to load FW payload, system reset is required to avoid PCR7 measurement */
  }
  else
  {
    DEBUG((EFI_D_ERROR,"TsDevice: didn't find capsule.\n"));
  }
}


/**
  TsDeviceRegisterUpdateEvent

  Register the event for device firmware update

  @retval EFI_STATUS

**/
EFI_STATUS
TsDeviceRegisterUpdateEvent()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  DEBUG ((EFI_D_INFO, "TSFW:registering for capsule event from uefi core\n"));

  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                              TPL_CALLBACK,
                              TsDeviceUpdateFirmware,
                              NULL,
                              &UXCapOEMGuid,
                              &TsDeviceUpdateEvent
  );

  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR,"TsDevice: Error in creating OEM Update event"));
  }

  return Status;
}


/**
  TsFwUpgradeDxeInitialize ()

  @brief
  Entry function of updater
 */
EFI_STATUS
TsFwUpgradeDxeInitialize(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  ESRT_ENTRY *ent = NULL;
  UINTN PayloadSize = 0;
  VOID *pPayload = NULL;
  UINT32 OsType = OS_TYPE_OTHER;
  CHAR8  OSName[16];
  UINTN  OSNameLength   = sizeof(OSName);


  /*
   Save a local copies of ImageHandle and SystemTable
  */

  gImageHandle_TsFw = ImageHandle;
  gSystemTable = SystemTable;

  /*
   ESRT phase
  */
  Status = GetOsType (&OsType, (CHAR8*)&OSName, &OSNameLength);
  DEBUG((EFI_D_INFO, "TSFW:: OSType is detected and is %d\n", OsType));
  DEBUG((EFI_D_INFO, "TSFW:: OSNameLength is %d\n", OSNameLength));
  DEBUG((EFI_D_INFO, "TSFW:: OSName is %c\n", *OSName));
  DEBUG((EFI_D_INFO, "TSFW:: OSName is %c\n", *(OSName + 1)));
  DEBUG((EFI_D_INFO, "TSFW:: OSName is %c\n", *(OSName + 2)));
  ASSERT_EFI_ERROR(Status);

  if ((EFI_SUCCESS == Status) && (OsType == OS_TYPE_WA))
  {
    Status = EsrtPhase(SystemTable);
    DEBUG ((EFI_D_INFO, "TSFW:ESRT Phase Done\n"));


    if(EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "TSFW:: Failed to write default ESRT entry\n"));
        return Status;
    }

    ent = &esrt_ent;
    if(Fw_UpdateFindCapsule(&(ent->FWClass), gSystemTable, &PayloadSize,&pPayload ))
    {
        DEBUG ((EFI_D_INFO, "TSFW:Capsule GUID found\n"));
        /* Register capsule GUID for capsule deletion */
        Status =FwUpdateRegisterCapsuleGUID(&(ent->FWClass),&TsDeviceCapsuleDeleteEvent);
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR,"TSFW:: FwUpdateRegisterCapsuleGUID failed\n"));
            return Status;
        }

        /*
            Register event for firmware update. The event will be signaled by QCOM
            system firmware updater at BDS phase.
        */
        Status = TsDeviceRegisterUpdateEvent();
        DEBUG ((EFI_D_INFO, "TSFW:leaving Ts DXe init\n"));

    }
  }
  return Status;
}

/**
 * Determine if the battery power levels have sufficient power
 * to permit firmware update to proceed based on the pre-condition
 * checks required for UEFI firmware update:
 *    1) System must have at least 25% battery life
 *
 * For Qualcomm reference hardware (if ALLOW_NO_BATTERY_FIRMWARE_UPDATE is defined):
 *    2) or has a DC adaptor plugged in with no battery
 *
 * If the function returns EFI_SUCCESS, then *bPowerGood and *lastAttemptStatus
 * will be modified.  If bPowerGood is FALSE, then lastAttemptStatus
 * should indicate either of FW_LAST_ATMPT_STATUS_POWER_AC_NOT_CONNECTED
 * or FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY depending on
 * the power condition.
 *
 * @param bPowerGood        - Return boolean indicating there is sufficient
 *                            power to proceed with firmware update.
 * @param lastAttemptStatus - The error code to populate to the ESRT
 *                            last attempt status field if bPowerGood is
 *                            FALSE.
 * @param QcResultFlag      - The error code to return to QCFirmwareUpdate
 *                            result
 * @return EFI_STATUS
 */

EFI_STATUS
FwUpdateCheckPowerLevel(
  OUT BOOLEAN *bPowerGood,
  OUT ESRT_LAST_ATTEMPT_STATUS *lastAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_BATTERY_CHARGING_PROTOCOL  *ChargerProtocol = NULL;

  /* Parameters to get the current status of battery */
  UINT32 CurrentSOC = 0;
  UINT32 RatedCapacity = 0;
  INT32  ChargeCurrent = 0;

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE
  EFI_BATTERY_CHARGING_EXTN_PROTOCOL *ChargerExtProtocol = NULL;
  BOOLEAN bBatteryPresent = FALSE;
  EFI_CHARGER_ATTACHED_CHGR_TYPE PowerPath = EFI_CHARGER_ATTACHED_CHGR__NONE;
#endif /* ALLOW_NO_BATTERY_FIRMWARE_UPDATE */

  if ((bPowerGood == NULL) || (lastAttemptStatus == NULL) || (QcResultFlag == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto power_exit;
  }

  /* Locate Charger Protocol */
  Status = gBS->LocateProtocol(
                  &gBatteryChargerProtocolGuid,
                  NULL,
                  (VOID**) &ChargerProtocol );

  if ((Status != EFI_SUCCESS) || (ChargerProtocol == NULL))
  {
    DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: Locate EFI_BATTERY_CHARGING_PROTOCOL returned %r\n", Status));
    goto power_exit;
  }

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE
  if (!RETAIL)
  {
    /* Locate the Charger Extension Protocol, used on Qualcomm reference devices to
               check if a battery is connected and if a DC wall adaptor is connected. */
    Status = gBS->LocateProtocol( &gBatteryChargerExtnProtocolGuid,
                                  NULL,
                                  (VOID **) &ChargerExtProtocol );

    if ((Status != EFI_SUCCESS) || (ChargerExtProtocol == NULL))
    {
      DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: Locate EFI_BATTERY_CHARGING_EXTN_PROTOCOL returned %r\n", Status));

      /* Protocol not found for non-Qualcomm reference devices. Implementation required by OEM. */
      ChargerExtProtocol = NULL;
    }
  }
#endif /* ALLOW_NO_BATTERY_FIRMWARE_UPDATE */

  /* GetBatteryStatus - Get current state of charge */
  Status = ChargerProtocol->GetBatteryStatus( ChargerProtocol,
                                              &CurrentSOC,
                                              &RatedCapacity,
                                              &ChargeCurrent );

  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: GetBatteryStatus returned %r\n", Status));
    goto power_exit;
  }

  if (CurrentSOC < MINIMUM_BATTERY_CHARGE_PERCENT)
  {
    *bPowerGood = FALSE;

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE

    if (!RETAIL)
    {
      /* If Qualcomm reference hardware, check if there is no battery with DC adaptor cable plugged in,
         and if so then permit the firmware update to proceed. */
      if (ChargerExtProtocol != NULL)
      {
         if (ChargerExtProtocol->GetPowerPath(&PowerPath) != EFI_SUCCESS)
         {
           /* Failed querying if a DC adaptor is present. Return insufficient battery. */
           goto power_exit;
         }

         if (ChargerExtProtocol->GetBatteryPresence(&bBatteryPresent) != EFI_SUCCESS)
         {
           /* Failed querying if a battery is present. Return insufficient battery. */
           goto power_exit;
         }

         /* If the DC adaptor is plugged in and there is no battery, then allow the firmware update to proceed. */
         if ((PowerPath == EFI_CHARGER_ATTACHED_CHGR__DCIN) && (bBatteryPresent == FALSE))
         {
           *bPowerGood = TRUE;
         }
      }
    }
#endif

  }
  else
  {
    *bPowerGood = TRUE;
  }

power_exit:

  if ((Status == EFI_SUCCESS) && (*bPowerGood == FALSE))
  {
    *lastAttemptStatus = FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY;
    *QcResultFlag = QCFW_UPDATE_ERR_RESOUCE_POWER_INSUFFICEIENT_BATTERY;
  }

  return Status;

}
