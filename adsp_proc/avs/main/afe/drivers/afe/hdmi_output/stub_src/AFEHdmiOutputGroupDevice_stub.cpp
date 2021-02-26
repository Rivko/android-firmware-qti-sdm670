/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/stub_src/AFEHdmiOutputGroupDevice_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEHdmiOutputGroupDevice.cpp

DESCRIPTION: Main Interface to the AFE Device driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/stub_src/AFEHdmiOutputGroupDevice_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEGroupDeviceDriver.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"

#include "AFEHdmiOutputGroupDevice.h"

#include "AFEHdmiOutputDrv_i.h"



ADSPResult afe_hdmi_output_set_group_device_module_params(uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size)
{
   return ADSP_EUNSUPPORTED;
}



