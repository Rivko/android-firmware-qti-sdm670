/** @file FwFileIdentifiers.c
   
  File name/ File Guid lookup for FwCommonLib.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/08/08     mic         Added maximum update attempts support
 2012/08/08     rsb         Clean up FilePaths to remove unused entries
 2012/06/25     mic         Added OPM keys update and provisioning support
 2012/06/21     rs          Adding support for FPDT ACPI table
 2012/06/12     rs          Added support for BreakChangeNumber
 2012/05/07     rs          Added support for decryption of Boot Firmware
 2012/03/10     rs          Added SSD Keystore file path
 2011/10/12     jthompso    Initial version
 2012/01/25     jd          Added file path for FactoryTool commit and result files

=============================================================================*/
#include <Uefi.h>
#include "FwHandle_File.h"


/* Caution: The array FilePaths must be one-to-one matching with the enumeration list QCOM_FW_UPDATE_FW_FILE */
const QCOM_FW_UPDATE_FILE_IDENTIFIER FilePaths[QCOM_FW_UPDATE_FILE_MAX] ={
 
  {QCOM_FW_UPDATE_FILE_LEGACY_UPDATE_COMMIT_FLAG_FILE,QCOM_FW_UPDATE_FILE_FLAG_LEGACY_UPDATE_COMMIT_PATH, QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_FACTORY_PROVISIONED_FLAG_FILE, QCOM_FW_UPDATE_FILE_FLAG_FACTORY_PROVISIONED_PATH,  QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_FACTORY_FLAG_FILE,             QCOM_FW_UPDATE_FILE_FLAG_FACTORY_PATH,              QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE,             QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE_PATH,         QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE,           QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE_PATH,       QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_BP_GPT_FILE,                   QCOM_FW_UPDATE_FILE_BP_GPT_FILE_PATH,               QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_GPP_GPT_FILE,                  QCOM_FW_UPDATE_FILE_GPP_GPT_FILE_PATH,              QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_PAYLOAD_APP_FILE,                   QCOM_FW_UPDATE_PAYLOAD_APP_FILE_PATH,               QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_OA3_MSDM_ORIG_FILE,                 QCOM_FW_UPDATE_OA3_MSDM_ORIG_FILE_PATH,             QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE,                 QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE_PATH,             QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE,                  QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE_PATH,              QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_DPP_OEM_DIR_FILE,                   QCOM_FW_UPDATE_DPP_OEM_DIR_FILE_PATH,               QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_VERSION_FILE,                       NULL,                                               QCOM_FW_UPDATE_FILE_VERSION_METADATA_IN_PAYLOAD_GUID},
  {QCOM_FW_UPDATE_SSD_KEYSTORE_FILE,                  QCOM_FW_UPDATE_SSD_KEYSTORE_FILE_PATH,              QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE,           QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE_PATH,       QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_OPM_PUBKEY_FILE,                    QCOM_FW_UPDATE_OPM_PUB_KEY_FILE_PATH,               QCOM_FW_UPDATE_OPM_PUB_KEY_IN_PAYLOAD_GUID},
  {QCOM_FW_UPDATE_OPM_PUBKEY_BCK_FILE,                QCOM_FW_UPDATE_OPM_PUB_KEY_BCK_FILE_PATH,           QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIVKEY_FILE,         QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIV_KEY_FILE_PATH,    QCOM_FW_UPDATE_OPM_PRIV_KEY_PAYLOAD_GUID},
  {QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIVKEY_BCK_FILE,     QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIV_KEY_BCK_FILE_PATH,QCOM_FW_UPDATE_NULL_GUID},
  {QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE,            QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE_PATH,        QCOM_FW_UPDATE_NULL_GUID}
    
};

/**
 * Returns pointer to a 
 *  filename string.
 *  
 *  
 * @param Name - File enum
 * @param NameString - Output - Pointer to namestring.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileName(
    IN QCOM_FW_UPDATE_FW_FILE   Name,
    OUT UINT16                  **NameString

    )
{
    if(Name < QCOM_FW_UPDATE_FILE_MAX)
    {
        *NameString = FilePaths[Name].FileName;
        return EFI_SUCCESS;
    }

    return EFI_INVALID_PARAMETER;
}

/**
 * Returns pointer to a 
 *  file guid.
 *  
 *  
 * @param Name - File enum
 * @param NameString - Output - Pointer to FileGuid.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileGuid(
    IN QCOM_FW_UPDATE_FW_FILE   Name,
    OUT const EFI_GUID          **FileGuid

    )
{
    if(Name < QCOM_FW_UPDATE_FILE_MAX)
    {
        *FileGuid = &FilePaths[Name].FileGuid;
        return EFI_SUCCESS;
    }

    return EFI_INVALID_PARAMETER;
}
