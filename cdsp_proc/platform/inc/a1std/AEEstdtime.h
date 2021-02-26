#ifndef AEESTDTIME_H
#define AEESTDTIME_H
/*============================================================================
  Copyright (c) 2008-2014 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
============================================================================*/
#include "AEEStdDef.h"
#include "AEEDateTime.h"

/* GPS, CDMA, BREW Epoch */
static const AEEDateTime dtGPSEpoch  = { 1980, 1, 6, 0, 0, 0, 6 };
/* Unix, Java Epoch */
static const AEEDateTime dtUnixEpoch = { 1970, 1, 1, 0, 0, 0, 3 };

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

extern AEEResult stdtime_AddSeconds(const AEEDateTime* cpdtAddend, int nSecs, 
                                    AEEDateTime* pdtSum);

extern AEEResult stdtime_GetDiffSeconds(const AEEDateTime *cpdtMinuend, 
                                        const AEEDateTime *cpdtSubtrahend,
                                        int *pnDifference);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */
/*============================================================================
==============================================================================
CONSTANTS DOCUMENTATION
==============================================================================

dtGPSEpoch

Description:
   AEEDateTime representation of the GPS Epoch. This Epoch is also used by CDMA and Brew MP.

Definition:
   static const AEEDateTime dtGPSEpoch  = { 1980, 1, 6, 0, 0, 0, 6 }

Comments:
   None

See Also:
   AEEDateTime

==============================================================================

dtGPSEpoch

Description:
   AEEDateTime representation of the UNIX Epoch.  This Epoch is also used by Java.

Definition:
   static const AEEDateTime dtUnixEpoch = { 1970, 1, 1, 0, 0, 0, 3 }

Comments:
   None

See Also:
   AEEDateTime

==============================================================================
INTERFACES DOCUMENTATION
==============================================================================

stdtime Interface

Description:
  Contains procedures to perform operations on Gregorian
  calendar date and time values: day, month, year, hour, minute, second.

  It can be used to convert between Gregorian units and a numeric offset in
  seconds from some fixed point in time.  For example, you can obtain
  Gregorian year, month, and day from the number of seconds into the GPS
  epoch, or obtain the number of seconds into the Unix epoch from a
  Gregorian year, month and day.

  The Gregorian calendar is the one commonly used in modern times.  In this
  calendar, most years are 365 days long, except for leap years.
  
     - Years divisible by 4 are leap years, and years divisible by 400 are leap years, however...
     - Years divisible by 100 are NOT leap years.
     
 See Also:
    AEEDateTime
	stdtime_AddSeconds
	stdtime_GetDiffSeconds

==============================================================================

stdtime_AddSeconds

Description:

   Creates a new time by adding a number of seconds to a time. 
   This conversion does not take leap-seconds into account.

Prototype:
   AEEResult stdtime_AddSeconds(const AEEDateTime* cpdtAddend, int nSecs,
                                AEEDateTime* pdtSum);

Parameters:
   cpdtAddend: time.
   nSecs: number of seconds.
   pdtSum: resulting sum.

Return Value:
   AEE_SUCCESS : successful execution.
   A failure-specific error code : otherwise.

Comments:
   Times follow the Gregorian calender.

See Also:
   AEEDateTime
   Error Codes
   stdtime

==============================================================================

stdtime_GetDiffSeconds

Description:
   Calculates the difference between two times and expresses 
   the difference in seconds.  This does not take leap-seconds into account.

Prototype:
   AEEResult stdtime_GetDiffSeconds(const AEEDateTime *cpdtMinuend,
                                    const AEEDateTime *cpdtSubtrahend,
                                    int *pnDifference);

Parameters:
   cpdtMinuend: time.
   cpdtSubtrahend: time.
   int pnDifference: difference in seconds.

Return Value:
   AEE_SUCCESS : successful execution.
   A failure-specific error code : otherwise.
   
Comments:
   Times follow the Gregorian calender.

See Also:
   AEEDateTime
   Error Codes
   stdtime

============================================================================*/

#endif /* AEESTDTIME_H */

