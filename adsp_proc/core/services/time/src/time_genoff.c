/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
  Implements Generic framework to handle Time bases.
 
 Copyright (c) 2009 - 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
 
 =============================================================================*/
 
 
 /*=============================================================================
 
                         EDIT HISTORY FOR MODULE
 
 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
 $Header: //components/rel/core.qdsp6/2.1/services/time/src/time_genoff.c#1 $
 
 when       who     what, where, why
 --------   ---     ------------------------------------------------------------
 
 =============================================================================*/
 
 

 /*=============================================================================
 
                            INCLUDE FILES
 
 =============================================================================*/

#include <stringl/stringl.h>
#include "time_conv.h"
#include "timetick_sclk64.h"
#include "stringl/stringl.h"
#include "amssassert.h"

/*=============================================================================

FUNCTION TIME_GENOFF_GET_TS_FROM_OFFSET_AND_TIMETICK

DESCRIPTION
  Calculates timestamp from offset and timetick that are passed as inptu args.

DEPENDENCIES
  None

RETURN VALUE
  time in timestamp format

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_get_ts_from_offset_and_timetick
(
  /* Generic offset in timestamp format */
  time_type           sum_genoff_ts,
  /* Sign of offset. FALSE if +ve, True if -ve */
  boolean             is_offset_negative,
  /* timetick at which time is required */
  uint64              timetick,
  /* Calculated timestamp is returned */
  time_type           ts_val  
)
{

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
   qw_set( ts_val, 0, 0 );
  
   /* If offset is not negative, directly use it for
      addition of timetick by copying offset to ts_val */
   if(is_offset_negative == FALSE)
      qw_equ(ts_val, sum_genoff_ts);

   /* Advance the output timestamp by the elapsed slow clocks */
   time_sclk_get(ts_val, timetick);

   /* add / sub sum_generic_offset_ts from uptime based
	  on the state of sum_generic_offset_ts_is_negative */
   if(is_offset_negative == TRUE)
   {
      time_conv_ts_sub(ts_val, sum_genoff_ts);
   }

   return;
} /* time_genoff_get_ts_from_offset_and_timetick */

