/** @file FileLogTestApp.c
   
  Tests for FileLog.

  Copyright (c) 2017 by Qualcomm Technologies Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/28/13   va     Initial revision
.
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/BaseLib.h>
#include <Library/QcomBaseLib.h>

#include <Library/ULogLib/ULogCoreInternal.h>
#include "ULog.h"
#include "npa_log.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiFileLib.h>
#include <Library/ULogUEFI.h>
#include <Library/ULogFront.h>
#include "CoreString.h"
#include <Drivers/ULogDxe/ULogFile.h>


#define PROFILE_LOG_NAME "ULogProfileTestLog"
#define PROFILE_RUNS 10

typedef struct 
{
  char *cmd;  //Example list,dump
  char LogName[ULOG_MAX_NAME_SIZE];  //Example "PmicLog"
}FileLogTestCmdLineParamsType;
FileLogTestCmdLineParamsType FileLogTestCmdLineParams; //Global should get unloaded when the app exits


char *FileLogTestApp_Cmds[]=
{
  "file",
  "help",
};

STATIC CHAR8 *sTestLogDirectory;  // Target-specific log directory
STATIC UINTN sTestMaxLogFileSize; // Target-specific maximum log size


//Declarations
EFI_STATUS ProcessCmdLineParams(EFI_HANDLE ImageHandle);
void FileLogTestApp_PrintHelp(void);
EFI_STATUS FileLogTest(void);
EFI_STATUS FileLogTest_InitLogDirectoryAndSize(void);
ULogResult FileLogTest_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath);
//From Library/UefiCfgLib.h - Unable to include TODO
EFI_STATUS GetConfigValue (CHAR8* Key, UINT32* Value);
EFI_STATUS GetConfigString (CHAR8* Key, CHAR8* Value, UINTN* ValBuffSize);
int FileLogTestApp_CheckDriveExists(void);


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

#define FILELOG_TESTAPP_MSG_STR_SIZE 4096


EFI_STATUS
EFIAPI
FileLogTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_NOT_READY;

  TEST_START("FILELOG TEST");

  do{//For Easy error handling

    Status = FileLogTest_InitLogDirectoryAndSize();
    if(Status != EFI_SUCCESS)
    {
      break;
    }
    Status = ProcessCmdLineParams(ImageHandle);
    if(Status != EFI_SUCCESS)
    {
      break;
    }
    if(NULL == FileLogTestCmdLineParams.cmd)//If no params - Perform some basic sanity
    {

      TEST_START("FILELOG FILE");
      //Display the File Logging Info - LogPartition, MaxFileSize
      AsciiPrint("%-20a = %a\n"  , "LogPartition Used",sTestLogDirectory );
      AsciiPrint("%-20a = %d%a\n"  , "MaxFileSize"    ,sTestMaxLogFileSize/1024, "kB" );
      //Perform Write into the Partition
      if(FileLogTestApp_CheckDriveExists())
      {
        AsciiPrint("Passed! Partition [%a] exists! \n",sTestLogDirectory );
        Status = EFI_SUCCESS;
      }
      else
      {
        AsciiPrint("Failed! Issues reading partition [%a]\n",sTestLogDirectory );
        Status = EFI_NOT_FOUND;
        TEST_STOP("FILELOG FILE");
        break;
      }
      TEST_STOP("FILELOG FILE");
    }
    else if(0 == AsciiStrCmp(FileLogTestCmdLineParams.cmd,"file"))
    {
      Status = FileLogTest();
      if(Status != EFI_SUCCESS)
      {
        break;
      }
    }
    else if(0 == AsciiStrCmp(FileLogTestCmdLineParams.cmd,"help"))
    {
      FileLogTestApp_PrintHelp();
    }
    //No else as we already have error checks in ProcessCmdLineParams() for bad cmd line params
  }while(0);

  //In case there are bad commmand line parameters returned from ProcessCmdLineParams() we print
  //Otherwise TestStatus() would anyways print the Failure Status
  if(EFI_UNSUPPORTED == Status)
  {
    AsciiPrint("Bad Command Line Parameters\n");
    FileLogTestApp_PrintHelp();
  }

  TestStatus("FILELOG TEST", Status);
  TEST_STOP("FILELOG TEST");

  return Status;
}

/*Get the Cmd line params - Perform param check and load into global FileLogTestCmdLineParams for further processing*/
EFI_STATUS ProcessCmdLineParams(EFI_HANDLE ImageHandle)
{
  UINTN argc;
  CHAR8 **argv;
  EFI_STATUS Status = EFI_UNSUPPORTED;

  //Memset  FileLogTestCmdLineParams
  SetMem((void *) &FileLogTestCmdLineParams, sizeof(FileLogTestCmdLineParams), '\0');


  do //For Easy error handling
  {
    Status = GetCmdLineArgs (ImageHandle, &argc, &argv);
    if(EFI_SUCCESS != Status)
    {
      break;
    }
    else//EFI_SUCCESS 
    {
      //Param check
      if (argc == 0)//FileLogTest <NoParam> is a valid scenario
      {
        Status = EFI_SUCCESS;
        break;
      }
      //Check if it is a valid command
      int CmdCntMax = (sizeof(FileLogTestApp_Cmds)/sizeof(char *));
      int cmdCnt;
      for(cmdCnt=0; cmdCnt < CmdCntMax ; cmdCnt++)
      {
        if (0 == AsciiStrCmp(argv[0],FileLogTestApp_Cmds[cmdCnt]))
        {
          break;//Success - Break from for loop - Not from While(0);
        }
      }
      if (cmdCnt == CmdCntMax)//If Param1(argv[0]) does not match any of the supported commands
      {
        Status = EFI_UNSUPPORTED;
        break;//Failure - Break out of the do while
      }
      /*Load Cmd Line params into our global - FileLogTestCmdLineParams*/
      FileLogTestCmdLineParams.cmd = argv[0]; //argv will stay till we free it at the end of the test.
      Status = EFI_SUCCESS;//Success!!
    }
  }while(0);
  return Status;
}


/**
 * @brief Check if a log is enabled, and enable it if it isn't
 *
 * @param h: the log handle
 * @param core: The table to fill in with the reader and writer cores
 * @return  DAL_SUCCESS if the log was enabled successfully
 *          ULOG_ERR_ALREADYENABLED if the log was previously enabled
 *          Appropriate ULOG_ERROR otherwise
 */
ULogResult FileLogTest_CheckEnabled(ULogHandle *h, ULOG_CORE_VTABLE *core) {
    ULogResult Status = ULOG_ERR_NOTSUPPORTED;
    if (NULL == h || NULL == core) {
        DEBUG(( EFI_D_ERROR, "FileLogTest_CheckEnabled: Invalid parameter \n" ));
        return ULOG_ERR_INVALIDPARAMETER;
    }
    if ( TRUE == ULogCore_IsEnabled(h, &core)) {
        DEBUG((EFI_D_INFO, "FileLogTest: Log is enabled => successful creation. \n"));
        Status = ULOG_ERR_ALREADYENABLED;
    } else {
        DEBUG((EFI_D_INFO, "Enabling log\n" ));
        Status = ULogCore_Enable(h);
        DEBUG((EFI_D_INFO, "FileLogTest: Log enable returned %d \n", Status));
    }
    return Status;
}

/**
 **/
EFI_STATUS FileLogTest(void)
{
    ULogResult Status = EFI_SUCCESS;
    ULogHandle ProfileHdl; // for profiling UEFI File Logging
    UINT32 logsize = 2*1024*1024; 
    BOOLEAN    FileLoggingIsEnabled = TRUE;
    UINT32     VarSize = 1;

    AsciiPrint("\nRunning tests in this order:\n"
               "> Open\n"
               "> Enable\n"
               "> Profile\n"
               "> Write\n");

    // Get rid of the whitespace that TEST_START adds, so printfs appear properly
    AsciiPrint("\n");

    Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
                               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                               VarSize, &FileLoggingIsEnabled);

    // UEFI File Logging:
    Status = ULogFile_Open(&ProfileHdl, (CONST CHAR8 *)PROFILE_LOG_NAME, logsize);

    // Memory Logging (For reference):
    // Status = ULogFront_RealTimeInit(&ProfileHdl, PROFILE_LOG_NAME, logsize, (UINT32) ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);

    if (Status != DAL_SUCCESS && Status != ULOG_ERR_ALREADYCREATED) {
        AsciiPrint("Didn't open test log %a, ERROR %d \n", PROFILE_LOG_NAME, Status);
        TestStatus("FILELOG_FILE_TEST", Status);
        TEST_STOP("FILELOG_FILE_TEST");
        return Status;
    } else {
        Status = EFI_SUCCESS;
    }

    AsciiPrint("Querying config information\n");
    ULOG_CONFIG_TYPE settings;
    Status = ULogFile_GetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("GetConfig failed\n");
      TestStatus("FILELOG_FILE_TEST", Status);
      TEST_STOP("FILELOG_FILE_TEST");
      return Status;
    }

    AsciiPrint("Separator = '%c'\n", settings.separator);
    AsciiPrint("PrintTimestamp = %d\n", settings.PrintTimestamp);
    AsciiPrint("TrimNewline = %d\n", settings.TrimNewline);

    // Profile UEFI File Logging
    CHAR8 *ShortMessage = "Shooort";  // 8 Bytes (7 chars + \0)
    UINT32 ShortLength = AsciiStrSize(ShortMessage);
    CHAR8 *LongMessage = "Loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong\r\n"; // 128 Bytes
    UINT32 LongLength = AsciiStrSize(LongMessage);
    UINT32 n = 1024;
    UINT32 NumBytes, Seconds;
    UINT64 StartTime, EndTime, DeltaShort, DeltaLong, ShortBps, LongBps, ShortTimePerMsg, LongTimePerMsg, NetTimeShort,
                NetTimeLong;

    UINT32 iteration, i;
    iteration = 0;
    NetTimeShort = 0;
    NetTimeLong = 0;

    AsciiPrint("Disabling timestamp and enabling newline trimming for short messages\n");
    settings.PrintTimestamp = FALSE;
    settings.TrimNewline = TRUE;
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("Timestamp disable failed\n");
      TestStatus("FILELOG_FILE_TEST", Status);
      TEST_STOP("FILELOG_FILE_TEST");
      return Status;
    }
    AsciiPrint("Disabled timestamp\n");

    AsciiPrint("Profile test: %d sets, %d iterations per set = %d logs in total for each message length \n",
               PROFILE_RUNS, n, (PROFILE_RUNS * n));
    AsciiPrint("Length\tIteration\tTime(us)\tTime/Message (us)\tSpeed(B/s) \n");

    for (iteration = 0; iteration < PROFILE_RUNS; iteration++) {
        i = 0;
        StartTime = 0;
        EndTime = 0;
        DeltaShort = 0;
        ShortBps = 0;
        ShortTimePerMsg = 0;

        StartTime = GetTimerCountus();
        for (i = 0; i < n; i++) {
            ULOG_RT_PRINTF_1(ProfileHdl, "%s", ShortMessage);
        }
        EndTime = GetTimerCountus();
        DeltaShort = EndTime - StartTime;
        NetTimeShort += DeltaShort;

        // Bytes/s = ( n iterations * Bytes) / ( usec/n iterations * 1 sec / 1000000 usec * n iterations), n iterations cancel out

        if (DeltaShort != 0) {
            NumBytes = (n + 1) * ShortLength;   // i goes from 0 to n-1
            Seconds = DeltaShort / 1000000;
            ShortBps = (NumBytes / Seconds);
        }

        if (n != 0) {
            ShortTimePerMsg = DeltaShort / n;
        }

        AsciiPrint("Short\t%d\t\t%ld\t\t%lld\t\t\t%lld \n", iteration, DeltaShort, ShortTimePerMsg, ShortBps);
    }

    AsciiPrint("Trying to change separator to invalid value: '\\0' (should fail)\n");
    settings.separator = '\0';
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != ULOG_ERR_INVALIDPARAMETER){
      AsciiPrint("Applied invalid separator. Stopping test\n");
      TestStatus("FILELOG TEST", EFI_INVALID_PARAMETER);
      TEST_STOP("FILELOG TEST");
      return Status;
    }

    AsciiPrint("Changing separator to ','\n");
    settings.separator = ',';
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("Couldn't apply valid separator. Stopping test\n");
      TestStatus("FILELOG TEST", Status);
      TEST_STOP("FILELOG TEST");
      return Status;
    }
    AsciiPrint("Enabling timestamp and disabling newline trimming for long messages\n");
    settings.PrintTimestamp = TRUE;
    settings.TrimNewline = FALSE;
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("Timestamp enable failed\n");
      TestStatus("FILELOG_FILE_TEST", Status);
      TEST_STOP("FILELOG_FILE_TEST");
      return Status;
    }
    AsciiPrint("Enabled.\n");


    for (iteration = 0; iteration < PROFILE_RUNS; iteration++) {
        i = 0;
        StartTime = 0;
        EndTime = 0;
        DeltaLong = 0;
        LongBps = 0;
        LongTimePerMsg = 0;

        StartTime = GetTimerCountus();
        for (i = 0; i < n; i++) {
            ULOG_RT_PRINTF_1(ProfileHdl, "%s", LongMessage);
        }
        EndTime = GetTimerCountus();
        DeltaLong = EndTime - StartTime;
        NetTimeLong += DeltaLong;

        if (DeltaLong != 0) {
            NumBytes = (n + 1) * LongLength; // i goes from 0 to n-1
            Seconds = DeltaLong / 1000000;
            LongBps = (NumBytes / Seconds);
        }

        if (n != 0) {
            LongTimePerMsg = DeltaLong / n;
        }
        AsciiPrint("Long\t%d\t\t%ld\t\t%lld\t\t\t%lld \n", iteration, DeltaLong, LongTimePerMsg, LongBps);
    }

    UINT64 AvgTimeShort = 0;
    if (n * PROFILE_RUNS != 0) {
        AvgTimeShort = NetTimeShort / (n * PROFILE_RUNS);
    }
    AsciiPrint("AvgShort\t\t%lld\n", AvgTimeShort);

    UINT64 AvgTimeLong = 0;
    if (n * PROFILE_RUNS != 0) {
        AvgTimeLong = NetTimeLong / (n * PROFILE_RUNS);
    }
    AsciiPrint("AvgLong\t\t\t%lld\n", AvgTimeLong);

    UINT64 AvgTime = 0;
    if (n * PROFILE_RUNS != 0) {
        AvgTime = (NetTimeShort + NetTimeLong) / (2 * n * PROFILE_RUNS);
    }
    AsciiPrint("Avg\t\t\t%lld\n", AvgTime);

    TestStatus("FILELOG TEST", Status);
    return Status;
}

EFI_STATUS FileLogTest_InitLogDirectoryAndSize()
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
      DEBUG((EFI_D_WARN, "FileLog: Couldn't allocate buffer \n"));
      return EFI_OUT_OF_RESOURCES;
    }

    Status = GetConfigString((CHAR8 *) "SpecialLogPartition", LogDir, &BufSize);
    if (Status == EFI_SUCCESS) {
      // Use the SpecialLogPartition to log
      sTestLogDirectory = LogDir;
    } else {
      // couldn't read for some reason, use the default value
      DEBUG(
          (EFI_D_WARN, "FileLog: Couldn't read the special log partition, Error %r. Using default directory\n", Status));
      sTestLogDirectory = ULOGFILE_DEFAULT_DIR;
    }
  } else {
    // A valid SpecialLogPartition was not specified, default to EFIESP:\LOGs
    sTestLogDirectory = ULOGFILE_DEFAULT_DIR;
  }

  // Now do the same for the max log size
  UINT32 Value = 0;
  Status = GetConfigValue("MaxLogFileSize", &Value);

  if ((Status == EFI_SUCCESS) && (Value > 0)) {
    sTestMaxLogFileSize = Value;
  } else {
    sTestMaxLogFileSize = ULOGFILE_DEFAULT_MAX_SIZE;
  }

  return EFI_SUCCESS;
}


ULogResult FileLogTest_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath)
{

  if (LogName == NULL) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Received request for NULL file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (AsciiStrCmp(LogName, "") == 0) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Received request for Empty file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (FilePath == NULL) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Received request for NULL destination \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }

  CHAR8 *PathStr;
  UINT32 length = AsciiStrLen(sTestLogDirectory) + AsciiStrLen(LogName) + AsciiStrLen(ULOGFILE_EXTENSION) + 1;

  PathStr = AllocateZeroPool(length);
  if (PathStr == NULL) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Couldn't allocate zero'd buffer for path \n" ));
    return ULOG_ERR_MALLOC;
  }

  // Concatenate the strings in the format directory/filename.extension
  length = AsciiStrLen(sTestLogDirectory);
  AsciiStrCpy(PathStr, sTestLogDirectory);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Couldn't copy sLogDirectory string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(LogName);
  AsciiStrCat(PathStr, LogName);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Couldn't copy LogName string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(ULOGFILE_EXTENSION);
  AsciiStrCat(PathStr, ULOGFILE_EXTENSION);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "FileLogTest_MakePathFromName: Couldn't copy ULOGFILE_EXTENSION string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  StrnCpy((CHAR16 *) FilePath, (CONST CHAR16 *) PathStr, length);

  FreePool(PathStr);
  return EFI_SUCCESS;
}

//Returns 1 if PASS, 0 if FAIL
int FileLogTestApp_CheckDriveExists(void)
{
  EFI_OPEN_FILE *OpenFile = EfiOpen((CHAR8 *) sTestLogDirectory, EFI_FILE_MODE_READ, 0);
  if (OpenFile == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: ERROR: Failed to open Drive:%a \n", sTestLogDirectory ));
    return 0;
  }
  EfiClose(OpenFile);
  return 1;
}

#if 0
int FileLogTestApp_CheckDriveExists(void)
{
  CHAR8 FilePath[MAX_PATHNAME];
  SetMem(FilePath, MAX_PATHNAME, '\0');
  ULogResult Result = FileLogTest_MakePathFromName("FileLogTest", FilePath);
  if (Result != EFI_SUCCESS) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: Coudln't create file path string\n" ));
    return 0;
  }
  EFI_OPEN_FILE *OpenFile = EfiOpen((CHAR8 *) FilePath, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
  if (OpenFile == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFile_Open: ERROR: Failed to open file %a \n", FilePath ));
    return 0;
  }
  return 1;
}
#endif

void FileLogTestApp_PrintHelp(void)
{

AsciiPrint("\n Usage: \n");
AsciiPrint("==============================================================================\n");
AsciiPrint("FileLogTest dump     <NoParams>  - Dumps All Logs \n");
AsciiPrint("FileLogTest file     <NoParams>  - Test/Profile FileLogging \n");
AsciiPrint("FileLogTest help     <NoParam>   - Prints Usage \n");
AsciiPrint("==============================================================================\n");

}
