#ifndef OS_TYPE_H
#define OS_TYPE_H

/*===========================================================================

                      G-Link OS Specific types/functions


 Copyright (c) 2014 by QUALCOMM Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/14   bm      Initial version for XBL
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "string.h"
#include "ULogFront.h"
#include "DALSys.h"

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

#define OS_LOG_INIT( ) glink_os_log_init()

/** Tracing macro */
#ifdef FEATURE_PROFILING
#define OS_EVENT_TRACE( args... ) 
#else
#define OS_EVENT_TRACE( args... )
#endif

/** Generic debug message */
#define OS_LOG_MSG( args... ) 

/** Generic error message print */
#define OS_LOG_ERROR( args... ) 
#define OS_LOG_ERROR_FATAL( args... ) 

#define GLINK_OS_SEC_TO_TICK 1000

#define strcmp AsciiStrCmp
#define strlen AsciiStrLen

#ifndef ERR_FATAL
#define ERR_FATAL(str, a, b, c) ASSERT(0)
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#define GLINK_OS_UNREFERENCED_PARAM( param ) ( void )param

#define GLINK_OS_SEC_TO_USEC (1000000ULL)

void glink_os_dead_loop(void);

#define ASSERT(c) \
  if (!(c)) \
  {\
    DebugAssert(__FILE__, __LINE__, "Glink assert failed"); \
    glink_os_dead_loop(); \
  }
  
/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
typedef int os_cs_type;
typedef int os_mutex_type;
typedef unsigned int os_timetick_type;

typedef void ( *os_timer_cb_fn )( void *cb_data );

typedef int os_timer;

typedef int os_event_type;

typedef void ( *os_isr_cb_fn )( void *cb_data );

typedef struct {
  uint32 *intr_addr;
  uint32 intr_mask;
} os_ipc_intr_type;

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL VARIABLE DEFINITIONS
===========================================================================*/

#endif /* OS_TYPE_H */
