/*=============================================================================
          L O G   F U N C T I O N A L I T Y   F O R    D B I   

GENERAL DESCRIPTION     
  Logging driver for the debug image
  
  

	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdebug/sysdbg_log.h#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
04/06/14    AJCheriyan  Added reg dump enum 
07/09/14    AJCheriyan  Reduced size of log struct
12/12/12	AJCheriyan	Added support for running log (modified HisBuffer)
06/29/12	AJCheriyan	Moved log size definition to SConscript
02/19/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#ifndef _SYSDBG_LOG_H
#define _SYSDBG_LOG_H

#include "comdef.h"


/* Main API or Macro to be used for logging */
#define SYSDBG_LOG_EVENT(id, arg1, arg2, arg3) sysdbg_log_event_id4(id, arg1, arg2, arg3 )


/* Size of the logging structure is target specifc and can be trimmed based 
 * on memory footprint constraints. The value is set in the target sconscript */

/* Enumeration containing unique ids for every "event" to be logged */
typedef enum
{
	
	INVALID_EVENT = 0x0,
	// DBI Boot events
	BOOT_STARTED = 	0x1,
	BOOT_ENDED = 0x2,

	// One of the most important ones - fatal error
    FATAL_ERROR = 0x3,

	// DBI Pass information 
	PASS_ENTER = 0x4,
	PASS_EXIT = 0x5,

	// Events for entering and exiting a DBI function in a pass
	ENTER_FCN =	0x6,
	EXIT_FCN = 0x7,

	// Sleep event
	ENTER_SLEEP = 0x8,
	EXIT_SLEEP = 0x9,

	// Generic event
	GENERIC_EVENT = 0xA,

    // Register dump
    REG_DUMP = 0xB,
    // Extend enum
    EVENT_END = 0x7FFFFFFF,

}sysdbg_event_type;

/* Structure for logging info */
typedef struct
{
 sysdbg_event_type id;
 uint64 arg1;
 uint64 arg2;
 uint64 arg3;
}sysdbg_log_entry_type;

// Main Buffer that contains all the logs
typedef struct
{
 uint32 index;
 sysdbg_log_entry_type buffer[];
}SysdbgBufferType;

/*===========================================================================
**  Function :  SDI_LOG_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* 	This function initializes the logging buffer for the DBI
* 
* @param[in] 
*   None
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
*
*/
void SysdbgLogInit(void);


/*===========================================================================
**  Function :  SYSDBG_LOG_EVENT_IDn
** ==========================================================================
*/
/*!
* 
* @brief
* 	This function logs an entry into the buffer with 'n' parameters. 'n' should
* 	match the size of the buffer entry. This API should NOT be used externally. 
* 	Use SYSDBG_LOG_EVENT instead.
* 
* @param[in] 
*   3 unsigned long integers with the event id to be loged
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
*
*/
void sysdbg_log_event_id4(sysdbg_event_type event, uint64 arg1, uint64 arg2, uint64 arg3);

/*===========================================================================
**  Function :  DBI_LOG_DEINIT
** ==========================================================================
*/
/*!
* 
* @brief
* 	This function de-initializes the logging buffer for the DBI. It saves any useful pieces of
* 	information to another memory. This is done because of HW/SW limitation that prevents
* 	retention of DBI memory when logs are collected.
* 
* @param[in] 
*   None
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
*
*/
void SysdbgLogDeInit(void);






#endif /* _SYSDBG_LOG_H */
