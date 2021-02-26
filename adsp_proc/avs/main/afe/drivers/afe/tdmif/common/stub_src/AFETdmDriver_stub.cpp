/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/stub_src/AFETdmDriver_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/stub_src/AFETdmDriver_stub.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

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
#include "AFEInternal.h"
#include "AFETdmDriver.h"


ADSPResult afe_tdm_driver_init(void)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_tdm_driver_deinit(void)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_tdm_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_tdm_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
   return ADSP_EUNSUPPORTED;
}

void afe_tdm_get_dev_port(uint16 unPortId, afe_dev_port_t **pDevPort)
{
  *pDevPort = NULL;
}

