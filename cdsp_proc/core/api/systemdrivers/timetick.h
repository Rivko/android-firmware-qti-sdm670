#ifndef TIMETICK_H
#define TIMETICK_H
/**
* @file timetick.h
* @brief Timetick Device Driver Interface Header File.
* This file contains the definitions of the constants, data
* structures, and interfaces that comprise the Timetick driver. 
*
* This file is deprecated. Use DDITimetick.h instead.
*/

/*
===========================================================================
                             Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/timetick.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright © 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/
#include "DDITimetick.h"

/** 
  @addtogroup deprecated_functions
  @{ 
*/

/**
 * Conversion from milliseconds to slow clocks at the nominal 
 * frequency 
 */
#define TIMETICK_SCLK_FROM_MS(ms) ((ms)*TIMETICK_NOMINAL_FREQ_HZ/1000)

/**
 * Conversion from microseconds to slow clocks at the nominal 
 * frequency 
 */
#define TIMETICK_SCLK_FROM_US(us) ((us)*TIMETICK_NOMINAL_FREQ_HZ/1000000)

/**
 * Nominial slow clock freq in hertz.
 */
#ifndef TIMETICK_NOMINAL_FREQ_HZ
  #define TIMETICK_NOMINAL_FREQ_HZ      32768uL
#endif 

typedef DalTimetickUnitType  timer_unit_type; /**< -- Various units supported by the timetick module*/
typedef DalTimetickTime32Type  timetick_type; /**< -- Time in 32bit ticks*/ 

#define T_SCLK  T_TICK

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/* ============================================================================
**  Function : timetick_get_min_rearm_sclk
** ============================================================================
*/
/*!
    @brief
    Returns the minimum rearm sclk value 
    
    @details
    Reading the current time, and programming the timetick match value 
    takes a finite period of time. "minimum rearm sclk" is used to ensure the match 
    value to be set is at least this many ticks after the value that was just 
    read. This value may vary based upon hardware, but for simplicity 
    timetick driver uses the maximum necessary value.
    
    @par Dependencies
    Regional function.  It may only be called from time*.c
    
    @par Side Effects
    None 
    
    @retval returns MIN_REARM_SCLK value always
    
    @sa DalTimetick_GetMinRearmSclk
*/
timetick_type timetick_get_min_rearm_sclk (void);


/* ============================================================================
**  Function : timetick_cvt_to_sclk
** ============================================================================
*/
/*!
    @brief
    Converts the given time value to ticks
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] time   Time interval to convert to ticks
    @param[in] unit   Units of time interval

    @par Dependencies
    Valid timetick frequency estimate
    
    @par Side Effects
    None 
    
    @retval returns # of ticks which correspond to the 
                              given time value
    
    @sa DalTimetick_CvtToTimetick32
*/
timetick_type timetick_cvt_to_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : timetick_cvt_from_sclk
** ============================================================================
*/
/*!
    @brief
    Converts the ticks to the given unit
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] time   Time interval to convert from ticks
    @param[in] unit   Units of time interval
 
    @par Dependencies
    Valid timetick frequency estimate
    
    @par Side Effects
    None 
    
    @retval returns Time in the unit requested 
    
    @sa DalTimetick_CvtFromTimetick32
*/
timetick_type timetick_cvt_from_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);



/* ============================================================================
**  Function : timetick_sclk_to_prec_us
** ============================================================================
*/
/*!
    @brief
    Converts the given ticks to precision microseconds.
        
    @details
    This function also supports the full range of timeticks.

    @param[in] sclks Duration in ticks to be converted into 
                      microseconds   

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns number of microseconds which corresponds
                      to the given time value
    
    @sa DalTimetick_TicksToPrecUs
*/
uint64 timetick_sclk_to_prec_us
(
  uint32      sclks
    /* Duration in sclks to be converted into microseconds */
);


/* ============================================================================
**  Function : timetick_get_safe
** ============================================================================
*/
/*!
    @brief
    Returns the current timetick count, corrected with the offset.
    
    @details
    The modem/application processors' h/w counter, may have an offset due to
    power collapse, w.r.t. MPM's timetick counter for e.g. This difference is 
    eliminated by adding an offset to the value read from the processor's 
    timetick counter.

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns Timetick counter value
    
    @sa DalTimetick_Get
*/
timetick_type timetick_get_safe( void );


/* ============================================================================
**  Function : timetick_get
** ============================================================================
*/
/*!
    @brief
    Returns the current timetick count, corrected with the offset.
    
    @details
    The modem/application processors' h/w counter, may have an offset due to
    power collapse, w.r.t. MPM's timetick counter for e.g. This difference is 
    eliminated by adding an offset to the value read from the processor's 
    timetick counter.

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns Timetick counter value
    
    @sa DalTimetick_Get
*/
timetick_type timetick_get( void );


/* ============================================================================
**  Function : timetick_get_raw
** ============================================================================
*/
/*!
    @brief
    Returns the current count after reading the timetick counter, without adding 
    any offset.
    
    @details
    This is the raw value, directly read from the hardware timer count register.
    
    @par Dependencies
    None
        
    @par Side Effects
    None 
    
    @retval returns Timetick counter value
    
    @sa DalTimetick_GetRaw
*/
timetick_type timetick_get_raw( void );


/* ============================================================================
**  Function : timetick_get_elapsed
** ============================================================================
*/
/*!
    @brief
    Computes the time elapsed from a previous timetick value
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] start   Time tick value at the start of the interval
    @param[in] unit   Units to return time interval in
                
    @par Dependencies
    Valid timetick estimate
    
    @par Side Effects
    None 
    
    @retval returns Elapsed time, in the unit provided
    
    @sa DalTimetick_GetElapsed
*/
timetick_type timetick_get_elapsed
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Units to return time interval in */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : timetick_diff
** ============================================================================
*/
/*!
    @brief
    Computes the difference between two timetick counts
    
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] start   Time tick value at the start of the interval
    @param[in] end   Timetick value at the end of the interval
    @param[in] unit   Units to return time interval in
                
    @par Dependencies
    Valid timetick estimate
    
    @par Side Effects
    None 
    
    @retval returns Time difference between the two timetick 
                              counts, in the unit given
    
    @sa DalTimetick_Diff
*/
timetick_type timetick_diff
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Time tick value at end of interval */
  timetick_type                   end,

  /* Units to return time interval in */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : timetick_get_ms
** ============================================================================
*/
/*!
    @brief
    Returns the timetick count in milliseconds.
    
    @details
    The value returned is unrelated to CDMA, GPS, HDR, or Time of Day 
    time-stamps. It will drift with respect to the above time references. 
    This function sacrifices accuracy for speed.

    @par Dependencies
    None
    
    @par Side Effects
    None 
    
    @retval returns Millisecond tick count
    
    @sa DalTimetick_GetMs
        DalTimetick_CvtFromTimetick32
*/
timetick_type timetick_get_ms( void );

/* ============================================================================
**  Function : timetick_init_sclk64
** ============================================================================
*/
/*!
    @brief
    This function initializes the 64-bit timetick system by defining a timer 
    to track the rollover of the counter. 
    
    @details
   
    @par Dependencies
    None
    
    @par Side Effects
    Timetick_GetTimetick64 can be used after this is called  
    
    @retval None
 
    @sa DalTimetick_InitTimetick64
*/
void timetick_init_sclk64( void );

/* ============================================================================
**  Function : timetick_enable
** ============================================================================
*/
/*!
    @brief
    Enables/Disables the Timetick module

    @details
 
    @param[in] enable  Boolean to enable/disable the module.
                   
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval None 
    
    @sa DalTimetick_Enable
*/
void timetick_enable( void );


/* ============================================================================
**  Function : timetick_cvt_to_usec
** ============================================================================
*/
/*!
    @brief
    Converts the given value to microseconds.
 
    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] time   Time interval to convert from
    @param[in] unit   Units of time interval
 
    @par Dependencies
    Valid timetick frequency estimate
 
    @par Side Effects
    None
 
    @retval returns Time in microseconds
    
    @sa DalTimetick_CvtToUsec
*/
uint64 timetick_cvt_to_usec
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : DalTimetick_CvtFromUsec
** ============================================================================
*/
/*!
    @brief
    Converts the given microsecs value to ticks.

    @details
    Supports various units of time, ranging from hours to microseconds.

    @param[in] time   Time in microseconds
    @param[in] unit   Units of time interval to convert to

    @par Dependencies
    Valid timetick frequency estimate
 
    @par Side Effects
    None
 
    @retval returns Time in the unit requested

    @sa DalTimetick_CvtFromUsec    
*/

timetick_type timetick_cvt_from_usec
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : timetick_get_sclk64
** ============================================================================
*/
/*!
    @brief
    Reads the timetick counter and adds the rollover offset to provide 
    64 bit dynamic range.
    
    @details

    @par Dependencies
    timetick_init_sclk64 must be called first to initialize the timer, 
    before calling this API.
    
    @par Side Effects
    None 
    
    @retval returns 64-bit tick count
    
    @sa DalTimetick_GetTimetick64
*/
uint64 timetick_get_sclk64( void );


/* ============================================================================
**  Function : timetick_cvt_to_sclk64
** ============================================================================
*/
/*!
    @brief
    Converts the given time to 64 bit ticks

    @details

    @param[in] time    Time interval to convert to ticks
    @param[in] unit     Units of time interval
                    
    @par Dependencies
    None  
      
    @par Side Effects
    None
    
    @retval returns Time in the unit requested
    
    @sa DalTimetick_CvtToTimetick64
*/

uint64 timetick_cvt_to_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);


/* ============================================================================
**  Function : timetick_cvt_from_sclk64
** ============================================================================
*/
/*!
    @brief
    Converts 64 bit ticks to the given unit

    @details
 
    @param[in] time    Time interval to convert from ticks
    @param[in] unit     Units of time interval

    @par Dependencies
    None    

    @par Side Effects
    None
    
    @retval returns Time in the unit requested
    
    @sa DalTimetick_CvtFromTimetick64
*/
uint64 timetick_cvt_from_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
);


/** 
  @}
*/ /* end_group deprecated_functions */

#endif /* TIMETICK_H */

