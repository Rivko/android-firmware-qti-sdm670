#ifndef __AFE_DEVICE_DRIVER_I_H__
#define __AFE_DEVICE_DRIVER_I_H__
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver_i, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceDriver_i.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#include "AFEDevService.h"
#include "AFEVfrHandler.h"

#if defined(__cplusplus)
extern "C" {
#endif

ADSPResult afe_dev_create_working_buffers(afe_dev_port_t *pDevPort);


ADSPResult afe_device_update_avt_drift(afe_dev_port_t *dev_port_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // !defined(__AFEDEVICEDRIVER_H__)

