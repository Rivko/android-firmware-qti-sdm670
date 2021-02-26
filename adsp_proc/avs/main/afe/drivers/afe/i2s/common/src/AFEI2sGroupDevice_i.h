#ifndef _AFE_I2S_GROUP_DEVICE_I_H_
#define _AFE_I2S_GROUP_DEVICE_I_H_
/**
@file AFEI2sGroupDevice_i.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sGroupDevice_i.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "AFEI2sDriver_i.h"
#include "AFEGroupDeviceDriver.h"


/*==========================================================================
  Macro Defines
  ========================================================================== */
typedef struct afe_i2s_group_device_state_struct
{
  afe_group_device_state_struct_t group_device_common_state;
 
  uint16_t                  channel_mode;
  /**< I2S lines and multichannel operation */
  
} afe_i2s_group_device_state_struct_t;

ADSPResult afe_dev_group_config_i2s_port(i2s_port_state_t * p_dev_state, afe_dev_grp_config_info_t * i2s_grp_cfg_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_TDM_GROUP_DEVICE_I_H_)
