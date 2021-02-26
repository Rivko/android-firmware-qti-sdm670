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

#include <stdint.h>
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"

#include "sns_sensor_uid.h"

#include "sns_async_com_port.pb.h"
#include "sns_diag_service.h"
#include "sns_interrupt.pb.h"
#include "sns_island_service.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_ambient_light.pb.h"
#include "sns_proximity.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"

/** Forward Declaration of Instance API */
sns_sensor_instance_api tmd2725_sensor_instance_api;

/** Number of registers to read for debug */
#define TMD2725_DEBUG_REGISTERS          (32)

/** Number of entries in reg_map table. */
#define TMD2725_REG_MAP_TABLE_SIZE       (11)

typedef struct tmd2725_com_port_info
{
	sns_com_port_config      com_config;
	sns_sync_com_port_handle *port_handle;
} tmd2725_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
	float min;
	float max;
} range_attr;

typedef enum {
 	TMD2725_ALS           = 0x01,
	TMD2725_PROX          = 0x02,
} tmd2725_sensor_type;

typedef enum {
	TMD2725_CONFIG_IDLE,            /** not configuring */
	TMD2725_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
	TMD2725_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
	TMD2725_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
	TMD2725_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} tmd2725_config_step;

typedef struct tmd2725_self_test_info
{
	sns_physical_sensor_test_type test_type;
	bool test_client_present;
} tmd2725_self_test_info;

typedef struct tmd2725_als_info
{
	sns_sensor_uid          als_suid;
	float                   dgf;
	float                   b_coef;
	float                   c_coef;
	float                   d_coef;
	bool                    use_max;
	float                   target_lux;
	int32_t                 photopic;
	int32_t                 als_ir;
	float                   lux;
	tmd2725_self_test_info  test_info;
} tmd2725_als_info;

typedef struct tmd2725_prox_info
{
	sns_sensor_uid          prox_suid;
	uint8_t                 raw;
	uint8_t                 nearby;
	sns_proximity_event     prox_event;
	tmd2725_self_test_info  test_info;
} tmd2725_prox_info;

typedef struct tmd2725_irq_info
{
	sns_interrupt_req       irq_config;
	bool                    irq_ready;
	bool                    irq_registered;
} tmd2725_irq_info;

typedef struct sns_tmd2725_registry_cfg
{
	tmd2725_sensor_type     sensor_type;
	matrix3                 fac_cal_corr_mat;
	float                   fac_cal_bias[3];
	uint32_t                version;
} sns_tmd2725_registry_cfg;

/** Private state. */
typedef struct tmd2725_instance_state
{
	/* Sensor to be published */
	uint8_t                 publish_sensors;
	uint8_t                 last_publish_sensors;
    uint8_t                 prox_high_thresh;
    uint8_t                 prox_low_thresh;
    uint8_t                 prox_pers;
    uint8_t                 prox_pers_init;
    uint8_t                 intenab;
    uint8_t                 intenab_init;
    uint8_t                 enable;
    uint8_t                 enable_init;
    uint8_t                 enable_backup;
	tmd2725_sensor_type     new_request_sensor;

	/* Ambient light HW config details*/
	tmd2725_als_info        als_info;

	/* Proximity HW config details*/
	tmd2725_prox_info       prox_info;

	/* Interrupt dependency info. */
	tmd2725_irq_info        irq_info;

	/* COM port info */
	tmd2725_com_port_info   com_port_info;

	/*--------DAE interface---------*/
	//tmd2725_dae_if_info   dae_if;

	tmd2725_config_step     config_step;

	sns_time                interrupt_timestamp;

	/* Data streams from dependentcies. */
	sns_sensor_uid          irq_suid;
	sns_sensor_uid          timer_suid;
	sns_data_stream         *interrupt_data_stream;
	sns_data_stream         *timer_data_stream;
	sns_data_stream         *als_timer_data_stream;
	sns_data_stream         *prox_timer_data_stream;
	sns_data_stream         *atime_timer_data_stream;
	sns_data_stream         *max_calib_timer_data_stream;

	/**----------Sensor specific registry configuration----------*/
	sns_registry_phy_sensor_cfg als_registry_cfg;
	sns_registry_phy_sensor_cfg prox_registry_cfg;

	uint8_t                 reg_status[TMD2725_DEBUG_REGISTERS];

	sns_diag_service        *diag_service;
	sns_sync_com_port_service *scp_service;
	sns_island_service      *island_service;

	bool                    instance_is_ready_to_configure;
	bool                    timer_is_active;
	bool                    is_dri;
	bool                    first_als;
	bool                    first_prox;
	bool                    als_to_report;
	bool                    prox_to_report;
	//bool                  fifo_flush_in_progress;
	bool                    new_self_test_request;
	bool                    update_fac_cal_in_registry;

	float					lux_scale;
	float                   lux_bias;
	uint32_t                als_fac_cal_version[2];
	float					near_threshold;
	float					far_threshold;
	uint32_t                prox_fac_cal_version[2];

	size_t                  log_interrupt_encoded_size;
	size_t                  log_raw_encoded_size;
	size_t                  encoded_als_event_len;
	size_t                  encoded_prox_event_len;
} tmd2725_instance_state;

typedef struct tmd2725_odr_reg_map
{
	float                  odr;
	uint8_t                discard_samples;
} tmd2725_odr_reg_map;

typedef struct sns_tmd2725_req
{
	float  desired_sample_rate;
	float  desired_report_rate;
} sns_tmd2725_req;

/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instace
 *
 * @return none
 */
void tmd2725_run_self_test(sns_sensor_instance *instance);

sns_rc tmd2725_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate);

sns_rc tmd2725_inst_deinit(sns_sensor_instance *const this);
