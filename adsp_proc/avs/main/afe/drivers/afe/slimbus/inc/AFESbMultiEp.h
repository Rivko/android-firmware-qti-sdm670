#ifndef __AFE_SB_MULTI_EP_H__
#define __AFE_SB_MULTI_EP_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/inc/AFESbMultiEp.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESbMultiEp.h

DESCRIPTION: Main Interface to the AFE SLIMbus Multi end point

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/inc/AFESbMultiEp.h#1 $ $DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
12-25-2016  UdayB  Initial Draft


============================================================================*/
/**
@file AFESbMultiEp.h

@brief has APIs related to SLIMbus multiple end point
 */

#include "mmdefs.h"
#include "adsp_error_codes.h"
#include "ddislimbus.h"

#if defined(__cplusplus)
extern "C" {
#endif

 /*=====================================================================
 Functions
 ======================================================================*/

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
                                                SlimBusResourceHandle *phPort, uint32_t req_num_channels);

  /**
    @brief Checks Hardware compatibility for multiple end point
           support.

    @param[in] p_core_driver    Handle to the SLIMbus driver

    @param[in] p_multi_ep_flag  pointer to multi end point flag

    @return                     ADSP_EOK on success, an error code on error
   */
  ADSPResult afe_sb_multi_ep_get_hw_capability(DalDeviceHandle *p_core_driver, bool_t *multi_ep_flag);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*#ifndef __AFE_SB_MULTI_EP_H__*/

