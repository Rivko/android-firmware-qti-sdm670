/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2016 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/stub_src/AFECodecService_stub.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "adsp_afe_service_commands.h"

/*==========================================================================
  Globals
========================================================================== */
afe_cdc_handler_t *cdc_handler_global_ptr = NULL;

/*==========================================================================
  Function implementations
========================================================================== */
ADSPResult afe_svc_set_cdc_dev_cfg(uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_svc_cdc_handler_deinit(void)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_svc_cdc_handler_init(void)
{
   return ADSP_ENOTIMPL;
}

ADSPResult afe_port_custom_cdc_int_handling(elite_msg_any_t* pMsg)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_cdc_intr_reg_handler(cdc_client_info_t *cdc_client_info_ptr, bool_t is_it_register)
{
   return ADSP_EUNSUPPORTED;
}
ADSPResult afe_svc_check_codec_interrupts_supported(void)
{
    return ADSP_EUNSUPPORTED;
}
ADSPResult afe_cdc_update_cdc_pwr_state_register(uint8_t *value_ptr)
{
    return ADSP_EUNSUPPORTED;
}

