/** @file FwPayloadEntry.h
   
  Payload entry structures and functions firmware update solution.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when             who         what, where, why
 ----------   ---         --------------------------------------------------
 2013/05/06    mic        Unsupport V1/V2 payload
 2012/09/21    mic        Initial version

=============================================================================*/

#ifndef __QCOM_FW_PAYLOADENTRY_H_
#define __QCOM_FW_PAYLOADENTRY_H_

#include <Library/FwCommonLib.h>
#include "FwEsrt.h"
#define QPAYLOAD_METADATA_SIGNATURE1  0x2E1946FB
#define QPAYLOAD_METADATA_SIGNATURE2  0x7F744D57

#define FILE_PATH_MAX_SIZE 255
#define DPP_NAME_MAX_SIZE 255

typedef enum
{
  FWENTRY_OPERATION_TYPE_IGNORE   = 0x00000000,
  FWENTRY_OPERATION_TYPE_UPDATE   = 0x00000001,
  FWENTRY_OPERATION_TYPE_DELETE   = 0x00000002,
  FWENTRY_OPERATION_TYPE_MAX      = 0x00000003
  
}FWENTRY_OPERATION_TYPE;

typedef enum
{
  FWENTRY_OPERATION_PATH_TYPE_SOURCE  = 0x00000000,
  FWENTRY_OPERATION_PATH_TYPE_DEST    = 0x00000001,
  FWENTRY_OPERATION_PATH_TYPE_BACKUP  = 0x00000002,
  FWENTRY_OPERATION_PATH_TYPE_MAX     = 0x00000003
  
}FWENTRY_OPERATION_PATH_TYPE;

typedef enum
{
  FWENTRY_SOURCE_TYPE_FV         = 0x00000000,
  FWENTRY_SOURCE_TYPE_MAX        = 0x00000001
    
} FWENTRY_SOURCE_TYPE;

typedef enum
{
  FWENTRY_UPDATE_TYPE_PARTITION       = 0x00000000,
  FWENTRY_UPDATE_TYPE_FAT_FILE        = 0x00000001,
  FWENTRY_UPDATE_TYPE_DPP_QCOM        = 0x00000002,
  FWENTRY_UPDATE_TYPE_DPP_OEM         = 0x00000003,
  FWENTRY_UPDATE_TYPE_OPM_PRIV_KEY    = 0x00000004,
  FWENTRY_UPDATE_TYPE_SSD             = 0x00000005, 
  FWENTRY_UPDATE_TYPE_MAX             = 0x00000006
  
} FWENTRY_UPDATE_TYPE;

typedef enum
{
  FWENTRY_BACKUP_TYPE_PARTITION       = 0x00000000,
  FWENTRY_BACKUP_TYPE_FAT_FILE        = 0x00000001,
  FWENTRY_BACKUP_TYPE_MAX             = 0x00000002

} FWENTRY_BACKUP_TYPE;

/*!
 * Data structure that describes the QPayload Metadata
 */
typedef struct _QPAYLOAD_METADATA_HEADER_V1
{
  UINT32  Signature1;               /* First signature: QPAYLOAD_METADATA_SIGNATURE1 */
  UINT32  Signature2;               /* Second signature: QPAYLOAD_METADATA_SIGNATURE2 */
  UINT32  Revision;                 /* Revision: 0x1 */
  UINT32  Size;                     /* Size in bytes of remaining data after the size field: 0x8 */
  UINT32  FirmwareVersion;          /* Version of the QPayload package */
  UINT32  LowestSupportedVersion;   /* Firmware cannot roll back to a version earlier than this */
} QPAYLOAD_METADATA_HEADER_V1;

/*!
 * Data structure that describes the QPayload Metadata with Breaking Change Number
 */
typedef struct _QPAYLOAD_METADATA_HEADER_V2
{
  UINT32  Signature1;               /* First signature: QPAYLOAD_METADATA_SIGNATURE1 */
  UINT32  Signature2;               /* Second signature: QPAYLOAD_METADATA_SIGNATURE2 */
  UINT32  Revision;                 /* Revision: 0x1 */
  UINT32  Size;                     /* Size in bytes of remaining data after the size field: 0x8 */
  UINT32  FirmwareVersion;          /* Version of the QPayload package */
  UINT32  LowestSupportedVersion;   /* Firmware cannot roll back to a version earlier than this */
  UINT32  BreakingChangeNumber;     /*Breaking Change Number to track if we need a double update*/
} QPAYLOAD_METADATA_HEADER_V2;

typedef struct _QPAYLOAD_METADATA_HEADER_V3
{
  UINT32  Signature1;               /* First signature: QPAYLOAD_METADATA_SIGNATURE1 */
  UINT32  Signature2;               /* Second signature: QPAYLOAD_METADATA_SIGNATURE2 */
  UINT32  Revision;                 /* Revision: QPAYLOAD_METADATA_REVISION */
  UINT32  Size;                     /* Size in bytes of remaining data after the size field */
  UINT32  FirmwareVersion;          /* Version of the QPayload package */
  UINT32  LowestSupportedVersion;   /* Firmware cannot roll back to a version earlier than this */
  UINT32  BreakingChangeNumber;     /* Breaking Change Number to track if we need a double update */
  UINT32  Reserved1;                /* Reserved for future use */
  UINT32  Reserved2;                /* Reserved for future use */
  UINT32  EntryCount;               /* Number of file entries to update */

} QPAYLOAD_METADATA_HEADER_V3;


/* One of PartitionName or PartitionTypeGUID must be specified, or both. If more than one partition of a given
   type is found, and name is not specified, then the first one found is used. */
   
#pragma pack(push)
#pragma pack(1)
typedef struct _FWENTRY_DEVICE_PATH
{
  FWENTRY_EMMC_PARTITION_TYPE   EmmcPartitionType;                      /* FWENTRY_EMMC_PARTITION_TYPE: User, Boot1, Boot2, GPP */
  CHAR16                        PartitionName[PARTITION_NAME_MAX_CHAR16_SIZE]; /* Optional: Name string of destination partition in GPT */ 
  EFI_GUID                      PartitionTypeGUID;                      /* Optional: Destination Partition Type GUID to write file */
  CHAR16                        FileName[FILE_PATH_MAX_SIZE];           /* For FAT type or DPP item only: Unicode full path to the file including file name */
  
} FWENTRY_DEVICE_PATH;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct _FWENTRY_METADATA
{
  EFI_GUID               FileGuid;       /* File GUID that corresponds with FVUpdate.xml file item */
  FWENTRY_OPERATION_TYPE Operation;      /* FWENTRY_OPERATION_TYPE: The type of operation (Ignore, Update, delete)*/
  FWENTRY_UPDATE_TYPE    UpdateType;     /* FWENTRY_UPDATE_TYPE: Type of update (Partition, FAT, DPP, OPM_Priv) */
  FWENTRY_BACKUP_TYPE    BackupType;     /* FWENTRY_BACKUP_TYPE: Type of backup (None, Partition, FAT) */
  FWENTRY_DEVICE_PATH    UpdatePath;     /* Path to file or partition being updated */
  FWENTRY_DEVICE_PATH    BackupPath;     /* Path to file or partition used for backup */

} FWENTRY_METADATA;
#pragma pack(pop)

typedef struct _QPAYLOAD_FW_ENTRY
{
   VOID                       *DecryptedFwEntryBuffer;                   /* Pointer to the buffer with the decrypted file */
   UINTN                      DecryptedFwEntrySize;                      /* Size of the DecryptedFileBuffer*/
   BOOLEAN                    IsEncrypted;                               /* Flag to track if this entry was encrypted in the payload */
   EFI_HANDLE                 Handle[FWENTRY_OPERATION_PATH_TYPE_MAX];   /* source/dest/backup handle for this entry*/
   FWENTRY_METADATA           Metadata;                                  /* Metadata entry extracted from payload*/
                                 
} QPAYLOAD_FW_ENTRY;

/**
 * Function to print FwEntry list
 * 
 * @param List - FwEntry LinkedList
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPrintFwList(
  IN FW_LINKED_LIST_TYPE *List,
  IN UINTN               DebugLevel
  );

/**
 * Function to print FwEntry
 * 
 * @param pFwEntry - pointer point to FwEntry
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPrintFwEntry(
  IN QPAYLOAD_FW_ENTRY *pFwEntry,
  IN UINTN             DebugLevel
  );

/**
 * Function to destruct   QPAYLOAD_FW_ENTRY
 * 
 * @param Node - node for destruction

 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryDestructor(
  IN VOID *Data
  );

/**
 * Function to get  FwEntry linkedlist handle
 * 
 * @param PartHandle - partition handle

 * @return : EFI_STATUS
 */
EFI_STATUS
FwGetFwEntryList(
    IN OUT FW_LINKED_LIST_TYPE **List
    );

/**
 * Payload parser main function
 *
 * @param  MsftAttemptStatus   -Microsoft last attempt status
 * @param  QcResultFlag        -QCFirmwareUpdate result
 *
 * @return : EFI_STATUS
 */

EFI_STATUS
FwPayloadParserMain(
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag 
    );

/**
 * . Function to get the Metadata 
 * 
 * @param Version    - metadata version
 * @param FileBuffer - Metadata buffer
 * @param BufferSize - Metadata size
 * @return : EFI_STATUS
 */
EFI_STATUS
FwFindMetadata(
  OUT UINT32 *Version,
  OUT VOID   **FileBuffer,
  OUT UINTN  *BufferSize,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    );

/**
 *. This function should parse V3 payload
 * @param MetaData     - Metadata buffer
 * @param BufSize        - Metadata buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPayloadV3Parser(
  IN VOID *MetaData,
  IN UINTN BufSize,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

    );


/**
 *   Function to validate a payload v3 fw entry
 * 
 * @param Data           - Payload firmware entry metadata
 * @param bEntryGood - Pesult of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryValidation(
    IN FWENTRY_METADATA *Data,
    OUT BOOLEAN         *bEntryGood,
    OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
    OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

    );

/**
 * Function to decrypt a payload v3 fw entry
 * 
 * @param FwEntry - payload fw entry

 * @return : EFI_STATUS
 */
EFI_STATUS
FwPayloadV3EntryCrypto(
    IN QPAYLOAD_FW_ENTRY* FwEntry
    );

#endif


