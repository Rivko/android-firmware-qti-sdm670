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
 * Report test result.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 * @param[i] test_result Test result
 *
 * @return none
 */
static void shtw2_send_com_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
      sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

/**
 * @brief Read sensor ID register.
 *
 * @param[in] *scp_service        Ptr to the COM port service
 * @param[in] *port_handle        Ptr to the COM port handle
 *
 * @return
 *   SNS_RC_SUCCESS on success, otherwise error code.
 */
sns_rc shtw2_check_ID_register(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle)
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t cmd[SHTW2_CMD_SIZE_BYTES] = SHTW2_CMD_READ_ID_REG;
  uint8_t read_buffer[SHTW2_RESP_FRAME_SIZE_BYTES];
  uint32_t xfer_bytes;

  rv = scp_service->api->sns_scp_simple_rw(port_handle,
                                           true,
                                           false,
                                           cmd,
                                           SHTW2_CMD_SIZE_BYTES,
                                           &xfer_bytes);

  if(rv != SNS_RC_SUCCESS || xfer_bytes != SHTW2_CMD_SIZE_BYTES)
  {
    return SNS_RC_FAILED;
  }

  rv = scp_service->api->sns_scp_simple_rw(port_handle,
                                           false,
                                           false,
                                           read_buffer,
                                           SHTW2_RESP_FRAME_SIZE_BYTES,
                                           &xfer_bytes);

  if(rv != SNS_RC_SUCCESS || xfer_bytes != SHTW2_RESP_FRAME_SIZE_BYTES)
  {
    return SNS_RC_FAILED;
  }

  /* Check CRC. */
  if(!shtw2_check_crc(read_buffer))
  {
    return SNS_RC_FAILED;
  }

  /* Check ID register content. */
  if((read_buffer[1] & SHTW2_ID_REG_MASK) != SHTW2_ID_REG_CONTENT)
  {
    return SNS_RC_FAILED;
  }

  return rv;
}

/**
 * Runs a communication test - probes sensor and publishes self test result.
 *
 * @param[i] instance  Instance reference
 *
 * @return none
 */
void shtw2_run_self_test(sns_sensor_instance *instance)
{
  shtw2_instance_state *state = (shtw2_instance_state*)instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  bool probe_success = false;

  rv = shtw2_check_ID_register(state->scp_service,
                               state->com_port_info.port_handle);
  if(rv == SNS_RC_SUCCESS)
  {
    probe_success = true;
  }

  if(state->temperature_info.test_info.test_client_present &&
     state->temperature_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
  {
    shtw2_send_com_test_event(instance, &state->temperature_info.suid, probe_success);
    state->temperature_info.test_info.test_client_present = false;
  }
  if(state->humidity_info.test_info.test_client_present &&
     state->humidity_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
  {
    shtw2_send_com_test_event(instance, &state->humidity_info.suid, probe_success);
    state->humidity_info.test_info.test_client_present = false;
  }
}
