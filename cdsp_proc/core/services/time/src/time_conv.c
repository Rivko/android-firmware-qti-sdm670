/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements conversion of time into different units. 


Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/services/time/src/time_conv.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/09/16   abh     Changes in the inline function to maintain C99 std.
03/16/12   cp      Mod'd file for 64bit native math and 19.2MHz XO Freq.
07/12/10   din     Corrected overflow error in time_conv_add_to_timestamp.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_conv.h"
#include "err.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Useful constants
-----------------------------------------------------------------------------*/

/* Mask to extract cx32's (lower 16 bits) from timestamp */
#define TIME_CONV_CX32_MASK            0x0ffffuLL

/* To convert TS to 1.25ms units, shift out cx32 bits of timestamp */
#define TIME_CONV_TO_1p25MS_SHIFT      16

/* Current XO's per sclk for conversion */
#define TIME_CONV_XOS_PER_SCLK         586
/* Max supported sclks = (2^56 - 1)/XOS_PER_SCLK  */
#define TIME_CONV_MAX_SCLK_SUPPORTED   (0xFFFFFFFFFFFFFFuLL / ((uint64)TIME_CONV_XOS_PER_SCLK))

/* Frequency of Sclk used */
#define TIME_CONV_SCLK_FREQ            19200000
/* Number of ticks per ms */
#define TIME_CONV_SCLK_FREQ_MS         (TIME_CONV_SCLK_FREQ/1000)
/* Dividend of factor to multiply for converting sclk to cx32 */
#define TIME_CONV_SCLK_CX32_DIVIDEND   256
/* Divisor of factor to multiply for converting sclk to cx32 */
#define TIME_CONV_SCLK_CX32_DIVISOR    125

/* Conversion from cx32 to 1.25ms */
#define TIME_CONV_CX32_PER_1p25MS      0xC000
/* Conversion from cx32 to 0.25ms */
#define TIME_CONV_CX32_PER_0p25MS      ((49152+3)/5) /* 0xC000/5 */
#define TIME_CONV_0p25MS_PER_1p25MS    5

/* To convert timestamp to 40ms, first shift out cx32 bits of timestamp, and
   then shift down an addition 5, to convert 1.25ms units to 40ms units */
#define TIME_CONV_TO_40MS_SHIFT        (TIME_CONV_TO_1p25MS_SHIFT+5)

/* # of 40ms units in 1 second (1000ms) */
#define TIME_CONV_40MS_PER_SEC         (1000/40)

/* Seconds per TimeZone (30 min/zone x 60sec/min) */
#define TIME_CONV_SEC_PER_TIME_ZONE    (30 * 60)

/* Shift from 0.25ms units to 1ms units */
#define TIME_CONV_0p25MS_TO_1MS_SHIFT  2

/* To convert 1.25ms units to TS units, shift in cx32 bits of timestamp */
#define TIME_CONV_1p25MS_TO_TS_SHIFT   16

/* One second (800 x 1.25ms) expressed as a timestamp format value */
#define TIME_CONV_1_SEC                 (800uL << 16)


/*=============================================================================

FUNCTION TIME_CONV_UINT64_TO_QW

DESCRIPTION
  Convert a 64bit native type to qword

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static inline void time_conv_uint64_to_qw(uint64 native, time_type legacy)
{
  legacy[0] = (uint32)(native&0xFFFFFFFFuLL);
  legacy[1] = (uint32)((native>>32)&0xFFFFFFFFuLL);
}

static inline uint64 time_conv_qw_to_uint64(time_type legacy)
{
  uint64 native = (((uint64)legacy[1]) << 32) | ((uint64)legacy[0]);
  return native;
}


/*=============================================================================

FUNCTION TIME_CONV_TO_MS

DESCRIPTION
  Convert a timestamp from System Time to millisecond units

DEPENDENCIES
  None

RETURN VALUE
  The converted value is stored in the 'time' parameter.

SIDE EFFECTS
  None

=============================================================================*/

void time_conv_to_ms
(
  /* In: Time in time-stamp format;  Out: Time in milliseconds */
  time_type                       ts_val
)
{
  /* Lower 16 bits of the timestamp (chipx32's) */
  uint64                          cx32;
  uint64                          local_ts;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*  Convert to local timestamp for math */
  local_ts = time_conv_qw_to_uint64(ts_val);

  /* Save lower 16 bits (cx32's) of timestamp */
  cx32 = (uint64) (local_ts & TIME_CONV_CX32_MASK);

  /* Shift down by 16 to convert to get 1.25ms ticks */
  local_ts >>= TIME_CONV_TO_1p25MS_SHIFT;

  /* Multiply by 5 to convert 1.25ms ticks to 0.25ms ticks */
  local_ts *= TIME_CONV_0p25MS_PER_1p25MS;

  /* Convert cx32's to 0.25ms ticks and add, plus 2 for rounding (below) */
  local_ts += (cx32 / TIME_CONV_CX32_PER_0p25MS) + 2;

  /* Divide by 4 to get milliseconds ticks (rounded via +2, above) */
  local_ts >>= TIME_CONV_0p25MS_TO_1MS_SHIFT;

  time_conv_uint64_to_qw(local_ts,ts_val);

} /* time_conv_to_ms */



/*=============================================================================

FUNCTION TIME_CONV_TO_SEC

DESCRIPTION
  Convert a timestamp from System Time to seconds

DEPENDENCIES
  None

RETURN VALUE
  Seconds, corresponding to the time stamp

SIDE EFFECTS
  Input argument value is destroyed.

=============================================================================*/

uint32 time_conv_to_sec
(
  /* In: Time in time-stamp format;  Out: Garbage */
  time_type                       ts_val
)
{
  uint64                          local_ts;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert qword to uint64 for math */
  local_ts = time_conv_qw_to_uint64(ts_val);

  /* Shift down to convert to 40ms ticks */
  local_ts >>= TIME_CONV_TO_40MS_SHIFT;

  /* Divide to convert 40ms ticks to seconds */
  local_ts /= TIME_CONV_40MS_PER_SEC;

  /* Return the lower-order word, giving a 136 year range */
  return (uint32)local_ts;

} /* time_conv_to_sec */



/*=============================================================================

FUNCTION TIME_CONV_MS_TO_TIMESTAMP

DESCRIPTION
  Converts from milliseconds to timestamp.  ts_val receives the timestamp after
  the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_ms_to_timestamp
(
  /* IN : Timestamp in ms, OUT : Timestamp in timestamp format  */
  time_type                       ts_val
)
{
  uint64                          local_ts;
  uint64                          cx32;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert to uint64 for math */
  local_ts = time_conv_qw_to_uint64(ts_val);

  /* Convert from ms to 1.25 ms units */
  cx32 = local_ts *= 4;
  local_ts /= 5;
  cx32 -= local_ts * 5;
  cx32 *= TIME_CONV_CX32_PER_0p25MS;
  
  /* Shift up by 16 to convert to get timestamp */
  local_ts <<= TIME_CONV_1p25MS_TO_TS_SHIFT;
  local_ts |= (cx32 & 0xFFFFuLL);

  /* Convert back to qword for return value */
  time_conv_uint64_to_qw(local_ts,ts_val);

} /* time_conv_ms_to_timestamp */



/*=============================================================================

FUNCTION TIME_CONV_SECS_TO_TIMESTAMP

DESCRIPTION
  Converts from seconds to timestamp.  ts_val receives the timestamp after
  the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_secs_to_timestamp
(
  /* IN : Timestamp in secs, OUT : Timestamp in timestamp format  */
  time_type                       ts_val
)
{
  uint64                          local_ts;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert to uint64 for math */
  local_ts = time_conv_qw_to_uint64(ts_val);

  local_ts *= TIME_CONV_1_SEC;

  /* Convert back to qword for return */
  time_conv_uint64_to_qw(local_ts,ts_val);

} /* time_conv_secs_to_timestamp */



/*=============================================================================

FUNCTION TIME_CONV_TO_20MS_FRAME_TIME

DESCRIPTION
  Converts from timestamp to 2oms frame time.ts_val receives the timestamp 
  after the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_to_20ms_frame_time
(
  /* Timestamp holder to convert to */
  time_type                       ts_val
)
{
  uint64                          local_ts;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert to uint64 for math */
  local_ts = time_conv_qw_to_uint64(ts_val);

  /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
     then divide the 1.25 ms units by 16 to get 20ms units.
     A divide by 16*(64*1024) = 2^20. So, use a bitshift for speed */

  local_ts >>= 20;

  time_conv_uint64_to_qw(local_ts,ts_val);

} /* time_conv_to_20ms_frame_time */



/*=============================================================================

FUNCTION TIME_CONV_ADD_OFFSET_TO_TIMESTAMP

DESCRIPTION
  Adds offset( in ms ) to timestamp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the added timestamp.

=============================================================================*/

void time_conv_add_offset_to_timestamp
(
  /* Timestamp holder to convert to */
  time_type                       ts_val,

  /* Offset to be added, in ms */
  int64                           offset
)
{
  /* Advance time in form of sclks, most accurate way */
  timetick_sclk64_add_xo(ts_val,offset*TIME_CONV_SCLK_FREQ_MS);

} /* time_conv_add_offset_to_timestamp */



/*=============================================================================

FUNCTION TIMETICK_SCLK64_ADD_SCLKS

DESCRIPTION
  Adds offset( in ms ) to timestamp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the added timestamp.

=============================================================================*/

void timetick_sclk64_add_sclks
(
  time_type                       timestamp_io,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */
  uint64                          sclks
    /* Number of slow clocks to advance timestamp by */
)
{
  /* Check if we support adding the number of sclks requested, multiplication overflow */
  if ( sclks > TIME_CONV_MAX_SCLK_SUPPORTED )
  {
    /* TODO: Use error type and return error_unsupported */
    ERR_FATAL("Adding sclks[1]=%i sclks[0]=%i sclks to timestamp is not supported, \
              use timetick_sclk64_add_xo instead",
              (uint32)(sclks>>32),
              (uint32)(sclks&0xFFFFFFFFuLL),0);
  }

  timetick_sclk64_add_xo(timestamp_io,TIME_CONV_XOS_PER_SCLK*sclks);
} /* timetick_sclk64_add_sclks( ) */ 


/*=============================================================================

FUNCTION TIMETICK_SCLK64_ADD_XO

DESCRIPTION
  Adds offset( in ms ) to timestamp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the added timestamp.

=============================================================================*/

void timetick_sclk64_add_xo
(
  time_type                       timestamp_io,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */
  uint64                          xos
    /* Number of slow clocks to advance timestamp by */
)
{
  uint64                           timestamp,cx32,cx32_xos,ticks_1_25ms;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  timestamp = time_conv_qw_to_uint64(timestamp_io);
  cx32 = timestamp & 0xffffuLL;
  timestamp &= ~0xffffULL;

  /* Convert to cx32 units */
  cx32_xos = (xos * TIME_CONV_SCLK_CX32_DIVIDEND)/TIME_CONV_SCLK_CX32_DIVISOR;

  /* Add the number of 1.25ms units to the timestamp */
  cx32 += cx32_xos;
  ticks_1_25ms = cx32/TIME_CONV_CX32_PER_1p25MS;
  timestamp += (ticks_1_25ms << 16);

  /* Determine the remaining cx32s that we didn't add to the timestamp's upper part */
  cx32 -= (ticks_1_25ms * TIME_CONV_CX32_PER_1p25MS);

  /* Attach the cx32 back to end of the timestamp */
  timestamp |= cx32&0xffffULL;

  time_conv_uint64_to_qw(timestamp, timestamp_io);

} /* timetick_sclk64_add_xo( ) */ 

/*=============================================================================

FUNCTION TIME_CONV_TS_ADD

DESCRIPTION
  Adds two timestamps

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  First Timestamp will be updated with total.
=============================================================================*/
void time_conv_ts_add(time_type timestamp_io, time_type timestamp_to_add)
{
   uint64 cx1, cx2;
   uint64 su1, su2;
   
   su1 = time_conv_qw_to_uint64(timestamp_io);
   su2 = time_conv_qw_to_uint64(timestamp_to_add);

   cx1 = (su1 & 0xFFFFuLL);
   cx2 = (su2 & 0xFFFFuLL);
   
   su1 &= ~0xFFFFuLL;
   su2 &= ~0xFFFFuLL;
   
   cx1 += cx2;
   su1 += su2;
   
   if(cx1 >= TIME_CONV_CX32_PER_1p25MS)
   {
      cx1 -= TIME_CONV_CX32_PER_1p25MS;
      su1 += 0x10000uLL; /* Add 1 to 1.25 units */
   }
   su1 |= (cx1 & 0xFFFFuLL);
   time_conv_uint64_to_qw(su1, timestamp_io);
} /* time_conv_ts_add */


/*=============================================================================

FUNCTION TIME_CONV_TS_SUB

DESCRIPTION
  Subtracts one timestamp from another. Condition is first timestamp should be 
  greater than second timestamp.

DEPENDENCIES
  first input should be greater than second input

RETURN VALUE
  None

SIDE EFFECTS
  First Timestamp will be updated with total.

=============================================================================*/

void time_conv_ts_sub
(
  time_type                       timestamp_io,
    /* In: Base timestamp;  Out: Difference of the two timestamps */
  time_type                       timestamp_to_sub
    /* In: Timestamp to subract from timestamp_io */
)
{
  uint64                           timestamp, timestamp_subed, cx32, cx32_diff;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  timestamp = time_conv_qw_to_uint64(timestamp_io);
  timestamp_subed = time_conv_qw_to_uint64(timestamp_to_sub);
  
  cx32 = timestamp & 0xffffuLL;
  cx32_diff = ( timestamp_subed & 0xffffuLL );
  
  timestamp &= ~0xffffULL;
  timestamp_subed &= ~0xffffULL;

  timestamp -= timestamp_subed;

  if ( cx32_diff > cx32 )
  {
    cx32 = TIME_CONV_CX32_PER_1p25MS - ( cx32_diff - cx32 );
    timestamp -= ( 0x1 << 16 );
  }
  else
  {
    cx32 = cx32 - cx32_diff;
  }

  /* Attach the cx32 back to end of the timestamp */
  timestamp |= cx32 & 0xffffULL;

  time_conv_uint64_to_qw(timestamp,timestamp_io);

} /* time_conv_ts_sub */ 
