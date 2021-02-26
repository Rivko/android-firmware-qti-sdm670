/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/sw_dutycycle/stub_src/AFEDutyCycleDriver_Stub.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFEDutyCycleDriver.h"


/*==========================================================================
  Functions
========================================================================== */
ADSPResult afe_port_sw_duty_cycle_alloc_resources(afe_dev_port_t *dev_port_ptr)
{
  return ADSP_EUNSUPPORTED;
}

ADSPResult afe_port_sw_duty_cycle_init(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_port_sw_duty_cycle_dealloc_resources(afe_dev_port_t *dev_port_ptr)
{
  return ADSP_EUNSUPPORTED;
}

ADSPResult afe_port_sw_duty_cycle_deinit(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

 ADSPResult afe_port_sw_duty_cycle_set_param_handler(afe_dev_port_t *dev_port_ptr,
                                                      uint32_t param_id,
                                                      int8_t* params_buffer_ptr,
                                                      uint16_t param_size)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_port_sw_duty_cycle_enable_disable_handler(afe_dev_port_t *dev_port_ptr)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_port_sw_duty_cycle_reset_handler(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

void afe_port_disable_sw_duty_cycle(afe_dev_port_t *pDevPort)
{
  return;
}

