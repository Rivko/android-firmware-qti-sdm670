/*========================================================================
  This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2014 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_debug_i.h#1 $
 ====================================================================== */
#ifndef _CLOCK_DEBUG_I_H_
#define _CLOCK_DEBUG_I_H_
#include "clock_manager_type_i.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// Uncomment out the following to debug MUX select
// internal or external clk
//#define DEBUG_DUMP_MUX_CLK

// Uncomment out the following to have full debug print
// 1) It will print out the map, cfg after initiailzation
// 2) After every enable/disable call, it will print cfg.
//#define DEBUG_GENERIC_CLK

#ifdef DEBUG_GENERIC_CLK
void afe_print_cfg_nodes(generic_clk_cfg_global_info_t *
  afe_generic_clk_cfg_global_ctx_ptr);
void afe_print_map(generic_clk_cfg_global_info_t *
  afe_generic_clk_cfg_global_ctx_ptr);
void afe_print_dev_cfg(generic_clk_cfg_global_info_t *
  afe_generic_clk_cfg_global_ctx_ptr);
void HAL_clock_dump_mux_v1(uint32_t virbase);
#endif


#ifdef __cplusplus
}
#endif //__cplusplus

#endif