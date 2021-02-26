/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/stub_src/clock_manager_stub.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "clock_manager.h"
#include "adsp_afe_service_commands.h"

/*==========================================================================
  Globals
========================================================================== */

/*==========================================================================
  Function implementations
========================================================================== */
ADSPResult afe_clock_manager_set_clk(uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_clock_manager_reconfig_i2s_clk(uint32 clkid, int32 newSampleRate, int32 orgSampleRate)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_clock_manager_deinit(void)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_clock_manager_init(void)
{
   return ADSP_ENOTIMPL;
}

