/**
 * @file sns_interrupt_sensor_instance.c
 *
 * The Interrupt virtual Sensor Instance implementation
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/interrupt/hexagon/sns_interrupt_sensor_instance.c#3 $
 * $DateTime: 2018/06/01 07:55:33 $
 * $Change: 16303774 $
 *
 **/

#include "sns_interrupt.pb.h"
#include "sns_interrupt_sensor.h"
#include "sns_interrupt_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_signal.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "DDITlmm.h"
#include "sns_osa_thread.h"
#include "uGPIOInt.h"
#include "uTlmm.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_std.pb.h"


  /** uTLMM and dalTLMM drive type enum values are identical. */
static const DALGpioDriveType dal_drive_map[_sns_interrupt_drive_strength_ARRAYSIZE] =
                                               {
                                                  DAL_GPIO_2MA,
                                                  DAL_GPIO_4MA,
                                                  DAL_GPIO_6MA,
                                                  DAL_GPIO_8MA,
                                                  DAL_GPIO_10MA,
                                                  DAL_GPIO_12MA,
                                                  DAL_GPIO_14MA,
                                                  DAL_GPIO_16MA,
                                               };

static const uGPIOIntTriggerType dal_trigger_map[_sns_interrupt_trigger_type_ARRAYSIZE] =
                                                   {
                                                     UGPIOINT_TRIGGER_RISING,
                                                     UGPIOINT_TRIGGER_FALLING,
                                                     UGPIOINT_TRIGGER_DUAL_EDGE,
                                                     UGPIOINT_TRIGGER_HIGH,
                                                     UGPIOINT_TRIGGER_LOW,
                                                   };

static const DALGpioPullType dal_pull_map[_sns_interrupt_pull_type_ARRAYSIZE] =
                                             {
                                                DAL_GPIO_NO_PULL,
                                                DAL_GPIO_PULL_DOWN,
                                                DAL_GPIO_KEEPER,
                                                DAL_GPIO_PULL_UP,
                                             };

/**
 * Return trigger type value from uGPIOIntTriggerType enum.
 *
 * @param[i] trigger  sns_irq_trigger_type type
 *
 * @return uGPIOIntTriggerType
 */
static uGPIOIntTriggerType get_dal_trigger_type(sns_interrupt_trigger_type trigger)
{
  /** uTLMM and dalTLMM trigger type enum values are identical. */
   return dal_trigger_map[trigger];
}

/**
 * Return pull type value from DALGpioPullType enum.
 *
 * @param[i] pull   sns_irq_pull type
 *
 * @return DALGpioPullType
 */
static DALGpioPullType get_dal_pull_type(sns_interrupt_pull_type pull)
{
  /** uTLMM and dalTLMM pull type enum values are identical. */
  return dal_pull_map[pull];
}

/**
 * Return drive strength type from DALGpioDriveType enum.
 *
 * @param[i] drive   sns_irq_drive_strength type
 *
 * @return DALGpioDriveType
 */
static DALGpioDriveType get_dal_drive_type(sns_interrupt_drive_strength drive)
{
  return dal_drive_map[drive];
}

/**
 * Get DAL GPIO configuration for input request.
 *
 * @param[i] req    pointer to request config that contains IRQ
 *                  pin config
 *
 * @return uint32_t DAL GPIO config
 */
static uint32_t get_irq_pin_config(sns_interrupt_req const* req)
{
  DALGpioPullType pull = get_dal_pull_type(req->interrupt_pull_type);
  DALGpioDriveType drive = get_dal_drive_type(req->interrupt_drive_strength);

  /** Interrupts are always INPUT with func = 0 */
  /** uTLMM and dalTLMM GPIO CFG macros are identical. */
  /** Example: UTLMM_GPIO_CFG(gpio, func, dir, pull, drive) */

  return UTLMM_GPIO_CFG(req->interrupt_num, 0, DAL_GPIO_INPUT, pull, drive);
}

/**
 * Enables or disables interrupt pin.
 *
 * @param[i] this     current sensor instance
 * @param[i] cfg      interrupt in config
 * @param[i] enable   update action
 *
 * @return
 * true if update was successful else false.
 */
static bool update_irq_pin(sns_sensor_instance *const this,
                           uint32_t cfg, bool is_chip_pin, bool enable)
{
  bool success = false;
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  DALResult err = DAL_ERROR_INVALID_PARAMETER;

  if(is_chip_pin)
  {
     /** Configuring chip level TLMM wake up interrupts may not be
      *  needed. The default config of these pins is done in the
      *  boot image. This functinality is no-op in older DDF
      *  architecture as well. */

     if(NULL == state->irq_info.pin_tlmm_info.gpio_int_handle)
     {
       /** Register GPIO DAL device*/
       if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_TLMM , &state->irq_info.pin_tlmm_info.gpio_int_handle))
       {
         return success;
       }
     }

     err = DalTlmm_ConfigGpio(state->irq_info.pin_tlmm_info.gpio_int_handle,
                              (DALGpioSignalType)cfg,
                              (enable ? DAL_TLMM_GPIO_ENABLE : DAL_TLMM_GPIO_DISABLE));
     if(err == DAL_SUCCESS)
     {
        success = true;
     }

     /** Release GPIO DAL handle incase this is a disable request */
     if(!enable && state->irq_info.pin_tlmm_info.gpio_int_handle)
     {
        DAL_DeviceDetach(state->irq_info.pin_tlmm_info.gpio_int_handle);
     }
  }
  else
  {
     success = uTlmm_ConfigGpio((uTlmmGpioSignalType)cfg,
                                (enable ? UTLMM_GPIO_ENABLE : UTLMM_GPIO_DISABLE));
  }

  return success;
}

/**
 * Generate and send an interrupt event
 */
SNS_SECTION(".text.sns") static void gen_irq_events(sns_sensor_instance *const this)
{
  SNS_PROFILE(SNS_INTERRUPT_GENERATE_EVENT_START, 0);
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;

  for(;state->events_processed
    < state->read_only_buffer.events_occured;
    state->events_processed++)
  {

    sns_interrupt_event irq_event;
    uint32_t index = state->events_processed % SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH;
    state->irq_info.prev_timestamp = state->irq_info.timestamp;
    state->irq_info.timestamp =
      state->read_only_buffer.circular_buffer[index].timestamp;
    if(!state->irq_info.is_registered)
    {
       state->irq_info.warning_cnt++;
       SNS_INST_PRINTF(LOW, this, "Unregistered IRQ %d fired count %d",
                  state->irq_info.irq_num,
                  state->irq_info.warning_cnt++);
    }

    // PEND: Publish the timestamp status so that the clients can handle corner
    //       cases instead of the interrupt sensor having to make all of the
    //       hard decisions.
    if(state->read_only_buffer.circular_buffer[index].ts_status != SNS_INTERRUPT_TIMESTAMP_VALID)
    {
      // Overflow happens if the timestamp is updated in the middle of reading
      // it. That could mean a spurious interrupt happened or the interrupts
      // are coming in too fast for us to handle them in a timely manner
      if(state->read_only_buffer.circular_buffer[index].ts_status == SNS_INTERRUPT_TIMESTAMP_VALID_OVERFLOW)
      {
        SNS_INST_PRINTF(HIGH, this, "Timestamp 0x%x%08x overflowed",
            (uint32_t)(state->irq_info.timestamp >> 32),
            (uint32_t)(state->irq_info.timestamp));
      }
      else
      {
        // If the timestamp has an invalid status, just print out a warning
        SNS_INST_PRINTF(HIGH, this, "Timestamp 0x%x%08x has invalid status 0x%x",
            (uint32_t)(state->irq_info.timestamp >> 32),
            (uint32_t)(state->irq_info.timestamp),
            state->read_only_buffer.circular_buffer[index].ts_status);
      }
    }

    irq_event.interrupt_num = state->irq_info.irq_num;
    irq_event.timestamp = state->irq_info.timestamp;

    SNS_PROFILE(SNS_DDF_SIGNAL_POST, 3, sns_osa_thread_get_thread_id(), irq_event.interrupt_num, (uint32_t)irq_event.timestamp);

    if(!pb_send_event(this, sns_interrupt_event_fields, &irq_event,
                  sns_get_system_time(), SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT,
                  NULL))
    {
       SNS_INST_PRINTF(ERROR, this, "Failed to send Interrupt event for irq %d",
                  state->irq_info.irq_num);
    }

  }
  SNS_PROFILE(SNS_INTERRUPT_GENERATE_EVENT_END, 0);
}

/**
 * After a Physical sensor driver registers the interrupt,
 * Interrupt Sensor Instance registers this call back with
 * uGPIOInt. ISR calls sns_ddf_smgr_notify_irq(). Do minimal
 * operation in this function because it is in ISR
 * context.
 *
 * @param param   context provided to uGPIOInt. It is pointer to
 *                the Sensor Instance in this case.
 *
 */
SNS_SECTION(".text.sns") void sns_interrupt_sensor_notify(uint32_t param)
{
  // PEND: In Napali, only 5 interrupts can utilize the HW timestamping
  //       unit at a time. So we will continue to grab the system time
  //       here to support the other interrupts. Once the HW timestamping
  //       unit can support all interrupts at the same time, then we can
  //       remove this.
  sns_time sig_timestamp = sns_get_system_time();
  sns_interrupt_instance_state *state;
  uint32_t index;
  sns_sensor_instance * this = (sns_sensor_instance *) param;
  sns_interrupt_timestamp_status ts_status = SNS_INTERRUPT_TIMESTAMP_VALID;

  if(NULL == this)
  {
    // If input parameter is NULL then the instance is most likely destroyed.
    return;
  }

  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }

  state = (sns_interrupt_instance_state*)this->state->state;

  SNS_PROFILE(SNS_INTERRUPT_CALLBACK_START, 0);


  // If this interrupt uses the hardware timestamping unit, then go out and
  // read the timestamp.
  if (state->irq_info.uses_hw_timestamp_unit)
  {
    sns_rc ret;
    sns_time read_hw_timestamp;
    ret = read_timestamp(this, state->irq_info.irq_num, &read_hw_timestamp, &ts_status);
    // If the timestamp was successfully read, then use it. Otherwise keep
    // using the timestamp that was read at the beginning of this function.
    if (SNS_RC_SUCCESS == ret)
    {
      sig_timestamp = read_hw_timestamp;
//      SNS_INST_PRINTF(HIGH, this, "read_timestamp succeeded, timestamp %u", (uint32_t)sig_timestamp);
    }
//    else
//    {
//      SNS_INST_PRINTF(ERROR, this, "read_timestamp failed %u", ret);
//    }
  }

  index = state->read_only_buffer.events_occured % SNS_INTERRUPTS_CIRCULAR_BUFFER_DEPTH;
  state->read_only_buffer.circular_buffer[index].timestamp = sig_timestamp;
  state->read_only_buffer.circular_buffer[index].ts_status = ts_status;
  state->read_only_buffer.events_occured += 1;
  SNS_PROFILE(SNS_INTERRUPT_SET_SIGNAL, 2, state->irq_info.irq_num, state->sig_flag);
  sns_osa_thread_sigs_set(state->sig_thread->thread, state->sig_flag);

  SNS_PROFILE(SNS_INTERRUPT_CALLBACK_END, 0);
}

/**
 * Register the interrupt signal with uGPIOInt.

 * @param[i] this    current instance
 * @param[i] req     IRQ config request
 *
 * @return
 * SNS_RC_INVALID_VALUE - invalid request
 * SNS_RC_SUCCESS
 */
static sns_rc register_interrupt(sns_sensor_instance *const this,
                                 sns_request const *client_request)
{
  sns_interrupt_req const *new_req = (sns_interrupt_req*)client_request->request;
  int32_t err = 0;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;
  bool irq_is_enabled = false;
  sns_service_manager *service_manager = this->cb->get_service_manager(this);
  uint32_t flags = 0;

  flags |= sns_interrupt_sensor_get_island_flag();

  if(state->irq_info.is_registered)
  {
    /* If the irq_pin is registered then proceed only if
       the incoming request is diffrent from current config.*/
    if(0 == sns_memcmp(new_req, &state->client_req, sizeof(*new_req)))
    {
       /* new_req is same as current config. No-op.*/
       return SNS_RC_SUCCESS;
    }
  }

  state->event_service =
     (sns_event_service*)service_manager->get_service(service_manager,
                                                      SNS_EVENT_SERVICE);
  state->irq_info.irq_num = new_req->interrupt_num;
  state->irq_info.warning_cnt = 0;
  state->irq_info.irq_pin_cfg = get_irq_pin_config(new_req);
  state->irq_info.timestamp = 0;
  state->irq_info.prev_timestamp = 0;
  state->irq_info.trigger_type = get_dal_trigger_type(new_req->interrupt_trigger_type);
  state->irq_info.pin_tlmm_info.is_chip_pin = new_req->is_chip_pin;

  irq_is_enabled = update_irq_pin(this,
                                  state->irq_info.irq_pin_cfg,
                                  state->irq_info.pin_tlmm_info.is_chip_pin,
                                  true);

  if(!irq_is_enabled)
  {
    return SNS_RC_INVALID_VALUE;
  }

  if(state->irq_info.trigger_type == UGPIOINT_TRIGGER_HIGH
     ||
     state->irq_info.trigger_type == UGPIOINT_TRIGGER_LOW)
  {
      flags |= sns_interrupt_sensor_get_level_trigger_flag();
  }

  // TODO: CHECK THE REGISTRY TO SEE IF THIS PIN SHOULD USE THE HW TIMESTAMPING UNIT
  state->irq_info.uses_hw_timestamp_unit = true;
  if (state->irq_info.uses_hw_timestamp_unit)
  {
    flags |= sns_interrupt_sensor_get_hw_timestamp_flag();
  }

  /** Always reqister with uGPIOInt for both uTLMM and dalTLMM
   *  pin. */
  err = uGPIOInt_RegisterInterrupt(state->irq_info.irq_num,
                                  (uGPIOIntTriggerType)state->irq_info.trigger_type,
                                  (uGPIOINTISR)sns_interrupt_sensor_notify,
                                  (uGPIOINTISRCtx)this,
                                  flags);

  if(err != -1)
  {
      /* Copy the new irq client config in private state of this Instance.*/
      sns_memscpy(&state->client_req,
                  sizeof(state->client_req),
                  new_req,
                  sizeof(*new_req));

     if(!state->irq_info.is_registered)
     {
       state->irq_info.is_registered = true;
     }
  }
  else
  {
    update_irq_pin(this,
                   state->irq_info.irq_pin_cfg,
                   state->irq_info.pin_tlmm_info.is_chip_pin,
                   false);
    SNS_INST_PRINTF(ERROR, this, "Failed to register gpio err = %d pin = %d", err,
               state->irq_info.irq_num);
    return SNS_RC_INVALID_VALUE;
  }

  return SNS_RC_SUCCESS;
}

/**
 * De-Register the interrupt signal with uGPIOInt.

 * @param[i] this    current instance
 * @param[i] req     IRQ config request
 *
 * @return
 * SNS_RC_INVALID_VALUE - invalid request
 * SNS_RC_SUCCESS
 */
static sns_rc deregister_interrupt(sns_sensor_instance *const this)
{
  int32_t err = 0;
  sns_interrupt_instance_state* state =
    (sns_interrupt_instance_state*)this->state->state;

  if(state->irq_info.is_registered)
  {
    err = uGPIOInt_DeregisterInterrupt(state->irq_info.irq_num);
    if (err != -1)
    {
      state->irq_info.is_registered = false;
      update_irq_pin(this,
                     state->irq_info.irq_pin_cfg,
                     state->irq_info.pin_tlmm_info.is_chip_pin,
                     false);
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to de-register gpio err = %d pin = %d", err,
               state->irq_info.irq_num);
      return SNS_RC_INVALID_VALUE;
    }
  }
  return SNS_RC_SUCCESS;
}


/** See sns_sensor_instance_api::notify_event */
SNS_SECTION(".text.sns") static sns_rc sns_irq_inst_notify_event(sns_sensor_instance *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::init */
static sns_rc sns_irq_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sensor_state)
{
  sns_rc rc;
  interrupt_state *sstate = (interrupt_state*)sensor_state->state;
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  state->sig_thread = sstate->signal_thread;
  rc = sns_signal_register_signal(state->sig_thread, this, &state->sig_flag);
  if (SNS_RC_SUCCESS == rc)
  {
    state->irq_info.is_registered = false;
    state->irq_info.uses_hw_timestamp_unit = false;
    state->irq_info.irq_num = 0xFFFF;
    state->irq_info.warning_cnt = 0;
    state->irq_info.irq_pin_cfg = 0;
    state->irq_info.timestamp = 0;
    state->irq_info.prev_timestamp = 0;
    state->irq_info.pin_tlmm_info.is_chip_pin = true;
    state->irq_info.pin_tlmm_info.gpio_int_handle = NULL;

    state->events_processed = 0;
    state->read_only_buffer.events_occured = 0;
  }
  else
  {
    rc = SNS_RC_POLICY;
  }

  return rc;
}

/** See sns_sensor_instance_api::set_client_config */
SNS_SECTION(".text.sns") static sns_rc sns_irq_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request)
{
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;
  sns_rc ret_val  = SNS_RC_SUCCESS;
  if(client_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ)
  {
    ret_val = register_interrupt(this, client_request);
    if(SNS_RC_SUCCESS != ret_val ||
      !pb_send_event(this, NULL, NULL,sns_get_system_time(),
                     SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT, NULL))
    {
       SNS_INST_PRINTF(ERROR, this, "Failed to register irq %d",
                  state->irq_info.irq_num);
       ret_val = SNS_RC_INVALID_STATE;
    }
  }
  else if(client_request->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_IS_CLEARED)
  {
    send_interrupt_done(this, client_request);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Unsupported message ID %d",
                    client_request->message_id);
  }
  return ret_val;
}

static sns_rc sns_irq_inst_deinit(sns_sensor_instance *const this)
{
  sns_interrupt_instance_state *state =
    (sns_interrupt_instance_state*)this->state->state;

  // de-register the interrupt when instance is being de-init
  deregister_interrupt(this);
  sns_signal_unregister_signal(state->sig_thread, state->sig_flag);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") void sns_irq_gen_event(sns_sensor *sensor, uint32_t signal, void const* instance)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(signal);
  sns_sensor_instance *this = (sns_sensor_instance *)instance;
  gen_irq_events(this);
}

/** Public Data Definitions. */

sns_sensor_instance_api interrupt_sensor_instance_api SNS_SECTION(".data.sns") =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &sns_irq_inst_init,
  .deinit            = &sns_irq_inst_deinit,
  .set_client_config = &sns_irq_inst_set_client_config,
  .notify_event      = &sns_irq_inst_notify_event
};
