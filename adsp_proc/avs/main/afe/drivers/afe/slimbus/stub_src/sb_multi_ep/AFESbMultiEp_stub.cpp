/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/stub_src/sb_multi_ep/AFESbMultiEp_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESbMultiEp_stub.cpp

DESCRIPTION: Main Interface to the AFE SLIMbus multi end point

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/stub_src/sb_multi_ep/AFESbMultiEp_stub.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
12-25-2016  UdayB  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFESbMultiEp.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_sb_multi_ep_get_hw_capability(DalDeviceHandle *p_core_driver, bool_t *p_multi_ep_flag)
{
  *p_multi_ep_flag = FALSE;

  return ADSP_EOK;
}

ADSPResult afe_sb_multi_ep_alloc_master_ports(DalDeviceHandle *p_core_driver, uint32_t uMinFifoBytes,
                                              SlimBusResourceHandle *phPort, uint32_t req_num_channels)
{
return ADSP_EUNSUPPORTED;
}
