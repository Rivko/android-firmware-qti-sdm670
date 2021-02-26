/*! @file ClockFileLog.c
 *
 *  CLOCK FILE Logging
 *
 *  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/17   op      Enable File Logging

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
#include "ClockFileLog.h"


/**
Protocol Dependencies
*/
#include <Protocol/EFIPlatformInfo.h>

/**
UEFI DEBUG Dependencies
*/
#include <Library/DebugLib.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/

/*===========================================================================
                    GLOBAL VARIABLE
=============================================================================*/
static BDS_CLOCKLOG_STATE   gnEBSClockLogSetting;

/*===========================================================================
                               LOCAL FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/
static ULogHandle  gClockULogHandle = NULL;
/*===========================================================================
                  FUNCTION DEFINATIONS
===========================================================================*/

void EFIAPI
ULogPrint (
  IN  UINTN        ErrorLevel,
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

  SetMem(CurrentFormat, sizeof(CurrentFormat), 0x00);

  /* If volume corrupted do not attempt to log */
  if (EFI_VOLUME_CORRUPTED == lastLogResult)
    return;

  for (i = 0, dataCount = 0; (NULL != (Format + i)) && (Format[i] != '\0'); i++, CurrentFormatIndex++)
  {
   CurrentFormat[CurrentFormatIndex] = Format[i];
   if (Format[i] == '%' && !boEsc)
   {
     if(Format[i + 1] == 'a')
     {
       appendFlag = TRUE;
       VA_START(vlist, Format);
       FnName = (CHAR8* ) va_arg(vlist, CHAR8*);
       AsciiVSPrint((CurrentFormat + CurrentFormatIndex), size, FnName, vlist);
       CurrentFormatIndex = CurrentFormatIndex + AsciiStrLen(FnName);
       CurrentFormat[CurrentFormatIndex] = ' ';
       i+=2;
     }else if (Format[i + 1] == 'r')
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

  if(appendFlag == FALSE)
  {
    VA_START(vlist, Format);
  }
  if(appendFlag)
  {
    CurrentLogResult = (EFI_STATUS ) ULogFront_RealTimeVprintf(gClockULogHandle,  dataCount, CurrentFormat, vlist);
  }
  else
  {
    CurrentLogResult = (EFI_STATUS ) ULogFront_RealTimeVprintf(gClockULogHandle,  dataCount, Format, vlist);
  }
  if(CurrentLogResult == EFI_VOLUME_CORRUPTED)
  {
    DEBUG((EFI_D_WARN, "ClockDxe:: %a File Log Print Volume Error = %r \r\n",__FUNCTION__, CurrentLogResult));
    lastLogResult = CurrentLogResult;
  }else if (CurrentLogResult != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN, "ClockDxe:: %a File Log Print Error = %r \r\n",__FUNCTION__, CurrentLogResult));
  }else { /* SUCCESS use case */}

   VA_END(vlist);
}

EFI_STATUS ClockFileLog_GetEBSClockLogSetting(BDS_CLOCKLOG_STATE *eLogState)
{
  /* Get BDS menu ULOG setting from BDS Menu */
  /* Get BDS menu ULOG setting from BDS Menu */
  EFI_STATUS       eStatus;
  UINTN            VarSize;
  INT8             eClockLogMode = BDSClockLogStateDisable;

  *eLogState = BDSClockLogStateDisable;
  gnEBSClockLogSetting = BDSClockLogStateDisable;

  // for testing BDs Menu only
  VarSize = sizeof(eClockLogMode);

  eStatus = gRT->GetVariable (BDS_CLOCKLOG_STATE_VARNAME, &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &eClockLogMode);
  if (eStatus != EFI_SUCCESS) {
    DEBUG(( EFI_D_ERROR, "ClockDxe: Can't dump Clock's state. Please set your choice as following: \r\n"));
    DEBUG(( EFI_D_ERROR, ">>> Select <CLOCK Menu> under BDS Menu  \r\n", __FUNCTION__, eStatus));
    DEBUG(( EFI_D_ERROR, ">>> Select <Toggle Logging Clocks states> \r\n", __FUNCTION__, eStatus));
    DEBUG(( EFI_D_ERROR, ">>> Select <1> to dump clock state to UART \r\n", __FUNCTION__, eStatus));
  }

  *eLogState = eClockLogMode;
  /* Save ClockLogSetting to  global variables */
  gnEBSClockLogSetting = eClockLogMode;
  return eStatus;
  }

/* Init File Logging in ULog */
EFI_STATUS ClockFileLog_InitFileLog(ULogHandle *ULhandle)
{
  /* Get BDS menu ULOG setting from BDS Menu */
  EFI_STATUS         Status     =  EFI_SUCCESS;
  ULogResult         result     =  0;
  ULOG_CONFIG_TYPE   configType = {0};
  BOOLEAN            FileLoggingIsEnabled;
  UINTN              VarSize;

  if (!RETAIL)
  {
    /* Mounting Log fs */
    Status = MountFatPartition(L"logfs");
    if (Status != EFI_SUCCESS){
      DEBUG((EFI_D_ERROR, "ClockDxe: %a LogFS mount Status False = %r \r\n", __FUNCTION__, Status));
      return (Status);
    }
  }

  FileLoggingIsEnabled = TRUE;
  VarSize = sizeof(FileLoggingIsEnabled);

  Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
                               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                               VarSize, &FileLoggingIsEnabled);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "ClockDxe:: %a Set ULog file Logging False =  %d \r\n", __FUNCTION__, Status));
    return EFI_UNSUPPORTED;
  }

  if (!FileLoggingIsEnabled)
  {
    DEBUG(( EFI_D_WARN, "ClockDxe:: ULog file Logging is disabled. \n"));
    return EFI_UNSUPPORTED;
  }

  gClockULogHandle = NULL;
  result = ULogFile_Open(&gClockULogHandle, CLOCKLOGFILE_IN_EFS, CLOCKLOGFILE_SIZE);
  if ((0 == result) && (gClockULogHandle!=NULL))
  {
     /* Set ULog configuration */
     ULogFile_GetConfig(&gClockULogHandle, &configType);
     configType.separator = ',';
     configType.PrintTimestamp = TRUE;
     configType.TrimNewline = TRUE;
     Status = ULogFile_SetConfig(&gClockULogHandle, &configType);
     if (Status != EFI_SUCCESS){
       DEBUG(( EFI_D_ERROR, "ClockDxe:: ClockFileLog_InitFileLog - SetConfig FAILED \r\n"));
       return EFI_INVALID_PARAMETER;
     }
     DEBUG(( EFI_D_WARN, "ClockDxe:: ClockFileLog_InitFileLog SUCCESS gClockULogHandle = %d \r\n", gClockULogHandle));
   }
   else
   {
     DEBUG(( EFI_D_WARN, "ClockDxe:: ClockFileLog_InitFileLog FAILED - Clock Logging is Disabled\r\n"));
     return EFI_UNSUPPORTED;
   }
   *ULhandle = gClockULogHandle;
   return EFI_SUCCESS;
}

void Clock_GetgClockLogSetting(BDS_CLOCKLOG_STATE *eLogSetting)
{
  *eLogSetting = gnEBSClockLogSetting;
}