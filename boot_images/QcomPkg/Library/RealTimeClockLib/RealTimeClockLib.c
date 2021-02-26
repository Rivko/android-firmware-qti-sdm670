/** @file
  Implement EFI RealTimeClock runtime services via RTC Lib.

  Currently this driver does not support runtime virtual calling.

  Copyright (c) 2010-2015, Qualcomm Technologies Inc. All rights
  reserved. Portions copyright (c) 2008 - 2009, Apple Inc. All
  rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/03/15   jb      Add timer based runtime support
 07/28/15   jb      Add runtime mapping for time sync
 07/22/15   dks     Add time sync support
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 03/02/15   jb      Fix order of pointer conversion
 02/12/15   dg      Update runtime behavior
 10/18/14   jb      Add BMC support, allow PMIC less behavior
 05/30/14   cp      Using new PMIC PWRON API to detect power loss to RTC
 10/16/12   cp      Changed RTC driver to use offsets and make RTC HW R/O
 09/12/12   al      Removed resource index parameter
 07/27/12   al      Added value for device index and resource index
 06/01/12   cp      Added support for storing TimeZone info in persistent storage
 05/09/12   cp      Added support for handling EFI_UNSPECIFIED_TIMEZONE and adjusting for local time
 04/08/12   cp      Added checks for input parameters for LibGetTime and LibSetTime
 03/15/12   cp      Changed LibGetTime to return local timezone if timezone is unspecified
 06/28/11   yg      Added dependency on PMIC RTC protocol

=============================================================================*/

#include "RtcConversions.h"
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/RealTimeClockLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/EFIPmicRTC.h>
#include <Guid/EventGroup.h>
#include <Library/TimerLib.h>
#include <Library/BmcRealTimeClockLib.h>
#include <Library/UefiCfgLib.h>

/**
Definitions for number of seconds used for calculations
*/
#define RTC_SECONDS_PER_MINUTE    60
#define RTC_SECONDS_PER_HOUR      RTC_SECONDS_PER_MINUTE * 60
#define RTC_SECONDS_PER_DAY       RTC_SECONDS_PER_HOUR * 24
#define RTC_SECONDS_PER_YEAR      RTC_SECONDS_PER_DAY * 365
#define RTC_SECONDS_PER_LEAP_YEAR RTC_SECONDS_PER_DAY * 366

extern EFI_GUID gQcomTokenSpaceGuid;

/* Interface for the protocol to access the RTC */
EFI_QCOM_PMIC_RTC_PROTOCOL    *PmicRtc = NULL;
VOID      *PmicRtcNotificationToken = NULL;
EFI_EVENT PmicRtcNotificationEvent  = (EFI_EVENT)NULL;

/**
TimeSync related structures
*/
extern void TimeSyncMutexLock (void *mutex);
extern void TimeSyncMutexUnlock (void *mutex);

/* Shared MEM address and size holding the time sync related dtaa */
#ifndef RTC_TIME_SYNC_SMEM_ADDR
  #define RTC_TIME_SYNC_SMEM_ADDR (0x10FF80)
#endif
#ifndef RTC_TIME_SYNC_SMEM_SIZE
  #define RTC_TIME_SYNC_SMEM_SIZE (128)
#endif

/* Magic to check sanity of smem region */
#define TIME_SYNC_MAGIC 0x54534E43

/* Structure to hold the data needed to sync time from applications processor
   The data is shared by keeping it in Shared RAM, so the structure should
   exactly match on each proc that writes to and reads from it. */
typedef struct __attribute__((__packed__))
{
  UINT32 TimeSyncMagic;
  UINT32 UpdateStatus;
  UINT32 Mutex;
  UINT64 SyncOffset;
} TimeSyncSharedDataType;

typedef struct{
  UINT32 Offset;
  INT16  TimeZone;
  UINT8  Valid;
  UINT8  Pad;
  TimeSyncSharedDataType *TimeSyncData;
} RTC_Info_Type_Struct;

static RTC_Info_Type_Struct RTCInfo;

/**
Store if the time is affected by daylight savings.
*/
UINT8 UEFI_Daylight = EFI_TIME_IN_DAYLIGHT;


/**
Store offset between Timer and PMIC RTC.
*/
STATIC INT64 RTCTimerOffset = 0;

/**
Device Index to RTC
*/
STATIC UINT32 RTCPmicDeviceIndex = 0;

#include <Library/TimerLib.h>
/**
The Event used to detect when we are in runtime mode.
*/
EFI_EVENT mRtcVirtualAddrChangeEvent;

/**
This function gets the current timezone information from the SCM
driver
*/
static
EFI_STATUS
RealTimeClock_GetTimeZone (
  OUT INT16               *TimeZone
  );

/**
This function sets the timezone information in the SCM driver
*/
static
VOID
RealTimeClock_SetTimeZone (
  IN INT16                TimeZone
  );

/**
This function gets the current timezone information from the SCM
driver
*/
static
EFI_STATUS
RealTimeClock_GetOffset (
  OUT UINT32              *pOffset
  );

/**
This function sets the timezone information in the SCM driver
*/
static
VOID
RealTimeClock_SetOffset (
  IN UINT32               Offset
  );

static
EFI_STATUS
RealTimeClock_CheckSpecParameters (
  IN  EFI_TIME               *Time
  );

static
VOID
RealTimeClock_InitializeVariables (
  VOID
  );

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time                  A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities          An optional pointer to a buffer to receive the real time clock
                                device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware error.

**/
EFI_STATUS
EFIAPI
LibGetTime (
  OUT EFI_TIME                *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities
  )
{
  EFI_STATUS Status;
  UINT64 TimeInSecondsTimer = 0;
  UINT32 TimeInSeconds = 0;
  UINT32 TimeOffset = 0;
  EFI_PM_RTC_JULIAN_TYPE pmic_time;
  UINT64 Nanoseconds = GetPerformanceCounter() % GetPerformanceCounterProperties(NULL, NULL);
  Nanoseconds *= (1000000000 / GetPerformanceCounterProperties(NULL, NULL));

  if(Time == NULL) {
    DEBUG((EFI_D_INFO, "RTCGetTime Time ptr is NULL, Return\n"));
    return EFI_INVALID_PARAMETER;
  }

  TimeInSecondsTimer = GetPerformanceCounter() / GetPerformanceCounterProperties(NULL, NULL);
  if ((PmicRtc != NULL) && !EfiAtRuntime()) {
    /* Get raw time value from pmic RTC */
    Status = PmicRtc->GetTimeValueRaw(RTCPmicDeviceIndex, &TimeInSeconds);

    /* If RTC is not ticking, let's make it run */
    if (Status != EFI_SUCCESS) {
        /* This is the first set, let's set RTC to 0 by setting epoch time */
      SetMem(&pmic_time, sizeof(EFI_PM_RTC_JULIAN_TYPE), 0);
      pmic_time.day = 6; /* 06 */
      pmic_time.month = 1; /* Jan */
      pmic_time.year = 1980; /* 1980 */
      Status = PmicRtc->SetTimeValue(RTCPmicDeviceIndex, &pmic_time);
      if( Status != EFI_SUCCESS ) {
        DEBUG((EFI_D_INFO, "RTCGet Cannot Initialize PMIC RTC\n"));
        return Status;
      }

      PmicRtc->GetTimeValueRaw(RTCPmicDeviceIndex, &TimeInSeconds);
    }
    RTCTimerOffset = TimeInSeconds - TimeInSecondsTimer;
  } else {
    TimeInSeconds = TimeInSecondsTimer + RTCTimerOffset;
  }

  RealTimeClock_GetOffset(&TimeOffset);

  /* Add the offset to the time from RTC to get local time */
  TimeInSeconds += TimeOffset;

  RealTimeClock_GetTimeZone(&Time->TimeZone);

  /* Check to see if timezone if okay after adjusting the offset */
  if(Time->TimeZone < 0 &&
     (UINT32)(-Time->TimeZone) > TimeInSeconds) {
    return EFI_DEVICE_ERROR;
  }

  /* Adjust for TimeZone */
  if(Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE) {
    if(Time->TimeZone >= 0) {
      TimeInSeconds += (UINT32)(Time->TimeZone);
    }
    else
    {
      TimeInSeconds -= (UINT32)(-Time->TimeZone);
    }
  }

  app_rtc_julian_from_secs(TimeInSeconds, &pmic_time);

  Time->Year = pmic_time.year;
  Time->Month = pmic_time.month;
  Time->Day = pmic_time.day;
  Time->Hour = pmic_time.hour;
  Time->Minute = pmic_time.minute;
  Time->Second = pmic_time.second;
  Time->Pad1 = 0;
  Time->Nanosecond = Nanoseconds;

  Time->Daylight = UEFI_Daylight;

  Time->Pad2 = 0;

  if (Capabilities != NULL) {
      Capabilities->Accuracy = PcdGet32(PcdRealTimeClockAccuracy);
      Capabilities->Resolution = PcdGet32(PcdRealTimeClockResolution);
      if (PcdGet32(PcdRealTimeClockSetToZero)) {
        Capabilities->SetsToZero = TRUE;
      }
      else{
        Capabilities->SetsToZero = FALSE;
      }
  }

  return EFI_SUCCESS;
}


/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
LibSetTime (
  IN EFI_TIME                *Time
  )
{

  EFI_STATUS Status;
  INT16 TimeZone = 0;
  UINT32 TimeInSeconds = 0;
  UINT32 RTCVal = 0;
  UINT64 TimerRTCVal = 0;
  EFI_PM_RTC_JULIAN_TYPE pmic_time;

  if(Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /* Let's check the input parameters to see if they are valid */
  if( RealTimeClock_CheckSpecParameters(Time) != EFI_SUCCESS ) {
    return EFI_INVALID_PARAMETER;
  }

    /* Now that the range has been reduced, let's find the device specific errors */
  if( Time->Year  < 1980  ||
      Time->Year  > 2115  ||
      (Time->Year  == 1980 && Time->Month == 1 && Time->Day < 6) ||
      (Time->Year  == 2115 && Time->Month > 2) ||
      (Time->Year  == 2115 && Time->Month == 2 && Time->Day > 11) ||
      (Time->Year  == 2115 && Time->Month == 2 && Time->Day == 11 && Time->Hour > 8) ||
      (Time->Year  == 2115 && Time->Month == 2 && Time->Day == 11 && Time->Hour == 8 && Time->Minute > 8) ||
      (Time->Year  == 2115 && Time->Month == 2 && Time->Day == 11 && Time->Hour == 8 && Time->Minute == 8 && Time->Second > 15) ) {
    return EFI_DEVICE_ERROR;
  }

  pmic_time.year = Time->Year;
  pmic_time.month = Time->Month;
  pmic_time.day = Time->Day;
  pmic_time.hour = Time->Hour;
  pmic_time.minute = Time->Minute;
  pmic_time.second = Time->Second;

  //Day of week is set to a nonce since UEFI does not need it.
  //This should not affect PMIC timekeeping either if it is a valid entry.
  pmic_time.day_of_week = 0;

  TimeInSeconds = app_rtc_julian_to_secs(&pmic_time);

  /* If they don't specify a timezone, we want to set the local time */
  if(Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE) {
    Status = RealTimeClock_GetTimeZone(&TimeZone);
    /* If we don't currently have a timezone stored, we will get a failure */
    if(Status != EFI_SUCCESS) {
      /* Assume UTC if the current timezone is not specified */
      TimeZone = 0;
      Status = EFI_SUCCESS;
    }
  }
  else
  {
    /* Populate Timezone with the correct value for the correction operation below */
    TimeZone = Time->TimeZone;
  }

  /* Check error case for timezone being larger than the time set in seconds from epoch */
  if(TimeZone < 0 &&
     (UINT32)(TimeZone) > TimeInSeconds) {
    return EFI_DEVICE_ERROR;
  }

  /* Adjust for UTC Time */
  if(TimeZone >= 0) {
    TimeInSeconds -= (UINT32)(TimeZone);
  }
  else
  {
    TimeInSeconds += (UINT32)(-TimeZone);
  }

  TimerRTCVal = GetPerformanceCounter() / GetPerformanceCounterProperties(NULL, NULL);
  if ((NULL != PmicRtc) && !EfiAtRuntime()) {
    /* Get raw time value from pmic RTC */
    /* Note: If RTC is greater than time set, then offset will very large! */
    /* Also, If RTC is large, the dynamic range significantly reduces */
    Status = PmicRtc->GetTimeValueRaw(RTCPmicDeviceIndex,&RTCVal);

    /* If RTC is not ticking, let's make it run */
    if (Status != EFI_SUCCESS) {
      /* This is the first set, let's set RTC to 0 by setting epoch time */
      SetMem(&pmic_time,sizeof(EFI_PM_RTC_JULIAN_TYPE),0);
      pmic_time.day = 6; /* 06 */
      pmic_time.month = 1; /* Jan */
      pmic_time.year = 1980; /* 1980 */
      Status = PmicRtc->SetTimeValue(RTCPmicDeviceIndex,&pmic_time);
      if( Status != EFI_SUCCESS ) {
        DEBUG((EFI_D_INFO, "RTCSet Cannot set PMIC RTC\n"));
        return Status;
      }

      PmicRtc->GetTimeValueRaw(RTCPmicDeviceIndex,&RTCVal);
    }
    RTCTimerOffset = RTCVal - TimerRTCVal;
  } else {
    RTCVal = TimerRTCVal + RTCTimerOffset;
  }

  if(RTCVal > TimeInSeconds) {
    /* We cannot support this time being set since the RTC value exceeds the time */
    return EFI_DEVICE_ERROR;
  }

  /* Set the offset as the difference between the two */
  RealTimeClock_SetOffset(TimeInSeconds - RTCVal);

  /* Let's set timezone info after we set on pmic incase of power loss */
  UEFI_Daylight = Time->Daylight;
  RealTimeClock_SetTimeZone(TimeZone);

  if (!EfiAtRuntime()) {
    BmcSyncTime();
  }

  return Status;
}


/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or disabled.
  @param  Pending               Indicates if the alarm signal is pending and requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.

**/
EFI_STATUS
EFIAPI
LibGetWakeupTime (
  OUT BOOLEAN     *Enabled,
  OUT BOOLEAN     *Pending,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}


/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was enabled. If
                                Enable is FALSE, then the wakeup alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.

**/
EFI_STATUS
EFIAPI
LibSetWakeupTime (
  IN BOOLEAN      Enabled,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}

//
// Notification routines
//
VOID
PmicRtcInstalledNotification (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS              Status;

  //
  // Get the PmicRtc protocol that this driver requires.
  //
  Status = gBS->LocateProtocol(&gQcomPmicRtcProtocolGuid,
                               NULL,
                               (VOID**) &PmicRtc);
  ASSERT_EFI_ERROR(Status);

  Status = PmicRtc->SetDisplayMode(0,EFI_PM_RTC_24HR_MODE);
  ASSERT_EFI_ERROR(Status);
}

/**
  This is the declaration of an EFI image entry point.

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
LibRtcInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS Status;

  PmicRtc = NULL;

  DEBUG((EFI_D_INFO, "RTC in LibRtcInitialize !!!\n"));

  RealTimeClock_InitializeVariables();

  Status = gBS->LocateProtocol(&gQcomPmicRtcProtocolGuid,
                               NULL,
                               (VOID**) &PmicRtc);
  if (EFI_ERROR(Status)) {
    // Set up to be notified when the Cpu protocol is installed.
    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, PmicRtcInstalledNotification, NULL, &PmicRtcNotificationEvent);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (&gQcomPmicRtcProtocolGuid, PmicRtcNotificationEvent, (VOID *)&PmicRtcNotificationToken);
    ASSERT_EFI_ERROR (Status);
  } else {
    Status = PmicRtc->SetDisplayMode(0,EFI_PM_RTC_24HR_MODE);
    ASSERT_EFI_ERROR(Status);
  }

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  LibRtcVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mRtcVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  RTCInfo.TimeSyncData = (TimeSyncSharedDataType *)RTC_TIME_SYNC_SMEM_ADDR;
  RTCInfo.TimeSyncData->TimeSyncMagic = TIME_SYNC_MAGIC;
    
  Status = GetConfigValue("RTCPmicDeviceIndex", 
                            &RTCPmicDeviceIndex);
  if (EFI_ERROR(Status)) {
    RTCPmicDeviceIndex = 0;
  }

  return EFI_SUCCESS;
}


/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibRtcVirtualNotifyEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Only needed if you are going to support the OS calling RTC functions in virtual mode.
  // You will need to call EfiConvertPointer (). To convert any stored physical addresses
  // to virtual address. After the OS transistions to calling in virtual mode, all future
  // runtime calls will be made in virtual mode.
  //
  EfiConvertPointer (0x0, (VOID**)&PmicRtc);
  EfiConvertPointer (0x0, (VOID**)&(RTCInfo.TimeSyncData));
  EfiConvertPointer (0x0, (VOID**)&(gRT->SetVariable));
  EfiConvertPointer (0x0, (VOID**)&gRT);
  return;
}

static
EFI_STATUS
RealTimeClock_CheckSpecParameters (
  IN  EFI_TIME               *Time
  )
{
  /* Let's check the input parameters to see if they are valid */
  if( Time->Year       <   1900      ||
      Time->Year       >   9999      ||
      Time->Month      <   1         ||
      Time->Month      >   12        ||
      Time->Day        <   1         ||
      Time->Day        >   31        ||

      (Time->Month      ==  4  && Time->Day > 30) ||
      (Time->Month      ==  6  && Time->Day > 30) ||
      (Time->Month      ==  9  && Time->Day > 30) ||
      (Time->Month      ==  11 && Time->Day > 30) ||

      /* Check Corner Cases For Leap-Years */
      (Time->Month      ==  2  && Time->Day > 29) ||
      /* Every 100 years is a non-leap year unless it's divisible by 400 */
      /* Ex. 2000 was a leap year, but 2100, 2200, 2300 aren't */
      (Time->Month      ==  2  && Time->Day == 29 && Time->Year%100 == 0 && Time->Year%400 != 0) ||
      /* Non-100 year case, if it's not divisible by 4, it cannot be a leap year */
      (Time->Month      ==  2  && Time->Day == 29 && Time->Year%100 != 0 && Time->Year%4 != 0) ||
      /* End leap-year corner cases */

      Time->Hour       >   23        ||
      Time->Minute     >   59        ||
      Time->Second     >   59        ||
      Time->Nanosecond >   999999999 ||
      Time->TimeZone   <   -1440     ||
      (Time->TimeZone   >   1440      && Time->TimeZone < EFI_UNSPECIFIED_TIMEZONE) ||
      (Time->TimeZone   >   EFI_UNSPECIFIED_TIMEZONE) ) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
This function gets the current timezone information from the SCM
driver
*/
static
EFI_STATUS
RealTimeClock_GetTimeZone (
  OUT INT16               *TimeZone
  )
{
  *TimeZone = RTCInfo.TimeZone;
  return EFI_SUCCESS;
}

/**
This function sets the current timezone information from the SCM
driver
*/
static
VOID
RealTimeClock_SetTimeZone (
  IN INT16                TimeZone
  )
{
  UINTN SizeIO = sizeof(RTC_Info_Type_Struct);
  UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS);

  RTCInfo.TimeZone = TimeZone;

  gRT->SetVariable(L"RTCInfo",
                   &gQcomTokenSpaceGuid,
                   Attributes,
                   SizeIO,
                   (VOID*)&RTCInfo);
}

/**
This function gets the current offset information from the SCM
driver
*/
static
EFI_STATUS
RealTimeClock_GetOffset (
  OUT UINT32              *pOffset
  )
{
  *pOffset = RTCInfo.Offset;
  return EFI_SUCCESS;
}

static
VOID
RealTimeClock_TimeSync (
  IN UINT64               OffsetSecs
  )
{
  TimeSyncMutexLock (&RTCInfo.TimeSyncData->Mutex);

  RTCInfo.TimeSyncData->UpdateStatus = ~(0);

  RTCInfo.TimeSyncData->SyncOffset = OffsetSecs;

  TimeSyncMutexUnlock (&RTCInfo.TimeSyncData->Mutex);
}

/**
This function sets the offset information in the SCM driver
*/
static
VOID
RealTimeClock_SetOffset (
  IN UINT32               Offset
  )
{
  UINTN SizeIO = sizeof(RTC_Info_Type_Struct);
  UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS);

  RTCInfo.Offset = Offset;
  RTCInfo.Valid = 1;

  DEBUG ((EFI_D_INFO, "Init SetVariable: RTCInfo.Offset: 0x%x\n", RTCInfo.Offset));
  gRT->SetVariable(L"RTCInfo",
                   &gQcomTokenSpaceGuid,
                   Attributes,
                   SizeIO,
                   (VOID*)&RTCInfo);
  RealTimeClock_TimeSync(Offset);
}

/**
This function initializes Variables Services for RTC Drivers so
that RTC Driver can maintain a cached version of RTCInfo
*/
static
VOID
RealTimeClock_InitializeVariables (
  VOID
  )
{
  EFI_STATUS Status;
  UINTN SizeIO = sizeof(RTC_Info_Type_Struct);
  UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS);

  Status = gRT->GetVariable(L"RTCInfo",
                            &gQcomTokenSpaceGuid,
                            &Attributes, /* Attributes */
                            &SizeIO,
                            (VOID*)&RTCInfo);
  DEBUG ((EFI_D_INFO, "Init GetVariable: RTCInfo.Offset: 0x%x\n", RTCInfo.Offset));

  if(Status == EFI_NOT_FOUND) {
    /* Set local copy of everything to zero */
    SetMem(&RTCInfo, sizeof(RTC_Info_Type_Struct), 0);

    /* Set the variable in memory to state the RTC is invalid, first boot up */
    gRT->SetVariable(L"RTCInfo",
                     &gQcomTokenSpaceGuid,
                     Attributes,
                     SizeIO,
                     (VOID*)&RTCInfo);
  }
}
