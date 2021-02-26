/** @file QcomChargerAppFileLogging.c

  Implements the Qualcomm's Charging App file logging API definitions.

  Copyright (c) 2016-2017,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 03/30/17     ll      Implemented level file-logging
 12/28/16     sm      Initial draft

=============================================================================*/
#include <Uefi.h>

/**
  EFI interfaces
 */
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <api/pmic/pm/pm_uefi.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/QcomBaseLib.h>

#include <Protocol/EFIVariableServices.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/BaseMemoryLib.h>
#include <PrintShLib.h>

/**
  EFI interfaces
 */

#include <Protocol/EFIQcomCharger.h>
#include <Protocol/EFIPlatformInfo.h>

/**
  EFI interfaces
 */
#include <Library/BaseLib.h>

#include "QcomChargerAppFileLogging.h"
#include "QcomChargerAppEventHandler.h"

/**
File Logging Dependencies 
*/
#include "ULogFront.h"
#include "ULogUEFI.h"

/*===========================================================================
                    GLOBAL VARIABLE
=============================================================================*/
STATIC EFI_QCOM_CHARGER_LOG_INFO    FileLogInfo;

EFI_STATUS QcomChargerAppFileLog_InitFileLog(VOID);
EFI_STATUS QcomChargerAppFileLog_ChargerDebugDateTime (VOID);

#define QCOM_CHARGER_LOGFILE_IN_EFS "ChargerLog" /* logfile name is file system */
#define QCOM_CHARGER_LOGFILE_SIZE   8630272 // 7MB

/*===========================================================================
                  FUNCTIONS DEFINITIONS                                    
=============================================================================*/
/**
QcomChargerAppFileLog_Init()

@brief
Returns QcomChargerAppFileLog_Init Flag Status
*/
EFI_STATUS QcomChargerAppFileLog_Init(void)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_CHARGER_PROTOCOL    *pQcomChargerProtocol  = NULL;

  SetMem(&FileLogInfo, sizeof(EFI_QCOM_CHARGER_LOG_INFO), 0x00);

  if (!RETAIL)
    {
      Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
      if(EFI_SUCCESS == Status)
      {
        Status = pQcomChargerProtocol->GetLogInfo(&FileLogInfo);
        if(EFI_SUCCESS != Status )
        {
          CHARGERAPP_DEBUG(( EFI_D_WARN, "QcomChargerApp:: %a GetFileLogInfo Failed = %r \r\n", __FUNCTION__, Status));
          Status =  EFI_SUCCESS;/* should continue as not a critical error */
          FileLogInfo.gChargerLogHandle = NULL;
        }
      }
      if(FileLogInfo.bPrintChargerAppDbgMsgToFile == TRUE)
      {
        Status = QcomChargerAppFileLog_InitFileLog();
      }
      else
      {  /* Make sure that file log handle is zero */
        FileLogInfo.gChargerLogHandle = NULL;
      }
      CHARGERAPP_DEBUG(( EFI_D_WARN, "QcomChargerApp:: %a FileHandle = %lld PrintDbgMsg = %d \r\n", __FUNCTION__, (UINT64)FileLogInfo.gChargerLogHandle, FileLogInfo.bPrintChargerAppDbgMsg));
    }

    if (!RETAIL)
    {
      Status = QcomChargerAppFileLog_ChargerDebugDateTime();
      if(EFI_SUCCESS != Status)
      {
        CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a ChargerDebugDateTime Error = %r \r\n",__FUNCTION__, Status));
        /*override status */
        Status = EFI_SUCCESS;
      }
    }

    return Status;
}

/**
QcomChargerAppFileLog_PrintDebugMsg()

@brief
Returns QcomChargerAppFileLog_PrintDebugMsg Flag Status
*/
inline BOOLEAN
EFIAPI
QcomChargerAppFileLog_PrintDebugMsg(void)
{
  return ((BOOLEAN)FileLogInfo.bPrintChargerAppDbgMsg);
}

/**
QcomChargerAppFileLog_PrintDebugMsgToFile()

@brief
Returns QcomChargerAppFileLog_PrintDebugMsgToFile Flag Status
*/
inline BOOLEAN
EFIAPI
QcomChargerAppFileLog_PrintDebugMsgToFile(void)
{
  if (NULL != FileLogInfo.gChargerLogHandle) return (TRUE);
  return FALSE;
}


UINT64 QcomChargerAppFileLog_TimeGet64(void)
{
  UINT64 Result = 0 ;
  Result = GetPerformanceCounter();
  return(Result);
}


/**
ULogPrint ()

@brief Helper API to print multiple arguments
Returns
*/
inline void EFIAPI
ULogPrint(
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

  if ((LogLevel & FileLogInfo.fileLogLevelMask) != 0)
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
      /* if(appendFlag)
      {
      va_end(vlist);
      } */
      /* Append end of the string */
      CurrentFormat[CurrentFormatIndex] = '\0';
      if (appendFlag == FALSE)
      {
          VA_START(vlist, Format);
      }

      //CHARGERAPP_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a dataCount = %d \r\n",__FUNCTION__, dataCount));

      //CHARGERAPP_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a formatstring = %a \r\n",__FUNCTION__, Format));
      //CHARGERAPP_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a Current Fn Name = %a size = %d \r\n",__FUNCTION__, CurrentFormat, size));
      if (appendFlag)
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(FileLogInfo.gChargerLogHandle, dataCount, CurrentFormat, vlist);
      }
      else
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(FileLogInfo.gChargerLogHandle, dataCount, Format, vlist);
      }
      if (CurrentLogResult == EFI_VOLUME_CORRUPTED)
      {
          CHARGERAPP_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a File Log Print Volume Error = %r \r\n", __FUNCTION__, CurrentLogResult));
          lastLogResult = CurrentLogResult;
      }
      else if (CurrentLogResult != EFI_SUCCESS)
      {
          CHARGERAPP_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a File Log Print Error = %r \r\n", __FUNCTION__, CurrentLogResult));
      }
      else { /* SUCCESS use case */ }

      VA_END(vlist);
  }
}


/**
  QcomChargerApp_ChargerDebugDateTime ()

  @brief
  Debug Date & time
 */
EFI_STATUS QcomChargerAppFileLog_ChargerDebugDateTime (void)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_TIME   Time = {0};
  UINTN      CpuFrequency = 0;
  UINT64     Seconds = 0;
  UINT64     TotalTicks = 0;

  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  TotalTicks   = QcomChargerAppFileLog_TimeGet64();
  Seconds      = QcomChargerAppFileLog_CalculateTime();
  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a CpuFrequency = %lld Hz Seconds = %lld TotalTicks = %lld \r\n",__FUNCTION__, CpuFrequency, Seconds, TotalTicks));

  /* Print Date time once */
  Status = gRT->GetTime(&Time, NULL);
  if (Status == EFI_SUCCESS)
  {
    CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: Date(year/month/Day):Time(Hour/Minute/sec/nano/timezone/daylight) "));
	CHARGERAPP_DEBUG((EFI_D_WARN, "-> %d/%d/%d:%d/%d/%d/%d/%d/%d \r\n",
            Time.Year, Time.Month, Time.Day, Time.Hour, Time.Minute, Time.Second, Time.Nanosecond, Time.TimeZone, Time.Daylight));
  }
  else
  {
    CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Error in Getting systems Date and Time: 0x%08X \r\n", Status));
    Status = EFI_SUCCESS;
  }

  return Status;
}

UINT64 QcomChargerAppFileLog_CalculateTime (void)
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = QcomChargerAppFileLog_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000000;

  return Seconds;
}


EFI_STATUS QcomChargerAppFileLog_LogParam(EFI_QCOM_CHARGER_ACTION_INFO *ChargerActionInfo)
{
  CHARGERAPP_DEBUG(( EFI_D_WARN, "QcomChargerApp:: %a StateOfCharge = %d, VBat = %d mV Current = %d mA Temperature = %d C \r\n",
                           __FUNCTION__, ChargerActionInfo->BatteryStatusInfo.StateOfCharge, ChargerActionInfo->BatteryStatusInfo.BatteryVoltage,
                           ChargerActionInfo->BatteryStatusInfo.ChargeCurrent, ChargerActionInfo->BatteryStatusInfo.BatteryTemperature));

  /* DEBUG when charging is progress and charger logs are disabled */
  CHARGERAPP_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: Battery Status %d,%d,%d,%d \r\n", ChargerActionInfo->BatteryStatusInfo.StateOfCharge,
                              ChargerActionInfo->BatteryStatusInfo.BatteryVoltage, ChargerActionInfo->BatteryStatusInfo.ChargeCurrent,
                              ChargerActionInfo->BatteryStatusInfo.BatteryTemperature));

  return EFI_SUCCESS;
}


/**
QcomChargerAppFileLog_InitFileLog( )

@brief  Returns File long handle if alreayd initilized
Returns Status
*/
EFI_STATUS QcomChargerAppFileLog_InitFileLog(VOID )
{
  ULogResult       result     = 0;
  ULOG_CONFIG_TYPE configType = {0};

  if(NULL != FileLogInfo.gChargerLogHandle)
  {
    result = ULogFile_Open(&(FileLogInfo.gChargerLogHandle), QCOM_CHARGER_LOGFILE_IN_EFS, QCOM_CHARGER_LOGFILE_SIZE);
    if (0 == result)
    {
      /* Set ULog configuration */
      ULogFile_GetConfig(&(FileLogInfo.gChargerLogHandle), &configType);
      configType.separator = ',';
      configType.PrintTimestamp = TRUE;
      configType.TrimNewline = TRUE;
      ULogFile_SetConfig(&FileLogInfo.gChargerLogHandle, &configType);
      ULOG_RT_PRINTF_1(FileLogInfo.gChargerLogHandle, "QcomChargerApp::InitFileLog SUCCESS gULogHandle = %lld", FileLogInfo.gChargerLogHandle);
      CHARGERAPP_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a InitFileLog SUCCESS \r\n",  __FUNCTION__));
    }
    else
    {
      CHARGERAPP_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a InitFileLog FAILED \r\n",  __FUNCTION__));
      FileLogInfo.gChargerLogHandle = NULL;
    }
  }

   return EFI_SUCCESS;
}

