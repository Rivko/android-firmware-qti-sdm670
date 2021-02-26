#ifndef RCESN_H
#define RCESN_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcesn.h
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
$Header: //components/rel/core.qdsp6/2.1/api/debugtools/rcesn.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

// CLIENTS WILL USE THE APPROPRIATE NHLOS HEADER FILE TO ACCESS THE API AS DEPENDENCY.
// DIRECT CLIENT DEPENDENCY THROUGH THIS HEADER FILE ALONE IS NOT A SUPPORTED OPERATION.

// #include "rcesn_dal.h"     // NHLOS DAL
// #include "rcesn_posix.h"   // NHLOS POSIX
// #include "rcesn_qurt.h"    // NHLOS QURT
// #include "rcesn_rex.h"     // NHLOS REX

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
/* Export Default to be LOCAL for .SO */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_LOCAL                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "timer.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char const* RCESN_NAME;                                                  /**< Name */
typedef void* RCESN_HANDLE;                                                      /**< Opaque Handle */
typedef enum { RCESN_FALSE = 0, RCESN_TRUE } RCESN_BOOL;                         /**< Boolean Query Return */
typedef unsigned long RCESN_THRESHOLD;                                           /**< Threshold */
typedef unsigned long RCESN_STATE;                                               /**< State Value */
typedef RCESN_BOOL (*RCESN_FNSIG_COMPARE)(RCESN_HANDLE const);                   /**< Comparision Function (handle to 'curr' for registrant) */

/**
API, Enumeration of Supported Notification Types
*/
typedef enum
{
   RCESN_SIGEX_TYPE_UNKNOWN            = 0x00,
   RCESN_SIGEX_TYPE_SIGNATIVE          = 0x30,
   RCESN_SIGEX_TYPE_SIGDAL             = 0x31,
   RCESN_SIGEX_TYPE_SIGPOSIX           = 0x32,
   RCESN_SIGEX_TYPE_SIGQURT            = 0x33,
   RCESN_SIGEX_TYPE_SIGQURT_USER       = 0x34,
   RCESN_SIGEX_TYPE_SIGREX             = 0x35,
   RCESN_SIGEX_TYPE_NONE               = 0xff                                    /**< RCEVT [0x30..0x3f] */

} RCESN_SIGEX_TYPE;                                                              /**< Enum Values DO NOT Overlap RCEVT, RCECB, RCESN */

typedef void* RCESN_SIGEX;                                                       /**< Opaque Handle to Abstracted Event Notification Structures */

#define RCESN_ZERO   0                                                           /**< Manifest Constant Assist */
#define RCESN_NULL   (void*)0                                                    /**< ISO/ANSI-C, Forward reference NULL pointer */

/**
API, Service initialization
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCESN_BOOL rcesn_init(void);

/**
API, Service termination
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCESN_BOOL rcesn_term(void);

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
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_create_name(RCESN_NAME const name);

/**
API, Search an event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@return
RCESN_BOOL -- Boolean reflecting event previously created
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_search_name(RCESN_NAME const name);

/**
API, Count of registrants for event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT int rcesn_getregistrants_handle(RCESN_HANDLE const handle);

/**
API, Count of registrants for event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT int rcesn_getregistrants_name(RCESN_NAME const name);

/**
API, Current State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_getstatecurr_handle(RCESN_HANDLE const handle);

/**
API, Current State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] State to apply
@return
RCESN_STATE -- State of event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_setstatecurr_handle(RCESN_HANDLE const handle, RCESN_STATE const state);

/**
API, Previous State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_getstateprev_handle(RCESN_HANDLE const handle);

/**
API, Previous State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] State to apply
@return
RCESN_STATE -- State of event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_setstateprev_handle(RCESN_HANDLE const handle, RCESN_STATE const state);

/**
API, Current State of event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_getstatecurr_name(RCESN_NAME const name);

/**
API, Previous State of event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_STATE rcesn_getstateprev_name(RCESN_NAME const name);

/**
API, Register notification to event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_register_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Register notification to event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_register_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Unegister notification to event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_unregister_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Unegister notification to event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_unregister_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Signal event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_signal_handle(RCESN_HANDLE const handle, RCESN_STATE const state);

/**
API, Signal event, with handle and timetick
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@param[in] Maximum timer value associated to event
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_signal_handle_timetick(RCESN_HANDLE const handle, RCESN_STATE const state, unsigned long* const timetick_p);

/**
API, Signal event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_signal_name(RCESN_NAME const name, RCESN_STATE const state);

/**
API, Signal event, with name and timetick
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@param[in] Maximum timer value associated to event
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_signal_name_timetick(RCESN_NAME const name, RCESN_STATE const state, unsigned long* const timetick_p);

/**
API, Wait for event at specific threshold, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_wait_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Wait for event at specific state, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_wait_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex);

/**
API, Wait for event, one time per call, until compare met, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Comparator function pointer
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_wait_handle(RCESN_HANDLE const handle, RCESN_FNSIG_COMPARE const compare);

/**
API, Wait for event, one time per call, until compare met, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Comparator function pointer
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_wait_name(RCESN_NAME const name, RCESN_FNSIG_COMPARE const compare);

/**
API, Common Comparision Functions
See prerequsite note with rcesn_create_name
Clients have the ability to supply comparator callback functions for use. Primarily
these functions will capture the policy logic and determine if notification should
occur with a TRUE/FALSE return. NOTE: these callback functions are never allowed
to block by policy; to do so is considered a bug. Callbacks are exeucted and if
they do not complete within an gracious time period (~20ms), the system is halted.
The timer is not deferred, and will preculde DVCS sleep and power collapse.
@param[in] Opaque handle of the event
@return
RCESN_BOOL -- Boolean reflecting comparision status
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_lt_0(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_eq_0(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_ne_0(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_gt_0(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_lt_prev(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_eq_prev(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_ne_prev(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_gt_prev(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_mask_t_0001(RCESN_HANDLE const handle);

DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_BOOL rcesn_compare_mask_f_0001(RCESN_HANDLE const handle);

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
