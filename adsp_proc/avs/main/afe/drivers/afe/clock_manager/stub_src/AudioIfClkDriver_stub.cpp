/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/stub_src/AudioIfClkDriver_stub.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AudioIfClkDriver.h"
#include "adsp_afe_service_commands.h"

/*==========================================================================
  Globals
========================================================================== */

/*==========================================================================
  Function implementations
========================================================================== */
ADSPResult afe_audioif_clk_init(uint32_t hw_version)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_audioif_clk_deinit(void)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_set_audioif_clk_cfg(uint16_t port_id, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_set_audioif_reconfig_i2s_clk(uint16_t port_id, uint32_t newSampleRate, uint32_t orgSampleRate)
{
   return ADSP_ENOTIMPL;
}
