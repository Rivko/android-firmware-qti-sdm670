#ifndef AUDIO_COMMON_ENABLE_CALIB_H
#define AUDIO_COMMON_ENABLE_CALIB_H

/*==============================================================================
  @file audio_common_enable_calib.h
  @brief This file contains common parameters
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

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/
/**
    ID of the Enable parameter used by any audio processing module.

    This generic/common parameter is used to configure or determine the
    state of any audio processing module.

 */
#define AUDPROC_PARAM_ID_ENABLE                                   0x00010904

/* Structure for Enable parameter for any Audio processing modules. */
typedef struct audproc_enable_param_t audproc_enable_param_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_ENABLE", AUDPROC_PARAM_ID_ENABLE}
    @h2xmlp_description {Structure for Enable parameter for any Audio processing modules.}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_ENABLE parameter used by
 any Audio Processing module
 */
struct audproc_enable_param_t
{
   uint32_t enable;
   /**< @h2xmle_description  {Specifies whether the audio processing module is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
        @h2xmle_default      {0}  */

}
#include "adsp_end_pack.h"
;
#endif
