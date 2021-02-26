/*========================================================================
   This file contains functions to set the parameters for different modules.

Copyright (c) 2015-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_table_v1_i.h#1 $
 ====================================================================== */
#ifndef _CLOCK_TABLE_V1_I_H_
#define _CLOCK_TABLE_V1_I_H_

#include "Elite.h"
#include "adsp_afe_service_commands.h"
#include "clock_manager_type_i.h"
#include "clock_table_func_i.h"

// This CLK ID is only for internal use and not available in public API
// Upper 16-bit is used to avoid the lower 16-bits are used by public API
#define  AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK (0x20000)

// This CLK ID is only for internal use and not available in public API
// Upper 16-bit is used to avoid the lower 16-bits are used by public API
#define  AFE_CLOCK_SET_CLOCK_ID_INTERNAL_SEC_PCM_OE_CLK (0x20001)

// List of clk-id, clk-name, clk-attr, enable, disable functions
const generic_map_tab_info_t g_generic_clk_map_table_v1[] = 
{
  // Clk-ID, Clk-name, Clk-Attr, enable-func, disable-func
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT, "audio_core_lpaif_codec_spkr_ibit_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_EBIT, "audio_core_lpaif_codec_spkr_ebit_clk",
    afe_clk_set_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_OSR,  "audio_core_lpaif_codec_spkr_osr_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},

  // Primary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT, "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},
  
  // Secondary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "audio_core_lpaif_sec_ibit_clk", 
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT, "audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT, "audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery MI2S
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT,"audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT,"audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Primary PCM
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT,  "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT,  "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},
  
  // Secondary PCM
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT,  "audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT,  "audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary PCM
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT,  "audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT,  "audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery PCM
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT, "audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT, "audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},
    
  // PCM OE CLOCK, it is an internal managed CLK ID, not published
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "audio_core_lpaif_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // MCLK 0
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_1,        "audio_wrapper_ext_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 1
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_2,        "audio_wrapper_ext_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 2
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,        "audio_wrapper_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
   
};

/**< List of clk-id, clk-name, clk-attr, enable, disable 
 *  functions */
const generic_map_tab_info_t g_generic_clk_map_table_v2[] = 
{
  /**< Primary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT, "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Secondary MI2S, shared with Pri MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT, "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Tertiary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT, "audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Quaternary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, "audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT, "audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Quinary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, "audio_core_lpaif_qui_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_EBIT, "audio_core_lpaif_qui_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},
  
  /**< Senary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_IBIT, "audio_core_lpaif_sen_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_EBIT, "audio_core_lpaif_sen_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Primary PCM, shared with Quaternary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, "audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT, "audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  /**< Secondary PCM, shared with Quinary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, "audio_core_lpaif_qui_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT, "audio_core_lpaif_qui_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},
    
  /**< PCM OE CLOCK, internally managed CLK ID, not 
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "audio_core_lpaif_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< MCLK 0 */ 
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_1,  "audio_wrapper_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< MCLK 1 */ 
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_2,  "audio_wrapper_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Internal Digital Codec Core Clock */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_DIGITAL_CODEC_CORE,  "digcodec_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
};


// List of clk-id, clk-name, clk-attr, enable, disable functions
const generic_map_tab_info_t g_generic_clk_map_table_v3[] =
{
  // Clk-ID, Clk-name, enable-func, disable-func
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT, "lpass_audio_core_lpaif_codec_spkr_ibit_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_EBIT, "lpass_audio_core_lpaif_codec_spkr_ebit_clk",
    afe_clk_set_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_OSR,  "lpass_audio_core_lpaif_codec_spkr_osr_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},

  // Primary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "lpass_audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT, "lpass_audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "lpass_audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT, "lpass_audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "lpass_audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT, "lpass_audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery MI2S
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT,"lpass_audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT,"lpass_audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Primary PCM
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT,  "lpass_audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT,  "lpass_audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary PCM
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT,  "lpass_audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT,  "lpass_audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary PCM
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT,  "lpass_audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT,  "lpass_audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery PCM
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT, "lpass_audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT, "lpass_audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // PCM OE CLOCK, it is an internal managed CLK ID, not published
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "lpass_audio_core_lpaif_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // MCLK 0
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_1,        "lpass_audio_wrapper_ext_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 1
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_2,        "lpass_audio_wrapper_ext_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 2
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,        "lpass_audio_wrapper_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
};



/**< List of clk-id, clk-name, clk-attr, enable, disable 
 *  functions */
const generic_map_tab_info_t g_generic_clk_map_table_v4[] = 
{
  /**< Primary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Secondary MI2S, shared with Pri MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Tertiary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "gcc_ultaudio_lpaif_sec_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Quaternary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, "gcc_ultaudio_lpaif_aux_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Primary PCM, shared with Quaternary MI2S */ 
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, "gcc_ultaudio_lpaif_aux_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
    
  /**< PCM OE CLOCK, internally managed CLK ID, not 
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "gcc_ultaudio_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< MCLK 2 */
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,  "gcc_ultaudio_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Internal Digital Codec Core Clock */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_DIGITAL_CODEC_CORE,  "gcc_codec_digcodec_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
};



// List of clk-id, clk-name, clk-attr, enable, disable functions
const generic_map_tab_info_t g_generic_clk_map_table_v5[] =
{
  // Primary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT, "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT, "audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT, "audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery MI2S
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT,"audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT,"audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Primary PCM
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT,  "audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT,  "audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary PCM
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT,  "audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT,  "audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary PCM
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT,  "audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT,  "audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery PCM
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT, "audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT, "audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

   //INT 0 I2S BIT Clock. Both INT0 and INT 1 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT0_I2S_IBIT,  "audio_core_int_i2s0_1_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
   
  //INT 1 I2S BIT Clock. Both INT0 and INT 1 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT1_I2S_IBIT,  "audio_core_int_i2s0_1_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 2 I2S BIT Clock. 
  {AFE_CLOCK_SET_CLOCK_ID_INT2_I2S_IBIT,  "audio_core_int_i2s2_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  //INT 3 I2S BIT Clock. 
  {AFE_CLOCK_SET_CLOCK_ID_INT3_I2S_IBIT,  "audio_core_int_i2s3_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 4 I2S BIT Clock. 
  {AFE_CLOCK_SET_CLOCK_ID_INT4_I2S_IBIT,  "audio_core_int_i2s4_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 5 I2S BIT Clock. 
  {AFE_CLOCK_SET_CLOCK_ID_INT5_I2S_IBIT,  "audio_core_int_i2s5_6_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 6 I2S BIT Clock. 
  {AFE_CLOCK_SET_CLOCK_ID_INT6_I2S_IBIT,  "audio_core_int_i2s5_6_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},


  // PCM OE CLOCK, it is an internal managed CLK ID, not published
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "audio_core_lpaif_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // MCLK 0
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_1,        "audio_wrapper_ext_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 1
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_2,        "audio_wrapper_ext_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 2
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,        "audio_wrapper_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 4
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_4,        "audio_wrapper_ext_mclk3_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // INT MCLK 0  this is digital codec clock
  {AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_0,        "audio_core_int_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  //INT MCLK1   this is soundwire clock
  {AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_1,        "audio_core_int_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // this is soundwire NPL  clock
  {AFE_CLOCK_SET_CLOCK_ID_SWR_NPL_CLK,        "audio_wrapper_sw_npl_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},


};

/**< List of clk-id, clk-name, clk-attr, enable, disable
 *  functions */
const generic_map_tab_info_t g_generic_clk_map_table_v6[] =
{
  /**< Primary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Secondary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "gcc_ultaudio_lpaif_sec_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Primary PCM, shared with primary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Secondary PCM, shared with Secondary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, "gcc_ultaudio_lpaif_sec_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< PCM OE CLOCK, internally managed CLK ID, not
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "gcc_ultaudio_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< SEC PCM OE CLOCK, internally managed CLK ID, not
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_SEC_PCM_OE_CLK, "gcc_ultaudio_sec_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< MCLK 2 */
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,  "gcc_ultaudio_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
};

/**< List of clk-id, clk-name, clk-attr, enable, disable
 *  functions */
const generic_map_tab_info_t g_generic_clk_map_table_v7[] =
{
  /**< Primary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Secondary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "gcc_ultaudio_lpaif_sec_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Primary PCM, shared with primary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, "gcc_ultaudio_lpaif_pri_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< Secondary PCM, shared with Secondary MI2S */
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, "gcc_ultaudio_lpaif_sec_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< PCM OE CLOCK, internally managed CLK ID, not
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "gcc_ultaudio_lpaif_pcm_dataoe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< SEC PCM OE CLOCK, internally managed CLK ID, not
   *  published */
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_SEC_PCM_OE_CLK, "gcc_ultaudio_lpaif_aux_pcm_dataoe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  /**< MCLK 2 */
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,  "gcc_ultaudio_lpaif_ext_i2s_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
};

// List of clk-id, clk-name, enable, disable functions
const generic_map_tab_info_t g_generic_clk_map_table_v8[] =
{
  // Primary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, "lpass_audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT, "lpass_audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, "lpass_audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT, "lpass_audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary MI2S
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, "lpass_audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT, "lpass_audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quantery MI2S
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT,"lpass_audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT,"lpass_audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quinary MI2S. For warlock, SPKR i2s updated as bi-directional and named as QUi mi2s.
  {AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, "lpass_audio_core_lpaif_codec_spkr_ibit_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_EBIT, "lpass_audio_core_lpaif_codec_spkr_ebit_clk",
    afe_clk_set_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_OSR,  "lpass_audio_core_lpaif_codec_spkr_osr_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},

  // Primary PCM
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT,  "lpass_audio_core_lpaif_pri_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT,  "lpass_audio_core_lpaif_pri_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Secondary PCM
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT,  "lpass_audio_core_lpaif_sec_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT,  "lpass_audio_core_lpaif_sec_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Tertiary PCM
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT,  "lpass_audio_core_lpaif_ter_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT,  "lpass_audio_core_lpaif_ter_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quaternary PCM
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT, "lpass_audio_core_lpaif_quad_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT, "lpass_audio_core_lpaif_quad_ebit_clk",
    afe_clk_set_enable,  afe_clk_set_disable},

  // Quinary PCM. For warlock, SPKR i2s updated as bi-directional and named as QUi mi2s.
  {AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_IBIT, "lpass_audio_core_lpaif_codec_spkr_ibit_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_EBIT, "lpass_audio_core_lpaif_codec_spkr_ebit_clk",
    afe_clk_set_enable, afe_clk_set_disable},
  {AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_OSR,  "lpass_audio_core_lpaif_codec_spkr_osr_clk",
    afe_clk_set_freq_divider_enable, afe_clk_set_disable},

   //INT 0 I2S BIT Clock. Both INT0 and INT 1 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT0_I2S_IBIT,  "audio_core_int_i2s0_1_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  //INT 1 I2S BIT Clock. Both INT0 and INT 1 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT1_I2S_IBIT,  "audio_core_int_i2s0_1_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 2 I2S BIT Clock.
  {AFE_CLOCK_SET_CLOCK_ID_INT2_I2S_IBIT,  "audio_core_int_i2s2_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  //INT 3 I2S BIT Clock.
  {AFE_CLOCK_SET_CLOCK_ID_INT3_I2S_IBIT,  "audio_core_int_i2s3_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 4 I2S BIT Clock.
  {AFE_CLOCK_SET_CLOCK_ID_INT4_I2S_IBIT,  "audio_core_int_i2s4_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 5 I2S BIT Clock. Both INT5 and INT6 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT5_I2S_IBIT,  "audio_core_int_i2s5_6_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

   //INT 6 I2S BIT Clock. Both INT5 and INT6 are using same clock
  {AFE_CLOCK_SET_CLOCK_ID_INT6_I2S_IBIT,  "audio_core_int_i2s5_6_ibit_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},


  // PCM OE CLOCK, it is an internal managed CLK ID, not published
  {AFE_CLOCK_SET_CLOCK_ID_INTERNAL_PCM_OE_CLK, "lpass_audio_core_lpaif_pcm_data_oe_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // MCLK 0
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_1,        "lpass_audio_core_ext_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 1
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_2,        "lpass_audio_core_ext_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // MCLK 2
  {AFE_CLOCK_SET_CLOCK_ID_MCLK_3,        "lpass_audio_core_ext_mclk2_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},

  // INT MCLK 0  this is digital codec clock
  {AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_0,        "lpass_audio_core_int_mclk0_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  //INT MCLK1   this is soundwire clock
  {AFE_CLOCK_SET_CLOCK_ID_INT_MCLK_1,        "lpass_audio_core_int_mclk1_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable},
  // this is soundwire NPL  clock
  {AFE_CLOCK_SET_CLOCK_ID_SWR_NPL_CLK,        "lpass_audio_core_sw_npl_clk",
    afe_clk_set_freq_divider_enable,  afe_clk_set_disable}
};

#endif
