/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_dalcfg.c
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
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcinit/src/rcinit_dalcfg.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_DALCFG)

#define DLL_API_BUILD_RCINIT

#include "err.h"
#include "stdint.h"
#include "stringl/stringl.h"

#include "DALSysTypes.h"
#include "DALPropDef.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#include "rcinit_dal.h"
#include "tms_rcinit.h"

// DALCFG Image Payload May Contain Specific Initialization

void rcinit_internal_devcfg_check_load(void)
{
   DALSYSPropertyVar propValue;
   DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

   if (DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr("tms_rcinit", pHandle))
   {
      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_err_fatal_enable", 0, &propValue))
      {
         rcinit_term_err_fatal_set(propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_latency_enable", 0, &propValue))
      {
         rcinit_term_latency_enable_set(propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout", 0, &propValue))
      {
         rcinit_term_timeout_set(propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_0", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_0, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_1", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_1, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_2", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_2, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_3", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_3, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_4", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_4, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_5", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_5, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_6", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_6, propValue.Val.dwVal);
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle, "rcinit_term_timeout_group_7", 0, &propValue))
      {
         rcinit_term_timeout_group_set(RCINIT_GROUP_7, propValue.Val.dwVal);
      }
   }
}

#endif
