/*******************************************************************************
 * Copyright (c) 2017-18, Bosch Sensortec GmbH
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

/**
 * @file sns_bmi160_sensor_island.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "sns_bmi160_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_motion_detect.pb.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"

#include "sns_cal.pb.h"

static sns_sensor_uid const* bmi160_accel_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = ACCEL_SUID;

    return &sensor_uid;
}

static sns_sensor_uid const* bmi160_gyro_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = GYRO_SUID;

    return &sensor_uid;
}

static sns_sensor_uid const* bmi160_motion_detect_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = MOTION_DETECT_SUID;

    return &sensor_uid;
}

static sns_sensor_uid const* bmi160_sensor_temp_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = SENSOR_TEMPERATURE_SUID;

    return &sensor_uid;
}



#if BMI160_CONFIG_POWER_RAIL
void bmi160_start_power_rail_timer(sns_sensor *const this,
        sns_time timeout_ticks,
        bmi160_power_rail_pending_state pwr_rail_pend_state)
{
    bmi160_state *sstate = (bmi160_state*)this->state->state;

    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    size_t req_len;
    uint8_t buffer[20];
    sns_memset(buffer, 0, sizeof(buffer));
    req_payload.is_periodic = false;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;

    if(NULL == sstate->timer_stream)
    {
        sns_service_manager *smgr = this->cb->get_service_manager(this);
        sns_stream_service  *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
        sns_sensor_uid      suid;

        sns_suid_lookup_get(&sstate->common.suid_lookup_data, "timer", &suid);


        stream_svc->api->create_sensor_stream(stream_svc, this, suid,
                &sstate->timer_stream);
    }

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
            sns_timer_sensor_config_fields, NULL);
    if(req_len > 0 && NULL != sstate->timer_stream)
    {
        sns_request timer_req =
        {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
            .request = buffer, .request_len = req_len  };
        sstate->timer_stream->api->send_request(sstate->timer_stream, &timer_req);
        sstate->power_rail_pend_state = pwr_rail_pend_state;

        BMI160_SENSOR_LOG(LOW, this, "timer requested by: %d", sstate->sensor);
    }
    else
    {
        BMI160_SENSOR_LOG(ERROR, this, "LSM timer req encode error");
    }
}
#endif


/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
static void bmi160_set_self_test_inst_config(sns_sensor *this,
        sns_sensor_instance *instance)
{

    sns_request config;

    config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
void bmi160_set_self_test_inst_config_bias(sns_sensor *this,
        sns_sensor_instance *instance)
{
    sns_request config;

    config.message_id = SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}
#endif

/**
 * send the flush request to the framework
 * @param this           sensor handler
 * @param instance       instance handler
 *
 * @return none
 */
static void bmi160_send_flush_config(sns_sensor * const this,
        sns_sensor_instance *instance)
{
    sns_request config;

    config.message_id = SNS_STD_MSGID_SNS_STD_FLUSH_REQ;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sorted in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool bmi160_get_decoded_imu_request(sns_sensor const *this, sns_request const *in_request,
        sns_std_request *decoded_request,
        sns_std_sensor_config *decoded_payload)
{
    UNUSED_VAR(this);
    pb_istream_t stream;
    pb_simple_cb_arg arg =
    { .decoded_struct = decoded_payload,
        .fields = sns_std_sensor_config_fields };
    decoded_request->payload = (struct pb_callback_s)
    { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(in_request->request,
            in_request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        BMI160_SENSOR_LOG(ERROR, this, "decode error");
        return false;
    }
    return true;
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successful else false.
 */
static bool bmi160_get_decoded_self_test_request(
        sns_sensor const        *this,
        sns_request const       *request,
        sns_std_request         *decoded_request,
        sns_physical_sensor_test_config *test_config)
{
    UNUSED_VAR(this);
    pb_istream_t stream;
    pb_simple_cb_arg arg =
    { .decoded_struct = test_config,
        .fields = sns_physical_sensor_test_config_fields };
    decoded_request->payload = (struct pb_callback_s)
    { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(request->request,
            request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        BMI160_SENSOR_LOG(ERROR, this, "BMI160 decode error");
        return false;
    }
    return true;
}


#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
static bool bmi160_get_decoded_self_test_request_bias(sns_sensor const *this, sns_request const *request,
        sns_std_request *decoded_request,
        sns_physical_sensor_oem_config *test_config)
{
    pb_istream_t stream;
    pb_simple_cb_arg arg =
    { 	.decoded_struct = test_config,
        .fields = sns_physical_sensor_oem_config_fields
    };
    decoded_request->payload = (struct pb_callback_s)
    {
        .funcs.decode = &pb_decode_simple_cb,
            .arg = &arg
    };
    stream = pb_istream_from_buffer(request->request, request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        BMI160_SENSOR_LOG(ERROR, this, "bmi160 :decode error");

        return false;
    }
    return true;
}
#endif


/**
 * get BMI160 ACC/Gyro configurations
 * @param this                                      sensor handler
 * @param instance                                  the valid instance handler
 * @param sensor_type                               sensor type from request
 * @param chosen_sample_rate                        chosen sample rate in hz
 * @param chosen_report_rate                        chosen report rate in hz
 * @param non_gated_sensor_client_present           non gated sensor client present flag
 * @param gated_sensor_client_present               gated sensor client present flag
 *
 * @return none
 */
static void bmi160_get_imu_config(sns_sensor *this,
        sns_sensor_instance *instance,
        bmi160_sensor_type  sensor_type,
        float               *chosen_sample_rate,
        float               *chosen_report_rate,
        sns_time            *chosen_flush_period_ticks,
        bool                *non_gated_sensor_client_present,
        bool                *gated_sensor_client_present)
{
#if BMI160_CONFIG_ENABLE_FLUSH_PERIOD
    UNUSED_VAR(this);
    sns_sensor_uid          suid;
    sns_request const       *request;
    bool is_max_batch = true;
    bool is_flush_only = true;

    if (BMI160_ACCEL == sensor_type)
    {
        sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)ACCEL_SUID), sizeof(sns_sensor_uid));
    }
    else if (BMI160_GYRO == sensor_type)
    {
        sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)GYRO_SUID), sizeof(sns_sensor_uid));
    }
#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
    else if (BMI160_MAG == sensor_type)
    {
    }
#endif
    else
    {
    }

    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *non_gated_sensor_client_present = false;
    *chosen_flush_period_ticks = 0;
    if (gated_sensor_client_present)
    {
        *gated_sensor_client_present = false;
    }

    /** Parse through existing requests and get fastest sample
     *  rate and report rate requests. */
    for(request = instance->cb->get_client_request(instance, &suid, true);
            NULL != request;
            request = instance->cb->get_client_request(instance, &suid, false))
    {
        sns_std_request         decoded_request;
        sns_std_sensor_config   decoded_payload = {0};

        if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
                ||
                request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if (bmi160_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
            {
                float report_rate = 0.0f;
                *chosen_sample_rate = SNS_MAX(*chosen_sample_rate, decoded_payload.sample_rate);
                if (decoded_request.has_batching)
                {
                    is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);


                    is_flush_only &= decoded_request.batching.flush_only;

                    if (decoded_request.batching.has_flush_period)
                    {
                        *chosen_flush_period_ticks =
                            SNS_MAX(*chosen_flush_period_ticks,
                                    sns_convert_ns_to_ticks(decoded_request.batching.flush_period*1000));
                    }
                    else
                    {
                        *chosen_flush_period_ticks =
                            SNS_MAX(*chosen_flush_period_ticks,
                                    sns_convert_ns_to_ticks(decoded_request.batching.batch_period*1000));
                    }
                }
                else
                {
                    report_rate = *chosen_sample_rate;
                    *chosen_flush_period_ticks = UINT64_MAX;
                    is_max_batch = false;
                    is_flush_only = false;
                }


                if(decoded_request.has_batching
                        &&
                        decoded_request.batching.batch_period > 0)
                {
                    report_rate = (1000000.0f / (float)decoded_request.batching.batch_period);
                }
                else
                {
                    report_rate = decoded_payload.sample_rate;
                }

                *chosen_report_rate = SNS_MAX(*chosen_report_rate,
                        report_rate);

                if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
                {
                    *non_gated_sensor_client_present = true;
                }
                else
                {
                    if (gated_sensor_client_present)
                    {
                        *gated_sensor_client_present = true;
                    }
                }
            }
        }
    }

    /** If there is a client for the sensor and
     *  if max_batch or flush_only are set in all requests then choose the largest WM. */
    if (is_max_batch)
    {
        if(*non_gated_sensor_client_present ||
                (gated_sensor_client_present && *gated_sensor_client_present))
        {
            *chosen_report_rate = (1.0f / UINT32_MAX);
            BMI160_INST_LOG(LOW, instance, "max_batch: %d", sensor_type);
        }
    }
#else
    UNUSED_VAR(this);
    UNUSED_VAR(chosen_flush_period_ticks);
    sns_sensor_uid          suid;
    sns_request const       *request;

    if (BMI160_ACCEL == sensor_type)
    {
        sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)ACCEL_SUID), sizeof(sns_sensor_uid));
    }
    else if (BMI160_GYRO == sensor_type)
    {
        sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)GYRO_SUID), sizeof(sns_sensor_uid));
    }
#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
    else if (BMI160_MAG == sensor_type)
    {
    }
#endif
    else
    {
    }

    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *non_gated_sensor_client_present = false;
    if (gated_sensor_client_present)
    {
        *gated_sensor_client_present = false;
    }

    /** Parse through existing requests and get fastest sample
     *  rate and report rate requests. */
    for(request = instance->cb->get_client_request(instance, &suid, true);
            NULL != request;
            request = instance->cb->get_client_request(instance, &suid, false))
    {
        sns_std_request         decoded_request;
        sns_std_sensor_config   decoded_payload = {0};

        if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
                ||
                request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if (bmi160_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
            {
                float report_rate;
                *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                        decoded_payload.sample_rate);
                if (decoded_request.has_batching
                        &&
                        decoded_request.batching.batch_period > 0)
                {
                    report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
                }
                else
                {
                    report_rate = *chosen_sample_rate;
                }
                *chosen_report_rate = SNS_MAX(*chosen_report_rate,
                        report_rate);

                if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
                {
                    *non_gated_sensor_client_present = true;
                }
                else
                {
                    if (gated_sensor_client_present)
                    {
                        *gated_sensor_client_present = true;
                    }
                }
            }
        }
    }
#endif
}

/**
 * Get motion detection configuration
 *
 * @param this                  the sensor handler
 * @param instance              the valid instance handler
 * @param chosen_md_enable      MD enable flag
 * @param md_client_present     MD client present flag
 *
 * @return none
 */
    static
void bmi160_get_motion_detect_config(
        sns_sensor              *this,
        sns_sensor_instance     *instance,
        bool                    *chosen_md_enable,
        bool                    *md_client_present)
{
    UNUSED_VAR(this);
    sns_sensor_uid              suid = MOTION_DETECT_SUID;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    bmi160_state                *sstate = (bmi160_state*)this->state->state;
    sns_request const           *request;

    UNUSED_VAR(sstate);
    *chosen_md_enable = false;
    *md_client_present = false; //TOCHECK


    for (request = instance->cb->get_client_request(instance, &suid, true);
            NULL != request;
            request = instance->cb->get_client_request(instance, &suid, false))
    {
        if (SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == request->message_id)
        {
            // Enable MD interrupt when:
            // 1. There is a new request and MD is in MDF/MDD state OR
            // 2. There is an existing request and MD is in MDE/MDD state
            // Introduced for power measurement testing,
            // Disable md interrupt using registry setting and send Disable event to md client,
            // if disable flag is true and client is present
#if BMI160_CONFIG_ENABLE_MD_TEST
            istate->md_info.client_present = true;
#endif

            BMI160_SENSOR_LOG(LOW, this, "MD sstate p:%d n:%d t:%d",
                    (int)istate->md_info.client_present, (int)istate->md_info.md_new_req, (int)istate->md_info.md_state.motion_detect_event_type);

            if(!istate->md_info.md_config.disable)
            {
#if 0
                // seems this is always true
                *chosen_md_enable = ((istate->md_info.md_new_req &&
                            (istate->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_FIRED
                             || istate->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_DISABLED))
                        ||
                        (istate->md_info.client_present &&
                         (istate->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED
                          || istate->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_DISABLED)));
#endif
                //FOLLOWED_LSM_EXAMPLE
                *chosen_md_enable = true;
            }

            //*chosen_md_enable = true;
            *md_client_present = true;
            // Consumed new request
            istate->md_info.md_new_req = false;

            return;
        }
    }
}


/**
 * get sensor temperature configuration
 * @param this                 sensor handler
 * @param instance             instance handler
 * @param chosen_sample_rate   chosen sample rate in hz
 * @param chosen_report_rate   chosen report rate in hz
 * @param sensor_temp_client_present  sensor temperature client present flag
 *
 * @return none
 */
static void bmi160_get_sensor_temp_config(sns_sensor *this,
        sns_sensor_instance *instance,
        float *chosen_sample_rate,
        float *chosen_report_rate,
        sns_time *chosen_flush_period_ticks,
        bool *sensor_temp_client_present)
{
#if  BMI160_CONFIG_ENABLE_FLUSH_PERIOD
    UNUSED_VAR(this);
    bmi160_instance_state *istate =
        (bmi160_instance_state*)instance->state->state;
    sns_sensor_uid suid = SENSOR_TEMPERATURE_SUID;
    sns_request const *request;
    bool is_max_batch = true;
    bool is_flush_only = true;

    *chosen_report_rate = 0.0f;
    *chosen_sample_rate = 0.0f;
    *chosen_flush_period_ticks = 0;
    *sensor_temp_client_present = false;

    /** Parse through existing requests and get fastest sample
     *  rate and report rate requests. */
    for (request = instance->cb->get_client_request(instance, &suid, true);
            NULL != request;
            request = instance->cb->get_client_request(instance, &suid, false))
    {
        sns_std_request decoded_request;
        sns_std_sensor_config decoded_payload = {0};

        if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if (bmi160_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
            {
                float report_rate = 0.0f;
                *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                        decoded_payload.sample_rate);

                if (decoded_request.has_batching
                        &&
                        decoded_request.batching.batch_period >0)
                {
                    report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
                }
                else
                {
                    report_rate = decoded_payload.sample_rate;
                }

                if(decoded_request.has_batching)
                {
                    is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);
                    is_flush_only &= (decoded_request.batching.flush_only);

                    if(decoded_request.batching.has_flush_period)
                    {
                        *chosen_flush_period_ticks =
                            SNS_MAX(*chosen_flush_period_ticks,
                                    sns_convert_ns_to_ticks(decoded_request.batching.flush_period*1000));
                    }
                    else
                    {
                        *chosen_flush_period_ticks =
                            SNS_MAX(*chosen_flush_period_ticks,
                                    sns_convert_ns_to_ticks(decoded_request.batching.batch_period*1000));
                    }
                }

                *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
                *sensor_temp_client_present = true;
            }
        }
    }

    if(*sensor_temp_client_present &&
            (is_max_batch))
    {
        *chosen_report_rate = (1.0f / UINT32_MAX);
    }

    istate->sensor_temp_info.report_rate_req  = *chosen_report_rate;
    istate->sensor_temp_info.sample_rate_req = *chosen_sample_rate;
#else
    UNUSED_VAR(this);
    bmi160_instance_state *istate =
        (bmi160_instance_state*)instance->state->state;
    sns_sensor_uid suid = SENSOR_TEMPERATURE_SUID;
    sns_request const *request;

    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *sensor_temp_client_present = false;

    /** Parse through existing requests and get fastest sample
     *  rate and report rate requests. */
    for (request = instance->cb->get_client_request(instance, &suid, true);
            NULL != request;
            request = instance->cb->get_client_request(instance, &suid, false))
    {
        sns_std_request decoded_request;
        sns_std_sensor_config decoded_payload = {0};

        if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if (bmi160_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
            {
                float report_rate;
                *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                        decoded_payload.sample_rate);

                bool rc = sns_sensor_util_decide_max_batch(instance, &suid);
                //There is request with max batch not set .
                // do normal calculation

                if (!rc) {
                    if (decoded_request.has_batching
                            &&
                            decoded_request.batching.batch_period> 0)
                    {
                        report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
                    }
                    else
                    {
                        report_rate = *chosen_sample_rate;
                    }
                } else {
                    report_rate = (1000000.0 / (float)UINT32_MAX);
                }

                *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
                *sensor_temp_client_present = true;
            }
        }
    }
    istate->sensor_temp_info.report_rate_req  = *chosen_report_rate;
    istate->sensor_temp_info.sample_rate_req = *chosen_sample_rate;
#endif
}


/**
 *
 * @param this
 * @param instance
 * @param registry_cfg
 * @param message_id
 */
static void bmi160_set_inst_config(
        sns_sensor                  *this,
        sns_sensor_instance         *instance,
        bmi160_req_payload          req_payload,
        uint32_t                    message_id)
{
    sns_request                     config;

    config.message_id = message_id;
    config.request_len = sizeof(req_payload);
    config.request = &req_payload;


    this->instance_api->set_client_config(instance, &config);
}

#if BMI160_CONFIG_POWER_RAIL
/**
 * Turn off power rails
 * @param this   sensor handler
 */
static void bmi160_turn_rails_off(sns_sensor *this)
{
    sns_sensor *sensor;

    for(sensor = this->cb->get_library_sensor(this, true);
            NULL != sensor;
            sensor = this->cb->get_library_sensor(this, false))
    {
        bmi160_state *sensor_state = (bmi160_state*)sensor->state->state;
        if (sensor_state->common.rail_config.rail_vote != SNS_RAIL_OFF)
        {
            sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
            sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                    sensor,
                    &sensor_state->common.rail_config,
                    NULL);
        }
    }

}
#endif

    static
bool bmi160_check_n_approve_md_req(
        bmi160_state            *sstate,
        bmi160_instance_state   *istate)
{
    bool approve_req = true;
    UNUSED_VAR(sstate);

    BMI160_SENSOR_LOG(LOW, sstate->owner, "get MD req");
    if (istate->fifo_info.publish_sensors & BMI160_ACCEL)
    {
        //send event as MD disabled since non-gated client is active
        //no need of this as we alreay set md_info state
        sns_motion_detect_event md_state;
        md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
        pb_send_event(istate->owner,
                sns_motion_detect_event_fields,
                &md_state,
                bmi160_get_sys_tick(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &istate->md_info.sstate->my_suid);
        //as per requirement: reject the request directly
        BMI160_SENSOR_LOG(HIGH, sstate->owner, "MD request rejected");
        approve_req = false;
    }
    //FOLLOWED_LSM_EXAMPLE
    else if (istate->int_en_flags_req.bits.md)
    {
        approve_req = false;
        //there is exsisting md client already present, just send event
        pb_send_event(istate->owner,
                sns_motion_detect_event_fields,
                &istate->md_info.md_state,
                bmi160_get_sys_tick(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &istate->md_info.sstate->my_suid);
        BMI160_SENSOR_LOG(HIGH, sstate->owner, "there is already existing md client");
    }
    else
    {
        istate->md_info.md_new_req = true;
    }

    return approve_req;
}


/**
 * Decodes a physical sensor self test request and updates
 * instance state with request info.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[i] new_request Encoded request
 *
 * @return True if request is valid else false
 */
static bool bmi160_extract_self_test_info(
        sns_sensor          *this,
        sns_sensor_instance *instance,
        struct sns_request const *new_request)
{
    sns_std_request         decoded_request;
    sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
    bmi160_state            *sstate = (bmi160_state*)this->state->state;
    bmi160_instance_state   *istate = (bmi160_instance_state*)instance->state->state;
    bmi160_self_test_info   *self_test_info;

    if (sstate->sensor == BMI160_ACCEL)
    {
        self_test_info = &istate->accel_info.test_info;
    }
    else if (sstate->sensor == BMI160_GYRO)
    {
        self_test_info = &istate->gyro_info.test_info;
    }
    else if (sstate->sensor == BMI160_MOTION_DETECT)
    {
        self_test_info = &istate->md_info.test_info;
    }
    else if (sstate->sensor == BMI160_SENSOR_TEMP)
    {
        self_test_info = &istate->sensor_temp_info.test_info;
    }
    else
    {
        return false;
    }

    self_test_info->sensor = sstate->sensor;

    if (bmi160_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
    {
        self_test_info->test_type = test_config.test_type;
        self_test_info->test_client_present = true;

        BMI160_SENSOR_LOG(MED, this, "new client_request to do test: %d %d", sstate->sensor, self_test_info->test_type);
        return true;
    }
    else
    {
        return false;
    }
}

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
static bool bmi160_extract_self_test_info_bias(sns_sensor *this,
        sns_sensor_instance *instance,
        struct sns_request const *new_request)
{
    sns_std_request decoded_request;
    sns_physical_sensor_oem_config test_config = sns_physical_sensor_oem_config_init_default;
    bmi160_state *state = (bmi160_state*)this->state->state;
    bmi160_instance_state *inst_state = (bmi160_instance_state*)instance->state->state;
    bmi160_self_test_info_bias *self_test_info_bias;

    if(state->sensor == BMI160_ACCEL)
    {
        self_test_info_bias = &inst_state->accel_info.test_info_bias;
    }
    else if(state->sensor == BMI160_GYRO)
    {
        self_test_info_bias = &inst_state->gyro_info.test_info_bias;
    }
    else
    {
        return false;
    }

    if(bmi160_get_decoded_self_test_request_bias(this, new_request, &decoded_request, &test_config))
    {
        self_test_info_bias->config_type = test_config.config_type;
        self_test_info_bias->test_client_present = true;

        self_test_info_bias->offset_x = test_config.offset_x;
        self_test_info_bias->offset_y = test_config.offset_y;
        self_test_info_bias->offset_z = test_config.offset_z;

        BMI160_SENSOR_LOG(HIGH, this, "bmi160 : bmi16_extract_self_test_info_bias [config_type:%d][x:%d][y:%d][z:%d]",
                test_config.config_type,
                (int)(test_config.offset_x*10),
                (int)(test_config.offset_y*10),
                (int)(test_config.offset_z*10));
        return true;
    }
    else
    {
        return false;
    }
}
#endif


/** Functions shared by all BMI160 Sensors */
/**
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 *
 * @param[i] this          Sensor reference
 * @param[i] instance      Sensor Instance to config
 * @param[i] sensor_type   Sensor type
 *
 * @return none
 *
 */

    static
void bmi160_reval_instance_config(
        sns_sensor              *this,
        sns_sensor_instance     *instance,
        bmi160_sensor_type      sensor_type)
{
    /**
     * 1. Get best non-gated and gated Accel Config.
     * 2. Get best Gyro Config.
     * 3. Get Motion Detect Config.
     * 4. Get best Sensor Temperature Config.
     * 5. Decide best Instance Config based on above outputs.
     */
    UNUSED_VAR(sensor_type);
    bmi160_state                *sstate = (bmi160_state*)this->state->state;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    float                       sample_rate = 0;
    float                       report_rate = 0;
    bool                        a_sensor_client_present_non_gated = false;
    //only accel sensor is gated
    bool                        a_sensor_client_present_gated = false;
    bool                        g_sensor_client_present = false;
    bool                        md_sensor_client_present = false;
    bool                        sensor_temp_client_present = false;
    bool                        enable_md_int = false;
    sns_time                    acc_flush_period_ticks     = 0;
    sns_time                    gyro_flush_period_ticks    = 0;
    sns_time                    temp_flush_period_ticks    = 0;

    BMI160_INST_LOG(LOW, instance, "reval instance entrance from sensor:%d", sstate->sensor);

    bmi160_req_payload          req_payload = {.sensor_type = sensor_type};

    UNUSED_VAR(sstate);


    if (!istate->fac_test_in_progress) {
    } else {
        BMI160_INST_LOG(MED, instance, "NOTICE!!! inst_cfg postponed due to fac_test_in_progress: %d", istate->fac_test_info.fac_test_sensor);
        return;
    }

    bmi160_get_imu_config(this,
            instance,
            BMI160_ACCEL,
            &sample_rate,
            &report_rate,
            &acc_flush_period_ticks,
            &a_sensor_client_present_non_gated,
            &a_sensor_client_present_gated);

    istate->accel_info.sample_rate_req = sample_rate;
    istate->accel_info.report_rate_req = report_rate;
    istate->accel_info.gated_client_present = a_sensor_client_present_gated;
    istate->accel_info.client_present = a_sensor_client_present_non_gated;
    istate->accel_info.flush_period_ticks = acc_flush_period_ticks;

    BMI160_INST_LOG(MED, instance, "cfg_acc cp:%d, rates sr:%d, rr:%d fp:%u",
            ((a_sensor_client_present_gated<<1) | (a_sensor_client_present_non_gated&1)),
            (int)(sample_rate * 1000), (int)(report_rate * 1000),
            (uint32_t)acc_flush_period_ticks);

    if (a_sensor_client_present_gated && a_sensor_client_present_non_gated) {
        BMI160_SENSOR_LOG(MED, this, "NOTICE both gated and non-gated clients are present");
    }


    bmi160_get_imu_config(this,
            instance,
            BMI160_GYRO,
            &sample_rate,
            &report_rate,
            &gyro_flush_period_ticks,
            &g_sensor_client_present,
            NULL);

    istate->gyro_info.sample_rate_req = sample_rate;
    istate->gyro_info.report_rate_req = report_rate;
    istate->gyro_info.client_present = g_sensor_client_present;
    istate->gyro_info.flush_period_ticks = gyro_flush_period_ticks;

    BMI160_INST_LOG(MED, instance, "cfg_gyro cp:%d, rates sr:%d, rr:%d fp:%u",
            g_sensor_client_present,
            (int)(sample_rate * 1000), (int)(report_rate * 1000),
            (uint32_t)gyro_flush_period_ticks);


#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
#endif

    bmi160_get_motion_detect_config(this,
            instance,
            &enable_md_int,
            &md_sensor_client_present);

    istate->int_en_flags_req.bits.md = enable_md_int;
    istate->md_info.client_present = md_sensor_client_present;

    BMI160_SENSOR_LOG(MED, this, "cfg_md cp:%d, en:%d",
            istate->md_info.client_present,
            istate->int_en_flags_req.bits.md);

    if (a_sensor_client_present_non_gated)
    {
        istate->int_en_flags_req.bits.md = false;
    }


    bmi160_get_sensor_temp_config(this,
            instance,
            &sample_rate,
            &report_rate,
            &temp_flush_period_ticks,
            &sensor_temp_client_present);

    istate->sensor_temp_info.client_present = sensor_temp_client_present;
    //istate->sensor_temp_info.max_requested_flush_ticks = temp_flush_period_ticks;

    BMI160_INST_LOG(MED, instance, "cfg_temp cp:%d, rates sr:%d, rr:%d fp:%u",
            sensor_temp_client_present,
            (int)(sample_rate * 1000), (int)(report_rate * 1000),
            (uint32_t) temp_flush_period_ticks);

    /** Start with a clean sstate */
    istate->hw_mod_needed = 0;
    istate->fifo_info.publish_sensors = 0;

    if (a_sensor_client_present_non_gated)
    {
        istate->hw_mod_needed |= BMI160_ACCEL;
        istate->fifo_info.publish_sensors |= BMI160_ACCEL;
    }

    if (a_sensor_client_present_gated)
    {
        istate->hw_mod_needed |= BMI160_ACCEL;
        if (!istate->int_en_flags_req.bits.md)
        {
            istate->fifo_info.publish_sensors |= BMI160_ACCEL;
        }
    }

    if (g_sensor_client_present)
    {
        istate->hw_mod_needed |= (BMI160_ACCEL | BMI160_GYRO); //TOCHECK
        istate->fifo_info.publish_sensors |= BMI160_GYRO;
    }

#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
#endif

    if (md_sensor_client_present)
    {
        istate->hw_mod_needed |= BMI160_ACCEL;
    }

    if (sensor_temp_client_present)
    {
        istate->hw_mod_needed |= BMI160_ACCEL;
        istate->fifo_info.publish_sensors |= BMI160_SENSOR_TEMP;
    }

    BMI160_SENSOR_LOG(MED, this, "inst_cfg summary ps:0x%x, hmn:0x%x",
            istate->fifo_info.publish_sensors,
            istate->hw_mod_needed);

    bmi160_set_inst_config(this,
            instance,
            req_payload,
            SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);

#if 0
    if (!istate->hw_mod_needed && !istate->new_self_test_request)
    {
#if BMI160_CONFIG_POWER_RAIL
        bmi160_turn_rails_off(this);
        BMI160_SENSOR_LOG(MED, this, "sensor: %d req to turn off rails", sstate->sensor);
#endif
        //TODO2: check more
        istate->instance_is_ready_to_configure = false;
        istate->sbus_in_normal_mode = false;
        istate->pmu_stat.reg = 0;
    }
#endif
}


/** See sns_bmi160_sensor.h*/
sns_rc bmi160_sensor_notify_event(sns_sensor *const this)
{
    bmi160_state            *sstate = (bmi160_state*)this->state->state;
    sns_rc                  rv = SNS_RC_SUCCESS;
    sns_sensor_event        *event;


    BMI160_SENSOR_LOG(MED, this, "<bmi160_if_ notify_event: %d this: 0x%x>", sstate->sensor, this);

    if (!sns_suid_lookup_complete(&sstate->common.suid_lookup_data))
    {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
        sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif
        sns_suid_lookup_handle(this, &sstate->common.suid_lookup_data);

#if BMI160_CONFIG_ENABLE_REGISTRY
        if(sns_suid_lookup_get(&sstate->common.suid_lookup_data, "registry", NULL))
        {
            bmi160_request_registry(this);
        }
#else
        sns_bmi160_registry_def_config(this);
#endif
        BMI160_SENSOR_LOG(LOW, this, "snr: %d r_idx=%d", sstate->sensor, sstate->resolution_idx);

        if (sns_suid_lookup_complete(&sstate->common.suid_lookup_data))
        {
            sns_suid_lookup_deinit(this, &sstate->common.suid_lookup_data);
        }
    }
    else
    {
        BMI160_SENSOR_LOG(LOW, this, "look up complete");
    }

    /**----------------------Handle a Timer Sensor event.-------------------*/
    if (NULL != sstate->timer_stream)
    {
        event = sstate->timer_stream->api->peek_input(sstate->timer_stream);
        while(NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                    event->event_len);
            sns_timer_sensor_event timer_event;
            if (SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
            {

                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
#if BMI160_CONFIG_POWER_RAIL
                    if (sstate->power_rail_pend_state == BMI160_POWER_RAIL_PENDING_INIT)
                    {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
                        /** Initial HW discovery is OK to run in normal mode. */
                        sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif


                        sstate->common.hw_is_present = bmi160_discover_hw(this);
                        if (sstate->common.hw_is_present)
                        {
                            BMI160_SENSOR_LOG(MED, this, "sensor:%d init finished", sstate->sensor);
                            bmi160_publish_available(this);
                            bmi160_update_sibling_sensors(this);
                        }
                        else
                        {
                            rv = SNS_RC_INVALID_LIBRARY_STATE;
                            BMI160_SENSOR_LOG(ERROR, this, "BMI160 HW absent");
                        }
                        sstate->power_rail_pend_state = BMI160_POWER_RAIL_PENDING_NONE;
                    }
                    else if (sstate->power_rail_pend_state == BMI160_POWER_RAIL_PENDING_SET_CLIENT_REQ)
                    {
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);

                        BMI160_SENSOR_LOG(LOW, this, "timer event for instance: %p set_client_request snr: %d", instance, sstate->sensor);
                        if (NULL != instance)
                        {
                            bmi160_instance_state *istate =
                                (bmi160_instance_state*) instance->state->state;
                            istate->instance_is_ready_to_configure = true;
                            bmi160_reval_instance_config(this, instance, sstate->sensor);

                            if (istate->new_self_test_request)
                            {
                                bmi160_set_self_test_inst_config(this, instance);
                            }
#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
                            if(istate->new_self_test_request_bias)
                            {
                                bmi160_set_self_test_inst_config_bias(this, instance);
                            }
#endif


                        }
                        sstate->power_rail_pend_state = BMI160_POWER_RAIL_PENDING_NONE;
                    }
#endif  //BMI160_CONFIG_POWER_RAIL
                }
                else
                {
                    BMI160_SENSOR_LOG(ERROR, this, "pb_decode error");
                }
            }

            event = sstate->timer_stream->api->get_next_input(sstate->timer_stream);
        }

        /** Free up timer stream if not needed anymore */
        if (sstate->power_rail_pend_state == BMI160_POWER_RAIL_PENDING_NONE)
        {
            sns_sensor_util_remove_sensor_stream(this, &sstate->timer_stream);
        }
    }

#if BMI160_CONFIG_ENABLE_REGISTRY
    if (NULL != sstate->reg_data_stream)
    {
        event = sstate->reg_data_stream->api->peek_input(sstate->reg_data_stream);
        while (NULL != event)
        {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
            /** All registry events can be handled in normal mode. */
            sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif
            bmi160_sensor_process_registry_event(this, event);

            event = sstate->reg_data_stream->api->get_next_input(sstate->reg_data_stream);
        }

#if BMI160_CONFIG_ENABLE_ISLAND_MODE
        sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif
        bmi160_sensor_check_registry_col_progress(this);
    }
#endif

    //note: all the registry items must be provided
    if (sns_suid_lookup_get(&sstate->common.suid_lookup_data, "timer", NULL) &&
            sstate->common.registry_pf_cfg_received && sstate->registry_cfg_received &&
            sstate->common.registry_orient_received && sstate->common.registry_placement_received)
    {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
        /** Initial HW detection sequence is OK to run in normal mode. */
        sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif

        if (!sstate->common.hw_is_present) {
            BMI160_SENSOR_LOG(MED, this, "start_hw_detect_seq snr:%d", sstate->sensor);
            bmi160_start_hw_detect_sequence(this);
        }
    }

    return rv;
}


/** See sns_bmi160_sensor.h */
sns_sensor_instance* bmi160_set_client_request(
        sns_sensor          *const this,
        struct sns_request  const *exist_request,
        struct sns_request  const *new_request,
        bool                remove)
{
    sns_sensor_instance     *instance = sns_sensor_util_get_shared_instance(this);
    bmi160_state            *sstate = (bmi160_state*)this->state->state;
    bmi160_instance_state   *istate = NULL;
    sns_time                on_timestamp;
    sns_time                delta;
    bool                    reval_config = false;



    BMI160_SENSOR_LOG(MED, this, "<bmi160_sif_ set_client_request> snr:%d",
            sstate->sensor);
    BMI160_SENSOR_LOG(MED, this, "<bmi160_sif_ set_client_request> ereq:0x%x nreq:0x%x r:%d",
            exist_request, new_request, remove);

    if (remove)
    {
        if (NULL != instance)
        {
            BMI160_SENSOR_LOG(LOW, this, "remove exist_request");
            instance->cb->remove_client_request(instance, exist_request);
            /* Assumption: The FW will call deinit() on the instance before destroying it.
               Putting all HW resources (sensor HW, COM port, power rail)in
               low power sstate happens in Instance deinit().*/

            bmi160_reval_instance_config(this, instance, sstate->sensor);
        }
    }
    else
    {
        //false == remove
        // 1. If new request then:
        //     a. Power ON rails.
        //     b. Power ON COM port - Instance must handle COM port power.
        //     c. Create new instance.
        //     d. Re-evaluate existing requests and choose appropriate instance config.
        //     e. set_client_config for this instance.
        //     f. Add new_request to list of requests handled by the Instance.
        //     g. Power OFF COM port if not needed- Instance must handle COM port power.
        //     h. Return the Instance.
        // 2. If there is an Instance already present:
        //     a. Add new_request to list of requests handled by the Instance.
        //     b. Remove exist_request from list of requests handled by the Instance.
        //     c. Re-evaluate existing requests and choose appropriate Instance config.
        //     d. set_client_config for the Instance if not the same as current config.
        //     e. publish the updated config.
        //     f. Return the Instance.
        // 3.  If "flush" request:
        //     a. Perform flush on the instance.
        //     b. Return NULL.

        if (NULL == instance)
        {
#if BMI160_CONFIG_POWER_RAIL
            if (sstate->sensor == BMI160_GYRO)
            {
                sstate->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
            }
            else
            {
                //TOCHECK
                sstate->common.rail_config.rail_vote = sstate->common.registry_rail_on_state;
            }
            sstate->pwr_rail_service->api->sns_vote_power_rail_update(
                    sstate->pwr_rail_service,
                    this,
                    &sstate->common.rail_config,
                    &on_timestamp);

            BST_ASSUME_TS_IS_64BIT
            delta = sns_get_system_time() - on_timestamp;

            // Use on_timestamp to determine correct Timer value.
            if (delta < sns_convert_ns_to_ticks(BMI160_OFF_TO_IDLE_MS * 1000 * 1000))
            {
                bmi160_start_power_rail_timer(this,
                        sns_convert_ns_to_ticks(BMI160_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
                        BMI160_POWER_RAIL_PENDING_SET_CLIENT_REQ);
            }
            else
            {
                // rail is already ON for time long enough
                reval_config = true;
                BMI160_SENSOR_LOG(MED, this, "rail already on: %u %u", BMI160_SYS_TIME_LH(on_timestamp), BMI160_SYS_TIME_LH(delta));
            }
#else
            UNUSED_VAR(on_timestamp);
            UNUSED_VAR(delta);
            reval_config = true;
#endif

            /** create_instance() calls init() for the Sensor Instance */
            instance = this->cb->create_instance(this,
                    sizeof(bmi160_instance_state));

            istate = (bmi160_instance_state*)instance->state->state;
            BMI160_SENSOR_LOG(LOW, this, "instance created: 0x%x istate:0x%x, 0x%x", instance, istate, bmi160_inst_singleton);
            if (NULL == bmi160_inst_singleton) {
                bmi160_inst_singleton = istate;
            }
            /* If rail is already ON then flag instance OK to configure */
            if (reval_config)
            {
                istate->instance_is_ready_to_configure = true;
            }
        }
        else
        {
            if (NULL != exist_request
                    &&
                    NULL != new_request
                    &&
                    new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
            {
                istate = (bmi160_instance_state*)instance->state->state;

                BMI160_SENSOR_LOG(MED, this, "flush req: %d@%u", istate->fifo_info.ff_sensors_en_curr, BMI160_SYS_TIME_LH(sns_get_system_time()));

                if (istate->fifo_info.ff_sensors_en_curr)
                {
                    //TOCHECK
                    bmi160_send_flush_config(this, instance);
                    /** Do not update instance client request list at this point
                      because FIFO flush is a transitory request for an on-going
                      stream request. */
                    return instance;
                }
                else
                {
                    /** There aren't any FIFO sensors enabled to support flush */
                    /*  Send flush complete event anyway. */
                    sns_time ts_curr = bmi160_get_sys_tick();
                    BMI160_SENSOR_LOG(MED, this, "fifo_flush_done event: %d@%u", istate->fifo_info.publish_sensors, BMI160_SYS_TIME_LH(ts_curr));
                    bmi160_send_fifo_flush_done(instance, &sstate->my_suid, ts_curr, BMI160_FIFO_FLUSH_DONE_CTX_CLIENT_RES_NOW);
                    return NULL;
                }
            }
            else
            {
                reval_config = true;

                /** An existing client is changing request*/
                if ((NULL != exist_request) && (NULL != new_request))
                {
                    BMI160_SENSOR_LOG(LOW, this, "remove exist_request due to change of req");
                    instance->cb->remove_client_request(instance, exist_request);
                }
                /** A new client sent new_request*/
                else if (NULL != new_request)
                {
                    // No-op. new_request will be added to requests list below.
                }
            }
        }

        /** Add the new request to list of client_requests.*/
        if (NULL != instance)
        {
            istate = (bmi160_instance_state*)instance->state->state;
            if (NULL != new_request)
            {
                instance->cb->add_client_request(instance, new_request);

                if ((new_request->message_id ==
                            SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
                        &&
                        (BMI160_MOTION_DETECT == sstate->sensor))
                {
                    if (!bmi160_check_n_approve_md_req(sstate, istate)) {
                        reval_config = false;
                    }
                }

                if (new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET) {
                    bool    op_valid = false;

                    //TODO2: how about temperature
                    if ((BMI160_ACCEL == sstate->sensor) || (BMI160_GYRO == sstate->sensor)) {
                        op_valid = true;
                    }

                    BMI160_SENSOR_LOG(MED, this, "Request for resetting cal data: %d");
                    if (op_valid) {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
                        sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif
                        bmi160_reset_fac_cal_data(instance, sstate, true);

#if BMI160_CONFIG_ENABLE_REGISTRY
                        bmi160_update_registry(this, instance, sstate->sensor);
#endif
                        bmi160_send_fac_cal_event(instance, sstate);
                    } else {
                    }

                    reval_config = false;
                }

                if (new_request->message_id ==
                        SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
                {
                    if (bmi160_extract_self_test_info(this, instance, new_request))
                    {
                        istate->new_self_test_request = true;
                    }
                } else {
                }
#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
                if (new_request->message_id ==
                        SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG)
                {
                    BMI160_SENSOR_LOG(MED, this, "SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG is called");
                    if (bmi160_extract_self_test_info_bias(this, instance, new_request))
                    {
                        istate->new_self_test_request_bias = true;
                    }
                }
#endif
            }

            if (reval_config && istate->instance_is_ready_to_configure)
            {
                bmi160_reval_instance_config(this, instance, sstate->sensor);

                if (istate->new_self_test_request)
                {
                    bmi160_set_self_test_inst_config(this, instance);
                }

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
                if (istate->new_self_test_request_bias)
                {
                    bmi160_set_self_test_inst_config_bias(this, instance);
                }
#endif
            }
        }
    }

#if BMI160_CONFIG_ENABLE_REGISTRY
    if (NULL != instance) {
        istate = (bmi160_instance_state*)instance->state->state;
        if (istate->update_fac_cal_in_registry)
        {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
            /** Registry write can be handled in normal mode. */
            sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif

            bmi160_update_registry(this, instance, BMI160_ACCEL);
            bmi160_update_registry(this, instance, BMI160_GYRO);

            istate->update_fac_cal_in_registry = false;
        }
    }
#endif

    // QC: Sensors are required to call remove_instance when clientless
    if (NULL != instance &&
            NULL == instance->cb->get_client_request(instance,
                &(sns_sensor_uid)MOTION_DETECT_SUID, true) &&
            NULL == instance->cb->get_client_request(instance,
                &(sns_sensor_uid)ACCEL_SUID, true) &&
            NULL == instance->cb->get_client_request(instance,
                &(sns_sensor_uid)GYRO_SUID, true) &&
            NULL == instance->cb->get_client_request(instance,
                &(sns_sensor_uid)SENSOR_TEMPERATURE_SUID, true))
    {
        BMI160_SENSOR_LOG(MED, this, "instance has no clients, will be removed");
        this->cb->remove_instance(instance);
#if BMI160_CONFIG_POWER_RAIL
        bmi160_turn_rails_off(this);
#endif
    }

    return instance;
}




sns_sensor_api bmi160_accel_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &bmi160_accel_init,
    .deinit             = &bmi160_accel_deinit,
    .get_sensor_uid     = &bmi160_accel_get_sensor_uid,
    .set_client_request = &bmi160_set_client_request,
    .notify_event       = &bmi160_sensor_notify_event,
};

sns_sensor_api bmi160_gyro_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &bmi160_gyro_init,
    .deinit             = &bmi160_gyro_deinit,
    .get_sensor_uid     = &bmi160_gyro_get_sensor_uid,
    .set_client_request = &bmi160_set_client_request,
    .notify_event       = &bmi160_sensor_notify_event,
};

sns_sensor_api bmi160_motion_detect_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &bmi160_motion_detect_init,
    .deinit             = &bmi160_motion_detect_deinit,
    .get_sensor_uid     = &bmi160_motion_detect_get_sensor_uid,
    .set_client_request = &bmi160_set_client_request,
    .notify_event       = &bmi160_sensor_notify_event,
};

sns_sensor_api bmi160_sensor_temp_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &bmi160_sensor_temp_init,
    .deinit             = &bmi160_sensor_temp_deinit,
    .get_sensor_uid     = &bmi160_sensor_temp_get_sensor_uid,
    .set_client_request = &bmi160_set_client_request,
    .notify_event       = &bmi160_sensor_notify_event,
};

