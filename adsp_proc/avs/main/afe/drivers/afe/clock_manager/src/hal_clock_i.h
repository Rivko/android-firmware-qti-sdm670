/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2017 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/hal_clock_i.h#1 $
 ====================================================================== */

#ifndef _HAL_CLOCK_I_H_
#define _HAL_CLOCK_I_H_

// Typedef: Pointer to function that locate a pair of clk-id(s) given a clk-id
typedef ADSPResult (*hal_clk_loc_clkid_pair_func)(uint32_t clkid, uint32_t *dividend_clkid_ptr, 
  uint32_t *divisor_clkid_ptr);
// Typedef: Pointer to function that set a clock related MUX
typedef void (*hal_clk_mux_sel_func)(uint32_t clkid, uint32 virbase);

// V1/V2 implementation of the above
void HAL_clock_mux_sel_default(uint32_t virbase);	//makes the calls necessary to set the default bitclocks for the chipset
ADSPResult HAL_clock_locate_clkid_pair(uint32_t clkid, 
  uint32_t *dividend_clkid_ptr, uint32_t *divisor_clkid_ptr);
void HAL_clock_mux_sel(uint32_t clkid, uint32_t virbase);


#endif
