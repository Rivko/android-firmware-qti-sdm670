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

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
static sns_sensor_uid const* ams_als_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = ALS_SUID;

  return &sensor_uid;
}
static sns_sensor_uid const* ams_als_streaming_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = ALS_STREAMING_SUID;

  return &sensor_uid;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
static sns_sensor_uid const* ams_rgb_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = RGB_SUID;

  return &sensor_uid;
}
static sns_sensor_uid const* ams_rgb_streaming_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = RGB_STREAMING_SUID;

  return &sensor_uid;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
static sns_sensor_uid const* ams_prox_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = PROX_SUID;

  return &sensor_uid;
}
static sns_sensor_uid const* ams_prox_streaming_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = PROX_STREAMING_SUID;

  return &sensor_uid;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
sns_sensor_api ams_als_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_als_init,
  .deinit             = &ams_als_deinit,
  .get_sensor_uid     = &ams_als_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_sensor_notify_event,
};
sns_sensor_api ams_als_streaming_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_als_streaming_init,
  .deinit             = &ams_als_deinit,
  .get_sensor_uid     = &ams_als_streaming_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_streaming_sensor_notify_event,
};
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
sns_sensor_api ams_rgb_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_rgb_init,
  .deinit             = &ams_rgb_deinit,
  .get_sensor_uid     = &ams_rgb_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_sensor_notify_event,
};
sns_sensor_api ams_rgb_streaming_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_rgb_streaming_init,
  .deinit             = &ams_rgb_deinit,
  .get_sensor_uid     = &ams_rgb_streaming_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_streaming_sensor_notify_event,
};
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
sns_sensor_api ams_prox_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_prox_init,
  .deinit             = &ams_prox_deinit,
  .get_sensor_uid     = &ams_prox_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_sensor_notify_event,
};
sns_sensor_api ams_prox_streaming_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &ams_prox_streaming_init,
  .deinit             = &ams_prox_deinit,
  .get_sensor_uid     = &ams_prox_streaming_get_sensor_uid,
  .set_client_request = &ams_set_client_request,
  .notify_event       = &ams_streaming_sensor_notify_event,
};
#endif

