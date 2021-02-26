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
 * @file sns_bmi160_sensor_instance.h
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#pragma once

#include <stdint.h>
#include "sns_sensor_instance.h"
#include "sns_data_stream.h"
#include "sns_time.h"
#include "sns_com_port_types.h"

#include "sns_sensor_uid.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"
#include "sns_island_service.h"
#include "sns_std_sensor.pb.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"
#include "sns_math_util.h"
#include "sns_registry_util.h"

#include "sns_bmi160_dae_if.h"
#include "sns_bmi160_config.h"
#include "sns_bmi160_ver.h"



#if  BMI160_CONFIG_ENABLE_DIAG_LOG
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#endif

//#define BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION 1

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
#include "sns_physical_sensor_oem_config.pb.h"
#endif

/** Forward Declaration of Instance API */
extern sns_sensor_instance_api bmi160_sensor_instance_api;


// <Registry Sensors>

#ifndef BMI160_CONFIG_ENABLE_FLUSH_PERIOD
#define BMI160_CONFIG_ENABLE_FLUSH_PERIOD 1
#endif




#ifndef BMI160_CONFIG_ENABLE_LOG_TS_BATCH
#define BMI160_CONFIG_ENABLE_LOG_TS_BATCH 1
#endif

#ifndef BMI160_CONFIG_ENABLE_LOG_TS_INDIVIDUAL_FRAME
#define BMI160_CONFIG_ENABLE_LOG_TS_INDIVIDUAL_FRAME 0
#endif



#ifndef BMI160_CONFIG_ENABLE_MD_TEST
#define BMI160_CONFIG_ENABLE_MD_TEST    0
#endif

#ifndef BMI160_CONFIG_ENABLE_DEBUG_TEST
#define BMI160_CONFIG_ENABLE_DEBUG_TEST 0
#endif

#define BMI160_CONFIG_ENABLE_WORKAROUND_4_SPI_SYNC_READ_LONG_DELAY 0
#define BMI160_CONFIG_ENABLE_TS_IMPROVE         0

#define BMI160_CONFIG_ENABLE_FIFO_ACCOUNTING   0

#define BMI160_CONFIG_DFT_LOG_LEVEL SNS_MED



#define BMI160_CONFIG_MAG_LOWEST_ODR            1
#define BMI160_CONFIG_MAG_FASTEST_ODR           100


#define BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW_FILT 1
#define BMI160_CONFIG_ENABLE_ERROR_ON_HIGHODR   1


#define BMI160_CONFIG_ENABLE_WAIT4CMD_IMPROVED  1

#define BMI160_CONFIG_ENABLE_PROFILING_SYS_DELAY    0
#define BMI160_CONFIG_ENABLE_TS_REF_SPECULATION     0

#define BMI160_CONFIG_FIFO_WMI_MISSING_ITVL_US_THRES    5000


#define BMI160_CONFIG_COM_FLOW_CTRL_THRESH      2


#define BMI160_CONFIG_CMD_REQ_CACHE_DEPTH       6

#define BMI160_CONFIG_FIFO_HEADROOM             10 * 13

#define BMI160_CONFIG_ACC_RANGE_DFT             BMI160_REGV_RANGE_ACC_PM8G
#define BMI160_CONFIG_GYR_RANGE_DFT             BMI160_REGV_RANGE_GYR_PM2000DPS

#define BMI160_CONFIG_ACC_BWP                   2   //normal filter
#define BMI160_CONFIG_GYR_BWP                   2   //normal filter

#define BMI160_CONFIG_MD_ACC_BWP                0
#define BMI160_CONFIG_MD_ODR                    BMI160_REGV_ODR_100HZ
#define BMI160_CONFIG_MD_THRES                  0x14    //for 2g range
#define BMI160_CONFIG_MD_DUR                    0

#define BMI160_CONFIG_INT_LATCH_REGV            BMI160_REGV_INT_LATCH_5MS


#define BMI160_CONFIG_SEE_ASYNC_COM_DELAY_US    3500    //w/ some buffer, 4500 - 1000
#define BMI160_CONFIG_SEE_SPI_BYTE_XFER_WAIT_CYCLES 2       //based on DB820


#define BMI160_CONFIG_TS_ITVL_EST_COEF          0.8f
#define BMI160_CONFIG_TS_ITVL_EST_1ST_COEF      0.8f


#define BMI160_CONFIG_TS_DELAY_ADJ_I2C          1


#if  BMI160_CONFIG_ENABLE_DEBUG
#define BMI160_SENSOR_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMI160_CONFIG_DFT_LOG_LEVEL) { \
            SNS_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}


#define BMI160_INST_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMI160_CONFIG_DFT_LOG_LEVEL) { \
            SNS_INST_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#else
#define BMI160_SENSOR_LOG(LOG_LEVEL, this, arg...)
#define BMI160_INST_LOG(LOG_LEVEL, this, arg...)

#endif

#define BMI160_FTI

#define BMI160_NUM_AXES                         3


typedef enum BMI160_REGV_ODR {
    BMI160_REGV_ODR_OFF = 0,
    BMI160_REGV_ODR_0_78HZ,
    BMI160_REGV_ODR_1_56HZ,
    BMI160_REGV_ODR_3_12HZ,
    BMI160_REGV_ODR_6_25HZ,
    BMI160_REGV_ODR_12_5HZ,
    BMI160_REGV_ODR_25HZ,
    BMI160_REGV_ODR_50HZ,
    BMI160_REGV_ODR_100HZ,
    BMI160_REGV_ODR_200HZ,
    BMI160_REGV_ODR_400HZ,
    BMI160_REGV_ODR_800HZ,
    BMI160_REGV_ODR_1600HZ,
} bmi160_regv_odr_t;

typedef enum
{
    BMI160_ACCEL         = 0x1,
    BMI160_GYRO          = 0x2,
    BMI160_MAG           = 0x4, //this is also compatible with the FIFO header bit position
    BMI160_MOTION_DETECT = 0x8,
    BMI160_SENSOR_TEMP   = 0x10
} bmi160_sensor_type;


typedef enum
{
    BMI160_CONFIG_IDLE,            /** not configuring */
    BMI160_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
    BMI160_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
    BMI160_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
    BMI160_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} bmi160_config_step;

typedef struct bmi160_self_test_info
{
    uint32_t                    test_type       :4; //of type sns_physical_sensor_test_type ;

    uint32_t                    sensor          :8; //of type bmi160_sensor_type
    uint32_t                    test_client_present:1;//of type bool
    //uint32_t                    hw_detect_success  :1;//of type bool
} bmi160_self_test_info;

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
typedef struct bmi160_self_test_info_bias
{
	sns_physical_sensor_oem_config_type config_type;
	bool 						test_client_present;
	float						offset_x;
	float						offset_y;
	float						offset_z;
} bmi160_self_test_info_bias;
#endif


typedef enum
{
    BMI160_FAC_TEST_NO_ERROR,
    BMI160_FAC_TEST_HIGH_BIAS,
    BMI160_FAC_TEST_DEV_NOT_STATIONARY,
    BMI160_FAC_TEST_ZERO_VARIANCE,
    BMI160_FAC_TEST_DEV_BUSY
} bmi160_test_err_code;

typedef struct bmi160_factory_test_info
{
    float variance_threshold;        /** stationary detect variance threshold */
    float variance[TRIAXIS_NUM];          /** variance */
    float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
    float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
    float bias_thresholds[TRIAXIS_NUM];


    uint32_t  num_samples       :20;             /** number of samples acquired */
    uint32_t  at_rest           :1;
    uint32_t  fac_test_sensor   :3; //type bmi160_sensor_type
} bmi160_factory_test_info;


typedef float bmi160_sstvt_t;
typedef bmi160_sstvt_t bmi160_gyro_sstvt;

typedef enum {
    BMI160_REGV_RANGE_ACC_PM2G = 3,
    BMI160_REGV_RANGE_ACC_PM4G = 5,
    BMI160_REGV_RANGE_ACC_PM8G = 8,
    BMI160_REGV_RANGE_ACC_PM16G = 12,
} bmi160_regv_acc_range_t;

typedef enum {
    BMI160_RANGE_ACC_PM2G = 0,
    BMI160_RANGE_ACC_PM4G,
    BMI160_RANGE_ACC_PM8G,
    BMI160_RANGE_ACC_PM16G,
} bmi160_acc_range_t;



typedef enum {
    BMI160_REGV_RANGE_GYR_PM2000DPS = 0,
    BMI160_REGV_RANGE_GYR_PM1000DPS,
    BMI160_REGV_RANGE_GYR_PM500DPS,
    BMI160_REGV_RANGE_GYR_PM250DPS,
    BMI160_REGV_RANGE_GYR_PM125DPS,
} bmi160_regv_gyr_range_t;

typedef enum {
    BMI160_RANGE_GYR_PM125DPS = 0,
    BMI160_RANGE_GYR_PM250DPS,
    BMI160_RANGE_GYR_PM500DPS,
    BMI160_RANGE_GYR_PM1000DPS,
    BMI160_RANGE_GYR_PM2000DPS,
} bmi160_gyr_range_t;



typedef struct bmi160_com_port_info {
    sns_com_port_config           com_config;
    sns_sync_com_port_handle      *port_handle;
} bmi160_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
    float min;
    float max;
} range_attr;


typedef enum BMI160_FIFO_READ_CTX_TYPE {
    BMI160_FIFO_READ_CTX_TYPE_WMI,
    BMI160_FIFO_READ_CTX_TYPE_FLUSH,
} bmi160_fifo_read_ctx_type_t;


typedef enum BMI160_FIFO_FLUSH_TRIGGER {
    BMI160_FIFO_FLUSH_TRIGGER_CLIENTS = 1,
    BMI160_FIFO_FLUSH_TRIGGER_HW_CHANGE = (1 << 1),
} bmi160_fifo_flush_trigger_t;


typedef enum BMI160_FIFO_FLUSH_DONE_CTX {
    BMI160_FIFO_FLUSH_DONE_CTX_CLIENT_REQ = 1,
    BMI160_FIFO_FLUSH_DONE_CTX_HW_CHANGE = (1 << 1),
    BMI160_FIFO_FLUSH_DONE_CTX_CLIENT_RES_NOW = (1 << 2),
} bmi160_fifo_flush_ctx_t;



typedef struct bmi160_fifo_read_ctx {
    uint8_t             ctx_type                :4;
    uint8_t             sync_read               :2;
} bmi160_fifo_read_ctx_t;


typedef struct bmi160_fifo_time_marker {
    //frame count
    int32_t             fc_accum;
    sns_time            time;
} bmi160_fifo_time_marker_t;


typedef float bmi160_real_t;

typedef struct bmi160_fifo_time_info {
    int32_t             fc_accum_curr_acc;

    int32_t             fc_accum_curr_gyr;

    sns_time            ts_end_frame_last_batch_acc;
    sns_time            ts_end_frame_last_batch_gyr;

    sns_time            ts_end_frame_this_batch_acc;
    sns_time            ts_end_frame_this_batch_gyr;

    sns_time            ts_1st_frame_this_batch_acc;
    sns_time            ts_1st_frame_this_batch_gyr;

    bmi160_real_t       itvl_this_batch_acc;
    bmi160_real_t       itvl_this_batch_gyr;
    //TODOMAG

    uint32_t            avail_ts_last_batch     :3;
    uint32_t            avail_ts_this_batch     :3;

    //uint32_t            frm_header_at_irq       :3;
    //this will be the frm_cnt for one of the sensors in frm_header_at_irq;
    //int32_t             frm_cnt_accum_at_irq;
    //
    uint32_t            ff_ts_dev_delay         :2;

    //uint32_t            avail_ts_ref_masters    :1;

    uint32_t            boost_read_size         :1;

    uint32_t            ts_dev_ff_last          :24;


    int32_t            fc_accum_masters;
    int32_t            fc_masters_last_dev_ff;


    sns_time            ts_ref_masters;

    //TODOMAG
} bmi160_fifo_time_info_t;

/** HW FIFO information */
typedef struct bmi160_fifo_info
{
    /** fifo cur rate index */
    bmi160_regv_odr_t   ff_master_odr_req;
    uint16_t            ff_wml_bytes_req;

    bmi160_regv_odr_t   ff_master_odr_curr;
    uint16_t            ff_wml_bytes_curr;

    bmi160_real_t       ff_itvl_ticks_est_masters;
    bmi160_real_t       ff_itvl_ticks_est_acc;
    bmi160_real_t       ff_itvl_ticks_est_gyr;


    /** Determines which Sensor data to publish. Uses
     *  bmi160_sensor_type as bit mask. */
    uint32_t            publish_sensors         :8;

    uint32_t            ff_sensors_to_invalidate:3;
    uint32_t            ff_sensors_flush_done_before_invalid:3;
    uint32_t            ff_sensors_en_req       :3;
    /** bits for sensors enabled in FIFO. Uses bmi160_sensor_type as bit mask
     *  to determine which FIFO Sensors are enabled */
    uint32_t            ff_sensors_en_curr      :3;


    uint32_t            ff_master_sensors_curr   :3;


    uint32_t            ff_suggested_bytes2read :10;

    uint32_t            ff_read_ctx_type        :3;

    uint32_t            ff_master_sensors_ready :3;
    uint32_t            ff_all_synced           :1;

    uint32_t            ff_flush_trigger        :3;

    uint32_t            ff_flush_in_proc        :1;

    uint32_t            ff_wmi_missing_possible :1;
    uint32_t            ff_int_masked           :1;

#if BMI160_CONFIG_ENABLE_TS_REF_SPECULATION
    int32_t             devi_est_irq;
#endif

    bmi160_fifo_time_info_t ff_tm_info;
} bmi160_fifo_info;

struct bmi160_state;

typedef struct bmi160_accel_info
{
    struct bmi160_state     *sstate;

    float                   sample_rate_req;
    float                   report_rate_req;

    // flush
    sns_time                flush_period_ticks; //NU

    bmi160_regv_odr_t       odr_req;
    uint16_t                ff_wml_req;

    bmi160_regv_acc_range_t range_req;

    bmi160_regv_odr_t       odr_curr;


    int32_t                 sstvt_curr;
    bmi160_regv_acc_range_t range_curr;

    uint32_t                ff_wml_curr             :16;

    uint32_t                gated_client_present    :1;
    uint32_t                client_present  :1;

    uint32_t                normal_mode_req_sent    :1;

    uint32_t                in_lpm                  :1;

    //uint32_t                resolution_idx          :4;

    bmi160_self_test_info   test_info;
#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
	bmi160_self_test_info_bias	test_info_bias;
#endif

} bmi160_accel_info;

typedef struct bmi160_gyro_info
{
    struct bmi160_state     *sstate;
    float                   sample_rate_req;
    float                   report_rate_req;

    // flush
    sns_time                flush_period_ticks; //NU
    bmi160_regv_odr_t       odr_req;
    uint16_t                ff_wml_req;

    bmi160_regv_gyr_range_t range_req;
    //CHECK for redundency

    bmi160_regv_odr_t       odr_curr;

    int32_t                 sstvt_curr;
    bmi160_regv_gyr_range_t range_curr;

    uint32_t                ff_wml_curr             :16;

    uint32_t                client_present          :1;

    uint32_t                normal_mode_req_sent    :1;
    //uint32_t                resolution_idx          :4;

#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW
    uint32_t                downsample_sw_factor    :4;
    uint32_t                downsample_sw_cnt       :4;

#if BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW_FILT
    int16_t                 downsample_sw_dcache[3];
#endif

#endif

    bmi160_self_test_info   test_info;
#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
	bmi160_self_test_info_bias	test_info_bias;
#endif

} bmi160_gyro_info;

typedef struct bmi160_mag_info
{
    struct bmi160_state     *sstate;

    float                   sample_rate_req;
    float                   report_rate_req;

    bmi160_regv_odr_t       odr_curr;
    uint16_t                ff_wml_req;



    uint32_t                client_present          :1;
    bmi160_self_test_info   test_info;
} bmi160_mag_info;

typedef struct bmi160_motion_detect_info
{
    struct bmi160_state     *sstate;
    //uint16_t                desired_wmk;
    //bmi160_regv_odr_t       desired_odr;

    //bmi160_accel_sstvt      sstvt_curr;
    //bmi160_regv_acc_range_t range;
    bmi160_acc_range_t      range_idx_req;

    uint32_t                enable_md_int           :1;
    uint32_t                client_present          :1;

    uint32_t                md_new_req              :1;
    sns_motion_detect_event md_state;

    bmi160_self_test_info   test_info;
    sns_registry_md_cfg     md_config;
} bmi160_motion_detect_info;

typedef struct bmi160_sensor_temp_info
{
    struct bmi160_state     *sstate;

    float                   sample_rate_req;
    float                   report_rate_req;


    sns_time                sampling_intvl;

    //uint64_t                max_requested_flush_ticks;

    uint32_t                timer_is_active         :1;
    uint32_t                timer_itvl_changed      :1;
    uint32_t                client_present          :1;

    uint32_t                sample_rate_curr        :16;

    bmi160_self_test_info   test_info;
} bmi160_sensor_temp_info;

typedef struct bmi160_irq_info
{
    sns_interrupt_req       irq_config;
    bool                    irq_ready;
    bool                    irq_registered;
} bmi160_irq_info;

typedef struct bmi160_async_com_port_info
{
    uint32_t              port_handle;
} bmi160_async_com_port_info;


typedef union bmi160_pmu_stat_reg {
    uint8_t reg;
} bmi160_pmu_stat_reg_t;

typedef union bmi160_int_en_fifo_drdy_flag {
    uint8_t flag;
} bmi160_int_en_fifo_drdy_flag_t;

typedef union bmi160_int_en_flag {
    struct {
        bmi160_int_en_fifo_drdy_flag_t drdy;
        bmi160_int_en_fifo_drdy_flag_t fifo;

        uint8_t  pedo           :1;
        uint8_t  sigmot         :1;
        uint8_t  md             :1;
        uint8_t  pmu_trigger    :1;
        uint8_t  dt             :1;
    } bits;

    uint32_t flag;
} bmi160_int_en_flag_t;



typedef struct bmi160_reg_int_context {
    uint32_t        ts_dev;
    sns_time        ts_sys;

    uint8_t         status;

    uint8_t         int_status_0;
    uint8_t         int_status_1;
    uint8_t         int_status_2;

    uint16_t        temperature;
    uint16_t        fifo_len;

    union {
        struct {
            uint8_t ts              :1;
            uint8_t status          :1;
            uint8_t int_status_0    :1;
            uint8_t int_status_1    :1;
            uint8_t int_status_2    :1;
            uint8_t int_status_3    :1;
            uint8_t temperature     :1;
            uint8_t fifo_len        :1;
        } bits;

        uint8_t flags;
    } avail;
} bmi160_reg_int_ctx_t;

typedef union bmi160_int_stat_drdy {
    struct {
        uint8_t acc :1;
        uint8_t gyr :1;
        uint8_t mag :1;
    } bits;

    uint8_t flag;
} bmi160_int_stat_drdy_t;

typedef union bmi160_int_stat_flag {
    struct {
        bmi160_int_stat_drdy_t          drdy_detail;
        uint8_t  drdy                   :1;

        uint8_t  ff_wml                 :1;
        uint8_t  ff_full                :1;
        uint8_t  md                     :1;
        uint8_t  dt                     :1;
        uint8_t  sigmot                 :1;
        uint8_t  step                   :1;
        uint8_t  pmu_trigger            :1;
    } bits;

    uint32_t flag;
} bmi160_int_stat_flag_t;


typedef struct bmi160_int_cfg_any_motion {
    uint32_t int_anym_th                :8;
    uint32_t int_anym_dur               :2;
} bmi160_int_cfg_any_motion_t;

struct bmi160_cmd_req {
    uint32_t    regv               :8;

    uint32_t    type               :3;
    uint32_t    pending            :1;
    uint32_t    exec_time_us       :20;

    sns_time    ts_req;
};


typedef struct {
    struct bmi160_cmd_req   cmd_req_acc;
    struct bmi160_cmd_req   cmd_req_gyr;

    sns_time                ts_cmd_expire_est;

    uint32_t                cmd_in_proc         :1;
    uint32_t                cmd_last_written    :8;

    sns_data_stream         *timer_cmd_stream;

} bmi160_cmd_handler_t;


typedef struct bmi160_sys_dev_time_pair {
    sns_time                    ts_sys;
    uint32_t                    ts_dev      :24;


    uint32_t                    avail_1st   :1;
} bmi160_sys_dev_time_pair_t;


typedef struct log_sensor_state_raw_info
{
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
    uint32_t log_sample_cnt;

    uint32_t batch_sample_cnt;
} log_sensor_state_raw_info;


// Unencoded batch sample
typedef struct
{
    /* Batch Sample type as defined in sns_diag.pb.h */
#if BMI160_CONFIG_ENABLE_DIAG_LOG
    sns_diag_batch_sample_type sample_type;
#endif
    /* Timestamp of the sensor state data sample */
    sns_time timestamp;
    /*Raw sensor state data sample*/
    float sample[BMI160_NUM_AXES];
    /* Data status.*/
    sns_std_sensor_sample_status status;
} bmi160_batch_sample;


typedef struct bmi160_fifo_sample_report_ctx {
    log_sensor_state_raw_info   *log_state_info_acc;
    log_sensor_state_raw_info   *log_state_info_gyr;

} bmi160_fifo_sample_report_ctx_t;


typedef struct sns_bmi160_registry_cfg {
    bmi160_sensor_type    sensor_type;
    matrix3               fac_cal_corr_mat;
    float                 fac_cal_bias[3];
    uint32_t              version;
    uint8_t               res_idx;     /* resolution index */
} sns_bmi160_registry_cfg;


enum BMI160_INT_CHECK_TRIGGER {
    BMI160_INT_CHECK_TRIGGER_IRQ,
    BMI160_INT_CHECK_TRIGGER_POST_FIFO_READ,
};

typedef struct bmi160_int_check_ctx {
    uint32_t                    int_check_trigger   :3;
    uint32_t                    req_ts_hw           :1;

    sns_time                    timestamp;
} bmi160_int_check_ctx;

/** Private state. */
typedef struct bmi160_instance_state
{
    sns_sensor_instance     *owner;

    //the crea
    struct bmi160_state     *sstate_creator;

    /** accel HW config details*/
    bmi160_accel_info       accel_info;


    /** gyro HW config details*/
    bmi160_gyro_info        gyro_info;

#if BMI160_CONFIG_ENABLE_MAG_IF
    bmi160_mag_info         mag_info;
#endif

    /** motion detect info */
    bmi160_motion_detect_info md_info;

    /** Sensor Temperature config details. */
    bmi160_sensor_temp_info sensor_temp_info;


    //OPTIM3 /** COM port info */
    bmi160_com_port_info    com_port_info;

    sns_time                ts_irq;

    /*preserve the timer suid to handle the timer for temperature*/
    //OPTIM3
    sns_sensor_uid          timer_suid;
    /** Data streams from dependentcies. */
    sns_stream_service      *stream_mgr;
    sns_data_stream         *interrupt_data_stream;
    sns_data_stream         *timer_data_stream;
    sns_data_stream         *async_com_port_data_stream;
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
    sns_island_service      *island_service;
#endif


    size_t                  encoded_imu_event_len;
    size_t                  encoded_sensor_temp_event_len;


    sns_sync_com_port_service *scp_service;
    sns_gpio_service        *gpio_service;

#if BMI160_CONFIG_ENABLE_DIAG_LOG
    sns_diag_service        *diag_service;
    size_t                  log_interrupt_encoded_size;
    size_t                  log_raw_encoded_size;
#endif

    sns_time                ts_last_sbus_read_pre;
    sns_time                ts_last_sbus_write;

    uint32_t                sbus_mon_single_byte_rw_off         :1;
    uint32_t                sbus_in_normal_mode                 :1;

    uint32_t                hw_mod_needed                       :3;
    uint32_t                hw_config_pending                   :1;

    //uint32_t                avail_1st_est_ts_hw_res             :1;
    uint32_t                new_self_test_request               :1;

    uint32_t                fac_test_in_progress                :1;

#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
    bool                    new_self_test_request_bias       :1;
#endif


    uint32_t                cnt_hw_res_est                      :10;
#if BMI160_CONFIG_ENABLE_MAG_IF //TODOMAG
#endif
    uint32_t                instance_is_ready_to_configure      :1;
    uint32_t                update_fac_cal_in_registry          :1;

    uint32_t                sensors_col                         :8;

    uint32_t                irq_ready                           :1;
    uint32_t                irq_registered                      :1;

    uint32_t                ff_flush_client_req                 :1;

    uint32_t                bus_is_spi                          :1;

    uint32_t                bus_cycle_time_ns                   :16;

    int32_t                 async_com_read_requqest;
    int32_t                 async_com_read_response;

    bmi160_pmu_stat_reg_t   pmu_stat;

    bmi160_cmd_handler_t    cmd_handler;


    //resolution of HW timestamps
    bmi160_real_t           ts_hw_res_ticks_per_bit;
    bmi160_real_t           ts_hw_res_ticks_per_bit_ideal;
    bmi160_real_t           xfer_time_per_byte_ticks;

    uint32_t                ticks_in_1ms;

#if BMI160_CONFIG_ENABLE_TS_IMPROVE
    uint32_t                ticks_delta_i;
    uint32_t                ticks_delta_p;
#endif

    /** fifo details*/
    bmi160_fifo_info        fifo_info;


    /**--------DAE interface---------*/
#if BMI160_CONFIG_ENABLE_DAE
    bmi160_dae_if_info      dae_if;
#endif
    bmi160_config_step      config_step;


    bmi160_sys_dev_time_pair_t ts_pair_sys_dev;
    bmi160_sys_dev_time_pair_t ts_pair_sys_dev_4_ts_res_est;

    bmi160_int_en_flag_t    int_en_flags_req;
    bmi160_int_en_flag_t    int_en_flags_curr;

    bmi160_reg_int_ctx_t    reg_int_ctx;

    uint8_t                 cache_regv_fifo_config_1;   //NU
    uint8_t                 cache_regv_int_map_1;   //NU

    bmi160_factory_test_info fac_test_info;

    /**----------debug----------*/

} bmi160_instance_state;

typedef struct odr_reg_map
{
    float                 odr;
    bmi160_regv_odr_t     regv;
} odr_reg_map;

typedef struct bmi160_req_payload
{
    uint32_t reserved;
    sns_bmi160_registry_cfg     registry_cfg;
    uint32_t                    sensor_type     :4;

} bmi160_req_payload;


void bmi160_inst_assess_overall_req(bmi160_instance_state *istate);

sns_rc bmi160_inst_init(sns_sensor_instance *const this,
        sns_sensor_state const *sstate);

sns_rc bmi160_inst_deinit(sns_sensor_instance *const this);

sns_rc bmi160_inst_set_client_config(
        sns_sensor_instance     *const this,
        sns_request const       *client_request);

void bmi160_run_self_test(sns_sensor_instance *instance);

/**
 * Processes results for factory test for accel and gyro.
 *
 * @param[i] instance   reference to instance
 */
void bmi160_process_fac_test(sns_sensor_instance *instance);

void bmi160_send_fac_cal_event(
        sns_sensor_instance *const  instance,
        const struct bmi160_state   *sstate);

void bmi160_reset_fac_cal_data(
        sns_sensor_instance *const  instance,
        struct bmi160_state         *sstate,
        bool                        update_version);

typedef sns_rc (*fp_read_gpio)(uint32_t gpio, bool is_chip_pin, sns_gpio_state *state);

typedef sns_rc (*fp_sns_scp_register_rw)(sns_sync_com_port_handle *port_handle,
                                sns_port_vector *vectors,
                                int32_t num_vectors,
                                bool save_write_time,
                                uint32_t *xfer_bytes);


#if BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION
void bmi160_run_self_test_bias(sns_sensor_instance *instance);
#endif

