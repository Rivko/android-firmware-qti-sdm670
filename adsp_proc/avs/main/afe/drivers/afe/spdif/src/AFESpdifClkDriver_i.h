/*========================================================================
This file contains function header for the AFE Spdif clock config

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifClkDriver_i.h#1 $
 ====================================================================== */

#ifndef _AFE_SPDIF_CLK_DRIVER_I_H_
#define _AFE_SPDIF_CLK_DRIVER_I_H_


#if defined(__cplusplus)
extern "C" {
#endif


ADSPResult afe_spdif_init_clks(void);

ADSPResult afe_spdif_deinit_clks(void);

ADSPResult afe_spdif_set_clks(uint16 clk_root, uint32 clk_value);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // !defined(_AFE_SPDIF_CLK_DRIVER_I_H_)


