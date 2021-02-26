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
 02/05/15   al      Changing TPL_CALLBACK to TPL_CALLBACK
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
#include <Protocol/EFIPmicRTC.h>
#include <Protocol/EFIPmicPwrOn.h>
#include <Guid/EventGroup.h>

/** 
Definitions for number of seconds used for calculations 
*/
#define RTC_SECONDS_PER_MINUTE    60
#define RTC_SECONDS_PER_HOUR      RTC_SECONDS_PER_MINUTE * 60
#define RTC_SECONDS_PER_DAY       RTC_SECONDS_PER_HOUR * 24
#define RTC_SECONDS_PER_YEAR      RTC_SECONDS_PER_DAY * 365
#define RTC_SECONDS_PER_LEAP_YEAR RTC_SECONDS_PER_DAY * 366

extern EFI_GUID gQcomTokenSpaceGuid;

typedef struct {
  /* Interface for the protocol to access the RTC */
  EFI_QCOM_PMIC_RTC_PROTOCOL    *pmic_rtc;
  /* Interface for the protocol to access the RTC */
  EFI_QCOM_PMIC_PWRON_PROTOCOL  *pmic_pwron;
  /* Index to pass into pmic calls */
  UINT32                        nPmicIndex;
}
RTCInternalType;

/**
Internal RealTimeClockLib Variables 
*/
static RTCInternalType RTCInternal = 
{
  NULL,
  NULL,
  0
};

/**
Store if the time is affected by daylight savings.
*/ 
UINT8 UEFI_Daylight = EFI_TIME_IN_DAYLIGHT;

/**
Store if  device is in Bootup or Runtime.
*/
BOOLEAN inRunTimeServicesMode;

/**
The Event used to detect when we are in runtime mode.
*/
EFI_EVENT ExitBootServiceEvent;

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

typedef struct{
  UINT32 Offset;
  INT16  TimeZone;
  UINT8  Valid;
  UINT8  Pad;
}
RTC_Info_Type_Struct;

static RTC_Info_Type_Struct RTCInfo;

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
  UINT32 TimeInSeconds = 0;
  UINT32 TimeOffset = 0; 
  EFI_PM_RTC_JULIAN_TYPE pmic_time;

  if(Time == NULL) {
    DEBUG((EFI_D_INFO, "RTCGetTime Time ptr is NULL, Return\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (inRunTimeServicesMode) {
    DEBUG((EFI_D_INFO, "RTCGetTime in RunTime Services Mode, Return\n"));
    return EFI_UNSUPPORTED;
  }

  if (RTCInternal.pmic_rtc == NULL) {
    DEBUG((EFI_D_INFO, "RTCGetTime pmic_rtc NULL, Return\n"));
    return EFI_UNSUPPORTED;
  }

  /* Get raw time value from pmic RTC */
  Status = RTCInternal.pmic_rtc->GetTimeValueRaw(0, &TimeInSeconds);
  
  /* If RTC is not ticking, let's make it run */
  if (Status != EFI_SUCCESS) {
      /* This is the first set, let's set RTC to 0 by setting epoch time */
    SetMem(&pmic_time, sizeof(EFI_PM_RTC_JULIAN_TYPE), 0);
    pmic_time.day = 6; /* 06 */
    pmic_time.month = 1; /* Jan */
    pmic_time.year = 1980; /* 1980 */
    Status = RTCInternal.pmic_rtc->SetTimeValue(0, &pmic_time);
    if( Status != EFI_SUCCESS ) {
      DEBUG((EFI_D_INFO, "RTCGet Cannot Initialize PMIC RTC\n"));
      return Status;
    }
  
    RTCInternal.pmic_rtc->GetTimeValueRaw(0, &TimeInSeconds);
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
  Time->Nanosecond = 0; // we do not get data at this resolution.

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
  EFI_PM_RTC_JULIAN_TYPE pmic_time;

  if(Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (inRunTimeServicesMode) {
    DEBUG ((EFI_D_WARN, "RTC Lib in RunTime Services Mode. Return\n"));
    return EFI_UNSUPPORTED;
  }
  if (RTCInternal.pmic_rtc == NULL) {
    return EFI_UNSUPPORTED;
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
  
  /* Get raw time value from pmic RTC */
  /* Note: If RTC is greater than time set, then offset will very large! */
  /* Also, If RTC is large, the dynamic range significantly reduces */
  Status = RTCInternal.pmic_rtc->GetTimeValueRaw(0,&RTCVal);

  /* If RTC is not ticking, let's make it run */
  if (Status != EFI_SUCCESS) {
    /* This is the first set, let's set RTC to 0 by setting epoch time */
    SetMem(&pmic_time,sizeof(EFI_PM_RTC_JULIAN_TYPE),0);
    pmic_time.day = 6; /* 06 */
    pmic_time.month = 1; /* Jan */
    pmic_time.year = 1980; /* 1980 */
    Status = RTCInternal.pmic_rtc->SetTimeValue(0,&pmic_time);
    if( Status != EFI_SUCCESS ) {
      DEBUG((EFI_D_INFO, "RTCSet Cannot set PMIC RTC\n"));
      return Status;    
    }
  
    RTCInternal.pmic_rtc->GetTimeValueRaw(0,&RTCVal);
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

/**
Callback when ExitBootServices is called. This will let the RTC DXE know that 
we are in Runtime and act appropiately. 
  @param Event Pointer to the Event that expired.
  @param Context Context that this callback is happening in
 
  @retval EFI_SUCCESS Able to update the variable cooresponding to the state of
                      the device.
*/
EFI_STATUS
EFIAPI
RTCExitBootService (
  EFI_EVENT                      Event,
  VOID                           *Context
  )
{
  inRunTimeServicesMode = TRUE;
  RTCInternal.pmic_rtc = NULL;
  return EFI_SUCCESS;
}

/*
* Notifications
*/
VOID      *PmicProtocolNotificationToken = NULL;
VOID      *PmicPwrOnProtocolNotificationToken = NULL;
EFI_EVENT PmicProtocolNotificationEvent  = (EFI_EVENT)NULL;
EFI_EVENT PmicPwrOnProtocolNotificationEvent  = (EFI_EVENT)NULL;

VOID
PmicProtocolInstalledNotification (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS              Status;

  Status = gBS->LocateProtocol(&gQcomPmicRtcProtocolGuid, 
                               NULL,
                               (VOID**) &RTCInternal.pmic_rtc);
  if (Status != EFI_SUCCESS) {
    return;
  }

  Status = RTCInternal.pmic_rtc->SetDisplayMode(0,EFI_PM_RTC_24HR_MODE);
  if(Status != EFI_SUCCESS) {
    return;
  }

  DEBUG((EFI_D_INFO, "RTC Display Mode Set\n"));
}

VOID
PmicPwrOnProtocolInstalledNotification (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS              Status;

  Status = gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, 
                               NULL,
                               (VOID**) &RTCInternal.pmic_pwron);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_WARN, "RTC Could Not Find Pwr On Module!!!\n"));
    return;
  }

  RealTimeClock_InitializeVariables();

  DEBUG((EFI_D_INFO, "RTC Pwr On Module Setup Successfully\n"));
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

  RTCInternal.pmic_rtc = NULL;
  
  DEBUG((EFI_D_INFO, "RTC in LibRtcInitialize !!!\n"));

  Status = gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, 
                               NULL,
                               (VOID**) &RTCInternal.pmic_pwron);

  if (Status != EFI_SUCCESS) {
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, 
                              TPL_CALLBACK, 
                              PmicPwrOnProtocolInstalledNotification, 
                              NULL, 
                              &PmicPwrOnProtocolNotificationEvent);
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_WARN, "RTC Could not Create Pwr On Event!!!, Status = 0x%x\n", Status));
      return Status;
    }

    Status = gBS->RegisterProtocolNotify(&gQcomPmicPwrOnProtocolGuid,
                                         PmicPwrOnProtocolNotificationEvent, 
                                         (VOID *)&PmicPwrOnProtocolNotificationToken);
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_WARN, "RTC Could Not Register for Pwr On Notify!!!, Status = 0x%x\n", Status));
      return Status;
    }
  }
  else {
    /* Power On Module initialized already, we can initialize RTC */
    RealTimeClock_InitializeVariables();
  }


  Status = gBS->LocateProtocol(&gQcomPmicRtcProtocolGuid, 
                               NULL,
                               (VOID**) &RTCInternal.pmic_rtc);
  if (Status != EFI_SUCCESS) {
    /*
     * Set up to be notified when the Pmic RTC protocol is installed.
     */
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, 
                              TPL_CALLBACK, 
                              PmicProtocolInstalledNotification, 
                              NULL, 
                              &PmicProtocolNotificationEvent);
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_WARN, "RTC Could Not Register for PmicProtocol Event!!!, Status = 0x%x\n", Status));
      return Status;
    }
  
    Status = gBS->RegisterProtocolNotify(&gQcomPmicRtcProtocolGuid,
                                         PmicProtocolNotificationEvent, 
                                         (VOID *)&PmicProtocolNotificationToken);
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_WARN, "RTC Could Not Register for PmicProtocol Notify!!!, Status = 0x%x\n", Status));
      return Status;
    }
  }
  else
  {
    Status = RTCInternal.pmic_rtc->SetDisplayMode(0,EFI_PM_RTC_24HR_MODE);
    if(Status != EFI_SUCCESS) {
      DEBUG((EFI_D_WARN, "RTC SetDisplayMode Error!!!, Status = 0x%x\n", Status));
      return Status;
    }
  }


  inRunTimeServicesMode = FALSE;
  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                (EFI_EVENT_NOTIFY)RTCExitBootService,
                NULL,
                &gEfiEventExitBootServicesGuid,
                &ExitBootServiceEvent
                );
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_WARN, "RTC Could not register for RTCExitBootService!!!, Status = 0x%x\n", Status));
    return Status;
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
  BOOLEAN pmicWasBatteryRemoved = FALSE;
  UINT32 Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS);

  Status = RTCInternal.pmic_pwron->GetPonPblStatus (RTCInternal.nPmicIndex, 
                                                    EFI_PM_PWRON_PON_PBL_STATUS_XVDD_RB_OCCURRED,
                                                    &pmicWasBatteryRemoved);
  if (Status != EFI_SUCCESS ||
      pmicWasBatteryRemoved == TRUE) {
    /* Assume power has been removed */
    DEBUG((EFI_D_WARN, "RealTimeClock_InitializeVariables: Detecting Loss in Power to RTC! Status:0x%x pmicWasBatteryRemoved:0x%x \n",
           Status,
           (UINT32)pmicWasBatteryRemoved));

    /* This will set the structure of the RTCInfo to 0 along with the "valid" field */
    SetMem(&RTCInfo, sizeof(RTC_Info_Type_Struct), 0);
    
    /* Set the variable in memory to state the RTC is invalid */
    gRT->SetVariable(L"RTCInfo", 
                     &gQcomTokenSpaceGuid,
                     Attributes,
                     SizeIO,
                     (VOID*)&RTCInfo);

  }
  else{
    /* We did not lose power to RTC, let's restore the variable */
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
}
