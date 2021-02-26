/** @file  
                    MeasureBootApp.c

  This file contains modules to measure various data and code. The architecture 
  is shown below:


             MeasureSecurityVariables   
             MeasureBootVariables  |   
                         |         |       MeasureUEFIVersionId
                         |         |            |
                       MeasureVaribles          |
  MeasureAcpi------          |                  |  MeasureGPT 
  MeasureSmbios   |          |                  |      |   
  MeasureSal  |   |          |   MeasureString  |      |        
        |     |   |          |         |        |      |        MeasurePeImage
        |     |   |          |         |        |      |              |
        |     |   |          |         |        |      |              |             
        |_____|___|__________|_________|________|______|______________|
                                |  
                                |         
                      HashExtendLogTreeEvent
                                |
                                |
   TrEE Dxe layer: pEfiTrEEProtocol->HashLogExtendEvent

 
  Copyright (c) 2011 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.


**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
 04/24/14   rp       KW fix
 12/10/13   sm       Updated print statements
 08/22/13   shl      Added offline crash dump support.
 07/25/13   shl      Changed printing level.
 05/14/13   shl      Changed memory allocation to avoid using ZI data
 04/08/13   shl      IMEM base address could be 0.
 03/15/13   shl      Use config fuction call to replace PCD call
 02/26/13   shl      ClockWork fix
 02/20/13   shl      ClockWork fix
 12/10/11   shl      Warning fix 
 12/06/12   shl      Modified B-Family in MeasureUefiVersionID
 11/08/12   shl      Added support for B-Family in MeasureUefiVersionID
 10/19/12   shl      Added measure boot option and boot order.
 07/07/12   niting   Changed logging levels
 05/17/12   shl      Modified Version measurement
 05/15/12   shl      Added Support for OEM FW update 
 04/01/12   shl      Fixed some measurement for PRC 7, changed ACPI measurement
 12/05/11   shl      Fixed the UEFI version measurement
 11/27/11   shl      Added some fixes 
 07/18/11   shl      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Base.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Uefi/UefiSpec.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/ImageAuthentication.h>
#include <Library/SecurityManagementLib.h>
#include <Guid/EventGroup.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/SmBios.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/Sal.h>
#include <Guid/SalSystemTable.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/EFITrEE.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>
#include <Library/QcomLib.h>
#include <Uefi/UefiGpt.h>
#include <Protocol/BlockIo.h>
#include <Library/DevicePathLib.h>
#include "SecBoot_pkcs7.h"
#include "SecBoot_x509.h"
#include "MeasureBootLib.h"
#include "PeHashLib.h"
#include "UefiSigDb.h"
#include <Protocol/EFIAcpiPlatform.h>
#include <Library/UefiCfgLib.h>

// used for measure validation entry
#define MAX_CERT_ENTRY_SIZE 10000
UINT8  *CertEntry = NULL;
UINT32  CertEntrySize;

#define EV_EFI_VARIABLE_AUTHORITY   0x800000E0

// not found in ImageAuthentication.h, defined here.
#define EFI_SECURE_BOOT_NAME   L"SecureBoot"
#define EFI_BOOT_ORDER_NAME    L"BootOrder"
#define BOOT_OPTION_MAX_CHAR   10
#define MAX_NUM_OF_TABLE       20
#define MAX_HASH_LEN           64
#define MAX_NUM_OF_IMAGES       5
#define NUM_SECURITY_VARIABLES  5

#define EFI_OFFLINE_CRASH_DUMP   "Offline Crash Dump"

#define MEASURE_BOOT_IMEM_OFFSET    0x00000834
#define MEASURE_BOOT_IMEM_SIZE      0x00000100

// string to be measured.
#define SIZE_OF_SEPARATOR      4
UINT8 EVENT_SEPARATOR[] = {0x00, 0x00, 0x00, 0x00};
UINT8 CallingEeiApplicationFromBootOption[] = EFI_CALLING_EFI_APPLICATION;
UINT8 ReturningFromEfiApplicationFromBootOption[] = EFI_RETURNING_FROM_EFI_APPLICATOIN;
UINT8 ExitBootServicesInvocation[] = EFI_EXIT_BOOT_SERVICES_INVOCATION;
UINT8 ExitBootServicesReturnedWithFailure[] = EFI_EXIT_BOOT_SERVICES_FAILED;
UINT8 ExitBootServicesReturnedWithSuccess[] = EFI_EXIT_BOOT_SERVICES_SUCCEEDED;
UINT8 OfflineCrashDump[] = EFI_OFFLINE_CRASH_DUMP;

// for EMMC Hw Device path
extern EFI_GUID gEfiEmmcUserPartitionGuid;

// Microsoft new proposal for this structure
typedef struct {
EFI_GUID  VariableName;
UINT64    UnicodeNameLength;   
UINT64    VariableDataLength;  
CHAR16    UnicodeName[1];      
INT8      VariableData[1]; 
} EFI_VARIABLE_DATA_NEW;

//This is the shared structures between UEFI and other SBL images for B-Family.
typedef struct
{
  UINT32    ImageAddress;     // pointer(address) to the image to be hashed.
  UINT32    ImageLength;      // the image length
  UINT32    HashLength;       // hash length
  UINT8     Hash[32];         // hash value 
}HashInfoType;

// this is for security variable
typedef struct
{
  CHAR16    *VariableName;  
  EFI_GUID  *VariableGuid;
} VariableType;

VariableType SecurityVariable[NUM_SECURITY_VARIABLES] = 
{ 
  { EFI_SECURE_BOOT_NAME,         &gEfiGlobalVariableGuid },
  { EFI_PLATFORM_KEY_NAME,        &gEfiGlobalVariableGuid },
  { EFI_KEY_EXCHANGE_KEY_NAME,    &gEfiGlobalVariableGuid },
  { EFI_IMAGE_SECURITY_DATABASE,  &gEfiImageSecurityDatabaseGuid },
  { EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid }
};

// TrEE protocol
EFI_TREE_PROTOCOL  *pEfiTrEEProtocol = NULL; 

// pointer to measured security database entiries
#define   MAX_SIGNATURE_COUNT   20
UINT8    *pMeasuredSignature = NULL;
UINT32    MeasuredSignatureCount = 0;
UINT32    MeasuredSignatureLength[MAX_SIGNATURE_COUNT] = {0};
UINT32    SecurityDbSize = 0;

// system table event
EFI_EVENT SmbiosTableEvent;
EFI_EVENT SalTableEvent;

// function forward declaration
extern int secx509_check_issued
(
  secx509_cert_info_type* issuer,
  secx509_cert_info_type* subject
);


BOOLEAN OemFwUpdateFlag = FALSE;
EFI_STATUS MeasureBootSetOemFwUpdateFlag()
{
  OemFwUpdateFlag = TRUE;
  return EFI_SUCCESS;
}


/**
  This is called when the  SmbiosTableEvent is signaled. It will measure Smbios table

  @param  Event      The event which is signaled.
  @param  *Context   A pointer to VOID type.
  
  @retval Status     MeasureSmbios() return value.

**/
VOID EFIAPI SmbiosTableEventHandler0(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  EFI_STATUS   Status;
  Status = MeasureSmbios();
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, " MeasureSmbios ---, Status = (0x%p)\r\n", Status));
  }
}

VOID EFIAPI SmbiosTableEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  EFI_STATUS   Status;
  Status = MeasureUefiVersionId();
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, " MeasureUefiVersionId ---, Status = (0x%p)\r\n", Status));
  }
}

/**
  This is called when the  SmbiosTableEvent is signaled. It will measure Sal table

  @param  Event      The event which is signaled.
  @param  *Context   A pointer to VOID type.
  
  @retval Status     MeasureSal() return value.

**/
VOID EFIAPI SalTableEventHandler(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  EFI_STATUS   Status;
  Status = MeasureSal();
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, " MeasureSal ---, Status = (0x%p)\r\n", Status));
  }
}


/**
  GetVariableUtility retrieves variables from system table.
  The caller is responsible to release the memory claimed in this function.

  @param[in] *VariableName    A pointer to the variable name in wide char.  
  @param[in] *VariableGuid    A pointer to the variable guid.
  @param[in] *Variable        A pointer to the variable retrieved.  
  @param[in] *VariableSize    A pointer to the size of variable retrieved.
  
  @retval EFI_SUCCESS              Retrieve successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFI_STATUS GetVariableUtility( CHAR16 *VariableName, EFI_GUID *VariableGuid, VOID **Variable, UINTN *VariableSize )
{
  EFI_STATUS  Status;
  
  if( VariableName == NULL || VariableGuid == NULL || Variable == NULL || VariableSize == NULL )
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;    
  }

  // init data
  *VariableSize = 0;
  // locate the signature database, get database size first
  Status = gRT->GetVariable ( VariableName, VariableGuid, NULL, VariableSize, *Variable );
  if( Status != EFI_BUFFER_TOO_SMALL)
  {
    // Variable is empty, reset the Status and bail out
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  // Allocate buffer to get the variable.
  *Variable = AllocatePool(*VariableSize);
  if (*Variable == NULL)
  {
    // should never be here. Rare case.
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // Get database data.
  Status = gRT->GetVariable ( VariableName, VariableGuid, NULL, VariableSize, *Variable );
  if (EFI_ERROR (Status) ||  *Variable == NULL )
  {
    // should never be here. Rare case.
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

ErrorExit:
  return Status;
}

/**
  HashExtendLogTreeEvent composes the event date and then calls TrEE protocol 
  to hash the data, extend and log event.
 
  @param[in] *pDataToHash    A pointer to data to be hashed.  
  @param[in]  DataToHashLen  Length of data to be hashed.
  @param[in]  TreeFlags      Flag indicating how the data to be processed.  
  @param[in]  Version        TrEE event version.
  @param[in]  PCRIndex       PCR index number.  
  @param[in]  EventType      Event type to be logged.
  @param[in] *pEventData     A pointer to event data.  
  @param[in]  EventDataSize  Size of event data.
  
  @retval EFI_SUCCESS              Retrieve successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFI_STATUS HashExtendLogTreeEvent(
 IN  UINT8           *pDataToHash,
 IN  UINT32           DataToHashLen,
 IN  UINT64           TreeFlags,    
 IN  UINT32           Version,
 IN  TrEE_PCRINDEX    PCRIndex,
 IN  TrEE_EVENTTYPE   EventType,
 IN  UINT8           *pEventData,
 IN  UINT32           EventDataSize
)
{
  EFI_STATUS                     Status;
  TrEE_EVENT                    *pTreeEvent = NULL;
  UINT32                         EventSize;

  // locate the TrEE protocol
  if( pEfiTrEEProtocol == NULL ) 
  {
    Status = gBS->LocateProtocol(&gEfiTrEEProtocolGuid, NULL, (VOID **) &pEfiTrEEProtocol);
    if ( EFI_ERROR (Status) )
    {
      DEBUG(( EFI_D_INFO, "Locate TrEE protocol ---, Status = (0x%p)\r\n", Status));
      goto ErrorExit;
    }
  }

  // allocate a pool for this TreeEvent
  EventSize = sizeof(UINT32) + sizeof(TrEE_EVENT_HEADER) + EventDataSize;

  pTreeEvent = (TrEE_EVENT *)AllocatePool( EventSize);
  if( pTreeEvent == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // populate TreeEventHeader
  pTreeEvent->Header.HeaderSize = sizeof(TrEE_EVENT_HEADER);
  pTreeEvent->Header.HeaderVersion = Version; 
  pTreeEvent->Header.PCRIndex = PCRIndex; 
  pTreeEvent->Header.EventType = EventType;
  // populate TreeEvent.Event
  CopyMem( pTreeEvent->Event, pEventData, EventDataSize);
  pTreeEvent->Size = EventSize;

  Status = pEfiTrEEProtocol->HashLogExtendEvent(
                           pEfiTrEEProtocol,
                           TreeFlags,
  ( EFI_PHYSICAL_ADDRESS ) pDataToHash,
                           DataToHashLen,
                           pTreeEvent
                         );
ErrorExit:
  if( pTreeEvent != NULL )
  {
    FreePool( pTreeEvent);
  }

  return Status;
}

/**
  MeasureVariable retrieves variable from system table and composes the varible 
  event and finally calls HashExtendLogTreeEvent for further processing.
 
  @param[in] *VariableName    A pointer to the variable name in wide char.  
  @param[in] *VariableGuid    A pointer to the variable guid.
  @param[in]  TreeFlags       Flag indicating how the data to be processed.  
  @param[in]  Version         TrEE event version.
  @param[in]  PCRIndex        PCR index number.  
  @param[in]  EventType       Event type to be logged.
  @param[in] *pEventData      A pointer to event data.  
  @param[in]  EventDataSize   Size of event data.
  
  @retval EFI_SUCCESS              Retrieve successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFI_STATUS MeasureVariable(
 IN  CHAR16            *VariableName, 
 IN  EFI_GUID          *VariableGuid,
 IN  UINT64             TreeFlags,    
 IN  UINT32             TreeVersion, 
 IN  TrEE_PCRINDEX      PCRIndex,
 IN  TrEE_EVENTTYPE     EventType
)
{
  EFI_STATUS              Status;
  UINTN                   VariableSize;
  VOID                   *Variable = NULL;
  EFI_VARIABLE_DATA_NEW  *pEventData = NULL;
  UINT32                  EventDataSize;

  // retrieve variable
  Status = GetVariableUtility( VariableName, 
                               VariableGuid,
                              &Variable,
                              &VariableSize
                             );
  if ( EFI_ERROR (Status) )
  {
    if( Status != EFI_NOT_FOUND )
    {
      DEBUG(( EFI_D_INFO, "GetVariableUtility ---, Status = (0x%p)\r\n", Status));
      goto ErrorExit;
    }
    // spec says.
    VariableSize = 0;
  }

  // compose the variable event
  EventDataSize = sizeof(EFI_GUID)+ 2*sizeof(UINT64) + sizeof(CHAR16)*StrLen(VariableName) + VariableSize;
  pEventData = (EFI_VARIABLE_DATA_NEW *)AllocatePool( EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  pEventData->VariableName = *VariableGuid;
  pEventData->UnicodeNameLength = StrLen(VariableName);
  pEventData->VariableDataLength = VariableSize;
  CopyMem( (UINT8 *)(pEventData->UnicodeName),  (UINT8 *)VariableName, sizeof(CHAR16)*(pEventData->UnicodeNameLength) );
  if(  VariableSize!= 0 )
  {
    CopyMem( ((UINT8 *)pEventData) + sizeof(EFI_GUID)+ 2*sizeof(UINT64) + sizeof(CHAR16)*StrLen(VariableName), Variable, pEventData->VariableDataLength);
  }

  // pass to next layer
  Status = HashExtendLogTreeEvent( (UINT8 *)pEventData,
                                   EventDataSize,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                          (UINT8 *)pEventData,
                                   EventDataSize
                                 );
ErrorExit:
  if( Variable != NULL )
  {
    FreePool( Variable );
  }

  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;
}

/**
  Measure the UEFI version number. Per MS request, it measures the Pre-UEFI
  environment. These include hashes of pre UEFI fw images from SBL3, TZ, and
  UEFI, these hashes are calculated outside UEFI.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/

EFI_STATUS MeasureUefiVersionId( VOID )
{
  EFI_STATUS                    Status;
  UINT8                        *pDataToHash = NULL;
  UINT32                        DataToHashLen = 0;
  UINT8                        *pEventData = NULL;;
  UINT32                        EventDataSize;
  UINT64                        TreeFlags = 0;    
  UINT32                        TreeVersion = 1; 
  TrEE_PCRINDEX                 PCRIndex = 0;
  TrEE_EVENTTYPE                EventType = EV_S_CRTM_VERSION;

  UINT32                        IMemBase = 0;
  UINT32                       *IMemMeasureBootBase = NULL;
  UINT32                        IMemMeasureBootSize;
  UINT32                        NumOfHashBlock;

  // retrieve from IMEM base address.
  IMemMeasureBootSize = MEASURE_BOOT_IMEM_SIZE;
  Status = GetConfigValue ("SharedIMEMBaseAddr", &IMemBase);
  if (Status != EFI_SUCCESS)
  {
    // something wrong, force it to measure default data
    NumOfHashBlock = 100;
  }
  else
  {	  
    IMemMeasureBootBase = (UINT32 *)(UINTN)(IMemBase + MEASURE_BOOT_IMEM_OFFSET);
    // get number of blocks
    NumOfHashBlock = *IMemMeasureBootBase;
  }

  // 4 is field length for NumOfHashBlock
  DataToHashLen = NumOfHashBlock*sizeof(HashInfoType) + 4;

  // size is 256
  if( (DataToHashLen > 4 ) && (DataToHashLen <= IMemMeasureBootSize) && ( IMemMeasureBootBase != NULL))
  {
    // allocate a pool for this data to be hashed.
    pDataToHash = (UINT8 *)AllocatePool(DataToHashLen);
    if( pDataToHash == NULL )
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    };
	
    CopyMem( pDataToHash, (UINT8 *)IMemMeasureBootBase, DataToHashLen);
  
    // compose log event data
    EventDataSize = DataToHashLen; 
    pEventData = ( UINT8 *)AllocatePool(EventDataSize);
    if( pEventData == NULL )
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    CopyMem( pEventData, pDataToHash, EventDataSize);
  }
  else
  {
    // Microsoft requirement
    DataToHashLen = 20;
    // allocate a pool for this data to be hashed.
    pDataToHash = (UINT8 *)AllocatePool(DataToHashLen);
    if( pDataToHash == NULL )
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    };
	
    SetMem( pDataToHash, DataToHashLen, 0);

    // compose log event data
    EventDataSize = DataToHashLen; 
    pEventData = ( UINT8 *)AllocatePool(EventDataSize);
    if( pEventData == NULL )
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SetMem( pEventData, EventDataSize, 0);
  }

  Status = HashExtendLogTreeEvent( 
                                   pDataToHash,
                                   DataToHashLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                          (UINT8 *)pEventData,
                                   EventDataSize
                                );

  if ( EFI_ERROR(Status) )
  {
    DEBUG(( EFI_D_INFO, "Measure Version Status = (0x%p)\r\n", Status));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "Measure Version Status = (0x%p)\r\n", Status));
  }

ErrorExit:

  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }
  if( pDataToHash != NULL )
  {
    FreePool( pDataToHash );
  }

  return Status;
}


/**
  MeasureAcpi retrieves ACPI table and parses the table for getting the data
  to be hashed, composes the event data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES     Variable memory allocation failed.

**/
EFIAPI EFI_STATUS MeasureAcpi ( VOID )
{
  EFI_STATUS                                     Status; 
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp3;      
  UINT8                                         *pDataToHash = NULL;
  UINT32                                         DataToHashLen = 0;
  EFI_HANDOFF_TABLE_POINTERS                    *pEventData = NULL;;
  UINT32                                         EventDataSize;
  EFI_CONFIGURATION_TABLE                        ConfigTable;
  UINT64                                         TreeFlags = 0;    
  UINT32                                         TreeVersion = 1; 
  TrEE_PCRINDEX                                  PCRIndex = 0; //??? 1
  TrEE_EVENTTYPE                                 EventType = EV_EFI_HANDOFF_TABLES;
  EFI_QCOM_ACPIPLATFORM_PROTOCOL                *pEfiQcomAcpiPlatformProtocol = NULL;
  UINTN                                          PreFixUpTableSize;
  UINTN                                          PostFixUpTableSize;
  UINTN                                          TotalNumOfTables;

  // locate the ACPI table pointer from system table
  Status = EfiGetSystemConfigurationTable (&gEfiAcpiTableGuid, (VOID **)&Rsdp3);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "Get Acpi 3.0 ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  Status = gBS->LocateProtocol(&gQcomAcpiPlatformProtocolGuid, NULL, (VOID **) &pEfiQcomAcpiPlatformProtocol);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "Locate Protocol failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // get the pre fixup acpi table size
  Status = pEfiQcomAcpiPlatformProtocol->GetAcpiTableSize (
                                                            pEfiQcomAcpiPlatformProtocol, 
                                                           &PreFixUpTableSize, 
                                                           &PostFixUpTableSize,
                                                           &TotalNumOfTables
                                                          );
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "Get PreFixUpTableSize failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  DEBUG(( EFI_D_INFO, "PostFixUpTableSize = %d\r\n", PostFixUpTableSize));


  // successfully retrieve Rsdp3.
  // according to Uday, Xsdt never used, will consider this case in next phase.
  
  if( PreFixUpTableSize > PostFixUpTableSize )
  {	
    DataToHashLen = PreFixUpTableSize;
  }	
  else
  {
    DataToHashLen = PostFixUpTableSize;
  }

  if( DataToHashLen == 0 )
  {
    goto ErrorExit;
  }

  // allocate a pool for this data to be hashed.
  pDataToHash = (UINT8 *)AllocatePool(DataToHashLen);
  if( pDataToHash == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // get the acpi data from AcpiPlatformDxe
  Status = pEfiQcomAcpiPlatformProtocol->GetAcpiTable (
                                                        pEfiQcomAcpiPlatformProtocol,
                                                        pDataToHash,
                                                       &PreFixUpTableSize,
                                                        TRUE
                                                      );
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "get PreFixUpTableSize failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // compose Event
  EventDataSize = sizeof(UINTN) + sizeof(EFI_CONFIGURATION_TABLE); 
  pEventData = ( EFI_HANDOFF_TABLE_POINTERS * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  ConfigTable.VendorGuid = gEfiAcpiTableGuid;
  ConfigTable.VendorTable = Rsdp3;

  pEventData->NumberOfTables = TotalNumOfTables;
  CopyMem( pEventData->TableEntry, &ConfigTable, sizeof( EFI_CONFIGURATION_TABLE));

  PCRIndex = 0;

  Status = HashExtendLogTreeEvent( pDataToHash,
                                   DataToHashLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                         (UINT8 *) pEventData,
                                   EventDataSize
                                 );

  // get the acpi data from AcpiPlatformDxe
  Status = pEfiQcomAcpiPlatformProtocol->GetAcpiTable (
                                                        pEfiQcomAcpiPlatformProtocol,
                                                        pDataToHash,
                                                       &PostFixUpTableSize,
                                                        FALSE
                                                      );
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "get PostFixUpTableSize failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  PCRIndex = 1;
  Status = HashExtendLogTreeEvent( pDataToHash,
                                   DataToHashLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                         (UINT8 *) pEventData,
                                   EventDataSize
                                 );
  if ( EFI_ERROR(Status) )
  {
    DEBUG(( EFI_D_INFO, "MeasureAcpi Status = (0x%p)\r\n", Status));
  }
  else
  {
    DEBUG(( EFI_D_INFO, "MeasureAcpi Status = (0x%p)\r\n", Status));
  }

ErrorExit:

  if( pDataToHash != NULL )
  {
    FreePool( pDataToHash );
  }

  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;

}

/**
  MeasureSmbios retrieves SMBIOS table and parses the table for getting the date
  to be hashed, composes the event data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/EFI_STATUS MeasureSmbios( VOID )
{
  EFI_STATUS                    Status;
  SMBIOS_TABLE_ENTRY_POINT     *SmbiosTablePtr;
  UINT32                        EntryPointLength;
  UINT32                        TableLength;

  UINT8                        *pDataToHash = NULL;
  UINT32                        DataToHashLen = 0;
  EFI_HANDOFF_TABLE_POINTERS   *pEventData = NULL;;
  UINT32                        EventDataSize;
  EFI_CONFIGURATION_TABLE       ConfigTable;
  UINT64                        TreeFlags = 0;    
  UINT32                        TreeVersion = 1; 
  TrEE_PCRINDEX                 PCRIndex = 1;
  TrEE_EVENTTYPE                EventType = EV_EFI_HANDOFF_TABLES;

  // locate the SMBIOS pointer
  Status = EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **) &SmbiosTablePtr);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "Get Smbios ---, Status = (0x%p)\r\n", Status));
    // create an event, when this envent is signaled, Smbios table is going to be measured.
    if( Status == EFI_NOT_FOUND )
    {
      Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                  TPL_CALLBACK,
                                  SmbiosTableEventHandler0,
                                  NULL,
                                 &gEfiSmbiosTableGuid,
                                 &SmbiosTableEvent);
    }
    goto ErrorExit;
  }

  if( SmbiosTablePtr == NULL )
  {
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  // allocate a pool for this data to be hashed.
  EntryPointLength = SmbiosTablePtr->EntryPointLength;
  TableLength = SmbiosTablePtr->TableLength;
  DataToHashLen = EntryPointLength + TableLength;
  pDataToHash = (UINT8 *)AllocatePool(DataToHashLen);
  if( pDataToHash == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CopyMem( pDataToHash, (UINT8 *)SmbiosTablePtr, EntryPointLength );
  CopyMem( pDataToHash+EntryPointLength, (UINT8 *)&(SmbiosTablePtr->TableAddress), TableLength);

  // compose log event data
  EventDataSize = sizeof(UINTN) + sizeof(EFI_CONFIGURATION_TABLE); 
  pEventData = ( EFI_HANDOFF_TABLE_POINTERS * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  ConfigTable.VendorGuid = gEfiSmbiosTableGuid;
  ConfigTable.VendorTable = (UINT8 *)SmbiosTablePtr;

  pEventData->NumberOfTables = 1;
  CopyMem( pEventData->TableEntry, &ConfigTable, sizeof( EFI_CONFIGURATION_TABLE));

  Status = HashExtendLogTreeEvent( 
                                   pDataToHash,
                                   DataToHashLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                          (UINT8 *)pEventData,
                                   EventDataSize
                                );

ErrorExit:
  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;
}

/**
  MeasureSal retrieves SAL table and parses the table for getting the date
  to be hashed, composes the event data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFIAPI  EFI_STATUS MeasureSal( VOID )
{
  EFI_STATUS                    Status;
  SAL_SYSTEM_TABLE_HEADER      *SalSystemTable;

  EFI_HANDOFF_TABLE_POINTERS   *pEventData = NULL;;
  UINT32                        EventDataSize;
  EFI_CONFIGURATION_TABLE       ConfigTable;
  UINT64                        TreeFlags = 0;    
  UINT32                        TreeVersion = 1; 
  TrEE_PCRINDEX                 PCRIndex = 1;
  TrEE_EVENTTYPE                EventType = EV_EFI_HANDOFF_TABLES;

  // locate the SMBIOS pointer
  Status = EfiGetSystemConfigurationTable (&gEfiSalSystemTableGuid, (VOID **) &SalSystemTable);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "Get Sal ---, Status = (0x%p)\r\n", Status));
    // create an event, when this envent is signaled, Sal table is going to be measured.
    if( Status == EFI_NOT_FOUND )
    {
      Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                    TPL_CALLBACK,
                                    SalTableEventHandler,
                                    NULL,
                                   &gEfiSalSystemTableGuid,
                                   &SalTableEvent);
    }
    goto ErrorExit;
  }

  if( SalSystemTable == NULL )
  {
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  // compose log event data
  EventDataSize = sizeof(UINTN) + sizeof(EFI_CONFIGURATION_TABLE); 
  pEventData = ( EFI_HANDOFF_TABLE_POINTERS * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    goto ErrorExit;
  }

  ConfigTable.VendorGuid = gEfiSalSystemTableGuid;
  ConfigTable.VendorTable = (UINT8 *)SalSystemTable;

  pEventData->NumberOfTables = 1;
  CopyMem( pEventData->TableEntry, &ConfigTable, sizeof( EFI_CONFIGURATION_TABLE));

  Status = HashExtendLogTreeEvent( 
                          (UINT8 *)SalSystemTable,
                                   SalSystemTable->Length,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                          (UINT8 *)pEventData,
                                   EventDataSize
                                 );
ErrorExit:
  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;
}

/**
  MeasureSecurityVariable retrieves security related variables to hash, 
  extend and log the data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS            Measure successfully.
  @retval EFI_NOT_FOUND          Variable not found.
  @retval EFI_OUT_OF_RESOURCES   Variable memory allocation failed.

**/
EFIAPI EFI_STATUS MeasureSecurityVariables( VOID )
{
  EFI_STATUS         Status;
  UINT64             TreeFlags = 0;    
  UINT32             TreeVersion = 1; 
  TrEE_PCRINDEX      PCRIndex = 7;
  TrEE_EVENTTYPE     EventType = EV_EFI_VARIABLE_DRIVER_CONFIG;
  UINT32             i;

  for ( i=0; i< NUM_SECURITY_VARIABLES; i++ )
  {
    Status = MeasureVariable( SecurityVariable[i].VariableName, 
                              SecurityVariable[i].VariableGuid,
                              TreeFlags,
                              TreeVersion, 
                              PCRIndex,
                              EventType
                            );
    if ( EFI_ERROR (Status) )
    {
      DEBUG(( EFI_D_INFO, "Measure security variable ---, Status = (0x%p)\r\n", Status));
    }
  }

  return Status;
}


/**
  MeasureCertInBuffer retrieves the entry which is used to validate 
  PE image, to hash, extend and log the data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS            Measure successfully.
  @retval EFI_NOT_FOUND          Variable not found.
  @retval EFI_OUT_OF_RESOURCES   Variable memory allocation failed.

**/

EFIAPI VOID InitCertBufferForMeasure()
{
  // reset size
  CertEntrySize = 0;

  // check pointer
  if( CertEntry == NULL )
  {
    CertEntry =  (UINT8 *)AllocatePool( MAX_CERT_ENTRY_SIZE); 
    if( CertEntry == NULL )
    {
      DEBUG(( EFI_D_INFO, "AllocatePool( MAX_CERT_ENTRY_SIZE) failed...\r\n"));
    } 
  }
}

EFIAPI VOID StoreCertBufferForMeasure( UINT8 * Cert, UINT32 CertSize,  EFI_GUID *OwnerGuid)
{
  if( (CertEntry != NULL) && (CertEntrySize == 0 ) && ((CertSize + sizeof(EFI_GUID)) < MAX_CERT_ENTRY_SIZE )  )
  {
    CopyMem( CertEntry, (UINT8 *)OwnerGuid, sizeof(EFI_GUID));
    CopyMem( CertEntry+sizeof(EFI_GUID), Cert, CertSize);
    CertEntrySize = sizeof(EFI_GUID)+CertSize;
  }
}

EFIAPI EFI_STATUS MeasureCertInBuffer( )
{
  EFI_STATUS             Status;
  UINT64                 TreeFlags = 0;    
  UINT32                 TreeVersion = 1; 
  TrEE_PCRINDEX          PCRIndex = 7;
  TrEE_EVENTTYPE         EventType = EV_EFI_VARIABLE_AUTHORITY;
  CHAR16                *VariableName = EFI_IMAGE_SECURITY_DATABASE;
  UINTN                  VariableSize;
  VOID                  *Variable = NULL;
  EFI_VARIABLE_DATA_NEW *pEventData = NULL;
  UINT32                 EventDataSize;
  UINT32                 i;
  UINT8                 *pTemp;

  if( CertEntry == NULL || CertEntrySize == 0 )
  {
    return EFI_NOT_FOUND;
  }

  // point to buffer holding measured entries, never release
  if( pMeasuredSignature == NULL )
  {
    // retrieve variable
    Status = GetVariableUtility( 
                                (CHAR16 *) EFI_IMAGE_SECURITY_DATABASE, 
                              (EFI_GUID *)&gEfiImageSecurityDatabaseGuid,
                                          &Variable,
                                          &VariableSize
                             );
    if ( EFI_ERROR (Status) )
    {
      if( Status != EFI_NOT_FOUND )
      {  
        DEBUG(( EFI_D_INFO, "GetVariableUtility ---, Status = (0x%p)\r\n", Status));
        goto ErrorExit;
      }
    }

    SecurityDbSize = VariableSize;
    pMeasuredSignature = (UINT8 *)AllocatePool(SecurityDbSize);
    if( pMeasuredSignature == NULL )
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }
    // init it
    SetMem(pMeasuredSignature, SecurityDbSize, 0 );
  }


  // look up for measured DB entries
  pTemp = pMeasuredSignature;
  for ( i = 0; i< MeasuredSignatureCount; i++)
  {
    if( MeasuredSignatureLength[i] == CertEntrySize )
    {
      if( CompareMem( pTemp, CertEntry, CertEntrySize) == 0 )
      {
        // already measured, don't measure 
        Status = EFI_SUCCESS;
        goto ErrorExit;
      }
    }
    pTemp += MeasuredSignatureLength[i];  
  }

  // this signature never measured, measure it.

  // compose the variable event
  EventDataSize = sizeof(EFI_GUID)+ 2*sizeof(UINT64) + sizeof(CHAR16)*StrLen(VariableName) + CertEntrySize;
  pEventData = (EFI_VARIABLE_DATA_NEW *)AllocatePool( EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  pEventData->VariableName = gEfiImageSecurityDatabaseGuid;
  pEventData->UnicodeNameLength = StrLen(VariableName);
  pEventData->VariableDataLength =  CertEntrySize;
  CopyMem( (UINT8 *)(pEventData->UnicodeName),  (UINT8 *)VariableName, sizeof(CHAR16)*(pEventData->UnicodeNameLength) );
  CopyMem( ((UINT8 *)pEventData) + sizeof(EFI_GUID)+ 2*sizeof(UINT64) + sizeof(CHAR16)*StrLen(VariableName),
	       CertEntry, CertEntrySize);

  // pass to next layer
  Status = HashExtendLogTreeEvent( 
                         (UINT8 *) pEventData,
                                   EventDataSize,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                         (UINT8 *) pEventData,
                                   EventDataSize
                                 );
  if( Status == EFI_SUCCESS )
  {
    if( (pTemp-pMeasuredSignature + CertEntrySize) <= SecurityDbSize && MeasuredSignatureCount < MAX_SIGNATURE_COUNT )
    {
      CopyMem( pTemp, CertEntry, CertEntrySize );
      MeasuredSignatureLength[MeasuredSignatureCount] = CertEntrySize;
      MeasuredSignatureCount++;
    }
  }

ErrorExit:
  if( Variable != NULL )
  {
    FreePool( Variable );
  }

  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;

}

/**
  MeasureGPT retrieves GPT table and parses the table for getting the data
  to be hashed, composes the event data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFIAPI EFI_STATUS MeasureGPT( VOID )
{
  EFI_STATUS                  Status;
  EFI_BLOCK_IO_PROTOCOL      *pEmmcBlockIoProtocol = NULL;
  UINT32                      BlockSize;
  UINT32                      NumOfBlocks;
  UINT32                      MediaId;
  EFI_PARTITION_TABLE_HEADER *PartitionHeader = NULL;
  UINT8                      *Entries = NULL; 
  UINT32                      TotalEntrySize;
  UINT32                      OneBlockSize;
  UINT8                      *pDataToHash = NULL;
  UINT32                      DataToHashLen = 0;
  UINT32                      GptHeaderSize = sizeof(EFI_PARTITION_TABLE_HEADER);
  EFI_GPT_DATA               *pEventData = NULL;;
  UINT32                      EventDataSize;
  UINT64                      TreeFlags = 0;    
  UINT32                      TreeVersion = 1; 
  TrEE_PCRINDEX               PCRIndex = 5;
  TrEE_EVENTTYPE              EventType = EV_EFI_GPT_EVENT;

  UINT32                      Attribs = 0;
  HandleInfo                  HandleInfoListEmmc;
  UINT32                      MaxHandles = 1;
  PartiSelectFilter           HandleFilter;

  // locate EMMc handle.
  Attribs |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  HandleFilter.PartitionType = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, &HandleInfoListEmmc, &MaxHandles);
  if ( EFI_ERROR (Status) || (MaxHandles == 0) )
  {
    DEBUG(( EFI_D_INFO, "GetEmmcHandle ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  pEmmcBlockIoProtocol = HandleInfoListEmmc.BlkIo;
  OneBlockSize = pEmmcBlockIoProtocol->Media->BlockSize;
  MediaId      = pEmmcBlockIoProtocol->Media->MediaId;

  BlockSize = ( GptHeaderSize/OneBlockSize + 1 ) * OneBlockSize;
  PartitionHeader = ( EFI_PARTITION_TABLE_HEADER * )AllocateZeroPool (BlockSize);
  if( PartitionHeader == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = pEmmcBlockIoProtocol->ReadBlocks (
            (EFI_BLOCK_IO_PROTOCOL *)pEmmcBlockIoProtocol ,
                                     MediaId,                    
                                     PRIMARY_PART_HEADER_LBA,                    
                                     BlockSize,            
                              (VOID*)PartitionHeader
                            );
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "GetBlkIOHandles ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // calculate the size
  TotalEntrySize = PartitionHeader->NumberOfPartitionEntries * PartitionHeader->SizeOfPartitionEntry;
  NumOfBlocks = TotalEntrySize/OneBlockSize;
  if( TotalEntrySize%OneBlockSize != 0 )
  {
    NumOfBlocks++;
  }
  BlockSize = NumOfBlocks * OneBlockSize;

  Entries = ( UINT8 * )AllocateZeroPool (BlockSize);
  if( Entries == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = pEmmcBlockIoProtocol->ReadBlocks (
                                  pEmmcBlockIoProtocol,
                                  MediaId,                    
                                  PartitionHeader->PartitionEntryLBA,                    
                                  BlockSize,            
                           (VOID*)Entries
                                );
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "GetBlkIOHandles ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  DataToHashLen = GptHeaderSize + TotalEntrySize;
  pDataToHash = AllocatePool( DataToHashLen );
  if( pDataToHash == NULL )
  {
     Status = EFI_OUT_OF_RESOURCES;
     goto ErrorExit;
  }

  CopyMem( pDataToHash, PartitionHeader, GptHeaderSize);
  CopyMem( pDataToHash + GptHeaderSize, Entries, TotalEntrySize );
 
  // compose log event
  EventDataSize = GptHeaderSize + sizeof(UINTN) + TotalEntrySize; 
  pEventData = ( EFI_GPT_DATA * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // populate TreeEvent.Event
  CopyMem( &(pEventData->EfiPartitionHeader), PartitionHeader, GptHeaderSize);
  pEventData->NumberOfPartitions = PartitionHeader->NumberOfPartitionEntries; 
  CopyMem( pEventData->Partitions, Entries, TotalEntrySize);

  Status = HashExtendLogTreeEvent( pDataToHash,
                                   DataToHashLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                           (UINT8*)pEventData,
                                   EventDataSize
                                 ); 
ErrorExit:
  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  if( pDataToHash != NULL )
  {
    FreePool( pDataToHash );
  }

  if( PartitionHeader != NULL )
  {
    FreePool( PartitionHeader );
  }

  if( Entries != NULL )
  {
    FreePool( Entries );
  }

  return Status;
}


/**
  MeasureBootVariables retrieves boot related variables to hash, 
  extend and log the data.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS              Measure successfully.
  @retval EFI_NOT_FOUND            Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFIAPI EFI_STATUS MeasureBootVariables( VOID )
{
  EFI_STATUS         Status;
  UINT16             BootOption[BOOT_OPTION_MAX_CHAR];
  UINT16             BootCurrent;
  UINTN              VariableSize = sizeof(BootCurrent);
  UINT64             TreeFlags = 0;    
  UINT32             TreeVersion = 1;
  TrEE_PCRINDEX      PCRIndex = 5;
  TrEE_EVENTTYPE     EventType = EV_EFI_VARIABLE_BOOT;

  Status = gRT->GetVariable (
                             L"BootCurrent",
                             &gEfiGlobalVariableGuid,
                              0,
                             &VariableSize,
                             &BootCurrent
                            );
  if (EFI_ERROR (Status))
  {
     DEBUG(( EFI_D_INFO, "Get BootCurrent failed!  Status = (0x%p)\r\n", Status));
     goto ErrorExit;
  }

  UnicodeSPrint (BootOption, sizeof(BootOption), L"Boot%04x", BootCurrent);

  Status = MeasureVariable( (CHAR16 *) BootOption, 
                          (EFI_GUID *)&gEfiGlobalVariableGuid,
                                       TreeFlags,
                                       TreeVersion, 
                                       PCRIndex,
                                       EventType
                           );


  Status = MeasureVariable( (CHAR16 *) EFI_BOOT_ORDER_NAME, 
                          (EFI_GUID *) &gEfiGlobalVariableGuid,
                                       TreeFlags,
                                       TreeVersion, 
                                       PCRIndex,
                                       EventType
                           ); 

ErrorExit:
  return Status;
}

/**
  This function measures PE image.
  
  @param[in]  AuthenticationStatus  Points to PE/COFF image in disk format.
  @param[in]  *DevicePath           EFI_DEVICE_PATH_PROTOCOL pointer
  @param[in]  *FileBuffer           Points to PE/COFF image in disk format.
  @param[in]  FileSize              PE/COFF image size

  @retval EFI_SUCCESS           Verified successfully.
  @retval EFI_INVALID_PARAMETER Parmeter is not right or data not recoginized
  @retval EFI_UNSUPPORTED       Verification not supported.
**/

EFI_STATUS EFIAPI MeasurePeImg
(
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
)
{
  EFI_STATUS             Status;
  UINT64                 TreeFlags = 0 | PE_COFF_IMAGE;    
  UINT32                 TreeVersion = 1;
  TrEE_PCRINDEX          PCRIndex = 4;
  TrEE_EVENTTYPE         EventType = EV_EFI_BOOT_SERVICES_APPLICATION;
  EFI_IMAGE_LOAD_EVENT  *pEventData = NULL;;
  UINT32                 EventDataSize;
  UINTN                  LengthOfDevicePath = GetDevicePathSize(DevicePath);

  if ((FileBuffer == NULL) || (LengthOfDevicePath == 0))
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }
  // compose Event
  EventDataSize = sizeof(EFI_PHYSICAL_ADDRESS) + 3*sizeof(UINTN) + LengthOfDevicePath; 
  pEventData = ( EFI_IMAGE_LOAD_EVENT * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // populate TreeEvent.Event
  pEventData->ImageLocationInMemory = *((EFI_PHYSICAL_ADDRESS *)FileBuffer);
  pEventData->ImageLengthInMemory   = FileSize;
  pEventData->ImageLinkTimeAddress  = *((EFI_PHYSICAL_ADDRESS *)FileBuffer); 
  pEventData->LengthOfDevicePath    = LengthOfDevicePath; 
  CopyMem( &(pEventData->DevicePath), DevicePath, LengthOfDevicePath);

  Status = HashExtendLogTreeEvent( 
                          (UINT8 *)FileBuffer,
                                   FileSize,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                          (UINT8 *)pEventData,
                                   EventDataSize
                                 ); 

ErrorExit:
  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;

}


/**
  This is the function will be measure the entry in securityDB and PE image.
  
  @param[in]  AuthenticationStatus  Points to PE/COFF image in disk format.
  @param[in]  *File                 EFI_DEVICE_PATH_PROTOCOL pointer
  @param[in]  *FileBuffer           Points to PE/COFF image in disk format.
  @param[in]  FileSize              PE/COFF image size

  @retval EFI_SUCCESS           Verified successfully.
  @retval EFI_INVALID_PARAMETER Parmeter is not right or data not recoginized
  @retval EFI_UNSUPPORTED       Verification not supported.
**/

EFI_STATUS EFIAPI MeasurePeImgAll
(
  IN  OUT   UINT32                      AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  VOID                             *FileBuffer,
  IN  UINTN                             FileSize
)
{
  EFI_STATUS             Status;

  if( OemFwUpdateFlag == TRUE)
  {
    return EFI_SUCCESS;
  }

  Status = MeasurePeImg( AuthenticationStatus, DevicePath, FileBuffer, FileSize);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "MeasurePeImg ---, Status = (0x%p)\r\n", Status));
  }

  Status = MeasureBootVariables();
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_INFO, "MeasureBootVariables, Status = (0x%p)\r\n", Status));
  }

//  return Status;
  // no matter what, return success to avoid not loading the image.
  return EFI_SUCCESS;
}

/**
  This is the function registeres MeasureBoot as a callback in 
  security framework, it will be called at beginning of BDS phase.

  @param[in]  VOID

  @retval EFI_SUCCESS              The handlers were registered successfully.
**/

EFI_STATUS EFIAPI RegisterMeasurePeImage( VOID )
{
  EFI_STATUS  Status;
  UINT32      AuthenticationOperation = 0;
  
  // TODO: need image buffer in this case
  AuthenticationOperation |= EFI_AUTH_OPERATION_IMAGE_REQUIRED;
  AuthenticationOperation |= EFI_AUTH_OPERATION_MEASURE_IMAGE;

  Status = RegisterSecurityHandler(
         MeasurePeImgAll,
         AuthenticationOperation
       );

  return Status;
}


/**
  MeasureString will hash, extend and log a string. Most of the strings
  are action strings defined in MeasureBootLib.h
 
  @param[in] *pString           A pointer to a string to be measured.
  @param[in] StringLen          Length of a string to be measured.
  @param[in] EventType          Event type.
  @param[in] PCRIndex           PCRIndex to be extended to.
  
  @retval EFI_SUCCESS           Measure successfully.
  @retval EFI_NOT_FOUND         Variable not found.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFI_STATUS MeasureString( 
  IN  UINT8          *pString,
  IN  UINT32          StringLen,
  IN  TrEE_EVENTTYPE  EventType,
  IN  TrEE_PCRINDEX   PCRIndex
)
{
  EFI_STATUS         Status;
  UINT64             TreeFlags = 0;    
  UINT32             TreeVersion = 1;
  UINT8             *pEventData = NULL;;
  UINT32             EventDataSize;

  // compose Event
  EventDataSize = StringLen; 
  pEventData = ( UINT8 * )AllocatePool(EventDataSize);
  if( pEventData == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  // populate TreeEvent.Event
  CopyMem( pEventData, pString, StringLen);

  Status = HashExtendLogTreeEvent( pString,
                                   StringLen,
                                   TreeFlags,   
                                   TreeVersion,
                                   PCRIndex,
                                   EventType,
                                   pEventData,
                                   EventDataSize
                                 ); 
ErrorExit:
  if( pEventData != NULL )
  {
    FreePool( pEventData );
  }

  return Status;
}

/**
  MeasureXXX utilities for measuring various strings.
 
  @param[in] VOID
  
  @retval EFI_SUCCESS           Measure successfully.
  @retval EFI_OUT_OF_RESOURCES  Variable memory allocation failed.

**/
EFIAPI EFI_STATUS MeasureSeparator(VOID)
{
  return MeasureString( EVENT_SEPARATOR, SIZE_OF_SEPARATOR, EV_SEPARATOR, 7 );
}

EFIAPI EFI_STATUS MeasureSeparatorAll(VOID)
{
  EFI_STATUS  Status;
  UINT8       i;

  for( i=0; i<8; i++ )
  {
    Status = MeasureString( EVENT_SEPARATOR, SIZE_OF_SEPARATOR, EV_SEPARATOR, i);
    if ( EFI_ERROR (Status) )
    {
      DEBUG(( EFI_D_INFO, "MeasureString ---, Status = (0x%p)\r\n", Status));
    }
  }
  // this should be treated seriously, it could be misleading.
  return Status;
}

EFIAPI EFI_STATUS MeasureCallEfiAppFromBootOption(VOID)
{
  return MeasureString( 
                  CallingEeiApplicationFromBootOption,
                  sizeof(CallingEeiApplicationFromBootOption)-1,
                  EV_EFI_ACTION,
                  5
               );
}

EFIAPI EFI_STATUS MeasureReturnFromEfiAppFromBootOption(VOID)
{
  return MeasureString( 
                  ReturningFromEfiApplicationFromBootOption,
                  sizeof(ReturningFromEfiApplicationFromBootOption)-1,
                  EV_EFI_ACTION,
                  5  
               );
}

EFIAPI EFI_STATUS MeasureExitBootServiceSuccess(VOID)
{
  return MeasureString( 
                  ExitBootServicesReturnedWithSuccess,
                  sizeof(ExitBootServicesReturnedWithSuccess)-1,
                  EV_EFI_ACTION,
                  5  
               );
}

EFIAPI EFI_STATUS MeasureExitBootServiceFailure(VOID)
{
  return MeasureString( 
                  ExitBootServicesReturnedWithFailure,
                  sizeof(ExitBootServicesReturnedWithFailure)-1,
                  EV_EFI_ACTION,
                  5  
              );
}

EFIAPI EFI_STATUS MeasureExitBootServiceInvocation(VOID)
{
  return MeasureString( 
                  ExitBootServicesInvocation,
                  sizeof(ExitBootServicesInvocation)-1,
                  EV_EFI_ACTION,
                  5  
              );
};

EFIAPI EFI_STATUS MeasureOfflineCrashDump(VOID)
{
  return MeasureString( 
                  OfflineCrashDump,
                  sizeof(OfflineCrashDump)-1,
                  EV_EFI_ACTION,
                  7  
              );
};
