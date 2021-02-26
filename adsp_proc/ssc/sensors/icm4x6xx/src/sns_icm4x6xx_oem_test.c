#include "sns_icm4x6xx_oem_test.h"
#ifdef ICM4X6XX_OEM_TEST
#include "sns_icm4x6xx_sensor.h"
#include "sns_physical_sensor_oem_config.pb.h"
#include "sns_pb_util.h"

static bool icm4x6xx_get_decoded_oem_config_request(sns_sensor const *this,
                sns_request const *in_request, sns_std_request *decoded_request,
                sns_physical_sensor_oem_config *decoded_payload)
{
   pb_istream_t stream;

  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_physical_sensor_oem_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    ICM4X6XX_PRINTF(ERROR, this, "decode error - oem config");
    return false;
  }

  return true;
}

static void icm4x6xx_send_oem_config_done(sns_sensor_instance *const instance, sns_sensor_uid *suid)
{
  ICM4X6XX_INST_PRINTF(LOW, instance, "oem config event");

  sns_physical_sensor_oem_config_event oem_config_event =
      sns_physical_sensor_oem_config_event_init_default;

  oem_config_event.config_result = true;
  oem_config_event.config_type = SNS_PHYSICAL_SENSOR_CONFIG_BIAS;

  pb_send_event(instance,
          sns_physical_sensor_oem_config_event_fields,
          &oem_config_event,
          sns_get_system_time(),
          SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG_EVENT,
          suid);
}

static void icm4x6xx_set_oem_config(sns_sensor *const this, sns_sensor_instance *instance,
                                   struct sns_request const *request)
{
  icm4x6xx_instance_state *inst_state = (icm4x6xx_instance_state*)instance->state->state;
  icm4x6xx_state *state = (icm4x6xx_state *) this->state->state;
  sns_sensor_uid suid;
  sns_sensor_uid* suid_ptr = NULL;
  sns_std_request decoded_request;
  sns_physical_sensor_oem_config decoded_payload_oem =
      sns_physical_sensor_oem_config_init_default;

  if (state->sensor == ICM4X6XX_ACCEL || state->sensor == ICM4X6XX_GYRO) {
      suid_ptr = &state->my_suid;
  }

  if(suid_ptr == NULL)
  {
    return;
  }

  sns_memscpy(&suid, sizeof(suid), suid_ptr, sizeof(sns_sensor_uid));

  if(icm4x6xx_get_decoded_oem_config_request(this, request,
                                            &decoded_request, &decoded_payload_oem))
  {
    if(decoded_payload_oem.config_type == SNS_PHYSICAL_SENSOR_CONFIG_BIAS)
    {
      if(state->sensor == ICM4X6XX_ACCEL)
      {
        inst_state->accel_registry_cfg.fac_cal_bias[0] = decoded_payload_oem.offset_x;
        inst_state->accel_registry_cfg.fac_cal_bias[1] = decoded_payload_oem.offset_y;
        inst_state->accel_registry_cfg.fac_cal_bias[2] = decoded_payload_oem.offset_z;
        inst_state->accel_registry_cfg.version++;
        ICM4X6XX_PRINTF(LOW, this,
            "set accel bias values*1000: X=%d Y=%d Z=%d",
            (int32_t)(inst_state->accel_registry_cfg.fac_cal_bias[0]*1000),
            (int32_t)(inst_state->accel_registry_cfg.fac_cal_bias[1]*1000),
            (int32_t)(inst_state->accel_registry_cfg.fac_cal_bias[2]*1000));
      }
      else if(state->sensor == ICM4X6XX_GYRO)
      {
        inst_state->gyro_registry_cfg.fac_cal_bias[0] = decoded_payload_oem.offset_x;
        inst_state->gyro_registry_cfg.fac_cal_bias[1] = decoded_payload_oem.offset_y;
        inst_state->gyro_registry_cfg.fac_cal_bias[2] = decoded_payload_oem.offset_z;
        inst_state->gyro_registry_cfg.version++;
        ICM4X6XX_PRINTF(LOW, this,
            "set gyro bias values*1000: X=%d Y=%d Z=%d",
            (int32_t)(inst_state->gyro_registry_cfg.fac_cal_bias[0]*1000),
            (int32_t)(inst_state->gyro_registry_cfg.fac_cal_bias[1]*1000),
            (int32_t)(inst_state->gyro_registry_cfg.fac_cal_bias[2]*1000));
      }

      icm4x6xx_send_oem_config_done(instance, &suid);
	  icm4x6xx_send_cal_event(instance, state->sensor);
	  icm4x6xx_update_sensor_state(this, instance);
    }
  }
}

void icm4x6xx_handle_oem_request(sns_sensor *const this, sns_sensor_instance *instance,
                                struct sns_request const *request)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;

  if(request->message_id == SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG)
  {
	ICM4X6XX_INST_PRINTF(LOW, instance, "icm4x6xx_handle_oem_request message_id :%d sensor: %d", request->message_id, state->sensor);
    if((state->sensor == ICM4X6XX_ACCEL) || (state->sensor == ICM4X6XX_GYRO))
	{
		icm4x6xx_set_oem_config(this, instance, request);
	}
  }

  ICM4X6XX_INST_PRINTF(LOW, instance, "oem config event");
}
#endif

