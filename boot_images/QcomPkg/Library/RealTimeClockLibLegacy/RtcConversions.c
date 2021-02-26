/*! @file RtcConversions.c 

 *  RTC CONVERSIONS RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the RTC module.
 *
 *  Copyright (c) 2012  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/12   cpaulo  New file for RTC conversions based off modem. 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "RtcConversions.h"

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
              LOCAL MACROS / VARIABLE / CONSTANT DEFINITIONS
===========================================================================*/
#define RTC_MIN_YEAR          1980  // Lowest valid year
#define RTC_MAX_YEAR          2100  // Highest valid year
#define RTC_MAX_MONTH         12    // Highest valid number of months
#define RTC_MAX_DOW           6     // Highest valid number of day-of-week
#define RTC_MAX_DAY           31    // Highest valid number of days
#define RTC_MAX_24HR_HOUR     23    // Highest valid number of hours (24HR mode)
#define RTC_MAX_12HR_HOUR     32    // Highest valid number of hours (12HR mode)
#define RTC_MIN_12HR_HOUR     01    // Lowest valid number of hours  (12HR mode)
#define RTC_MIDL_12HR_HOUR    12    // In 12HR mode, 13 to 20 are
#define RTC_MIDH_12HR_HOUR    21    //    not valid values
#define RTC_MAX_MINUTES       59    // Highest valid number of minutes
#define RTC_MAX_SECONDS       59    // Highest valid number of seconds

/* Time Translation tables */

/* This is the number of days in a leap year set.
   A leap year set includes 1 leap year, and 3 normal years. */
#define TIME_JUL_QUAD_YEAR        (366+(3*365))

/* The rtc_year_table table is used for determining the number of days which
   have elapsed since the start of each year of a leap year set. It has
   1 extra entry which is used when trying to find a 'bracketing' year.
   The search is for a day >= rtc_year_table[i] and day < rtc_year_table[i+1]. */

static const UINT16 rtc_year_table[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};


/* The rtc_normal_month_table table holds the number of cumulative days that have
   elapsed as of the end of each month during a non-leap year. */

static const UINT16 rtc_normal_month_table[] = {
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

/* The rtc_leap_month_table table holds the number of cumulative days that have
   elapsed as of the end of each month during a leap year. */

static const UINT16 rtc_leap_month_table[] = {
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

/* The rtc_day_offset table holds the number of days to offset
   as of the end of each year. */

static const UINT16 rtc_day_offset[] = {
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
static UINT32 app_rtc_div4x2
(
  UINT32 dividend,       /* Dividend, note UINT32     */
  UINT16  divisor,         /* Divisor                  */
  UINT16  *rem_ptr    /* Pointer to the remainder */
);

/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*=============================================================================

FUNCTION app_rtc_julian_from_secs

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

void app_rtc_julian_from_secs
(
  /* Number of seconds since base date */
  UINT32                          secs,

  /* OUT: Pointer to Julian date record */
  EFI_PM_RTC_JULIAN_TYPE        *julian
)
{
  /* Loop index */
  unsigned int /* fast */         i;

  /* Days since beginning of year or quad-year */
  UINT16                          days;

  /* Quad years since base date */
  unsigned int /* fast */         quad_years;

  /* Leap-year or non-leap year month table */
  const UINT16                    *month_table;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add number of seconds from Jan 1 to Jan 6 from input seconds
     in order to have number of seconds since Jan 1, 1980 for calculation */

  secs += TIME_JUL_OFFSET_S;


  /* Divide elapsed seconds by 60: remainder is seconds of Julian date;
     quotient is number of elapsed minutes. */

  secs = app_rtc_div4x2 ( secs, 60, &julian->second );


  /* Divide elapsed minutes by 60: remainder is minutes of Julian date;
     quotient is elapsed hours. */

  secs = app_rtc_div4x2 ( secs, 60, &julian->minute );


  /* Divide elapsed hours by 24: remainder is hours of Julian date;
     quotient is elapsed days. */

  secs = app_rtc_div4x2 ( secs, 24, &julian->hour );


  /* Now things get messier. We have number of elapsed days. The 1st thing
     we do is compute how many leap year sets have gone by. We multiply
     this value by 4 (since there are 4 years in a leap year set) and add
     in the base year. This gives us a partial year value. */

  quad_years = app_rtc_div4x2( secs, TIME_JUL_QUAD_YEAR, &days );

  julian->year = TIME_JUL_BASE_YEAR + (4 * quad_years);


  /* Now we use the rtc_year_table to figure out which year of the leap year
     set we are in. */

  for ( i = 0; days >= rtc_year_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= rtc_year_table[ i ];
  
  /* Use search index to arrive at current year. */
  julian->year += i;  


  /* Take the day-of-week offset for the number of quad-years, add in
     the day-of-week offset for the year in a quad-year, add in the number
     of days into this year. */

  julian->day_of_week =
        (rtc_day_offset[3] * quad_years + rtc_day_offset[i] + days) % 7;


  /* Now we know year, hour, minute and second. We also know how many days
     we are into the current year. From this, we can compute day & month. */


  /* Use rtc_leap_month_table in leap years, and rtc_normal_month_table in other years */

  month_table = (i == 0) ? rtc_leap_month_table : rtc_normal_month_table;


  /* Search month-table to compute month */

  for ( i = 0; days >= month_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }


  /* Compute & store day of month. */
  julian->day = days - month_table[ i ] + 1;  

  /* Store month. */
  julian->month = i + 1;

} /* app_rtc_julian_from_secs */


/*=============================================================================

FUNCTION app_rtc_julian_to_secs

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

UINT32 app_rtc_julian_to_secs
(
  /* Pointer to Julian date record */
  const EFI_PM_RTC_JULIAN_TYPE          *julian
)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  UINT32                          time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First, compute number of days contained in the last whole leap
     year set. */

  time = ( (julian->year - TIME_JUL_BASE_YEAR) / 4L ) * TIME_JUL_QUAD_YEAR;


  /* Now, add in days since last leap year to start of this month. */

  if ( (julian->year & 0x3) == 0 )
  {
    /* If this is a leap year, add in only days since the beginning of the
       year to start of this month. */

    time += rtc_leap_month_table[ julian->month - 1 ];
  }
  else
  {
    /* If this is not a leap year, add in days since last leap year plus
       days since start of this year to start of this month. */

    time += rtc_year_table[ julian->year & 0x3 ];

    time += rtc_normal_month_table[ julian->month - 1 ];
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

} /* app_rtc_julian_to_secs */

/*===========================================================================

FUNCTION app_rtc_div4x2                                  LOCAL FUNCTION

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
static UINT32 app_rtc_div4x2
(
  UINT32 dividend,       /* Dividend, note UINT32     */
  UINT16  divisor,         /* Divisor                  */
  UINT16  *rem_ptr    /* Pointer to the remainder */
)
{
  UINT32 quotient = 0 ;
  if ( divisor != 0 ) 
  {
    *rem_ptr = (UINT16) (dividend % divisor);
    quotient = dividend / divisor ;
  }
  return quotient ;
} /* END app_rtc_div4x2 */
