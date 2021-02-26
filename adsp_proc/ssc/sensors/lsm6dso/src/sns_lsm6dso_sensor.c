/**
 * @file sns_lsm6dso_sensor.c
 *
 * Common implementation for LSM6DSO Sensors.
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
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
 *
 **/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_service.h"
#include "sns_sensor_util.h"
#include "sns_types.h"
#include "sns_attribute_util.h"

#include "sns_lsm6dso_sensor.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"

#define LSM6DSO_REG_CONFIG_ACCEL            "lsm6dso_0.accel.config"
#define LSM6DSO_REG_CONFIG_GYRO             "lsm6dso_0.gyro.config"
#define LSM6DSO_REG_CONFIG_TEMP             "lsm6dso_0.temp.config"
#define LSM6DSO_REG_CONFIG_MD               "lsm6dso_0.md.config"
#define LSM6DSO_REG_PLATFORM_CONFIG         "lsm6dso_0_platform.config"
#define LSM6DSO_REG_PLATFORM_PLACEMENT      "lsm6dso_0_platform.placement"
#define LSM6DSO_REG_PLATFORM_ORIENT         "lsm6dso_0_platform.orient"
#define LSM6DSO_REG_PLATFORM_FAC_CAL_ACCEL  "lsm6dso_0_platform.accel.fac_cal"
#define LSM6DSO_REG_PLATFORM_FAC_CAL_GYRO   "lsm6dso_0_platform.gyro.fac_cal"
#define LSM6DSO_REG_PLATFORM_FAC_CAL_TEMP   "lsm6dso_0_platform.temp.fac_cal"
#define LSM6DSO_REG_PLATFORM_CONFIG_MD      "lsm6dso_0_platform.md.config"

#define MAX_DEP_LENGTH 30

// temp structure for pb arg
typedef struct pb_arg_my
{
  sns_sensor_instance* sensor;
  const char*       name;
}pb_arg_my;

static char def_dependency[][MAX_DEP_LENGTH] =  {
  "interrupt", "async_com_port", "timer", "data_acquisition_engine", "registry"
};
static const char name[] = NAME;
static const char vendor[] = VENDOR;
static const uint32_t version = SNS_VERSION_LSM6DSO; // major[31:16].minor[15:0]

static const uint32_t max_fifo_depth = LSM6DSO_MAX_FIFO; // samples

static const sns_std_sensor_stream_type stream_type = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
static const bool is_dynamic = false;
static const sns_std_sensor_rigid_body_type rigid_body = SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY;
static const uint32_t hardware_id = 0;
static const bool supports_dri = true;
static const bool supports_sync_stream = false;

void lsm6dso_publish_def_attributes(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;

  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->available;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = version;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = max_fifo_depth;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR};
    char const op_mode1[] = LSM6DSO_LPM;
    char const op_mode2[] = LSM6DSO_NORMAL;
    char const op_mode3[] = LSM6DSO_HIGH_PERF;

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
    values[1].str.funcs.encode = pb_encode_string_cb;
    values[1].str.arg = &((pb_buffer_arg)
        { .buf = op_mode2, .buf_len = sizeof(op_mode2) });
    values[2].str.funcs.encode = pb_encode_string_cb;
    values[2].str.arg = &((pb_buffer_arg)
        { .buf = op_mode3, .buf_len = sizeof(op_mode3) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
        values, ARR_SIZE(values), false);
  }
  {
    float data[3] = {0};
    state->encoded_event_len =
        pb_get_encoded_size_sensor_stream_event(data, 3);
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->encoded_event_len;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = stream_type;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = is_dynamic;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
  }
{
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = rigid_body;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = 0;
    values[1].has_flt = true;
    values[1].flt = 0;
    values[2].has_flt = true;
    values[2].flt = 0;
    values[3].has_flt = true;
    values[3].flt = 0;
    values[4].has_flt = true;
    values[4].flt = 0;
    values[5].has_flt = true;
    values[5].flt = 0;
    values[6].has_flt = true;
    values[6].flt = 0;
    values[7].has_flt = true;
    values[7].flt = 0;
    values[8].has_flt = true;
    values[8].flt = 0;
    values[9].has_flt = true;
    values[9].flt = 0;
    values[10].has_flt = true;
    values[10].flt = 0;
    values[11].has_flt = true;
    values[11].flt = 0;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = hardware_id;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = supports_dri;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = supports_sync_stream;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, true);
  }
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void publish_registry_attributes(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->is_dri;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->supports_sync_stream;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->hardware_id;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    for(uint8_t i = 0; i < 12; i++)
    {
      values[i].has_flt = true;
      values[i].flt = state->placement[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }

  if(state->sensor == LSM6DSO_ACCEL ||
     state->sensor == LSM6DSO_GYRO)
  {
    /** Only accel and gyro use registry information for selected resolution. */
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_flt = true;
    value.flt = (state->sensor == LSM6DSO_ACCEL) ?
       lsm6dso_accel_resolutions[state->accel_resolution_idx] :
       lsm6dso_gyro_resolutions[state->gyro_resolution_idx];
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1, false);

    /** Only accel and gyro use registry information for selected range. */
    sns_std_attr_value_data values[] = {SNS_ATTR};
    sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
    rangeMinMax[0].has_flt = true;
    rangeMinMax[0].flt = (state->sensor == LSM6DSO_ACCEL) ?
       lsm6dso_accel_range_min[state->accel_resolution_idx] :
       lsm6dso_gyro_range_min[state->gyro_resolution_idx];
    rangeMinMax[1].has_flt = true;
    rangeMinMax[1].flt = (state->sensor == LSM6DSO_ACCEL) ?
       lsm6dso_accel_range_max[state->accel_resolution_idx] :
       lsm6dso_gyro_range_max[state->gyro_resolution_idx];
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
  }
}

static void publish_available(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
  state->available = true;
}

static void send_registry_request(sns_sensor *const this, char *reg_group_name)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){
    .buf = reg_group_name,
    .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
                                  &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    sns_rc rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    if(SNS_RC_SUCCESS == rc)
    {
      state->outstanding_reg_requests++;
    }
  }
  else
  {
    DBG_PRINT(state->diag_service, this, ERROR, __FILENAME__, __LINE__,
              "send_registry_request:: group not sent");
  }
}

static void send_registry_requests(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;

  if(LSM6DSO_ACCEL == state->sensor)
  {
    send_registry_request(this, LSM6DSO_REG_PLATFORM_CONFIG);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_PLACEMENT);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_ORIENT);

    send_registry_request(this, LSM6DSO_REG_CONFIG_ACCEL);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_FAC_CAL_ACCEL);
  }
  else if(LSM6DSO_GYRO == state->sensor)
  {
    send_registry_request(this, LSM6DSO_REG_CONFIG_GYRO);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_FAC_CAL_GYRO);
  }
  else if(LSM6DSO_SENSOR_TEMP == state->sensor)
  {
    send_registry_request(this, LSM6DSO_REG_CONFIG_TEMP);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_FAC_CAL_TEMP);
  }
  else if(LSM6DSO_MOTION_DETECT == state->sensor)
  {
    send_registry_request(this, LSM6DSO_REG_CONFIG_MD);
    send_registry_request(this, LSM6DSO_REG_PLATFORM_CONFIG_MD);
  }
}

static void process_registry_suid(sns_sensor *const this, sns_sensor_uid *reg_suid)
{
  sns_sensor *sensor;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service  *stream_svc  =
    (sns_stream_service*) service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    lsm6dso_state *state = (lsm6dso_state*)sensor->state->state;

    state->reg_suid = *reg_suid;

    stream_svc->api->create_sensor_stream(stream_svc, sensor, *reg_suid,
                                          &state->reg_data_stream);
    if(NULL != state->reg_data_stream)
    {
      send_registry_requests(sensor);

      DBG_PRINT(state->diag_service, sensor, HIGH, __FILENAME__, __LINE__,
                "process_registry_suid:: sensor=%u reg_stream=%x #req=%u",
                state->sensor, state->reg_data_stream, state->outstanding_reg_requests);
    }
    else
    {
      DBG_PRINT(state->diag_service, sensor, ERROR, __FILENAME__, __LINE__,
                "Failed to create registry stream");
    }
  }
}

static void sensor_save_registry_pf_cfg(
  sns_sensor *const this,
  sns_registry_phy_sensor_pf_cfg const * phy_sensor_pf_cfg)
{
  sns_rc rc;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_diag_service *diag = state->diag_service;

  state->rigid_body_type = phy_sensor_pf_cfg->rigid_body_type;
  state->com_port_info.com_config.bus_type = phy_sensor_pf_cfg->bus_type;
  state->com_port_info.com_config.bus_instance = phy_sensor_pf_cfg->bus_instance;
  state->com_port_info.com_config.slave_control = phy_sensor_pf_cfg->slave_config;
  state->com_port_info.com_config.min_bus_speed_KHz = phy_sensor_pf_cfg->min_bus_speed_khz;
  state->com_port_info.com_config.max_bus_speed_KHz = phy_sensor_pf_cfg->max_bus_speed_khz;
  state->com_port_info.com_config.reg_addr_type = phy_sensor_pf_cfg->reg_addr_type;
  state->irq_config.interrupt_num = phy_sensor_pf_cfg->dri_irq_num;
  state->irq_config.interrupt_pull_type = phy_sensor_pf_cfg->irq_pull_type;
  state->irq_config.is_chip_pin = phy_sensor_pf_cfg->irq_is_chip_pin;
  state->irq_config.interrupt_drive_strength = phy_sensor_pf_cfg->irq_drive_strength;
  state->irq_config.interrupt_trigger_type = phy_sensor_pf_cfg->irq_trigger_type;
  state->rail_config.num_of_rails = phy_sensor_pf_cfg->num_rail;
  state->registry_rail_on_state = phy_sensor_pf_cfg->rail_on_state;
  sns_strlcpy(state->rail_config.rails[0].name,
              phy_sensor_pf_cfg->vddio_rail,
              sizeof(state->rail_config.rails[0].name));
  sns_strlcpy(state->rail_config.rails[1].name,
              phy_sensor_pf_cfg->vdd_rail,
              sizeof(state->rail_config.rails[1].name));

  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
            state->com_port_info.com_config.min_bus_speed_KHz,
            state->com_port_info.com_config.max_bus_speed_KHz,
            state->com_port_info.com_config.reg_addr_type);

  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d",
            state->irq_config.interrupt_num,
            state->irq_config.interrupt_pull_type,
            state->irq_config.is_chip_pin);

  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "interrupt_drive_strength:%d interrupt_trigger_type:%d"
            " rigid body type:%d", state->irq_config.interrupt_drive_strength,
            state->irq_config.interrupt_trigger_type,
            phy_sensor_pf_cfg->rigid_body_type);

  /**-----------------Register and Open COM Port-------------------------*/
  state->scp_service =  (sns_sync_com_port_service *)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  rc = state->scp_service->api->sns_scp_register_com_port(
    &state->com_port_info.com_config,
    &state->com_port_info.port_handle);

  if(rc == SNS_RC_SUCCESS)
  {
    rc = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
  }
  else
  {
    SNS_PRINTF(ERROR, this, "sns_scp_register_com_port fail rc:%u",rc);
  }

  /**---------------------Register Power Rails --------------------------*/
  if(NULL == state->pwr_rail_service && rc == SNS_RC_SUCCESS)
  {
    state->rail_config.rail_vote = SNS_RAIL_OFF;

    state->pwr_rail_service =
      (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                      SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->rail_config);
  }
}

static void process_registry_event(sns_sensor *const this, sns_sensor_event *event)
{
  bool rv = true;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_diag_service *diag = state->diag_service;

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_PRINTF(ERROR, this, "Error decoding registry event");
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == strncmp((char*)group_name.buf, LSM6DSO_REG_CONFIG_ACCEL,
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, LSM6DSO_REG_CONFIG_GYRO,
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, LSM6DSO_REG_CONFIG_TEMP,
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, LSM6DSO_REG_CONFIG_MD,
                      group_name.buf_len))
      {
        sns_registry_phy_sensor_cfg sensor_cfg = {0,0,0,0};
        sns_registry_decode_arg arg = {
          .item_group_name = &group_name,
          .parse_info_len = 1,
          .parse_info[0] = {
            .group_name = "config",
            .parse_func = sns_registry_parse_phy_sensor_cfg,
            .parsed_buffer = &sensor_cfg
          }
        };

        read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
        read_event.data.items.arg = &arg;

        rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

        if(rv)
        {
          state->is_dri               = sensor_cfg.is_dri;
          state->hardware_id          = sensor_cfg.hw_id;
          if(state->sensor == LSM6DSO_ACCEL)
          {
            state->accel_resolution_idx      = sensor_cfg.res_idx;
            DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                      "Accel resolution_idx:%d",
                      state->accel_resolution_idx);
          }
          else if(state->sensor == LSM6DSO_GYRO)
          {
            state->gyro_resolution_idx       = sensor_cfg.res_idx;
            DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                      "Gyro resolution_idx:%d",
                      state->gyro_resolution_idx);
          }
          else
          {
          state->resolution_idx       = sensor_cfg.res_idx;
            DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                      "resolution_idx:%d",
                      state->accel_resolution_idx);
          }

          state->supports_sync_stream = sensor_cfg.sync_stream;

          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
		"supports_sync_stream:%d ",state->supports_sync_stream);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, LSM6DSO_REG_PLATFORM_CONFIG,
                           group_name.buf_len))
      {
        sns_registry_phy_sensor_pf_cfg phy_sensor_pf_cfg;
        memset(&phy_sensor_pf_cfg, 0, sizeof(phy_sensor_pf_cfg));
        sns_registry_decode_arg arg = {
          .item_group_name = &group_name,
          .parse_info_len = 1,
          .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
              .parsed_buffer = &phy_sensor_pf_cfg
          }
        };

        read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
        read_event.data.items.arg = &arg;

        rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

        if(rv)
        {
          sensor_save_registry_pf_cfg(this, &phy_sensor_pf_cfg);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, LSM6DSO_REG_PLATFORM_PLACEMENT,
                         group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->placement,
            .arr_index = &arr_index,
            .arr_len = 12
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "placement",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, LSM6DSO_REG_PLATFORM_ORIENT,
                             group_name.buf_len))
      {
        sns_registry_decode_arg arg = {
          .item_group_name = &group_name,
          .parse_info_len = 1,
          .parse_info[0] = {
            .group_name = "orient",
            .parse_func = sns_registry_parse_axis_orientation,
            .parsed_buffer = state->axis_map
          }
        };

        read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
        read_event.data.items.arg = &arg;

        rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

        if(rv)
        {
          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Input Axis:%d maps to Output Axis:%d with inversion %d",
                    state->axis_map[0].ipaxis,
                    state->axis_map[0].opaxis, state->axis_map[0].invert);

          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Input Axis:%d maps to Output Axis:%d with inversion %d",
                    state->axis_map[1].ipaxis, state->axis_map[1].opaxis,
                    state->axis_map[1].invert);

          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Input Axis:%d maps to Output Axis:%d with inversion %d",
                    state->axis_map[2].ipaxis, state->axis_map[2].opaxis,
                    state->axis_map[2].invert);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           LSM6DSO_REG_PLATFORM_FAC_CAL_ACCEL,
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                           LSM6DSO_REG_PLATFORM_FAC_CAL_GYRO,
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                           LSM6DSO_REG_PLATFORM_FAC_CAL_TEMP,
                           group_name.buf_len))
      {
        {
          uint8_t bias_arr_index = 0, scale_arr_index = 0;
          pb_float_arr_arg bias_arr_arg = {
            .arr = state->fac_cal_bias,
            .arr_index = &bias_arr_index,
            .arr_len = TRIAXIS_NUM
          };

          pb_float_arr_arg scale_arr_arg = {
            .arr = state->fac_cal_scale,
            .arr_index = &scale_arr_index,
            .arr_len = TRIAXIS_NUM
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 3,
            .parse_info[0] = {
              .group_name = "bias",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &bias_arr_arg
            },
            .parse_info[1] = {
              .group_name = "scale",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &scale_arr_arg
            },
            .parse_info[2] = {
              .group_name = "corr_mat",
              .parse_func = sns_registry_parse_corr_matrix_3,
              .parsed_buffer = &state->fac_cal_corr_mat
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          // TODO: implement the below logic once facility is available from Qualcomm framework
//          state->registry_persist_version = <version_from_registry>;
          if(state->fac_cal_scale[0] != 0.0)
          {
            state->fac_cal_corr_mat.e00 = state->fac_cal_scale[0];
            state->fac_cal_corr_mat.e11 = state->fac_cal_scale[1];
            state->fac_cal_corr_mat.e22 = state->fac_cal_scale[2];
          }

          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Fac Cal Corr Matrix e00:%d e01:%d e02:%d",
                     state->fac_cal_corr_mat.e00,state->fac_cal_corr_mat.e01,
                     state->fac_cal_corr_mat.e02);
          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Fac Cal Corr Matrix e10:%d e11:%d e12:%d",
                     state->fac_cal_corr_mat.e10,state->fac_cal_corr_mat.e11,
                     state->fac_cal_corr_mat.e12);
          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Fac Cal Corr Matrix e20:%d e21:%d e22:%d",
                     state->fac_cal_corr_mat.e20,state->fac_cal_corr_mat.e21,
                     state->fac_cal_corr_mat.e22);
          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "Fac Cal Bias x:%d y:%d z:%d",
                     state->fac_cal_bias[0], state->fac_cal_bias[1],
                     state->fac_cal_bias[2]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           LSM6DSO_REG_PLATFORM_CONFIG_MD,
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = LSM6DSO_REG_PLATFORM_CONFIG_MD,
              .parse_func = sns_registry_parse_md_cfg,
              .parsed_buffer = &state->md_config }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                    "MD Threshold(*1000):%d m/s2 MD Window(*1000):%d sec MD Disable :%d",
                    (int)(state->md_config.thresh*1000),(int) (state->md_config.win*1000), state->md_config.disable);
        }
      }
      else
      {
        rv = false;
      }

      if(!rv)
      {
        SNS_PRINTF(ERROR, this, "Error decoding registry group ");
      }
    }
    state->outstanding_reg_requests--;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u",
                             event->message_id);
  }
}

static bool
encode_group_for_registry(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  pb_arg_my* pb_arg = (pb_arg_my*)*arg;
  lsm6dso_instance_state *state = (lsm6dso_instance_state*)pb_arg->sensor->state->state;

  if (0 == strncmp(pb_arg->name,"bias",strlen("bias")))
  {
    char const *names[] = {"x", "y", "z"};

    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                      { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.has_version = true;

      if(((state->self_test_info.sensor == LSM6DSO_ACCEL) && !(state->registry_reset.request)) || ((state->registry_reset.sensor_type == LSM6DSO_ACCEL) && (state->registry_reset.request)))
      {
        if(state->registry_reset.request)
        {
          state->accel_registry_cfg.fac_cal_bias[i] = 0;
        }
        pb_item.flt = state->accel_registry_cfg.fac_cal_bias[i];
        pb_item.version = state->accel_registry_cfg.registry_persist_version;
      }
      else
      {
        if(state->registry_reset.request)
        {
          state->gyro_registry_cfg.fac_cal_bias[i] = 0;
        }
        pb_item.flt = state->gyro_registry_cfg.fac_cal_bias[i];
        pb_item.version = state->gyro_registry_cfg.registry_persist_version;
      }
      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_INST_PRINTF(ERROR,pb_arg->sensor,"Error encoding sns_registry_data_item_fields");
        return false;
      }
    }
  }
  #if SUPPORT_SCALE_FACTOR
  else if (0 == strncmp(pb_arg->name,"scale",strlen("scale")))
  {
    char const *names[] = {"sf_0", "sf_1", "sf_2"};
    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
//      pb_item.flt = state->persist_state.output.scale_factor[i];
      pb_item.flt = 0;
      pb_item.has_version = true;
      pb_item.version = state->accel_registry_cfg.registry_persist_version;

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_INST_PRINTF(ERROR,pb_arg->sensor,"Error encoding sns_registry_data_item_fields");
        return false;
      }
    }
  }
  #endif
  else if (0 == strncmp(pb_arg->name,"corr_mat",strlen("corr_mat")))
  {
    char const *names[] = {"0_0", "0_1", "0_2",
                           "1_0", "1_1", "1_2",
                           "2_0", "2_1", "2_2",};

    float identity_data[] = {1,0,0,0,1,0,0,0,1};
    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                             { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
//      pb_item.flt = state->persist_state.output.compensation_matrix[i];
      pb_item.has_version = true;
      if(((state->self_test_info.sensor == LSM6DSO_ACCEL) && !(state->registry_reset.request)) || ((state->registry_reset.sensor_type == LSM6DSO_ACCEL) && (state->registry_reset.request)))
      {
        if(state->registry_reset.request)
        {
          state->accel_registry_cfg.fac_cal_corr_mat.data[i] = identity_data[i];
        }
        pb_item.flt = state->accel_registry_cfg.fac_cal_corr_mat.data[i];
        pb_item.version = state->accel_registry_cfg.registry_persist_version;
      }
      else
      {
        if(state->registry_reset.request)
        {
          state->gyro_registry_cfg.fac_cal_corr_mat.data[i] = identity_data[i];
        }
        pb_item.flt = state->gyro_registry_cfg.fac_cal_corr_mat.data[i];
        pb_item.version = state->gyro_registry_cfg.registry_persist_version;
      }

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_INST_PRINTF(ERROR,pb_arg->sensor,"Error encoding sns_registry_data_item_fields");
        return false;
      }
    }
  }
  return true;
}

static bool
sns_send_to_registry_persist_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  sns_sensor_instance *this = (sns_sensor_instance*)*arg;
//  lsm6dso_instance_state *state = (lsm6dso_instance_state*)this->state->state;

//  char const *names[] = {"bias", "scale","corr_mat"};
//  char const *names[] = {"bias"};
#if SUPPORT_SCALE_FACTOR
  char const *names[] = {"bias", "scale","corr_mat"};
#else
  char const *names[] = {"bias", "corr_mat"};
#endif

  for(int i = 0; i < ARR_SIZE(names); i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_arg_my pb_arg= (pb_arg_my){
      .name = NULL,.sensor= this
    };

    pb_item.name.arg = &name_data;
    pb_item.name.funcs.encode = &pb_encode_string_cb;

    if (0==strncmp(names[i],"bias",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;

    }
#if SUPPORT_SCALE_FACTOR
    else if (0==strncmp(names[i],"scale",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;

    }
#endif
    else if (0==strncmp(names[i],"corr_mat",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_INST_PRINTF(ERROR, this,"Error encoding corr_mat");

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_INST_PRINTF(ERROR, this,"Error encoding sns_registry_data_item_fields");
      return false;
    }
  }

  return true;
}

static void send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;

  if(state->fw_stream == NULL)
  {
    sns_service_manager *manager = this->cb->get_service_manager(this);
    sns_stream_service *stream_service =
      (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
     stream_service->api->create_sensor_stream(stream_service, this, sns_get_suid_lookup(),
                                               &state->fw_stream);
  }

  if(state->fw_stream != NULL)
  {
    size_t encoded_len;
    pb_buffer_arg data = (pb_buffer_arg){ .buf = data_type, .buf_len = data_type_len };
    uint8_t buffer[50];

    sns_suid_req suid_req = sns_suid_req_init_default;
    suid_req.has_register_updates = true;
    suid_req.register_updates = true;
    suid_req.data_type.funcs.encode = &pb_encode_string_cb;
    suid_req.data_type.arg = &data;
    sns_rc rc = SNS_RC_SUCCESS;

    encoded_len = pb_encode_request(buffer, sizeof(buffer), &suid_req, sns_suid_req_fields, NULL);
    if(0 < encoded_len)
    {
      sns_request request = (sns_request){
        .request_len = encoded_len, .request = buffer, .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
      rc = state->fw_stream->api->send_request(state->fw_stream, &request);
    }
    if(0 >= encoded_len || SNS_RC_SUCCESS != rc)
    {
      DBG_PRINT(state->diag_service, this, ERROR, __FILENAME__, __LINE__,
                "encoded_len=%d rc=%u", encoded_len, rc);
    }
  }
}

static void init_dependencies(sns_sensor *const this)
{
  int i = 0;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;

  DBG_PRINT(state->diag_service, this, LOW, __FILENAME__, __LINE__, "init_dependencies sensor");

  for(i=0;i<ARR_SIZE(def_dependency);i++)
  {
    send_suid_req(this, def_dependency[i], strlen(def_dependency[i]));
  }
}

void lsm6dso_init_sensor_info(sns_sensor *const this,
                              sns_sensor_uid const *suid,
                              lsm6dso_sensor_type sensor_type)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  struct sns_service_manager *smgr= this->cb->get_service_manager(this);

  state->diag_service = (sns_diag_service *)
    smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service = (sns_sync_com_port_service*)
    smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);

  state->sensor_client_present = false;
  state->sensor = sensor_type;
  sns_memscpy(&state->my_suid, sizeof(state->my_suid), suid, sizeof(sns_sensor_uid));

  DBG_PRINT(state->diag_service, this, LOW, __FILENAME__, __LINE__, "lsm6dso_init_sensor_info sensor %d", sensor_type);

  if(LSM6DSO_MOTION_DETECT != sensor_type)
  {
    // initialize fac cal correction matrix to identity
    state->fac_cal_corr_mat.e00 = 1.0;
    state->fac_cal_corr_mat.e11 = 1.0;
    state->fac_cal_corr_mat.e22 = 1.0;
    state->registry_persist_version = 0.0;
  }

  // Do this for all sesors since axis mapping is the same for all sensors supported by LSM6DSO
  // i.e. accel, gyro, temp, MD
  for(uint32_t i = 0; i < ARR_SIZE(state->axis_map); i++)
  {
    state->axis_map[i].ipaxis = i;
    state->axis_map[i].opaxis = i;
    state->axis_map[i].invert = false;
  }

  if(LSM6DSO_ACCEL == sensor_type)
  {
    init_dependencies(this);
  }
}

void lsm6dso_process_suid_events(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_data_stream *stream = state->fw_stream;
  sns_service_manager *service_mgr;
  sns_stream_service  *stream_svc;

  if(NULL == stream || 0 == stream->api->get_input_cnt(stream))
  {
    return;
  }

  service_mgr = this->cb->get_service_manager(this);
  stream_svc = (sns_stream_service*) service_mgr->get_service(service_mgr,
                                                              SNS_STREAM_SERVICE);
  for(sns_sensor_event *event = stream->api->peek_input(stream);
      NULL != event;
      event = stream->api->get_next_input(stream))
  {
    if(SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
    {
      sns_diag_service* diag = state->diag_service;
      pb_istream_t pbstream = pb_istream_from_buffer((void*)event->event, event->event_len);
      sns_suid_event suid_event = sns_suid_event_init_default;
      pb_buffer_arg data_type_arg = { .buf = NULL, .buf_len = 0 };
      sns_sensor_uid uid_list;
      sns_suid_search suid_search;
      suid_search.suid = &uid_list;
      suid_search.num_of_suids = 0;

      suid_event.data_type.funcs.decode = &pb_decode_string_cb;
      suid_event.data_type.arg = &data_type_arg;
      suid_event.suid.funcs.decode = &pb_decode_suid_event;
      suid_event.suid.arg = &suid_search;

      if(!pb_decode(&pbstream, sns_suid_event_fields, &suid_event))
      {
         DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__, "pb_decode() failed");
         continue;
       }

      /* if no suids found, ignore the event */
      if(suid_search.num_of_suids == 0)
      {
        continue;
      }

      /* save suid based on incoming data type name */
      if(0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len))
      {
        state->irq_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
      {
        state->timer_suid = uid_list;
        stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                              &state->timer_stream);
        if(NULL == state->timer_stream)
        {
          DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                    "process_suid_events:: Failed to create timer stream");
        }
      }
      else if (0 == strncmp(data_type_arg.buf, "async_com_port",
                            data_type_arg.buf_len))
      {
        state->acp_suid = uid_list;
      }
      else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
      {
        process_registry_suid(this, &uid_list);
      }
      else if (0 == strncmp(data_type_arg.buf, "data_acquisition_engine",
                            data_type_arg.buf_len))
      {
        state->dae_suid = uid_list;
      }
      else
      {
        DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                  "process_suid_events:: invalid datatype_name");
      }
    }
  }
  return;
}

sns_rc lsm6dso_process_registry_events(sns_sensor *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_data_stream *stream = state->reg_data_stream;
  sns_diag_service *diag = state->diag_service;

  if(NULL == stream || 0 == stream->api->get_input_cnt(stream))
  {
    return rv;
  }

  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "process_registry_event:: sensor=%u stream=%x",
            state->sensor, stream);

  for(; 0 != stream->api->get_input_cnt(stream); stream->api->get_next_input(stream))
  {
    sns_sensor_event *event = stream->api->peek_input(stream);
    process_registry_event(this, event);
  }
  if (state->outstanding_reg_requests == 0)
  {
    sns_sensor_util_remove_sensor_stream(this, &state->reg_data_stream);
  }
  return rv;
}

bool lsm6dso_sensor_write_output_to_registry(
  sns_sensor *const this,
  sns_sensor_instance *const instance)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  lsm6dso_instance_state *inst_state =
  (lsm6dso_instance_state*)instance->state->state;

  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  uint8_t buffer[1000];
  int32_t encoded_len;
  char accel_name[] = LSM6DSO_REG_PLATFORM_FAC_CAL_ACCEL;
  char gyro_name[] = LSM6DSO_REG_PLATFORM_FAC_CAL_GYRO;
  pb_buffer_arg name_data;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  if(((inst_state->self_test_info.sensor == LSM6DSO_ACCEL) && !(inst_state->registry_reset.request)) || ((inst_state->registry_reset.sensor_type == LSM6DSO_ACCEL) && (inst_state->registry_reset.request)))
  {
    name_data = (pb_buffer_arg)
          { .buf = accel_name, .buf_len = strlen(accel_name) + 1 };
  }
  else
  {
    name_data = (pb_buffer_arg)
          { .buf = gyro_name, .buf_len = strlen(gyro_name) + 1 };
  }

  write_req.name.funcs.encode = &pb_encode_string_cb;
  write_req.name.arg = &name_data;
  write_req.data.items.funcs.encode = &sns_send_to_registry_persist_cb;
  write_req.data.items.arg = instance;

  // updating version number for persistent data
  state->registry_persist_version++;
  if(((inst_state->self_test_info.sensor == LSM6DSO_ACCEL) && !(inst_state->registry_reset.request)) || ((inst_state->registry_reset.sensor_type == LSM6DSO_ACCEL) && (inst_state->registry_reset.request)))
  {
    inst_state->accel_registry_cfg.registry_persist_version = state->registry_persist_version;
  }
  else
  {
    inst_state->gyro_registry_cfg.registry_persist_version = state->registry_persist_version;
  }

  if(NULL == state->reg_data_stream)
  {
    stream_svc->api->create_sensor_stream(stream_svc, this, state->reg_suid,
                                          &state->reg_data_stream);
  }
  if(NULL == state->reg_data_stream)
  {
    return false;
  }
  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &write_req, sns_registry_write_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
    state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
  return true;
}

void lsm6dso_discover_hw(sns_sensor *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_diag_service *diag = state->diag_service;
  uint8_t buffer[1];

  /**-------------------Read and Confirm WHO-AM-I------------------------*/
  buffer[0] = 0x0;
  DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
      "Calling who am i");
  rv = lsm6dso_get_who_am_i(state->scp_service,state->com_port_info.port_handle, &buffer[0]);
  DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
            "Error = 0x%x : WhoAmI=0x%x", rv, buffer[0]);

  if(rv == SNS_RC_SUCCESS
      &&
      buffer[0] == LSM6DSO_WHOAMI_VALUE)
  {
    DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
        "Got LSM6DSO");
    sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
    if(instance)
    {
      DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
                "Instance available resetting device");
      // Reset Sensor
      rv = lsm6dso_reset_device(instance,
          LSM6DSO_ACCEL | LSM6DSO_GYRO | LSM6DSO_MOTION_DETECT | LSM6DSO_SENSOR_TEMP);
      if(rv == SNS_RC_SUCCESS)
      {
        state->hw_is_present = true;
      }
    }
    else
    {
      DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                "Instance Not available");
      state->hw_is_present = true;
    }
  }
  else
  {
    DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
              "Error = 0x%x : WhoAmI=0x%x", rv, buffer[0]);
  }
  state->who_am_i = buffer[0];

  /**------------------Power Down and Close COM Port--------------------*/
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
  state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);

  /**----------------------Turn Power Rail OFF--------------------------*/
  state->rail_config.rail_vote = SNS_RAIL_OFF;
  state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                           this,
                                                           &state->rail_config,
                                                           NULL);
}

void lsm6dso_update_siblings(sns_sensor *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  bool all_suids_available = true;

  if(   SUID_IS_NULL(&state->irq_suid)
     || SUID_IS_NULL(&state->acp_suid)
     || SUID_IS_NULL(&state->timer_suid)
     || SUID_IS_NULL(&state->reg_suid)
     || SUID_IS_NULL(&state->dae_suid)
    )
  {
    all_suids_available = false;
  }

  if(all_suids_available)
  {
    sns_sensor *lib_sensor;
    sns_service_manager *smgr= this->cb->get_service_manager(this);

    for(lib_sensor = this->cb->get_library_sensor(this, true);
        NULL != lib_sensor;
        lib_sensor = this->cb->get_library_sensor(this, false))
    {
      lsm6dso_state *lib_state = (lsm6dso_state*)lib_sensor->state->state;

      DBG_PRINT(lib_state->diag_service, lib_sensor, HIGH, __FILENAME__, __LINE__,
                "update_siblings:: sensor=%u", lib_state->sensor);

      if(lib_sensor != this)
      {
        sns_sync_com_port_service *scp_service;

        lib_state->irq_suid              = state->irq_suid;
        lib_state->acp_suid              = state->acp_suid;
        lib_state->dae_suid              = state->dae_suid;
        lib_state->timer_suid            = state->timer_suid;
        lib_state->accel_resolution_idx  = state->accel_resolution_idx;
        if(lib_state->sensor == LSM6DSO_GYRO)
          state->gyro_resolution_idx  = lib_state->gyro_resolution_idx;
        lib_state->sensor_client_present = false;
        lib_state->hw_is_present         = true;
        lib_state->who_am_i              = state->who_am_i;
        lib_state->rigid_body_type       = state->rigid_body_type;
        lib_state->com_port_info         = state->com_port_info;
        lib_state->irq_config            = state->irq_config;
        lib_state->rail_config           = state->rail_config;

        sns_memscpy(lib_state->axis_map, sizeof(lib_state->axis_map),
                    state->axis_map, sizeof(state->axis_map));
        sns_memscpy(lib_state->placement, sizeof(lib_state->placement),
                    state->placement, sizeof(state->placement));
        lib_state->scp_service = scp_service =
          (sns_sync_com_port_service*)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
        lib_state->pwr_rail_service =
          (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);
        scp_service->api->sns_scp_register_com_port(&lib_state->com_port_info.com_config,
                                                    &lib_state->com_port_info.port_handle);
      }
      publish_registry_attributes(lib_sensor);
      publish_available(lib_sensor);
    }
    sns_sensor_util_remove_sensor_stream(this, &state->fw_stream);
  }
}

