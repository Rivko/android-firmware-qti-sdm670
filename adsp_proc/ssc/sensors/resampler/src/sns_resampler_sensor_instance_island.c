/*=============================================================================
  @file sns_resampler_sensor_instance_island.c

  The Resampler virtual Sensor Instance implementation

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_cal.pb.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_resampler_sensor.h"
#include "sns_resampler_sensor_instance.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_types.h"


#define RESAMPLER_VERBOSE_DEBUG 0


#if RESAMPLER_VERBOSE_DEBUG
#include "sns_printf_int.h"
#endif /* RESAMPLER_VERBOSE_DEBUG */

/* Nanoseconds per second */
#define NS_PER_SEC 1000000000

/* The % by which actual ODR may fall short of the requested
   ODR for non interpolated scenarios*/
#define RESAMPLER_ODR_THRESHOLD_DELTA_HIGH      (1.1f) /* 110% */
#define RESAMPLER_ODR_THRESHOLD_DELTA_LOW       (0.9f) /* 90% */

/* Don't interpolate if the actual ODR is within this %
   of the requested ODR */
#define RESAMPLER_INTERP_THRESH_HIGH (1.05f) /* 105% */
#define RESAMPLER_INTERP_THRESH_LOW (0.95f) /* 95% */

#define RESAMPLER_UPDATE_INTERP_TS(iobj) \
  (iobj)->desired_timestamp += (iobj)->interval_ticks;

/*---------------------------------------------------------------------------
  Non-island functions
 ---------------------------------------------------------------------------*/

extern sns_rc
sns_resampler_inst_init(sns_sensor_instance *this, sns_sensor_state const *state);

extern sns_rc
sns_resampler_inst_deinit(sns_sensor_instance *const this);

/*===========================================================================

  FUNCTION:   resampler_cic_reset

===========================================================================*/

/*=========================================================================*/
static void
resampler_cic_reset(resampler_cic_obj_s* cic_obj)
{
  sns_memzero(cic_obj->accumulator,cic_obj->axis_cnt_bytes);
  cic_obj->usable_count = cic_obj->count  = 0;
  cic_obj->status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
}

/*===========================================================================

  FUNCTION:   resampler_cic_init

===========================================================================*/
static bool
resampler_cic_init(resampler_cic_obj_s *cic_obj,
                   float input_rate,
                   float resampled_rate,
                   int32_t sampling_factor,
                   uint32_t ticks_per_sec,
                   bool cic_requested,
                   bool interp_requested)
{
  int32_t cic_factor;
  float cic_frequency;

  if ( !cic_requested )
  {
    cic_factor = 1;
    cic_frequency = resampled_rate;
  }
  else
  {
    float min_rate = resampled_rate * RESAMPLER_INTERP_THRESH_LOW;
    float max_rate = resampled_rate * RESAMPLER_INTERP_THRESH_HIGH;
    float acc_ts_rate = input_rate/sampling_factor;

    cic_factor = sampling_factor;

    if ( !interp_requested ||
         ( min_rate < acc_ts_rate && max_rate > acc_ts_rate ) )
    {
      cic_frequency = input_rate;
    }
    else
    {
      cic_frequency = resampled_rate * cic_factor;
    }
  }

  /* Re init the CIC filter if the CIC factor has changed,
     or if the CIC frequency * has changed by more than
     the threshold percent */
  if ( (cic_factor != cic_obj->factor) ||
       ( (cic_frequency <
          (cic_obj->input_frequency *
           RESAMPLER_ODR_THRESHOLD_DELTA_LOW )) &&
         (cic_frequency >
          (cic_obj->input_frequency *
           RESAMPLER_ODR_THRESHOLD_DELTA_HIGH )) ) )
  {
    //reset count and values in accumulator
    resampler_cic_reset(cic_obj);
    cic_obj->factor = cic_factor;
    cic_obj->input_frequency = cic_frequency;
    cic_obj->delay_ticks =
        ticks_per_sec*((1.0f/cic_frequency)*(cic_factor-1)/2);
  }

  return (cic_factor > 1);
}

/*===========================================================================

  FUNCTION:   resampler_cic_update

===========================================================================*/

/*=========================================================================*/
static void
resampler_cic_update(resampler_cic_obj_s *cic_obj,
                     const resampler_sample * restrict input_sample,
                     resampler_sample * restrict output_sample, int32_t axis_cnt)
{
  int32_t i = 0;

  if( input_sample->status != SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE )
  {
    /* Sum axes into their accumulators */
    for ( i = 0; i < axis_cnt ; i++ )
    {
      cic_obj->accumulator[i] += input_sample->data[i];
    }
    cic_obj->usable_count++;

    /* Assigning output accuracy as the minimum of the current output accuracy and input accuracy */
    cic_obj->status = SNS_MIN(cic_obj->status, input_sample->status);
  }
  else
  {
    /* If any input samples are UNRELIABLE, set the output status to be at best LOW. */
    cic_obj->status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW;
  }

  /* Tally count. Count can never be > factor, but prevent runaway if it happens. */
  if ( (++(cic_obj->count)) >= cic_obj->factor )
  {
    if( cic_obj->usable_count > 0 )
    {
      output_sample->status = cic_obj->status;
      for ( i = 0; i < axis_cnt ; i++ )
      {
        output_sample->data[i]   = cic_obj->accumulator[i] / cic_obj->usable_count;
      }
    }
    else /* all input samples are unreliable, use values of latest sample rather than zeros*/
    {
      output_sample->status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
      for ( i = 0; i < axis_cnt ; i++ )
      {
        output_sample->data[i] = input_sample->data[i];
      }
    }
    output_sample->ts = input_sample->ts - cic_obj->delay_ticks;

#if RESAMPLER_VERBOSE_DEBUG
    SNS_PRINTF(
      HIGH, sns_fw_printf, "cic: #usable=%u #factor=%u ts=%u status=%u",
      cic_obj->usable_count, cic_obj->factor, output_sample->ts, output_sample->status);
#endif
    resampler_cic_reset(cic_obj);
  }
}

/*===========================================================================

  FUNCTION:   resampler_interp_update

===========================================================================*/

/*=========================================================================*/
static void
resampler_interp_update( resampler_interp_obj_s *int_obj,
                         const resampler_sample *input_sample,
                         sns_std_sensor_sample_status* output_status)
{
  /* shifts the samples s1 <- s2 <- new */
  int_obj->sample1.ts = int_obj->sample2.ts;
  int_obj->sample1.status = int_obj->sample2.status;
  sns_memscpy(int_obj->sample1.data,int_obj->sample1.axis_cnt_bytes,
              int_obj->sample2.data,int_obj->sample2.axis_cnt_bytes);
  int_obj->sample2.ts = input_sample->ts;
  int_obj->sample2.status = input_sample->status;
  sns_memscpy(int_obj->sample2.data,int_obj->sample2.axis_cnt_bytes,
  input_sample->data,input_sample->axis_cnt_bytes);

  if ( int_obj->sample1.ts != 0 ) /* both s1 and s2 are present? */
  {
    sns_time ts2 = int_obj->sample2.ts;
    sns_time ts1 = int_obj->sample1.ts;

    /*use the minimum accuracy of the two samples used for interpolation*/
    *output_status = SNS_MIN(int_obj->sample1.status,int_obj->sample2.status);
    if ( ts2 >= ts1 )
    {
      /* s1 older than s2 - the normal case */
      /* desired timestamp is in the past hence not producible */
      if(ts1 > int_obj->desired_timestamp)
      {
        int_obj->desired_timestamp +=
          ((ts1 - int_obj->desired_timestamp + int_obj->interval_ticks)/int_obj->interval_ticks)*
          int_obj->interval_ticks;
      }
    }
  }
  else
  {
    /*if both samples not available use accuracy of the available sample*/
    *output_status = input_sample->status;
  }
}

/*===========================================================================

  FUNCTION:   resampler_interpolate

===========================================================================*/

/*=========================================================================*/
static bool
resampler_interpolate(resampler_interp_obj_s*  int_obj,
                      float* dest_sample,
                      int32_t axis_cnt)
{
  bool new_sample = false;
  sns_time ts1 = int_obj->sample1.ts;
  sns_time ts2 = int_obj->sample2.ts;
  sns_time desired_ts = int_obj->desired_timestamp;

  /* make sure timestamp of interpolated sample is between timestamps of
          the two known samples */
  if ( (ts1 != 0) && (ts2 != 0) &&
       desired_ts >= ts1 &&
       ts2 >= int_obj->desired_timestamp )
  {
    int32_t i;
    sns_time tsgap_smp1_smp2 = ts2 - ts1;

    new_sample = true;

    if ( tsgap_smp1_smp2 != 0 )
    {
      float factor;
      sns_time tsgap_smp_smp1 = desired_ts - ts1;
      factor = (float)(tsgap_smp_smp1) / tsgap_smp1_smp2;

      for ( i=0; i<axis_cnt; i++ )
      {
        float delta =
          int_obj->sample2.data[i] - int_obj->sample1.data[i];
        dest_sample[i] =
          int_obj->sample1.data[i] + delta * factor;
      }
    }
    else
    {
      /* this is the case of first interpolated
         sample generated from one raw sample */
      for ( i=0; i<axis_cnt; i++)
      {
        dest_sample[i] = int_obj->sample1.data[i];
      }
    }
  }

  return new_sample;
}


/*===============================================================

  FUNCTION:   resampler_compute_sampling_factor

=================================================================*/
static int32_t
resampler_compute_sampling_factor(float input_rate,
                                  float resampled_rate)
{
  float resulting_rate = input_rate;
  int32_t sampling_factor = 1;

  if ( input_rate != resampled_rate )
  {
    sampling_factor = input_rate / resampled_rate;
    sampling_factor = SNS_MAX(sampling_factor, 1);
  }

  if ( sampling_factor > 1 )
  {
    int32_t change = 0;
    const float min_resulting_rate = resampled_rate * RESAMPLER_ODR_THRESHOLD_DELTA_LOW;
    const float max_resulting_rate = resampled_rate * 2;
    resulting_rate = input_rate / sampling_factor;

    if ( resulting_rate < min_resulting_rate )
    {
      sampling_factor--;
      change = -1;
    }
    else if ( resulting_rate > max_resulting_rate )
    {
      sampling_factor++;
      change = 1;
    }
    if ( change != 0 )
    {
      resulting_rate = input_rate / sampling_factor;
    }
  }

  return sampling_factor;

}

/*===============================================================

  FUNCTION:   resampler_compute_effective_rate

=================================================================*/

/*===============================================================*/
static float
resampler_compute_effective_rate(float input_rate,
                                 float resampled_rate,
                                 int32_t sampling_factor,
                                 resampler_cic_obj_s *cic_obj,
                                 bool cic_enabled,
                                 bool interp_requested)
{
  float effective_rate;
  float min_rate = resampled_rate *
    RESAMPLER_INTERP_THRESH_LOW;
  float max_rate = resampled_rate *
    RESAMPLER_INTERP_THRESH_HIGH;
  float acc_ts_rate = input_rate / sampling_factor;

  //SNS_PRINTF(LOW, 4,
  //   "input_rate,%f,resampled_rate,%f,min_rate,%f,max_rate,%f",
  //   input_rate, resampled_rate,min_rate,max_rate);

  if ( cic_enabled )
  {
    effective_rate = cic_obj->input_frequency/cic_obj->factor;
  }
  else if ( !interp_requested &&
            ( min_rate < acc_ts_rate && max_rate > acc_ts_rate ) )
  {
    effective_rate = acc_ts_rate;
  }
  else
  {
    effective_rate = resampled_rate;
  }

  return effective_rate;
}

/*===========================================================================

  FUNCTION:   resampler_interpolation_required

===========================================================================*/

/*=========================================================================*/
static bool
resampler_interpolation_required(float input_rate,
                                 float resampled_rate,
                                 int32_t sampling_factor)
{
  bool interpolation_required = false;

  // Minimum & maximum allowed rates for no interpolation.
  // If effective rate is outside of this range,
  // interpolate the data
  float resulting_rate = input_rate / sampling_factor;
  float min_rate = resampled_rate *
    RESAMPLER_INTERP_THRESH_LOW;
  float max_rate = resampled_rate *
    RESAMPLER_INTERP_THRESH_HIGH;

  if ( min_rate > resulting_rate || max_rate < resulting_rate )
  {
    interpolation_required = true;
  }

  return interpolation_required;
}

/*==========================================================================

  FUNCTION:   resampler_determine_sample_quality

===========================================================================*/
static sns_resampler_quality
resampler_determine_sample_quality( bool interp_enabled,
                                    bool cic_enabled,
                                    resampler_cic_obj_s *cic_obj,
                                    resampler_interp_obj_s *interpolator_obj,
                                    uint32_t sampling_interval,
                                    sns_time ts_last_saved,
                                    uint32_t ticks_per_sec,
                                    sns_time sync_ts_anchor,
                                    sns_time event_ts)
{
  sns_resampler_quality quality;

  /* determines sample quality to be used in indications */
  if ( interp_enabled )
  {
    if ( cic_enabled )
    {
      quality = SNS_RESAMPLER_QUALITY_INTERPOLATED_FILTERED;
      interpolator_obj->interval_ticks =
        lroundf((float)ticks_per_sec/cic_obj->input_frequency);
    }
    else
    {
      quality = SNS_RESAMPLER_QUALITY_INTERPOLATED;
      interpolator_obj->interval_ticks = sampling_interval;;
    }

    if( 0 == ts_last_saved )
    {
      ts_last_saved = event_ts - interpolator_obj->interval_ticks;
    }
    if( 0 != sync_ts_anchor )
    {
      // For S4S: set the interpolator to the next synced timestamp.
      int64_t delta = (int64_t)sync_ts_anchor - (int64_t)ts_last_saved;
      int32_t num_periods = delta / interpolator_obj->interval_ticks;

      interpolator_obj->desired_timestamp =
        sync_ts_anchor + num_periods * interpolator_obj->interval_ticks;
#if RESAMPLER_VERBOSE_DEBUG
      SNS_SPRINTF(ERROR, sns_fw_printf, "resampler setting s4s initial ts %llu num_periods:%d delta:%lld interval:%llu last_saved:%llu now:%llu",
                  interpolator_obj->desired_timestamp,
                  num_periods,
                  delta,
                  (uint64_t)interpolator_obj->interval_ticks,
                  ts_last_saved,
                  sns_get_system_time());
#endif /* RESAMPLER_VERBOSE_DEBUG */
    }
  }
  else
  {
    if ( cic_enabled )
    {
      quality = SNS_RESAMPLER_QUALITY_FILTERED;
    }
    else
    {
      quality = SNS_RESAMPLER_QUALITY_CURRENT_SAMPLE;
    }
  }

  return quality;
}

/*===========================================================================

  FUNCTION:   resampler_process_sample

===========================================================================*/
/*=========================================================================*/
static sns_rc
resampler_process_sample( resampler_instance_state *state,
                          resampler_sample * restrict input_sample,
                          resampler_sample * restrict output_sample, int32_t axis_cnt)
{
  sns_rc rc = SNS_RC_SUCCESS;

  //allocating the required space using uint64_t (so that the start address is aligned) and typecasting to resampler_sample
  uint64_t t_sample[((ALIGN_8(sizeof(resampler_sample)) + axis_cnt*sizeof(float))/sizeof(uint64_t))+1];
  resampler_sample* tmp_sample=(resampler_sample*)t_sample;
  tmp_sample->axis_cnt_bytes=axis_cnt * sizeof(float);
  tmp_sample->data=(float*)((uint8_t*)tmp_sample + ALIGN_8(sizeof(resampler_sample)));

  sns_time sample_timestamp = 0;
  bool new_sample = false;

  // PEND: sample sample check

  resampler_cic_obj_s* cic_obj = &state->cic_obj;
  resampler_interp_obj_s* int_obj = &state->interp_obj;

  sample_timestamp = input_sample->ts;

  switch ( state->quality )
  {
  case SNS_RESAMPLER_QUALITY_CURRENT_SAMPLE:
    if ( state->cur_samp_obj.sampling_count == 0 )
    {
      // PEND: add min_acceptable_gap check
      sns_memscpy(output_sample->data,output_sample->axis_cnt_bytes,
                  input_sample->data,input_sample->axis_cnt_bytes);
      output_sample->ts = sample_timestamp;
    }
    state->cur_samp_obj.sampling_count++;
    state->cur_samp_obj.sampling_count %= state->cur_samp_obj.sampling_factor;
    //assigning input sample accuracy to output sample
    output_sample->status = input_sample->status;
    break;

  case SNS_RESAMPLER_QUALITY_INTERPOLATED:
    resampler_interp_update(int_obj, input_sample, &output_sample->status);
    new_sample = resampler_interpolate(int_obj, output_sample->data, axis_cnt);
    if ( new_sample )
    {
      output_sample->ts = int_obj->desired_timestamp;
      RESAMPLER_UPDATE_INTERP_TS(int_obj);
    }
    break;

  case SNS_RESAMPLER_QUALITY_INTERPOLATED_FILTERED:
    resampler_interp_update(int_obj, input_sample, &output_sample->status);
    new_sample = resampler_interpolate(int_obj, tmp_sample->data, axis_cnt);

    if ( !new_sample )
    {
      break;
    }
    tmp_sample->ts = int_obj->desired_timestamp;
    RESAMPLER_UPDATE_INTERP_TS(int_obj);
    //Output sample has the intermediate accuracy status of the interpolation. This needs to be assigned to the tmp_sample before passing it on for filtering
    tmp_sample->status = output_sample->status;

    sns_memscpy(input_sample->data, input_sample->axis_cnt_bytes,
                tmp_sample->data, tmp_sample->axis_cnt_bytes);
    input_sample->ts     = tmp_sample->ts;
    input_sample->status = tmp_sample->status;

    /* fallthrough */
  case SNS_RESAMPLER_QUALITY_FILTERED:
    resampler_cic_update(cic_obj,
                         input_sample,
                         output_sample,
                         axis_cnt);
    break;
  }
  return rc;
}

/*===========================================================================

  FUNCTION:   resampler_state_reset

===========================================================================*/

/*=========================================================================*/
static void
resampler_state_reset(sns_sensor_instance *this,
                      sns_time sync_ts_anchor,
                      bool stream_is_synchronous,
                      float input_rate,
                      sns_time event_ts)
{
  resampler_instance_state *state = (resampler_instance_state *)this->state->state;
  const uint32_t ticks_per_sec = sns_convert_ns_to_ticks(NS_PER_SEC);
  bool interp_required = false;
  const bool cic_requested = state->client_config.filter;
  const bool interp_requested = (state->client_config.rate_type == SNS_RESAMPLER_RATE_FIXED);
  const bool orig_interp_enabled = state->interp_enabled;
  float effective_rate_hz;
  int64_t sampling_interval;
  bool rv = true;

  state->cur_samp_obj.sampling_count  = 0;
  state->cur_samp_obj.sampling_factor = 1;

  state->cur_samp_obj.sampling_factor =
    resampler_compute_sampling_factor(input_rate,
                                      state->client_config.resampled_rate);

  state->cic_enabled = resampler_cic_init(&state->cic_obj,
                                          input_rate,
                                          state->client_config.resampled_rate,
                                          state->cur_samp_obj.sampling_factor,
                                          ticks_per_sec,
                                          cic_requested,
                                          interp_requested);

  effective_rate_hz =
    resampler_compute_effective_rate(input_rate,
                                     state->client_config.resampled_rate,
                                     state->cur_samp_obj.sampling_factor,
                                     &state->cic_obj,
                                     state->cic_enabled,
                                     interp_requested);

  if( effective_rate_hz > ((float)ticks_per_sec/UINT32_MAX) )
  {
    sampling_interval =
      llroundf(ticks_per_sec/effective_rate_hz);
  }
  else
  {
    sampling_interval = UINT32_MAX;
  }

  /* Force interpolation to be enabled for synchronous/S4S sensors not yet
   * synchronized */
  if( sync_ts_anchor != 0 && !stream_is_synchronous )
  {
    state->interp_enabled = true;
    SNS_INST_PRINTF(MED, this, "forcing interp for s4s");
  }
  else
  {
    interp_required =
      resampler_interpolation_required(input_rate,
                                       state->client_config.resampled_rate,
                                       state->cur_samp_obj.sampling_factor);
    state->interp_enabled = interp_requested & interp_required;
  }

  if( orig_interp_enabled && !state->interp_enabled )
  {
    /* Interpolator was disabled. Verify that timestamps are aligned */
    if( state->interp_obj.sample2.ts != state->interp_obj.desired_timestamp )
    {
      SNS_INST_PRINTF(HIGH, this, "WARNING: disabling interp with unaligned ts. "
                      "ts2:%u desired:%u delta:%u",
                      (uint32_t)state->interp_obj.sample2.ts,
                      (uint32_t)state->interp_obj.desired_timestamp,
                      (uint32_t)(state->interp_obj.desired_timestamp-state->interp_obj.sample2.ts));
    }
  }

  state->quality =
    resampler_determine_sample_quality(state->interp_enabled,
                                       state->cic_enabled,
                                       &state->cic_obj,
                                       &state->interp_obj,
                                       (uint32_t)sampling_interval,
                                       state->ts_last_saved,
                                       ticks_per_sec,
                                       sync_ts_anchor,
                                       event_ts);

  sns_resampler_config_event config_event
      = sns_resampler_config_event_init_default;
  config_event.quality =  state->quality;

  if( 0 == state->ts_last_saved )
  {
     rv = pb_send_event(
             this, sns_resampler_config_event_fields, &config_event,
             sns_get_system_time(), SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT, NULL);
  }
  else
  {
     rv = pb_send_event(
             this, sns_resampler_config_event_fields, &config_event,
             state->ts_last_saved, SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT, NULL);
  }
  if(rv)
  {
    SNS_INST_PRINTF(MED, this, "state_reset - quality:%u desired_ts=%u", 
                    state->quality, (uint32_t)state->interp_obj.desired_timestamp);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "resampler config tx failure");
  }
}

/**
 * Process incoming events to the Resampler Sensor Instance.
 */
static sns_rc
sns_resampler_inst_notify_event(sns_sensor_instance *const this)
{
  SNS_PROFILE(SNS_RS_INST_NOTIFY_START, 1, this);
  sns_rc rc = SNS_RC_SUCCESS;
  resampler_instance_state *state =
    (resampler_instance_state*)this->state->state;
  sns_sensor_event *event_in =
      state->sensor_stream->api->peek_input(state->sensor_stream);

  sns_sensor_uid resampler_suid = {.sensor_uid = {RESAMPLER_SUID}};

  while(NULL != event_in)
  {
    /*
    SNS_INST_PRINTF(LOW, this,
                  "event len:%u event ts:%u",
                  event_in->event_len,
                  (unsigned int)(event_in->timestamp));
    */
    if( event_in->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT )
    {
      if( state->phys_cfg_received == true )
      {
        float input_data [AXIS_CNT_MAX] = { 0 };
        uint8_t arr_index = 0;

        pb_istream_t stream =
          pb_istream_from_buffer((pb_byte_t*)event_in->event, event_in->event_len);
        sns_std_sensor_event stream_event = sns_std_sensor_event_init_default;
        pb_float_arr_arg arg = {.arr = input_data,
                                .arr_len = ARR_SIZE(input_data), .arr_index = &arr_index};

        stream_event.data = (struct pb_callback_s)
          {.funcs.decode = &pb_decode_float_arr_cb, .arg = &arg};

        if(!pb_decode(&stream, sns_std_sensor_event_fields, &stream_event))
        {
          SNS_INST_PRINTF(ERROR, this,
                          "Error decoding stream evt");
        }
        else
        {

          //Get minimum of number of axes provided by client and received from data to avoid overflow
          uint32_t min_axes = SNS_MIN(state->axis_cnt, arr_index);
          uint32_t min_axes_bytes = sizeof(float)*min_axes;
          //allocating the required space using uint64_t (so that the start address is aligned) and typecasting to resampler_sample
          uint64_t op_sample[((ALIGN_8(sizeof(resampler_sample)) + min_axes_bytes)/sizeof(uint64_t)) + 1];
          uint64_t ip_sample[((ALIGN_8(sizeof(resampler_sample)) + min_axes_bytes)/sizeof(uint64_t)) + 1];
          resampler_sample* output_sample = (resampler_sample*)op_sample;
          resampler_sample* input_sample = (resampler_sample*)ip_sample;
          output_sample->axis_cnt_bytes=min_axes_bytes;
          output_sample->data=(float*)((uint8_t*)output_sample + ALIGN_8(sizeof(resampler_sample)));
          input_sample->axis_cnt_bytes=min_axes_bytes;
          input_sample->data=(float*)((uint8_t*)input_sample + ALIGN_8(sizeof(resampler_sample)));
          /*Initializing the output accuracy to HIGH and later assigning it the minimum of the samples used for interpolation or filtering*/
          output_sample->status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;

          input_sample->ts = event_in->timestamp;
          output_sample->ts = 0;
          input_sample->status = stream_event.status;

          for (int i = 0; i < min_axes; i++)
          {
            input_sample->data[i] = input_data[i];
          }

          rc = resampler_process_sample(
                                        state,
                                        input_sample,
                                        output_sample, min_axes );

#if RESAMPLER_VERBOSE_DEBUG
          if( min_axes == 3 )
          {
            SNS_SPRINTF(
              LOW, sns_fw_printf, "%llu [%f, %f, %f] --> %llu [%f, %f, %f]",
              input_sample->ts, input_sample->data[0], input_sample->data[1], input_sample->data[2],
              output_sample->ts, output_sample->data[0], output_sample->data[1], output_sample->data[2]);
          }
          else
          {
            uint8_t axis=0;
            for(axis=0; axis < min_axes ;axis++)
            {
              SNS_SPRINTF(LOW, sns_fw_printf, "rx sensor data :"
                        "ts:%llu,data[%u]:%f,"
                        "sensor data output,"
                        "ts:%llu,data[%u]:%f",input_sample->ts,
                        axis, input_sample->data[axis], output_sample->ts,
                        axis, output_sample->data[axis]);
            }
          }
#endif /* RESAMPLER_VERBOSE_DEBUG */

          if ( SNS_RC_SUCCESS == rc && 0 != output_sample->ts && 
               output_sample->status != SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE )
          {
            size_t encoded_event_len =
              pb_get_encoded_size_sensor_stream_event(output_sample->data,
                                                      min_axes);
            rc = pb_send_sensor_stream_event(this, NULL, output_sample->ts,
                                             SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                             output_sample->status,
                                             output_sample->data, min_axes,
                                             encoded_event_len);
            SNS_PROFILE(SNS_RS_REPORT_OUTPUT, 1, this);

            if (SNS_RC_SUCCESS != rc)
            {
              SNS_INST_PRINTF(ERROR, this,
                              "tx stream evt failed %d", rc);
            }
            state->ts_last_saved = output_sample->ts;
          }
        }
      }
      else  // physical sensor config not received yet!
      {
        SNS_INST_PRINTF(ERROR, this, "didn't rx phys cfg, suid low 0x%x %x",
            (uint32_t)(state->client_config.sensor_uid.suid_low >> 32),
            (uint32_t)(state->client_config.sensor_uid.suid_low & 0xffffffff));
      }
    }
    else if (event_in->message_id ==
             SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT)
    {
      pb_istream_t stream =
        pb_istream_from_buffer((pb_byte_t*)event_in->event, event_in->event_len);
      sns_std_sensor_physical_config_event sensor_config = sns_std_sensor_physical_config_event_init_default;

      if(!pb_decode(&stream, sns_std_sensor_physical_config_event_fields, &sensor_config))
      {
        SNS_INST_PRINTF(ERROR, this, "cfg evt decode error");
      }
      else
      {
        if(state->sync_ts_anchor != sensor_config.sync_ts_anchor ||
           state->stream_is_synchronous != sensor_config.stream_is_synchronous ||
           sensor_config.sample_rate < 1.0f || // sub Hz
           (fabsf(state->sample_rate - sensor_config.sample_rate) > 0.01f))
        {
          state->sync_ts_anchor = sensor_config.sync_ts_anchor;
          state->stream_is_synchronous = sensor_config.stream_is_synchronous;
          state->sample_rate = sensor_config.sample_rate;
          state->phys_cfg_received = true;

          resampler_state_reset(this, state->sync_ts_anchor,
                                state->stream_is_synchronous,
                                state->sample_rate,
                                event_in->timestamp);

          SNS_INST_PRINTF(LOW, this,
                          "rx new cfg evt: rate:%d/100 sync:%d anchor:%u",
                          (int32_t)(sensor_config.sample_rate*100),
                          (int32_t)sensor_config.stream_is_synchronous,
                          (uint32_t)sensor_config.sync_ts_anchor);
#if RESAMPLER_VERBOSE_DEBUG
          SNS_SPRINTF(LOW, sns_fw_printf,
                      "anchor:%llu",
                      sensor_config.sync_ts_anchor);
#endif
        }
        else
        {
          SNS_INST_PRINTF(LOW, this,
                          "rx existing cfg evt: rate:%d/100 sync:%d anchor:%u",
                          (int32_t)(sensor_config.sample_rate*100),
                          (int32_t)sensor_config.stream_is_synchronous,
                          (uint32_t)sensor_config.sync_ts_anchor);
        }
      }

      // Forward the config event message to clients
      pb_send_event(this, sns_std_sensor_physical_config_event_fields, &sensor_config,
                     event_in->timestamp, SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                     &resampler_suid);

    }
    else
    {
      sns_service_manager *manager =
        this->cb->get_service_manager(this);
      sns_event_service *event_service =
        (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
      sns_sensor_event *event_copy =
          event_service->api->alloc_event(event_service, this, event_in->event_len);

      event_copy->timestamp = event_in->timestamp;
      event_copy->event_len = event_in->event_len;
      event_copy->message_id = event_in->message_id;
      sns_memscpy(event_copy->event,event_copy->event_len,
          event_in->event,event_in->event_len);

      event_service->api->publish_event(event_service,
            this,event_copy,&resampler_suid);

      SNS_INST_PRINTF(LOW, this, "Resampler forward event:%u", event_in->message_id);
    }

    event_in = state->sensor_stream->api->get_next_input(state->sensor_stream);
  }
  SNS_PROFILE(SNS_RS_INST_NOTIFY_END, 1, this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
sns_resampler_inst_set_client_config(sns_sensor_instance *const this,
                                     sns_request const *client_request)
{
  sns_rc rc = SNS_RC_SUCCESS;
  resampler_instance_state *state =
     (resampler_instance_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

  if ( client_request->message_id == SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG )
  {
    resampler_request *resampler_req = (resampler_request *)client_request->request;
    sns_resampler_config *resampler_config = &state->client_config;

    sns_memscpy(&state->client_config,
                sizeof(state->client_config),
                &resampler_req->config,
                sizeof(resampler_req->config));

    if ( NULL == state->sensor_stream )
    {
      stream_service->api->
        create_sensor_instance_stream(stream_service,
                                      this,
                                      *((sns_sensor_uid*)&resampler_config->sensor_uid),
                                      &state->sensor_stream);
    }

    if ( NULL != state->sensor_stream )
    {
      size_t encoded_len;
      sns_std_sensor_config sensor_config = sns_std_sensor_config_init_default;
      uint8_t buffer[100];

      /* Forward config request to target sensor */
      sensor_config.sample_rate = resampler_config->resampled_rate;
      encoded_len = pb_encode_request(buffer,
                                      sizeof(buffer),
                                      &sensor_config,
                                      sns_std_sensor_config_fields,
                                      &resampler_req->std_request);
      if(0 < encoded_len)
      {
        uint32_t message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;

        if ( resampler_config->has_event_gated && resampler_config->event_gated)
        {
          message_id = SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
        }

        sns_request request = (sns_request){
          .message_id = message_id ,
          .request_len = encoded_len, .request = buffer };
        rc |= state->sensor_stream->api->send_request(state->sensor_stream, &request);
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this,
                        "encode sensor config fail");
      }
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this,
                      "sensor stream fail");
    }

    SNS_INST_PRINTF(MED, this,
                    "resampled rate:[%d/100] rate type: %u filter: %u",
                    (int32_t)(resampler_config->resampled_rate*100),
                    resampler_config->rate_type,
                    resampler_config->filter);
  }
  else   //pass through  all other req
  {
    if(NULL != state->sensor_stream )
    {
      sns_request request = (sns_request){
        .message_id = client_request->message_id ,
        .request_len = client_request->request_len, .request = client_request->request };
      rc = state->sensor_stream->api->send_request(state->sensor_stream, &request);
    }
    else
    {
      rc = SNS_RC_INVALID_STATE;
    }
  }
  return rc;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_resampler_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_resampler_inst_init,
  .deinit = &sns_resampler_inst_deinit,
  .set_client_config = &sns_resampler_inst_set_client_config,
  .notify_event = &sns_resampler_inst_notify_event
};
