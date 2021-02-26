/** @file PartitionHandles.h
   
  Defines types for PartionHandles.c
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/11/15     mic         Added FwUpdateGetEmmcPartitionTypeGuid function
 2012/10/08     mic         Removed unused Guids and comments
 2012/08/08     mic         Added maximum update attempts support
 2012/08/08     rsb         Removed unused file enum types
 2012/07/10     rs          Changes in FindinFV to check all FWFile Enums
 2012/06/22     mic         Added support for OPM keys 
 2012/06/21     rs          Adding support for FPDT ACPI table
 2012/06/12     rs          Added support for BreakChangeNumber
 2011/10/12     jthompso    Initial version

=============================================================================*/
#ifndef __QCOM_FW_HANDLES_PARTITION_H__
#define __QCOM_FW_HANDLES_PARTITION_H__

#include <Library/FwCommonLib.h>


typedef enum
{
  FWENTRY_EMMC_PARTITION_USER_DATA = 0x00000000,
  FWENTRY_EMMC_PARTITION_BOOT1     = 0x00000001,
  FWENTRY_EMMC_PARTITION_BOOT2     = 0x00000002,
  FWENTRY_EMMC_PARTITION_RPMB      = 0x00000003,
  FWENTRY_EMMC_PARTITION_GPP1      = 0x00000004,
  FWENTRY_EMMC_PARTITION_GPP2      = 0x00000005,
  FWENTRY_EMMC_PARTITION_GPP3      = 0x00000006,
  FWENTRY_EMMC_PARTITION_GPP4      = 0x00000007,
  FWENTRY_EMMC_PARTITION_MAX       = 0x00000008,
  FWENTRY_EMMC_PARTITION_DONT_CARE = 0x0000ffff

} FWENTRY_EMMC_PARTITION_TYPE;

#define PARTITION_NAME_MAX_CHAR16_SIZE 36

//Vendor IDs (GUIDs to distinguish eMMC physical partitons
#define EFI_EMMC_USER_PARTITION_GUID \
    { 0xb615f1f5, 0x5088, 0x43cd, { 0x80, 0x9c, 0xa1, 0x6e, 0x52, 0x48, 0x7d, 0x00 } }

#define EFI_EMMC_BOOT_PARTITION1_GUID \
    { 0x12C55B20, 0x25D3, 0x41C9, { 0x8E, 0x06, 0x28, 0x2D, 0x94, 0xC6, 0x76, 0xAD } }

#define EFI_EMMC_BOOT_PARTITION2_GUID \
    { 0x6B76A6DB, 0x0257, 0x48A9, { 0xAA, 0x99, 0xF6, 0xB1, 0x65, 0x5F, 0x7B, 0x00 } }
 
#define EFI_EMMC_RPMB_PARTITION_GUID \
    { 0xC49551EA, 0xD6BC, 0x4966, { 0x94, 0x99, 0x87, 0x1E, 0x39, 0x31, 0x33, 0xCD } }

#define EFI_EMMC_GPP_PARTITION1_GUID \
   { 0xB9251EA5, 0x3462, 0x4807, { 0x86, 0xC6, 0x89, 0x48, 0xB1, 0xB3, 0x61, 0x63 } }

#define EFI_EMMC_GPP_PARTITION2_GUID \
   { 0x24F906CD, 0xEE11, 0x43E1, { 0x84, 0x27, 0xDC, 0x7A, 0x36, 0xF4, 0xC0, 0x59 } }

#define EFI_EMMC_GPP_PARTITION3_GUID \
   { 0x5A5709A9, 0xAC40, 0x4F72, { 0x88, 0x62, 0x5B, 0x01, 0x04, 0x16, 0x6E, 0x76 } }

#define EFI_EMMC_GPP_PARTITION4_GUID \
   { 0xA44E27C9, 0x258E, 0x406E, { 0xBF, 0x33, 0x77, 0xF5, 0xF2, 0x44, 0xC4, 0x87 } }


//Partition Type GUID

//DEA0BA2C-CBDD-4805-B4 F9-F4 28 25 1C 3E 98
#define QCOM_FW_UPDATE_PARTITION_TYPE_BOOT1_SBL1 \
  { \
    0xDEA0BA2C, 0xCBDD, 0x4805, {0xB4, 0xF9, 0xF4, 0x28, 0x25, 0x1C, 0x3E, 0x98 } \
  }
//B2CCEF8E-81BD-4E27-A2 28-5F 0D E0 8C A1 2C
#define QCOM_FW_UPDATE_PARTITION_TYPE_BOOT1_SBL1_BAK \
  { \
    0xB2CCEF8E, 0x81BD, 0x4E27, {0xA2, 0x28, 0x5F, 0x0D, 0xE0, 0x8C, 0xA1, 0x2C } \
  }
//8C6B52AD-8A9E-4398-AD 09-AE 91 6E 53 AE 2D
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SBL2 \
  { \
    0x8C6B52AD, 0x8A9E, 0x4398, {0xAD, 0x09, 0xAE, 0x91, 0x6E, 0x53, 0xAE, 0x2D } \
  }
//62E53DFC-81D1-47F3-8867-22BC5592605F
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SBL2_BAK \
  { \
    0x62E53DFC, 0x81D1, 0x47F3, {0x88, 0x67, 0x22, 0xBC, 0x55, 0x92, 0x60, 0x5F } \
  }
//05E044DF-92F1-4325-B6 9E-37 4A 82 E9 7D 6E
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SBL3 \
  { \
    0x05E044DF, 0x92F1, 0x4325, {0xB6, 0x9E, 0x37, 0x4A, 0x82, 0xE9, 0x7D, 0x6E } \
  }
//7EBF5EA5-4218-44A5-8D57-BF1C749FE7F1
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SBL3_BAK \
  { \
    0x7EBF5EA5, 0x4218, 0x44A5, {0x8D, 0x57, 0xBF, 0x1C, 0x74, 0x9F, 0xE7, 0xF1 } \
  }
//400FFDCD-22E0-47E7-9A23-F16ED9382388
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_UEFI \
  { \
    0x400FFDCD, 0x22E0, 0x47E7, {0x9A, 0x23, 0xF1, 0x6E, 0xD9, 0x38, 0x23, 0x88 } \
  }
//E1D14C55-ED7E-464E-A340-E045E6D3108A
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_UEFI_BAK \
  { \
    0xE1D14C55, 0xED7E, 0x464E, {0xA3, 0x40, 0xE0, 0x45, 0xE6, 0xD3, 0x10, 0x8A } \
  }
//098DF793-D712-413D-9D4E-89D711772228
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_RPM \
  { \
    0x098DF793, 0xD712, 0x413D, {0x9D, 0x4E, 0x89, 0xD7, 0x11, 0x77, 0x22, 0x28 } \
  }
//BF132EB0-FC69-4D84-A13C-52F333E63906
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_RPM_BAK \
  { \
    0xBF132EB0, 0xFC69, 0x4D84, {0xA1, 0x3C, 0x52, 0xF3, 0x33, 0xE6, 0x39, 0x06 } \
  }
//A053AA7F-40B8-4B1C-BA08-2F68AC71A4F4
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_TZ \
  { \
    0xA053AA7F, 0x40B8, 0x4B1C, {0xBA, 0x08, 0x2F, 0x68, 0xAC, 0x71, 0xA4, 0xF4 } \
  }
//FD78FB93-1037-4AAA-BC9C-37E716D27BEE
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_TZ_BAK \
  { \
    0xFD78FB93, 0x1037, 0x4AAA, {0xBC, 0x9C, 0x37, 0xE7, 0x16, 0xD2, 0x7B, 0xEE } \
  }
//69B4201F-A5AD-45EB-9F49-45B38CCDAEF5
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_WINSECAPP \
  { \
    0x69B4201F, 0xA5AD, 0x45EB, {0x9F, 0x49, 0x45, 0xB3, 0x8C, 0xCD, 0xAE, 0xF5 } \
  }
//B8DB2AFE-A8D9-45B3-8661-C29F592B6E76
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_WINSECAPP_BAK \
  { \
    0xB8DB2AFE, 0xA8D9, 0x45B3, {0x86, 0x61, 0xC2, 0x9F, 0x59, 0x2B, 0x6E, 0x76 } \
  }

//2C86E742-745E-4FDD-BF D8-B6 A7 AC 63 87 72
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SSD \
  { \
    0x2C86E742, 0x745E, 0x4FDD, {0xBF, 0xD8, 0xB6, 0xA7, 0xAC, 0x63, 0x87, 0x72 } \
  }

//9992FD7D-EC66-4CBC-A3370DA1D4C93F8F
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_DPP \
  { \
    0x9992FD7D, 0xEC66, 0x4CBC, {0xA3, 0x37, 0x0D, 0xA1, 0xD4, 0xC9, 0x3F, 0x8F } \
  }

//543C031A-4CB6-4897-BF FE-4B 48 57 68 A8 AD
#define QCOM_FW_UPDATE_PARTITION_TYPE_GPP_FAT16 \
  { \
    0x543C031A, 0x4CB6, 0x4897, {0xBF, 0xFE, 0x4B, 0x48, 0x57, 0x68, 0xA8, 0xAD } \
  }

//C12A7328-F81F-11D2-BA 4B-00 A0 C9 3E C9 3B 
#define QCOM_FW_UPDATE_PARTITION_TYPE_USER_ESP \
  { \
    0xC12A7328, 0xF81F, 0x11D2, {0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B } \
  }

#define QCOM_FW_UPDATE_PARTITION_TYPE_NULL \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }


/* Caution: The enumeration list QCOM_FW_UPDATE_FW_FILE must be one-to-one matching with the array FilePaths[] */
typedef enum
{
    QCOM_FW_UPDATE_FILE_LEGACY_UPDATE_COMMIT_FLAG_FILE,
    QCOM_FW_UPDATE_FILE_FACTORY_PROVISIONED_FLAG_FILE,
    QCOM_FW_UPDATE_FILE_FACTORY_FLAG_FILE,
    QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE,
    QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE,
    QCOM_FW_UPDATE_FILE_BP_GPT_FILE,
    QCOM_FW_UPDATE_FILE_GPP_GPT_FILE, 
    QCOM_FW_UPDATE_PAYLOAD_APP_FILE,
    QCOM_FW_UPDATE_OA3_MSDM_ORIG_FILE,
    QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE,
    QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE,
    QCOM_FW_UPDATE_DPP_OEM_DIR_FILE,
    QCOM_FW_UPDATE_VERSION_FILE,
    QCOM_FW_UPDATE_SSD_KEYSTORE_FILE,
    QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE,
    QCOM_FW_UPDATE_OPM_PUBKEY_FILE,
    QCOM_FW_UPDATE_OPM_PUBKEY_BCK_FILE,
    QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIVKEY_FILE,
    QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIVKEY_BCK_FILE,
    QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE,
    QCOM_FW_UPDATE_FILE_MAX

}QCOM_FW_UPDATE_FW_FILE;


typedef enum
{
  QCOM_FW_UPDATE_PARTITION_GPP_SSD,
  QCOM_FW_UPDATE_PARTITION_GPP_FAT16,
  QCOM_FW_UPDATE_PARTITION_USER_ESP,
  QCOM_FW_UPDATE_PARTITION_MAX

}QCOM_FW_UPDATE_FW_PARTITION;


typedef enum
{
    QCOM_FW_UPDATE_PARTITION_TYPE_BLOCK,
    QCOM_FW_UPDATE_PARTITION_TYPE_FAT,
    QCOM_FW_UPDATE_PARTITION_TYPE_UPDATE_PAYLOAD,
    QCOM_FW_UPDATE_PARTITION_TYPE_EFS,
    QCOM_FW_UPDATE_PARTITION_TYPE_MAX


}QCOM_FW_UPDATE_FW_PARITION_TYPE;

/**
 * This struct is used to keep track 
 * of a EFI_HANDLE corresponding to 
 * a given partition on the system.
 */
typedef struct
{
    QCOM_FW_UPDATE_FW_PARTITION     Part; //Enum for the partition.
    EFI_HANDLE                      Handle; //Handle for partition
    QCOM_FW_UPDATE_FW_PARITION_TYPE Type; //Type of parition
    EFI_GUID                        PartType; //Partition type GUID
    EFI_GUID                        VendorID; //GUID to distinguish eMMC physical partiton.

}QCOM_FW_UPDATE_PARTITION_HANDLE;

/**
 * This struct holds partition info 
 */
typedef struct
{
    UINT64  PartSize;
    UINT32  BlockSize;
}PARTITION_INFO, *PPARTITION_INFO;

#pragma pack(push)
#pragma pack(1)
typedef struct{
    UINT8   Type;
    UINT8   SubType;
    UINT16  StructLength;
    UINT32  PartNumber;
    UINT64  StartLBA;
    UINT64  PartSize;
    UINT8   PartSignature[16];
    UINT8   PartFormat;
    UINT8   SignatureType;

}QCOM_FW_UPDATE_HARD_DRIVE_DEVICE_PATH;

typedef struct{
    UINT8   Type;
    UINT8   SubType;
    UINT16  StructLength;
    EFI_GUID  VendorGuid;

}QCOM_FW_UPDATE_VENDOR_DEVICE_PATH;


typedef struct{
    UINT8   Type;
    UINT8   SubType;
    UINT16  StructLength;
}QCOM_FW_UPDATE_DEVICE_PATH;

#pragma pack(pop)

#define TYPE_MEDIA_DEVICE_PATH 4
#define SUBTYPE_HARD_DRIVE 1

#define TYPE_HARDWARE_DEVICE_PATH 1
#define SUBTYPE_VENDOR 4

#define TYPE_END_DEVICE_PATH 0x7F


/**
 * Function to get a EMMC type guid by a given 
 * emmc partition type enum 
 *  
 * @param EmmcType      - Emmc partition type enum
 * @Param EmmcTypeGuid  - Emmc type GUID
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetEmmcPartitionTypeGuid(
    IN  FWENTRY_EMMC_PARTITION_TYPE  EmmcType,
    OUT EFI_GUID                    *EmmcTypeGuid
    );

/**
 * Function to get  partition handle
 * 
 * @param PartHandle    - partition handle
 * @param EmmcType      - Emmc partition type
 * @Param PartTypeGuid  - Partition type GUID
 * @Param PartitionName - Partition name
 * @Param Protocol      - Protocol
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetPartitionHandle(
  IN OUT EFI_HANDLE                       *PartHandle,
    IN FWENTRY_EMMC_PARTITION_TYPE EmmcType,
    IN EFI_GUID                    *PartTypeGuid,
    IN CHAR16                      *PartitionName,
    IN EFI_GUID                    *Protocol
    );

/**
 * Locates instances of a given
 *  protocol and adds them to the
 *  handle registry, if applicable.
 *  
 * @param Protocol - Protocol to locate. 
 * @return EFI_STATUS 
 */
EFI_STATUS
AddHandlesForProtocol(
    IN EFI_GUID *Protocol
    );
   


/**
 * Function iterates through all
 *  handles to EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
 *  and EFI_BLOCK_IO_PROTOCOL and fills
 *  in the HandleRegistry.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
UpdateHandleRegistry(
    IN EFI_SYSTEM_TABLE  *SystemTable
    );



/**
 *  
 * Returns the Handle for the 
 * given partition from the HandleRegistry. 
 * 
 * 
 * @param Part - Partition Enum, the partition whose 
 *          handle we want. 
 * @param Handle - Pointer to the output Handle.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetHandleForPartition(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    OUT EFI_HANDLE                  *Handle
    );


/**
 * Looks up the Read function 
 *  for the appropriate partition 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Read() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetReadFunction(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    );

/**
 * Looks up the Write function 
 *  for the appropriate partition. 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Write() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetWriteFunction(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    );



/**
 * Looks up the Read function 
 *  for the appropriate partition 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Read() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetReadFunction2(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    );

/**
 * Looks up the Write function 
 *  for the appropriate partition. 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Write() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetWriteFunction2(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    );


/**
 * This function reads partition info into a buffer.
 * 
 * 
 * @param SrcPart - Enum for the source partition
 * @param PartInfo - Buffer to store partition info
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
ReadPartitionInfo(
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT PPARTITION_INFO             PartInfo
    );



/**
 * Reads block data from a partition.
 *
 * @param SrcPart - Enum for the source partition
 * @param Offset  - Logical block address to read from.
 * @param Length  - Number of bytes to read. Must be
 *                  aligned to block size.
 * @param Buffer  - Buffer to store the data. 
 *
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS
ReadPartitionBlocks(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN UINT32                       Offset,
    IN OUT UINT32                   *Length,
    OUT VOID                        **Buffer
    );



/**
 * Writes block data to a partition.
 *
 * @param SrcPart - Enum for the source partition
 * @param Offset  - Logical block address to write to.
 * @param Length  - Number of bytes to write. Must be
 *                  aligned to block size.
 * @param Buffer  - Buffer to store the data.
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS
WritePartitionBlocks(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN UINT32                       Offset,
    IN OUT UINT32                   *Length,
    IN VOID                         **Buffer
    );

/**
 * Given a FAT directory path, retrieve the names of all files  
 * that immediately reside under specified path. This is not a 
 * recursive operation so subfolders are not pinged.
 * 
 * @param Part - Enumeration of FAT partition handle. 
 * @param FwFile - Enumeration of directory path. 
 * @param Path - Upon return, store the unicode string of 
 *             directory path
 * @param Subfolder - Upon return, store the bottom-level 
 *                subfolder name of specified directory path,
 *                e.g. if path is "\DPP\QCOM\", then return
 *                "QCOM".
 * @param FileList - Upon return, a pointer to a list of unicode 
 *                 Null-terminated file names.
 * @param FileCount - Upon return, the number of files. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
ReadFatFileList(
    IN QCOM_FW_UPDATE_FW_PARTITION  Part,
    IN QCOM_FW_UPDATE_FW_FILE       FwFile,
    OUT CHAR16                      **Path,
    OUT CHAR16                      **Subfolder,
    OUT CHAR16                      ***FileList,
    OUT UINTN                       *FileCount
    );

/**
 * Given a FW File Guid, returns whether this belongs to 
 * the set of valid Bootloader or ACPI file GUIDs possible 
 * in a payload
 * 
 * @param FileGuid  - Enumeration of FWFile GUIDs. 
 * @param ValidGuid - return whether the FileGuid is a valid 
 *            bootloader GUID or not
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
isValidBootLoaderGuid(
  IN const EFI_GUID *FileGuid,
  OUT BOOLEAN *ValidGuid
  );


#endif
