#pragma once

//#define ICM4X6XX_OEM_TEST

#ifdef ICM4X6XX_OEM_TEST
#include "sns_sensor_instance.h"
void icm4x6xx_handle_oem_request(sns_sensor *const this, sns_sensor_instance *instance,
                                struct sns_request const *request);
#endif
