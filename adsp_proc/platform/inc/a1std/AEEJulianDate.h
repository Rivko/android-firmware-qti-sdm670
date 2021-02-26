#ifndef AEEJULIANDATE_H
#define AEEJULIANDATE_H
/*
=======================================================================

FILE:         AEEJulianDate.h

SERVICES:     JulianDate type

=======================================================================
  Copyright (c) 2005-2014 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
=======================================================================
*/
#include "AEEStdDef.h"
#include "AEEstdtime.h"

typedef struct JulianDate {
   uint16      wYear;
   uint16      wMonth;
   uint16      wDay;
   uint16      wHour;
   uint16      wMinute;
   uint16      wSecond;
   uint16      wWeekDay;
} JulianDate;

#define DATETIME_TO_JULIANDATE(__dt, __jd) \
   do { \
      __jd.wYear = __dt.wYear; \
      __jd.wMonth = __dt.wMonth; \
      __jd.wDay = __dt.wDay; \
      __jd.wHour = __dt.wHour; \
      __jd.wMinute = __dt.wMinute; \
      __jd.wSecond = __dt.wSecond; \
      __jd.wWeekDay = __dt.wWeekDay; \
   } while (0)

#define JULIANDATE_TO_DATETIME(__jd, __dt) \
   do { \
      __dt.wYear = __jd.wYear; \
      __dt.wMonth = __jd.wMonth; \
      __dt.wDay = __jd.wDay; \
      __dt.wHour = __jd.wHour; \
      __dt.wMinute = __jd.wMinute; \
      __dt.wSecond = __jd.wSecond; \
      __dt.wWeekDay = __jd.wWeekDay; \
   } while (0)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

JulianDate

Description:
This structure contains Julian date information.

Definition:
typedef struct JulianDate
{
   uint16 wYear;
   uint16 wMonth;
   uint16 wDay;
   uint16 wHour;
   uint16 wMinute;
   uint16 wSecond;
   uint16 wWeekDay;
} JulianDate;

Members:

wYear : 4-digit year
wMonth  :  Month 1-12(January=1, December=12)
wDay : Day 1-31
wHour : Hour 0-23
wMinute : Minute 0-59
wSecond : Seconds 0-59
wWeekDay : Day of the week 0-6 (0=Monday, 6=Sunday)

Comments:
None

See Also:
None

======================================================================= */

#endif /* #ifndef AEEJULIANDATE_H */

