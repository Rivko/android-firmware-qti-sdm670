 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   3. Neither the name of Vishay Capella Microsystems nor the
*     names of its contributors may be used to endorse or promote products
*     derived from this software without specific prior written permission.
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

#include "sns_com_port_types.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_cm32181_hal.h"
#include "sns_cm32181_sensor.h"
#include "sns_cm32181_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_pb_util.h"

#include "sns_std_sensor.pb.h"

#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_std.pb.h"
#include "sns_timer.pb.h"

#include "sns_printf.h"

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
	float sample[CM32181_NUM_AXES];
	/* Data status.*/
	sns_std_sensor_sample_status status;
} cm32181_batch_sample;

/**
 * Encode log sensor state raw packet
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *              the log
 * @param[o] encoded_log Pointer to location where encoded
 *             log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *     during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc cm32181_encode_log_sensor_state_raw(void *log,
										  size_t log_size,
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
	float temp[CM32181_NUM_AXES];
	pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = CM32181_NUM_AXES, .arr_index = &arr_index};

	if (NULL == encoded_log || NULL == log || NULL == bytes_written)
	{
		return SNS_RC_FAILED;
	}

	batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
	batch_sample.sample.arg = &arg;

	if (!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields, &batch_sample))
	{
		return SNS_RC_FAILED;
	}

	pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
	cm32181_batch_sample *raw_sample = (cm32181_batch_sample *)log;

	while (parsed_log_size < log_size &&
		   (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
		   i < (uint32_t)(-1))
	{
		arr_index = 0;
		arg.arr = (float *)raw_sample[i].sample;

		batch_sample.sample_type = raw_sample[i].sample_type;
		batch_sample.status = raw_sample[i].status;
		batch_sample.timestamp = raw_sample[i].timestamp;

		if (!pb_encode_tag(&stream, PB_WT_STRING, sns_diag_sensor_state_raw_sample_tag))
		{
			rc = SNS_RC_FAILED;
			break;
		}
		else if (!pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample))
		{
			rc = SNS_RC_FAILED;
			break;
		}

		parsed_log_size += sizeof(cm32181_batch_sample);
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
 * @param[i] diag     Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *  be allocated. If not provided by setting to 0, will
 *  default to using maximum supported log packet size
 */
void cm32181_log_sensor_state_raw_alloc(log_sensor_state_raw_info *log_raw_info,
									   uint32_t log_size)
{
	uint32_t max_log_size = 0;

	if (NULL == log_raw_info || NULL == log_raw_info->diag ||
		NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
	{
		return;
	}

	// allocate memory for sensor state - raw sensor log packet
	max_log_size = log_raw_info->diag->api->get_max_log_size(log_raw_info->diag);

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

	log_raw_info->log = log_raw_info->diag->api->alloc_log(log_raw_info->diag,
														   log_raw_info->instance,
														   log_raw_info->sensor_uid,
														   log_raw_info->log_size,
														   SNS_DIAG_SENSOR_STATE_LOG_RAW);

	log_raw_info->log_sample_cnt = 0;
	log_raw_info->bytes_written = 0;
}

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *     pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *     of batch
 *  */
void cm32181_log_sensor_state_raw_submit(log_sensor_state_raw_info *log_raw_info,
										bool batch_complete)
{
	cm32181_batch_sample *sample = NULL;

	if (NULL == log_raw_info || NULL == log_raw_info->diag ||
		NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
		NULL == log_raw_info->log)
	{
		return;
	}

	sample = (cm32181_batch_sample *)log_raw_info->log;

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

	log_raw_info->diag->api->submit_log(log_raw_info->diag,
										log_raw_info->instance,
										log_raw_info->sensor_uid,
										log_raw_info->bytes_written,
										log_raw_info->log,
										SNS_DIAG_SENSOR_STATE_LOG_RAW,
										log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
										cm32181_encode_log_sensor_state_raw);

	log_raw_info->log = NULL;
}

/**
 *
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *            pertaining to the sensor
 * @param[i] raw_data   Uncalibrated sensor data to be logged
 * @param[i] timestamp  Timestamp of the sensor data
 * @param[i] status     Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc cm32181_log_sensor_state_raw_add(log_sensor_state_raw_info *log_raw_info,
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

	if ((log_raw_info->bytes_written + sizeof(cm32181_batch_sample)) > log_raw_info->log_size)
	{
		// no more space in log packet
		// submit and allocate a new one
		cm32181_log_sensor_state_raw_submit(log_raw_info, false);
		cm32181_log_sensor_state_raw_alloc(log_raw_info, 0);
	}

	if (NULL == log_raw_info->log)
	{
		rc = SNS_RC_FAILED;
	}
	else
	{
		cm32181_batch_sample *sample = (cm32181_batch_sample *)log_raw_info->log;

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

		log_raw_info->bytes_written += sizeof(cm32181_batch_sample);

		log_raw_info->log_sample_cnt++;
		log_raw_info->batch_sample_cnt++;
	}

	return rc;
}

/**
 * Read wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle    port handle
 * @param[i] reg_addr     register address
 * @param[i] buffer       read buffer
 * @param[i] bytes      bytes to read
 * @param[o] xfer_bytes     bytes read
 *
 * @return sns_rc
 */
static sns_rc cm32181_com_read_wrapper(sns_sync_com_port_service *scp_service,
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

/**
 * Write wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle    port handle
 * @param[i] reg_addr     register address
 * @param[i] buffer       write buffer
 * @param[i] bytes      bytes to write
 * @param[o] xfer_bytes     bytes written
 * @param[i] save_write_time  true to save write transfer time.
 *
 * @return sns_rc
 */
static sns_rc cm32181_com_write_wrapper(sns_sync_com_port_service *scp_service,
										sns_sync_com_port_handle *port_handle,
										uint32_t reg_addr,
										uint8_t *buffer,
										uint32_t bytes,
										uint32_t *xfer_bytes,
										bool save_write_time)
{
	sns_port_vector port_vec;
	port_vec.buffer = buffer;
	port_vec.bytes = bytes;
	port_vec.is_write = true;
	port_vec.reg_addr = reg_addr;

	return scp_service->api->sns_scp_register_rw(port_handle,
												 &port_vec,
												 1,
												 save_write_time,
												 xfer_bytes);
}

/**
 * see sns_cm32181_hal.h
 */
sns_rc cm32181_reset_device(sns_sync_com_port_service *scp_service,
						   sns_sync_com_port_handle *port_handle,
						   cm32181_sensor_type sensor)
{
	UNUSED_VAR(sensor);
	sns_rc rv = SNS_RC_SUCCESS;
	uint16_t conf_reg, psm_reg;
	uint32_t xfer_bytes;

	conf_reg = CM32181_CONF_DEFAULT;// | CM32181_CONF_SD;
	rv = cm32181_com_write_wrapper(scp_service,
								  port_handle,
								  CM32181_REG_CONF,
								  (uint8_t *)&conf_reg,
								  2,
								  &xfer_bytes,
								  false);

    if ((rv != SNS_RC_SUCCESS) || (xfer_bytes != 2))
    {
       return SNS_RC_FAILED;
    }
								  
	psm_reg = CM32181_PSM_DEFAULT;
	rv = cm32181_com_write_wrapper(scp_service,
								  port_handle,
								  CM32181_REG_PSM,
								  (uint8_t *)&psm_reg,
								  2,
								  &xfer_bytes,
								  false);
								  
	if ((rv != SNS_RC_SUCCESS) || (xfer_bytes != 2))
	{
		rv = SNS_RC_FAILED;
	}								  

	return rv;
}

/**
 * see sns_cm32181_hal.h
 */
sns_rc cm32181_get_who_am_i(sns_sync_com_port_service * scp_service,
						   sns_sync_com_port_handle *port_handle,
						   uint8_t *buffer)
{
	sns_rc rv = SNS_RC_SUCCESS;
	uint32_t xfer_bytes;

	rv = cm32181_com_read_wrapper(scp_service,
								 port_handle,
								 CM32181_REG_ID,
								 buffer,
								 2,
								 &xfer_bytes);

	if ((rv != SNS_RC_SUCCESS) || (xfer_bytes != 2))
	{
		rv = SNS_RC_FAILED;
	}

	return rv;
}

void cm32181_start_polling_timer(sns_sensor_instance *this)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;
	sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
	size_t req_len;
	uint8_t buffer[50];
	sns_memset(buffer, 0, sizeof(buffer));
	req_payload.is_periodic = true;
	req_payload.start_time = sns_get_system_time();
	req_payload.timeout_period = state->ambient_light_info.sampling_intvl;	

	req_len = pb_encode_request(buffer,
								sizeof(buffer),
								&req_payload,
								sns_timer_sensor_config_fields,
								NULL);
	if (req_len > 0)
	{
		sns_request timer_req =	{ .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
								  .request_len = req_len,
								  .request = buffer };
		state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
		state->polling_timer_is_active = true;	
	}
}

/**
 * Updates ambient light sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
void cm32181_set_polling_config(sns_sensor_instance *const this)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;
  
	if (state->ambient_light_info.sampling_intvl > 0)
	{
		if (!state->polling_timer_is_active)
		{		
			cm32181_start_polling_timer(this);
		}
	}
	else
	{
		if (state->polling_timer_is_active)
		{
			state->polling_timer_is_active = false;
			sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
		}
	}
}

/** See sns_cm32181_hal.h */
void cm32181_send_config_event(sns_sensor_instance *const instance)
{
	cm32181_instance_state *state = (cm32181_instance_state*)instance->state->state;

	sns_std_sensor_physical_config_event phy_sensor_config = sns_std_sensor_physical_config_event_init_default;

	// TODO: Use appropriate op_mode selected by driver.
	char operating_mode[] = "NORMAL";

	pb_buffer_arg op_mode_args;

	op_mode_args.buf = &operating_mode[0];
	op_mode_args.buf_len = sizeof(operating_mode);

	phy_sensor_config.has_sample_rate = false;
	phy_sensor_config.has_water_mark = false;
	phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
	phy_sensor_config.operation_mode.arg = &op_mode_args;
	phy_sensor_config.has_active_current = true;
	phy_sensor_config.active_current = 35;
	phy_sensor_config.has_resolution = true;
	phy_sensor_config.resolution = CM32181_ALS_RESOLUTION;	
	phy_sensor_config.range_count = 2;
	phy_sensor_config.range[0] = CM32181_ALS_RANGE_MIN;
	phy_sensor_config.range[1] = CM32181_ALS_RANGE_MAX;	
	phy_sensor_config.has_dri_enabled = true;
	phy_sensor_config.dri_enabled = true;
	phy_sensor_config.has_DAE_watermark = false;
	
	pb_send_event(instance,
				  sns_std_sensor_physical_config_event_fields,
				  &phy_sensor_config,
				  sns_get_system_time(),
				  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
				  &state->ambient_light_info.suid);
}

/** See sns_cm32181_hal.h */
void cm32181_handle_ambient_light_sample(sns_sensor_instance *const instance)
{
	cm32181_instance_state *state = (cm32181_instance_state*)instance->state->state;
	sns_diag_service* diag = state->diag_service;
	uint16_t als_data;
	uint32_t xfer_bytes = 0;
	float data[2] = {0};
	log_sensor_state_raw_info log_ambient_light_state_raw_info;

	SNS_INST_PRINTF(MED, instance, "cm32181_handle_ambient_light_sample: Enter");

	/* Read ALS data */
	cm32181_com_read_wrapper(state->scp_service,
							state->com_port_info.port_handle,
							CM32181_REG_ALS_DATA,
							(uint8_t *)&als_data,
							2,
							&xfer_bytes);

	SNS_INST_PRINTF(MED, instance, "cm32181_handle_ambient_light_sample: als_data = %d", als_data);

	// Each stream event contains two output data fields where data is
	// factory calibrated and ordered as:
	// data[0] = ambient light in Lux
	// data[1] = raw ADC value associated with data[0]
	
	// factory calibration
	// Sc = C * (Sr - B)
	// Where,
	// *Sc = Calibrated sample
	// *Sr = Raw sample
	// *C = Scale
	// *B = Bias

	data[0] = als_data * CM32181_LUX_PER_ADC_CODE * state->ambient_light_fac_cal.scale;
	data[1] = als_data;

	pb_send_sensor_stream_event(instance,
								&state->ambient_light_info.suid,
								sns_get_system_time(),
								SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
								SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
								data,
								ARR_SIZE(data),
								state->encoded_ambient_light_event_len);

	// Log raw uncalibrated sensor data
	// Allocate Sensor State Raw log packets for ambient light
	sns_memzero(&log_ambient_light_state_raw_info, sizeof(log_ambient_light_state_raw_info));
	log_ambient_light_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
	log_ambient_light_state_raw_info.diag = diag;
	log_ambient_light_state_raw_info.instance = instance;
	log_ambient_light_state_raw_info.sensor_uid = &state->ambient_light_info.suid;
	cm32181_log_sensor_state_raw_alloc(&log_ambient_light_state_raw_info, 0);

	cm32181_log_sensor_state_raw_add(&log_ambient_light_state_raw_info,
									 data,
									 sns_get_system_time(),
									 SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH);

	cm32181_log_sensor_state_raw_submit(&log_ambient_light_state_raw_info, true);

	SNS_INST_PRINTF(MED, instance, "cm32181_handle_ambient_light_sample: Exit");
}

/**
 * Runs a communication test - verfies WHO_AM_I, publishes self
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
static void cm32181_send_com_test_event(sns_sensor_instance *instance,
									   sns_sensor_uid *uid, bool test_result)
{
	uint8_t data[1] = {0};
	pb_buffer_arg buff_arg = (pb_buffer_arg){ .buf = &data, .buf_len = sizeof(data) };
	sns_physical_sensor_test_event test_event = sns_physical_sensor_test_event_init_default;

	test_event.test_passed = test_result;
	test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
	test_event.test_data.funcs.encode = &pb_encode_string_cb;
	test_event.test_data.arg = &buff_arg;

	pb_send_event(instance,
				  sns_physical_sensor_test_event_fields,
				  &test_event,
				  sns_get_system_time(),
				  SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
				  uid);
}

/** See sns_cm32181_hal.h */
void cm32181_run_self_test(sns_sensor_instance *instance)
{
	cm32181_instance_state *state = (cm32181_instance_state*)instance->state->state;
	sns_rc rv = SNS_RC_SUCCESS;
	uint8_t buffer = 0;
	bool who_am_i_success = false;

	rv = cm32181_get_who_am_i(state->scp_service,
							 state->com_port_info.port_handle,
							 &buffer);
	if (rv == SNS_RC_SUCCESS &&	buffer == CM32181_WHOAMI_VALUE)
	{
		who_am_i_success = true;
	}

	if (state->ambient_light_info.test_info.test_client_present)
	{
		if (state->ambient_light_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
		{
			cm32181_send_com_test_event(instance, &state->ambient_light_info.suid, who_am_i_success);
		}
		else if (state->ambient_light_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
		{
		  // Handle factory test. The driver may choose to reject any new
		  // streaming/self-test requests when factory test is in progress.
		}
		state->ambient_light_info.test_info.test_client_present = false;
	}
}
