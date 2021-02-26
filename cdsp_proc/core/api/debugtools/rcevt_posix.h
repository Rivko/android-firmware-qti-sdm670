#ifndef RCEVT_POSIX_H
#define RCEVT_POSIX_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_posix.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcevt_posix.h#1 $
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

#include "pthread.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// pthread_mutex_t client_mutex;
// pthread_cond_t client_cond;
//
// pthread_mutex_init(&client_mutex, NULL);
// pthread_cond_init(&client_cond, NULL);
//
// if (RCEVT_NULL == rcevt_register_name_posix(CLIENT_EVENTNAME, &client_cond, &client_mutex))
// {
// MSG("error"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// pthread_mutex_lock(&client_mutex);
// pthread_cond_wait(&client_cond, &client_mutex);
// pthread_mutex_unlock(&client_mutex);
//
// MSG("event handler");

////////////////////////////////////////
// UNREGISTER
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_unregister_name_posix(CLIENT_EVENTNAME, &client_cond, &client_mutex))
// {
// MSG("error"); // handle error
// }
//
// pthread_cond_destroy(&client_cond);
// pthread_mutex_destroy(&client_mutex);

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
   pthread_mutex_t* mutex_p;

   pthread_cond_t* cond_p;

} RCEVT_SIGEX_SIGPOSIX;

/**
API, Register notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_register_handle_posix(RCEVT_HANDLE const handle, pthread_cond_t* const cond_p, pthread_mutex_t* const mutex_p)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if (RCEVT_NULL != handle && RCEVT_NULL != cond_p && RCEVT_NULL != mutex_p)
   {
      RCEVT_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      rc = rcevt_register_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Register notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_register_name_posix(RCEVT_NAME const name, pthread_cond_t* const cond_p, pthread_mutex_t* const mutex_p)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if (RCEVT_NULL != name && RCEVT_NULL != cond_p && RCEVT_NULL != mutex_p)
   {
      RCEVT_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      rc = rcevt_register_sigex_name(name, RCEVT_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_unregister_handle_posix(RCEVT_HANDLE const handle, pthread_cond_t* const cond_p, pthread_mutex_t* const mutex_p)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if (RCEVT_NULL != handle && RCEVT_NULL != cond_p && RCEVT_NULL != mutex_p)
   {
      RCEVT_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      rc = rcevt_unregister_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCEVT_HANDLE rcevt_unregister_name_posix(RCEVT_NAME const name, pthread_cond_t* const cond_p, pthread_mutex_t* const mutex_p)
{
   RCEVT_HANDLE rc = RCEVT_NULL;

   if (RCEVT_NULL != name && RCEVT_NULL != cond_p && RCEVT_NULL != mutex_p)
   {
      RCEVT_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      rc = rcevt_unregister_sigex_name(name, RCEVT_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
