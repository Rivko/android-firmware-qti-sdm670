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


#include "sns_dae.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"

/* ------------------------------------------------------------------------------------ */

static bool stream_usable(sx931x_dae_stream *dae_stream)
{
    return (NULL != dae_stream->stream && dae_stream->stream_usable);
}

static bool send_SAR_config(sx931x_dae_stream *dae_stream, sx931x_sensor_cfg_info *temp_info)
{
    bool cmd_sent = false;
    sns_time time_now = sns_get_system_time();
    uint64_t dae_wm = (uint64_t)(temp_info->sampling_rate_hz / temp_info->report_rate_hz);
    sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_default;
    uint8_t encoded_msg[sns_dae_set_streaming_config_size];
    sns_request req =
    {
        .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
        .request      = encoded_msg
    };

    config_req.dae_watermark = (dae_wm > UINT32_MAX) ? UINT32_MAX : (uint32_t)dae_wm;
    config_req.has_polling_config = true;
    config_req.polling_config.polling_interval_ticks = temp_info->sampling_intvl;
    config_req.polling_config.polling_offset = (time_now + temp_info->sampling_intvl) / temp_info->sampling_intvl * temp_info->sampling_intvl;

    if((req.request_len =
            pb_encode_request(encoded_msg,
                              sizeof(encoded_msg),
                              &config_req,
                              sns_dae_set_streaming_config_fields,
                              NULL)) > 0)
    {
        if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
        {
            dae_stream->state = STREAM_STARTING;
            cmd_sent = true;
        }
    }
    return cmd_sent;
}

 
/* ------------------------------------------------------------------------------------ */
static bool flush_hw(sx931x_dae_stream *dae_stream)
{
    bool cmd_sent = false;
    sns_request req =
    {
        .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_HW,
        .request      = NULL,
        .request_len  = 0
    };

    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
        cmd_sent = true;
        dae_stream->flushing_hw = true;
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool flush_samples(sx931x_dae_stream *dae_stream)
{
    bool cmd_sent = false;
    sns_request req =
    {
        .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS,
        .request      = NULL,
        .request_len  = 0
    };

    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
        cmd_sent = true;
        dae_stream->flushing_data = true;
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool stop_streaming(sx931x_dae_stream *dae_stream)
{
    bool cmd_sent = false;
    sns_request req =
    {
        .message_id   = SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING,
        .request      = NULL,
        .request_len  = 0
    };

    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
        cmd_sent = true;
        dae_stream->state = STREAM_STOPPING;
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static void process_pressure_samples(
    sns_sensor_instance *this,
    sns_time            timestamp,
    const uint8_t       *buf,
    size_t              buf_len)
{
    UNUSED_VAR(timestamp);
    UNUSED_VAR(buf);
    if(buf_len == 6)
    {
        //sx931x_convert_and_send_sar_sample(this, timestamp, buf);
    }
    else
    {
        SNS_INST_PRINTF(ERROR, this, "Unexpected data len %u from DAE sensor", buf_len);
    }
}

/* ------------------------------------------------------------------------------------ */
static void process_temp_samples(
    sns_sensor_instance *this,
    sns_time            timestamp,
    const uint8_t       *buf,
    size_t              buf_len)
{
    UNUSED_VAR(timestamp);
    UNUSED_VAR(buf);

    if(buf_len == 3)
    {
        //sx932x_convert_and_send_temp_sample(this, timestamp, buf);
    }
    else
    {
        SNS_INST_PRINTF(ERROR, this, "Unexpected data len %u from DAE sensor", buf_len);
    }
}


/* ------------------------------------------------------------------------------------ */
static void process_data_event(sns_sensor_instance *this,
                               sx931x_dae_stream  *dae_stream,
                               pb_istream_t        *pbstream)
{
    pb_buffer_arg decode_arg;
    sns_dae_data_event data_event = sns_dae_data_event_init_default;
    data_event.sensor_data.funcs.decode = &pb_decode_string_cb;
    data_event.sensor_data.arg = &decode_arg;
    if(pb_decode(pbstream, sns_dae_data_event_fields, &data_event))
    {
        sx931x_dae_if_info *dae_if = &((sx931x_instance_state *)this->state->state)->dae_if;
        if (dae_stream == &dae_if->sar)
        {
            process_pressure_samples(
                this, data_event.timestamp, (uint8_t *)decode_arg.buf, decode_arg.buf_len);

        }
        else
        {
            process_temp_samples(
                this, data_event.timestamp, (uint8_t *)decode_arg.buf, decode_arg.buf_len);
        }
    }
}

/* ------------------------------------------------------------------------------------ */
static void process_response(sns_sensor_instance *this,
                             sx931x_dae_stream  *dae_stream,
                             pb_istream_t        *pbstream)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;

    sns_dae_resp resp = sns_dae_resp_init_default;
    if(pb_decode(pbstream, sns_dae_resp_fields, &resp))
    {
        switch(resp.msg_id)
        {
        case SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG:
            if(SNS_STD_ERROR_NO_ERROR != resp.err)
            {
                /* DAE sensor does not have support for this driver */
                dae_stream->stream_usable = false;
                SX931x_INST_PRINTF(LOW, this, "DAE_SET_STATIC_CONFIG error %d", resp.err);
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_S4S_DYNAMIC_CONFIG:
            break;
        case SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG:
            if(dae_stream->stream != NULL && dae_stream->state == STREAM_STARTING)
            {
                dae_stream->state = (SNS_STD_ERROR_NO_ERROR == resp.err) ? STREAMING : IDLE;
                SX931x_INST_PRINTF(LOW, this, "DAE_SET_STATIC_CONFIG error %d", resp.err);
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_FLUSH_HW:
            if(state->config_step != CONFIG_IDLE)
            {
                //sx931x_reconfig_hw(this);
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS:
            break;
        case SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING:
            if(dae_stream->state == STREAM_STOPPING)
            {
                dae_stream->state = (SNS_STD_ERROR_NO_ERROR != resp.err) ? STREAMING : IDLE;
            }
            if(NULL != state->dae_if.sar.stream &&
               state->dae_if.sar.state != STREAM_STOPPING &&
               NULL != state->dae_if.temp.stream &&
               state->dae_if.temp.state != STREAM_STOPPING)
            {
                /* done waiting */
                if(state->config_step == CONFIG_STOPPING_STREAM)
                {
                    sx931x_dae_if_flush_hw(this);
                }
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_PAUSE_S4S_SCHED:
            break;

        case SNS_DAE_MSGID_SNS_DAE_RESP:
        case SNS_DAE_MSGID_SNS_DAE_DATA_EVENT:
            break; /* unexpected */
        }
    }
}

/* ------------------------------------------------------------------------------------ */
static void process_events(sns_sensor_instance *this, sx931x_dae_stream *dae_stream)
{
    sns_sensor_event *event;
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;

    while(NULL != dae_stream->stream &&
          NULL != (event = dae_stream->stream->api->peek_input(dae_stream->stream)))
    {
        if(dae_stream->stream_usable)
        {
            pb_istream_t pbstream =
                pb_istream_from_buffer((pb_byte_t *)event->event, event->event_len);

            if (SNS_DAE_MSGID_SNS_DAE_DATA_EVENT == event->message_id)
            {
                process_data_event(this, dae_stream, &pbstream);
            }
            else if(SNS_DAE_MSGID_SNS_DAE_INTERRUPT_EVENT == event->message_id)
            {
            }
            else if(SNS_DAE_MSGID_SNS_DAE_RESP == event->message_id)
            {
                process_response(this, dae_stream, &pbstream);
            }
            else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
            {
                SX931x_INST_PRINTF(LOW, this, "DAE error event");
                dae_stream->stream_usable = false;
            }
            else
            {
                SNS_INST_PRINTF(ERROR, this, "Unexpected message id %u", event->message_id);
            }
        }
        event = dae_stream->stream->api->get_next_input(dae_stream->stream);
    }

    if(!dae_stream->stream_usable && NULL != dae_stream->stream)
    {
        SX931x_INST_PRINTF(LOW, this, "removing DAE stream");
        sns_service_manager *service_mgr = this->cb->get_service_manager(this);
        sns_stream_service *stream_mgr =
            (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
        stream_mgr->api->remove_stream(stream_mgr, dae_stream->stream);
        dae_stream->stream = NULL;

        if(state->deploy_info.publish_sensors & SX9XXX_SAR)
        {
            sx931x_set_sar_polling_config(this);
        } 
    }
}


/***************************************************************************
* public function
***************************************************************************/
bool sx931x_dae_if_available(sns_sensor_instance *this)
{
    /* both streams must be availble */
    sx931x_dae_if_info *dae_if = &((sx931x_instance_state *)this->state->state)->dae_if;
    return (stream_usable(&dae_if->sar) && stream_usable(&dae_if->temp));
}

/* ------------------------------------------------------------------------------------ */
sns_rc sx931x_dae_if_init(
    sns_sensor_instance *const this,
    sns_stream_service  *stream_mgr,
    sns_sensor_uid      *dae_suid)
{
    sns_rc rc = SNS_RC_NOT_AVAILABLE;
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    sx931x_dae_if_info *dae_if = &state->dae_if;
    sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;

    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            *dae_suid,
            &dae_if->sar.stream);
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            *dae_suid,
            &dae_if->temp.stream);

    if(NULL != dae_if->sar.stream && NULL != dae_if->temp.stream)
    {
        uint8_t encoded_msg[sns_dae_set_static_config_size];
        sns_request req =
        {
            .message_id  = SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG,
            .request     = encoded_msg,
            .request_len = 0
        };

        dae_if->sar.nano_hal_vtable_name = "sx931x_sar_hal_table";

        sns_strlcpy(config_req.func_table_name,
                    dae_if->sar.nano_hal_vtable_name,
                    sizeof(config_req.func_table_name));
        config_req.interrupt              = false;
        config_req.has_irq_config         = false;

        req.request_len = pb_encode_request(encoded_msg,
                                            sizeof(encoded_msg),
                                            &config_req,
                                            sns_dae_set_static_config_fields,
                                            NULL);
        if(0 < req.request_len)
        {
            rc = dae_if->sar.stream->api->send_request(dae_if->sar.stream, &req);
        }

    }

    if(SNS_RC_SUCCESS != rc)
    {
        sx931x_dae_if_deinit(state, stream_mgr);
    }
    else
    {
        dae_if->sar.stream_usable   = true;
        dae_if->temp.stream_usable = true;
    }
    return rc;
}

/* ------------------------------------------------------------------------------------ */
void sx931x_dae_if_deinit(sx931x_instance_state *state, sns_stream_service *stream_mgr)
{
    if(NULL != state->dae_if.sar.stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->dae_if.sar.stream);
        state->dae_if.sar.stream = NULL;
    }
    if(NULL != state->dae_if.temp.stream)
    {
        stream_mgr->api->remove_stream(stream_mgr, state->dae_if.temp.stream);
        state->dae_if.temp.stream = NULL;
    }
}

/* ------------------------------------------------------------------------------------ */
bool sx931x_dae_if_stop_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    sx931x_dae_if_info *dae_if = &state->dae_if;

    if(stream_usable(&dae_if->sar) &&
       (dae_if->sar.state == STREAMING || dae_if->sar.state == STREAM_STARTING))
    {
        cmd_sent |= stop_streaming(&dae_if->sar);
    }

    if(stream_usable(&dae_if->temp) &&
       (dae_if->temp.state == STREAMING || dae_if->temp.state == STREAM_STARTING))
    {
        cmd_sent |= stop_streaming(&dae_if->temp);
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool sx931x_dae_if_start_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    sx931x_dae_if_info    *dae_if = &state->dae_if;

    if(stream_usable(&dae_if->sar) && 0 < state->sar_info.sampling_intvl)
    {
        cmd_sent |= send_SAR_config(&dae_if->sar, &state->sar_info);
    }
 
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool sx931x_dae_if_flush_hw(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    sx931x_dae_if_info *dae_if = &((sx931x_instance_state *)this->state->state)->dae_if;

    if(stream_usable(&dae_if->sar) && !dae_if->sar.flushing_hw)
    {
        cmd_sent |= flush_hw(&dae_if->sar);
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool sx931x_dae_if_flush_samples(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    sx931x_dae_if_info *dae_if = &((sx931x_instance_state *)this->state->state)->dae_if;

    if(stream_usable(&dae_if->sar))
    {
        if(!dae_if->sar.flushing_data)
        {
            flush_samples(&dae_if->sar);
        }
        cmd_sent |= dae_if->sar.flushing_data;
    }

    if(stream_usable(&dae_if->temp))
    {
        if(!dae_if->temp.flushing_data)
        {
            flush_samples(&dae_if->temp);
        }
        cmd_sent |= dae_if->temp.flushing_data;
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
void sx931x_dae_if_process_events(sns_sensor_instance *this)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;
    process_events(this, &state->dae_if.sar);
    process_events(this, &state->dae_if.temp);

    if(NULL == state->dae_if.sar.stream || NULL == state->dae_if.temp.stream)
    {
        /* both streams are needed; if one was removed, remove the other one too */
        sns_service_manager *service_mgr = this->cb->get_service_manager(this);
        sns_stream_service *stream_mgr =
            (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
        sx931x_dae_if_deinit(state, stream_mgr);
    }
}

