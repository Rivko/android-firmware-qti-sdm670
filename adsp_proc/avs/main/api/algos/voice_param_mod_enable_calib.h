#ifndef VOICE_PARAM_MOD_ENABLE_CALIB_H
#define VOICE_PARAM_MOD_ENABLE_CALIB_H
/*==============================================================================
  @file voice_param_mod_enable_calib.h
  @brief This file contains PARAM_MOD_ENABLE API
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who      what, where, why
 --------   ---      -------------------------------------------------------
 03/28/17   shrani   Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"

#define VOICE_PARAM_MOD_ENABLE 0x00010E00

#include "adsp_begin_pack.h"

typedef struct voice_param_mod_enable_t voice_param_mod_enable_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MOD_ENABLE", VOICE_PARAM_MOD_ENABLE}
    @h2xmlp_description {Enables or disables a module.} */

struct voice_param_mod_enable_t {

    uint16_t Enable;
    /**< @h2xmle_description {Enables/disables this feature}
         @h2xmle_rangeList        {"Disable"= 0;
                                   "Enable"=1}
         @h2xmle_default     {0}

         @h2xmle_policy       {Basic} */

    uint16_t Reserved;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..0xFFFF}
         @h2xmle_readOnly     {true}
         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;

#endif
