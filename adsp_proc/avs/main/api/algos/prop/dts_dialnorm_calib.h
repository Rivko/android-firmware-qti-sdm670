#ifndef DTS_DIALNORM_CALIB_H
#define DTS_DIALNORM_CALIB_H
/*==============================================================================
  @file dts_dialnorm_calib.h
  @brief This file contains DTS_DIALNORM API
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

/** @h2xml_title1           {Audio DTS Dialogue Normalization (DialNorm)}
    @h2xml_title_agile_rev  {Audio DTS Dialogue Normalization (DialNorm)}
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

/** ID of the DTS Dialogue Normalization (DialNorm) module. */
#define AUDPROC_MODULE_ID_DTS_DIALNORM    0x0001071B
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_DTS_DIALNORM",
                          AUDPROC_MODULE_ID_DTS_DIALNORM}

    @h2xmlm_description  {ID of the DTS DialNorm  module.\n

   This module supports the AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE parameter ID.\n }

    @h2xmlm_toolPolicy              {NO_SUPPORT}
    @h2xmlm_deprecated              {true}

    @{                   <-- Start of the Module -->
*/

/** ID of the DTS DialNorm Enable parameter used by AUDPROC_MODULE_ID_DTS_DIALNORM. */
#define AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE    0x0001071C

/* Structure for the enable parameter of dts_dialnorm module. */
typedef struct audproc_dts_dialnorm_enable_t audproc_dts_dialnorm_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE", AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE}
    @h2xmlp_description {Enables the DTS DialNorm module}
    @h2xmlp_deprecated              {true} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE enable_flag parameter
 used by the DialNorm module.
 */
struct audproc_dts_dialnorm_enable_t
{

   uint32_t enable_flag;
   /**< @h2xmle_description  { Specifies whether DTS DialNorm is enabled. }
         @h2xmle_rangeList   {"Disable"= 0;
                              "Enable"=1}  */

}
#include "adsp_end_pack.h"
;

#endif

/** @}                   <-- End of the Module -->*/
