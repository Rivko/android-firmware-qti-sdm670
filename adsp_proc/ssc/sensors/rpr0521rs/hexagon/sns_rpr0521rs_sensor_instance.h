//==============================================================================
// MODULE        : sns_rpr0521rs_sensor_instance.h
// FUNCTION      : RPR0521 Sensor Instance implementation
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Nov/16/2017
// MODIFIED      : Ryosuke Yamamoto
// MODIFIED DATE : Dec/4/2018
// REMARKS       :
// COPYRIGHT     : Copyright (C) 2017 - 2018 - ROHM CO.,LTD.
//               : Redistribution and use in source and binary forms, with or
//               : without modification, are permitted provided that the
//               : following conditions are met:
//               : 1. Redistributions of source code must retain the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer.
//               : 2. Redistributions in binary form must reproduce the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer in the documentation and/or other materials
//               : provided with the distribution.
//               : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//               : contributors may be used to endorse or promote products
//               : derived from this software without specific prior written
//               : permission.
//               : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//               : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//               : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//               : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//               : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//               : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//               : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//               : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//               : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//               : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//               : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//               : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//               : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//               : SUCH DAMAGE.
//==============================================================================
#ifndef SNS_RPR0521RS_SENSOR_INSTANCE_H_
#define SNS_RPR0521RS_SENSOR_INSTANCE_H_ (1)

#include <math.h>
#include "sns_com_port_types.h"
#include "sns_sensor_uid.h"
#include "sns_rpr0521rs_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"

#define RPR0521RS_ALS_GAIN_X1   (  1U) // gain rate x1
#define RPR0521RS_ALS_GAIN_X2   (  2U) // gain rate x2
#define RPR0521RS_ALS_GAIN_X64  ( 64U) // gain rate x64
#define RPR0521RS_ALS_GAIN_X128 (128U) // gain rate x128

#define SEND_DATA_SIZE (2) // Lux, data0

// Factory Test Spec
#define RPR0521RS_FAC_TEST_VARIATION      (0.1F)   // 10%
#define RPR0521RS_FAC_TEST_LIMIT_L_PARA   (1.0F - RPR0521RS_FAC_TEST_VARIATION)
#define RPR0521RS_FAC_TEST_LIMIT_H_PARA   (1.0F + RPR0521RS_FAC_TEST_VARIATION)
#define RPR0521RS_FAC_TEST_SAMPLE_CNT     (22U) // number of measurements
#define RPR0521RS_FAC_TEST_ALS_REF_VAL    (560.0F) // Lux
#define RPR0521RS_FAC_TEST_PS_REF_VAL     ( 38.0F) // LSB
#define RPR0521RS_FAC_TEST_T_MEAS_MARGIN  (10) // ms

#define FLOAT_ZERO_CHECK (0.0001F) // 1/10000
#define DIGIT_COEFF      (1000)

// Forward Declaration of Sensor instance API
extern sns_sensor_instance_api rpr0521rs_sensor_instance_api;

typedef struct {
    uint8_t mode_ctl;
    uint8_t psals_ctl;
    uint8_t ps_ctl;
    uint8_t reserved0;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
    uint8_t intr;
    uint8_t psth_hl;
    uint8_t psth_hh;
    uint8_t psth_ll;
    uint8_t psth_lh;
    uint8_t alsth_hl;
    uint8_t alsth_hh;
    uint8_t alsth_ll;
    uint8_t alsth_lh;
} ALL_REG_BUF;

typedef enum {
    RPR0521RS_CONFIG_IDLE,            // not configuring
    RPR0521RS_CONFIG_POWERING_DOWN,   // cleaning up when no clients left
    RPR0521RS_CONFIG_STOPPING_STREAM, // stream stop initiated, waiting for completion
    RPR0521RS_CONFIG_FLUSHING_HW,     // FIFO flush initiated, waiting for completion
    RPR0521RS_CONFIG_UPDATING_HW      // updating sensor HW, when done goes back to IDLE
} RPR0521_CONFIG_STEP;

typedef struct {
    RPR0521RS_SENSOR_TYPE sensor_type;
    float_t  sampling_rate_hz;
    float_t  fac_cal_bias;
    float_t  fac_cal_scale_factor;
    uint32_t fac_cal_version;
} RPR0521RS_REQ;

typedef struct {
    float_t min;
    float_t max;
} RPR0521RS_RANGE_ATTR;

typedef struct {
    uint8_t enabled;
} RPR0521RS_SENSOR_INFO;

typedef struct {
    sns_physical_sensor_test_type test_type;
    bool                          test_client_present;
} RPR0521RS_SELF_TEST_INFO;

typedef struct {
    float_t sample_sum;
    float_t store_samples[RPR0521RS_FAC_TEST_SAMPLE_CNT];
    float_t reference_val;
} RPR0521RS_FACTORY_TEST_INFO;

typedef struct {
    float_t  fac_cal_bias;
    float_t  fac_cal_scale_factor;
    uint32_t fac_cal_version;
} RPR0521RS_REGISTRY_CFG;

typedef struct {
    bool                        timer_is_active;
    bool                        heart_beat_timer_is_active;
    bool                        is_dri;
    uint8_t                     heart_beat_count;
    uint16_t                    curr_intvl_ms;
    sns_sensor_uid              suid;
    float_t                     sampling_rate_hz;
    sns_time                    sampling_intvl;
    sns_time                    heart_beat_timestamp;
    sns_time                    heart_beat_timeout;
    sns_data_stream             *timer_data_stream;
    sns_data_stream             *timer_heart_beat_data_stream;
    RPR0521RS_SELF_TEST_INFO    test_info;
    RPR0521RS_FACTORY_TEST_INFO fac_test_info;
    RPR0521RS_REGISTRY_CFG      registry_cfg;
} RPR0521RS_SENSOR_COMMON_INFO;

typedef struct {
    RPR0521RS_SENSOR_COMMON_INFO common;
    uint16_t                     ori_thresh_near;
    uint16_t                     ori_thresh_far;
    uint16_t                     thresh_near;
    uint16_t                     thresh_far;
} RPR0521RS_SENSOR_PS_INFO;

typedef struct {
    RPR0521RS_SENSOR_COMMON_INFO common;
    
    RPR0521RS_COEFF_UNIT coefficient;
    uint8_t              gain0;
    uint8_t              gain1;
    uint16_t             thresh_high;
    uint16_t             thresh_low;
} RPR0521RS_SENSOR_ALS_INFO;

typedef struct {
    sns_interrupt_req irq_config;
    bool              irq_ready;
    bool              irq_registered;
    uint8_t           irq_trig;
} RPR0521RS_IRQ_INFO;

typedef struct log_sensor_state_raw_info {
    // Pointer to diag service 
    sns_diag_service *diag;
    // Pointer to sensor instance 
    sns_sensor_instance *instance;
    // Pointer to sensor UID
    struct   sns_sensor_uid *sensor_uid;
    // Size of a single encoded sample 
    size_t   encoded_sample_size;
    // Pointer to log
    void     *log;
    // Size of allocated space for log
    uint32_t log_size;
    // Number of actual bytes written
    uint32_t bytes_written;
    // Number of batch samples written
    // A batch may be composed of several logs
    uint32_t batch_sample_cnt;
    // Number of log samples written
    uint32_t log_sample_cnt;
} LOG_SENSOR_STATE_RAW_INFO;

typedef struct {
    // Batch Sample type as defined in sns_diag.pb.h
    sns_diag_batch_sample_type   sample_type;
    // Timestamp of the sensor state data sample
    sns_time                     timestamp;
    // Raw sensor state data sample
    float_t                      sample[SEND_DATA_SIZE];
    // Data status.
    sns_std_sensor_sample_status status;
} RPR0521RS_ALS_SAMPLE;

typedef struct {
    // Batch Sample type as defined in sns_diag.pb.h
    sns_diag_batch_sample_type   sample_type;
    // Timestamp of the sensor state data sample
    sns_time                     timestamp;
    // Raw sensor state data sample
    float_t                      sample;
    // Data status.
    sns_std_sensor_sample_status status;
} RPR0521RS_PS_SAMPLE;

typedef struct {
    uint8_t                    raw_regist_odr;
    uint8_t                    fac_test_num_samples;
    bool                       fac_test_in_progress;
    bool                       update_fac_cal_in_registry;
    bool                       new_self_test_request;
    bool                       revive_sensor;
    bool                       reset_threshold;
    bool instance_is_ready_to_configure;
    size_t                     log_interrupt_encoded_size;
    size_t                     log_raw_encoded_size;
    size_t                     encoded_ps_event_len;
    size_t                     encoded_als_event_len;
    RPR0521RS_SENSOR_TYPE      fac_test_sensor;
    sns_time                   interrupt_timestamp;
    sns_time                   power_on_timestamp;
    // Sensor type info
    RPR0521RS_SENSOR_INFO    sensor_type_info;    
    RPR0521RS_SENSOR_ALS_INFO als_info;      // Sensor als config details
    RPR0521RS_SENSOR_PS_INFO  ps_info;       // Sensor ps config details
    RPR0521RS_COM_PORT_INFO   com_port_info; // COM port info
    RPR0521RS_IRQ_INFO        irq_info;      // Interrupt dependency info
    sns_diag_service           *diag_service;
    sns_sync_com_port_service  *scp_service;
    sns_data_stream            *interrupt_data_stream;
    sns_island_service         *island_service;
    sns_sensor_uid             timer_suid;    
    ALL_REG_BUF                reg_buf;
    
} RPR0521RS_INSTANCE_STATE;

//==============================================================================
// function prototype
//==============================================================================
sns_rc rpr0521rs_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate);
sns_rc rpr0521rs_inst_deinit(sns_sensor_instance *const this);

sns_rc rpr0521rs_check_who_am_i(const sns_sync_com_port_service *scp_service, sns_sync_com_port_handle *port_handle, uint8_t *read_data);
sns_rc rpr0521rs_als_setting_register(RPR0521RS_INSTANCE_STATE *state, uint16_t speed, uint16_t *raw_intvl);
sns_rc rpr0521rs_ps_setting_register(RPR0521RS_INSTANCE_STATE *state, uint16_t speed, uint16_t *raw_intvl);
sns_rc rpr0521rs_device_init(RPR0521RS_INSTANCE_STATE *state);
sns_rc rpr0521rs_device_ps_read_data(RPR0521RS_INSTANCE_STATE *state, uint8_t *data0, uint16_t *raw_data);
sns_rc rpr0521rs_device_als_read_data(RPR0521RS_INSTANCE_STATE *state, ALS_DATA *raw_data);
sns_rc rpr0521rs_als_threshold_ctl(RPR0521RS_INSTANCE_STATE *state, uint16_t adata0);
sns_rc rpr0521rs_device_set_threshold(RPR0521RS_INSTANCE_STATE *state, uint16_t high, uint16_t low, uint32_t reg_addr);
sns_rc rpr0521rs_device_interrupt_read(RPR0521RS_INSTANCE_STATE *state, uint8_t *intr);
sns_rc rpr0521rs_device_set_interrupt(RPR0521RS_INSTANCE_STATE *state);
sns_rc rpr0521rs_device_psctl_read(RPR0521RS_INSTANCE_STATE *state, uint8_t *ps_ctl);

void rpr0521rs_reconfig_hw(sns_sensor_instance *this);
void rpr0521rs_send_cal_event(sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type);
void rpr0521rs_register_interrupt(RPR0521RS_INSTANCE_STATE* state);
void rpr0521rs_start_sensor_polling_timer(sns_sensor_instance *const this, RPR0521RS_SENSOR_COMMON_INFO *sensor_info);
void rpr0521rs_dump_reg(sns_sensor_instance *this);
void rpr0521rs_data_streaming_control(RPR0521RS_INSTANCE_STATE *state);
void rpr0521rs_inst_create_heart_beat_timer(sns_sensor_instance *this, RPR0521RS_SENSOR_COMMON_INFO *sensor_info);
void rpr0521rs_recovery_device_state(sns_sensor_instance *instance);
void rpr0521rs_process_factory_test(sns_sensor_instance *const this, float_t sample);
void rpr0521rs_run_self_test(sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type);

#endif // SNS_RPR0521RS_SENSOR_INSTANCE_H_

