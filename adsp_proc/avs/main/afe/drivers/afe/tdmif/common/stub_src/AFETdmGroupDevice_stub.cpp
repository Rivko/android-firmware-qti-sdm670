/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/stub_src/AFETdmGroupDevice_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     

DESCRIPTION: Main Interface to the AFE Device driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/stub_src/AFETdmGroupDevice_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "AFETdmGroupDevice_i.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"


ADSPResult afe_tdm_set_group_device_module_params(uint32_t param_id, int8_t *param_buffer_ptr, uint32_t param_size)
{
  return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dev_group_config_tdm_port(tdm_port_state_t * p_dev_state, afe_dev_grp_config_info_t * tdm_grp_cfg_ptr)
{
  return ADSP_EUNSUPPORTED;
}



