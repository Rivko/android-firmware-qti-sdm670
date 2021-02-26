#pragma once
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



#include <stdint.h>
#include "sns_sensor_instance.h"
#include "sns_data_stream.h"
#include "sns_com_port_types.h"
#include "sns_time.h"
#include "sns_com_port_types.h"
#include "sns_sync_com_port_service.h"
#include "sns_sensor_uid.h"
#include "sns_async_com_port.pb.h"
#include "sns_math_util.h"
#include "sns_interrupt.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_diag_service.h"
#include "sns_sx931x_dae_if.h"
#include "sns_registry_util.h"


/** Forward Declaration of Instance API */
sns_sensor_instance_api sns_see_sx931x_sensor_instance_api;

// Enable below macro to enable driver debug messages
#define SX931x_ENABLE_DEBUG_MSG
#define CAP_TAG  "[SX931x] "  

#ifdef SX931x_ENABLE_DEBUG_MSG
/** TODO: consider removing unsed parameters from these macros like diag, file, line. */
#define SX931x_PRINTF(prio, sensor, fmt, args...) do { \
  SNS_PRINTF(prio, sensor, CAP_TAG fmt, ##args); \
} while (0)
#define SX931x_INST_PRINTF(prio, inst, fmt, args...) do { \
  SNS_INST_PRINTF(prio, inst , CAP_TAG fmt, ##args); \
} while (0)
#else  
#define SX931x_PRINTF(sensor,...)
#define SX931x_INST_PRINTF(inst,...)
#endif  

#ifdef SX931X_ENABLE_DUAL_SENSOR
#define DUAL_SENSOR_SUPPORT                 1
#else
#define DUAL_SENSOR_SUPPORT                 0
#endif


#ifndef s8
#define s8                                  int8_t
#define u8                                  uint8_t
#define s16                                 int16_t
#define u16                                 uint16_t
#define s32                                 int32_t
#define u32                                 uint32_t
typedef int64_t                             s64;/**< used for signed 64bit */
typedef uint64_t                            u64;/**< used for signed 64bit */
#endif



/* physical COM port structure */
typedef struct sx931x_com_port_info
{
    sns_com_port_config        com_config;
    sns_sync_com_port_handle  *port_handle;
} sx931x_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr
{
    float min;
    float max;
} range_attr;


typedef enum
{
    SX9XXX_SAR = 0x1,
    //9XXX_TEMPERATURE = 0x2,
    SX9XXX_SENSOR_INVALID = 0xFF
} sx931x_sensor_type;

typedef enum
{
    CONFIG_IDLE,            /** not configuring */
    CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
    CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
    CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
    CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} sx931x_config_step;

typedef struct sx931x_self_test_info
{
    sns_physical_sensor_test_type test_type;
    bool test_client_present;
} sx931x_self_test_info;



/*!
* low level operation mode
* */
typedef enum
{
    SLEEP_MODE,
    NORMAL_MODE,
    MAX_NUM_OP_MODE,    /* the max number of operation mode */
    INVALID_WORK_MODE = MAX_NUM_OP_MODE, /* invalid mode */
} sx931x_power_mode;


typedef struct sx931x_sensor_deploy_info
{
    /** Determines which Sensor data to publish. */
    uint8_t           publish_sensors;
    uint8_t           enable;
} sx931x_sensor_deploy_info;


typedef struct sx931x_sensor_cfg_info
{
    bool              timer_is_active;
    uint32_t          report_timer_hz;
    float             report_rate_hz;
    float             sampling_rate_hz;
    float             desired_odr;
    float             curr_odr;
    sns_sensor_uid    suid;
    uint64_t          trigger_num;
    sns_time          timeout_ticks; /* derived from the odr */
    sns_time          expection_timeout_ticks_derived_from_odr;
    sns_time          sampling_intvl;
    sns_time          expect_time;
    sx931x_self_test_info  test_info;
} sx931x_sensor_cfg_info;

typedef struct sx931x_irq_info
{
    sns_interrupt_req       irq_config;
    bool                    irq_ready;
    bool                    irq_registered;
} sx931x_irq_info;

typedef struct sns_sx931x_cfg_req
{
    float               sample_rate;
    float               report_rate;
    sx931x_sensor_type  sensor_type;
    sx931x_power_mode      op_mode;
} sns_sx931x_cfg_req;


/** Private state. */
typedef struct sx931x_instance_state
{
    bool                is_dri;
    uint32_t            hw_idx;
    /***** sensor configuration details *********/
    sx931x_sensor_cfg_info 				sar_info;
    sx931x_sensor_deploy_info			deploy_info;
    /** COM port info */
    sx931x_com_port_info 					com_port_info;
    /** Interrupt dependency info. */
    sx931x_irq_info        			  irq_info;
    /********Async Com Port****************/
    sns_async_com_port_config  ascp_config;
    /*********DAE interface*****************/
    sx931x_dae_if_info       dae_if;
    sx931x_config_step       config_step;
    sns_time                 interrupt_timestamp;
    /************************ Data streams from dependentcies. ************************/
    sns_sensor_uid           timer_suid;
    sns_data_stream          *temperature_timer_data_stream;
    sns_data_stream          *async_com_port_data_stream;                 /* data streaming channel */
    sns_data_stream          *sar_timer_data_stream;                      /* sample sensor data */
    sns_data_stream	     *interrupt_data_stream;                      /*interrupt sensor data*/
    /************************ request/configure stream *******************************/
    uint32_t                       client_req_id;
    //sns_std_sensor_config          bmp285_req;   /* stream for the configure */
    size_t                         encoded_imu_event_len;
    sx931x_power_mode              op_mode;
    sns_diag_service               *diag_service;/*for diagnostic to print debug message */
    sns_sync_com_port_service      *scp_service;
    uint32_t                       interface;
    sns_rc (*com_read)(sns_sync_com_port_service *scp_service,
                       sns_sync_com_port_handle *port_handle,
                       uint32_t rega,
                       uint8_t  *regv,
                       uint32_t bytes,
                       uint32_t *xfer_bytes);
    sns_rc (*com_write)(sns_sync_com_port_service *scp_service,
                        sns_sync_com_port_handle *port_handle,
                        uint32_t rega,
                        uint8_t  *regv,
                        uint32_t bytes,
                        uint32_t *xfer_bytes,
                        bool save_write_time);

    bool  		instance_is_ready_to_configure;
    bool 		new_self_test_request;
    size_t              log_raw_encoded_size;
    size_t		log_interrupt_encoded_size;
} sx931x_instance_state;


sns_rc sns_see_sx931x_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate);
sns_rc sns_see_sx931x_inst_deinit(sns_sensor_instance *const this);
sns_rc sns_see_sx931x_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request);
