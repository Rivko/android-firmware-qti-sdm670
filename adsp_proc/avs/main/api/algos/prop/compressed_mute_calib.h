#ifndef COMPRESSED_MUTE_CALIB_H
#define COMPRESSED_MUTE_CALIB_H
/*==============================================================================
  @file compressed_mute_calib.h
  @brief This file contains COMPRESSED_MUTE API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {Audio Compressed Mute}
    @h2xml_title_agile_rev  {Audio Compressed Mute}
    @h2xml_title_date       {February 9, 2017} */

#include  "mmdefs.h"


/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define AUDPROC_MODULE_ID_COMPRESSED_MUTE    0x00010770



/** ID of the Compressed Mute parameter used by AUDPROC_MODULE_ID_COMPRESSED_MUTE. */
#define AUDPROC_PARAM_ID_COMPRESSED_MUTE     0x00010771

/* Structure for Compressed Mute parameter muteFlag. */
typedef struct audproc_compressed_mute_param_t audproc_compressed_mute_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_COMPRESSED_MUTE", AUDPROC_PARAM_ID_COMPRESSED_MUTE}
    @h2xmlp_description {Specifies whether compressed mute is enabled.}
    @h2xmlp_toolPolicy  {NO_SUPPORT}  */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_COMPRESSED_MUTE muteFlag parameter used by
 AUDPROC_MODULE_ID_COMPRESSED_MUTE.
 */
struct audproc_compressed_mute_param_t
{
   uint32_t mute_flag;
   /**< @h2xmle_description  {Specifies whether compressed mute is enabled. }
         @h2xmle_range   {0..4294967295}
         @h2xmle_default {0} */


}
#include "adsp_end_pack.h"
;





/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_COMPRESSED_MUTE",
                          AUDPROC_MODULE_ID_COMPRESSED_MUTE}
	@h2xmlm_displayName  {"AUDIO COMPRESSED MUTE"}

    @h2xmlm_description  {ID of the Compressed Mute.\n

   This module supports the AUDPROC_PARAM_ID_COMPRESSED_MUTE parameter ID.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
*/

/**
  @h2xml_Select         {audproc_compressed_mute_param_t}
   @h2xmlm_InsertParameter
*/

/** @}                   <-- End of the Module -->*/

#endif

