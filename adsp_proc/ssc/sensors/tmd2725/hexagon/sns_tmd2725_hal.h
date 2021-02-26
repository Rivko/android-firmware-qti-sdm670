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
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_gpio_service.h"

#include "sns_tmd2725_sensor_instance.h"

/* Default ALS/Color equation coefficients */
#define D_Factor                      200.0f
#define B_Coef                        0.26f
#define C_Coef                        0.8f
#define D_Coef                        0.27f
#define USE_MAX                       1
#define CALIB_TARGET_LUX              600.0f

/* Register definitions */
#define TMD2725_ENABLE_REG            0x80 /* bit3 WEN, bit2 PEN, bit1 AEN, bit0 PON */
#define TMD2725_ATIME_REG             0x81 /* 2.8ms */
#define TMD2725_PRATE_REG             0x82 /* 88us */
#define TMD2725_WTIME_REG             0x83 /* 2.8ms, and 2.8ms*12 if WLONG */
#define TMD2725_AILTL_REG             0x84
#define TMD2725_AILTH_REG             0x85
#define TMD2725_AIHTL_REG             0x86
#define TMD2725_AIHTH_REG             0x87
#define TMD2725_PILT_REG              0x88
#define TMD2725_PIHT_REG              0x8A
#define TMD2725_PERS_REG              0x8C /* bit7:4 PPERS, bit3:0 APERS */
#define TMD2725_CFG0_REG              0x8D /* bit7:3 must be set to 10000b, bit2 WLONG */
#define	TMD2725_PCFG0_REG             0x8E /* bit7:6 PULSE LEN, bit5:0 PPLUSES */
#define	TMD2725_PCFG1_REG             0x8F /* bit7:6 PGAIN, bit4:0 PLDRIVE 6mA */
#define TMD2725_CFG1_REG              0x90 /* bit1:0 AGAIN */
#define	TMD2725_REVID_REG             0x91
#define	TMD2725_ID_REG                0x92
#define	TMD2725_STATUS_REG            0x93 /* ASAT|PSAT|PINT|AINT|CINT|RESERVED|PSAT_RELECTIVE|PSAT_AMBIENT */
#define TMD2725_PHOTOPICL_REG         0x94
#define TMD2725_PHOTOPICH_REG         0x95
#define TMD2725_ALS_IRL_REG           0x96
#define TMD2725_ALS_IRH_REG           0x97
#define	TMD2725_PDATA_REG             0x9C
#define	TMD2725_CFG2_REG              0x9F /* bit2 AGAINL */
#define	TMD2725_CFG3_REG              0xAB /* bit7 INT_READ_CLEAR, bit6 ALS, bit4 SAI */
#define	TMD2725_POFFSETL_REG          0xC0
#define	TMD2725_POFFSETH_REG          0xC1
#define	TMD2725_CALIB_REG             0xD7 /* bit5 ELEC_CALIB, bit1 START_CALIB */
#define	TMD2725_CALIBCFG_REG          0xD9 /* bit7:5 BINSRCH_TARGET, bit3 AUTO_OFFSET_ADJ, bit2:0 PROX_AVG */
#define	TMD2725_CALIBSTAT_REG         0xDC /* bit0 CALIB_FINISHED */
#define	TMD2725_INTENAB_REG           0xDD /* ASIEN|PSIEN|PIEN|AIEN|CIEN|RESERVE|RESERVE|RESERVE */
#define	TMD2725_POFFSET_MAG_REG       0xE6
#define	TMD2725_POFFSET_SIGN_REG      0xE7

//TMD2725_ENABLE_REG @0x80
#define	PON                           (0x01 << 0)
#define	AEN                           (0x01 << 1)
#define	PEN                           (0x01 << 2)
#define	WEN                           (0x01 << 3)

//TMD2725_PERS_REG @0x8C
#define	APERS_MASK                    (0x0F << 0)
#define	PPERS_MASK                    (0x0F << 4)

//TMD2725_CFG0_REG @0x8D
#define WLONG                         (0x01 << 2)
#define CFG0_RESERVED                 (0x10 << 3)

//TMD2725_PCFG0_REG @0x8E
#define PPULSE_MASK                   (0x3F << 0)
#define PPULSE_LEN_MASK               (0x03 << 6)
#define PPLEN_4US                     (0x00 << 6)
#define PPLEN_8US                     (0x01 << 6)
#define PPLEN_16US                    (0x02 << 6)
#define PPLEN_32US                    (0x03 << 6)

//TMD2725_PCFG1_REG @0x8F
#define PLDRIVE_MASK                  (0x1F << 0)
#define PGAIN_MASK                    (0x03 << 6)
#define PGAIN_1X                      (0x00 << 6)
#define PGAIN_2X                      (0x01 << 6)
#define PGAIN_4X                      (0x02 << 6)
#define PGAIN_8X                      (0x03 << 6)

//TMD2725_CFG1_REG @0x90
#define AGAIN_MASK                    (0x03 << 0)
#define AGAIN_1X                      (0x00 << 0)
#define AGAIN_4X                      (0x01 << 0)
#define AGAIN_16X                     (0x02 << 0)
#define AGAIN_64X                     (0x03 << 0)

//TMD2725_REVID_REG @0x91
#define REV_ID_MASK                   (0x07 << 0)

//TMD2725_ID_REG @0x92
#define ID_MASK                       (0x3F << 2)

//TMD2725_STATUS_REG @0x93
#define PSAT_AMBIENT                  (0x01 << 0)
#define PSAT_REFLECTIVE               (0x01 << 1)
#define CINT                          (0x01 << 3)
#define AINT                          (0x01 << 4)
#define PINT                          (0x01 << 5)
#define PSAT                          (0x01 << 6)
#define ASAT                          (0x01 << 7)

//TMD2725_CFG2_REG @0x9F
#define AGAINL                        (0x01 << 2)
#define GAINMAX                       (0x01 << 4)

//TMD2725_CFG3_REG @0xAB
#define CFG2_RESERVED1                (0x0C << 0)
#define SAT                           (0x01 << 4)
#define CFG2_RESERVED2                (0x02 << 5)
#define INT_READ_CLEAR                (0x01 << 7)

//TMD2725_CALIB_REG @0xD7
#define START_OFFSET_CALIB            (0x01 << 0)
#define ELECTRICAL_CALIBRATION        (0x01 << 5)

//TMD2725_CALIBCFG_REG @0xD9
#define PROX_AVG_MASK                 (0x07 << 0)
#define PROX_DATA_AVG_DISABLE         (0x00 << 0)
#define PROX_DATA_AVG_2               (0x01 << 0)
#define PROX_DATA_AVG_4               (0x02 << 0)
#define PROX_DATA_AVG_8               (0x03 << 0)
#define PROX_DATA_AVG_16              (0x04 << 0)
#define PROX_DATA_AVG_32              (0x05 << 0)
#define PROX_DATA_AVG_64              (0x06 << 0)
#define PROX_DATA_AVG_128             (0x07 << 0)
#define AUTO_OFFSET_ADJ               (0x01 << 3)
#define CALIBCFG_RESERVED             (0x01 << 4)
#define BINSRCH_TARGET_MASK           (0x07 << 5)
#define BINSRCH_TARGET_0              (0x00 << 5)
#define BINSRCH_TARGET_1              (0x01 << 5)
#define BINSRCH_TARGET_3              (0x02 << 5)
#define BINSRCH_TARGET_7              (0x03 << 5)
#define BINSRCH_TARGET_15             (0x04 << 5)
#define BINSRCH_TARGET_31             (0x05 << 5)
#define BINSRCH_TARGET_63             (0x06 << 5)
#define BINSRCH_TARGET_127            (0x07 << 5)

//TMD2725_CALIBSTAT_REG @0xDC
#define CALIB_FINISHED                (0x01 << 0)

//TMD2725_INTENAB_REG @0xDD
#define CIEN                          (0x01 << 3)
#define AIEN                          (0x01 << 4)
#define PIEN                          (0x01 << 5)
#define PSIEN                         (0x01 << 6)
#define ASIEN                         (0x01 << 7)

//Configration calculations
#define ATIME_PER_STEP_X100           280
#define ATIME_MS(ms)                  (uint8_t)(((uint32_t)ms*100 + ((uint32_t)ATIME_PER_STEP_X100 >> 1))/(uint32_t)ATIME_PER_STEP_X100 - 1)
#define PTIME_PER_STEP                88
#define PTIME_US(us)                  (uint8_t)(((uint32_t)us + ((uint32_t)PTIME_PER_STEP >> 1))/PTIME_PER_STEP - 1)
#define WTIME_PER_STEP_X100           280
#define WTIME_MS(ms)                  (uint8_t)(((uint32_t)ms*100 + ((uint32_t)WTIME_PER_STEP_X100 >> 1))/(uint32_t)WTIME_PER_STEP_X100 - 1)
#define ALS_PERSIST(p)                (uint8_t)((p & 0x0F) << 0)
#define PROX_PERSIST(p)               (uint8_t)((p & 0x0F) << 4)
#define PPULSES(c)                    (uint8_t)(c - 1)
#define PLDRIVE_MA(ma)                (uint8_t)((ma+3)/6 - 1)

/* For ALS/Color auto gain setting */
#define MAX_ALS_VALUE                 0xFFFF
#define	MIN_ALS_VALUE                 3
#define	GAIN_SWITCH_LEVEL             200

/* Proximity thresholds */
#define AMS_PROX_THRESH_NEAR          80  /* unit of ADC count */
#define AMS_PROX_THRESH_FAR           50  /* unit of ADC count */
#define AMS_PROX_THRESH_VERY_NEAR     250 /* unit of ADC count */
#define AMS_PROX_THRESH_CONTAMINATED  150 /* unit of ADC count */
#define AMS_PROX_MAX_VALUE            255

/* Max offset limitation, should <= 150 */
#define TMD2725_MAX_OFFSET            150

/* Time */
#define TMD2725_OFF_TO_IDLE_MS        10  //ms
#define TMD2725_POLLING_MS            100
#define TMD2725_DELAY_AFTER_PON       1
#define TMD2725_MAX_CALIB_TIME        30

#define TMD2725_WHOAMI_VALUE          0xE4
#define TMD2725_PARTNO_MASK           0xFC

/* Proximity parameters */
#define TMD2725_PROX_MAX_VALUE        255
#define TMD2725_VERY_NEAR_THRESHOLD   250
#define TMD2725_THRESH_CONTAMINATED   150
#define	TMD2725_CONTAMINATED_COUNT    16
#define TMD2725_VERY_NEAR_COUNT       4
#define	TMD2725_DATA_JITTER_TH        3

/* If greater than this value, we think it is unreasonable */
#define TMD2725_MAX_POFFSET           240

typedef struct tmd2725_reg_setting {
	uint8_t reg;
	uint8_t value;
} tmd2725_reg_setting;

/******************* Function Declarations ***********************************/

/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[o] buffer        who am I value read from HW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc tmd2725_get_who_am_i(sns_sync_com_port_service *scp_service,
                                    sns_sync_com_port_handle *port_handle,
                                    uint8_t *buffer);

/**
 * Reset TMD2725 sensor to default.
 *
 * @param[i] state         state of sensor instance
 * @param[i] sensor        bit mask for sensors to handle
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 * @return none
 */
sns_rc tmd2725_reset_device(tmd2725_instance_state *state,
                                    tmd2725_sensor_type sensor);

/**
 * Modify TMD2725 ENABLE register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] mask          mask of the bits which will be modified
 * @param[i] val           the value to be written into the mask bits
 *
 * @return none
 */
sns_rc tmd2725_modify_enable(tmd2725_instance_state *state,
                                     uint8_t mask,
                                     uint8_t val);

/**
 * Modify TMD2725 INTENABLE register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] mask          mask of the bits which will be modified
 * @param[i] val           the value to be written into the mask bits
 *
 * @return none
 */
sns_rc tmd2725_modify_intenab(tmd2725_instance_state *state,
                                      uint8_t mask,
                                      uint8_t val);

/**
 * TMD2725 config start calibration.
 *
 * @param[i] state         state of sensor instance
 *
 * @return none
 */
sns_rc tmd2725_start_calibration(tmd2725_instance_state *state);

/**
 * Get ALS interrupt persistence from register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   store the interrupt persistence of ALS sensor
 *
 * @return none
 */
sns_rc tmd2725_get_als_pers(tmd2725_instance_state *state, uint8_t *persistence);

/**
 * Write ALS interrupt persistence setting to register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   interrupt persistence for ALS sensor
 *
 * @return none
 */
sns_rc tmd2725_set_als_pers(tmd2725_instance_state *state, uint8_t persistence);

/**
 * Get proximity interrupt persistence from register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   store the interrupt persistence of proximity sensor
 *
 * @return none
 */
sns_rc tmd2725_get_prox_pers(tmd2725_instance_state *state, uint8_t *persistence);

/**
 * Write proximity interrupt persistence setting to register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   interrupt persistence for proximity sensor
 *
 * @return none
 */
sns_rc tmd2725_set_prox_pers(tmd2725_instance_state *state, uint8_t persistence);

/**
 * Update ALS interrupt threshold registers.
 *
 * @param[i] state                 Instance state
 * @param[i] high_thresh           High threshold
 * @param[i] low_thresh            Low threshold
 *
 * @return none
 */
sns_rc tmd2725_update_als_threshold(tmd2725_instance_state *state,
                                               uint16_t high_thresh, uint16_t low_thresh);

/**
 * Update Proximity interrupt threshold registers.
 *
 * @param[i] state                 Instance state
 * @param[i] high_thresh           High threshold
 * @param[i] low_thresh            Low threshold
 *
 * @return none
 */
sns_rc tmd2725_update_prox_threshold(tmd2725_instance_state *state,
                                                uint8_t high_thresh, uint8_t low_thresh);

/**
 * Write ALS gain setting to register.
 *
 * @param[i] state                 Instance state
 * @param[i] gain                  Gain value
 *
 * @return none
 */
sns_rc tmd2725_set_als_gain(tmd2725_instance_state *state, uint32_t gain);

/**
 * Procee sensor data.
 *
 * @param[i] instance   Sensor Instance
 */
void tmd2725_process_sensor_data(sns_sensor_instance *const instance);

/**
 * TMD2725 proximity do factory calibration.
 *
 * @param[i] instance   Sensor Instance
 *
 * @return SNS_RC_SUCCESS if success
 */
sns_rc tmd2725_prox_factory_calibration(sns_sensor_instance *const instance);

/**
 * TMD2725 ALS do factory calibration.
 *
 * @param[i] instance   Sensor Instance
 *
 * @return SNS_RC_SUCCESS if success
 */
sns_rc tmd2725_als_factory_calibration(sns_sensor_instance *const instance);

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void tmd2725_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor ALS event.
 *
 * @param[i] instance   Sensor Instance
 * @param[i] timeout_time timestamp in ticks
 */
void tmd2725_handle_sensor_als_sample(sns_sensor_instance *const instance, uint64_t timeout_ticks);

/**
 * Sends sensor proximity event.
 *
 * @param[i] instance   Sensor Instance
 * @param[i] timeout_time timestamp in ticks
 */
void tmd2725_handle_sensor_prox_sample(sns_sensor_instance *const instance, uint64_t timeout_ticks);
