/*=============================================================================
  @file sns_async_com_port_instance.c

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_async_com_port.h"
#include "sns_async_com_port.pb.h"
#include "sns_async_com_port_instance.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_event_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_signal_sensor_api.h"
#include "sns_types.h"

/*=============================================================================
  Extern data
  ===========================================================================*/

extern sns_ascp_port_api sns_async_port_api;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct ascp_pb_dec_cb_args
{
  sns_async_com_port_queued_transaction extracted_vectors;
  sns_sensor_instance *const instance;
}ascp_pb_dec_cb_args;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") static void
  sns_async_com_port_callback(void *port_handle,
                              uint8_t num_vectors,
                              sns_port_vector buffer[],
                              uint32_t xfer_bytes,
                              sns_rc rw_err,
                              void *user_args)
{
  SNS_PROFILE(SNS_ASCP_CALLBACK_START, 2, xfer_bytes, rw_err);
  UNUSED_VAR(port_handle);
  UNUSED_VAR(num_vectors);
  UNUSED_VAR(buffer);
  sns_async_com_port_instance_state *state =
    (sns_async_com_port_instance_state *)user_args;
  if(NULL != state)
  {
    if (!sns_island_is_island_ptr((intptr_t)state))
    {
      SNS_ISLAND_EXIT();
    }
    state->last_rw_timestamp = sns_get_system_time();
    state->xfer_size = xfer_bytes;
    state->rw_err = rw_err;
    sns_osa_thread_sigs_set(state->signal_thread->thread, state->signal_flag);
  }
  SNS_PROFILE(SNS_ASCP_CALLBACK_END, 0);
}

SNS_SECTION(".text.sns") static void
extract_vectors(sns_port_vector       *vector,
                void                  *user_args)
{
  sns_async_com_port_queued_transaction *extracted_vectors =
    (sns_async_com_port_queued_transaction *)user_args;
  uint32_t curr_index = extracted_vectors->num_vectors;
  extracted_vectors->vectors[curr_index].is_write = vector->is_write;
  extracted_vectors->vectors[curr_index].reg_addr = vector->reg_addr;
  extracted_vectors->vectors[curr_index].bytes = vector->bytes;
  extracted_vectors->vectors[curr_index].buffer = sns_malloc(SNS_HEAP_ISLAND, vector->bytes);
  if (NULL == extracted_vectors->vectors[curr_index].buffer)
  {
    SNS_ISLAND_EXIT();
    extracted_vectors->vectors[curr_index].buffer = sns_malloc(SNS_HEAP_MAIN, vector->bytes);
  }
  if(extracted_vectors->vectors[curr_index].is_write)
  {
    sns_memscpy(extracted_vectors->vectors[curr_index].buffer, vector->bytes, vector->buffer, vector->bytes);
  }
  extracted_vectors->num_vectors += 1;
}

SNS_SECTION(".text.sns") static bool
  sns_async_com_port_pb_req_dec_cb(pb_istream_t *stream, const pb_field_t *fields, void **args)
{
  UNUSED_VAR(fields);
  ascp_pb_dec_cb_args *dec_args = *args;
  return sns_ascp_for_each_vector_do(stream, extract_vectors, &dec_args->extracted_vectors);
}

SNS_SECTION(".text.sns") static uint32_t
  sns_async_com_port_get_vectors_count(sns_request const* client_request)
{
  pb_istream_t stream = pb_istream_from_buffer(client_request->request, client_request->request_len);
  sns_std_request temp;
  sns_async_com_port_vector_rw decoded_struct;
  sns_memzero(&temp, sizeof(sns_std_request));
  sns_memzero(&decoded_struct, sizeof(sns_async_com_port_vector_rw));
  pb_simple_cb_arg arg = (pb_simple_cb_arg)
  {
    .decoded_struct = &decoded_struct,
    .fields = sns_async_com_port_vector_rw_fields
  };
  temp.payload.funcs.decode = pb_decode_simple_cb;
  temp.payload.arg = &arg;
  pb_decode(&stream, sns_std_request_fields, &temp);
  return decoded_struct.num_vectors;
}

SNS_SECTION(".text.sns") void
sns_async_com_port_get_vectors_from_req(sns_request const*    client_request,
                                        sns_port_vector            **vectors,
                                        uint32_t                   *num_vectors,
                                        sns_sensor_instance* const this)
{
  uint32_t vectors_count = sns_async_com_port_get_vectors_count(client_request);
  *vectors = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_port_vector)*vectors_count);
  if (NULL == *vectors)
  {
    SNS_ISLAND_EXIT();
    *vectors = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_port_vector)*vectors_count);
  }

  pb_istream_t stream = pb_istream_from_buffer(client_request->request, client_request->request_len);
  sns_std_request temp;
  sns_memzero(&temp, sizeof(sns_std_request));
  ascp_pb_dec_cb_args args = (ascp_pb_dec_cb_args)
  {
    .instance = this,
    .extracted_vectors.vectors = *vectors,
    .extracted_vectors.num_vectors = 0
  };
  temp.payload.funcs.decode = &sns_async_com_port_pb_req_dec_cb;
  temp.payload.arg = &args;
  pb_decode(&stream, sns_std_request_fields, &temp);

  SNS_ASSERT(vectors_count == args.extracted_vectors.num_vectors);
  *num_vectors = args.extracted_vectors.num_vectors;
}

SNS_SECTION(".text.sns") static uint32_t
  sns_async_com_port_get_pending_transactions(sns_async_com_port_instance_state *state)
{
  sns_isafe_list_iter iter;
  sns_isafe_list_iter_init(&iter, &state->pending_transactions, true);
  return sns_isafe_list_iter_len(&iter);
}

SNS_SECTION(".text.sns") static void
  sns_async_com_port_queue_transaction(sns_async_com_port_instance_state *state,
                                       sns_request const* client_request,
                                       sns_sensor_instance* const this)
{
  sns_isafe_list_iter iter;
  sns_port_vector *vectors;
  uint32_t num_vectors;

  sns_async_com_port_get_vectors_from_req(client_request, &vectors, &num_vectors,
    this);

  sns_async_com_port_queued_transaction *tx =
    sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_async_com_port_queued_transaction));

  if (NULL == tx)
  {
    SNS_ISLAND_EXIT();
    tx = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_async_com_port_queued_transaction));
  }
  SNS_ASSERT(NULL != tx);

  tx->vectors = vectors;
  tx->num_vectors = num_vectors;

  sns_isafe_list_iter_init(&iter, &state->pending_transactions, false);
  sns_isafe_list_item_init(&tx->list_entry, tx);
  sns_isafe_list_iter_insert(&iter, &tx->list_entry, true);
}

SNS_SECTION(".text.sns") static sns_rc sns_async_com_port_dequeue_transaction(sns_async_com_port_queued_transaction *tx,
                                                     sns_async_com_port_instance_state *state)
{
  sns_isafe_list_iter iter;
  uint8_t i;
  sns_isafe_list_item *curr_pending_tx = NULL;
  for(curr_pending_tx = sns_isafe_list_iter_init(&iter, &state->pending_transactions, true);
    NULL != curr_pending_tx;
    curr_pending_tx = sns_isafe_list_iter_advance(&iter))
  {
    if( tx == sns_isafe_list_item_get_data(curr_pending_tx))
    {
      sns_isafe_list_iter_remove(&iter);
      for(i = 0; i < tx->num_vectors; i++)
      {
        if (!sns_island_is_island_ptr((intptr_t)tx->vectors) ||
            !sns_island_is_island_ptr((intptr_t)tx->vectors[i].buffer))
        {
          SNS_ISLAND_EXIT();
        }
        sns_free(tx->vectors[i].buffer);
      }

      // debug
      sns_memscpy(&state->pre_vector, sizeof(state->pre_vector), &tx->vectors[0], sizeof(tx->vectors[0]));

      sns_free(tx->vectors);
      sns_free(tx); //TODO: Figure out who frees memory from PRAM
      return SNS_RC_SUCCESS;
    }
  }
  return SNS_RC_FAILED;
}

SNS_SECTION(".text.sns") static void
sns_async_com_port_send_error(sns_sensor_instance* const this,
                              sns_rc error)
{
  uint32_t i;
  uint32_t num_vectors = 1;
  sns_async_com_port_instance_state *state =
    (sns_async_com_port_instance_state *)this->state->state;
  if(NULL != state->current_tx)
  {
    if (!sns_island_is_island_ptr((intptr_t)state->current_tx) ||
        !sns_island_is_island_ptr((intptr_t)state->current_tx->vectors))
    {
      SNS_ISLAND_EXIT();
    }
    num_vectors = state->current_tx->num_vectors;
  }

  for(i = 0; i < num_vectors; i++)
  {
    sns_async_com_port_error async_com_port_event = sns_async_com_port_error_init_default;

    if(NULL != state->current_tx)
    {
      async_com_port_event.reg_addr = state->current_tx->vectors[i].reg_addr;
      async_com_port_event.num_bytes = state->current_tx->vectors[i].bytes;
    }
    async_com_port_event.error_code = (sns_std_error)error;
    pb_send_event( this, sns_async_com_port_error_fields, &async_com_port_event,
                   sns_get_system_time(), SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR,
                   NULL );
  }
}

SNS_SECTION(".text.sns") static sns_rc
 sns_async_com_port_start_next_transaction(sns_sensor_instance *const this)
{
  sns_async_com_port_instance_state *state =
    (sns_async_com_port_instance_state*)this->state->state;
  sns_isafe_list_iter iter;
  sns_rc ret_val = SNS_RC_SUCCESS;
  sns_isafe_list_iter_init(&iter, &state->pending_transactions, true);
  sns_async_com_port_queued_transaction *tx =
    (sns_async_com_port_queued_transaction *)sns_isafe_list_iter_get_curr_data(&iter);

  if(NULL != tx)
  {
    state->current_tx = tx;
    state->rw_err = SNS_RC_SUCCESS;

    if (!sns_island_is_island_ptr((intptr_t)state->current_tx) ||
        !sns_island_is_island_ptr((intptr_t)state->current_tx->vectors))
    {
      SNS_ISLAND_EXIT();
    }
    else
    {
      uint32_t i;
      for (i = 0; i < state->current_tx->num_vectors; i++)
      {
        if (!sns_island_is_island_ptr((intptr_t)state->current_tx->vectors[i].buffer))
        {
          SNS_ISLAND_EXIT();
        }
        if (NULL == state->current_tx->vectors[i].buffer &&
            0 < state->current_tx->vectors[i].bytes )
        {
          state->rw_err = SNS_RC_NOT_AVAILABLE;
        }
      }
    }

    if (SNS_RC_SUCCESS == state->rw_err)
    {
      SNS_PROFILE(SNS_ASCP_TX_START, 1, tx->num_vectors);
      ret_val = sns_async_port_api.sync_com_port->sns_scp_update_bus_power(state->priv_handle, true);
      if(SNS_RC_SUCCESS != ret_val)
      {
        SNS_INST_PRINTF(ERROR, this, "scp_update_bus_power failed with %u", ret_val);
      }
      state->rw_err = sns_async_port_api.sns_ascp_register_rw(state->priv_handle,
                                                              tx->num_vectors,
                                                              tx->vectors,
                                                              false);
    }

    if(SNS_RC_SUCCESS != state->rw_err)
    {
      sns_async_com_port_send_error(this, ret_val);
      sns_async_com_port_dequeue_transaction(tx, state);
      state->current_tx = NULL;
      SNS_INST_PRINTF(ERROR, this, "ascp_register_rw failed with %u", ret_val);
    }
    SNS_PROFILE(SNS_ASCP_TX_END, 0);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") static void
 sns_async_com_port_finish_tx_and_send(sns_async_com_port_instance_state *state,
                                       sns_event_service *event_service,
                                       sns_sensor_instance* const this)
{
  sns_async_com_port_queued_transaction *tx = state->current_tx;
  if(NULL != tx)
  {
    if (!sns_island_is_island_ptr((intptr_t)tx) ||
        !sns_island_is_island_ptr((intptr_t)tx->vectors))
    {
      SNS_ISLAND_EXIT();
    }
    else
    {
      uint32_t i;
      for (i = 0; i < tx->num_vectors; i++)
      {
        if (!sns_island_is_island_ptr((intptr_t)tx->vectors[i].buffer))
        {
          SNS_ISLAND_EXIT();
          break;
        }
      }
    }
    if(SNS_RC_SUCCESS == state->rw_err)
    {
      uint32_t buffer_size = sns_ascp_estimate_pb_buffer_size(tx->vectors,tx->num_vectors, false);
      uint32_t enc_len = 0;
      sns_sensor_event *event_out =
        event_service->api->alloc_event(event_service, this, buffer_size);
      if ( NULL != event_out)
      {
        if(sns_ascp_create_encoded_vectors_buffer(tx->vectors,tx->num_vectors, false,
          (uint8_t *)event_out->event, event_out->event_len, &enc_len))
        {
          event_out->message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW;
          event_out->timestamp = sns_get_system_time();
          event_out->event_len = enc_len;
          event_service->api->publish_event(event_service, this, event_out, NULL);
        }
        else
        {
          SNS_INST_PRINTF(ERROR, this, "pb encode failed");
          sns_async_com_port_send_error(this, SNS_RC_FAILED);
        }
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "Failed to allocate output data event");
      }
    }
    else
    {
      sns_async_com_port_send_error(this, SNS_RC_FAILED);
    }
    sns_async_port_api.sync_com_port->sns_scp_update_bus_power(state->priv_handle, false);
    state->last_rw_event_timestamp = state->last_rw_timestamp;
    sns_async_com_port_dequeue_transaction(tx,state);
    state->current_tx = NULL;
  }
}

/* See sns_sensor_instance_api::event_notify */
SNS_SECTION(".text.sns") static sns_rc
sns_async_com_port_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
SNS_SECTION(".text.sns") static sns_rc
sns_async_com_port_inst_init(sns_sensor_instance* const this, sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(sensor_state);
  sns_async_com_port_instance_state * state = (sns_async_com_port_instance_state *)this->state->state;
  sns_async_com_port_state *ascp_sensor_state = (sns_async_com_port_state *)sensor_state->state;
  sns_memzero(state, sizeof(sns_async_com_port_instance_state));
  sns_isafe_list_init(&state->pending_transactions);

  state->signal_flag = ascp_sensor_state->signal;
  state->signal_thread = ascp_sensor_state->sig_thread;
  state->current_tx = NULL;
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
sns_async_com_port_inst_deinit(sns_sensor_instance* const this)
{
  sns_async_com_port_instance_state * state = (sns_async_com_port_instance_state *)this->state->state;
  sns_isafe_list_iter iter;

  // Close the com port if it was successfully opened
  if (state->priv_handle != NULL)
  {
    sns_async_port_api.sync_com_port->sns_scp_deregister_com_port((sns_sync_com_port_handle **)&state->priv_handle);
  }

  sns_isafe_list_iter_init(&iter, &state->pending_transactions, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    state->current_tx = sns_isafe_list_iter_get_curr_data(&iter);
    sns_async_com_port_send_error(this, SNS_RC_FAILED);
    sns_async_com_port_dequeue_transaction(state->current_tx, state);
    sns_isafe_list_iter_init(&iter, &state->pending_transactions, true);
  }

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_bus_type
sns_ascp_map_bus_type(sns_async_com_port_bus_type ascp_bus_type)
{
  switch(ascp_bus_type)
  {
    case SNS_ASYNC_COM_PORT_BUS_TYPE_I2C:
      return SNS_BUS_I2C;
    case SNS_ASYNC_COM_PORT_BUS_TYPE_SPI:
      return SNS_BUS_SPI;
    default:
      return SNS_BUS_MIN;
  }
}
SNS_SECTION(".text.sns") sns_reg_addr_type
sns_ascp_map_reg_addr_type(sns_async_com_port_reg_addr_type ascp_reg_addr)
{
  switch(ascp_reg_addr)
  {
    case SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT:
      return SNS_REG_ADDR_8_BIT;
    case SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_16_BIT:
      return SNS_REG_ADDR_16_BIT;
    case SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT:
      return SNS_REG_ADDR_32_BIT;
    default :
      return SNS_REG_ADDR_8_BIT;
  }
}
/* See sns_sensor_instance_api::set_client_config */
SNS_SECTION(".text.sns") static sns_rc
sns_async_com_port_inst_set_client_config(sns_sensor_instance* const this,
                  sns_request const* client_request)
{
  sns_async_com_port_instance_state * state = (sns_async_com_port_instance_state *)this->state->state;
  sns_rc ret_val = SNS_RC_FAILED;
  if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG == client_request->message_id)
  {
    sns_com_port_config port_config;
    sns_async_com_port_config async_port_config;
    sns_memzero(&async_port_config, sizeof(sns_async_com_port_config));
    if(pb_decode_request(client_request, sns_async_com_port_config_fields, NULL, &async_port_config))
    {
      port_config.bus_instance = async_port_config.bus_instance;
      port_config.bus_type = sns_ascp_map_bus_type(async_port_config.bus_type);
      port_config.max_bus_speed_KHz = async_port_config.max_bus_speed_kHz;
      port_config.min_bus_speed_KHz = async_port_config.min_bus_speed_kHz;
      port_config.reg_addr_type = sns_ascp_map_reg_addr_type(async_port_config.reg_addr_type);
      port_config.slave_control = async_port_config.slave_control;

      sns_memscpy(&state->config, sizeof(sns_com_port_config),
        &port_config, sizeof(sns_com_port_config) );

      ret_val = sns_async_port_api.sync_com_port->sns_scp_register_com_port(&state->config,
                                              (sns_sync_com_port_handle **) &state->priv_handle);
      if( SNS_RC_SUCCESS == ret_val)
      {
        sns_async_port_api.sns_ascp_register_callback(state->priv_handle,
          sns_async_com_port_callback, state);
        ret_val = sns_async_port_api.sync_com_port->sns_scp_open(state->priv_handle);
        if (SNS_RC_SUCCESS == ret_val)
        {
          ret_val = sns_async_port_api.sync_com_port->sns_scp_update_bus_power(state->priv_handle, false);
          if (SNS_RC_SUCCESS != ret_val)
          {
            SNS_INST_PRINTF(ERROR, this, "sns_scp_update_bus_power Failed %d", ret_val);
          }
        }
        else
        {
          SNS_INST_PRINTF(ERROR, this, "sns_scp_open Failed: %d", ret_val);
        }
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "sns_scp_register_com_port Failed %d", ret_val);
      }
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this,
                      "Failed to decode SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG req");
    }
    if (SNS_RC_SUCCESS != ret_val)
    {
      sns_async_com_port_send_error(this, SNS_RC_INVALID_VALUE);
      return ret_val;
    }
  }
  else if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW == client_request->message_id)
  {
    sns_async_com_port_queue_transaction(state, client_request, this);
    while(NULL == state->current_tx && 0 < sns_async_com_port_get_pending_transactions(state))
    {
      sns_async_com_port_start_next_transaction(this);
    }
  }
  return ret_val;
}
/*===========================================================================
  Public Function Definition
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_rc
sns_async_com_port_inst_generate_event(sns_sensor_instance *const async_com_port_inst)
{
  sns_async_com_port_instance_state * state =
    (sns_async_com_port_instance_state *)async_com_port_inst->state->state;

  if(state->last_rw_event_timestamp != state->last_rw_timestamp)
  {
    sns_service_manager *manager =
      async_com_port_inst->cb->get_service_manager(async_com_port_inst);
    sns_event_service *event_service =
      (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

    sns_async_com_port_finish_tx_and_send(state, event_service, async_com_port_inst);

    while(NULL == state->current_tx && 0 < sns_async_com_port_get_pending_transactions(state))
    {
      sns_async_com_port_start_next_transaction(async_com_port_inst);
    }
  }
  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_async_com_port_instance_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_async_com_port_inst_init,
  .deinit = &sns_async_com_port_inst_deinit,
  .set_client_config = &sns_async_com_port_inst_set_client_config,
  .notify_event = &sns_async_com_port_inst_notify_event
};
