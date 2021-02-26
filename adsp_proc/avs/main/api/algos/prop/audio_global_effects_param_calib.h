#ifndef AUDIO_GLOBAL_EFFECTS_PARAM_CALIB_H
#define AUDIO_GLOBAL_EFFECTS_PARAM_CALIB_H

/*==============================================================================
  @file audio_global_effects_param_calib.h
  @brief This file contains audio global effects parameters
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 07/07/17   rishabht  Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/**
    Param ID to support dynamic enablement/disablement of Audio effects.
    This PID will be common for all audio effects and will be shared accross multiple module IDs.
    HLOS will use regular set/get param interface to set/get effects to/from DSP. This PID will not be visible in QACT.
    Each module will generally use 0th bit to set effect. If a module supports more than one effect than subsequent LSB bits can be used.

    payload size -- 4 bytes
*/
#define GLOBAL_EFFECT_PARAM_ENABLE                                      0x00010EAF

/* Structure for setting/Querying  Audio module effects. */
typedef struct audproc_effects_payload_t audproc_effects_payload_t;
/** @h2xmlp_parameter   {"GLOBAL_EFFECT_PARAM_ENABLE", GLOBAL_EFFECT_PARAM_ENABLE}
    @h2xmlp_description {To dynamically enable/disable Audio effects.}
    @h2xmlp_toolPolicy  {NO_SUPPORT}
    @h2xmlp_readOnly    {false}*/

#include "adsp_begin_pack.h"

/* Payload of the GLOBAL_EFFECT_PARAM_ENABLE parameter used by
 any Audio Processing module
 */
struct audproc_effects_payload_t
{
   uint32_t global_effect;
   /**< @h2xmle_description  {effect set to an Audio module} */
}
#include "adsp_end_pack.h"
;
#endif  // AUDIO_GLOBAL_EFFECTS_PARAM_CALIB_H


