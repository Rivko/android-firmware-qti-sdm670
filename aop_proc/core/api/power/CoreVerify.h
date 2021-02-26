#ifndef COREVERIFY_H
#define COREVERIFY_H
/*==============================================================================

FILE:      CoreVerify.h

DESCRIPTION: result checking macros

   Unlike assert macros, CORE_VERIFY macros do not get compiled away
   at runtime.  They can be used for actual runtime error checking

   This file can also be used on off-target builds if built with the
   WINSIM macro defined


INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2009-2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

// Necessary for DAL macros
// 
// #include "DALStdErr.h"

#ifdef WINSIM
// Windows Based Simulation configuration.
//

#include <stdlib.h>

// Map ERR_FATAL to abort() on pc simulation builds 
//
#define ERR_FATAL( str, a, b, c ) abort()

#else
// On Target configuration.
//

#include "err.h"

#ifdef USES_BLAST

#include "diagstub.h"
#endif

#endif


/* Common Helper Macros for building error messages */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#ifndef MIN
#define MIN( a, b ) ((a)<(b)) ? (a) : (b)
#endif

/* ************************************************************************
 * CORE_VERIFY macros
 * *********************************************************************** */

/*
 * CORE_VERIFY( expr )
 *
 * If the logical expression is not true, raise an ERR_FATAL.
 *
 * This is the basic macro that the other CORE_VERIFY macros extend
 */

#ifdef FEATURE_CORE_LOW_MEM
  #define VERIFY_MESSAGE(x) 0
#else
  #define VERIFY_MESSAGE(x) "Assertion failure: ( " x ") " AT
#endif

#define CORE_VERIFY(x)                                       \
  do {                                                       \
    if (0 == (x)) {ERR_FATAL(VERIFY_MESSAGE(#x), 0, 0, 0);}  \
  } while(0)

/*
 * CORE_STATIC_VERIFY( expr )
 *
 * If the logical expression is not true, fail at compile-time
 *
 * This is the basic macro that the other CORE_VERIFY macros extend
 */
#define CORE_STATIC_VERIFY(expr) extern char __CORE_STATIC_VERIFY__[(expr)?1:-1]

/*
 * CORE_VERIFY_PTR( ptr )
 *
 * Verify the ptr is not NULL
 */
#define CORE_VERIFY_PTR(ptr) CORE_VERIFY( NULL != (ptr))

/*
 * CORE_DAL_VERIFY( dal_fcn )
 *
 * Verify the dal fcn returns DAL_SUCCESS
 */
#define CORE_DAL_VERIFY(dal_fcn) CORE_VERIFY( DAL_SUCCESS == (dal_fcn) )

/*
 * CORE_UNIMPLEMENTED()
 *
 * Unconditionally raise ERR_FATAL. Intende for use on unimplemnted
 * code paths to stop someone from accidentally executing an
 * unimplemnted code path, and also be easy to the developer to locate
 * as-yet unimplemented features
 */
#define CORE_UNIMPLEMENTED() CORE_VERIFY( 0 );

/* ************************************************************************
 * CORE_LOG_VERIFY macros
 *
 * Same conditions as normal CORE_VERIFY macros, but issue a log
 * message if the clogical condition fails before invoking ERR_FATAL
 *
 * Technically the log message can be any expression, not necessarily
 * a log message, but logging is the intended usage
 * *********************************************************************** */

/*
 * CORE_LOG_VERIFY( expr, log )
 *
 * If the logical expression is not true, execute log call and  * raise an ERR_FATAL.
 */
#define CORE_LOG_VERIFY( x, log )                                        \
  do {                                                                  \
    if (0 == (x)) {log ; ERR_FATAL(VERIFY_MESSAGE(#x), 0, 0, 0);} \
  } while(0)

/*
 * CORE_VERIFY_PTR( ptr )
 *
 * Verify the ptr is not NULL.  If not, execute log call and raise
 * ERR_FATAL
 */
#define CORE_LOG_VERIFY_PTR(ptr, log) CORE_LOG_VERIFY( NULL != (ptr), log)

/*
 * CORE_LOG_DAL_VERIFY( dal_fcn )
 *
 * Verify the dal fcn returns DAL_SUCCESS.  If not, execute log call
 * and raise ERR_FATAL
 */
#define CORE_LOG_DAL_VERIFY(dal_fcn, log) CORE_LOG_VERIFY( DAL_SUCCESS == (dal_fcn), log )


/*
 * CORE_LOG_UNIMPLEMENTED(log)
 *
 * Unconditionally excecute the log call and then raise ERR_FATAL. 
 */
#define CORE_LOG_UNIMPLEMENTED() CORE_VERIFY( 0, log );


#ifdef __cplusplus
}
#endif
#endif /* COREVERIFY_H */
