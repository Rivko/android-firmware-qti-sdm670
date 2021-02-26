/**
 * @file esp_dpp.h
 *
 * Public header for EFI System Partition and Device Provisioning for Windows on ARM.
 *
 * This utility library provides interfaces for accessing the EFI System Partition
 * and files stored in Device Provisioning area.
 *
 * !!!IMPORTANT!!!
 *
 * Currently this header is shared between UEFI OsAppsPkg and FactoryTool/UpdateTool.
 * Due to build restriction, two identical copies of esp_dpp.h are maintained at:
 *
 * //deploy/qcom/qct/platform/wpci/prod/woa/QCDK/main/latest/inc/esp_dpp.h
 * //deploy/qcom/qct/platform/wpci/prod/woa/UEFI/main/latest/edk2/OsAppsPkg/Include/esp_dpp.h
 *
 * MAKE SURE THESE TWO COPIES ARE ALWAYS IN-SYNC WITH EACH OTHER!!!
 *
 */

/*==============================================================================
     Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/13   rsb     Added support for HDCK 2.x key provisioning
10/23/12   mic     Added support for firmware update return code
06/04/12   mic     Added support for OPM public key and encrypted OPM private key provisioning
02/23/12   rsb     Added support for UEFI secure boot provisioning
01/25/12   jd      Made it compatible with UEFI build environment
12/02/11   rsb     Clean up API
09/20/11   rsb     Created initial library for ESP/DPP on WoA

===========================================================================*/

#ifndef ESP_DPP_H
#define ESP_DPP_H

/*===========================================================================
                                INCLUDE FILES
===========================================================================*/
/*=========================================================================== 
                           DEVICE UPDATE DEFINITIONS
===========================================================================*/
  
/*!
 * Enum defining the different return codes for the QCFirmwareUpdate.exe, 
 * Please note this flag is only 8 bits.
 */

typedef enum
{
   QCFW_UPDATE_COMMIT_SET                                     = 0x00,
   QCFW_UPDATE_SUCCESS                                        = 0x01,
   QCFW_UPDATE_INIT                                           = 0x02,
   QCFW_UPDATE_ERR_FLAG_GET_STAMACHINE_STATE                  = 0x03,

   /*State and flag related error*/
   QCFW_UPDATE_ERR_FLAG_STATE_EXCEEDED_UPDATE_ATTEMPT_MAX     = 0x20,  
 
   /*ESRT related error*/
   QCFW_UPDATE_ERR_ESRT_GET_ENTRY                             = 0x30,

   /*Capsule related error*/
   QCFW_UPDATE_ERR_CAPSULE_REGISTER_SYSTEM_CAPSULE            = 0x40,

   /*UEFI API related error*/
   QCFW_UPDATE_ERR_UEFI_API_PRINT                             = 0x50,
   QCFW_UPDATE_ERR_UEFI_API_ALLOCATE_POOL                     = 0x51,

   /*Payload related error*/
   QCFW_UPDATE_ERR_PAYLOAD_LOWEST_SUPPORT_VERSION             = 0x60,
   QCFW_UPDATE_ERR_PAYLOAD_AUTHENTICATION                     = 0x61,
   QCFW_UPDATE_ERR_PAYLOAD_IMG_FORMAT                         = 0x62,
   QCFW_UPDATE_ERR_PAYLOAD_LOADING                            = 0x63,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP                      = 0x64,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP_LOCATE_PROTOCOL      = 0x65,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP_HANDLE_PROTOCOL      = 0x66,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP_GET_FILE_NAME        = 0x67,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP_ALLOCATE_POOL        = 0x68,
   QCFW_UPDATE_ERR_PAYLOAD_LOAD_FROM_ESP_APPEND_DEV_NODE      = 0x69,

   /*Resource related error*/
   QCFW_UPDATE_ERR_RESOUCE_POWER_INSUFFICEIENT_BATTERY        = 0x70,

   /*Handle related error*/
   QCFW_UPDATE_ERR_HANDLE_REGISTRY                            = 0x90,

   /*MetaData related error*/
   QCFW_UPDATE_ERR_METADATA_INVALID_PARAM                     = 0xA0,
   QCFW_UPDATE_ERR_METADATA_GET_FV_HANDLE                     = 0xA1,
   QCFW_UPDATE_ERR_METADATA_FV_READ                           = 0xA2,
   QCFW_UPDATE_ERR_METADATA_BAD_SIGNATURE                     = 0xA3,
   QCFW_UPDATE_ERR_METADATA_BAD_HEADER_SIZE                   = 0xA4,
   QCFW_UPDATE_ERR_METADATA_BAD_V1_HEADER_SIZE                = 0xA5,
   QCFW_UPDATE_ERR_METADATA_BAD_V2_HEADER_SIZE                = 0xA6,
   QCFW_UPDATE_ERR_METADATA_BAD_V3_HEADER_SIZE                = 0xA7,
   /*Parser related error*/
   QCFW_UPDATE_ERR_PARSER_SET_PARTITION_HANDLES               = 0xB0,

   /*Crypto related error*/
   QCFW_UPDATE_ERR_CRYPTO_SSD_CHECKING                        = 0xC0,
   QCFW_UPDATE_ERR_CRYPTO_SSD_DECRYPT                         = 0xC1,
   QCFW_UPDATE_ERR_CRYPTO_OPM_PRIV_KEY_CRYPTO                 = 0xC2,
   QCFW_UPDATE_ERR_CRYPTO_MSM_KEY_ENCRYPT                     = 0xC3,

   /*FwEntry related error*/
   QCFW_UPDATE_ERR_FWENTRY_READ_ENTRY_FROM_FV                 = 0xD0,
   QCFW_UPDATE_ERR_FWENTRY_UNKNOWN_UPDATE_TYPE                = 0xD1,
   QCFW_UPDATE_ERR_FWENTRY_UNKNOWN_UPDATE_BACKUP_TYPE         = 0xD2,
   QCFW_UPDATE_ERR_FWENTRY_NULL_ENTRY                         = 0xD3,
   QCFW_UPDATE_ERR_FWENTRY_VALUE_CHECKING                     = 0xD4,
   QCFW_UPDATE_ERR_FWENTRY_BAD_VALUE                          = 0xD5,
   QCFW_UPDATE_ERR_FWENTRY_COMBINATION_CKECKING               = 0xD6,
   QCFW_UPDATE_ERR_FWENTRY_BAD_COMBINATION                    = 0xD7,

   /*FwEntry list related error*/
   QCFW_UPDATE_ERR_LIST_ADD_NODE                              = 0xE0,
   QCFW_UPDATE_ERR_LIST_SSD_EXCLUSIVE_CHECKING                = 0xE1,
   QCFW_UPDATE_ERR_LIST_SSD_NOT_EXCLUSIVE                     = 0xE2,
   QCFW_UPDATE_ERR_LIST_PARTITION_DEVPATH_EXCLUSIVE_CHECKING  = 0xE3,
   QCFW_UPDATE_ERR_LIST_PARTITION_DEVPATH_NOT_EXCLUSIVE       = 0xE4,
   QCFW_UPDATE_ERR_LIST_QCOM_DPP_DEVPATH_EXCLUSIVE_CHECKING   = 0xE5,
   QCFW_UPDATE_ERR_LIST_QCOM_DPP_DEVPATH_NOT_EXCLUSIVE        = 0xE6,
   QCFW_UPDATE_ERR_LIST_OEM_DPP_DEVPATH_EXCLUSIVE_CHECKING    = 0xE7,
   QCFW_UPDATE_ERR_LIST_OEM_DPP_DEVPATH_NOT_EXCLUSIVE         = 0xE8,
   QCFW_UPDATE_ERR_LIST_FAT_DEVPATH_EXCLUSIVE_CHECKING        = 0xE9,
   QCFW_UPDATE_ERR_LIST_FAT_DEVPATH_NOT_EXCLUSIVE             = 0xEA,
   QCFW_UPDATE_ERR_LIST_NULL_NODE                             = 0xEB,
   QCFW_UPDATE_MAX 

} QCFW_UPDATE_FLAG_VALUE;


/*=========================================================================== 
                         DEVICE PROVISIONING DEFINITIONS
===========================================================================*/

/*!
 * Enum defining the different return codes for the DPP get/set API
 */
typedef enum DPP_STATUS
{
	DPP_STATUS_SUCCESS = 0,
	DPP_STATUS_FAIL,
	DPP_STATUS_FAIL_GENERATING_PROVISIONING_PATH,
	DPP_STATUS_FAIL_GETTING_FILE_HANDLE,
	DPP_STATUS_FAIL_FILE_READ,
	DPP_STATUS_FAIL_FILE_WRITE,
	DPP_STATUS_FAIL_AUTHENTICATION,
	DPP_STATUS_FAIL_GETTING_FILE_INFO,
	DPP_STATUS_FAIL_CREATING_DIRECTORY,
	DPP_STATUS_FAIL_INVALID_ARGUMENTS,
	DPP_STATUS_FAIL_INSUFFICIENT_MEMORY,
	DPP_STATUS_FAIL_READING_REGISTRY
} DPP_STATUS;

/*!
 * The path (from the root of the EFI System Partition) to the folder where
 * device provisioning read-only files are located.
 */
#define DPP_FOLDER_PATH L"DPP"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the factory provisioning commit operations.
 */
#define PROVISIONING_COMMIT_FILE_NAME L"FACTORY.FLG"

/*!
 * The path (from the root of the GPP FAT16) to the folder where
 * device SMBIOS configuration file is located.
 */
#define SMBIOS_FOLDER_PATH L"SMBIOS"

/*!
 * Specify if the given factory provisioning operation should be committed or not
 */
typedef enum PROVISIONING_OPERATION_TYPE
{
   PROVISIONING_OPERATION_NO_COMMIT = 0,
   PROVISIONING_OPERATION_COMMIT = 1,

} PROVISIONING_OPERATION_TYPE;

#define PROVISIONING_COMMIT_FILE_VERSION 0x01

/*!
 * Data structure that describes the operation request (commit or no commit) for each
 * factory provisioning operation.
 */
typedef struct _PROVISIONING_COMMIT_FILE_TYPE
{
   UINT8 Version;      /* The revision of PROVISIONING_COMMIT_FILE_VERSION: 0x01 */

   /* The following use PROVISIONING_OPERATION_TYPE values to specify commit or no commit: */
   
   UINT8 Provision;    /* Commit for /Provision */
   UINT8 OA3;          /* Commit for /OA3 */
   UINT8 SMBIOS;       /* Commit for /SMBIOS */
   UINT8 SSD;          /* Commit for /SSD */
   UINT8 EraseOA3;     /* Commit for /EraseOA3 */
   UINT8 EraseAll;     /* Commit for /EraseAll */
   UINT8 Lock;         /* Commit for /Lock */
   UINT8 KEKpub;       /* Commit for /KEKpub */
   UINT8 SignatureDB;  /* Commit for /SignatureDB */
   UINT8 SignatureDBX; /* Commit for /SignatureDBX */
   UINT8 PKpub;        /* Commit for /PKpub */
   UINT8 RPMB;         /* Commit for /RPMB */
   UINT8 DebugPolicy;  /* Commit for /DebugPolicy */
   UINT8 DebugPolicyRemove; /* Commit for /DebugPolicy /Remove */
   UINT8 OPM;          /* Commit for /OPM */
   UINT8 HDCPCEK;      /* Commit for /HDCPCEK */
   UINT8 HDCPData;     /* Commit for /HDCPData */
   UINT8 Reserved;     /* Future use, set to 0x0 */
   
} PROVISIONING_COMMIT_FILE_TYPE;

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the factory provisioning result codes giving the status for each operation
 */
#define PROVISIONING_RESULT_FILE_NAME L"FACTORY.RES"

/*!
 * Result value types for a given factory provisioning operation
 */
typedef enum PROVISIONING_RESULT_TYPE
{
   PROVISIONING_RESULT_NO_OPERATION = 0,
   PROVISIONING_RESULT_SUCCESS,
   PROVISIONING_RESULT_FAIL_LOCKED,
   PROVISIONING_RESULT_FAIL_GENERAL,
   PROVISIONING_RESULT_FAIL_PRIOR_INSTANCE_EXIST,
   PROVISIONING_RESULT_MAX
} PROVISIONING_RESULT_TYPE;

#define PROVISIONING_RESULT_FILE_VERSION 0x01

/*!
 * Data structure that describes the result for each of the provisioning operations
 */
typedef struct _PROVISIONING_RESULT_FILE_TYPE
{
   UINT8 Version;      /* Version number: PROVISIONING_RESULT_FILE_VERSION: 0x01 */

   /* The following use PROVISIONING_RESULT_TYPE values: */

   UINT8 Provision;    /* Result for /Provision */
   UINT8 OA3;          /* Result for /OA3 */
   UINT8 SMBIOS;       /* Result for /SMBIOS */
   UINT8 SSD;          /* Result for /SSD */
   UINT8 EraseOA3;     /* Result for /EraseOA3 */
   UINT8 EraseAll;     /* Result for /EraseAll */
   UINT8 Lock;         /* Result for /Lock */
   UINT8 KEKpub;       /* Result for /KEKpub */
   UINT8 SignatureDB;  /* Result for /SignatureDB */
   UINT8 SignatureDBX; /* Result for /SignatureDBX */
   UINT8 PKpub;        /* Result for /PKpub */
   UINT8 RPMB;         /* Result for /RPMB */
   UINT8 DebugPolicy;  /* Result for /DebugPolicy */
   UINT8 DebugPolicyRemove; /* Result for /DebugPolicy /Remove */
   UINT8 OPM;          /* Result for /OPM */
   UINT8 HDCPCEK;      /* Result for /HDCPCEK */
   UINT8 HDCPData;     /* Result for /HDCPData */
   UINT8 Reserved;     /* Future use, set to 0x0 */

} PROVISIONING_RESULT_FILE_TYPE;

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the OEM Activation 3.0 MSDM ACPI table.
 */
#define PROVISIONING_OA3_FILE_NAME L"MSDM.ACP"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the SMBIOS configuration.
 */
#define PROVISIONING_SMBIOS_CFG_FILE_NAME L"SMBIOS.CFG"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the SSD decryption key.
 */
#define PROVISIONING_SSD_KEY_FILE_NAME L"SSD.KEY"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the KEKpub.
 */
#define PROVISIONING_KEKPUB_FILE_NAME L"KEKPUB.DAT"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the SignatureDB.
 */
#define PROVISIONING_SIGNATUREDB_FILE_NAME L"SIGDB.DAT"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the SignatureDB.
 */
#define PROVISIONING_SIGNATUREDBX_FILE_NAME L"SIGDBX.DAT"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the PKpub.
 */
#define PROVISIONING_PKPUB_FILE_NAME L"PKPUB.DAT"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the Debug Policy.
 */
#define PROVISIONING_DEBUGPOLICY_FILE_NAME L"DBGPOLICY.DAT"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the OPM public cert
 */
#define PROVISIONING_OPM_PUB_FILE_NAME L"OPM_PUB.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the encrypted OPM Private key
 */
#define PROVISIONING_OPM_PRIV_FILE_NAME L"OPM_PRIV.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the HDCP 2.x CEK key
 */
#define PROVISIONING_HDCPCEK_FILE_NAME L"HDCPCEK.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the file containing
 * the HDCP 2.x data
 */
#define PROVISIONING_HDCPDATA_FILE_NAME L"HDCPDATA.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH) to the folder where
 * Qualcomm provisioning files are located.
 *
 * To be used as the lpszCompanyName argument for calls to DPP APIs.
 */
#define DPP_QUALCOMM_FOLDER_PATH L"QCOM"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the WLAN provisioning data.
 */
#define WLAN_PROVISION_DATA L"WLAN.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the WLAN CLPC provisioning data.
 */
#define WLAN_CLPC_PROVISION_DATA L"WLAN_CLPC.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the bluetooth provisioning data.
 */
#define BT_PROVISION_DATA L"BT.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the battery provisioning data.
 */
#define BATTERY_PROVISION_DATA L"BATTERY.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the audio provisioning data.
 */
#define AUDIO_PROVISION_DATA L"AUDIO.PROVISION"

/*!
 * The path (from the root of the DPP_FOLDER_PATH\QCOM) to the file containing
 * the ultrasound pen unit calibration packet provisioning data.
 */
#define US_PEN_UNIT_CALIB_PROVISION_DATA L"US.PEN.UNIT.PROVISION"


/*! 
 *      DPP Layout 
 *  
 *      +------------------------------+
 *      |    DPP Header                |
 *      +------------------------------+
 *      |    DPP Directory Header      |
 *      +------------------------------+
 *      |    DPP Directory Entry 0     |
 *      +------------------------------+
 *      |    ......                    |
 *      +------------------------------+
 *      |    DPP Directory Entry N - 1 |
 *      +------------------------------+
 *      |                              |
 *      |                              |
 *      |    Device Privision Data     |
 *      |                              |
 *      |                              |
 *      +------------------------------+
 *  
 */

/*!
 * Data structure that describes the DPP Partion status
 */
typedef struct _DPP_PART_STATUS
{
    BOOLEAN Initialized;
} DPP_PART_STATUS, *PDPP_PART_STATUS;

/*!
 * DPP Provision Data Type
 */
typedef enum _DPP_DATA_TYPE 
{
    DPP_DATA_TYPE_QCOM = 0,
    DPP_DATA_TYPE_OEM,
    DPP_DATA_TYPE_MAX
} DPP_DATA_TYPE;

/*!
 * DPP Provision Data Entry Constants
 */
#define DPP_DATA_TYPE_MAX_LENGTH    4
#define DPP_DATA_NAME_MAX_LENGTH    256
#define DPP_MAX_DATA_ENTRIES        200

/*!
 * DPP Data Status
 */
typedef enum _DPP_DATA_STATUS 
{
    DPP_DATA_INVALID = 0,
    DPP_DATA_VALID
} DPP_DATA_STATUS;

/*!
 * Data structure that describes the Provision Data Entires
 */
typedef struct _DPP_DATA_ENTRY
{
    UINT16  Name[DPP_DATA_NAME_MAX_LENGTH+1];   /* Provision Data Name, unicode */
    UINT16  Type[DPP_DATA_TYPE_MAX_LENGTH+1];   /* Provision Data Type, unicode */
    UINT32  AllocatedSize;                      /* Space allocated */
    UINT32  UsedSize;                           /* Space currently used */
    UINT32  Valid;                              /* Indicate if it is a valid entry */
    UINT32  Offset;                             /* Offset of associated Provision Data within DPP data space*/
    UINT8   Reserved[484];                      /* Reserved */
} DPP_DATA_ENTRY, *PDPP_DATA_ENTRY;

/*!
 * Data structure that describes the DPP Directory Header
 */
typedef struct _DPP_DIRECTORY_HEADER
{
    UINT32  Size;                           /* Directory Header size */
    UINT32  DataEntriesOffset;              /* Offset of 1st data entry within DPP */
    UINT32  DataEntriesAllocated;           /* Max number of Provision Data entries allowed */
    UINT32  DataEntriesUsed;                /* Number of Provision Data entries currently in use */
    UINT32  DataSpaceOffset;                /* Offset of 1st provision data within DPP */     
    UINT32  DataSpaceAllocated;             /* Allocated provision data space in bytes */
    UINT32  DataSpaceUsed;                  /* Used provision data space in bytes */
    UINT8   Reserved[36];                   /* Reserved */
} DPP_DIRECTORY_HEADER, *PDPP_DIRECTORY_HEADER; 

/*!
 * Data structure that describes the DPP Directory
 */
typedef struct _DPP_DIRECTORY
{
    DPP_DIRECTORY_HEADER    Header;                     /* Header */
    DPP_DATA_ENTRY          ProvisionDataEntries[1];    /* Provision Data Entries */
} DPP_DIRECTORY, *PDPP_DIRECTORY;

/*!
 * DPP Header Constants
 */
#define DPP_SIGNATURE   0x20505044  /* 'DPP ' */
#define DPP_REVISION    0x00010000

/*!
 * Data structure that describes the DPP header
 */
typedef struct _DPP_HEADER
{
   UINT32  Signature;       /* Signature */
   UINT32  Revision;        /* Revision */
   UINT32  Size;            /* DPP header size */
   UINT32  Reserved1;       /* Reserved */
   UINT32  DirectoryOffset; /* Offset of Provisioning Data Directory within DPP */
   UINT32  BlockSize;       /* DPP block size */
   UINT64  PartSize;        /* DPP size*/
   UINT8   Reserved2[32];   /* Reserved */
} DPP_HEADER, *PDPP_HEADER;



/*=========================================================================== 
                         FIRMWARE UPDATE DEFINITIONS
===========================================================================*/

/*!
 * The path (from the root of the EFI System Partition) to the file containing
 * the firmware update payload.
 */
#define FWUPDATE_PAYLOAD_FILE L"QPAYLOAD.EFI"

/*!
 * The path (from the root of the EFI System Partition) to the file containing
 * the legacy firmware update commit flag file.
 */
#define FWUPDATE_COMMIT_FILE L"FCT_COMM.FLG"

/*=========================================================================== 
                         PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

#endif /* ESP_DPP_H */
