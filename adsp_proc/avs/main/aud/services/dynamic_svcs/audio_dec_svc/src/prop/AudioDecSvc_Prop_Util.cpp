/*========================================================================

 *//** @file AudioDecSvc_Prop_Utils.cpp
This file contains functions for Elite Decoder service.

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/28/16   HB		Create file for proprietary information.
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_CapiV2Util.h"



/* =======================================================================
FUNCTION DEFINITION
========================================================================== */

//Function to determine the output media format of converter.
ADSPResult AudioDecSvc_DetermineConverterOutputFormat(uint32_t media_fmt_id, uint32_t dec_conv_mode, uint32_t *output_media_fmt)
{
   *output_media_fmt = 0;

   switch (media_fmt_id)
   {
	   case ASM_MEDIA_FMT_EAC3:
	   {
		  if (dec_conv_mode == ASM_DDP_DD_CONVERTER_MODE)
		  {
			 *output_media_fmt = ASM_MEDIA_FMT_AC3;
		  }
		  else
		  {
			 return ADSP_EUNSUPPORTED;
		  }
		  break;
	   }
	   default:
		  return ADSP_EUNSUPPORTED;
   }
   return ADSP_EOK;
}


