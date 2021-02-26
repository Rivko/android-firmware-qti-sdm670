#ifndef _AFE_TDM_GROUP_DEVICE_I_H_
#define _AFE_TDM_GROUP_DEVICE_I_H_
/**
@file AFETdmGroupDevice_i.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmGroupDevice_i.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "AFETdmDriver_i.h"


/*==========================================================================
  Macro Defines
  ========================================================================== */
typedef struct afe_tdm_group_device_state_struct
{
  afe_group_device_state_struct_t group_device_common_state;
  
  uint16_t nslots_per_frame;
  /**< Number of slots per frame for tdm group */
  
  uint16_t slot_width;
  /**< Slot width */
  
  uint32_t slot_mask;
  /**< Slot mask to identify which channels is in active */
  
} afe_tdm_group_device_state_struct_t;


ADSPResult afe_dev_group_config_tdm_port(tdm_port_state_t * p_dev_state, afe_dev_grp_config_info_t * tdm_grp_cfg_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_TDM_GROUP_DEVICE_I_H_)
