/*========================================================================
  This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2018 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/inc/clock_manager.h#2 $
 ====================================================================== */

#ifndef _CLOCK_MANAGER_H_
#define _CLOCK_MANAGER_H_

#include "Elite.h"
#include "DALSys.h"

/*==========================================================================
  Include Files
========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

ADSPResult afe_clock_manager_init(void);
ADSPResult afe_clock_manager_deinit(void);
ADSPResult afe_clock_manager_set_clk(uint32_t param_id, int8_t *param_buffer_prt, uint16_t  param_size);
ADSPResult afe_clock_manager_reconfig_i2s_clk(uint32_t clkid, uint32_t newSampleRate, uint32_t orgSampleRate);
ADSPResult afe_clock_manager_device_proxy(DalDeviceHandle *pHandle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
