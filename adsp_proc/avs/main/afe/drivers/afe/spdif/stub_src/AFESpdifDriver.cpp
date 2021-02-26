/*========================================================================
    This file contains main interface to the AFE Spdif driver
  

  Copyright (c) 2009-2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
   
    $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/stub_src/AFESpdifDriver.cpp#1 $
   ====================================================================== */

#include "AFESpdifDriver.h"


ADSPResult afe_spdif_driver_init(void)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_spdif_driver_deinit()
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_spdif_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_EUNSUPPORTED;

}

ADSPResult afe_spdif_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_spdif_open(afe_dev_port_t *p_dev_port)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_spdif_close(afe_dev_port_t *p_dev_port)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_spdif_set_cfg (afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_spdif_get_cfg (afe_dev_port_t *p_dev_port, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_spdif_set_chstatus_cfg(afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_spdif_get_chstatus_cfg(afe_dev_port_t *p_dev_port, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_spdif_set_clk_cfg(afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_spdif_write(afe_dev_port_t *afe_port_ptr,  int16_t *p_buffer, uint32_t num_samples)
{
   return ADSP_EUNSUPPORTED;
}



ADSPResult afe_get_time_for_spdif_port(afe_dev_port_t *p_dev_port, uint64 *curr_time)
{ 
      return ADSP_EUNSUPPORTED;
}

ADSPResult afe_update_avt_drift_for_spdif_port(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

uint64_t afe_spdif_get_tsdiff_timestamp(afe_dev_port_t *afe_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

MmpmCoreIdType afe_get_spdif_mmpm_coreid()
{
   return MMPM_CORE_ID_NONE;
}
