#ifndef _COREVERIFY_H_
#define _COREVERIFY_H_
/*==============================================================================
  FILE:         CoreVerify.h
  
  OVERVIEW:     Result checking macros. Unlike assert macros, CORE_VERIFY macros must not be
                compiled away at runtime. They are used for actual runtime error checking.

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreVerify.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CUST_H
 #include CUST_H
#endif

#ifndef TARGET_UEFI
#include <stdio.h>
#endif

#if defined(WINSIM) || defined(UTILS_USE_MPD_APP)

#include <stdlib.h>

#define ERR_FATAL( str, a, b, c ) abort()

#elif defined(TARGET_UEFI)

#define ERR_FATAL( str, a, b, c )

#else

#include "err.h"
#ifndef ERR_FATAL
/* TODO: get real error fatal */
#define ERR_FATAL( str, a, b, c ) {while(1) {*(unsigned int *)0 = 0xdeadbeef;} }
#endif

#endif

/* Common Helper Macros for building error messages */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define AT __FILENAME__ ":" TOSTRING(__LINE__)

/* ************************************************************************
 * CORE_VERIFY macros
 * *********************************************************************** */

/*
 * CORE_VERIFY( expr )
 *
 * If the logical expression is not true, raise an ERR_FATAL.
 *
 * This is the basic macro that the other CORE_VERIFY macros extend.
 * The verbosity of the macro may be changed by using one of the below FEATUREs
 */

#if defined(FEATURE_CORE_VERIFY_VERBOSE)
  #define VERIFY_MESSAGE(x) "Assertion failure: ( " x " ) " AT
#elif defined(FEATURE_CORE_VERIFY_AT)
  #define VERIFY_MESSAGE(x) AT
#else
  #define VERIFY_MESSAGE(x) 0
#endif

#ifndef CORE_VERIFY
#define CORE_VERIFY(x)                                       \
  do {                                                       \
    if (0 == (x)) {ERR_FATAL(VERIFY_MESSAGE(#x), 0, 0, 0);}  \
  } while(0)
#endif

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
#define CORE_UNIMPLEMENTED() CORE_VERIFY( 0 )

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
#define CORE_LOG_VERIFY( x, log )                                 \
  do {                                                            \
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

#endif /* _COREVERIFY_H_ */
