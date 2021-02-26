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
 * @file sns_bmi160_sensor_instance_island.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include "sns_island_service.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_event_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_bmi160_hal.h"
#include "sns_bmi160_sensor.h"
#include "sns_bmi160_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_std_event_gated_sensor.pb.h"

#if BMI160_CONFIG_ENABLE_DIAG_LOG
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#endif

#include "sns_sync_com_port_service.h"
#include "sns_cal_util.h"



/**
 * validate the sensor temperature odr
 * @param istate   instance state handler
 *
 * @return  SNS_RC_SUCCESS on success  otherwise value on failure
 */
    static
sns_rc bmi160_validate_sensor_temp_odr(bmi160_instance_state *istate)
{
    sns_rc rc = SNS_RC_SUCCESS;

    if (istate->sensor_temp_info.sample_rate_req <= BMI160_SENSOR_TEMP_ODR_1)
    {
        istate->sensor_temp_info.sample_rate_req = BMI160_SENSOR_TEMP_ODR_1;
    } else
#if BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR
        if (istate->sensor_temp_info.sample_rate_req <= BMI160_SENSOR_TEMP_ODR_5)
        {
            istate->sensor_temp_info.sample_rate_req = BMI160_SENSOR_TEMP_ODR_5;
        }
        else
        {
            istate->sensor_temp_info.sample_rate_req = 0;
            istate->sensor_temp_info.sampling_intvl = 0;
            rc = SNS_RC_NOT_SUPPORTED;
        }
#else
    {
        istate->sensor_temp_info.sample_rate_req = BMI160_SENSOR_TEMP_ODR_5;
    }
#endif

    if ((rc == SNS_RC_SUCCESS) && !BST_IS_FLOAT_ZERO(istate->sensor_temp_info.sample_rate_req))
    {
        sns_time                sampling_intvl = istate->sensor_temp_info.sampling_intvl;
        istate->sensor_temp_info.sampling_intvl = sns_convert_ns_to_ticks(1000000000.0 / istate->sensor_temp_info.sample_rate_req);

        istate->sensor_temp_info.timer_itvl_changed = (sampling_intvl != istate->sensor_temp_info.sampling_intvl);
    }

    return rc;
}

extern struct bmi160_odr_regv_map BMI160_REGV_ODR_MAP[];

//is_max_batch
    static
void bmi160_inst_determine_max_wml_req(bmi160_instance_state *istate)
{
    uint32_t wml_max = 0;

    uint8_t  odr_wml_max = BMI160_ODR_0;
    float    scale_wml;

    if (istate->accel_info.ff_wml_req > 0) {
        wml_max = istate->accel_info.ff_wml_req;
        odr_wml_max = istate->accel_info.odr_req;
    }

    if (istate->gyro_info.ff_wml_req > istate->accel_info.ff_wml_req) {
        wml_max = istate->gyro_info.ff_wml_req;
        odr_wml_max = istate->gyro_info.odr_req;
    }
    //TODOMAG

    wml_max = (wml_max > 0) ? wml_max : BMI160_FF_MAX_FRAMES_IMU;

    if (istate->fifo_info.publish_sensors & BMI160_ACCEL) {
        if (0 == istate->accel_info.ff_wml_req) {   //this is true when is_max_batch is set
            if (odr_wml_max > BMI160_ODR_0) {
                scale_wml = BMI160_REGV_ODR_MAP[istate->accel_info.odr_req].odr / BMI160_REGV_ODR_MAP[odr_wml_max].odr;
                istate->accel_info.ff_wml_req = wml_max * scale_wml;
            } else {
                istate->accel_info.ff_wml_req = wml_max;
            }
        }
    }

    if (istate->fifo_info.publish_sensors & BMI160_GYRO) {
        if (0 == istate->gyro_info.ff_wml_req) {
            if (odr_wml_max > BMI160_ODR_0) {
                scale_wml = BMI160_REGV_ODR_MAP[istate->gyro_info.odr_req].odr / BMI160_REGV_ODR_MAP[odr_wml_max].odr;
                istate->gyro_info.ff_wml_req = wml_max * scale_wml;
            } else {
                istate->gyro_info.ff_wml_req = wml_max;
            }
        }
    }
    //TODOMAG
}

#if BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR
void bmi160_inst_report_error(
        bmi160_instance_state   *istate,
        bmi160_sensor_type      sensor_type,
        sns_sensor_uid          *uid,
        uint32_t                error)
{
    sns_sensor_instance         *this = istate->owner;
    sns_service_manager         *mgr = this->cb->get_service_manager(this);
    sns_event_service           *event_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event            *event = event_service->api->alloc_event(event_service, this, 0);
    UNUSED_VAR(sensor_type);
    UNUSED_VAR(error);

    if (NULL != event)
    {
        event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
        event->event_len = 0;
        event->timestamp = sns_get_system_time();

        //event->event_len = sizeof(event->event);
        event->event[0] = (sensor_type << 16) | error;
        //use helper publish_error()
        //OPTIM3
        event_service->api->publish_event(event_service, this, event, uid);
        BMI160_INST_LOG(ERROR, istate->owner, "std_error_ev %d %d", sensor_type, error);
    }
}
#endif
/**
 * Access all the requests
 * @param istate    instance state handler
 *
 * @return  none
 */
void bmi160_inst_assess_overall_req(bmi160_instance_state *istate)
{
    bmi160_regv_odr_t   regv_odr;
    sns_rc              rc;
    float               odr_matched;

    //accel_info
    if (istate->accel_info.sample_rate_req > 0) {
        istate->accel_info.sample_rate_req = SNS_MAX(istate->accel_info.sample_rate_req, BMI160_CONFIG_ACC_LOWEST_ODR);

        if (istate->accel_info.sample_rate_req > BMI160_CONFIG_ACC_FASTEST_ODR) {
            istate->accel_info.sample_rate_req = BMI160_CONFIG_ACC_FASTEST_ODR;
#if BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR
            bmi160_inst_report_error(istate, BMI160_ACCEL, &istate->accel_info.sstate->my_suid, 0);
#endif
        }
    }


    bmi160_hal_match_odr(BMI160_ACCEL, istate->accel_info.sample_rate_req, &odr_matched, &regv_odr);


    istate->accel_info.odr_req = regv_odr;

    if (istate->accel_info.report_rate_req > odr_matched) {
        istate->accel_info.report_rate_req = odr_matched;
        BMI160_INST_LOG(ERROR, istate->owner, "NOTICE acc rr lt sr");
    }

    if (BST_IS_FLOAT_ZERO(istate->accel_info.report_rate_req)) {
        istate->accel_info.ff_wml_req = 0;
        BMI160_INST_LOG(HIGH, istate->owner, "acc_rr:zero");
    } else {
        istate->accel_info.ff_wml_req = odr_matched / istate->accel_info.report_rate_req;
    }

    {
        bmi160_regv_acc_range_t acc_range_regv_array[] = {
            BMI160_REGV_RANGE_ACC_PM2G,
            BMI160_REGV_RANGE_ACC_PM4G,
            BMI160_REGV_RANGE_ACC_PM8G,
            BMI160_REGV_RANGE_ACC_PM16G,
        };

        istate->accel_info.range_req   = acc_range_regv_array[istate->accel_info.sstate->resolution_idx];
        BMI160_INST_LOG(LOW, istate->owner, "acc resolution idx=%d, range regv:%x",
                istate->accel_info.sstate->resolution_idx,
                istate->accel_info.range_req);
    }

    if (istate->gyro_info.sample_rate_req > 0) {
        istate->gyro_info.sample_rate_req = SNS_MAX(istate->gyro_info.sample_rate_req, BMI160_CONFIG_GYR_LOWEST_ODR);

        //istate->gyro_info.sample_rate_req = SNS_MIN(istate->gyro_info.sample_rate_req, BMI160_CONFIG_GYR_FASTEST_ODR);
        if (istate->gyro_info.sample_rate_req > BMI160_CONFIG_GYR_FASTEST_ODR) {
            istate->gyro_info.sample_rate_req = BMI160_CONFIG_GYR_FASTEST_ODR;
#if BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR
            bmi160_inst_report_error(istate, BMI160_GYRO, &istate->gyro_info.sstate->my_suid, 0);
#endif
        }
    }
    //gyro_info
    bmi160_hal_match_odr(BMI160_GYRO, istate->gyro_info.sample_rate_req, &odr_matched, &regv_odr);
    istate->gyro_info.odr_req = regv_odr;

    if (istate->gyro_info.report_rate_req > odr_matched) {
        istate->gyro_info.report_rate_req = odr_matched;
        BMI160_INST_LOG(ERROR, istate->owner, "NOTICE gyr rr lt sr");
    }

    if (BST_IS_FLOAT_ZERO(istate->gyro_info.report_rate_req)) {
        istate->gyro_info.ff_wml_req = 0;
        BMI160_INST_LOG(HIGH, istate->owner, "gyr_rr:zero");
    } else {
        istate->gyro_info.ff_wml_req = odr_matched / istate->gyro_info.report_rate_req;
    }


    {
        bmi160_regv_gyr_range_t gyro_range_regv_array[] = {
            BMI160_REGV_RANGE_GYR_PM125DPS,
            BMI160_REGV_RANGE_GYR_PM250DPS,
            BMI160_REGV_RANGE_GYR_PM500DPS,
            BMI160_REGV_RANGE_GYR_PM1000DPS,
            BMI160_REGV_RANGE_GYR_PM2000DPS
        };

        istate->gyro_info.range_req    = gyro_range_regv_array[istate->gyro_info.sstate->resolution_idx];
        BMI160_INST_LOG(LOW, istate->owner, "gyro resolution idx=%d, range regv:%x",
                istate->gyro_info.sstate->resolution_idx,
                istate->gyro_info.range_req);
    }


    istate->md_info.range_idx_req = (bmi160_acc_range_t)istate->accel_info.sstate->resolution_idx;

    bmi160_inst_determine_max_wml_req(istate);

    bmi160_hal_fifo_update_master_odr_req(istate);
    bmi160_hal_fifo_calc_wml_req_ldt(istate);

    istate->int_en_flags_req.bits.fifo.flag = istate->fifo_info.publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);

#if BMI160_CONFIG_ENABLE_DRI_MODE
    bool dri_mode_allowed;

    //TODOMAG
    //bmi160_inst_determine_max_wml_req has already made sure that (ff_wml_req > 0) if publish_sensors bit is set
    dri_mode_allowed = (istate->accel_info.ff_wml_req <= 1) && (istate->gyro_info.ff_wml_req <= 1);
    if (dri_mode_allowed) {
        istate->int_en_flags_req.bits.drdy.flag = istate->int_en_flags_req.bits.fifo.flag;
    } else {
        istate->int_en_flags_req.bits.drdy.flag = 0;
    }

    if (istate->fac_test_in_progress) {
        istate->int_en_flags_req.bits.drdy.flag = 0;
    }

    if (istate->int_en_flags_req.bits.drdy.flag) {
        istate->int_en_flags_req.bits.fifo.flag = 0;
    }

    BMI160_INST_LOG(HIGH, istate->owner, "drdy_flag:%d", istate->int_en_flags_req.bits.drdy.flag);
#endif

    //istate->fifo_info.ff_sensors_en_req = istate->fifo_info.publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);
    istate->fifo_info.ff_sensors_en_req = istate->int_en_flags_req.bits.fifo.flag;

    if (istate->fifo_info.publish_sensors & BMI160_SENSOR_TEMP)
    {
        rc = bmi160_validate_sensor_temp_odr(istate);
        if (rc != SNS_RC_SUCCESS)
        {
            BMI160_INST_LOG(ERROR, istate->owner, "sensor_temp ODR match error %d", rc);
#if BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR
            bmi160_inst_report_error(istate, BMI160_SENSOR_TEMP, &istate->sensor_temp_info.sstate->my_suid, 0);
#endif
        }
    }
}



/**
 * Process COM PORT vector event
 * @param vector         the vector handler
 * @param user_arg       user defined arguments
 *
 * @return none
 */
    static
void bmi160_process_com_port_vector(sns_port_vector *vector, void *user_arg)
{
    sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;

    if(BMI160_REGA_USR_FIFO_DATA == vector->reg_addr)
    {
        //Vector contains a FIFO buffer read
        if (vector->bytes > 0)
        {
            bmi160_hal_process_fifo_data_buffer(instance,
                    vector->buffer,
                    vector->bytes);
        }
    }
}



sns_rc bmi160_inst_handle_event_int(
        sns_sensor_instance     *const this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state *) this->state->state;
    sns_sensor_event            *event;
    sns_rc                      rc = SNS_RC_SUCCESS;

    sns_interrupt_event         irq_event = sns_interrupt_event_init_zero;
    bmi160_int_check_ctx        ctx;

    // Handle interrupts
    if(NULL != istate->interrupt_data_stream)
    {
        ctx.int_check_trigger = BMI160_INT_CHECK_TRIGGER_IRQ;

        event = istate->interrupt_data_stream->api->peek_input(istate->interrupt_data_stream);
        while(NULL != event)
        {
            if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT == event->message_id)
            {
                int enable_fifo_stream = istate->fifo_info.publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);

                enable_fifo_stream |= istate->fac_test_in_progress;

                BMI160_INST_LOG(MED, this, "irq_ready event id=%d", event->message_id);

                istate->irq_ready = 1;

                if (enable_fifo_stream
                        || istate->int_en_flags_req.bits.md) {
                    rc |= bmi160_hal_config_int_output(this, true);
                }

                if (istate->md_info.enable_md_int)
                {
                    bmi160_hal_config_int_md(this, true, false);
                }

                if (enable_fifo_stream)
                {
                    bmi160_hal_config_int_fifo(this, true);
                }
            }
            else if (SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT == event->message_id)
            {
                pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                        event->event_len);


                if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
                {
                    ctx.timestamp = irq_event.timestamp;
                    rc = bmi160_hal_handle_interrupt(this, &ctx);
                }
            }
            else
            {
                BMI160_INST_LOG(ERROR, this, "Received invalid event id=%d", event->message_id);
            }

            event = istate->interrupt_data_stream->api->get_next_input(istate->interrupt_data_stream);
        }
    } else {
        BMI160_INST_LOG(ERROR, this, "WARNING!!! interrupt_data_stream is NULL");
    }

    return SNS_RC_SUCCESS;
}

sns_rc bmi160_inst_handle_event_async_com(
        bmi160_instance_state   *istate)
{
    sns_sensor_instance         *const this = istate->owner;
    sns_sensor_event            *event;

    if(NULL != istate->async_com_port_data_stream)
    {
        bool                    ascp_vector_found = false;

        event = istate->async_com_port_data_stream->api->peek_input(istate->async_com_port_data_stream);
        while (NULL != event)
        {
            if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR == event->message_id)
            {
                //TODO: Warning;
                BMI160_INST_LOG(
                        ERROR, istate->owner,
                        "Received ASCP error event id=%d",
                        event->message_id);
            }
            else if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW == event->message_id)
            {
                pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
                sns_ascp_for_each_vector_do(&stream, bmi160_process_com_port_vector, (void *)this);

                ascp_vector_found = true;
            }
            event = istate->async_com_port_data_stream->api->get_next_input(istate->async_com_port_data_stream);
        }

        if (ascp_vector_found) {
            if (istate->fac_test_in_progress)
            {
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
                /** All self-tests can be handled in normal mode. */
                istate->island_service->api->sensor_instance_island_exit(istate->island_service, this);
#endif
                bmi160_process_fac_test(this);
            }
        }
    }

    return SNS_RC_SUCCESS;
}

sns_rc bmi160_inst_handle_event_timer(
        bmi160_instance_state   *istate)
{
    sns_sensor_instance         *const this = istate->owner;
    sns_sensor_event            *event;


    if (NULL != istate->timer_data_stream)
    {
        event = istate->timer_data_stream->api->peek_input(istate->timer_data_stream);
        while(NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                    event->event_len);
            if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
            {
                sns_timer_sensor_event timer_event;
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    if (event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
                    {
                        if ((istate->fifo_info.publish_sensors & BMI160_SENSOR_TEMP)
                                &&
                                istate->sensor_temp_info.timer_is_active
                                &&
                                istate->sensor_temp_info.sampling_intvl > 0)
                        {
                            //istate->sensor_temp_info.timer_is_active = false;
                            bmi160_hal_handle_sensor_temp_sample(this);
                            //bmi160_hal_start_sensor_temp_polling_timer(this);
                        }
                    }
                }
            }
            else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT)
            {
                /** TODO: decode and qse timer_reg_event*/
                BMI160_INST_LOG(LOW, this, "TIMER_SENSOR_REG_EVENT");
            }
            else
            {
                BMI160_INST_LOG(MED, this, "unknown message_id %d", event->message_id);
            }
            event = istate->timer_data_stream->api->get_next_input(istate->timer_data_stream);
        }

        if ((istate->fifo_info.publish_sensors & BMI160_SENSOR_TEMP) == 0)
        {
            //remove timer stream	//FIXLK
            bmi160_hal_stop_tempetature_timer(this);
        }
    }

    if(NULL != istate->cmd_handler.timer_cmd_stream)
    {
        event = istate->cmd_handler.timer_cmd_stream->api->peek_input(istate->cmd_handler.timer_cmd_stream);
        while(NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
            sns_timer_sensor_event timer_event;
            if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
            {
                if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
                {
                    bmi160_hal_handle_timer_cmd(istate);
                }
            }
            else
            {
            }
            event = istate->cmd_handler.timer_cmd_stream->api->get_next_input(istate->cmd_handler.timer_cmd_stream);
        }
    }

    return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc bmi160_inst_set_client_config(
        sns_sensor_instance     *const this,
        sns_request const       *client_request)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    uint32_t                    time_info;
    sns_rc                      rc;

#if BMI160_CONFIG_ENABLE_DEBUG
    bmi160_req_payload          *req_payload = (bmi160_req_payload *) client_request->request;

    BMI160_INST_LOG(MED, this, "<bmi160_iif set_client_config 0x%x @sensor:%d", this,
            (NULL != req_payload) ? req_payload->sensor_type : -1);
#endif

    time_info = sns_convert_ns_to_ticks(1000 * 1000);

    // Turn COM port ON
    istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle,
            true);

    if (
#if BMI160_CONFIG_ENABLE_DAE
            !bmi160_dae_if_available(this)
#else
            1
#endif
       ) {
        bmi160_hal_register_interrupt(this);
    }


    if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
        // 1. Extract sample, report rates from client_request.
        // 2. Configure sensor HW.
        // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
        // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
        // 5. Save the current config information like type, sample_rate, report_rate, etc.

        bmi160_inst_assess_overall_req(istate);

#if BMI160_CONFIG_ENABLE_DAE
        if (BMI160_CONFIG_IDLE == istate->config_step &&
                bmi160_dae_if_stop_streaming(this)
           )
        {
            istate->config_step = BMI160_CONFIG_STOPPING_STREAM;
        }
#endif

        if (BMI160_CONFIG_IDLE == istate->config_step)
        {
            rc = bmi160_hal_reconfig_hw(this);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
            if (istate->hw_config_pending) {
                bmi160_hal_send_config_event(this);
            }
        }

    }
    else if (client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
    {
        //TOCHECK
        bool dae_flush_done = false;

#if BMI160_CONFIG_ENABLE_DAE
        dae_flush_done = bmi160_dae_if_flush_samples(this);
#endif

        if (!dae_flush_done) {
            BMI160_INST_LOG(HIGH, this, "fifo_flush status: %x",
                    (istate->ff_flush_client_req) |
                    (istate->fifo_info.ff_flush_in_proc << 1)|
                    (istate->fifo_info.ff_flush_trigger << 2) |
                    (istate->hw_config_pending << 3));

            istate->ff_flush_client_req = 1;
            bmi160_hal_fifo_drain(istate, false, BMI160_FIFO_FLUSH_TRIGGER_CLIENTS);
        }
    }
    else if (client_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
    {
        // 1. Extract test type from client_request.
        // 2. Configure sensor HW for test type.
        // 3. send_request() for Timer Sensor in case test needs polling/waits.
        // 4. Factory test is TBD.
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
        istate->island_service->api->sensor_instance_island_exit(istate->island_service, this);
#endif
        bmi160_run_self_test(this);
        istate->new_self_test_request = false;
    }
#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
    else if (client_request->message_id == SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG)
    {
        BMI160_INST_LOG(HIGH, this, "BMI160 : SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG is received");
        /** All self-tests can be handled in normal mode. */
        istate->island_service->api->sensor_instance_island_exit(istate->island_service, this);
        bmi160_run_self_test_bias(this);
        istate->new_self_test_request_bias = false;
    }
#endif


    // Turn COM port OFF
    istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle,
            false);

    return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::notify_event */
    BMI160_FTI
static sns_rc bmi160_inst_notify_event(
        sns_sensor_instance     *const this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    sns_rc                      rc = SNS_RC_SUCCESS;


#if BMI160_CONFIG_ENABLE_DAE
    bmi160_dae_if_process_events(this);
#endif
    BMI160_INST_LOG(MED, istate->owner, "<bmi160_if_ inst_notify_ev>");

    // Turn COM port ON
    istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle,
            true);


    // Handle event int
    rc = bmi160_inst_handle_event_int(this);


    // Handle Async Com Port events
    rc |= bmi160_inst_handle_event_async_com(istate);

    // Handle Timer events
    rc |= bmi160_inst_handle_event_timer(istate);

    // Turn COM port OFF
    istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle,
            false);

    //BMI160_INST_LOG(LOW, istate->owner, "<bmi160_if_ inst_notify_ev> exit!!!")
    return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */

sns_sensor_instance_api bmi160_sensor_instance_api =
{
    .struct_len        = sizeof(sns_sensor_instance_api),
    .init              = &bmi160_inst_init,
    .deinit            = &bmi160_inst_deinit,
    .set_client_config = &bmi160_inst_set_client_config,
    .notify_event      = &bmi160_inst_notify_event
};

