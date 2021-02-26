#ifndef RCEVT_DAL_H
#define RCEVT_DAL_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_dal.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcevt_dal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_EXPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "DALSys.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// DALSYSEventHandle hEvent;
// RCEVT_SIGEX_SIGDAL client_sigex;
//
// if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL))
// {
// MSG("error"); // handle error
// }
//
// client_sigex.signal = hEvent;
//
// if (RCEVT_NULL == rcevt_register_name_dal(CLIENT_EVENTNAME, hEvent))
// {
// MSG("error"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// if (DAL_SUCCESS != DALSYS_EventWait(hEvent))
// {
// MSG("error"); // handle error
// }
// else
// {
// MSG("event handler");
// }

////////////////////////////////////////
// UNREGISTER
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_unregister_name_dal(CLIENT_EVENTNAME, hEvent))
// {
// MSG("error"); // handle error
// }
//
// if (DAL_SUCCESS != DALSYS_DestroyObject(hEvent))
// {
// MSG("error"); // handle error
// }

////////////////////////////////////////
// SIGNAL FROM OTHER CONTEXT
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_create_name(CLIENT_EVENTNAME)) // Prior to Use
// {
// MSG("error"); // handle error
// }

// if (RCEVT_NULL == rcevt_signal_name(CLIENT_EVENTNAME))
// {
// MSG("error"); // handle error
// }

/**
API, NHLOS Specific Structure
*/
typedef struct
{
   DALSYSEventHandle signal;

} RCEVT_SIGEX_SIGDAL;

/**
API, Register notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_register_handle_dal(RCEVT_HANDLE const handle, DALSYSEventHandle const signal)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if(RCEVT_NULL != handle)
   {
      RCEVT_SIGEX_SIGDAL sigex;
      sigex.signal = signal;
      rc = rcevt_register_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGDAL, &sigex);
   }

   return rc;
}

/**
API, Register notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_register_name_dal(RCEVT_NAME const name, DALSYSEventHandle const signal)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if(RCEVT_NULL != name)
   {
      RCEVT_SIGEX_SIGDAL sigex;
      sigex.signal = signal;
      rc = rcevt_register_sigex_name(name, RCEVT_SIGEX_TYPE_SIGDAL, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_unregister_handle_dal(RCEVT_HANDLE const handle, DALSYSEventHandle const signal)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if(RCEVT_NULL != handle)
   {
      RCEVT_SIGEX_SIGDAL sigex;
      sigex.signal = signal;
      rc = rcevt_unregister_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGDAL, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_unregister_name_dal(RCEVT_NAME const name, DALSYSEventHandle const signal)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if(RCEVT_NULL != name)
   {
      RCEVT_SIGEX_SIGDAL sigex;
      sigex.signal = signal;
      rc = rcevt_unregister_sigex_name(name, RCEVT_SIGEX_TYPE_SIGDAL, &sigex);
   }

   return rc;
}

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
