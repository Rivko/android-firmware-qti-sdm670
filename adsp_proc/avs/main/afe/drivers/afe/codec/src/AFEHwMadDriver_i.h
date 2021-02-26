/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFEHwMadDriver_i.h#1 $
 ====================================================================== */
#ifndef _AFE_HW_MAD_DRIVER_I_H_
#define _AFE_HW_MAD_DRIVER_I_H_

/*==========================================================================
  Include Files
========================================================================== */
#include "AFEHwMadDriver.h"
#include "AFEDeviceDriver.h"
#include "AFESlimbusDriver.h"
#include "qurt_elite.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Globals
========================================================================== */
extern aud_stat_afe_svc_t *afe_svc_global_ptr;

/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult hw_mad_interrupt_register_handler(afe_dev_port_t *pDevPort, uint16_t mad_ctr, uint16_t mad_type);
ADSPResult afe_hw_mad_dc_codec_sequence(hw_mad_data_t *hw_mad_ptr, uint16_t mad_type);
ADSPResult afe_cdc_hw_mad_reg_cfg_init(void *hw_mad_ptr);
uint16_t   afe_cdc_hwmad_sleep_time_conversion(uint16_t sleep_time_ms);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AFE_HW_MAD_DRIVER_I_H_
