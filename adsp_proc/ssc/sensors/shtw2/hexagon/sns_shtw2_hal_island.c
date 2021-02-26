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
#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_com_port_types.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_service_manager.h"
#include "sns_shtw2_hal.h"
#include "sns_shtw2_sensor.h"
#include "sns_shtw2_sensor_instance.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_timer.pb.h"
#include "sns_types.h"

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
bool shtw2_check_crc(uint8_t *data)
{
  uint8_t crc = SHTW2_CRC_INIT;
  uint8_t data_length = SHTW2_RESP_DATA_SIZE_BYTES;
  uint8_t current_byte;
  uint8_t bit;

  /* Calculates 8-Bit checksum with given polynomial. */
  for (current_byte = 0; current_byte < data_length; ++current_byte) {
      crc ^= (data[current_byte]);
      for (bit = 8; bit > 0; --bit) {
          if (crc & 0x80)
              crc = (crc << 1) ^ SHTW2_CRC_POLYNOMIAL;
          else
              crc = (crc << 1);
      }
  }

  return crc == (data)[SHTW2_RESP_FRAME_SIZE_BYTES - 1];
}

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
                               sns_sync_com_port_handle *port_handle)
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t cmd[SHTW2_CMD_SIZE_BYTES] = SHTW2_CMD_START_MEASUREMENT;
  uint32_t xfer_bytes;

  rv = scp_service->api->sns_scp_simple_rw(port_handle,
                                           true,
                                           false,
                                           cmd,
                                           SHTW2_CMD_SIZE_BYTES,
                                           &xfer_bytes);

  if(rv != SNS_RC_SUCCESS || xfer_bytes != SHTW2_CMD_SIZE_BYTES)
  {
    rv = SNS_RC_FAILED;
  }

  return rv;
}

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
                              shtw2_humidity_info *humidity_info)
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t read_buffer[SHTW2_READ_SIZE_BYTES];
  uint32_t xfer_bytes;
  float temperature;
  float humidity;
  float diff;

  rv = scp_service->api->sns_scp_simple_rw(port_handle,
                                           false,
                                           false,
                                           read_buffer,
                                           SHTW2_READ_SIZE_BYTES,
                                           &xfer_bytes);

  if(rv != SNS_RC_SUCCESS || xfer_bytes != SHTW2_READ_SIZE_BYTES)
  {
    return SNS_RC_FAILED;
  }

  if (!shtw2_check_crc(&read_buffer[SHTW2_TEMPERATURE_FRAME_START]) ||
      !shtw2_check_crc(&read_buffer[SHTW2_HUMIDITY_FRAME_START]))
  {
    return SNS_RC_FAILED;
  }

  temperature = SHTW2_FRAME_TO_TEMPERATURE(&read_buffer[SHTW2_TEMPERATURE_FRAME_START]);
  humidity = SHTW2_FRAME_TO_HUMIDITY(&read_buffer[SHTW2_HUMIDITY_FRAME_START]);

  if(temperature_info->enabled)
  {
    /* Factory calibration using scale and bias */
    temperature = temperature_registry_cfg.fac_cal_scale[0] *
                  (temperature - temperature_registry_cfg.fac_cal_bias[0]);
    diff = (temperature_info->latest_reading < temperature) ?
           (temperature - temperature_info->latest_reading) :
           (temperature_info->latest_reading - temperature);

    if(diff > temperature_registry_cfg.threshold)
    {
      temperature_info->latest_reading = temperature;
      temperature_info->report_sample = true;
    }
    else
    {
      temperature_info->report_sample = false;
    }
  }
  if(humidity_info->enabled)
  {
    /* Factory calibration using scale and bias */
    humidity = humidity_registry_cfg.fac_cal_scale[0] *
               (humidity - humidity_registry_cfg.fac_cal_bias[0]);
    diff = (humidity_info->latest_reading < humidity) ?
    (humidity - humidity_info->latest_reading) :
    (humidity_info->latest_reading - humidity);

    if(diff > humidity_registry_cfg.threshold)
    {
      humidity_info->latest_reading = humidity;
      humidity_info->report_sample = true;
    }
    else
    {
      humidity_info->report_sample = false;
    }
  }

  return rv;
}
