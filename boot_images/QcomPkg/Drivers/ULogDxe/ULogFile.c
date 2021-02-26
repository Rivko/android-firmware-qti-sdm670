/** @file ULogFile.c
 Implementation of UEFI logging to file

 Copyright (c) 2014-2015, Qualcomm Technologies Inc. All rights reserved.

 **/

/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/17/15   rli      Added NULL-check in ULogFile_IncrementBootCount to fix KW
 10/13/15   rli      Fixed Debug Print for EfiSeek error
 09/17/15   rli      Fixed cleaning left-over after wraparound
 09/10/15   sj       64 Bit Compilation errors, Error Checks
 09/08/15   sj       Updated appending across reboots, Fix in Overlap Logic
 05/22/15   rli      Merged with UEFI.3.1 (config API, partition, max size)
 10/27/14   vpopuri  Added check for disabling file logging via BDS Menu
 08/13/14   vpopuri  Updated variables to use UEFI types
 08/06/14   rli      Updated to use static buffer for BootCountStr
 07/30/14   vpopuri  Added ULogFile_Open, Write, and supporting functions
 07/23/14   vpopuri  Added delayed flushing and automatic boot counter
 11/12/13   rli      Initial revision


 =============================================================================*/
#define TARGET_UEFI

#include <Library/DebugLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseLib.h>
#include <Library/ULogFront.h>
#include <Library/ULog.h>
#include <Library/ULogLib/ULogCoreInternal.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include "DALSys.h"
#include "ULogFile.h"
#include "CoreString.h"
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include "CoreVerify.h"

STATIC BOOLEAN sBootCountNeedsUpdate = TRUE;
STATIC UINTN sCurrentBootCount;    // Updated on the first call to ULogFile_Open in the current boot cycle
STATIC CHAR8 *sBootCountStr = NULL;
STATIC UINTN sBootCountLen = 0;

STATIC UINTN sOverlapSeparatorLen = 0;
STATIC CHAR8 *sOverlapSeparatorString;

STATIC CHAR8 *sLogDirectory;  // Target-specific log directory
STATIC UINTN sMaxLogFileSize; // Target-specific maximum log size
STATIC UINTN sSizeOfLogTransactionWrite; // Size of LogTransactionWrite - Used as size when writing into MetaData

extern EFI_GUID gQcomTokenSpaceGuid;
extern EFI_RUNTIME_SERVICES *gRT;

/** 
 * <!-- ULog_Alt_AsciiPrintfWrite -->
 *
 * @brief The alternate transport write to printf for everything except
 * multipart messages.
 *
 * Decodes the message and sends it out to printf.
 *
 * @param h : A ULog handle for the open file.
 * @param firstMsgCount : how many bytes of data in the first array of content
 * @param firstMsgContent : The data to put into the log.
 * @param secondMsgCount : how many bytes of data in the second array of content
 * @param secondMsgContent : The remaining data to put into the log.
 * @param interfaceType : Not used, this is a legacy argument.
 *
 * @return DAL_SUCCESS if no problems encountered.
 */
ULogResult ULog_Alt_AsciiPrintfWrite(ULogHandle h, UINT32 firstMsgCount, CONST CHAR8 * firstMsgContent,
    UINT32 secondMsgCount, CONST CHAR8 * secondMsgContent, ULOG_INTERFACE_TYPE interfaceType)
{

  CHAR8 print_buffer[512];
  SetMem(print_buffer, 512, '\0');

  ULOG_READ_TO_ASCII_DATA parse_data;

  print_buffer[0] = 0; //NULL terminate
  parse_data.msgpart1 = (CHAR8 *) firstMsgContent;
  parse_data.msgpart2 = (CHAR8 *) secondMsgContent;
  parse_data.part1_size = firstMsgCount;
  parse_data.part2_size = secondMsgCount;
  parse_data.outputsz = print_buffer;
  parse_data.outputsz_size = 512;
  parse_data.outOffset = 0;
  parse_data.environment = ULOG_PARSE_DATA_STREAM; // tells parser not to follow %m messages into next logs that don't exist yet.

  ULogParsers_FormatParseData(&parse_data, 0);
  ULogResult result = ULogFile_Write(&h, parse_data.outputsz);

  return result;
}

//Alternate transport function table for Printf Ulog Output
static ULOG_CORE_VTABLE ULogVtablePrintfOut = { ULog_Alt_AsciiPrintfWrite, NULL, NULL };

/*
 * @brief Update the sLogDirectory and sMaxLogFileSize variables with the values for the current target.
 *
 * Default to EFIESP:\LOGs\ and 128kB
 *
 * @return EFI_SUCCESS upon successful set, EFI_XXXXXX upon error
 */
EFI_STATUS ULogFile_SetLogDirectoryAndSize()
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  CHAR8 *LogDir = ""; // GetConfigString doesn't accept NULL for this
  UINTN BufSize = 0;

  // Get the length of the log directory string
  Status = GetConfigString((CHAR8 *) "SpecialLogPartition", LogDir, &BufSize);

  if (Status == EFI_BUFFER_TOO_SMALL) {
    // A SpecialLogPartition was specified
    LogDir = AllocatePool(BufSize);
    if (LogDir == NULL) {
      DEBUG((EFI_D_WARN, "ULogFile: Couldn't allocate buffer \n"));
      return EFI_OUT_OF_RESOURCES;
    }

    Status = GetConfigString((CHAR8 *) "SpecialLogPartition", LogDir, &BufSize);
    if (Status == EFI_SUCCESS) {
      // Use the SpecialLogPartition to log
      sLogDirectory = LogDir;
    } else {
      // couldn't read for some reason, use the default value
      DEBUG(
          (EFI_D_WARN, "ULogFile: Couldn't read the special log partition, Error %r. Using default directory\n", Status));
      sLogDirectory = ULOGFILE_DEFAULT_DIR;
    }
  } else {
    // A valid SpecialLogPartition was not specified, default to EFIESP:\LOGs
    sLogDirectory = ULOGFILE_DEFAULT_DIR;
  }

  // Now do the same for the max log size
  UINT32 Value = 0;
  Status = GetConfigValue("MaxLogFileSize", &Value);

  if ((Status == EFI_SUCCESS) && (Value > 0)) {
    sMaxLogFileSize = Value;
  } else {
    sMaxLogFileSize = ULOGFILE_DEFAULT_MAX_SIZE;
  }

  DEBUG(
      (EFI_D_WARN, "ULogFile: Max log file size is 0x%x\n" "\tLogs will be stored in %a\n", sMaxLogFileSize, sLogDirectory));

  return EFI_SUCCESS;
}

/*
 * @brief Check the status of UEFI File Logging
 *
 * @return TRUE if enabled, FALSE if disabled.
 */
BOOLEAN ULogFile_CheckFileLoggingStatus(VOID)
{
  BOOLEAN FileLoggingIsEnabled = FALSE;
  EFI_STATUS Status = EFI_UNSUPPORTED;
  UINTN VarSize = 0;

  VarSize = sizeof(FileLoggingIsEnabled);

  Status = gRT->GetVariable(L"EnableFileLogging", &gQcomTokenSpaceGuid, NULL, &VarSize,
                            (UINT8 *) &FileLoggingIsEnabled);
  if (Status != EFI_SUCCESS) {
    // Variable was not found, default to disabled
    FileLoggingIsEnabled = FALSE;
  }

  return FileLoggingIsEnabled;
}

/**
 * Create the absolute timestamp
 *
 * The Absolute TimeStamp is a boot counter that is incremented on the first call to ULogFile_Open in the current boot.
 * The boot count is a 16-digit HEX number that is stored in ULOGFILE_BOOTCOUNTFILE.
 *
 * @param BootCountStr: A pointer to the caller-allocated buffer that will hold the current boot count
 * @return  EFI_SUCCESS if the boot counter file was opened and read successfully
 *          EFI_INVALID_PARAMETER if the provided buffer is NULL
 *          EFI_BAD_BUFFER_SIZE if the provided buffer is too small
 */
EFI_STATUS ULogFile_AbsoluteTimeStamp(CHAR8 *BootCountStr)
{
  if (BootCountStr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EFI_STATUS Result = EFI_SUCCESS;
  UINTN BufferSize = ULOGFILE_BOOTCOUNT_SIZE + 1 + 1; // bootcount + separator + '\0'
  UINTN BootCount = sCurrentBootCount;
  core_snprintf(BootCountStr, BufferSize, "0x%X ", BootCount);
  if (AsciiStrLen(BootCountStr) > ULOGFILE_BOOTCOUNT_SIZE + 1) {
    DEBUG(( EFI_D_WARN, "ULogFile_AbsoluteTimeStamp: Buffer Overflow \n" ));
    BootCountStr[ULOGFILE_BOOTCOUNT_SIZE+1] = '\0';
    Result = EFI_BAD_BUFFER_SIZE;
  }

  return Result;
}

/**
 * Increment the boot count in the bootcountfile, if necessary
 * @return  EFI_SUCCESS if boot count was updated successfully
 *          Errors from EfiRead, EfiWrite, or EfiFlush for read/write errors
 *          DAL_ERROR if the file couldn't be opened
 */
EFI_STATUS ULogFile_IncrementBootCount()
{
  EFI_STATUS Result = EFI_UNSUPPORTED;

  if (sBootCountNeedsUpdate == FALSE) {
    Result = EFI_SUCCESS;
  } else {
    // Obtain the current bootcount
    sBootCountStr = AllocatePool( ULOGFILE_BOOTCOUNT_SIZE + 1 + 1); // Bootcount + 1-char separator + '\0'
    while (sBootCountStr) {
      UINTN BufLen = ULOGFILE_BOOTCOUNT_SIZE;
      UINTN BootCount = 0;

      CHAR8 *BootCountFilePath = AllocatePool(
          AsciiStrLen(sLogDirectory) + AsciiStrLen(ULOGFILE_BOOTCOUNTFILE_NAME) + 1);
      if (BootCountFilePath == NULL) {
        DEBUG((EFI_D_WARN, "ULogFile_IncrementBootCount: Couldn't allocate logdir string \n"));
        break;
      }
      SetMem((void *) BootCountFilePath, AsciiStrLen(BootCountFilePath), '\0');
      AsciiStrCpy(BootCountFilePath, sLogDirectory);
      AsciiStrCat(BootCountFilePath, ULOGFILE_BOOTCOUNTFILE_NAME);

      EFI_OPEN_FILE *BootCountFile = EfiOpen((CHAR8 *) BootCountFilePath,
                                             EFI_FILE_MODE_READ | EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE, 0);

      if (BootCountFile == NULL) {
        DEBUG(( EFI_D_WARN, "ULogFile_IncrementBootCount: Couldn't open boot count file \n" ));
        Result = EFI_NOT_FOUND;
        break;
      }

      Result = EfiRead(BootCountFile, sBootCountStr, &BufLen);
      if (Result == EFI_SUCCESS) {
        // Parse the current boot count
        BootCount = AsciiStrHexToUintn((CONST CHAR8 *) sBootCountStr);  // Returns 0 upon error, i.e. invalid boot count
        // Force NULL termination
        sBootCountStr[ULOGFILE_BOOTCOUNT_SIZE] = '\0';
      } else if (Result != EFI_END_OF_FILE) { // EFI_END_OF_FILE => File is empty, which is fine
        DEBUG(( EFI_D_WARN, "ULogFile_IncrementBootCount: Read failed \n" ));
        break;
      }

      // Increment the boot count to indicate a new boot.
      BootCount++;

      /*
       * Write the bootcount to the start of the file
       */
      core_snprintf(sBootCountStr, (BufLen + 1), "0x%016X", BootCount);

      Result = EfiSeek(BootCountFile, 0, EfiSeekStart);
      if (Result != EFI_SUCCESS) {
        DEBUG(( EFI_D_WARN, "ULogFile_IncrementBootCount: Seek-to-start failed \n" ));
        break;
      }
      BufLen = ULOGFILE_BOOTCOUNT_SIZE;
      Result = EfiWrite(BootCountFile, sBootCountStr, &BufLen);
      if (Result != EFI_SUCCESS) {
        DEBUG(( EFI_D_WARN, "ULogFile_IncrementBootCount: Write failed \n" ));
        EfiClose(BootCountFile);
        break;
      }

      // Flush the file to make sure it's been written
      Result = BootCountFile->FsFileHandle->Flush(BootCountFile->FsFileHandle);
      if (Result != EFI_SUCCESS) {
        DEBUG(( EFI_D_WARN, "ULogFile_IncrementBootCount: Flush failed \n" ));
        EfiClose(BootCountFile);
        break;
      }

      // Unset the bootcount update flag, to prevent every call to Open() from coming in here
      sBootCountNeedsUpdate = FALSE;

      // Store the current boot count, so we don't need to read the file every time we need a time stamp
      sCurrentBootCount = BootCount;

      Result = EfiClose(BootCountFile);
      break;
    }

    // Store the current boot count, so we don't need to read the file every time we need a time stamp
    Result = ULogFile_AbsoluteTimeStamp(sBootCountStr);
    if (sBootCountStr != NULL) {
      sBootCountLen = AsciiStrLen(sBootCountStr);
    }
    // Store length of overlap separator, so we don't need to calculate it every time - The overlap separator now contains the string "sBootCountStr" as well.
    sOverlapSeparatorLen = AsciiStrLen(ULOGFILE_OVERLAP_SEPARATOR_PREFIX) + sBootCountLen + 1; //ULOGFILE_OVERLAP_SEPARATOR_PREFIX  + sBootCountLen +  \n
    
    sOverlapSeparatorString = AllocatePool(sOverlapSeparatorLen + 1 ); // Bootcount  + '\0'
    if (sOverlapSeparatorString == NULL) {
      DEBUG((EFI_D_WARN, "ULogFile_IncrementBootCount: Couldn't allocate sOverlapSeparatorString string \n"));
      return EFI_OUT_OF_RESOURCES;
    }
    
    SetMem((void *) sOverlapSeparatorString, sOverlapSeparatorLen + 1, '\0');
    
    AsciiStrCpy(sOverlapSeparatorString, ULOGFILE_OVERLAP_SEPARATOR_PREFIX);
    if (sBootCountStr != NULL) {
        AsciiStrCat(sOverlapSeparatorString, sBootCountStr);
    }
    AsciiStrCat(sOverlapSeparatorString, "\n");
    //sOverlapSeparatorString = 
  }
  return Result;
}

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
ULogResult ULogFile_SetConfig(ULogHandle *h, CONST ULOG_CONFIG_TYPE *settings){
  ULogResult retval = ULOG_ERR_NOTSUPPORTED;
  if(settings == NULL || h == NULL || *h == NULL){
    DEBUG((EFI_D_WARN, "ULogFile_SetConfig: Received invalid config struct or ULog handle\n"));
    retval = ULOG_ERR_INVALIDPARAMETER;
  }

  else {
    // input validate
    if(settings->separator == '\0'){
      DEBUG((EFI_D_WARN, "ULogFile_SetConfig: Invalid separator\n"));
      retval = ULOG_ERR_INVALIDPARAMETER;
    }
    else {
      ULOG_TYPE *Log = (ULOG_TYPE *)(*h);
      UINT32 nCopied = memcpy_s((void *)(Log->config), sizeof(ULOG_CONFIG_TYPE), (CONST VOID *)settings, sizeof(ULOG_CONFIG_TYPE));
      if(nCopied != sizeof(ULOG_CONFIG_TYPE)){
        DEBUG((EFI_D_WARN, "ULogFile_SetConfig: Error copying config struct. Copied %u bytes instead of %u bytes\n", nCopied, sizeof(ULOG_CONFIG_TYPE)));
        retval = ULOG_ERR_OVERRUN;
      }

      else{
        retval = EFI_SUCCESS;
      }
    }
  }

  return retval;
}

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
ULogResult ULogFile_GetConfig(ULogHandle *h, ULOG_CONFIG_TYPE *settings){
  ULogResult retval = ULOG_ERR_NOTSUPPORTED;
  if (settings == NULL || h == NULL || *h == NULL){
    DEBUG((EFI_D_WARN, "enclosing_method: Received invalid buffer or handle\n"));
    retval = ULOG_ERR_INVALIDPARAMETER;
  }

  else{
    ULOG_TYPE *Log = (ULOG_TYPE *)(*h);
    UINT32 nCopied = memcpy_s((void *)settings, sizeof(ULOG_CONFIG_TYPE), (const void *)(Log->config), sizeof(ULOG_CONFIG_TYPE));
    if (nCopied != sizeof(ULOG_CONFIG_TYPE)){
      DEBUG((EFI_D_WARN, "ULogFile_GetConfig: Error copying settings. Copied %u bytes instead of %u bytes\n", nCopied, sizeof(ULOG_CONFIG_TYPE)));
      retval = ULOG_ERR_OVERRUN;
    }

    else{
      retval = EFI_SUCCESS;
    }
  }

  return retval;
}
/**
 * Flush the file buffer of the provided ULog handle.
 *
 * Flushing is performed when the number of stored messages since the last flush exceeds ULOGFILE_MAX_UNFLUSHED_MESSAGES
 * The number of messages since the last flush is stored in the ULog entry.
 *
 * @param h: The ULog whose log file buffer needs to be flushed
 * @return  EFI_SUCCESS on success, or if the buffer isn't full enough (delayed flushing)
 *          Errors from EFI_FILE.Flush() otherwise.
 */
EFI_STATUS ULogFile_FlushFileBuffer(ULogHandle *h)
{
  if (h == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_FlushFileBuffer: NULL ULog handle \n" ));
    return EFI_INVALID_PARAMETER;
  }
  ULOG_TYPE *Log = (ULOG_TYPE *) (*h);
  EFI_OPEN_FILE *OpenFile = Log->OpenFile;
  EFI_OPEN_FILE *OpenFileMetaData = Log->OpenFileMetaData;  
  EFI_STATUS FlushResult = EFI_SUCCESS;

  if ((OpenFile == NULL) || (OpenFile->FsFileHandle == NULL) || (OpenFile->FsFileHandle->Flush == NULL) 
    || (OpenFileMetaData == NULL) || (OpenFileMetaData->FsFileHandle->Flush == NULL)) {
    DEBUG(( EFI_D_WARN, "ULogFile_FlushFileBuffer: Flushing buffer of a NULL file, FsFileHandle, or Flush() \n" ));
    return EFI_INVALID_PARAMETER;
  }

  UINT32 UnflushedMessages = ++(Log->MessagesSinceFlush);

  if (UnflushedMessages > ULOGFILE_MAX_UNFLUSHED_MESSAGES) {
    FlushResult = OpenFile->FsFileHandle->Flush(OpenFile->FsFileHandle);
    if (FlushResult == EFI_SUCCESS) {
      FlushResult = OpenFileMetaData->FsFileHandle->Flush(OpenFileMetaData->FsFileHandle);
      if (FlushResult != EFI_SUCCESS) {
        DEBUG(( EFI_D_WARN, "ULogFile_FlushFileBuffer: Meta File Flush failed: %r\n", FlushResult ));
      }
    }
    else
    {
      DEBUG(( EFI_D_WARN, "ULogFile_FlushFileBuffer: OpenFile Flush failed: %r\n", FlushResult ));
    }

    if (FlushResult == EFI_SUCCESS) {
      Log->MessagesSinceFlush = 0;
    }
  } 
  return FlushResult;
}


/**
 * <!-- ULogFile_Open -->
 *
 * Open a ULog file on disk, creating it if it doesn't exist
 * The file name is sLogDirectory<log_name>.ULOGFILE_EXTENSION
 *
 * The handle for the log file is stored in the ULog entry to
 * improve performance.
 * @param h: File handle for the log
 * @param _FileName:  Name of the file to open, will be truncated if
 *                    size > ULOG_MAX_NAME_SIZE
 * @param FileSize:  Maximum log file size in bytes
 * @return EFI_SUCCESS upon success,
 *         Appropriate ULOG_ERROR error on failure.
 */

ULogResult ULogFile_Open(ULogHandle * h, CONST CHAR8 *_FileName, UINT32 FileSize)
{
  if (h == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Trying to assign a log to a NULL handle \n" ));
    return ULOG_ERR_INVALIDHANDLE;
  }

  if (_FileName == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Trying to open a NULL FileName \n" ));
    return ULOG_ERR_INVALIDNAME;
  }

  if (strcmp(_FileName, "") == 0) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Received request for Empty file name \n" ));
    return ULOG_ERR_INVALIDNAME;
  }

  // If File Logging is disabled, exit immediately
  if (ULogFile_CheckFileLoggingStatus() == FALSE) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: File Logging is disabled. Not opening %a. \n", _FileName));
    return ULOG_ERR_NOTSUPPORTED;
  }

  // Set the log directory before opening the boot count file
  ULogResult Result = ULOG_ERR_MISC;
  Result = ULogFile_SetLogDirectoryAndSize();
  if (Result != EFI_SUCCESS) {
    DEBUG((EFI_D_WARN, "ULogFile_Open: Couldn't update log directory: %r", Result));
    return ULOG_ERR_MISC;
  }

  // Increment the boot count if this is the first call to ULogFile_Open(), so absolute timestamps will work
  // Do this at the start of the function, so the boot count is incremented even if a crash occurs later in
  // this function.
  Result = ULogFile_IncrementBootCount();
  if (Result != EFI_SUCCESS) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Couldn't update boot count, ERROR %d \n", Result ));
    return ULOG_ERR_MISC;
  }

  // Truncate the file name if it is too long
  CHAR8 *FileName = (CHAR8 *) _FileName;
  if (AsciiStrLen(FileName) > ULOG_MAX_NAME_SIZE) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Truncating file name to %d chars \n", ULOG_MAX_NAME_SIZE ));
    FileName[ULOG_MAX_NAME_SIZE] = '\0';
  }

  if (AsciiStrLen(sLogDirectory) + AsciiStrLen(FileName) > MAX_PATHNAME) {
    DEBUG(
        ( EFI_D_WARN, "ULogFile_Open: The requested path name is too long: Please use a file name shorter than", "%d characters \n", (MAX_PATHNAME - AsciiStrLen(sLogDirectory)) ));
    return ULOG_ERR_INVALIDPARAMETER;
  }

  CHAR8 FilePath[MAX_PATHNAME];
  SetMem(FilePath, MAX_PATHNAME, '\0');
  Result = ULogFile_MakePathFromName(FileName, FilePath);
  if (Result != EFI_SUCCESS) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Coudln't create file path string\n" ));
    return ULOG_ERR_MISC;
  }

  ULogHandle hdl = NULL;
  EFI_OPEN_FILE * OpenFile = NULL;

  //  Check if the file exists in the list of managed logs.
  //  If this is the first access to the ULog Core functions for this boot, then the ULog list will
  //  be uninitialized, and will assert when accessed by HandleGet().
  //  Core initialization takes place in LogCreate, but we manually call ULogCore_Init(), since we
  //  want to skip LogCreate if the log already exists;
  ULogCore_Init();

  hdl = ULogCore_HandleGet((CHAR8 *) FileName);

  if (hdl != NULL) {
    // A log with the requested name is already managed by the system, so we can just return this handle.
    *h = hdl;
    return DAL_SUCCESS;
  }

  else {
    // A log with the requested name isn't managed by the system.
    Result = ULogFront_RealTimeInit(&hdl, FileName, 64, ULOG_MEMORY_LOCAL, ULOG_LOCK_NONE);

    if (Result != DAL_SUCCESS) {
      // Major error occurred
      DEBUG(( EFI_D_WARN, "ULogFile_Open: ERROR: Couldn't open log entry %a. ERROR %d \n", FileName, Result ));
      return Result;
    }

    // The code above is performed for all new logs, whether or not there's a file on disk. The log file now needs
    // to be opened/created on disk, for read and write
    OpenFile = EfiOpen((CHAR8 *) FilePath, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
    if (OpenFile == NULL) {
      DEBUG(( EFI_D_WARN, "ULogFile_Open: ERROR: Failed to open file %a \n", FilePath ));
      return ULOG_ERR_MISC;
    }

    // The file is opened on disk, and a handle to the ULog entry exists. All that's left is to redirect output.
    //If the following fails then something has really gone bad. So we ERR_FATAL.
    CORE_VERIFY(DAL_SUCCESS == ULogCore_SetTransportToAlt(hdl, &ULogVtablePrintfOut));
  } // Log was not managed by the system

  *h = hdl;

  ULOG_TYPE *Log = (ULOG_TYPE *) hdl;

  Log->config = AllocateZeroPool(sizeof(ULOG_CONFIG_TYPE));
  if(Log->config == NULL){
    DEBUG((EFI_D_WARN, "ULogFile_Open: Couldn't allocate config struct\n"));
    return ULOG_ERR_MALLOC;
  }

  Log->config->separator = ULOGFILE_DEFAULT_BOOTCOUNT_SEPARATOR;
  Log->config->PrintTimestamp = TRUE;
  Log->config->TrimNewline = FALSE;

  // Store the file handle in the ULog entry, so we don't need to reopen the file for every access
  Log->OpenFile = OpenFile;

  // Store the maximum file size in the log, to allow the client to specify when to wrap around
  Log->MaxLogFileSize = FileSize < sMaxLogFileSize ? (UINTN) FileSize : (UINTN) sMaxLogFileSize;

  //With the new FileLogging implementation to keep appending filelogs, we would need to determine if the file is already full
  //If Full set the Log->LogHasOverlapped = TRUE.
  if (EfiTell(OpenFile, NULL)>= Log->MaxLogFileSize)
  {
    Log->LogHasOverlapped = TRUE;
  }

  UINTN MetaDataFileFilePathLen = AsciiStrLen(sLogDirectory) + AsciiStrLen(Log->name)+ AsciiStrLen(ULOGFILE_METADATAFILE_NAME) + 1;

  if (MetaDataFileFilePathLen > MAX_PATHNAME)
  {
   DEBUG(( EFI_D_WARN, "ULogFile_Open: MetaDataFileFilePathLen > MAX_PATHNAME \n" ));
   return ULOG_ERR_INSUFFICIENT_BUFFER;
  }
  SetMem((void *) FilePath, MAX_PATHNAME, '\0');
  AsciiStrCpy(FilePath, sLogDirectory);
  AsciiStrCat(FilePath, Log->name);
  AsciiStrCat(FilePath, ULOGFILE_METADATAFILE_NAME);

  Log->OpenFileMetaData = EfiOpen((CHAR8 *) FilePath,
                                     EFI_FILE_MODE_READ | EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE, 0);

  if (Log->OpenFileMetaData == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Couldn't open Meta data file \n" ));
    Result = ULOG_ERR_NAMENOTFOUND;
  }
  else//Log->OpenFileMetaData != NULL
  {
    sSizeOfLogTransactionWrite = sizeof(Log->transactionWriteStart);//Saving the size of FileWrite for better performance instead of calculating everytime

    //Load the currWriteIndex from the persistent store into  Log->transactionWriteStart - So we write from there instead of the default start 
    //For appending the logs.
    EFI_STATUS ReadResult = EfiRead(Log->OpenFileMetaData, &Log->transactionWriteStart, &sSizeOfLogTransactionWrite);
    if (ReadResult == EFI_END_OF_FILE)//If we have an empty file -  The first time the file is created - It is a valid scenario
    {
      Log->transactionWriteStart = 0; //Reset the Log->transactionWriteStart  to zero.
    }
    else if (ReadResult != EFI_SUCCESS) {
      DEBUG(( EFI_D_WARN, "ULogFile_Open: Couldn't read Meta data file: %r \n", ReadResult ));
      Result = ULOG_ERR_MISC;
    }

    //If write index from meta file exceeds existing file size adjust WriteStart position accordingly 
    if (Log->transactionWriteStart > Log->OpenFile->MaxPosition )
    {
       DEBUG(( EFI_D_WARN, "ULogFile_Open: [%a] Fixing append position from 0x%X to 0x%llX,\n",
                                    Log->name, Log->transactionWriteStart, Log->OpenFile->MaxPosition));
       Log->transactionWriteStart = Log->OpenFile->MaxPosition;
    }
  }

  return Result;
}

/**
 * @brief Create a file system path name from a log name, in sLogDirectory with file extension
 *        ULOGFILE_EXTENSION
 *
 * @param LogName:  The name of the log managed by the ULog system
 * @param FilePath: The file system path to the log file is returned here. Caller is responsible for
 *                  allocation.
 *
 * @return  EFI_SUCCESS on successful creation
 *          DAL_ERROR for errors
 */
ULogResult ULogFile_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath)
{

  if (LogName == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Received request for NULL file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (strcmp(LogName, "") == 0) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Received request for Empty file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (FilePath == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Received request for NULL destination \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }

  CHAR8 *PathStr;
  UINT32 length = AsciiStrLen(sLogDirectory) + AsciiStrLen(LogName) + AsciiStrLen(ULOGFILE_EXTENSION) + 1;

  PathStr = AllocateZeroPool(length);
  if (PathStr == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Couldn't allocate zero'd buffer for path \n" ));
    return ULOG_ERR_MALLOC;
  }

  // Concatenate the strings in the format directory/filename.extension
  length = AsciiStrLen(sLogDirectory);
  AsciiStrCpy(PathStr, sLogDirectory);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Couldn't copy sLogDirectory string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(LogName);
  AsciiStrCat(PathStr, LogName);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Couldn't copy LogName string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(ULOGFILE_EXTENSION);
  AsciiStrCat(PathStr, ULOGFILE_EXTENSION);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFile_MakePathFromName: Couldn't copy ULOGFILE_EXTENSION string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  StrnCpy((CHAR16 *) FilePath, (CONST CHAR16 *) PathStr, length);

  FreePool(PathStr);
  return EFI_SUCCESS;
}

/**
 * <!-- ULogFile_Write -->
 * @brief Write from a buffer into a file.
 *
 * Implements the ALTernative write method for persistent ULogs.
 * The requested message is written to the log file associated with the provided handle.
 * If the provided handle does not correspond to a log managed by the system, it
 * is treated as an error.
 *
 * The file is not closed after writing, but is flushed if necessary.
 *
 * NOTE:  The expected file name is sLogDirectory<log_name>.ULOGFILE_EXTENSION.
 *
 * @param h : The ULogHandle corresponding to the file where the write will occur
 * @param buffer : The string to output to the file
 *
 * @return  DAL_SUCCESS if no errors occurred
 *          The appropriate ULOG_ERROR if a ULog-related error occurs
 *          DAL_ERROR for all other failures
 */
ULogResult ULogFile_Write(ULogHandle *h, CHAR8 *Buffer)
{

  if (h == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Write: Trying to write to a NULL ULog handle \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (Buffer == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Write: Trying to write a NULL buffer \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }

  EFI_OPEN_FILE *OpenFile = NULL;

  ULogResult WriteResult = ULOG_ERR_NOTSUPPORTED;
  UINTN BytesToWrite = AsciiStrLen(Buffer);

  ULOG_TYPE *Log = (ULOG_TYPE *) (*h);
  if (Log == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Write: Couldn't access ULog Entry \n" ));
    return ULOG_ERR_MISC;
  }

  OpenFile = Log->OpenFile;

  // Open the file and write to it
  if (OpenFile == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Write: Failed to open file \n" ));
    WriteResult = DAL_ERROR;
  }

  else {
    UINT64 CurPos = 0;
    UINTN MaxFileSize = Log->MaxLogFileSize;

    //Log->transactionWriteStart always indicates the next position to write as we currently save it in our metafile
    //This is either in case of overlap or non-overlap case.
    CurPos = Log->transactionWriteStart;

    if(Log->config->PrintTimestamp)
    {
      BytesToWrite += sBootCountLen;
    } else {
      BytesToWrite -= ULOGFILE_TIMESTAMPLENGTH + 1 + 1;
    }
    if (CurPos + BytesToWrite > MaxFileSize)
    {
      //Corner case when the message is bigger than max file size
      if(BytesToWrite > MaxFileSize)
      {
        //Trim the message
        if(Log->config->PrintTimestamp)
        {
           UINTN n = (sBootCountLen < MaxFileSize) ? MaxFileSize-sBootCountLen : 0;
           Buffer[n] = '\0';
        }
        else
        {
           Buffer[ULOGFILE_TIMESTAMPLENGTH + 1 + MaxFileSize] = '\0';
        }
      }
      else
      {
        // Clean leftover from the previous wraparound at the bottom of the file and start from the top
        if (EFI_SUCCESS != EfiSeek(OpenFile, CurPos, EfiSeekStart)) // Seek to the end of last write
        {
          DEBUG(( EFI_D_WARN, "ULogFile_Write: EfiSeek[CurPos=0x%llX, MaxFileSize=0x%X, %a->MaxPosition=0x%llX] failed,\n",
                CurPos, MaxFileSize, Log->name,OpenFile->MaxPosition));
        }
        else
        {
          UINTN i, n = 1;
          for (i = CurPos; i < MaxFileSize; i++) {
            WriteResult = EfiWrite(OpenFile, " ", &n);
          }
        }
      }
      // Wrap around needed
      CurPos = 0;
      Log->LogHasOverlapped = TRUE; // Trigger the Separator
      Log->transactionWriteStart = 0;
    }

    if (EFI_SUCCESS != EfiSeek(OpenFile, CurPos, EfiSeekStart)) // Seek to the end of last write
    {
      DEBUG(( EFI_D_WARN, "ULogFile_Write: EfiSeek[CurPos=0x%llX, MaxFileSize=0x%X, %a->MaxPosition=0x%llX] failed,\n",
              CurPos, MaxFileSize, Log->name,OpenFile->MaxPosition));
      WriteResult = ULOG_ERR_MISC;
    }
    else
    {
      if(Log->config->PrintTimestamp){
        sBootCountStr[sBootCountLen-1]   = Log->config->separator; // Bootcount-timestamp separator
        Buffer[ULOGFILE_TIMESTAMPLENGTH] = Log->config->separator; // Timestamp-message separator

        // Print the bootcount and bootcount-timestamp separator, the timestamp will be printed later
        BytesToWrite = sBootCountLen;
        WriteResult = EfiWrite(OpenFile, sBootCountStr, &BytesToWrite);
      }

      else{
        Buffer += ULOGFILE_TIMESTAMPLENGTH + 1 + 1;  // Original message was Timestamp: Message, so account for the extra space as well
      }

      // Remove one extra \n from the end, if it exists: CR719222
      if(Log->config->TrimNewline && Buffer[BytesToWrite-1] == '\n' && Buffer[BytesToWrite-2] == '\n'){
        Buffer[BytesToWrite-1] = '\0';
        BytesToWrite--;
      }

      // Write the actual message
      BytesToWrite = AsciiStrLen(Buffer);
      WriteResult = EfiWrite(OpenFile, Buffer, &BytesToWrite);

      // Store the current position back in the ULog list.
      EfiTell(OpenFile, (UINT64 *) (&(Log->transactionWriteStart)));

      //Store the current position -  Log->transactionWriteStart - even in the persistent Meta File for everywrite
      //SJ To Do : Alternately this can be done during exit bootservices for better performance
      if(Log->OpenFileMetaData)
      {
        WriteResult = EfiSeek(Log->OpenFileMetaData, 0, EfiSeekStart);
        if (WriteResult != EFI_SUCCESS) {
          DEBUG(( EFI_D_WARN, "ULogFile_Write: Seek-to-start failed: %r \n", WriteResult ));
          return ULOG_ERR_MISC;
        }
        WriteResult = EfiWrite(Log->OpenFileMetaData, &Log->transactionWriteStart, &sSizeOfLogTransactionWrite);
        if (WriteResult != EFI_SUCCESS) {
          DEBUG(( EFI_D_WARN, "ULogFile_Write: Write failed: %r \n", WriteResult ));
          EfiClose(Log->OpenFileMetaData);
          return ULOG_ERR_MISC;
        }
      }
      // Print the overlap indicator if necessary
      if (Log->LogHasOverlapped && !EFI_ERROR(WriteResult) && 
                  ((sOverlapSeparatorLen + Log->transactionWriteStart) < MaxFileSize) ) 
      {
        BytesToWrite = sOverlapSeparatorLen;
        WriteResult = EfiWrite(OpenFile, sOverlapSeparatorString, &BytesToWrite);
      }

      WriteResult = ULogFile_FlushFileBuffer(h);
    }

  } // if OpenFile != NULL

  return WriteResult;
}


