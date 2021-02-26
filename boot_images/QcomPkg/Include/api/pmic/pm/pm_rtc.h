#ifndef __PM_UEFI_RTC_H__
#define __PM_UEFI_RTC_H__

/*===========================================================================

PMIC RTC SERVICES HEADER FILE

DESCRIPTION
This file contains functions prototypes and variable/type/constant 
declarations for the RTC services developed for the Qualcomm Power
Management IC.

Copyright (c) 2003 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/15   al      Adding API to read RTC alarm and interrupt
05/20/14   al      Architecture update
02/08/13   al      Changing pm_dev_ to pm_ and fixed warnings
07/27/12   al	   Renamed all Enums,added device index and resource index
04/28/11   dy      Include PMIC version check
03/23/11   dy      Port to PMIC UEFI
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/23/08   jtn     Added API pm_hal_rtc_prescaler_reset()
06/18/07   cng     Added meta comments to miscellaneous RTC APIs
05/31/06   Vish    Fixed LINT warnings.
05/03/05   Vish    Modified pm_hal_rtc_get_time() to read RTC time even in
the case when RTC was not running.
01/19/05   Vish    Updated function header for pm_hal_rtc_get_time().
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/07/03   Vish    Added the task of clearing the alarm trigger condition
within pm_hal_rtc_disable_alarm() so that the master
RTC alarm interrupt could be cleared afterward.
10/02/03   Vish    Added pm_hal_rtc_get_alarm_time() and 
pm_hal_rtc_get_alarm_status().
09/23/03   Vish    Changed all pm_rtc_xxx variables/fns to pm_hal_rtc_xxx.
09/13/03   Vish    Created.
===========================================================================*/

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/* =========================================================================
TYPE DEFINITIONS
========================================================================= */

// RTC time struct used for setting and retrieving current time and also for
// setting the alarm time
typedef struct
{
    uint32  sec;
} pm_hal_rtc_time_type;

// Type for selecting different physical alarms available in RTC
typedef enum
{
    PM_HAL_RTC_ALARM_1,
    PM_HAL_RTC_ALARM_INVALID
} pm_hal_rtc_alarm;

// Status of the various physical alarms available in RTC
typedef struct
{
    boolean  alarm_1_triggered;
} pm_hal_rtc_alarm_status_type;

// Real-time clock command type
typedef enum
{
    PM_RTC_SET_CMD,
    PM_RTC_GET_CMD,
    PM_RTC_INVALID_CMD
}pm_rtc_cmd_type;

// Real-time clock display mode type
typedef enum
{
    PM_RTC_12HR_MODE,
    PM_RTC_24HR_MODE,
    PM_RTC_INVALID_MODE
}pm_rtc_display_type;

// PMIC's version of the Julian time structure
typedef struct
{
    word year;            // Year [1980..2100]
    word month;           // Month of year [1..12]
    word day;             // Day of month [1..31]
    word hour;            // Hour of day [0..23]
    word minute;          // Minute of hour [0..59]
    word second;          // Second of minute [0..59]
    word day_of_week;     // Day of the week [0..6] Monday..Sunday
} pm_rtc_julian_type;

// RTC periph type
typedef enum
{
   PM_RTC_WR,
   PM_RTC_ALARM,
   PM_RTC_TIMER,
   PM_RTC_INVALID_TYPE,
} pm_rtc_periph_type;

/* =========================================================================
FUNCTION PROTOTYPES
========================================================================= */

/*===========================================================================
FUNCTION  pm_rtc_lib_init                                  EXTERNAL FUNCTION

DESCRIPTION
Initialize RTC module

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.      

===========================================================================*/
pm_err_flag_type pm_rtc_lib_init(
                                        uint32 pmic_device_index,
                                        uint8 pmic_rev_id
                                        );

/*===========================================================================
FUNCTION  pm_hal_rtc_start                                 EXTERNAL FUNCTION

DESCRIPTION
This function gets the RTC ticking with the indicated time as its current
(start-up) time.

PARAMETERS
1) Name: start_time_ptr
- Pointer to the start-up time for the real-time clock.
Type: pm_hal_rtc_time_type*
- Valid inputs:
Pointer to a valid non-NULL RTC time structure containing any 32
bit number indicating the number of seconds elapsed from a
known point in time in history.

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_INVALID_PARAMETER     -> NULL pointer provided for 
start_time_ptr.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.      
- PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_hal_rtc_start(
    uint32 pmic_device_index, 
    const pm_hal_rtc_time_type *start_time_ptr
    );


/*===========================================================================
FUNCTION  pm_hal_rtc_stop                                  EXTERNAL FUNCTION

DESCRIPTION
This function stops the RTC from ticking.

PARAMETERS
None.

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.           

===========================================================================*/
pm_err_flag_type pm_hal_rtc_stop(uint32 pmic_device_index);


/*===========================================================================
FUNCTION  pm_hal_rtc_get_time                              EXTERNAL FUNCTION

DESCRIPTION
This function returns the current time of the RTC. This will be the actual
present time if the RTC has been ticking or the time at which the RTC
was last stopped.

PARAMETERS
1) Name: time_ptr
- RTC structure used for returning the current time of the real-time clock.
Type: pm_hal_rtc_time_type*
- Valid inputs:
Non-NULL valid RTC time structure

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_INVALID_PERIPHERAL    -> RTC is not running.     
- PM_ERR_FLAG_INVALID_PARAMETER     -> NULL pointer provided for 
time_ptr.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.      
- PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_rtc_hal_rtc_get_time(
    uint32 pmic_device_index, 
    pm_hal_rtc_time_type *time_ptr
    );




/*===========================================================================
FUNCTION  pm_hal_rtc_set_time_adjust                       EXTERNAL FUNCTION

DESCRIPTION
This function sets the time adjustment correction factor for the RTC crystal
oscillator that is slightly off the 32768 Hz frequency. Every 10th second,
the freq divider is switched from the nominal 32768 to (32768 - 64 + time_adjust).

PARAMETERS
1) Name: time_adjust
- Adjustment factor.
Type: uint8
- Valid inputs:
0 to 63    -->  Compensates for a slower crystal oscillator
64         -->  Provides for no compensation
65 to 127  -->  Compensates for a faster crystal oscillator

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_INVALID_PARAMETER     -> Out-of-range value specified for 
time_adjust.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.      
- PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_hal_rtc_set_time_adjust(
    uint32 pmic_device_index, 
    uint8 time_adjust
    );


/*===========================================================================
FUNCTION  pm_hal_rtc_get_time_adjust                       EXTERNAL FUNCTION

DESCRIPTION
This function returns the current time adjustment correction factor in use as
set by the previous pm_hal_rtc_set_time_adjust().

PARAMETERS
1) Name: time_adjust_ptr
- Pointer to the adjustment factor.
Type: uint8*
- Valid inputs:
Non-NULL pointer to a valid uint8 containing:
0 to 63    -->  Compensation for a slower crystal oscillator
64         -->  No compensation
65 to 127  -->  Compensation for a faster crystal oscillator

RETURN VALUE
Type: pm_err_flag_type
- Possible values:
- PM_ERR_FLAG_SUCCESS               -> Operation succeeded.
- PM_ERR_FLAG_INVALID_PARAMETER     -> NULL pointer provided in 
time_adjust_ptr.
- PM_ERR_FLAG_FEATURE_NOT_SUPPORTED -> Feature not available on this 
version of the PMIC.      
- PM_ERR_FLAG_BUS_ERR           -> Communication with PM chip failed.

===========================================================================*/
pm_err_flag_type pm_hal_rtc_get_time_adjust(
    uint32 pmic_device_index, uint8 *time_adjust_ptr
    );

/**
 * @brief This function reads RTC enabled status
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] perph_index:  
 *                rtc number. Refer enum pm_rtc_periph_type.
 *  
 * @param[out] enabled:  
 *                TRUE: Enabled  
 *                FALSE:Disable 
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_rtc_alarm_enable_status(uint32 pmic_chip, pm_rtc_periph_type perph_index, boolean* enabled);


/**
 * @brief This function enables or disables rtc irq
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] rtc:  
 *                rtc number. Refer enum pm_rtc_perph_index.
 *  
 * @param[in] enable:  
 *                TRUE: Enable interrupt for the rtc 
 *                FALSE:Disable interrupt for the rtc
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_rtc_irq_enable(uint32 pmic_chip, pm_rtc_periph_type perph_index, boolean enable);


/**
 * @brief This function clears the rtc irq 
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] rtc:  
 *                rtc number. Refer enum pm_rtc_perph_index.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_rtc_irq_clear(uint32 pmic_chip, pm_rtc_periph_type perph_index);


/**
 * @brief This function configures the rtc for irq 
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] rtc:  
 *                rtc number. Refer enum pm_rtc_perph_index.
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_uefi_irq.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_rtc_irq_set_trigger(uint32 pmic_chip, pm_rtc_periph_type perph_index, pm_irq_trigger_type trigger);

/**
 * @brief This function gets RTC IRQ status 
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] rtc:  
 *                rtc number. Refer enum pm_rtc_perph_index.
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_uefi_irq.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_rtc_irq_status(uint32 pmic_chip, pm_rtc_periph_type perph_index, pm_irq_status_type type, boolean *status);

#endif // __PM_UEFI_RTC_H__


