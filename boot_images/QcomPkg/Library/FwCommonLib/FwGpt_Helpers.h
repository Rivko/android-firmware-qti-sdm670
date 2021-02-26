/** @file FwGptHelpers.h
   
  Header for FwGptHelpers.h

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/04/15     mic         Added 64bit firmware update support
 2012/10/10     jd          Handle GPT switch based on firmware entry list  
 2012/10/08     mic         renamed gpt functions
 2011/10/12     hozefak     Initial version

=============================================================================*/

#ifndef __QCOM_FW_GPT_HELPERS_H__
#define __QCOM_FW_GPT_HELPERS_H__

#include "FwLinkedList.h"
#include <Library/FwUpdateLib/FwPayloadEntry.h>

typedef enum{
    ORIGINAL_TABLE,
    SWITCHED_TABLE
}GPT_TABLE_STATE;

typedef enum{
    BACKUP_GPT_TABLE,
    PRIMARY_GPT_TABLE
}GPT_TABLE_TYPE;


/**
 * Modify Backup GPT such that it now points to the backup bootloaders
 *
 * @param NONE
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS ModBckUpGPTforBckUp(VOID) ;

/**
 * Modify the Backup GPT by making it similar to the Primary GPT table
 *
 * @param NONE
 *
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS ModBckUpGPTforPrimary(VOID) ;

/**
 * Invalidate ALL Primary GPT Tables i.e USER BOOT and GPP
 * Partitions
 *
 * @param InvalidateBootGpt - Invalidate BOOT partition primary 
 *                          GPT.
 * @param InvalidateGppGpt - Invalidate GPP partition primary 
 *                         GPT.
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS InvalidateAllPrimaryGPT(
  IN BOOLEAN InvalidateBootGpt,
  IN BOOLEAN InvalidateGppGpt
  );

/**
 * Invalidate ALL Backup GPT Tables
 *
 * @param InvalidateBootGpt - Invalidate BOOT partition primary 
 *                          GPT.
 * @param InvalidateGppGpt - Invalidate GPP partition primary 
 *                         GPT.
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS InvalidateAllBackUpGPT(
   IN BOOLEAN InvalidateBootGpt,
   IN BOOLEAN InvalidateGppGpt
   );

/**
 * Validate the primary GPT 
 *
 * @param NONE
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS ValidatePrimaryGPT(VOID);


/**
 * Copy Boot Partition Table and GPP Partition Table to FAT16.bin in files BPT.TBL GPP.TBL
 *
 * @param BackupBootGpt : Indicate whether or not backup BOOT 
 *                      partition GPT
 * @param BackupGppGpt  : Indicate whether or not backup GPP 
 *                      partition GPT
 *
 * @return EFI _STATUS
 *
 */

EFI_STATUS CopyPartitionTabletoFat16(
   IN BOOLEAN BackupBootGpt,
   IN BOOLEAN BackupGppGpt
   );

/**
 * Modifies Boot Partition and GPP partition tables to point to the backup tables 
 * Also modifies and fills in the right CRC 
 * 
 * @param TABLE_TYPE
 * 
 * @param RestoreBootGpt 
 *  
 * @param RestoreGppGpt 
 * 
 * @return EFI_STATUS 
 */


EFI_STATUS RestoreAllGPTTablesFromFat(
  IN GPT_TABLE_TYPE TABLE_TYPE,
  IN BOOLEAN RestoreBootGpt,
  IN BOOLEAN RestoreGppGpt
    );


/**
 * Modifies all gpt partition tables (BOOT partition and GPP)
 *
 * @param List - Firmware entry List 
 * @param ModifyBootGpt - Indicate whether or not modify BOOT 
 *                      GPT
 * @param ModifyGppGpt - Indicate whether or not modify GPP GPT 
 *
 * @return EFI_STATUS
 *
 */


EFI_STATUS ModifyAllBackupGptTablesForFailSafeUpdate(
    IN  FW_LINKED_LIST_TYPE *List,
    IN  BOOLEAN ModifyBootGpt,
    IN  BOOLEAN ModifyGppGpt
    );

/**
 * Compare GPT with the table in FAT partition and check if it the swapped table 
 * or it is the primary original table 
 *
 * @param TableType       :   Type of GPT Table 
 * @param Emmc_GUID       :   Vendor GUID whose GPT has to be 
 *                        compared 
 * @param gptTableState   :   returns if GPT table is in the
 *                            original or switched state
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS CmpGPTTable(
    IN GPT_TABLE_TYPE TableType,
    IN EFI_GUID Emmc_GUID,
    OUT GPT_TABLE_STATE* gptTableState
);


/**
 * Reads the partition table and returns Filesize and FileBuffer
 * with data from the GPT tables. In case FileBuffer is NULL
 * will return only the FileSize by reading the GPT header
 *
 * @param Memory_GUID     : Vendor GUID for which GPT tables
 *                          have to be read
 * @param FileBuffer      : Pointer to buffer where GPT should
 *                          be read, if FileSize Function will
 *                          return only the FileSize
 * @param FileSize        : Size of the buffer to be read, i.e
 *                          size of the partition table
 * @param TableType
 * 
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS ReadGPTTable(
    IN  EFI_GUID Memory_GUID,
    OUT VOID **FileBuffer,
    OUT UINTN *FileSize,
    IN  GPT_TABLE_TYPE TableType
    ) ;

/**
 * Invalidate the Primary GPT Table
 *
 * @param Emmc_GUID
 *
 * @return EFI_STATUS
 *
 */


EFI_STATUS InvalidatePrimaryGPT(
        IN EFI_GUID Emmc_GUID
        );

/**
 * Invalidate the Backup GPT Table
 *
 * @param Emmc_GUID
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS InvalidateBackUpGPT(
        IN EFI_GUID Emmc_GUID
        );

/**
 * This function goes through the firmware entry list and checks
 * if backup GPT of BOOT and GPP1 partitions needs to be 
 * switched for fail-safe update. 
 *  
 * @param List          :  Firmware entry list 
 * @param SwitchBootGpt :  Upon return, indicates if BOOT backup
 *                      GPT needs to be switched
 * @param SwitchGppGpt  :  Upon return, indicates if GPP backup 
 *                      GPT needs to be switched
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS GetAllSwitchGptRequestsFromFwEntryList(
  IN FW_LINKED_LIST_TYPE *List,
  OUT BOOLEAN *SwitchBootGpt,
  OUT BOOLEAN *SwitchGppGpt
  );

#endif
