/** @file Variable.c
   
  Implementation for Qualcomm UEFI Variable storage.

  Copyright (c) 2011-2015, 2017 Qualcomm Technologies, Inc.  All 
  Rights Reserved. Qualcomm Technologies Proprietary and 
  Confidential. 
  Portions copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
 
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
 06/22/17   bd      SCM call for deleting secure boot variables
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 05/16/14   tonyo   Support for 32/64 bit environment  
 07/23/13   yg      Reduce logging
 04/03/13   niting  Added offline crash dump support
 03/14/13   niting  Added dump of UefiSecApp log buffer
 03/04/13   niting  Added more logging.
 01/10/13   niting  Updated to use new SCM interface.
 07/17/12   niting  Fix firmware desync if called multiple times
 07/16/12   niting  Assert if variable tables are not initialized
 05/30/12   jd      Removed DPP protocol installation.
 05/08/12   yg      Remove Variable tables sync
 05/01/12   vishalo Reset VariableSyncEnabled flag
 04/20/12   niting  Removed runtime UEFI variable support through direct calls
                    to UEFI
 04/02/12   jd      Added DPP protocol installation.
 03/16/12   jz      Pass up variable for AppId
 03/06/12   niting  Changed disable variable sync to firmware desync.
 03/06/12   jz      Renamed PcdTZSharedParamsBaseOffset to PcdTZScmCmdBufferBaseOffset.
 03/01/12   niting  Moved runtime memory mapping to Sec.
 02/23/12   niting  Changes to support UEFI runtime calls into TZ.
 01/20/12   niting  Added event guid to disable variable sync.
 12/20/11   niting  Changed logging of Variable APIs.
 09/27/11   niting  Search through secure tables first.
 09/15/11   niting  Added support for QueryVariableInfo and
                    GetNextVariableName.
 09/13/11   niting  Allow for nonsecure signature databases.
 07/29/11   niting  Initial revision.

=============================================================================*/

#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/PcdLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>

#include <Protocol/Variable.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFIVariableServices.h>
#include <Protocol/EFIDebug.h>
#include <Protocol/EFIScm.h>

#include <Guid/EventGroup.h>
#include <Guid/ImageAuthentication.h>

#include "ScmCmdVar.h"

#define USE_TZ_SCM_CALLS

/* Round off to 4KB pages */
#define ROUND_TO_PAGE(x) (x & 0xfffff000)

STATIC EFI_HANDLE  mHandle = NULL;
STATIC EFI_EVENT   mExitBootServicesEvent = NULL;

EFI_STATUS EFIAPI RuntimeFlushVariableNV ( 
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  );
EFI_STATUS EFIAPI DeleteSecBootVars ( 
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  );
EFI_STATUS EFIAPI SendDisableDelSecBootVarsMilestoneCall ( 
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  );
  
EFI_STATUS EFIAPI GetDebugLog ( 
  IN     EFI_QCOMDEBUG_PROTOCOL           *This,
  IN OUT UINT8                            *DebugLogBuffer,
  IN OUT UINT32                           *DebugLogBufferSize,
  IN     UINT32                           LogType
);

static EFI_VARIABLESERVICES_PROTOCOL gVariableServicesProtocol = 
{
  EFI_VARIABLESERVICES_PROTOCOL_REVISION,
  RuntimeFlushVariableNV,
  DeleteSecBootVars,
  SendDisableDelSecBootVarsMilestoneCall
};

static EFI_QCOMDEBUG_PROTOCOL gQcomDebugProtocol = 
{
  EFI_QCOM_DEBUG_PROTOCOL_REVISION,
  GetDebugLog
};

STATIC EFI_EVENT FirmwareDesyncEvent = NULL;
STATIC BOOLEAN VariableSyncEnabled = TRUE;

#define EFI_EVENT_GROUP_FIRMWARE_DESYNC \
  { 0x24FA5E72, 0x1A82, 0x49A2, { 0x97, 0x0B, 0x32, 0x30, 0x37, 0x26, 0x62, 0xA5 } }

extern EFI_GUID gEfiEventFirmwareDesyncGuid;

typedef enum {
  TABLE_ID_RESERVED    = 0,
  TABLE_ID_BS_NV_S     = 1,
  TABLE_ID_BS_NV_NS    = 2,
  TABLE_ID_RT_NV_S     = 3,
  TABLE_ID_RT_NV_NS    = 4,

  TABLE_ID_COUNT       = 5,

  /* Add any extra tables here */

  TABLE_TYPE_MAX         = 6      // Should be the last entry in this enum
}TableType;

EFI_STATUS
EFIAPI
SyncAllTables ( VOID )
{
  return ScmCmdSyncVarTables(TABLE_ID_RESERVED);
}


/** 
   
  Disables the syncing of variable storage tables.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
DisableVariableSync (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  /* Sync all tables before disabling sync */
  if (VariableSyncEnabled)
  {
    SyncAllTables();
  }

  VariableSyncEnabled = FALSE;
}

/**

  Returns the value of a variable.

  @param VariableName               A Null-terminated string that is the name 
                                    of the vendor’s variable.
  @param VendorGuid                 A unique identifier for the vendor.
  @param Attributes                 If not NULL, a pointer to the memory 
                                    location to return the attributes bitmask
                                    for the variable.
  @param DataSize                   On input, the size in bytes of the return 
                                    Data buffer. On output the size in bytes of
                                    data returned in Data.
  @param Data                       The buffer to return the contents of the 
                                    variable.
                      
  @return EFI_SUCCESS               Found the specified variable successfully.
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_NOT_FOUND             Variable was not found.
  @return EFI_BUFFER_TO_SMALL       DataSize is too small for the result.
  @return EFI_DEVICE_ERROR          The variable could not be retrieved due to 
                                    a hardware error.
   
**/
EFI_STATUS
EFIAPI
RuntimeGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  do
  {
    /* UEFI Variable services through UEFI API unsupported at runtime
       All runtime access to UEFI variable services is through
       HLOS SCM driver */
    if (EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    /* Validate inputs */
    if ((VariableName == NULL) || (VendorGuid == NULL) || 
        (DataSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
  
    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "GetVariable :\n  Name:%s\n"
             "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
             "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
             "  Buffer Size:%d\n",
             VariableName, 
             VendorGuid->Data1, VendorGuid->Data2, VendorGuid->Data3,
             VendorGuid->Data4[0], VendorGuid->Data4[1], VendorGuid->Data4[2], 
             VendorGuid->Data4[3], VendorGuid->Data4[4], VendorGuid->Data4[5],
             VendorGuid->Data4[6], VendorGuid->Data4[7],
             *DataSize));
    }

    if ((*DataSize != 0) && (Data == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    
#ifdef USE_TZ_SCM_CALLS
    Status = ScmCmdGetVariable(VariableName,
                               VendorGuid,
                               Attributes,
                               DataSize,
                               Data);
#else
    Status = TzGetVariable(VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data);
#endif
  
    if (!EfiAtRuntime())
    {
      if (DataSize)
      {
        DEBUG((EFI_D_VARIABLE, "  Data Size:%d\n", *DataSize));
      }
    }
  } while (0);

  return Status;
}

/**

  Enumerates the current variable names.

  @param VariableNameSize           The size in bytes of the VariableName 
                                    buffer.
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
  @return EFI_BUFFER_TO_SMALL       The VariableNameSize is too small for the 
                                    result. VariableNameSize has been updated
                                    with the size needed to complete the
                                    request.
  @return EFI_INVALID_PARAMETER     Invalid parameter.
  @return EFI_DEVICE_ERROR          The variable name could not be retrieved 
                                    due to a hardware error.
   
**/
EFI_STATUS
EFIAPI
RuntimeGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  do
  {
    /* UEFI Variable services through UEFI API unsupported at runtime
       All runtime access to UEFI variable services is through
       HLOS SCM driver */
    if (EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    if ((VariableNameSize == NULL) || 
        (VariableName == NULL) || 
        (VendorGuid == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "GetNextVariable :\n  Name:%s\n"
             "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
             "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
             "  Name Size:%d\n",
             VariableName, 
             VendorGuid->Data1, VendorGuid->Data2, VendorGuid->Data3,
             VendorGuid->Data4[0], VendorGuid->Data4[1], VendorGuid->Data4[2], 
             VendorGuid->Data4[3], VendorGuid->Data4[4], VendorGuid->Data4[5],
             VendorGuid->Data4[6], VendorGuid->Data4[7],
             *VariableNameSize));
    }

#ifdef USE_TZ_SCM_CALLS
    Status = ScmCmdGetNextVariableName(VariableNameSize,
                                       VariableName,
                                       VendorGuid);
#else
    Status = TzGetNextVariableName(VariableNameSize,
                                   VariableName,
                                   VendorGuid);
#endif

    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "  NextVariable :\n  Name:%s\n"
             "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
             "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
             "  Name Size:%d\n",
             VariableName, 
             VendorGuid->Data1, VendorGuid->Data2, VendorGuid->Data3,
             VendorGuid->Data4[0], VendorGuid->Data4[1], VendorGuid->Data4[2], 
             VendorGuid->Data4[3], VendorGuid->Data4[4], VendorGuid->Data4[5],
             VendorGuid->Data4[6], VendorGuid->Data4[7],
             *VariableNameSize));
    }

  } while (0);

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
RuntimeSetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  IN      UINT32            Attributes,
  IN      UINTN             DataSize,
  IN      VOID              *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UINT32 TableId = 0;
  UINT32 SyncSize = 0;

  CHAR16 *NullChar = L"\0";

  do
  {
    /* UEFI Variable services through UEFI API unsupported at runtime
       All runtime access to UEFI variable services is through
       HLOS SCM driver */
    if (EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    if ((VariableName == NULL) || (VendorGuid == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (*VariableName == *NullChar)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if ((DataSize != 0) && (Data == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "SetVariable:\n  Name:%s\n"
             "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
             "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
             "  Attributes:%d\n  Size:%d\n",
             VariableName, 
             VendorGuid->Data1, VendorGuid->Data2, VendorGuid->Data3,
             VendorGuid->Data4[0], VendorGuid->Data4[1], VendorGuid->Data4[2], 
             VendorGuid->Data4[3], VendorGuid->Data4[4], VendorGuid->Data4[5],
             VendorGuid->Data4[6], VendorGuid->Data4[7],
             Attributes, DataSize));
    }

#ifdef USE_TZ_SCM_CALLS
    Status = ScmCmdSetVariable(VariableName,
                               VendorGuid,
                               Attributes,
                               DataSize,
                               Data,
                               &TableId,
                               &SyncSize);
#else
    Status = TzSetVariable(VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data);
#endif

  } while (0);

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
RuntimeQueryVariableInfo (
  IN      UINT32            Attributes,
  OUT     UINT64            *MaximumVariableStorageSize,
  OUT     UINT64            *RemainingVariableStorageSize,
  OUT     UINT64            *MaximumVariableSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  do
  {
    /* UEFI Variable services through UEFI API unsupported at runtime
       All runtime access to UEFI variable services is through
       HLOS SCM driver */
    if (EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    if ((MaximumVariableStorageSize == NULL) || 
        (RemainingVariableStorageSize == NULL) ||
        (MaximumVariableSize == NULL))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
  
    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "QueryVariableInfo:\n  "));
    }

#ifdef USE_TZ_SCM_CALLS
    Status = ScmCmdQueryVariableInfo(Attributes,
                                     MaximumVariableStorageSize,
                                     RemainingVariableStorageSize,
                                     MaximumVariableSize);
#else
    Status = TzQueryVariableInfo(Attributes,
                                 MaximumVariableStorageSize,
                                 RemainingVariableStorageSize,
                                 MaximumVariableSize);
#endif

    if (!EfiAtRuntime())
    {
      DEBUG((EFI_D_VARIABLE, "  MaximumVariableStorageSize: 0x%llx\n"
                         "  RemainingVariableStorageSize: 0x%llx\n"
                         "  MaximumVariableSize: 0x%llx\n",
                         *MaximumVariableStorageSize,
                         *RemainingVariableStorageSize,
                         *MaximumVariableSize));
    }
  } while (0);

  return Status;
}

/**
  Exit Boot Services notification call back. Sends ExitBootServices Notification
      to variable services in TZ. 

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
ExitBootServicesCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  ScmCmdExitBootServices();
  VariableSyncEnabled = FALSE;
}


/**
  
  Delete Seure boot variables - PK/KEK/DB/DBX
  
  @param[in]   This                     A pointer to the 
                                        EFI_VARIABLESERVICES_PROTOCOL instance.

  @return EFI_SUCCESS                   Function completed successfully.
  @return EFI_INVALID_PARAMETER         An invalid parameter was passed.
  @return EFI_UNSUPPORTED               Function is not supported in this 
                                        context.

**/
EFI_STATUS
EFIAPI
DeleteSecBootVars (
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  )
{
  return ScmCmdDelSecBootVars();
}


/**
  
  Send Mile stone call;After this Delete secboot SCM call can not be sent
  
  @param[in]   This                     A pointer to the 
                                        EFI_VARIABLESERVICES_PROTOCOL instance.

  @return EFI_SUCCESS                   Function completed successfully.
  @return EFI_INVALID_PARAMETER         An invalid parameter was passed.
  @return EFI_UNSUPPORTED               Function is not supported in this 
                                        context.

**/
EFI_STATUS
EFIAPI
SendDisableDelSecBootVarsMilestoneCall (
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  )
{
  return ScmCmdSendDisableDelSecBootVarsMSCall();
}


/**

  Flushes all non-volatile storage tables to non-volatile memory.
  
  @param[in]   This                     A pointer to the 
                                        EFI_VARIABLESERVICES_PROTOCOL instance.

  @return EFI_SUCCESS                   Function completed successfully.
  @return EFI_INVALID_PARAMETER         An invalid parameter was passed.
  @return EFI_UNSUPPORTED               Function is not supported in this 
                                        context.
   
**/
EFI_STATUS
EFIAPI
RuntimeFlushVariableNV (
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (VariableSyncEnabled == TRUE)
  {
    Status = SyncAllTables();
    if (Status != EFI_SUCCESS)
      DEBUG((EFI_D_ERROR, "Failed to sync all UEFI tables, Status = 0x%08x\n", Status));
    else
      DEBUG((EFI_D_WARN, "Successfully synced all UEFI tables\n"));
  }
  else
  {
    DEBUG((EFI_D_WARN, "Table syncing disabled, will not sync UEFI tables\n"));
  }

  return Status;
}

/** 
  Copies the Debug Log Buffer to input buffer
  
  @param[in]  This                    Pointer to the EFI_QCOMDEBUG_PROTOCOL instance.
  @param[in]  DebugLogBuffer          Buffer pointer.
  @param[in]  DebugLogBufferSize      Pointer to size of buffer in bytes. 
                                      On input, size of input buffer.
                                      On output, size of data copied or required
                                      if input DumpLogBufferSize is too small.
  @param[in]  LogType                 Identifies which log to retrieve.

  @return
  EFI_SUCCESS           -- Log buffer successfully copied. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_BUFFER_TOO_SMALL  -- To small of a buffer supplied.
*/
EFI_STATUS EFIAPI GetDebugLog ( 
  IN     EFI_QCOMDEBUG_PROTOCOL           *This,
  IN OUT UINT8                            *DebugLogBuffer,
  IN OUT UINT32                           *DebugLogBufferSize,
  IN     UINT32                           LogType
)
{
  if (EfiAtRuntime())
  {
    return EFI_UNSUPPORTED;
  }

  if (LogType == EFI_DEBUG_LOG_UEFISECAPP)
    return ScmCmdDumpLogBuffer(DebugLogBuffer, DebugLogBufferSize);
  else 
    return EFI_UNSUPPORTED;
}

/**
  TzeLoaderDxe notification event handler.

  Register the tables with TZ.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context. 
   
**/
STATIC
VOID
EFIAPI
InitializeVariableTables ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  VARIABLE_INIT_PARAMS VarInitParams;
  UINT8 VarTableInit = 0;
  UINTN VarTableInitSize = sizeof(VarTableInit);
  UINT32 UefiSecAppId = 0;
          
  Status = ScmCmdInitUefiSecApp(&UefiSecAppId);
  ASSERT_EFI_ERROR(Status);

  /* Clear init params for now - no longer used; TODO: remove parameters */
  SetMem(&VarInitParams, sizeof(VarInitParams), 0x00);

  /* Initialize Variable Services Tables */
#ifdef USE_TZ_SCM_CALLS
  Status = ScmCmdVariableInitialize((UINT8*)&VarInitParams, 
                                    sizeof(VARIABLE_INIT_PARAMS));
#else
  Status = TzVariableInitialize(&VarInitParams, 
                                sizeof(VARIABLE_INIT_PARAMS));
#endif

  ASSERT_EFI_ERROR(Status);

  /* Check if variable tables have been initialized or not */
  Status = RuntimeGetVariable(L"VarTableInit",
                              &gQcomTokenSpaceGuid,
                              NULL,
                              &VarTableInitSize, 
                              &VarTableInit);

  /* Ignore this check if not found */
  if (Status != EFI_NOT_FOUND)
  {
    /* If tables are initialized */
    if ((Status == EFI_SUCCESS) && (VarTableInit == 1))
    {
      DEBUG((EFI_D_INFO, "Variable Tables Successfully Initialized\n"));
    }
    else
    {
      DEBUG((EFI_D_ERROR, "Variable Tables Not Successfully Initialized\n"));
      ASSERT(0);
    }
  }

  /* Set variable for AppId */
  Status = RuntimeSetVariable(L"UefiSecAppId",
                              &gQcomTokenSpaceGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                              sizeof(UefiSecAppId), 
                              &UefiSecAppId);

  ASSERT_EFI_ERROR(Status);

  return;
}

/**
  Variable Service entry point. The driver initializes the volatile variable 
  store and also registers a callback for when a block I/O device gets 
  installed. At this point the callback will initialize the non-volatile 
  variable store by first reading/validating in any existing storage tables. 
  This driver also updates the EFI System Table with the necessary pointers to 
  the GetVariable and SetVariable. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       Variable service successfully initialized.

**/
EFI_STATUS
EFIAPI
VariableDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  /* Load and initialize UEFI Variable tables */
  InitializeVariableTables();
  
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  ExitBootServicesCallback,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  /* Create event to disable sync if required */
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  DisableVariableSync,
                  NULL,
                  &gEfiEventFirmwareDesyncGuid,
                  &FirmwareDesyncEvent
                  );
  ASSERT_EFI_ERROR (Status);

  /* Install the Variable Runtime Architectural Protocols */
  SystemTable->RuntimeServices->GetVariable         = RuntimeGetVariable;
  SystemTable->RuntimeServices->GetNextVariableName = RuntimeGetNextVariableName;
  SystemTable->RuntimeServices->SetVariable         = RuntimeSetVariable;
  SystemTable->RuntimeServices->QueryVariableInfo   = RuntimeQueryVariableInfo;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiVariableArchProtocolGuid,
                  NULL,
                  &gEfiVariableWriteArchProtocolGuid,
                  NULL,
                  &gEfiVariableServicesProtocolGuid,
                  &gVariableServicesProtocol,
                  &gEfiQcomDebugProtocolGuid,
                  &gQcomDebugProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
