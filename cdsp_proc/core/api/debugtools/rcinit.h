#ifndef RCINIT_H
#define RCINIT_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcinit.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcinit.h#1 $
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
/* Export Default to be Local to ?.SO only */
#define DLL_API_EXPORT_RCINIT DLL_API_LOCAL                                      /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "rcevt.h" // internal use, supplies RCEVT_SIGEX

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

/**
API, Exposed Events for Observers
*/
#define RCINIT_RCECB_PREFIX                  ""                                  /**< prefix for rcinit events (localized protection domain) */
#define RCINIT_RCECB_INITGROUPS              RCINIT_RCECB_PREFIX "initgroups"    /**< observers ok */
#define RCINIT_RCECB_TERMGROUPS              RCINIT_RCECB_PREFIX "termgroups"    /**< observers ok */

#define RCINIT_RCESN_PREFIX                  ""                                  /**< prefix for rcinit events (localized protection domain) */
#define RCINIT_RCESN_INITGROUPS              RCINIT_RCEVT_PREFIX "initgroups"    /**< observers ok */
#define RCINIT_RCESN_TERMGROUPS              RCINIT_RCEVT_PREFIX "termgroups"    /**< observers ok */

#define RCINIT_RCEVT_PREFIX                  ""                                  /**< prefix for rcinit events (localized protection domain) */
#define RCINIT_RCEVT_INITGROUPS              RCINIT_RCEVT_PREFIX "initgroups"    /**< observers ok */
#define RCINIT_RCEVT_TERMGROUPS              RCINIT_RCEVT_PREFIX "termgroups"    /**< observers ok */

/**
API, Enumeration for Return Type
*/
typedef enum
{
   RCINIT_SUCCESS = 0,
   RCINIT_ERROR = -1

} RCINIT_RESULT;

typedef enum { RCINIT_FALSE = 0, RCINIT_TRUE } RCINIT_BOOL;                         /**< Boolean Query Return */

#define RCINIT_NULL           ((void*)0)                                         /**< return result */
#define RCINIT_ZERO           (0)                                                /**< return result */

/**
API, Enumeration for Group Representation
*/
typedef enum
{
   RCINIT_GROUP_0 = 0,                                                           /**< internal representation group 0 */
   RCINIT_GROUP_1 = 1,                                                           /**< internal representation group 1 */
   RCINIT_GROUP_2 = 2,                                                           /**< internal representation group 2 */
   RCINIT_GROUP_3 = 3,                                                           /**< internal representation group 3 */
   RCINIT_GROUP_4 = 4,                                                           /**< internal representation group 4 */
   RCINIT_GROUP_5 = 5,                                                           /**< internal representation group 5 */
   RCINIT_GROUP_6 = 6,                                                           /**< internal representation group 6 */
   RCINIT_GROUP_7 = 7,                                                           /**< internal representation group 7 */
   RCINIT_GROUP_NONE = 8,
   RCINIT_GROUP_PARKED = 7,
   RCINIT_GROUP_MAX = RCINIT_GROUP_NONE                                          /**< end of list (overloaded to NONE) */

} RCINIT_GROUP;

/**
API, Enumeration for Task/Function Type Representation
*/
typedef enum
{
   RCINIT_TASK_POLICYFN = 1,                                                     /**< policy function callbacks (not task) */
   RCINIT_TASK_INITFN = 2,                                                       /**< initialization function callbacks (not task) */
   RCINIT_TASK_TERMFN = 3,                                                       /**< termination function callbacks (not task) */
   RCINIT_TASK_DAL = 4,                                                          /**< dalsys tasks */
   RCINIT_TASK_POSIX = 5,                                                        /**< pthread tasks */
   RCINIT_TASK_QURT = 6,                                                         /**< qurt tasks */
   RCINIT_TASK_REX = 7,                                                          /**< rex tasks */
   RCINIT_TASK_LEGACY = 8,                                                       /**< deprecated rex tasks with feature guard (tmc) */
   RCINIT_TASK_NONE                                                              /**< end of list */

} RCINIT_TASK;

#define RCINIT_TASK_DALTASK   RCINIT_TASK_DAL                                    /**< Deprecated Alias */
#define RCINIT_TASK_QURTTASK  RCINIT_TASK_QURT                                   /**< Deprecated Alias */
#define RCINIT_TASK_REXTASK   RCINIT_TASK_REX                                    /**< Deprecated Alias */

/**
API, Task Priority Representation
*/
typedef short RCINIT_PRIO;

#define RCINIT_PRIO_MAX             (255)                                        /**< Priority HIGH = 255, to LOW = 1, mapping performed internally for nonhlos use */
#define RCINIT_PRIO_MIN             (1)                                          /**< Priority HIGH = 255, to LOW = 1, mapping performed internally for nonhlos use */
#define RCINIT_PRIO_NONE            (-1)                                         /**< Not Assigned */
#define RCINIT_PRIO_INITFN          (-2)                                         /**< Place Holder (not actual priority initfn run at) */
#define RCINIT_PRIO_POLICYFN        (-3)                                         /**< Place Holder (not actual priority policyfn run at) */

/**
API, Task Stack Size Representation
*/
typedef unsigned long RCINIT_STKSZ;                                              /**< Stack Size (KB) */

#define RCINIT_STKSZ_ZERO (0)                                                    /**< Zero Length Stack (Initialization Decision) */
#define RCINIT_STKSZ_NONE (-1)                                                   /**< No Stack (Initialization Decision) */

/**
API, Task CPU (Hardware Thread) Affinity Representation
*/
typedef unsigned char RCINIT_AFFINITY;                                           /**< Place holder; not all nonhlos kernel interfaces use this */

/**
API, Task Stack type Representation
*/
typedef enum 
{
   RCINIT_STACK_TYPE_NONE = 0,                                                   /**< Init functions */
   RCINIT_STACK_TYPE_STATIC = 1,                                                 /**< Static Stacks */
   RCINIT_STACK_TYPE_DYNAMIC = 2,                                                /**< Dynamic Stacks */   
   RCINIT_STACK_TYPE_AUTO = 3,                                                   /**< Auto Stacks */
}RCINIT_STKTYPE;  /**< Stack type */

typedef unsigned long RCINIT_HEAPQSZ;                                            /**< Heap Size (Byte) */
typedef void * RCINIT_HEAP_PTR;

#define RCINIT_HEAPQSZ_ZERO            (RCINIT_HEAPQSZ)(0)                       /**< Zero Length Stack (Internal Initialization Decision) */
#define RCINIT_HEAPQSZ_NONE            (RCINIT_HEAPQSZ)(-1)                      /**< No Stack (Internal Initialization Decision) */

typedef void* RCINIT_ENTRY;                                                      /**< Entry Point */

typedef int RCINIT_POLICY;                                                       /**< Policy Filter Result */

#define RCINIT_POLICY_NONE                   (-1)
#define RCINIT_POLICY_NAME_DEFAULT           "default"                           /**< Maps to Arbitrary Placement at Runtime */

typedef void (*RCINIT_FNSIG_VOID_VOID)(void);                                    /**< Specific Function Signature */
typedef void* (*RCINIT_FNSIG_VOIDP_VOID)(void);                                  /**< Specific Function Signature */

typedef void* RCINIT_INFO;                                                       /**< Opaque Handle */
typedef const char* RCINIT_NAME;                                                 /**< Pointer to const char */

#define RCINIT_NAME_MAX 16                                                       /**< Tracks All NHLOS Maximum Task Name + NULL Terminator */


DLL_API_EXPORT_RCINIT extern unsigned long rcinit_internal_tls_available;        /**< Internal Flag */
#define RCINIT_TASK_TLS_AVAILABLE      0xf00df173                                /**< TLS Available Magic */

/**
API, Unique Identifier
*/
typedef unsigned long RCINIT_HASH;                                               /**< Unique Identifier */

/**
API, name to RCINIT_INFO Opaque handle
@param[in] NULL terminated string, string identifier for task context
@returns
NULL -- Error
RCINIT_INFO -- Opaque handle representing an RCINIT_INFO object
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_INFO rcinit_lookup(RCINIT_NAME const name);


/**
API, self to RCINIT_INFO Opaque handle
@returns
RCINIT_NULL -- Error
RCINIT_INFO -- Opaque handle representing an RCINIT_INFO object
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_INFO rcinit_lookup_self(void);

/**
API, RCINIT_INFO Opaque handle to name
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
NULL -- Error
RCINIT_NAME -- NULL terminated string, string identifier for task context
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_NAME rcinit_lookup_name_info(RCINIT_INFO const info);

/**
API, name to Policy Extension Inclusion (returns only First Occurance if Multiple)
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_POLICY -- Returns the Policy Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_POLICY rcinit_lookup_policy(RCINIT_NAME const name);

/**
API, Current Policy Extension (eg, returns default)
@returns
RCINIT_POLICY -- Returns the Policy Representation
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_POLICY rcinit_lookup_policy_current(void);

/**
API, name to Task Priority Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_PRIO -- Returns the Priority Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_PRIO rcinit_lookup_prio(RCINIT_NAME const name);

/**
API, RCINIT_INFO Opaque handle to Task Priority Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
RCINIT_PRIO -- Returns the Priority Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_PRIO rcinit_lookup_prio_info(RCINIT_INFO const info);

/**
API, name to Task Group Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_GROUP rcinit_lookup_group(RCINIT_NAME const name);

/**
API, entry to Task Group Provisioned at Compile Time
@param[in] Address of entry point
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_GROUP rcinit_lookup_group_entry(void* const entry);

/**
API, Current group processing during initialization or termination
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_GROUP rcinit_lookup_group_current(void);

/**
API, name to Task Stack Size Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
unsigned long -- Returns the Stack Size Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT unsigned long rcinit_lookup_stksz(RCINIT_NAME const name);

/**
API, RCINIT_INFO Opaque handle to Task Stack Size Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
unsigned long -- Returns the Stack Size Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT unsigned long rcinit_lookup_stksz_info(RCINIT_INFO const info);

/**
API, name to Task Heap Quota Size Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_HEAPQSZ -- Returns the heap quota for the task name
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_HEAPQSZ rcinit_lookup_heapqsz(RCINIT_NAME const name, RCINIT_HEAP_PTR heap_ptr);

/**
API, name to Task Heap Quota Size Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
RCINIT_HEAPQSZ -- Returns the Heap Quota Size Representation
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_HEAPQSZ rcinit_lookup_heapqsz_info(RCINIT_INFO const info, RCINIT_HEAP_PTR heap_ptr);

/**
API, self to Heap Quota Size
@returns
RCINIT_HEAPQSZ -- Returns the Heap Quota Size Representation
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_HEAPQSZ rcinit_lookup_heapqsz_self(RCINIT_HEAP_PTR heap_ptr);

/**
API, name to Pointer Provisioned at Compile Time Presense
@param[in] NULL terminated string, string identifier for task context
@returns
NULL -- Netagive Query for Presense
void* -- Positive Query for Presense
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT void* rcinit_lookup_entry(RCINIT_NAME const name);

/**
API, RCINIT_INFO Opaque handle to Pointer Provisioned at Compile Time Presense
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
NULL -- Netagive Query for Presense
void* -- Positive Query for Presense
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT void* rcinit_lookup_entry_info(RCINIT_INFO const info);

/**
API, name to Pointer Provisioned at Compile Time Presense
@param[in] Opaque handle representing an RCINIT_PRIO object
@returns
RCINIT_PRIO_NONE -- Netagive Query for Presense
RCINIT_PRIO -- Positive Query for Presense (MAY NEED TO BE MAPPED TO NHLOS INTERFACES)
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCINIT_PRIO rcinit_lookup_priority(RCINIT_NAME const name);

/**
API, Required Handshake at Startup by All Tasks, Logical Barrier Rendevous
@returns
None.
*/
DLL_API_EXPORT_RCINIT void rcinit_handshake_init(void);

/**
API, Required Handshake at Startup by All Tasks, Logical Barrier Rendevous (ALIAS)
@returns
None.
*/
void rcinit_handshake_startup(void);

/**
API, Required Handshake at Termination by Registered Tasks, Logical Barrier Rendevous
@returns
None.
*/
DLL_API_EXPORT_RCINIT void rcinit_handshake_term(void);

/**
API, Required Handshake at Termination by Registered Tasks, Logical Barrier Rendevous (ALIAS)
@returns
None.
*/
DLL_API_EXPORT_RCINIT void rcinit_handshake_shutdown(void);

/**
API, Runtime Termination Registeration by Tasks, General Interface required RCEVT SIGEX Setup
@param[in] RCEVT SIGEX Notification Type
@param[in] RCEVT SIGEX Type Specific Union
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_register_term(RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Runtime Termination Unegisteration by Tasks, General Interface required RCEVT SIGEX Setup
@param[in] RCEVT SIGEX Notification Type
@param[in] RCEVT SIGEX Type Specific Union
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_unregister_term(RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Runtime Termination Registeration by Tasks, General Interface required RCEVT SIGEX Setup
@param[in] RCINIT Termination Group Enumeration
@param[in] RCEVT SIGEX Notification Type
@param[in] RCEVT SIGEX Type Specific Union
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_register_term_group(RCINIT_GROUP const group, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Runtime Termination Unegisteration by Tasks, General Interface required RCEVT SIGEX Setup
@param[in] RCINIT Termination Group Enumeration
@param[in] RCEVT SIGEX Notification Type
@param[in] RCEVT SIGEX Type Specific Union
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_unregister_term_group(RCINIT_GROUP const group, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Runtime Termination Registeration by Functions
@param[in] RCINIT Callback Function
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_register_termfn(RCINIT_FNSIG_VOID_VOID const fn);

/**
API, Runtime Termination Registeration by Functions
@param[in] RCINIT Termination Group Enumeration
@param[in] RCINIT Callback Function
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_register_termfn_group(RCINIT_GROUP const group, RCINIT_FNSIG_VOID_VOID const fn);

/**
API, Runtime Termination Unegisteration by Functions
@param[in] RCINIT Termination Group Enumeration
@param[in] RCINIT Callback Function
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_unregister_termfn_group(RCINIT_GROUP const group, RCINIT_FNSIG_VOID_VOID const fn);

/**
API, Highly Specialized Use Case to Start Tasks Before GROUP_0, Very Specialized
Use Case that is utilized by Timers to start their task before Functions of GROUP_0
are executed. This API is not intended for general technology area utilization.
@param[in] RCINIT_INFO Opaque Handle
@param[in] Task Entry Point
@returns
None.
*/

DLL_API_EXPORT_RCINIT void rcinit_initfn_spawn_task(RCINIT_INFO const info, void* const entry);

/** @} end_addtogroup rcinit_api */

/** @addtogroup rcinit_internal_api @{ */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/

/**
INTERNAL, Highly Specialized Use Case to leverage C/C++ Compiler Preprocessor into
building compile time specific language constructs. These macros are for internal
module utilization only, and support the automatic generation of C/C++ source modules
created at build time by the RCINIT Framework SCONS Build Tool.

REQUIRES ANSI/ISO C99 Compliant Compiler Extensions to be Enabled
*/

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#define RCINIT_STRIFY(arg)                RCINIT_STRIFY1(arg)
#define RCINIT_STRIFY1(arg)               RCINIT_STRIFY2(arg)
#define RCINIT_STRIFY2(arg)               #arg

#define RCINIT_CONCAT(arg1, arg2)         RCINIT_CONCAT1(arg1, arg2)
#define RCINIT_CONCAT1(arg1, arg2)        RCINIT_CONCAT2(arg1, arg2)
#define RCINIT_CONCAT2(arg1, arg2)        arg1##arg2

#define RCINIT_FOREACH_1(what, x, ...)    what(x) /* __VA_ARGS__ */
#define RCINIT_FOREACH_2(what, x, ...)    what(x) , RCINIT_FOREACH_1(what, __VA_ARGS__)
#define RCINIT_FOREACH_3(what, x, ...)    what(x) , RCINIT_FOREACH_2(what, __VA_ARGS__)
#define RCINIT_FOREACH_4(what, x, ...)    what(x) , RCINIT_FOREACH_3(what, __VA_ARGS__)
#define RCINIT_FOREACH_5(what, x, ...)    what(x) , RCINIT_FOREACH_4(what, __VA_ARGS__)
#define RCINIT_FOREACH_6(what, x, ...)    what(x) , RCINIT_FOREACH_5(what, __VA_ARGS__)
#define RCINIT_FOREACH_7(what, x, ...)    what(x) , RCINIT_FOREACH_6(what, __VA_ARGS__)
#define RCINIT_FOREACH_8(what, x, ...)    what(x) , RCINIT_FOREACH_7(what, __VA_ARGS__)

#define RCINIT_FOREACH_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define RCINIT_FOREACH_RSEQ_N()           8,7,6,5,4,3,2,1,0
#define RCINIT_FOREACH_NARG_(...)         RCINIT_FOREACH_ARG_N(__VA_ARGS__)
#define RCINIT_FOREACH_NARG(...)          RCINIT_FOREACH_NARG_(__VA_ARGS__, RCINIT_FOREACH_RSEQ_N())

#define RCINIT_FOREACH_(N, what, x, ...)  RCINIT_CONCAT(RCINIT_FOREACH_, N)(what, x, __VA_ARGS__)
#define RCINIT_FOREACH(what, x, ...)      RCINIT_FOREACH_(RCINIT_FOREACH_NARG(x, ##__VA_ARGS__), what, x, ##__VA_ARGS__)

#define RCINIT_CTOR_ADDFUNC(x, gr, fn, ...) \
void rcinit_func_ctor_##x##(void) RCINIT_CTOR_ATTRIB; \
static deps_t const rcinit_func_const_##x = { #x, fn, gr, (char const*[RCINIT_FOREACH_NARG(__VA_ARGS__) + 1]){ RCINIT_FOREACH(RCINIT_STRIFY, __VA_ARGS__) , NULL } }; \
static input_t rcinit_func_rwdata_##x = { &rcinit_func_const_##x, NULL }; \
void rcinit_func_ctor_##x##(void) { \
}

#define RCINIT_CTOR_ADDTASK(x, gr, fn, ...) \
void rcinit_task_ctor_##x##(void) RCINIT_CTOR_ATTRIB; \
static deps_t const rcinit_task_const_##x = { #x, fn, gr, (char const*[RCINIT_FOREACH_NARG(__VA_ARGS__) + 1]){ RCINIT_FOREACH(RCINIT_STRIFY, __VA_ARGS__) , NULL } }; \
static input_t rcinit_task_rwdata_##x = { &rcinit_task_const_##x, NULL }; \
void rcinit_task_ctor_##x##(void) { \
}

#endif

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
#define RCINIT_CTOR_ORDER           128                                                /**< Constructor Ordering Hint for Linker */
#define RCINIT_CTOR_ATTRIB          __attribute__((constructor(RCINIT_CTOR_ORDER)))    /**< Constructor Compiler Attribute */
#define RCINIT_DTOR_ORDER           128                                                /**< Destructor Ordering Hint for Linker */
#define RCINIT_DTOR_ATTRIB          __attribute__((destructor(RCINIT_DTOR_ORDER)))     /**< Destructor Compiler Attribute */

#define RCINIT_STACK_ALIGN          16                                                 /**< Internal Stack Alignment */
#define RCINIT_STACK_NULL           ((rcinit_stack_p)0)                                /**< NULL Stack */
#define RCINIT_STACK_NONE           ((rcinit_stack_p)1)                                /**< INVALID Stack */
#define RCINIT_STACK_INIT           (0xa5)                                             /**< Stack Fill */

#define RCINIT_ENTRY_NULL           ((rcinit_entry_p)0)                                /**< NULL Entry */
#define RCINIT_ENTRY_NONE           ((rcinit_entry_p)1)                                /**< INVALID Entry */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
#define RCINIT_AFFINITY_NONE           (0)                                             /**< No Affinity (aka ALL) */
#define RCINIT_AFFINITY_SINGLE         (1)                                             /**< Single Affinity */
#define RCINIT_AFFINITY_ALL            (255)                                           /**< All Affinity */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef void* rcinit_tid_p;                                                      /**< Internal opaque task handle */
typedef unsigned long rcinit_stack_t, *rcinit_stack_p;                           /**< Internal native stack */
typedef void* rcinit_entry_p;                                                    /**< Internal opaque task entry address */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_handle_s                                                   /**< Internal Database */
{
   rcinit_tid_p tid_p;                                                           /**< Internal opaque task handle */
   rcinit_stack_p stack;                                                         /**< Internal native stack */
   rcinit_entry_p entry;                                                         /**< Internal opaque task entry address */
   void* unused1;
   void* unused2;

} rcinit_handle_t, *rcinit_handle_p;

/**
INTERNAL, CLIENT DIRECT USE NOT SUPPORTED, ONLY USE RUNTIME API ACCESSORS, MUTABLE DEFINITIONS
*/
typedef void* rcinit_heap_p;                                                      /**< Internal opaque heap handle */

typedef struct rcinit_heapq_s                                                    /**< Internal Heap quota structure*/
{
   rcinit_heap_p heap_p;
   RCINIT_HEAPQSZ heap_quota;
} rcinit_heapq_t, *rcinit_heapq_p;
/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_info_s                                                     /**< Internal Database */
{
   RCINIT_NAME const name;                                                       /**< Internal name */
   RCINIT_HASH hash;                                                             /**< Internal name hash */
   RCINIT_GROUP group;                                                           /**< Internal group */
   RCINIT_TASK type;                                                             /**< Internal type */
   RCINIT_ENTRY entry;                                                           /**< Internal opaque task entry address */
   RCINIT_PRIO prio;                                                             /**< Internal priority value */
   RCINIT_NAME const prio_name;                                                  /**< Internal priority name */
   RCINIT_STKSZ stksz;                                                           /**< Internal stack size */
   RCINIT_STKTYPE stktype;                                                       /**< Internal Stack type */
   rcinit_heapq_p heapq_list;                                                    /**< Internal handle heap quotas */
   RCINIT_AFFINITY cpu_affinity;                                                 /**< Internal processor affinity */
   RCINIT_NAME const (*plist)[];                                                 /**< Internal policy extension */
   RCINIT_NAME const (*dlist)[];                                                 /**< Internal dependency */
   rcinit_handle_p handle;                                                       /**< Internal handle to volatile memory */

} rcinit_info_t, *rcinit_info_p;

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_name_map_s                                                 /**< Internal Database */
{
   rcinit_info_t const* info_p;
   struct rcinit_name_map_s* next;

} rcinit_name_map_t, *rcinit_name_map_p;

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
DLL_API_EXPORT_RCINIT extern rcinit_name_map_p rcinit_name_map;                  /**< Internal Database */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_internal_name_map_s                                        /**< Internal Database */
{
   RCINIT_NAME const name;                                                       /**< Internal name */
   const rcinit_info_t* info_p;                                                  /**< Internal info */

} rcinit_internal_name_map_t;

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_internal_policy_map_s                                      /**< Internal Database */
{
   RCINIT_NAME const name;                                                       /**< Internal name */
   int index;                                                                    /**< Internal index */

} rcinit_internal_policy_map_t;

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
typedef struct rcinit_internal_priority_map_s                                    /**< Internal Database */
{
   RCINIT_NAME const name;                                                       /**< Internal priority name */
   RCINIT_PRIO prio;                                                             /**< Internal priority value */

} rcinit_internal_priority_map_t;

/** @} end_addtogroup rcinit_internal_api */

#if defined(__cplusplus)
}
#endif

#endif
