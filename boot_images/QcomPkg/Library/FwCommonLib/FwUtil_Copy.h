/** @file FwUtil_Copy.h
   
  Header file for copy functions for Firmware update.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/05/07     rs          Added support for decryption of Boot Firmware
 2011/10/12     jthompso    Initial version

=============================================================================*/

#ifndef __QCOM_FW_UTIL_COPY_H__
#define __QCOM_FW_UTIL_COPY_H__


#include "FwHandle_Partition.h"
#define DEFAULT_COPY_LOOP_CNT 2


/**
 * This struct holds the arguments 
 * for a call to CopyFwFile. 
 *  
 * This allows for the creation of a list 
 * of calls to CopyFwFile. 
 *  
 */
typedef struct
{
    QCOM_FW_UPDATE_FW_FILE FwFile;
    QCOM_FW_UPDATE_FW_PARTITION SrcPart;
    QCOM_FW_UPDATE_FW_PARTITION DestPart;
    QCOM_FW_UPDATE_FW_FILE FwFileDest;

}QCOM_FW_UPDATE_COPYFWFILE_ARGUMENTS;

/**
 * Helper to copy fw files, ensure two files are identical
 *  
 * 
 * @param FwFile           - Enum for the desired file
 * @param SrcPart          - Enum for the source partition
 * @param DestPart         - Enum for the destination partition 
 * @param OutputFileName   - Enum for the output name of the 
 *                           file
 * 
 * @return 
 * TRUE                    - files are identical  
 * FALSE                   - files are not identical 
 */
BOOLEAN
CompareFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFileSrc,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    );


/**
 * Helper to CopyFwFile, ensures input 
 * combination is valid. 
 *  
 * This function blocks invalid copy 
 *  scenarios (i.e., FAT to FV, a
 *  case we do not use) from happening.
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * 
 * @return BOOLEAN 
 */
BOOLEAN
CopyFwFileValidInputs(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    );



/**
 * This function copies a firmware file (DPP, EFS, SBL, etc), 
 * from the given source to the given destination. 
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
CopyFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    );
/**
 * Helper to write a buffer in DPP and  ensure the write is successful
 *  
 * @param Srcbuffer - the source buffer
 * @param bufSzie - buffer size
 * @param FwFileDest - the dest file name
*  @param[in]  Type  - An unicode string specifying provision
								      data type, e.g. L"QCOM" or L"OEM".
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
WriteBuffertoDPPAndCheck(
    IN VOID*  Srcbuffer,
    IN INTN   bufSize,
    IN UINT16 *FwFileDest,
    IN CHAR16 *Type,    
    IN INTN   trycnt
    );

/**
 * Helper to back up a buffer in FAT system and  ensure the back up is successful
 *  
 * @param Srcbuffer - the source buffer
 * @param bufSize - buffer size
 * @param FwFileDest - the dest file name
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
WriteBuffertoFatAndCheck(
    IN VOID* Srcbuffer,
    IN INTN bufSize,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest,
    IN INTN trycnt
    );

/**
 * Helper to compare fw files and check if the two files are 
 * identical 
 *  
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
CopyFwFileAndCheck(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest,
    IN INTN trycnt
    );

/**
 *Function Name:DeleteFwFile()
 *
 *Parameters:FwFile Enum for File to be Deleted 
 *SrcPart Enum for partition from where the file has to be deleted 
 *
 *Description:
 *
 *Returns:
 *
 **/

EFI_STATUS
DeleteFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION Partition
    );

/**
 * This function detects the presence of a firmware file at the 
 * given destination.
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param Partition - Enum for the destination partition 
 * @param FoundFile - Indicate whether specified file is found 
 *                  or not
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FindFwFile(
    IN  QCOM_FW_UPDATE_FW_FILE FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION Partition,
    OUT BOOLEAN *FoundFile
    );
/**
 * This function copies a firmware file (DPP, EFS, SBL, etc), 
 * from the given source to the given destination. 
 *  
 *  
 * @param SrcPart - Enum for the source partition 
 * @param SrcFileName - Unicode Null-terminated source file name
 * @param DestPart - Enum for the destination partition 
 * @param DestFileName - Unicode Null-terminated destination 
 *                     file name
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CopyFwFile2(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN CHAR16                       *SrcFileName,
    IN QCOM_FW_UPDATE_FW_PARTITION  DestPart,
    IN CHAR16                       *DestFileName
    );


/**
 * This function delete a firmware file (DPP, EFS, SBL, etc)
 *  
 *  
 * @param FileName - Unicode null-terminated file path
 * @param Partition - Enum of the partition
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
DeleteFwFile2(
    IN CHAR16                       *FileName,
    IN QCOM_FW_UPDATE_FW_PARTITION  Partition
    );

/**
 * This function detects the presence of a firmware file at the 
 * given destination.
 * 
 * 
 * @param FileName - Unicode null-terminated file path
 * @param Partition - Enum for the destination partition 
 * @param FoundFile - Indicate whether specified file is found 
 *                  or not
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FindFwFile2(
    IN  CHAR16                      *FileName,
    IN  QCOM_FW_UPDATE_FW_PARTITION Partition,
    OUT BOOLEAN                     *FoundFile
    );

/**
 * Helper to compare decrypted FW files, checks if two files are
 * identical 
 *  
 * 
 * @param FwFile              - Enum for the desired file
 * @param DecryptedBuffer     - Source Buffer to copy from 
 * @param DecryptedBufferSize - Size of the source buffer 
 * @param DestPart            - Enum for the destination 
 *  						    partition
 * @param OutputFileName      - Enum for the output name of the 
 *                              file
 * 
 * @return 
 * TRUE                       - files are identical 
 * FALSE                      - files are not identical    
 */
BOOLEAN
CompareDecryptFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFileSrc,
    IN VOID *DecryptedBuffer,
	IN UINTN DecryptedBufferSize,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    );

#endif

