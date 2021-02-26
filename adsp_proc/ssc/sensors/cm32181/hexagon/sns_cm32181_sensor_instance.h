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

#pragma once

#include <stdint.h>
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"

#include "sns_sensor_uid.h"

#include "sns_diag_service.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

/** Forward Declaration of Instance API */
sns_sensor_instance_api cm32181_sensor_instance_api;

/** Number of entries in reg_map table. */
#define CM32181_REG_MAP_TABLE_SIZE     (11)

typedef struct cm32181_com_port_info
{
	sns_com_port_config    com_config;
	sns_sync_com_port_handle *port_handle;
} cm32181_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
	float min;
	float max;
} range_attr;

typedef enum
{
	CM32181_AMBIENT_LIGHT  = 0x1
} cm32181_sensor_type;

typedef struct cm32181_self_test_info
{
	sns_physical_sensor_test_type test_type;
	bool test_client_present;
} cm32181_self_test_info;

typedef struct cm32181_ambient_light_info
{
	sns_sensor_uid suid;
	float report_rate_hz;
	float sampling_rate_hz;
	sns_time sampling_intvl;	
	cm32181_self_test_info test_info;
} cm32181_ambient_light_info;

typedef struct cm32181_async_com_port_info
{
	uint32_t port_handle;
}cm32181_async_com_port_info;

typedef struct
{
	float scale;
	float bias;
} cm32181_ambient_light_fac_cal;

/** Private state. */
typedef struct cm32181_instance_state
{
	/** Determines which Sensor data to publish. Uses
	 *  cm32181_sensor_type as bit mask. */
	uint16_t publish_sensors;	
	
	bool polling_timer_is_active;	
	
	/** Ambient Light HW config details */
	cm32181_ambient_light_info ambient_light_info;
	
	/** COM port info */
	cm32181_com_port_info com_port_info;

	/** Data streams from dependencies. */
	sns_data_stream *timer_data_stream;

	sns_std_sensor_config ambient_light_req;

	size_t encoded_ambient_light_event_len;

	// factory calibration
	cm32181_ambient_light_fac_cal ambient_light_fac_cal;

	/**----------debug----------*/
	float ambient_light_stream_event[5];

	sns_diag_service *diag_service;
	sns_sync_com_port_service *scp_service;
	bool instance_is_ready_to_configure;
	bool new_self_test_request;

	size_t log_raw_encoded_size;
} cm32181_instance_state;

typedef struct sns_cm32181_ambient_light_req
{
	float sample_rate;
	float report_rate;
	cm32181_ambient_light_fac_cal ambient_light_fac_cal;	
} sns_cm32181_ambient_light_req;

sns_rc cm32181_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate);
sns_rc cm32181_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request);				 
sns_rc cm32181_inst_deinit(sns_sensor_instance *const this);

