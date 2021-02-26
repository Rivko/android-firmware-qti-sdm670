#ifndef __AFE_GROUP_DEVICE_DRIVER_H__
#define __AFE_GROUP_DEVICE_DRIVER_H__
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEGroupDeviceDriver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "AFEDevService.h"
#include "AFEInternal.h"


/** @brief Enums for group device cfg versions */
typedef enum
{
	GROUP_DEVICE_CFG_V1 = 0x1
}afe_param_id_group_device_cfg_minor_version;

bool_t afe_svc_validate_group_dev_id(uint16_t group_dev_id);

ADSPResult afe_svc_set_group_device_module_params(uint32_t param_id,
                                                  int8_t *param_buffer_ptr,
                                                  uint32_t param_size);

#endif // !defined(__AFE_GROUP_DEVICE_DRIVER_H__)
