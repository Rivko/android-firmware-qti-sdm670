#ifndef DTS_DRC_CALIB_H
#define DTS_DRC_CALIB_H
/*==============================================================================
  @file dts_drc_calib.h
  @brief This file contains DTS_DRC API
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

/** @h2xml_title1           {DTS_DRC}
    @h2xml_title_agile_rev  {DTS_DRC}
    @h2xml_title_date       {February 8, 2017} */
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
#define AUDPROC_MODULE_ID_DTS_DRC    0x0001071D
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_DTS_DRC",
                          AUDPROC_MODULE_ID_DTS_DRC}
    @h2xmlm_description  {ID of the DTS_DRC module.\n

    This module supports the AUDPROC_PARAM_ID_DTS_DRC_RATIO parameter ID.\n}

     @h2xmlm_toolPolicy              {NO_SUPPORT}
     @h2xmlm_deprecated              {true}

    @{                   <-- Start of the Module -->
*/

/** ID of the DTS DRC Ratio parameter used by AUDPROC_MODULE_ID_DTS_DRC. */
#define AUDPROC_PARAM_ID_DTS_DRC_RATIO    0x0001071E

/* Structure for the DTS DRC Ratio parameter DRC module. */
typedef struct audproc_dts_drc_param_t audproc_dts_drc_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DTS_DRC_RATIO", AUDPROC_PARAM_ID_DTS_DRC_RATIO}
    @h2xmlp_description {Specifies the DTS DRC_ratio used by the DRC.\n }
    @h2xmlp_deprecated              {true} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_DTS_DRC_RATIO parameter used by the DRC module. */

struct audproc_dts_drc_param_t
{

   uint32_t drc_ratio;
    /**< @h2xmle_description  { Specifies the DTS DRC_ratio used by the DRC.\n
     If this value is greater than 4, the DRC is enabled. Otherwise, it is bypassed.\n}
        @h2xmle_range    {0..100}  */

}
#include "adsp_end_pack.h"
;

#endif

/** @}                   <-- End of the Module -->*/


























