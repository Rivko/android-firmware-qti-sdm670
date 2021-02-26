/** @file ULogTestApp.c
   
  Tests for ULog.

  Copyright (c) 2013-2015 by Qualcomm Technologies Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/30/15   sj/rli  Support new test commands such as list, dump, file.
 10/02/14   vpopuri Merge with ULOGv5
 08/20/14   sj      KW Fixes
 06/03/14   vpopuri Temporarily skipping this for 64bit, until parser is fixed
 05/14/14   sj      64 bit compilation error fix
 05/01/13   rli     Initial revision.
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
//#include <Library/UefiCfgLib.h>


#define FILENAME "ULogTestFile"
#define FILENAME2 "ULogTest2"
#define PROFILE_LOG_NAME "ULogProfileTestLog"
#define PROFILE_RUNS 10

typedef enum
{
  ULOGTEST_DUMP_SHORT,
  ULOGTEST_DUMP_FULL  
}enumULogDumpType;

typedef struct 
{
  char *cmd;  //Example list,dump
  char LogName[ULOG_MAX_NAME_SIZE];  //Example "PmicLog"
}ULogTestCmdLineParamsType;
ULogTestCmdLineParamsType ULogTestCmdLineParams; //Global should get unloaded when the app exits

//#pragma pack(2) //as StrnCpy() expects FileLogPath to be that way.
typedef struct 
{
  char *LogName;
  ULogHandle *h;
  unsigned char transport; //indicates RAM, STM or ALT(FILE)
  unsigned char padding;
  char FileLogPath[MAX_PATHNAME];
}ULogInfoListType;
//#pragma pack()

ULogInfoListType *arrULogInfoList = NULL; //pointer to array
char *  gNameArray = NULL;//gNameArray into which we read the UlogList names
uint32 ULogInfoListCnt = 0;

char *ULogTestApp_Cmds[]=
{
  "list",
  "dump",
  "file",
  "help",
};

STATIC CHAR8 *sTestLogDirectory;  // Target-specific log directory
STATIC UINTN sTestMaxLogFileSize; // Target-specific maximum log size
DALREG_DriverInfoList gDALModDriverInfoList = { 0, NULL};//For compilation after adding snprintf -> CoreUtilsLib




//Declarations
EFI_STATUS ProcessCmdLineParams(EFI_HANDLE ImageHandle);
EFI_STATUS InitULogTestList(void);
EFI_STATUS DumpULog(char *logName, enumULogDumpType eDumpType);
void ULogTestApp_PrintHelp(void);
EFI_STATUS ULogFileTest(void);
void ULogTestApp_PrintULogList(void);
EFI_STATUS ULogFileTest_InitLogDirectoryAndSize(void);
ULogResult ULogFileTest_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath);
//From Library/UefiCfgLib.h - Unable to include TODO
EFI_STATUS GetConfigValue (CHAR8* Key, UINT32* Value);
EFI_STATUS GetConfigString (CHAR8* Key, CHAR8* Value, UINTN* ValBuffSize);
int ULogTestApp_CheckDriveExists(void);



/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

#define ULOG_TESTAPP_MSG_STR_SIZE 4096


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_NOT_READY;
  //ULogHandle h;
  //ULogResult result;
  //char *outputData;

  TEST_START("ULOG TEST");

  do{//For Easy error handling

    Status = ULogFileTest_InitLogDirectoryAndSize();
    if(Status != EFI_SUCCESS)
    {
      break;
    }
    Status = ProcessCmdLineParams(ImageHandle);
    if(Status != EFI_SUCCESS)
    {
      break;
    }
    if(NULL == ULogTestCmdLineParams.cmd)//If no params - Perform some basic sanity
    {
      //Retaining existing behavior for legacy reasons
      Status = DumpULog(NPA_DEFAULT_LOG_NAME, ULOGTEST_DUMP_SHORT);
      if(Status != EFI_SUCCESS)
      {
        break;
      }
      
      TEST_START("ULOG FILE");
      //Display the File Logging Info - LogPartition, MaxFileSize
      AsciiPrint("%-20a = %a\n"  , "LogPartition Used",sTestLogDirectory );
      AsciiPrint("%-20a = %d%a\n"  , "MaxFileSize"    ,sTestMaxLogFileSize/1024, "kB" );
      //Perform Write into the Partition
      if(ULogTestApp_CheckDriveExists())
      {
        AsciiPrint("Passed! Partition [%a] exists! \n",sTestLogDirectory );
        Status = EFI_SUCCESS;
      }
      else
      {
        AsciiPrint("Failed! Issues reading partition [%a]\n",sTestLogDirectory );
        Status = EFI_NOT_FOUND;
        TEST_STOP("ULOG FILE");
        break;
      }
      TEST_STOP("ULOG FILE");
      
    }
    else if(0 == AsciiStrCmp(ULogTestCmdLineParams.cmd,"list"))
    {
      Status = InitULogTestList();
      if(Status != EFI_SUCCESS)
      {
        break;
      }
      ULogTestApp_PrintULogList();
    }
    else if(0 == AsciiStrCmp(ULogTestCmdLineParams.cmd,"dump"))
    {
      Status = InitULogTestList();
      if(Status != EFI_SUCCESS)
      {
        break;
      }
      if (AsciiStrLen(ULogTestCmdLineParams.LogName) > 0)//If there is a valid param passed as file name - NonEmpty string
      {
        //Dump the Log
        Status = DumpULog(ULogTestCmdLineParams.LogName, ULOGTEST_DUMP_FULL);
        if(Status != EFI_SUCCESS)
        {
          break;
        }
       }
      else
      {
        //Print all Dumps
        int nameCnt;        
        for(nameCnt = 0; nameCnt < ULogInfoListCnt ; nameCnt++)
        {
          if(arrULogInfoList[nameCnt].transport != ULOG_TRANSPORT_ALTERNATE)//If not-FileLogs
          {
            Status = DumpULog(arrULogInfoList[nameCnt].LogName,ULOGTEST_DUMP_FULL);
            if(Status != EFI_SUCCESS)
            {
              break;//out of for loop 
            }
          }
        }
        if(Status != EFI_SUCCESS)
        {
          break;//out of while(0)
        }
      }
    }
    else if(0 == AsciiStrCmp(ULogTestCmdLineParams.cmd,"file"))
    {
      Status = ULogFileTest();
      if(Status != EFI_SUCCESS)
      {
        break;
      }
    }
    else if(0 == AsciiStrCmp(ULogTestCmdLineParams.cmd,"help"))
    {
      ULogTestApp_PrintHelp();
    }
    //No else as we already have error checks in ProcessCmdLineParams() for bad cmd line params
  }while(0);

  //In case there are bad commmand line parameters returned from ProcessCmdLineParams() we print
  //Otherwise TestStatus() would anyways print the Failure Status
  if(EFI_UNSUPPORTED == Status)
  {
    AsciiPrint("Bad Command Line Parameters\n");
    ULogTestApp_PrintHelp();
  }

  if(arrULogInfoList)
  {
    FreePool(arrULogInfoList);
  }
  if(gNameArray)
  {
    FreePool(gNameArray);
  }
  TestStatus("ULOG TEST", Status);
  TEST_STOP("ULOG TEST");
  return Status;
}


/*Will init the arrULogInfoList with the current list of UlogNames*/
//Does not free arrULogInfoList and gNameArray in success case - and will be freed at the end of the test.
EFI_STATUS InitULogTestList(void)
{
  uint32  registeredCount, nameArraySize, namesReadCount;
  ULogResult result;
  char *nameArray;

  nameArraySize = ULOG_MAX_NAME_SIZE+1;

  nameArray = gNameArray =  AllocateZeroPool(ULOG_MAX_NAME_SIZE+1);//Start off with the smallest buffer to copy 1 buffer

  if(NULL == nameArray)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  
  result = ULogCore_List(&registeredCount, nameArraySize, &namesReadCount, nameArray);//Read back the registeredCount here
  if(result != EFI_SUCCESS)//ULOG_ERR_INSUFFICIENT_BUFFER is not an option as we have atleast size  of buffer greater than ULOG_MAX_NAME_SIZE
  {
    return result;
  }
  FreePool(gNameArray);//Free now as we are mallocing again next with the correct size

  nameArraySize = (ULOG_MAX_NAME_SIZE+1)*registeredCount;
  nameArray = gNameArray  =  AllocateZeroPool((ULOG_MAX_NAME_SIZE+1)*registeredCount);//Now we allocate enough buffer to read all the ULogNames
  if(NULL == nameArray)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  result = ULogCore_List(&registeredCount, nameArraySize, &namesReadCount, nameArray);
  if(result != EFI_SUCCESS || registeredCount != namesReadCount)
  {
    return EFI_BAD_BUFFER_SIZE;//Something went wrong
  }


  //Now we have the nameArray with all the names - Copying this into the arrULogInfoList
  arrULogInfoList = (ULogInfoListType *)AllocateZeroPool(sizeof(*arrULogInfoList) *namesReadCount);
  
  if(NULL == arrULogInfoList)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  char *EndofnameArray= nameArray + (ULOG_MAX_NAME_SIZE+1)*registeredCount;//Just for some check

  int nameCnt;
  ULogInfoListCnt = 0;
  for(nameCnt = 0; nameCnt < namesReadCount; nameCnt++)
  {
   arrULogInfoList[nameCnt].LogName = nameArray; //Assign it as the we are not freeing nameArray here.
   ULogInfoListCnt++;

   ULogHandle h;
   h = ULogCore_HandleGet(arrULogInfoList[nameCnt].LogName);
   if (NULL == h)
   {
     AsciiPrint("InitULogTestList: ULogCore_HandleGet Failed");
     return EFI_UNSUPPORTED;
   }
   ULOG_TYPE * log = (ULOG_TYPE *)h;
   arrULogInfoList[nameCnt].transport = log->transport;
   if(log->transport == ULOG_TRANSPORT_ALTERNATE)
   {
     core_snprintf(arrULogInfoList[nameCnt].FileLogPath, sizeof(arrULogInfoList[nameCnt].FileLogPath), \
       "%s:%s", log->OpenFile->DeviceName, log->OpenFile->FileName);
   }
   while(*(nameArray++) != 0); //Skip to the next string
   if(nameArray > EndofnameArray)//Gentle check - IF we have traversed beyond the array.
   {
      break;//break out of while
   }
  }
  
  //FreePool(gNameArray);//Do not free gNameArray & arrULogInfoList here as it would be used later - and free'd at the end of the test
  return EFI_SUCCESS;

}

/*Get the Cmd line params - Perform param check and load into global ULogTestCmdLineParams for further processing*/
EFI_STATUS ProcessCmdLineParams(EFI_HANDLE ImageHandle)
{
  UINTN argc;
  CHAR8 **argv;
  EFI_STATUS Status = EFI_UNSUPPORTED;

  //Memset  ULogTestCmdLineParams
  SetMem((void *) &ULogTestCmdLineParams, sizeof(ULogTestCmdLineParams), '\0');


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
#if 0 //Following will not work because of the UEFI bug - workaround below
      if (argc > 2)//Cannot be more than 2 params - SJ TODO Check how to handle for lognames with spaces
      {
        break;//Failure
      }
#endif      
      if (argc == 0)//ULogTest <NoParam> is a valid scenario
      {
        Status = EFI_SUCCESS;
        break;
      }
      //Check if it is a valid command
      int CmdCntMax = (sizeof(ULogTestApp_Cmds)/sizeof(char *));
      int cmdCnt;
      for(cmdCnt=0; cmdCnt < CmdCntMax ; cmdCnt++)
      {
        if (0 == AsciiStrCmp(argv[0],ULogTestApp_Cmds[cmdCnt]))
        {
          break;//Success - Break from for loop - Not from While(0);
        }
      }
      if (cmdCnt == CmdCntMax)//If Param1(argv[0]) does not match any of the supported commands
      {
        Status = EFI_UNSUPPORTED;
        break;//Failure - Break out of the do while
      }
      /*Load Cmd Line params into our global - ULogTestCmdLineParams*/
      ULogTestCmdLineParams.cmd = argv[0]; //argv will stay till we free it at the end of the test.
      if (0 == AsciiStrCmp(ULogTestCmdLineParams.cmd,"dump")) 
      {
        //Processing the parameters of "dump"
        int cnt;
        SetMem (ULogTestCmdLineParams.LogName, sizeof(ULogTestCmdLineParams.LogName),'\0' );
        //Following is a workaround for "More than one command line param not getting passed as part of LoadedImage->LoadOptions if not enclosed within quotes"
        //The rest of the parameters is part of the file name - as file names can contain spaces
        for(cnt = 1; cnt < argc; cnt++)
        {
          //Concat
          AsciiStrCat(ULogTestCmdLineParams.LogName,argv[cnt]);
          if (cnt < argc-1)//We concat spaces except at the end. - Like "Clock Log" has only one Space and no space at the end.
          {
            AsciiStrCat(ULogTestCmdLineParams.LogName," ");
          }
        }
      }
      Status = EFI_SUCCESS;//Success!!
    }
  }while(0);
  return Status;
}


EFI_STATUS DumpULog(char *logName, enumULogDumpType eDumpType )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  ULogHandle h;
  ULogResult uLogResult;
  char *msgString;
  //char errString[25];
  //SetMem((void *)errString, sizeof(errString), '\0');
  uint32 outCount;
  char outMem[1024];
  
  msgString = (char *)AllocateZeroPool(ULOG_TESTAPP_MSG_STR_SIZE*sizeof(char));
  if (NULL == msgString)
  {
    return EFI_OUT_OF_RESOURCES;
  }


  do//For easy error handling
  {
    h = ULogCore_HandleGet(logName);
    if (NULL == h )
    {
      AsciiPrint("DumpULog: ULogCore_HandleGet Failed for %a\n",logName);
      AsciiPrint("TIP: Please make sure to enclose dump and log name with quotes;Check if logName is valid\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    ULOG_TYPE * log = (ULOG_TYPE *)h;
    if(log->transport == ULOG_TRANSPORT_ALTERNATE)
    {
      AsciiPrint("%a is a File Log - Does not support printing FileLogs as of now\n",logName);
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    uLogResult = ULogCore_Enable(h);
    if( ULOG_ERR_ALREADYENABLED != uLogResult)
    {
      AsciiPrint("DumpULog: ULogCore_Enable Failed\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    AsciiPrint("==========================Printing contents of ULog %a - Start==================\n",logName);
    while(1)
    {
      //Read 1 message
      uLogResult = ULogCore_ReadEx(h, sizeof(outMem), outMem, &outCount, 1);
      if(uLogResult != DAL_SUCCESS)
      {
        AsciiPrint("DumpULog: ULogCore_ReadEx Failed\n");
        break;//out of while(1)
      }
      if (outCount > 4)
      {
        uLogResult = ULogCore_MsgFormat(h, &outMem[4], msgString, ULOG_TESTAPP_MSG_STR_SIZE, &outCount);
        if(uLogResult != DAL_SUCCESS)
        {
          AsciiPrint("DumpULog: ULogCore_ReadEx Failed\n");
          break;//out of while(1)
        }
        AsciiPrint(msgString);
        Status = DAL_SUCCESS;
        if(ULOGTEST_DUMP_SHORT == eDumpType)//If short - break after the first print - This is to retain legacy behavior
        {
          break;
        }
      }
      else
      {
        /*Rewind ReadIndex - so that next time we read the same log -it dumps from the start*/
        uLogResult = ULogCore_ReadSessionComplete(h);
        if(uLogResult != DAL_SUCCESS)
        {
          AsciiPrint("DumpULog: ULogCore_ReadSessionComplete Failed\n");
          break;//out of while(1)
        }
        log->readerCore->read = log->writerCore->readWriter;
        uLogResult = DAL_SUCCESS;
        break;//break of out while(1) as we have read all messages
      }
    }
    AsciiPrint("==========================Printing contents of ULog %a - End==================\n",logName);
    if (uLogResult != DAL_SUCCESS)
    {
      Status = EFI_INVALID_PARAMETER;
    }
    else
    {
      Status = EFI_SUCCESS;
    }
  }while(0);

  FreePool( msgString );
  return  Status;

}




void ULogTestApp_PrintULogList(void)
{
  int nameCnt;
  AsciiPrint("\nULog List: \n"); 
  AsciiPrint("ULogList Count: %d \n",ULogInfoListCnt); 
  for(nameCnt = 0; nameCnt < ULogInfoListCnt ; nameCnt++)
  {
    AsciiPrint("%-20a",arrULogInfoList[nameCnt].LogName); 
    if (ULOG_TRANSPORT_ALTERNATE == arrULogInfoList[nameCnt].transport)
    {
      AsciiPrint("[%a]",arrULogInfoList[nameCnt].FileLogPath);//For eg: [LOGFS:\\ChargerLog.txt]
    }
    AsciiPrint("\n");
  }
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
ULogResult ULogFileTest_CheckEnabled(ULogHandle *h, ULOG_CORE_VTABLE *core) {
    ULogResult Status = ULOG_ERR_NOTSUPPORTED;
    if (NULL == h || NULL == core) {
        DEBUG(( EFI_D_ERROR, "ULogFileTest_CheckEnabled: Invalid parameter \n" ));
        return ULOG_ERR_INVALIDPARAMETER;
    }
    if ( TRUE == ULogCore_IsEnabled(h, &core)) {
        DEBUG((EFI_D_INFO, "ULogFileTest: Log is enabled => successful creation. \n"));
        Status = ULOG_ERR_ALREADYENABLED;
    } else {
        DEBUG((EFI_D_INFO, "Enabling log\n" ));
        Status = ULogCore_Enable(h);
        DEBUG((EFI_D_INFO, "ULogFileTest: Log enable returned %d \n", Status));
    }
    return Status;
}

/**
 **/
EFI_STATUS ULogFileTest(void)
{
    ULogResult Status = EFI_SUCCESS;
    ULogHandle ProfileHdl; // for profiling UEFI File Logging
    UINT32 logsize = 2*1024*1024; 

    /*
     * The plan here is to create a bunch of sub-tests. Eventually, each section of this
     * function is going to be its own test.
     */
    //TEST_START("ULOG_FILE_TEST RL");
    AsciiPrint("\nRunning tests in this order:\n"
               "> Open\n"
               "> Enable\n"
               "> Profile\n"
               "> Write\n");

    // Get rid of the whitespace that TEST_START adds, so printfs appear properly
    AsciiPrint("\n");

    //open a valid ulog file

    // UEFI File Logging:
    Status = ULogFile_Open(&ProfileHdl, (CONST CHAR8 *)PROFILE_LOG_NAME, logsize);

    // Memory Logging (For reference):
    // Status = ULogFront_RealTimeInit(&ProfileHdl, PROFILE_LOG_NAME, logsize, (UINT32) ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);

    if (Status != DAL_SUCCESS && Status != ULOG_ERR_ALREADYCREATED) {
        AsciiPrint("Didn't open test log %a, ERROR %d \n", PROFILE_LOG_NAME, Status);
        TestStatus("ULOG_FILE_TEST", Status);
        TEST_STOP("ULOG_FILE_TEST");
        return Status;
    } else {
        Status = EFI_SUCCESS;
    }

    AsciiPrint("Querying config information\n");
    ULOG_CONFIG_TYPE settings;
    Status = ULogFile_GetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("GetConfig failed\n");
      TestStatus("ULOG_FILE_TEST", Status);
      TEST_STOP("ULOG_FILE_TEST");
      return Status;
    }

    AsciiPrint("Separator = '%c'\n", settings.separator);
    AsciiPrint("PrintTimestamp = %d\n", settings.PrintTimestamp);
    AsciiPrint("TrimNewline = %d\n", settings.TrimNewline);

    // Profile UEFI File Logging
    CHAR8 *ShortMessage = "Shooort";  // 8 Bytes (7 chars + \0)
    UINT32 ShortLength = AsciiStrSize(ShortMessage);
//    CHAR8 *LongMessage = "Loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong"; // 64 Bytes
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
      TestStatus("ULOG_FILE_TEST", Status);
      TEST_STOP("ULOG_FILE_TEST");
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
      TestStatus("ULOG TEST", EFI_INVALID_PARAMETER);
      TEST_STOP("ULOG TEST");
      return Status;
    }

    AsciiPrint("Changing separator to ','\n");
    settings.separator = ',';
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("Couldn't apply valid separator. Stopping test\n");
      TestStatus("ULOG TEST", Status);
      TEST_STOP("ULOG TEST");
      return Status;
    }
    AsciiPrint("Enabling timestamp and disabling newline trimming for long messages\n");
    settings.PrintTimestamp = TRUE;
    settings.TrimNewline = FALSE;
    Status = ULogFile_SetConfig(&ProfileHdl, &settings);
    if (Status != EFI_SUCCESS){
      AsciiPrint("Timestamp enable failed\n");
      TestStatus("ULOG_FILE_TEST", Status);
      TEST_STOP("ULOG_FILE_TEST");
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

    TestStatus("ULOG TEST", Status);
    //TEST_STOP("ULOG TEST");
    return Status;
}

EFI_STATUS ULogFileTest_InitLogDirectoryAndSize()
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
      sTestLogDirectory = LogDir;
    } else {
      // couldn't read for some reason, use the default value
      DEBUG(
          (EFI_D_WARN, "ULogFile: Couldn't read the special log partition, Error %r. Using default directory\n", Status));
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


ULogResult ULogFileTest_MakePathFromName(CONST CHAR8 *LogName, CHAR8 *FilePath)
{

  if (LogName == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Received request for NULL file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (AsciiStrCmp(LogName, "") == 0) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Received request for Empty file name \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }
  if (FilePath == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Received request for NULL destination \n" ));
    return ULOG_ERR_INVALIDPARAMETER;
  }

  CHAR8 *PathStr;
  UINT32 length = AsciiStrLen(sTestLogDirectory) + AsciiStrLen(LogName) + AsciiStrLen(ULOGFILE_EXTENSION) + 1;

  PathStr = AllocateZeroPool(length);
  if (PathStr == NULL) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Couldn't allocate zero'd buffer for path \n" ));
    return ULOG_ERR_MALLOC;
  }

  // Concatenate the strings in the format directory/filename.extension
  length = AsciiStrLen(sTestLogDirectory);
  AsciiStrCpy(PathStr, sTestLogDirectory);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Couldn't copy sLogDirectory string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(LogName);
  AsciiStrCat(PathStr, LogName);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Couldn't copy LogName string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  length += AsciiStrLen(ULOGFILE_EXTENSION);
  AsciiStrCat(PathStr, ULOGFILE_EXTENSION);
  if (AsciiStrLen(PathStr) < length) {
    DEBUG(( EFI_D_WARN, "ULogFileTest_MakePathFromName: Couldn't copy ULOGFILE_EXTENSION string \n" ));
    FreePool(PathStr);
    return DAL_ERROR;
  }

  StrnCpy((CHAR16 *) FilePath, (CONST CHAR16 *) PathStr, length);

  FreePool(PathStr);
  return EFI_SUCCESS;
}

//Returns 1 if PASS, 0 if FAIL
int ULogTestApp_CheckDriveExists(void)
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
int ULogTestApp_CheckDriveExists(void)
{
  CHAR8 FilePath[MAX_PATHNAME];
  SetMem(FilePath, MAX_PATHNAME, '\0');
  ULogResult Result = ULogFileTest_MakePathFromName("ULogTest", FilePath);
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
void ULogTestApp_PrintHelp(void)
{

AsciiPrint("\n Usage: \n");
AsciiPrint("==============================================================================\n");
AsciiPrint("ULogTest <NoParam>            - Dumps NPA ULog - Retaining for legacy reasons \n");
AsciiPrint("ULogTest list     <NoParams>  - Lists all availabe logs \n");
AsciiPrint("ULogTest \"dump     <LogName>\"   - Dumps <LogName> \n");
AsciiPrint("   Please ensure to use quotes as above ; LogName is case-sensitive\n");
AsciiPrint("ULogTest dump     <NoParams>  - Dumps All Logs \n");
AsciiPrint("ULogTest file     <NoParams>  - Test/Profile FileLogging \n");
AsciiPrint("ULogTest help     <NoParam>   - Prints Usage \n");
AsciiPrint("==============================================================================\n");

}
