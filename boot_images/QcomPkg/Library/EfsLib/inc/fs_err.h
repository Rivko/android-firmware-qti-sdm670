/***********************************************************************
 * fs_err.h
 *
 * Error macros for EFS2.
 * Copyright (C) 2002--2006, 2008--2010, 2012--2014 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/inc/fs_err.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2014-06-12    wek     Bringup of hotplug for XBL.
2014-05-27    dks     Use MSG_SSID_DFLT in FS_MSG_* defines.
2014-04-26     vm     Define more FS_MSG macros to save RAM.
2013-08-26     nr     Add support for FS_ASSERT.
2012-05-08    dks     Correct FS_MSG_ERROR macro name in boot.
2012-03-28     nr     Add colors and clean debug MSGs and ERRORs.
2012-03-20    wek     Cleanup customer header file inclusion.
2012-03-08    wek     Rename macros and remove header files to compile for boot
2012-01-17    wek     Remove unused header files to compile for boot.
2010-03-05     vr     Add support for QEFS2 in BOOT
2009-08-06     ds     Added OSBL macro needed for 8k
2009-07-07    ebb     Changed #elif FEATURE_EFS_BOOT_ENV to #elif defined(..)
2009-07-06    ebb     Added alternate FS_ERR_FATAL definition for boot.
2008-10-17    yog     Moved err.h to be included for all cases.
2006-09-20    dlb     Lint cleanup.
2005-10-12    dlb     Fix ISO C warnings.
2005-01-27    dlb     Allow standalone builds.
2004-04-15    dlb     Update copyright line.
2003-04-22     cr     Add FS_TRACE macro.
2003-01-09     gr     Created.

===========================================================================*/

#ifndef _FS_ERR_H
#define _FS_ERR_H

#include "fs_config_i.h"
#include "comdef.h"

#include <setjmp.h>

#ifdef FEATURE_EFS_FATAL_ERROR_HANDLER
/* Alloc fatal errors to be "handled".  If this feature is defined, the
 * function handle_efs_err_fatal will be called.  Normally, this function
 * should either do nothing, or call efs_reset_nostop() and then return.
 */
extern void handle_fs_err_fatal (void);
#define _FS_ERR_FATAL_HANDLER   handle_fs_err_fatal ()

#else  /* not FEATURE_EFS_FATAL_ERROR_HANDLER */
#define _FS_ERR_FATAL_HANDLER   do { } while (0)
#endif  /* not FEATURE_EFS_FATAL_ERROR_HANDLER */

#if defined (FS_STANDALONE)

#include <stdio.h>
#include "msg.h"

#ifndef NORETURN
  #ifdef __GNUC__
    #define NORETURN   __attribute__ ((noreturn))
  #else
    #define NORETURN
  #endif
#endif

extern void err_fatal_raise (void) NORETURN;


#define FS_ERR_FATAL(fmt,a,b,c) \
  do { \
    MSG_BASE ("FATAL", fmt, a, b, c, FS_MSG_RED); \
    _FS_ERR_FATAL_HANDLER; \
    err_fatal_raise (); \
  } while (0)

#define FS_ERR(fmt,a,b,c) \
  do { \
    MSG_BASE ("ERR", fmt, a, b, c, FS_MSG_RED); \
  } while (0)

/* If we are compiling for boot redirect to oemsbl_error_handler */
#elif defined (FEATURE_EFS_BOOT_ENV)
  #include "boot_api.h"
  #define FS_ERR_FATAL(fmt,a,b,c) BOOT_ERR_FATAL()
  #define FS_ERR(fmt,a,b,c) while (0)

#else /* !FEATURE_EFS_BOOT_ENV && !FS_STANDALONE */
#include "err.h"

  #define FS_ERR_FATAL(fmt,a,b,c) \
  do { \
    err_disable_fs_log (); \
    _FS_ERR_FATAL_HANDLER; \
    ERR_FATAL (fmt, a, b, c); \
  } while (0)

  #define FS_ERR(fmt,a,b,c) \
  do { \
    err_disable_fs_log (); \
    ERR (fmt, a, b, c); \
    err_enable_fs_log (); \
  } while (0)

#endif

#ifdef FEATURE_EFS_BOOT_ENV /* If not defined use regular MSG macros*/

  #define FS_MSG_ERROR_0(a)       while (0)
  #define FS_MSG_ERROR_1(a,b)     do{(void) b;}while (0)
  #define FS_MSG_ERROR_2(a,b,c)   do{(void) b;(void) c;}while (0)
  #define FS_MSG_ERROR_3(a,b,c,d) do{(void) b;(void) c;(void) d;}while (0)
  #define FS_MSG_HIGH_0(a)        while (0)
  #define FS_MSG_HIGH_1(a,b)      do{(void) b;}while (0)
  #define FS_MSG_HIGH_2(a,b,c)    do{(void) b;(void) c;}while (0)
  #define FS_MSG_HIGH_3(a,b,c,d)  do{(void) b;(void) c;(void) d;}while (0)
  #define FS_MSG_MED_0(a)         while (0)
  #define FS_MSG_MED_1(a,b)       do{(void) b;}while (0)
  #define FS_MSG_MED_2(a,b,c)     do{(void) b;(void) c;}while (0)
  #define FS_MSG_MED_3(a,b,c,d)   do{(void) b;(void) c;(void) d;}while (0)
  #define FS_MSG_LOW_0(a)         while (0)
  #define FS_MSG_LOW_1(a,b)       do{(void) b;}while (0)
  #define FS_MSG_LOW_2(a,b,c)     do{(void) b;(void) c;}while (0)
  #define FS_MSG_LOW_3(a,b,c,d)   do{(void) b;(void) c;(void) d;}while (0)

#else /* FEATURE_EFS_BOOT_ENV is defined, don't use these macros. */
#include "msg.h"

  #define FS_MSG_ERROR_0(a) \
          MSG(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a)
  #define FS_MSG_ERROR_1(a,b) \
          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b)
  #define FS_MSG_ERROR_2(a,b,c) \
          MSG_2(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b,c)
  #define FS_MSG_ERROR_3(a,b,c,d) \
          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b,c,d)

  #define FS_MSG_HIGH_0(a) \
          MSG(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a)
  #define FS_MSG_HIGH_1(a,b) \
          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b)
  #define FS_MSG_HIGH_2(a,b,c) \
          MSG_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b,c)
  #define FS_MSG_HIGH_3(a,b,c,d) \
          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b,c,d)

  #define FS_MSG_MED_0(a) \
          MSG(MSG_SSID_DFLT,MSG_LEGACY_MED,a)
  #define FS_MSG_MED_1(a,b) \
          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_MED,a,b)
  #define FS_MSG_MED_2(a,b,c) \
          MSG_2(MSG_SSID_DFLT,MSG_LEGACY_MED,a,b,c)
  #define FS_MSG_MED_3(a,b,c,d) \
          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_MED,a,b,c,d)

  #define FS_MSG_LOW_0(a) \
          MSG(MSG_SSID_DFLT,MSG_LEGACY_LOW,a)
  #define FS_MSG_LOW_1(a,b) \
          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_LOW,a,b)
  #define FS_MSG_LOW_2(a,b,c) \
          MSG_2(MSG_SSID_DFLT,MSG_LEGACY_LOW,a,b,c)
  #define FS_MSG_LOW_3(a,b,c,d) \
          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_LOW,a,b,c,d)

#endif

/* Simplistic exception handling method for EFS calls.  The can be caught
 * to rollback transactions.  If there is not catch, FS_ERR_FATAL is
 * raised. */

/* jmp_buf is an array type of unspecified field types, wrap it in a
 * structure so normal pointers will work.  This can also be extended with
 * other fields, if necessary. */
struct efs_catch_block {
  jmp_buf       jmp;
};

extern struct efs_catch_block *_fs_catch;
extern struct efs_catch_block _fs_static_catch;

/* Catch block for exceptions.  Used like this:
 *
 *   int status;
 *   FS_SIMPLE_TRY (status);
 *   if (status == 0) {
 *     Normal part of code.
 *   } else {
 *     Error handling with raise called with 'status' for arg.
 *   }
 *   FS_SIMPLE_TRY_CLEANUP ();
 */
#define FS_SIMPLE_TRY(_status) \
  do { \
    if (_fs_catch != NULL) \
      FS_ERR_FATAL ("Nested FS_SIMPLE_TRY not allowed", 0, 0, 0); \
    _fs_catch = &_fs_static_catch; \
    (_status) = setjmp (_fs_static_catch.jmp); \
  } while (0)

#define FS_SIMPLE_TRY_CLEANUP() \
  do { \
    _fs_catch = NULL; \
  } while (0)

/* Raise an FS exception.  The first argument is the status to give back to
 * the TRY block.  It must not be zero.  The remaining arguments are as per
 * FS_ERR_FATAL, and will be given to FS_ERR_FATAL is there is no handler.
 */
#define FS_RAISE(status, fmt, a, b, c) \
  do { \
    if (_fs_catch == NULL) \
      FS_ERR_FATAL (fmt, a, b, c); \
    longjmp (_fs_catch->jmp, (status)); \
  } while (0)

/* Return true if an exception will be caught. */
#define FS_HANDLER_PRESENT() (_fs_catch != NULL)

#define FS_ASSERT(cond) \
  do { \
    if (!(cond)) \
    { \
      FS_ERR_FATAL ("FS Assertion failure ['%s'] failed!", #cond, 0, 0); \
    } \
  } while (0)

#endif /* End of _FS_ERR_H */

