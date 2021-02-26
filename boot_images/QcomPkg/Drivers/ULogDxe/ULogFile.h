/** @file ULogFile.h

 ULog File logging interface.

 Copyright (c) 2013-2015, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

 **/

/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/15   sj       Updated Appending across reboots
 05/22/15   rli      Merged with UEFI.3.1 (config API, LOGFS)
 10/27/14   vpopuri  Added check for disabling file logging via BDS Menu 
 08/18/14   vpopuri  Use EFIESP instead of fs2:
 08/12/14   vpopuri  Updated variables to use UEFI types
 07/20/14   vpopuri  Added FlushFileBuffer to handle delayed flushes
 06/20/14   vpopuri  Added utility functions and ULOG_MAX_FILE_SIZE
 11/14/13   rli      Initial revision.

 =============================================================================*/

#ifndef ULOGFILE_H
#define ULOGFILE_H

#include <Library/EfiFileLib.h>

// Default values for log files
#define ULOGFILE_DEFAULT_MAX_SIZE 0x20000ULL       // Maximum log file size, after which, writes wrap around to the top.
#define ULOGFILE_DEFAULT_DIR "LOGFS:\\"           // Log files are placed in ULOGFILE_DIR:\ULOGFILE_DIR\<Name>.ULOGFILE_EXTENSION
#define ULOGFILE_EXTENSION ".txt"

// Separators
#define ULOGFILE_OVERLAP_SEPARATOR_PREFIX    "-*-LOG_OVERLAP-*-BOOT="   // Indicates an overlap - We suffix it with the CurrBootCount as well



// Boot counter info
#define ULOGFILE_BOOTCOUNT_SIZE              18                  // 64-bit counter = 16 HEX digits, + "0x"
#define ULOGFILE_BOOTCOUNTFILE_NAME          "BootCountFile.txt"
#define ULOGFILE_METADATAFILE_NAME           ".meta"    //This will be postfixed to the LogName -> Which means File will look like : PmicLog.meta
#define ULOGFILE_DEFAULT_BOOTCOUNT_SEPARATOR '\t'               // Separate the boot count from the temporary time stamp
#define ULOGFILE_TIMESTAMPLENGTH             18                 //  64-bit timestamp = 16 HEX digits, + "0x"



// Delayed Flushing
#define ULOGFILE_MAX_UNFLUSHED_MESSAGES 0   // How full the buffer should be (Messages since last flush) before flushing

/**
 * The ALTernate transport write for printf-format messages, for everything except multipart messages
 * Decodes the message and sends it to ULogFile_Write.
 *
 * @param h: A ULog handle to the enabled ULog
 * @param firstMsgCount: Length of the first array
 * @param firstMsgContent: The first array to write
 * @param secondMsgCount: Length of the second array
 * @param secondMsgContent: The second array to write
 * @param interfaceType: Legacy, not used.
 * @return DAL_SUCCESS if no problems encountered. See ULogFile_Write for errors.
 */
ULogResult ULog_Alt_AsciiPrintfWrite(ULogHandle h, UINT32 firstMsgCount, CONST CHAR8 * firstMsgContent,
                                     UINT32 secondMsgCount, CONST CHAR8 * secondMsgContent,
                                     ULOG_INTERFACE_TYPE interfaceType);

/**
 * @brief Check the status of UEFI File Logging
 *
 * @return TRUE if file logging is enabled, FALSE if disabled.
 *
 */
BOOLEAN ULogFile_CheckFileLoggingStatus(void);

/**
 * Flush the file buffer for the provided file
 *
 * @param h: A pointer to the Alternate ULog whose file buffer needs to be flushed
 * @return  EFI_SUCCESS if flushing was successful
 *
 */
EFI_STATUS ULogFile_FlushFileBuffer(ULogHandle *h);

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
 * If the log doesn't exist in the ULog list, a ULog entry is created, and an empty file is opened
 * on disk.
 *
 * @param h: The handle to the opened ULog will be returned here
 * @param _FileName: The name of the log to open/create
 * @param FileSize: The size (bytes) of the log file. Actual log size is min(FileSize, ULOG_MAX_FILE_SIZE)
 *                  A file size of 0 prints out one message at the top of the file, and overwrites this
 *                  message for each successive log request.
 * @return  DAL_SUCCESS upon successful open
 *          ULOG_ERR_* error on ULog core function failures
 *          DAL_ERROR for all other errors
 */
ULogResult ULogFile_Open(ULogHandle * h, CONST CHAR8 *_FileName, UINT32 FileSize);

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

/**
 * Create a file system path from a log name, in directory ULOGFILE_DIR with extension ULOGFILE_EXTENSION
 *
 * @param LogName: The name of the log managed by the ULog system
 * @param FilePath: The file system path to the log file is returned here. Caller is responsible
 *                  for allocation.
 * @return  EFI_SUCCESS on successful creation
 *          ULOG_ERR_* for ULog core function failures
 *          DAL_ERROR for other errors
 */
ULogResult ULogFile_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath);

/**
 * @brief Write from a buffer to a ULog file
 *
 * Implements the ALTernate write method for persistent ULogs, accessed by ULOG_CORE_VTABLE.write().
 * The requested message is written to the log file associated with the provided handle. If the
 * provided handle does not have a corresponding log entry in the system, it is treated as an error.
 *
 * If the current write exceeds the maximum file size limit, the following write starts from the top,
 * after adding an overlap indicator. All subsequent writes start from the top of the file.
 *
 * The expected name for the file on disk is as in ULogFile_Open.
 *
 * @param h: The handle to the ULog where the write will occur
 * @param buffer: The string to be output to the file
 * @return  DAL_SUCCESS if no errors occur
 *          ULOG_ERR_* if a ULog core function failure occurs
 *          DAL_ERROR for all other errors
 */
ULogResult ULogFile_Write(ULogHandle *h, CHAR8 *buffer);

#endif /* ULOGFILE_H */