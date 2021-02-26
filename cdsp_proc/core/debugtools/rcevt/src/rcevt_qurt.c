/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_qurt.c
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcevt/src/rcevt_qurt.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"

#include "rcevt_qurt.h"
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

#if 0

/**
INTERNAL
*/
void rce_mutex_init_qurt(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      secure_memset((void*)mutex_p, 0, sizeof(*mutex_p));

      qurt_pimutex_init(&mutex_p->mutex_qurt);
   }
}

/**
INTERNAL
*/
void rce_mutex_lock_qurt(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      qurt_pimutex_lock(&mutex_p->mutex_qurt);
   }
}

/**
INTERNAL
*/
void rce_mutex_unlock_qurt(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      qurt_pimutex_unlock(&mutex_p->mutex_qurt);
   }
}

#endif

#endif
