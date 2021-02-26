/** @file
  Qualcomm ACPI Platform Driver

  Copyright (c) 2011-2017, Qualcomm Technologies Inc. All rights
  reserved. 
  Portions Copyright (c) 2008 - 2010, Intel Corporation. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
   
**/ 

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/13/17   ltg     Add MSDM table loading
 02/04/16   wayne   fix 64bit: ACPI table entries are not updated
                    in acpiplatformdxe driver.  
 10/01/15   wayne   Optimize ZI region size to reduce footprint
 09/24/15   wayne   Clean kw warning
 07/09/15   wayne   Added for Chip Serial number callback.
 04/23/15   vk      Add UFS support
 03/20/15   wayne   Added Acpi chip info callback
 06/17/14   swi     Updated AML register test
 04/17/14   swi     Fixed KW warning
 03/26/14   swi     Changed output to EFI_D_WARN as appropriate
                    Fixed 64-bit warnings
 02/19/14   swi     Implemented callback mechanism to update AML variables in DSDT
                    Added test functionality and test callbacks 
                    Changed ReadSize type to UINTN (64-bit compatible)
 01/16/14   vk      Remove InCarveOut
 08/12/13   sahn    Implemented callback mechanism to update ACPI tables
 07/22/13   zzx     Added register interface to enable functionality of ACPI table 
                    query/update with ACPITableLoadEvent.
 04/10/13   xc      remove PcdTZStorageTablesBaseOffset, read mem address from cfg file
 02/04/13   shl     Re-arrange the order get appliation ID and set IMEM bit.
 01/09/13   vk      Fix KlockWork warnings
 12/10/12   shl     Moved ACPI_TPM2_TABLE out to EFITree.h.
 09/14/12   bmuthuku Added logic to check if TPM is enabled or Disabled
 07/07/12   niting  Changed logging levels
 03/30/12   shl     Added functions for store acpi before and after fixup. This 
                    is requred by Microsoft
 03/06/12   jz      Updated to use PcdTZStorageTablesBaseOffset
 02/29/11   eamonn  Allocated TPM data at top of SCM region
 11/21/11   vishalo Load ACPI tables from specified partitions only
 11/21/11   shl     Added fTPM support 
 08/30/11   niting  Load ACPI tables in BDS.
 05/19/11   niting  Creation.

=============================================================================*/

#include <AcpiPlatform.h>
#include <AcpiPlatformChipInfo.h>
#include <Library/BootConfig.h>

EFI_QCOM_ACPIPLATFORM_PROTOCOL AcpiPlatformProtocolImpl =
{
  ACPIPLATFORM_REVISION,
  GetAcpiTable,
  GetAcpiTableSize,
  AcpiTableRegister,
  AmlVariableRegister
};

EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL AcpiPlatformTestProtocolImpl =
{
  ACPIPLATFORMTEST_REVISION,
  AcpiTableRegisterTest
};

// Acpi table measurement support
AcpiTableEntry           *pPreFixArray;
AcpiTableEntry           *pPostFixArray;
UINTN                    NumOfAcpiTables = 0;

// AML Variable arrays
AmlVariableEntry           *pPreFixAmlVariableArray;//[MAX_AML_VARIABLES] = {

AmlVariableEntry    *pPostFixAmlVariableArray;//[MAX_AML_VARIABLES] = 

UINTN                      NumOfAmlVariables = 0;

#define MAX_ACPI_REGISTERS  256
static RegisterTableType    *pRegisteredCallbacks;//[MAX_ACPI_REGISTERS];
static UINT32               NumRegisteredCallbacks = 0;

#define MAX_AML_REGISTERS   256
static RegisterAmlVariableTable    *pRegisteredAmlVariableCallbacks;//[MAX_AML_REGISTERS];
static UINT32                      NumRegisteredAmlVariableCallbacks = 0;

CHAR8 *AMLVARIABLENAME_ALL = "ALL";

STATIC CHAR8                     *pFileName;//[MAX_PATHNAME];
STATIC CHAR8                     *pFilePath;//[MAX_PATHNAME];
STATIC HandleInfo                HandleInfoList[2];
STATIC PartiSelectFilter         HandleFilter;

EFI_EVENT ACPITableLoadEvent  = NULL;
static BOOLEAN RegistrationClosed = FALSE;
extern EFI_GUID gEfiACPITableLoadGuid;

//Root Partition
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiUfsLU4Guid;

//Partition Types
extern EFI_GUID gEfiPartTypeSystemPartGuid;
extern EFI_GUID gEfiPlatPartitionTypeGuid;


const AMLRegisterStruct AMLRegisterInfo[]=
{
    {{'S','O','I','D'},GETMODE_PREFIX,NULL,AmlUpdateChipId},
    {{'S','I','D','S'},GETMODE_PREFIX,NULL,AmlUpdateChipChipIdString},
    {{'S','I','D','V'},GETMODE_PREFIX,NULL,AmlUpdateChipVersion},
    {{'S','V','M','J'},GETMODE_PREFIX,NULL,AmlUpdateMajorChipVersion},
    {{'S','V','M','I'},GETMODE_PREFIX,NULL,AmlUpdateMinorChipVersion},
    {{'S','D','F','E'},GETMODE_PREFIX,NULL,AmlUpdateChipFamily},
    {{'S','I','D','M'},GETMODE_PREFIX,NULL,AmlUpdateModemInfo},
    {{'S','O','S','N'},GETMODE_PREFIX,NULL,AmlUpdateChipSerialNum}
};


#define CHECK_NULL_POINTER_ABORT_IF_TRUE(p)   \
if(p == NULL)  \
{                       \
    return EFI_ABORTED;  \
}                       

#define CHECK_NULL_POINTER_CLEAN_IF_TRUE(p,d)  \
if(p == NULL)  \
{                       \
    goto d;     \
}

#define SET_ACPI_TABLE_ENTRY_LIMITS(val)        \
if(NumOfAcpiTables > MAX_ACPI_TABLES)               \
{                                                   \
  val = MAX_ACPI_TABLES;                    \
  DEBUG(( EFI_D_WARN, "Acpi Table Entry are over limitation.\r\n"));    \
}                                                   \
else                                                \
{                                                   \
  val = NumOfAcpiTables;                    \
}                                                   

#define SET_REGISTER_CALLBACK_LIMITS(val)       \
if(NumRegisteredCallbacks > MAX_ACPI_REGISTERS)                         \
{                                                                       \
  val = MAX_ACPI_REGISTERS;                                             \
  DEBUG(( EFI_D_WARN, "Callbacks registerd are over limitation.\r\n")); \
}                                                                       \
else                                                                    \
{                                                                       \
    val = NumRegisteredCallbacks;                                       \
}


#define SET_AML_VARIABLE_ARRAY_LIMITS(val)      \
if(NumOfAmlVariables > MAX_AML_VARIABLES)                           \
{                                                                       \
  val = MAX_AML_VARIABLES;                                              \
  DEBUG(( EFI_D_WARN, "Callbacks registerd are over limitation.\r\n")); \
}                                                                       \
else                                                                    \
{                                                                       \
    val = NumOfAmlVariables;                                        \
}

#define SET_AML_REGISTER_CALLBACK_LIMITS(val)       \
if(NumRegisteredAmlVariableCallbacks > MAX_AML_REGISTERS)                         \
{                                                                       \
  val = MAX_AML_REGISTERS;                                             \
  DEBUG(( EFI_D_WARN, "Callbacks registerd are over limitation.\r\n")); \
}                                                                       \
else                                                                    \
{                                                                       \
    val = NumRegisteredAmlVariableCallbacks;                                       \
}


EFI_STATUS ACPIPlatformAllocatePool(void)
{
    EFI_STATUS Status = EFI_SUCCESS;

  if(pPreFixArray ==NULL)
      pPreFixArray = (AcpiTableEntry *) AllocatePool(MAX_ACPI_TABLES*sizeof(AcpiTableEntry));
  
  if(pPostFixArray ==NULL)
      pPostFixArray= (AcpiTableEntry *) AllocatePool(MAX_ACPI_TABLES*sizeof(AcpiTableEntry));

  if(pPreFixAmlVariableArray ==NULL)
    pPreFixAmlVariableArray  = (AmlVariableEntry *) AllocatePool(MAX_AML_VARIABLES*sizeof(AmlVariableEntry)); 
  
  if(pPostFixAmlVariableArray ==NULL) 
    pPostFixAmlVariableArray = (AmlVariableEntry *) AllocatePool(MAX_AML_VARIABLES*sizeof(AmlVariableEntry)); 
  
  if(pRegisteredCallbacks ==NULL)
    pRegisteredCallbacks  = (RegisterTableType *) AllocatePool(MAX_ACPI_REGISTERS*sizeof(RegisterTableType));
  
  if(pRegisteredAmlVariableCallbacks ==NULL)
    pRegisteredAmlVariableCallbacks = (RegisterAmlVariableTable *) AllocatePool(MAX_AML_REGISTERS*sizeof(RegisterAmlVariableTable));
  
  if(pFileName ==NULL)
    pFileName = (CHAR8 *) AllocatePool(MAX_PATHNAME);
  
  if(pFilePath ==NULL)
    pFilePath = (CHAR8 *) AllocatePool(MAX_PATHNAME);

  if((pPreFixArray ==NULL)
    ||(pPostFixArray ==NULL)
    ||(pPreFixAmlVariableArray ==NULL)
    ||(pPostFixAmlVariableArray ==NULL)
    ||(pRegisteredCallbacks ==NULL)
    ||(pRegisteredAmlVariableCallbacks ==NULL)
    ||(pFileName ==NULL)
    ||(pFilePath ==NULL))
  {  
        if(pPreFixArray!=NULL)
            FreePool(pPreFixArray);

        if(pPostFixArray!=NULL)
            FreePool(pPostFixArray);

        if(pPreFixAmlVariableArray!=NULL)
            FreePool(pPreFixAmlVariableArray);  

        if(pPostFixAmlVariableArray!=NULL)
            FreePool(pPostFixAmlVariableArray); 

        if(pRegisteredCallbacks!=NULL)
            FreePool(pRegisteredCallbacks);

        if(pRegisteredAmlVariableCallbacks!=NULL)
            FreePool(pRegisteredAmlVariableCallbacks);

        if(pFileName!=NULL)
            FreePool(pFileName); 

        if(pFilePath!=NULL)
            FreePool(pFilePath );
        
        Status = EFI_OUT_OF_RESOURCES;
  }
  return Status;
}

void ACPIPlatformFreePool(void)
{

    if(pPreFixArray!=NULL)
    {
        FreePool(pPreFixArray);
        pPreFixArray = NULL;
    }

    if(pPostFixArray!=NULL)            
    {
        FreePool(pPostFixArray);
        pPostFixArray = NULL;
    }

    if(pPreFixAmlVariableArray!=NULL)          
    {
        FreePool(pPreFixAmlVariableArray);
        pPreFixAmlVariableArray = NULL;
    }

    if(pPostFixAmlVariableArray!=NULL)          
    {
        FreePool(pPostFixAmlVariableArray);
        pPostFixAmlVariableArray = NULL;
    }

    if(pRegisteredCallbacks!=NULL)          
    {
        FreePool(pRegisteredCallbacks);
        pRegisteredCallbacks = NULL;
    }

    if(pRegisteredAmlVariableCallbacks!=NULL)          
    {
        FreePool(pRegisteredAmlVariableCallbacks);
        pRegisteredAmlVariableCallbacks = NULL;
    }

    if(pFileName!=NULL)          
    {
        FreePool(pFileName);
        pFileName = NULL;
    }
    
    if(pFilePath!=NULL)          
    {
        FreePool(pFilePath );
        pFilePath = NULL;
    }

}

/**
  Print AML variables in either Prefix or PostFix

  @param  GetMode                GETMODE value to be printed

  @return EFI_SUCCESS            Successful
  @return EFI_INVALID_PARAMETERS GetMode is invalid
**/
EFI_STATUS
PrintAmlVariables( GETMODE GetMode )
{ 
    UINTN i;
    UINTN j;
    UINT8* BytePtr;
    UINT8 ByteValue;
    AmlVariableEntry *CurrentAmlVariableArray;
    if (GetMode != GETMODE_PREFIX && GetMode != GETMODE_POSTFIX)
    return EFI_INVALID_PARAMETER;

    CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixAmlVariableArray);
    CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixAmlVariableArray);

    if (NumOfAmlVariables == 0)
    {
        return EFI_SUCCESS;
    }


    if (GetMode == GETMODE_PREFIX)
    {
        CurrentAmlVariableArray = pPreFixAmlVariableArray;
        DEBUG ((ACPI_PLATFORM_DEBUG_PRINT, "Prefix variable values:\r\n"));
    }
    else
    {
        CurrentAmlVariableArray = pPostFixAmlVariableArray;
        DEBUG ((ACPI_PLATFORM_DEBUG_PRINT, "Postfix variable values:\r\n"));
    }
    if(NumOfAmlVariables>MAX_AML_VARIABLES)
    {
        return EFI_SUCCESS;
    }
    for(i=0; i<NumOfAmlVariables; i++)
    {
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"=================================\r\n"));
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "  Name: "));
        for(j=0; j<AML_NAMESTRING_LENGTH; j++)
          DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "%c", CurrentAmlVariableArray[i].AmlVariableName[j]));
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT," Length: %d Type:0x%02x\r\n  Value: ", CurrentAmlVariableArray[i].AmlVariableSize,
          ((AmlVariableEncoding*)CurrentAmlVariableArray[i].AmlVariable)->AmlVariableDataType));
        // Length of payload = AmlVariableSize-AML_NAMESPACE_HEADER_SIZE
        BytePtr = (UINT8*)((AmlVariableEncoding*)CurrentAmlVariableArray[i].AmlVariable);
        for(j=AML_NAMESPACE_HEADER_SIZE;j<CurrentAmlVariableArray[i].AmlVariableSize; j++)
        {
          ByteValue = *(BytePtr+j);
          DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"%x", ByteValue));
        }
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"\r\n"));
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"=================================\r\n"));
    }
  return EFI_SUCCESS;
}
/**
  Store an ACPI table.
  Never release the allocated memory. When boot time is finished, the memory
  will be collected.

  @param  *Table                Pointer to the table to be copied
  @param   TableSize            Size of the table to be copied
  @param   Signature            Signature of the table to be copied
  @param   Revision             Revision of the table to be copied
  @param  *ArrayEntry           Pointer to the address the table to be copied

  @return EFI_SUCCESS           Successful
  @return EFI_OUT_OF_RESOURCES  Could not allocate memory to make a copy of the table.
  @return EFI_INVALID_PARAMETERS Table is NULL, TableSize is 0, Signature is 0, or 
                                ArrayEntry is NULL.

**/
EFI_STATUS
StoreAcpi( VOID *Table, UINTN TableSize, UINT32 Signature, AcpiTableEntry *ArrayEntry )
{
  if( Table == NULL || TableSize == 0 || Signature == 0 || ArrayEntry == NULL)
  {
    DEBUG(( DEBUG_ERROR, "StoreAcpi called with invalid parameters.\r\n",TableSize));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "TableSize = %d\r\n",TableSize));
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "Table Signature = %x\r\n",Signature));

  // Allocate.
  ArrayEntry->Table = (VOID *)AllocatePool(TableSize);
  if ( ArrayEntry->Table == NULL)
  {
    DEBUG(( DEBUG_ERROR, "StoreAcpi failed to allocate memory.\r\n",TableSize));
    return EFI_OUT_OF_RESOURCES;
  }    

  // copy table data.
  CopyMem( ArrayEntry->Table, Table, TableSize );
  ArrayEntry->TableSize = TableSize;
  ArrayEntry->Signature = Signature;

  return EFI_SUCCESS;
}

/**
  Store an Aml Variable in a table.
  Never release the allocated memory. When boot time is finished, the memory
  will be collected.

  @param  *AmlVariable                Pointer to the variable to be copied
  @param   AmlVariableSize            Size of the variable buffer to be copied
  @param  *AmlVariableName            Name of variable to be copied
  @param  *ArrayEntry                 Pointer to the address of the array to be copied into

  @return EFI_SUCCESS                 Successful
  @return EFI_OUT_OF_RESOURCES        Could not allocate memory to make a copy of the table.
  @return EFI_INVALID_PARAMETERS      AmlVariable is NULL, AmlVariableSize is 0, 
                                      AmlVariableName is NULL, or ArrayEntry is NULL.
**/
EFI_STATUS
StoreAmlVariable( VOID *AmlVariable, UINTN AmlVariableSize, 
  CHAR8 AmlVariableName[AML_NAMESTRING_LENGTH], UINTN TableOffset, 
  AmlVariableEntry *ArrayEntry )
{
  UINTN i;
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "=========Store Aml Var Start==========\r\n"));
  if( AmlVariable == NULL || AmlVariableSize == 0 || AmlVariableName == NULL || ArrayEntry == NULL)
  {
    DEBUG(( DEBUG_ERROR, "StoreAmlVariable called with invalid parameters.\r\n", AmlVariableSize));
    return EFI_INVALID_PARAMETER;
  }
  
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "AmlVariable Name = %c%c%c%c\r\n",AmlVariableName[0],AmlVariableName[1],AmlVariableName[2],AmlVariableName[3]));
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "AmlVariable Size = %d\r\n",AmlVariableSize));
  // Allocate.
  ArrayEntry->AmlVariable = (VOID *)AllocatePool(AmlVariableSize);
  if ( ArrayEntry->AmlVariable == NULL)
  {
    DEBUG(( DEBUG_ERROR, "StoreAcpi failed to allocate memory.\r\n",AmlVariableSize));
    return EFI_OUT_OF_RESOURCES;
  }    

  // copy AmlVariable data.
  CopyMem( ArrayEntry->AmlVariable, AmlVariable, AmlVariableSize );
  ArrayEntry->AmlVariableSize = AmlVariableSize;
  ArrayEntry->TableOffset = TableOffset;
  CopyMem( ArrayEntry->AmlVariableName, AmlVariableName, AML_NAMESTRING_LENGTH );
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "Storing variable:"));  
  for(i=0; i<AML_NAMESTRING_LENGTH; i++)    
   DEBUG((ACPI_PLATFORM_DEBUG_PRINT,"%c", AmlVariableName[i] ));
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "=========Store Aml Var End==========\r\n"));

  return EFI_SUCCESS;
}

/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param  Instance      Return pointer to the first instance of the protocol

  @return EFI_SUCCESS           The function completed successfully.
  @return EFI_NOT_FOUND         The protocol could not be located.
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

**/
EFI_STATUS
LocateFvInstanceWithTables (
  OUT EFI_FIRMWARE_VOLUME2_PROTOCOL **Instance
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FvInstance;

  FvStatus = 0;

  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiFirmwareVolume2ProtocolGuid,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
  if (EFI_ERROR (Status)) {
    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }



  //
  // Looking for FV with ACPI storage file
  //

  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiFirmwareVolume2ProtocolGuid,
                     (VOID**) &FvInstance
                     );
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Status = FvInstance->ReadFile (
                           FvInstance,
                           (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                           NULL,
                           &Size,
                           &FileType,
                           &Attributes,
                           &FvStatus
                           );

    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      *Instance = FvInstance;
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //

  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  return Status;
}


/**
  This function calculates and updates an UINT8 checksum.

  @param  Buffer          Pointer to buffer to checksum
  @param  Size            Number of bytes to checksum

**/
VOID
AcpiPlatformChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8(Buffer, Size);
}

/**
  This function loads ACPI files from the specified FolderPath.

  @param  FolderPath      String for path of ACPI folder

**/
EFI_STATUS
LoadFromFilesystem(
  CHAR8 *FolderPath
  )
{
  EFI_STATUS                    Status         = EFI_LOAD_ERROR;
  EFI_OPEN_FILE                 *Dir           = NULL;
  EFI_OPEN_FILE                 *File          = NULL;
  EFI_FILE_INFO                 *DirInfo       = NULL;
  UINTN                         ReadSize       = 0;
  VOID                          *FileBuffer    = NULL;
  UINTN                         FileBufferSize = 0;
  UINTN                         TableSize;
  UINT32                        Signature;
  UINTN                         TableCount     = 0;

  // initialize the table count
  NumOfAcpiTables = 0;

  /* Attempt to load from any available filesystem */
  Dir = EfiOpen (FolderPath, EFI_FILE_MODE_READ, 0);
  if (Dir == NULL) {
    goto Done;
  }

  if ((Dir->Type == EfiOpenFileSystem) || (Dir->Type == EfiOpenBlockIo)) {
    if (Dir->FsFileInfo ==  NULL) {
      goto Done;
    }

    if (!(Dir->FsFileInfo->Attribute & EFI_FILE_DIRECTORY)) {
      goto Done;
    }

    Dir->FsFileHandle->SetPosition (Dir->FsFileHandle, 0);
    while (1) {
      // First read gets the size
      ReadSize = 0;
      Status = Dir->FsFileHandle->Read (Dir->FsFileHandle, &ReadSize, DirInfo);
      if (Status == EFI_BUFFER_TOO_SMALL) {
        // Allocate the buffer for the real read
        DirInfo = AllocatePool (ReadSize);
        if (DirInfo == NULL) {
          goto Done;
        }
        
        // Read the data
        Status = Dir->FsFileHandle->Read (Dir->FsFileHandle, &ReadSize, DirInfo);
        if ((EFI_ERROR (Status)) || (ReadSize == 0)) {
          goto Done;
        }
      } else {
        goto Done;
      }
      
      if (DirInfo->Attribute & EFI_FILE_DIRECTORY) {
        // Silently skip folders
        FreePool (DirInfo);
        DirInfo = NULL;
        continue;
      }
      
      if((pFilePath==NULL)||(pFileName==NULL)) 
      {      
          goto Done;
      }
      
      AsciiStrnCpy (pFilePath, FolderPath, MAX_PATHNAME);
      UnicodeStrToAsciiStr (DirInfo->FileName, pFileName);
      AsciiStrnCat (pFilePath, pFileName, MAX_PATHNAME);

      DEBUG ((DEBUG_LOAD, "ACPI: Loading %a from FS.\r\n", pFilePath));

      File = EfiOpen (pFilePath, EFI_FILE_MODE_READ, 0);
      if (File == NULL) {
        goto Done;
      }

      Status = EfiReadAllocatePool (File, &FileBuffer, &FileBufferSize);

      if ((EFI_ERROR (Status)) 
        ||(FileBufferSize == 0)
        ||(FileBuffer == NULL)
        ||(pPreFixArray == NULL)) 
      {
        goto Done;
      }
        
      //
      // Add the table
      //
      TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) FileBuffer)->Length;
      ASSERT (FileBufferSize >= TableSize);
      Signature = ((EFI_ACPI_DESCRIPTION_HEADER *) FileBuffer)->Signature;
     
      // Checksum ACPI table
      AcpiPlatformChecksum ((UINT8*)FileBuffer, TableSize);
      if(NumOfAcpiTables>=MAX_ACPI_TABLES)
      {
          DEBUG((EFI_D_WARN, "StoreAcpi failed. Signature: %x . Too many tables to handle.\r\n",Signature));
          goto Done;
      }
      // store for measurement
      Status = StoreAcpi( FileBuffer, TableSize, Signature, &pPreFixArray[NumOfAcpiTables] );
      if (EFI_ERROR (Status)) 
      {
        DEBUG((EFI_D_WARN, "StoreAcpi failed. Signature: %x, Status: %d.\r\n",
          pPreFixArray[NumOfAcpiTables].Signature, Status));
        goto Done;
      }

      // increase the table count
      if(NumOfAcpiTables<MAX_ACPI_TABLES)
      {      
        NumOfAcpiTables++;
      }
      else
      {
        DEBUG((EFI_D_WARN, "StoreAcpi. Too many tables to handle.\r\n"));
      }

      TableCount++;

      EfiClose (File);
      File = NULL;
      FreePool (DirInfo);
      DirInfo = NULL;
      FileBuffer = NULL;
    }
  }

Done:
  if (0 == TableCount) {
    Status = EFI_NOT_FOUND;
  }
  else {
    DEBUG ((DEBUG_LOAD, "ACPI: Loaded %d tables\r\n", TableCount));
  }

  if (Dir != NULL) {
    EfiClose (Dir);
  }
  if (File != NULL) {
    EfiClose (File);
  }
  if (DirInfo != NULL) {
    FreePool (DirInfo);
  }

  return Status;
}

/**
  Loads ACPI from a file path specified by PCD

  @return EFI_ABORTED if any error
  @return EFI_SUCCESS if successful

**/
EFI_STATUS
LoadFromImage (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol;
  INTN                           Instance;
  EFI_ACPI_COMMON_HEADER         *CurrentTable;
  UINT32                         FvStatus;
  UINTN                          TableSize;
  UINT32                         Signature;
  UINTN                          Size;

  Instance     = 0;
  CurrentTable = NULL;

  // initialize the table count
  NumOfAcpiTables = 0;

  //
  // Locate the firmware volume protocol
  //
  Status = LocateFvInstanceWithTables (&FwVol);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);

    
  //
  // Read tables from the storage file.
  //
  while (Status == EFI_SUCCESS) {

    Status = FwVol->ReadSection (
                      FwVol,
                      (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID**) &CurrentTable,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR(Status)) {
      //
      // Add the table
      //

      TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Length;
      ASSERT (Size >= TableSize);
      Signature = ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Signature;

      // Checksum ACPI table
      AcpiPlatformChecksum ((UINT8*)CurrentTable, TableSize);
      if(NumOfAcpiTables >= MAX_ACPI_TABLES)
      {
        return EFI_ABORTED;
      }
      // store for measurement
      Status = StoreAcpi( CurrentTable, TableSize, Signature, &pPreFixArray[NumOfAcpiTables] );
      if (EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, " StoreAcpi failed. Signature: %x, Status: %d.\r\n",
          pPreFixArray[NumOfAcpiTables].Signature, Status));
        return Status;
      }
      
      if(NumOfAcpiTables < MAX_ACPI_TABLES)
      {
          // increase the table count
          NumOfAcpiTables++;
      }
      else
      {
          return EFI_ABORTED;
      }

      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  DEBUG ((DEBUG_LOAD, "ACPI: Loaded %d tables\r\n", Instance));

  return EFI_SUCCESS;
}

/**
  Loads ACPI from a partition specified by Root Device type GUID 
  Partition Type GUID and Removable or Non-removable media
  
  @param  Path            Directory containging ACPI table files
  @param  RootDeviceType  Root Device Type GUID 
  @param  Partition Type  GUID eg: gEfiPartTypeSystemPartGuid
  @param  SelectNonRemovable  
            TRUE for Non-removable 
            FALSE for Removable
  @return EFI_SUCCESS if successful
**/
 
EFI_STATUS
LoadAcpiFromVolume(
  CHAR16        *Path,
  EFI_GUID      *RootDeviceType,
  EFI_GUID      *PartitionType,
  BOOLEAN       SelectNonRemovable
)
{
  EFI_STATUS Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   RootFileHandle = NULL;
  EFI_FILE_HANDLE                   DirFileHandle = NULL;
  EFI_FILE_HANDLE                   AcpiFileHandle = NULL;
  EFI_FILE_INFO                     *DirInfo = NULL, *Info = NULL;
  EFI_OPEN_FILE                     *AcpiFile = NULL;
  VOID                              *AcpiFileBuffer = NULL;
  UINTN                             AcpiFileBufferSize = 0, BufferSize = 0;
  UINTN                             ReadSize = 0;
  UINTN                             TableSize = 0;
  UINT32                            Signature = 0;
  UINTN                             TableCount = 0;
  UINT32                            MaxHandles = 0;
  UINT32                            Attrib = 0;
  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  if( ( (RootDeviceType == NULL) && (PartitionType == NULL) ) || (Path == NULL) ) 
    return EFI_INVALID_PARAMETER;

  // initialize the table count
  NumOfAcpiTables = 0;

  Attrib |= BLK_IO_SEL_PARTITIONED_GPT;

  if (SelectNonRemovable == TRUE)
    Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;

  if (RootDeviceType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  if (PartitionType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  HandleFilter.PartitionType = PartitionType;
  HandleFilter.RootDeviceType = RootDeviceType;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attrib, &HandleFilter, HandleInfoList, &MaxHandles);

  if(Status == EFI_SUCCESS) {
    if(MaxHandles == 0) {
      return EFI_NO_MEDIA;
    }
    if(MaxHandles != 1) {
      //Unable to deterministically load from single partition 
      DEBUG(( EFI_D_WARN, "LoadAcpiFromVolume(Path: %s): multiple partitions found \r\n", Path));
      return EFI_LOAD_ERROR;
    }
  }

  // File the file system interface to the device
  Status = gBS->HandleProtocol (
                  HandleInfoList[0].Handle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID *) &Volume
                  );
  if(Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_WARN, "ACPI: LoadAcpiFromVolume() unable to find file system interface.\r\n" ));
    return Status;
  }

  // Open the root directory of the volume
  if (!EFI_ERROR (Status)) {
    Status = Volume->OpenVolume (
                      Volume,
                      &RootFileHandle
                      );
  }
  if(Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_WARN, "ACPI: LoadAcpiFromVolume() failed to open volume \r\n" ));
    return Status;
  }
  
  //Open Directory 
  Status = RootFileHandle->Open(RootFileHandle,
                   &DirFileHandle,
                   Path,
                   EFI_FILE_MODE_READ,
                   0);
  if ((Status != EFI_SUCCESS) || (DirFileHandle == NULL))
  {
    DEBUG ((EFI_D_WARN, "ACPI: LoadAcpiFromVolume() failed to open directory\r\n" ));
    return Status;
  }
  
  Status = DirFileHandle->SetPosition(DirFileHandle, 0);
  if(Status != EFI_SUCCESS)
     return Status;
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  

  while (1) {
      // Look at each directory listing
      // First read gets the size
      ReadSize = 0;
      Status = DirFileHandle->Read (DirFileHandle, &ReadSize, DirInfo);
      if (Status == EFI_BUFFER_TOO_SMALL) {
        // Allocate the buffer for the real read
        DirInfo = AllocatePool (ReadSize);
        if (DirInfo == NULL) {
          goto Done;
        }
        
        // Read the data
        Status = DirFileHandle->Read (DirFileHandle, &ReadSize, DirInfo);
        if ((EFI_ERROR (Status)) || (ReadSize == 0)) {
            goto Done;
        }
      } else {
        goto Done;
      }
      
      if (DirInfo->Attribute & EFI_FILE_DIRECTORY) {
        // Silently skip folders
        FreePool (DirInfo);
        DirInfo = NULL;
        continue;
      }

    if((pFileName==NULL)) 
    {      
        goto Done;
    }
      
    UnicodeStrToAsciiStr (DirInfo->FileName, pFileName);
    DEBUG ((DEBUG_LOAD, "ACPI: Loading %a\r\n", pFileName));
    Status = DirFileHandle->Open(DirFileHandle,
                   &AcpiFileHandle,
                   DirInfo->FileName,
                   EFI_FILE_MODE_READ,
                   0);
    if(Status != EFI_SUCCESS)
       return Status;

    //Get file size info
    BufferSize  = SIZE_OF_EFI_FILE_INFO + 200;
    do{
      Info   = NULL;
      Info = AllocatePool (BufferSize);
      if (Info == NULL)
        goto Done;
      Status = AcpiFileHandle->GetInfo (
                           AcpiFileHandle,
                           &gEfiFileInfoGuid,
                           &BufferSize,
                           Info
                        );
      if(Status == EFI_SUCCESS)
        break;
      if (Status != EFI_BUFFER_TOO_SMALL) {
        FreePool (Info);
        goto Done;
      }
      FreePool (Info);
    } while (TRUE);

    AcpiFileBufferSize = Info->FileSize;
    FreePool (Info);
    AcpiFileBuffer = AllocatePool(AcpiFileBufferSize);
    // AllocatePool failed for some reason
    if (AcpiFileBuffer == NULL)
    {
      DEBUG(( EFI_D_WARN, " ACPI: AcpiFileBuffer is null\r\n"));
      goto Done;
    }
    // Read file content
    Status = AcpiFileHandle->Read (AcpiFileHandle, &AcpiFileBufferSize, AcpiFileBuffer);
    if ((EFI_ERROR (Status)) || (AcpiFileBufferSize == 0)) 
    {
      UnicodeStrToAsciiStr (DirInfo->FileName, pFileName);
      DEBUG ((EFI_D_WARN, "ACPI: LoadAcpiFromVolume() failed reading %a\r\n", pFileName));
      goto Done;
    }
   
    // Add the table
    TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)->Length;
    ASSERT (AcpiFileBufferSize >= TableSize);
    Signature = ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)->Signature;

    // Checksum ACPI table
    AcpiPlatformChecksum ((UINT8*)AcpiFileBuffer, TableSize);
    DEBUG ((ACPI_PLATFORM_DEBUG_PRINT, "Signature: %x Length: %x Checksum %x\r\n", Signature, TableSize, 
      ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)-> Checksum ));

    if(NumOfAcpiTables >= MAX_ACPI_TABLES)
    {
        Status = EFI_ABORTED;
        goto Done;
    }
    // store for measurement
    Status = StoreAcpi( AcpiFileBuffer, TableSize, Signature, &pPreFixArray[NumOfAcpiTables] );
    if (EFI_ERROR (Status)) 
    {
      DEBUG(( EFI_D_WARN, "StoreAcpi failed. Signature: %x, Status: %d.\r\n",
        pPreFixArray[NumOfAcpiTables].Signature, Status));
      goto Done;
    }
    if(NumOfAcpiTables < MAX_ACPI_TABLES)
    {
        NumOfAcpiTables++;
    }
    // increase the table count

    TableCount++;

    FreePool (DirInfo);
    DirInfo = NULL;
    FreePool(AcpiFileBuffer);
    AcpiFileBuffer = NULL;
  }

  Done:
  if (0 == TableCount) {
    Status = EFI_NOT_FOUND;
  }
  else {
    DEBUG ((DEBUG_LOAD, "ACPI: Loaded %d tables\r\n", TableCount));
  }

  if (AcpiFile != NULL)
    EfiClose (AcpiFile);

  if (DirInfo != NULL)
    FreePool (DirInfo);
  
  if( DirFileHandle != NULL)
    DirFileHandle->Close(DirFileHandle); 

  return Status;
}

/**
  Loads ACPI from DPP partition

  @param[in]  Name              Unicode string specifying the ACPI file 
                                name, e.g. L"MSDM.ACP".
  @param[in]  TableSignature    Signature specifying the ACPI table
  @return EFI_SUCCESS if successful
**/
EFI_STATUS
LoadAcpiFromDPP(
    IN CHAR16                   *DPPName,
    IN UINT32                    TableSignature
    )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_DPP_PROTOCOL                  *DppProtocol = NULL;

  UINTN                             TableSize = 0;
  VOID                              *AcpiFileBuffer = NULL;
  UINTN                             AcpiFileBufferSize = 0;
  UINT32                            Signature = 0;

  if (DPPName == NULL)
  {   
    return EFI_INVALID_PARAMETER;
  }

  // Get DPP protocol interface
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if (EFI_ERROR(Status)){
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] gBS->LocateProtocol (DPP) failed. Status: %d.\r\n", Status));
    goto LoadAcpiFromDPPExit;
  }

  //Call with NULL to get the size
  Status = DppProtocol->GetDPP(DppProtocol,
                               L"QCOM",
                               DPPName,
                               NULL,
                               &AcpiFileBufferSize);
  if (EFI_ERROR(Status)){
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] DppProtocol->GetDPP failed. Status: %d.\r\n", Status));
    goto LoadAcpiFromDPPExit;
  }

  AcpiFileBuffer = AllocatePool(AcpiFileBufferSize);
  if (AcpiFileBuffer == NULL)
  {
    DEBUG((EFI_D_ERROR, "[LoadAcpiFromDPP] AcpiFileBuffer is null\r\n"));
    goto LoadAcpiFromDPPExit;
  }

  //Call with the buffer now to get the DPP item
  Status = DppProtocol->GetDPP(DppProtocol,
                               L"QCOM",
                               DPPName,
                               AcpiFileBuffer,
                               &AcpiFileBufferSize);
  if (EFI_ERROR(Status)){
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] DppProtocol->GetDPP failed. Status: %d.\r\n", Status));
    goto LoadAcpiFromDPPExit;
  }

  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);

  // Add the table
  TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)->Length;
  ASSERT (AcpiFileBufferSize >= TableSize);
  Signature = ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)->Signature;
  if (TableSignature != Signature){
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] Signature is not valid!\r\n"));
    Status = EFI_ABORTED;
    goto LoadAcpiFromDPPExit;
  }

  // Checksum ACPI table
  AcpiPlatformChecksum ((UINT8*)AcpiFileBuffer, TableSize);
  DEBUG ((ACPI_PLATFORM_DEBUG_PRINT, "[LoadAcpiFromDPP] Signature: %x Length: %x Checksum %x\r\n", Signature, TableSize, 
    ((EFI_ACPI_DESCRIPTION_HEADER *) AcpiFileBuffer)-> Checksum ));

  if(NumOfAcpiTables >= MAX_ACPI_TABLES)
  {
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] Exceeded max number of ACPI tables.\r\n"));
    Status = EFI_ABORTED;
    goto LoadAcpiFromDPPExit;
  }

  // store for measurement
  Status = StoreAcpi( AcpiFileBuffer, TableSize, Signature, &pPreFixArray[NumOfAcpiTables] );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "[LoadAcpiFromDPP] StoreAcpi failed. Signature: %x, Status: %d.\r\n",
      pPreFixArray[NumOfAcpiTables].Signature, Status));
    goto LoadAcpiFromDPPExit;
  }

  // increase the table count
  if(NumOfAcpiTables < MAX_ACPI_TABLES)
  {
      NumOfAcpiTables++;
  }

LoadAcpiFromDPPExit:

  if (AcpiFileBuffer != NULL){
    FreePool(AcpiFileBuffer);
    AcpiFileBuffer = NULL;
  }
  
  return Status;
}

/**
  Allocates space for a copy of the ACPI tables, copies the ACPI table array 
  that buffer, and returns the copy.
  
  @param  Destination           Contains the copy of the array when the function returns.
  @param  TableSize             Contains the size of the destination when the 
                                function returns.
  @param  AcpiTablesArray       The ACPI tables array to be copied.
  @param  NumOfAcpiTAbles       The number of tables in the array.
  @return EFI_SUCCESS           Successful
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to create the buffer.
*/
STATIC 
EFI_STATUS CopyAcpiTables (
  OUT VOID          **Destination, 
  OUT UINTN          *TableSize,
  IN AcpiTableEntry  *AcpiTablesArray,
  IN UINTN            NumOfTables
  )
{
  UINTN i = 0;
  UINTN Offset = 0;

  *TableSize = 0;

  // Calculate the amount of space that needs to be allocated.
  for (i=0; i<NumOfTables; i++) 
    *TableSize += AcpiTablesArray[i].TableSize;

  // Allocate a buffer for that table.
  *Destination = AllocatePool(*TableSize);
  if (*Destination == NULL) {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to allocate buffer.\r\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  // Copy the tables into the buffer.
  for (i=0; i<NumOfTables; i++) {
    CopyMem((UINT8 *)*Destination + Offset, (UINT8 *)AcpiTablesArray[i].Table,
      AcpiTablesArray[i].TableSize);
    Offset += AcpiTablesArray[i].TableSize;
  }

  return EFI_SUCCESS;
}
/**
  Allocates space for a copy of all the AML Variable buffers, copies the variables to
  that buffer, and returns the copy.
  
  @param  Destination           Contains the full buffer of variables when the function returns.
  @param  AmlVariableListSize   Contains the size of the destination when the function returns.
  @param  AmlVariableEntry      The aml Variables to be copied.
  @param  NumOfAmlVariables     The number of variables in the array.
  @return EFI_SUCCESS           Successful
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to create the buffer.
*/
STATIC 
EFI_STATUS CopyAmlVariableTables (
  OUT VOID            **Destination, 
  OUT UINTN            *AmlVariableListSize,
  IN AmlVariableEntry  *AmlVariableEntry,
  IN UINTN              NumOfAmlVariables
  )
{
  UINTN i = 0;
  UINTN Offset = 0;

  *AmlVariableListSize = 0;

  // Calculate the amount of space that needs to be allocated. It is AML Variable buffer
  // plus size of AmlVariableSize, as we insert the size of the buffer between buffers
  for (i=0; i<NumOfAmlVariables; i++) 
    *AmlVariableListSize += (AmlVariableEntry[i].AmlVariableSize + sizeof(UINT8));

  // Allocate a buffer for that table.
  *Destination = AllocatePool(*AmlVariableListSize);
  if (*Destination == NULL) {
    DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to allocate buffer.\r\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  // Copy the tables into the buffer.
  for (i=0; i<NumOfAmlVariables; i++) {
    CopyMem((UINT8 *)*Destination + Offset, &(AmlVariableEntry[i].AmlVariableSize),sizeof(UINT8));
      Offset += sizeof(UINT8);
    CopyMem((UINT8 *)*Destination + Offset, (UINT8 *)AmlVariableEntry[i].AmlVariable,
      AmlVariableEntry[i].AmlVariableSize);
    Offset += AmlVariableEntry[i].AmlVariableSize;
  }

  return EFI_SUCCESS;
}


/**
 Call each of the registered GetTable callback functions.

 @param  IsPrefix              TRUE to call the callback functions with the 
                               pre fixed tables, FALSE to call the callback 
                               functions with the post fixed tables.
 @return EFI_SUCCESS           Successful
 @return EFI_OUT_OF_RESOURCES  Out of memory.
 @return EFI_INVALID_PARAMETER GetMode is not GETMODE_PREFIX or GETMODE_POSTFIX.
*/
STATIC 
EFI_STATUS CallGetTableCallbacks ( 
  IN BOOLEAN GetMode
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINTN                           i;
  UINTN                           j;
  VOID                           *TablePtr;
  UINTN                           TableSize;
  AcpiTableEntry                 *CurrentAcpiTables = NULL;
  UINTN    NumCallbacks = 0;
  UINTN    NumACPITable = 0;
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pRegisteredCallbacks);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable);
  SET_REGISTER_CALLBACK_LIMITS(NumCallbacks);
  
  if (GetMode != GETMODE_PREFIX && GetMode != GETMODE_POSTFIX)
    return EFI_INVALID_PARAMETER;

  if (GetMode == GETMODE_PREFIX)
    CurrentAcpiTables = pPreFixArray;
  else
    CurrentAcpiTables = pPostFixArray;


  // Process each registration and call GetTable callback functions as appropriate
  // Loop through each of the registered callbacks
  for (i=0; i<NumCallbacks; i++)
  {
    // If GetTableCallback is not registered or it's not registered for the current
    // GETMODE, skip this callback.
    if (pRegisteredCallbacks[i].GetTableCallback == NULL 
        || (pRegisteredCallbacks[i].GetMode & GetMode) != GetMode) 
      continue;

    // If the registered signature is ACPI_TABLE_SIGNATURE_ALL, then copy all of the tables and 
    // call the callback.
    if (pRegisteredCallbacks[i].TableSignature == ACPI_TABLE_SIGNATURE_ALL)
    {
      // Copy the tables
      Status = CopyAcpiTables(&TablePtr, &TableSize, 
        (GetMode == GETMODE_PREFIX ? pPreFixArray : pPostFixArray), NumOfAcpiTables);
      if (EFI_ERROR(Status)) {
        DEBUG(( EFI_D_WARN, "Failed to copy the ACPI table array.\r\n"));
        continue;
      }

      // Call the callback function.
      Status = pRegisteredCallbacks[i].GetTableCallback(TablePtr, 
        TableSize, GetMode == GETMODE_PREFIX);
      if ( EFI_ERROR (Status)) {
        DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call GetTable callback function.\r\n"));
        continue;
      }
    }
    // If the registered callback has a signature, find the table with that 
    // signature and call the callback with it.
    else 
    {
      // Loop through each of the ACPI tables
      for (j=0; j<NumACPITable; j++) {
        // If the signature doesn't match, skip this ACPI table.
        if(pRegisteredCallbacks[i].TableSignature != CurrentAcpiTables[j].Signature) 
            continue;

        // Allocate a buffer for the table to be passed to the callback function,
        // and copy the ACPI table to the buffer.
        TablePtr = (VOID *)AllocatePool(CurrentAcpiTables[j].TableSize);
        if ( TablePtr == NULL) {
          DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to allocate buffer.\r\n"));
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem((UINT8 *)TablePtr, (UINT8 *)CurrentAcpiTables[j].Table, 
          CurrentAcpiTables[j].TableSize);

        // Call the callback function.
        DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "Calling GetTableCallback for %x at %x\r\n", 
          CurrentAcpiTables[j].Signature, pRegisteredCallbacks[i].GetTableCallback));

        Status = pRegisteredCallbacks[i].GetTableCallback(TablePtr, 
          CurrentAcpiTables[j].TableSize, GetMode == GETMODE_PREFIX);

        if ( EFI_ERROR (Status)) {
          DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call GetTableCallback "
            "for signature %x and function address %x. Status: %d\r\n",
            CurrentAcpiTables[j].Signature, 
            pRegisteredCallbacks[i].GetTableCallback, Status));
        }
        break;
      }

      // If we got through all the acpi tables without finding the right one, call the
      // callback function passing in NULL for the table.
      if (j == NumACPITable) {
        DEBUG(( EFI_D_WARN, "Callback registered with invalid signature: %d.\r\n",
            pRegisteredCallbacks[i].TableSignature));
        Status = pRegisteredCallbacks[i].GetTableCallback(NULL, 
          0, GetMode == GETMODE_PREFIX);
        if ( EFI_ERROR (Status)) {
          DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call GetTableCallback "
            "function.\r\n"));
        }
      }
    }
  }
  return Status;
}

/**
 Call the SetTable callback functions, passing them the pre fixed tables, to get the 
 the post fixed tables. The post fixed tables are copied to PostFixArray.

 @return EFI_SUCCESS           Successful
 @return EFI_OUT_OF_RESOURCES  Out of memory.
*/
STATIC 
EFI_STATUS CallSetTableCallbacks ( 
 )
{
  UINTN                           CallbackIndex;
  UINTN                           TableIndex;
  BOOLEAN                         CallbackCalled[MAX_ACPI_REGISTERS];
  UINTN                           Counter;
  UINTN    NumACPITable = 0;
  UINTN    NumCallbacks = 0;

  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pRegisteredCallbacks);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable);
  SET_REGISTER_CALLBACK_LIMITS(NumCallbacks);
  
  //set CallbacksCalled to false for each registration
  for (Counter = 0; Counter < MAX_ACPI_REGISTERS; Counter++)
  {
    CallbackCalled[Counter] = 0;
  }
  


  // Loop through each of the ACPI tables
  for (TableIndex = 0; TableIndex < NumACPITable; TableIndex++)
  {
    // TODO: remove this and replace with a callback.
    if(EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE == pPreFixArray[TableIndex].Signature)
    {
      EFI_STATUS Status = EFI_SUCCESS;
      VOID *TablePtr = NULL;
      UINTN TableSize = 0;

      TableSize = pPreFixArray[TableIndex].TableSize;
      TablePtr = AllocatePool(TableSize);
      if (TablePtr == NULL)
      {
        DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to allocate buffer.\r\n"));
        return EFI_OUT_OF_RESOURCES;
      }

      CopyMem(TablePtr, pPreFixArray[TableIndex].Table, TableSize);

      // Enable BGRT logo processing.
      RenderSplashScreen(TablePtr);
      // Checksum ACPI table
      AcpiPlatformChecksum ((UINT8*)TablePtr, TableSize);
      // Copy the table to the PostFixArray.
      Status = StoreAcpi(TablePtr, TableSize, pPreFixArray[TableIndex].Signature,
        &pPostFixArray[TableIndex]);
      FreePool(TablePtr);
      if (EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, "StoreAcpi failed. Signature: %x, Status: %d.\r\n",
          pPreFixArray[TableIndex].Signature, Status));
        return Status;
      }
      continue;
    }

    // Loop through each of the registered callback functions until we 
    // find one with a matching signature.
    for (CallbackIndex = 0; CallbackIndex < NumCallbacks; CallbackIndex++)
    {
      if ((pPreFixArray[TableIndex].Signature 
          == pRegisteredCallbacks[CallbackIndex].TableSignature)
        && pRegisteredCallbacks[CallbackIndex].SetTableCallback != NULL)
      {
        EFI_STATUS Status = EFI_SUCCESS;
        VOID *TablePtr = NULL;
        UINTN TableSize = 0;

        TableSize = pPreFixArray[TableIndex].TableSize;
        TablePtr = AllocatePool(TableSize);
        if (TablePtr == NULL)
        {
          DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to allocate buffer.\r\n"));
          return EFI_OUT_OF_RESOURCES;
        }

        CopyMem(TablePtr, pPreFixArray[TableIndex].Table, TableSize);

        // Call the callback function.
        DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "Calling SetTableCallback for %x at %x\r\n", pPreFixArray[TableIndex].Signature, 
          pRegisteredCallbacks[CallbackIndex].SetTableCallback));

        Status = pRegisteredCallbacks[CallbackIndex].SetTableCallback(
          &TablePtr, &TableSize);
        if ( EFI_ERROR (Status)) 
        {
          DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call SetTableCallback "
            "for signature %x and function address %x. Status: %d\r\n",
            pPreFixArray[TableIndex].Signature, 
            pRegisteredCallbacks[CallbackIndex].SetTableCallback, Status));
          FreePool(TablePtr);
          return Status;
        }
        // Mark this callback as called.
        CallbackCalled[CallbackIndex] = TRUE;

        // Calculate the checksum.
        AcpiPlatformChecksum ((UINT8*)TablePtr, TableSize);
        /* Our plan is to have client driver calculate checksum, and verify in AcpiPlatformDxe, however
         * this requires coordination of teams as it will break existing functionality. Code below is to
         * be uncommented when ready and remove above checksum calculation.
 
        //Calculate the checksum of table and confirm with checksum value in the table
        Checksum = CalculateCheckSum8(TablePtr, TableSize);
        if( ((UINT8 *)TablePtr)[OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum)] != Checksum)
        {
          DEBUG(( EFI_D_WARN, " Checksum %x for table %x failed to match checksum %x in the table\r\n Client drivers must update checksum", 
            Checksum, PreFixArray[TableIndex].Signature, ((UINT8 *)TablePtr)[OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum)]  ));
          return EFI_INVALID_PARAMETER;
        }
        */

        // Copy the table to the PostFixArray.
        Status = StoreAcpi(TablePtr, TableSize, pPreFixArray[TableIndex].Signature,
          &pPostFixArray[TableIndex]);
        FreePool(TablePtr);
        if (EFI_ERROR (Status)) 
        {
          DEBUG(( EFI_D_WARN, "StoreAcpi failed. Signature: %x, Status: %d.\r\n",
            pPreFixArray[TableIndex].Signature, Status));
          return Status;
        }
        break;
      }
    }
    // If we got through all of the registered callbacks without finding 
    // one with a matching signature, simply copy the table from the prefix
    // to the postfix table.
    if (CallbackIndex == NumCallbacks)
    {
      EFI_STATUS Status = StoreAcpi(pPreFixArray[TableIndex].Table, pPreFixArray[TableIndex].TableSize,
        pPreFixArray[TableIndex].Signature, &pPostFixArray[TableIndex]);
      if (EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, "StoreAcpi failed. Signature: %x, Status: %d.\r\n",
          pPreFixArray[TableIndex].Signature, Status));
        return Status;
      }
    }
  }

  // Loop through all the callbacks that haven't been called, and call them
  // passing in NULL for the table to the indicate an invalid signature.
  for (CallbackIndex = 0; CallbackIndex < NumCallbacks; CallbackIndex++)
  {
    if (!CallbackCalled[CallbackIndex] 
      && pRegisteredCallbacks[CallbackIndex].SetTableCallback != NULL)
    {
      EFI_STATUS Status = EFI_SUCCESS;
      pRegisteredCallbacks[CallbackIndex].SetTableCallback(NULL, NULL);
      if ( EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call SetTableCallback "
          "function.\r\n"));
        return Status;
      }
    }
  }
  return EFI_SUCCESS;
}

/**
 Call each of the registered AMLVariableGetTable callback functions.

 @param  IsPrefix              TRUE to call the callback functions with the 
                               pre fixed tables, FALSE to call the callback 
                               functions with the post fixed tables.
 @return EFI_SUCCESS           Successful
 @return EFI_OUT_OF_RESOURCES  Out of memory.
 @return EFI_INVALID_PARAMETER GetMode is not GETMODE_PREFIX or GETMODE_POSTFIX.
*/
STATIC 
EFI_STATUS CallGetAmlVariableCallbacks ( 
  IN BOOLEAN GetMode
 )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINTN                           i;
  UINTN                           j;
  VOID                           *AmlVariableBufferPtr;
  UINTN                           AmlVariableBufferSize;
  AmlVariableEntry               *CurrentAmlVariables = NULL;
  UINTN    NumCallbacks = 0;
  UINTN    NumAMLVariableArraySize = 0;
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixAmlVariableArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixAmlVariableArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pRegisteredAmlVariableCallbacks);
  SET_AML_REGISTER_CALLBACK_LIMITS(NumCallbacks);
  SET_AML_VARIABLE_ARRAY_LIMITS(NumAMLVariableArraySize);
  
  if (GetMode != GETMODE_PREFIX && GetMode != GETMODE_POSTFIX)
    return EFI_INVALID_PARAMETER;
  

  if (GetMode == GETMODE_PREFIX)
    CurrentAmlVariables = pPreFixAmlVariableArray;
  else
    CurrentAmlVariables = pPostFixAmlVariableArray;
  
  // Process each registration and call the callback functions as appropriate
  // Loop through each of the registered callbacks
  for (i=0; i<NumCallbacks; i++)
  {
    // If GetAmlVariableCallback is not registered or it's not registered for the current
    // GETMODE, skip this callback.
    if (pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback == NULL 
        || (pRegisteredAmlVariableCallbacks[i].GetMode & GetMode) != GetMode) 
      continue;

    // If the registered variable name is 'ALL', then copy all of the variables and 
    // call the callback.
    if (AsciiStrnCmp(pRegisteredAmlVariableCallbacks[i].VariableName, AMLVARIABLENAME_ALL, AML_NAMESTRING_LENGTH) == 0)
    {
      // Copy the tables
      Status = CopyAmlVariableTables(&AmlVariableBufferPtr, &AmlVariableBufferSize, 
        (GetMode == GETMODE_PREFIX ? pPreFixAmlVariableArray : pPostFixAmlVariableArray), NumAMLVariableArraySize);
      if (EFI_ERROR(Status)) {
        DEBUG(( EFI_D_WARN, " Failed to copy the Aml variable table array.\r\n"));
        continue;
      }

      // Call the callback function.
      Status = pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback(AmlVariableBufferPtr, 
        AmlVariableBufferSize, GetMode == GETMODE_PREFIX);
      if ( EFI_ERROR (Status)) {
        DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call GetAmlVariable callback "
          "function.\r\n"));
        continue;
      }
    }
    // If the registered callback has a specific variable association, find the specific 
    // callback for it.
    else 
    {
      // Loop through each of the Aml Variable entries 
      for (j=0; j<NumAMLVariableArraySize; j++) {
        // If the name doesn't match then move on.
        if(AsciiStrnCmp(pRegisteredAmlVariableCallbacks[i].VariableName,
             CurrentAmlVariables[j].AmlVariableName, AML_NAMESTRING_LENGTH)!=0) 
            continue;

        // Allocate a buffer for the variable entry to be passed to the callback function,
        // and copy the entry to the buffer.
        AmlVariableBufferPtr = (VOID *)AllocatePool(CurrentAmlVariables[j].AmlVariableSize);
        if ( AmlVariableBufferPtr == NULL) {
          DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to allocate buffer.\r\n"));
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem((UINT8 *)AmlVariableBufferPtr, (UINT8 *)CurrentAmlVariables[j].AmlVariable, 
          CurrentAmlVariables[j].AmlVariableSize);

        // Call the callback function.
        DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " Calling GetAmlVariableCallback for variable %x and "
          "function address %x.\r\n", CurrentAmlVariables[j].AmlVariableName, 
          pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback));

        Status = pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback(AmlVariableBufferPtr, 
          CurrentAmlVariables[j].AmlVariableSize, GetMode == GETMODE_PREFIX);

        if ( EFI_ERROR (Status)) {
          DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call GetAmlVariableCallback "
            "for signature %x and function address %x. Status: %d\r\n",
            CurrentAmlVariables[j].AmlVariableName, 
            pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback, Status));
        }
        else
        {
          DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " Successfully called GetAmlVariableCallback for signature "
            "%x and function address %x.\r\n", CurrentAmlVariables[j].AmlVariableName, 
            pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback));
        }
        break;
      }

      // If we got through all the variables without finding the right one, call the
      // callback function passing in NULL for the variable.
      if (j == NumAMLVariableArraySize) {
        DEBUG(( EFI_D_WARN, " Callback registered with invalid signature: %d.\r\n",
            pRegisteredAmlVariableCallbacks[i].VariableName));
        Status = pRegisteredAmlVariableCallbacks[i].GetAmlVariableCallback(NULL, 
          0, GetMode == GETMODE_PREFIX);
        if ( EFI_ERROR (Status)) {
          DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call GetAmlVariableCallback "
            "function.\r\n"));
        }
      }
    }
  }
  return Status;
}
STATIC 
EFI_STATUS AmlVariableUpdateVerification ( 
  IN AmlVariableEntry  AmlVariableEntry,
  IN VOID              *AmlVariableBuffer
 )
{
  UINT8   *OriginalPtr = AmlVariableEntry.AmlVariable;
  UINT8   *UpdatePtr = AmlVariableBuffer;
  UINTN    i;
  BOOLEAN  IsStringTerminated = FALSE;
  if( AmlVariableEntry.AmlVariable == NULL || AmlVariableBuffer == NULL)
  {
    DEBUG((EFI_D_WARN, "  Verification failed for SetAmlVariableCallback: Buffer is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Header should not change, so verify each byte is the same in both buffers
  for(i=0; i<AML_NAMESPACE_HEADER_SIZE; i++)
  {
    if( *(OriginalPtr+i) != *(UpdatePtr+i) )
    {
      DEBUG((EFI_D_WARN, "  Verification failed for SetAmlVariableCallback: Header was changed\r\n"));
      return EFI_INVALID_PARAMETER;
    }
  }
  /* All that's left to check is the Data Value. At this point we know the DataType hasn't changed and the buffer
   * size won't change. So we just have to make sure that a string type is correctly terminated with null char. If
   * the client driver wants to reduce string length, they must pad with null op, so we must make sure string 
   * terminates, and then pads null op to match the original string length.
   */
  
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "  AmlVariableUpdateVerification: Header ok \r\n"));
  if(*(UpdatePtr+i-1) == AML_STRING_PREFIX)
  {
    DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "  AmlVariableUpdateVerification: Check String Data \r\n"));
    // only check for original length
    for(;i<AmlVariableEntry.AmlVariableSize;i++)
    {
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "  AmlVariableUpdateVerification: Byte %x IsStringTerminated: %d \r\n", *(UpdatePtr+i), IsStringTerminated));
      if(IsStringTerminated)
      {
        if(*(UpdatePtr+i) != AML_ZERO_OP)
        {
          DEBUG((EFI_D_WARN, "  Verification failed for SetAmlVariableCallback: String data after Null Char is not Zero Op\r\n"));
          return EFI_INVALID_PARAMETER;
        }
      }
      else
      {
        if( *(UpdatePtr+i) == AML_NULL_CHAR)
          IsStringTerminated = TRUE;
        // Confirm the byte is a valid char for Strings
        else if( *(UpdatePtr+i) < '0' || *(UpdatePtr+i) > AML_NAME_CHAR__ )
        {
          DEBUG((EFI_D_WARN, "  Verification failed for SetAmlVariableCallback: String Data was invalid\r\n"));
          return EFI_INVALID_PARAMETER;
        }
      }
    }
    // confirm StringData was terminated
    if(IsStringTerminated == FALSE)
    {
      DEBUG((EFI_D_WARN, "  Verification failed for SetAmlVariableCallback: String data was never terminated\r\n"));
      return EFI_INVALID_PARAMETER;
    }
  }
  return EFI_SUCCESS;
}
/**
 Call the SetAmlVariable callback functions, passing them the pre fixed binary data, to get the 
 the post fixed data. The post fixed buffers are copied to PostFixAmlVariableArray and also 
 loaded into the PostFixArray of ACPI Tables. Thus the variable info is installed along with tables.

 @return EFI_SUCCESS           Successful
 @return EFI_INVALID_PARAMETER Out of memory.
*/
STATIC 
EFI_STATUS CallSetAmlVariableCallbacks ( 
 )
{
  UINTN                           CallbackIndex;
  UINTN                           VariableArrayIndex;
  BOOLEAN                         CallbackCalled[MAX_ACPI_REGISTERS];
  UINTN                           TableIndex = 0;
  UINTN                           i;
  UINTN     NumCallbacks = 0;
  UINTN     NumAMLVariableArraySize = 0;
  UINTN     NumACPITable = 0;
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixAmlVariableArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixAmlVariableArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pRegisteredAmlVariableCallbacks);
  SET_AML_REGISTER_CALLBACK_LIMITS(NumCallbacks);
  SET_AML_VARIABLE_ARRAY_LIMITS(NumAMLVariableArraySize);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable);

  //set CallbacksCalled to false for each registration
  for (i = 0; i < MAX_ACPI_REGISTERS; i++)
  {
    CallbackCalled[i] = 0;
  }
  //Find DSDT in table array 
  for(i = 0; i < NumACPITable; i++)
  {  
    if(pPostFixArray[i].Signature ==
        EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE)
    {    
     TableIndex = i;
     break;
    } 
  }
  
  if( i == NumACPITable) 
    return EFI_INVALID_PARAMETER;


  // Loop through the variable array
  for (VariableArrayIndex = 0; VariableArrayIndex < NumAMLVariableArraySize; VariableArrayIndex++)
  {
    // Loop through each of the registered callback functions until we 
    // find one with a matching variable name.
    for (CallbackIndex = 0; CallbackIndex < NumCallbacks; CallbackIndex++)
    {
      if (pRegisteredAmlVariableCallbacks[CallbackIndex].SetAmlVariableCallback != NULL &&
           (AsciiStrnCmp(pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName, 
            pRegisteredAmlVariableCallbacks[CallbackIndex].VariableName, AML_NAMESTRING_LENGTH)== 0))
      {
        EFI_STATUS Status = EFI_SUCCESS;
        VOID *AmlVariablePtr = NULL;
        UINTN AmlVariableSize = 0;

        AmlVariableSize = pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableSize;
        AmlVariablePtr = AllocatePool(AmlVariableSize);
        if (AmlVariablePtr == NULL)
        {
          DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to allocate buffer.\r\n"));
          return EFI_OUT_OF_RESOURCES;
        }

        CopyMem(AmlVariablePtr, pPreFixAmlVariableArray[VariableArrayIndex].AmlVariable, AmlVariableSize);

        // Call the callback function.


        Status = pRegisteredAmlVariableCallbacks[CallbackIndex].SetAmlVariableCallback(
          &AmlVariablePtr, AmlVariableSize);
        if ( EFI_ERROR (Status)) 
        {
          DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call SetAmlVariableCallback "
            "for AmlVariableName %x and function address %x. Status: %d\r\n",
            pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName, 
            pRegisteredAmlVariableCallbacks[CallbackIndex].SetAmlVariableCallback, Status));
          FreePool(AmlVariablePtr);
          return Status;
        }
        else
        {
            DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " Calling SetAmlVariableCallback for AmlVariableName %c%c%c%c \r\n", 
              pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName[0], 
              pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName[1],
              pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName[2], 
              pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName[3]
             ));
        }

        /* Ensure the AML update adheres to guidelines:
         * Can only change payload (AmlVariableDataValue), should not touch the header (object type, name, data type)
         * Overall size should remain same, and Strings should finish with AML_NAME_CHAR_NULL. If verification fails
         * then ignore the callback.
         */
        Status = AmlVariableUpdateVerification(pPreFixAmlVariableArray[VariableArrayIndex], AmlVariablePtr);  
        if (!EFI_ERROR (Status)){
          DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "  Verification of SetAmlVariableCallback passed\r\n"));
          // Copy the variable to the PostFixAmlVariableArray.
          Status = StoreAmlVariable(AmlVariablePtr, AmlVariableSize, pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName,
                   pPreFixAmlVariableArray[VariableArrayIndex].TableOffset, &pPostFixAmlVariableArray[VariableArrayIndex]);
          if (EFI_ERROR (Status)) 
            { 
              DEBUG(( EFI_D_WARN, " StoreAmlVariable failed. AmlVariableName: %x, Status: %d.\r\n",
              pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName, Status));
              return Status;
            }
          // Copy variable buffer into DSDT
          for(i=0;i< pPostFixAmlVariableArray[VariableArrayIndex].AmlVariableSize;i++)
            CopyMem( (((UINT8*)pPostFixArray[TableIndex].Table)+pPreFixAmlVariableArray[VariableArrayIndex].TableOffset), AmlVariablePtr, AmlVariableSize);
          // Mark this callback as called.
          CallbackCalled[CallbackIndex] = TRUE;
        }
        FreePool(AmlVariablePtr);
        // we don't need to look at any more callbacks
        break;
      }
    }
    // If we got through all of the registered callbacks without finding 
    // one with a matching AmlVariableName, simply copy the variable from the prefix
    // to the postfix table.
    if (CallbackIndex == NumCallbacks)
    {
      EFI_STATUS Status = StoreAmlVariable(pPreFixAmlVariableArray[VariableArrayIndex].AmlVariable, 
        pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableSize,
        pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName, 
        pPreFixAmlVariableArray[VariableArrayIndex].TableOffset,
        &pPostFixAmlVariableArray[VariableArrayIndex]);
      if (EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, " StoreAmlVariable failed. AmlVariableName: %x, Status: %d.\r\n",
          pPreFixAmlVariableArray[VariableArrayIndex].AmlVariableName, Status));
        return Status;
      }
    }
  }
  
  // Loop through all the callbacks that haven't been called, and call them
  // passing in NULL for the table to the indicate an invalid signature.
  for (CallbackIndex = 0; CallbackIndex < NumCallbacks; CallbackIndex++)
  {
    if (!CallbackCalled[CallbackIndex] 
      && pRegisteredAmlVariableCallbacks[CallbackIndex].SetAmlVariableCallback != NULL)
    {
      EFI_STATUS Status = EFI_SUCCESS;
      pRegisteredAmlVariableCallbacks[CallbackIndex].SetAmlVariableCallback(NULL, 0);
      if ( EFI_ERROR (Status)) 
      {
        DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call SetAmlVariableCallback "
          "function.\r\n"));
        return Status;
      }
    }
  }

  return EFI_SUCCESS;
}
EFI_STATUS
LoadAcpi (  )
{
  UINTN                           i;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                           TableHandle;
  EFI_STATUS                      Status;
  UINTN     NumACPITable = 0;
  

  //Turn off registration 
  RegistrationClosed = TRUE;

  // Find the AcpiTable protocol
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTable);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to locate EfiAcpiTableProtocol.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE successfully located EfiAcpiTableProtocol.\r\n"));
  }
  
  // Call GetTable callback functions if prefix tables are desired
  Status = CallGetTableCallbacks (GETMODE_PREFIX);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call GetTable callbacks to get prefix tables.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE successfully called GetTable callbacks to get prefix tables.\r\n"));
  }
  
  // Call SetTable callback functions if any
  Status = CallSetTableCallbacks ();
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call SetTable callbacks.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE successfully called SetTable callbacks.\r\n"));
  }
  
  // Call GetTable callback functions if postfix tables are desired
  Status = CallGetTableCallbacks (GETMODE_POSTFIX);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to call GetTable callbacks to get postfix tables.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE successfully called GetTable callbacks to get postfix tables.\r\n"));
  }
  
  /* Run AML variable callbacks */
  PrintAmlVariables(GETMODE_PREFIX);
  // Call GetAmlVariable callback functions if prefix tables are desired
  Status = CallGetAmlVariableCallbacks (GETMODE_PREFIX);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call GetAmlVariable callbacks to get prefix tables.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " ACPIPlatformDXE successfully called GetAmlVariable callbacks to get prefix tables.\r\n"));
  }
  // Call SetAmlVariable callback functions if any
  Status = CallSetAmlVariableCallbacks ();
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call SetAmlVariable callbacks.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " ACPIPlatformDXE successfully called SetAmlVariable callbacks.\r\n"));
  }
  // Call GetAmlVariable callback functions if postfix tables are desired
  Status = CallGetAmlVariableCallbacks (GETMODE_POSTFIX);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to call GetAmlVariable callbacks to get postfix tables.\r\n"));
    return EFI_ABORTED;
  } else {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, " ACPIPlatformDXE successfully called GetAmlVariable callbacks to get postfix tables.\r\n"));
  }
  PrintAmlVariables(GETMODE_PREFIX);
  PrintAmlVariables(GETMODE_POSTFIX);
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixArray);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable); 
  // All of the tables have been fixed up. Loop through all of the tables and
  // call InstallAcpiTable for each of them.
  for (i=0; i<NumACPITable; i++)
  {
    TableHandle = 0;
    // Install ACPI table
    Status = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          pPostFixArray[i].Table,
                          pPostFixArray[i].TableSize,
                          &TableHandle
                          );
    if (EFI_ERROR(Status)) {
      DEBUG(( EFI_D_WARN, "ACPIPlatformDXE failed to install ACPI table with "
        "signature %x.\r\n", pPostFixArray[i].Signature));
      return Status;
    } else {
      DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE successfully installed ACPI table with "
        "signature %x.\r\n", pPostFixArray[i].Signature));
    }
  }

  // Call GetTable callback functions if postfix tables are desired
  return Status; 
}
/**
  Given a pointer to the name op in binary data, the function parses the buffer and adds an 
  AmlVariableEntry to the given array
 @param  AmlNameObjectPtr      Pointer to byte in buffer with value AML_NAME_OP
 @param  TableOffset           Offset of this byte in the table. Stored in the AmlVariableEntry
 
 @return EFI_SUCCESS           Successful
 @return EFI_INVALID_PARAMETER Incorrect Table Signature given
**/
EFI_STATUS
LoadAmlNameObjectFromTable(
  IN VOID   *AmlNameObjectPtr,
  IN UINTN   TableOffset
)
{
  EFI_STATUS Status;
  UINTN  i=0;
  UINTN  Offset=0;
  UINTN  AmlNameObjectSize=0;
  UINT8  CurrentByte;
  CHAR8  AmlVariableName[AML_NAMESTRING_LENGTH];
 
  // if the first byte is not AML_NAME_OP then this buffer is invalid
  if((AmlNameObjectPtr == NULL) || (*((UINT8*)AmlNameObjectPtr) != AML_NAME_OP))
  {
    return EFI_INVALID_PARAMETER;
  }
  
  // after name op, the next AML_NAMESTRING_LENGTH bytes are the name of the variable
  for(i=0; i<AML_NAMESTRING_LENGTH; i++)
  {
    Offset++;
    /* Confirm the byte is a valid char. Compiler catches this error, but we're trying
     * to help weed out false positives for variables as we go byte by byte.
     */
    CurrentByte = *(((UINT8*)AmlNameObjectPtr)+Offset);
    if(CurrentByte < '0' || CurrentByte > AML_NAME_CHAR__ )
      return EFI_INVALID_PARAMETER;
    AmlVariableName[i]= (CHAR8) CurrentByte;
  }
  
  // The next byte is the data type
  Offset++;
  CurrentByte=*(((UINT8*)AmlNameObjectPtr)+Offset);
  
  /* If data type is AML_ZERO_OP, then name object was initialized with 0x00. This leads to
   * encoding optimization we can't handle. Print a warning message in that case. If data type 
   * is not 0x00 and not String, we know the length of data. If it is a string,
   * we must get length by searching through the bytes till we reach the null char
   */  
  switch (CurrentByte)
  {
    case AML_BYTE_PREFIX:
      Offset++;
      break;
    case AML_WORD_PREFIX:
      Offset+=2;
      break;
    case AML_DWORD_PREFIX:
      Offset+=4;
      break;
    case AML_STRING_PREFIX:
      do
      {
        Offset++;
        CurrentByte=*(((UINT8*)AmlNameObjectPtr)+Offset);
      } while(CurrentByte!=AML_NULL_CHAR);
      break;
    case AML_QWORD_PREFIX:
      Offset+=8;    
      break;
    case AML_ZERO_OP:
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "AML: Name Object "));
      for(i=0; i<AML_NAMESTRING_LENGTH; i++)
        DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "%c", AmlVariableName[i]));
      DEBUG((ACPI_PLATFORM_DEBUG_PRINT, " set to zero- must be nonzero to update\r\n"));
    default:
      return EFI_INVALID_PARAMETER;
  }
  // Offset+1 is now the total number of bytes for our aml variable size. We can store the variable in our array.
  AmlNameObjectSize = (Offset+1) * sizeof(UINT8);
  Status = StoreAmlVariable (AmlNameObjectPtr, AmlNameObjectSize, AmlVariableName, TableOffset,
             &pPreFixAmlVariableArray[NumOfAmlVariables]);
  if(NumOfAmlVariables>=MAX_AML_VARIABLES)
  {
      DEBUG((EFI_D_WARN, " Number of AmlVariables is over the limit \r\n"));
  }
  else{
    NumOfAmlVariables++;
  }
  return Status;    
}

/**
 Parsing function which takes a table signature as a parameter, finds the root name objects
 and puts them into the prefixed aml variable array

 @param  TableSignature        Signature of the table to parse for root variables

 @return EFI_SUCCESS           Successful
 @return EFI_INVALID_PARAMETER Incorrect Table Signature given
*/
EFI_STATUS
LoadAmlVariables (
  IN UINT32 TableSignature 
)
{
  UINTN i;
  UINTN j;
  UINT8 *TableBytePtr;
  UINTN NumACPITable =0;
  UINTN NumAMLVariableArraySize =0;
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixAmlVariableArray);
  SET_AML_VARIABLE_ARRAY_LIMITS(NumAMLVariableArraySize);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable); 
  
  DEBUG((ACPI_PLATFORM_DEBUG_PRINT, "AML: Loading objects from %x\r\n", TableSignature));
  for (i=0; i<NumACPITable; i++) {
    // If the signature doesn't match, skip this ACPI table.
    if(pPreFixArray[i].Signature != TableSignature) 
    {
        continue;
    }
    // Signature matches so call load function for variables on this table
    else
    {
      TableBytePtr = (UINT8*)pPreFixArray[i].Table;
      j=sizeof(EFI_ACPI_DESCRIPTION_HEADER);
      while(j<pPreFixArray[i].TableSize)
      {
        // we can stop looking once we hit a device
        if( *(TableBytePtr+j) == AML_EXT_OP)
          break;
        if(*(TableBytePtr+j) == AML_NAME_OP)
        {
          if (EFI_ERROR(LoadAmlNameObjectFromTable(TableBytePtr+j,j)))        
          // if LoadAmlNameObjectFromTable returns error than byte was not start of name op
            j++;

          else
          {
           // LoadAmlNameObjectFromTable was successful, so skip the aml variable to save time            
            SET_AML_VARIABLE_ARRAY_LIMITS(NumAMLVariableArraySize);
            j+=pPreFixAmlVariableArray[NumAMLVariableArraySize-1].AmlVariableSize;
          }
        }
        else if(*(TableBytePtr+j) == AML_SCOPE_OP)
        {
        // Skip Scope Content. Pls refer ACPI Spec 20.2.4 and 20.2.5.1 
            if((j+1)<pPreFixArray[i].TableSize)
            {
                j = j + 1 + ((*(TableBytePtr+j+1)&0xC0)>>6) + 1;
            }
            else
            {                
                j++;
            }
        }
        else
          j++;
      }
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS  AmlUpdateRegister()
{
    EFI_QCOM_ACPIPLATFORM_PROTOCOL *pEfiAcpiPlatformProtocol = NULL;
    EFI_STATUS Status;
    UINTN item_num = 0;
    
    Status = gBS->LocateProtocol(&gQcomAcpiPlatformProtocolGuid,
                                  NULL, (VOID**) &pEfiAcpiPlatformProtocol);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_WARN, "AML: Locate ACPI Protocol failed, Status = (0x%x)\r\n", Status));
        return Status;
    }    

    for(item_num =0 ;item_num< sizeof(AMLRegisterInfo)/sizeof(AMLRegisterStruct);item_num++)
    {
         pEfiAcpiPlatformProtocol->AmlVariableRegister( pEfiAcpiPlatformProtocol,
                                                        (CHAR8 *)AMLRegisterInfo[item_num].VariableName,
                                                        AMLRegisterInfo[item_num].GetMode,
                                                        AMLRegisterInfo[item_num].GetAmlVariableCallback,
                                                        AMLRegisterInfo[item_num].SetAmlVariableCallback);

    }
    return EFI_SUCCESS;
}





/**
 Sample callback function to show how the AML update mechanism works. The callback
 changes the value of the TEST variable from 0xFF to 0xAA 
*/ 

static EFI_STATUS EFIAPI AmlUpdateTestCallback(
  IN OUT VOID         **AmlVariableBuffer,
  IN OUT UINTN          AmlVariableBufferSize
)
{
  UINTN i;
  DEBUG((DEBUG_LOAD,"AML: Test Register\r\n"));
  if(AmlVariableBuffer == NULL)
  {
    DEBUG((EFI_D_WARN,"AML: Buffer is empty\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  else
  {
    DEBUG((DEBUG_LOAD,"  buffer:\r\n"));
  }
  for(i=0; i<AmlVariableBufferSize; i++)
    DEBUG((DEBUG_LOAD," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
  DEBUG((DEBUG_LOAD,"\r\n"));

  *( (UINT8*)(*AmlVariableBuffer)+AML_NAMESPACE_HEADER_SIZE) = 0xAA;
  
  DEBUG((DEBUG_LOAD,"  New chip info buffer:\r\n"));
  for(i=0; i<AmlVariableBufferSize; i++)
    DEBUG((DEBUG_LOAD," %02x", *( (UINT8*)(*AmlVariableBuffer)+i) ));
  DEBUG((DEBUG_LOAD,"\r\n"));
  return EFI_SUCCESS;
}

/**
 Sample register function to show how the AML update mechanism works. The function
 registers a callback to update the TEST variable in the DSDT
 changes the value of the TEST variable from 0xFF to 0xAA.

 To enable this function, search for the function call from earlier in this file 
 and uncomment the line. 
*/ 
  
EFI_STATUS
AmlUpdateRegisterTest ()
{
    EFI_QCOM_ACPIPLATFORM_PROTOCOL *pEfiAcpiPlatformProtocol = NULL;
    EFI_STATUS Status;

    CHAR8 AmlVariableName[AML_NAMESTRING_LENGTH] =  {'T','E','S','T'};
    Status = gBS->LocateProtocol(&gQcomAcpiPlatformProtocolGuid,
                                  NULL, (VOID**) &pEfiAcpiPlatformProtocol);
    if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN, "AML: Locate ACPI Protocol failed, Status = (0x%x)\r\n", Status));
        return Status;
      }
    

    // Update info 
    
    Status = pEfiAcpiPlatformProtocol->AmlVariableRegister( pEfiAcpiPlatformProtocol, AmlVariableName, 
      GETMODE_POSTFIX, NULL, AmlUpdateTestCallback);
    return Status;
}

/**
  Callback function for ACPI table load.

  @param Event           The event that is signaled.
  @param Context         Not used here.

**/
VOID
EFIAPI
ACPITableLoadNotification( 
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS Status = EFI_LOAD_ERROR;
  
  Status = ACPIPlatformAllocatePool();
  if(EFI_ERROR(Status))
    return;
  
  if(NumOfAcpiTables !=0)
  {
    NumOfAcpiTables = 0;
    ZeroMem(pPreFixArray,(MAX_ACPI_TABLES*sizeof(AcpiTableEntry)));    
    ZeroMem(pPostFixArray,(MAX_ACPI_TABLES*sizeof(AcpiTableEntry)));
  }
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE has %d registers. \r\n", NumRegisteredCallbacks));

  if (boot_from_emmc())
  {
    //Attempt to load from Plat parition in eMMC GPP
    Status = LoadAcpiFromVolume(L"\\ACPI\\",
                                &gEfiEmmcGppPartition1Guid,
                                &gEfiPlatPartitionTypeGuid,
                                TRUE
                               );    
    if (Status != EFI_SUCCESS) 
    {
      //Attempt to load from User Partition
      Status = LoadAcpiFromVolume(L"\\ACPI\\",
                                  &gEfiEmmcUserPartitionGuid,
                                  &gEfiPlatPartitionTypeGuid,
                                  TRUE
                                 );
        
    }
  }

  else if (boot_from_ufs())
  {
    //Attempt to load from UFS LUN4 PLAT partition
    Status = LoadAcpiFromVolume(L"\\ACPI\\",
                                &gEfiUfsLU4Guid,
                                &gEfiPlatPartitionTypeGuid,
                                TRUE
                               );    
  }

  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "ACPI: Failed to load from specified partitions !\r\n"));
    return;
  } 
   
  //Attempt to load MSDM.ACP from UFS LUN4 DPP partition
  Status = LoadAcpiFromDPP(L"MSDM.ACP", EFI_ACPI_5_0_DATA_MANAGEMENT_TABLE_SIGNATURE);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "ACPI: Failed to load from MSDM.ACP from DPP partition.\r\n"));
  } 

  //Parse the DSDT aml binary to set up the prefix array with variable information.
  Status = LoadAmlVariables(EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);
  if (Status == EFI_NOT_FOUND) {
      DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed to find DSDT\r\n"));
    DEBUG ((DEBUG_LOAD, "ACPI: Failed to find DSDT\r\n")); 
    return;
  }
  else if(Status != EFI_SUCCESS){
      DEBUG(( EFI_D_WARN, " ACPIPlatformDXE failed while parsing DSDT Error:%x\r\n", Status));
    DEBUG ((DEBUG_LOAD, "ACPI: Failed parsing DSDT: %x\r\n", Status)); 
  }

  // run the register for update chip information
  AmlUpdateRegister();

  // Load ACPI tables into memory
  Status = LoadAcpi();
  if(Status == EFI_SUCCESS) {
    DEBUG(( DEBUG_LOAD, "ACPI: tables loaded to system memory\r\n"));
      DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE finished callbacks and loaded tables\r\n"));
  } else {
    DEBUG ((EFI_D_WARN, "ACPI: Failed to load tables to system memory for HLOS!\r\n")); 
    DEBUG ((DEBUG_LOAD, "ACPI: Failed to load tables!\r\n")); 
  }
  return;
}

/**
  Get all ACPI tables.

  @param  This              Protocol instance pointer.
  @param  AcpiTablePtr      The pointer to the address acpi data to be copied to.
  @param  AcpiTableSize     The pointer to the size of acpi data.
  @param  IsPreFix          Table attribute, TRUE is pre fixup table, FLASE is post fixup table.

  @retval EFI_SUCCESS            The data was get successfully.
  @retval EFI_INVALID_PARAMETER  Bad parameter

**/

EFI_STATUS
GetAcpiTable (
  IN EFI_QCOM_ACPIPLATFORM_PROTOCOL  *This,
  OUT VOID                           *AcpiTablePtr,
  OUT UINTN                          *AcpiTableSize,
  IN BOOLEAN                          IsPreFix
  )
{
  UINT8  i;
  UINTN NumACPITable =0;

  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixArray);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable); 



  if(AcpiTablePtr == NULL || AcpiTableSize == NULL )
  {
    return  EFI_INVALID_PARAMETER;
  }

  *AcpiTableSize = 0;
  for( i= 0; i < NumACPITable; i++)
  {
    if( IsPreFix == TRUE )
    {
      CopyMem( (UINT8 *)AcpiTablePtr + (*AcpiTableSize), (UINT8 *)(pPreFixArray[i].Table),  pPreFixArray[i].TableSize); 
      *AcpiTableSize += pPreFixArray[i].TableSize;
    }
    else
    {
      CopyMem( (UINT8 *)AcpiTablePtr + (*AcpiTableSize), (UINT8 *)(pPostFixArray[i].Table), pPostFixArray[i].TableSize); 
      *AcpiTableSize += pPostFixArray[i].TableSize;
    }
  }
  return EFI_SUCCESS;
}


/**
  Get all ACPI table size.

  @param  This                   Protocol instance pointer.
  @param  PreFixUpAcpiTableSize  The pointer to the size of pre fixup acpi table size.
  @param  PostFixUpAcpiTableSize The pointer to the size of post fixup acpi table size.
  @param  TotalNumOfAcpiTables   The pointer to the number of acpi tables.

  @retval EFI_SUCCESS            The size was get successfully.
  @retval EFI_INVALID_PARAMETER  Bad parameter

**/

EFI_STATUS
GetAcpiTableSize (
  IN EFI_QCOM_ACPIPLATFORM_PROTOCOL  *This,
  IN OUT UINTN                       *PreFixUpAcpiTableSize,
  IN OUT UINTN                       *PostFixUpAcpiTableSize, 
  IN OUT UINTN                       *TotalNumOfAcpiTables 
  )
{
  UINTN  i;    
  UINTN NumACPITable =0;

  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPreFixArray);
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pPostFixArray);
  SET_ACPI_TABLE_ENTRY_LIMITS(NumACPITable); 

  if(PreFixUpAcpiTableSize == NULL || PostFixUpAcpiTableSize == NULL || TotalNumOfAcpiTables == NULL )
  {
    return  EFI_INVALID_PARAMETER;
  }

  *PreFixUpAcpiTableSize = 0;
  *PostFixUpAcpiTableSize = 0;
  for( i= 0; i < NumACPITable; i++)
  {
    *PreFixUpAcpiTableSize += pPreFixArray[i].TableSize;
    *PostFixUpAcpiTableSize += pPostFixArray[i].TableSize;
  }

  *TotalNumOfAcpiTables = NumACPITable;

  return EFI_SUCCESS;
}

/** 
  Register callback function to support ACPI table query/update services.

  @param[in] This                   A pointer to the EFI_QCOM_ACPIPLATFORM_REGISTER_PROTOCOL instance.
  @param[in] TableSignature         Signature of ACPI Table to be associated with. Such signature needs to 
                                    be a valid signature of an ACPI table found on the system. Or if the 
                                    signature is ACPI_TABLE_SIGNATURE_ALL, ACPIPLatformDXE assumes that the client driver operates
                                    on all ACPI tables.
  @param[in] GetMode                Mode of the GetTableCallback function. There are 3 modes, either prefix
                                    or postfix or both.
  @param[in] GetTableCallback       This is the pointer to get ACPI table callback function. Could be NULL.
                                    It could not be NULL if SetTableCallback is not NULL.
  @param[in] SetTableCallback       This is the pointer to set postfix ACPI table callback function. Could be NULL.

  @return
    EFI_SUCCESS            Registration succeed. 
    EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources,
                           Maximum number of registrations is defined as MAX_ACPI_REGISTERS.
    EFI_INVALID_PARAMETER  Registration is closed when ACPIPlatformDxe start working on ACPITableLoadEvent
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  GetMode is invalid
    EFI_INVALID_PARAMETER  Same table was already registered to be updated 
    EFI_INVALID_PARAMETER  Signature is ACPI_TABLE_SIGNATURE_ALL while SetTableCallback is not NULL
*/
EFI_STATUS
AcpiTableRegister (
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  UINT32                             TableSignature,
  IN  GETMODE                            GetMode,                                       
  IN  ACPITableGetCallbackPtr            GetTableCallback,
  IN  ACPITableSetCallbackPtr            SetTableCallback
  )
{
  UINTN i;
  UINTN NumCallbacks;
  
  CHECK_NULL_POINTER_ABORT_IF_TRUE(pRegisteredCallbacks);
  SET_REGISTER_CALLBACK_LIMITS(NumCallbacks);
  

  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "AcpiTableRegister called, Signature: %x; GetMode: %d;\r\n", 
    TableSignature, GetMode));
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "GetTableCallback ptr: %x; SetTableCallback ptr: %x.\r\n", 
    GetTableCallback, SetTableCallback));

  if(This == NULL)
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed, This is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(NumRegisteredCallbacks >=  MAX_ACPI_REGISTERS)
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed because of too many previous registrations.\r\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  if(RegistrationClosed ==  TRUE)
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed because "
      "registration is closed as ACPIPlatformDxe started working on "
      "ACPITableLoadEvent.\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(GetMode != GETMODE_PREFIX && GetMode != GETMODE_POSTFIX && GetMode != GETMODE_BOTH)
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed, GetMode "
      "is invalid, it is %d.\r\n", GetMode));
    return EFI_INVALID_PARAMETER;
  }
  if(TableSignature == ACPI_TABLE_SIGNATURE_ALL && SetTableCallback != NULL)
  {
    DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed, Set "
      "callback pointer is not NULL, and the client driver attempts to operate "
      "on all tables with Signature as ACPI_TABLE_SIGNATURE_ALL.\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < NumCallbacks; i++)
  {
    if (pRegisteredCallbacks[i].SetTableCallback != NULL 
      && pRegisteredCallbacks[i].TableSignature == TableSignature)
    {
      DEBUG(( EFI_D_WARN, "ACPIPlatformDXE Callback registration failed, Set "
        "callback pointer was already registered for signature %x.\r\n", 
        TableSignature));
      return EFI_INVALID_PARAMETER;
    }
  }
  if(NumCallbacks < MAX_ACPI_REGISTERS)  
  {
      pRegisteredCallbacks[NumCallbacks].TableSignature = TableSignature;
      pRegisteredCallbacks[NumCallbacks].GetMode = GetMode;
      pRegisteredCallbacks[NumCallbacks].GetTableCallback = GetTableCallback;
      pRegisteredCallbacks[NumCallbacks].SetTableCallback = SetTableCallback;
  
      NumRegisteredCallbacks++;
  }
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "ACPIPlatformDXE callback registration for index %d "
    "successful:\r\n", NumRegisteredCallbacks));

  return EFI_SUCCESS;
}

/** 
  Register callback function to support ACPI table query/update services.

  @param[in] This                       A pointer to the EFI_QCOM_ACPIPLATFORM_REGISTER_PROTOCOL instance.
  @param[in] VariableName               Name of the AML Variable being registered for callback. It needs to 
                                        be a valid name of a variable found in the AML. Or if the name is
                                        "ALL", ACPIPLatformDXE will return a list of all variables and their values
  @param[in] GetMode                    Mode of the GetAmlVariableCallback function. There are 3 modes: prefix,
                                        postfix, or both.
  @param[in] GetAmlVariableCallback    This is the pointer to get AML variable callback function. Could be NULL.
                                        It could not be NULL if SetAmlVariableCallback is not NULL.
  @param[in] SetAmlVariableCallback    This is the pointer to set postfix AML variable callback function. Could be NULL.

  @return
    EFI_SUCCESS            Registration succeed. 
    EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources,
                           Maximum number of registrations is defined as MAX_AML_VARIABLES_REGISTERS.
    EFI_INVALID_PARAMETER  Registration is closed when ACPIPlatformDxe start working on ACPITableLoadEvent
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  GetMode is invalid
    EFI_INVALID_PARAMETER  Same variable was already registered to be updated 
    EFI_INVALID_PARAMETER  Variable name is 'ALL' while SetTableCallback is not NULL
*/
EFI_STATUS
AmlVariableRegister
(
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  CHAR8                              VariableName[AML_NAMESTRING_LENGTH],
  IN  GETMODE                            GetMode,                                       
  IN  AMLVariableGetCallbackPtr          GetAmlVariableCallback,
  IN  AMLVariableSetCallbackPtr          SetAmlVariableCallback
)
{
  UINTN i;
  UINTN j;

  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "AML: Registering "));
  for (i = 0; i < AML_NAMESTRING_LENGTH; i++)
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "%c", VariableName[i]));
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "\r\n"));

  if(This == NULL)
  {
    DEBUG(( EFI_D_WARN, "AML: Registration failed- This is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(NumRegisteredAmlVariableCallbacks ==  MAX_AML_VARIABLES)
  {
    DEBUG(( EFI_D_WARN, "AML: Registration failed- too many previous registrations.\r\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  if(RegistrationClosed ==  TRUE)
  {
    DEBUG(( EFI_D_WARN, "AML: Registration failed- ACPITableLoadEvent occurred.\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(GetMode != GETMODE_PREFIX && GetMode != GETMODE_POSTFIX && GetMode != GETMODE_BOTH)
  {
    DEBUG(( EFI_D_WARN, "AML: Registration failed- GetMode %d is invalid.\r\n", GetMode));
    return EFI_INVALID_PARAMETER;
  }
  if(SetAmlVariableCallback != NULL && (AsciiStrnCmp(VariableName, AMLVARIABLENAME_ALL, AML_NAMESTRING_LENGTH) == 0) )
  {
    DEBUG(( EFI_D_WARN, "AML: Registration failed- Set "
      "callback pointer is not NULL, and the client driver attempts to update "
      "on all variables.\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < NumRegisteredAmlVariableCallbacks; i++)
  {
    if (pRegisteredAmlVariableCallbacks[i].SetAmlVariableCallback != NULL 
      && AsciiStrnCmp(pRegisteredAmlVariableCallbacks[i].VariableName, VariableName, AML_NAMESTRING_LENGTH) == 0)
    {
      DEBUG(( EFI_D_WARN, "AML: Registration failed- Set callback pointer already registered for "));
      for(j=0; j < AML_NAMESTRING_LENGTH; j++)
        DEBUG(( EFI_D_WARN, "%c", VariableName[j]));
      DEBUG(( EFI_D_WARN,"\r\n"));
      return EFI_INVALID_PARAMETER;
    }
  }

  for(j = 0; j < AML_NAMESTRING_LENGTH; j++)
    pRegisteredAmlVariableCallbacks[NumRegisteredAmlVariableCallbacks].VariableName[j] = VariableName[j];
  pRegisteredAmlVariableCallbacks[NumRegisteredAmlVariableCallbacks].GetMode = GetMode;
  pRegisteredAmlVariableCallbacks[NumRegisteredAmlVariableCallbacks].GetAmlVariableCallback = GetAmlVariableCallback;
  pRegisteredAmlVariableCallbacks[NumRegisteredAmlVariableCallbacks].SetAmlVariableCallback = SetAmlVariableCallback;
  pRegisteredAmlVariableCallbacks[NumRegisteredAmlVariableCallbacks].TableSignature = 
    EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "Aml object callback registration for index %d "
    "successful:\r\n", NumRegisteredAmlVariableCallbacks));
  NumRegisteredAmlVariableCallbacks++;
  return EFI_SUCCESS;
}


/**
  Entrypoint of Acpi Platform driver.

  @param  ImageHandle
  @param  SystemTable

  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  /*
   * Set up to be notified when the ACPITableLoadEvent is signalled.
   */
  EFI_STATUS Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, 
                                         TPL_CALLBACK, 
                                         ACPITableLoadNotification, 
                                         NULL,
                                         &gEfiACPITableLoadGuid,
                                         &ACPITableLoadEvent);

  ASSERT_EFI_ERROR (Status);

    // Install RNG protocols and Test protocol
  Status = gBS->InstallMultipleProtocolInterfaces (
                      &ImageHandle,
                      &gQcomAcpiPlatformProtocolGuid,
                      (void **)&AcpiPlatformProtocolImpl,
                      &gQcomAcpiPlatformTestProtocolGuid,
                      (void **)&AcpiPlatformTestProtocolImpl,
                       NULL
                    );
  if (EFI_ERROR (Status))
  {
    DEBUG(( ACPI_PLATFORM_DEBUG_PRINT, "AcpiPlatformDxeInitialize: Install Protocol failed, Status = (0x%p)\r\n", Status));
  }

  // We change those variable from array to pointer and allocate here which required by UEFI team for reducing .bss footprint of UEFI
  pPreFixArray = (AcpiTableEntry *) AllocatePool(MAX_ACPI_TABLES*sizeof(AcpiTableEntry));
  pPostFixArray= (AcpiTableEntry *) AllocatePool(MAX_ACPI_TABLES*sizeof(AcpiTableEntry));
  pPreFixAmlVariableArray  = (AmlVariableEntry *) AllocatePool(MAX_AML_VARIABLES*sizeof(AmlVariableEntry));  
  pPostFixAmlVariableArray = (AmlVariableEntry *) AllocatePool(MAX_AML_VARIABLES*sizeof(AmlVariableEntry)); 
  pRegisteredCallbacks  = (RegisterTableType *) AllocatePool(MAX_ACPI_REGISTERS*sizeof(RegisterTableType));
  pRegisteredAmlVariableCallbacks = (RegisterAmlVariableTable *) AllocatePool(MAX_AML_REGISTERS*sizeof(RegisterAmlVariableTable));
  pFileName = (CHAR8 *) AllocatePool(MAX_PATHNAME);
  pFilePath = (CHAR8 *) AllocatePool(MAX_PATHNAME);

  if((pPreFixArray ==NULL)
    ||(pPostFixArray ==NULL)
    ||(pPreFixAmlVariableArray ==NULL)
    ||(pPostFixAmlVariableArray ==NULL)
    ||(pRegisteredCallbacks ==NULL)
    ||(pRegisteredAmlVariableCallbacks ==NULL)
    ||(pFileName ==NULL)
    ||(pFilePath ==NULL))
  {
  
    if(pPreFixArray!=NULL)
        FreePool(pPreFixArray);

    if(pPostFixArray!=NULL)
        FreePool(pPostFixArray);

    if(pPreFixAmlVariableArray!=NULL)
        FreePool(pPreFixAmlVariableArray);  

    if(pPostFixAmlVariableArray!=NULL)
        FreePool(pPostFixAmlVariableArray); 

    if(pRegisteredCallbacks!=NULL)
        FreePool(pRegisteredCallbacks);

    if(pRegisteredAmlVariableCallbacks!=NULL)
        FreePool(pRegisteredAmlVariableCallbacks);

    if(pFileName!=NULL)
        FreePool(pFileName); 

    if(pFilePath!=NULL)
        FreePool(pFilePath );
    
    return EFI_OUT_OF_RESOURCES;
  }

  Status = MountFatPartition(L"plat");
  if (EFI_ERROR (Status))
  {
    gBS->CloseEvent(ACPITableLoadEvent);
  }
  
  return Status;
}
