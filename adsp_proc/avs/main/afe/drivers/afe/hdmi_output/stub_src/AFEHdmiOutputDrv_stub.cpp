/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/stub_src/AFEHdmiOutputDrv_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2013-2015 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/stub_src/AFEHdmiOutputDrv_stub.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "qurt_error.h"
#include "AFEDeviceDriver.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "adsp_afe_service_commands.h"
#include "AFEHdmiOutputDrv_i.h"
#include "AFEHdmiOutputDrv.h"
#include "AFEHdmiOutputGroupDevice.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalLpassHdmiOutput.h"
#include "AFEHalMdssHdmiOutput.h"

#include "AFEChStatusUtils.h"
#include "avtimer_drv_api.h"


/*=====================================================================
 Functions
 ======================================================================*/


ADSPResult afe_hdmi_output_driver_init(void)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_hdmi_output_driver_deinit(void)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_hdmi_output_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_ENOTIMPL;
}


ADSPResult afe_hdmi_output_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_set_hdmi_output_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_EUNSUPPORTED;
}



ADSPResult afe_hdmi_output_set_chstatus_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_hdmi_output_get_cfg (afe_dev_port_t *dev_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_hdmi_output_driver_open(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}



ADSPResult afe_hdmi_output_driver_close(afe_dev_port_t *dev_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}



ADSPResult afe_hdmi_output_driver_write(afe_dev_port_t *dev_port_ptr, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
   return ADSP_EUNSUPPORTED;
}



void afe_hdmi_output_get_dma_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr)
{
   return;
}


ADSPResult afe_get_time_for_hdmi_out_port(afe_dev_port_t *afe_port_ptr, uint64 *curr_time)
{
   return ADSP_EUNSUPPORTED;
}

MmpmCoreIdType afe_get_hdmitx_mmpm_coreid()
{
   return  MMPM_CORE_ID_NONE;
}


MmpmBwPortIdType afe_get_hdmitx_mmpm_bwportid()
{
    return MMPM_BW_PORT_ID_NONE;
}


ADSPResult afe_hdmi_over_dp_unmute(afe_dev_port_t *dev_port_ptr, void* p_ctx)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_hdmi_over_dp_mute(afe_dev_port_t *dev_port_ptr, void* p_ctx)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_hdmi_over_dp_cust_msg_handler(void *cust_payload_ptr)
{
   return ADSP_EUNSUPPORTED;
}
