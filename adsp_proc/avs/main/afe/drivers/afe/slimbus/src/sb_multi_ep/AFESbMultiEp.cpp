/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/sb_multi_ep/AFESbMultiEp.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESbMultiEp.cpp

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/sb_multi_ep/AFESbMultiEp.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
12-25-2016  udayB  Multi end point support


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFESbMultiEp.h"
#include "Elite.h"

/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Checks Hardware compatibility for multiple end point
         support.

  @param[in] p_core_driver    Handle to the SLIMbus driver

  @param[in] p_multi_ep_flag  pointer to multi end point flag

  @return                     ADSP_EOK on success, an error code on error
 */
ADSPResult afe_sb_multi_ep_get_hw_capability(DalDeviceHandle *p_core_driver, bool_t *p_multi_ep_flag)
{
  ADSPResult              result = ADSP_EOK;
  SlimBusHwCapType hw_cap = (SlimBusHwCapType)0;

  /* By default, select legacy case */
  *p_multi_ep_flag = FALSE;

  /* Check for multi end point support by SLIMbus HW*/
  if ((DAL_SUCCESS == (result = DalSlimBus_GetHardwareCapability(p_core_driver, &hw_cap))))
  {
    if(hw_cap & SLIMBUS_HW_CAP_PORT_MULTI_ENDPOINT)
    {
      *p_multi_ep_flag = TRUE;
    }
  }

  return result;
}

/**
  @brief Allocates master port with multiple end points for
          requested number of channels

  @param[in] p_core_driver     Handle to the SLIMbus driver

  @param[in] uMinFifoBytes     Minimum required size of port FIFO in bytes

  @param[in] phPort            Location to store the handle to the allocated port

  @param[in] req_num_channels  Requested number of channels

  @return                      ADSP_EOK on success, an error code on error
 */
ADSPResult afe_sb_multi_ep_alloc_master_ports(DalDeviceHandle *p_core_driver, uint32_t uMinFifoBytes,
                                              SlimBusResourceHandle *phPort, uint32_t req_num_channels)
{
  DALResult result = ADSP_EOK;

  //allocate master port with multiple endpoints
  if(DAL_SUCCESS != (result = DalSlimBus_AllocMasterPortWithEndPoints(p_core_driver, req_num_channels, uMinFifoBytes, phPort)) )
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate master port with multi end points for %d channels, 0x%x", req_num_channels, result);
    return result;
  }

  return result;
}
