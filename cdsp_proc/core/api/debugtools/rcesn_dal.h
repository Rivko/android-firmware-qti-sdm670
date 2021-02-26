#ifndef RCESN_DAL_H
#define RCESN_DAL_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcesn_dal.h
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
$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/rcesn_dal.h#1 $
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
#include "rcesn.h"
#include "rcevt_dal.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// DALSYSEventHandle hEvent;
//
// RCESN_SIGEX_SIGDAL client_sigex;
//
// if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL))
// {
// MSG("error"); // handle error
// }
//
// client_sigex.signal = hEvent;
//
// client_sigex.compare = compare;
//
// if (RCESN_NULL == rcesn_register_sigex_name(CLIENT_EVENTNAME, RCESN_SIGEX_TYPE_SIGDAL, &client_sigex))
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

// if (RCESN_NULL == rcesn_unregister_sigex_name(CLIENT_EVENTNAME, RCESN_SIGEX_TYPE_SIGDAL, &client_sigex))
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
   DALSYSEventHandle signal;

   RCESN_FNSIG_COMPARE compare;

} RCESN_SIGEX_SIGDAL;

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
