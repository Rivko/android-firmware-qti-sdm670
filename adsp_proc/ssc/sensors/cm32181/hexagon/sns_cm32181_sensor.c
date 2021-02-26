 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Vishay Capella Microsystems nor the
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

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_cm32181_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"

#include "sns_diag_service.h"
#include "sns_printf.h"

#define CM32181_CONFIG_AMBIENT_LIGHT			"cm32181_0.ambient_light.config"
#define CM32181_PLATFORM_CONFIG					"cm32181_0_platform.config"
#define CM32181_PLATFORM_PLACEMENT				"cm32181_0_platform.placement"
#define CM32181_PLATFORM_FAC_CAL_AMBIENT_LIGHT	"cm32181_0_platform.ambient_light.fac_cal"

/** See sns_cm32181_sensor.h */
void cm32181_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len)
{
    uint8_t buffer[50];
    sns_memset(buffer, 0, sizeof(buffer));
    cm32181_state *state = (cm32181_state*)this->state->state;
    
    SNS_PRINTF(MED, this, "cm32181_send_suid_req: Enter");
    
    sns_service_manager *manager = this->cb->get_service_manager(this);
    sns_stream_service *stream_service =
        (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
    size_t encoded_len;
    pb_buffer_arg data = (pb_buffer_arg){ .buf = data_type, .buf_len = data_type_len };

    sns_suid_req suid_req = sns_suid_req_init_default;
    suid_req.has_register_updates = true;
    suid_req.register_updates = true;
    suid_req.data_type.funcs.encode = &pb_encode_string_cb;
    suid_req.data_type.arg = &data;

    if (state->fw_stream == NULL)
    {
        stream_service->api->create_sensor_stream(stream_service, this,
                                                  sns_get_suid_lookup(), &state->fw_stream);
    }

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
                                    &suid_req, sns_suid_req_fields, NULL);
    if (0 < encoded_len)
    {
        sns_request request = (sns_request){ .request_len = encoded_len,
                                             .request = buffer,
                                             .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
        state->fw_stream->api->send_request(state->fw_stream, &request);
    }
    
    SNS_PRINTF(MED, this, "cm32181_send_suid_req: Exit");
}

/** See sns_cm32181_sensor.h */
void cm32181_process_suid_events(sns_sensor *const this)
{
	cm32181_state *state = (cm32181_state*)this->state->state;

	SNS_PRINTF(MED, this, "cm32181_process_suid_events: Enter");

	for (;
		 0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
		 state->fw_stream->api->get_next_input(state->fw_stream))
	{
		sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);

		if (SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
		{
			pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);
			sns_suid_event suid_event = sns_suid_event_init_default;
			pb_buffer_arg data_type_arg = { .buf = NULL, .buf_len = 0 };
			sns_sensor_uid uid_list;
			sns_suid_search suid_search;
			suid_search.suid = &uid_list;
			suid_search.num_of_suids = 0;

			suid_event.data_type.funcs.decode = &pb_decode_string_cb;
			suid_event.data_type.arg = &data_type_arg;
			suid_event.suid.funcs.decode = &pb_decode_suid_event;
			suid_event.suid.arg = &suid_search;

			if (!pb_decode(&stream, sns_suid_event_fields, &suid_event))
			{
				SNS_PRINTF(ERROR, this, "cm32181_process_suid_events: SUID decode failed");
				continue;
			}

			/* if no suids found, ignore the event */
			if (suid_search.num_of_suids == 0)
			{
				continue;
			}

			/* save suid based on incoming data type name */
			if (0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
			{
				state->timer_suid = uid_list;
			}
			else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
			{
				state->reg_suid = uid_list;
			}
			else
			{
				SNS_PRINTF(ERROR, this, "cm32181_process_suid_events: Unexpected datatype_name");
			}
		}
	}

	SNS_PRINTF(MED, this, "cm32181_process_suid_events: Exit");
}

static void cm32181_sensor_send_registry_request(sns_sensor *const this, char *reg_group_name)
{
	cm32181_state *state = (cm32181_state*)this->state->state;
	uint8_t buffer[100];
	int32_t encoded_len;
	sns_memset(buffer, 0, sizeof(buffer));
	sns_rc rc = SNS_RC_SUCCESS;

	sns_registry_read_req read_request;
	pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name, .buf_len = (strlen(reg_group_name) + 1) };

	read_request.name.arg = &data;
	read_request.name.funcs.encode = pb_encode_string_cb;

	encoded_len = pb_encode_request(buffer, sizeof(buffer),	&read_request, sns_registry_read_req_fields, NULL);
	if (0 < encoded_len)
	{
		sns_request request = { .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ,
								.request_len = encoded_len,
								.request = buffer };
		rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
	}

	//SENSOR_PRINTF_LOW_FULL(this, "Sending registry request for group name:%s", reg_group_name);
}

static void cm32181_sensor_process_registry_event(sns_sensor *const this,
                                                  sns_sensor_event *event)
{
	bool rv = true;
	cm32181_state *state = (cm32181_state*)this->state->state;

	SNS_PRINTF(MED, this, "cm32181_sensor_process_registry_event: Enter");	
	
	pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);

	if (SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
	{
		sns_registry_read_event read_event = sns_registry_read_event_init_default;
		pb_buffer_arg group_name = {0,0};
		read_event.name.arg = &group_name;
		read_event.name.funcs.decode = pb_decode_string_cb;

		if (!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
		{
			SNS_PRINTF(ERROR, this, "cm32181_sensor_process_registry_event: Error decoding registry event");
		}
		else
		{
			stream = pb_istream_from_buffer((void*)event->event, event->event_len);

			if (0 == strncmp((char*)group_name.buf, CM32181_CONFIG_AMBIENT_LIGHT, group_name.buf_len))
			{
				{
					sns_registry_decode_arg arg =
					{
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] =
						{
							.group_name = "config",
							.parse_func = sns_registry_parse_phy_sensor_cfg,
							.parsed_buffer = &state->registry_cfg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->registry_cfg_received = true;
					state->is_dri = state->registry_cfg.is_dri;
					state->hardware_id = state->registry_cfg.hw_id;
					state->resolution_idx = state->registry_cfg.res_idx;
					state->supports_sync_stream = state->registry_cfg.sync_stream;

					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: Registry group cm32181_0.ambient_light.config is received");					
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: is_dri = %d, hardware_id = %lld",
							   state->is_dri,
							   state->hardware_id);
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: resolution_idx = %d, supports_sync_stream = %d",
							   state->resolution_idx,
							   state->supports_sync_stream);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, CM32181_PLATFORM_CONFIG, group_name.buf_len))
			{
				{
					sns_registry_decode_arg arg =
					{
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] =
						{
							.group_name = "config",
							.parse_func = sns_registry_parse_phy_sensor_pf_cfg,
							.parsed_buffer = &state->registry_pf_cfg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->registry_pf_cfg_received = true;

					state->com_port_info.com_config.bus_type = state->registry_pf_cfg.bus_type;
					state->com_port_info.com_config.bus_instance = state->registry_pf_cfg.bus_instance;
					state->com_port_info.com_config.slave_control = state->registry_pf_cfg.slave_config;
					state->com_port_info.com_config.min_bus_speed_KHz = state->registry_pf_cfg.min_bus_speed_khz;
					state->com_port_info.com_config.max_bus_speed_KHz = state->registry_pf_cfg.max_bus_speed_khz;
					state->com_port_info.com_config.reg_addr_type = state->registry_pf_cfg.reg_addr_type;			
					state->rail_config.num_of_rails = state->registry_pf_cfg.num_rail;
					state->registry_rail_on_state = state->registry_pf_cfg.rail_on_state;
					sns_strlcpy(state->rail_config.rails[0].name,
								state->registry_pf_cfg.vddio_rail,
								sizeof(state->rail_config.rails[0].name));
					sns_strlcpy(state->rail_config.rails[1].name,
								state->registry_pf_cfg.vdd_rail,
								sizeof(state->rail_config.rails[1].name));

					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: Registry group cm32181_0_platform.config is received");
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: bus_type = %d, bus_instance = %d, slave_control = %d",					
							   state->com_port_info.com_config.bus_type,
							   state->com_port_info.com_config.bus_instance,
							   state->com_port_info.com_config.slave_control);

					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: min_bus_speed_KHz = %d, max_bus_speed_KHz = %d, reg_addr_type = %d",
							   state->com_port_info.com_config.min_bus_speed_KHz,
							   state->com_port_info.com_config.max_bus_speed_KHz,
							   state->com_port_info.com_config.reg_addr_type);

					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: rigid body type = %d",
							   state->registry_pf_cfg.rigid_body_type);
				}
			}
			else if (0 == strncmp((char*)group_name.buf, CM32181_PLATFORM_PLACEMENT, group_name.buf_len))
			{
				{
					uint8_t arr_index = 0;
					pb_float_arr_arg arr_arg =
					{
						.arr = state->placement,
						.arr_index = &arr_index,
						.arr_len = 12
					};

					sns_registry_decode_arg arg =
					{
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] =
						{
							.group_name = "placement",
							.parse_func = sns_registry_parse_float_arr,
							.parsed_buffer = &arr_arg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->registry_placement_received = true;
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: Registry group cm32181_0_platform.placement is received");					
				}
			}
			else if (0 == strncmp((char*)group_name.buf, CM32181_PLATFORM_FAC_CAL_AMBIENT_LIGHT, group_name.buf_len))
			{
				{
					uint8_t arr_index = 0;
					pb_float_arr_arg arr_arg =
					{
						.arr = (float *)&state->ambient_light_fac_cal,
						.arr_index = &arr_index,
						.arr_len = 6
					};

					sns_registry_decode_arg arg =
					{
						.item_group_name = &group_name,
						.parse_info_len = 1,
						.parse_info[0] =
						{
							.group_name = "fac_cal",
							.parse_func = sns_registry_parse_float_arr,
							.parsed_buffer = &arr_arg
						}
					};

					read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
					read_event.data.items.arg = &arg;

					rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
				}

				if (rv)
				{
					state->registry_fac_cal_received = true;
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: Registry group cm32181_0_platform.ambient_light.fac_cal is received");					
					SNS_PRINTF(LOW, this, "cm32181_sensor_process_registry_event: scale = %d, bias = %d",
							   (int32_t)(state->ambient_light_fac_cal.scale),
							   (int32_t)(state->ambient_light_fac_cal.bias));					
				}
			}
			else
			{
				rv = false;
			}

			if (!rv)
			{
				//SENSOR_PRINTF_ERROR_FULL(this, "Error decoding registry group %s due to %s", (char*)group_name.buf,
				//                         PB_GET_ERROR(&stream));
			}
		}
	}
	else
	{
		SNS_PRINTF(ERROR, this, "cm32181_sensor_process_registry_event: Received unsupported registry event msg id %u", event->message_id);
	}
	
	SNS_PRINTF(MED, this, "cm32181_sensor_process_registry_event: Exit");
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void cm32181_publish_registry_attributes(sns_sensor *const this)
{
	cm32181_state *state = (cm32181_state*)this->state->state;
	
	SNS_PRINTF(MED, this, "cm32181_publish_registry_attributes: Enter");
	
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = state->is_dri;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = state->supports_sync_stream;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = state->hardware_id;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
			SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
		for (uint8_t i = 0; i < 12; i++)
		{
			values[i].has_flt = true;
			values[i].flt = state->placement[i];
		}
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = state->registry_pf_cfg.rigid_body_type;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
	}
	
	SNS_PRINTF(MED, this, "cm32181_publish_registry_attributes: Exit");	
}

void cm32181_start_power_rail_timer(sns_sensor *const this,
								   sns_time timeout_ticks,
								   cm32181_power_rail_pending_state pwr_rail_pend_state)
{
	cm32181_state *state = (cm32181_state*)this->state->state;

	SNS_PRINTF(MED, this, "cm32181_start_power_rail_timer: Enter");  	
	
	sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
	size_t req_len;
	uint8_t buffer[20];
	sns_memset(buffer, 0, sizeof(buffer));
	req_payload.is_periodic = false;
	req_payload.start_time = sns_get_system_time();
	req_payload.timeout_period = timeout_ticks;

	req_len = pb_encode_request(buffer,
								sizeof(buffer),
								&req_payload,
								sns_timer_sensor_config_fields, NULL);
	if (req_len > 0)
	{
		sns_request timer_req =	{ .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
								  .request_len = req_len,
								  .request = buffer };
		state->timer_stream->api->send_request(state->timer_stream, &timer_req);
		state->power_rail_pend_state = pwr_rail_pend_state;
	}
	else
	{
		SNS_PRINTF(ERROR, this, "cm32181_start_power_rail_timer: Timer req encode error");
	}
	
	SNS_PRINTF(MED, this, "cm32181_start_power_rail_timer: Exit"); 	
}

bool cm32181_discover_hw(sns_sensor *const this)
{
	uint16_t buffer = 0x0;
	bool hw_is_present = false;
	cm32181_state *state = (cm32181_state*)this->state->state;
	sns_rc rv = SNS_RC_SUCCESS;

	/**-------------------Read and Confirm WHO-AM-I------------------------*/
	rv = cm32181_get_who_am_i(state->scp_service,state->com_port_info.port_handle,
							  (uint8_t *)&buffer);
	if ((rv == SNS_RC_SUCCESS) && ((buffer & 0xFF) == CM32181_WHOAMI_VALUE))
	{
		rv = cm32181_reset_device(state->scp_service,state->com_port_info.port_handle,
								  CM32181_AMBIENT_LIGHT);

		if (rv == SNS_RC_SUCCESS)
		{
			SNS_PRINTF(LOW, this, "cm32181_discover_hw: CM32181 HW present");			
			hw_is_present = true;
		}
	}

	state->who_am_i = (buffer & 0xFF);

	/**------------------Power Down and Close COM Port--------------------*/
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
													  false);

	state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
	state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);

	/**----------------------Turn Power Rail OFF--------------------------*/
	state->rail_config.rail_vote = SNS_RAIL_OFF;
	state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
															 this,
															 &state->rail_config,
															 NULL);

	return hw_is_present;  
}

static void cm32181_publish_available(sns_sensor *const this)
{
	sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
	value.has_boolean = true;
	value.boolean = true;
	sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void cm32181_set_self_test_inst_config(sns_sensor *this, sns_sensor_instance *instance)
{
	sns_request config;

	config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
	config.request_len = 0;
	config.request = NULL;

	this->instance_api->set_client_config(instance, &config);
}

/** See sns_cm32181_sensor.h*/
sns_rc cm32181_sensor_notify_event(sns_sensor *const this)
{
	cm32181_state *state = (cm32181_state*)this->state->state;
	sns_service_manager *service_mgr = this->cb->get_service_manager(this);
	sns_stream_service *stream_svc = (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
	sns_time on_timestamp;
	sns_rc rv = SNS_RC_SUCCESS;
	sns_sensor_event *event;

	SNS_PRINTF(MED, this, "cm32181_sensor_notify_event: Enter");

	if (state->fw_stream)
	{
		if ((0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))) ||	
			(0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
		{
			cm32181_process_suid_events(this);
			
			// place a request to registry sensor
			if ((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
			{
				if (state->reg_data_stream == NULL)
				{
					stream_svc->api->create_sensor_stream(stream_svc, this, state->reg_suid , &state->reg_data_stream);

					cm32181_sensor_send_registry_request(this, CM32181_PLATFORM_CONFIG);
					cm32181_sensor_send_registry_request(this, CM32181_PLATFORM_PLACEMENT);

					if (CM32181_AMBIENT_LIGHT == state->sensor)
					{
						cm32181_sensor_send_registry_request(this, CM32181_CONFIG_AMBIENT_LIGHT);
						cm32181_sensor_send_registry_request(this, CM32181_PLATFORM_FAC_CAL_AMBIENT_LIGHT);						
					}
				}
			}			
		}
	}

	/**----------------------Handle a Timer Sensor event.-------------------*/
	if (NULL != state->timer_stream)
	{
		event = state->timer_stream->api->peek_input(state->timer_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
			sns_timer_sensor_event timer_event;
			if (event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
			{
				if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
				{
					if (state->power_rail_pend_state == CM32181_POWER_RAIL_PENDING_INIT)
					{
                        state->hw_is_present = cm32181_discover_hw(this);
						
						if (state->hw_is_present)
						{
							cm32181_publish_available(this);
						}
						else
						{
							rv = SNS_RC_INVALID_STATE;
							SNS_PRINTF(LOW, this, "cm32181_sensor_notify_event: CM32181 HW absent");
						}
						state->power_rail_pend_state = CM32181_POWER_RAIL_PENDING_NONE;
					}
					else if (state->power_rail_pend_state == CM32181_POWER_RAIL_PENDING_SET_CLIENT_REQ)
					{
						sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
						cm32181_instance_state *inst_state = (cm32181_instance_state*) instance->state->state;
						if (NULL != instance)
						{
							inst_state->instance_is_ready_to_configure = true;
							cm32181_reval_instance_config(this, instance, state->sensor);

							if (inst_state->new_self_test_request)
							{
								cm32181_set_self_test_inst_config(this, instance);
							}			   
						}
						state->power_rail_pend_state = CM32181_POWER_RAIL_PENDING_NONE;
					}
				}
				else
				{
					SNS_PRINTF(ERROR, this, "cm32181_sensor_notify_event: pb_decode error");
				}
			}
			event = state->timer_stream->api->get_next_input(state->timer_stream);
		}
	}

	if (NULL != state->reg_data_stream)
	{
		event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
		while (NULL != event)
		{
			cm32181_sensor_process_registry_event(this, event);

			if (state->registry_cfg_received && state->registry_placement_received)
			{
				cm32181_publish_registry_attributes(this);
			}

			event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
		}
	}
  
	if (0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)) &&
		state->registry_pf_cfg_received && state->registry_cfg_received && state->registry_placement_received)
	{
		state->registry_pf_cfg_received = false;
	  
		/**-----------------Register and Open COM Port-------------------------*/
		if (NULL == state->com_port_info.port_handle)
		{
			rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
																	&state->com_port_info.port_handle);

			if (rv == SNS_RC_SUCCESS)
			{
				state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
			}
		}

		/**---------------------Register Power Rails --------------------------*/
		if (0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)) &&
			NULL == state->pwr_rail_service &&
			rv == SNS_RC_SUCCESS)
		{
			state->rail_config.rail_vote = SNS_RAIL_OFF;

			state->pwr_rail_service =
			  (sns_pwr_rail_service*)service_mgr->get_service(service_mgr, SNS_POWER_RAIL_SERVICE);

			state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service, &state->rail_config);

			/**---------------------Turn Power Rails ON----------------------------*/
			state->rail_config.rail_vote = state->registry_rail_on_state;
			state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
																	 this,
																	 &state->rail_config,
																	 &on_timestamp);

			/**-------------Create a Timer stream for Power Rail ON timeout.---------*/
			if (NULL == state->timer_stream)
			{
				stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid, &state->timer_stream);
				if (NULL != state->timer_stream)
				{
					cm32181_start_power_rail_timer(this,
												  sns_convert_ns_to_ticks(CM32181_OFF_TO_IDLE_MS * 1000 * 1000),
												  CM32181_POWER_RAIL_PENDING_INIT);
				}
			}
		}
	}

	SNS_PRINTF(MED, this, "cm32181_sensor_notify_event: Exit");

	return rv;
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as stored in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool cm32181_get_decoded_ambient_light_request(sns_sensor const *this, sns_request const *in_request,
										  sns_std_request *decoded_request,
										  sns_std_sensor_config *decoded_payload)
{
    pb_istream_t stream;
    pb_simple_cb_arg arg = { .decoded_struct = decoded_payload, .fields = sns_std_sensor_config_fields };
    decoded_request->payload = (struct pb_callback_s){ .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(in_request->request, in_request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        SNS_PRINTF(ERROR, this, "cm32181_get_decoded_ambient_light_request: Decode error");
        return false;
    }
	
    return true;
}

static void cm32181_get_ambient_light_config(sns_sensor *this,
                                 sns_sensor_instance *instance,
                                 float *chosen_sample_rate,
                                 float *chosen_report_rate,
                                 bool *sensor_ambient_light_client_present)
{
    cm32181_instance_state *inst_state = (cm32181_instance_state*)instance->state->state;
    sns_sensor_uid suid = AMBIENT_LIGHT_SUID;
    sns_request const *request;

    SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: Enter");	
	
    sns_memscpy(&inst_state->ambient_light_info.suid,
                sizeof(inst_state->ambient_light_info.suid),
                &suid,
                sizeof(suid));

    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *sensor_ambient_light_client_present = false;

    /** Parse through existing requests and get fastest sample
    *  rate and report rate requests. */
    for (request = instance->cb->get_client_request(instance, &suid, true);
         NULL != request;
         request = instance->cb->get_client_request(instance, &suid, false))
    {
		SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: request->message_id = %d", request->message_id);			

		if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
		{
			SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG");

			sns_std_request decoded_request;
			sns_std_sensor_config decoded_payload = {0};
			
			if (cm32181_get_decoded_ambient_light_request(this, request, &decoded_request, &decoded_payload))
			{
				SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: chosen_sample_rate = %d, decoded_payload.sample_rate = %d",
						   (int32_t)*chosen_sample_rate,
						   (int32_t)decoded_payload.sample_rate);				
				
				*chosen_sample_rate = SNS_MAX(*chosen_sample_rate, decoded_payload.sample_rate);
				*chosen_report_rate = *chosen_sample_rate;
				*sensor_ambient_light_client_present = true;
			}
		}		
		else if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
		{
			SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG");

			*sensor_ambient_light_client_present = true;
		}
    }
	inst_state->ambient_light_info.report_rate_hz = *chosen_report_rate;
	inst_state->ambient_light_info.sampling_rate_hz = *chosen_sample_rate;
	
    SNS_PRINTF(MED, this, "cm32181_get_ambient_light_config: Exit");		
}

static void cm32181_set_inst_config(sns_sensor *this,
									sns_sensor_instance *instance,
									float chosen_report_rate,
									float chosen_sample_rate,
									uint32_t message_id)                                        
{   
    sns_cm32181_ambient_light_req new_client_config;
    cm32181_state *state = (cm32181_state*)this->state->state;	
    sns_request config;

    SNS_PRINTF(MED, this, "cm32181_set_inst_config: Enter");	

    SNS_PRINTF(MED, this, "cm32181_set_inst_config: chosen_report_rate = %d, chosen_sample_rate = %d",
			   (int32_t)(chosen_report_rate),
			   (int32_t)(chosen_sample_rate));
	
    new_client_config.report_rate = chosen_report_rate;
    new_client_config.sample_rate = chosen_sample_rate;
    new_client_config.ambient_light_fac_cal = state->ambient_light_fac_cal;
	
    config.message_id = message_id;
    config.request_len = sizeof(sns_cm32181_ambient_light_req);
    config.request = &new_client_config;

    this->instance_api->set_client_config(instance, &config);
    
    SNS_PRINTF(MED, this, "cm32181_set_inst_config: Exit");    
}

static void cm32181_turn_rails_off(sns_sensor *this)
{
    sns_sensor *sensor;

    for (sensor = this->cb->get_library_sensor(this, true);
         NULL != sensor;
         sensor = this->cb->get_library_sensor(this, false))
    {
        cm32181_state *sensor_state = (cm32181_state*)sensor->state->state;
        if (sensor_state->rail_config.rail_vote != SNS_RAIL_OFF)
        {
            sensor_state->rail_config.rail_vote = SNS_RAIL_OFF;
            sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                                                                            sensor,
                                                                            &sensor_state->rail_config,
                                                                            NULL);
        }
    }
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool cm32181_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
												 sns_std_request *decoded_request,
												 sns_physical_sensor_test_config *test_config)
{
	pb_istream_t stream;
	pb_simple_cb_arg arg = { .decoded_struct = test_config, .fields = sns_physical_sensor_test_config_fields };
	decoded_request->payload = (struct pb_callback_s){ .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
	stream = pb_istream_from_buffer(request->request, request->request_len);
	if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
	{
		SNS_PRINTF(ERROR, this, "cm32181_get_decoded_self_test_request: Decode error");
		return false;
	}
	return true;
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
static bool cm32181_extract_self_test_info(sns_sensor *this,
										  sns_sensor_instance *instance,
										  struct sns_request const *new_request)
{
	sns_std_request decoded_request;
	sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
	cm32181_state *state = (cm32181_state*)this->state->state;
	cm32181_instance_state *inst_state = (cm32181_instance_state*)instance->state->state;
	cm32181_self_test_info *self_test_info;

	if (state->sensor == CM32181_AMBIENT_LIGHT)
	{
		self_test_info = &inst_state->ambient_light_info.test_info;
	}
	else
	{
		return false;
	}

	if (cm32181_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
	{
		self_test_info->test_type = test_config.test_type;
		self_test_info->test_client_present = true;
		return true;
	}
	else
	{
		return false;
	}
}

void cm32181_reval_instance_config(sns_sensor *this,
								   sns_sensor_instance *instance,
								   cm32181_sensor_type sensor_type)
{
    UNUSED_VAR(sensor_type);  
    float sample_rate = 0;
    float report_rate = 0;
    bool sensor_ambient_light_client_present = false;
    cm32181_instance_state *inst_state = (cm32181_instance_state*)instance->state->state; 

    SNS_PRINTF(MED, this, "cm32181_reval_instance_config: Enter");	
	
    cm32181_get_ambient_light_config(this,
									 instance,
									 &sample_rate,
									 &report_rate,
									 &sensor_ambient_light_client_present);

	/** Start with a clean slate */
	inst_state->publish_sensors = 0;						 
						 
	if (sensor_ambient_light_client_present)
	{
		inst_state->publish_sensors = inst_state->publish_sensors | CM32181_AMBIENT_LIGHT;
		SNS_PRINTF(MED, this, "cm32181_reval_instance_config: sensor_ambient_light_client_present = TRUE");
	}						 
						 
    cm32181_set_inst_config(this,
							instance,
							report_rate,
							sample_rate,
							SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
                              
    if (!sensor_ambient_light_client_present)
    {
        cm32181_turn_rails_off(this);
        inst_state->instance_is_ready_to_configure = false;
    }
	
    SNS_PRINTF(MED, this, "cm32181_reval_instance_config: Exit");	
}

/** See sns_cm32181_sensor.h */
sns_sensor_instance* cm32181_set_client_request(sns_sensor *const this,
											   struct sns_request const *exist_request,
											   struct sns_request const *new_request,
											   bool remove)
{
    sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
    cm32181_state *state = (cm32181_state*)this->state->state;    
    sns_time on_timestamp;
    sns_time delta;
    bool reval_config = false;

	SNS_PRINTF(MED, this, "cm32181_set_client_request: Enter");   
        
    if (remove)
    {
        if (NULL != instance)
        {
			SNS_PRINTF(MED, this, "cm32181_set_client_request: Remove client request"); 
			
            instance->cb->remove_client_request(instance, exist_request);
            /* Assumption: The FW will call deinit() on the instance before destroying it.
                         Putting all HW resources (sensor HW, COM port, power rail) in
                         low power state happens in Instance deinit().*/

            cm32181_reval_instance_config(this, instance, state->sensor);
        }
    }
    else
    {
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
            state->rail_config.rail_vote = state->registry_rail_on_state;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     &on_timestamp);

            delta = sns_get_system_time() - on_timestamp;

            // Use on_timestamp to determine correct Timer value.
            if (delta < sns_convert_ns_to_ticks(CM32181_OFF_TO_IDLE_MS * 1000 * 1000))
            {
                cm32181_start_power_rail_timer(this,
											   sns_convert_ns_to_ticks(CM32181_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
											   CM32181_POWER_RAIL_PENDING_SET_CLIENT_REQ);
            }
            else
            {
                // rail is already ON
                reval_config = true;
            }

            /** create_instance() calls init() for the Sensor Instance */
            instance = this->cb->create_instance(this, sizeof(cm32181_instance_state));
            /* If rail is already ON then flag instance OK to configure */
            if (reval_config)
            {
                cm32181_instance_state *inst_state = (cm32181_instance_state*)instance->state->state;

                inst_state->instance_is_ready_to_configure = true;
            }
        }
        else
        {
            if (0)  // flush_req
            {
                // TODO Flush FIFO samples.

                instance = NULL;
            }
            else
            {
                reval_config = true;

                /** An existing client is changing request*/
                if ((NULL != exist_request) && (NULL != new_request))
                {
					SNS_PRINTF(MED, this, "cm32181_set_client_request: An existing client is changing request");
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
            cm32181_instance_state *inst_state = (cm32181_instance_state*)instance->state->state;
            if (NULL != new_request)
            {
				SNS_PRINTF(MED, this, "cm32181_set_client_request: Add the new request to list of client_requests");
				instance->cb->add_client_request(instance, new_request);

				if (new_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
				{
					if (cm32181_extract_self_test_info(this, instance, new_request))
					{
						inst_state->new_self_test_request = true;
					}
				}				
            }
            if (reval_config && inst_state->instance_is_ready_to_configure)
            {
				cm32181_reval_instance_config(this, instance, state->sensor);

				if (inst_state->new_self_test_request)
				{
					cm32181_set_self_test_inst_config(this, instance);
				}				
            }
        }
    }

    if (NULL != instance &&
		NULL == instance->cb->get_client_request(instance, &(sns_sensor_uid)AMBIENT_LIGHT_SUID, true))
    {
		this->cb->remove_instance(instance);
		cm32181_turn_rails_off(this);		
    }

    SNS_PRINTF(MED, this, "cm32181_set_client_request: Exit");	
	
    return instance;
}

