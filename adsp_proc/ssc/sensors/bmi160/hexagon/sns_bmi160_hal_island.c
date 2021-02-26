/**************************************r****************************************
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
 * @file sns_bmi160_hal_island.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include "sns_rc.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_math_util.h"
#include "sns_service_manager.h"
#include "sns_com_port_types.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"
#include "sns_gpio_service.h"
#include "sns_cal_util.h"
#include "sns_sensor_util.h"

#include "sns_bmi160_hal.h"
#include "sns_bmi160_sensor.h"
#include "sns_bmi160_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"

#include "sns_std_sensor.pb.h"

#if BMI160_CONFIG_ENABLE_DIAG_LOG
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#endif

#include "sns_timer.pb.h"
#include "sns_std_event_gated_sensor.pb.h"

#include "sns_cal.pb.h"

#if BMI160_CONFIG_ENABLE_SEE_LITE
fp_read_gpio bmi160_fp_read_gpio;
#endif
fp_sns_scp_register_rw  bmi160_fp_scp_rw;


void bmi160_log_sensor_state_raw_submit(
        log_sensor_state_raw_info *log_raw_info,
        bool batch_complete);

//NOTE: edit this list with caution, the sequence/order is relied upon
//the list is arranged in the way that the regv member has the same value as the index itself
//OPTIM3
struct bmi160_odr_regv_map BMI160_REGV_ODR_MAP[] =
{
    {
        .odr  = 0,
        .regv = 0
    },

    {
        .odr  = 0.78f,
        .regv = BMI160_REGV_ODR_0_78HZ
    },

    {
        .odr  = 1.56f,
        .regv = BMI160_REGV_ODR_1_56HZ
    },

    {
        .odr  = 3.125f,
        .regv = BMI160_REGV_ODR_3_12HZ
    },

    {
        .odr  = 6.25f,
        .regv = BMI160_REGV_ODR_6_25HZ
    },

    {
        .odr  = 12.5f,
        .regv = BMI160_REGV_ODR_12_5HZ
    },

    {
        .odr  = 25,
        .regv = BMI160_REGV_ODR_25HZ
    },

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_50) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_50)
    {
        .odr  = 50,
        .regv = BMI160_REGV_ODR_50HZ
    },
#endif

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_100) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_100)
    //idx = 7
    {
        .odr  = 100,
        .regv = BMI160_REGV_ODR_100HZ
    },
#endif

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_200) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_200)
    {
        .odr  = 200,
        .regv = BMI160_REGV_ODR_200HZ
    },
#endif

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_400) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_400)
    {
        .odr  = 400,
        .regv = BMI160_REGV_ODR_400HZ
    },
#endif

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_800) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_800)
    {
        .odr  = 800,
        .regv = BMI160_REGV_ODR_800HZ
    },
#endif

#if (BMI160_CONFIG_ACC_FASTEST_ODR >= BMI160_ODR_1600) || (BMI160_CONFIG_GYR_FASTEST_ODR >= BMI160_ODR_1600)
    {
        .odr  = 1600,
        .regv = BMI160_REGV_ODR_1600HZ
    },
#endif
};


const range_attr bmi160_accel_ranges[] =
{
    {BMI160_ACCEL_RANGE_2G_MIN, BMI160_ACCEL_RANGE_2G_MAX},
    {BMI160_ACCEL_RANGE_4G_MIN, BMI160_ACCEL_RANGE_4G_MAX},
    {BMI160_ACCEL_RANGE_8G_MIN, BMI160_ACCEL_RANGE_8G_MAX},
    {BMI160_ACCEL_RANGE_16G_MIN, BMI160_ACCEL_RANGE_16G_MAX}
};

const float bmi160_accel_resolutions[] =
{
    BMI160_ACCEL_RESOLUTION_2G,
    BMI160_ACCEL_RESOLUTION_4G,
    BMI160_ACCEL_RESOLUTION_8G,
    BMI160_ACCEL_RESOLUTION_16G
};

const range_attr bmi160_gyro_ranges[] =
{
    {BMI160_GYRO_RANGE_125_MIN, BMI160_GYRO_RANGE_125_MAX},
    {BMI160_GYRO_RANGE_250_MIN, BMI160_GYRO_RANGE_250_MAX},
    {BMI160_GYRO_RANGE_500_MIN, BMI160_GYRO_RANGE_500_MAX},
    {BMI160_GYRO_RANGE_1000_MIN, BMI160_GYRO_RANGE_1000_MAX},
    {BMI160_GYRO_RANGE_2000_MIN, BMI160_GYRO_RANGE_2000_MAX}
};

const float bmi160_gyro_resolutions[] =
{
    BMI160_GYRO_RESOLUTION_125DPS,
    BMI160_GYRO_RESOLUTION_250DPS,
    BMI160_GYRO_RESOLUTION_500DPS,
    BMI160_GYRO_RESOLUTION_1000DPS,
    BMI160_GYRO_RESOLUTION_2000DPS
};


#if BMI160_CONFIG_ENABLE_DEBUG_TEST || BMI160_CONFIG_ENABLE_LOG_TS_BATCH
uint8_t    g_bmi160_cnt_session_acc;
uint8_t    g_bmi160_cnt_session_gyr;
#endif



    static
uint32_t bmi160_util_get_com_div(int32_t a, int32_t b)
{
    uint32_t mcd = 1;
    int32_t  r;
    int32_t  t;

    if (a < 0) {
        a = -a;
    }

    if (b < 0) {
        b = -b;
    }

    if (a < b) {
        t = a;
        a = b;
        b = t;
    }

    if (0 == a) {
        return b;
    }

    if (0 == b) {
        return a;
    }

    while (1) {
        mcd = b;
        r = a % b;

        if (0 != r) {
            a = b;
            b = r;
        } else {
            break;
        }
    }

    return mcd;
}

    static
uint32_t bmi160_util_get_max_div(uint32_t a, uint32_t cap)
{
    uint32_t i;

    cap = cap < a ? cap : a;

    if (cap == a) {
        cap--;
    }

    for (i = cap; i > 1; i--) {
        if (0 == (a % i)) {
            return i;
        }
    }

    return 1;
}


#if BMI160_CONFIG_ENABLE_DIAG_LOG
/**
 * Encode Sensor State Log.Interrupt
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
sns_rc bmi160_encode_sensor_state_log_interrupt(
        void      *log,             size_t    log_size,
        size_t    encoded_log_size, void      *encoded_log,
        size_t    *bytes_written)
{
    UNUSED_VAR(log_size);
    sns_rc rc = SNS_RC_SUCCESS;

    if (NULL == encoded_log || NULL == log || NULL ==bytes_written)
    {
        return SNS_RC_FAILED;
    }

    sns_diag_sensor_state_interrupt *sensor_state_interrupt =
        (sns_diag_sensor_state_interrupt *)log;
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

    if (!pb_encode(&stream, sns_diag_sensor_state_interrupt_fields,
                sensor_state_interrupt))
    {
        rc = SNS_RC_FAILED;
    }

    if (SNS_RC_SUCCESS == rc)
    {
        *bytes_written = stream.bytes_written;
    }


    return rc;
}

/**
 * Encode log sensor state raw packet
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
sns_rc bmi160_encode_log_sensor_state_raw(
        void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
        size_t *bytes_written)
{
    sns_rc rc = SNS_RC_SUCCESS;
    uint32_t i = 0;
    size_t encoded_sample_size = 0;
    size_t parsed_log_size = 0;
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    uint8_t arr_index = 0;
    float temp[BMI160_NUM_AXES];
    pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = BMI160_NUM_AXES,
        .arr_index = &arr_index};

    if (NULL == encoded_log || NULL == log || NULL == bytes_written)
    {
        return SNS_RC_FAILED;
    }

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;

    if (!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                &batch_sample))
    {
        return SNS_RC_FAILED;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
    bmi160_batch_sample *raw_sample = (bmi160_batch_sample *)log;

    while(parsed_log_size < log_size &&
            (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
            i < (uint32_t)(-1))
    {
        arr_index = 0;
        arg.arr = (float *)raw_sample[i].sample;

        batch_sample.sample_type = raw_sample[i].sample_type;
        batch_sample.status = raw_sample[i].status;
        batch_sample.timestamp = raw_sample[i].timestamp;

        if (!pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
        {
            rc = SNS_RC_FAILED;
            break;
        }
        else if (!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                    &batch_sample))
        {
            rc = SNS_RC_FAILED;
            break;
        }

        parsed_log_size += sizeof(bmi160_batch_sample);
        i++;
    }

    if (SNS_RC_SUCCESS == rc)
    {
        *bytes_written = stream.bytes_written;
    }

    return rc;
}

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
void bmi160_log_sensor_state_raw_alloc(
        log_sensor_state_raw_info *log_raw_info,
        uint32_t log_size)
{
    uint32_t max_log_size = 0;

    if (NULL == log_raw_info || NULL == log_raw_info->diag ||
            NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
    {
        return;
    }

    // allocate memory for sensor state - raw sensor log packet
    max_log_size = log_raw_info->diag->api->get_max_log_size(
            log_raw_info->diag);

    if (0 == log_size)
    {
        // log size not specified.
        // Use max supported log packet size
        log_raw_info->log_size = max_log_size;
    }
    else if (log_size <= max_log_size)
    {
        log_raw_info->log_size = log_size;
    }
    else
    {
        return;
    }

    log_raw_info->log = log_raw_info->diag->api->alloc_log(
            log_raw_info->diag,
            log_raw_info->instance,
            log_raw_info->sensor_uid,
            log_raw_info->log_size,
            SNS_DIAG_SENSOR_STATE_LOG_RAW);

    log_raw_info->log_sample_cnt = 0;
    log_raw_info->bytes_written = 0;
}

/**
 *
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *                        pertaining to the sensor
 * @param[i] raw_data     Uncalibrated sensor data to be logged
 * @param[i] timestamp    Timestamp of the sensor data
 * @param[i] status       Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
sns_rc bmi160_log_sensor_state_raw_add(
        log_sensor_state_raw_info *log_raw_info,
        float *raw_data,
        sns_time timestamp,
        sns_std_sensor_sample_status status)
{
    sns_rc rc = SNS_RC_SUCCESS;

    if (NULL == log_raw_info || NULL == log_raw_info->diag ||
            NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
            NULL == raw_data || NULL == log_raw_info->log)
    {
        return SNS_RC_FAILED;
    }

    if ( (log_raw_info->bytes_written + sizeof(bmi160_batch_sample)) >
            log_raw_info->log_size)
    {
        // no more space in log packet
        // submit and allocate a new one
        bmi160_log_sensor_state_raw_submit(log_raw_info, false);
        bmi160_log_sensor_state_raw_alloc(log_raw_info, 0);
    }

    if (NULL == log_raw_info->log)
    {
        rc = SNS_RC_FAILED;
    }
    else
    {
        bmi160_batch_sample *sample =
            (bmi160_batch_sample *)log_raw_info->log;

        if (0 == log_raw_info->batch_sample_cnt)
        {
            sample[log_raw_info->log_sample_cnt].sample_type =
                SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
        }
        else
        {
            sample[log_raw_info->log_sample_cnt].sample_type =
                SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
        }

        sample[log_raw_info->log_sample_cnt].timestamp = timestamp;

        sns_memscpy(sample[log_raw_info->log_sample_cnt].sample,
                sizeof(sample[log_raw_info->log_sample_cnt].sample),
                raw_data,
                sizeof(sample[log_raw_info->log_sample_cnt].sample));

        sample[log_raw_info->log_sample_cnt].status = status;

        log_raw_info->bytes_written += sizeof(bmi160_batch_sample);

        log_raw_info->log_sample_cnt++;
        log_raw_info->batch_sample_cnt++;
    }

    return rc;
}

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *       pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *       of batch
 *  */
void bmi160_log_sensor_state_raw_submit(
        log_sensor_state_raw_info *log_raw_info,
        bool batch_complete)
{
    bmi160_batch_sample *sample = NULL;

    if (NULL == log_raw_info || NULL == log_raw_info->diag ||
            NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
            NULL == log_raw_info->log)
    {
        return;
    }

    sample = (bmi160_batch_sample *)log_raw_info->log;

    if (batch_complete)
    {
        // overwriting previously sample_type for last sample
        if (1 == log_raw_info->batch_sample_cnt)
        {
            sample[0].sample_type =
                SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
        }
        else if (1 < log_raw_info->batch_sample_cnt)
        {
            sample[log_raw_info->log_sample_cnt - 1].sample_type =
                SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
        }
    }

    if (log_raw_info->log != NULL)
    {
        log_raw_info->diag->api->submit_log(
                log_raw_info->diag,
                log_raw_info->instance,
                log_raw_info->sensor_uid,
                log_raw_info->bytes_written,
                log_raw_info->log,
                SNS_DIAG_SENSOR_STATE_LOG_RAW,
                log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
                bmi160_encode_log_sensor_state_raw);

        log_raw_info->log = NULL;
    }
}

#endif


#if BMI160_CONFIG_ENABLE_SEE_LITE
sns_rc bmi160_read_gpio(
        bmi160_instance_state   *istate,
        sns_interrupt_req       *gpio_cfg,
        sns_gpio_state          *level)
{
    sns_rc rc;

    //rc = istate->gpio_service->api->read_gpio(gpio_cfg->interrupt_num, gpio_cfg->is_chip_pin, level);
    rc = bmi160_fp_read_gpio(gpio_cfg->interrupt_num, gpio_cfg->is_chip_pin, level);

#if BMI160_CONFIG_ENABLE_DEBUG
    if (*level) {
        BMI160_INST_LOG(MED, istate->owner, "gpio HIGH  rc=%d", rc);
    }
#else
    UNUSED_VAR(istate);
#endif


    return rc;
}
#endif

#if !BMI160_CONFIG_ENABLE_SEE_LITE
/** See sns_bmi160_hal.h */


/** See sns_bmi160_hal.h */
#ifndef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
void bmi160_write_gpio(sns_sensor_instance *instance, uint32_t gpio,
        bool is_chip_pin,
        sns_gpio_drive_strength drive_strength,
        sns_gpio_pull_type pull,
        sns_gpio_state gpio_state)
{
    sns_service_manager *smgr = instance->cb->get_service_manager(instance);
    sns_gpio_service *gpio_svc = (sns_gpio_service*)smgr->get_service(smgr, SNS_GPIO_SERVICE);
    sns_rc rc = SNS_RC_SUCCESS;

    rc = gpio_svc->api->write_gpio(gpio, is_chip_pin, drive_strength, pull, gpio_state);

    if (rc != SNS_RC_SUCCESS)
    {
        BMI160_INST_LOG(ERROR, instance, "rc = %d", rc);
    }
}
#endif
#endif


bool bmi160_sbus_is_in_normal_mode(
        uint8_t pmu_stat_reg)
{
    bool ret = 0;

    ret = (BMI160_REGV_PMU_STAT_ACC_NORMAL == BST_GET_VAL_BITBLOCK(pmu_stat_reg, 4, 5));

    ret = ret || (BMI160_REGV_PMU_STAT_GYR_NORMAL == BST_GET_VAL_BITBLOCK(pmu_stat_reg, 2, 3));

    ret = ret || (BMI160_REGV_PMU_STAT_MAG_NORMAL == BST_GET_VAL_BITBLOCK(pmu_stat_reg, 0, 1));

    return ret;
}

/**
 * Read wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           read buffer
 * @param[i] bytes            bytes to read
 * @param[o] xfer_bytes       bytes read
 *
 * @return sns_rc
 */
    static
sns_rc bmi160_com_read_wrapper(
        sns_sync_com_port_service *scp_service,
        sns_sync_com_port_handle *port_handle,
        uint32_t reg_addr,
        uint8_t *buffer,
        uint32_t bytes,
        uint32_t *xfer_bytes)
{
    sns_port_vector port_vec;
    port_vec.buffer = buffer;
    port_vec.bytes = bytes;
    port_vec.is_write = false;
    port_vec.reg_addr = reg_addr;

    return scp_service->api->sns_scp_register_rw(port_handle,
            &port_vec,
            1,
            false,
            xfer_bytes);
}


bmi160_instance_state *bmi160_inst_singleton = NULL;

sns_rc bmi160_sbus_read_wrapper(
        void                *sbus_obj,
        uint32_t            reg_addr,
        uint8_t             *buf,
        uint32_t            len)
{
    bmi160_instance_state   *istate = (bmi160_instance_state *)sbus_obj;
    sns_port_vector         port_vec;
    uint32_t                xfer_bytes = 0;

    sns_rc                  rc;
    uint32_t                delay_us = 0;

    if (len < 1) {
        return SNS_RC_INVALID_VALUE;
    }

    //<wait according to spec>
    if (!istate->sbus_in_normal_mode) {
        sns_time ts_curr = bmi160_get_sys_tick();
        uint32_t elapse_us;

        if (ts_curr > istate->ts_last_sbus_write) {
            elapse_us = bmi160_convert_tick_2_us(ts_curr - istate->ts_last_sbus_write);
        } else {
            int32_t elapse = ts_curr - istate->ts_last_sbus_write;
            elapse_us = bmi160_convert_tick_2_us(elapse);
        }

        if (elapse_us > BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US) {
            delay_us = 0;
        } else {
            delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US - elapse_us;
        }

        if (delay_us > 0) {
            bmi160_delay_us(delay_us);
#if BMI160_CONFIG_ENABLE_WAIT4CMD_IMPROVED
            bmi160_hal_update_pmu_stat(istate, false);
#endif
        }
    }
    //</wait according to spec>

    port_vec.buffer = buf;
    port_vec.bytes = len;
    port_vec.is_write = false;
    port_vec.reg_addr = reg_addr;

    //rc = istate->scp_service->api->sns_scp_register_rw(
    istate->ts_last_sbus_read_pre = bmi160_get_sys_tick();

    rc = bmi160_fp_scp_rw(
            istate->com_port_info.port_handle,
            &port_vec, 1,
            false,
            &xfer_bytes);

    UNUSED_VAR(xfer_bytes);


    if ((!istate->sbus_mon_single_byte_rw_off) || (SNS_RC_SUCCESS != rc)) {
        if (delay_us > 0) {
            BMI160_INST_LOG(HIGH, istate->owner, "sbrd:%d", delay_us);
        }

        if (1 == len) {
            BMI160_INST_LOG(HIGH, istate->owner, "<sbus_read> <@0x%02x:0x%02x rc:%d>",
                    reg_addr, buf[0], rc);
        }
    }

    if (SNS_RC_SUCCESS == rc) {
    } else {
        BMI160_INST_LOG(ERROR, istate->owner, "ERROR!!! sbus_read err: %d @0x%02x", rc, reg_addr);
    }

    return rc;
}

sns_rc bmi160_sbus_write_wrapper(
        void        *sbus_obj,
        uint32_t    reg_addr,
        uint8_t     *buf,
        uint32_t    len)
{
    bmi160_instance_state *istate;
    sns_rc          rc;
    uint32_t        delay_us = 0;

    bool            cmd_valid = 0;
    uint8_t         pmu_stat_bit_start = 0xff;
    uint8_t         pmu_stat_reg = 0;

    sns_port_vector port_vec;
    uint32_t        xfer_bytes = 0;
    union bmi160_hw_err_stat hw_err_st;

    istate = (bmi160_instance_state *)sbus_obj;


    if (len >= 1) {
    } else {
        return SNS_RC_INVALID_VALUE;
    }

    //<wait according to spec>
    if (!istate->sbus_in_normal_mode) {
        sns_time ts_curr = bmi160_get_sys_tick();
        uint32_t elapse_us;

        if (ts_curr > istate->ts_last_sbus_write) {
            elapse_us = bmi160_convert_tick_2_us(ts_curr - istate->ts_last_sbus_write);
        } else {
            int32_t elapse = ts_curr - istate->ts_last_sbus_write;
            elapse_us = bmi160_convert_tick_2_us(elapse);
        }

        if (elapse_us > BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US) {
            delay_us = 0;
        } else {
            delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US - elapse_us;
        }

        if (delay_us > 0) {
            bmi160_delay_us(delay_us);
#if BMI160_CONFIG_ENABLE_WAIT4CMD_IMPROVED
            bmi160_hal_update_pmu_stat(istate, false);
#endif
        }
    }
    //</wait according to spec>

    if ((1 == len) && (BMI160_REGA_CMD_CMD == reg_addr)) {
        cmd_valid = 1;
        switch (buf[0]) {
            case BMI160_REGV_CMD_ACC_MODE_NORMAL:
            case BMI160_REGV_CMD_MAG_MODE_NORMAL:
                //note:this API is not suitable for frequent power mode transitions because the timer will be started too frequently
                break;
            case BMI160_REGV_CMD_GYR_MODE_NORMAL:
                break;
            case BMI160_REGV_CMD_ACC_MODE_LP:
            case BMI160_REGV_CMD_ACC_MODE_SUSP:
                pmu_stat_bit_start =  4;

                if (BMI160_REGV_CMD_ACC_MODE_LP == buf[0]) {
                    istate->accel_info.in_lpm = 1;
                }
                break;
            case BMI160_REGV_CMD_GYR_MODE_SUSP:
                pmu_stat_bit_start =  2;
                break;
            case BMI160_REGV_CMD_MAG_MODE_SUSP:
                pmu_stat_bit_start =  0;
                break;
            case BMI160_REGV_CMD_SOFT_RESET:
                istate->sbus_in_normal_mode = false;
                break;
            case BMI160_REGV_CMD_INT_RESET:
                break;
            case BMI160_REGV_CMD_FIFO_FLUSH:
                break;
            default:
                cmd_valid = 0;
                break;
        }
    }

    if (0xff != pmu_stat_bit_start) {
        rc = bmi160_sbus_read_wrapper(sbus_obj, BMI160_REGA_USR_PMU_STATUS, &pmu_stat_reg, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        pmu_stat_reg = BST_SET_VAL_BITBLOCK(pmu_stat_reg, pmu_stat_bit_start, pmu_stat_bit_start + 1, 0);
        if (bmi160_sbus_is_in_normal_mode(pmu_stat_reg)) {
        } else {
            istate->sbus_in_normal_mode = 0;
            BMI160_INST_LOG(HIGH, istate->owner, "sbus in susp pmu");  //does not work for some reason TOCHECK
        }
    }


    port_vec.buffer = buf;
    port_vec.bytes = len;
    port_vec.is_write = true;
    port_vec.reg_addr = reg_addr;

    //rc = istate->scp_service->api->sns_scp_register_rw(
    rc = bmi160_fp_scp_rw(
            istate->com_port_info.port_handle,
            &port_vec, 1,
            true,
            &xfer_bytes);

    istate->ts_last_sbus_write = bmi160_get_sys_tick();

    if (!istate->sbus_mon_single_byte_rw_off || (SNS_RC_SUCCESS != rc)) {
        if (delay_us > 0) {
            BMI160_INST_LOG(HIGH, istate->owner, "sbwd:%d", delay_us);  //does not work for some reason TOCHECK
        }

        if (1 == len) {
            BMI160_INST_LOG(HIGH, istate->owner, "<sbus_write> <@0x%02x:0x%02x rc:%d>",
                    reg_addr, buf[0], rc);
        }
    }

    if (cmd_valid) {
        rc = bmi160_dev_get_reg_hw_err_stat(istate,
                &hw_err_st);
    }

    if (SNS_RC_SUCCESS == rc) {
    } else {
        BMI160_INST_LOG(ERROR, istate->owner, "ERROR!!! sbus_write err: %d @0x%02x", rc, reg_addr);
    }

    return rc;
}

sns_rc bmi160_dev_reg_read_modify_write(
        void        *sbus_obj,
        uint8_t     rega,
        uint8_t     bit_start,
        uint8_t     bit_end,
        uint8_t     bit_block_val)
{
    sns_rc          rc;

    uint8_t         regv = 0;


    rc = bmi160_sbus_read_wrapper(sbus_obj,
            rega, &regv, 1);

    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    if (BST_GET_VAL_BITBLOCK(regv, bit_start, bit_end) != bit_block_val) {
        regv = BST_SET_VAL_BITBLOCK(regv, bit_start, bit_end, bit_block_val);
        rc = bmi160_sbus_write_wrapper(sbus_obj, rega, &regv, 1);
    }

    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    return SNS_RC_SUCCESS;
}


sns_rc bmi160_dev_get_reg_hw_err_stat(
        void        *sbus_obj,
        union bmi160_hw_err_stat        *hw_err_st)
{
    sns_rc                  rc;
    uint8_t                 si_buf;
    bmi160_instance_state   *istate = (bmi160_instance_state *)sbus_obj;

    UNUSED_VAR(istate);

    rc = bmi160_sbus_read_wrapper(
            sbus_obj,
            BMI160_REGA_USR_ERR_REG,
            &si_buf,
            1);
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    hw_err_st->bits.mag_drdy_err    = BST_GET_VAL_BITBLOCK(si_buf, 7, 7);
    hw_err_st->bits.drop_cmd_err    = BST_GET_VAL_BITBLOCK(si_buf, 6, 6);
    hw_err_st->bits.i2c_fail_err    = BST_GET_VAL_BITBLOCK(si_buf, 5, 5);
    hw_err_st->bits.err_code        = BST_GET_VAL_BITBLOCK(si_buf, 1, 4);
    hw_err_st->bits.fatal_err       = BST_GET_VAL_BITBLOCK(si_buf, 0, 0);

    if (hw_err_st->regv && (0x80 != hw_err_st->regv)) {
        BMI160_INST_LOG(ERROR, istate->owner, "CMD_WARN!!! hw_err_st: 0x%x %d %d",
                hw_err_st->regv,
                hw_err_st->bits.fatal_err,
                hw_err_st->bits.err_code);

        BMI160_INST_LOG(ERROR, istate->owner, "CMD_WARN!!! hw_err_st: 0x%x %d %d",
                hw_err_st->bits.mag_drdy_err,
                hw_err_st->bits.drop_cmd_err,
                hw_err_st->bits.i2c_fail_err);
    } else if (0x80 == hw_err_st->regv) {
        BMI160_INST_LOG(ERROR, istate->owner, "NOTICE!!! hw_err_st: mag_drdy_err");
    }

    return SNS_RC_SUCCESS;
}

    static
sns_rc bmi160_dev_get_reg_fifo_count(
        void                            *sbus_obj,
        uint16_t                        *fifo_cnt)
{
    sns_rc                              rc;;
    uint8_t si_buf[2] = "";

    rc = bmi160_sbus_read_wrapper(sbus_obj,
            BMI160_REGA_USR_FIFO_LENGTH_0,
            si_buf, 2);

    *fifo_cnt = ((si_buf[1] & 0x07) << 8) | si_buf[0];

    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    return rc;
}

    static
sns_rc bmi160_dev_sw_reset(
        sns_sensor_instance     *instance,
        bmi160_sensor_type      sensor)
{
    UNUSED_VAR(instance);
    UNUSED_VAR(sensor);

    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_SOFT_RESET);
    bmi160_delay_us(BMI160_SPEC_SOFT_RESET_DELAY_TIME_MS * 1000);

    return SNS_RC_SUCCESS;
}

/**
 * see sns_bmi160_hal.h
 */
    static
sns_rc bmi160_hal_set_default_state(
        sns_sensor_instance         *instance,
        bmi160_sensor_type          sensor)
{
    UNUSED_VAR(instance);
    UNUSED_VAR(sensor);

    return SNS_RC_SUCCESS;
}

    static
void bmi160_dev_get_cmd_info(
        uint8_t                 regv_cmd,
        bmi160_cmd_type_t       *type,
        uint32_t                *exec_time_us)
{

    *exec_time_us = 0;

    switch (regv_cmd) {
        case BMI160_REGV_CMD_ACC_MODE_NORMAL:
        case BMI160_REGV_CMD_ACC_MODE_LP:
            *exec_time_us = BMI160_SPEC_ACC_STARTUP_TIME_US;
            break;
        case BMI160_REGV_CMD_GYR_MODE_NORMAL:
        case BMI160_REGV_CMD_GYR_MODE_FSUP:
            *exec_time_us = BMI160_SPEC_GYR_STARTUP_TIME_US;
            break;
        case BMI160_REGV_CMD_MAG_MODE_NORMAL:
        case BMI160_REGV_CMD_MAG_MODE_SUSP:
            *exec_time_us = BMI160_SPEC_MAG_STARTUP_TIME_US;
            break;
        default:
            break;
    }


    if ((regv_cmd >= BMI160_REGV_CMD_ACC_MODE_SUSP) && (regv_cmd <= BMI160_REGV_CMD_ACC_MODE_LP)) {
        *type = BMI160_CMD_TYPE_ACC;
    } else if ((regv_cmd >= BMI160_REGV_CMD_GYR_MODE_SUSP) && (regv_cmd <= BMI160_REGV_CMD_GYR_MODE_FSUP)) {
        *type = BMI160_CMD_TYPE_GYR;
    } else if ((regv_cmd >= BMI160_REGV_CMD_MAG_MODE_SUSP) && (regv_cmd <= BMI160_REGV_CMD_MAG_MODE_LP2)) {
        *type = BMI160_CMD_TYPE_MAG;
    } else {
        *type = BMI160_CMD_TYPE_MISC;
    }
}

    static
void bmi160_dev_parse_data_ts(
        const uint8_t           *buf,
        uint32_t                *ts_dev)
{
    *ts_dev = ((buf[2] << 16)
            | (buf[1] << 8)
            | (buf[0]));
}


BST_ASSUME_TS_IS_64BIT
    static inline
sns_time bmi160_hal_get_ts_backward(
        sns_time now,
        sns_time elapse)
{
    return now - elapse;
}

BST_ASSUME_TS_IS_64BIT
    static inline
sns_time bmi160_hal_get_ts_forward(
        sns_time now,
        sns_time elapse)
{
    return now + elapse;
}



    static
sns_rc bmi160_hal_read_int_ctx(
        bmi160_instance_state           *istate,
        bmi160_int_check_ctx            *int_check_ctx)
{
    sns_rc                      rc;
    int8_t                      reg_addr_start = -1;
    int8_t                      reg_addr_end = -1;
    int8_t                      len = 0;
    uint8_t                     si_buf[BMI160_REGA_USR_FIFO_LENGTH_1 - BMI160_REGA_USR_SENSORTIME_0 + 1] = "";


    bmi160_int_en_flag_t        *int_en_flags = &istate->int_en_flags_curr;
    struct bmi160_reg_int_context   *ctx = &istate->reg_int_ctx;

    sns_time                    t1;
    sns_time                    t2;

    ctx->avail.flags = 0;
    //always include ts
    if (int_check_ctx->req_ts_hw) {
        reg_addr_start = BMI160_REGA_USR_SENSORTIME_0;
        ctx->avail.bits.ts = 1;
        ctx->avail.bits.status = 1;
    } else {
        reg_addr_start = BMI160_REGA_USR_INT_STATUS_0;
    }

    ctx->avail.bits.int_status_0 = 1;
    ctx->avail.bits.int_status_1 = 1;


    if (!int_en_flags->bits.dt) {
        reg_addr_end = BMI160_REGA_USR_INT_STATUS_1;
    } else {
        reg_addr_end = BMI160_REGA_USR_INT_STATUS_2;
        ctx->avail.bits.int_status_2 = 1;
    }

    if (int_en_flags->bits.fifo.flag) {
        reg_addr_end = BMI160_REGA_USR_FIFO_LENGTH_1;
        ctx->avail.bits.int_status_2 = 1;
        ctx->avail.bits.int_status_3 = 1;
        ctx->avail.bits.temperature = 1;
        ctx->avail.bits.fifo_len = 1;
    } else {
    }

    len = reg_addr_end - reg_addr_start + 1;
    if ((len > 0) && (len <= BST_ARRAY_SIZE(si_buf))) {
        t1 = bmi160_get_sys_tick();
        rc = bmi160_sbus_read_wrapper(istate,
                reg_addr_start, int_check_ctx->req_ts_hw ? si_buf : (si_buf + 4), len);
        BMI160_DD_CHECK_RETVAL(rc, 0);

        if (int_check_ctx->req_ts_hw) {
            t2 = bmi160_get_sys_tick();

            uint32_t    delta;
            bool        delta_rej = false;


            delta = bmi160_get_time_elapse_sys(t1, t2);
            BMI160_INST_LOG(LOW, istate->owner, "NOTICE delta_i:%u %d", (uint32_t)delta, (istate->async_com_read_requqest - istate->async_com_read_response));

            if (delta <= (istate->ticks_in_1ms)) {
            } else {
                delta_rej = true;
            }
#if BMI160_CONFIG_ENABLE_TS_IMPROVE
            if (!delta_rej) {
                if (istate->ticks_delta_i > 0) {
                    uint32_t devi = BST_ABS((int32_t)delta - (int32_t)istate->ticks_delta_i);
                    if ((devi << 1) <= istate->ticks_delta_i) {
                        delta_rej = false;
                        istate->ticks_delta_i = ((istate->ticks_delta_i << 3) + (delta << 1)) / 10;
                    } else {
                        delta_rej = true;
                        BMI160_INST_LOG(MED, istate->owner, "devi_i:%u", devi);
                    }
                } else {
                    istate->ticks_delta_i = delta;
                    BMI160_INST_LOG(MED, istate->owner, "delta_i_init:%u", delta);
                }
            }
#endif

            if (!delta_rej) {
#if BMI160_CONFIG_ENABLE_WORKAROUND_4_SPI_SYNC_READ_LONG_DELAY
                uint32_t delay_read = (uint32_t)(istate->xfer_time_per_byte_ticks * 4);
                ctx->ts_sys = bmi160_hal_get_ts_forward(istate->ts_last_sbus_read_pre, delay_read);
#else
#if BMI160_CONFIG_SEE_LITE_SLOW_CLOCK
                ctx->ts_sys = bmi160_hal_get_ts_backward(t2, istate->bus_is_spi ? 1 : 10);
#else
                ctx->ts_sys = bmi160_hal_get_ts_backward(t2, (len - 1) * istate->xfer_time_per_byte_ticks);
#endif
#endif

                bmi160_dev_parse_data_ts(si_buf, &ctx->ts_dev);
            } else {
                BMI160_INST_LOG(HIGH, istate->owner, "NOTICE delta_i:%u", (uint32_t)delta);
                ctx->avail.bits.ts = 0;
            }

            ctx->status    = si_buf[3];

            BMI160_INST_LOG(MED, istate->owner, "ts_irq_ctx<%u,%u,0x%x>", BMI160_SYS_TIME_LH(t2), BMI160_SYS_TIME_LH(ctx->ts_sys), ctx->ts_dev);
        }

        ctx->int_status_0 = si_buf[4];
        ctx->int_status_1 = si_buf[5];
        ctx->int_status_2 = si_buf[6];

        if (ctx->avail.bits.temperature) {
            ctx->temperature = (si_buf[BMI160_REGA_USR_TEMPERATURE_0 - BMI160_REGA_USR_SENSORTIME_0]
                    | (si_buf[BMI160_REGA_USR_TEMPERATURE_1 - BMI160_REGA_USR_SENSORTIME_0] << 8));
        }

        if (ctx->avail.bits.fifo_len) {
            ctx->fifo_len =  (si_buf[BMI160_REGA_USR_FIFO_LENGTH_0 - BMI160_REGA_USR_SENSORTIME_0]
                    | (si_buf[BMI160_REGA_USR_FIFO_LENGTH_1 - BMI160_REGA_USR_SENSORTIME_0] << 8));
        }
    } else {
        return SNS_RC_INVALID_VALUE;
    }

    return SNS_RC_SUCCESS;
}

void bmi160_dev_parse_int_stat_flags(
        bmi160_reg_int_ctx_t    *ctx,
        bmi160_int_stat_flag_t  *int_stat)
{
    int_stat->bits.md       = BST_GET_VAL_BIT(ctx->int_status_0, 2);
    int_stat->bits.dt       = BST_GET_VAL_BIT(ctx->int_status_0, 4);
    int_stat->bits.step     = BST_GET_VAL_BIT(ctx->int_status_0, 0);

    int_stat->bits.drdy     = BST_GET_VAL_BIT(ctx->int_status_1, 4);
    int_stat->bits.ff_full  = BST_GET_VAL_BIT(ctx->int_status_1, 5);
    int_stat->bits.ff_wml   = BST_GET_VAL_BIT(ctx->int_status_1, 6);

}

sns_rc bmi160_hal_cmd_deque(bmi160_instance_state   *istate)
{
    struct bmi160_cmd_req       *cmd_req_cache = NULL;

    sns_time                    ts_req = (sns_time)(-1);


    if (istate->cmd_handler.cmd_req_acc.pending) {
        BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x@%u", istate->cmd_handler.cmd_req_acc.regv, istate->cmd_handler.cmd_req_acc.ts_req);

        BST_ASSUME_TS_IS_64BIT
            if (BMI160_TIME_ET(istate->cmd_handler.cmd_req_acc.ts_req, ts_req)) {
                cmd_req_cache = &istate->cmd_handler.cmd_req_acc;

                ts_req = cmd_req_cache->ts_req;
            }
    }

    if (istate->cmd_handler.cmd_req_gyr.pending) {
        BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x@%u", istate->cmd_handler.cmd_req_gyr.regv, istate->cmd_handler.cmd_req_gyr.ts_req);
        if (BMI160_TIME_ET(istate->cmd_handler.cmd_req_gyr.ts_req, ts_req)) {
            cmd_req_cache = &istate->cmd_handler.cmd_req_gyr;

            ts_req = cmd_req_cache->ts_req;
        }
    }

    //TODOMAG

    if (NULL != cmd_req_cache) {
        cmd_req_cache->pending = 0;
        BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x selected from pile", cmd_req_cache->regv);
        bmi160_hal_send_cmd(istate, cmd_req_cache->regv);
    }


    return SNS_RC_SUCCESS;
}


bool bmi160_dev_is_last_cmd_done(
        uint8_t                 regv_last_cmd,
        uint8_t                 regv_pmu_stat)
{
    bool                        cmd_done = false;

    switch (regv_last_cmd) {
        case BMI160_REGV_CMD_ACC_MODE_NORMAL:
            if (BST_GET_VAL_BITBLOCK(regv_pmu_stat, 4, 5) == BMI160_REGV_PMU_STAT_ACC_NORMAL) {
                cmd_done = true;
            }
            break;
        case BMI160_REGV_CMD_ACC_MODE_LP:
            if (BST_GET_VAL_BITBLOCK(regv_pmu_stat, 4, 5) == BMI160_REGV_PMU_STAT_ACC_LP1) {
                cmd_done = true;
            }
            break;
        case BMI160_REGV_CMD_GYR_MODE_NORMAL:
            if (BST_GET_VAL_BITBLOCK(regv_pmu_stat, 2, 3) == BMI160_REGV_PMU_STAT_GYR_NORMAL) {
                cmd_done = true;
            }
            break;
        case BMI160_REGV_CMD_MAG_MODE_NORMAL:
            if (BST_GET_VAL_BITBLOCK(regv_pmu_stat, 0, 1) == BMI160_REGV_PMU_STAT_MAG_NORMAL) {
                cmd_done = true;
            }
            break;
        default:
            cmd_done = true;
            break;
    }


    return cmd_done;
}

void bmi160_hal_wait4cmd2finish(
        bmi160_instance_state   *istate,
        uint8_t                 regv_cmd,
        sns_time                time_left)
{
#if BMI160_CONFIG_ENABLE_WAIT4CMD_IMPROVED
    uint8_t         regv_pmu_stat;

    sns_rc          rc;

    bool            cmd_done = false;

    while (time_left != 0) {
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_PMU_STATUS, &regv_pmu_stat, 1);
        if (SNS_RC_SUCCESS == rc) {
            cmd_done = bmi160_dev_is_last_cmd_done(regv_cmd, regv_pmu_stat);
        }

        if (cmd_done) {
            istate->sbus_in_normal_mode = bmi160_sbus_is_in_normal_mode(regv_pmu_stat);
            break;
        } else {
            time_left = time_left >> 1;
            rc = sns_busy_wait(time_left);
        }
    }
#else
    sns_busy_wait(time_left);
#endif
}

sns_rc bmi160_hal_send_cmd(
        bmi160_instance_state   *istate,
        uint8_t                 regv_cmd)
{
    sns_rc                      rc = SNS_RC_SUCCESS;

    uint32_t                    exec_time_us = 0;

    struct bmi160_cmd_req       *cmd_req_cache = NULL;
    sns_time                    ts_curr;
    bool                        exec_now = false;
    bmi160_cmd_type_t           cmd_type = BMI160_CMD_TYPE_UNKNOWN;



    bmi160_dev_get_cmd_info(regv_cmd, &cmd_type, &exec_time_us);


    if ((exec_time_us > 0) && (0 == istate->pmu_stat.reg)) {
        exec_time_us += 300;
    }

    switch (cmd_type) {
        case BMI160_CMD_TYPE_ACC:
            cmd_req_cache = &istate->cmd_handler.cmd_req_acc;
            break;
        case BMI160_CMD_TYPE_GYR:
            cmd_req_cache = &istate->cmd_handler.cmd_req_gyr;
            break;
        case BMI160_CMD_TYPE_MAG:
            //TODOMAG
            break;
        default:
            BMI160_INST_LOG(HIGH, istate->owner, "misc cmd: 0x%x will be exec now or soon", regv_cmd);
            break;
    }


    ts_curr = bmi160_get_sys_tick();

    if (istate->cmd_handler.cmd_in_proc) {
        if (NULL != cmd_req_cache) {
            if (cmd_req_cache->pending) {
                BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x@%u will be replaced by-",
                        (uint32_t)cmd_req_cache->regv, (uint32_t)cmd_req_cache->ts_req);
                BMI160_INST_LOG(HIGH, istate->owner, "0x%x@%u",
                        (uint32_t)regv_cmd, (uint32_t)ts_curr);
            }
            cmd_req_cache->regv = regv_cmd;
            cmd_req_cache->type = cmd_type;
            cmd_req_cache->pending = 1;
            cmd_req_cache->exec_time_us = exec_time_us;
            cmd_req_cache->ts_req = ts_curr;

            if (regv_cmd == istate->cmd_handler.cmd_last_written) {
                cmd_req_cache->pending = 0;

                BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x will be ignored", regv_cmd);
            }
        } else {
            int32_t delta = 0;

            delta = istate->cmd_handler.ts_cmd_expire_est - ts_curr;
            if (delta > 0) {
                BMI160_INST_LOG(HIGH, istate->owner, "wait4finish of current cmd: 0x%x", istate->cmd_handler.cmd_last_written);
                bmi160_hal_wait4cmd2finish(istate, istate->cmd_handler.cmd_last_written, delta);
                //TODO2: we should cancel the current timer, but how
                //also will bmi160_hal_start_timer() fail if the timer is not canceled?
            }

            exec_now = true;
        }
    } else {
        exec_now = true;
    }


    if (exec_now) {
        rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_CMD_CMD, &regv_cmd, 1);

        if (BMI160_CMD_TYPE_GYR == cmd_type) {
            istate->cnt_hw_res_est = 0;
            //force learning new res
            BMI160_INST_LOG(MED, istate->owner, "avail_1st_est reset");
        }

        istate->cmd_handler.cmd_last_written = regv_cmd;
        if (exec_time_us > 0) {
            istate->cmd_handler.cmd_in_proc = 1;
            istate->cmd_handler.ts_cmd_expire_est = ts_curr + sns_convert_ns_to_ticks(exec_time_us * 1000);
            BMI160_INST_LOG(HIGH, istate->owner, "cmd: 0x%x might expire@%u", regv_cmd, istate->cmd_handler.ts_cmd_expire_est);



            bmi160_hal_start_timer(istate->owner, istate->cmd_handler.timer_cmd_stream, sns_convert_ns_to_ticks(exec_time_us * 1000));

        } else {
            istate->cmd_handler.cmd_in_proc = 0;

            bmi160_hal_cmd_deque(istate);
        }
        //bmi160_hal_dump_reg(istate->owner);
    } else {
    }


    return SNS_RC_SUCCESS;
}

sns_rc bmi160_hal_update_pmu_stat(
        bmi160_instance_state   *istate,
        bool                    check_err)
{
    sns_rc                      rc;
    uint8_t                     buf[2];

    if (check_err) {
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_ERR_REG, buf, 2);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        BMI160_INST_LOG(HIGH, istate->owner, "update_pmu_stat <sbus_read> <@0x%02x:0x%02x rc:%d>",
                BMI160_REGA_USR_ERR_REG, buf[0], rc);

        BMI160_INST_LOG(HIGH, istate->owner, "update_pmu_stat <sbus_read> <@0x%02x:0x%02x rc:%d>",
                BMI160_REGA_USR_PMU_STATUS, buf[1], rc);
    } else {
        //some platform the odd address access will make a crash, so we make a copy from buf[0] to buf[1]
        //rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_PMU_STATUS, buf + 1, 1);
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_PMU_STATUS, buf, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        buf[1] = buf[0];
    }

#if 0
    if (BST_GET_VAL_BITBLOCK(istate->pmu_stat.reg, 2, 3) != BST_GET_VAL_BITBLOCK(buf[1], 2, 3)) {
        istate->cnt_hw_res_est = 0;
        //force learning new res
        BMI160_INST_LOG(MED, istate->owner, "avail_1st_est reset");
    }
#endif

    istate->pmu_stat.reg = buf[1];
    istate->sbus_in_normal_mode = bmi160_sbus_is_in_normal_mode(istate->pmu_stat.reg);


    return SNS_RC_SUCCESS;
}

sns_rc bmi160_hal_prepare_spi_if(bmi160_instance_state *istate)
{
    sns_rc                      rc;
    uint8_t                     regv;

    rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_CMD_EXT_MODE, &regv, 1);
    bmi160_delay_us(BMI160_SPEC_IF_SPI_SWITCH_TIME_US);
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    return SNS_RC_SUCCESS;
}


sns_rc bmi160_hal_sensor_prepare_spi_if(sns_sensor * const this)
{
    sns_rc rv = SNS_RC_SUCCESS;
    uint8_t regv;
    sns_port_vector         port_vec;
    uint32_t                xfer_bytes = 0;
    bmi160_state *sstate = (bmi160_state *) this->state->state;

    port_vec.buffer = &regv;
    port_vec.bytes  = 1;
    port_vec.is_write = false;
    port_vec.reg_addr = BMI160_REGA_CMD_EXT_MODE;

    rv = sstate->scp_service->api->sns_scp_register_rw(
            sstate->common.com_port_info.port_handle,
            &port_vec, 1,
            false,
            &xfer_bytes);

    UNUSED_VAR(xfer_bytes);
    bmi160_delay_us(BMI160_SPEC_IF_SPI_SWITCH_TIME_US);
    BMI160_DD_CHECK_RETVAL(rv, SNS_RC_SUCCESS)
        return  rv;
}


sns_rc bmi160_hal_reset_device(
        sns_sensor_instance     *instance,
        bmi160_sensor_type      sensor)
{
    sns_rc                      rc;

    uint8_t                     regv;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    /** HW reset only when both Accel and Gyro are requested for
     *  reset. */
    if (sensor == (BMI160_ACCEL | BMI160_GYRO | BMI160_MOTION_DETECT | BMI160_SENSOR_TEMP))
    {
        rc = bmi160_dev_sw_reset(instance, sensor);

        if (SNS_BUS_SPI == istate->com_port_info.com_config.bus_type) {
            bmi160_hal_prepare_spi_if(istate);
        }

        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_CHIP_ID, &regv, 1);
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_PMU_STATUS, &regv, 1);
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_ACC_RANGE, &regv, 1);
        if (rc == SNS_RC_SUCCESS)
        {
            rc = bmi160_hal_set_default_state(instance, sensor);
        }
    }


    return SNS_RC_SUCCESS;
}


/**
 * see sns_bmi160_hal.h
 */
sns_rc bmi160_hal_get_who_am_i(
        sns_sync_com_port_service   *scp_service,
        sns_sync_com_port_handle    *port_handle,
        uint8_t                     *buffer)
{
    sns_rc rv = SNS_RC_SUCCESS;
    uint32_t xfer_bytes;

    rv = bmi160_com_read_wrapper(scp_service,
            port_handle,
            BMI160_REGA_USR_CHIP_ID,
            buffer,
            1,
            &xfer_bytes);

    if (rv != SNS_RC_SUCCESS
            ||
            xfer_bytes != 1)
    {
        rv = SNS_RC_FAILED;
    }

    return rv;
}


sns_rc bmi160_hal_switch_2_spi(
        sns_sync_com_port_service   *scp_service,
        sns_sync_com_port_handle    *port_handle)
{
    sns_rc      rv = SNS_RC_SUCCESS;
    uint32_t    xfer_bytes;
    uint8_t     regv;

    rv = bmi160_com_read_wrapper(scp_service,
            port_handle,
            0x7f,
            &regv,
            1,
            &xfer_bytes);

    if (rv != SNS_RC_SUCCESS
            ||
            xfer_bytes != 1)
    {
        rv = SNS_RC_FAILED;
    }

    return rv;
}


void bmi160_hal_match_odr(
        bmi160_sensor_type      sensor_type,
        float                   odr_req,
        float                   *odr_matched,
        bmi160_regv_odr_t       *regv)
{
    uint8_t i;
    struct bmi160_odr_regv_map *map;

    uint8_t                     regv_min = 0;

    *regv = BMI160_REGV_ODR_OFF;
    *odr_matched = 0;

    //if ((odr_req >= 0.0f) && (odr_req <= 0.0f))
    if (BST_IS_FLOAT_ZERO(odr_req)) {
        return;
    }

    if (BMI160_ACCEL == sensor_type) {
        regv_min = BMI160_REGV_ODR_12_5HZ;
    } else if (BMI160_GYRO == sensor_type) {
        regv_min = BMI160_REGV_ODR_25HZ;
    }

    //TODOMAG

    for (i = 0; i < ARR_SIZE(BMI160_REGV_ODR_MAP); i++) {
        map = BMI160_REGV_ODR_MAP + i;

        if (odr_req <= map->odr) {
            if (map->regv >= regv_min)  {
                *regv = (bmi160_regv_odr_t) (map->regv);
                *odr_matched = map->odr;
                break;
            }
        }
    }
}


sns_rc bmi160_hal_config_int_output(
        sns_sensor_instance     *const  instance,
        bool                    enable)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    uint8_t                     regv;

    if (enable) {
        regv = 0x0a;
    } else {
        regv = 0x00;
    }

    rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_OUT_CTRL, 0, 3, regv);

    return rc;
}

/**
 * see sns_bmi160_hal.h
 */
//OPTIM
void bmi160_hal_dump_reg(sns_sensor_instance     *this)
{
#if BMI160_CONFIG_ENABLE_DUMP_REG
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    uint8_t                     regv;

    uint8_t reg_map[] = {

        BMI160_REGA_USR_CHIP_ID,
        BMI160_REGA_USR_ERR_REG,
        BMI160_REGA_USR_PMU_STATUS,

        BMI160_REGA_USR_ACC_CONF,
        BMI160_REGA_USR_ACC_RANGE,
        BMI160_REGA_USR_GYR_CONF,
        BMI160_REGA_USR_GYR_RANGE,

        BMI160_REGA_USR_FIFO_CONFIG_0,
        BMI160_REGA_USR_FIFO_CONFIG_1,

        BMI160_REGA_USR_INT_EN_0,
        BMI160_REGA_USR_INT_EN_1,

        BMI160_REGA_USR_INT_OUT_CTRL,

        BMI160_REGA_USR_INT_MAP_0,
        BMI160_REGA_USR_INT_MAP_1,

#if 0
        BMI160_REGA_USR_MAG_CONF,

        BMI160_REGA_USR_INT_EN_2,
        BMI160_REGA_USR_INT_MAP_2,


        BMI160_REGA_USR_INT_MOTION_0,
        BMI160_REGA_USR_INT_MOTION_1,

        BMI160_REGA_USR_INT_LATCH,

        BMI160_REGA_USR_IF_CONF,
        BMI160_REGA_USR_PMU_TRIGGER,


        BMI160_REGA_USR_INT_TAP_0,
        BMI160_REGA_USR_INT_TAP_1,

        BMI160_REGA_USR_PEDO_STEP_0,
        BMI160_REGA_USR_STEP_CONF_0,
        BMI160_REGA_USR_STEP_CONF_1,
#endif
    };

    uint8_t i = 0;
    uint16_t n = sizeof(reg_map) / sizeof(reg_map[0]);

    for(i = 0; i < n; i ++)
    {
        bmi160_sbus_read_wrapper(istate, reg_map[i], &regv, 1);
    }

#else
    UNUSED_VAR(this);
#endif
}


#define BMI160_SPEC_MD_THRESH_RESOLUTION 3.9025f
    static
void bmi160_hal_determine_md_param(
        bmi160_instance_state       *istate,
        bmi160_int_cfg_any_motion_t *param)
{
    uint8_t param_thresh;   //bit 0:1
    uint8_t param_dur;  //bit 0:7

    float mg;
    float dur_ms;
    float itvl_ms;

    mg = istate->md_info.sstate->md_config.thresh * 1000 / G;
    dur_ms = istate->md_info.sstate->md_config.win * 1000;

    param_thresh = mg / BMI160_SPEC_MD_THRESH_RESOLUTION;
    param_thresh = param_thresh >> (istate->md_info.range_idx_req - BMI160_RANGE_ACC_PM2G);

    itvl_ms = (1e3 * (1 << (BMI160_REGV_ODR_1600HZ - BMI160_CONFIG_MD_ODR))) / 1600;
    param_dur = (dur_ms / itvl_ms);

    if (param_dur >= 1) {
        param_dur--;
    }

    param_dur = param_dur < 2 ? param_dur : 2;

    param->int_anym_dur = param_dur;
    param->int_anym_th = param_thresh;
}


sns_rc bmi160_hal_set_md_config(sns_sensor_instance *const instance, bool enable)
{
    sns_rc                      rc;
    uint8_t                     regv;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;

    if (enable) {
        bmi160_int_cfg_any_motion_t param_md;

        bmi160_hal_determine_md_param(istate, &param_md);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MOTION_0, 0, 1, param_md.int_anym_dur);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MOTION_1, 0, 7, param_md.int_anym_th);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        regv = 0;
        regv = BST_SET_VAL_BIT(regv, 7);
        regv = BST_SET_VAL_BITBLOCK(regv, 4, 6, BMI160_CONFIG_MD_ACC_BWP);
        regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, BMI160_CONFIG_MD_ODR);
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_ACC_CONF, 0, 7, regv);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_ACC_MODE_LP);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        istate->accel_info.normal_mode_req_sent = 0;
        istate->accel_info.in_lpm = 1;
        //istate->accel_info.odr_curr = BMI160_REGV_ODR_OFF;
    } else {
    }


    return SNS_RC_SUCCESS;
}


sns_rc bmi160_hal_config_int_md(
        sns_sensor_instance     *const  instance,
        bool                    enable,
        bool                    md_not_armed_event)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;

    UNUSED_VAR(instance);
    UNUSED_VAR(enable);
    UNUSED_VAR(md_not_armed_event);

    if (enable) {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_LATCH, 0, 3, BMI160_CONFIG_INT_LATCH_REGV);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_EN_0, 0, 2, 7);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MAP_0, 2, 2, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        istate->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_ENABLED;
    } else {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_EN_0, 0, 2, 0);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MAP_0, 2, 2, 0);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        istate->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
    }


    if (enable || md_not_armed_event)
    {
        bool avoid_duplicate_md_ev;

        avoid_duplicate_md_ev = (istate->int_en_flags_req.bits.md && istate->int_en_flags_curr.bits.md) && (!md_not_armed_event);

        BMI160_INST_LOG(HIGH, istate->owner,
                "bmi160_update_md_intr md_ev_type:%d %d",
                istate->md_info.md_state.motion_detect_event_type,
                istate->int_en_flags_req.bits.md | (istate->int_en_flags_curr.bits.md << 1) | (istate->md_info.client_present << 2));


        if (!avoid_duplicate_md_ev) {
            pb_send_event(instance,
                    sns_motion_detect_event_fields,
                    &istate->md_info.md_state,
                    sns_get_system_time(),
                    SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                    &istate->md_info.sstate->my_suid);
        }
    }

    return SNS_RC_SUCCESS;
}


#if 0
sns_rc bmi160_hal_disable_int_fifo(
        sns_sensor_instance     *const  instance,
        bool                    fast)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    uint8_t                     regv;

    if (fast) {
        regv = istate->cache_regv_int_map_1;
    } else {
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        istate->cache_regv_int_map_1 = regv;
    }

    if (BST_GET_VAL_BITBLOCK(regv, 5, 6) != 0) {
        regv = BST_SET_VAL_BITBLOCK(regv, 5, 6, 0);
        rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    }

    return SNS_RC_SUCCESS;
}


sns_rc bmi160_hal_restore_int_fifo(
        sns_sensor_instance     *const  instance)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    uint8_t                     regv;

    regv = istate->cache_regv_int_map_1;

    rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    return SNS_RC_SUCCESS;
}
#endif


sns_rc bmi160_hal_config_int_fifo(
        sns_sensor_instance     *const  instance,
        bool                    enable)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    uint8_t                     regv;

    if (enable) {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_EN_1, 5, 6, 3);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        if (BST_GET_VAL_BITBLOCK(regv, 5, 6) != 3) {
            regv = BST_SET_VAL_BITBLOCK(regv, 5, 6, 3);
            rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
            istate->cache_regv_int_map_1 = regv;
        }
    } else {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MAP_1, 5, 6, 0);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    }

    return SNS_RC_SUCCESS;
}

#if BMI160_CONFIG_ENABLE_DRI_MODE
sns_rc bmi160_hal_config_int_drdy(
        sns_sensor_instance     *const  instance,
        bool                    enable)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    uint8_t                     regv;

    if (enable) {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_EN_1, 4, 4, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        if (BST_GET_VAL_BIT(regv, 7) != 1) {
            regv = BST_SET_VAL_BIT(regv, 7);
            rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
            istate->cache_regv_int_map_1 = regv;
        }
    } else {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_MAP_1, 7, 7, 0);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_INT_EN_1, 4, 4, 0);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    }

    return SNS_RC_SUCCESS;
}
#endif

/** See sns_bmi160_hal.h */
void bmi160_hal_send_config_event(sns_sensor_instance *const instance)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)instance->state->state;

    sns_std_sensor_physical_config_event phy_sensor_config =
        sns_std_sensor_physical_config_event_init_default;

    char operating_mode[] = "NORMAL";
    char operating_mode_lp[] = "LOW POWER";

    pb_buffer_arg op_mode_args;

    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    struct bmi160_state         *sstate;

    /** If no sensors are enabled for streaming then don't send
     *  config event */
    if (!istate->hw_mod_needed)
    {
        return;
    }


    phy_sensor_config.has_sample_rate = true;
    phy_sensor_config.sample_rate = 0.0;

    phy_sensor_config.has_water_mark = true;
    //phy_sensor_config.water_mark = istate->accel_info.ff_wml_req | (istate->fac_test_in_progress << 16);
    phy_sensor_config.water_mark = istate->accel_info.ff_wml_curr;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = BMI160_ACC_ACTIVE_CURRENT;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.has_stream_is_synchronous = true;
    phy_sensor_config.stream_is_synchronous = false;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = true;
    /* For sensors that route data through the SDC/DAE sensor, the DAE watermark
       should be set to the number of samples stored in SDC before waking up Q6. */
    phy_sensor_config.has_DAE_watermark = false; //TODO2: DAE
    phy_sensor_config.DAE_watermark =
#if BMI160_CONFIG_ENABLE_DAE
        (state->fifo_info.max_requested_flush_ticks == 0) ?
        UINT32_MAX :
        state->fifo_info.max_requested_wmk;  //REFER_LSM_EXAMPLE
#else
    0
#endif
        ;

    if (fifo_info->publish_sensors & BMI160_ACCEL)
    {
        sstate = istate->accel_info.sstate;
        phy_sensor_config.resolution = bmi160_accel_resolutions[sstate->resolution_idx];
        phy_sensor_config.range[0] = bmi160_accel_ranges[sstate->resolution_idx].min;
        phy_sensor_config.range[1] = bmi160_accel_ranges[sstate->resolution_idx].max;

        phy_sensor_config.sample_rate = BMI160_REGV_ODR_MAP[istate->accel_info.odr_curr].odr;


#if BMI160_CONFIG_ENABLE_SEE_LITE
        op_mode_args.buf = &operating_mode_lp[0];
        op_mode_args.buf_len = sizeof(operating_mode_lp);
#else
        op_mode_args.buf = &operating_mode[0];
        op_mode_args.buf_len = sizeof(operating_mode);
#endif
        pb_send_event(instance,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                sns_get_system_time(),
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &sstate->my_suid);
    }

    if (fifo_info->publish_sensors & BMI160_GYRO)
    {
#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW
        phy_sensor_config.sample_rate = (istate->gyro_info.downsample_sw_factor <= 1) ?
            //BMI160_REGV_ODR_MAP[istate->gyro_info.odr_req].odr : istate->gyro_info.sample_rate_req;
            BMI160_REGV_ODR_MAP[istate->gyro_info.odr_curr].odr : (BMI160_REGV_ODR_MAP[istate->gyro_info.odr_curr].odr / istate->gyro_info.downsample_sw_factor);

        phy_sensor_config.water_mark = (istate->gyro_info.downsample_sw_factor <= 1) ?
            istate->gyro_info.ff_wml_curr : (istate->gyro_info.ff_wml_curr / istate->gyro_info.downsample_sw_factor);

        phy_sensor_config.water_mark = SNS_MAX(phy_sensor_config.water_mark, 1);
#else
        phy_sensor_config.sample_rate = BMI160_REGV_ODR_MAP[istate->gyro_info.odr_curr].odr;

        phy_sensor_config.water_mark = istate->gyro_info.ff_wml_curr;
#endif
        // Override above values with gyro info
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = BMI160_GYRO_ACTIVE_CURRENT;
        phy_sensor_config.has_resolution = true;
        phy_sensor_config.range_count = 2;
        phy_sensor_config.has_dri_enabled = true;
        phy_sensor_config.dri_enabled = true;
        /* For sensors that route data through the SDC/DAE sensor, the DAE watermark
           should be set to the number of samples stored in SDC before waking up Q6. */
        phy_sensor_config.has_DAE_watermark = false; //TODO2: DAE
        phy_sensor_config.DAE_watermark =
#if BMI160_CONFIG_ENABLE_DAE
            (state->fifo_info.max_requested_flush_ticks == 0) ?
            UINT32_MAX :
            state->fifo_info.max_requested_wmk;  //REFER_LSM_EXAMPLE
#else
        0
#endif
            ;

        sstate = istate->gyro_info.sstate;
        phy_sensor_config.resolution = bmi160_gyro_resolutions[sstate->resolution_idx];
        phy_sensor_config.range[0] = bmi160_gyro_ranges[sstate->resolution_idx].min;
        phy_sensor_config.range[1] = bmi160_gyro_ranges[sstate->resolution_idx].max;

        op_mode_args.buf = &operating_mode[0];
        op_mode_args.buf_len = sizeof(operating_mode);
        pb_send_event(instance,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                sns_get_system_time(),
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &sstate->my_suid);
    }


    if (fifo_info->publish_sensors & BMI160_SENSOR_TEMP)
    {
        // Override above values with sensor temperature info
        phy_sensor_config.sample_rate = (float)(istate->sensor_temp_info.sample_rate_curr);

        phy_sensor_config.has_water_mark = false;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 180;
        phy_sensor_config.has_resolution = true;
        phy_sensor_config.resolution = BMI160_SENSOR_TEMPERATURE_RESOLUTION;
        phy_sensor_config.range_count = 2;
        phy_sensor_config.range[0] = BMI160_SENSOR_TEMPERATURE_RANGE_MIN;
        phy_sensor_config.range[1] = BMI160_SENSOR_TEMPERATURE_RANGE_MAX;
        phy_sensor_config.has_dri_enabled = true;
        phy_sensor_config.dri_enabled = false;
        phy_sensor_config.has_DAE_watermark = false;

#if BMI160_CONFIG_ENABLE_SEE_LITE
        op_mode_args.buf = &operating_mode_lp[0];
        op_mode_args.buf_len = sizeof(operating_mode_lp);
#else
        op_mode_args.buf = &operating_mode[0];
        op_mode_args.buf_len = sizeof(operating_mode);
#endif
        pb_send_event(instance,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                sns_get_system_time(),
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &istate->sensor_temp_info.sstate->my_suid);
    }

    //this section has to be at the very end because of the sns_memset
    if (istate->int_en_flags_curr.bits.md)
    {
        sns_memset(&phy_sensor_config, 0, sizeof(phy_sensor_config));

        op_mode_args.buf = &operating_mode_lp[0];
        op_mode_args.buf_len = sizeof(operating_mode_lp);
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 46;  //per configuration: ODR=100,AVG=1

        pb_send_event(instance,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                sns_get_system_time(),
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &istate->md_info.sstate->my_suid);
    }

    if (fifo_info->publish_sensors & BMI160_ACCEL) {
        bmi160_send_fac_cal_event(instance, istate->accel_info.sstate);
    }

    if (fifo_info->publish_sensors & BMI160_GYRO) {
        bmi160_send_fac_cal_event(instance, istate->gyro_info.sstate);
    }

}


void bmi160_dev_format_data_temp(
        const uint8_t           buf[],
        float                   *temp)
{
    int16_t temp_lsb;
    static const float BMI160_SCALE_FACTOR_TEMP = 1.0f / 512;

    temp_lsb = (int16_t)buf[0] | (((int16_t)buf[1]) << 8);

    *temp = (temp_lsb * BMI160_SCALE_FACTOR_TEMP) + 23;
}

void bmi160_hal_convert_and_send_temp_sample(
        sns_sensor_instance     *const instance,
        sns_time                timestamp,
        const uint8_t           buf[2])
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    float                       float_temp_val;

    bmi160_dev_format_data_temp(buf, &float_temp_val);


    // factory calibration
    // Sc = C * (Sr - B)
    // Where,
    // *Sc = Calibrated sample
    // *Sr = Raw sample
    // *C = Scale
    // *B = Bias
    //
    int32_t *fac_cal_bias;
    fac_cal_bias = istate->sensor_temp_info.sstate->fac_cal_bias;

    float_temp_val = istate->sensor_temp_info.sstate->fac_cal_corr_mat.e00 *
        (float_temp_val - (fac_cal_bias[0] / BMI160_SCALE_FACTOR_DATA_TEMP));

    pb_send_sensor_stream_event(instance,
            &istate->sensor_temp_info.sstate->my_suid,
            timestamp,
            SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
            SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
            &float_temp_val,
            1,
            istate->encoded_sensor_temp_event_len);
}


//OPTIM
void bmi160_hal_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    uint8_t buffer[50];
    sns_request timer_req = {
        .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request    = buffer
    };
    sns_rc rc = SNS_RC_SUCCESS;

    BMI160_INST_LOG(MED, this, "start temperature polling timer");

    if (NULL == istate->timer_data_stream)
    {
        sns_service_manager *smgr = this->cb->get_service_manager(this);
        sns_stream_service *strm_svc =
            (sns_stream_service*) smgr->get_service(smgr, SNS_STREAM_SERVICE);
        rc = strm_svc->api->create_sensor_instance_stream(strm_svc, this,
                istate->timer_suid, &istate->timer_data_stream);
    }

    if(rc != SNS_RC_SUCCESS
            || NULL == istate->timer_data_stream) {
        BMI160_INST_LOG(ERROR, this, "error creating stream %d", rc);
        return;
    }
    sns_memset(buffer, 0, sizeof(buffer));
    req_payload.is_periodic = true;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = istate->sensor_temp_info.sampling_intvl;

    timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
            sns_timer_sensor_config_fields, NULL);
    if (timer_req.request_len > 0)
    {
        istate->timer_data_stream->api->send_request(istate->timer_data_stream, &timer_req);
        istate->sensor_temp_info.timer_is_active = true;
    }
    else
    {
        //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
        //                         "timer req encode error");
    }
}

    static
sns_rc bmi160_hal_update_couple_ts_host_and_dev_rt(
        bmi160_instance_state   *istate)
{
    sns_rc                      rc;
    sns_time                    ts;
    sns_time                    te;
    uint8_t                     si_buf[3] = "";
    uint32_t                    delta;
    bool                        delta_rej = false;


    rc = bmi160_sbus_read_wrapper(istate,
            BMI160_REGA_USR_SENSORTIME_0,
            si_buf, 3);
    ts = istate->ts_last_sbus_read_pre;
    te = bmi160_get_sys_tick();
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);


    delta = bmi160_get_time_elapse_sys(ts, te);


    if (delta <= istate->ticks_in_1ms) {
    } else {
        delta_rej = true;
    }
#if BMI160_CONFIG_ENABLE_TS_IMPROVE
    BMI160_INST_LOG(LOW, istate->owner, "NOTICE delta_p:%u %u %d", (uint32_t)delta, istate->ticks_delta_p, (istate->async_com_read_requqest - istate->async_com_read_response));

    if (!delta_rej) {
        if (istate->ticks_delta_p > 0) {
            //uint32_t devi = (delta >= istate->ticks_delta_p) ? (delta - istate->ticks_delta_p) : (istate->ticks_delta_p - delta);
            uint32_t devi = BST_ABS((int32_t)delta - (int32_t)istate->ticks_delta_p);
            if ((devi << 1) <= istate->ticks_delta_p) {
                delta_rej = false;
                istate->ticks_delta_p = ((istate->ticks_delta_p << 3) + (delta << 1)) / 10;
            } else {
                delta_rej = true;
                BMI160_INST_LOG(MED, istate->owner, "devi_p:%u", devi);
            }
        } else {
            istate->ticks_delta_p = delta;
            BMI160_INST_LOG(MED, istate->owner, "delta_p_init:%u", delta);
        }
    } else {
        if (istate->ticks_delta_p > 0) {
        } else {
            istate->ticks_delta_p = delta;
            BMI160_INST_LOG(MED, istate->owner, "delta_p_init_forced:%u", delta);
            delta_rej = false;
        }
    }
#endif

    if (!delta_rej) {
#if BMI160_CONFIG_ENABLE_WORKAROUND_4_SPI_SYNC_READ_LONG_DELAY
        uint32_t delay_read = (uint32_t)(istate->xfer_time_per_byte_ticks * 4);
        istate->ts_pair_sys_dev.ts_sys = bmi160_hal_get_ts_forward(istate->ts_last_sbus_read_pre, delay_read);
#else

#if BMI160_CONFIG_SEE_LITE_SLOW_CLOCK
        istate->ts_pair_sys_dev.ts_sys = istate->bus_is_spi? te : bmi160_hal_get_ts_backward(te, 2);
#else
        istate->ts_pair_sys_dev.ts_sys = bmi160_hal_get_ts_backward(te, 2 * istate->xfer_time_per_byte_ticks);
#endif

#endif
        istate->ts_pair_sys_dev.ts_dev = si_buf[0] | (si_buf[1] << 8) | (si_buf[2] << 16);
    } else {
        BMI160_INST_LOG(HIGH, istate->owner, "NOTICE delta_p:%u", delta);
    }

    return SNS_RC_SUCCESS;
}

//uted
    static
sns_rc bmi160_hal_get_couple_ts_host_and_dev(
        bmi160_instance_state   *istate)
{
    sns_rc                      rc;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bool                        read_ts_dev;

    if (BMI160_FIFO_READ_CTX_TYPE_WMI == fifo_info->ff_read_ctx_type) {
        if (istate->reg_int_ctx.avail.bits.ts) {
            istate->ts_pair_sys_dev.ts_sys = istate->reg_int_ctx.ts_sys;
            istate->ts_pair_sys_dev.ts_dev = istate->reg_int_ctx.ts_dev;
            read_ts_dev = false;
        } else {
            read_ts_dev = true;
        }
    } else {
        read_ts_dev = true;
    }

    if (read_ts_dev) {
        rc = bmi160_hal_update_couple_ts_host_and_dev_rt(istate);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    }

    //istate->ts_pair_sys_dev.avail_1st = 1;

    return SNS_RC_SUCCESS;
}

    static
void bmi160_hal_fifo_cor_ff_len(
        bmi160_instance_state   *istate,
        bmi160_fifo_read_ctx_t  *ctx,
        uint16_t                *ff_count)
{
    uint32_t                    time_sbus_xfer_est;
    uint32_t                    aug = 0;
    uint32_t                    tmp;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    uint16_t                    odr_ff_master = BMI160_REGV_ODR_MAP[fifo_info->ff_master_odr_curr].odr;
    const uint16_t              BMI160_SPI_CLK_CNT_PER_BYTE = 8 + BMI160_CONFIG_SEE_SPI_BYTE_XFER_WAIT_CYCLES;


    if (istate->bus_is_spi) {
        time_sbus_xfer_est = ((((*ff_count) * BMI160_SPI_CLK_CNT_PER_BYTE) + 2) * istate->bus_cycle_time_ns) / 1000;
    } else {
        time_sbus_xfer_est = 26 * (*ff_count + 3);
    }

    time_sbus_xfer_est += (BMI160_CONFIG_SEE_ASYNC_COM_DELAY_US);

    tmp = time_sbus_xfer_est * odr_ff_master;
    aug = (tmp / BMS_SCALE_S2US);
    if ((tmp - aug * BMS_SCALE_S2US) > (BMS_SCALE_S2US >> 1)) {
        aug++;
    }

    if (aug > 0) {
        aug = aug * (1 +
                ((istate->accel_info.ff_wml_req ? BMI160_FF_DATA_LEN_ACC : 0) +
                 (istate->gyro_info.ff_wml_req? BMI160_FF_DATA_LEN_GYR : 0) +
                 (0 ? BMI160_FF_DATA_LEN_MAG : 0)));    //TODOMAG

        BMI160_INST_LOG(MED, istate->owner, "[cor_ff_count] odr: %d ff_count: %d aug: %d",
                odr_ff_master, *ff_count, aug);
    }

    if (BMI160_FIFO_READ_CTX_TYPE_WMI == ctx->ctx_type) {
        if (fifo_info->ff_tm_info.boost_read_size) {

            if (fifo_info->ff_master_odr_curr <= BMI160_REGV_ODR_200HZ) {
                aug += 5 * (BMI160_FF_DATA_LEN_ACC + BMI160_FF_DATA_LEN_GYR + BMI160_FF_DATA_LEN_MAG + 1);
            } else {
                aug += 10 * (BMI160_FF_DATA_LEN_ACC + BMI160_FF_DATA_LEN_GYR + BMI160_FF_DATA_LEN_MAG + 1);
            }
        }
    } else {
        aug += 4 * (BMI160_FF_DATA_LEN_ACC + BMI160_FF_DATA_LEN_GYR + BMI160_FF_DATA_LEN_MAG + 1);
    }

    *ff_count += aug;
}

    static
sns_rc bmi160_hal_fifo_determine_ff_len(
        bmi160_instance_state   *istate,
        bmi160_fifo_read_ctx_t  *ctx,
        uint16_t                *ff_len)
{
    uint8_t                     si_buf[2] = "";

    sns_rc                      rc;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    if ((BMI160_FIFO_READ_CTX_TYPE_WMI == ctx->ctx_type) && istate->reg_int_ctx.avail.bits.fifo_len) {
        *ff_len= istate->reg_int_ctx.fifo_len;
    } else {
        *ff_len = BMI160_FF_DEPTH_BYTES;
        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_FIFO_LENGTH_0, si_buf, 2);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        *ff_len = si_buf[0] | (si_buf[1] << 8);
    }


    if (*ff_len > 0) {
        bmi160_hal_fifo_cor_ff_len(istate, ctx, ff_len);

        if (BMI160_FIFO_READ_CTX_TYPE_WMI == ctx->ctx_type) {
            *ff_len += BMI160_FF_FRAME_LEN_TS + 1;
        } else {
            *ff_len += BMI160_FF_FRAME_LEN_TS + 1;
            if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
                *ff_len += BMI160_FF_DATA_LEN_ACC;
            }

            if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
                *ff_len += BMI160_FF_DATA_LEN_GYR;
            }

            //TODOMAG
        }
    } else {
    }

    return SNS_RC_SUCCESS;
}

    static
void bmi160_hal_fifo_update_sync_info(
        bmi160_instance_state   *istate,
        const uint8_t           fh_param,
        bmi160_fifo_parse_ctx_t *ff_parse_ctx,
        bmi160_fifo_parse_out_desc_t    *ff_parse_out_desc)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    UNUSED_VAR(istate);
    UNUSED_VAR(fh_param);
    UNUSED_VAR(ff_parse_ctx);
    UNUSED_VAR(ff_parse_out_desc);


    fifo_info->ff_master_sensors_ready |= (fh_param & fifo_info->ff_master_sensors_curr);

    if (fifo_info->ff_master_sensors_ready) {
        ff_parse_out_desc->fc_masters_this_batch++;
        fifo_info->ff_tm_info.fc_accum_masters++;

        if (fh_param & BMI160_ACCEL) {
            ff_parse_out_desc->fc_masters_mark_acc = ff_parse_out_desc->fc_masters_this_batch;
        }

        if (fh_param & BMI160_GYRO) {
            ff_parse_out_desc->fc_masters_mark_gyr = ff_parse_out_desc->fc_masters_this_batch;
        }
        //TODOMAG

    } else {
        BMI160_INST_LOG(HIGH, istate->owner, "NOTICE bmi160_cp_ ff_master sync_excep");
    }

    if (fh_param == fifo_info->ff_sensors_en_curr) {
        fifo_info->ff_all_synced = 1;
    }
}

    static
void bmi160_hal_report_single_frame_acc(
        bmi160_instance_state   *istate,
        const uint8_t           *ff_buf,
        sns_time                ts,
        bmi160_fifo_sample_report_ctx_t *ff_sample_rpt_ctx)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    int16_t                     data_in[3];
    int32_t                     data_remapped_fxp[3];
    float                       data_remapped[3];
    bool                        data_remapped_needed = true;

    uint8_t                     i;
    triaxis_conversion          *axis_map;

    data_in[TRIAXIS_X] = ((int16_t)((ff_buf[1] << 8) | ff_buf[0]));
    data_in[TRIAXIS_Y] = ((int16_t)((ff_buf[3] << 8) | ff_buf[2]));
    data_in[TRIAXIS_Z] = ((int16_t)((ff_buf[5] << 8) | ff_buf[4]));

#if BMI160_CONFIG_ENABLE_SIMPLE_CAL
    data_remapped_needed = false;
#endif

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    data_remapped_needed = true;
#endif

#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    if (istate->fac_test_in_progress && BMI160_ACCEL == istate->fac_test_info.fac_test_sensor) {
        data_remapped_needed = true;
    }
#endif

    axis_map = istate->accel_info.sstate->common.axis_map;
    for (i = 0; i < TRIAXIS_NUM; i++) {
        data_remapped_fxp[axis_map[i].opaxis] = (axis_map[i].invert ? -1 : 1) *
            data_in[axis_map[i].ipaxis] * istate->accel_info.sstvt_curr;

        if (data_remapped_needed) {
            data_remapped[axis_map[i].opaxis] = data_remapped_fxp[axis_map[i].opaxis] / BMI160_SCALE_FACTOR_DATA_ACCEL;
        }
    }

#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    if (istate->fac_test_in_progress && BMI160_ACCEL == istate->fac_test_info.fac_test_sensor)
    {
        istate->fac_test_info.num_samples++;

        // Discard first three samples for the 104Hz ODR
        for (i = 0; i < TRIAXIS_NUM; i++)
        {
            if (i == (TRIAXIS_NUM - 1))
            {
                data_remapped[i] -= G;
            }
            istate->fac_test_info.sample_sum[i] += (data_remapped[i]);
            istate->fac_test_info.sample_square_sum[i] +=
                ((int64_t)(data_remapped[i]) * (int64_t)(data_remapped[i]));
        }
    }
#endif

    int32_t *fac_cal_bias;
    fac_cal_bias = istate->accel_info.sstate->fac_cal_bias;
#if BMI160_CONFIG_ENABLE_SIMPLE_CAL
    vector3 data_cal;

    data_cal.x = (data_remapped_fxp[0] - fac_cal_bias[0]) / BMI160_SCALE_FACTOR_DATA_ACCEL;
    data_cal.y = (data_remapped_fxp[1] - fac_cal_bias[1]) / BMI160_SCALE_FACTOR_DATA_ACCEL;
    data_cal.z = (data_remapped_fxp[2] - fac_cal_bias[2]) / BMI160_SCALE_FACTOR_DATA_ACCEL;
#else
    vector3 fac_cal_bias_vec;

    fac_cal_bias_vec.x = fac_cal_bias[0] / BMI160_SCALE_FACTOR_DATA_ACCEL;
    fac_cal_bias_vec.y = fac_cal_bias[1] / BMI160_SCALE_FACTOR_DATA_ACCEL;
    fac_cal_bias_vec.z = fac_cal_bias[2] / BMI160_SCALE_FACTOR_DATA_ACCEL;

    vector3 data_cal = sns_apply_calibration_correction_3(
            make_vector3_from_array(data_remapped),
            fac_cal_bias_vec,
            istate->accel_info.sstate->fac_cal_corr_mat);
#endif

    BMI160_INST_LOG(LOW, istate->owner, "acc bias %d %d %d",
            (int)(fac_cal_bias[0]),
            (int)(fac_cal_bias[1]),
            (int)(fac_cal_bias[2]));

    if (
            //(fifo_info->publish_sensors & BMI160_ACCEL //this is the logic of sample code
        (fifo_info->ff_sensors_en_curr & BMI160_ACCEL)
#if BMI160_CONFIG_ENABLE_DRI_MODE
            || (istate->int_en_flags_curr.bits.drdy.flag)
#endif
            || (istate->accel_info.gated_client_present && !istate->int_en_flags_curr.bits.md)
            ) {
            sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

            pb_send_sensor_stream_event(istate->owner,
                &istate->accel_info.sstate->my_suid,
                ts,
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                status,
                data_cal.data,
                ARR_SIZE(data_cal.data),
                istate->encoded_imu_event_len);

#if BMI160_CONFIG_ENABLE_DIAG_LOG
            // Log raw uncalibrated sensor data_remapped
            bmi160_log_sensor_state_raw_add(
                    ff_sample_rpt_ctx->log_state_info_acc,
                    data_remapped,
                    ts,
                    status);
#else
            UNUSED_VAR(ff_sample_rpt_ctx);
#endif
            }
}

    static
void bmi160_hal_fifo_handle_frame_acc(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_ctx_t *ff_parse_ctx,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc,
        uint32_t                fc,
        const uint8_t           *ff_buf)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bmi160_fifo_time_info_t     *ff_tm_info = &fifo_info->ff_tm_info;
    sns_time                    ts;
    bmi160_fifo_sample_report_ctx_t *ff_sample_rpt_ctx = (bmi160_fifo_sample_report_ctx_t *)ff_parse_ctx->priv_data;

    ff_tm_info->fc_accum_curr_acc++;

    if (ff_parse_out_desc->fc_this_batch_acc > 1) {
        ts = bmi160_hal_get_ts_forward(ff_tm_info->ts_1st_frame_this_batch_acc, BST_CEIL_P(ff_tm_info->itvl_this_batch_acc * (fc - 1)));
    } else {
        ts = ff_tm_info->ts_1st_frame_this_batch_acc;
    }

    bmi160_hal_report_single_frame_acc(istate, ff_buf, ts, ff_sample_rpt_ctx);

#if BMI160_CONFIG_ENABLE_LOG_TS_INDIVIDUAL_FRAME
    BMI160_INST_LOG(HIGH, istate->owner, "dl_meta_acc:<0x%x,0x%x,%u,%u>",
            (fifo_info->ff_flush_trigger << 27) | (fifo_info->ff_read_ctx_type << 24) | (g_bmi160_cnt_session_acc << 16) | (ff_parse_out_desc->fc_this_batch_acc << 8) | fc,
            BMI160_SYS_TIME_HH(ts), BMI160_SYS_TIME_LH(ts), ff_tm_info->fc_accum_curr_acc);
#endif
}


    static
void bmi160_hal_report_single_frame_gyr(
        bmi160_instance_state   *istate,
        const uint8_t           *ff_buf,
        sns_time                ts,
        bmi160_fifo_sample_report_ctx_t *ff_sample_rpt_ctx)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bmi160_gyro_info            *gyro_info = &istate->gyro_info;

    int16_t                     data_in[3];
    int32_t                     data_remapped_fxp[3];
    float                       data_remapped[3];
    bool                        data_remapped_needed = true;

    uint8_t                     i;
    triaxis_conversion          *axis_map;


    //OPTIM
    data_in[TRIAXIS_X] = ((int16_t)((ff_buf[1] << 8) | ff_buf[0]));
    data_in[TRIAXIS_Y] = ((int16_t)((ff_buf[3] << 8) | ff_buf[2]));
    data_in[TRIAXIS_Z] = ((int16_t)((ff_buf[5] << 8) | ff_buf[4]));

#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW
    bool                        sample_sel = true;

#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW_FILT
    if (gyro_info->downsample_sw_factor <= 1) {
    } else {
        if (0 != (gyro_info->downsample_sw_cnt % gyro_info->downsample_sw_factor)) {
            data_in[TRIAXIS_X] = (data_in[TRIAXIS_X] + gyro_info->downsample_sw_dcache[TRIAXIS_X]);
            data_in[TRIAXIS_Y] = (data_in[TRIAXIS_Y] + gyro_info->downsample_sw_dcache[TRIAXIS_Y]);
            data_in[TRIAXIS_Z] = (data_in[TRIAXIS_Z] + gyro_info->downsample_sw_dcache[TRIAXIS_Z]);

            data_in[TRIAXIS_X] = (data_in[TRIAXIS_X] + (data_in[TRIAXIS_X] > 0 ? 1 : -1)) >> 1;
            data_in[TRIAXIS_Y] = (data_in[TRIAXIS_Y] + (data_in[TRIAXIS_Y] > 0 ? 1 : -1)) >> 1;
            data_in[TRIAXIS_Z] = (data_in[TRIAXIS_Z] + (data_in[TRIAXIS_Z] > 0 ? 1 : -1)) >> 1;
        }

        gyro_info->downsample_sw_dcache[TRIAXIS_X] = data_in[TRIAXIS_X];
        gyro_info->downsample_sw_dcache[TRIAXIS_Y] = data_in[TRIAXIS_Y];
        gyro_info->downsample_sw_dcache[TRIAXIS_Z] = data_in[TRIAXIS_Z];
    }
#endif


    if (gyro_info->downsample_sw_factor <= 1) {
    } else {
        gyro_info->downsample_sw_cnt++;
        if (0 != (gyro_info->downsample_sw_cnt % gyro_info->downsample_sw_factor)) {
            sample_sel = false;
        } else {
            gyro_info->downsample_sw_cnt = 0;
        }
    }

    if (sample_sel) {
    } else {
        return;
    }
#endif


#if BMI160_CONFIG_ENABLE_SIMPLE_CAL
    data_remapped_needed = false;
#endif

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    data_remapped_needed = true;
#endif

#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    if (istate->fac_test_in_progress && BMI160_GYRO == istate->fac_test_info.fac_test_sensor) {
        data_remapped_needed = true;
    }
#endif

    axis_map = istate->gyro_info.sstate->common.axis_map;
    for (i = 0; i < TRIAXIS_NUM; i++) {
        data_remapped_fxp[axis_map[i].opaxis] = (axis_map[i].invert ? -1 : 1) *
            data_in[axis_map[i].ipaxis] * istate->gyro_info.sstvt_curr;

        if (data_remapped_needed) {
            data_remapped[axis_map[i].opaxis] = data_remapped_fxp[axis_map[i].opaxis] / BMI160_SCALE_FACTOR_DATA_GYRO;
        }
    }

#if BMI160_CONFIG_ENABLE_SELF_TEST_FAC
    if (istate->fac_test_in_progress && BMI160_GYRO == istate->fac_test_info.fac_test_sensor)
    {
        istate->fac_test_info.num_samples++;
        data_remapped_needed = 1;

        for (i = 0; i < TRIAXIS_NUM; i++)
        {
            istate->fac_test_info.sample_sum[i] += (data_remapped[i]);
            istate->fac_test_info.sample_square_sum[i] +=
                ((int64_t)(data_remapped[i]) * (int64_t)(data_remapped[i]));
        }
    }
#endif

    int32_t *fac_cal_bias;
    fac_cal_bias = istate->gyro_info.sstate->fac_cal_bias;
#if BMI160_CONFIG_ENABLE_SIMPLE_CAL
    vector3 data_cal;

    data_cal.x = (data_remapped_fxp[0] - fac_cal_bias[0]) / BMI160_SCALE_FACTOR_DATA_GYRO;
    data_cal.y = (data_remapped_fxp[1] - fac_cal_bias[1]) / BMI160_SCALE_FACTOR_DATA_GYRO;
    data_cal.z = (data_remapped_fxp[2] - fac_cal_bias[2]) / BMI160_SCALE_FACTOR_DATA_GYRO;
#else
    vector3 fac_cal_bias_vec;

    fac_cal_bias_vec.x = fac_cal_bias[0] / BMI160_SCALE_FACTOR_DATA_GYRO;
    fac_cal_bias_vec.y = fac_cal_bias[1] / BMI160_SCALE_FACTOR_DATA_GYRO;
    fac_cal_bias_vec.z = fac_cal_bias[2] / BMI160_SCALE_FACTOR_DATA_GYRO;

    vector3 data_cal = sns_apply_calibration_correction_3(
            make_vector3_from_array(data_remapped),
            fac_cal_bias_vec,
            istate->gyro_info.sstate->fac_cal_corr_mat);
#endif

    //if (fifo_info->publish_sensors & BMI160_GYRO)     //the logic from sample code
    if (
            (fifo_info->ff_sensors_en_curr & BMI160_GYRO)
#if BMI160_CONFIG_ENABLE_DRI_MODE
            || (istate->int_en_flags_curr.bits.drdy.flag)
#endif
       )
    {
        sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

        pb_send_sensor_stream_event(istate->owner,
                &istate->gyro_info.sstate->my_suid,
                ts,
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                status,
                data_cal.data,
                ARR_SIZE(data_cal.data),
                istate->encoded_imu_event_len);

#if BMI160_CONFIG_ENABLE_DIAG_LOG
        // Log raw uncalibrated sensor data_remapped
        bmi160_log_sensor_state_raw_add(
                ff_sample_rpt_ctx->log_state_info_gyr,
                data_remapped,
                ts,
                status);
#else
        UNUSED_VAR(ff_sample_rpt_ctx);
#endif
    }
}

    static
void bmi160_hal_fifo_handle_frame_gyr(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_ctx_t *ff_parse_ctx,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc,
        uint32_t                fc,
        const uint8_t           *ff_buf)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bmi160_fifo_time_info_t     *ff_tm_info = &fifo_info->ff_tm_info;
    sns_time                    ts;
    bmi160_fifo_sample_report_ctx_t *ff_sample_rpt_ctx = (bmi160_fifo_sample_report_ctx_t *)ff_parse_ctx->priv_data;

    ff_tm_info->fc_accum_curr_gyr++;

    if (ff_parse_out_desc->fc_this_batch_gyr > 1) {
        ts = bmi160_hal_get_ts_forward(ff_tm_info->ts_1st_frame_this_batch_gyr, BST_CEIL_P(ff_tm_info->itvl_this_batch_gyr * (fc - 1)));
    } else {
        ts = ff_tm_info->ts_1st_frame_this_batch_gyr;
    }

    bmi160_hal_report_single_frame_gyr(istate, ff_buf, ts, ff_sample_rpt_ctx);

#if BMI160_CONFIG_ENABLE_LOG_TS_INDIVIDUAL_FRAME
    BMI160_INST_LOG(HIGH, istate->owner, "dl_meta_gyr:<0x%x,0x%x,%u,%u>",
            (fifo_info->ff_flush_trigger << 27) | (fifo_info->ff_read_ctx_type << 24) | (g_bmi160_cnt_session_gyr << 16) | (ff_parse_out_desc->fc_this_batch_gyr << 8) | fc,
            BMI160_SYS_TIME_HH(ts), BMI160_SYS_TIME_LH(ts), ff_tm_info->fc_accum_curr_gyr);
#endif
}


// TODOMAG
    static
void bmi160_hal_fifo_handle_frame_mag(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_ctx_t *ff_parse_ctx,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc,
        uint32_t                fc,
        const uint8_t           *ff_buf)
{
    UNUSED_VAR(istate);
    UNUSED_VAR(ff_buf);
    UNUSED_VAR(ff_parse_ctx);
    UNUSED_VAR(ff_parse_out_desc);
    UNUSED_VAR(fc);
}


//uted
    static inline
uint32_t bmi160_dev_get_time_elapse_dev_lsb(
        uint32_t                before,
        uint32_t                after)
{
    if (after >= before) {
        return (after - before);
    } else {
        return (BMI160_SPEC_TS_LSB_OVERFLOW_VAL - before + after);
    }
}

    static inline
uint32_t bmi160_dev_get_ts_dev_age(
        uint32_t                ts_dev,
        uint8_t                 ts_dev_lsb_bits)
{

    uint32_t                    age;

    age = (ts_dev & ((1 << ts_dev_lsb_bits) - 1));

    return age;
}

uint32_t bmi160_dev_get_ts_dev_update_msb(
        uint32_t                start,
        uint32_t                end,
        uint8_t                 ts_dev_lsb_bits)
{
    uint32_t                    mask = ~((1 << ts_dev_lsb_bits) - 1);
    uint32_t                    ret;

    start &= mask;
    end &= mask;

    if (end >= start) {
        ret = (end - start) >> ts_dev_lsb_bits;
    } else {
        ret = (BMI160_SPEC_TS_LSB_OVERFLOW_VAL - start + end) >> ts_dev_lsb_bits;
    }

    return ret;
}

    static
void bmi160_hal_fifo_update_ts_hw_res(
        bmi160_instance_state   *istate,
        sns_time                *ts_curr)
{
    float                       ts_hw_res_ticks_per_bit;
    float                       devi;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
    uint8_t                     ts_dev_lsb_bits = BMI160_REGV_ODR_1600HZ - fifo_info->ff_master_odr_curr + 4;
#endif

    uint32_t                    itvl;
    sns_rc                      rc;

    bmi160_sys_dev_time_pair_t  *ts_pair_sys_dev_old = &istate->ts_pair_sys_dev_4_ts_res_est;

    itvl = bmi160_get_time_elapse_sys(istate->ts_pair_sys_dev.ts_sys, *ts_curr);


    if (itvl >= (30 * istate->ticks_in_1ms)) {
    } else {
#if BMI160_CONFIG_ENABLE_TS_IMPROVE
        if (istate->cnt_hw_res_est > 1)
#else
            if (istate->cnt_hw_res_est > 0)
#endif
            {
                BMI160_INST_LOG(LOW, istate->owner, "bmi160_cp_ short itvl cpl: %u %u",
                        BMI160_SYS_TIME_LH(istate->ts_pair_sys_dev.ts_sys),
                        BMI160_SYS_TIME_LH(*ts_curr));
                return;
            } else {
            }
    }

    rc = bmi160_hal_get_couple_ts_host_and_dev(istate);


    itvl = bmi160_get_time_elapse_sys(istate->ts_pair_sys_dev_4_ts_res_est.ts_sys, *ts_curr);
    if (itvl < (1000 * istate->ticks_in_1ms)) {
        if (istate->cnt_hw_res_est) {
            BMI160_INST_LOG(LOW, istate->owner, "bmi160_cp_ short itvl: %u %u",
                    BMI160_SYS_TIME_LH(istate->ts_pair_sys_dev.ts_sys),
                    BMI160_SYS_TIME_LH(*ts_curr));
            return;
        }
    }


    ts_hw_res_ticks_per_bit = bmi160_get_time_elapse_sys(ts_pair_sys_dev_old->ts_sys, istate->ts_pair_sys_dev.ts_sys) * 1.0f /
        bmi160_dev_get_time_elapse_dev_lsb(ts_pair_sys_dev_old->ts_dev, istate->ts_pair_sys_dev.ts_dev);

    devi = BST_ABS(ts_hw_res_ticks_per_bit - istate->ts_hw_res_ticks_per_bit_ideal);
    if (BST_ABS(devi) * 69 <= istate->ts_hw_res_ticks_per_bit_ideal) {  //75->69 when 12.5hz is used
        if (istate->cnt_hw_res_est > 16) {
            istate->ts_hw_res_ticks_per_bit = istate->ts_hw_res_ticks_per_bit * BMI160_CONFIG_TS_ITVL_EST_COEF
                + (1 - BMI160_CONFIG_TS_ITVL_EST_COEF) * ts_hw_res_ticks_per_bit;
        } else {
            //istate->cnt_hw_res_est = 1;
            if (0 == istate->cnt_hw_res_est) {
                istate->ts_hw_res_ticks_per_bit = ts_hw_res_ticks_per_bit;
            } else {
                istate->ts_hw_res_ticks_per_bit =
                    istate->ts_hw_res_ticks_per_bit * (1 - BMI160_CONFIG_TS_ITVL_EST_1ST_COEF) + (BMI160_CONFIG_TS_ITVL_EST_1ST_COEF) * ts_hw_res_ticks_per_bit;
            }
        }

        if (istate->cnt_hw_res_est < 100) {
            istate->cnt_hw_res_est++;
        }

#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
        fifo_info->ff_itvl_ticks_est_masters = istate->ts_hw_res_ticks_per_bit * (1 << ts_dev_lsb_bits);
#endif
        fifo_info->ff_itvl_ticks_est_acc = istate->ts_hw_res_ticks_per_bit * (1 << (BMI160_REGV_ODR_1600HZ - istate->accel_info.odr_curr + 4));
        if (istate->gyro_info.odr_curr == istate->accel_info.odr_curr) {
            fifo_info->ff_itvl_ticks_est_gyr = fifo_info->ff_itvl_ticks_est_acc;
        } else {
            fifo_info->ff_itvl_ticks_est_gyr = istate->ts_hw_res_ticks_per_bit * (1 << (BMI160_REGV_ODR_1600HZ - istate->gyro_info.odr_curr + 4));
        }

        BMI160_INST_LOG(MED, istate->owner, "ts_hw_res: %d %d %d",
                (int)((istate->ts_hw_res_ticks_per_bit * 1000)), (int)(ts_hw_res_ticks_per_bit * 1000), (int)BST_CEIL_P(fifo_info->ff_itvl_ticks_est_masters));

        istate->ts_pair_sys_dev_4_ts_res_est = istate->ts_pair_sys_dev;
    } else {
        BMI160_INST_LOG(HIGH, istate->owner, "ts_hw_res ignored: %d", (int)(ts_hw_res_ticks_per_bit * 1000));
    }
}

    static
void bmi160_hal_cor_ts_dev(
        bmi160_instance_state   *istate,
        uint32_t                *ts_dev)
{
    uint32_t                    ts_dev_cor = *ts_dev;
    uint8_t                     ts_dev_update_msb;
    uint8_t                     ts_dev_lsb_bits;

    bmi160_fifo_time_info_t *ff_tm_info = &istate->fifo_info.ff_tm_info;

    //ts_dev_cor -= BMI160_CONFIG_TS_DELAY_ADJ_I2C;
    ts_dev_cor = (ts_dev_cor - 1) & (BMI160_SPEC_TS_LSB_OVERFLOW_VAL - 1);

    ts_dev_lsb_bits = BMI160_REGV_ODR_1600HZ - istate->fifo_info.ff_master_odr_curr + 4;
    ts_dev_update_msb = bmi160_dev_get_ts_dev_update_msb(
            ts_dev_cor,  *ts_dev, ts_dev_lsb_bits);
    if (!ts_dev_update_msb) {
    } else {
        BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ ts_dev_update_msb: %d 0x%x", ts_dev_update_msb, *ts_dev);

        if (1 == ts_dev_update_msb) {
            uint8_t tmp;
            tmp = bmi160_dev_get_ts_dev_update_msb(
                    ff_tm_info->ts_dev_ff_last, ts_dev_cor, ts_dev_lsb_bits);
            //if (tmp == (ff_tm_info->fc_accum_masters - ff_tm_info->fc_masters_last_dev_ff))
            if ((tmp + 1) != (ff_tm_info->fc_accum_masters - ff_tm_info->fc_masters_last_dev_ff)) {
                *ts_dev = ts_dev_cor;
            } else {
                BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ ts_dev_cor rej: %d,%d",
                        ff_tm_info->fc_accum_masters, ff_tm_info->fc_masters_last_dev_ff);
            }
        }
    }

    ff_tm_info->ts_dev_ff_last = *ts_dev;
    ff_tm_info->fc_masters_last_dev_ff = ff_tm_info->fc_accum_masters;
}


    static
void bmi160_hal_fifo_est_ts_dev_ff(
        bmi160_instance_state           *istate,
        bmi160_fifo_parse_out_desc_t    *ff_parse_out_desc)
{
    sns_rc                              rc;
    uint8_t                             buf[3] = "";
    uint32_t                            ts_dev = 0;
    uint8_t                             ts_dev_lsb_bits;
    bmi160_fifo_info                    *fifo_info = &istate->fifo_info;


    bmi160_fifo_parse_result_t          *ff_parse_rslt = &ff_parse_out_desc->ff_parse_rslt;
    ts_dev_lsb_bits = BMI160_REGV_ODR_1600HZ - fifo_info->ff_master_odr_curr + 4;

    rc = bmi160_sbus_read_wrapper(istate,
            BMI160_REGA_USR_SENSORTIME_0,
            buf, 3);
    if (SNS_RC_SUCCESS == rc) {
    } else {
        return;
    }

    bmi160_dev_parse_data_ts(buf, &ts_dev);

    if (ff_parse_rslt->bits.ff_avail_ts_header) {
        uint16_t    ff_count = 0xffff;
        rc = bmi160_dev_get_reg_fifo_count(istate, &ff_count);
        if ((SNS_RC_SUCCESS == rc) && (0 == ff_count)) {
            ff_parse_out_desc->ts_dev_ff = ts_dev;
            ff_parse_rslt->bits.ff_avail_ts = true;
            BMI160_INST_LOG(HIGH, istate->owner, "!!! bmi160_cp_ good luck 1: ts_dev_ff est: 0x%x", ts_dev);
        }
    } else {
#if 0
        //this is not reliable
        if (BMI160_FIFO_READ_CTX_TYPE_WMI == fifo_info->ff_read_ctx_type) {
            uint32_t   ts_dev_update_msb;
            ts_dev_update_msb = bmi160_dev_get_ts_dev_update_msb(
                    istate->reg_int_ctx.ts_dev,
                    ts_dev,
                    ts_dev_lsb_bits);

            if (0 == ts_dev_update_msb) {
                ff_parse_out_desc->ts_dev_ff =  ts_dev;
                ff_parse_rslt->bits.ff_avail_ts = true;
                BMI160_INST_LOG(HIGH, istate->owner, "!!! bmi160_cp_ good luck 2 (na): ts_dev_ff est: 0x%x", ts_dev);
            }
        }
#endif
    }

    if (ff_parse_rslt->bits.ff_avail_ts) {
    } else {
        BMI160_INST_LOG(ERROR, istate->owner, "NOTICE ts_dev_est not blessed");
    }
}

    static
void bmi160_hal_fifo_calc_ts_start_end_this_batch(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bmi160_fifo_time_info_t     *ff_tm_info = &fifo_info->ff_tm_info;
    uint32_t                    itvl_acc;
    uint32_t                    itvl_gyr;
    //TODOMAG

    uint32_t                    fc_this_batch;
    int32_t                     delta_fc;
    sns_time                    ts_start;
    sns_time                    ts_end;

    sns_time                    ts_curr;


    ts_curr = bmi160_get_sys_tick();

    fc_this_batch = ff_parse_out_desc->fc_this_batch_acc;

    if (fc_this_batch) {
        itvl_acc = BST_CEIL_P(fifo_info->ff_itvl_ticks_est_acc);

        if (fc_this_batch > 1) {
            if (ff_tm_info->avail_ts_last_batch & BMI160_ACCEL) {
                ts_start = bmi160_hal_get_ts_forward(ff_tm_info->ts_end_frame_last_batch_acc, itvl_acc);
            } else {
                if (ff_tm_info->avail_ts_this_batch & BMI160_ACCEL) {
                    ts_start = bmi160_hal_get_ts_backward(ff_tm_info->ts_end_frame_this_batch_acc, fifo_info->ff_itvl_ticks_est_acc * (fc_this_batch - 1));
                } else {
                    ts_start = bmi160_hal_get_ts_backward(ts_curr, itvl_acc * (fc_this_batch - 1));
                    BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: acc ts_start no ref: %u", BMI160_SYS_TIME_LH(ts_curr));
                }
            }
        }

        if (ff_tm_info->avail_ts_this_batch & BMI160_ACCEL) {
            //end is already available
            ts_end = ff_tm_info->ts_end_frame_this_batch_acc;
        } else {
            if (ff_tm_info->avail_ts_last_batch & BMI160_ACCEL) {
                ts_end = bmi160_hal_get_ts_forward(ff_tm_info->ts_end_frame_last_batch_acc, fifo_info->ff_itvl_ticks_est_acc * (fc_this_batch));
                if (BMI160_TIME_LT(ts_end, ts_curr)) {
                    ts_end = bmi160_hal_get_ts_backward(ts_curr, 1);
                    BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: acc ts_end lt curr");
                }
            } else {
                ts_end = bmi160_hal_get_ts_backward(ts_curr, 1);
                BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: acc ts_end no ref: %u", BMI160_SYS_TIME_LH(ts_curr));
            }

            ff_tm_info->ts_end_frame_this_batch_acc = ts_end;
        }

        if (1 == fc_this_batch) {
            ts_start = ts_end;
        }

        ff_tm_info->ts_1st_frame_this_batch_acc = ts_start;

#if 0
        delta_fc = fc_this_batch - 1;
        if (delta_fc > 0) {
#if (BMI160_CONFIG_ENABLE_OPTIM_LOAD_FP && !BMI160_CONFIG_SEE_LITE_SLOW_CLOCK)
            ff_tm_info->itvl_this_batch_acc = ((bmi160_get_time_elapse_sys(ts_start, ts_end) << 1) + delta_fc) / (delta_fc << 1);
#else
            ff_tm_info->itvl_this_batch_acc = bmi160_get_time_elapse_sys(ts_start, ts_end) * 1.0f / (delta_fc);
#endif
        }
#endif

#if BMI160_CONFIG_ENABLE_OPTIM_LOAD_FP
        ff_tm_info->itvl_this_batch_acc = fifo_info->ff_itvl_ticks_est_acc;
        UNUSED_VAR(delta_fc);
#else
        delta_fc = fc_this_batch - 1;
        if (delta_fc > 0) {
            ff_tm_info->itvl_this_batch_acc = bmi160_get_time_elapse_sys(ts_start, ts_end) * 1.0f / (delta_fc);
        }
#endif


#if BMI160_CONFIG_ENABLE_LOG_TS_BATCH
        BMI160_INST_LOG(HIGH, istate->owner, "dl_meta<:0x%x,%d,%u,%u,%u:>",
                (BMI160_ACCEL << 24) | (ff_tm_info->avail_ts_last_batch << 20) | (fifo_info->ff_flush_trigger << 19) | (fifo_info->ff_read_ctx_type << 16) | (g_bmi160_cnt_session_acc << 8),
                //ff_tm_info->fc_accum_curr_acc,
                fc_this_batch * 3,
                BMI160_SYS_TIME_LH(ts_start),
                BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_acc),
                BMI160_SYS_TIME_LH(ts_curr));
#endif
    }


    fc_this_batch = ff_parse_out_desc->fc_this_batch_gyr;

    if (fc_this_batch) {
        itvl_gyr = BST_CEIL_P(fifo_info->ff_itvl_ticks_est_gyr);

        if (fc_this_batch > 1) {
            if (ff_tm_info->avail_ts_last_batch & BMI160_GYRO) {
                ts_start = bmi160_hal_get_ts_forward(ff_tm_info->ts_end_frame_last_batch_gyr, itvl_gyr);
            } else {
                if (ff_tm_info->avail_ts_this_batch & BMI160_GYRO) {
                    ts_start = bmi160_hal_get_ts_backward(ff_tm_info->ts_end_frame_this_batch_gyr, fifo_info->ff_itvl_ticks_est_gyr * (fc_this_batch - 1));
                } else {
                    ts_start = bmi160_hal_get_ts_backward(ts_curr, itvl_gyr * (fc_this_batch - 1));
                    BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: gyr ts_start no ref: %u", BMI160_SYS_TIME_LH(ts_curr));
                }
            }
        }

        if (ff_tm_info->avail_ts_this_batch & BMI160_GYRO) {
            //end is already available
            ts_end = ff_tm_info->ts_end_frame_this_batch_gyr;
        } else {
            if (ff_tm_info->avail_ts_last_batch & BMI160_GYRO) {
                ts_end = bmi160_hal_get_ts_forward(ff_tm_info->ts_end_frame_last_batch_gyr, fifo_info->ff_itvl_ticks_est_gyr * (fc_this_batch));
                if (BMI160_TIME_LT(ts_end, ts_curr)) {
                    ts_end = bmi160_hal_get_ts_backward(ts_curr, 1);
                    BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: gyr ts_end lt curr");
                }
            } else {
                ts_end = bmi160_hal_get_ts_backward(ts_curr, 1);
                BMI160_INST_LOG(ERROR, istate->owner, "bmi160_cp_ NOTICE: gyr ts_end no ref");
            }

            ff_tm_info->ts_end_frame_this_batch_gyr = ts_end;
        }

        if (1 == fc_this_batch) {
            ts_start = ts_end;
        }

        ff_tm_info->ts_1st_frame_this_batch_gyr = ts_start;

#if 0
        delta_fc = fc_this_batch - 1;
        if (delta_fc > 0) {
#if (BMI160_CONFIG_ENABLE_OPTIM_LOAD_FP && !BMI160_CONFIG_SEE_LITE_SLOW_CLOCK)
            ff_tm_info->itvl_this_batch_gyr = ((bmi160_get_time_elapse_sys(ts_start, ts_end) << 1) + delta_fc) / (delta_fc << 1);
#else
            ff_tm_info->itvl_this_batch_gyr = bmi160_get_time_elapse_sys(ts_start, ts_end) * 1.0f / (delta_fc);
#endif
        }
#endif

#if BMI160_CONFIG_ENABLE_OPTIM_LOAD_FP
        ff_tm_info->itvl_this_batch_gyr = fifo_info->ff_itvl_ticks_est_gyr;
#else
        delta_fc = fc_this_batch - 1;
        if (delta_fc > 0) {
            ff_tm_info->itvl_this_batch_gyr = bmi160_get_time_elapse_sys(ts_start, ts_end) * 1.0f / (delta_fc);
        }
#endif


#if BMI160_CONFIG_ENABLE_LOG_TS_BATCH
        BMI160_INST_LOG(HIGH, istate->owner, "dl_meta<:0x%x,%d,%u,%u,%u:>",
                (BMI160_GYRO << 24) | (ff_tm_info->avail_ts_last_batch << 20) | (fifo_info->ff_flush_trigger << 19) | (fifo_info->ff_read_ctx_type << 16) | (g_bmi160_cnt_session_gyr << 8),
                fc_this_batch * 3,
                BMI160_SYS_TIME_LH(ts_start),
                BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_gyr),
                BMI160_SYS_TIME_LH(ts_curr));
#endif
    }

}


#if BMI160_CONFIG_ENABLE_PROFILING_SYS_DELAY
    static
void bmi160_hal_fifo_get_est_ts_fifo_read_start_est(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc,
        sns_time                ts_ref_dev_ff,
        sns_time                *ts_est_fifo_read_start)
{
    *ts_est_fifo_read_start = bmi160_hal_get_ts_backward(ts_ref_dev_ff,
            (ff_parse_out_desc->ff_parse_rslt.bits.offset_ts_header * istate->xfer_time_per_byte_ticks));
}
#endif


//This is strongly HW dependentant
    static
bool bmi160_hal_dev_time_a_lt_b_short_itvl(
        int32_t                a,
        int32_t                b)
{
    bool                        ret;
    int32_t                     delta;

    delta = (int32_t)((a << 8) - (b << 8));

    ret = (delta >= 0);

    return ret;
}

    static
void bmi160_hal_fifo_resolve_ts_ref(
        bmi160_instance_state   *istate,
        bmi160_fifo_parse_out_desc_t *ff_parse_out_desc)
{
    sns_time                    ts_curr;
    sns_time                    ts_ref_dev_ff = 0;
    sns_time                    ts_age = 0;

    uint8_t                     avail_ts_ref_this_batch = 0;

    uint32_t                    ts_dev_update_lsb = 0;
    uint8_t                     ts_dev_lsb_bits;
    uint8_t                     ts_dev_lsb_bits_acc;
    uint8_t                     ts_dev_lsb_bits_gyr;

    uint32_t                    ts_dev_ff = 0;

    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    bmi160_fifo_time_info_t     *ff_tm_info = &fifo_info->ff_tm_info;

#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
    sns_time                    ts_irq_est = 0;
    int32_t                     devi = 0;
#endif

#if BMI160_CONFIG_ENABLE_PROFILING_SYS_DELAY
    sns_time                    ts_est_fifo_read_start = 0;
#endif

    UNUSED_VAR(avail_ts_ref_this_batch);


    ts_curr = bmi160_get_sys_tick();

    bmi160_hal_fifo_update_ts_hw_res(istate, &ts_curr);


    ts_dev_lsb_bits = BMI160_REGV_ODR_1600HZ - fifo_info->ff_master_odr_curr + 4;
    ts_dev_lsb_bits_acc = BMI160_REGV_ODR_1600HZ - istate->accel_info.odr_curr + 4;
    ts_dev_lsb_bits_gyr = BMI160_REGV_ODR_1600HZ - istate->gyro_info.odr_curr + 4;
    //TODOMAG


    if (ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_ts) {
    } else {
        bmi160_hal_fifo_est_ts_dev_ff(istate, ff_parse_out_desc);
    }

    ff_tm_info->avail_ts_this_batch = 0;
    if (ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_ts) {
        ts_dev_ff = ff_parse_out_desc->ts_dev_ff;
        bmi160_hal_cor_ts_dev(istate, &ts_dev_ff);

        if (bmi160_hal_dev_time_a_lt_b_short_itvl(ts_dev_ff, istate->ts_pair_sys_dev.ts_dev)) {
            ts_dev_update_lsb = bmi160_dev_get_time_elapse_dev_lsb(istate->ts_pair_sys_dev.ts_dev, ts_dev_ff);
            ts_ref_dev_ff = bmi160_hal_get_ts_forward(istate->ts_pair_sys_dev.ts_sys, ts_dev_update_lsb * istate->ts_hw_res_ticks_per_bit);
        } else {
            ts_dev_update_lsb = bmi160_dev_get_time_elapse_dev_lsb(ts_dev_ff, istate->ts_pair_sys_dev.ts_dev);
            ts_ref_dev_ff = bmi160_hal_get_ts_backward(istate->ts_pair_sys_dev.ts_sys, ts_dev_update_lsb * istate->ts_hw_res_ticks_per_bit);
        }

#if BMI160_CONFIG_ENABLE_PROFILING_SYS_DELAY
        bmi160_hal_fifo_get_est_ts_fifo_read_start_est(istate, ff_parse_out_desc, ts_ref_dev_ff, &ts_est_fifo_read_start);
#endif

        if (ff_parse_out_desc->fc_this_batch_acc) {
            ts_age = bmi160_dev_get_ts_dev_age(ts_dev_ff, ts_dev_lsb_bits_acc) * istate->ts_hw_res_ticks_per_bit;
            ff_tm_info->ts_end_frame_this_batch_acc = bmi160_hal_get_ts_backward(ts_ref_dev_ff, ts_age);
            ff_tm_info->avail_ts_this_batch |= BMI160_ACCEL;
            BMI160_INST_LOG(MED, istate->owner, "rslvtr a: %u", BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_acc));
        }

        if (ff_parse_out_desc->fc_this_batch_gyr) {
            ts_age = bmi160_dev_get_ts_dev_age(ts_dev_ff, ts_dev_lsb_bits_gyr) * istate->ts_hw_res_ticks_per_bit;
            ff_tm_info->ts_end_frame_this_batch_gyr = bmi160_hal_get_ts_backward(ts_ref_dev_ff, ts_age);
            ff_tm_info->avail_ts_this_batch |= BMI160_GYRO;
            BMI160_INST_LOG(MED, istate->owner, "rslvtr g: %u", BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_gyr));
        }
        //TODOMAG
    } else {
        ff_tm_info->boost_read_size = 1;

#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
        if (BMI160_FIFO_READ_CTX_TYPE_WMI == fifo_info->ff_read_ctx_type) {
            sns_time            itvl;
            if ((istate->reg_int_ctx.fifo_len == fifo_info->ff_wml_bytes_curr)
                    && (fifo_info->ff_master_odr_req <= BMI160_REGV_ODR_100HZ)
                    && (fifo_info->ff_wml_bytes_curr > 13))
            {
                BMI160_INST_LOG(HIGH, istate->owner, "good luck 3");

                if (ff_parse_out_desc->fc_this_batch_acc) {
                    itvl = fifo_info->ff_itvl_ticks_est_acc;
                    ff_tm_info->ts_end_frame_this_batch_acc = bmi160_hal_get_ts_forward(istate->ts_irq,
                            itvl * (ff_parse_out_desc->fc_this_batch_acc - ff_parse_out_desc->fc_at_irq_acc));

                    BST_ASSUME_TS_IS_64BIT
                        ff_tm_info->ts_end_frame_this_batch_acc = (int64_t)ff_tm_info->ts_end_frame_this_batch_acc + fifo_info->devi_est_irq;
                    ff_tm_info->avail_ts_this_batch |= BMI160_ACCEL;
                    BMI160_INST_LOG(MED, istate->owner, "rslvtr a (irq): %u %u %u",
                            BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_acc),
                            ff_parse_out_desc->fc_this_batch_acc,
                            ff_parse_out_desc->fc_at_irq_acc);
                }

                if (ff_parse_out_desc->fc_this_batch_gyr) {
                    //itvl = istate->ts_hw_res_ticks_per_bit * (1 << ts_dev_lsb_bits_gyr);
                    itvl = fifo_info->ff_itvl_ticks_est_gyr;
                    ff_tm_info->ts_end_frame_this_batch_gyr = bmi160_hal_get_ts_forward(istate->ts_irq,
                            itvl * (ff_parse_out_desc->fc_this_batch_gyr - ff_parse_out_desc->fc_at_irq_gyr));

                    BST_ASSUME_TS_IS_64BIT
                        ff_tm_info->ts_end_frame_this_batch_gyr = (int64_t)ff_tm_info->ts_end_frame_this_batch_gyr + fifo_info->devi_est_irq;
                    ff_tm_info->avail_ts_this_batch |= BMI160_GYRO;
                    BMI160_INST_LOG(MED, istate->owner, "rslvtr g (irq): %u %u %u",
                            BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_gyr),
                            ff_parse_out_desc->fc_this_batch_gyr,
                            ff_parse_out_desc->fc_at_irq_gyr);
                }
            }
        } else {
            //TODO2
            BMI160_INST_LOG(ERROR, istate->owner, "NOTICE ts_ref end not resolved");
            //would help if we know the time when fifo_read by async_com_port_data_stream was finished
            ts_dev_ff = (istate->ts_pair_sys_dev.ts_dev - 1) & (BMI160_SPEC_TS_LSB_OVERFLOW_VAL - 1);
        }
#else
        BMI160_INST_LOG(ERROR, istate->owner, "NOTICE no luck getting ts_ref due to delays in ascp_read");
#endif

    }

#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
    if (ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_ts) {
        if ((ff_parse_out_desc->fc_at_irq_masters > 0) && (BMI160_FIFO_READ_CTX_TYPE_WMI == fifo_info->ff_read_ctx_type)) {
            if (istate->reg_int_ctx.fifo_len == fifo_info->ff_wml_bytes_curr) {
                ts_age = bmi160_dev_get_ts_dev_age(ts_dev_ff, ts_dev_lsb_bits) * istate->ts_hw_res_ticks_per_bit;
                ts_irq_est = bmi160_hal_get_ts_backward(ts_ref_dev_ff, ts_age);

                ts_irq_est = bmi160_hal_get_ts_backward(ts_irq_est,
                        (ff_parse_out_desc->fc_masters_this_batch - ff_parse_out_desc->fc_at_irq_masters) *
                        fifo_info->ff_itvl_ticks_est_masters);


                devi = ts_irq_est - istate->ts_irq;

                if (fifo_info->devi_est_irq) {
                    fifo_info->devi_est_irq = ((fifo_info->devi_est_irq << 3) + (devi << 1)) * 0.1f;
                } else {
                    fifo_info->devi_est_irq = devi;
                }

                BMI160_INST_LOG(MED, istate->owner, "devi: %d %d",
                        devi, fifo_info->devi_est_irq); //noise p2p: +/-80us

#if BMI160_CONFIG_ENABLE_PROFILING_SYS_DELAY
                BMI160_INST_LOG(MED, istate->owner, "sys_delay: %d",
                        (int32_t)(ts_est_fifo_read_start - ts_irq_est));
#endif
            }
        }
    }
#endif

    BMI160_INST_LOG(HIGH, istate->owner, "resolve_ts_ref: <0x%x,%u,0x%x,0x%x,%u>",
            ((istate->reg_int_ctx.fifo_len == fifo_info->ff_wml_bytes_curr) << 26) | (fifo_info->ff_read_ctx_type << 25) | (ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_ts << 24) |
            (ff_tm_info->avail_ts_last_batch << 20) | (ff_tm_info->avail_ts_this_batch << 16),

            BMI160_SYS_TIME_LH(istate->ts_pair_sys_dev.ts_sys),
            istate->ts_pair_sys_dev.ts_dev,
            ts_dev_ff + BMI160_CONFIG_TS_DELAY_ADJ_I2C,
            BMI160_SYS_TIME_LH(ts_ref_dev_ff));

    //(int)istate->ts_hw_res_ticks_per_bit);

    bmi160_hal_fifo_calc_ts_start_end_this_batch(istate, ff_parse_out_desc);

    if (BMI160_TIME_ET(ff_tm_info->ts_end_frame_this_batch_acc, ff_tm_info->ts_end_frame_last_batch_acc)
            ||  BMI160_TIME_LT(ff_tm_info->ts_end_frame_this_batch_acc, ts_curr)
       ) {
        INSERT_TRACE_POINT3_T(bmi160, 'E', 'R', 'R');
        BMI160_INST_LOG(ERROR, istate->owner, "WARNING 0x%x %u %u %u %u",
                istate->ts_pair_sys_dev.ts_dev,
                ts_dev_update_lsb,
                BMI160_SYS_TIME_HH(ff_tm_info->ts_end_frame_this_batch_acc),
                BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_last_batch_acc),
                BMI160_SYS_TIME_LH(ff_tm_info->ts_end_frame_this_batch_acc));
    }

    ff_tm_info->ts_end_frame_last_batch_acc = ff_tm_info->ts_end_frame_this_batch_acc;
    ff_tm_info->ts_end_frame_last_batch_gyr = ff_tm_info->ts_end_frame_this_batch_gyr;

    ff_tm_info->avail_ts_last_batch |= ff_tm_info->avail_ts_this_batch;
    if (ff_tm_info->avail_ts_last_batch == fifo_info->ff_sensors_en_curr) {
        fifo_info->ff_tm_info.boost_read_size = 0;
    }
}

    static
sns_rc bmi160_hal_fifo_parse_n_proc_frames(
        bmi160_instance_state           *istate,
        bmi160_fifo_parse_ctx_t         *ff_parse_ctx,
        bmi160_fifo_parse_out_desc_t    *ff_parse_out_desc)
{
    uint16_t                err_code = 0;


    uint8_t                 frame_header;
    uint8_t                 fh_mode;
    uint8_t                 fh_param;

    uint8_t                 frames_skipped;
    uint8_t                 frame_length;

    uint32_t                ff_buf_idx = 0;
    //uint8_t                 ff_cache_full_agm = 0;
    const uint8_t           *ff_buf = ff_parse_ctx->ff_buf;
    uint32_t                ff_buf_len = ff_parse_ctx->ff_buf_len;
    uint32_t                *ff_proc_len_left = &ff_parse_ctx->ff_proc_len_left;


    uint32_t                ts_dev_ff;


    uint32_t                fc_acc = 0;
    uint32_t                fc_gyr = 0;
    uint32_t                fc_mag = 0;


    bool                    ff_parse_flag_dryrun = !!(ff_parse_ctx->ff_parse_flag & BMI160_FIFO_PARSE_FLAG_DRYRUN);
    bmi160_fifo_parse_result_t      *ff_parse_rslt = &ff_parse_out_desc->ff_parse_rslt;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;


#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
    uint16_t i;

    for (i = 0; i < ff_buf_len; i++) {
        BMI160_INST_LOG(LOW, istate->owner, "[ffpb] @%03d: 0x%02x",
                i, ff_buf[i]);

        if (0x80 == ff_buf[i]) {
            i++;
            BMI160_INST_LOG(LOW, istate->owner, "[ffpb] %d bytes omitted next byte: 0x%02x",
                    ff_buf_len - i, ff_buf[i]);
            break;
        }
    }
#endif

    err_code = 0;
    ff_buf_idx = 0;

    while ((ff_buf_idx < ff_buf_len) &&  (!err_code)) {
        frame_header = ff_buf[ff_buf_idx];
        ff_buf_idx += 1;
        if (BMI160_FF_FH_EMPTY == frame_header) {
            // fifo empty
            if (ff_parse_flag_dryrun) {
                BMI160_INST_LOG(LOW, istate->owner, "@%d f_end", ff_buf_idx - 1);
            }

            if (0 == (ff_buf_idx - 1)) {
                ff_parse_rslt->bits.ff_batch_empty = 1;
            }

            ff_parse_rslt->bits.ff_avail_end_frame = 1;
            break;
        }

        fh_mode         = BMI160_FF_FH_MODE(frame_header);
        fh_param        = BMI160_FF_FH_PARAM(frame_header);

#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
        BMI160_INST_LOG(MED, istate->owner, "[ffp]frame_header = 0x%02x fh_mode: 0x%02x fh_param: 0x%02x",
                frame_header, fh_mode, fh_param);
#endif

        if (BMI160_FF_FH_MODE_REGULAR == fh_mode) {
            frame_length = 0;
            frame_length += ((fh_param & BMI160_MAG) ? BMI160_FF_DATA_LEN_MAG : 0);
            frame_length += ((fh_param & BMI160_GYRO) ? BMI160_FF_DATA_LEN_GYR : 0);
            frame_length += ((fh_param & BMI160_ACCEL) ? BMI160_FF_DATA_LEN_ACC : 0);

            if (ff_buf_idx + frame_length > ff_buf_len) {
                // buf is over
                BMI160_INST_LOG(MED, istate->owner,
                        "[ffp]INFO!!! partial read of frame, %d bytes yet to be read, total: %d fh: 0x%x",
                        ff_buf_idx + frame_length - ff_buf_len,
                        ff_buf_len,
                        frame_header);

                err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8) | frame_header;
                break;
            }
#if 0
            ff_cache_full_agm = 0;

            if (!ff_parse_flag_dryrun) {
                //bmi160_hal_fifo_check_buf_cap(istate, fh_param, &ff_cache_full_agm);
                if (!ff_cache_full_agm) {
                } else {
                    BMI160_INST_LOG(HIGH, istate->owner, "ff_cache_full: %d", ff_cache_full_agm);
                }
            }

            ff_parse_rslt->bits.ff_cache_full_agm = ff_cache_full_agm;
            if (!ff_cache_full_agm) { }
#endif
            {
                if (fh_param & BMI160_MAG) {
                    // mag data
                    if (!ff_parse_flag_dryrun) {
                        fc_mag++;
                        bmi160_hal_fifo_handle_frame_mag(istate, ff_parse_ctx, ff_parse_out_desc, fc_mag, ff_buf + ff_buf_idx);
                    } else {
                        ff_parse_out_desc->fc_this_batch_mag++;
                    }

                    ff_buf_idx += BMI160_FF_DATA_LEN_MAG;
                }

                if (fh_param & BMI160_GYRO) {
                    // gyr data
                    if (!ff_parse_flag_dryrun) {
                        fc_gyr++;
                        bmi160_hal_fifo_handle_frame_gyr(istate, ff_parse_ctx, ff_parse_out_desc, fc_gyr, ff_buf + ff_buf_idx);
                    } else {
                        ff_parse_out_desc->fc_this_batch_gyr++;
                    }

                    ff_buf_idx += BMI160_FF_DATA_LEN_GYR;
                }

                if (fh_param & BMI160_ACCEL) {
                    // acc data
                    if (!ff_parse_flag_dryrun) {
                        fc_acc++;
                        bmi160_hal_fifo_handle_frame_acc(istate, ff_parse_ctx, ff_parse_out_desc, fc_acc, ff_buf + ff_buf_idx);
                    } else {
                        ff_parse_out_desc->fc_this_batch_acc++;
                    }

                    ff_buf_idx += BMI160_FF_DATA_LEN_ACC;
                }

                if (ff_parse_flag_dryrun) {
                    bmi160_hal_fifo_update_sync_info(istate, fh_param, ff_parse_ctx, ff_parse_out_desc);

                    if (ff_buf_idx >= fifo_info->ff_wml_bytes_curr) {
                        if (0 == ff_parse_out_desc->ff_frm_header_at_irq) {
                            ff_parse_out_desc->ff_frm_header_at_irq = fh_param;

                            ff_parse_out_desc->fc_at_irq_acc = ff_parse_out_desc->fc_this_batch_acc;
                            ff_parse_out_desc->fc_at_irq_gyr = ff_parse_out_desc->fc_this_batch_gyr;
                            ff_parse_out_desc->fc_at_irq_mag = ff_parse_out_desc->fc_this_batch_mag;
                            ff_parse_out_desc->fc_at_irq_masters = ff_parse_out_desc->fc_masters_this_batch;

                            BMI160_INST_LOG(MED, istate->owner, "fc_at_irq: 0x%x@%d %d %d",
                                    fh_param, ff_buf_idx,
                                    ff_parse_out_desc->fc_this_batch_acc, ff_parse_out_desc->fc_this_batch_gyr);
                        }
                    }
                }
            }
        } else if (BMI160_FF_FH_MODE_CTRL == fh_mode) {
            switch (fh_param) {
                case 0x00:
                    if (ff_buf_idx + 1 > ff_buf_len) {
                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }
                    //fifo overflowed
                    ff_parse_rslt->bits.ff_overflow = true;

                    frames_skipped = ff_buf[ff_buf_idx];
                    ff_buf_idx += 1;
                    BMI160_INST_LOG(ERROR, istate->owner, "[ffp]@%d frame_ctrl WARNING!!! frames_skipped = 0x%02x some samples are missing due to delay of FIFO read",
                            ff_buf_idx - 2,
                            frames_skipped);

                    BST_IGNORE_WARNING_UNUSED_VAR(frames_skipped);

                    *ff_proc_len_left = 0;
                    goto exit_parse_ff_frames;
                    break;
                case 0x01:
                    ff_parse_rslt->bits.ff_avail_ts_header = 1;

                    if (ff_buf_idx + 3 > ff_buf_len) {
                        BMI160_INST_LOG(HIGH, istate->owner, "[ffp]@%d INFO!!! frame_ctrl sensortime partial read, %d more bytes needed",
                                ff_buf_idx - 1, ff_buf_len + 3 - ff_buf_idx);

                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }

                    if (ff_parse_flag_dryrun) {
                        bmi160_dev_parse_data_ts(ff_buf + ff_buf_idx, &ts_dev_ff);
                        ff_parse_out_desc->ts_dev_ff = ts_dev_ff;

                        ff_parse_rslt->bits.offset_ts_header = ff_buf_idx;
                        BMI160_INST_LOG(HIGH, istate->owner,
                                "@%d f_ctrl st:0x%x",
                                ff_buf_idx - 1,
                                ts_dev_ff);
                    }


                    ff_buf_idx += 3;
                    ff_parse_rslt->bits.ff_avail_ts = 1;
                    break;
                case 0x02:
                    // Fifo_Input_Config Frame
                    if (ff_buf_idx + 1 > ff_buf_len) {
                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }

                    uint8_t                 frame_input_cfg;
                    frame_input_cfg = ff_buf[ff_buf_idx];
                    ff_buf_idx += 1;
                    BMI160_INST_LOG(ERROR, istate->owner, "[ffp]@%d f_ctrl Fifo_Input_Config=0x%02x",
                            ff_buf_idx - 2,
                            frame_input_cfg);
                    BST_IGNORE_WARNING_UNUSED_VAR(frame_input_cfg);
                    break;
                default:
                    BMI160_INST_LOG(ERROR, istate->owner,
                            "[ffp]@%d frame_unknown WARNING!!! frame format unknown frame_header: 0x%02x",
                            ff_buf_idx,
                            frame_header);
                    err_code = (BMI160_FF_FRAME_ERR_UNKNOWN << 8)
                        | frame_header;


                    *ff_proc_len_left = 0;
                    goto exit_parse_ff_frames;
                    break;
            }
        } else {
            BMI160_INST_LOG(ERROR, istate->owner, "[ffp]@%d frame_unknown WARNING!!! unknown fifo frame_header: 0x%02x",
                    ff_buf_idx - 1,
                    frame_header);
            err_code = (BMI160_FF_FRAME_ERR_UNKNOWN << 8)
                | frame_header;
            break;
        }
    }
    //end of while

    *ff_proc_len_left = 0;
exit_parse_ff_frames:
    if (err_code &&
            (((err_code >> 8) & 0xff) != BMI160_FF_FRAME_ERR_PARTIAL_READ)) {
        BMI160_INST_LOG(ERROR, istate->owner, "[ffp] WARNING!!! error seen during parsing: <err: 0x%04x %d %d>",
                err_code, ff_buf_idx, ff_buf_len);
    }

    return SNS_RC_SUCCESS;
}

    static
void bmi160_hal_fifo_read_out(
        bmi160_instance_state   *istate,
        bmi160_fifo_read_ctx_t  *ctx)
{
    uint8_t                     buffer[100];
    uint32_t                    enc_len;
    sns_port_vector             async_read_msg;

    uint16_t                    ff_len = 0;

    UNUSED_VAR(ctx);

    bmi160_hal_fifo_determine_ff_len(istate, ctx, &ff_len);

    if (ff_len > 0)
    {
        if (!ctx->sync_read) {
            // Compose the async com port message
            async_read_msg.bytes = ff_len;
            async_read_msg.reg_addr = BMI160_REGA_USR_FIFO_DATA;
            async_read_msg.is_write = false;
            async_read_msg.buffer = NULL;

            sns_ascp_create_encoded_vectors_buffer(&async_read_msg, 1, true, buffer, sizeof(buffer), &enc_len);

            // Send message to Async Com Port
            sns_request async_com_port_request =
                (sns_request)
                {
                    .message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW,
                    .request_len = enc_len,
                    .request = buffer
                };

            sns_rc              rc = SNS_RC_POLICY;
            bool                com_port_read_granted;
            com_port_read_granted = (istate->async_com_read_requqest - istate->async_com_read_response) < BMI160_CONFIG_COM_FLOW_CTRL_THRESH;

            if (com_port_read_granted) {
                rc = istate->async_com_port_data_stream->api->send_request(
                        istate->async_com_port_data_stream, &async_com_port_request);
            } else {
                BMI160_INST_LOG(MED, istate->owner, "dropped async_com_req: %d",
                        (istate->async_com_read_requqest - istate->async_com_read_response));
            }

            if (SNS_RC_SUCCESS == rc) {
                istate->async_com_read_requqest++;
            } else {
                if (com_port_read_granted) {
                    BMI160_INST_LOG(ERROR, istate->owner, "ERROR ascp_req: %d", rc);
                }
            }

        } else {
            //TODO3: synchronous fifo read
            //not supported because dynamic memory allocation is not allowed
        }
    }
    else
    {
        bmi160_fifo_info            *fifo_info = &istate->fifo_info;
        // reset FIFO istate if there are no samples to flush
        if (fifo_info->ff_flush_in_proc)
        {
            if ((istate->async_com_read_requqest - istate->async_com_read_response) <= 0) {
                fifo_info->ff_flush_in_proc = false;

                if (istate->ff_flush_client_req) {
                    istate->ff_flush_client_req = 0;
                }

                sns_time ts_curr = bmi160_get_sys_tick();
                BMI160_INST_LOG(MED, istate->owner, "fifo_flush_done event: %d@%u", fifo_info->ff_sensors_en_curr, BMI160_SYS_TIME_LH(ts_curr));

                if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
                    bmi160_send_fifo_flush_done(istate->owner, &istate->accel_info.sstate->my_suid, ts_curr, 0);
                }

                if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
                    bmi160_send_fifo_flush_done(istate->owner, &istate->gyro_info.sstate->my_suid, ts_curr, 0);
                }
                //TODOMAG
            }
        }
    }
}

void bmi160_hal_fifo_drain(
        bmi160_instance_state   *istate,
        bool                    sync_read,
        bmi160_fifo_flush_trigger_t trigger)
{
    bmi160_fifo_read_ctx_t      ctx;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    ctx.ctx_type = BMI160_FIFO_READ_CTX_TYPE_FLUSH;
    ctx.sync_read = sync_read;

    fifo_info->ff_read_ctx_type = BMI160_FIFO_READ_CTX_TYPE_FLUSH;

    if (!sync_read) {
        fifo_info->ff_flush_in_proc = true;
    } else {
        fifo_info->ff_flush_in_proc = false;
    }
    fifo_info->ff_flush_trigger = trigger;

    BMI160_INST_LOG(HIGH, istate->owner, "fifo_flush trigger: %d %d", trigger, sync_read);

    bmi160_hal_fifo_read_out(istate, &ctx);
}


void bmi160_hal_fifo_update_master_odr_req(bmi160_instance_state *istate)
{
    uint16_t    odr_ff_master = BMI160_REGV_ODR_OFF;
    bmi160_fifo_info                *fifo_info = &istate->fifo_info;


    if (fifo_info->publish_sensors & BMI160_ACCEL) {
        if (istate->accel_info.ff_wml_req) {
            odr_ff_master = SNS_MAX(odr_ff_master, istate->accel_info.odr_req);
        }
    }

    if (fifo_info->publish_sensors & BMI160_GYRO) {
        if (istate->gyro_info.ff_wml_req) {
            odr_ff_master = SNS_MAX(odr_ff_master, istate->gyro_info.odr_req);
        }
    }
    //TODOMAG

    fifo_info->ff_master_odr_req = (bmi160_regv_odr_t) odr_ff_master;
}


    static
void bmi160_hal_fifo_invalidate_master_info(
        bmi160_instance_state *istate)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    fifo_info->ff_tm_info.fc_masters_last_dev_ff = 0;
    fifo_info->ff_tm_info.fc_accum_masters = 0;
}

void bmi160_hal_fifo_update_curr_masters(bmi160_instance_state *istate)
{
    uint8_t                     ff_masters = 0;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    if (fifo_info->publish_sensors & BMI160_ACCEL) {
        if (istate->accel_info.ff_wml_req) {
            if (istate->accel_info.odr_curr == fifo_info->ff_master_odr_req) {
                ff_masters |= BMI160_ACCEL;
            }
        }
    }

    if (fifo_info->publish_sensors & BMI160_GYRO) {
        if (istate->gyro_info.ff_wml_req) {
            if (istate->gyro_info.odr_curr == fifo_info->ff_master_odr_req) {
                ff_masters |= BMI160_GYRO;
            }
        }
    }
    //TODOMAG

#if BMI160_CONFIG_ENABLE_DRI_MODE
    if (istate->int_en_flags_req.bits.drdy.flag) {
        ff_masters = 0;
    }
#endif

    BMI160_INST_LOG(MED, istate->owner, "bmi160_cp_ fifo_update_master: <0x%x 0x%x %d %d>",
            (int)ff_masters, (int)fifo_info->ff_master_sensors_curr,
            (int)fifo_info->ff_master_odr_curr, (int)fifo_info->ff_master_odr_req);

    if (!(ff_masters & fifo_info->ff_master_sensors_curr) ||
            (fifo_info->ff_master_odr_curr != fifo_info->ff_master_odr_req)) {
        bmi160_hal_fifo_invalidate_master_info(istate);
    }

    //update
    fifo_info->ff_master_sensors_curr = ff_masters;
    fifo_info->ff_master_odr_curr = fifo_info->ff_master_odr_req;

    fifo_info->ff_master_sensors_ready = 0;
    fifo_info->ff_all_synced = 0;
}


void bmi160_hal_fifo_validate_n_adj_wml_req(
        bmi160_instance_state   *istate,
        uint32_t                *ff_wml_req_acc,
        uint32_t                *ff_wml_req_gyr,
        uint32_t                *ff_wml_req_mag)
{
    uint32_t                    mcd;
    uint32_t                    wml_adj;
    uint32_t                    wml_adj_needed= 0;
    uint32_t                    ff_max_len = 0;

    UNUSED_VAR(istate);

    mcd = bmi160_util_get_com_div(*ff_wml_req_acc, *ff_wml_req_gyr);
    mcd = bmi160_util_get_com_div(mcd, *ff_wml_req_mag);

    wml_adj_needed = ((*ff_wml_req_acc) >= BMI160_FF_MAX_FRAMES_IMU);
    wml_adj_needed |= (((*ff_wml_req_gyr) >= BMI160_FF_MAX_FRAMES_IMU) << 1);
    //TODOMAG

    wml_adj = mcd;
    while (true) {
        ff_max_len = (*ff_wml_req_acc) ? ((BMI160_FF_DATA_LEN_ACC + 1) * wml_adj) : 0;
        ff_max_len += (*ff_wml_req_gyr) ? ((BMI160_FF_DATA_LEN_GYR + 1) * wml_adj) : 0;
        //ff_max_len += *ff_wml_req_mag ? ((BMI160_FF_DATA_LEN_MAG + 1) * wml_adj) : 0;   //TODOMAG

        if ((ff_max_len < (BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM)) && (wml_adj < BMI160_FF_MAX_FRAMES_IMU)) {
            break;
        } else {
            wml_adj = bmi160_util_get_max_div(mcd, BST_MIN((wml_adj - 1), BMI160_FF_MAX_FRAMES_IMU));

            wml_adj_needed |= (ff_max_len >= (BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM)) ? (1 << 8) : 0;
        }
    }


    BMI160_INST_LOG(MED, istate->owner, "wml_adj <%d %d %d>",
            *ff_wml_req_acc, *ff_wml_req_gyr, *ff_wml_req_mag);

    if (wml_adj_needed) {
        BMI160_INST_LOG(MED, istate->owner, "wml_adj_needed <0x%x %d>",
                wml_adj_needed, wml_adj);

        *ff_wml_req_acc = wml_adj;
        *ff_wml_req_gyr  = wml_adj;

        istate->accel_info.ff_wml_req = wml_adj;
        istate->gyro_info.ff_wml_req = wml_adj;
#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
        *ff_wml_req_mag  = wml_adj;
        istate->mag_info.ff_wml_req = wml_adj;
#endif
    }
}

void bmi160_hal_fifo_calc_wml_req_ldt(bmi160_instance_state *istate)
{
    uint16_t                    odr_acc = 0;
    uint16_t                    odr_gyr = 0;
    uint16_t                    odr_mag = 0;
    uint16_t                    odr_fifo_master  = 0;

    uint32_t                    fc_when_time_expires_acc = 0;
    uint32_t                    fc_when_time_expires_gyr = 0;
    uint32_t                    fc_when_time_expires_mag = 0;

    uint32_t                    fc_wml_master = 0;
    uint32_t                    fc_wml_acc_max = 0;
    uint32_t                    fc_wml_gyr_max = 0;
    uint32_t                    fc_wml_mag_max = 0;

    uint32_t                    ff_wml_req_acc = 0;
    uint32_t                    ff_wml_req_gyr = 0;
    uint32_t                    ff_wml_req_mag = 0;

    uint32_t                    bytes_when_time_expires_max = 0;
    uint32_t                    bytes_when_wml_fires = 0;

    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    ff_wml_req_acc = istate->accel_info.ff_wml_req;
    ff_wml_req_gyr = istate->gyro_info.ff_wml_req;

    bmi160_hal_fifo_validate_n_adj_wml_req(istate, &ff_wml_req_acc, &ff_wml_req_gyr, &ff_wml_req_mag);

    odr_acc = istate->accel_info.odr_req;
    odr_gyr = istate->gyro_info.odr_req;
    //TODOMAG
    odr_fifo_master = fifo_info->ff_master_odr_req;

    if (odr_acc > 0) {
        fc_when_time_expires_acc = (1 << (odr_fifo_master - odr_acc)) * ff_wml_req_acc;
    }

    if (odr_gyr > 0) {
        fc_when_time_expires_gyr = (1 << (odr_fifo_master - odr_gyr)) * ff_wml_req_gyr;
    }

    if (odr_mag > 0) {
        //TODOMAG
    }

    fc_wml_master = bmi160_util_get_com_div(fc_when_time_expires_acc, fc_when_time_expires_gyr);
    fc_wml_master = bmi160_util_get_com_div(fc_wml_master, fc_when_time_expires_mag);

    if (odr_acc > 0) {
        fc_wml_acc_max = (odr_acc == odr_fifo_master) ? fc_wml_master : 0;
        fc_wml_acc_max = fc_wml_acc_max ? fc_wml_acc_max : 1;
    }

    if (odr_gyr > 0) {
        fc_wml_gyr_max = (odr_gyr == odr_fifo_master) ? fc_wml_master : 0;
        fc_wml_gyr_max = fc_wml_gyr_max ? fc_wml_gyr_max : 1;
    }

    if (odr_mag > 0) {
        fc_wml_mag_max = (odr_mag == odr_fifo_master) ? fc_wml_master : 0;
        fc_wml_mag_max = fc_wml_mag_max ? fc_wml_mag_max : 1;
    }

    bytes_when_time_expires_max =
        BMI160_FF_DATA_LEN_ACC * fc_wml_acc_max +
        BMI160_FF_DATA_LEN_GYR * fc_wml_gyr_max +
        BMI160_FF_DATA_LEN_MAG * fc_wml_mag_max +
        + fc_wml_master;

    bytes_when_wml_fires =
        BMI160_FF_DATA_LEN_ACC * (fc_wml_master >> (odr_fifo_master - odr_acc)) +
        BMI160_FF_DATA_LEN_GYR * (fc_wml_master >> (odr_fifo_master - odr_gyr)) +
        BMI160_FF_DATA_LEN_MAG * (fc_wml_master >> (odr_fifo_master - odr_mag)) +
        + fc_wml_master;



    if ((1000000 * fc_wml_master / BMI160_REGV_ODR_MAP[odr_fifo_master].odr) <= BMI160_CONFIG_FIFO_WMI_MISSING_ITVL_US_THRES) {
        fifo_info->ff_wmi_missing_possible = 1;
        BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ ff_wmi_missing_possible");
    } else {
        fifo_info->ff_wmi_missing_possible = 0;
    }


    //OUTPUT
    BMI160_INST_LOG(HIGH, istate->owner, "odr <%d %d %d %d>",
            odr_acc, odr_gyr, odr_mag, odr_fifo_master);

    BMI160_INST_LOG(HIGH, istate->owner, "fc_when_time_expires<%d %d %d> fc_wml_master: %d",
            fc_when_time_expires_acc, fc_when_time_expires_gyr, fc_when_time_expires_mag, fc_wml_master);

    fifo_info->ff_suggested_bytes2read = bytes_when_time_expires_max;

    BMI160_INST_LOG(HIGH, istate->owner, "[calc_wml_ldt] ff_wml_bytes: %d ff_suggested_bytes2read: %d",
            bytes_when_wml_fires, fifo_info->ff_suggested_bytes2read);

    if (bytes_when_wml_fires > (BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM)) {
        bytes_when_wml_fires = BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM;
        fifo_info->ff_suggested_bytes2read = bytes_when_wml_fires;
    }

    fifo_info->ff_wml_bytes_req = bytes_when_wml_fires;

}



//TODO: check more
void bmi160_hal_fifo_invalidate_sensors(
        bmi160_instance_state   *istate,
        uint32_t                sensors_to_invalidate)
{
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;
    uint32_t                    sensors_to_invalidate_all;


    //sensors_to_invalidate_all = sensors_to_invalidate | (fifo_info->ff_sensors_en_curr ^ fifo_info->ff_sensors_en_req);
    sensors_to_invalidate_all = sensors_to_invalidate;


    if (sensors_to_invalidate_all) {
        BMI160_INST_LOG(MED, istate->owner, "bmi160_cp_ sensors_to_invalidate: %x %x %x %x",
                (int)sensors_to_invalidate, (int)fifo_info->ff_sensors_en_curr, (int)fifo_info->ff_sensors_en_req, (int)sensors_to_invalidate_all);
    }

    fifo_info->ff_tm_info.avail_ts_last_batch &= ~(sensors_to_invalidate_all);

    if (sensors_to_invalidate_all & BMI160_ACCEL) {
        fifo_info->ff_tm_info.fc_accum_curr_acc = 0;

    }


    if (sensors_to_invalidate_all & BMI160_GYRO) {
        fifo_info->ff_tm_info.fc_accum_curr_gyr = 0;
    }

    //TODOMAG

}


uint8_t bmi160_hal_fifo_get_sensors_to_invalidate(bmi160_instance_state   *istate)
{
    uint8_t                     ff_sensors_to_invalidate = 0;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;


    //if range of existing sensors in FIFO changed
    if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
        if (istate->accel_info.range_curr != istate->accel_info.range_req) {
            ff_sensors_to_invalidate |= BMI160_ACCEL;
        }

        if (istate->accel_info.odr_curr != istate->accel_info.odr_req) {
            ff_sensors_to_invalidate |= BMI160_ACCEL;
        }

        if (!(fifo_info->ff_sensors_en_req & BMI160_ACCEL)) {
            ff_sensors_to_invalidate |= BMI160_ACCEL;
        }

    }

    if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
        if (istate->gyro_info.range_curr != istate->gyro_info.range_req) {
            ff_sensors_to_invalidate |= BMI160_GYRO;
        }


        if (istate->gyro_info.odr_curr != istate->gyro_info.odr_req) {
            ff_sensors_to_invalidate |= BMI160_GYRO;
        }


        if (!(fifo_info->ff_sensors_en_req & BMI160_GYRO)) {
            ff_sensors_to_invalidate |= BMI160_GYRO;
        }
    }



    //TODOMAG

    return ff_sensors_to_invalidate;
}


//TODO: check, it seems that framework takes care of flush? SNS_STD_MSGID_SNS_STD_FLUSH_REQ
//OPTIM
    static
sns_rc bmi160_hal_fifo_prepare_4_cos(sns_sensor_instance *this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    sns_rc                      rc;

    uint8_t                     ff_sensors_to_invalidate = 0;
    uint8_t                     ff_sensors_en_curr_dur_transit = 0;

    bool                        ff_flush_needed = false;
    uint8_t                     regv;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;


    ff_sensors_to_invalidate = bmi160_hal_fifo_get_sensors_to_invalidate(istate);

    //if (ff_sensors_to_invalidate)
    if (ff_sensors_to_invalidate & (~fifo_info->ff_sensors_flush_done_before_invalid)) {
        ff_flush_needed = true;

        rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_FIFO_CONFIG_1, &regv, 1);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        ff_sensors_en_curr_dur_transit = BST_GET_VAL_BITBLOCK(regv, 5, 7);
        if (ff_sensors_to_invalidate & BMI160_ACCEL) {
            ff_sensors_en_curr_dur_transit = BST_CLR_VAL_BIT(ff_sensors_en_curr_dur_transit, 1);
        }

        if (ff_sensors_to_invalidate & BMI160_GYRO) {
            ff_sensors_en_curr_dur_transit = BST_CLR_VAL_BIT(ff_sensors_en_curr_dur_transit, 2);
        }

        if (ff_sensors_to_invalidate & BMI160_MAG) {
            ff_sensors_en_curr_dur_transit = BST_CLR_VAL_BIT(ff_sensors_en_curr_dur_transit, 0);
        }

        if (BST_GET_VAL_BITBLOCK(regv, 5, 7) != ff_sensors_en_curr_dur_transit) {
            regv = BST_SET_VAL_BITBLOCK(regv, 5, 7, ff_sensors_en_curr_dur_transit);
            rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_FIFO_CONFIG_1, &regv, 1);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

            BMI160_INST_LOG(HIGH, istate->owner, "sensors disabled during transition: %d", ff_sensors_to_invalidate);
        }
    } else {
        BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ sensors not disabled during transition: %d,%d",
                ff_sensors_to_invalidate, fifo_info->ff_sensors_flush_done_before_invalid);
    }

    fifo_info->ff_sensors_flush_done_before_invalid = 0;


    //if overall wml goes lower
    if (fifo_info->ff_wml_bytes_req < fifo_info->ff_wml_bytes_curr) {
        ff_flush_needed = true;
    }


    if (ff_flush_needed) {
        if (fifo_info->ff_flush_in_proc) {
            BMI160_INST_LOG(HIGH, istate->owner, "ff_flush_in_proc trigger: %d", fifo_info->ff_flush_trigger);
        } else {
            bmi160_hal_fifo_drain(istate, false, BMI160_FIFO_FLUSH_TRIGGER_HW_CHANGE);
        }
    }

    fifo_info->ff_sensors_to_invalidate = ff_sensors_to_invalidate;

    return SNS_RC_SUCCESS;
}



    static
int32_t bmi160_hal_fifo_get_wml_compensated(
        struct bst_sbus_spec    *bus,
        uint32_t                ff_wml_bytes,
        uint16_t                odr_acc,
        uint16_t                odr_gyr,
        uint16_t                odr_mag)
{
    int32_t                     ff_wml_bytes_compensated = 0;
    uint32_t                    bits;
    uint32_t                    traffic_delay_us;
    uint16_t                    fc_new_acc = 0;
    uint16_t                    fc_new_gyr = 0;
    uint16_t                    fc_new_mag = 0;
    uint16_t                    fc_new_max;

    if (bus->type) {
        //SPI
        bits = (((ff_wml_bytes + 1) * (8 + BMI160_CONFIG_SEE_SPI_BYTE_XFER_WAIT_CYCLES)) + 2);
    } else {
        bits = (30 + (ff_wml_bytes * 9));
    }

    traffic_delay_us = bits * 1e6 / bus->clk_rate;

    traffic_delay_us += BMI160_CONFIG_SEE_ASYNC_COM_DELAY_US;

    if (odr_acc > 0) {
        fc_new_acc = BST_CEIL_P_BUF(
                ((traffic_delay_us) * odr_acc) * 1e-6, 0.2);
    }

    if (odr_gyr > 0) {
        fc_new_gyr = BST_CEIL_P_BUF(
                ((traffic_delay_us) * odr_gyr) * 1e-6, 0.2);
    }

    if (odr_mag > 0) {
        fc_new_mag = BST_CEIL_P_BUF(
                ((traffic_delay_us) * odr_mag) * 1e-6, 0.2);
    }

    fc_new_max = fc_new_acc;

    fc_new_max = (fc_new_max > fc_new_gyr) ? fc_new_max : fc_new_gyr;
    fc_new_max = (fc_new_max > fc_new_mag) ? fc_new_max : fc_new_mag;

    ff_wml_bytes_compensated = (int32_t)ff_wml_bytes
        - (fc_new_acc * BMI160_FF_DATA_LEN_ACC)
        - (fc_new_gyr * BMI160_FF_DATA_LEN_GYR)
        - (fc_new_mag * BMI160_FF_DATA_LEN_MAG)
        - fc_new_max;

    if (ff_wml_bytes_compensated <= 0) {
        ff_wml_bytes_compensated = ff_wml_bytes;
    }

    return ff_wml_bytes_compensated;
}


//OPTIM
sns_rc bmi160_hal_config_acc(sns_sensor_instance *this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    sns_rc                      rc;
    uint8_t                     regv;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    if (istate->hw_mod_needed & BMI160_ACCEL) {
        if ( (fifo_info->publish_sensors & BMI160_ACCEL)
                || ((fifo_info->publish_sensors & BMI160_SENSOR_TEMP)
                    && !(fifo_info->publish_sensors & BMI160_GYRO))
                //TODOMAG
           ) {
            if (!istate->accel_info.normal_mode_req_sent) {
                rc = bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_ACC_MODE_NORMAL);
                BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
                istate->accel_info.normal_mode_req_sent = 1;
            }
        }

        regv = istate->accel_info.range_req;


        BMI160_INST_LOG(HIGH, istate->owner, "config_acc range: %d,%d", istate->accel_info.range_curr, istate->accel_info.range_req);

        if (istate->accel_info.range_curr != istate->accel_info.range_req) {
            rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_ACC_RANGE, 0, 7, regv);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
            istate->accel_info.range_curr = istate->accel_info.range_req;
        }

        bmi160_sstvt_t acc_ssvt_array[] = {
            /* in the unit of micro-g/lsb */
            BMI160_ACCEL_SSTVT_2G,
            BMI160_ACCEL_SSTVT_4G,
            BMI160_ACCEL_SSTVT_8G,
            BMI160_ACCEL_SSTVT_16G,
        };
        istate->accel_info.sstvt_curr = roundf(acc_ssvt_array[istate->accel_info.sstate->resolution_idx] * G * (istate->accel_info.sstate->scale_factor * 1e-6));
        //istate->accel_info.sstvt_curr = BMI160_ACCEL_SSTVT_8G * G;


        if (fifo_info->publish_sensors & BMI160_ACCEL) {
            //if (istate->accel_info.odr_curr != istate->accel_info.odr_req) {
            if (istate->accel_info.odr_req != BMI160_REGV_ODR_OFF) {
                regv = 0;
                regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, istate->accel_info.odr_req);
                regv = BST_SET_VAL_BITBLOCK(regv, 4, 6, BMI160_CONFIG_ACC_BWP);
                rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_ACC_CONF, 0, 7, regv);
                BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
                istate->accel_info.odr_curr = istate->accel_info.odr_req;

            }
            //}
        }
    } else {
        if (istate->accel_info.normal_mode_req_sent) {
            rc = bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_ACC_MODE_SUSP);
            istate->accel_info.normal_mode_req_sent = 0;
            istate->accel_info.range_curr = BMI160_REGV_RANGE_ACC_PM2G;
            //istate->accel_info.odr_curr = BMI160_REGV_ODR_OFF;
        }
    }

    return SNS_RC_SUCCESS;
}

//OPTIM
    static
sns_rc bmi160_hal_config_gyro(sns_sensor_instance *this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    sns_rc                      rc;
    uint8_t                     regv;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    if (istate->hw_mod_needed & BMI160_GYRO) {
        if (fifo_info->publish_sensors & BMI160_GYRO) {
            if (!istate->gyro_info.normal_mode_req_sent) {
                rc = bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_GYR_MODE_NORMAL);
                BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
                istate->gyro_info.normal_mode_req_sent = 1;
            }
        }

        if (istate->gyro_info.range_curr != istate->gyro_info.range_req) {
            regv = istate->gyro_info.range_req;

            rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_GYR_RANGE, 0, 7, regv);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
            istate->gyro_info.range_curr = istate->gyro_info.range_req;
        }

        bmi160_gyro_sstvt gyro_ssvt_array[] = {
            BMI160_GYRO_SSTVT_125DPS,
            BMI160_GYRO_SSTVT_250DPS,
            BMI160_GYRO_SSTVT_500DPS,
            BMI160_GYRO_SSTVT_1000DPS,
            BMI160_GYRO_SSTVT_2000DPS,
        };
        istate->gyro_info.sstvt_curr = roundf(gyro_ssvt_array[istate->gyro_info.sstate->resolution_idx] * (istate->gyro_info.sstate->scale_factor * 1e-6));

        if (fifo_info->publish_sensors & BMI160_GYRO) {
            if (istate->gyro_info.odr_curr != istate->gyro_info.odr_req) {
                if (istate->gyro_info.odr_req != BMI160_REGV_ODR_OFF) {
                    regv = 0;
                    regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, istate->gyro_info.odr_req);
                    regv = BST_SET_VAL_BITBLOCK(regv, 4, 5, BMI160_CONFIG_GYR_BWP);
                    rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_GYR_CONF, 0, 7, regv);
                    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
                    istate->gyro_info.odr_curr = istate->gyro_info.odr_req;
                }
            }

#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW
            if (istate->gyro_info.odr_req != BMI160_REGV_ODR_OFF) {
                uint8_t downsample_sw_factor_old = istate->gyro_info.downsample_sw_factor;
                if (BMI160_REGV_ODR_MAP[istate->gyro_info.odr_req].odr >= istate->gyro_info.sample_rate_req) {
                    istate->gyro_info.downsample_sw_factor = BMI160_REGV_ODR_MAP[istate->gyro_info.odr_req].odr
                        / istate->gyro_info.sample_rate_req;
                } else {
                    istate->gyro_info.downsample_sw_factor = 1;
                }

                BMI160_INST_LOG(MED, istate->owner, "ds_sw: %d", istate->gyro_info.downsample_sw_factor);

                if (downsample_sw_factor_old != istate->gyro_info.downsample_sw_factor) {
                    istate->gyro_info.downsample_sw_cnt = 0;
                }
            }
#endif
        }


    } else {
        if (istate->gyro_info.normal_mode_req_sent) {
            rc = bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_GYR_MODE_SUSP);
            istate->gyro_info.normal_mode_req_sent = 0;
            istate->gyro_info.range_curr = BMI160_REGV_RANGE_GYR_PM2000DPS;
            //istate->gyro_info.odr_curr = BMI160_REGV_ODR_OFF;
        }
    }

    return SNS_RC_SUCCESS;
}

//OPTIM
    static
sns_rc bmi160_hal_config_shared_fifo(sns_sensor_instance *this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    sns_rc                      rc;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    struct bst_sbus_spec        bus_spec;

    uint8_t                     regv;
    uint16_t                    ff_wml_bytes;

    uint16_t                    odr_acc;
    uint16_t                    odr_gyr;
    uint16_t                    odr_mag;

    uint8_t                     ff_sensors_en_orig = fifo_info->ff_sensors_en_curr;


    regv = 0;
    if (fifo_info->publish_sensors & BMI160_ACCEL) {
        regv = BST_SET_VAL_BIT(regv, 6);
    }

    if (fifo_info->publish_sensors & BMI160_GYRO) {
        regv = BST_SET_VAL_BIT(regv, 7);
    }

    //TODOMAG

#if BMI160_CONFIG_ENABLE_DRI_MODE
    if (istate->int_en_flags_req.bits.drdy.flag) {
        regv = 0;
    }
#endif

    if (fifo_info->publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG))  {
        regv = BST_SET_VAL_BIT(regv, 1);
    }

    regv = BST_SET_VAL_BIT(regv, 4);

    rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_FIFO_CONFIG_1, 0, 7, regv);
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    istate->cache_regv_fifo_config_1 = regv;

    //INSERT_TRACE_POINT4_T(bmi160, 'F', 'F', fifo_info->ff_sensors_en_curr, fifo_info->ff_sensors_en_req);

#if BMI160_CONFIG_ENABLE_DEBUG_TEST
    if (!(fifo_info->ff_sensors_en_curr & BMI160_ACCEL)) {
        if (fifo_info->ff_sensors_en_req & BMI160_ACCEL) {
            g_bmi160_cnt_session_acc++;
        }
    }

    if (!(fifo_info->ff_sensors_en_curr & BMI160_GYRO)) {
        if (fifo_info->ff_sensors_en_req & BMI160_GYRO) {
            g_bmi160_cnt_session_gyr++;
        }
    }
#endif

    fifo_info->ff_sensors_en_curr = fifo_info->publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);

#if BMI160_CONFIG_ENABLE_DRI_MODE
    if (istate->int_en_flags_req.bits.drdy.flag) {
        fifo_info->ff_sensors_en_curr = 0;
    }
#endif

    //we have calls of config_acc() and config_gyr() ahead of this
    bmi160_hal_fifo_update_curr_masters(istate);

    //config wml in hw
    bus_spec.type = (SNS_BUS_SPI == istate->com_port_info.com_config.bus_type);
    bus_spec.clk_rate = (istate->com_port_info.com_config.min_bus_speed_KHz + istate->com_port_info.com_config.max_bus_speed_KHz) * 500; //500: / 2 * 1000

    odr_acc = BMI160_REGV_ODR_MAP[istate->accel_info.odr_req].odr;
    odr_gyr = BMI160_REGV_ODR_MAP[istate->gyro_info.odr_req].odr;
    odr_mag = 0;    //TODOMAG
    ff_wml_bytes = bmi160_hal_fifo_get_wml_compensated(&bus_spec, fifo_info->ff_wml_bytes_req, odr_acc, odr_gyr, odr_mag);

    //INSERT_TRACE_POINT4_T(bmi160, 'F', fifo_info->ff_wml_bytes_req & 0xff, (fifo_info->ff_wml_bytes_req >> 8)&0xff, ff_wml_bytes);
    regv = ff_wml_bytes >> 2;

    //if (regv > 0)
    if ((regv > 0) && (fifo_info->ff_sensors_en_curr))
    {
        rc = bmi160_dev_reg_read_modify_write(istate, BMI160_REGA_USR_FIFO_CONFIG_0, 0, 7, regv);
        BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
    }

    fifo_info->ff_wml_bytes_curr = ff_wml_bytes;


    BMI160_INST_LOG(HIGH, istate->owner, "ff_wml: %d", ff_wml_bytes);


    if (fifo_info->ff_sensors_en_curr) {
        bmi160_hal_update_couple_ts_host_and_dev_rt(istate);
        BMI160_INST_LOG(HIGH, istate->owner, "ts_pair<%u,0x%x>",
                BMI160_SYS_TIME_LH(istate->ts_pair_sys_dev.ts_sys),
                istate->ts_pair_sys_dev.ts_dev);

        istate->ts_pair_sys_dev_4_ts_res_est = istate->ts_pair_sys_dev;


        fifo_info->ff_tm_info.boost_read_size = 1;
#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
        fifo_info->devi_est_irq = 0;
#endif

        fifo_info->ff_itvl_ticks_est_masters = istate->ts_hw_res_ticks_per_bit * (1 << (BMI160_REGV_ODR_1600HZ - fifo_info->ff_master_odr_curr + 4));
        fifo_info->ff_itvl_ticks_est_acc = istate->ts_hw_res_ticks_per_bit * (1 << (BMI160_REGV_ODR_1600HZ - istate->accel_info.odr_curr + 4));
        fifo_info->ff_itvl_ticks_est_gyr = istate->ts_hw_res_ticks_per_bit * (1 << (BMI160_REGV_ODR_1600HZ - istate->gyro_info.odr_curr + 4));

    } else {
        if (ff_sensors_en_orig) {
            bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_FIFO_FLUSH);
        }

        istate->ts_pair_sys_dev.avail_1st = 0;
    }

    istate->accel_info.ff_wml_curr = istate->accel_info.ff_wml_req;
    istate->gyro_info.ff_wml_curr = istate->gyro_info.ff_wml_req;


    return SNS_RC_SUCCESS;
}

/**
 * Updates temp sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */

    static
void bmi160_hal_config_polling_timer_4_temp(sns_sensor_instance *const this)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)this->state->state;

    BMI160_INST_LOG(MED, this, "sample interval:%u. timer is active:%d", (uint32_t) istate->sensor_temp_info.sampling_intvl,
            (istate->sensor_temp_info.timer_is_active | (istate->sensor_temp_info.timer_itvl_changed << 1)));
    if (istate->sensor_temp_info.sampling_intvl > 0)
    {
        if (!istate->sensor_temp_info.timer_is_active || istate->sensor_temp_info.timer_itvl_changed)
        {
            if (istate->timer_data_stream == NULL)
            {
                BMI160_INST_LOG(MED, this, "timer data stream created");
                istate->stream_mgr->api->create_sensor_instance_stream(
                        istate->stream_mgr,
                        this,
                        istate->timer_suid,
                        &istate->timer_data_stream);
            }
            bmi160_hal_start_sensor_temp_polling_timer(this);

            istate->sensor_temp_info.timer_itvl_changed = false;
        } else {
            BMI160_INST_LOG(MED, this, "timer reconfig ignored");
        }
    }
    else if (istate->sensor_temp_info.timer_is_active)
    {
        istate->sensor_temp_info.timer_is_active = false;
        sns_sensor_util_remove_sensor_instance_stream(this, &istate->timer_data_stream);
        BMI160_INST_LOG(MED, this, "timer removed");
    }
}


    void
bmi160_hal_stop_tempetature_timer(sns_sensor_instance  *const this)
{
    bmi160_instance_state *istate = (bmi160_instance_state *)this->state->state;
    if((NULL != istate->timer_data_stream) &&
            (istate->sensor_temp_info.timer_is_active))
    {
        BMI160_INST_LOG(MED, this, "timer data stream removed");
        istate->stream_mgr->api->remove_stream(istate->stream_mgr, istate->timer_data_stream);
        istate->timer_data_stream = NULL;
        istate->sensor_temp_info.timer_is_active = false;
    }
}

void bmi160_send_fac_cal_event(
        sns_sensor_instance *const  instance,
        const struct bmi160_state   *sstate)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)instance->state->state;

    const sns_sensor_uid            *suid = &(sstate->my_suid);

    sns_cal_event new_calibration_event = sns_cal_event_init_default;
    float bias_data[] = {0,0,0};
    float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

    UNUSED_VAR(istate);

    bias_data[0] = sstate->fac_cal_bias[0] * 1.0f / sstate->scale_factor; // values in istate are usually from registry or from factory test routine
    bias_data[1] = sstate->fac_cal_bias[1] * 1.0f / sstate->scale_factor;
    bias_data[2] = sstate->fac_cal_bias[2] * 1.0f / sstate->scale_factor;
    comp_matrix_data[0] = sstate->fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = sstate->fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = sstate->fac_cal_corr_mat.data[2];
    comp_matrix_data[3] = sstate->fac_cal_corr_mat.data[3];
    comp_matrix_data[4] = sstate->fac_cal_corr_mat.data[4];
    comp_matrix_data[5] = sstate->fac_cal_corr_mat.data[5];
    comp_matrix_data[6] = sstate->fac_cal_corr_mat.data[6];
    comp_matrix_data[7] = sstate->fac_cal_corr_mat.data[7];
    comp_matrix_data[8] = sstate->fac_cal_corr_mat.data[8];

    pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
    { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
    pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
    { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };

    new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
    new_calibration_event.bias.arg = &buff_arg_bias;
    new_calibration_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
    new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
    new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

    BMI160_INST_LOG(HIGH, instance, "bmi cal evt: %d <%d %d %d> ver: %d", sstate->sensor,
            (int)(bias_data[0] * 1000), (int)(bias_data[1] * 1000), (int)(bias_data[2] * 1000),
            sstate->fac_cal_version);

    pb_send_event(instance,
            sns_cal_event_fields,
            &new_calibration_event,
            sns_get_system_time(),
            SNS_CAL_MSGID_SNS_CAL_EVENT,
            suid);
}

//OPTIM
    static
sns_rc bmi160_hal_do_config_hw_now(sns_sensor_instance  *this)
{
    sns_rc                      rc;
    bool                        enable_fifo_stream = true;
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    BMI160_INST_LOG(HIGH, istate->owner, "<dchw> hw_mod_needed: %d publish_sensors: %d",
            istate->hw_mod_needed, fifo_info->publish_sensors);

    //config power, range odr of accel
    rc = bmi160_hal_config_acc(this);

    //config power, range odr of gyro
    rc |= bmi160_hal_config_gyro(this);

    //TODOMAG

    //<config_shared_fifo>
    rc |= bmi160_hal_config_shared_fifo(this);
    //</config_shared_fifo>


    //<config_interrupts>
    enable_fifo_stream = fifo_info->publish_sensors & (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG);
    enable_fifo_stream |= istate->fac_test_in_progress;

    if (istate->irq_ready)
    {
        if (enable_fifo_stream
                || istate->int_en_flags_req.bits.md) {
            rc |= bmi160_hal_config_int_output(this, true);
            BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);
        }
    }

    if (istate->int_en_flags_req.bits.md) {
        rc |= bmi160_hal_set_md_config(this, true);
        rc |= bmi160_hal_config_int_md(this,  true, false);
    } else {
        if (istate->md_info.client_present) {
            rc |= bmi160_hal_config_int_md(this, false, true);
        }
    }


#if BMI160_CONFIG_ENABLE_DRI_MODE
    rc |= bmi160_hal_config_int_drdy(this, istate->int_en_flags_req.bits.drdy.flag);
#endif

    BMI160_INST_LOG(MED, this, "md int:0x%x, enable_fifo_stream=%d", istate->int_en_flags_req.bits.md | (istate->md_info.client_present << 1), enable_fifo_stream);
    rc |= bmi160_hal_config_int_fifo(this, enable_fifo_stream);


    istate->int_en_flags_curr.flag = istate->int_en_flags_req.flag;
    //</config_interrupts>



    //<config_dae>
    // Enable timer in case of sensor temp clients
    BMI160_INST_LOG(MED, this, "publish sensors:0x%x", fifo_info->publish_sensors);
    if (fifo_info->publish_sensors & BMI160_SENSOR_TEMP) {
        bool avail_dae_if = false;

#if BMI160_CONFIG_ENABLE_DAE
        avail_dae_if = bmi160_dae_if_available(this);
#endif
        if (!avail_dae_if) {
            bmi160_hal_config_polling_timer_4_temp(this);
        }
    }
    else
    {
        bmi160_hal_stop_tempetature_timer(this);
    }

    istate->sensor_temp_info.sample_rate_curr = (uint16_t)(istate->sensor_temp_info.sample_rate_req);


#if BMI160_CONFIG_ENABLE_DAE
    if (0 != fifo_info->publish_sensors) {
        bmi160_dae_if_start_streaming(this);
    }
#endif
    //</config_dae>


    istate->config_step = BMI160_CONFIG_IDLE; /* done with reconfig */

    bmi160_hal_send_config_event(this);

    //bmi160_hal_dump_reg(this);

    return SNS_RC_SUCCESS;
}

sns_rc bmi160_hal_reconfig_hw(sns_sensor_instance *this)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;

    sns_rc                      rc = SNS_RC_SUCCESS;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    //INSERT_TRACE_POINT3_T(bmi160, 'H', fifo_info->ff_sensors_en_req, fifo_info->ff_sensors_en_curr);
    BMI160_INST_LOG(MED, this, "reconfig_hw: %d %d", fifo_info->ff_sensors_en_req, fifo_info->ff_sensors_en_curr);
    //we cannot change the HW setting when FIFO async read is still pending
    if (fifo_info->ff_flush_in_proc) {
        BMI160_INST_LOG(HIGH, this, "hw_config_pending due to fifo_flush 1: %d", fifo_info->ff_flush_trigger);
        istate->hw_config_pending = 1;
        return SNS_RC_SUCCESS;
    }

    rc = bmi160_hal_fifo_prepare_4_cos(this);

    if (!fifo_info->ff_flush_in_proc) {
        istate->hw_config_pending = 0;

        bmi160_hal_fifo_invalidate_sensors(istate, fifo_info->ff_sensors_to_invalidate);
        fifo_info->ff_sensors_to_invalidate = 0;
    } else {
        istate->hw_config_pending = 1;

        BMI160_INST_LOG(HIGH, istate->owner, "hw_config_pending due to fifo_flush 2: 0x%x", (fifo_info->ff_sensors_to_invalidate<<8) | fifo_info->ff_flush_trigger);

        return SNS_RC_SUCCESS;
    }

    rc = bmi160_hal_do_config_hw_now(this);
    BMI160_DD_CHECK_RETVAL(rc, SNS_RC_SUCCESS);

    return SNS_RC_SUCCESS;
}

/** See sns_bmi160_hal.h */
void bmi160_hal_handle_sensor_temp_sample(sns_sensor_instance *const instance)
{
    bmi160_instance_state       *istate = (bmi160_instance_state*)instance->state->state;
    sns_rc                      rc;
    uint8_t                     buf[2];


    rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_TEMPERATURE_0, buf, 2);

    if (SNS_RC_SUCCESS == rc) {
        bmi160_hal_convert_and_send_temp_sample(instance,
                sns_get_system_time(),
                buf);
    }
}

void bmi160_hal_fifo_buffer_proc_cleanup(
        bmi160_instance_state           *istate,
        bmi160_fifo_parse_out_desc_t    *ff_parse_out_desc)
{
    bmi160_fifo_info                *fifo_info = &istate->fifo_info;

    uint8_t                         regv = istate->cache_regv_int_map_1;
    sns_rc                          rc = SNS_RC_SUCCESS;

    bmi160_int_check_ctx            ctx;

    uint32_t                        ff_send_flush_done = 0;

    UNUSED_VAR(fifo_info);
    UNUSED_VAR(regv);
    UNUSED_VAR(rc);

    istate->reg_int_ctx.avail.flags = 0;

    istate->async_com_read_response++;
    if (istate->async_com_read_response <= istate->async_com_read_requqest) {
    } else {
        istate->async_com_read_requqest = istate->async_com_read_response;
        BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ handle exception in ascp_req");
    }

    if (!istate->ff_flush_client_req) {
    } else {
        istate->ff_flush_client_req = 0;
        ff_send_flush_done |= BMI160_FIFO_FLUSH_DONE_CTX_CLIENT_REQ;
    }

    if (!fifo_info->ff_flush_in_proc) {
    } else {
        if ((istate->async_com_read_requqest == istate->async_com_read_response)
                && ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_end_frame)
        {
            fifo_info->ff_flush_in_proc = false;

            if (istate->hw_config_pending) {
                istate->hw_config_pending = 0;

                fifo_info->ff_sensors_flush_done_before_invalid = fifo_info->ff_sensors_to_invalidate;
                BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ pick up pending hw_config task: 0x%x",
                        fifo_info->ff_sensors_flush_done_before_invalid);

                bmi160_hal_fifo_invalidate_sensors(istate, fifo_info->ff_sensors_to_invalidate);
                fifo_info->ff_sensors_to_invalidate = 0;
                //in extreme cases, the state may have changed during the fifo flush
                rc = bmi160_hal_reconfig_hw(istate->owner);
            }

            if (fifo_info->ff_flush_in_proc) {
                BMI160_INST_LOG(HIGH, istate->owner, "bmi160_cp_ another fifo_flush");
            } else {
                ff_send_flush_done |= BMI160_FIFO_FLUSH_DONE_CTX_HW_CHANGE;
            }
        } else {
            BMI160_INST_LOG(MED, istate->owner, "ff_flush_in_proc still 0x%x",
                    ((istate->async_com_read_requqest - istate->async_com_read_response) << 1) |
                    ff_parse_out_desc->ff_parse_rslt.bits.ff_avail_end_frame);
        }
    }

    if (ff_send_flush_done) {
        sns_time ts_curr = bmi160_get_sys_tick();
        if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
            //TODO: does it matter if we do this after bmi160_hal_reconfig_hw
            bmi160_send_fifo_flush_done(istate->owner, &istate->accel_info.sstate->my_suid, ts_curr, ff_send_flush_done);
        }

        if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
            bmi160_send_fifo_flush_done(istate->owner, &istate->gyro_info.sstate->my_suid, ts_curr, ff_send_flush_done);
        }
        //TODOMAG
        BMI160_INST_LOG(MED, istate->owner, "fifo_flush_done event: %d,%d@%u",
                ff_send_flush_done, fifo_info->ff_sensors_en_curr, BMI160_SYS_TIME_LH(ts_curr));
    }

    //if (fifo_info->ff_wmi_missing_possible)
    if (fifo_info->ff_int_masked) {
        //rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        istate->sbus_mon_single_byte_rw_off = 0;
        fifo_info->ff_int_masked = 0;

        ctx.int_check_trigger = BMI160_INT_CHECK_TRIGGER_POST_FIFO_READ;
        ctx.timestamp = bmi160_get_sys_tick();
        bool                com_port_read_granted;
        com_port_read_granted  = (istate->async_com_read_requqest - istate->async_com_read_response) < BMI160_CONFIG_COM_FLOW_CTRL_THRESH;
        if (com_port_read_granted) {
            bmi160_hal_handle_interrupt(istate->owner, &ctx);
        } else {
            BMI160_INST_LOG(MED, istate->owner, "dropped int_check: %d", (istate->async_com_read_requqest - istate->async_com_read_response));
        }

    }
}

bool bmi160_hal_int_pin_is_high(
        bmi160_instance_state       *istate)
{
    bool                        is_high;
    sns_rc                      rc;

#if BMI160_CONFIG_ENABLE_SEE_LITE
    sns_gpio_state              gpio_level = SNS_GPIO_STATE_HIGH;

    rc = bmi160_read_gpio(istate, &istate->sstate_creator->common.irq_config, &gpio_level);
    if ((gpio_level == SNS_GPIO_STATE_HIGH) || (SNS_RC_SUCCESS != rc)) {
        is_high = true;
    } else {
        is_high = false;
    }
#else
    uint8_t                     si_buf[2] = "";

    rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_INT_STATUS_0, si_buf, 2);
    is_high = (si_buf[0] | si_buf[1]) || (SNS_RC_SUCCESS != rc);
#endif

    return is_high;
}

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    static
void bmi160_hal_init_diag_sample_state(
        sns_sensor_instance             *instance,
        bmi160_fifo_sample_report_ctx_t *ctx,
        uint8_t                         sensors)
{
    bmi160_instance_state       *istate = (bmi160_instance_state *)instance->state->state;
    sns_diag_service            *diag = istate->diag_service;

    if (sensors & BMI160_ACCEL) {
        log_sensor_state_raw_info *tmp = ctx->log_state_info_acc;

        sns_memzero(tmp, sizeof(log_sensor_state_raw_info));
        tmp->encoded_sample_size = istate->log_raw_encoded_size;

        tmp->diag = diag;
        tmp->instance = instance;
        tmp->sensor_uid = &istate->accel_info.sstate->my_suid;
        bmi160_log_sensor_state_raw_alloc(tmp, 0);
    }

    if (sensors & BMI160_GYRO) {
        log_sensor_state_raw_info *tmp = ctx->log_state_info_gyr;

        sns_memzero(tmp, sizeof(log_sensor_state_raw_info));
        tmp->encoded_sample_size = istate->log_raw_encoded_size;

        tmp->diag = diag;
        tmp->instance = instance;
        tmp->sensor_uid = &istate->gyro_info.sstate->my_suid;
        bmi160_log_sensor_state_raw_alloc(tmp, 0);
    }

    //TODOMAG
}
#endif

void bmi160_hal_process_fifo_data_buffer(
        sns_sensor_instance     *instance,
        const uint8_t           *fifo_buf,
        uint32_t                fifo_len)
{
    bmi160_instance_state       *istate = (bmi160_instance_state *)instance->state->state;
    //sns_service_manager         *service_manager = instance->cb->get_service_manager(instance);
    //sns_event_service           *event_service = (sns_event_service*)service_manager->get_service(service_manager, SNS_EVENT_SERVICE);


    bmi160_fifo_parse_ctx_t     ff_parse_ctx = {NULL, 0, 0, 0, NULL};
    bmi160_fifo_parse_ctx_t     ff_parse_ctx_dryrun = {NULL, 0, 0, 0, NULL};
    bmi160_fifo_parse_out_desc_t  ff_parse_out_desc;


    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    uint8_t                     regv;


    //if (fifo_info->ff_wmi_missing_possible)
    if (bmi160_hal_int_pin_is_high(istate)) {
        regv = BST_SET_VAL_BITBLOCK(istate->cache_regv_int_map_1, 5, 6, 0);
        istate->sbus_mon_single_byte_rw_off = 1;
        //bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        fifo_info->ff_int_masked = 1;
    }

    ff_parse_ctx.ff_buf = fifo_buf;
    ff_parse_ctx.ff_buf_len = fifo_len;
    ff_parse_ctx_dryrun = ff_parse_ctx;
    ff_parse_ctx_dryrun.ff_parse_flag |= BMI160_FIFO_PARSE_FLAG_DRYRUN;


    memset(&ff_parse_out_desc, 0, sizeof(ff_parse_out_desc));
    bmi160_hal_fifo_parse_n_proc_frames(istate, &ff_parse_ctx_dryrun, &ff_parse_out_desc);

    if (!ff_parse_out_desc.ff_parse_rslt.bits.ff_batch_empty) {
        bmi160_hal_fifo_resolve_ts_ref(istate, &ff_parse_out_desc);

        BMI160_INST_LOG(LOW, instance, "fifo sensor enable:0x%x", fifo_info->ff_sensors_en_curr);

#if BMI160_CONFIG_ENABLE_DIAG_LOG
        log_sensor_state_raw_info   log_accel_state_raw_info;
        log_sensor_state_raw_info   log_gyro_state_raw_info;
        bmi160_fifo_sample_report_ctx_t ff_sample_rpt_ctx;

        ff_sample_rpt_ctx.log_state_info_acc = &log_accel_state_raw_info;
        ff_sample_rpt_ctx.log_state_info_gyr = &log_gyro_state_raw_info;

        bmi160_hal_init_diag_sample_state(instance, &ff_sample_rpt_ctx, fifo_info->ff_sensors_en_curr);

        ff_parse_ctx.priv_data = &ff_sample_rpt_ctx;
#endif

        bmi160_hal_fifo_parse_n_proc_frames(istate, &ff_parse_ctx, &ff_parse_out_desc);


#if BMI160_CONFIG_ENABLE_DEBUG_TEST
        if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
            INSERT_TRACE_POINT3_T(bmi160, g_bmi160_cnt_session_acc + 1, fifo_info->ff_tm_info.fc_accum_curr_acc & 0xff, (fifo_info->ff_tm_info.fc_accum_curr_acc >> 8) & 0xff);
        }

        if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
            INSERT_TRACE_POINT3_T(bmi160, g_bmi160_cnt_session_gyr + 1, fifo_info->ff_tm_info.fc_accum_curr_gyr & 0xff, (fifo_info->ff_tm_info.fc_accum_curr_gyr >> 8) & 0xff);
        }
#endif

#if BMI160_CONFIG_ENABLE_DIAG_LOG
        if (fifo_info->ff_sensors_en_curr & BMI160_ACCEL) {
            bmi160_log_sensor_state_raw_submit(&log_accel_state_raw_info, true);
        }

        if (fifo_info->ff_sensors_en_curr & BMI160_GYRO) {
            bmi160_log_sensor_state_raw_submit(&log_gyro_state_raw_info, true);
        }
#endif

    }

    bmi160_hal_fifo_buffer_proc_cleanup(istate, &ff_parse_out_desc);

    //TODOMAG
}


/**
 * Changes all gated accel requests to non-gated accel requests.
 *
 * @param instance   Reference to the instance
 *
 * @return None
 */
static void bmi160_hal_convert_accel_gated_req_to_non_gated(
        sns_sensor_instance *const   instance)
{
    sns_request                 *request;
    bool                        req_converted_to_non_gated = false;

    /** Parse through existing requests and change gated accel
     *  requests to non-gated accel requests. */
    for (request = (sns_request *)instance->cb->get_client_request(instance, &((sns_sensor_uid)ACCEL_SUID), true);
            NULL != request;
            request = (sns_request *)instance->cb->get_client_request(instance, &((sns_sensor_uid)ACCEL_SUID), false))
    {
        if (request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
            req_converted_to_non_gated = true;
        }
    }

    /** Send an event to gated stream clients that the request is
     *  now treated as non-gated */
    if (req_converted_to_non_gated)
    {
        sns_service_manager *mgr = instance->cb->get_service_manager(instance);
        sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
        sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);
        bmi160_instance_state *istate = (bmi160_instance_state *)instance->state->state;

        if (NULL != event)
        {
            event->message_id = SNS_STD_EVENT_GATED_SENSOR_MSGID_GATED_REQ_CONVERTED_TO_NON_GATED;
            event->event_len = 0;
            event->timestamp = sns_get_system_time();
            e_service->api->publish_event(e_service, instance, event, &istate->accel_info.sstate->my_suid);
        }
    }
}

void bmi160_hal_handle_interrupt_md(
        bmi160_instance_state   *istate,
        sns_time                irq_timestamp)
{
    sns_sensor_instance         *instance = istate->owner;

    //if (ctx->avail.bits.int_status_0 && (ctx->int_status_0 & BMI160_INT_STATUS_0_ANYM_INT))
    {
        istate->md_info.md_state.motion_detect_event_type =
            SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
        pb_send_event(instance,
                sns_motion_detect_event_fields,
                &istate->md_info.md_state,
                irq_timestamp,
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &istate->md_info.sstate->my_suid);

        BMI160_INST_LOG(HIGH, instance, "MD fired");

        bmi160_hal_convert_accel_gated_req_to_non_gated(instance);

        // Sensor State HW Interrupt Log
#if BMI160_CONFIG_ENABLE_DIAG_LOG
        sns_diag_service            *diag = istate->diag_service;

        sns_diag_sensor_state_interrupt *log =
            (sns_diag_sensor_state_interrupt *)diag->api->alloc_log(
                    diag,
                    instance,
                    &istate->md_info.sstate->my_suid,
                    sizeof(sns_diag_sensor_state_interrupt),
                    SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT);

        if (NULL != log)
        {
            log->interrupt = SNS_DIAG_INTERRUPT_MOTION;
            log->timestamp = irq_timestamp;

            diag->api->submit_log(diag,
                    instance,
                    &istate->md_info.sstate->my_suid,
                    sizeof(sns_diag_sensor_state_interrupt),
                    log,
                    SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT,
                    istate->log_interrupt_encoded_size,
                    bmi160_encode_sensor_state_log_interrupt);
        }
#endif
    }
}

void bmi160_hal_handle_interrupt_fifo_full(bmi160_instance_state *istate)
{
    uint8_t regv;
    sns_rc  rc;
    bmi160_hal_fifo_invalidate_sensors(istate, (BMI160_ACCEL | BMI160_GYRO | BMI160_MAG));

    istate->ts_pair_sys_dev.avail_1st = 0;
    bmi160_hal_update_couple_ts_host_and_dev_rt(istate);
    istate->ts_pair_sys_dev_4_ts_res_est = istate->ts_pair_sys_dev;


    bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_FIFO_FLUSH);

    regv = 0;
    rc = bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_FIFO_CONFIG_0, &regv, 1);
    if (SNS_RC_SUCCESS == rc) {
        regv = regv >> 1;
        regv = (regv >= 1) ? regv : 1;
        rc = bmi160_sbus_write_wrapper(istate, BMI160_REGA_USR_FIFO_CONFIG_0, &regv, 1);
    }

    BMI160_INST_LOG(ERROR, istate->owner, "WARNING fifo full");
    //TODO: how to send fifo full event
}


#if BMI160_CONFIG_ENABLE_DRI_MODE
    static
void bmi160_hal_read_data_n_ctx_at_dri(
        bmi160_instance_state   *istate,
        uint8_t                 *si_buf,
        uint8_t                 regv_status,
        uint8_t                 len_max)
{
    int8_t                      reg_addr_start      = BMI160_REGA_USR_STATUS;
    int8_t                      reg_addr_end        = BMI160_REGA_USR_STATUS;
    uint8_t                     len                 = 0;
    //uint8_t                     flag                = istate->int_en_flags_curr.bits.drdy.flag;
    sns_rc                      rc;

    si_buf[BMI160_REGA_USR_STATUS] = 0;

    if (regv_status & B7_SET) { //accel
        reg_addr_start = BMI160_REGA_USR_DATA_14;
    }

    if (regv_status & B6_SET) { //gyro
        reg_addr_start = BMI160_REGA_USR_DATA_8;
    }

    //TODOMAG

    len = 1 + reg_addr_end - reg_addr_start;

    if ((len + reg_addr_start) <= len_max) {
        rc = bmi160_sbus_read_wrapper(istate, reg_addr_start, si_buf + reg_addr_start, len);
    }

}

void bmi160_hal_handle_interrupt_drdy(
        bmi160_instance_state   *istate,
        bmi160_int_check_ctx    *ctx)
{
    uint8_t                     si_buf[BMI160_REGA_USR_TEMPERATURE_1 + 1];
    uint8_t                     flag = istate->int_en_flags_curr.bits.drdy.flag;
    uint8_t                     sensors_to_report = 0;
    uint8_t                     regv_status;
    sns_time                    ts = ctx->timestamp;
    bmi160_fifo_sample_report_ctx_t ff_sample_rpt_ctx;

    istate->sbus_mon_single_byte_rw_off = 1;
    bmi160_sbus_read_wrapper(istate, BMI160_REGA_USR_STATUS, &regv_status, 1);
    istate->sbus_mon_single_byte_rw_off = 0;

    bmi160_hal_read_data_n_ctx_at_dri(istate, si_buf, regv_status, ARR_SIZE(si_buf));


    if ((regv_status & B7_SET) && (flag & BMI160_ACCEL)) {
        sensors_to_report |= BMI160_ACCEL;
    }

    if ((regv_status & B6_SET) && (flag & BMI160_GYRO)) {
        sensors_to_report |= BMI160_GYRO;
    }

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    log_sensor_state_raw_info   log_accel_state_raw_info;
    log_sensor_state_raw_info   log_gyro_state_raw_info;

    ff_sample_rpt_ctx.log_state_info_acc = &log_accel_state_raw_info;
    ff_sample_rpt_ctx.log_state_info_gyr = &log_gyro_state_raw_info;

    bmi160_hal_init_diag_sample_state(istate->owner, &ff_sample_rpt_ctx, sensors_to_report);
#endif

    if (sensors_to_report & BMI160_ACCEL) {
        bmi160_hal_report_single_frame_acc(istate, si_buf + BMI160_REGA_USR_DATA_14, ts, &ff_sample_rpt_ctx);
    }

    if (sensors_to_report & BMI160_GYRO) {
        bmi160_hal_report_single_frame_gyr(istate, si_buf + BMI160_REGA_USR_DATA_8, ts, &ff_sample_rpt_ctx);
    }

    //TODOMAG

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    if (sensors_to_report & BMI160_ACCEL) {
        bmi160_log_sensor_state_raw_submit(&log_accel_state_raw_info, true);
    }

    if (sensors_to_report & BMI160_GYRO) {
        bmi160_log_sensor_state_raw_submit(&log_gyro_state_raw_info, true);
    }
#endif

    BMI160_INST_LOG(MED, istate->owner, "dl_drdy:%x,%x", flag, regv_status);

}
#endif

void bmi160_hal_handle_interrupt_fifo_wml(
        bmi160_instance_state   *istate)
{

    bmi160_fifo_read_ctx_t      ctx;

    ctx.ctx_type = BMI160_FIFO_READ_CTX_TYPE_WMI;
    ctx.sync_read = false;

    istate->fifo_info.ff_read_ctx_type = BMI160_FIFO_READ_CTX_TYPE_WMI;

    bmi160_hal_fifo_read_out(istate, &ctx);
    INSERT_TRACE_POINT2_T(bmi160, 'f', 'f');
}

sns_rc bmi160_hal_prepare_interrupt_handling(
        bmi160_instance_state   *istate)
{
    sns_rc                      rc;
    UNUSED_VAR(rc);

    if (!istate->sbus_in_normal_mode) {
        bmi160_hal_update_pmu_stat(istate, true);

        if (!istate->sbus_in_normal_mode) {
            bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_ACC_MODE_NORMAL);
        }
    }


    return SNS_RC_SUCCESS;
}

sns_rc bmi160_hal_handle_interrupt(
        sns_sensor_instance     *const this,
        bmi160_int_check_ctx    *ctx)
{
    sns_rc                      rc;
    bmi160_instance_state       *istate = (bmi160_instance_state*)this->state->state;

    bmi160_int_stat_flag_t      int_stat = {.flag = 0};

    bool                        ff_wmi_missing_possible;
    uint8_t                     cnt_irq_check;
    bmi160_fifo_info            *fifo_info = &istate->fifo_info;

    bmi160_hal_prepare_interrupt_handling(istate);

    cnt_irq_check = 0;

#if BMI160_CONFIG_ENABLE_DRI_MODE
    if (istate->int_en_flags_curr.bits.drdy.flag) {
        bmi160_hal_handle_interrupt_drdy(istate, ctx);

        cnt_irq_check = 1;  //no need to read sensortime in the following steps

        bmi160_int_en_flag_t int_en_flags_curr = istate->int_en_flags_curr;
        int_en_flags_curr.bits.drdy.flag = 0;
        int_en_flags_curr.bits.fifo.flag = 0;
        if (!int_en_flags_curr.flag) {
            return SNS_RC_SUCCESS;
        }
    }
#endif

    do {
        ctx->req_ts_hw = (0 == cnt_irq_check);
        rc = bmi160_hal_read_int_ctx(istate, ctx);
        BMI160_INST_LOG(LOW, istate->owner, "int_status_1:0x%x", istate->reg_int_ctx.int_status_1
                | (((uint32_t)BMI160_SEE_DD_ATTRIB_VERSION<<8) & (~0xff)) );
        if (SNS_RC_SUCCESS == rc) {
            cnt_irq_check++;
        } else {
            //try again in case of error
            ff_wmi_missing_possible = true;
            continue;
        }
        bmi160_dev_parse_int_stat_flags(&istate->reg_int_ctx, &int_stat);


        BMI160_INST_LOG(HIGH,
                istate->owner, "ist<0x%x,0x%x,0x%x>",
                ((istate->int_en_flags_curr.bits.dt << 24) |
                 (istate->int_en_flags_curr.bits.md << 16) |
                 (istate->int_en_flags_curr.bits.fifo.flag << 8) |
                 (istate->int_en_flags_curr.bits.drdy.flag)),

                (
                 (cnt_irq_check << 8) |
                 (int_stat.bits.dt << 4) |
                 (int_stat.bits.md << 3) |
                 (int_stat.bits.ff_full << 2) |
                 (int_stat.bits.ff_wml << 1) |
                 (int_stat.bits.drdy)),

                (ctx->int_check_trigger | (((uint32_t)BMI160_SEE_DD_ATTRIB_VERSION<<8) & (~0xff)) )
                );


        ff_wmi_missing_possible = false;

        if (int_stat.bits.ff_wml || int_stat.bits.ff_full) {
            if (istate->int_en_flags_curr.bits.fifo.flag) {
                istate->ts_irq = ctx->timestamp;

                if (!int_stat.bits.ff_full) {
                    INSERT_TRACE_POINT2_T(bmi160, 'f', '0');
                    bmi160_hal_handle_interrupt_fifo_wml(istate);
                } else {
                    bmi160_hal_handle_interrupt_fifo_full(istate);
                }
            }
        } else {
            if (int_stat.bits.md | int_stat.bits.dt | int_stat.bits.step) {
                if (fifo_info->ff_sensors_en_curr) {
                    ff_wmi_missing_possible = true;
                    //bmi160_hal_disable_int_fifo(this, true);
                }
            }
        }

        if (int_stat.bits.md) {
            /**
             * 1. Handle MD interrupt: Send MD fired event to client.
             * 2. Disable MD.
             * 3. Start Gated Accel FIFO stream with desired config.
             */
            if (istate->int_en_flags_curr.bits.md) {
                bmi160_hal_handle_interrupt_md(istate, ctx->timestamp);

                istate->int_en_flags_curr.bits.md = false;
                istate->int_en_flags_req.bits.md = false;
                bmi160_hal_config_int_md(this, false, false);

                istate->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
                if (istate->accel_info.gated_client_present)
                {
                    //TODO: check if this is needed by driver or will SEE framework do set_client_request() again
                    fifo_info->publish_sensors |= BMI160_ACCEL;
                    bmi160_inst_assess_overall_req(istate);

                    bmi160_hal_reconfig_hw(this);
                    if (istate->hw_config_pending) {
                    }

                    //bmi160_hal_send_config_event(this);
#if BMI160_CONFIG_ENABLE_DAE
                    bmi160_dae_if_start_streaming(this);
#endif
                }
                //bmi160_hal_dump_reg(this);
            }
        }

        if (int_stat.bits.md | int_stat.bits.dt | int_stat.bits.step) {
            bmi160_hal_send_cmd(istate, BMI160_REGV_CMD_INT_RESET);
        }

    } while (ff_wmi_missing_possible && (cnt_irq_check < 3));


    return SNS_RC_SUCCESS;
}

/** See bmi160_hal.h */
void bmi160_send_fifo_flush_done(sns_sensor_instance *const instance, sns_sensor_uid *uid, sns_time ts, uint8_t context)
{
    sns_service_manager *mgr = instance->cb->get_service_manager(instance);
    sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event *event = NULL;

    event = e_service->api->alloc_event(e_service, instance, 0);

    if ((NULL != event))
    {
        event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
        event->event_len = 0;
        event->timestamp = (ts & (~(sns_time)0x07)) | (context & 0x07) + 0x08;
        //event->timestamp = ts;

        e_service->api->publish_event(e_service, instance, event, uid);
    }
}

sns_rc bmi160_hal_handle_timer_cmd(bmi160_instance_state   *istate)
{
    BMI160_INST_LOG(MED, istate->owner, "inst_ev timer_cmd_stream");

    bmi160_hal_update_pmu_stat(istate, true);
    bmi160_hal_dump_reg(istate->owner);

    if (bmi160_dev_is_last_cmd_done(istate->cmd_handler.cmd_last_written, istate->pmu_stat.reg)) {
        istate->cmd_handler.cmd_in_proc = 0;
        bmi160_hal_cmd_deque(istate);
    } else {
        BMI160_INST_LOG(MED, istate->owner, "bmi160_cp_ last cmd not finished retry: %x",
                istate->cmd_handler.cmd_last_written);
        //this is really some unknown exception, we will retry the last command
        istate->cmd_handler.cmd_in_proc = 0;
        bmi160_hal_send_cmd(istate, istate->cmd_handler.cmd_last_written);
    }

    return SNS_RC_SUCCESS;
}

void bmi160_hal_start_timer(
        sns_sensor_instance     *this,
        sns_data_stream         *timer_stream,
        sns_time                time_out)
{
    bmi160_instance_state   *istate = (bmi160_instance_state*)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    uint8_t                 buffer[50] = "";
    sns_request             timer_req = {
        .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request    = buffer
    };

    UNUSED_VAR(istate);

    if (NULL == timer_stream) {
        BMI160_INST_LOG(ERROR, istate->owner, "timer_stream NULL");
        return;
    }

    sns_memset(buffer, 0, sizeof(buffer));
    req_payload.is_periodic = false;
    req_payload.start_time = bmi160_get_sys_tick();
    req_payload.timeout_period = time_out;

    timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
            sns_timer_sensor_config_fields, NULL);
    if (timer_req.request_len > 0)
    {
        timer_stream->api->send_request(timer_stream, &timer_req);

        BMI160_INST_LOG(HIGH, istate->owner, "timer_start expires %u ticks later", (uint32_t)time_out);
    }
    else
    {
        BMI160_INST_LOG(ERROR, this, "timer req encode error");
    }
}

void bmi160_hal_register_interrupt(sns_sensor_instance *this)
{
    bmi160_instance_state *istate = (bmi160_instance_state*)this->state->state;
    if(!(istate->irq_registered))
    {
        sns_data_stream* data_stream = istate->interrupt_data_stream;
        uint8_t buffer[20];
        sns_request irq_req =
        {
            .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
            .request    = buffer
        };

        irq_req.request_len = pb_encode_request(buffer,
                sizeof(buffer),
                &istate->sstate_creator->common.irq_config,
                sns_interrupt_req_fields,
                NULL);
        if(irq_req.request_len > 0)
        {
            data_stream->api->send_request(data_stream, &irq_req);
            istate->irq_registered = true;
        }
    }
}

