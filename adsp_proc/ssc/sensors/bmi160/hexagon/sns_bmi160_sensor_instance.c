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
 * @file sns_bmi160_sensor_instance.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/
#include <stdint.h>
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_types.h"

#include "sns_bmi160_hal.h"
#include "sns_bmi160_sensor.h"
#include "sns_bmi160_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"

#if BMI160_CONFIG_ENABLE_DIAG_LOG
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#endif

#include "sns_cal.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"

#include "sns_sync_com_port_service.h"

/**
 * Instance cleanup
 * @param istate        instance handler
 * @param stream_mgr    stream manager
 */
static void bmi160_inst_cleanup(sns_sensor_instance *const  this, sns_stream_service *stream_mgr)
{
    bmi160_instance_state *istate = (bmi160_instance_state *) this->state->state;

    BMI160_INST_LOG(MED, this, "<bmi160_if inst_cleanup>");

    UNUSED_VAR(stream_mgr);

    // TODO: Is this OK to do here? Introduced by DAE sensor, since we could be
    // waiting for response from DAE before writing to HW -- but we don't have that chance.
    if (NULL != istate->com_port_info.port_handle)
    {
        istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle, true);

        bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_SOFT_RESET);
		//TODO2: odr_curr should be set to OFF etc
		//lsm6ds3_set_fifo_config(istate, 0, 0, 0, 0 );
        //bmi160_hal_reconfig_hw(this);

        istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle, false);
    }



#if BMI160_CONFIG_ENABLE_DAE
    bmi160_dae_if_deinit(istate, stream_mgr);
#endif

    sns_sensor_util_remove_sensor_instance_stream(this, &istate->interrupt_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &istate->async_com_port_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &istate->timer_data_stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &istate->cmd_handler.timer_cmd_stream);

    if(NULL != istate->scp_service)
    {
        istate->scp_service->api->sns_scp_close(istate->com_port_info.port_handle);
        istate->scp_service->api->sns_scp_deregister_com_port(&istate->com_port_info.port_handle);
        istate->scp_service = NULL;
    }
}

// <sensor test>
/**
 * Runs a communication test - verfies WHO_AM_I, publishes self
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
static void bmi160_send_self_test_event(
        sns_sensor_instance     *instance,
        const sns_sensor_uid    *uid,
        bool                    test_result,
        sns_physical_sensor_test_type type,
        bmi160_test_err_code    err_code)

{
    uint8_t data[1] = {(uint8_t)err_code};
    pb_buffer_arg buff_arg = (pb_buffer_arg)
    { .buf = &data, .buf_len = sizeof(data) };
    sns_physical_sensor_test_event test_event =
        sns_physical_sensor_test_event_init_default;

    test_event.test_passed = test_result;
    test_event.test_type   = type;
    test_event.test_data.funcs.encode = &pb_encode_string_cb;
    test_event.test_data.arg = &buff_arg;

    BMI160_INST_LOG(HIGH, instance, "bmi160 self_test_ev: %d %d %d",
		test_result, err_code, type);
    pb_send_event(instance,
            sns_physical_sensor_test_event_fields,
            &test_event,
            sns_get_system_time(),
            SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
            uid);
}

#if BMI160_CONFIG_ENABLE_SELF_TEST_HW_SW
struct bmi160_val_acc {
    int16_t x;
    int16_t y;
    int16_t z;
};

    static
void bmi160_parse_data_acc(
        uint8_t                 *buf,
        uint8_t                 len,
        struct bmi160_val_acc   *val)
{
    UNUSED_VAR(len);
    val->x = (buf[1] << 8) | (buf[0]);
    val->y = (buf[3] << 8) | (buf[2]);
    val->z = (buf[5] << 8) | (buf[4]);
}


// sensor H/W test
    static
sns_rc bmi160_run_hw_sw_test(
        sns_sensor_instance     *instance,
        bmi160_sensor_type      sensor_type,
        int32_t                 *err_code
        )
{
    sns_rc                  status = SNS_RC_SUCCESS;
    uint8_t                 regv;
    bmi160_instance_state *istate = (bmi160_instance_state *) instance->state->state;

    BMI160_INST_LOG(MED, instance, "h/w self test start");
    /*! soft reset */
    regv = BMI160_REGV_CMD_SOFT_RESET;
    status = bmi160_sbus_write_wrapper(istate, BMI160_REGA_CMD_CMD, &regv, 1);
    if (SNS_RC_SUCCESS != status) {
        return SNS_RC_FAILED;
    }

    bmi160_delay_us(BMI160_SPEC_SOFT_RESET_DELAY_TIME_MS * 1000);

    if (BMI160_ACCEL == sensor_type) {
        /*! set to +/-8G range */
        struct bmi160_val_acc   val1_acc_static = {0, 0, 0};
        struct bmi160_val_acc   val2_acc_static = {0, 0, 0};
        uint8_t                 si_buf_rd[7] = "";
        int32_t                 diff_val_acc_static = 0;

        regv = BMI160_REGV_CMD_ACC_MODE_NORMAL;
        status = bmi160_sbus_write_wrapper(istate, BMI160_REGA_CMD_CMD, &regv, 1);
        bmi160_delay_us (BMI160_SPEC_ACC_STARTUP_TIME_US * 1000);
        //prepare for the self test
        regv = 0x08;
        status = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_ACC_RANGE, 0, 7, regv);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        // odr
        regv = 0;
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, istate->accel_info.odr_req);
        regv = BST_SET_VAL_BITBLOCK(regv, 4, 6, BMI160_CONFIG_ACC_BWP);
        status = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_ACC_CONF, 0, 7, regv);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }
        bmi160_delay_us(10000);   //10ms

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 1);     //axis-x
        regv = BST_SET_VAL_BIT(regv, 2);                //positive
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);

        bmi160_delay_us(BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val1_acc_static);

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 1);     //axis-x
        regv = BST_CLR_VAL_BIT(regv, 2);                //negative
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_delay_us(BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val2_acc_static);

        diff_val_acc_static = (int32_t)val1_acc_static.x - (int32_t)val2_acc_static.x;
        BMI160_INST_LOG(HIGH, instance, "diff_val_acc_static_x: <%d %d %d>",
                val1_acc_static.x,
                val2_acc_static.x,
                diff_val_acc_static);

        /* 800mg for x axis */
        if (BST_ABS(diff_val_acc_static) < 3277) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }
        //</test_acc_x>

        //<test_acc_y>
        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 2);     //axis-y
        regv = BST_SET_VAL_BIT(regv, 2);                //positive
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);

        bmi160_delay_us (BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val1_acc_static);

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 2);     //axis-y
        regv = BST_CLR_VAL_BIT(regv, 2);                //negative
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);

        bmi160_delay_us (BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val2_acc_static);

        diff_val_acc_static = (int32_t)val1_acc_static.y - (int32_t)val2_acc_static.y;
        BMI160_INST_LOG (HIGH, instance, "diff_val_acc_static_y: <%d %d %d>",
                val1_acc_static.y,
                val2_acc_static.y,
                diff_val_acc_static);

        /* 800mg for y axis */
        if (BST_ABS(diff_val_acc_static) < 3277) {
            if (SNS_RC_SUCCESS != status) {
                status = SNS_RC_FAILED;
                goto exit_err_test_sw_hw;
            }
        }
        //</test_acc_y>

        //<test_acc_z>
        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 3);     //axis-z
        regv = BST_SET_VAL_BIT(regv, 2);                //positive
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);

        bmi160_delay_us (BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val1_acc_static);

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 3);                //high-amp
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 3);     //axis-z
        regv = BST_CLR_VAL_BIT(regv, 2);                //negative
        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);

        bmi160_delay_us(BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS * 1000);

        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_DATA_14, si_buf_rd, 6);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_parse_data_acc(si_buf_rd, 6, &val2_acc_static);

        diff_val_acc_static = (int32_t)val1_acc_static.z - (int32_t)val2_acc_static.z;

        BMI160_INST_LOG(HIGH, instance, "diff_val_acc_static_z: <%d %d %d>",
                val1_acc_static.z,
                val2_acc_static.z,
                diff_val_acc_static);
        /* 400mg for z axis */
        if (BST_ABS(diff_val_acc_static) < 1639) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }
        //</test_acc_z>
        //</test_acc>

        status = SNS_RC_SUCCESS;
    } else if (BMI160_GYRO == sensor_type) {
        regv = BMI160_REGV_CMD_GYR_MODE_NORMAL;
        status = bmi160_sbus_write_wrapper(istate, BMI160_REGA_CMD_CMD, &regv, 1);
        bmi160_delay_us (BMI160_SPEC_GYR_STARTUP_TIME_US * 1000);

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 4);

        status = bmi160_sbus_write_wrapper(istate,
                BMI160_REGA_USR_SELF_TEST, &regv, 1);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        bmi160_delay_us(BMI160_SPEC_GYR_SELF_TEST_WAIT_TIME_MS * 1000);

        regv = 0;
        status = bmi160_sbus_read_wrapper(istate,
                BMI160_REGA_USR_STATUS, &regv, 1);
        if (SNS_RC_SUCCESS != status) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

        if (!(B1_SET & regv)) {
            status = SNS_RC_FAILED;
            goto exit_err_test_sw_hw;
        }

    }
exit_err_test_sw_hw:
    BMI160_INST_LOG(HIGH, instance, "[run_test_sw_hw] sensor_type: %d status: %d",
            sensor_type, status);


    /*! soft reset */
    regv = BMI160_REGV_CMD_SOFT_RESET;
    status = bmi160_sbus_write_wrapper(istate, BMI160_REGA_CMD_CMD, &regv, 1);
    bmi160_delay_us (BMI160_SPEC_SOFT_RESET_DELAY_TIME_MS * 1000);

    *err_code = (int32_t)(status);
    istate->fac_test_in_progress = false;

    BMI160_INST_LOG(MED, instance, "h/w self test finish:%d", status);

    return SNS_RC_SUCCESS;
}
#endif

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
static void bmi160_send_self_test_event_bias(sns_sensor_instance *instance,
        sns_sensor_uid *uid, bool test_result,
        sns_physical_sensor_oem_config_type type)
{
    sns_physical_sensor_oem_config_event test_event =
        sns_physical_sensor_oem_config_event_init_default;

    test_event.config_result = test_result;
    test_event.config_type = type;

    pb_send_event(instance,
            sns_physical_sensor_oem_config_event_fields,
            &test_event,
            sns_get_system_time(),
            SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG_EVENT,
            uid);
}
#endif

#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
static
void bmi160_test_fac_req_hw(sns_sensor_instance     *instance)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)instance->state->state;
    BMI160_INST_LOG(HIGH, instance, "fac_test req_hw");

    istate->hw_mod_needed = 0;
    istate->fifo_info.publish_sensors = 0;

    if (BMI160_ACCEL == istate->fac_test_info.fac_test_sensor) {
        istate->hw_mod_needed |= BMI160_ACCEL;
        istate->fifo_info.publish_sensors |= BMI160_ACCEL;

        //istate->accel_info.odr_curr = BMI160_REGV_ODR_OFF;
        //istate->accel_info.range_curr = BMI160_REGV_RANGE_ACC_PM2G;

        //istate->accel_info.odr_req = BMI160_REGV_ODR_100HZ;
        //istate->accel_info.range_req = BMI160_REGV_RANGE_ACC_PM8G;

        istate->accel_info.sample_rate_req = 100;
        istate->accel_info.report_rate_req = istate->accel_info.sample_rate_req;
    } else if (BMI160_GYRO == istate->fac_test_info.fac_test_sensor) {
        istate->hw_mod_needed |= BMI160_GYRO;
        istate->fifo_info.publish_sensors |= BMI160_GYRO;

        //istate->gyro_info.odr_curr = BMI160_REGV_ODR_OFF;
        //istate->gyro_info.range_curr = BMI160_REGV_RANGE_GYR_PM2000DPS;

        istate->gyro_info.sample_rate_req = 100;
        istate->gyro_info.report_rate_req = istate->gyro_info.sample_rate_req;
        //istate->gyro_info.odr_req = BMI160_REGV_ODR_100HZ;
        //istate->gyro_info.range_req = BMI160_REGV_RANGE_GYR_PM2000DPS;
    }
    //TODOMAG

    bmi160_hal_register_interrupt(instance);
    istate->irq_ready = true;

    //istate->int_en_flags_req.bits.fifo.flag = istate->fifo_info.publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);
    bmi160_inst_assess_overall_req(istate);
    bmi160_hal_reconfig_hw(instance);
}


static
void bmi160_test_fac_rls_hw(sns_sensor_instance     *instance)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)instance->state->state;

    BMI160_INST_LOG(HIGH, instance, "fac_test rls_hw");

    istate->hw_mod_needed = 0;
    istate->fifo_info.publish_sensors  = 0;

    bmi160_hal_reconfig_hw(instance);
}
#endif

int bmi160_start_factory_test(
        sns_sensor_instance     *instance,
        bmi160_sensor_type      sensor,
        int32_t                 *err_code)
{
    int rc = SNS_RC_SUCCESS;

    UNUSED_VAR(instance);
    UNUSED_VAR(sensor);
    UNUSED_VAR(err_code);
#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    bmi160_instance_state *istate = (bmi160_instance_state*)instance->state->state;


    if (istate->fifo_info.ff_sensors_en_req) {
        BMI160_INST_LOG(HIGH, instance, "WARNING!!! normal streaming is in process, will be paused now and resumed after fac test finishes");
    }


    istate->fac_test_info.num_samples = 0;
    istate->fac_test_info.sample_square_sum[0] = 0;
    istate->fac_test_info.sample_square_sum[1] = 0;
    istate->fac_test_info.sample_square_sum[2] = 0;
    istate->fac_test_info.sample_sum[0] = 0;
    istate->fac_test_info.sample_sum[1] = 0;
    istate->fac_test_info.sample_sum[2] = 0;
    istate->fac_test_in_progress = true;
    istate->fac_test_info.fac_test_sensor = sensor;
    istate->fac_test_info.at_rest = true;


    if (BMI160_ACCEL == sensor)
    {
        istate->fac_test_info.bias_thresholds[0] = 300 * G / 1000; // 180 m/s2
        istate->fac_test_info.bias_thresholds[1] = 300 * G / 1000; // 180 m/s2
        istate->fac_test_info.bias_thresholds[2] = 360 * G / 1000; // 210 m/s2
        istate->fac_test_info.variance_threshold = 360.0; // 360(m/s2)2
    }
    else if (BMI160_GYRO == sensor)
    {
        istate->fac_test_info.bias_thresholds[0] = 40 * PI / 180; //40 rad/sec
        istate->fac_test_info.bias_thresholds[1] = 40 * PI / 180; //40 rad/sec
        istate->fac_test_info.bias_thresholds[2] = 40 * PI / 180; //40 rad/sec
        istate->fac_test_info.variance_threshold = 100.0; // 100(rad/sec)2
    }
    else
    {
        BMI160_INST_LOG(ERROR, instance, "fac self test not supported for sensor: %d", sensor);
        return SNS_RC_NOT_SUPPORTED;
    }

    BMI160_INST_LOG(HIGH, instance, "start_factory_test() sensor %d", sensor);


    bmi160_test_fac_req_hw(instance);
#endif

    return rc;
}


#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
void bmi160_run_self_test_bias(sns_sensor_instance *instance)
{
    bmi160_instance_state *state = (bmi160_instance_state*)instance->state->state;

    BMI160_INST_LOG(HIGH, instance, "bmi160 : bmi160_run_self_test_bias");
    if (state->accel_info.test_info_bias.test_client_present)
    {
        if (state->accel_info.test_info_bias.config_type == SNS_PHYSICAL_SENSOR_CONFIG_BIAS)
        {
            // Handle factory test. The driver may choose to reject any new
            // streaming/self-test requests when factory test is in progress.
            /** update_fac_cal_in_registry is used to demonstrate a registry write operation.*/
            /** Using dummy data for registry write demonstration. */
            state->accel_info.sstate->fac_cal_bias[0] = roundf(state->accel_info.test_info_bias.offset_x * BMI160_SCALE_FACTOR_DATA_ACCEL);
            state->accel_info.sstate->fac_cal_bias[1] = roundf(state->accel_info.test_info_bias.offset_y * BMI160_SCALE_FACTOR_DATA_ACCEL);
            state->accel_info.sstate->fac_cal_bias[2] = roundf(state->accel_info.test_info_bias.offset_z * BMI160_SCALE_FACTOR_DATA_ACCEL);
            state->update_fac_cal_in_registry = true;
            state->accel_info.sstate->fac_cal_version++;
            BMI160_INST_LOG(HIGH, instance, "bmi : Custom Accel Calibration [version:%d][x:%d][y:%d][z:%d]",
                    state->accel_info.sstate->fac_cal_version,
                    (int)(state->accel_info.sstate->fac_cal_bias[0]),
                    (int)(state->accel_info.sstate->fac_cal_bias[1]),
                    (int)(state->accel_info.sstate->fac_cal_bias[2]));

            bmi160_send_self_test_event_bias(instance, &state->accel_info.sstate->my_suid,
                    true, state->accel_info.test_info_bias.config_type);
        }
        state->accel_info.test_info_bias.test_client_present = false;
    }
    if (state->gyro_info.test_info_bias.test_client_present)
    {
        if(state->gyro_info.test_info_bias.config_type == SNS_PHYSICAL_SENSOR_CONFIG_BIAS)
        {
            // Handle factory test. The driver may choose to reject any new
            // streaming/self-test requests when factory test is in progress.
            /** update_fac_cal_in_registry is used to demonstrate a registry write operation.*/
            /** Using dummy data for registry write demonstration. */
            state->gyro_info.sstate->fac_cal_bias[0] = roundf(state->gyro_info.test_info_bias.offset_x * BMI160_SCALE_FACTOR_DATA_GYRO);
            state->gyro_info.sstate->fac_cal_bias[1] = roundf(state->gyro_info.test_info_bias.offset_y * BMI160_SCALE_FACTOR_DATA_GYRO);
            state->gyro_info.sstate->fac_cal_bias[2] = roundf(state->gyro_info.test_info_bias.offset_z * BMI160_SCALE_FACTOR_DATA_GYRO);
            state->update_fac_cal_in_registry = true;
            state->gyro_info.sstate->fac_cal_version++;
            BMI160_INST_LOG(HIGH, instance, "bmi160 : Custom Gyro Calibration [version:%d][x:%d][y:%d][z:%d]",
                    state->gyro_info.sstate->fac_cal_version,
                    (int)(state->gyro_info.sstate->fac_cal_bias[0]),
                    (int)(state->gyro_info.sstate->fac_cal_bias[1]),
                    (int)(state->gyro_info.sstate->fac_cal_bias[2]));

            bmi160_send_self_test_event_bias(instance, &state->gyro_info.sstate->my_suid,
                    true, state->gyro_info.test_info_bias.config_type);
        }
        state->accel_info.test_info_bias.test_client_present = false;
    }
}
#endif

void bmi160_reset_fac_cal_data(
        sns_sensor_instance *const  instance,
        struct bmi160_state         *sstate,
        bool                        update_version)
{
    UNUSED_VAR(instance);

    memset(sstate->fac_cal_bias, 0, sizeof(sstate->fac_cal_bias));
    memset(&sstate->fac_cal_corr_mat, 0, sizeof(sstate->fac_cal_corr_mat));

    sstate->fac_cal_corr_mat.e00 = 1.0;
    sstate->fac_cal_corr_mat.e11 = 1.0;
    sstate->fac_cal_corr_mat.e22 = 1.0;

    if (update_version) {
        sstate->fac_cal_version++;
    }
}

void bmi160_process_fac_test(sns_sensor_instance *instance)
{
    UNUSED_VAR(instance);
#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    int i;
    bool                        test_pass = true;
    bmi160_instance_state       *state = (bmi160_instance_state*)instance->state->state;
    sns_sensor_uid              *uid;
    bmi160_state                *sstate;

    float                       fac_cal_bias[3];

    bmi160_test_err_code        err_code = BMI160_FAC_TEST_NO_ERROR;

    BMI160_INST_LOG(HIGH, instance, "fac_test process_result for sensor: %d %d ", state->fac_test_info.fac_test_sensor, state->fac_test_info.num_samples);

    if (BMI160_ACCEL == state->fac_test_info.fac_test_sensor) {
        sstate = state->accel_info.sstate;
    } else if (BMI160_GYRO == state->fac_test_info.fac_test_sensor) {
        sstate = state->gyro_info.sstate;
    } else {
        return;
    }

    uid = &sstate->my_suid;

    //if(state->fac_test_info.num_samples == 64)
    if (state->fac_test_info.num_samples >= 64)
    {
        BMI160_INST_LOG(HIGH, instance, "fac sample sums: %d %d %d",
                (int32_t)(state->fac_test_info.sample_sum[0] * 1000),
                (int32_t)(state->fac_test_info.sample_sum[1] * 1000),
                (int32_t)(state->fac_test_info.sample_sum[2] * 1000));
        BMI160_INST_LOG(HIGH, instance, "fac sample square sums: %d %d %d",
                (int32_t)(state->fac_test_info.sample_square_sum[0] * 1000),
                (int32_t)(state->fac_test_info.sample_square_sum[1] * 1000),
                (int32_t)(state->fac_test_info.sample_square_sum[2] * 1000));

        state->fac_test_in_progress = false;
        bmi160_test_fac_rls_hw(instance);


        //state->fac_test_info.num_samples -= 3; // subtract discarded number of samples

        fac_cal_bias[0] =
            state->fac_test_info.sample_sum[0] / state->fac_test_info.num_samples;
        fac_cal_bias[1] =
            state->fac_test_info.sample_sum[1] / state->fac_test_info.num_samples;
        fac_cal_bias[2] =
            state->fac_test_info.sample_sum[2] / state->fac_test_info.num_samples;

        for (i = 0; i < TRIAXIS_NUM; i++)
        {
            float varT = (state->fac_test_info.sample_sum[i]) * (state->fac_test_info.sample_sum[i]);

            state->fac_test_info.variance[i] = (state->fac_test_info.sample_square_sum[i]
                    - (varT / state->fac_test_info.num_samples)) / state->fac_test_info.num_samples;

            // Check variance to determine whether device is stationary
            if(state->fac_test_info.variance[i] > state->fac_test_info.variance_threshold)
            {
                // device is not stationary
                state->fac_test_info.at_rest = false;
                test_pass = false;
                err_code = BMI160_FAC_TEST_DEV_NOT_STATIONARY;
                BMI160_INST_LOG(ERROR, instance, "ERROR!!! fac_test FAILED device not stationary var[%u]=%u  %u",
                        i,
                        (uint32_t)state->fac_test_info.variance[i],
                        (uint32_t)state->fac_test_info.variance_threshold);
                break;
            }

            // Check biases are in defined limits
            if (fabsf(fac_cal_bias[i]) > state->fac_test_info.bias_thresholds[i])
            {
                test_pass = false;
                err_code = BMI160_FAC_TEST_HIGH_BIAS;
                BMI160_INST_LOG(ERROR, instance, "ERROR!!! fac_test FAILED bias very large: %d %d", i, (int)(1000 * fac_cal_bias[i]));
                break;
            }

            // Check for zero variance
            if(state->fac_test_info.variance[i] == 0.0)
            {
                test_pass = false;
                err_code = BMI160_FAC_TEST_ZERO_VARIANCE;
                BMI160_INST_LOG(ERROR, instance, "ERROR!!! fac_test FAILED zero variance");
                break;
            }
        }

        /** update_fac_cal_in_registry if test is successful.*/
        state->update_fac_cal_in_registry = test_pass;

        bmi160_send_self_test_event(instance, uid,
                test_pass, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);

        if (test_pass) {
            for (i = 0; i < TRIAXIS_NUM; i++) {
                sstate->fac_cal_bias[i] = roundf(fac_cal_bias[i] * sstate->scale_factor);
            }

            sstate->fac_cal_version++;

            bmi160_send_fac_cal_event(instance, sstate);
        }
    }
#endif
}


    static
void bmi160_run_self_test_per_sensor(
        sns_sensor_instance     *instance,
        bmi160_self_test_info   *test_info,
        const sns_sensor_uid    *suid,
        bool                    hw_detect_success)
{
    int rc = SNS_RC_SUCCESS;
    int32_t err_code = 0;
    UNUSED_VAR(instance);
    bmi160_instance_state   *istate = (bmi160_instance_state*)instance->state->state;

    if (test_info->test_client_present) {
        BMI160_INST_LOG(HIGH, instance, "test_client_present sensor:%d test_type:%d", test_info->sensor, test_info->test_type);

        if (SNS_PHYSICAL_SENSOR_TEST_TYPE_COM == test_info->test_type) {
            bmi160_send_self_test_event(instance, suid,
                    hw_detect_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMI160_FAC_TEST_NO_ERROR);
        } else if (SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY == test_info->test_type) {
            if (hw_detect_success) {
                if (!istate->fac_test_in_progress) {
                    rc = bmi160_start_factory_test(instance, test_info->sensor, &err_code);
                } else {
                    BMI160_INST_LOG(HIGH, instance, "NOTICE!!! fac only one sensor at a time, curr_sensor: %d", istate->fac_test_info.fac_test_sensor);
                    rc = SNS_RC_NOT_AVAILABLE;
                    err_code = BMI160_FAC_TEST_DEV_BUSY;
                }

                if ((!istate->fac_test_in_progress) || (SNS_RC_SUCCESS != rc)) {
                    bmi160_send_self_test_event(instance, suid,
                            false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);
                }
            } else {
                err_code = SNS_RC_FAILED;   //because of hw_detect_success
                bmi160_send_self_test_event(instance, suid,
                        false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);
            }
        }
#if BMI160_CONFIG_ENABLE_SELF_TEST_HW_SW
        else if ((SNS_PHYSICAL_SENSOR_TEST_TYPE_HW == test_info->test_type) ||
                (SNS_PHYSICAL_SENSOR_TEST_TYPE_SW == test_info->test_type)) {
            if (hw_detect_success && (!istate->fac_test_in_progress)) {
                bmi160_sensor_type ss_type = (bmi160_sensor_type) test_info->sensor;
                rc = bmi160_run_hw_sw_test(instance, ss_type, &err_code);
                // always send out the test result
                bmi160_send_self_test_event(instance, suid,
                        hw_detect_success, test_info->test_type, err_code);
            } else {
                bmi160_send_self_test_event(instance, suid,
                        hw_detect_success, test_info->test_type, hw_detect_success ? BMI160_FAC_TEST_NO_ERROR : istate->fac_test_in_progress);
            }
        }
#endif

        test_info->test_client_present = false;
    }
}
/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instace
 *
 * @return none
 */
void bmi160_run_self_test(sns_sensor_instance *instance)
{
    bmi160_instance_state   *istate = (bmi160_instance_state*)instance->state->state;
    sns_rc                  rv = SNS_RC_SUCCESS;
    uint8_t                 buffer = 0;
    bool                    who_am_i_success = false;

    BMI160_INST_LOG(HIGH, instance, "run_self_test()");

    rv = bmi160_hal_get_who_am_i(istate->scp_service,
            istate->com_port_info.port_handle,
            &buffer);
    if (rv == SNS_RC_SUCCESS
            &&
            (BMI160_REGV_CHIP_ID_MAJOR == (buffer & BMI160_REGV_CHIP_ID_MAJOR)))
    {
        who_am_i_success = true;
    }


    bmi160_run_self_test_per_sensor(instance, &istate->accel_info.test_info, &istate->accel_info.sstate->my_suid, who_am_i_success);
    bmi160_run_self_test_per_sensor(instance, &istate->gyro_info.test_info, &istate->gyro_info.sstate->my_suid, who_am_i_success);
    bmi160_run_self_test_per_sensor(instance, &istate->md_info.test_info, &istate->md_info.sstate->my_suid, who_am_i_success);
    bmi160_run_self_test_per_sensor(instance, &istate->sensor_temp_info.test_info, &istate->sensor_temp_info.sstate->my_suid, who_am_i_success);
}
// </sensor test>


extern
fp_read_gpio bmi160_fp_read_gpio;
extern
fp_sns_scp_register_rw  bmi160_fp_scp_rw;

void bmi160_inst_collect_sensors(
        bmi160_instance_state   *istate,
        bmi160_state            *sstate)
{
    sns_sensor                  *sensor_this = sstate->owner;
    sns_sensor                  *sensor;
    bmi160_state                *sensor_state;

    for (sensor = sensor_this->cb->get_library_sensor(sensor_this, true);
            sensor != NULL;
            sensor = sensor_this->cb->get_library_sensor(sensor_this, false)) {
        sensor_state = (bmi160_state*)sensor->state->state;
        if (BMI160_ACCEL == sensor_state->sensor) {
            istate->accel_info.sstate = sensor_state;
        } else if (BMI160_GYRO == sensor_state->sensor) {
            istate->gyro_info.sstate = sensor_state;
            //TODOMAG
        } else if (BMI160_MOTION_DETECT == sensor_state->sensor) {
            istate->md_info.sstate = sensor_state;
        } else if (BMI160_SENSOR_TEMP == sensor_state->sensor) {
            istate->sensor_temp_info.sstate = sensor_state;
        }

        istate->sensors_col |= sensor_state->sensor;
    }

    BMI160_INST_LOG(LOW, istate->owner, "sensors_col: 0x%x", istate->sensors_col);
}

void bmi160_inst_config_ascp(
        bmi160_instance_state   *istate)
{
    sns_data_stream             *data_stream = istate->async_com_port_data_stream;
    sns_com_port_config         *com_config = &istate->sstate_creator->common.com_port_info.com_config;

    sns_async_com_port_config   ascp_config;

    uint8_t                     pb_encode_buffer[100];
    bmi160_real_t               bus_cycle_time_us;

    sns_request                 async_com_port_request =
    {
        .message_id  = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
        .request     = &pb_encode_buffer
    };

    memset(&ascp_config, 0, sizeof(ascp_config));

    if (SNS_BUS_SPI == com_config->bus_type)
    {
        ascp_config.bus_type          = SNS_ASYNC_COM_PORT_BUS_TYPE_SPI;
    }
    else
    {
        ascp_config.bus_type          = SNS_ASYNC_COM_PORT_BUS_TYPE_I2C;
    }
    ascp_config.slave_control     = com_config->slave_control;
    ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
    ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
    ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
    ascp_config.bus_instance      = com_config->bus_instance;

    bus_cycle_time_us = BMS_SCALE_S2US * 1.0 / ((com_config->min_bus_speed_KHz + com_config->max_bus_speed_KHz) / 2 * 1000);
    istate->bus_cycle_time_ns = roundf(bus_cycle_time_us * 1000);

    if (SNS_BUS_SPI == com_config->bus_type) {
        istate->xfer_time_per_byte_ticks = bmi160_convert_us2ticks(bus_cycle_time_us * (8 + BMI160_CONFIG_SEE_SPI_BYTE_XFER_WAIT_CYCLES));

        istate->bus_is_spi = 1;
    } else {
        istate->xfer_time_per_byte_ticks = bmi160_convert_us2ticks(bus_cycle_time_us * 9);
    }

    async_com_port_request.request_len =
        pb_encode_request(pb_encode_buffer,
                sizeof(pb_encode_buffer),
                &ascp_config,
                sns_async_com_port_config_fields,
                NULL);
    data_stream->api->send_request(data_stream, &async_com_port_request);
}

/** See sns_sensor_instance_api::init */
sns_rc bmi160_inst_init(
        sns_sensor_instance *const  this,
        sns_sensor_state const      *sensor_state)
{
    bmi160_instance_state           *istate = (bmi160_instance_state*)this->state->state;
    bmi160_state                    *sstate = (bmi160_state*)sensor_state->state;
    sns_service_manager             *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service              *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    float                           data[3];
    float                           temp_data[1];
    uint8_t                         si_buf[2];
    sns_sensor_uid                  irq_suid;
    sns_sensor_uid                  ascp_suid;

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    uint64_t                        buffer[10];
    pb_ostream_t                    stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));


    uint8_t                         arr_index = 0;
    float                           diag_temp[BMI160_NUM_AXES];


    pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = BMI160_NUM_AXES,
        .arr_index = &arr_index};

    sns_diag_batch_sample           batch_sample = sns_diag_batch_sample_init_default;

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;
#endif

    UNUSED_VAR(si_buf);

    bmi160_inst_singleton = istate;

    istate->owner = this;
    istate->sstate_creator = sstate;

    istate->stream_mgr = stream_mgr;
    istate->scp_service = (sns_sync_com_port_service*)service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
    bmi160_fp_scp_rw = istate->scp_service->api->sns_scp_register_rw;

#if BMI160_CONFIG_ENABLE_ISLAND_MODE
    istate->island_service = (sns_island_service*)
        service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);
#endif
    istate->gpio_service = (sns_gpio_service*)service_mgr->get_service(service_mgr, SNS_GPIO_SERVICE);

#if BMI160_CONFIG_ENABLE_SEE_LITE
    bmi160_fp_read_gpio = istate->gpio_service->api->read_gpio;
#endif

    /**---------Setup stream connections with dependent Sensors---------*/
    sns_suid_lookup_get(&sstate->common.suid_lookup_data, "interrupt", &irq_suid);
    sns_suid_lookup_get(&sstate->common.suid_lookup_data, "async_com_port", &ascp_suid);
    sns_suid_lookup_get(&sstate->common.suid_lookup_data, "timer", &istate->timer_suid);
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            irq_suid,
            &istate->interrupt_data_stream);

    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            ascp_suid,
            &istate->async_com_port_data_stream);

    //CHECK4IMPROVE: this is where improvement when needed can be made, we don't need timer to send temperature data, we can take advantage of the  fifo interrupt for acc/gyr
    //this improvement will save some island memory size needed
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            istate->timer_suid,
            &istate->timer_data_stream);

    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
            this,
            istate->timer_suid,
            &istate->cmd_handler.timer_cmd_stream);

    /** Initialize COM port to be used by the Instance */
    sns_memscpy(&istate->com_port_info.com_config,
            sizeof(istate->com_port_info.com_config),
            &sstate->common.com_port_info.com_config,
            sizeof(sstate->common.com_port_info.com_config));

    istate->scp_service->api->sns_scp_register_com_port(&istate->com_port_info.com_config,
            &istate->com_port_info.port_handle);

    if(NULL == istate->interrupt_data_stream ||
            NULL == istate->async_com_port_data_stream ||
            NULL == istate->timer_data_stream ||
            NULL == istate->cmd_handler.timer_cmd_stream||
            NULL == istate->com_port_info.port_handle)
    {
        bmi160_inst_cleanup(this, stream_mgr);
        return SNS_RC_FAILED;
    }

    /**-------------------------Init FIFO State-------------------------*/
    istate->fifo_info.ff_wml_bytes_curr = 0;

    if (SNS_BUS_I2C == sstate->common.com_port_info.com_config.bus_type) {
        istate->fifo_info.ff_tm_info.ff_ts_dev_delay = BMI160_CONFIG_TS_DELAY_ADJ_I2C;
    }

    istate->ts_hw_res_ticks_per_bit_ideal = sns_convert_ns_to_ticks(BMS_SCALE_S2US) * BMI160_SPEC_SENSORTIME_RES_US / 1000;
    istate->ts_hw_res_ticks_per_bit = istate->ts_hw_res_ticks_per_bit_ideal;

    /**-------------------------Init Accel State-------------------------*/
    istate->accel_info.odr_curr = BMI160_REGV_ODR_OFF;
    istate->accel_info.range_curr = BMI160_REGV_RANGE_ACC_PM2G;

    /**-------------------------Init Gyro State-------------------------*/
    istate->gyro_info.odr_curr = BMI160_REGV_ODR_OFF;
    istate->gyro_info.range_curr = BMI160_REGV_RANGE_GYR_PM2000DPS;

    istate->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(data, 3);
    istate->encoded_sensor_temp_event_len = pb_get_encoded_size_sensor_stream_event(temp_data, 1);

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    istate->diag_service =  (sns_diag_service*)
        service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
#endif

    istate->scp_service =  (sns_sync_com_port_service*)
        service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

    istate->scp_service->api->sns_scp_open(istate->com_port_info.port_handle);

    //do we really need this
    bmi160_hal_reset_device(this, BMI160_ACCEL | BMI160_GYRO | BMI160_MOTION_DETECT | BMI160_SENSOR_TEMP);

    istate->scp_service->api->sns_scp_update_bus_power(istate->com_port_info.port_handle,
            false);

    /** Configure the Async Com Port */
    bmi160_inst_config_ascp(istate);


    bmi160_inst_collect_sensors(istate, sstate);

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    /** Determine sizes of encoded logs */
    sns_diag_sensor_state_interrupt sensor_state_interrupt =
        sns_diag_sensor_state_interrupt_init_default;
    pb_get_encoded_size(&istate->log_interrupt_encoded_size,
            sns_diag_sensor_state_interrupt_fields,
            &sensor_state_interrupt);

    /** Determine size of sns_diag_sensor_state_raw as defined in
     *  sns_diag.proto
     *  sns_diag_sensor_state_raw is a repeated array of samples of
     *  type sns_diag_batch sample. The following determines the
     *  size of sns_diag_sensor_state_raw with a single batch
     *  sample */
    if(pb_encode_tag(&stream, PB_WT_STRING,
                sns_diag_sensor_state_raw_sample_tag))
    {
        if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                    &batch_sample))
        {
            istate->log_raw_encoded_size = stream.bytes_written;
        }
    }
#endif
    BMI160_INST_LOG(MED, this, "<bmi160_iif inst_init> 0x%x %d 0x%x", this, sstate->sensor, istate);
    BMI160_INST_LOG(MED, this, "0x%x 0x%x 0x%x", bmi160_inst_singleton, istate->scp_service, istate->com_port_info.port_handle);


    INSERT_TRACE_POINT_P(bmi160, this);

    istate->sbus_in_normal_mode = false;


#if BMI160_CONFIG_ENABLE_DAE
    sns_sensor_uid dae_suid;
    sns_suid_lookup_get(&sstate->common.suid_lookup_data, "data_acquisition_engine", &dae_suid);
    bmi160_dae_if_init(this, stream_mgr, &dae_suid);
#endif

    istate->config_step = BMI160_CONFIG_IDLE;
    istate->ticks_in_1ms = sns_convert_ns_to_ticks(1000 * 1000);


    return SNS_RC_SUCCESS;
}

sns_rc bmi160_inst_deinit(sns_sensor_instance *const this)
{
    bmi160_instance_state   *istate = (bmi160_instance_state*)this->state->state;

    sns_service_manager     *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service      *stream_mgr = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);


    UNUSED_VAR(istate);
    BMI160_INST_LOG(HIGH, istate->owner, "<bmi160_iif inst_deinit> 0x%x", this);

    bmi160_inst_cleanup(this, stream_mgr);


    bmi160_inst_singleton = NULL;

    return SNS_RC_SUCCESS;
}

