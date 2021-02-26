#ifndef RCXH_H
#define RCXH_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcxh.h
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcxh.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

// CLIENTS WILL USE THE APPROPRIATE NHLOS HEADER FILE TO ACCESS THE API AS DEPENDENCY.
// DIRECT CLIENT DEPENDENCY THROUGH THIS HEADER FILE ALONE IS NOT A SUPPORTED OPERATION.

// #include "rcinit_dal.h"     // NHLOS DAL
// #include "rcinit_posix.h"   // NHLOS POSIX
// #include "rcinit_qurt.h"    // NHLOS QURT
// #include "rcinit_rex.h"     // NHLOS REX

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_EXPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "setjmp.h" // internal use, supplies setjmp/longjmp

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

// 1. Portable. Does not require language polymorphism (C++), language run time
// type information (C++ RTTI). All utilization is On-Demand, and not as hidden
// overhead typically thought of in compiler language extenstions.
//
// 2. Not an ERR Replacement. Fatal Errors are still FATAL. The software exception
// model is an extenstion to normal software execution flow. It allows processing
// of exceptions to be isolated from the normal code execution path.
//
// 3. Not an DIAG Replacement. The software exception model is not an diagnostics
// logging framework. It can be utilized as a container to logging facilities, but
// does not directly provide logging facilities.
//
// 4. Extend C Language with more modern exception handling framework
// construct; similar to C++, Java, Objective-C language constructs for software
// exception handling.
//
// The following boilerplate demonstrates the structure needed to declare
// an Try/Catch/Finally/EndTry block.
//
// RCXH_EXT(xh_example);                  // defined elsewhere
//
// RCXH_DEFSUB(xh_example_1, xh_example); // extends xh_example "class"
// RCXH_DEFSUB(xh_example_2, xh_example); // extends xh_example "class"
//
// fn()
// {
//    body;
//
//    RCXH_THROW(xh_example_1);
//
//    RCXH_TRY
//    {
//       body;
//
//       RCXH_THROW(xh_example_2);
//    }
//    RCXH_CATCH(xh_example)
//    {
//       body;
//    }
//    RCXH_ENDTRY
//
//    RCXH_THROW(xh_example_2);
// }
//
// RCXH_TRY
// {
//    body;
//
//    fn();
// }
// RCXH_CATCH(xh_example_1)   // catches : xh_example_1
// {
//    body;
// }
// RCXH_CATCH(xh_example)     // catches : xh_example, xh_example_2
// {
//    body;
// }
// RCXH_FINALLY
// {
//    body;
// }
// RCXH_ENDTRY

#if defined(__FILENAME__)
#define _RCXH_FILE_ __FILENAME__                                                 /**< QC Build Extension (Command Line -D__FILENAME__=\"$@\" */
#else
#error IMAGE OWNER : Correct Build Process Configuration to Supply __FILENAME__  /**< QC Build Extension Must Be Configured */
#endif

#if !defined(NULL)
#define NULL ((void*)0)                                                          /**< ISO/ANSI-C, Forward reference NULL pointer */
#endif

/**
API, Enumeration for boolean return type
*/
typedef enum { RCXH_FALSE = 0, RCXH_TRUE } RCXH_BOOL;

/**
API, Structure captures representation of exception "class" hierchy. Internal
elements should not be accessed individually.
*/
typedef struct _rcxh_s
{
   char const* name;                                                             /**< internal, NULL terminated string representation of the identifier */
   struct _rcxh_s* parent;                                                       /**< internal, pointer to parent exception */

} * RCXH_HANDLE;                                                                 /**< API, Opaque Handle */

/**
Internal, Structure captures representation of "undef" exception "class" hierchy
elements should not be accessed individually.
*/
extern struct _rcxh_s __rcxh_undef;                                              /**< internal, undefined exception, base "class", undefined, internal */
extern struct _rcxh_s* _rcxh_undef;                                              /**< internal, undefined exception, base "class", undefined, internal */

/**
API, Exception "class", Create the opaque handle for an exception "class" by name
*/
#define RCXH_HANDLE(name)     (struct _rcxh_s*(rcxh_##name))

/**
API, Language Extension by Macro, Name of exception "class" as NULL terminated string, runtime
*/
#define RCXH_HANDLE_NAME(name) (RCXH_HANDLE(name)->name)

/**
API, Exception "class", Extern to an existing exception implementation
*/
#define RCXH_EXT(name)        extern struct _rcxh_s _rcxh_##name; extern struct _rcxh_s* rcxh_##name

/**
API, Exception "class", Define an exception implementation
*/
#define RCXH_DEF(name)        struct _rcxh_s _rcxh_##name = { #name, &__rcxh_undef }; struct _rcxh_s* rcxh_##name = &_rcxh_##name

/**
API, Exception "class", Define an exception implementation and associate it's parent class
*/
#define RCXH_DEFSUB(name, parent) struct _rcxh_s _rcxh_##name = { #name, &_rcxh_##parent }; struct _rcxh_s* rcxh_##name = &_rcxh_##name

/**
Internal, Structure captures representation of exception scope during the lifetime of its exsistence.
*/
typedef struct _rcxh_scope_s
{
   jmp_buf frame;                                                                /**< internal, setjmp/longjmp register file storage */
   struct _rcxh_scope_s* old;                                                    /**< internal, pointer to previous enclosing try */

   /**
   Internal, Enumeration captures current check of register file
   */
   volatile enum
   {
      _rcxh_frame_check_ok = 0x5a,                                               /**< internal, register file check */
      _rcxh_frame_check_no = 0x7f                                                /**< internal, register file check */

   } check;                                                                      /**< internal, register file check */

   /**
   Internal, Enumeration captures current syntax element of execution where user provided code may be executing.
   */
   enum
   {
      _rcxh_where_none,                                                          /**< internal, undefined or unknown */
      _rcxh_where_try,                                                           /**< internal, in try { body; } */
      _rcxh_where_exp,                                                           /**< internal, in catch (expression) */
      _rcxh_where_catch,                                                         /**< internal, in catch { body; } */
      _rcxh_where_finally,                                                       /**< internal, in finally { body; } */

   } where, src;                                                                 /**< internal, current where, thrown where src */

   /**
   Internal, Enumeration captures current state of exception processing.
   */
   enum                                                                          /**< internal */
   {
      _rcxh_state_unthrown,                                                      /**< internal, no exception */
      _rcxh_state_thrown,                                                        /**< internal, thrown, matching for a catch */
      _rcxh_state_caught,                                                        /**< internal, thrown, catch found and executed */

   } state;                                                                      /**< internal, current state */

   RCXH_HANDLE handle;                                                           /**< internal, "class" of the thrown exception */
   char const* file;                                                             /**< internal, file in which the exception was thrown */
   int line;                                                                     /**< internal, line on which the exception was thrown */

} _rcxh_scope_t, * _rcxh_scope_p;

/**
Initialization of service prior to use
@return
None.
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCXH_BOOL rcxh_init(void);

/**
Termination of service following use
@return
None.
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCXH_BOOL rcxh_term(void);

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Throw implementation
@param[in] NULL terminated string, file name of throw
@param[in] Integer, line number of throw
@param[in] Handle to the exception "class" of throw
@returns
None.
*/

DLL_API_EXPORT_RCINIT void _rcxh_throw(char const* const file, int const line, RCXH_HANDLE const handle);

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Try clause initialization implementation
@param[in] Current exception handler scope
@returns
None.
*/

DLL_API_EXPORT_RCINIT void _rcxh_scope_init(_rcxh_scope_p const scope);

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Catch clause initialization implementation
@param[in] Current exception handler scope
@param[in] Handle for active exception handler Try/Catch/Finally/EndTry
@returns
RCXH_TRUE -- Catching exception
RCXH_FALSE -- Not catching exception
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCXH_BOOL _rcxh_catching(_rcxh_scope_p const scope, RCXH_HANDLE const handle);

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Finally clause initialization implementation
@param[in] Current exception handler scope
@returns
None.
*/

DLL_API_EXPORT_RCINIT void _rcxh_finally(_rcxh_scope_p const scope);

/**
Internal, Supporting code for exception scope endtry clause
@param[in] Pointer to a valid exception scope
@returns
None.
*/

DLL_API_EXPORT_RCINIT void _rcxh_end(_rcxh_scope_p const scope);

/**
API, Language Extension by Macro, Required, Define Try clause of the Try/Catch/Finally/EndTry block
*/
#define RCXH_TRY              do { _rcxh_scope_t _rcxh_scope; _rcxh_scope_init(&_rcxh_scope); if (RCXH_FALSE == setjmp(_rcxh_scope.frame)) { _rcxh_scope.check = _rcxh_frame_check_ok;

/**
API, Language Extension by Macro, Optional, Define Catch clause of the Try/Catch/Finally/EndTry block
*/
#define RCXH_CATCH(handle)    } else if (RCXH_TRUE == _rcxh_catching(&_rcxh_scope, rcxh_##handle)) {

/**
API, Language Extension by Macro, Optional, Define Finally clause of the Try/Catch/Finally/EndTry block
*/
#define RCXH_FINALLY          } _rcxh_finally(&_rcxh_scope); {

/**
API, Language Extension by Macro, Required, Define EndTry clause of the Try/Catch/Finally/EndTry block
*/
#define RCXH_ENDTRY           } _rcxh_end(&_rcxh_scope); _rcxh_scope.check = _rcxh_frame_check_no; } while(0) /* MUST SUPPLY TERMINATOR */

/**
API, Language Extension by Macro, Required, Define Throw clause, Used inside an exception scope
*/
#define RCXH_THROW(handle)    _rcxh_throw(_RCXH_FILE_, __LINE__, rcxh_##handle)

/**
API, Language Extension by Macro, Required, Define Throw clause, Used inside an exception scope, Not Ever Compiled Out
*/
#define RCXH_ASSERT_THROW(condition, handle) do { if(!(condition)) _rcxh_throw(_RCXH_FILE_, __LINE__, rcxh_##handle); } while(0)

/**
API, Language Extension by Macro, File Name of Throw clause occurance
*/
#define RCXH_THROWN_FILE()    ((_rcxh_state_thrown == _rcxh_scope.state || _rcxh_state_caught == _rcxh_scope.state) ? _rcxh_scope.file : NULL)

/**
API, Language Extension by Macro, Line Number of Throw clause occurance
*/
#define RCXH_THROWN_LINE()    ((_rcxh_state_thrown == _rcxh_scope.state || _rcxh_state_caught == _rcxh_scope.state) ? _rcxh_scope.line : 0)

/**
API, Language Extension by Macro, Exception handle of Throw clause occurance
*/
#define RCXH_THROWN_HANDLE()  ((_rcxh_state_thrown == _rcxh_scope.state || _rcxh_state_caught == _rcxh_scope.state) ? _rcxh_scope.handle : NULL)

/**
API, Language Extension by Macro, Exception Handler, Tiny Version, Module Scope Localized Use Only

The tiny version is only suitable for use within a single module and is scope localized. Nesting is
allowed by compilers and enforced. However, a throw is never passed upwards from its enclosed scope.
This type of handling has a more limited set of use cases, but may be appropriate for localized
exception handling.

@param[in] integer or enum used as value in a scoped switch/case handler
@returns
None.
*/
#define RCXH_TRY_       do { jmp_buf _rcxh_frame; switch ( setjmp(_rcxh_frame) ) { case 0: while (1) {
#define RCXH_ENDTRY_    break; } } } while (0)
#define RCXH_CATCH_(x)  break; case x:
#define RCXH_FINALLY_   break; } default: {
#define RCXH_THROW_(x)  longjmp(_rcxh_frame, x)

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
