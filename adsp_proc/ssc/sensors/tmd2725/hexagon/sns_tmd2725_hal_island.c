/*
 * Copyright (c) 2018, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sns_com_port_types.h"
#include "sns_event_service.h"
#include "sns_gpio_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_pb_util.h"

#include "sns_std_sensor.pb.h"

#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_timer.pb.h"

#include "sns_cal_util.h"
#include "sns_printf.h"

#include "sns_time.h"

#include "sns_tmd2725_hal.h"
#include "sns_tmd2725_sensor.h"
#include "sns_tmd2725_sensor_instance.h"

#include "sns_stream_service.h"

/* Need to use ODR table. */
extern const tmd2725_odr_reg_map tmd2725_reg_map[TMD2725_REG_MAP_TABLE_SIZE];

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

uint8_t tmd2725_again[] =
{
	1,
	4,
	16,
	64,
	138
};

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
void tmd2725_log_sensor_state_raw_alloc(
                                    log_sensor_state_raw_info *log_raw_info,
                                    uint32_t log_size)
{
	uint32_t max_log_size = 0;

	if (NULL == log_raw_info || NULL == log_raw_info->diag ||
		NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
	{
		return;
	}

	/* Allocate memory for sensor state - raw sensor log packet */
	max_log_size = log_raw_info->diag->api->get_max_log_size(log_raw_info->diag);

	if (0 == log_size)
	{
		/* Log size not specified.
		** Use max supported log packet size */
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
static sns_rc tmd2725_com_read_wrapper(
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

sns_rc tmd2725_com_rw_wrapper(sns_sync_com_port_service *scp_service,
                                    sns_sync_com_port_handle *port_handle,
                                    sns_port_vector *vectors,
                                    int32_t num_vectors,
                                    uint32_t *xfer_bytes)
{
    return scp_service->api->sns_scp_register_rw(port_handle,
        vectors,
        num_vectors,
        false,
        xfer_bytes);
}

/**
 * Read one byte value from register for sensor instance using.
 *
 * @param[i] state     state of sensor instance
 * @param[i] reg       register addr to be read
 * @param[i] val       pointer of the buffer to save value
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc tmd2725_inst_i2c_read_byte(tmd2725_instance_state *state,
                                                   uint8_t reg,
                                                   uint8_t *buffer)
{
	sns_rc rv = SNS_RC_FAILED;
	uint32_t xfer_bytes;
	sns_port_vector port_vec;
	port_vec.buffer = buffer;
	port_vec.bytes = 1;
	port_vec.is_write = false;
	port_vec.reg_addr = reg;

	rv = state->scp_service->api->sns_scp_register_rw(
					state->com_port_info.port_handle,
					&port_vec,
					1,
					false,
					&xfer_bytes);

	if (SNS_RC_SUCCESS != rv || 1 != xfer_bytes)
	{
		rv = SNS_RC_FAILED;
	}

	return rv;
}

/**
 * Write one byte value from register for sensor instance using.
 *
 * @param[i] state     state of sensor instance
 * @param[i] reg       register addr to be read
 * @param[i] val       the value to be written
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc tmd2725_inst_i2c_write_byte(tmd2725_instance_state *state,
                                                     uint8_t reg,
                                                     uint8_t val)
{
	sns_rc rv = SNS_RC_FAILED;
	uint32_t xfer_bytes;
	sns_port_vector port_vec;
	port_vec.buffer = &val;
	port_vec.bytes = 1;
	port_vec.is_write = true;
	port_vec.reg_addr = reg;

	rv = state->scp_service->api->sns_scp_register_rw(
					state->com_port_info.port_handle,
					&port_vec,
					1,
					false,
					&xfer_bytes);

	if (SNS_RC_SUCCESS != rv || 1 != xfer_bytes)
	{
		rv = SNS_RC_FAILED;
	}

	return rv;
}

/**
 * Modify the bits value in register for sensor instance using.
 *
 * @param[i] state	   state of sensor instance 
 * @param[i] reg       register addr to be modified
 * @param[i] mask      mask of the bits which will be modified
 * @param[i] val       the value to be written into the mask bits
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
static sns_rc tmd2725_inst_i2c_modify(tmd2725_instance_state *state,
                                               uint8_t reg,
                                               uint8_t mask,
                                               uint8_t val)
{
	sns_rc rv = SNS_RC_FAILED;

	if (0xFF == mask)
	{
		rv = tmd2725_inst_i2c_write_byte(state, reg, val);
	}
	else
	{
	    uint8_t * rw_buffer = NULL;
        rv = SNS_RC_SUCCESS;
        switch (reg)
        {
            case TMD2725_PERS_REG:
                if (!state->prox_pers_init) 
                {
                    rv = tmd2725_inst_i2c_read_byte(state, reg, &state->prox_pers);
                    state->prox_pers_init = 1;
                } 
                rw_buffer = &state->prox_pers;
                break;

            case TMD2725_INTENAB_REG:
                if (!state->intenab_init) 
                {
                    rv = tmd2725_inst_i2c_read_byte(state, reg, &state->intenab);
                    state->intenab_init = 1;
                } 
                rw_buffer = &state->intenab;
                break;
            
            case TMD2725_ENABLE_REG:
                if (!state->enable_init) 
                {
                    rv = tmd2725_inst_i2c_read_byte(state, reg, &state->enable);
                    state->enable_init = 1;
                } 
                rw_buffer = &state->enable;
                break;
        }
		if (SNS_RC_SUCCESS == rv)
		{
			*rw_buffer &= ~mask;
			*rw_buffer |= (val&mask);
			rv = tmd2725_inst_i2c_write_byte(state, reg, *rw_buffer);
		}
	}

	return rv;
}

sns_rc tmd2725_get_who_am_i(sns_sync_com_port_service *scp_service,
									sns_sync_com_port_handle *port_handle,
									uint8_t *buffer)
{
	sns_rc rv = SNS_RC_FAILED;
	uint32_t xfer_bytes;

	rv = tmd2725_com_read_wrapper(scp_service,
			port_handle,
			TMD2725_ID_REG,
			buffer,
			1,
			&xfer_bytes);

	if (SNS_RC_SUCCESS != rv || 1 != xfer_bytes)
	{
		rv = SNS_RC_FAILED;
	}

	return rv;
}

/**
 * Loads default values in config registers.
 *
 * @param[i] state       state of sensor instance
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
static sns_rc tmd2725_device_set_default(tmd2725_instance_state *state)
{
	sns_rc rv = SNS_RC_FAILED;
    uint32_t xfer_bytes;
    sns_port_vector atime_vec;
    uint8_t atime = ATIME_MS(50);
    atime_vec.buffer = &atime;
    atime_vec.bytes = 1;
    atime_vec.is_write = true;
    atime_vec.reg_addr = TMD2725_ATIME_REG;
    sns_port_vector ptime_vec;
    uint8_t ptime = PTIME_US(2000);
    ptime_vec.buffer = &ptime;
    ptime_vec.bytes = 1;
    ptime_vec.is_write = true;
    ptime_vec.reg_addr = TMD2725_PRATE_REG;
    sns_port_vector wtime_vec;
    uint8_t wtime = WTIME_MS(50);
    wtime_vec.buffer = &wtime;
    wtime_vec.bytes = 1;
    wtime_vec.is_write = true;
    wtime_vec.reg_addr = TMD2725_WTIME_REG;
    sns_port_vector cfg0_vec;
    uint8_t cfg0 = CFG0_RESERVED;
    cfg0_vec.buffer = &cfg0;
    cfg0_vec.bytes = 1;
    cfg0_vec.is_write = true;
    cfg0_vec.reg_addr = TMD2725_CFG0_REG;
    sns_port_vector pcfg0_vec;
    uint8_t pcfg0 = (PPLEN_8US|PPULSES(16));
    pcfg0_vec.buffer = &pcfg0;
    pcfg0_vec.bytes = 1;
    pcfg0_vec.is_write = true;
    pcfg0_vec.reg_addr = TMD2725_PCFG0_REG;
    sns_port_vector pcfg1_vec;
    uint8_t pcfg1 = (PGAIN_2X|PLDRIVE_MA(100));
    pcfg1_vec.buffer = &pcfg1;
    pcfg1_vec.bytes = 1;
    pcfg1_vec.is_write = true;
    pcfg1_vec.reg_addr = TMD2725_PCFG1_REG;
    sns_port_vector cfg1_vec;
    uint8_t cfg1 = AGAIN_16X;
    cfg1_vec.buffer = &cfg1;
    cfg1_vec.bytes = 1;
    cfg1_vec.is_write = true;
    cfg1_vec.reg_addr = TMD2725_CFG1_REG;
    sns_port_vector cfg2_vec;
    uint8_t cfg2 = AGAINL;
    cfg2_vec.buffer = &cfg2;
    cfg2_vec.bytes = 1;
    cfg2_vec.is_write = true;
    cfg2_vec.reg_addr = TMD2725_CFG2_REG;
    sns_port_vector cfg3_vec;
    uint8_t cfg3 = (CFG2_RESERVED2|CFG2_RESERVED1);
    cfg3_vec.buffer = &cfg3;
    cfg3_vec.bytes = 1;
    cfg3_vec.is_write = true;
    cfg3_vec.reg_addr = TMD2725_CFG3_REG;
    sns_port_vector calibcfg_vec;
    uint8_t calibcfg = (PROX_DATA_AVG_4|CALIBCFG_RESERVED|AUTO_OFFSET_ADJ|BINSRCH_TARGET_15);
    calibcfg_vec.buffer = &calibcfg;
    calibcfg_vec.bytes = 1;
    calibcfg_vec.is_write = true;
    calibcfg_vec.reg_addr = TMD2725_CALIBCFG_REG;
    
    sns_port_vector vectors[] =
        {atime_vec, ptime_vec, wtime_vec,
        cfg0_vec, pcfg0_vec, pcfg1_vec,
        cfg1_vec, cfg2_vec, cfg3_vec,
        calibcfg_vec}; 
    
    rv = tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
    if (SNS_RC_SUCCESS != rv)
	{
		return rv;
	}
	return SNS_RC_SUCCESS;
}

sns_rc tmd2725_reset_device(tmd2725_instance_state *state, tmd2725_sensor_type sensor)
{
	sns_rc rv = SNS_RC_FAILED;

	/* Reset Enable reigster only when both ALS and RGB are requested */
	if ((TMD2725_ALS|TMD2725_PROX) == sensor)
	{
    	uint32_t xfer_bytes;
        sns_port_vector enable_vec;
        uint8_t enable_val = 0x00;
        enable_vec.buffer = &enable_val;
        enable_vec.bytes = 1;
        enable_vec.is_write = true;
        enable_vec.reg_addr = TMD2725_ENABLE_REG;
        sns_port_vector intenab_vec;
        uint8_t intenab_val = 0x00;
        intenab_vec.buffer = &intenab_val;
        intenab_vec.bytes = 1;
        intenab_vec.is_write = true;
        intenab_vec.reg_addr = TMD2725_INTENAB_REG;
        sns_port_vector vectors[] = {enable_vec, intenab_vec}; 
        rv = tmd2725_com_rw_wrapper(state->scp_service, 
                    state->com_port_info.port_handle, 
                    vectors, 
                    ARRAY_SIZE(vectors), 
                    &xfer_bytes);
		if (SNS_RC_SUCCESS != rv)
		{
			rv = SNS_RC_FAILED;
			goto reset_exit;
		}
	}

reset_exit:
	if (SNS_RC_SUCCESS == rv)
	{
		rv = tmd2725_device_set_default(state);
	}

	return rv;
}

sns_rc tmd2725_modify_enable(tmd2725_instance_state *state,
                                     uint8_t mask,
                                     uint8_t val)
{
	return tmd2725_inst_i2c_modify(state, TMD2725_ENABLE_REG, mask, val);
}

sns_rc tmd2725_modify_intenab(tmd2725_instance_state *state,
                                      uint8_t mask,
                                      uint8_t val)
{
	return tmd2725_inst_i2c_modify(state, TMD2725_INTENAB_REG, mask, val);
}

sns_rc tmd2725_start_calibration(tmd2725_instance_state *state)
{
	sns_rc rv = SNS_RC_FAILED;
    uint32_t xfer_bytes;
    sns_port_vector status_vec;
    uint8_t status = 0xFF;
    status_vec.buffer = &status;
    status_vec.bytes = 1;
    status_vec.is_write = true;
    status_vec.reg_addr = TMD2725_STATUS_REG;
    sns_port_vector calibcfg_vec;
    uint8_t calibcfg = (BINSRCH_TARGET_15 | CALIBCFG_RESERVED |AUTO_OFFSET_ADJ | PROX_DATA_AVG_4);
    calibcfg_vec.buffer = &calibcfg;
    calibcfg_vec.bytes = 1;
    calibcfg_vec.is_write = true;
    calibcfg_vec.reg_addr = TMD2725_CALIBCFG_REG;
    sns_port_vector calib_vec;
    uint8_t calib = START_OFFSET_CALIB;
    calib_vec.buffer = &calib;
    calib_vec.bytes = 1;
    calib_vec.is_write = true;
    calib_vec.reg_addr = TMD2725_CALIB_REG;    
    sns_port_vector vectors[] = {status_vec, calibcfg_vec, calib_vec}; 
    rv = tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
    if (SNS_RC_SUCCESS != rv)
	{
		return rv;
	}

	return rv;
}

sns_rc tmd2725_get_als_pers(tmd2725_instance_state *state, uint8_t *persistence)
{
	sns_rc rv = SNS_RC_FAILED;

	rv = tmd2725_inst_i2c_read_byte(state, TMD2725_PERS_REG, persistence);
	if (SNS_RC_SUCCESS == rv)
	{
		*persistence &= APERS_MASK;
	}

	return rv;
}

sns_rc tmd2725_set_als_pers(tmd2725_instance_state *state, uint8_t persistence)
{
	return tmd2725_inst_i2c_modify(state, TMD2725_PERS_REG, APERS_MASK, ALS_PERSIST(persistence));
}

sns_rc tmd2725_get_prox_pers(tmd2725_instance_state *state, uint8_t *persistence)
{
	sns_rc rv = SNS_RC_FAILED;

	rv = tmd2725_inst_i2c_read_byte(state, TMD2725_PERS_REG, persistence);
	if (SNS_RC_SUCCESS == rv)
	{
		*persistence &= PPERS_MASK;
		*persistence >>= 4;
	}

	return rv;
}

sns_rc tmd2725_set_prox_pers(tmd2725_instance_state *state, uint8_t persistence)
{
	return tmd2725_inst_i2c_modify(state, TMD2725_PERS_REG, PPERS_MASK, PROX_PERSIST(persistence));
}

sns_rc tmd2725_set_als_gain(tmd2725_instance_state *state, uint32_t again)
{
	sns_rc rv = SNS_RC_FAILED;
	uint8_t cfg1 = 0, cfg2 = 0;

	/* 138X gain is special */
	if (again == 138)
	{
		cfg2 = (AGAINL|GAINMAX);
	}
	else
	{
		cfg2 = AGAINL;
	}

	switch (again)
	{
	case 1:
		cfg1 = AGAIN_1X;
		break;
	case 4:
		cfg1 = AGAIN_4X;
		break;
	case 16:
		cfg1 = AGAIN_16X;
		break;
	case 64:
	case 138:
		cfg1 = AGAIN_64X;
		break;
	default:
		break;
	}
    uint32_t xfer_bytes;
    sns_port_vector cfg1_vec;
    cfg1_vec.buffer = &cfg1;
    cfg1_vec.bytes = 1;
    cfg1_vec.is_write = true;
    cfg1_vec.reg_addr = TMD2725_CFG1_REG;
    sns_port_vector cfg2_vec;
    cfg2_vec.buffer = &cfg2;
    cfg2_vec.bytes = 1;
    cfg2_vec.is_write = true;
    cfg2_vec.reg_addr = TMD2725_CFG2_REG;
    sns_port_vector vectors[] = {cfg1_vec, cfg2_vec}; 
    rv = tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
    if (SNS_RC_SUCCESS != rv)
	{
		return rv;
	}
	rv = tmd2725_set_als_pers(state, 0);
	return rv;
}

sns_rc tmd2725_update_als_threshold(tmd2725_instance_state *state,
                                               uint16_t high_thresh, uint16_t low_thresh)
{
	sns_rc rv = SNS_RC_FAILED;
	uint8_t data[4];

	data[0] = (uint8_t)(low_thresh & 0xFF);
	data[1] = (uint8_t)((low_thresh >> 8) & 0xFF);
	data[2] = (uint8_t)(high_thresh & 0xFF);
	data[3] = (uint8_t)((high_thresh >> 8) & 0xFF);
    uint32_t xfer_bytes;
    sns_port_vector ailtl_vec;
    ailtl_vec.buffer = &data[0];
    ailtl_vec.bytes = 1;
    ailtl_vec.is_write = true;
    ailtl_vec.reg_addr = TMD2725_AILTL_REG;
    sns_port_vector ailth_vec;
    ailth_vec.buffer = &data[1];
    ailth_vec.bytes = 1;
    ailth_vec.is_write = true;
    ailth_vec.reg_addr = TMD2725_AILTH_REG;
    sns_port_vector aihtl_vec;
    aihtl_vec.buffer = &data[2];
    aihtl_vec.bytes = 1;
    aihtl_vec.is_write = true;
    aihtl_vec.reg_addr = TMD2725_AIHTL_REG;
    sns_port_vector aihth_vec;
    aihth_vec.buffer = &data[3];
    aihth_vec.bytes = 1;
    aihth_vec.is_write = true;
    aihth_vec.reg_addr = TMD2725_AIHTH_REG; 
    sns_port_vector vectors[] = {ailtl_vec, ailth_vec, aihtl_vec, aihth_vec}; 
    rv = tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
    if (SNS_RC_SUCCESS != rv)
    {
        return rv;
    }
	return rv;
}

sns_rc tmd2725_update_prox_threshold(tmd2725_instance_state *state,
                                                uint8_t high_thresh, uint8_t low_thresh)
{
	sns_rc rv = SNS_RC_FAILED;
    uint32_t xfer_bytes;
    sns_port_vector pilt_vec;
    pilt_vec.buffer = &low_thresh;
    pilt_vec.bytes = 1;
    pilt_vec.is_write = true;
    pilt_vec.reg_addr = TMD2725_PILT_REG;
    sns_port_vector piht_vec;
    piht_vec.buffer = &high_thresh;
    piht_vec.bytes = 1;
    piht_vec.is_write = true;
    piht_vec.reg_addr = TMD2725_PIHT_REG;
    sns_port_vector vectors[] = {pilt_vec, piht_vec}; 
    rv = tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
	return rv;
}

void tmd2725_get_lux(tmd2725_instance_state *state, bool is_dri)
{
	uint16_t again, again_max;
	float atime;
	uint32_t saturation;
	float lux, lux1, lux2;
	uint8_t data[4] = {0};
	uint8_t buffer = 0;
    uint32_t xfer_bytes;
    uint8_t atime_buf = 0;
    uint8_t cfg1_buf = 0;
    uint8_t cfg2_buf = 0;
    sns_port_vector atime_vec;
    atime_vec.buffer = &atime_buf;
    atime_vec.bytes = 1;
    atime_vec.is_write = false;
    atime_vec.reg_addr = TMD2725_ATIME_REG;
    sns_port_vector cfg1_vec;
    cfg1_vec.buffer = &cfg1_buf;
    cfg1_vec.bytes = 1;
    cfg1_vec.is_write = false;
    cfg1_vec.reg_addr = TMD2725_CFG1_REG;
    sns_port_vector cfg2_vec;
    cfg2_vec.buffer = &cfg2_buf;
    cfg2_vec.bytes = 1;
    cfg2_vec.is_write = false;
    cfg2_vec.reg_addr = TMD2725_CFG2_REG;
    sns_port_vector photopicl_vec;
    photopicl_vec.buffer = &data[0];
    photopicl_vec.bytes = 4;
    photopicl_vec.is_write = false;
    photopicl_vec.reg_addr = TMD2725_PHOTOPICL_REG;
    sns_port_vector vectors[] = {atime_vec, cfg1_vec, cfg2_vec, photopicl_vec}; 
    tmd2725_com_rw_wrapper(state->scp_service, 
        state->com_port_info.port_handle, 
        vectors, 
        ARRAY_SIZE(vectors), 
        &xfer_bytes);
    atime = (float)(atime_buf+1) * ATIME_PER_STEP_X100 / 100.0;
	saturation = ((uint32_t)(atime_buf+1) << 10) - 1;
	saturation = saturation > MAX_ALS_VALUE ? MAX_ALS_VALUE : saturation;
	saturation = saturation * 8 / 10;
    again = cfg1_buf;
	again_max = cfg2_buf;
    if (again_max&GAINMAX)
	{
		again = tmd2725_again[4];
	}
	else
	{
		again = tmd2725_again[again&AGAIN_MASK];
	}
    state->als_info.photopic = *((uint16_t*)&data[0]);
	state->als_info.als_ir = *((uint16_t*)&data[2]);
	if (is_dri)
	{
		/* Set ALS thresholds, and persistence if it is working in dri mdoe */
		uint16_t als_high_thresh, als_low_thresh;
		uint32_t temp;
	
		temp = state->als_info.photopic >> 3;
		if (temp < 3)
		{
			als_low_thresh = (state->als_info.photopic > 3) ? (state->als_info.photopic-3) : 0;
			als_high_thresh = state->als_info.photopic + 3;
		}
		else
		{
			als_low_thresh = state->als_info.photopic - temp;
			temp = state->als_info.photopic + temp;
			als_high_thresh = (temp > saturation) ? saturation : temp;
		}
		tmd2725_update_als_threshold(state, als_high_thresh, als_low_thresh);
	
		tmd2725_get_als_pers(state, &buffer);
		if (!buffer)
		{	
			tmd2725_set_als_pers(state, 1);
			tmd2725_modify_enable(state, AEN, 0);
			tmd2725_modify_enable(state, AEN, AEN);
		}
	}

	if (!state->first_als)
	{
		/* Auto gain */
		if (4 == again && (state->als_info.photopic > saturation || state->als_info.als_ir > saturation))
		{
			tmd2725_set_als_gain(state, 1);
			goto skip_lux_calculation;
		}
		else if ((1 == again && state->als_info.photopic < GAIN_SWITCH_LEVEL)
			|| (16 == again && (state->als_info.photopic > saturation || state->als_info.als_ir > saturation)))
		{
			tmd2725_set_als_gain(state, 4);
			goto skip_lux_calculation;
		}
		else if ((4 == again && state->als_info.photopic < GAIN_SWITCH_LEVEL)
			|| (64 == again && (state->als_info.photopic > saturation || state->als_info.als_ir > saturation)))
		{
			tmd2725_set_als_gain(state, 16);
			goto skip_lux_calculation;
		}
		else if ((16 == again && state->als_info.photopic < GAIN_SWITCH_LEVEL)
			|| (138 == again && (state->als_info.photopic > saturation || state->als_info.als_ir > saturation)))
		{
			tmd2725_set_als_gain(state, 64);
			goto skip_lux_calculation;
		}
		else if (64 == again && state->als_info.photopic < GAIN_SWITCH_LEVEL)
		{
			tmd2725_set_als_gain(state, 138);
			goto skip_lux_calculation;
		}

		/* Ready to reaport Lux */
		state->als_to_report = true;
	}

	if ((1 == again || state->first_als) && state->als_info.photopic > saturation)
	{
		/* Saturated */
		state->als_info.lux = TMD2725_ALS_RANGE_MAX;
		goto skip_lux_calculation;
	}
	else if ((64 == again || state->first_als) && state->als_info.photopic < MIN_ALS_VALUE)
	{
		/* Very dark */
		state->als_info.lux = 0;
		goto skip_lux_calculation;
	}

	/* Calcualte the Lux */
	lux1 = (float)state->als_info.photopic + (float)state->als_info.als_ir * state->als_info.b_coef;
	lux2 = (float)state->als_info.photopic * state->als_info.c_coef + (float)state->als_info.als_ir * state->als_info.d_coef;
	if (state->als_info.use_max)
	{
		lux = lux1 > lux2 ? lux1 : lux2;
	}
	else
	{
		lux = lux1 < lux2 ? lux1 : lux2;
	}
	
	if (lux < 0.0)
	{
		lux = 0.0;
	}
	
	if (lux > ((float)again*atime*100.0))
	{
		lux /= (atime*again);
		lux *= state->als_info.dgf;
	}
	else
	{
		lux *= state->als_info.dgf;
		lux /= (atime*again);
	}
	state->als_info.lux = lux * state->lux_scale;
	/* Reserve one digit after the decimal point */
	state->als_info.lux = (float)((uint32_t)((state->als_info.lux + 0.05) * 10)) / 10.0;

skip_lux_calculation:
	if (state->first_als)
	{
		state->first_als = false;
	}
	return;
}

void tmd2725_insert_sort(uint8_t data[], uint32_t count)  
{  
	int32_t i, j;
	uint8_t temp;
	for (i = 1; i < count; i++)
	{
		temp = data[i];
		j = i-1;

		while(data[j] > temp && j >= 0)
		{
			data[j+1] = data[j];
			j--;
		}

		if (j != (i-1))
		{
			data[j+1] = temp;
		}
	}
}
void tmd2725_get_prox_state(tmd2725_instance_state *state, bool is_dri)
{
	static uint8_t data_count = 0, very_near_count = 0;
	static uint8_t data_buf[TMD2725_CONTAMINATED_COUNT];
	uint8_t pdata[TMD2725_CONTAMINATED_COUNT];
	uint8_t buffer, i, delta;
	bool doCalibration = false;
	
	tmd2725_inst_i2c_read_byte(state, TMD2725_PDATA_REG, &buffer);
	state->prox_info.raw = buffer;

	if (state->first_prox)
	{
		/* First triggered proximity event */
		state->first_prox = false;

		if (state->prox_info.raw > state->near_threshold)
		{
			/* Near by */
			state->prox_info.nearby = SNS_PROXIMITY_EVENT_TYPE_NEAR;
			state->prox_low_thresh = state->far_threshold;
			state->prox_high_thresh = TMD2725_VERY_NEAR_THRESHOLD;
		}
		else
		{
			/* Far away */
			state->prox_info.nearby = SNS_PROXIMITY_EVENT_TYPE_FAR;
			state->prox_low_thresh = 1;
			state->prox_high_thresh = state->near_threshold;
		}

		state->prox_to_report = true;
		data_count = 0;
		very_near_count = 0;
	}
	else
	{
		if (SNS_PROXIMITY_EVENT_TYPE_NEAR == state->prox_info.nearby)
		{
			/* Near by state */
			if (state->prox_info.raw < state->prox_low_thresh)
			{
				/* Will change to Far away state */
				if (state->prox_info.raw < state->far_threshold)
				{
					state->prox_info.nearby = SNS_PROXIMITY_EVENT_TYPE_FAR;
					data_count = 0;
					very_near_count = 0;
					state->prox_to_report = true;
					state->prox_low_thresh = 1;
					state->prox_high_thresh = state->near_threshold;
				}
				else
				{
					/* THRESH_FAR < Raw data < THRESH_CONTAMINATED,
						might have contamination issue, set same high and low threshold to collect raw data */
					state->prox_low_thresh = (state->prox_info.raw + 10) > TMD2725_THRESH_CONTAMINATED ?
									TMD2725_THRESH_CONTAMINATED : (state->prox_info.raw + 10);
					state->prox_high_thresh = state->prox_low_thresh;

					data_buf[data_count%TMD2725_CONTAMINATED_COUNT] = state->prox_info.raw;
					if (++data_count >= TMD2725_CONTAMINATED_COUNT)
					{
						/* Ring buffer is full, copy raw data to temp buffer */
						if ((TMD2725_CONTAMINATED_COUNT*2) == data_count)
						{
							data_count = TMD2725_CONTAMINATED_COUNT;
						}
						for (i = 0; i < TMD2725_CONTAMINATED_COUNT; i++)
						{
							pdata[i] = data_buf[i];
						}
						/* Do data sorting */
						tmd2725_insert_sort(pdata, TMD2725_CONTAMINATED_COUNT);
						/* Get delta, and judge whether it meet contamination issue or not */
						delta = pdata[TMD2725_CONTAMINATED_COUNT-3] - pdata[2];
						if (delta < TMD2725_DATA_JITTER_TH)
						{
							/* Need to do offset calibration, reset ring buffer and flags */
							state->prox_info.nearby = SNS_PROXIMITY_EVENT_TYPE_FAR;
							data_count = 0;
							very_near_count = 0;
							state->prox_to_report = true;
							doCalibration = true;
						}
					}
				}
			}
			else
			{
				/* Judge whether the very near condition is triggered or not */
				if (state->prox_info.raw > TMD2725_VERY_NEAR_THRESHOLD
					&& very_near_count < TMD2725_VERY_NEAR_COUNT)
				{
					if (TMD2725_VERY_NEAR_COUNT == ++very_near_count)
					{
						/* Very near condition is triggered */
						state->prox_low_thresh = TMD2725_THRESH_CONTAMINATED;
						state->prox_high_thresh = TMD2725_PROX_MAX_VALUE;
					}
				}
				else
				{
					/* Raw data is bigger than high thresh, stop to collect
						raw data, and reset the ring buffer */
					state->prox_high_thresh = TMD2725_PROX_MAX_VALUE;
					data_count = 0;
				}
			}

		}
		else
		{
			/* Far away state */
			if (state->prox_info.raw > state->prox_high_thresh)
			{
				/* Change to Near by state */
				state->prox_info.nearby = SNS_PROXIMITY_EVENT_TYPE_NEAR;
				if (1 == state->prox_low_thresh)
				{
					/* It means raw data can be lower than THRESH_FAR */
					state->prox_low_thresh = state->far_threshold;
				}
				else
				{
					/* Otherwise set threshold higher to guarantee the Far away state
						can be triggered*/
					state->prox_low_thresh = (state->prox_high_thresh - 5) > 1 ? (state->prox_high_thresh - 5) : 1;
				}
				state->prox_high_thresh = TMD2725_VERY_NEAR_THRESHOLD;
				state->prox_to_report = true;
			}
			else if (state->prox_info.raw < state->prox_low_thresh)
			{
				/* Might just after offset calibration */
				if (state->prox_info.raw == 0)
				{
					doCalibration = true;
				}
				else if (state->prox_info.raw < state->far_threshold)
				{
					/* Raw data is low enough, set threshold */
					state->prox_low_thresh = 1;
					state->prox_high_thresh = state->near_threshold;
				}
				else
				{
					/* Raw data is still high, set proper threshold to guarantee the Near by and
					Far away events can be triggered, and call offset calibration again */
					state->prox_low_thresh = (state->prox_info.raw - 10) > 1 ? (state->prox_info.raw - 10) : 1;
					state->prox_high_thresh = (state->prox_info.raw + 10) < state->near_threshold ?
									state->near_threshold : (state->prox_info.raw + 10);
				}
			}

		}
	}

	if (doCalibration)
	{
		/* Do calibration again */
		tmd2725_modify_intenab(state, (PIEN|AIEN|CIEN), CIEN);
		tmd2725_modify_enable(state, 0xFF, PON);
		tmd2725_start_calibration(state);
	}
	if (is_dri)
	{
		tmd2725_update_prox_threshold(state, state->prox_high_thresh, state->prox_low_thresh);
		if (!state->prox_pers_init) 
        {
            tmd2725_get_prox_pers(state, &state->prox_pers);
            state->prox_pers_init = 1;
        }
		if (!state->prox_pers)
		{
			tmd2725_set_prox_pers(state, 1);
			state->prox_to_report = true;
		}
	}
	else
	{
		state->prox_to_report = true;
	}
}

void tmd2725_process_sensor_data(sns_sensor_instance *const instance)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;
	uint8_t status = 0, enable = 0, intenab = 0;

	tmd2725_inst_i2c_read_byte(state, TMD2725_STATUS_REG, &status);
	if (0 != status)
	{
		tmd2725_inst_i2c_write_byte(state, TMD2725_STATUS_REG, status);
	}

	SNS_INST_PRINTF(LOW, instance, "TMD2725 process_sensor_data status=0x%x\n", status);
	state->als_to_report = false;
	state->prox_to_report = false;

	if (status&CINT)
	{
		if (state->publish_sensors&TMD2725_ALS)
		{
			enable |= AEN;
		}
		if (state->publish_sensors&TMD2725_PROX)
		{
			enable |= PEN;
		}
		if ((enable&(AEN|PEN)) == PEN)
		{
			enable |= WEN;
		}
		
		if (state->als_registry_cfg.is_dri && (state->publish_sensors&TMD2725_ALS))
		{
			intenab |= AIEN;
		}
		if (state->prox_registry_cfg.is_dri && (state->publish_sensors&TMD2725_PROX))
		{
			intenab |= PIEN;
		}

		if (enable != 0)
		{
			tmd2725_modify_intenab(state, (PIEN|AIEN|CIEN), intenab);
			tmd2725_modify_enable(state, PEN, enable);
			tmd2725_modify_enable(state, (WEN|AEN), enable);
		}
		else
		{
			tmd2725_modify_enable(state, 0xFF, 0);
		}
	}
	
	if (status&AINT)
	{
		tmd2725_get_lux(state, state->als_registry_cfg.is_dri);
	}

	if (status&PINT)
	{
		tmd2725_get_prox_state(state, state->prox_registry_cfg.is_dri);
	}

	return;
}

void tmd2725_start_timer(sns_sensor_instance *const this, sns_data_stream ** timer_stream,
									sns_time timeout_ticks)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)this->state->state;

	sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
	size_t req_len;
	uint8_t buffer[20];
	req_payload.is_periodic = false;
	req_payload.start_time = sns_get_system_time();
	req_payload.timeout_period = timeout_ticks;

	if (NULL == *timer_stream)
	{
		sns_service_manager *smgr = this->cb->get_service_manager(this);
		sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
		stream_svc->api->create_sensor_instance_stream(stream_svc, this, state->timer_suid,
			timer_stream);
	}

	req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
			sns_timer_sensor_config_fields, NULL);
	if (req_len > 0 && NULL != *timer_stream)
	{
		sns_request timer_req =
			{.message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
			.request = buffer, .request_len = req_len};
		(*timer_stream)->api->send_request(*timer_stream, &timer_req);
	}
	else
	{
		SNS_INST_PRINTF(ERROR, this, "TMD2725 timer req encode error");
	}
}

sns_rc tmd2725_als_factory_calibration(sns_sensor_instance *const instance)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;
	sns_rc rv = SNS_RC_SUCCESS;
    state->enable_backup = 0;
    rv = tmd2725_inst_i2c_read_byte(state, TMD2725_ENABLE_REG, &state->enable_backup);
	if (SNS_RC_SUCCESS != rv)
	{
		return rv;
	}

	/* Config to run ALS only */
	rv = tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, PON);
	if (SNS_RC_SUCCESS != rv)
	{
		goto als_fac_cal_exit;
	}
    tmd2725_start_timer(instance, &state->als_timer_data_stream, TMD2725_OFF_TO_IDLE_MS*1000*1000);
als_fac_cal_exit:
    if (SNS_RC_SUCCESS != rv && SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
    {
        rv = SNS_RC_FAILED;
    }
    
	return rv;
}

sns_rc tmd2725_prox_factory_calibration(sns_sensor_instance *const instance)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;
	sns_rc rv = SNS_RC_SUCCESS;
    state->enable_backup = 0;
    rv = tmd2725_inst_i2c_read_byte(state, TMD2725_ENABLE_REG, &state->enable_backup);
	if (SNS_RC_SUCCESS != rv)
	{
        SNS_INST_PRINTF(HIGH, instance, "read_byte failed");
		return rv;
	}

	/* Config before start offset calibration */
	rv = tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, PON);
	if (SNS_RC_SUCCESS != rv)
	{
		goto prox_fac_cal_exit;
	}
    tmd2725_start_timer(instance, &state->prox_timer_data_stream, TMD2725_OFF_TO_IDLE_MS*1000*1000);
prox_fac_cal_exit:
	if (SNS_RC_SUCCESS != rv && SNS_RC_SUCCESS != tmd2725_inst_i2c_write_byte(state, TMD2725_ENABLE_REG, state->enable_backup))
	{
		rv = SNS_RC_FAILED;
	}

	return rv;
}

void tmd2725_send_config_event(sns_sensor_instance *const instance)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;

	sns_std_sensor_physical_config_event phy_sensor_config =
		sns_std_sensor_physical_config_event_init_default;

	char operating_mode[] = "NORMAL";

	pb_buffer_arg op_mode_args;

	op_mode_args.buf = &operating_mode[0];
	op_mode_args.buf_len = sizeof(operating_mode);

	if (state->publish_sensors&TMD2725_ALS)
	{
		phy_sensor_config.has_sample_rate = true;
		phy_sensor_config.sample_rate = 10.0f;
		phy_sensor_config.has_dri_enabled = true;
		if (state->als_registry_cfg.is_dri)
		{
			phy_sensor_config.dri_enabled = true;
		}
		else
		{
			phy_sensor_config.dri_enabled = false;
		}

		phy_sensor_config.has_water_mark = false;
		phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
		phy_sensor_config.operation_mode.arg = &op_mode_args;
		phy_sensor_config.has_active_current = true;
		phy_sensor_config.active_current = TMD2725_ALS_ACTIVE_CURRENT;
		phy_sensor_config.has_resolution = true;
		phy_sensor_config.resolution = TMD2725_ALS_RESOLUTION;
		phy_sensor_config.range_count = 2;
		phy_sensor_config.range[0] = TMD2725_ALS_RANGE_MIN;
		phy_sensor_config.range[1] = TMD2725_ALS_RANGE_MAX;
		phy_sensor_config.has_stream_is_synchronous = false;
		phy_sensor_config.has_DAE_watermark = false;

		pb_send_event(instance,
			sns_std_sensor_physical_config_event_fields,
			&phy_sensor_config,
			sns_get_system_time(),
			SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
			&state->als_info.als_suid);
	}

	if (state->publish_sensors&TMD2725_PROX)
	{
		phy_sensor_config.has_sample_rate = true;
		phy_sensor_config.sample_rate = 10.0f;
		phy_sensor_config.has_dri_enabled = true;
		if (state->prox_registry_cfg.is_dri)
		{
			phy_sensor_config.dri_enabled = true;
		}
		else
		{
			phy_sensor_config.dri_enabled = false;
		}

		phy_sensor_config.has_water_mark = false;
		phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
		phy_sensor_config.operation_mode.arg = &op_mode_args;
		phy_sensor_config.has_active_current = true;
		phy_sensor_config.active_current = TMD2725_PROX_ACTIVE_CURRENT;
		phy_sensor_config.has_resolution = true;
		phy_sensor_config.resolution = TMD2725_PROX_RESOLUTION;
		phy_sensor_config.range_count = 2;
		phy_sensor_config.range[0] = TMD2725_PROX_RANGE_MIN;
		phy_sensor_config.range[1] = TMD2725_PROX_RANGE_MAX;
		phy_sensor_config.has_stream_is_synchronous = false;
		phy_sensor_config.has_DAE_watermark = false;

		pb_send_event(instance,
			sns_std_sensor_physical_config_event_fields,
			&phy_sensor_config,
			sns_get_system_time(),
			SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
			&state->prox_info.prox_suid);
	}
}

void tmd2725_handle_sensor_als_sample(sns_sensor_instance *const instance, uint64_t timeout_ticks)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;

	float data[2];

	data[0] = state->als_info.lux;
	data[1] = (float)state->als_info.photopic;

	pb_send_sensor_stream_event(instance,
		&state->als_info.als_suid,
		(sns_time)timeout_ticks,
		SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
		SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
		data,
		2,
		state->encoded_als_event_len);
}

void tmd2725_handle_sensor_prox_sample(sns_sensor_instance *const instance, uint64_t timeout_ticks)
{
	tmd2725_instance_state *state = (tmd2725_instance_state*)instance->state->state;	
	sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

	state->prox_info.prox_event.raw_adc = (uint32_t)state->prox_info.raw;
	state->prox_info.prox_event.status = status;
	state->prox_info.prox_event.proximity_event_type = 
		(sns_proximity_event_type)state->prox_info.nearby;

	pb_send_event(instance,
		sns_proximity_event_fields,
		&state->prox_info.prox_event,
		timeout_ticks,
		SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
		&state->prox_info.prox_suid);
}
