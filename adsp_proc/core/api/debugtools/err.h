#ifndef ERR_H
#define ERR_H

/*===========================================================================

                    Error Handling Common Header File

Description
  The error handling services (ERR) provide user error handling services
  through an assert-like ERR_FATAL macro.  Calling this macro indicates an
  unrecoverable error. Other services are provided as defined by the APIs
  in this header file.

Copyright (c) 1992-2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1/api/debugtools/err.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/16   ph      Enable Qshrink 4 on user PD.
04/11/16   ph      Added support for QShrink 4.0
09/10/14   din     Optimized ERR_FATAL macro to reduce code size
07/31/14   din     Added support for Error on User PD. 
04/18/14   din     Added changes for ADSP island mode.
05/23/08   tbg     Added FEATURE_EXTERNAL_ERR_FATAL
04/04/08   tbg     FEATURE_GENERIC_ERR_FATAL_MSG for additional configurations
12/18/07   taw     Use IMAGE_APPS_PROC instead of DIAG_APPS_PROC around
                   err_lcd_message.
01/08/07   tbg     Added support for new err_auto_action API.
12/19/06   as      Reverted FEATURE_ERR_SMALL_BUFFER change.
12/18/06   as      Added support to decrease buffer size under
                   FEATURE_ERR_SMALL_BUFFER.
11/11/05   tbg     Changed ERR_EXTENDED_STORE to share NV item for max files
                   with debug trace (if feature is defined)
06/29/05   as      Increased ERR_DATA_MAX_SIZE.
04/17/04   ash     Added (FEATURE_MEM_USAGE_AT_EXCEPTION)
                   for application memory usage during crash
08/03/04   tbg     Extended FEATURE_ERR_EXTENDED_STORE to support multiple
                   files on EFS, as well as multiple logs per session.
07/09/04   eav     Added feature to save ERR_FATAL to trace buffer as a message
04/27/04   eav     Moved ERR_DATA_MAGIC_NUMBER and ERR_DATA_RESET_MAGIC_NUMBER
                   here so it is accessible from msg.c.
04/06/04   tbg     Added FAETURE_ERR_EXTENDED_STORE
10/08/03   gr      Redefined ERR_FATAL to make one function call instead of
                   three to save code space.
07/24/03   as      Added ERR_FATAL_REPORT(   ) macro
11/14/02   jct     Added REX core dump to ERR_FATAL
09/13/02   et      added ERR_LOG macro to log err to NV w/o F3 msg
08/20/02   lad     F_DIAG_COUPLED_ARCHITECTURE support.
04/06/01   lad     Added T_THOR to feature list.
02/23/01   lad     Changes for portability and re-use.
02/02/00   lad     Changed macros to parse as single expressions.
06/30/99   jct     Added PC_EMULATOR support for ERR_FATAL, made inclusion
                   of target.h conditional, added decoupling of inclusion of
                   nv.h based on assertion of FEATURE_ERR_DECOUPLE_NVM.
                   This allows resuse of ERR_FATAL in environment where NV is
                   not supported.
06/02/99   lad     Fixed multi-line macros to evaluate as single expression.
04/28/99   ms/sk   Inserted call to clear SL register in ERR_FATAL.
11/05/98   jct     Merged in changes for ARM targets.  Make calls to err_
                   functions const correct.
01/22/97   rdh     Used Pascal directive on err_*_put_log to save ROM.
07/24/95   jah     Changed ERR/ERR_FATAL() macro to use msg_file if MSG_FILE
                   is defined.
06/18/93   jah     Removed MSG_FATAL from phone version of ERR_FATAL() macro,
                   as it takes code space and the phone resets before the DM
                   can get the results from MSG.
10/29/92   ip      Updated for release of full error logging.
08/03/92   ip      Removed '{' and '}' from ERR_...
07/06/92   ip      Fixed target include files, added '{' and '}' to ERR_...
05/13/92   ip      Removed parentheses around 'format' in MSG_... calls.
03/07/92   ip      First porting of file from brassboard to subscriber.

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"
#include <stdio.h>

/* DEPRECATION FLAG */
#ifndef ERR_DEPRECATED_ATTRIBUTE
  #define ERR_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#endif

/* NO RETURN FLAG */
#ifndef ERR_NORETURN_ATTRIBUTE
  #define ERR_NORETURN_ATTRIBUTE __attribute__((noreturn))
#endif

/* HEXMSGABI FLAGS*/
#ifdef __HEXMSGABI_2_SUPPORTED__
#define ERR_HEXMSGABI_ATTRIBUTE __attribute__((hexmsgabi))
#else
#define ERR_HEXMSGABI_ATTRIBUTE
#endif

#ifndef ERR_NORETURN_HEXMSGABI_ATTRIBUTE
#define ERR_NORETURN_HEXMSGABI_ATTRIBUTE __attribute__((noreturn, hexmsgabi))
#endif

#ifndef ERR_DLL_API_GLOBAL
#define ERR_DLL_API_GLOBAL              __attribute__((visibility("default")))
#endif


/*
#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 200000)) || (defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
#define ERR_DEPRECATED __attribute__ ((deprecated))
#else
#define ERR_DEPRECATED
#endif
 */

typedef struct
{
 const char *fmt;      /* Printf style format string */
 const char *fname;    /* Pointer to source file name */
 uint16 line;    /* Line number in source file */
} err_FatalParam_type;


#include "msg.h"


#define ERR_HAS_NV_LOG_PACKET /* flag to indicate that ERR code will register
                                 the NV log read and clear packets. Definitions
								 for these packets are given below. */

/* -------------------------------------------------------------------------
   Definitions and Declarations
   ------------------------------------------------------------------------- */

/* Type used for post-crash callbacks from error handler */
typedef void (*err_cb_ptr)(void);
typedef void (*err_cb_ptr_u32)(uint32);

/* Enum for error code while registering or de-registering error callbacks */
typedef enum{
  ERRCB_E_SUCCESS = 0,
  ERRCB_E_FAILURE = -1,
  ERRCB_E_INVALID_PARAM = -2,
  ERRCB_E_NOT_ALLOWED = -3,
  ERRCB_E_MAX_LIMIT_REACHED = -4,
  ERRCB_E_NOT_FOUND = -5
}err_cb_error_t;

/* Enum for callback type i.e. void or uint32 type */
typedef enum{
  ERRCB_TYPE_VOID = 0,
  ERRCB_TYPE_UINT32 = 1
}err_cb_type_t;

/**
 * This will allow client to choose at what level in error handling
 * their callback needs to invoke.
 *
 * @note only ERRCB_BUCKET_PRE_STM type callback will be invoked before
 *       moving into STM (single threaded mode), remaining all will be 
 *       invoked post STM and in sequence given in enum.
 *
 * @note use ERRCB_BUCKET_NORMAL if sequence not required.
 */
typedef enum{
  ERRCB_BUCKET_PRE_STM = 0,
  ERRCB_BUCKET_POST_STM = 1,
  ERRCB_BUCKET_NORMAL = 2,
  ERRCB_BUCKET_LATE = 3,
  ERRCB_BUCKET_LAST = 4,
  ERRCB_BUCKET_MAX = 5
}err_cb_bucket_t;

/** 
 * This is reserved, use only ERRCB_ORDER_NORMAL for now.
 */
typedef enum{
  ERRCB_ORDER_FIRST = 0, // only one client allowed (singleton)
  ERRCB_ORDER_EARLY = 1, // multiple client can register
  ERRCB_ORDER_NORMAL = 2, // multiple client can register (use this if no priority)
  ERRCB_ORDER_LATE = 3,   // multiple client can register
  ERRCB_ORDER_LAST = 4,    // only one client allowed (singleton)
  ERRCB_ORDER_MAX = 5
}err_cb_order_t;

/* Err callbacks information need to provide via this struct. */
typedef struct{
  err_cb_type_t type;
  union{
    err_cb_ptr cb;
    err_cb_ptr_u32 cb_u32;
  }err_cb;
  err_cb_bucket_t bucket;
  err_cb_order_t order;
}err_cb_info_t;

/* Status returned from err_get_log and err_clr_log. */

typedef enum
{
  ERR_DONE_S,           /* Request completed OK */
  ERR_NONE_S            /* No such log number */
}
err_stat_type;

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
extern const char err_generic_msg[];
extern const char err_generic_msg_dynamic[];
#endif


/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */




/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function checks if NV has been built.  If so then it loads up the
  error log cache from NV, it initializes error counts and local variables,
  and it sets error services to online.  If NV has not been built then
  error log is not loaded and error services remain offline and error
  logging does not take place.  Any access to NV is performed here directly,
  without relying on the NV task.  Thus error service can be initialized
  and used early in the life of the DMSS and right after boot.  Note
  that if NV has not been built, error services provide no error logging
  capabilities.  However NV is built once, after initial factory startup,
  and rarely thereafter.  Thus except for the first ever powerup of the
  unit after NV build or re-build, error logging will take place as usual.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/
  void err_init (void);

/*=========================================================================

FUNCTION err_crash_cb_register

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called after an ERR_FATAL
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg(err_cb_ptr cb);
boolean err_crash_cb_register(err_cb_ptr cb);

/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error record both in cache and in NV.
  An error record is cleared simply by clearing the error count field
  in it.

  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared.
  ERR_NONE_S    - Error record out of range or clear failed.

SIDE EFFECTS
  The specified error record is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/
  err_stat_type err_clr_log (byte log_num);

/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  other local functions to store the error.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.  This function must not be called directly; it may
  be called only from the ERR macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/
  void err_put_log (word line, const char *file_ptr);


/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void);


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void);


/*===========================================================================

FUNCTION ERR_FATAL_LOCK
DESCRIPTION
  Obtains err_fatal mutex

============================================================================*/
void err_fatal_lock( void );


/*===========================================================================

MACRO ERR

DESCRIPTION
  Log an error to error log and update cache and return to caller.
  Error is sent to the MSG utility to be routed to DM or PC screen,
  depending on conditional compile switches defined in MSG.  If host
  is a PC then NV logging does not take place and only MSG output
  is used.

DEPENDENCIES
  If MSG_FILE is defined, ERR uses it as the filename.

RETURN VALUE
  None.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR must only be called because of
  an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effect are OK.

===========================================================================*/

#define ERR_FILE_NAME   msg_file

/* Map to MSG_ERR_LOG for memory savings.
   Equivalent to calling:
     MSG_ERROR(format, (code1), (code2), (code3));
     err_put_log ((word) __LINE__, ERR_FILE_NAME);
 */
#define  ERR(format, code1, code2, code3)          \
  MSG_ERR_LOG (3, format, code1, code2, code3)


/*===========================================================================

MACRO ERR_LOG

DESCRIPTION
  Log an error to error log and update cache and return to caller.

DEPENDENCIES
  err_init must have been called

RETURN VALUE
  None.

SIDE EFFECTS
  updates NV memory with the file name and line# of the error

===========================================================================*/
#define ERR_LOG() \
  do {                                             \
    err_put_log ( __LINE__, ERR_FILE_NAME );       \
  } while(0)                                       \


/*===========================================================================

MACRO ERR_FATAL

DESCRIPTION
  Log an error to error log, update the cache (for debug purpose only)
  and RESET THE SYSTEM.  Control DOES NOT RETURN to caller.  Error is
  sent to the MSG utility to be routed to DM or PC screen, depending
  on conditional compile switches defined in MSG.  If host is a PC
  then NV logging does not take place.  Instead a MSG output is made
  and the program exits (simulating hardware reset of sorts).
  ERR_FATAL populates SFR with the message and params provided in the argument.
  If the message is implanted with %s format specifier, ERR_FATAL treats 
  it as a hexadecimal value.

DEPENDENCIES
  If MSG_FILE is defined, ERR_FATAL uses it as the filename.

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR_FATAL must only be called because
  of an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effects are OK.

===========================================================================*/

/*===========================================================================

MACRO ERR_FATAL_REPORT

DESCRIPTION
  DEPRECATED

===========================================================================*/

  /* Prototypes register core dump function */
  extern void jettison_core( void );


typedef struct __attribute__((packed))
{
  uint16     line;
  const char *fmt;      /*!< Printf style format string */
  const char *fname;    /*!< Pointer to source file name */
}
err_const_type;


#if defined (FEATURE_FATAL_SECTION)

/********************
 NOTE:
  Define FEATURE_FATAL_SECTION *globally* to enable this feature. Necesary
  scatter loader/linker script sections, etc. must be defined as needed and
  are *not* the responsibility of error services.  This FEATURE is required
  for error compression to work (external dependencies belong to
  modem performance team)

 *********************/


#pragma GCC diagnostic ignored "-Wstatic-in-inline"
static const char __THIS_FILENAME__[] __attribute__ ((aligned(1),section (".fatal.file.rodata." ))) = __FILENAME__;


// NOTE: fatal.fmt.rodata. and fatal.struct.rodata are zlib compressed post compilation
//       fatal.file.rodata. is NOT compressed to prevent possible conflicts with the optimizer
//       all of these *MUST* remain outside of q6zip compression sections or they maybe
//       paged out during err_fatal, which is unrecoverable
#define _XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
        static const char _xx_fmt [] __attribute__ ((section ("fatal.fmt.rodata." ))) = {xx_fmt}; \
        static const err_const_type xx_msg_const __attribute__ ((section ("fatal.struct.rodata." ))) = { \
              __LINE__, (_xx_fmt), __THIS_FILENAME__}

#else /* FEATURE_FATAL_SECTION */

#define _XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
        static const err_const_type xx_msg_const = { \
              __LINE__, (xx_fmt), __FILENAME__}

#endif /* FEATURE_FATAL_SECTION */

ERR_DLL_API_GLOBAL void err_Fatal_internal3 (const err_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3) ERR_NORETURN_ATTRIBUTE ERR_HEXMSGABI_ATTRIBUTE;
ERR_DLL_API_GLOBAL void err_Fatal_internal2 (const err_const_type* const_blk, uint32 code1, uint32 code2) ERR_NORETURN_ATTRIBUTE ERR_HEXMSGABI_ATTRIBUTE;
ERR_DLL_API_GLOBAL void err_Fatal_internal1 (const err_const_type* const_blk, uint32 code1) ERR_NORETURN_ATTRIBUTE ERR_HEXMSGABI_ATTRIBUTE;
ERR_DLL_API_GLOBAL void err_Fatal_internal0 (const err_const_type* const_blk) ERR_NORETURN_ATTRIBUTE ERR_HEXMSGABI_ATTRIBUTE;

#define ERR_TEST_CONSTZERO_CALL(test,funcConst,funcVar) (__builtin_constant_p(test) ? ( (((int)test) == 0) ? funcConst :  funcVar  ) :funcVar)

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
 #define ERR_FATAL(format, xx_arg1, xx_arg2, xx_arg3) \
  do { \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg );    \
          ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg3), \
             ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg2), \
                ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg1), \
                   err_Fatal_internal0(&xx_msg_const), \
                     err_Fatal_internal1(&xx_msg_const, (uint32)(xx_arg1))), \
                        err_Fatal_internal2(&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2))), \
                           err_Fatal_internal3(&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3))) ;\
  } while (0)
#else
  #define ERR_FATAL(format, xx_arg1, xx_arg2, xx_arg3) \
  do { \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, format );             \
          ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg3), \
             ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg2), \
                ERR_TEST_CONSTZERO_CALL((uint32)(xx_arg1), \
                   err_Fatal_internal0(&xx_msg_const), \
                     err_Fatal_internal1(&xx_msg_const, (uint32)(xx_arg1))), \
                        err_Fatal_internal2(&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2))), \
                           err_Fatal_internal3(&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3))) ;\
  } while (0)
#endif /* FEATURE_GENERIC_ERR_FATAL_MSG */

  #define ERR_FATAL_NO_SAVE(format, code1, code2, code3)                     \
          ERR_FATAL_REPORT(format, code1, code2, code3)


/*===========================================================================

MACRO ERR_FATAL_AUX_MSG

DESCRIPTION
  Accepts a backward compatible, zero-parameter ERR_FATAL string, as well
  as an additional string to fill a dynamic message buffer
  which is stored in the coredump.err.aux_msg.

  The macro is not available for Island mode.

NOTE
  The SFR created by this message is parsed by some HLOS's/customers.
  As such format maybe important.  Please consult with owners before
  using.

DEPENDENCIES
  None

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  Terminates processing, dumps core

===========================================================================*/
ERR_DLL_API_GLOBAL void err_Fatal_aux_msg 
(
  const err_const_type* const_blk, 
  const char * dynamic_msg,
  uint16 size_dynamic_msg
);

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL_AUX_MSG(legacy_msg, dynamic_msg, size_dynamic_msg)             \
    do                                                                             \
    {                                                                              \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg_dynamic ); \
        err_Fatal_aux_msg (&xx_msg_const, dynamic_msg, size_dynamic_msg);          \
    /*lint -save -e{717} */                                                        \
    } while (0)                                                                    \
    /* lint -restore */

#else /* !FEATURE_GENERIC_ERR_FATAL_MSG */
  #define ERR_FATAL_AUX_MSG(legacy_msg, dynamic_msg, size_dynamic_msg)             \
    do                                                                             \
    {                                                                              \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, legacy_msg );              \
        err_Fatal_aux_msg (&xx_msg_const, dynamic_msg, size_dynamic_msg );         \
    /*lint -save -e{717} */                                                        \
    } while (0)                                                                    \
    /* lint -restore */
#endif /* FEATURE_GENERIC_ERR_FATAL_MSG */

/*===========================================================================

FUNCTION err_exception_handler
DESCRIPTION
  Expose exception handler entry point for use by main(); no other use
  case is supported. Context running main() blocks permanently at the
  exception handler on REX API targets once kernel initialization of
  rex_init() is completed.
============================================================================*/
void err_exception_handler(void) ERR_NORETURN_ATTRIBUTE; // exposed for main()

/** err_cb_enable
 * This will allow clients to register for callback to be invoke in path
 * of error handling post-crash.
 *
 * @param cb_info err_cb_info_t structure having required info.
 *
 * @return ERRCB_E_SUCCESS in case of successful registration else
 *         err_cb_error_t error codes.
 */
err_cb_error_t err_cb_enable(err_cb_info_t * cb_info);

/** err_cb_disable
 * This will allow clients to de-register callback, which was register via 
 * err_cb_enable.
 *
 * @param cb_info err_cb_info_t structure having required info.
 *
 * @return ERRCB_E_SUCCESS in case of successful de-registration else
 *         err_cb_error_t error codes.
 */
err_cb_error_t err_cb_disable(err_cb_info_t * cb_info);


/*===========================================================================
FUNCTION err_crash_cb_postflush_register
DESCRIPTION
  Register for callback function. The Callback function will be called after
  external functions are already called. 
  err_cb_ptr is void * func(void )
  It will be upto the callback to resolve its cache issues. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None
============================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_postflush_register (err_cb_ptr cb);

/*=========================================================================

FUNCTION err_crash_cb_postflush_deregister

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_postflush_deregister (err_cb_ptr cb);

/*=========================================================================

FUNCTION err_crash_cb_reg_pre_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr_u32) to be called immediately before
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback.

===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_reg_pre_STM(err_cb_ptr_u32 cb);


/*=========================================================================

FUNCTION err_crash_cb_dereg_pre_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_dereg_pre_STM(err_cb_ptr_u32 cb);

/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback,
  this API was provided only for special case handling to stop ULT Audio Core
  in DPM PL
===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb);

/*=========================================================================

FUNCTION err_crash_cb_dereg_next_to_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb);

/*=========================================================================

FUNCTION err_set_image_version_info

DESCRIPTION
  Error set image versioning information. This is enabled only for user PD. 
  Should be only called after err_init is called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void err_set_image_version_info
( 
  char * arg_QC_IMAGE_VERSION_STRING_AUTO_UPDATED,
  char * arg_IMAGE_VARIANT_STRING_AUTO_UPDATED,
  char * arg_OEM_IMAGE_VERSION_STRING_AUTO_UPDATED,
  char * arg_OEM_IMAGE_UUID_STRING_AUTO_UPDATED
);


#ifdef __cplusplus
}
#endif

#endif /* ERR_H */
