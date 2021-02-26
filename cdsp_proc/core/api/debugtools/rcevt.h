#ifndef RCEVT_H
#define RCEVT_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcevt.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcevt.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

// CLIENTS WILL USE THE APPROPRIATE NHLOS HEADER FILE TO ACCESS THE API AS DEPENDENCY.
// DIRECT CLIENT DEPENDENCY THROUGH THIS HEADER FILE ALONE IS NOT A SUPPORTED OPERATION.

// #include "rcevt_dal.h"     // NHLOS DAL
// #include "rcevt_posix.h"   // NHLOS POSIX
// #include "rcevt_qurt.h"    // NHLOS QURT
// #include "rcevt_rex.h"     // NHLOS REX

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
/* Export Default to be Local to ?.SO only */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_LOCAL                                      /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char const* RCEVT_NAME;                                                  /**< Name */
typedef void* RCEVT_HANDLE;                                                      /**< Opaque Handle */
typedef enum { RCEVT_FALSE = 0, RCEVT_TRUE } RCEVT_BOOL;                         /**< Boolean Query Return */
typedef unsigned long RCEVT_THRESHOLD;                                           /**< Threshold */

/**
API, Enumeration of Supported Notification Types
*/
typedef enum
{
   RCEVT_SIGEX_TYPE_UNKNOWN            = 0x00,
   RCEVT_SIGEX_TYPE_SIGNATIVE          = 0x20,
   RCEVT_SIGEX_TYPE_SIGDAL             = 0x21,
   RCEVT_SIGEX_TYPE_SIGPOSIX           = 0x22,
   RCEVT_SIGEX_TYPE_SIGQURT            = 0x23,
   RCEVT_SIGEX_TYPE_SIGQURT_USER       = 0x24,
   RCEVT_SIGEX_TYPE_SIGREX             = 0x25,
   RCEVT_SIGEX_TYPE_NONE               = 0xff                                    /**< RCEVT [0x20..0x2f] */

} RCEVT_SIGEX_TYPE;                                                              /**< Specialized Enum Values DO NOT Overlap RCEVT, RCECB, RCESN */

typedef void* RCEVT_SIGEX;                                                       /**< Opaque Handle to Abstracted Event Notification Structures */

#define RCEVT_ZERO   0                                                           /**< Manifest Constant Assist */
#define RCEVT_NULL   (void*)0                                                    /**< ISO/ANSI-C, Forward reference NULL pointer */

/**
API, Service initialization
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCEVT_BOOL rcevt_init(void);

/**
API, Service termination
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCEVT_BOOL rcevt_term(void);

/**
API, Create an event, with name

Prerequsite call before posting event -- create the event
before use to avoid a potential "race condition" between
contexts that will post the event and concurrently wait on
the event being posted.

For performance, cache the resulting handle for use within
the API. Name based use cases will take a penalty when doing
a dictionary lookup on every call.

@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_create_name(RCEVT_NAME const name);

/**
API, Search an event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_BOOL -- Boolean reflecting event previously created
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_BOOL rcevt_search_name(RCEVT_NAME const name);

/**
API, Count of registrants for event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT int rcevt_getregistrants_handle(RCEVT_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE int rcevt_getcontexts_handle(RCEVT_HANDLE const handle) /**< OBSOLETE; CLIENTS USE rcevt_getregistrants_handle */
{
   return rcevt_getregistrants_handle(handle);
}

/**
API, Count of registrants for event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT int rcevt_getregistrants_name(RCEVT_NAME const name);

DLL_API_WARN_UNUSED_RESULT DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE int rcevt_getcontexts_name(RCEVT_NAME const name) /**< OBSOLETE; CLIENTS USE rcevt_getregistrants_name */
{
   return rcevt_getregistrants_name(name);
}

/**
API, Count of times event signaled, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_THRESHOLD -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_THRESHOLD rcevt_getcount_handle(RCEVT_HANDLE const handle);

/**
API, Count of times event signaled, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_THRESHOLD -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_THRESHOLD rcevt_getcount_name(RCEVT_NAME const name);

/**
API, Register notification to event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_register_sigex_handle(RCEVT_HANDLE const handle, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const signal);

/**
API, Register notification to event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_register_sigex_name(RCEVT_NAME const name, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const signal);

/**
API, Unegister notification to event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_unregister_sigex_handle(RCEVT_HANDLE const handle, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Unegister notification to event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_unregister_sigex_name(RCEVT_NAME const name, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Signal event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_signal_handle(RCEVT_HANDLE const handle);

/**
API, Signal event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_signal_name(RCEVT_NAME const name);

/**
API, Wait for event at specific threshold, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification begins with threshold and continues until unregistered
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_sigex_handle(RCEVT_HANDLE const handle, RCEVT_THRESHOLD const threshold, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Wait for event at specific threshold, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification begins with threshold and continues until unregistered
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_sigex_name(RCEVT_NAME const name, RCEVT_THRESHOLD const threshold, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex);

/**
API, Wait for event, one time per call, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_handle(RCEVT_HANDLE const handle);

/**
API, Wait for event, one time per call, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_name(RCEVT_NAME const name);

/**
API, Wait for event, one time per call, until threshold met, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_count_handle(RCEVT_HANDLE const handle, RCEVT_THRESHOLD const threshold);

/**
API, Wait for event, one time per call, until threshold met, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCEVT_HANDLE rcevt_wait_count_name(RCEVT_NAME const name, RCEVT_THRESHOLD const threshold);

/**
The following function signatures should not be used. The correct dependency
mechanism is to use RCEVT_REX.H supplied API. These functions cannot be universal.
*/
typedef unsigned long RCEVT_SIGNAL DLL_API_WARN_DEPRECATED; /**< OBSOLETE; CLIENTS USE RCEVT_REX.H */

DLL_API_WARN_UNUSED_RESULT DLL_API_WARN_DEPRECATED
DLL_API_LOCAL RCEVT_HANDLE rcevt_register_handle(RCEVT_HANDLE const handle, unsigned long const signal); /**< OBSOLETE; CLIENTS USE RCEVT_REX.H */

DLL_API_WARN_UNUSED_RESULT DLL_API_WARN_DEPRECATED
DLL_API_LOCAL RCEVT_HANDLE rcevt_register_name(RCEVT_NAME const name, unsigned long const signal); /**< OBSOLETE; CLIENTS USE RCEVT_REX.H */

/** @} end_addtogroup rcinit_api */

/** @addtogroup rcinit_internal_api @{ */

/**
INTERNAL, CLIENT USE NOT SUPPORTED, MUTABLE DEFINITIONS
*/
#define RCEVT_CTOR_ORDER            127                                                /**< Constructor Ordering Hint for Linker */
#define RCEVT_CTOR_ATTRIB           __attribute__((constructor(RCEVT_CTOR_ORDER)))     /**< Constructor Compiler Attribute */
#define RCEVT_DTOR_ORDER            129                                                /**< Destructor Ordering Hint for Linker */
#define RCEVT_DTOR_ATTRIB           __attribute__((destructor(RCEVT_DTOR_ORDER)))      /**< Destructor Compiler Attribute */

/** @} end_addtogroup rcinit_internal_api */

#if defined(__cplusplus)
}
#endif

#endif
