/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/stub_src/AFEHwMadDriver_stub.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFEInternal.h"
#include "AFEHwMadDriver.h"
#include "AFESwMad.h"
#include "AFEMmpm.h"


/*==========================================================================
  Functions
========================================================================== */
ADSPResult afe_port_hw_mad_alloc_resources(void *port_ptr)
{
  return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_init(void *port_ptr)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_dealloc_resources(void *port_ptr)
{
  return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_deinit(void *port_ptr)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_set_param_handler(void *port_ptr, uint32_t param_id, int8_t* params_buffer_ptr, uint16_t param_size)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_enable_disable_handler(void *ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_interrupt_handler(void *ptr, cdc_client_info_t *client_info)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_port_hw_mad_dc_reset_handler(void *ptr, uint16_t mad_type)
{
   return ADSP_ENOTIMPL;
}
