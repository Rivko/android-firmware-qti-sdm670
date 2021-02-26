/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2014 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_table_func_i.h#1 $
 ====================================================================== */
#ifndef _CLOCK_TABLE_FUNC_I_H_
#define _CLOCK_TABLE_FUNC_I_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "clock_manager_type_i.h"


//Part set function - enable for I2S osr and PCM clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_EnableClock
static DALResult afe_clk_set_freq_divider_enable(DalDeviceHandle* handle_ptr, uint32_t divider,
  generic_clk_cfg_info_t* clk_cfg_ptr);

//Part set function - enable for all external clks, include
//1. DalClock_EnableClock
static DALResult afe_clk_set_enable(DalDeviceHandle* handle_ptr, uint32_t divider,
  generic_clk_cfg_info_t* clk_cfg_ptr);

//Part set function disable for all clks, include
//1. DalClock_DisableClock
static DALResult afe_clk_set_disable(DalDeviceHandle* handle_ptr, uint32_t divider,
  generic_clk_cfg_info_t* clk_cfg_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
  
#endif