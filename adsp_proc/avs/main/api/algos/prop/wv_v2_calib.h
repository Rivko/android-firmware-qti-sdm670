#ifndef WV_V2_CALIB_H
#define WV_V2_CALIB_H
/*==============================================================================
  @file wv_v2_calib.h
  @brief This file contains WV_V2
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/28/17   shrani  Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_WV_V2}
    @h2xml_title_agile_rev  {VOICE_MODULE_WV_V2}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_WV_V2 0x00010E42

#include "adsp_begin_pack.h"

typedef struct voice_param_wv_v2_t voice_param_wv_v2_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_WV_V2", VOICE_PARAM_WV_V2}
    @h2xmlp_description {Wide Voice version 2 algorithm in the voice decoder postprocessor. \n }
    @h2xmlp_version     {0}*/

struct voice_param_wv_v2_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2}
         @h2xmle_isVersion    {true}

         @h2xmle_readOnly      {true} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly      {true} */

    int32_t big_noise_adjust_thld_low;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x03E8}
         @h2xmle_range       {0..6628}

         @h2xmle_policy       {Basic} */

    int32_t big_noise_adjust_thld_high;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4A38}
         @h2xmle_range       {0..52427}

         @h2xmle_policy       {Basic} */

    int32_t bg_noise_pwr_update_thld;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x9C40}
         @h2xmle_range       {0..93230}

         @h2xmle_policy       {Basic} */

    int32_t hb_syn_noise_gate_thld;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0640}
         @h2xmle_range       {0..5455}

         @h2xmle_policy       {Basic} */

    int32_t relax_uv_rough_thld;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0DAC}
         @h2xmle_range       {0..98304}

         @h2xmle_policy       {Basic} */

    int16_t fixed_bass_boost;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..7}

         @h2xmle_policy       {Basic} */

    int16_t adaptive_bass_boost_voiced;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..1}

         @h2xmle_policy       {Basic} */

    int16_t low_band_contrib;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {16384..19660}

         @h2xmle_policy       {Basic} */

    int16_t high_band_contrib;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x39E8}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_voiced;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2D4E}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_voiced_end;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2D4E}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_transition;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2D4E}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_unvoiced_beg;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2012}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_unvoiced;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2012}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scal_fact_exc_non_sibilant;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2012}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gain_floor_at_big_noise;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t noise_mix_factor_alpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0325}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t noise_mix_factor_beta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t noise_shape_smooth_fact;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0CCD}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    uint16_t zero_cross_thld;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x28}
         @h2xmle_range       {0..160}

         @h2xmle_policy       {Basic} */

    int16_t bg_noise_forget_fact;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0147}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t mask_gain_forget_fact;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4DF8}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t hb_syn_gain_floor;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t hb_syn_mask_thld;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this fiels to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly      {true} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_WV_V2  0x00010F1A
/**
    @h2xmlm_module       {"VOICE_MODULE_WV_V2",
                          VOICE_MODULE_WV_V2}
    @h2xmlm_displayName  {"RX VOICE WV V2"}
	@h2xmlm_description {Wide Voice version 2 algorithm in the voice decoder postprocessor.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_WV_V2\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}

    @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter
   @h2xml_Select       {voice_param_mod_enable_t::Enable}
   @h2xmle_readOnly      {true}

   @h2xml_Select					{voice_param_wv_v2_t}
   @h2xmlm_InsertParameter





   @}                   <-- End of the Module -->*/
#endif
