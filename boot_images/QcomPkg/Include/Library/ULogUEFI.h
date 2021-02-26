/** @file ULogUEFI.h

 Public interface for UEFI-specific ULog code

  Copyright (c) 2014-2015 by Qualcomm Technologies Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

 **/

/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/22/15   rli     Added ULogFile config API
 08/14/14   vpopuri Updated variables to use UEFI types
 07/25/14   vpopuri Initial revision
 =============================================================================*/
#ifndef ULOGUEFI_H
#define ULOGUEFI_H

#include "ULog.h"

typedef struct {
  CHAR8 separator;          // Separator between bootcount, timestamp, and message
  BOOLEAN PrintTimestamp;   // Used to enable or disable bootcount+timestamp printing
  BOOLEAN TrimNewline;      // Used to enable or disable replacing a terminating \n\n with \n
} ULOG_CONFIG_TYPE;

/**
 * @brief Open a ULog for alternate logging, creating the entry if necessary.
 *
 * Create a ULog entry for the linked list, then open/create a file on disk for this log. The file
 * name is of the form  ULOGFILE_DIR<fileName>.ULOGFILE_EXTENSION. If the provided log name is too
 * long, it will be truncated to ULOG_MAX_NAME_SIZE characters.
 *
 * If the file is already managed by the system, the handle to the log is returned. No checking is
 * done to see if a corresponding file exists on disk.
 *
 * If the file doesn't exist in the ULog list, a ULog entry is created, and an empty file is opened
 * on disk.
 *
 * @param h: The handle to the opened ULog will be returned here
 * @param _FileName: The name of the log to open/create
 * @param FileSize: The size (bytes) of the log's buffer. A size of 0 will leave the buffer unallocated
 * @return  DAL_SUCCESS upon successful open
 *          ULOG_ERR_* error on ULog core function failures
 *          DAL_ERROR for all other errors
 */
ULogResult ULogFile_Open( ULogHandle * h, CONST CHAR8 *fileName,
                          UINT32 FileSize );

/**
 * <!-- ULogFile_SetConfig -->
 * @brief Modify the file logging settings
 *
 * @param h: Pointer to the ULog handle to modify
 * @param settings: The new settings to apply
 *
 * @return EFI_SUCCESS if all input settings are valid
 *         ULOG_ERR_XX otherwise
 */
ULogResult ULogFile_SetConfig(ULogHandle *h, CONST ULOG_CONFIG_TYPE *settings);

/**
 * <!-- ULogFile_GetConfig -->
 * @brief Return the file logging settings
 *
 * @param h: Pointer to the handle for the ULog entry in questin
 * @param settings: Pointer to the caller-alloc'd buffer in which to return the settings
 *
 * @return EFI_SUCCESS if settings could be copied successfully
 *         ULOG_ERR_XX otherwise
 */
ULogResult ULogFile_GetConfig(ULogHandle *h, ULOG_CONFIG_TYPE *settings);

#endif /* ULOGUEFI_H  */
