/*! @file PmicRtcConversions.c 

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
02/27/12   al      Moved to QcomPkg/Drivers
03/22/11   dy      New file. 
                   Combining Julian and RTC mode conversion APIs
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "pm_uefi.h"
#include "PmicRtcConversions.h"

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
              LOCAL MACROS / VARIABLE / CONSTANT DEFINITIONS
===========================================================================*/
#define PM_RTC_MIN_YEAR          1980  // Lowest valid year
#define PM_RTC_MAX_YEAR          2100  // Highest valid year
#define PM_RTC_MAX_MONTH         12    // Highest valid number of months
#define PM_RTC_MAX_DOW           6     // Highest valid number of day-of-week
#define PM_RTC_MAX_DAY           31    // Highest valid number of days
#define PM_RTC_MAX_24HR_HOUR     23    // Highest valid number of hours (24HR mode)
#define PM_RTC_MAX_12HR_HOUR     32    // Highest valid number of hours (12HR mode)
#define PM_RTC_MIN_12HR_HOUR     01    // Lowest valid number of hours  (12HR mode)
#define PM_RTC_MIDL_12HR_HOUR    12    // In 12HR mode, 13 to 20 are
#define PM_RTC_MIDH_12HR_HOUR    21    //    not valid values
#define PM_RTC_MAX_MINUTES       59    // Highest valid number of minutes
#define PM_RTC_MAX_SECONDS       59    // Highest valid number of seconds

/* Time Translation tables */

/* This is the number of days in a leap year set.
   A leap year set includes 1 leap year, and 3 normal years. */
#define TIME_JUL_QUAD_YEAR        (366+(3*365))

/* The pm_rtc_year_table table is used for determining the number of days which
   have elapsed since the start of each year of a leap year set. It has
   1 extra entry which is used when trying to find a 'bracketing' year.
   The search is for a day >= pm_rtc_year_table[i] and day < pm_rtc_year_table[i+1]. */

static const uint16 pm_rtc_year_table[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};


/* The pm_rtc_normal_month_table table holds the number of cumulative days that have
   elapsed as of the end of each month during a non-leap year. */

static const uint16 pm_rtc_normal_month_table[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+28,                                /* Feb */
  31+28+31,                             /* Mar */
  31+28+31+30,                          /* Apr */
  31+28+31+30+31,                       /* May */
  31+28+31+30+31+30,                    /* Jun */
  31+28+31+30+31+30+31,                 /* Jul */
  31+28+31+30+31+30+31+31,              /* Aug */
  31+28+31+30+31+30+31+31+30,           /* Sep */
  31+28+31+30+31+30+31+31+30+31,        /* Oct */
  31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The pm_rtc_leap_month_table table holds the number of cumulative days that have
   elapsed as of the end of each month during a leap year. */

static const uint16 pm_rtc_leap_month_table[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+29,                                /* Feb */
  31+29+31,                             /* Mar */
  31+29+31+30,                          /* Apr */
  31+29+31+30+31,                       /* May */
  31+29+31+30+31+30,                    /* Jun */
  31+29+31+30+31+30+31,                 /* Jul */
  31+29+31+30+31+30+31+31,              /* Aug */
  31+29+31+30+31+30+31+31+30,           /* Sep */
  31+29+31+30+31+30+31+31+30+31,        /* Oct */
  31+29+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+29+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The pm_rtc_day_offset table holds the number of days to offset
   as of the end of each year. */

static const uint16 pm_rtc_day_offset[] = {
  1,                                    /* Year 0 (leap year) */
  1+2,                                  /* Year 1             */
  1+2+1,                                /* Year 2             */
  1+2+1+1                               /* Year 3             */
};


/*-----------------------------------------------------------------------------
  Date conversion constants
-----------------------------------------------------------------------------*/

/* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */

#define TIME_JUL_OFFSET_S         432000UL


/* This is the year upon which all time values used by the Clock Services
** are based.  NOTE:  The user base day (GPS) is Jan 6 1980, but the internal
** base date is Jan 1 1980 to simplify calculations */

#define TIME_JUL_BASE_YEAR        1980


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
//Not Local
void pm_app_rtc_conv_12hr_to_24hr_format(EFI_PM_RTC_JULIAN_TYPE *time_ptr);
void pm_app_rtc_conv_24hr_to_12hr_format(EFI_PM_RTC_JULIAN_TYPE *time_ptr);

static dword pm_app_rtc_div4x2
(
  dword dividend,       /* Dividend, note dword     */
  word  divisor,         /* Divisor                  */
  word  *rem_ptr    /* Pointer to the remainder */
);

/*===========================================================================
                       FUNCTION DEFINITIONS
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
            EFI_PM_RTC_12HR_MODE
            EFI_PM_RTC_24HR_MODE

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
boolean pm_is_rtc_valid(
   EFI_PM_RTC_DISPLAY_TYPE          rtcMode,
   const EFI_PM_RTC_JULIAN_TYPE    *time)
{
   boolean time_valid = TRUE;

   /* Is the time read within range? */
   if ( (time->year < PM_RTC_MIN_YEAR)
        || (time->year > PM_RTC_MAX_YEAR)
        || (time->month > PM_RTC_MAX_MONTH)
        || (time->day_of_week > PM_RTC_MAX_DOW)
        || (time->day > PM_RTC_MAX_DAY)
        || (time->minute > PM_RTC_MAX_MINUTES)
        || (time->second > PM_RTC_MAX_SECONDS) )
   {
      time_valid = FALSE;
   }
   else
   {
      // Is the number of hours read within range?
      // check for both 24 and 12 hour modes

      // Here is the list of valid values:
      // 24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
      // 12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31
      switch(rtcMode)
      {
         case EFI_PM_RTC_12HR_MODE:
            if ( (time->hour > PM_RTC_MAX_12HR_HOUR)
                 || (time->hour < PM_RTC_MIN_12HR_HOUR)
                 || ( (time->hour > PM_RTC_MIDL_12HR_HOUR)
                      && (time->hour < PM_RTC_MIDH_12HR_HOUR) ) )
            {
               time_valid = FALSE;
            }
            break;

         case EFI_PM_RTC_24HR_MODE:
            if (time->hour > PM_RTC_MAX_24HR_HOUR)
            {
               time_valid = FALSE;
            }
            break;

         default:
            /* Invalid display mode */
            time_valid = FALSE;
      }
   }

   return time_valid;
}

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
void pm_app_rtc_conv_12hr_to_24hr_format(EFI_PM_RTC_JULIAN_TYPE *time_ptr)
{
   word  hr = time_ptr->hour;

   if (12 == hr)
   {
      time_ptr->hour = 0;
   }
   else if (32 == hr)
   {
      time_ptr->hour = 12;
   }
   else if ( (hr >= 21) && (hr <= 31) )
   {
      time_ptr->hour -= 8;
   }
}


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
void pm_app_rtc_conv_24hr_to_12hr_format(EFI_PM_RTC_JULIAN_TYPE *time_ptr)
{
   word  hr = time_ptr->hour;

   if (0 == hr)
   {
      time_ptr->hour = 12;
   }
   else if (12 == hr)
   {
      time_ptr->hour = 32;
   }
   else if ( (hr >= 13) && (hr <= 23) )
   {
      time_ptr->hour += 8;
   }
}

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
)
{
  /* Loop index */
  unsigned int /* fast */         i;

  /* Days since beginning of year or quad-year */
  uint16                          days;

  /* Quad years since base date */
  unsigned int /* fast */         quad_years;

  /* Leap-year or non-leap year month table */
  const uint16                    *month_table;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add number of seconds from Jan 1 to Jan 6 from input seconds
     in order to have number of seconds since Jan 1, 1980 for calculation */

  secs += TIME_JUL_OFFSET_S;


  /* Divide elapsed seconds by 60: remainder is seconds of Julian date;
     quotient is number of elapsed minutes. */

  secs = pm_app_rtc_div4x2 ( secs, 60, &julian->second );


  /* Divide elapsed minutes by 60: remainder is minutes of Julian date;
     quotient is elapsed hours. */

  secs = pm_app_rtc_div4x2 ( secs, 60, &julian->minute );


  /* Divide elapsed hours by 24: remainder is hours of Julian date;
     quotient is elapsed days. */

  secs = pm_app_rtc_div4x2 ( secs, 24, &julian->hour );


  /* Now things get messier. We have number of elapsed days. The 1st thing
     we do is compute how many leap year sets have gone by. We multiply
     this value by 4 (since there are 4 years in a leap year set) and add
     in the base year. This gives us a partial year value. */

  quad_years = pm_app_rtc_div4x2( secs, TIME_JUL_QUAD_YEAR, &days );

  julian->year = TIME_JUL_BASE_YEAR + (4 * quad_years);


  /* Now we use the pm_rtc_year_table to figure out which year of the leap year
     set we are in. */

  for ( i = 0; days >= pm_rtc_year_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= pm_rtc_year_table[ i ];
  
  /* Use search index to arrive at current year. */
  julian->year += i;  


  /* Take the day-of-week offset for the number of quad-years, add in
     the day-of-week offset for the year in a quad-year, add in the number
     of days into this year. */

  julian->day_of_week =
        (pm_rtc_day_offset[3] * quad_years + pm_rtc_day_offset[i] + days) % 7;


  /* Now we know year, hour, minute and second. We also know how many days
     we are into the current year. From this, we can compute day & month. */


  /* Use pm_rtc_leap_month_table in leap years, and pm_rtc_normal_month_table in other years */

  month_table = (i == 0) ? pm_rtc_leap_month_table : pm_rtc_normal_month_table;


  /* Search month-table to compute month */

  for ( i = 0; days >= month_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }


  /* Compute & store day of month. */
  julian->day = days - month_table[ i ] + 1;  

  /* Store month. */
  julian->month = i + 1;

} /* pm_app_rtc_julian_from_secs */


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
  const EFI_PM_RTC_JULIAN_TYPE          *julian
)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  uint32                          time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First, compute number of days contained in the last whole leap
     year set. */

  time = ( (julian->year - TIME_JUL_BASE_YEAR) / 4L ) * TIME_JUL_QUAD_YEAR;


  /* Now, add in days since last leap year to start of this month. */

  if ( (julian->year & 0x3) == 0 )
  {
    /* If this is a leap year, add in only days since the beginning of the
       year to start of this month. */

    time += pm_rtc_leap_month_table[ julian->month - 1 ];
  }
  else
  {
    /* If this is not a leap year, add in days since last leap year plus
       days since start of this year to start of this month. */

    time += pm_rtc_year_table[ julian->year & 0x3 ];

    time += pm_rtc_normal_month_table[ julian->month - 1 ];
  }

  /* Add in days in current month. */
  time += julian->day - 1;

  /* Add in elapsed hours, minutes, and seconds  */
  time = time * 24  +  julian->hour;
  time = time * 60  +  julian->minute;
  time = time * 60  +  julian->second;


  /* Subtract number of seconds from base (GPS) date of 6 Jan 1980 to
     calculation base date of 1 Jan 1980 */

  time -= TIME_JUL_OFFSET_S;


  /* Return elapsed seconds. */
  return time;

} /* pm_app_rtc_julian_to_secs */

/*===========================================================================

FUNCTION pm_app_rtc_div4x2                                  LOCAL FUNCTION

DESCRIPTION
  This procedure divides a specified 32 bit unsigned dividend by a
  specified 16 bit unsigned divisor. Both the quotient and remainder
  are returned.

DEPENDENCIES
  None

RETURN VALUE
  The quotient of dividend / divisor.

SIDE EFFECTS
  None

===========================================================================*/
static dword pm_app_rtc_div4x2
(
  dword dividend,       /* Dividend, note dword     */
  word  divisor,         /* Divisor                  */
  word  *rem_ptr    /* Pointer to the remainder */
)
{
  dword quotient = 0 ;
  if ( divisor != 0 ) 
  {
    *rem_ptr = (word) (dividend % divisor);
    quotient = dividend / divisor ;
  }
  return quotient ;
} /* END pm_app_rtc_div4x2 */
