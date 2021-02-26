/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_sx931x_hal.h"
#include "sns_sx931x_sensor.h"
#include "sns_sx931x_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag_service.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_diag.pb.h"
#include "sns_sync_com_port_service.h"


static sns_rc sns_see_sx931x_inst_notify_event(sns_sensor_instance *const this)
{
    sx931x_instance_state *state = (sx931x_instance_state *) this->state->state;
    sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
    sns_sensor_event *event;

    SX931x_INST_PRINTF(LOW, this, "<sns_see_sx931x_inst_notify_event> enter");

    sx931x_dae_if_process_events(this);
    /* Turn COM port ON */
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

    /*************************Handle interrupt*******************************/
    if(NULL != state->interrupt_data_stream)
    {
        event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
        while(NULL != event)
        {
            if(event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
            {
                SX931x_INST_PRINTF(LOW, this, "<sns_see_sx931x_inst_notify_event>INTERRUPT_REG_EVENT");
                state->irq_info.irq_ready = true; 
                sx931x_interrupt_config(state->scp_service, state->com_port_info.port_handle, 0x68);
            }
            else if(event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT)
            {
                //for test
                SX931x_INST_PRINTF(LOW, this, "<sns_see_sx931x_inst_notify_event>INTERRUPT_EVENT");

                pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event, event->event_len);

                if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
                {
                    sx931x_read_gpio(this, state->irq_info.irq_config.interrupt_num,
                                     state->irq_info.irq_config.is_chip_pin);
                    sx931x_handle_sar_data_stream_interrupt_event(this);
                }
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d", event->message_id);
            }
            event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
        }
    }
    /*********************** Handle Async Com Port events *********************/
    if(NULL != state->async_com_port_data_stream)
    {
        event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream);
        while (NULL != event)
        {
            if (event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR)
            {
                /*add com err process*/
            }
            else if (event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW)
            {
                /*add port vector rw*/
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d", event->message_id);
            }
            event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
        }
    }

    /***** timer event, different odr should has different timer to handle it in a instance ****/
    if (state->sar_timer_data_stream != NULL)
    {
        event = state->sar_timer_data_stream->api->peek_input(state->sar_timer_data_stream);
        while (NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event, event->event_len);
            sns_timer_sensor_event timer_event;
            if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
            {
                if (event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
                {
                    if(state->sar_info.timer_is_active && state->sar_info.sampling_intvl > 0)
                    {
                        sx931x_handle_sar_data_stream_timer_event(this);
                    }
                }
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d", event->message_id);
            }
            event = state->sar_timer_data_stream->api->get_next_input(state->sar_timer_data_stream);
        }
    }

    // Turn COM port OFF
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

    return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */
sns_sensor_instance_api sns_see_sx931x_sensor_instance_api =
{
    .struct_len = sizeof(sns_sensor_instance_api),
    .init = &sns_see_sx931x_inst_init,
    .deinit = &sns_see_sx931x_inst_deinit,
    .set_client_config = &sns_see_sx931x_inst_set_client_config,
    .notify_event = &sns_see_sx931x_inst_notify_event
};
