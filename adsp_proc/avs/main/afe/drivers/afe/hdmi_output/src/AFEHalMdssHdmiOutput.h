/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalMdssHdmiOutput.h#1 $
 ====================================================================== */
#ifndef _HAL_MDSS_HDMI_OUTPUT_I_H
#define _HAL_MDSS_HDMI_OUTPUT_I_H

/*==========================================================================
  Include files
  ========================================================================== */
#include "typedef_drivers.h" 
#include "lpasshwio_devcfg.h"

/*==========================================================================
  Macro Defines
  ========================================================================== */

typedef enum 
{
   MODE_INVALID = -1,
   MODE_0 = 0,             //single stream via legacy hw, lpass uses LPAIF DMA
   MODE_1,                 //single stream via new hw
   MODE_2,                 //multiple streams via new hw
} mdss_hdmi_audio_mode;

/* The following type used as a generic flag to indicate enable/disable of a module */
typedef enum
{
  DISABLE = 0,
  ENABLE = 1
} hdmi_mod_en_state_t;

/*==========================================================================
  Function Declerations/Inlines
  ========================================================================== */


void hal_mdss_hdmi_aud_init(uint32 version, uint32 phy_base, uint32 virt_base);
void hal_mdss_hdmi_disable_core(void);
void hal_mdss_hdmi_enable_core(void);
void hal_mdss_hdmi_disable_hw_pack(void);
void hal_mdss_hdmi_enable_hw_pack(void);
void hal_mdss_hdmi_disable_packet_ctl(void);
void hal_mdss_hdmi_enable_packet_ctl(void);
void hal_mdss_hdmi_reset_sample_present_bit(void);

void hal_mdss_hdmi_cfg_audio_mode(mdss_hdmi_audio_mode mode);

void hal_mdss_hdmi_start_audio_info_frame(void);
void hal_mdss_hdmi_stop_audio_info_frame(void);

void hal_mdss_hdmi_start_audio_metadata(void);
void hal_mdss_hdmi_update_audio_metadata(void);     //for add and remove sub-stream
void hal_mdss_hdmi_stop_audio_metadata(void);

/**
 * Configures High bit rate (HBR) enable bit, to enable/disable
 * high bit rate for compressed pass-through use case.
 *
 * @param[in] enable flag that indicates to enable or disable HBR
 *
 * @return  None
 *
 * @dependencies
 * None.
 */
void hal_mdss_hdmi_enable_disable_hbr(hdmi_mod_en_state_t enable);

#endif  //_HAL_MDSS_HDMI_OUTPUT_I_H

