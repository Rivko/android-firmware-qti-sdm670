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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/


#include "sns_island_service.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_bmp285_hal.h"
#include "sns_bmp285_sensor.h"
#include "sns_bmp285_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_std_event_gated_sensor.pb.h"
#if(BMP_CONFIG_ENABLE_DIAG_LOG == 1)
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#endif
#include "sns_sensor_util.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"


static sns_rc bmp285_inst_notify_event(sns_sensor_instance * const this)
{
  bmp285_instance_state *state = (bmp285_instance_state*) this->state->state;
  sns_sensor_event *event;
  #if BMP_CONFIG_ENABLE_DAE
  bmp285_dae_if_process_events(this);
  #endif
  /* Turn COM port ON */
  state->scp_service->api->sns_scp_update_bus_power(
      state->com_port_info.port_handle, true);

  /* Handle Async Com Port events */
  if(NULL != state->async_com_port_data_stream)
  {
    event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream);
    while(NULL != event)
    {
      if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR)
      {
        //TODO: Warning;
        SNS_INST_PRINTF(ERROR, this, "Received ASCP error event id=%d",
                                      event->message_id);
      }
      else if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW)
      {
      }
      event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
    }
  }

  /* timer event, different odr should has different timer to handle it in a instance */
  if(state->pressure_timer_data_stream != NULL)
  {
    event = state->pressure_timer_data_stream->api->peek_input(state->pressure_timer_data_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,event->event_len);
      sns_timer_sensor_event timer_event;
      if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
      {
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
        {
          if(state->pressure_info.timer_is_active &&
            state->pressure_info.sampling_intvl > 0)
          {
            bmp285_handle_pressure_data_stream_timer_event(this, &timer_event);
          }
        }
      }
      else
      {
        SNS_INST_PRINTF(MED, this, "unknown message_id %d", event->message_id);
      }
      event = state->pressure_timer_data_stream->api->get_next_input(state->pressure_timer_data_stream);
    }
  }

  if(state->temperature_timer_data_stream != NULL)
  {
    event = state->temperature_timer_data_stream->api->peek_input(state->temperature_timer_data_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,event->event_len);
      sns_timer_sensor_event timer_event;
      if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
      {
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
        {
          if(state->temperature_info.timer_is_active &&
            state->temperature_info.sampling_intvl > 0)
          {
            bmp285_handle_temperature_data_stream_timer_event(this, &timer_event);
          }
        }
      }
      else
      {
        SNS_INST_PRINTF(MED, this, "unknown message_id %d", event->message_id);
      }
      event = state->temperature_timer_data_stream->api->get_next_input(state->temperature_timer_data_stream);
    }
  }
  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(
      state->com_port_info.port_handle, false);

  return SNS_RC_SUCCESS;
}
/** Public Data Definitions. */
sns_sensor_instance_api sns_see_bmp285_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &bmp285_inst_init,
  .deinit = &bmp285_inst_deinit,
  .set_client_config = &bmp285_inst_set_client_config,
  .notify_event = &bmp285_inst_notify_event
};
