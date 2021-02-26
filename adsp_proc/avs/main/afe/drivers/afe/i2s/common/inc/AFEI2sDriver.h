#ifndef _AFE_I2S_DRIVER_H_
#define _AFE_I2S_DRIVER_H_
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/inc/AFEI2sDriver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
14/03/11    mspk      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "AFEInternal.h"
#include "mmdefs.h"
#include "VfrDriverInternal.h"
#include "AFEDeviceDriver.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/**
  @brief Perform I2S Driver Initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_i2s_driver_init(void);

/**
  @brief Perform I2S Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_i2s_driver_deinit(void);


/**
  @brief Perform I2S port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_i2s_port_init(aud_stat_afe_svc_t* afe_svc_ptr);


/**
  @brief Perform I2S port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_i2s_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_I2S_DRIVER_H_)
