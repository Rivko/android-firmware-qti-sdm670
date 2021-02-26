/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/cdc_pwr_ctl/stub_src/AFECdcPwrCtl.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFECdcPwrCtl.cpp

DESCRIPTION: Main Interface to the AFE Codec power control

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/cdc_pwr_ctl/stub_src/AFECdcPwrCtl.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
09-30-2016  mk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFECdcPwrCtl.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
 * Configure codec power state control
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state  *
 *
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_cfg_cdc_pwr_state_cntl(void* p_drv)
{
  return ADSP_EUNSUPPORTED;
}
