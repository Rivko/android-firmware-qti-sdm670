/*******************************************************************************
 * Copyright (c) 2017, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/
#include <string.h>

#include "sns_shtw2_sensor.h"
#include "sns_types.h"

static sns_sensor_uid const* shtw2_temperature_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = TEMPERATURE_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* shtw2_humidity_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = HUMIDITY_SUID;

  return &sensor_uid;
}

sns_sensor_api shtw2_temperature_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &shtw2_temperature_init,
  .deinit             = &shtw2_temperature_deinit,
  .get_sensor_uid     = &shtw2_temperature_get_sensor_uid,
  .set_client_request = &shtw2_set_client_request,
  .notify_event       = &shtw2_sensor_notify_event,
};

sns_sensor_api shtw2_humidity_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &shtw2_humidity_init,
  .deinit             = &shtw2_humidity_deinit,
  .get_sensor_uid     = &shtw2_humidity_get_sensor_uid,
  .set_client_request = &shtw2_set_client_request,
  .notify_event       = &shtw2_sensor_notify_event,
};
