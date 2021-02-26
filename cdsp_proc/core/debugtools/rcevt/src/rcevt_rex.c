/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_rex.c
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
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcevt/src/rcevt_rex.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"

#include "rcevt_rex.h"
#include "tms_rcevt.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_ULOG_SWEVT)
#endif

/**
API
*/

/**
The following function signature should not be used. The correct dependency
mechanism is to use RCEVT_REX.H supplied API.
*/
RCEVT_HANDLE rcevt_register_handle(RCEVT_HANDLE const handle, unsigned long signal) /**< Deprecated API, Use RCEVT_REX.H */
{
   RCEVT_SIGEX_SIGREX rcevt_sigex;
   rcevt_sigex.signal = rex_self();
   rcevt_sigex.mask = (rex_sigs_type)signal;
   return rcevt_register_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex);
}

/**
The following function signature should not be used. The correct dependency
mechanism is to use RCEVT_REX.H supplied API.
*/
RCEVT_HANDLE rcevt_register_name(RCEVT_NAME const name, unsigned long signal)     /**< Deprecated API, Use RCEVT_REX.H */
{
   RCEVT_SIGEX_SIGREX rcevt_sigex;
   rcevt_sigex.signal = rex_self();
   rcevt_sigex.mask = (rex_sigs_type)signal;
   return rcevt_register_sigex_name(name, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex);
}

/**
INTERNAL
*/

#endif
