/*! @file PmicRtcProtocol.c 

*  PMIC-RTC MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC RTC module.
*
*  Copyright (c) 2011 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
 05/21/15  al      Adding API to read RTC ALARM OK and Exipred status
06/09/14   al      Arch update
13/12/13   aa      PmicLib Dec Addition
11/22/13   va      PmicLib Dec Addition
02/08/13   al      Fixed warnings
01/22/13   al	   Added SetTimeValueRaw 
10/25/12   al      File renamed 
10/23/12   al	   Updating copyright info 
02/27/12   al      Commented out to compile for 8960 and 8974.
03/20/11   dy      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "pm_uefi.h"
#include "PmicRtcConversions.h"
#include "pm_rtc.h"

#include "Protocol/EFIPmicRTC.h"

/*===========================================================================
LOCAL STRUCTURE DEFINITIONS
===========================================================================*/

/*===========================================================================
LOCAL CONSTANT DEFINITIONS
===========================================================================*/

/*===========================================================================
LOCAL FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
LOCAL VARIABLE DEFINITIONS
===========================================================================*/
static EFI_PM_RTC_DISPLAY_TYPE  pm_app_rtc_current_display_mode = EFI_PM_RTC_24HR_MODE;

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/**
EFI_PmicRTCGetDisplayMode ()

@brief
GetDisplayMode implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCGetDisplayMode
(
 IN UINT32 PmicDeviceIndex, 
 OUT EFI_PM_RTC_DISPLAY_TYPE *ModePtr
 )
{
    *ModePtr = pm_app_rtc_current_display_mode;

    return EFI_SUCCESS;
}

/**
EFI_PmicRTCSetDisplayMode ()

@brief
SetDisplayMode implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCSetDisplayMode
(
 IN UINT32 PmicDeviceIndex, 
 IN EFI_PM_RTC_DISPLAY_TYPE Mode
 )
{
    if (Mode >= EFI_PM_RTC_INVALID_MODE)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_app_rtc_current_display_mode = Mode;

    return EFI_SUCCESS;
}

/**
EFI_PmicRTCGetTimeValue()

@brief
GetTimeValue implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCGetTimeValue
(
 IN UINT32 PmicDeviceIndex, 
 OUT EFI_PM_RTC_JULIAN_TYPE *CurrentTimePtr
 )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_time_type  hal_time;

    //pm_hal_rtc_get_time
    errFlag = pm_rtc_hal_rtc_get_time( PmicDeviceIndex, &hal_time );

    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        //Convert to Julian
        pm_app_rtc_julian_from_secs(hal_time.sec, CurrentTimePtr);

        if (EFI_PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)
        {
            //Convert to 12HR Mode
            pm_app_rtc_conv_24hr_to_12hr_format( CurrentTimePtr );
        }
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
EFI_PmicRTCSetTimeValue()

@brief
SetTimeValue implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCSetTimeValue
(
 IN UINT32 PmicDeviceIndex, 
 OUT EFI_PM_RTC_JULIAN_TYPE *CurrentTimePtr
 )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_time_type  hal_time;

    if (CurrentTimePtr == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    // Check if RTC Time is Valid before setting it
    if (pm_is_rtc_valid(pm_app_rtc_current_display_mode, CurrentTimePtr))
    {
        if (EFI_PM_RTC_12HR_MODE == pm_app_rtc_current_display_mode)
        {
            // convert to a 24 hr time format understood by clk API
            pm_app_rtc_conv_12hr_to_24hr_format( CurrentTimePtr );
        }

        // convert Julian time to seconds elapsed from a known time in the past
        hal_time.sec = pm_app_rtc_julian_to_secs( CurrentTimePtr );
        // start RTC
        ////pm_hal_rtc_start
        errFlag = pm_hal_rtc_start(PmicDeviceIndex, &hal_time);
    }
    else
    {
        errFlag = PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
EFI_PmicRTCIsTimeValid()

@brief
IsTimeValid implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
void
EFIAPI
EFI_PmicRTCIsTimeValid
(
 IN UINT32 PmicDeviceIndex, 
 IN EFI_PM_RTC_DISPLAY_TYPE RtcMode,
 CONST EFI_PM_RTC_JULIAN_TYPE    *Time,
 BOOLEAN *Valid
 )
{
    *Valid = pm_is_rtc_valid(RtcMode, Time );
}

/**
EFI_PmicRTCGetTimeValueRaw()

@brief
GetTimeValueRaw implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCGetTimeValueRaw
(
 IN UINT32 PmicDeviceIndex,
 OUT UINT32 *TimeValueSeconds
 )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_time_type  hal_time;

    if (TimeValueSeconds == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
                                      
    errFlag = pm_rtc_hal_rtc_get_time( PmicDeviceIndex,&hal_time);

    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        *TimeValueSeconds = hal_time.sec;
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
EFI_PmicRTCSetTimeValueRaw()

@brief
SetTimeValueRaw implementation of EFI_QCOM_PMIC_RTC_PROTOCOL
*/
EFI_STATUS
EFIAPI
EFI_PmicRTCSetTimeValueRaw
(
 IN UINT32 PmicDeviceIndex,
 IN UINT32 TimeValueSeconds
 )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_time_type  hal_time;

    hal_time.sec=TimeValueSeconds;

    // start RTC
    ////pm_hal_rtc_start
    errFlag = pm_hal_rtc_start(PmicDeviceIndex, &hal_time);

    if (errFlag)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


//configure ALRAM check
EFI_STATUS
EFIAPI
EFI_PmicRTCAlarmEnableStatus
(
 IN  UINT32  PmicDeviceIndex,
 OUT BOOLEAN *Enabled
 )
{
    EFI_STATUS Status = EFI_SUCCESS;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if (NULL==Enabled)
    {
        return EFI_DEVICE_ERROR;
    }

    /*check alarm OK status */
    errFlag = pm_rtc_alarm_enable_status(PmicDeviceIndex, PM_RTC_ALARM, Enabled);    

    Status = (errFlag == PM_ERR_FLAG__SUCCESS) ? EFI_SUCCESS : EFI_DEVICE_ERROR ;

    return Status;
}

//read the alarm status
EFI_STATUS
EFIAPI
EFI_PmicRTCAlarmExpireStatus
(
 IN  UINT32  PmicDeviceIndex,
 OUT BOOLEAN *ExpireStatus
 )
{
    EFI_STATUS Status = EFI_SUCCESS;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if (NULL==ExpireStatus)
    {
        return EFI_DEVICE_ERROR;
    }
    
    errFlag = pm_rtc_irq_status(PmicDeviceIndex, PM_RTC_ALARM, PM_IRQ_STATUS_RT, ExpireStatus);

    Status = (errFlag == PM_ERR_FLAG__SUCCESS) ? EFI_SUCCESS : EFI_DEVICE_ERROR ;

    return Status;
}




/**
PMIC Client Device UEFI Protocol implementation
*/
EFI_QCOM_PMIC_RTC_PROTOCOL PmicRtcProtocolImplementation = 
{
    PMIC_RTC_REVISION,
    EFI_PmicRTCGetDisplayMode,
    EFI_PmicRTCSetDisplayMode,
    EFI_PmicRTCGetTimeValue,
    EFI_PmicRTCSetTimeValue,
    EFI_PmicRTCIsTimeValid,
    EFI_PmicRTCGetTimeValueRaw,
    EFI_PmicRTCSetTimeValueRaw,
    EFI_PmicRTCAlarmEnableStatus,
    EFI_PmicRTCAlarmExpireStatus,
};
