#ifndef AEEDATETIME_H
#define AEEDATETIME_H
/*==============================================================================
File:         AEEDateTime.h

Description:  definitions, typedefs, etc. for Date and Time
   
================================================================================
        Copyright 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "AEEStdDef.h"

typedef struct AEEDateTime {
   uint16 wYear;
   uint16 wMonth;
   uint16 wDay;
   uint16 wHour;
   uint16 wMinute;
   uint16 wSecond;
   uint16 wWeekDay;
} AEEDateTime;

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

AEEDateTime

Description:
   This structure contains date and time information.

Definition:
   typedef struct AEEDateTime {
      uint16 wYear;
      uint16 wMonth;
      uint16 wDay;
      uint16 wHour;
      uint16 wMinute;
      uint16 wSecond;
      uint16 wWeekDay;
   } AEEDateTime;

Members:
   wYear : 4-digit year 
   wMonth : Month 1-12 (January=1, December=12) 
   wDay : Day 1-31
   wHour : Hour 0-23 
   wMinute : Minute 0-59 
   wSecond : Seconds 0-59 
   wWeekDay : Day of the week 0-6 (0=Monday, 6=Sunday)

Comments: 
   None

See Also:
   None
================================================================================
==============================================================================*/

#endif /* ifndef AEEDATETIME_H*/

