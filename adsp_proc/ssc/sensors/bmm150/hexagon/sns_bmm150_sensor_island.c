/*******************************************************************************
* Copyright (c) 2018, Bosch Sensortec GmbH
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Bosch Sensortec GmbH nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_event_service.h"
#include "sns_stream_service.h"
#include "sns_service.h"
#include "sns_sensor_util.h"
#include "sns_mem_util.h"
#include "sns_math_util.h"
#include "sns_types.h"
#include "sns_diag_service.h"
#include "sns_attribute_util.h"
#include "sns_sync_com_port_service.h"

#include "sns_bmm150_sensor.h"
#include "sns_bmm150_hal.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_motion_detect.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"
#include "sns_printf.h"

static sns_sensor_uid const* bmm150_mag_get_sensor_uid(sns_sensor const * const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = MAG_SUID;
  return &sensor_uid;
}
static sns_sensor_uid const* bmm150_temperature_get_sensor_uid(sns_sensor const * const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = TEMPERATURE_SUID;
  return &sensor_uid;
}
sns_sensor_api bmm150_mag_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &bmm150_mag_init,
  .deinit = &bmm150_mag_deinit,
  .get_sensor_uid = &bmm150_mag_get_sensor_uid,
  .set_client_request = &bmm150_sensor_set_client_request,
  .notify_event = &bmm150_sensor_notify_event,
};

sns_sensor_api bmm150_temperature_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &bmm150_temperature_init,
  .deinit = &bmm150_temperature_deinit,
  .get_sensor_uid = &bmm150_temperature_get_sensor_uid,
  .set_client_request = &bmm150_sensor_set_client_request,
  .notify_event = &bmm150_sensor_notify_event,
};
