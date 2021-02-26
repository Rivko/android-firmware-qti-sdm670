/*========================================================================
  This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2014 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/inc/AudioIfClkDriver.h#1 $
 ====================================================================== */

#ifndef _AUDIOIF_CLK_DRIVER_H_
#define _AUDIOIF_CLK_DRIVER_H_

#include "Elite.h"

/*==========================================================================
  Include Files
========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

ADSPResult afe_audioif_clk_init(uint32_t hw_version);
ADSPResult afe_audioif_clk_deinit(void);
ADSPResult afe_set_audioif_clk_cfg(uint16_t port_id, int8_t *cfg, uint32_t payload_size);
ADSPResult afe_set_audioif_reconfig_i2s_clk(uint16_t port_id, uint32_t newSampleRate, uint32_t orgSampleRate);
ADSPResult afe_set_digital_codec_coreclk_cfg (uint16_t port_id, int8_t *cfg, uint32_t payload_size);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif
