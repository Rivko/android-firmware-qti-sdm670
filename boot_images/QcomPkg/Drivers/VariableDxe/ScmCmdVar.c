/** @file ScmCmdVar.c
   
  Implements library functions for SCM commands used for Variable Storage.

  Copyright (c) 2011-2017, Qualcomm Technologies, Inc.  All 
  Rights Reserved. Qualcomm Technologies Proprietary and 
  Confidential. 
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/22/17   bd      SCM call for deleting secure boot variables
 09/30/16   aus     Fixed GetNextVariable name to support 0 size for VarNameSize
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 05/16/14   tonyo   Support for 32/64 bit environment
 03/14/13   niting  Added dump of UefiSecApp log buffer
 02/21/13   shl     ScmDxe cleanup.
 01/10/13   niting  Updated to use SCM protocol. Removed unused functions.
 12/10/12   yg      Align response pointer
 06/12/12   niting  Allow NULL Data when calling ScmCmdSetVariable for delete
 05/01/12   niting  Raised TPL of SCM calls
 02/24/12   niting  Added physical address pointer for SCM buffer to pass to TZ  
 10/20/11   niting  Initial revision.

=============================================================================*/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ScmCmdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIScm.h>

#include "ScmCmdVar.h"

QCOM_SCM_PROTOCOL *QcomScmProtocol = NULL;
STATIC SCM_CMD_PARAMS_T ScmCmdInitParams = {NULL, 0, 0, NULL};

/*===========================================================================
  UEFI VARIABLE SERVICES
===========================================================================*/
#define SERVICE_UEFI_VAR_CREATE_CMD(x) (TZEXEC_SVC_UEFI_VAR_ID | x)

#define SERVICE_UEFI_VAR_GET_VARIABLE                       \
        SERVICE_UEFI_VAR_CREATE_CMD(0)     /* Function 0 */

#define SERVICE_UEFI_VAR_SET_VARIABLE                       \
        SERVICE_UEFI_VAR_CREATE_CMD(1)     /* Function 1 */

#define SERVICE_UEFI_VAR_GET_NEXT_VARIABLE_NAME             \
        SERVICE_UEFI_VAR_CREATE_CMD(2)     /* Function 2 */

#define SERVICE_UEFI_VAR_QUERY_VARIABLE_INFO                \
        SERVICE_UEFI_VAR_CREATE_CMD(3)     /* Function 3 */

#define SERVICE_UEFI_VAR_VARIABLE_INITIALIZE                \
        SERVICE_UEFI_VAR_CREATE_CMD(6)     /* Function 6 */

#define SERVICE_UEFI_VAR_EXIT_BOOT_SERVICES                 \
        SERVICE_UEFI_VAR_CREATE_CMD(10)     /* Function 10 */

#define SERVICE_UEFI_VAR_SYNC_VAR_TABLES                    \
        SERVICE_UEFI_VAR_CREATE_CMD(11)     /* Function 11 */

#define SERVICE_UEFI_VAR_DUMP_LOG_BUFFER                    \
        SERVICE_UEFI_VAR_CREATE_CMD(12)     /* Function 12 */
        
#define SERVICE_UEFI_VAR_DEL_SECBOOT_VARS                   \
        SERVICE_UEFI_VAR_CREATE_CMD(13)     /* Function 13 */
        
#define SERVICE_UEFI_VAR_DIS_DEL_SECBOOT_VARS_MS_CALL       \
        SERVICE_UEFI_VAR_CREATE_CMD(14)     /* Function 14 */             

/* RETURN_UINTN_EFI_STATUS(a) This is to account for EFI status that is returned as 32bit efi_status code
 in 64bit environment. The conversions of 64 bit are taken care of only in non secure context and
 all secure API's are 32 bit, with the assumption that none of the sizes would exceed 4GB*/

#define RETURN_UINTN_EFI_STATUS(a)                       \
  (((a) == (0)) ? (a) : (ENCODE_ERROR((a)&0xFF)))

/*===========================================================================
  GetVariable
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;

  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;

  /* Parameters required for the service */
  /** Byte offset from the start of this structure to the VariableName
   *  requested */
  UINT32             VariableNameOffset;

  /** Size in bytes of the VariableName requested */
  UINT32             VariableNameSize;

  /** Byte offset from the start of this structure to the Vendor Guid for the
   *  variable requested */
  UINT32             VendorGuidOffset;

  /** Size in bytes of the Vendor Guid */
  UINT32             VendorGuidSize;

  /** Size in bytes of the input buffer */
  UINT32             DataSize;
} TZ_SVC_REQ_VAR_GET_VARIABLE;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;

  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;

  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

  /** Attributes of the variable found */
  UINT32             Attributes;

  /** Byte offset from the start of this structure to the variable data
   *  requested */
  UINT32             DataOffset;

  /** Size in bytes of the data returned; if error occurred, this will reflect
   *  either the buffer size required to store the data or the original input
   *  buffer size */
  UINT32             DataSize;

} TZ_SVC_RSP_VAR_GET_VARIABLE;

/*===========================================================================
  SetVariable
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Parameters required for the service */
  /** Byte offset from the start of this structure to the VariableName
   *  requested */
  UINT32             VariableNameOffset;

  /** Size in bytes of the VariableName requested */
  UINT32             VariableNameSize;

  /** Byte offset from the start of this structure to the Vendor Guid for the
   *  variable requested */
  UINT32             VendorGuidOffset;

  /** Size in bytes of the Vendor Guid */
  UINT32             VendorGuidSize;

  /** Attributes of the variable to be stored */
  UINT32             Attributes;

  /** Byte offset from the start of this structure to the variable data to be
   *  stored */
  UINT32             DataOffset;

  /** Size in bytes of the data to store */
  UINT32             DataSize;
} TZ_SVC_REQ_VAR_SET_VARIABLE;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

  /** Id of the table that was updated - this is to be used when retrieving the
   *  data to sync to storage */
  UINT32             TableId;

  /** Size in bytes of the table that was updated */
  UINT32             SyncSize;
} TZ_SVC_RSP_VAR_SET_VARIABLE;

/*===========================================================================
  GetNextVariableName
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Parameters required for the service */
  /** Byte offset from the start of this structure to the Vendor Guid for the
   *  input variable */
  UINT32             VendorGuidOffset;

  /** Size in bytes of the Vendor Guid */
  UINT32             VendorGuidSize;

  /** Byte offset from the start of this structure to the input VariableName */
  UINT32             VariableNameOffset;

  /** Size in bytes of the input VariableName */
  UINT32             VariableNameSize;

} TZ_SVC_REQ_VAR_GET_NEXT_VARIABLE_NAME;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

  /** Byte offset from the start of this structure to the Vendor Guid for the
   *  next variable */
  UINT32             VendorGuidOffset;

  /** Size in bytes of the Vendor Guid corresponding to the next variable */
  UINT32             VendorGuidSize;

  /** Byte offset from the start of this structure to the next VariableName */
  UINT32             VariableNameOffset;

  /** Size in bytes of the next VariableName */
  UINT32             VariableNameSize;

} TZ_SVC_RSP_VAR_GET_NEXT_VARIABLE_NAME;

/*===========================================================================
  QueryVariableInfo
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Parameters required for the service */
  /** Attributes for the variable info requested */
  UINT32             Attributes;
} TZ_SVC_REQ_VAR_QUERY_VARIABLE_INFO;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

  /** Padding required for alignment */
  UINT32             Padding;

  /** Maximum storage space in bytes available for the variables specified by the
   *  input Attributes */
  UINT64             MaximumVariableStorageSize;
  /** Remaining storage space in bytes available for the variables specified by
   *  the input Attributes */
  UINT64             RemainingVariableStorageSize;
  /** Maximum size in bytes of a variable that could be stored specified by the
   *  input Attributes - this number accounts for the maximum overhead needed to
   *  store an individual variable */
  UINT64             MaximumVariableSize;
} TZ_SVC_RSP_VAR_QUERY_VARIABLE_INFO;

/*===========================================================================
  VariableInitialize
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Parameters required for the service */
  /** Byte offset from the start of this structure to the parameters used to
   *  initialize the variable store */
  UINT32             InitParamsOffset;

  /** Size in bytes of the variable initialization parameters structure */
  UINT32             InitParamsSize;

} TZ_SVC_REQ_VAR_VARIABLE_INITIALIZE;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_VARIABLE_INITIALIZE;

/*===========================================================================
  ExitBootServices
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
} TZ_SVC_REQ_VAR_EXIT_BOOT_SERVICES;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_EXIT_BOOT_SERVICES;

/*===========================================================================
  SyncVarTables
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;

  /* Parameters required for the service */
  /** ID of table to sync */
  UINT32             TableId;

} TZ_SVC_REQ_VAR_SYNC_VAR_TABLES;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_SYNC_VAR_TABLES;

/*===========================================================================
  DumpLogBuffer
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Parameters required for the service */
  /** Size of the input buffer */
  UINT32             DumpLogBufferSize;
  
} TZ_SVC_REQ_VAR_DUMP_LOG_BUFFER;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

  /* Parameters required for the service */
  /** Byte offset from the start of this structure to the log buffer */
  UINT32             DumpLogBufferOffset;

  /** Size of the output buffer */
  UINT32             DumpLogBufferSize;

} TZ_SVC_RSP_VAR_DUMP_LOG_BUFFER;

/*===========================================================================
  Delete SecBoot Vars
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length; 

  
} TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS;

/*===========================================================================
  Milestone call to ignore Delete SecBoot Vars SCM call
===========================================================================*/
typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length; 

  
} TZ_SVC_REQ_VAR_SECBOOT_VARS_MILESTONE_CALL;

typedef struct
{
  /** First 4 bytes should always be command id */
  UINT32             CommandId;
  
  /** Second 4 bytes should always be length in bytes of this structure
   *  including the data contained within this command packet */
  UINT32             Length;
  
  /* Return parameters required for the service */
  /** Return status of the operation */
  UINT32             Status;

} TZ_SVC_RSP_VAR_SECBOOT_VARS_MILESTONE_CALL;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/**

  This code loads the SCM protocol instance to use for any SCM calls. This
  also returns the loaded UefiSecApp App ID.
 
  @param  UefiSecAppId              App ID corresponding to loaded UefiSecApp
                         
  @return EFI_SUCCESS               If app has been loaded and
                                    QCOM_SCM_PROTOCOL in loaded.

**/
EFI_STATUS
EFIAPI
ScmCmdInitUefiSecApp ( 
  OUT     UINT32            *UefiSecAppId
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8* UefiSecAppName = "qcom.tz.uefisecapp";
  UINTN ScmCmdBufferSize = 0x20000; /* 128KB */
  UINTN ScmCmdBufferPages = ScmCmdBufferSize/EFI_PAGE_SIZE;

  if (UefiSecAppId == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((QcomScmProtocol != NULL) && (ScmCmdInitParams.ScmAppId != 0) && 
      (ScmCmdInitParams.ScmCmdBuffer != NULL))
  {
    *UefiSecAppId = ScmCmdInitParams.ScmAppId;
    return EFI_SUCCESS;
  }

  DEBUG((EFI_D_INFO, "Attempting to load SCM protocol\n"));
  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&QcomScmProtocol);
  ASSERT_EFI_ERROR(Status);

  DEBUG((EFI_D_INFO, "Attempting to retrieve uefisecapp App ID\n"));
  Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                           APP_QUERY_APPID_CMD,
                                           &(ScmCmdInitParams.ScmAppId),
                                           UefiSecAppName,
                                           AsciiStrSize(UefiSecAppName),
                                           NULL,
                                           0);
  if ((Status != EFI_SUCCESS) || (ScmCmdInitParams.ScmAppId == 0))
  {
    DEBUG((EFI_D_ERROR, "Failed to retrieve App ID for uefisecapp\n"));
    return Status;
  }

  *UefiSecAppId = ScmCmdInitParams.ScmAppId;

  /* Initialize scratch buffer to copy data to for request and responses */
  ScmCmdInitParams.ScmCmdBuffer = AllocateAlignedPages(ScmCmdBufferPages, EFI_PAGE_SIZE);
  if (ScmCmdInitParams.ScmCmdBuffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
  }

  ScmCmdInitParams.ScmCmdBufferSize = ScmCmdBufferSize;
  ScmCmdInitParams.ScmCmdBufferPhys = ScmCmdInitParams.ScmCmdBuffer;
  Status = ScmCmdInit(&ScmCmdInitParams);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**

  This code finds variable in storage blocks (Volatile or Non-Volatile).
   
  @param VariableName               Name of variable to be retrieved.
  @param VendorGuid                 Vendor Guid corresponding to the variable to
                                    be retrieved.
  @param Attributes                 Pointer to the attributes of the variable 
                                    found. If this is NULL, then this buffer
                                    will not be updated.
  @param DataSize                   Size of data found. If size is less than the
                                    size of the retrieved data, this value
                                    contains the required size.
  @param Data                       Pointer to the data.
                      
  @return EFI_SUCCESS               Found the specified variable successfully.
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_NOT_FOUND             Variable was not found.
  @return EFI_BUFFER_TO_SMALL       DataSize is too small for the result.
  @return EFI_DEVICE_ERROR          The variable could not be retrieved due to 
                                    a hardware error.

**/
EFI_STATUS
EFIAPI
ScmCmdGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_GET_VARIABLE *Request = 
    (TZ_SVC_REQ_VAR_GET_VARIABLE *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_GET_VARIABLE *Response = NULL;
  UINT8* ParamLocation = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((VariableName == NULL) || (VendorGuid == NULL) || 
        (DataSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (ScmCmdParamsInitialized == FALSE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;      
    }

    Request->CommandId = SERVICE_UEFI_VAR_GET_VARIABLE;
    Request->VariableNameOffset = sizeof(TZ_SVC_REQ_VAR_GET_VARIABLE);
    Request->VariableNameSize = StrSize(VariableName);
    Request->VendorGuidOffset = Request->VariableNameOffset + Request->VariableNameSize;
    Request->VendorGuidSize = sizeof(EFI_GUID);
    Request->DataSize = (UINT32)*DataSize;
    Request->Length = Request->VendorGuidOffset + Request->VendorGuidSize;
  
    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Copy remaining input parameters for the function */
    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VariableNameOffset);
    CopyMem(ParamLocation, VariableName, Request->VariableNameSize);
    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VendorGuidOffset);
    CopyMem(ParamLocation, VendorGuid, Request->VendorGuidSize);
  
    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_GET_VARIABLE *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_GET_VARIABLE) + ((UINT32)*DataSize);
  
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);
    if(Attributes != NULL)
    {
      *Attributes = Response->Attributes;
    }

    if ((Response->DataSize > 0) && (Status == EFI_SUCCESS) &&
        (Data != NULL))
    {
      if (Response->DataSize > (UINT32)*DataSize)
      {
        *DataSize = (UINTN)Response->DataSize;
        Status = EFI_BUFFER_TOO_SMALL;
        break;
      }
      ParamLocation = (UINT8*)((UINT8*)Response + Response->DataOffset);
      CopyMem(Data, ParamLocation, Response->DataSize);
    }
    *DataSize = (UINTN)Response->DataSize;

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Sets the value of a variable.

  @param VariableName               A Null-terminated string that is the name of 
                                    the vendor’s variable. Each VariableName is
                                    unique for each VendorGuid. VariableName
                                    must contain 1 or more characters. If
                                    VariableName is an empty string, then
                                    EFI_INVALID_PARAMETER is returned.
  @param VendorGuid                 A unique identifier for the vendor.
  @param Attributes                 Attributes bitmask to set for the variable.
  @param DataSize                   The size in bytes of the Data buffer. Unless 
                                    the EFI_VARIABLE_APPEND_WRITE,
                                    EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS, or
                                    EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
                                    attribute is set, a size of zero causes the
                                    variable to be deleted. When the
                                    EFI_VARIABLE_APPEND_WRITE attribute is set,
                                    then a SetVariable() call with a DataSize of
                                    zero will not cause any change to the
                                    variable value (the timestamp associated
                                    with the variable may be updated).
  @param Data                       The contents for the variable. 
  @param TableId                    Unique identifier used to indicate which 
                                    table was updated by this call to
                                    SetVariable.
  @param SyncSize                   Size in bytes of the table that was updated.
 
  @return EFI_SUCCESS               The firmware has successfully stored the
                                    variable and its data as defined by the
                                    Attributes.
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_INVALID_PARAMETER     An invalid combination of attribute bits
                                    was supplied, or the DataSize exceeds the
                                    maximum allowed.
  @return EFI_INVALID_PARAMETER     VariableName is an empty string.
  @return EFI_DEVICE_ERROR          The variable could not be saved due to 
                                    a hardware error.
  @return EFI_WRITE_PROTECTED       The variable in question is read-only.
  @return EFI_OUT_OF_RESOURCES      Not enough storage is available to hold the
                                    variable and its data.
  @return EFI_SECURITY_VIOLATION    The variable could not be written due
                                    to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS or 
                                    EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACESS
                                    being set, but the AuthInfo does NOT pass
                                    the validation check carried out by the
                                    firmware.
  @return EFI_NOT_FOUND             The variable trying to be updated or deleted
                                    was not found.

**/
EFI_STATUS
EFIAPI
ScmCmdSetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  IN      UINT32            Attributes,
  IN      UINTN             DataSize,
  IN      VOID              *Data,
  OUT     UINT32            *TableId,
  OUT     UINT32            *SyncSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_SET_VARIABLE *Request = 
    (TZ_SVC_REQ_VAR_SET_VARIABLE *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_SET_VARIABLE *Response = NULL;
  UINT8* ParamLocation = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((VariableName == NULL) || (VendorGuid == NULL) || 
        ((Data == NULL) && (DataSize > 0)) || 
        (TableId == NULL) || (SyncSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (ScmCmdParamsInitialized == FALSE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;      
    }

    Request->CommandId = SERVICE_UEFI_VAR_SET_VARIABLE;
    Request->VariableNameOffset = sizeof(TZ_SVC_REQ_VAR_SET_VARIABLE);
    Request->VariableNameSize = StrSize(VariableName);
    Request->VendorGuidOffset = Request->VariableNameOffset + Request->VariableNameSize;
    Request->VendorGuidSize = sizeof(EFI_GUID);
    Request->Attributes = Attributes;
    Request->DataOffset = Request->VendorGuidOffset + Request->VendorGuidSize;
    Request->DataSize = (UINT32)DataSize;
    Request->Length = Request->DataOffset + Request->DataSize;

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Copy remaining input parameters for the function */
    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VariableNameOffset);
    CopyMem(ParamLocation, VariableName, Request->VariableNameSize);

    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VendorGuidOffset);
    CopyMem(ParamLocation, VendorGuid, Request->VendorGuidSize);

    if (Request->DataSize > 0)
    {
      ParamLocation = (UINT8*)((UINT8*)Request + 
                               Request->DataOffset);
      CopyMem(ParamLocation, Data, Request->DataSize);
    }

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_SET_VARIABLE *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_SET_VARIABLE);
 
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);
    *TableId = Response->TableId;
    *SyncSize = Response->SyncSize;

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Enumerates the current variable names.

  @param VariableNameSize           Size in bytes of the VariableName buffer.
  @param VariableName               On input, supplies the last VariableName 
                                    that was returned by GetNextVariableName().
                                    On output, returns the Null-terminated
                                    string of the current variable.
  @param VendorGuid                 On input, supplies the last VendorGuid that 
                                    was returned by GetNextVariableName(). On
                                    output, returns the VendorGuid of the
                                    current variable.

  @return EFI_SUCCESS               The function completed successfully.
  @return EFI_NOT_FOUND             The next variable was not found.
  @return EFI_BUFFER_TOO_SMALL      The VariableNameSize is too small for the 
                                    result. VariableNameSize has been updated
                                    with the size needed to complete the
                                    request.
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_DEVICE_ERROR          The variable name could not be retrieved 
                                    due to a hardware error.

**/
EFI_STATUS
EFIAPI
ScmCmdGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_GET_NEXT_VARIABLE_NAME *Request = 
    (TZ_SVC_REQ_VAR_GET_NEXT_VARIABLE_NAME *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_GET_NEXT_VARIABLE_NAME *Response = NULL;
  UINT8* ParamLocation = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((VariableNameSize == NULL) || (VariableName == NULL) || 
        (VendorGuid == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (ScmCmdParamsInitialized == FALSE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;      
    }

    Request->CommandId = SERVICE_UEFI_VAR_GET_NEXT_VARIABLE_NAME;
    Request->VendorGuidOffset = sizeof(TZ_SVC_REQ_VAR_GET_NEXT_VARIABLE_NAME);
    Request->VendorGuidSize = sizeof(EFI_GUID);
    Request->VariableNameOffset = Request->VendorGuidOffset + Request->VendorGuidSize;
    Request->VariableNameSize = (UINT32)*VariableNameSize;
    Request->Length = Request->VariableNameOffset + Request->VariableNameSize;
  
    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Copy remaining input parameters for the function */
    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VendorGuidOffset);
    CopyMem(ParamLocation, VendorGuid, Request->VendorGuidSize);

    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->VariableNameOffset);
    CopyMem(ParamLocation, VariableName, StrSize(VariableName));
  
    
    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_GET_NEXT_VARIABLE_NAME *)((UINT8*)Request + ResponseOffset);
    ResponseLength = RequestLength - sizeof(TZ_SVC_REQ_VAR_GET_NEXT_VARIABLE_NAME) + 
                     sizeof(TZ_SVC_RSP_VAR_GET_NEXT_VARIABLE_NAME);
      
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

    if (Status == EFI_SUCCESS)
    {
      if (Response->VendorGuidSize != sizeof(EFI_GUID))
      {
        Status = EFI_DEVICE_ERROR;
        break;
      }
      ParamLocation = (UINT8*)((UINT8*)Response + 
                               Response->VendorGuidOffset);
      CopyMem(VendorGuid, ParamLocation, Response->VendorGuidSize);
  
      if (Response->VariableNameSize > (UINT32)*VariableNameSize)
      {
	    *VariableNameSize = (UINTN)Response->VariableNameSize;
        Status = EFI_BUFFER_TOO_SMALL;
        break;
      }
      ParamLocation = (UINT8*)((UINT8*)Response + 
                               Response->VariableNameOffset);
      CopyMem(VariableName, ParamLocation, Response->VariableNameSize);
      *VariableNameSize = (UINTN)Response->VariableNameSize;
    }
	else if (Status == EFI_BUFFER_TOO_SMALL)
	{
	  *VariableNameSize = (UINTN)Response->VariableNameSize;
	}

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Returns information about the EFI variables.

  @param Attributes                     Attributes bitmask to specify the type 
                                        of variables on which to return
                                        information.
  @param MaximumVariableStorageSize     Pointer to the maximum size of the 
                                        storage space available for the EFI
                                        variables associated with the attributes
                                        specified.
  @param RemainingVariableStorageSize   Pointer to the remaining size of the 
                                        storage space available for EFI
                                        variables associated with the attributes
                                        specified.
  @param MaximumVariableSize            Pointer to the maximum size of an 
                                        individual EFI variables associated with
                                        the attributes specified.

  @return EFI_SUCCESS                   Valid answer returned.
  @return EFI_INVALID_PARAMETER         An invalid combination of attributes 
                                        bits was supplied.
  @return EFI_UNSUPPORTED               The attribute is not supported on this 
                                        platform, and the
                                        MaximumVariableStorageSize,
                                        RemainingVariableStorageSize,
                                        MaximumVariableSize are undefined.

**/
EFI_STATUS
EFIAPI
ScmCmdQueryVariableInfo (
  IN      UINT32            Attributes,
  OUT     UINT64            *MaximumVariableStorageSize,
  OUT     UINT64            *RemainingVariableStorageSize,
  OUT     UINT64            *MaximumVariableSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_QUERY_VARIABLE_INFO *Request = 
    (TZ_SVC_REQ_VAR_QUERY_VARIABLE_INFO *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_QUERY_VARIABLE_INFO *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((MaximumVariableStorageSize == NULL) || 
        (RemainingVariableStorageSize == NULL) || 
        (MaximumVariableSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (ScmCmdParamsInitialized == FALSE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Request->CommandId = SERVICE_UEFI_VAR_QUERY_VARIABLE_INFO;
    Request->Attributes = Attributes;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_QUERY_VARIABLE_INFO);
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_QUERY_VARIABLE_INFO *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_QUERY_VARIABLE_INFO);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
  
    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);
    *MaximumVariableStorageSize = 
      Response->MaximumVariableStorageSize;
    *RemainingVariableStorageSize = 
      Response->RemainingVariableStorageSize;
    *MaximumVariableSize = Response->MaximumVariableSize;

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Initializes the Variable storage in TZ.

  @param InitParams                 Pointer to parameters used to initialize the
                                    tables in TZ.
  @param InitParamsSize             Size in bytes of InitParams. 

  @return EFI_SUCCESS               Successfully returned the table.
  @return EFI_INVALID_PARAMETER     Invalid parameter passed.
  @return EFI_OUT_OF_RESOURCES      Not enough memory available to store the 
                                    tables.

**/
EFI_STATUS
EFIAPI
ScmCmdVariableInitialize (
  IN      UINT8                              *InitParams,
  IN      UINT32                             InitParamsSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_VARIABLE_INITIALIZE *Request = 
    (TZ_SVC_REQ_VAR_VARIABLE_INITIALIZE *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_VARIABLE_INITIALIZE *Response = NULL;
  UINT8* ParamLocation = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((InitParams == NULL) || (InitParamsSize == 0))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (ScmCmdParamsInitialized == FALSE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;      
    }

    Request->CommandId = SERVICE_UEFI_VAR_VARIABLE_INITIALIZE;
    Request->InitParamsOffset = sizeof(TZ_SVC_REQ_VAR_VARIABLE_INITIALIZE);
    Request->InitParamsSize = InitParamsSize;
    Request->Length = Request->InitParamsOffset + Request->InitParamsSize;
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_VARIABLE_INITIALIZE *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_VARIABLE_INITIALIZE);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    ParamLocation = (UINT8*)((UINT8*)Request + 
                             Request->InitParamsOffset);
    CopyMem(ParamLocation, InitParams, Request->InitParamsSize);
  
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Notifies UEFI in TZ that ExitBootServices has occurred.

  @return EFI_SUCCESS               Successfully notified. 
  @return EFI_INVALID_PARAMETER     Invalid parameter passed.

**/
EFI_STATUS
EFIAPI
ScmCmdExitBootServices (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_EXIT_BOOT_SERVICES *Request = 
    (TZ_SVC_REQ_VAR_EXIT_BOOT_SERVICES *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_EXIT_BOOT_SERVICES *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    Request->CommandId = SERVICE_UEFI_VAR_EXIT_BOOT_SERVICES;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_EXIT_BOOT_SERVICES);
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_EXIT_BOOT_SERVICES *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_EXIT_BOOT_SERVICES);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
  
    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Syncs tables based on ID. If TABLE_ID_RESERVED is passed, all tables are synced.

  @param TableId                    ID corresponding to the tables to sync.

  @return EFI_SUCCESS               Successfully synced the tables.
  @return EFI_INVALID_PARAMETER     An invalid parameter was supplied.

**/
EFI_STATUS
EFIAPI
ScmCmdSyncVarTables (
  IN      UINT32                             TableId
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_SYNC_VAR_TABLES *Request = 
    (TZ_SVC_REQ_VAR_SYNC_VAR_TABLES *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_SYNC_VAR_TABLES *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    Request->CommandId = SERVICE_UEFI_VAR_SYNC_VAR_TABLES;
    Request->TableId = TableId;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_SYNC_VAR_TABLES);
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_SYNC_VAR_TABLES *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_SYNC_VAR_TABLES);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**
  Copies UefiSecApp Log Buffer into input buffer if size is valid. 
   
  @param DumpLogBuffer           Buffer pointer.
  @param DumpLogBufferSize       Pointer to size of buffer in bytes. 
                                 On input, size of input buffer.
                                 On output, size of data copied or required
                                 if input DumpLogBufferSize is too small.

  @retval EFI_SUCCESS            Log buffer successfully copied. 
  @retval EFI_INVALID_PARAMETER  Invalid parameter supplied. 
  @retval EFI_BUFFER_TOO_SMALL   To small of a buffer supplied.  

**/
EFI_STATUS
EFIAPI
ScmCmdDumpLogBuffer (
  IN OUT  UINT8                     *DumpLogBuffer,
  IN OUT  UINT32                    *DumpLogBufferSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_DUMP_LOG_BUFFER *Request = 
    (TZ_SVC_REQ_VAR_DUMP_LOG_BUFFER *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_DUMP_LOG_BUFFER *Response = NULL;
  UINT8* ParamLocation = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    if ((DumpLogBuffer == NULL) || (DumpLogBufferSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Request->CommandId = SERVICE_UEFI_VAR_DUMP_LOG_BUFFER;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_DUMP_LOG_BUFFER);
    Request->DumpLogBufferSize = *DumpLogBufferSize;
    RequestLength = Request->Length;

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_DUMP_LOG_BUFFER *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_DUMP_LOG_BUFFER) + *DumpLogBufferSize;

    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

    if ((Response->DumpLogBufferSize > 0) && (Status == EFI_SUCCESS))
    {
      if (Response->DumpLogBufferSize > *DumpLogBufferSize)
      {
        *DumpLogBufferSize = Response->DumpLogBufferSize;
        Status = EFI_BUFFER_TOO_SMALL;
        break;
      }
      ParamLocation = (UINT8*)((UINT8*)Response + Response->DumpLogBufferOffset);
      CopyMem(DumpLogBuffer, ParamLocation, Response->DumpLogBufferSize);
    }
    *DumpLogBufferSize = Response->DumpLogBufferSize;

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}


/**

  Trigger SCM call to UEFI SECAPP to delete secure boot variables.

  @return EFI_SUCCESS               Successfully notified. 
  @return EFI_INVALID_PARAMETER     Invalid parameter passed.

**/
EFI_STATUS
EFIAPI
ScmCmdDelSecBootVars (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS *Request = 
    (TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    Request->CommandId = SERVICE_UEFI_VAR_DEL_SECBOOT_VARS;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_DEL_SECBOOT_VARS);
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_DEL_SECBOOT_VARS);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }

    if(NULL  == QcomScmProtocol)
    {
      DEBUG(( EFI_D_ERROR, "QcomScmProtocol is NULL \r\n"));
      Status = EFI_INVALID_PARAMETER;
      break;      
    }    
  
    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}

/**

  Send Mile stone call;After this Delete secboot SCM call can not be sent

  @return EFI_SUCCESS               Successfully notified. 
  @return EFI_INVALID_PARAMETER     Invalid parameter passed.

**/
EFI_STATUS
EFIAPI
ScmCmdSendDisableDelSecBootVarsMSCall (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TZ_SVC_REQ_VAR_SECBOOT_VARS_MILESTONE_CALL *Request = 
    (TZ_SVC_REQ_VAR_SECBOOT_VARS_MILESTONE_CALL *) ScmCmdParams.ScmCmdBuffer;
  TZ_SVC_RSP_VAR_SECBOOT_VARS_MILESTONE_CALL *Response = NULL;
  UINT32 RequestLength;
  UINT32 ResponseOffset;
  UINT32 ResponseLength;
  EFI_TPL PrevTPL;
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

  do
  {
    Request->CommandId = SERVICE_UEFI_VAR_DIS_DEL_SECBOOT_VARS_MS_CALL;
    Request->Length = sizeof(TZ_SVC_REQ_VAR_SECBOOT_VARS_MILESTONE_CALL);
    RequestLength = Request->Length;

    /* Calculate location of response */
    ResponseOffset = ALIGN_VALUE(Request->Length, 32);
    RequestLength = ResponseOffset;
    Response = (TZ_SVC_RSP_VAR_SECBOOT_VARS_MILESTONE_CALL *)((UINT8*)Request + ResponseOffset);
    ResponseLength = sizeof(TZ_SVC_RSP_VAR_SECBOOT_VARS_MILESTONE_CALL);

    /* Validate the size of the request */
    if(Request->Length > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
	
    /* Validate the size of response + request */
    if((ResponseOffset + ResponseLength) > ScmCmdInitParams.ScmCmdBufferSize)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;      
    }
  
    if(NULL  == QcomScmProtocol)
    {
      DEBUG(( EFI_D_ERROR, "QcomScmProtocol is NULL \r\n"));
      Status = EFI_INVALID_PARAMETER;
      break;      
    }    

    /* Make the SCM call - pass in the relevant pointer to the actual request and
       response pointers */
    Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                             APP_SEND_DATA_CMD,
                                             &ScmCmdParams.ScmAppId,
                                             Request,
                                             RequestLength,
                                             Response,
                                             ResponseLength);
    if (EFI_ERROR (Status)) 
    {
       DEBUG(( EFI_D_ERROR, "ScmSlowsyscall() failed, Status = (0x%x)\r\n", Status));
       break;
    }

    /* Copy output parameters */
    Status = RETURN_UINTN_EFI_STATUS(Response->Status);

  } while(0);

  gBS->RestoreTPL(PrevTPL);
  return Status;
}