/*=============================================================================
  @file sns_resampler_sensor_island.c

  The Resampler virtual Sensor implementation

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_resampler_sensor.h"
#include "sns_resampler_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_sensor_util_internal.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_types.h"

/*=============================================================================
 * Non-island functions
 ============================================================================*/

extern sns_rc
sns_resampler_notify_event(sns_sensor *const this);

extern sns_rc
sns_resampler_init(sns_sensor *const this);

sns_rc
sns_resampler_deinit(sns_sensor *const this);

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
sns_resampler_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid resampler_suid = {.sensor_uid = {RESAMPLER_SUID}};
  return &resampler_suid;
}

/**
 * See sns_sensor_util_instance_match_cb
 */
static bool
find_instance_match(struct sns_request const *request,
   sns_sensor_instance const *instance)
{
  resampler_instance_state const *inst_state =
      (resampler_instance_state*)instance->state->state;
  resampler_request *resampler_req = (resampler_request *)request->request;

  //TODO: If a client is asking for a resampled rate that is
  // within 5% of a sensor's ODR, that instance can be shared
  // with other clients also asking for within 5% of the ODR
  // because resampler does not interpolate for those clients.
  return (0 == sns_memcmp(&inst_state->client_config,
                          &resampler_req->config,
                          sizeof(resampler_req->config)));
}

/*
 * Create sensor instance. Initialize all pointers and data.
 */
static sns_sensor_instance*
create_instance_and_initialize(sns_sensor * const this,
    resampler_request* resampler_req)
{
  uint32_t axis_cnt;
  sns_sensor_instance* rv_inst;
  if(resampler_req->config.has_axis_cnt)
  {
    axis_cnt = resampler_req->config.axis_cnt;
    if(axis_cnt > AXIS_CNT_MAX)
    {
      SNS_PRINTF(ERROR, this,
          "Could not spawn new instance: Axis count too large : %u", axis_cnt);
      return NULL;
    }
  }
  else
  {
    //keeping the default size 6 if client does not specify axis_cnt
    axis_cnt = AXIS_CNT_DEFAULT;
  }
  //there are 3 variable sized arrays in the resampler_instance_state
  rv_inst = this->cb->create_instance(this,
      ALIGN_8(sizeof(resampler_instance_state))
          + VARIABLE_SIZE_ARRAY_COUNT * sizeof(float) * axis_cnt);
  resampler_instance_state* inst_state =
      (resampler_instance_state*) rv_inst->state->state;
  inst_state->axis_cnt = axis_cnt;
  inst_state->cic_obj.accumulator = (float*) ((uint8_t*) inst_state
      + ALIGN_8(sizeof(resampler_instance_state)));
  inst_state->interp_obj.sample1.data =
      (float*) (inst_state->cic_obj.accumulator + inst_state->axis_cnt);
  inst_state->interp_obj.sample2.data =
      (float*) (inst_state->interp_obj.sample1.data + inst_state->axis_cnt);
  inst_state->interp_obj.sample1.axis_cnt_bytes = sizeof(float)
      * inst_state->axis_cnt;
  inst_state->interp_obj.sample2.axis_cnt_bytes = sizeof(float)
      * inst_state->axis_cnt;
  inst_state->cic_obj.axis_cnt_bytes = sizeof(float) * inst_state->axis_cnt;
  //Initialize it to max accuracy and then assign it the min status of all samples used
  //for filtering
  inst_state->cic_obj.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  return rv_inst;
}

/*
 * Update the resampler std request based on output of utility function.
 * Call set client request for the given instance
 */
static void resampler_set_client_config(sns_sensor *const this,
    sns_sensor_instance* instance,
    sns_request* decoded_req)
{
  resampler_request* resampler_req = (resampler_request*)decoded_req->request;
  if(resampler_req->std_request.batching.batch_period > 0)
  {
    resampler_req->std_request.has_batching = true;
  }
  else
  {
    resampler_req->std_request.has_batching = false;
  }

  if(resampler_req->std_request.batching.flush_period > 0)
  {
    resampler_req->std_request.has_batching = true;
    resampler_req->std_request.batching.has_flush_period = true;
  }

  if(resampler_req->std_request.batching.max_batch)
  {
    resampler_req->std_request.has_batching = true;
    resampler_req->std_request.batching.has_max_batch= true;
  }
  if(resampler_req->std_request.batching.flush_only)
  {
    resampler_req->std_request.has_batching = true;
    resampler_req->std_request.batching.has_flush_only = true;
  }
  this->instance_api->set_client_config(instance,decoded_req);
}

/*
 * Re-evaluate and update the std_request each time a client is removed.
 */
static void reval_resampler_request(sns_sensor *const this,
    sns_sensor_instance* instance)
{
  sns_request const* reval_request = instance->cb->get_client_request(instance,
      sns_resampler_get_sensor_uid(this), true);
  if(NULL != reval_request){
    pb_simple_cb_arg arg;
    pb_istream_t stream;
    resampler_request resampler_req =
    {
      .std_request = sns_std_request_init_default,
      .config = sns_resampler_config_init_default
    };
    arg.decoded_struct = &resampler_req.config;
    arg.fields = sns_resampler_config_fields;
    resampler_req.std_request.payload = (struct pb_callback_s)
        {.funcs.decode = &pb_decode_simple_cb, .arg = &arg};
    stream = pb_istream_from_buffer(reval_request->request, reval_request->request_len);
    if(pb_decode(&stream, sns_std_request_fields, &resampler_req.std_request))
    {
      sns_request decoded_req = {.message_id = reval_request->message_id,
          .request_len = sizeof(resampler_req), .request = &resampler_req };
      sns_rc rc;
      rc = sns_sensor_util_find_consolidated_req(instance,
        sns_resampler_get_sensor_uid(this),
        &resampler_req.std_request);
      if (SNS_RC_SUCCESS == rc)
      {
        resampler_set_client_config(this, instance, &decoded_req);
      }
    }
  }
}

/* See sns_sensor::set_client_request */
static sns_sensor_instance*
sns_resampler_set_client_request(sns_sensor *const this,
                                 sns_request const *curr_req,
                                 sns_request const *new_req,
                                 bool remove)
{
  sns_sensor_instance *curr_inst =
     sns_sensor_util_find_instance(this, curr_req,
         this->sensor_api->get_sensor_uid(this));
  sns_sensor_instance *rv_inst = NULL;

  //SNS_PRINTF(sns_fw_printf, (LOW, 2, "set_client_req len:%u remove: %d",
  //              new_req ? new_req->request_len : 0, remove);

  if(remove)
  {
    if(NULL != curr_inst)
    {
      curr_inst->cb->remove_client_request(curr_inst, curr_req);
      reval_resampler_request(this, curr_inst);
    }
  }
  else if(NULL != new_req)
  {
    //SNS_PRINTF(sns_fw_printf, (LOW, 2, "set_client_req len:%u remove: %d",
    //            new_req->request_len, remove);

    //if flush_req, no new inst, send to curr_inst
    if( SNS_STD_MSGID_SNS_STD_FLUSH_REQ == new_req->message_id)
    {
      if(NULL != curr_inst)
      {
        sns_request decoded_req = {.message_id = new_req->message_id,
                .request_len = 0, .request = NULL };
        this->instance_api->set_client_config(curr_inst,&decoded_req);
      }
      rv_inst = curr_inst;
    }
    else
    {
      pb_simple_cb_arg arg;
      pb_istream_t stream, stream_cur;
      resampler_request resampler_req =
      {
        .std_request = sns_std_request_init_default,
        .config = sns_resampler_config_init_default
      };
      resampler_request resampler_req_cur =
      {
        .std_request = sns_std_request_init_default,
        .config = sns_resampler_config_init_default
      };

      arg.decoded_struct = &resampler_req.config;
      arg.fields = sns_resampler_config_fields;
      resampler_req.std_request.payload = (struct pb_callback_s)
        { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
      stream = pb_istream_from_buffer(new_req->request, new_req->request_len);

      if(pb_decode(&stream, sns_std_request_fields, &resampler_req.std_request))
      {
        sns_request decoded_req = {.message_id = new_req->message_id,
          .request_len = sizeof(resampler_request), .request = &resampler_req };
        sns_sensor_instance *match_inst = sns_sensor_util_find_instance_match(
            this, &decoded_req, &find_instance_match);

        if(NULL == curr_inst)
        {
          // If this is a request from a new client
          if(NULL == match_inst)
          {
            rv_inst=create_instance_and_initialize(this,&resampler_req);
          }
          else
          {
            rv_inst = match_inst;
          }
        }
        else
        {
          //reject if new_req and curr_req SUID mismatch
          arg.decoded_struct = &resampler_req_cur.config;
          arg.fields = sns_resampler_config_fields;
          resampler_req_cur.std_request.payload = (struct pb_callback_s)
                  { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
          stream_cur = pb_istream_from_buffer(curr_req->request, curr_req->request_len);
          if(pb_decode(&stream_cur, sns_std_request_fields, &resampler_req_cur.std_request))
          {
            if(!sns_sensor_uid_compare((sns_sensor_uid const *)&resampler_req_cur.config.sensor_uid,
                                          (sns_sensor_uid const *)&resampler_req.config.sensor_uid))
            {
              SNS_PRINTF(ERROR, this, "reject new_req due to SUID mismatch with curr_req!");
              return NULL;
            }
          }

          curr_inst->cb->remove_client_request(curr_inst, curr_req);
          //re-evaluate only if match_inst is not same as curr_inst
          if(match_inst != curr_inst)
          {
            reval_resampler_request(this, curr_inst);
          }
          if(NULL != match_inst)
          {
            rv_inst = match_inst;
          }
          else
          {
            rv_inst=create_instance_and_initialize(this,&resampler_req);
          }
        }
        if(NULL != rv_inst)
        {
          sns_rc rc;
          rv_inst->cb->add_client_request(rv_inst, new_req);
          rc = sns_sensor_util_find_consolidated_req(rv_inst,
            sns_resampler_get_sensor_uid(this),
            &resampler_req.std_request);

          if (SNS_RC_SUCCESS == rc)
          {
            resampler_set_client_config(this, rv_inst, &decoded_req);
          }
          else
          {
            return NULL;
          }
        }
      }
    }
  }

  if(NULL != curr_inst && NULL == curr_inst->cb->get_client_request(
        curr_inst, sns_resampler_get_sensor_uid(this), true))
  {
    this->cb->remove_instance(curr_inst);
  }

  return rv_inst;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_resampler_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_resampler_init,
  .deinit = &sns_resampler_deinit,
  .get_sensor_uid = &sns_resampler_get_sensor_uid,
  .set_client_request = &sns_resampler_set_client_request,
  .notify_event = &sns_resampler_notify_event,
};
