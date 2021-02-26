#pragma once
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
#include "sns_data_stream.h"
#include "sns_island_service.h"
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_diag_service.h"
#include "sns_math_util.h"
#include "sns_registry_util.h"

#include "sns_tmd2725_sensor_instance.h"
#include "sns_tmd2725_hal.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define TMD2725_DRIVER_VERSION         0x0101

#define TMD2725_ALS_SUID \
	{  \
		.sensor_uid =  \
		{  \
			0x61, 0x6D, 0x73, 0x54, 0x4D, 0x44, 0x32, 0x37,  \
			0x32, 0x35, 0x41, 0x4C, 0x53, 0x5F, 0x5F, 0x5F  \
		}  \
	}

#define TMD2725_PROX_SUID \
	{  \
		.sensor_uid =  \
		{  \
			0x61, 0x6D, 0x73, 0x54, 0x4D, 0x44, 0x32, 0x37,  \
			0x32, 0x35, 0x50, 0x52, 0x4F, 0x58, 0x5F, 0x5F  \
		}  \
	}

#define ALS_FAC_CAL_NUM                2
#define ALS_EVENT_SIZE                 2
#define PROX_EVENT_SIZE                2

/* TMD2725 ODR (Hz) definitions */
#define TMD2725_ODR_2                  2.0f
#define TMD2725_ODR_5                  5.0f
#define TMD2725_ODR_10                 10.0f
#define TMD2725_ODR_15                 15.0f
#define TMD2725_ODR_20                 20.0f
#define TMD2725_ODR_50                 50.0f
#define TMD2725_ODR_100                100.0f

/* TMD2725 ALS ranges */
#define TMD2725_ALS_RANGE_MIN          0.0f
#define TMD2725_ALS_RANGE_MAX          65535.0f

/* TMD2725 ALS resolutions */
#define TMD2725_ALS_RESOLUTION         0.1f

/* TMD2725 ALS power */
#define TMD2725_ALS_ACTIVE_CURRENT     80
#define TMD2725_ALS_SLEEP_CURRENT      1

/* TMD2725 PROX ranges */
#define TMD2725_PROX_RANGE_MIN         0.0f
#define TMD2725_PROX_RANGE_MAX         5.0f

/* TMD2725 PROX resolutions */
#define TMD2725_PROX_RESOLUTION        1.0f

/* TMD2725 PROX power */
#define TMD2725_PROX_ACTIVE_CURRENT    1000
#define TMD2725_PROX_SLEEP_CURRENT     1

/* Supported operating modes */
#define TMD2725_LPM                    "LPM"
#define TMD2725_NORMAL                 "NORMAL"

/* Forward Declaration of Ambient Light Sensor API */
sns_sensor_api tmd2725_als_sensor_api;

/* Forward Declaration of Proximity Sensor API */
sns_sensor_api tmd2725_prox_sensor_api;

/* Power rail timeout States for the TMD2725 Sensors.*/
typedef enum
{
	TMD2725_POWER_RAIL_PENDING_NONE,
	TMD2725_POWER_RAIL_PENDING_INIT,
	TMD2725_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} tmd2725_power_rail_pending_state;

typedef struct tmd2725_pb_custom_float_parse_arg
{
	float *data_array;
	uint32_t *version_array;
	uint8_t *index;
	uint8_t size;
} tmd2725_pb_custom_float_parse_arg;

/* Interrupt Sensor State. */
typedef struct tmd2725_common_state
{
	sns_sensor_uid          reg_suid;
	sns_sensor_uid          irq_suid;
	sns_sensor_uid          timer_suid;

	tmd2725_com_port_info   com_port_info;
	sns_interrupt_req       irq_config;

	sns_rail_config         rail_config;
	sns_power_rail_state    registry_rail_on_state;

	bool                    start_hw_detect;
	bool                    hw_is_present;
	uint16_t                who_am_i;

	/* Registry sensor config */
	bool                    als_registry_cfg_received;
	bool                    prox_registry_cfg_received;
	sns_registry_phy_sensor_cfg als_registry_cfg;
	sns_registry_phy_sensor_cfg prox_registry_cfg;

	/* Registry sensor platform config */
	bool                    registry_pf_cfg_received;
	sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

	/* factory calibration */
	float                   fac_cal_scale;
	float                   fac_cal_bias;
	uint32_t                als_fac_cal_version[2];
	float					near_threshold;
	float					far_threshold;
	uint32_t                prox_fac_cal_version[2];

	/* Coefficients */
	bool                    registry_coefficient_received;
	float                   coefficient[12];
} tmd2725_common_state;

typedef struct tmd2725_state
{
	tmd2725_common_state    common;

	/* Sensor Type */
	tmd2725_sensor_type     sensor;

	sns_diag_service        *diag_service;
	sns_sync_com_port_service *scp_service;
	sns_pwr_rail_service    *pwr_rail_service;
	//sns_island_service      *island_service;

	sns_data_stream         *fw_stream;
	sns_data_stream         *reg_data_stream;
	sns_data_stream         *timer_stream;

	sns_sensor_uid          my_suid;

	tmd2725_power_rail_pending_state    power_rail_pend_state;

	/* factory calibration */
	bool                    registry_fac_cal_received;

	uint32_t                als_encoded_event_len;
	uint32_t                prox_encoded_event_len;
} tmd2725_state;

/* Functions shared by all TMD2725 Sensors */
/**
 * Sends a request to the SUID Sensor to get SUID of a dependent
 * Sensor.
 *
 * @param[i] this          Sensor reference
 * @param[i] data_type     data_type of dependent Sensor
 * @param[i] data_type_len Length of the data_type string
 */
void tmd2725_send_suid_req(sns_sensor *this, char *const data_type,
									uint32_t data_type_len);

/**
 * Call this function to do common initialization for TMD2725
 * Sensor.
 *
 * @param[i] this          Sensor reference
 */
void tmd2725_common_init(sns_sensor *const this);

/**
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 *
 * @param[i] this          Sensor reference
 * @param[i] instance      Sensor Instance to config
 *
 * @return none
 */
void tmd2725_reval_instance_config(sns_sensor *this,
									sns_sensor_instance *instance);

/**
 * Processes events from SUID Sensor.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void tmd2725_process_suid_events(sns_sensor *const this);

/**
 * Request registry.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void tmd2725_request_registry(sns_sensor *const this);

/**
 * Processes registry events.
 *
 * @param[i] this   Sensor reference
 * @param[i] event  Sensor event
 *
 * @return none
 */
void tmd2725_sensor_process_registry_event(sns_sensor *const this,
									sns_sensor_event *event);

bool tmd2725_discover_hw(sns_sensor *const this);

void tmd2725_publish_available(sns_sensor *const this);

void tmd2725_update_sibling_sensors(sns_sensor *const this);

void tmd2725_start_power_rail_timer(sns_sensor *const this,
									sns_time timeout_ticks,
									tmd2725_power_rail_pending_state pwr_rail_pend_state);

void tmd2725_start_hw_detect_sequence(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all TMD2725 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc tmd2725_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all TMD2725
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* tmd2725_set_client_request(sns_sensor *const this,
									struct sns_request const *exist_request,
									struct sns_request const *new_request,
									bool remove);
/* ALS init & deinit */
sns_rc tmd2725_als_init(sns_sensor *const this);
sns_rc tmd2725_als_deinit(sns_sensor *const this);

/* Proximity init & deinit */
sns_rc tmd2725_prox_init(sns_sensor *const this);
sns_rc tmd2725_prox_deinit(sns_sensor *const this);

/* Write back to registry */
bool tmd2725_write_calibration_to_registry(sns_sensor *const this);


sns_rc tmd2725_inst_i2c_read_byte(tmd2725_instance_state *state,
                                                   uint8_t reg,
                                                   uint8_t *buffer);
sns_rc tmd2725_inst_i2c_write_byte(tmd2725_instance_state *state,
                                                     uint8_t reg,
                                                     uint8_t val);
sns_rc tmd2725_com_rw_wrapper(sns_sync_com_port_service *scp_service,
                                    sns_sync_com_port_handle *port_handle,
                                    sns_port_vector *vectors,
                                    int32_t num_vectors,
                                    uint32_t *xfer_bytes);

void tmd2725_start_timer(sns_sensor_instance *const this, sns_data_stream ** timer_stream,
                                 sns_time timeout_ticks);

void tmd2725_get_lux(tmd2725_instance_state *state, bool is_dri);
void tmd2725_send_self_test_event(sns_sensor_instance *instance,
									sns_sensor_uid *uid, bool test_result,
									sns_physical_sensor_test_type type);


