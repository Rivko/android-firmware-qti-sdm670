#ifndef RCESN_POSIX_H
#define RCESN_POSIX_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcesn_posix.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcesn_posix.h#1 $
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
#include "rcesn.h"

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
// if (RCESN_NULL == rcesn_register_name_posix(CLIENT_EVENTNAME, compare, &client_cond, &client_mutex))
// {
// MSG("error"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// pthread_mutex_lock(&client_mutex);
//
// pthread_cond_wait(&client_cond, &client_mutex);
//
// pthread_mutex_unlock(&client_mutex);
//
// MSG("event handler");

////////////////////////////////////////
// UNREGISTER
////////////////////////////////////////

// if (RCESN_NULL == rcesn_unregister_name_posix(CLIENT_EVENTNAME, compare, &client_cond, &client_mutex))
// {
// MSG("error"); // handle error
// }
//
// pthread_cond_destroy(&client_cond);
//
// pthread_mutex_destroy(&client_mutex);

////////////////////////////////////////
// SIGNAL FROM OTHER CONTEXT
////////////////////////////////////////

// if (RCESN_NULL == rcesn_create_name(CLIENT_EVENTNAME)) // Prior to Use
// {
// MSG("error"); // handle error
// }

// if (RCESN_NULL == rcesn_signal_name(CLIENT_EVENTNAME))
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

   RCESN_FNSIG_COMPARE compare;

} RCESN_SIGEX_SIGPOSIX;

/**
API, Register notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Function pointer to Comparision Function
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCESN_HANDLE rcesn_register_handle_posix(RCESN_HANDLE const handle, RCESN_FNSIG_COMPARE const compare, pthread_cond_t* const cond_p, pthread_mutex_t* const mutex_p)
{
   RCESN_HANDLE rc = RCESN_NULL;

   if (RCESN_NULL != handle && RCESN_NULL != compare && RCESN_NULL != cond_p && RCESN_NULL != mutex_p)
   {
      RCESN_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      sigex.compare = compare;

      rc = rcesn_register_sigex_handle(handle, RCESN_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Register notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Function pointer to Comparision Function
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCESN_HANDLE rcesn_register_name_posix(RCESN_NAME const name, RCESN_FNSIG_COMPARE compare, pthread_cond_t* cond_p, pthread_mutex_t* mutex_p)
{
   RCESN_HANDLE rc = RCESN_NULL;

   if (RCESN_NULL != name && RCESN_NULL != compare && RCESN_NULL != cond_p && RCESN_NULL != mutex_p)
   {
      RCESN_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      sigex.compare = compare;
      rc = rcesn_register_sigex_name(name, RCESN_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with handle
@param[in] Opaque handle of the event
@param[in] Function pointer to Comparision Function
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCESN_HANDLE rcesn_unregister_handle_posix(RCESN_HANDLE const handle, RCESN_FNSIG_COMPARE compare, pthread_cond_t* cond_p, pthread_mutex_t* mutex_p)
{
   RCESN_HANDLE rc = RCESN_NULL;

   if (RCESN_NULL != handle && RCESN_NULL != compare && RCESN_NULL != cond_p && RCESN_NULL != mutex_p)
   {
      RCESN_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      sigex.compare = compare;
      rc = rcesn_unregister_sigex_handle(handle, RCESN_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/**
API, Unegister notification to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Function pointer to Comparision Function
@param[in] Notification NHLOS specific notification information
@param[in] Notification NHLOS specific notification information
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCESN_HANDLE rcesn_unregister_name_posix(RCESN_NAME const name, RCESN_FNSIG_COMPARE compare, pthread_cond_t* cond_p, pthread_mutex_t* mutex_p)
{
   RCESN_HANDLE rc = RCESN_NULL;

   if (RCESN_NULL != name && RCESN_NULL != compare && RCESN_NULL != cond_p && RCESN_NULL != mutex_p)
   {
      RCESN_SIGEX_SIGPOSIX sigex;
      sigex.cond_p = cond_p;
      sigex.mutex_p = mutex_p;
      sigex.compare = compare;
      rc = rcesn_unregister_sigex_name(name, RCESN_SIGEX_TYPE_SIGPOSIX, &sigex);
   }

   return rc;
}

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
