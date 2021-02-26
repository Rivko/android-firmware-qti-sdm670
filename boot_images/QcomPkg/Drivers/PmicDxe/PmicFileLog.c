/*! @file PmicFileLog.c 
 *
 *  PMIC FILE Logging
 *
 *  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/17   ll      Implemented level file-logging
01/27/17   ai      Fix use of retail
09/29/16   va      update to print function names %a and handle %r format specifier
07/07/16   va      DEBUG print to print time stamp in seconds
04/26/16   va      volume Error handling for file Log
03/30/16   va      update global file logging flag 
10/23/15   va      Time stamp for UART logging
12/09/14   al      Sync with latest 
08/21/14   va      Enable File Logging 

===========================================================================*/

/*===========================================================================
                     CONSTANTS FOR MODULE
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomTargetLib.h>

#include <Library/BaseMemoryLib.h>
#include "string.h"
#include <PrintShLib.h>

#include "pm_uefi.h"
#include "PmicFileLog.h"
/** 
Protocol Dependencies 
*/ 
#include <Protocol/EFIPlatformInfo.h>


/*===========================================================================
                               LOCAL FUNCTION DECLARATIONS 
===========================================================================*/


/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/
STATIC ULogHandle  gULogHandle = NULL;
STATIC BOOLEAN     PrintChargerAppDbgMsg = FALSE;
STATIC BOOLEAN     PrintChargerAppDbgMsgToFile = FALSE;
UINT32 gFileLogLevelMask = 0;

/*===========================================================================
                  FUNCTION DEFINATIONS
===========================================================================*/
/**
PmicFileLog_PrintDebugMsg()

@brief
Returns PmicFileLog_PrintDebugMsg Flag Status
*/
BOOLEAN
EFIAPI
  PmicFileLog_PrintDebugMsg(void)
{
  return PrintChargerAppDbgMsg;
}

/**
PmicFileLog_PrintDebugMsgToFile()

@brief
Returns PmicFileLog_PrintDebugMsgToFile Flag Status
*/
BOOLEAN
EFIAPI
  PmicFileLog_PrintDebugMsgToFile(void)
{
  //TBD ULOG to file dump
  if (NULL != gULogHandle)
    return PrintChargerAppDbgMsgToFile;
  return FALSE;
}


void EFIAPI
ULogPrint (
  IN  UINTN        LogLevel,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  va_list vlist;
  UINT32  dataCount = 0;
  UINTN   i         = 0;
  BOOLEAN  boEsc = FALSE;
  STATIC EFI_STATUS lastLogResult = EFI_SUCCESS;
  EFI_STATUS CurrentLogResult     = EFI_SUCCESS;
  CHAR8 CurrentFormat[512];
  UINTN CurrentFormatIndex = 0;
  UINT32 size = sizeof(CurrentFormat);
  BOOLEAN appendFlag = FALSE;
  CHAR8 *FnName = NULL;

  if ((LogLevel & gFileLogLevelMask) != 0)
  {
      SetMem(CurrentFormat, sizeof(CurrentFormat), 0x00);

      /* If volume corrupted do not attempt to log */
      if (EFI_VOLUME_CORRUPTED == lastLogResult)
          return;

      for (i = 0, dataCount = 0; (NULL != (Format + i)) && (Format[i] != '\0'); i++, CurrentFormatIndex++)
      {
          CurrentFormat[CurrentFormatIndex] = Format[i];
          if (Format[i] == '%' && !boEsc)
          {
              if (Format[i + 1] == 'a')
              {
                  appendFlag = TRUE;
                  VA_START(vlist, Format);
                  FnName = (CHAR8*)va_arg(vlist, CHAR8*);
                  AsciiVSPrint((CurrentFormat + CurrentFormatIndex), size, FnName, vlist);
                  CurrentFormatIndex = CurrentFormatIndex + AsciiStrLen(FnName);
                  CurrentFormat[CurrentFormatIndex] = ' ';
                  i += 2;
              }
              else if (Format[i + 1] == 'r')
              {
                  CurrentFormat[++CurrentFormatIndex] = 'd';
                  i++;
                  dataCount++;
              }
              else
              {
                  dataCount++;
              }
          }
          else if (Format[i] == '\\' && !boEsc)
          {
              boEsc = TRUE;
          }
          else if (boEsc)
          {
              boEsc = FALSE;
          }
      }
      /* Append end of the string */
      CurrentFormat[CurrentFormatIndex] = '\0';

      if (appendFlag == FALSE)
      {
          VA_START(vlist, Format);
      }
      if (appendFlag)
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, CurrentFormat, vlist);
      }
      else
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, Format, vlist);
      }
      if (CurrentLogResult == EFI_VOLUME_CORRUPTED)
      {
          PMIC_UART_DEBUG((EFI_D_WARN, "PmicDxe:: %a File Log Print Volume Error = %r \r\n", __FUNCTION__, CurrentLogResult));
          lastLogResult = CurrentLogResult;
      }
      else if (CurrentLogResult != EFI_SUCCESS)
      {
          PMIC_UART_DEBUG((EFI_D_WARN, "PmicDxe:: %a File Log Print Error = %r \r\n", __FUNCTION__, CurrentLogResult));
      }
      else { /* SUCCESS use case */ }

      VA_END(vlist);
  }
}


/* Init File Logging in ULog */
EFI_STATUS PmicFileLog_InitFileLog(BOOLEAN PrintChgAppDbgMsg, BOOLEAN PrintChgAppDbgMsgToFile, UINT32 logLevelMask)
{
  /* Get BDS menu ULOG setting from BDS Menu */
  EFI_STATUS        Status     =  EFI_SUCCESS;
  ULogResult        result     =  0;
  ULOG_CONFIG_TYPE  configType = {0};
  BOOLEAN           FileLoggingIsEnabled = TRUE;
  UINT32            VarSize = 1;

  if(FALSE != PrintChargerAppDbgMsgToFile)
  {
     return EFI_SUCCESS;
  }

  PrintChargerAppDbgMsg = PrintChgAppDbgMsg;

  VarSize = sizeof(FileLoggingIsEnabled);

  if (TRUE == PrintChgAppDbgMsgToFile)
  {
    /* Enable default file logging if production mode is disabled */
    if(!RETAIL)
    {
      Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                 VarSize, &FileLoggingIsEnabled);
      if(Status != EFI_SUCCESS)
      {
        DEBUG(( EFI_D_WARN, "PmicDxe:: %a Global File Logging BDS Menu Variable read error =  %d \r\n", __FUNCTION__, Status));
        return EFI_SUCCESS;
      }

      result = ULogFile_Open(&gULogHandle, LOGFILE_IN_EFS, LOGFILE_SIZE);
      if (0 == result)
      {
		/* Save log level mask configuration */
		gFileLogLevelMask = logLevelMask;

        ULOG_RT_PRINTF_1(gULogHandle, "PmicDxe:: InitFileLog SUCCESS gULogHandle = %lld", gULogHandle);
        /* Set ULog configuration */
        ULogFile_GetConfig(&gULogHandle, &configType);
        configType.separator = ',';
        configType.PrintTimestamp = TRUE;
        configType.TrimNewline = TRUE;
        Status = ULogFile_SetConfig(&gULogHandle, &configType);
        if (Status != EFI_SUCCESS){
          PMIC_UART_DEBUG(( EFI_D_ERROR, "PmicDxe:: ULogFile_SetConfig FAILED \r\n"));
        }
        /* Set Flag now */
        PrintChargerAppDbgMsgToFile = PrintChgAppDbgMsgToFile;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: PmicFileLog_InitFileLog SUCCESS gULogHandle = %lld \r\n", gULogHandle));
      }
      else
      {
        DEBUG(( EFI_D_ERROR, "PmicDxe:: PmicFileLog_InitFileLog FAILED \r\n"));
      }
    }
    else
    {
      /* Disable file logging flag since production mode is enabled */
      PrintChargerAppDbgMsgToFile = FALSE;
    }
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: PmicFileLog_InitFileLog File Logging is Disabled \r\n"));
  }

  return EFI_SUCCESS;
}


UINT64 PmicFileLog_TimeGet64(void)
{
  UINT64 Result = 0 ;
  Result = GetPerformanceCounter();
  return(Result);
}

UINT64 PmicFileLog_CalculateTime ()
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = PmicFileLog_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000000;

  return Seconds;
}


UINT32 PmicFileLog_GetFileLogLevelMask()
{
	return gFileLogLevelMask;
}