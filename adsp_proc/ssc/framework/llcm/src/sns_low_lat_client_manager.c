/*=============================================================================
  @file sns_low_lat_client_manager.c

  This module provides low latency sensor data channel by works with few
  other modules.
  This module receive sensor request via fastRPC call and write sensor
  data into shared memory.  Some APIs of this module are exported and
  called by fastRPC remote functions, these exported APIs receive sensor
  requests and this module will generate sensor client requests accordingly
  to sensor framework, then receive sensor events from sensor framework
  and write sensor data into shared memory which accessible to sensor data
  consumer.

  This file handles the sensor api part of the module implementation and
  includes APIs for fastRPC call.

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_assert.h"
#include "sns_attribute_util.h"
#include "sns_client.pb.h"
#include "sns_data_stream.h"
#include "sns_fw_log.h"
#include "sns_fw_sensor.h"
#include "sns_island.h"
#include "sns_low_lat_buffer_android.h"
#include "sns_low_lat_client_manager.h"
#include "sns_low_lat_client_manager_instance.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_register.h"
#include "sns_reg_util.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_suid_util.h"
#include "sns_timer.h"
#include "sns_types.h"

/*=============================================================================
  Macros & Definitions
  ===========================================================================*/
#define LLCM_REGISTRY_NAME "sns_llcm"
#define QCOM_VR_MODE  "qti_vr_mode"
/*=============================================================================
  Static Data
  ===========================================================================*/
static sns_sensor *llcm_this = NULL;
SNS_SECTION(".data.sns") sns_llcm_channel sns_llcm_channel_list[SNS_LLCM_MAX_CHANNELS];

extern sns_sensor_instance_api llcm_sensor_instance_api;

/* Lock for stream operations, global lock for all data streams and calibration streams*/
SNS_SECTION(".data.sns") sns_osa_lock llcm_stream_op_lock;
/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/*=============================================================================
  Static Sensor API Function Definitions
  ===========================================================================*/
/**
 * Request registry
 */
SNS_SECTION(".text.island")
static void llcm_request_registry(sns_sensor *const this)
{
  sns_service_manager* service_mgr = this->cb->get_service_manager(this);
  sns_stream_service* stream_service =
    (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                SNS_STREAM_SERVICE);
  sns_llcm_state *state = (sns_llcm_state*)this->state->state;
  uint8_t buffer[100];
  size_t encoded_len;

  if(NULL == state->registry_stream)
  {
    sns_sensor_uid registry_suid;

    sns_suid_lookup_get(&state->suid_lookup_data, "registry", &registry_suid);
    /* Create a data stream to Registry */
    stream_service->api->create_sensor_stream(stream_service,
                                              this,
                                              registry_suid,
                                              &state->registry_stream);
  }

  /* Send a request to Registry */
  sns_registry_read_req sns_llcm_registry;
  pb_buffer_arg data = (pb_buffer_arg)
  {
    .buf = LLCM_REGISTRY_NAME,
    .buf_len = (strlen(LLCM_REGISTRY_NAME) + 1)
  };

  sns_llcm_registry.name.arg = &data;
  sns_llcm_registry.name.funcs.encode = pb_encode_string_cb;

  sns_std_request registry_std_req = sns_std_request_init_default;
  encoded_len = pb_encode_request(buffer,
                                  sizeof(buffer),
                                  &sns_llcm_registry,
                                  sns_registry_read_req_fields,
                                  &registry_std_req);
  if(0 < encoded_len && NULL != state->registry_stream )
  {
    sns_request request = (sns_request){
     .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ,
     .request_len = encoded_len, .request = buffer };
    state->registry_stream->api->send_request(
      state->registry_stream, &request);
  }
}

/**
 * Parse registry
 */
SNS_SECTION(".text.island")
static void llcm_handle_registry_event(sns_sensor* const this)
{
  sns_llcm_state *state = (sns_llcm_state*)this->state->state;

  if(NULL != state->registry_stream)
  {
    sns_llcm_reg_config *config = &state->registry_config;
    sns_reg_util_table_item items[] =
    {
      { QCOM_VR_MODE, SNS_REG_TYPE_INT, &config->qc_vr_mode }
    };
    sns_reg_util_table groups[] =
    {
      { LLCM_REGISTRY_NAME, ARR_SIZE(items), items, false }
    };
    sns_reg_util_handle(state->registry_stream, groups, ARR_SIZE(groups));

    if(groups[0].group_recv)
    {      
      sns_sensor_util_remove_sensor_stream(this, &state->registry_stream);
    }
    
  }
}

/**
 * @See sns_sensor::notify_event.
 */
SNS_SECTION(".text.island")
static sns_rc sns_llcm_notify_event(
    sns_sensor *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_llcm_state *state = (sns_llcm_state*)this->state->state;

  sns_osa_lock_acquire(&llcm_stream_op_lock);

  sns_suid_lookup_handle(this, &state->suid_lookup_data);
  sns_suid_lookup_handle(this, &state->opt_suid_lookup_data);

  if (sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    llcm_request_registry(this);
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);
  }
  if (sns_suid_lookup_complete(&state->opt_suid_lookup_data))
  {
    sns_suid_lookup_deinit(this, &state->opt_suid_lookup_data);
  }

  llcm_handle_registry_event(this);

  sns_osa_lock_release(&llcm_stream_op_lock);

  return rc;
}

/**
 * Publish attributes of sensor to SEE.
 */
SNS_SECTION(".text.island")
static void llcm_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = SNS_LLCM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.str.funcs.encode = pb_encode_string_cb;
      value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = SNS_LLCM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.str.funcs.encode = pb_encode_string_cb;
      value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = SNS_LLCM_VENDOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.str.funcs.encode = pb_encode_string_cb;
      value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = SNS_LLCM_PROTO_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.str.funcs.encode = pb_encode_string_cb;
      value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_boolean = true;
      value.boolean     = true;
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_sint = true;
      value.sint     = 1;
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_sint = true;
      value.sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
      sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }
}

/**
 * Perform all initialization for the low latency Client Manager.
 *
 * @see sns_sensor::init
 */
SNS_SECTION(".text.island")
static sns_rc sns_llcm_init(
    sns_sensor *const this)
{
  uint32_t i;
  sns_rc rc = SNS_RC_SUCCESS;
  sns_llcm_state *state = (sns_llcm_state*)this->state->state;
  sns_osa_lock_attr lock_attr;

  /* Initilize state structure. */
  for (i = 0; i < ARR_SIZE(sns_llcm_channel_list); i++)
  {
    sns_llcm_channel_list[i].output_buf = NULL;
  }

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "gyro_cal");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "resampler");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");
  SNS_SUID_LOOKUP_INIT(state->opt_suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->opt_suid_lookup_data, "accel_cal");
  sns_suid_lookup_add(this, &state->opt_suid_lookup_data, "mag_cal");

  llcm_publish_attributes(this);

  /* Run at common VR mode by default */
  state->registry_config.qc_vr_mode = false;

  state->island_client = sns_island_aggregator_register_client("LLCM");
  state->scpm_client = sns_pwr_scpm_mgr_create_client();
  llcm_this = this;

  rc = sns_osa_lock_attr_init(&lock_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_NORMAL);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_init(&lock_attr, &llcm_stream_op_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  return rc;
}

/**
 * @See sns_sensor::deinit.
 */
SNS_SECTION(".text.island")
static sns_rc sns_llcm_deinit(sns_sensor* const this)
{
  sns_llcm_state *state = (sns_llcm_state*)this->state->state;

  llcm_this = NULL;

  sns_osa_lock_deinit(&llcm_stream_op_lock);
  sns_suid_lookup_deinit(this, &state->suid_lookup_data);
  sns_suid_lookup_deinit(this, &state->opt_suid_lookup_data);
  SNS_PRINTF(LOW, this, "sns_llcm_deinit");

  return SNS_RC_SUCCESS;
}

/**
 * Do nothing because this module will NOT be client
 * of any SSC module.
 *
 * @see See sns_sensor::set_client_request
 */
SNS_SECTION(".text.island")
static sns_sensor_instance* sns_llcm_set_client_request(
    sns_sensor *const this,
    sns_request const *curr_req,
    sns_request const *new_req,
    bool remove)
{
  UNUSED_VAR(this);
  UNUSED_VAR(curr_req);
  UNUSED_VAR(new_req);
  UNUSED_VAR(remove);

  return NULL;
}

/**
 * @See sns_sensor::get_sensor_uid
 */
SNS_SECTION(".text.island")
static sns_sensor_uid const *sns_llcm_get_sensor_uid(
    sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid =
  {
    .sensor_uid =
    { 0x35, 0x07, 0x62, 0x49, 0x25, 0x0c, 0x4f, 0x7b,
      0x92, 0xe1, 0xe3, 0xcf, 0xd1, 0x9c, 0xc9, 0x9d
    }
  };

  return &sensor_uid;
}

/**
 * @brief
 * This utility function is used to find an appropriate channel slot in the array.
 * If there's a channel that has a matching output buffer, then use that channel.
 * If not then use the first available channel slot in the array.
 *
 * @param[in] output_buf    Pointer of output buffer which look for matching channel.
 * @param[out] channel_id  Channel associated to the buffer.
 *
 * @return sns_rc
 */
static sns_rc sns_llcm_find_channel(void* output_buf, uint32_t *channel_id)
{
  uint32_t i;
  sns_rc ret = SNS_RC_INVALID_VALUE;

  *channel_id = SNS_LLCM_INVALID_STREAM_ID;
  /* Iterate through all of the channels */
  for (i = 0; i < ARR_SIZE(sns_llcm_channel_list); i++)
  {
    /* If a channel is found with a matching output_buf, return that index */
    if (output_buf == sns_llcm_channel_list[i].output_buf)
    {
      SNS_PRINTF(MED, llcm_this,
          "sns_llcm_find_channel - found matching output_buf at index %u", i);
      *channel_id = i;
      ret = SNS_RC_SUCCESS;
      break;
    }
    /* Keep track of the first unused channel slot, if we don't find a channel with a
        * attaching output_buf, we'll use the aforementioned channel slot.
        */
    if ((NULL == sns_llcm_channel_list[i].output_buf) &&
        (SNS_RC_INVALID_VALUE == ret))
    {
      SNS_PRINTF(MED, llcm_this,
          "sns_llcm_find_channel - found first open channel at index %u", i);
      *channel_id = i;
      ret = SNS_RC_SUCCESS;
      /* Not break here that allow search through all of the channels */
    }
  }

  return ret;
}

/**
 * @brief
 * Check if any valid channel in channel list.
 *
 * @param[in] state    Poniter to sensor state.
 *
 * @return TRUE if any valid channel, FALSE if cannot find any valid channel.
 */
static bool sns_llcm_is_any_valid_channel(sns_llcm_state *state)
{
  uint32_t i;
  UNUSED_VAR(state);
  /* Iterate through all of the channel slots */
  for (i = 0; i < ARR_SIZE(sns_llcm_channel_list); i++)
  {
    if (NULL != sns_llcm_channel_list[i].output_buf)
    {
      return true;
    }
  }

  return false;
}


/**
 * @brief
 * This utility function is used to initialize the circular buffer for the
 * android format
 *
 * @param[i]      Pointer of channel to be initialized.
 *
 * @return sns_rc
 */
static sns_rc sns_llcm_init_circ_buf_android(sns_llcm_channel* channel)
{
  sns_rc ret = SNS_RC_SUCCESS;

  channel->output_buf_max_samples = channel->output_buf_size / sizeof(sensors_event_t);

  /* Return an error if the max_samples is zero (buffer is too small) */
  if (0 == channel->output_buf_max_samples)
  {
    ret = SNS_RC_INVALID_VALUE;
    SNS_PRINTF(ERROR, llcm_this,
        "Buffer is too small! output_buf_size %u sensors_event_t size %u",
        channel->output_buf_size, sizeof(sensors_event_t));
  }

  /* Set the most recent index past the end. This is done so during the first
     * sample insert, when the index is incremented, the sample will end up in
     * the proper location.
     */
  channel->output_buf_most_recent_idx = channel->output_buf_max_samples;

  channel->atomic_counter = 0;

  SNS_PRINTF(HIGH, llcm_this, "sns_llcm_init_circ_buf_android - max_samples %d",
      channel->output_buf_max_samples);

  return ret;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api llcm_sensor_api SNS_SECTION(".rodata.island") =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_llcm_init,
  .deinit = &sns_llcm_deinit,
  .get_sensor_uid = &sns_llcm_get_sensor_uid,
  .set_client_request = &sns_llcm_set_client_request,
  .notify_event = &sns_llcm_notify_event,
};

/*===========================================================================
  Public Function Definitions
  ===========================================================================*/


sns_rc sns_register_llcm(
    sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_llcm_state), &llcm_sensor_api,
      &llcm_sensor_instance_api);

  return SNS_RC_SUCCESS;
}

/**
 * This function configure sensor in particular low latency channel.
 *
 * @param[in] output_buf          Point to sensor data output buffer.
 * @param[in] output_buf_size     Size of output buffer, in bytes.
 * @param[in] output_buf_offset   Write offset for first sensor data, give non-zero value if not start from begin of output buffer.
 * @param[in] timestamp_offset    Timestamp offset between AP and SSC.
 * @param[in] sensor_suid         Sensor to configure.
 * @param[in] sample_period_us    Sensor sampling period, set this to zero will stop the sensor.
 * @param[in] flags               Special configure flags, refer to "SNS_LLCM_FLAG_X"
 * @param[in] sensor_handle       Sensor identifier used in Android.
 *                                All sensors in same channel wil be stopped if handle is -1.
 * @param[out] stream_id          Pointer of Low lateny channel(stream) ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_config(void* output_buf,
                       unsigned int output_buf_size,
                       unsigned int output_buf_offset,
                       int64_t timestamp_offset,
                       const sns_std_suid *sensor_suid,
                       unsigned int sample_period_us,
                       unsigned int flags,
                       int  sensor_handle,
                       int* stream_id)
{
  sns_rc               rc = SNS_RC_SUCCESS;
  uint32_t             channel_id = 0;
  sns_llcm_config_req  llcm_config_req;
  sns_request          client_request;
  sns_llcm_state       *llcm_state;
  sns_sensor_instance  *shared_inst;
  sns_fw_sensor *sensor = (sns_fw_sensor*)llcm_this;

  /* Not allow call before init */
  SNS_ASSERT(NULL != llcm_this);

  SNS_PROFILE(SNS_LLCM_CONFIG_START, 1, sensor_handle);

  /* Validate inputs */
  if ((NULL == output_buf) ||
      (0 == output_buf_size) ||
      (NULL == sensor_suid) ||
      (SNS_LLCM_MIN_SAMPLE_PERIOD_US > sample_period_us && 0 != sample_period_us) ||
      (SNS_LLCM_MAX_SAMPLE_PERIOD_US < sample_period_us) ||
      (NULL == stream_id))
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_config - invalid inputs buffer=0x%x, size=%d, suid=0x%0x",
          output_buf, output_buf_size, sensor_suid);
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_config - invalid inputs sample_period=%d, flags=%u, stream_ptr=0x%x",
          sample_period_us, flags, stream_id);
    SNS_PROFILE(SNS_LLCM_CONFIG_END_ERROR, 0);
    return SNS_RC_INVALID_VALUE;
  }

  SNS_PRINTF(HIGH, llcm_this, "sns_llcm_config - output_buf=0x%x size=%u period_us=%u handle=%d flags=%d",
        output_buf, output_buf_size, sample_period_us, sensor_handle, flags);
  /* Initialize the stream_id */
  *stream_id = SNS_LLCM_INVALID_STREAM_ID;

  /* Find an appropriate channel in the stream list */
  rc = sns_llcm_find_channel(output_buf, &channel_id);
  if (SNS_RC_SUCCESS != rc)
  {
    /* If an open channel can't be found, then return an error code */
    SNS_PRINTF(ERROR, llcm_this, "Cannot find open channel");
    SNS_PROFILE(SNS_LLCM_CONFIG_END_ERROR, 0);
    return SNS_RC_FAILED;
  }
  SNS_PRINTF(MED, llcm_this, "sns_llcm_config - using channel %d", channel_id);

  llcm_state = (sns_llcm_state*)llcm_this->state->state;
  /* If this is a new channel, then initialize the object */
  if (NULL == sns_llcm_channel_list[channel_id].output_buf)
  {
    sns_llcm_channel_list[channel_id].output_buf = (void*) ((unsigned int)output_buf + output_buf_offset);
    sns_llcm_channel_list[channel_id].output_buf_size = output_buf_size;
    sns_isafe_list_init(&sns_llcm_channel_list[channel_id].sensor_client_list);
    qurt_signal2_init(&sns_llcm_channel_list[channel_id].signal);

    if (!(flags & SNS_LLCM_FLAG_ANDROID_STYLE_OUTPUT))
    {
      SNS_PRINTF(ERROR, llcm_this, "Not support other memory layout, use Android style layout anyway.");
    }
    if (0 != sns_llcm_init_circ_buf_android(&sns_llcm_channel_list[channel_id]))
    {
      SNS_PROFILE(SNS_LLCM_CONFIG_END_ERROR, 0);
      return SNS_RC_FAILED;
    }
  }
  *stream_id = channel_id;

  sns_osa_lock_acquire(sensor->library->library_lock);
  shared_inst = sns_sensor_util_get_shared_instance(llcm_this);
  sns_osa_lock_release(sensor->library->library_lock);
  if (NULL == shared_inst)
  {
    /* Create shared instance to get sensor event. */
    shared_inst = llcm_this->cb->create_instance(llcm_this, sizeof(sns_llcm_inst_state));
    if(NULL != shared_inst)
    {
      sns_island_block(llcm_state->island_client);
      sns_pwr_scpm_mgr_mcps_bump_up(llcm_state->scpm_client);
      SNS_PRINTF(HIGH, llcm_this, "Bump up SCPM.");
      sns_pwr_sleep_mgr_set_latency(SNS_LLCM_WAKEUP_LATENCY_US);
    }
  }

  llcm_config_req.channel_id = channel_id;
  llcm_config_req.sample_period_us = sample_period_us;
  llcm_config_req.sensor_suid = sensor_suid;
  llcm_config_req.sensor_handle = sensor_handle;
  llcm_config_req.calibrated_stream = flags & SNS_LLCM_FLAG_CALIBRATED_STREAM;
  llcm_config_req.timestamp_offset_ns = timestamp_offset;
  llcm_config_req.offset_update_only = false;

  client_request.message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  client_request.request_len = sizeof(sns_llcm_config_req);
  client_request.request = &llcm_config_req;
  sns_osa_lock_acquire(sensor->library->library_lock);
  llcm_this->instance_api->set_client_config(shared_inst, &client_request);
  sns_osa_lock_release(sensor->library->library_lock);

  /* Remove instance if no more active client */
  if ((NULL != shared_inst) && (!sns_llcm_is_any_valid_channel(llcm_state)))
  {
    sns_osa_lock_acquire(sensor->library->library_lock);
    //sns_osa_lock_acquire(&llcm_stream_op_lock);
    llcm_this->cb->remove_instance(shared_inst);
    //sns_osa_lock_release(&llcm_stream_op_lock);
    sns_osa_lock_release(sensor->library->library_lock);

    sns_island_unblock(llcm_state->island_client);
    sns_pwr_scpm_mgr_mcps_bump_down(llcm_state->scpm_client);
    SNS_PRINTF(HIGH, llcm_this, "Bump down SCPM.");
    sns_pwr_sleep_mgr_release_latency();
  }
  SNS_PROFILE(SNS_LLCM_CONFIG_END, 0);

  return rc;
}

/**
 * Disable all sensors in particular low latency channel.
 *
 * @param[in] stream_id            Low lateny channel ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_disable(int stream_id)
{
  sns_llcm_config_req     llcm_config_req;
  sns_request             config;
  sns_llcm_state          *state;
  sns_sensor_instance     *shared_inst;
  sns_std_suid            suid = {0, 0};
  sns_fw_sensor *sensor = (sns_fw_sensor*)llcm_this;

  SNS_ASSERT(NULL != llcm_this);

  SNS_PRINTF(MED, llcm_this, "sns_llcm_disable - stream_id %d", stream_id);

  /* Validate input */
  if ((0 > stream_id) || (ARR_SIZE(sns_llcm_channel_list) <= stream_id))
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_disable - invalid input, stream_id %d",
        stream_id);
    return SNS_RC_INVALID_VALUE;
  }

  state = (sns_llcm_state*)llcm_this->state->state;
  sns_osa_lock_acquire(sensor->library->library_lock);
  shared_inst = sns_sensor_util_get_shared_instance(llcm_this);
  sns_osa_lock_release(sensor->library->library_lock);

  if (NULL == shared_inst)
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_disable - invalid sensor instance");
    return SNS_RC_INVALID_VALUE;
  }

  llcm_config_req.sensor_suid = &suid;
  llcm_config_req.channel_id = stream_id;
  llcm_config_req.sample_period_us = 0;
  llcm_config_req.sensor_handle = -1;
  llcm_config_req.calibrated_stream = false;
  llcm_config_req.offset_update_only = false;

  config.message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  config.request_len = sizeof(sns_llcm_config_req);
  config.request = &llcm_config_req;
  sns_osa_lock_acquire(sensor->library->library_lock);
  llcm_this->instance_api->set_client_config(shared_inst, &config);
  sns_osa_lock_release(sensor->library->library_lock);

  /* Reset the channel slot */
  sns_llcm_channel_list[stream_id].output_buf = NULL;
  sns_llcm_channel_list[stream_id].output_buf_size = 0;
  qurt_signal2_destroy(&sns_llcm_channel_list[stream_id].signal);

  /* Remove instance if active client */
  if ((NULL != shared_inst) && (!sns_llcm_is_any_valid_channel(state)))
  {
    sns_osa_lock_acquire(sensor->library->library_lock);
    //sns_osa_lock_acquire(&llcm_stream_op_lock);
    llcm_this->cb->remove_instance(shared_inst);
    //sns_osa_lock_release(&llcm_stream_op_lock);
    sns_osa_lock_release(sensor->library->library_lock);

    sns_island_unblock(state->island_client);
    sns_pwr_scpm_mgr_mcps_bump_down(state->scpm_client);
    SNS_PRINTF(HIGH, llcm_this, "Bump down SCPM.");
    sns_pwr_sleep_mgr_release_latency();
  }
  return SNS_RC_SUCCESS;
}

/**
 * Polling particular low latency channel for new sensor data.
 *
 * @param[in] stream_id            Low lateny channel ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_poll(int stream_id)
{
  unsigned int  signal;
  int           sig_wait_error;

  SNS_ASSERT(NULL != llcm_this);

  if ((0 > stream_id) || (ARR_SIZE(sns_llcm_channel_list) <= stream_id))
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_poll - invalid input, stream_id %d",
        stream_id);
    return SNS_RC_INVALID_VALUE;
  }

  /* Use cancellable version that allow the wait be cancelled when stream is disabled */
  sig_wait_error = qurt_signal2_wait_cancellable(&sns_llcm_channel_list[stream_id].signal,
      SNS_LLCM_SIGNAL_IND | SNS_LLCM_SIGNAL_CANCEL_POLL,
      QURT_SIGNAL_ATTR_WAIT_ANY,
      &signal);
  if (QURT_EOK != sig_wait_error)
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_poll - error waiting for SIGNAL_IND, error %d, stream_id %d",
      sig_wait_error, stream_id);
    return SNS_RC_FAILED;
  }

  /* Clear any lingering signals */
  qurt_signal2_clear(&sns_llcm_channel_list[stream_id].signal,
      SNS_LLCM_SIGNAL_IND | SNS_LLCM_SIGNAL_CANCEL_POLL);

  /* Return an error if this polling call needs to be canceled */
  if (signal & SNS_LLCM_SIGNAL_CANCEL_POLL)
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_poll - canceled polling call, stream_id %d",
      stream_id);
    return SNS_RC_INVALID_STATE;
  }

  return SNS_RC_SUCCESS;
}

/**
 * Update the timestamp offset between AP and SSC
 *
 * @param[in] timestamp_offset       Timestamp offset between AP and SSC.
 * @param[in] slope       Slope of timestamp offset change, this is reserved for future use.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_offset_update(int64_t timestamp_offset,
                       int64_t slope)
{
  sns_llcm_config_req  llcm_config_req;
  sns_request          client_request;
  sns_sensor_instance  *shared_inst;
  sns_std_suid         suid = {0, 0};
  sns_fw_sensor *sensor = (sns_fw_sensor*)llcm_this;

  UNUSED_VAR(slope);

  SNS_ASSERT(NULL != llcm_this);

  SNS_PRINTF(LOW, llcm_this, "sns_llcm_offset_update offset HI:0x%x LO:0x%x",
  	SNS_LLCM_FETCH_HI_32(timestamp_offset),
  	SNS_LLCM_FETCH_LOW_32(timestamp_offset));

  sns_osa_lock_acquire(sensor->library->library_lock);
  shared_inst = sns_sensor_util_get_shared_instance(llcm_this);
  sns_osa_lock_release(sensor->library->library_lock);
  if (NULL == shared_inst)
  {
    SNS_PRINTF(ERROR, llcm_this, "sns_llcm_offset_update - invalid sensor instance");
    return SNS_RC_INVALID_VALUE;
  }

  llcm_config_req.sensor_suid = &suid;
  llcm_config_req.channel_id = 0;
  llcm_config_req.sample_period_us = 0;
  llcm_config_req.sensor_handle = 0;
  llcm_config_req.offset_update_only = true;
  llcm_config_req.timestamp_offset_ns = timestamp_offset;

  client_request.message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  client_request.request_len = sizeof(sns_llcm_config_req);
  client_request.request = &llcm_config_req;
  sns_osa_lock_acquire(sensor->library->library_lock);
  llcm_this->instance_api->set_client_config(shared_inst, &client_request);
  sns_osa_lock_release(sensor->library->library_lock);

  return SNS_RC_SUCCESS;
}
