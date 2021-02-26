/***********************************************************************
 * fs_util.h
 *
 * Utility functions for EFS2.
 * Copyright (C) 2003-2005, 2007-2009, 2011, 2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ------------------------------------------------------
2013-10-09   nr    Add safer versions of memory functions with overlap checks.
2013-07-17   wek   Move MD5 stubs into util header file.
2013-04-29   wek   Stub out qword functions for bootloaders.
2013-02-07   nr    Modify fs_unix_time_ms to not check if time is valid.
2013-02-04   rp    Add function to get unix time in milliseconds.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2009-03-25   wek   Move FAT time/date conversions functions to fs_util.c.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-01-30   umr   Integrated Julian time conversions.
2007-01-12   dlb   Provide implementations of safer string routines.
2005-12-06   dlb   Change time simulator for unit test.
2004-10-15   dlb   Update copyright header.
2004-10-07   dlb   Whitespace cleanup.
2003-11-16    gr   Corrected CDMA_TO_FS_TIME_DELTA. The GPS epoch used to
                   compute this value was off by 5 days.
2003-10-17    gr   Moved a couple of definitions in from fs_vnode.h.
2003-10-17    gr   Moved a couple more functions in from elsewhere in EFS2.
2003-10-17    gr   Created.

===========================================================================*/

#ifndef _FS_UTIL_H
#define _FS_UTIL_H

#include "comdef.h"
#include "qw.h"
#include <string.h>

/* If MD5 library is disabled, provide stubs. */
#ifdef FEATURE_EFS_COMPILE_WITHOUT_MD5
typedef int MD5_CTX;

static inline void MD5Init (MD5_CTX *context)
{
  (void) context;
}

static inline void MD5Update (MD5_CTX *context, unsigned char *input,
                              unsigned int inputLen)
{
  (void) context; (void) input; (void) inputLen;
}

static inline void MD5Final (unsigned char digest[16], MD5_CTX *context)
{
  (void) digest; (void) context;
}
#else
  #include "md5.h"
#endif /* FEATURE_EFS_COMPILE_WITHOUT_MD5 */

/* Julian time structure */
typedef struct
{
  uint16  year;        /* Year [1980..2100]                         */
  uint16  month;       /* Month of year [1..12]                     */
  uint16  day;         /* Day of month [1..31]                      */
  uint16  hour;        /* Hour of day [0..23]                       */
  uint16  minute;      /* Minute of hour [0..59]                    */
  uint16  second;      /* Second of minute [0..59]                  */
  uint16  day_of_week; /* Day of the week [0..6] [Monday .. Sunday] */
}
fs_time_julian_type;

/******************************************************************************
 * FUNCTION     FS_TIME_JUL_FROM_SECS
 *
 * DESCRIPTION  This procedure converts a specified number of elapsed seconds
 *              since Jan 6th 1980 (sometimes called CDMA time) to its
 *              equivalent Julian date and time.
 *
 *              It can convert from Jan 6th 1980 00:00:00 (secs = 0) to
 *              Feb 28th 2100 11:59:59.
 *
 * RETURN VALUE The specified Julian date record is filled in with equivalent
 *              date/time, and returned into the area pointed to by julian.
 *
 *****************************************************************************/
void
fs_time_jul_from_secs
(
  /* Number of seconds since base date */
  uint32                          secs,

  /* OUT: Pointer to Julian date record */
  fs_time_julian_type                *julian
);

/******************************************************************************
 * FUNCTION       FS_TIME_JUL_TO_SECS
 *
 * DESCRIPTION    This procedure converts a specified Julian date and time to
 *                an equivalent number of elapsed seconds since Jan 6th 1980.
 *
 *                It can convert from Jan 6th 1980 00:00:00 (secs = 0) to
 *                Feb 28th 2100 11:59:59.
 *
 * RETURN VALUE   Number of elapsed seconds since base date.
 *
 *****************************************************************************/
uint32
fs_time_jul_to_secs
(
  /* Pointer to Julian date record */
  const fs_time_julian_type          *julian
);

/*
 * The number of seconds to add to CDMA time to convert to POSIX(TM) time.
 * CDMA time starts at Jan 6, 1980.  POSIX(TM) time starts at Jan 1, 1970.  In
 * between those two dates are two years, 1972 and 1976 which are leap years.
 * To figure out the delta, the equation is as follows:
 *
 * 3657 days = 315964800 seconds = 0x12D53D80
 */
#define CDMA_TO_FS_TIME_DELTA    (0x12D53D80)
#define CDMA_TO_FS_TIME_DELTA_MS (0x12D53D80ULL * 1000)

/*
 * The time returned by clk_read_secs() must be at least this great, or the
 * time is bogus and we might as well return 0.  This will indicate that we no
 * not yet have a valid time (i.e. have not yet received a Sync Channel Message
 * with valid time in it.  We will use a flat 20 years with no leap days for
 * the calculation.
 *
 *       20 years * 365 days/year = 7300 days
 *       7300 days * 24 hours/day = 175200 hours
 *       175200 hours * 60 min/hr = 10512000 minutes
 *       10512000 min * 60 sec/min = 630720000 seconds = 0x25980600 seconds
 */
#define CDMA_VALID_TIME     (0x25980600)

/***********************************************************************
 * FUNCTION      fs_time
 *
 * DESCRIPTION   This is a function to return the time in seconds
 *               since Jan 1, 1970 which is the starting point for time
 *               according the the POSIX(TM) standard.
 *
 * RETURN VALUE  Time in seconds since Jan 1, 1970
 **********************************************************************/
unsigned int
fs_time (void);

/***********************************************************************
 * FUNCTION      fs_unix_time_ms
 *
 * DESCRIPTION   This is a function to return the time in milliseconds
 *               since Jan 1, 1970 which is the starting point for time
 *               according the the POSIX(TM) standard.
 *
 * RETURN VALUE  Time in milliseconds since Jan 1, 1970
 **********************************************************************/
uint64
fs_unix_time_ms (void);

/***********************************************************************
 * FUNCTION      fs_time_ms
 *
 * DESCRIPTION   Returns the time in milliseconds since Jan 6, 1980
 *               CDMA Time.
 **********************************************************************/
void
fs_time_ms (qword now);

/***********************************************************************
 * FUNCTION      fs_time_ms_u64
 *
 * DESCRIPTION   Returns the time in milliseconds since Jan 6, 1980
 *               CDMA Time. Same as fs_time_ms, but result is a uint64.
 **********************************************************************/
uint64
fs_time_ms_u64 (void);

/***********************************************************************
 * FUNCTION      fs_util_wait
 *
 * DESCRIPTION   Waits for a specified time in milliseconds.
 *               It is assumed that FS_OP_COMPLETE_SIG will not be
 *               set for the calling task while in this function waiting
 *               for timer expiration.
 **********************************************************************/
void
fs_util_wait (int msec);

/****************************************************************************
 * FUNCTION      fs_posix_to_fat_date
 *
 * DESCRIPTION   Converts an POSIX time to an FAT Format Date. The maximum
 *               value we can convert to fat date is Feb 7th 2106 06:28:15
 *               due to the limitations of using an uint32 to represent
 *               posix time.
 *
 * PARAMETERS
 *       ptime   Time in seconds since Jan 1 1970 (POSIX TIME)
 *
 * RETURNS       Date in FAT format. All dates before Jan 1st 1980 will be
 *               rounded to Jan 1st 1980.
 ***************************************************************************/
uint16 fs_posix_to_fat_date (uint32 ptime);

/****************************************************************************
 * FUNCTION      fs_posix_to_fat_time
 *
 * DESCRIPTION   Converts an POSIX time to an FAT Format Time.
 *
 * PARAMETERS
 *       ptime   Time in seconds since Jan 1 1970 (POSIX TIME)
 *
 * RETURNS       Time in FAT format. All dates before Jan 1st 1980 00:00:00
 *               will be rounded to 00:00:00.
 ***************************************************************************/
uint16 fs_posix_to_fat_time (uint32 ptime);

/****************************************************************************
 * FUNCTION      fs_fat_to_posix_time
 *
 * DESCRIPTION   Converts an FAT time date format to POSIX time. The maximum
 *               value we can convert from fat date is Feb 7th 2106 06:28:15
 *               due to the limitations of using an uint32 to represent
 *               posix time. Any time above that will be rounded down to
 *               the this maximum date or the equivalent 0xFFFFFFFF.
 *
 * PARAMETERS
 *    fat_time   FAT representation of time (two byte for time of day)
 *    fat_date   FAT representation of date (two byte for date)
 *
 * RETURNS       Time since POSIX epoch, seconds since Jan 1st 1970.
 ***************************************************************************/
uint32 fs_fat_to_posix_time (uint16 fat_time, uint16 fat_date);


/****************************************************************************
 * FUNCTION      fs_qw_hi
 *
 * DESCRIPTION   This function returns the most significant 32 bits of a
                 specified qword.
 *
 * PARAMETERS
 *    qw         The qword whose most significant 32-bits are needed.
 *
 * RETURNS       A uint32 equal to the high order 32 bit bits of the qword.
 ***************************************************************************/
uint32 fs_qw_hi (qc_qword qw);

/****************************************************************************
 * FUNCTION      fs_qw_lo
 *
 * DESCRIPTION   This function returns the least significant 32 bits of a
                 specified qword.
 *
 * PARAMETERS
 *    qw         The qword whose least significant 32-bits are needed.
 *
 * RETURNS       A uint32 equal to the low order 32 bit bits of the qword.
 ***************************************************************************/
uint32 fs_qw_lo (qc_qword qw);

/****************************************************************************
 * FUNCTION      fs_qw_sub
 *
 * DESCRIPTION   This function subtracts a specified qword subtractor from a
                 specified qword subtrahend and places the result in a
                 specified qword difference.
 *
 * PARAMETERS
 *  difference   The result of the subtraction.
 *  subtrahend   The first argument for subtraction.
 *  subtractor   The second argument for subtraction.
 ***************************************************************************/
void fs_qw_sub (qc_qword difference, qc_qword subtrahend,qc_qword subtractor);

/****************************************************************************
 * FUNCTION      fs_memscpy
 *
 * DESCRIPTION   This function copies bytes from the source buffer to the
                 destination buffer. This function ensures that there will not
                 be a copy beyond the size of the destination buffer. The
                 result of calling this on overlapping source and destination
                 buffers is undefined.
 *
 * PARAMETERS
 *   dst       Destination buffer.
 *   dst_size  Size of the destination buffer in bytes.
 *   src       Source buffer.
 *   src_size  Number of bytes to copy from source buffer
 ***************************************************************************/
void fs_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

/****************************************************************************
 * FUNCTION      fs_memsmove
 *
 * DESCRIPTION    Moves bytes from the source buffer to the destination buffer.
                  This function ensures that there will not be a copy beyond
                  the size of the destination buffer.

                  This function should be used in preference to memscpy() if
                  there is the possiblity of source and destination buffers
                  overlapping. The result of the operation is defined to be as
                  if the copy were from the source to a temporary buffer that
                  overlaps neither source nor destination, followed by a copy
                  from that temporary buffer to the destination.
 *
 * PARAMETERS
 *   dst       Destination buffer.
 *   dst_size  Size of the destination buffer in bytes.
 *   src       Source buffer.
 *   src_size  Number of bytes to copy from source buffer
 ***************************************************************************/
void fs_memsmove(void *dst, size_t dst_size, const void *src, size_t src_size);

#endif /* End of _FS_UTIL_H */

