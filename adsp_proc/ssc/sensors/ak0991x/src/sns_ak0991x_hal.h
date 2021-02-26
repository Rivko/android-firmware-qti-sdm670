#pragma once
/**
 * @file sns_ak0991x_hal.h
 *
 * Hardware Access Layer functions.
 *
 * Copyright (c) 2016-2018 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include <stdint.h>

#include "sns_ak0991x_lite.h"
#include "sns_ak0991x_sensor_instance.h"
#include "sns_diag.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#ifdef AK0991X_ENABLE_DIAG_LOGGING
#include "sns_std.pb.h"
#endif
#include "sns_std_sensor.pb.h"
#include "sns_registry_util.h"

/* Referenced data sheet version
 * AK09911  data sheet version MS1626_E-01
 * AK09912  data sheet version MS1547-E-02
 * AK09913  data sheet version 015007259-E-00
 * AK09915C data sheet version 015006484-E-02
 * AK09915D data sheet version 016009278-E-00
 * AK09916C data sheet version 015007392-E-02
 * AK09916D data sheet version preliminary_E-00
 * AK09917D data sheet version preliminary_E-00-Q
 * AK09918  data sheet version 016014242_E_00
 */

// Enable for test code
//#ifndef AK0991X_ENABLE_TEST_CODE
//#define AK0991X_ENABLE_TEST_CODE         1
//#endif

/**
 *  Address registers
 */
#define AKM_AK0991X_REG_WIA1                        (0x00)
#define AKM_AK0991X_REG_WIA2                        (0x01)
#define AKM_AK0991X_REG_INFO1                       (0x02)
#define AKM_AK0991X_REG_INFO2                       (0x03)
#define AKM_AK0991X_REG_ST1                         (0x10)
#define AKM_AK0991X_REG_HXL                         (0x11)
#define AKM_AK0991X_REG_HXH                         (0x12)
#define AKM_AK0991X_REG_HYL                         (0x13)
#define AKM_AK0991X_REG_HYH                         (0x14)
#define AKM_AK0991X_REG_HZL                         (0x15)
#define AKM_AK0991X_REG_HZH                         (0x16)
#define AKM_AK0991X_REG_TMPS                        (0x17)
#define AKM_AK0991X_REG_ST2                         (0x18)
#define AKM_AK0991X_REG_CNTL1                       (0x30)
#define AKM_AK0991X_REG_CNTL2                       (0x31)
#define AKM_AK0991X_REG_CNTL3                       (0x32)

#define AKM_AK0991X_FUSE_ASAX                       (0x60)
#define AKM_AK0991X_FUSE_ASAY                       (0x61)
#define AKM_AK0991X_FUSE_ASAZ                       (0x62)

#define AKM_AK0991X_REG_TPH1                        (0xC0)
#define AKM_AK0991X_REG_TPH2                        (0xC1)
#define AKM_AK0991X_REG_RR                          (0xC2)
#define AKM_AK0991X_REG_SYT                         (0xC3)
#define AKM_AK0991X_REG_DT                          (0xC4)
/** AK0991X number of data types*/
#define AK0991X_NUM_READ_DEV_ID                     4
#define AK0991X_NUM_SENSITIVITY                     3
#define AK0991X_NUM_DATA_ST1_TO_ST2                 9
#define AK0991X_NUM_DATA_HXL_TO_ST2                 8

/** DEVICE ID */
#define AK0991X_WHOAMI_COMPANY_ID                   (0x48) /** Who Am I company ID */
#define AK09911_WHOAMI_DEV_ID                       (0x5)  /** Who Am I device ID */
#define AK09912_WHOAMI_DEV_ID                       (0x4)  /** Who Am I device ID */
#define AK09913_WHOAMI_DEV_ID                       (0x8)  /** Who Am I device ID */
#define AK09915_WHOAMI_DEV_ID                       (0x10) /** Who Am I device ID */
#define AK09916C_WHOAMI_DEV_ID                      (0x9)  /** Who Am I device ID */
#define AK09916D_WHOAMI_DEV_ID                      (0xB)  /** Who Am I device ID */
#define AK09917_WHOAMI_DEV_ID                       (0xD)  /** Who Am I device ID */
#define AK09918_WHOAMI_DEV_ID                       (0xC)  /** Who Am I device ID */

/** DEVICE SUB ID to distinguish AK09915C and AK09915D */
#define AK09915_SUB_ID_IDX                          0x3 /** RSV2 (03h) */
#define AK09915C_SUB_ID                             0x0
#define AK09915D_SUB_ID                             0x2

/** Data ready bit */
#define AK0991X_DRDY_BIT                            0x1

/** Data over run bit */
#define AK0991X_DOR_BIT                             0x2

/** Magnetic sensor overflow bit */
#define AK0991X_HOFL_BIT                            0x8

/** Invalid fifo data bit */
#define AK0991X_INV_FIFO_DATA                       0x4

/** Soft reset */
#define AK0991X_SOFT_RESET                          0x1

/** fifo paramters */
#define AK09911_FIFO_SIZE                           0
#define AK09912_FIFO_SIZE                           0
#define AK09913_FIFO_SIZE                           0
#define AK09915_FIFO_SIZE                           25  //to prevent DOR (HW FIFO buffer size = 32)
#define AK09916_FIFO_SIZE                           0
#define AK09917_FIFO_SIZE                           25  //to prevent DOR (HW FIFO buffer size = 32)
#define AK09918_FIFO_SIZE                           0
#define AK0991X_MAX_FIFO_SIZE                       AK09915_FIFO_SIZE * \
                                                      AK0991X_NUM_DATA_HXL_TO_ST2
#define AK0991X_MAX_PHYSICAL_FIFO_SIZE              32 // Physical mag senosr allows maximum upto 32 samples

/** Off to idle time */
#define AK0991X_OFF_TO_IDLE_MS                      100 //ms

/** Wait time before mode setting */
#define AK0991X_TWAIT_USEC                          100 //us

/** masurement time */
/*
#define AK09911_TIME_FOR_MEASURE_US                 8500 //us (MAX)
#define AK09912_TIME_FOR_MEASURE_US                 8500 //us (MAX)
#define AK09913_TIME_FOR_MEASURE_US                 8200 //us (MAX)
#define AK09915_TIME_FOR_LOW_POWER_MODE_MEASURE_US  4950 //us (MAX)
#define AK09915_TIME_FOR_LOW_NOISE_MODE_MEASURE_US  9350 //us (MAX)
#define AK09916_TIME_FOR_MEASURE_US                 8200 //us (MAX)
#define AK09917_TIME_FOR_LOW_POWER_MODE_MEASURE_US  4100 //us (MAX)
#define AK09917_TIME_FOR_LOW_NOISE_MODE_MEASURE_US  8200 //us (MAX)
#define AK09918_TIME_FOR_MEASURE_US                 8200 //us (MAX)
*/
#define AK09911_TIME_FOR_MEASURE_US                 7200 //us (TYP)
#define AK09912_TIME_FOR_MEASURE_US                 7200 //us (TYP)
#define AK09913_TIME_FOR_MEASURE_US                 7200 //us (TYP)
#define AK09915_TIME_FOR_LOW_POWER_MODE_MEASURE_US  4500 //us (TYP)
#define AK09915_TIME_FOR_LOW_NOISE_MODE_MEASURE_US  8500 //us (TYP)
#define AK09916_TIME_FOR_MEASURE_US                 7200 //us (TYP)
#define AK09917_TIME_FOR_LOW_POWER_MODE_MEASURE_US  3600 //us (TYP)
#define AK09917_TIME_FOR_LOW_NOISE_MODE_MEASURE_US  7200 //us (TYP)
#define AK09918_TIME_FOR_MEASURE_US                 7200 //us (TYP)

#ifdef AK0991X_ENABLE_S4S
/** s4s configuration */
#define AK0991X_S4S_INTERVAL_MS                     1000 //ms
#define AK0991X_S4S_RR                              1
#endif // AK0991X_ENABLE_S4S

/** Limit of factory shipment test */
#define TLIMIT_NO_READ_ID                           0x001
#define TLIMIT_NO_INVALID_ID                        0x002
#define TLIMIT_NO_RESET                             0x003
#define TLIMIT_NO_READ_ASA                          0x004
#define TLIMIT_NO_SET_SELFTEST                      0x005
#define TLIMIT_NO_READ_ST1                          0x006
#define TLIMIT_NO_READ_DATA                         0x007
#define TLIMIT_NO_ASAX                              0x101
#define TLIMIT_LO_ASAX                              1
#define TLIMIT_HI_ASAX                              254

#define TLIMIT_NO_ASAY                              0x102
#define TLIMIT_LO_ASAY                              1
#define TLIMIT_HI_ASAY                              254

#define TLIMIT_NO_ASAZ                              0x103
#define TLIMIT_LO_ASAZ                              1
#define TLIMIT_HI_ASAZ                              254

#define TLIMIT_NO_SLF_RVHX                          0x201
#define TLIMIT_NO_SLF_RVHY                          0x202
#define TLIMIT_NO_SLF_RVHZ                          0x203
#define TLIMIT_NO_SLF_ST2                           0x204
#define TLIMIT_LO_SLF_ST2                           0
#define TLIMIT_HI_SLF_ST2                           0
#define TLIMIT_ST2_MASK                             (0x08)

#if     defined(AK0991X_TARGET_AK09911)
#define TLIMIT_LO_SLF_RVHX                          -30
#define TLIMIT_HI_SLF_RVHX                          30
#define TLIMIT_LO_SLF_RVHY                          -30
#define TLIMIT_HI_SLF_RVHY                          30
#define TLIMIT_LO_SLF_RVHZ                          -400
#define TLIMIT_HI_SLF_RVHZ                          -50

#elif   defined(AK0991X_TARGET_AK09912)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1600
#define TLIMIT_HI_SLF_RVHZ                          -400

#elif   defined(AK0991X_TARGET_AK09913)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1000
#define TLIMIT_HI_SLF_RVHZ                          -200

#elif   defined(AK0991X_TARGET_AK09915C)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -800
#define TLIMIT_HI_SLF_RVHZ                          -200

#elif   defined(AK0991X_TARGET_AK09915D)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -800
#define TLIMIT_HI_SLF_RVHZ                          -200

#elif   defined(AK0991X_TARGET_AK09916C)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1000
#define TLIMIT_HI_SLF_RVHZ                          -200

#elif   defined(AK0991X_TARGET_AK09916D)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1000
#define TLIMIT_HI_SLF_RVHZ                          -200

#elif   defined(AK0991X_TARGET_AK09917)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1000
#define TLIMIT_HI_SLF_RVHZ                          -150

#elif   defined(AK0991X_TARGET_AK09918)
#define TLIMIT_LO_SLF_RVHX                          -200
#define TLIMIT_HI_SLF_RVHX                          200
#define TLIMIT_LO_SLF_RVHY                          -200
#define TLIMIT_HI_SLF_RVHY                          200
#define TLIMIT_LO_SLF_RVHZ                          -1000
#define TLIMIT_HI_SLF_RVHZ                          -150

#else

/*******************************
* AK09918 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_AK09918                  -200
#define TLIMIT_HI_SLF_RVHX_AK09918                  200
#define TLIMIT_LO_SLF_RVHY_AK09918                  -200
#define TLIMIT_HI_SLF_RVHY_AK09918                  200
#define TLIMIT_LO_SLF_RVHZ_AK09918                  -1000
#define TLIMIT_HI_SLF_RVHZ_AK09918                  -150

/*******************************
* AK09917 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_AK09917                  -200
#define TLIMIT_HI_SLF_RVHX_AK09917                  200
#define TLIMIT_LO_SLF_RVHY_AK09917                  -200
#define TLIMIT_HI_SLF_RVHY_AK09917                  200
#define TLIMIT_LO_SLF_RVHZ_AK09917                  -1000
#define TLIMIT_HI_SLF_RVHZ_AK09917                  -150

/*******************************
* AK09916 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_AK09916                  -200
#define TLIMIT_HI_SLF_RVHX_AK09916                  200
#define TLIMIT_LO_SLF_RVHY_AK09916                  -200
#define TLIMIT_HI_SLF_RVHY_AK09916                  200
#define TLIMIT_LO_SLF_RVHZ_AK09916                  -1000
#define TLIMIT_HI_SLF_RVHZ_AK09916                  -200

/*******************************
* AK09915 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_AK09915                  -200
#define TLIMIT_HI_SLF_RVHX_AK09915                  200
#define TLIMIT_LO_SLF_RVHY_AK09915                  -200
#define TLIMIT_HI_SLF_RVHY_AK09915                  200
#define TLIMIT_LO_SLF_RVHZ_AK09915                  -800
#define TLIMIT_HI_SLF_RVHZ_AK09915                  -200

/*******************************
 * AK09913 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_AK09913                  -200
#define TLIMIT_HI_SLF_RVHX_AK09913                  200
#define TLIMIT_LO_SLF_RVHY_AK09913                  -200
#define TLIMIT_HI_SLF_RVHY_AK09913                  200
#define TLIMIT_LO_SLF_RVHZ_AK09913                  -1000
#define TLIMIT_HI_SLF_RVHZ_AK09913                  -200

/*******************************
 * AK09912 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_AK09912                  -200
#define TLIMIT_HI_SLF_RVHX_AK09912                  200
#define TLIMIT_LO_SLF_RVHY_AK09912                  -200
#define TLIMIT_HI_SLF_RVHY_AK09912                  200
#define TLIMIT_LO_SLF_RVHZ_AK09912                  -1600
#define TLIMIT_HI_SLF_RVHZ_AK09912                  -400

/*******************************
 * AK09911 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_AK09911                  -30
#define TLIMIT_HI_SLF_RVHX_AK09911                  30
#define TLIMIT_LO_SLF_RVHY_AK09911                  -30
#define TLIMIT_HI_SLF_RVHY_AK09911                  30
#define TLIMIT_LO_SLF_RVHZ_AK09911                  -400
#define TLIMIT_HI_SLF_RVHZ_AK09911                  -50

#endif // AK0991X_TARGET_AK09911-18

/*******************************
 * Number of axes in a 3 axis sensor
 */
#define AK0991X_NUM_AXES                            TRIAXIS_NUM

/*******************************
 * Measurement time calculation bit resolution
 */
#define AK0991X_CALC_BIT_RESOLUTION                 13
#define AK0991X_IRQ_NUM_FOR_OSC_ERROR_CALC          3

#ifdef AK0991X_BOARD_HDK820
#define AK0991X_CALC_BIT_ERROR                      4000  // HDK820 (huge jitter observed)
#else
#define AK0991X_CALC_BIT_ERROR                      40
#endif

#ifdef AK0991X_ENABLE_DIAG_LOGGING
/*******************************
 * Log structure definition
 */
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

/*******************************
 * Unencoded batch sample
 */
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[AK0991X_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} ak0991x_batch_sample;
#else
typedef struct log_sensor_state_raw_info
{
  // enmpty
} log_sensor_state_raw_info;
#endif

/******************* Function Declarations ***********************************/
/**
 * Write wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           write buffer
 * @param[i] bytes            bytes to write
 * @param[o] xfer_bytes       bytes written
 * @param[i] save_write_time  true to save write transfer time.
 *
 * @return sns_rc
 */
sns_rc ak0991x_com_write_wrapper(sns_sensor_instance *const this,
                                 sns_sync_com_port_service * scp_service,
                                 sns_sync_com_port_handle *port_handle,
                                 uint32_t reg_addr,
                                 uint8_t *buffer,
                                 uint32_t bytes,
                                 uint32_t *xfer_bytes,
                                 bool save_write_time );

/**
 * Resets the Sensor SW.
 * This function is used in ak0991x drivers flow only.
 * If call from other flow directly for HW reset,
 * should also reset the SW settings like a mag_info.curr_odr.
 *
 * @param[i] port_handle   handle to synch COM port
 * @param[i] scp_service   synch COM port service
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_device_sw_reset(sns_sensor_instance *const this,
                               sns_sync_com_port_service * scp_service,
                               sns_sync_com_port_handle *port_handle);

/**
 * Enable Mag streaming. enables Mag sensor with
 * non-zero desired ODR.
 *
 * @param[i] state         Instance state
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_start_mag_streaming(sns_sensor_instance *const this);

/**
 * Disable Mag streaming.
 *
 * @param[i] state         Instance state
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_stop_mag_streaming(sns_sensor_instance *const this);

/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] port_handle   handle to synch COM port
 * @param[i] scp_service   handle to synch COM port service
 *
 * @param[o] buffer        who am I value read from HW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_get_who_am_i(sns_sync_com_port_service * scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer);

/**
 * Sets sensitivity adjustment for the sensor.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[i] device_select device ID
 * @param[o] sstvt_adj     Set sensitivity adjustment
 *
 * @return sns_rc
 * SNS_RC_FAILED
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_set_sstvt_adj(
                             sns_sync_com_port_service* scp_service,
                             sns_sync_com_port_handle *port_handle,
                             uint8_t device_select,
                             float *sstvt_adj);

/**
 * Gets current ODR.
 *
 * @param[i] curr_odr       Current ODR.
 *
 * @return current ODR
 */
float ak0991x_get_mag_odr(ak0991x_mag_odr curr_odr);

/**
 * Provides sample interval based on current ODR
 *
 * @param[i] curr_odr       Current ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time ak0991x_get_sample_interval(ak0991x_mag_odr curr_odr);

/**
 * Process a fifo buffer and extracts mag samples from the buffer
 * and generates event.
 *
 * @param[i] instance              Sensor instance
 * @param[i] first_ts              Timestamp of first sample in fifo
 * @param[i] interval              Sampling interval in time ticks
 * @param[i] fifo                  Buffer containing sample read from HW FIFO
 * @param[i] num_bytes             Number of bytes in fifo buffer
 *
 */
void ak0991x_process_mag_data_buffer(sns_sensor_instance *instance,
                                     sns_time            first_ts,
                                     sns_time            interval,
                                     uint8_t             *fifo,
                                     size_t              num_bytes);


// QC -  Please add function header
void ak0991x_get_st1_status(sns_sensor_instance *const instance);

/**
 * Sends a FIFO complete event.
 *
 * @param instance   Instance reference
 */
void ak0991x_send_fifo_flush_done(sns_sensor_instance *const instance);

/**
 * Read mag samples from the buffer
 * and generates event.
 *
 * @param instance                 Sensor Instance
 */
void ak0991x_read_mag_samples(sns_sensor_instance *const instance);

/**
 * Sends config update event for the chosen sample_rate
 *
 * @return sns_rc
 * SNS_RC_FAILED
 * SNS_RC_SUCCESS
 * @param[i] instance    reference to this Instance
 */
sns_rc ak0991x_send_config_event(sns_sensor_instance *const instance);

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *       pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *       of batch
 *  */
void ak0991x_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete);

/**
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *                        pertaining to the sensor
 * @param[i] raw_data     Uncalibrated sensor data to be logged
 * @param[i] timestamp    Timestamp of the sensor data
 * @param[i] status       Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc ak0991x_log_sensor_state_raw_add(
  log_sensor_state_raw_info *log_raw_info,
  float *raw_data,
  sns_time timestamp,
  sns_std_sensor_sample_status status);

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
void ak0991x_log_sensor_state_raw_alloc(
  log_sensor_state_raw_info *log_raw_info,
  uint32_t log_size);

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
 *                       during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc ak0991x_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written);

/**
 * Enable interrupt if not already enabled
 *
 */
void ak0991x_register_interrupt(sns_sensor_instance *this);

/**
 * Enable heart beat timer if not already enabled
 *
 */
void ak0991x_register_heart_beat_timer(sns_sensor_instance *this);

/**
 * Enable timer if not already enabled
 *
 */
void ak0991x_register_timer(sns_sensor_instance *this);

/**
 * Get time for measurement
 *
 * @param[i] device_select  Device type
 * @param[i] sdr            Drive mode setting
 * @param[o] measure_us     Measurement time in usec
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc ak0991x_get_meas_time(akm_device_type device_select, uint8_t sdr, sns_time* measure_us );

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 * @return sns_rc
 * SNS_RC_FAILED
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_reconfig_hw(sns_sensor_instance *this);

/**
 * Run a hardware self-tests.
 *
 * @param[i] instance     reference to the instance
 * @param[o] err          error code
 *
 * @return sns_rc
 * SNS_RC_FAILED
 * SNS_RC_SUCCESS
 */
sns_rc ak0991x_hw_self_test(sns_sensor_instance *instance,
                            uint32_t *err);

/**
 * Executes requested self-tests.
 *
 * @param instance     reference to the instance
 *
 * @return none
 */
void ak0991x_run_self_test(sns_sensor_instance *instance);

#ifdef AK0991X_ENABLE_DRI
/**
 * Clock error procedure for DRI mode.
 *
 * @param instance     reference to the instance
 *
 * @return none
 */
void ak0991x_clock_error_calc_procedure(sns_sensor_instance *const instance);
#endif

/**
 * Send Calibration event to client
 *
 * @param[i] instance        instance reference
 * @return none
 */
void ak0991x_send_cal_event(sns_sensor_instance * const instance);

/**
 * Reset Calibration values
 *
 * @param[i] instance        instance reference
 * @return none
 */
void ak0991x_reset_cal_data(sns_sensor_instance *const instance);

#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
uint32_t ak0991x_device_mode2cal_id(sns_sensor_instance *const instance);
#endif
