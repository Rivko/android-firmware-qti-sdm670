
/*
==============================================================================

FILE:         timetickLegacy.c

DESCRIPTION:  
  This is the legacy timetick file. It access the timetick dal to read 
  the timetick count register. All the timetick values returned from this file 
  are calculated assuming that the underlying frequency is sclk frequency 
  (32768 Hz). 

===============================================================================
*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timetick.h"
#include "DDITimetick.h"

/*=========================================================================   
   
                              DATA DEFINITIONS
   
==========================================================================*/
/*------------------------------------------------------------------------
  Constants used by the timetick driver
------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
  Microsecond threshold for slow clock
     50000 us * 60 kHz < 2^32 micro-cycles
------------------------------------------------------------------------*/
#define TIMETICK_US_THRESHOLD     50000uL

/*-----------------------------------------------------------------------
  Millisecond threshold for slow clock
     60000 ms * 60 kHz < 2^32 milli-cycles
------------------------------------------------------------------------*/
#define TIMETICK_MS_THRESHOLD     60000uL

/*-------------------------------------------------------------------------
  Division factor to convert from sclk ticks to QTimer ticks and viceversa
--------------------------------------------------------------------------*/
#define SCLK_DIV_FACTOR           586

static uint32 timetick_sclk_freq = 32768;

/*
 * phDalTimetickLegacyHandle
 *
 * Handle for communicating with the Timetick DAL.
 */
static DalDeviceHandle *phDalTimetickLegacyHandle = NULL;

/* Information used when acquiring the millisecond value */ 
typedef struct
{
  /* origin for millisecond tick measurements */
  uint32                          origin;

  /* Millisecond count at origin point */
  uint32                          msec;
}
timetick_ms_info_struct_type;

static timetick_ms_info_struct_type ms_info;

/*=============================================================================

                   FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION CVT_TICKS_TO_MS

DESCRIPTION
  Converts the given 32KHz ticks to milliseconds

DEPENDENCIES
  Valid ticks 

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ticks_to_ms
(
  uint32                ticks
    /* Duration to be converted into milliseconds */
)
{
  uint32                milliseconds = 0;
    /* Seconds portion of duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ticks >= timetick_sclk_freq )
  {
    /* Compute seconds portion of time period first, to avoid overflows */
    milliseconds  = (ticks / timetick_sclk_freq) * 1000;
    ticks        %= timetick_sclk_freq;
  }

  milliseconds += (ticks * 1000 + timetick_sclk_freq/2) / timetick_sclk_freq;
    /* Convert remaining ticks to milliseconds, with rounding. */
 
  return milliseconds;

} /* cvt_ticks_to_ms */

/*=============================================================================

FUNCTION CVT_MS_TO_TICKS

DESCRIPTION
  Converts the given millisecond amount to 32KHzticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ms_to_ticks
(
  uint32                msecs
    /* Duration to be converted into ticks */
)
{
  uint32      secs = 0;
    /* Seconds portion of duration */

  uint32      ticks = 0;
    /* Tick duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If duration exceeds 1 minute, compute tick duration in two
     parts, seconds and milliseconds, to avoid uint32 overflow in "ms*f" */

  if ( msecs >= TIMETICK_MS_THRESHOLD )
  {
    secs   = msecs / 1000;
    msecs %= 1000;
    ticks = secs * timetick_sclk_freq  +  msecs * timetick_sclk_freq / 1000;
  }
  else
  {
    ticks = msecs * timetick_sclk_freq / 1000;
  }

  return (ticks);

} /* cvt_ms_to_ticks */

/*=============================================================================

FUNCTION CVT_TIMETICK64_TO_MS

DESCRIPTION
  Converts the given 32KHz ticks value to milliseconds

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_timetick64_to_ms
(
  uint64                          ticks
    /* Duration to be converted into milliseconds */
)
{
  /* Seconds portion of duration */
  uint64                          milliseconds = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ticks >= timetick_sclk_freq )
  {
    /* Compute seconds portion of time period first, to avoid overflows */
    milliseconds  = (ticks / timetick_sclk_freq) * 1000;
    ticks        %= timetick_sclk_freq;
  }

  milliseconds += 
     (ticks * 1000 + timetick_sclk_freq/2) / timetick_sclk_freq;
    /* Convert remaining ticks to milliseconds, with rounding. */

  return milliseconds;

} /* cvt_timetick64_to_ms */


/*=============================================================================

FUNCTION CVT_MS_TO_TIMETICK64

DESCRIPTION
  Converts the given millisecond amount to 32KHz ticks

DEPENDENCIES
  Valid timetick estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_ms_to_timetick64
(
  uint64                          msecs
    /* Duration to be converted into ticks */
)
{
  /* Seconds portion of duration */
  uint64                          secs = 0;
    
  /* Timetick duration */
  uint64                          ticks = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If duration exceeds 1 minute, compute slow clock duration in two
     parts, seconds and milliseconds, to avoid uint32 overflow in "ms*f" */

  if ( msecs >= TIMETICK_MS_THRESHOLD )
  {
    secs   = msecs / 1000;
    msecs %= 1000;
    ticks = ( secs * timetick_sclk_freq  +  
              msecs * timetick_sclk_freq / 1000
              );
  }
  else
  {
    ticks = msecs * timetick_sclk_freq / 1000;
  }

  return (ticks);

} /* cvt_ms_to_timetick64 */


/*=============================================================================

FUNCTION CVT_US_TO_TICKS

DESCRIPTION
  Converts the given microsecond value to 32KHz ticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_us_to_ticks
(
  uint32                microseconds
  /* Duration to be converted into ticks */
)
{
  uint32                          milliseconds = 0;
    /* Milliseconds portion of duration */

  uint32                          ticks = 0;
    /* Duration in ticks */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If duration exceeds 50 miliseconds, compute tick duration
     in two parts, ms and us, to avoid uint32 overflow in "us*f".
     50 miliseconds was chosen as a boundary to avoid overflow and
     still accomodate for timetick frequencies allowed in the
     specification (30-60 kHz) */

  if ( microseconds >= TIMETICK_MS_THRESHOLD )
  {
    milliseconds  = microseconds / 1000;
    microseconds %= 1000;
    ticks = cvt_ms_to_ticks(milliseconds ) +
           (microseconds * timetick_sclk_freq) / 1000000;
  }
  else
  {
    ticks = (microseconds * timetick_sclk_freq) / 1000000;
  }

  return (ticks);

} /* cvt_us_to_ticks */


/*=============================================================================

FUNCTION CVT_US_TO_TIMETICK64

DESCRIPTION
  Converts the given microsecond value to 32KHz ticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_us_to_timetick64
(
  /* Duration to be converted into slow clocks */
  uint64                          microseconds
    
)
{
  /* Milliseconds portion of duration */
  uint64                          milliseconds;
    
  /* Duration in ticks */
  uint64                          ticks;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If duration exceeds 50 miliseconds, compute slow clock duration
     in two parts, ms and us, to avoid uint32 overflow in "us*f".
     50 miliseconds was chosen as a boundary to avoid overflow and
     still accomodate for slow clock frequencies allowed in the
     specification (30-60 kHz) */

  if ( microseconds >= TIMETICK_MS_THRESHOLD )
  {
    milliseconds  = microseconds / 1000;
    microseconds %= 1000;
    ticks = cvt_ms_to_timetick64( milliseconds ) +
           (microseconds * timetick_sclk_freq) / 1000000;
  }
  else
  {
    ticks = (microseconds * timetick_sclk_freq) / 1000000;
  }
  return (ticks);

} /* cvt_us_to_timetick64 */


/*=============================================================================

FUNCTION timetick_attach

DESCRIPTION
  Attaches to the Timetick DAL if it was not already done

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Gives a handle to the Timetick DAL

=============================================================================*/
 
static void timetick_attach(void)
{

  /* Check if the handle for timetick DAL already exists */
  if (phDalTimetickLegacyHandle == NULL)
  {
    if(DAL_SUCCESS == DalTimetick_Attach("SystemTimer", &phDalTimetickLegacyHandle))
    {
      /* Open a connection */
      DalDevice_Open(phDalTimetickLegacyHandle, DAL_OPEN_SHARED);
    }
  }
}
/*=============================================================================

FUNCTION TIMETICK_GET_MIN_REARM_SCLK

DESCRIPTION
  Returns 0 as we do not use it with QTimer.

DEPENDENCIES
  None

RETURN VALUE
  MIN_REARM_SCLK

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_min_rearm_sclk (void)
{
  return 0;
}
/*=============================================================================

FUNCTION TIMETICK_CVT_TO_SCLK

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_to_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Seconds to slow clocks */
      time *= timetick_sclk_freq;
      break;

    case T_MSEC:
      /* Milliseconds to slow clocks */
      time = cvt_ms_to_ticks( time );
      break;

    case T_USEC:
      /* Microseconds to slow clocks */
      time = cvt_us_to_ticks( time );
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }

  return time;

} /* timetick_cvt_to_sclk */


/*=============================================================================

FUNCTION TIMETICK_SCLK_TO_PREC_US

DESCRIPTION
  Converts the given slow clock value to precision microseconds. This function
  also supports the full range of sclk values.

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of microseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_sclk_to_prec_us
(
  uint32      time
    /* Duration in sclks to be converted into microseconds */
)
{

 return (((uint64)time * 1000000) / timetick_sclk_freq);

} /* timetick_sclk_to_prec_us */


/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_SCLK

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_from_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
   /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time /= timetick_sclk_freq * scale;
      break;

    case T_MSEC:
      /* Convert slow clocks to milliseconds */
      time = cvt_ticks_to_ms( time );
      break;

    case T_USEC:
      /* Convert slow clocks to microseconds */
      time = cvt_ticks_to_ms( time * 10 ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }

  return time;

} /* timetick_cvt_from_sclk */

/*=============================================================================

FUNCTION TIMETICK_GET

DESCRIPTION
  Returns the equivalent current timetick count w.r.t 32KHz frequency. 
  The actual count returned from the DAL APIs is 19.2MHz based. We divide it 
  by SCLK_DIV_FACTOR to get the equivalent sclk ticks. 

DEPENDENCIES
  None.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get( void )
{

  DalTimetickTime64Type           count64 = 0;

  timetick_attach();

  if (phDalTimetickLegacyHandle != NULL)
  {
    DalTimetick_GetTimetick64(phDalTimetickLegacyHandle, &count64);
  }

  return ((timetick_type)(count64/SCLK_DIV_FACTOR));

} /* timetick_get */


/*=============================================================================

FUNCTION TIMETICK_INIT_SCLK64

DESCRIPTION
  Does nothing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_init_sclk64( void )
{
  /* do nothing */
} /* timetick_init_sclk64 */


/*=============================================================================

FUNCTION TIMETICK_GET_SCLK64

DESCRIPTION
  Reads the 64 bit timetick count and converts into ticks based on 
  32KHz frequency.

DEPENDENCIES
  None

RETURN VALUE
  64-bit tick count

SIDE EFFECTS
  None

=============================================================================*/


uint64 timetick_get_sclk64( void )
{
 /* timetick in 64 bits */
  uint64                          sclk64 = 0;

  timetick_attach();

  if (phDalTimetickLegacyHandle != NULL)
  {
    DalTimetick_GetTimetick64(phDalTimetickLegacyHandle, &sclk64);
  }
  /* Return 64-bit sclk */
  return (sclk64/SCLK_DIV_FACTOR);

} /* timetick_get_sclk64 */



/*=============================================================================

FUNCTION TIMETICK_GET_ELAPSED

DESCRIPTION
  Compute the time elapsed from a previous timetick value

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Elapsed time, in the unit provided

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_elapsed
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  timetick_type                 end = 0;

  end = timetick_get();
  return timetick_diff(start, end, unit);

} /* timetick_get_elapsed */



/*=============================================================================

FUNCTION TIMETICK_DIFF

DESCRIPTION
  Compute the difference between two slow clock tick counts

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Time difference between the two slow clock tick counts, in the unit given

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_diff
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Time tick value at end of interval */
  timetick_type                   end,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  timetick_type delta = end - start;

  return timetick_cvt_from_sclk(delta, unit);

} /* timetick_diff */



/*=============================================================================

FUNCTION TIMETICK_GET_MS

DESCRIPTION
  Returns a monotonically increasing millisecond count.
  *** This function sacrifices accuracy for speed ***

DEPENDENCIES
  None

RETURN VALUE
  Millisecond tick count

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_ms( void )
{
  timetick_type                   ticks = 0;
  timetick_type                   secs = 0;

  /* Get the elapsed time since the arbitrary origin */
  ticks = timetick_get();
  
  ticks = ticks - ms_info.origin;

  /*** Optimized for slow clock frequency of 32768 Hz = 2^15 ***/
    
  /* Compute number of seconds that have elapsed. */
  secs = ticks >> 15;

  /* If over a second has elapsed, advance origin. */
  if ( secs > 0 )
  {
    ms_info.origin += secs << 15;
    ms_info.msec += secs * 1000;
  }

  /* Compute millisecond timestamp */
  return((((ticks & 0x7FFF) * 1000) >> 15) + ms_info.msec);

} /* timetick_get_ms */


/*=============================================================================

FUNCTION TIMETICK_GET_RAW

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  None

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_raw( void )
{
  return timetick_get();
} /* timetick_get_raw */

/*=============================================================================

FUNCTION TIMETICK_GET_SAFE

DESCRIPTION
    Returns the SLEEP_XTAL_TIMETICK/AGPT timer count.
  
DEPENDENCIES

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_safe( void )
{
  return timetick_get();
} /* timetick_get_safe */


/*=============================================================================

FUNCTION TIMETICK_ENABLE

DESCRIPTION
  Enables the Timetick module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_enable (void)
{
  // do nothing
} /* timetick_enable */

/*=============================================================================

FUNCTION TIMETICK_CVT_TO_SCLK64

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_to_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /* fall-through */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /* fall-through */

    case T_SEC:
      /* Seconds to ticks */
      time *= timetick_sclk_freq;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = cvt_ms_to_timetick64(time );
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = cvt_us_to_timetick64(time );
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
     break;
  }
  return time;

} /* timetick_cvt_to_sclk64 */


/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_SCLK64

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_from_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
 /* Factor for converting to hours or minutes */
  uint64                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time /= timetick_sclk_freq * scale;
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_timetick64_to_ms( time );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = cvt_timetick64_to_ms( time * 10 ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }
  return time;

} /* timetick_cvt_from_sclk64 */

/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_USEC

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_from_usec
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* Converted value in 32-bit integer */
  uint32           converted_time = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      scale *= 1000;

    case T_MSEC:
      scale *= 1000;

    case T_USEC:
      converted_time = (timetick_type)(time/scale);
      break;

    case T_TICK:
      converted_time = cvt_us_to_ticks(time);
      break;

    default:
      break;
  }
  return converted_time;

} /* timetick_cvt_from_usec */


/*=============================================================================



FUNCTION TIMETICK_CVT_TO_USEC

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_to_usec
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  uint64   usecs;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  usecs = (uint64)time;

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      usecs *= 60;
      /* fall-through */
    case T_MIN:
      /* Convert minutes to seconds */
      usecs *= 60;
      /* fall-through */
    case T_SEC:
      /* Seconds to slow clocks */
      usecs *= 1000;
    case T_MSEC:
      /* Milliseconds to slow clocks */
      usecs = usecs * 1000;
    case T_USEC:
      break;
    case T_TICK:
      usecs = ((uint64)time * 1000000) / timetick_sclk_freq;
      break;

    default:
      break;
  }

  return usecs;

} /* timetick_cvt_to_usec */

