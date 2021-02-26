#ifndef QSEE_TIME_H
#define QSEE_TIME_H
/*===========================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   rk      Included new header file time_msg.h
02/04/14   rk      Moved out of tzdrm
02/18/11   CM      Initial Version. 
03/04/11   jct     Defined command structures for Time service functions 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup playready
  @} */

#include <stdint.h>
#include "comdef.h" /* Temporary dependency for KM */

#define TZ_MAX_TIME_NAME_LEN    256
#define TZ_MAX_TIME_DATA_LEN    18000

/* The norm_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a non-leap year. 
 */
static const uint16_t norm_month_tab[] = {
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

typedef struct tztimespec
{
  uint32_t      tv_sec;         /* seconds */
  uint32_t      tv_nsec;        /* nanoseconds */
}__attribute__ ((packed)) tztimespec_t;

typedef struct tztm
{
  int         tm_sec;         /* seconds */
  int         tm_min;         /* minutes */
  int         tm_hour;        /* hours */
  int         tm_mday;        /* day of the month */
  int         tm_mon;         /* month */
  int         tm_year;        /* year */
  int         tm_wday;        /* day of the week */
  int         tm_yday;        /* day in the year */
  int         tm_isdst;       /* daylight saving time */
}__attribute__ ((packed)) tztm_t;

/**
 * Check if the specified year is a leap year
 *
 * @param year: Year to check
 *
 * @return int: 1 if yes, 0 otherwise
 */
int time_is_leap_year(unsigned int year);

/**
 * Get the current time
 *
 * @param tzTimeSpec: Pointer of where to return the timespec data
 *
 * @return int: 0 for success, -1 otherwise
 */
int time_getutcsec(struct tztimespec *tzTimeSpec);

/**
 * Get the current system time
 *
 * @param tzTime: Pointer of where to return the system time data
 *
 * @return int: 0 for success, -1 otherwise
 */
int time_getsystime(struct tztm *tzTime);

/**
 * Get the current system time in milliseconds
 *
 * @return int: -1 for failure, system time in milliseconds otherwise
 */
int time_getmsec(void);

/**
 * Notifies the time service that operations can cease.
 *
 * @return int: 0 for success, -1 upon failure
 */
int time_end(void);

#endif //QSEE_TIME_H
