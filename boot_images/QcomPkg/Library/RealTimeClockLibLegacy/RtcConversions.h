#ifndef __RTCCONVERSIONS_H__
#define __RTCCONVERSIONS_H__

/*! @file RtcConversions.h 

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

#include <PiDxe.h> /* Needed for compilation */
#include <Library/BaseLib.h>
#include <Protocol/EFIPmicRTC.h>

/*===========================================================================
                       FUNCTION PROTOTYPES
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
);


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
  const EFI_PM_RTC_JULIAN_TYPE   *julian
);

#endif // __RTCCONVERSIONS_H__
