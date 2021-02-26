/*===========================================================================

                                boot profiler 
                                Source File

GENERAL DESCRIPTION
  This source file contains C definitions for 
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
08/20/15   elt          Added boot_prof_start_time_marker and 
                        boot_prof_stop_time_marker
06/22/15   elt          Fixed to use outer level BOOT_PROFILER_FEATURE define 
                        to control compilation 
02/27/15   plc          Initial revision
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_profiler.h"
#include "boot_logger.h"
#include <stdio.h>
#include <stdarg.h>
/*===========================================================================

                        LOCAL GLOBAL DEFINITIONS

===========================================================================*/


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
void boot_prof_message(char *xxstring, va_list args)
{
  char message[BOOT_LOG_TEMP_BUFFER_SIZE];
  
  vsnprintf(message, sizeof(message), xxstring, args);
  boot_log_message(message);
}

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
void boot_prof_start_timer(char *xxstring, va_list args)
{
  char message[BOOT_LOG_TEMP_BUFFER_SIZE];

  vsnprintf(message, sizeof(message), xxstring, args);
  boot_log_message(message);
  boot_log_start_timer();
}

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
void boot_prof_stop_timer(char *xxstring, va_list args)
{
  char message[BOOT_LOG_TEMP_BUFFER_SIZE];

  vsnprintf(message, sizeof(message), xxstring, args);
  boot_log_stop_timer(message);
}

/*===========================================================================

**  Function : boot_prof_start_time_marker

** ==========================================================================
*/
/*!
* 
* @brief  
*   Format's string and arguments passed in, and passes to boot_log_message()
*   and starts the time marker via boot_log_start_time_marker(). Returns 
*   time marker handle created by boot_log_start_time_marker
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
uint32 boot_prof_start_time_marker(char *xxstring, va_list args)
{
  char message[BOOT_LOG_TEMP_BUFFER_SIZE];

  vsnprintf(message, sizeof(message), xxstring, args);
  boot_log_message(message);
  return boot_log_start_time_marker();
}

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
void boot_prof_stop_time_marker(uint32 handle, char *xxstring, va_list args)
{
  char message[BOOT_LOG_TEMP_BUFFER_SIZE];

  vsnprintf(message, sizeof(message), xxstring, args);
  boot_log_stop_time_marker(handle, message);
}
#endif /* BOOT_PROFILER_FEATURE */
