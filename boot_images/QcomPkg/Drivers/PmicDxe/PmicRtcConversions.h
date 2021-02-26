#ifndef __PMICRTCCONVERSIONS_H__
#define __PMICRTCCONVERSIONS_H__

/*! @file PmicRtcConversions.h 

 *  PMIC-RTC CONVERSIONS RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC RTC module.
 *
 *  Copyright (c) 2011  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/12   al      Moved to QcomPkg\Drivers
03/22/11   dy      New file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "com_dtypes.h"

#include "Protocol/EFIPmicRTC.h"

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
FUNCTION   pm_is_rtc_valid                                 EXTERNAL FUNCTION

DESCRIPTION
   Use this function to check if the RTC time going in or out of the
   PMIC if valid (within range). This function is meant to be used
   in conjunction with "pm_rtc_rw_cmd()".

   Example:
      pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);

      if (pm_is_rtc_valid(rtcMode, &time))
      {
         // RTC time is valid
      }

PARAMETERS
   1) Name: rtcMode
      - Display mode (24 or 12 hr)
      Type: EFI_PM_RTC_DISPLAY_TYPE
      - Valid Values:
            PM_RTC_12HR_MODE
            PM_RTC_24HR_MODE

   2) Name: time
      - Pointer to the Julian time structure that is to be validated.
      Type: EFI_PM_RTC_JULIAN_TYPE* Structure pointer.
     - Structure members:

        word year         = Year [1980..2100]
        word month        = Month of year [1..12]
        word day          = Day of month [1..31]
        word hour         = Depending on the display mode
        word minute       = Minute of hour [0..59]
        word second       = Second of minute [0..59]
        word day_of_week  = Day of the week [0..6] Monday through Sunday

RETURN VALUE
   Type: boolean
   - Possible values:
     - TRUE   -->  Time information pointed to by 'time' is valid
     - FALSE  -->  Otherwise

DEPENDENCIES
   The following function(s) must have been called:
   1) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
boolean pm_is_rtc_valid
(
  EFI_PM_RTC_DISPLAY_TYPE         rtcMode,
  const EFI_PM_RTC_JULIAN_TYPE   *time
);

/*===========================================================================
FUNCTION   pm_app_rtc_conv_12hr_to_24hr_format                EXTERNAL FUNCTION

DESCRIPTION
   This function converts the time from the 12-hr (AM/PM) format to the 24-hr
   format.

   24-hr and 12-hr formatting of the "hour hand" are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31

PARAMETERS
   1) Name: time_ptr
      -  Pointer to a valid Julian time structure having time in 12-hr format.
         This function modifies the time pointed to by 'time_ptr' to 24-hr
         format in place.
      Type: EFI_PM_RTC_JULIAN_TYPE*

RETURN VALUE
   None.

DEPENDENCIES
   None.

SIDE EFFECTS
   The input time is modified to produce the output of the time format
   conversion.
===========================================================================*/
void pm_app_rtc_conv_12hr_to_24hr_format
(
  EFI_PM_RTC_JULIAN_TYPE        *time_ptr
);


/*===========================================================================
FUNCTION   pm_app_rtc_conv_24hr_to_12hr_format                EXTERNAL FUNCTION

DESCRIPTION
   This function converts the time from the 24-hr format to the 12-hr (AM/PM)
   format.

   24-hr and 12-hr formatting of the "hour hand" are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31

PARAMETERS
   1) Name: time_ptr
      -  Pointer to a valid Julian time structure having time in 24-hr format.
         This function modifies the time pointed to by 'time_ptr' to 12-hr
         format in place.
      Type: EFI_PM_RTC_JULIAN_TYPE*


RETURN VALUE
   None.

DEPENDENCIES
   None.

SIDE EFFECTS
   The input time is modified to produce the output of the time format
   conversion.
===========================================================================*/
void pm_app_rtc_conv_24hr_to_12hr_format
(
  EFI_PM_RTC_JULIAN_TYPE        *time_ptr
);

/*=============================================================================

FUNCTION pm_app_rtc_julian_from_secs

DESCRIPTION
  This procedure converts a specified number of elapsed seconds   
  since the base date to its equivalent Julian date and time.     

DEPENDENCIES
  None

RETURN VALUE
  The specified Julian date record is filled in with equivalent date/time,
  and returned into the area pointed to by julian_ptr.

SIDE EFFECTS
  None

=============================================================================*/

void pm_app_rtc_julian_from_secs
(
  /* Number of seconds since base date */
  uint32                          secs,
  /* OUT: Pointer to Julian date record */
  EFI_PM_RTC_JULIAN_TYPE        *julian
);


/*=============================================================================

FUNCTION pm_app_rtc_julian_to_secs

DESCRIPTION
  This procedure converts a specified Julian date and time to an  
  equivalent number of elapsed seconds since the base date.    

DEPENDENCIES
  None

RETURN VALUE
  Number of elapsed seconds since base date.       

SIDE EFFECTS
  None

=============================================================================*/

uint32 pm_app_rtc_julian_to_secs
(
  /* Pointer to Julian date record */
  const EFI_PM_RTC_JULIAN_TYPE   *julian
);

#endif // __PMICRTCCONVERSIONS_H__
