#ifndef EANS_PARAM_CALIB_H
#define EANS_PARAM_CALIB_H
/*==============================================================================
  @file eans_param_calib.h
  @brief This file contains EANS parameter
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

/** @h2xml_title1           {Enhanced Audio Noise Suppression (EANS) parameter}
    @h2xml_title_agile_rev  {Enhanced Audio Noise Suppression (EANS) parameter}
    @h2xml_title_date       {February 24, 2017} */


#include "mmdefs.h"



/* ID of the EANS Enable parameter used by AUDPROC_MODULE_ID_EANS. */
#define AUDPROC_PARAM_ID_EANS_ENABLE                      0x00010C4B

/* Structure for enabling the configuration parameter for
 spectrum analyzer enable. */
typedef struct audproc_eans_enable_t audproc_eans_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EANS_ENABLE", AUDPROC_PARAM_ID_EANS_ENABLE}
    @h2xmlp_description {Specifies whether EANS is enabled.} */

#include "adsp_begin_pack.h"

struct audproc_eans_enable_t
{

   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether EANS is enabled.\n
        This flag is supported only for sampling rates of 8, 12, 16, 24, 32,
        and 48 kHz. It is not supported for sampling rates of 11.025,
        22.05, or 44.1 kHz.\n}

        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
        @h2xmle_default      {0}  */

}
#include "adsp_end_pack.h"
;

#endif

