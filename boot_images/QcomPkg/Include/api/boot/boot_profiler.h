#ifndef BOOT_PROFILER_H
#define BOOT_PROFILER_H

/*===========================================================================

                                boot profiler 
                                Header File

GENERAL DESCRIPTION
  This header file contains macro declarations and definitions for 
  Boot Profiler functionality.
  Time unit in boot profiler is microsecond(10^-6 second).

Copyright  2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   --------     ------------------------------------------------------
08/20/15   elt          Switched from macros to inline functions for 
                        boot_profiler_* functions to support return values.
                        Added profiler functions that use time markers
02/27/15   plc          Initial revision
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_logger.h"
#include <stdio.h>
#include <stdarg.h>
/*===========================================================================

                        LOCAL GLOBAL DEFINITIONS

===========================================================================*/

typedef enum boot_profiler_log_level
{
  PROFILER_TIMING_MSG = 0, /* Only profiling messages should be output per driver  */
  PROFILER_DEBUG_HIGH = 1, /* High level debug messages and profiling per driver   */
  PROFILER_DEBUG_LOW  = 2, /* Low and high level messages and profiling per driver */
  PROFILER_MAX_LEVEL  = 6  /* Upper limit of possible number of profiling levels   */
}boot_profiler_log_level;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#ifdef BOOT_PROFILER_FEATURE
/*===========================================================================

**  Function : boot_prof_message

** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to boot_log_message()
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_prof_message(char *xxstring, va_list args);
  
/*===========================================================================

**  Function : boot_prof_start_timer

** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to boot_log_message()
*   and starts the timer via boot_log_start_timer()
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_prof_start_timer(char *xxstring, va_list args);

/*===========================================================================
 **  Function : boot_prof_stop_timer
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to 
*   boot_log_stop_timer()
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
void boot_prof_stop_timer(char *xxstring, va_list args);

/*===========================================================================

**  Function : boot_prof_start_time_marker

** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to boot_log_message()
*   and starts the timer via boot_log_start_time_marker(). Returns time
*   marker handle created by boot_log_start_time_marker.
*
* @par Dependencies
*   None
*
* @retval
*   Handle to created time marker. Returns LOG_TIME_MARKER_OVERFLOW if space 
*   is not available for creating a new time marker.
* 
* @par Side Effects
*   None
*  
*/
uint32 boot_prof_start_time_marker(char *xxstring, va_list args);

/*===========================================================================
 **  Function : boot_prof_stop_time_marker
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to 
*   boot_log_stop_time_marker() using specified handle
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
void boot_prof_stop_time_marker(uint32 handle, char *xxstring, va_list args);


/*===========================================================================
 **  Function : boot_profiler_message
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Check's profiler verbosity and runs boot_prof_message if verbosity check
*   passes
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
static inline void boot_profiler_message(boot_profiler_log_level xxlevel, 
                                         char *xxstring, 
                                         ...)
{
  va_list args;
  va_start(args, xxstring);
  boot_profiler_log_level current_profiler_lvl = BOOT_PROFILER_LEVEL; 
  if ((xxlevel) <= current_profiler_lvl)                              
  {                                                                   
    boot_prof_message( xxstring, args);   
  }
  va_end(args);
}

/*===========================================================================
 **  Function : boot_profiler_start_timer
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Check's profiler verbosity and runs boot_prof_start_timer if verbosity 
*   check passes
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
static inline void boot_profiler_start_timer(boot_profiler_log_level xxlevel, 
                                             char *xxstring, 
                                             ...)
{
  va_list args;
  va_start(args, xxstring);
  boot_profiler_log_level current_profiler_lvl = BOOT_PROFILER_LEVEL; 
  if ((xxlevel) <= current_profiler_lvl)                              
  {                                                                   
    boot_prof_start_timer(xxstring, args); 
  }
  va_end(args);                              
}

/*===========================================================================
 **  Function : boot_profiler_stop_timer
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Check's profiler verbosity and runs boot_prof_stop_timer if verbosity 
*   check passes
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
static inline void boot_profiler_stop_timer(boot_profiler_log_level xxlevel, 
                                            char *xxstring, 
                                            ...)
{
  va_list args;
  va_start(args, xxstring);
  boot_profiler_log_level current_profiler_lvl = BOOT_PROFILER_LEVEL;
  if ((xxlevel) <= current_profiler_lvl)                              
  {                                                                   
    boot_prof_stop_timer( xxstring, args);
  }
  va_end(args);                             
}

/*===========================================================================
 **  Function : boot_profiler_start_time_marker
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Check's profiler verbosity and runs boot_prof_start_time_marker if verbosity
*   check passes
*
* @par Dependencies
*   None 
*
* @retval
*   Handle to created timer. Returns LOG_TIME_MARKER_OVERFLOW if space is not
*   available for creating a new time marker.
* 
* @par Side Effects
*   None
*  
*/  
static inline uint32 boot_profiler_start_time_marker
(
  boot_profiler_log_level xxlevel, 
  char *xxstring, 
  ...
)
{
  va_list args;
  va_start(args, xxstring);
  uint32 handle = LOG_TIME_MARKER_OVERFLOW;
  boot_profiler_log_level current_profiler_lvl = BOOT_PROFILER_LEVEL; 
  if ((xxlevel) <= current_profiler_lvl)                              
  {                                                                   
    handle = boot_prof_start_time_marker(xxstring, args);
  }
  va_end(args);
  return handle;
}

/*===========================================================================
 **  Function : boot_profiler_stop_time_marker
 ** ==========================================================================
*/
/*!
* 
* @brief  
*   Check's profiler verbosity and runs boot_prof_stop_time_marker if verbosity
*   check passes
*
* @par Dependencies
*   None 
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/  
static inline void boot_profiler_stop_time_marker
(
  uint32 handle,
  boot_profiler_log_level xxlevel, 
  char *xxstring, 
  ...
)
{
  va_list args;
  va_start(args, xxstring);
  boot_profiler_log_level current_profiler_lvl = BOOT_PROFILER_LEVEL; 
  if ((xxlevel) <= current_profiler_lvl)                              
  {                                                                   
    boot_prof_stop_time_marker(handle, xxstring, args);
  }
  va_end(args);
}


#else
static inline void boot_profiler_message(boot_profiler_log_level xxlevel, 
                                         char *xxstring, 
                                         ...)
{
}

static inline void boot_profiler_start_timer(boot_profiler_log_level xxlevel, 
                                             char *xxstring, 
                                             ...)
{
}

static inline void boot_profiler_stop_timer(boot_profiler_log_level xxlevel, 
                                            char *xxstring, 
                                            ...)
{
}

static inline uint32 boot_profiler_start_time_marker
(
  boot_profiler_log_level xxlevel, 
  char *xxstring, 
  ...
)
{
  return LOG_TIME_MARKER_OVERFLOW;
}

static inline void boot_profiler_stop_time_marker
(
  boot_profiler_log_level xxlevel, 
  uint32 handle,
  char *xxstring, 
  ...
)
{
}
#endif

#ifdef SBL_PROFILER_FEATURE
  #define boot_profiler_sbl_message(xxlevel, xxstring, ...) boot_profiler_message(xxlevel, xxstring, ##__VA_ARGS__)
  #define boot_profiler_sbl_start_timer(xxlevel, xxstring, ...) boot_profiler_start_timer(xxlevel, xxstring, ##__VA_ARGS__)		
  #define boot_profiler_sbl_stop_timer(xxlevel, xxstring, ...) boot_profiler_stop_timer(xxlevel, xxstring, ##__VA_ARGS__)
#else
  #define boot_profiler_sbl_message(xxlevel, xxstring, ...)
  #define boot_profiler_sbl_start_timer(xxlevel, xxstring, ...)
  #define boot_profiler_sbl_stop_timer(xxlevel, xxstring, ...)
#endif
  
#endif  /* BOOT_PROFILER_H */
