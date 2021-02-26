/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "sns_tmx4903_sensor.h"

/** Public Function Definitions. */

sns_rc sns_register_ams(sns_register_cb const *register_api)
{
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  /** Register ALS Sensor. */
  register_api->init_sensor(sizeof(ams_state), &ams_als_streaming_sensor_api,
      &ams_sensor_instance_api);
  register_api->init_sensor(sizeof(ams_state), &ams_als_sensor_api,
      &ams_sensor_instance_api);
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  /** Register RGB Sensor. */
  register_api->init_sensor(sizeof(ams_state), &ams_rgb_streaming_sensor_api,
      &ams_sensor_instance_api);
  register_api->init_sensor(sizeof(ams_state), &ams_rgb_sensor_api,
      &ams_sensor_instance_api);
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  /** Register Prox Sensor. */
  register_api->init_sensor(sizeof(ams_state), &ams_prox_streaming_sensor_api,
      &ams_sensor_instance_api);
  register_api->init_sensor(sizeof(ams_state), &ams_prox_sensor_api,
      &ams_sensor_instance_api);
#endif
  return SNS_RC_SUCCESS;
}
