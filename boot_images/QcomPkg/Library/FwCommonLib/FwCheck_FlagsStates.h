/** @file FwFlagsAndStates.h
   
  Header for FwFlagsAndStates.c, part of FwUpdateLib.

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
 2012/05/10     rs          Added support for decryption of bootloaders
 2011/10/12     jthompso    Initial version

=============================================================================*/

#ifndef __QCOM_FW_CHECK_FLAGSSTATES_H__
#define __QCOM_FW_CHECK_FLAGSSTATES_H__


#include "Uefi.h"
#include "esp_dpp.h"
#include "FwHandle_Partition.h"
#include "FwHandle_File.h"


#define QCOM_FW_UPDATE_FLAG_STATE_SIZE 1
#define QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_SIZE 4


typedef enum
{
  QCOM_FW_UPDATE_LEGACY_UPDATE_COMMIT_FLAG = 0,
  QCOM_FW_UPDATE_FLAG_FACTORY_PROVISIONED_FLAG,
  QCOM_FW_UPDATE_FLAG_FACTORY_FLAG,
  QCOM_FW_UPDATE_FLAG_MAX
}QCOM_FW_UPDATE_FLAG;


typedef enum
{
  QCOM_FW_UPDATE_FLAG_VALUE_UNUSED1 = 0,
  QCOM_FW_UPDATE_FLAG_VALUE_UNUSED2,
  QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_SET,
  QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_NOT_SET,
  QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_SET_AND_MIGRATED,
  QCOM_FW_UPDATE_FLAG_VALUE_MAX
}QCOM_FW_UPDATE_FACTORY_PROVISIONED_FLAG_VALUE;

typedef enum {
  FW_STATE_NOT_IN_PROGRESS,
  FW_STATE_BACKUP_DONE,
  FW_STATE_UPDATE_SUCCESS,
  FW_STATE_UPDATE_FAIL,   
  FW_STATE_ROLLBACK_DONE

} QCOM_FW_UPDATE_STATE_VARIABLES;

typedef enum UPDATE_STATUS{
  UPDATE_SUCCESS,
  UPDATE_FAILED
}UPDATE_STATUS;

typedef enum {
  QCOM_FW_UPDATE_STATE_FILE_UPDATE_STATE,
  QCOM_FW_UPDATE_STATE_FILE_FACTORY_RESULT,
  QCOM_FW_UPDATE_STATE_FILE_ATTEMPT_COUNT
}QCOM_FW_UPDATE_STATE_FILE;


typedef struct{
  QCOM_FW_UPDATE_FLAG         Flag;
  QCOM_FW_UPDATE_FW_FILE      File;
  QCOM_FW_UPDATE_FW_PARTITION Part;
}QCOM_FLAG_FILE_PART;

typedef struct{
  QCOM_FW_UPDATE_STATE_FILE   File;
  QCOM_FW_UPDATE_FW_FILE      FileName;
  QCOM_FW_UPDATE_FW_PARTITION Part;
}QCOM_STATE_FILE_PART;

/**
 * 
 * 
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
  );

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
  );

/**
 * Read the update attempt count from the file in FS
 *
 * @param  cnt      -  Output attempt count
 *
 * @return EFI_STATUS 
 *
 */

EFI_STATUS 
GetAttemptCount ( 
  OUT UINT8 *cnt
  );

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
  );



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
  );



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
  );

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
  );


/*
 *Function Name:UpdateReturnStateinESP(UPDATE_STATUS)
 *
 *Parameters: Update_status 
 *
 *Description: The update status file in the ESP should be updated with 
 *the Update_status passed as the parameter 
 *
 *Returns:EFI_STATUS
 *
 */

EFI_STATUS UpdateReturnState(
  IN UPDATE_STATUS State
  );


/**
 * Sets a flag used by FwUpdateLib.
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
  );


/**
 * Checks how a flag is set.
 * 
 * 
 * @param Flag - Which Flag to check.
 * @param FlagState - How the flag is set.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CheckFlag(        
  IN  QCOM_FW_UPDATE_FLAG         Flag,
  OUT VOID                        *FlagValue
  );

/**
 * This Function gets the state of secure boot
 * 
 * 
 * @param SecureBootEnabled - A caller allocated buffer to store 
 *  						  secure boot state.
 * @return EFI_STATUS 
 */

EFI_STATUS
GetSecureBootState(
  OUT BOOLEAN *SecureBootEnabled
  );

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
    );

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
  );

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
  );

#endif

