/*=============================================================================
                        System Debug Utils Module 

GENERAL DESCRIPTION     
		  This module defines generally useful macros and utils.
  
	Copyright 2012- 2014 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

09/14/14    AJCheriyan  Refactored for HoneyBadger
===========================================================================*/

#ifndef _SYSDBG_UTIL_H
#define _SYSDBG_UTIL_H


#include "com_dtypes.h"
#include "comdef.h"
#include "sysdbg_log.h"



/* Global flag / lock type */
typedef struct
{
    volatile uint64 value;
}global_lock_type;

/* Setter and getter methods for the lock */
void set_global_lock(uint64 *lock, uint64);
uint64 get_global_lock(uint64 *lock);


/* Useful macros */
#ifdef ARRAY_SIZE
	#undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define BITSET(val, bitpos) ((uint32)val | (1 << bitpos))
#define BITCLR(val, bitpos) ((uint32)val & (~(1 << bitpos)))
#define BITGET(val, bitpos) (((uint32)val & (1 << bitpos)) >> bitpos)

#ifdef TRUE
	#define SUCCESS		TRUE
#else
	#define SUCCESS		1
#endif
#ifdef FALSE
	#define FAIL		FALSE
#else
	#define FAIL		0
#endif


#define CONVERT_TO_CYCLES(time_in_ms, clock_in_hz) ((uint32)((time_in_ms * clock_in_hz)/(1000)))

#define CALL_IF_VALID(execute, fcnptr, subsys)	    if (execute) \
											            if (fcnptr) \
												            { SYSDBG_LOG_EVENT(ENTER_FCN, (uint64)fcnptr, get_current_cpu_seq(), 0); fcnptr(subsys);  SYSDBG_LOG_EVENT(EXIT_FCN, (uint64)fcnptr, get_current_cpu_seq(), 0);} \
											            else { break; }	\
										            else				\
											            { continue; }	\

#define CALL_IF_VALID_AND_CHECK(rval, execute, fcnptr, subsys)	    if (execute) \
															            if (fcnptr) \
																            { SYSDBG_LOG_EVENT(ENTER_FCN, (uint64)fcnptr, get_current_cpu_seq(), 0); rval = fcnptr(subsys); SYSDBG_LOG_EVENT(EXIT_FCN, (uint64)fcnptr, get_current_cpu_seq(), 0); } \
															            else					    \
																            { break; }				\
														            else							\
															            { continue; }




#endif /* _SYSDBG_UTIL_H */
