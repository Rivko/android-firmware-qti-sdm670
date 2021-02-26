/**
 * @file sns_test_std_sensor_island.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_cal.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_test_sensor.h"
#include "sns_test_std_sensor.h"
#include "sns_types.h"

/** See sns_test_std_sensor.h */
void sns_test_std_sensor_process_event(const sns_sensor *sensor,
                                  void* event,
                                  uint32_t event_len,
                                  void* test_data,
                                  uint32_t message_id,
                                  sns_time timestamp)
{
  switch (message_id)
  {
    case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG:
    /* TODO Add handling for sensors that will use std sensor config event */
    
    break;
    
    case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT:
    {
      SNS_PRINTF(MED, sensor, "test_std_sensor: sns_sensor_stream_msgid_sns_std_sensor_config_event");

      sns_std_sensor_physical_config_event phy_sensor_config =
          sns_std_sensor_physical_config_event_init_default;

      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event,
                                                   event_len);

      if (!pb_decode(&stream, sns_std_sensor_physical_config_event_fields, &phy_sensor_config))
      {
        SNS_PRINTF(ERROR, sensor, "pb_decode() failed for stream_event");
      }
      else
      {
        SNS_PRINTF(MED, sensor, "has_sample_rate = %d sample_rate = [%d/100]",
                                 phy_sensor_config.has_sample_rate, (int32_t)(phy_sensor_config.sample_rate*100));
        SNS_PRINTF(MED, sensor, "wm = %d  resolution = [%d/1000]  is_synch = %d",
                                 phy_sensor_config.water_mark, (int32_t)(phy_sensor_config.resolution*1000),
                                 phy_sensor_config.stream_is_synchronous);
        SNS_PRINTF(MED, sensor, "min_range = [%d/1000] max_range = [%d/1000] active_current = %d",
                                 (int32_t)(phy_sensor_config.range[0]*1000), (int32_t)(phy_sensor_config.range[1]*1000),
                                 phy_sensor_config.active_current);
      }
    }
    break;
    case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT:
    {
      float data[20] = { 0 };
      uint8_t arr_index = 0;
      uint8_t i;
      sns_std_sensor_event imu_event = sns_std_sensor_event_init_default;
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event,
                                                   event_len);
      pb_float_arr_arg arg = {
         .arr = data,
         .arr_len = ARR_SIZE(data),
         .arr_index = &arr_index
      };

      imu_event.data = (struct pb_callback_s) {
        .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg
      };

      if (!pb_decode(&stream, sns_std_sensor_event_fields, &imu_event))
      {
        SNS_PRINTF(ERROR, sensor, "pb_decode() failed for stream_event");
      }

      sns_test_std_sensor_data* tdata = (sns_test_std_sensor_data*)test_data;

      if (tdata->num_events > 0)
      {
        float delta = timestamp - tdata->last_ts;
        /* calculate running average of delta timestamps */
        tdata->avg_delta = (tdata->avg_delta * tdata->num_events + delta)
            / (tdata->num_events + 1);
      }
      tdata->last_ts = timestamp;
      tdata->num_events++;

      SNS_PRINTF(MED, sensor, "sensor sample #%04u: ts=%u ticks,"
                               " avg_delta=%u ticks",
                               tdata->num_events, (uint32_t)timestamp,
                               (uint32_t)tdata->avg_delta);
      for(i = 0; i < arr_index; i++)
      {
        SNS_PRINTF(LOW, sensor, "value[%d] = [%d/1000]", i, (int32_t)(data[i]*1000));
      }

    }
      break;
    default:
      SNS_PRINTF(ERROR, sensor, "incorrect message_id %u", message_id);
  }
}
