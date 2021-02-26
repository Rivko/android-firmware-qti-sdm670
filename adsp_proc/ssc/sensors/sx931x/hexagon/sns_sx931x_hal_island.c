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



#include "sns_rc.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_math_util.h"
#include "sns_service_manager.h"
#include "sns_com_port_types.h"
#include "sns_sync_com_port_service.h"
#include "sns_gpio_service.h"
#include "sns_types.h"

#include "sns_sx931x_hal.h"
#include "sns_sx931x_sensor.h"
#include "sns_sx931x_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"

#include "sns_std_sensor.pb.h"

#include "sns_diag_service.h"
#include "sns_std.pb.h"
#include "sns_diag.pb.h"

#include "sns_proximity.pb.h"


REGISTER_SETTING g_SX931xConfigurationSettings_0[] =
{
    { SX931x_IRQFUNC_REG,   (0x00) }, //0x04
    { SX931x_CPS_CTRL1_REG, (0x00) }, //0x11
    { SX931x_CPS_CTRL2_REG, (0x84) }, //0x12
    { SX931x_CPS_CTRL3_REG, (0x0F) }, //0x13
    { SX931x_CPS_CTRL4_REG, (0x0D) }, //0x14
    { SX931x_CPS_CTRL5_REG, (0xC1) }, //0x15
    { SX931x_CPS_CTRL6_REG, (0x20) }, //0x16
    { SX931x_CPS_CTRL7_REG, (0x0C) }, //0x17
    { SX931x_CPS_CTRL8_REG, (0xAD) }, //0x18
    { SX931x_CPS_CTRL9_REG, (0xAD) }, //0x19
    { SX931x_CPS_CTRL10_REG, (0x10)}, //0x1A
    { SX931x_CPS_CTRL11_REG, (0x00)}, //0x1B
    { SX931x_CPS_CTRL12_REG, (0x00)}, //0x1C
    { SX931x_CPS_CTRL13_REG, (0x00)}, //0x1D
    { SX931x_CPS_CTRL14_REG, (0x00)}, //0x1E
    { SX931x_CPS_CTRL15_REG, (0x00)}, //0x1F
    { SX931x_CPS_CTRL16_REG, (0x00)}, //0x20
    { SX931x_CPS_CTRL17_REG, (0x00)},  //0x21
    { SX931x_CPS_CTRL18_REG, (0x00)}, //0x22
    { SX931x_CPS_CTRL19_REG, (0x00)}, //0x23
    { SX931x_SAR_CTRL0_REG, (0x08)}, //0x2A
    { SX931x_SAR_CTRL1_REG, (0x76)}, //0x2B
    { SX931x_SAR_CTRL2_REG, (0x41)}, //0x2C
    { SX931x_IRQ_ENABLE_REG,(0x00) }, //0x03
    { SX931x_CPS_CTRL0_REG, (0x7F)}, //0x10
    { SX931x_IRQSTAT_REG, (0x10)}, //0x00 compensation
};
const uint32_t g_SX931xRegisterSize_0 = (sizeof (g_SX931xConfigurationSettings_0) / sizeof(REGISTER_SETTING));

#if DUAL_SENSOR_SUPPORT
REGISTER_SETTING g_SX931xConfigurationSettings_1[] =
{
    { SX931x_IRQFUNC_REG,       (0x00)}, //0x04
    { SX931x_CPS_CTRL1_REG,     (0x00)}, //0x11
    { SX931x_CPS_CTRL2_REG,     (0x84)}, //0x12
    { SX931x_CPS_CTRL3_REG,     (0x0F)}, //0x13
    { SX931x_CPS_CTRL4_REG,     (0x0D)}, //0x14
    { SX931x_CPS_CTRL5_REG,     (0xC1)}, //0x15
    { SX931x_CPS_CTRL6_REG,     (0x20)}, //0x16
    { SX931x_CPS_CTRL7_REG,     (0x0C)}, //0x17
    { SX931x_CPS_CTRL8_REG,     (0xAD)}, //0x18
    { SX931x_CPS_CTRL9_REG,     (0xAD)}, //0x19
    { SX931x_CPS_CTRL10_REG,    (0x10)}, //0x1A
    { SX931x_CPS_CTRL11_REG,    (0x00)}, //0x1B
    { SX931x_CPS_CTRL12_REG,    (0x00)}, //0x1C
    { SX931x_CPS_CTRL13_REG,    (0x00)}, //0x1D
    { SX931x_CPS_CTRL14_REG,    (0x00)}, //0x1E
    { SX931x_CPS_CTRL15_REG,    (0x00)}, //0x1F
    { SX931x_CPS_CTRL16_REG,    (0x00)}, //0x20
    { SX931x_CPS_CTRL17_REG,    (0x00)}, //0x21
    { SX931x_CPS_CTRL18_REG,    (0x00)}, //0x22
    { SX931x_CPS_CTRL19_REG,    (0x00)}, //0x23
    { SX931x_SAR_CTRL0_REG,     (0x08)}, //0x2A
    { SX931x_SAR_CTRL1_REG,     (0x76)}, //0x2B
    { SX931x_SAR_CTRL2_REG,     (0x41)}, //0x2C
    { SX931x_IRQ_ENABLE_REG,    (0x00)}, //0x03
    { SX931x_CPS_CTRL0_REG,     (0x7F)}, //0x10
    { SX931x_IRQSTAT_REG,       (0x10)}, //0x00 compensation
};
const uint32_t g_SX931xRegisterSize_1 = (sizeof (g_SX931xConfigurationSettings_1) / sizeof(REGISTER_SETTING));
#endif

PREGISTER_SETTING g_SX931xConfigurationSettings = NULL;
uint32_t          g_SX931xRegisterSize          = 0;


typedef struct log_sensor_state_raw_info
{
    /* Pointer to diag service */
    sns_diag_service *diag;
    /* Pointer to sensor instance */
    sns_sensor_instance *instance;
    /* Pointer to sensor UID*/
    struct sns_sensor_uid *sensor_uid;
    /* Size of a single encoded sample */
    size_t encoded_sample_size;
    /* Pointer to log*/
    void *log;
    /* Size of allocated space for log*/
    uint32_t log_size;
    /* Number of actual bytes written*/
    uint32_t bytes_written;
    /* Number of batch samples written*/
    /* A batch may be composed of several logs*/
    uint32_t batch_sample_cnt;
    /* Number of log samples written*/
    uint32_t log_sample_cnt;
} log_sensor_state_raw_info;

// Unencoded batch sample
typedef struct
{
    /* Batch Sample type as defined in sns_diag.pb.h */
    sns_diag_batch_sample_type sample_type;
    /* Timestamp of the sensor state data sample */
    sns_time timestamp;
    /*Raw sensor state data sample*/
    float sample[SX931x_NUM_AXES];
    /* Data status.*/
    sns_std_sensor_sample_status status;
} sx931x_batch_sample;

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
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc sx931x_encode_sensor_state_log_interrupt(
    void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
    size_t *bytes_written)
{
    UNUSED_VAR(log_size);
    sns_rc rc = SNS_RC_SUCCESS;

    if(NULL == encoded_log || NULL == log || NULL == bytes_written)
    {
        return SNS_RC_FAILED;
    }

    sns_diag_sensor_state_interrupt *sensor_state_interrupt =
        (sns_diag_sensor_state_interrupt *)log;
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

    if(!pb_encode(&stream, sns_diag_sensor_state_interrupt_fields,
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
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc sx931x_encode_log_sensor_state_raw(void *log, size_t log_size,
        size_t encoded_log_size,
        void *encoded_log,
        size_t *bytes_written)
{
    sns_rc rc = SNS_RC_SUCCESS;
    uint32_t i = 0;
    size_t encoded_sample_size = 0;
    size_t parsed_log_size = 0;
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    uint8_t arr_index = 0;
    float temp[SX931x_NUM_AXES];
    pb_float_arr_arg arg =
    {
        .arr = (float *)temp, .arr_len = SX931x_NUM_AXES,
        .arr_index = &arr_index
    };

    if(NULL == encoded_log || NULL == log || NULL == bytes_written)
    {
        return SNS_RC_FAILED;
    }

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;

    if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                            &batch_sample))
    {
        return SNS_RC_FAILED;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
    sx931x_batch_sample *raw_sample = (sx931x_batch_sample *)log;

    while(parsed_log_size < log_size &&
          (stream.bytes_written + encoded_sample_size) <= encoded_log_size &&
          i < (uint32_t)(-1))
    {
        arr_index = 0;
        arg.arr = (float *)raw_sample[i].sample;

        batch_sample.sample_type = raw_sample[i].sample_type;
        batch_sample.status = raw_sample[i].status;
        batch_sample.timestamp = raw_sample[i].timestamp;

        if(!pb_encode_tag(&stream, PB_WT_STRING,
                          sns_diag_sensor_state_raw_sample_tag))
        {
            rc = SNS_RC_FAILED;
            break;
        }
        else if(!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                     &batch_sample))
        {
            rc = SNS_RC_FAILED;
            break;
        }

        parsed_log_size += sizeof(sx931x_batch_sample);
        i++;
    }

    if (SNS_RC_SUCCESS == rc)
    {
        *bytes_written = stream.bytes_written;
    }

    return rc;
}


void sx931x_log_sensor_state_raw_alloc(log_sensor_state_raw_info *log_raw_info,
                                       uint32_t log_size)
{
    uint32_t max_log_size = 0;

    if(NULL == log_raw_info || NULL == log_raw_info->diag ||
       NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
    {
        return;
    }

    // allocate memory for sensor state - raw sensor log packet
    max_log_size = log_raw_info->diag->api->get_max_log_size(log_raw_info->diag);

    if(0 == log_size)
    {
        // log size not specified.
        // Use max supported log packet size
        log_raw_info->log_size = max_log_size;
    }
    else if(log_size <= max_log_size)
    {
        log_raw_info->log_size = log_size;
    }
    else
    {
        return;
    }

    log_raw_info->log = log_raw_info->diag->api->alloc_log(log_raw_info->diag,
                        log_raw_info->instance,
                        log_raw_info->sensor_uid,
                        log_raw_info->log_size,
                        SNS_DIAG_SENSOR_STATE_LOG_RAW);

    log_raw_info->log_sample_cnt = 0;
    log_raw_info->bytes_written = 0;
}


void sx931x_log_sensor_state_raw_submit(log_sensor_state_raw_info *log_raw_info,
                                        bool batch_complete)
{
    sx931x_batch_sample *sample = NULL;

    if(NULL == log_raw_info || NULL == log_raw_info->diag ||
       NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
       NULL == log_raw_info->log)
    {
        return;
    }

    sample = (sx931x_batch_sample *)log_raw_info->log;

    if(batch_complete)
    {
        // overwriting previously sample_type for last sample
        if(1 == log_raw_info->batch_sample_cnt)
        {
            sample[0].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
        }
        else if(1 < log_raw_info->batch_sample_cnt)
        {
            sample[log_raw_info->log_sample_cnt - 1].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
        }
    }

    log_raw_info->diag->api->submit_log(log_raw_info->diag,
                                        log_raw_info->instance,
                                        log_raw_info->sensor_uid,
                                        log_raw_info->bytes_written,
                                        log_raw_info->log,
                                        SNS_DIAG_SENSOR_STATE_LOG_RAW,
                                        log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
                                        sx931x_encode_log_sensor_state_raw);

    log_raw_info->log = NULL;
}

/*
* Add raw uncalibrated sensor data to Sensor State Raw log packet
*/
sns_rc sx931x_log_sensor_state_raw_add(log_sensor_state_raw_info *log_raw_info,
                                       float *raw_data,
                                       sns_time timestamp,
                                       sns_std_sensor_sample_status status)
{
    sns_rc rc = SNS_RC_SUCCESS;

    if(NULL == log_raw_info || NULL == log_raw_info->diag ||
       NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
       NULL == raw_data || NULL == log_raw_info->log)
    {
        return SNS_RC_FAILED;
    }

    if( (log_raw_info->bytes_written + sizeof(sx931x_batch_sample)) > log_raw_info->log_size)
    {
        // no more space in log packet
        // submit and allocate a new one
        sx931x_log_sensor_state_raw_submit(log_raw_info, false);
        sx931x_log_sensor_state_raw_alloc(log_raw_info, 0);
    }

    if(NULL == log_raw_info->log)
    {
        rc = SNS_RC_FAILED;
    }
    else
    {
        sx931x_batch_sample *sample = (sx931x_batch_sample *)log_raw_info->log;

        if(0 == log_raw_info->batch_sample_cnt)
        {
            sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
        }
        else
        {
            sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
        }
        sample[log_raw_info->log_sample_cnt].timestamp = timestamp;

        sns_memscpy(sample[log_raw_info->log_sample_cnt].sample,
                    sizeof(sample[log_raw_info->log_sample_cnt].sample),
                    raw_data,
                    sizeof(sample[log_raw_info->log_sample_cnt].sample));

        sample[log_raw_info->log_sample_cnt].status = status;

        log_raw_info->bytes_written += sizeof(sx931x_batch_sample);
        log_raw_info->log_sample_cnt++;
        log_raw_info->batch_sample_cnt++;
    }

    return rc;
}

/*
 * Read wrapper for Synch Com Port Service.
 */
sns_rc sx931x_com_read_wrapper(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle,
                               uint32_t reg_addr,
                               uint8_t  *buffer,
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

/*
 * Write wrapper for Synch Com Port Service.
 */
sns_rc sx931x_com_write_wrapper(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint32_t reg_addr,
                                uint8_t  *buffer,
                                uint32_t bytes,
                                uint32_t *xfer_bytes,
                                bool     save_write_time)
{
    sns_port_vector port_vec;
    port_vec.buffer = buffer;
    port_vec.bytes = bytes;
    port_vec.is_write = true;
    port_vec.reg_addr = reg_addr;

    return  scp_service->api->sns_scp_register_rw(port_handle,
            &port_vec,
            1,
            save_write_time,
            xfer_bytes);
}




sns_rc sx931x_device_force_compensation(sns_sync_com_port_service *scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        sx931x_sensor_type sensor)
{
    UNUSED_VAR(sensor);
    uint8_t buffer[1];
    sns_rc rv = SNS_RC_SUCCESS;
    sns_time cur_time;
    uint32_t xfer_bytes;


    buffer[0] = 0x0F; //compensation
    rv = sx931x_com_write_wrapper(scp_service,
                                  port_handle,
                                  SX931x_IRQSTAT_REG,
                                  &buffer[0],
                                  1,
                                  &xfer_bytes,
                                  false);
    if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
    {
        return rv;
    }

    cur_time = sns_get_system_time();

    do
    {
        if(sns_get_system_time() > (cur_time + sns_convert_ns_to_ticks(10 * 1000 * 1000)))
        {
            // Sensor HW has not recovered from SW reset.
            return SNS_RC_FAILED;
        }
        else
        {
            //1ms wait
            sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));

            rv = sx931x_com_read_wrapper(scp_service,
                                         port_handle,
                                         SX931x_STAT1_REG,
                                         &buffer[0],
                                         1,
                                         &xfer_bytes);

            if(rv != SNS_RC_SUCCESS)
            {
                // HW not ready. Keep going.
            }
            if(xfer_bytes != 1)
            {
                return SNS_RC_FAILED;
            }
        }

    }
    while((buffer[0] & 0x0F));   //compensation complete

    return SNS_RC_SUCCESS;
}

sns_rc sx931x_device_sw_reset(sns_sync_com_port_service *scp_service,
                              sns_sync_com_port_handle *port_handle)
{
    uint8_t buffer[1];
    sns_rc rv = SNS_RC_SUCCESS;
    sns_time cur_time;
    uint32_t xfer_bytes;


    buffer[0] = SX931x_SOFTRESET_VALUE;
    rv = sx931x_com_write_wrapper(scp_service,
                                  port_handle,
                                  SX931x_SOFTRESET_REG,
                                  &buffer[0],
                                  1,
                                  &xfer_bytes,
                                  false);
    if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
    {
        return rv;
    }

    cur_time = sns_get_system_time();

    do
    {
        if(sns_get_system_time() > (cur_time + sns_convert_ns_to_ticks(10 * 1000 * 1000)))
        {
            // Sensor HW has not recovered from SW reset.
            return SNS_RC_FAILED;
        }
        else
        {
            //1ms wait
            sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));

            rv = sx931x_com_read_wrapper(scp_service,
                                         port_handle,
                                         SX931x_IRQSTAT_REG,
                                         &buffer[0],
                                         1,
                                         &xfer_bytes);

            if(rv != SNS_RC_SUCCESS)
            {
                // HW not ready. Keep going.
            }
            if(xfer_bytes != 1)
            {
                return SNS_RC_FAILED;
            }
        }

    }
    while(!(buffer[0] & 0x80));

    return SNS_RC_SUCCESS;

}

/**
* see sns_sx932x_hal.h
*/
sns_rc sx931x_device_set_default_state(sns_sync_com_port_service *scp_service,
                                       sns_sync_com_port_handle *port_handle,
                                       uint32_t hw_id)
{
    sns_rc rv = SNS_RC_SUCCESS;
    REGISTER_SETTING register_set;
    uint32_t xfer_bytes;
    uint32_t i;
    sns_time cur_time;
    uint8_t buffer[1];

    g_SX931xRegisterSize          = g_SX931xRegisterSize_0;
    g_SX931xConfigurationSettings = &g_SX931xConfigurationSettings_0[0];
    
#if DUAL_SENSOR_SUPPORT
    if(hw_id == 1)
    {
        g_SX931xRegisterSize          = g_SX931xRegisterSize_1;
        g_SX931xConfigurationSettings = &g_SX931xConfigurationSettings_1[0];
    }
#else
    if(hw_id == 1)
    {
        return SNS_RC_FAILED;
    }
#endif
    for (i = 0; i < g_SX931xRegisterSize; i++)
    {
        register_set = g_SX931xConfigurationSettings[i];

        rv = sx931x_com_write_wrapper(scp_service, port_handle,
                                      register_set.Register,
                                      &register_set.Value,
                                      1,
                                      &xfer_bytes,
                                      false);

        if (rv != SNS_RC_SUCCESS || xfer_bytes != 1)
        {
            return rv;
        }
    }

    cur_time = sns_get_system_time();
    do
    {
        if(sns_get_system_time() > (cur_time + sns_convert_ns_to_ticks(2000 * 1000 * 1000)))
        {
            return SNS_RC_FAILED; 	// Sensor HW has not recovered from SW reset.
        }
        else
        {
            sns_busy_wait(sns_convert_ns_to_ticks(100 * 1000 * 1000)); 	//1ms wait
            rv = sx931x_com_read_wrapper(scp_service, port_handle,
                                         SX931x_STAT1_REG,
                                         &buffer[0],
                                         1,
                                         &xfer_bytes);

            if(rv != SNS_RC_SUCCESS)
            {
                // HW not ready. Keep going.
            }
            if(xfer_bytes != 1)
            {
                return SNS_RC_FAILED;
            }
        }
    }
    while((buffer[0] & 0x0F));  //compensation complete

    //rest IRQ STA
    rv = sx931x_com_read_wrapper(scp_service,
                                 port_handle,
                                 SX931x_IRQSTAT_REG,
                                 &buffer[0],
                                 1,
                                 &xfer_bytes);

    return SNS_RC_SUCCESS;

}





sns_rc sx931x_reset_device(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle *port_handle,
                           uint32_t hw_id)
{
    sns_rc rv = SNS_RC_SUCCESS;

    /*HW reset */
    rv = sx931x_device_sw_reset(scp_service, port_handle);

    if (rv == SNS_RC_SUCCESS)
    {
        rv = sx931x_device_set_default_state(scp_service, port_handle, hw_id);
    }
    return rv;
}


sns_rc sx931x_interrupt_config(sns_sync_com_port_service *scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        uint8_t config)
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t rw_data;
  uint32_t xfer_bytes;
  
  rw_data = config;
  rv = sx931x_com_write_wrapper(scp_service, 
                                port_handle,
                                SX931x_IRQ_ENABLE_REG,
                                &rw_data,
                                1,
                                &xfer_bytes,
                                false);
 return rv;

}


sns_rc sx931x_get_who_am_i(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle *port_handle,
                           uint8_t *buffer)
{
    sns_rc rv = SNS_RC_SUCCESS;
    uint32_t xfer_bytes;

    rv = sx931x_com_read_wrapper(scp_service,
                                 port_handle,
                                 SX931x_WHOAMI_REG,
                                 buffer,
                                 1,
                                 &xfer_bytes);

    if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
    {
        rv = SNS_RC_FAILED;
    }

    return rv;
}



void sx931x_read_gpio(sns_sensor_instance *instance, uint32_t gpio, bool is_chip_pin)
{
    sns_service_manager *smgr = instance->cb->get_service_manager(instance);
    sns_gpio_service *gpio_svc = (sns_gpio_service *)smgr->get_service(smgr, SNS_GPIO_SERVICE);
    sns_gpio_state val;
    sns_rc rc = SNS_RC_SUCCESS;

    rc = gpio_svc->api->read_gpio(gpio, is_chip_pin, &val);

    SX931x_INST_PRINTF(LOW, instance, "gpio_val = %d  rc = %d", val, rc);
}


void  sx931x_dump_reg(sns_sensor_instance *const instance, uint32_t hw_id)

{
    UNUSED_VAR(hw_id);
#ifndef SX931x_ENABLE_DEBUG_MSG
    UNUSED_VAR(instance);    
#else
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    /* variable used to return communication result*/
    int8_t com_rslt = E_ERROR;
    uint8_t a_data_u8r[32];
    uint32_t xfer_bytes;
    struct group_read {
    uint32_t first_reg;
    uint8_t  num_regs;
    } groups[] = { /* Read in groups for efficiency */
        { SX931x_IRQSTAT_REG, 5 },
        { SX931x_CPS_CTRL0_REG, 23 },
    };

    if (state == SX_NULL)
    {
        return;
    }

    for(uint32_t i=0; i<ARR_SIZE(groups); i++)
    {
        if(ARR_SIZE(a_data_u8r) < groups[i].num_regs){
	    return;
        }

        com_rslt = state->com_read(state->scp_service,
                               state->com_port_info.port_handle,
                               groups[i].first_reg,
                               a_data_u8r,
                               groups[i].num_regs,
                               &xfer_bytes);
	if(com_rslt != SUCCESS) {
	    SX931x_INST_PRINTF(MED, instance, "Dump:: read register error:%d",
                 com_rslt);
	    return;
	}
	for(uint32_t j=0; j<groups[i].num_regs; j++)
        {
            SX931x_INST_PRINTF(MED, instance, "Dump:: reg[0x%X] = 0x%X",
                 groups[i].first_reg+j, a_data_u8r[j]);
        }
    }
#endif
}


static void sx931x_process_sar_report_stream_data_buffer(sns_sensor_instance *const instance)
{
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    sns_diag_service *diag       = state->diag_service;
    log_sensor_state_raw_info log_sensor_state_raw_info;
    sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
    sns_time timestamp = sns_get_system_time();
    sns_sensor_uid suid;
    uint32_t xfer_bytes;

    int16_t useful_data_ph[3];
    int16_t diff_data_ph[3];
    uint16_t dc_offset_ph[3];
    uint8_t proximity_state;
    uint8_t tempData[2];
    float sample_data[4];
    uint8_t i = 0;

    //read state
    state->com_read(state->scp_service, state->com_port_info.port_handle,
                    SX931x_STAT0_REG, tempData, 1, &xfer_bytes);
    proximity_state = tempData[0] & 0x0F;

    for(i = 0; i < 3; i++)
    {
        //here just read the PH0
        state->com_write(state->scp_service, state->com_port_info.port_handle,
                         SX931x_CPSRD, &i, 1, &xfer_bytes, false);
        //read useful
        state->com_read(state->scp_service, state->com_port_info.port_handle,
                        SX931x_USEMSB, tempData, 2, &xfer_bytes);
        useful_data_ph[i] = (int16_t) tempData[0] << 8 | tempData[1];
        //read diff
        state->com_read(state->scp_service, state->com_port_info.port_handle,
                        SX931x_DIFFMSB, tempData, 2, &xfer_bytes);
        diff_data_ph[i] = (int16_t) tempData[0] << 8 | tempData[1];
        //read offset
        state->com_read(state->scp_service, state->com_port_info.port_handle,
                        SX931x_OFFSETMSB, tempData, 2, &xfer_bytes);
        dc_offset_ph[i] = (uint16_t) tempData[0] << 8 | tempData[1];

        SX931x_INST_PRINTF(MED, instance, "sx931x PH[%d] useful= %d; diff= %d; dc_offset= %d", 
                           i, useful_data_ph[i], diff_data_ph[i], dc_offset_ph[i]);
    }


    suid = state->sar_info.suid;

    sample_data[0] = (float)proximity_state;
    sample_data[1] = (float)diff_data_ph[0];
    sample_data[2] = (float)diff_data_ph[1];
    sample_data[3] = (float)diff_data_ph[2];

    pb_send_sensor_stream_event(instance,
                                &suid,
                                timestamp,
                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
                                sample_data,
                                4,
                                state->encoded_imu_event_len);
    // log information
    log_sensor_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
    log_sensor_state_raw_info.diag = diag;
    log_sensor_state_raw_info.instance = instance;
    log_sensor_state_raw_info.sensor_uid = &suid;
    sx931x_log_sensor_state_raw_alloc(&log_sensor_state_raw_info, 0);
    sx931x_log_sensor_state_raw_add(&log_sensor_state_raw_info,
                                    sample_data,
                                    timestamp,
                                    status);
    sx931x_log_sensor_state_raw_submit(&log_sensor_state_raw_info, true);
}



void sx931x_handle_sar_data_stream_interrupt_event(sns_sensor_instance *const instance)
{
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    sns_rc status;
    uint8_t tempData;
    uint32_t xfer_bytes;

    SX931x_INST_PRINTF(LOW, instance, "sx931x_handle_sar_data_stream_interrupt_event");

    if(NULL == state)
    {
        return;
    }
    status = state->com_read(state->scp_service, state->com_port_info.port_handle,
                             SX931x_IRQSTAT_REG, &tempData, 1, &xfer_bytes);


    if(status != SNS_RC_SUCCESS)
    {
        SX931x_INST_PRINTF(MED, instance, "sx932x_handle_sar_interrupt = %d", status);
        return;
    }

    sx931x_process_sar_report_stream_data_buffer(instance);
}


void sx931x_handle_sar_data_stream_timer_event(sns_sensor_instance *const instance)
{
    sx931x_instance_state *state = (sx931x_instance_state *)instance->state->state;
    SX931x_INST_PRINTF(LOW, instance, "sx931x_handle_sar_data_stream_timer_event");

    if(NULL == state)
    {
        return;
    }
    sx931x_process_sar_report_stream_data_buffer(instance);
}



void sx931x_register_interrupt(sns_sensor_instance *this)
{
    sx931x_instance_state *state = (sx931x_instance_state *)this->state->state;

    if(!state->irq_info.irq_registered)
    {
        sns_data_stream *data_stream = state->interrupt_data_stream;
        uint8_t buffer[20];
        sns_request irq_req =
        {
            .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
            .request    = buffer
        };

        irq_req.request_len = pb_encode_request(buffer,
                                                sizeof(buffer),
                                                &state->irq_info.irq_config,
                                                sns_interrupt_req_fields,
                                                NULL);
        if(irq_req.request_len > 0)
        {
            data_stream->api->send_request(data_stream, &irq_req);
            state->irq_info.irq_registered = true;
        }
    }
}


