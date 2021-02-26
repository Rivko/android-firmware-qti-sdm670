/** @file FwFlagsAndStates.c
   
  Change Flags and States for FwUpdate.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/12/08     mic         Deprecated FwGetOsType API
 2012/08/03     mic         Added maximum update attempts support
 2012/06/12     rs          Added support for BreakChangeNumber
 2012/06/05     rs          Added GetDebugModeState(), GetSecureBootState() and FwGetOSType()
 2012/01/25     jd          Enhanced GetOrSetFlagState to support FactoryTool commit and result files
 2011/10/12     jthompso    Initial version
=============================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/QcomLib.h>
#include "FwCheck_FlagsStates.h"

static EFI_GUID DebugModeVariableGuid = {0x77fa9abd, 0x0359, 0x4d32, { 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b}};

static QCOM_FLAG_FILE_PART FlagFilePart[] = 
{
  {QCOM_FW_UPDATE_LEGACY_UPDATE_COMMIT_FLAG,           QCOM_FW_UPDATE_FILE_LEGACY_UPDATE_COMMIT_FLAG_FILE,   QCOM_FW_UPDATE_PARTITION_USER_ESP},
  {QCOM_FW_UPDATE_FLAG_FACTORY_PROVISIONED_FLAG,       QCOM_FW_UPDATE_FILE_FACTORY_PROVISIONED_FLAG_FILE,    QCOM_FW_UPDATE_PARTITION_GPP_FAT16},
  {QCOM_FW_UPDATE_FLAG_FACTORY_FLAG,                   QCOM_FW_UPDATE_FILE_FACTORY_FLAG_FILE,                QCOM_FW_UPDATE_PARTITION_USER_ESP}  
};

static QCOM_STATE_FILE_PART StateFilePart[] =
{
  {QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE,            QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE,         QCOM_FW_UPDATE_PARTITION_GPP_FAT16},
  {QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT,          QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE,       QCOM_FW_UPDATE_PARTITION_USER_ESP},
  {QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT,           QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE,        QCOM_FW_UPDATE_PARTITION_GPP_FAT16}
};


/**
 * 
 * @param File - File to Read/Modify
 * @param Value - Value to set/Value Returned from read. 
 * @param Set - If TRUE, Get, if FALSE Set
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
GetOrSetFlagState(
  IN QCOM_FW_UPDATE_FW_FILE       File,
  IN QCOM_FW_UPDATE_FW_PARTITION  Part,
  IN OUT VOID                     *Value,
  BOOLEAN                         Get
  )
{
  EFI_STATUS  Status;
  EFI_STATUS  (*FatOperation)() = NULL;
  EFI_HANDLE  PartHandle;
  UINTN       FileSize = QCOM_FW_UPDATE_FLAG_STATE_SIZE;
  UINTN       ExpectedFileSize = QCOM_FW_UPDATE_FLAG_STATE_SIZE;
  VOID        *FileBuffer;
    
  Status = FwUpdateGetHandleForPartition(Part, &PartHandle);

  ERR_HANDLE(Status);

  if(Get)
  {
    FatOperation = FwUpdateReadFileFromFat;
  }
  else
  {   
  //(UINT8*) FileBuffer = Value;
    FileBuffer = Value;
    FatOperation = FwUpdateWriteFileToFat;
  }

  // Check the expected file size
  switch(File)
  {
    case QCOM_FW_UPDATE_FILE_FACTORY_FLAG_FILE:
      ExpectedFileSize = sizeof(PROVISIONING_COMMIT_FILE_TYPE);
    break;
    			
    case QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE:
      ExpectedFileSize = sizeof(UINT8);
    break;

    case QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE:
      ExpectedFileSize = sizeof(PROVISIONING_RESULT_FILE_TYPE);
    break;

	case QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE:
	  ExpectedFileSize = QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_SIZE;
	break;

	default:
      ExpectedFileSize = QCOM_FW_UPDATE_FLAG_STATE_SIZE;
    break;
  }
  FileSize = ExpectedFileSize;

  Status = FatOperation(PartHandle,
                        (VOID**)&FileBuffer,
                        File,
                        &FileSize);

  ERR_HANDLE(Status);

  // Check the return file size only if it is a Set operation.
  // Get operation may be legal to have a size difference to
  // accomodate older or newer structure versions.
 
  // Check the file size to see for write failure
  if((Get == FALSE) && (FileSize != ExpectedFileSize))
  {
    return RETURN_LOAD_ERROR;
  }
  if(Get)
  {
    CopyMem(Value, FileBuffer, FileSize);
    Status = gBS->FreePool(FileBuffer);
  }

  return Status;
}


/**
 * Read the state from the file in FS
 *
 * @param  State      -  Output STATE_VARIABLE
 *
 * @return EFI_STATUS 
 *
 */

EFI_STATUS 
GetState ( 
  OUT QCOM_FW_UPDATE_STATE_VARIABLES *State
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE].Part;
  return GetOrSetFlagState(FileName,Part,(UINT8*)State,TRUE);
}

/**
 * Read the attempt count from the file in FS
 *
 * @param  cnt      -  Output attempt count
 *
 * @return EFI_STATUS 
 *
 */

EFI_STATUS 
GetAttemptCount ( 
  OUT UINT8 *cnt
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT].Part;
  return GetOrSetFlagState(FileName,Part,(UINT8*)cnt,TRUE);
}

/**
 * Sets a flag used by FwCommonLib.
 * 
 * 
 * @param Flag - Which flag to set
 * @param FlagValue - How the flag should be set. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS 
SetFlag(       
  IN QCOM_FW_UPDATE_FLAG          Flag,
  IN VOID                         *FlagValue
  )
{
  QCOM_FW_UPDATE_FW_FILE      File;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  if((Flag < QCOM_FW_UPDATE_FLAG_MAX) && (FlagValue != NULL))
  {
    File = FlagFilePart[Flag].File;
    Part = FlagFilePart[Flag].Part;

    return GetOrSetFlagState(File,Part,FlagValue,FALSE);
  }
  return EFI_INVALID_PARAMETER;
}


/**
 * Reads the factory result file 
 * 
 * @param Result - The result of the provisioning operation 
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS 
GetFactoryProvisioningResult ( 
  OUT PROVISIONING_RESULT_FILE_TYPE *Result
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT].Part;
  return GetOrSetFlagState(FileName, Part, (VOID*)Result, TRUE);
}



/**
 * Updates the factory provisioning result file in FS
 * 
 * @param Result      - Input factory provisioning result to be 
 *  				    written to FS
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS 
SetFactoryProvisioningResult(
  IN PROVISIONING_RESULT_FILE_TYPE *Result
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT].Part;
  return GetOrSetFlagState(FileName, Part, (VOID*)Result, FALSE);
}

/**
 * This function updates the state flag in FS. 
 * Make sure after the write a flush command is issued to clear 
 * the FS cache 
 * 
 * @param State       - State of the system that needs to be 
 *  				    updated in the FS
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS 
UpdateStateinFS(
  IN QCOM_FW_UPDATE_STATE_VARIABLES State
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE].Part;
  return GetOrSetFlagState(FileName,Part,(UINT8*)&State,FALSE);
}

/**
 * This function updates the attempt count in FS. 
 * Make sure after the write a flush command is issued to clear 
 * the FS cache 
 * 
 * @param AttemptCnt       - attempt count to update in the FS
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS 
UpdateAttemptCountInFS(
  IN UINT8 AttemptCnt
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT].FileName;    
  Part = StateFilePart[QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT].Part;
  return GetOrSetFlagState(FileName,Part,(UINT8*)&AttemptCnt,FALSE);
}

/**
 * Checks the value of a flag.
 * 
 * 
 * @param Flag      - Flag to check
 * @param FlagState - State the flag is set to
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CheckFlag(        
  IN  QCOM_FW_UPDATE_FLAG         Flag,
  OUT VOID                        *FlagValue
  )
{
  QCOM_FW_UPDATE_FW_FILE      File;
  QCOM_FW_UPDATE_FW_PARTITION Part;
    
  if((Flag < QCOM_FW_UPDATE_FLAG_MAX) && (FlagValue!=NULL))
  {   
    File = FlagFilePart[Flag].File;    
    Part = FlagFilePart[Flag].Part;

    return GetOrSetFlagState(File,Part,FlagValue,TRUE);
  }
  return EFI_INVALID_PARAMETER;

}


/**
 	Variable GUID:  77fa9abd-0359-4d32-bd60-28f4e78f784b 
 	Variable Name: "CurrentPolicy"
 
    If the variable exist, and its value size is just one byte
 	then it is a dummy policy and the system is in production
 	mode. If the variable exist and its size is larger, then the
 	system may either have a custom production policy or a debug
 	policy. The former is unlikely as MSFT do not intend to
 	create any custom production policies and so you will be
 	able to determine that the system is in debug mode.
 
 	@param DebugModeEnabled - Update the current Debug Mode
 							  state.
    @return EFI_STATUS 
 
*/
EFI_STATUS
GetDebugModeState(
    OUT BOOLEAN *DebugModeEnabled
    )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       DebugMode ;
    UINTN       VarSize;

    VarSize = sizeof(DebugMode);
    Status = gRT->GetVariable (L"CurrentPolicy", &DebugModeVariableGuid,
                               NULL, &VarSize, &DebugMode);

    *DebugModeEnabled = Status==EFI_BUFFER_TOO_SMALL?TRUE:FALSE;

    return EFI_SUCCESS;
}

/**
 * Check if secure boot is enabled or disabled.
 * 
 * @param SecureBootEnabled - A caller allocated buffer to store secure boot state.
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS
GetSecureBootState(
  OUT BOOLEAN *SecureBootEnabled
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       SecureBoot;
  UINTN       VarSize;

  VarSize = sizeof(SecureBoot);
  Status = gRT->GetVariable (L"SecureBoot", &gEfiGlobalVariableGuid,
                             NULL, &VarSize, &SecureBoot);

  if(EFI_ERROR(Status))
  {   
      DEBUG((EFI_D_ERROR,": gRT->GetVariable returned %r\n", Status));
      goto GetSecureBootStateExit;
  }   

  *SecureBootEnabled = SecureBoot == 0 ? FALSE : TRUE;

GetSecureBootStateExit:

  return Status;
}


/**
 * Read the BreakingChangeNumber from the file in FS
 *
 * @param BreakingChangeNumber  - The BreakingChangeNumber from 
 *  							  the FS
 *
 * @return EFI_STATUS 
 *
 */

EFI_STATUS 
GetBreakingChangeNumber ( 
  OUT UINT32 *BreakingChangeNumber
  )
{
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  FileName = QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE;    
  Part = QCOM_FW_UPDATE_PARTITION_GPP_FAT16;
  return GetOrSetFlagState(FileName,Part,BreakingChangeNumber,TRUE);
}

/**
 * Write the BreakingChangeNumber to the file in FS
 *
 * @param BreakingChangeNumber  - The BreakingChangeNumber to 
 *  							  write to the FS
 *
 * @return EFI_STATUS 
 *
 */

EFI_STATUS 
SetBreakingChangeNumber ( 
  IN UINT32 BreakingChangeNumber
  )
{

  QCOM_FW_UPDATE_FW_FILE      File;
  QCOM_FW_UPDATE_FW_PARTITION Part;

  File = QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE;    
  Part = QCOM_FW_UPDATE_PARTITION_GPP_FAT16;

  return GetOrSetFlagState(File,Part,&BreakingChangeNumber,FALSE);

}
