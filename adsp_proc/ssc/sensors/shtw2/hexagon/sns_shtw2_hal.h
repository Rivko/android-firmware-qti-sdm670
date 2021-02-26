#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <stdint.h>

#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_shtw2_sensor_instance.h"

/**
 * Sensor communication.
 */
#define SHTW2_CMD_READ_ID_REG       {0xef, 0xc8}
#define SHTW2_CMD_SOFT_RESET        {0x80, 0x5d}
#define SHTW2_CMD_START_MEASUREMENT {0x78, 0x66}
#define SHTW2_MEAS_DURATION_US      14400
#define SHTW2_CMD_SIZE_BYTES        2
#define SHTW2_RESP_DATA_SIZE_BYTES  2
#define SHTW2_RESP_CRC_SIZE_BYTES   1
#define SHTW2_RESP_FRAME_SIZE_BYTES (SHTW2_RESP_DATA_SIZE_BYTES + \
                                      SHTW2_RESP_CRC_SIZE_BYTES)
#define SHTW2_TEMPERATURE_FRAME_START 0
#define SHTW2_HUMIDITY_FRAME_START  SHTW2_RESP_FRAME_SIZE_BYTES
#define SHTW2_READ_SIZE_BYTES       (2 * SHTW2_RESP_FRAME_SIZE_BYTES)
#define SHTW2_ID_REG_MASK           0x3F
#define SHTW2_ID_REG_CONTENT        0x07
#define SHTW2_CRC_POLYNOMIAL        0x31
#define SHTW2_CRC_INIT              0xFF
#define SHTW2_OFF_TO_IDLE_MS        1

/**
 * Sensor and driver information.
 */
#define SHTW2_SLEEP_CURRENT_UA        1
#define SHTW2_ACTIVE_CURRENT_UA       5         // average with 1Hz
#define SHTW2_VENDOR_NAME             "Sensirion AG"
#define SHTW2_DRIVER_NAME             "Sensirion SHTW2 driver"
#define SHTW2_TEMPERATURE_SENSOR_NAME "ambient_temperature"
#define SHTW2_HUMIDITY_SENSOR_NAME    "humidity"

/**
 * Sensor value calculations.
 */
#define SHTW2_FRAME_TO_TICKS(f)       ((uint16_t)(f)[1] + ((f)[0]<<8))
#define SHTW2_TICKS_TO_TEMPERATURE(t) \
    (-45.0f + ((int32_t)(t) * 175.0f) / 65535.0f)
#define SHTW2_FRAME_TO_TEMPERATURE(f) \
    SHTW2_TICKS_TO_TEMPERATURE(SHTW2_FRAME_TO_TICKS((f)))
#define SHTW2_TICKS_TO_HUMIDITY(t)    (((int32_t)(t) * 100.0f) / 65535.0f)
#define SHTW2_FRAME_TO_HUMIDITY(f)    \
    SHTW2_TICKS_TO_HUMIDITY(SHTW2_FRAME_TO_TICKS((f)))

#define SHTW2_NUM_AXES                (1)

/******************* Function Declarations ***********************************/

/**
 * Checks ID register.
 *
 * @param[in] *scp_service        Ptr to the COM port service
 * @param[in] *port_handle        Ptr to the COM port handle
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure or SHTW2 sensor not found
 * SNS_RC_SUCCESS - SHTW2 sensor found
 */
sns_rc shtw2_check_ID_register(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle);

/**
 * @brief Start a SHTW2 measurement.
 *
 * @detail
 *   Initiates a temperature and humidity measurement.
 *
 * @param[in] *scp_service        Ptr to the COM port service
 * @param[in] *port_handle        Ptr to the COM port handle
 *
 * @return
 *   SNS_RC_SUCCESS on success, otherwise error code.
 */
sns_rc shtw2_start_measurement(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle);

/**
 * @brief Read SHTW2 measurement.
 *
 * @detail
 *   Read results from a previously started temperature and humidity measurement.
 *
 * @param[in] *scp_service             Ptr to the COM port service
 * @param[in] *port_handle             Ptr to the COM port handle
 * @param[in] temperature_registry_cfg Temperature factory calibration params
 * @param[in] humidity_registry_cfg    Humidity factory calibration params
 * @param[out] *temperature_info       Ptr to the temperature reading
 * @param[out] *humidity_info          Ptr to the humidity reading
 *
 * @return
 *   SNS_RC_SUCCESS on success, otherwise error code.
 */
sns_rc shtw2_read_measurement(sns_sync_com_port_service *scp_service,
                              sns_sync_com_port_handle *port_handle,
                              sns_shtw2_registry_cfg temperature_registry_cfg,
                              sns_shtw2_registry_cfg humidity_registry_cfg,
                              shtw2_temperature_info *temperature_info,
                              shtw2_humidity_info *humidity_info);

/**
* Runs a communication test - probes sensor and publishes self test result.
*
* @param[i] instance  Instance reference
*
* @return none
*/
void shtw2_run_self_test(sns_sensor_instance *instance);

/**
 * @brief Compares checksum with calculated CRC.
 *
 * @detail
 *   Checks if the bytes received have the same CRC as the CRC received.
 *
 * @param[in] *data        Ptr to the received data
 *
 * @return
 *   True on success, otherwise false.
 */
bool shtw2_check_crc(uint8_t *data);
