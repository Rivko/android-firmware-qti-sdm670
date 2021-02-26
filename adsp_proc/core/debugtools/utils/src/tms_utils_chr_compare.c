/** vi: tw=128 ts=3 sw=3 et
@file tms_utils_chr_compare.c
@brief This file contains the API for the TMS Utilities, API 0.1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/utils/src/tms_utils_chr_compare.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(TMS_UTILS_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(TMS_UTILS_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "tms_utils_tracer_swe.h"
#endif

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "stdint.h"
#include "tms_utils.h"

int32_t tms_utils_chr_compare(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz)
{
   while (in_buf_sz && in_buf_p && in_buf2_sz && in_buf2_p)
   {
      if (*in_buf_p != *in_buf2_p)
      {
         return *in_buf_p - *in_buf2_p;
      }

      in_buf_sz--, in_buf_p++;

      in_buf2_sz--, in_buf2_p++;
   }

   return 0;
}
