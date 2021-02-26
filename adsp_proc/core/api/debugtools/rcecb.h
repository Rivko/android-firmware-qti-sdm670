#ifndef RCECB_H
#define RCECB_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcecb.h
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
$Header: //components/rel/core.qdsp6/2.1/api/debugtools/rcecb.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_LOCAL                                      /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
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
typedef char const* RCECB_NAME;                                                  /**< Name */
typedef void* RCECB_HANDLE;                                                      /**< Opaque Handle */
typedef enum { RCECB_FALSE = 0, RCECB_TRUE } RCECB_BOOL;                         /**< Boolean Query Return */

typedef unsigned long RCECB_PARM;

typedef void* RCECB_CALLBACK_FNSIG;                                              /**< Function Pointer Handle */
typedef void (*RCECB_CALLBACK_FNSIG_P0)(void);                                   /**< Specialized Function Pointer */
typedef void (*RCECB_CALLBACK_FNSIG_P1)(RCECB_PARM);                             /**< Specialized Function Pointer */
typedef void (*RCECB_CALLBACK_FNSIG_P2)(RCECB_PARM, RCECB_PARM);                 /**< Specialized Function Pointer */

typedef RCECB_CALLBACK_FNSIG RCECB_CONTEXT DLL_API_WARN_DEPRECATED;              /**< OBSOLETE; CLIENTS USE RCECB_CALLBACK_FNSIG */
typedef RCECB_CALLBACK_FNSIG_P0 RCECB_FNSIG_VOID_VOID DLL_API_WARN_DEPRECATED;   /**< OBSOLETE; CLIENTS USE RCECB_CALLBACK_FNSIG_P0 */

/**
API, Enumeration of Supported Notification Types
*/
typedef enum
{
   RCECB_SIGEX_TYPE_UNKNOWN            = 0x00,
   RCECB_SIGEX_TYPE_SIGNATIVE          = 0x10,
   RCECB_SIGEX_TYPE_CALLBACK_P0        = 0x11,
   RCECB_SIGEX_TYPE_CALLBACK           = RCECB_SIGEX_TYPE_CALLBACK_P0,           /**< Obsolete */
   RCECB_SIGEX_TYPE_CALLBACK_P1        = 0x12,
   RCECB_SIGEX_TYPE_CALLBACK_P2        = 0x13,
   RCECB_SIGEX_TYPE_NONE               = 0xff                                    /**< RCECB [0x10..0x1f] */

} RCECB_SIGEX_TYPE;                                                              /**< Specialized Enum Values DO NOT Overlap RCEVT, RCECB, RCESN */

typedef void* RCECB_SIGEX;                                                       /**< Opaque Handle to Abstracted Event Notification Structures */

#define RCECB_ZERO   0                                                           /**< Manifest Constant Assist */
#define RCECB_NULL   (void*)0                                                    /**< ISO/ANSI-C, Forward reference NULL pointer */

/**
API, Service initialization
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCECB_BOOL rcecb_init(void);

/**
API, Service termination
@param
None
@return
None
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_LOCAL RCECB_BOOL rcecb_term(void);

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
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_create_name(RCECB_NAME const name);

/**
API, Search an event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_BOOL -- Boolean reflecting event previously created
*/
DLL_API_WARN_UNUSED_RESULT  
DLL_API_EXPORT_RCINIT RCECB_BOOL rcecb_search_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig);

/**
API, Search an event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_BOOL -- Boolean reflecting event previously created
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_BOOL rcecb_search_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig);

/**
API, Count of registrants for event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT int rcecb_getregistrants_handle(RCECB_HANDLE const handle);

DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE int rcecb_getcontexts_handle(RCECB_HANDLE const handle) /**< OBSOLETE; CLIENTS USE rcecb_getregistrants_handle */
{
   return rcecb_getregistrants_handle(handle);
}

/**
API, Count of registrants for event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT int rcecb_getregistrants_name(RCECB_NAME const name);

DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE int rcecb_getcontexts_name(RCECB_NAME const name) /**< OBSOLETE; CLIENTS USE rcecb_getregistrants_name */
{
   return rcecb_getregistrants_name(name);
}

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig);

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm0_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig);

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm1_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1);

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm2_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2);

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_GLOBAL RCECB_HANDLE rcecb_register_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig);

DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE RCECB_HANDLE rcecb_register_context_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig) /**< OBSOLETE; CLIENTS USE rcecb_register_name */
{
   return rcecb_register_name(name, fnsig);
}

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm0_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P0 const fnsig);

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm1_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1);

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_register_parm2_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2);

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig);

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm0_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig);

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm1_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1);

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm2_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2);

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig);

DLL_API_WARN_DEPRECATED
DLL_API_STATIC_INLINE RCECB_HANDLE rcecb_unregister_context_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig) /**< OBSOLETE; CLIENTS USE rcecb_unregister_name */
{
   return rcecb_unregister_name(name, fnsig);
}

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm0_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P0 const fnsig);

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm1_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1);

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_unregister_parm2_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2);

/**
API, Signal event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_signal_handle(RCECB_HANDLE const handle);

/**
API, Signal event, with handle and timetick
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Maximum timer value associated to event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_signal_handle_timetick(RCECB_HANDLE const handle, unsigned long* const timetick_p);

/**
API, Signal event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_signal_name(RCECB_NAME const name);

/**
API, Signal event, with name and timetick
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Maximum timer value associated to event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCECB_HANDLE rcecb_signal_name_timetick(RCECB_NAME const name, unsigned long* const timetick_p);

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
