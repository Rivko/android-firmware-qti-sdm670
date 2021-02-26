/*=============================================================================
  @file sns_async_uart_instance.c

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Includes
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_async_uart.h"
#include "sns_async_uart.pb.h"
#include "sns_async_uart_instance.h"
#include "sns_event_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
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

#include "uart.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define SNS_ASYNC_UART_PARITY_MODE       UART_NO_PARITY;
#define SNS_ASYNC_UART_NUM_STOP_BITS     UART_1_0_STOP_BITS;
#define SNS_ASYNC_UART_BITS_PER_CHAR     UART_8_BITS_PER_CHAR;
#define SNS_ASYNC_UART_ENABLE_LOOPBACK   false;
#define SNS_ASYNC_UART_ENABLE_FLOW_CTRL  false;

/*=============================================================================
  Extern data
  ===========================================================================*/


/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_async_uart_buffer_arg
{
  const void* buffer;
  uint32_t buffer_size;
  bool has_transaction_id;
  uint32_t transaction_id;
} sns_async_uart_buffer_arg;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See UART_CALLBACK in uart.h */
SNS_SECTION(".text.sns") static void
sns_async_uart_tx_callback(uint32 num_bytes, void *cb_data)
{
  UNUSED_VAR(num_bytes);
  sns_async_uart_instance_state *state =
    (sns_async_uart_instance_state *)cb_data;
  if(NULL != state)
  {
    if (!sns_island_is_island_ptr((intptr_t)state))
    {
      SNS_ISLAND_EXIT();
    }

    // PEND: Make sure all of the data was transmitted. If the data isn't
    // completely transmitted, requeue the missed portion.

    // Mark this transaction as finished
    state->tx_transaction_finished = true;

    sns_osa_thread_sigs_set(state->signal_thread->thread, state->signal_flag);
  }
}

/* See UART_CALLBACK in uart.h */
SNS_SECTION(".text.sns") static void
sns_async_uart_rx_callback(uint32 num_bytes, void *cb_data)
{
  sns_async_uart_instance_state *state =
    (sns_async_uart_instance_state *)cb_data;
  if(NULL != state)
  {
    if (!sns_island_is_island_ptr((intptr_t)state))
    {
      SNS_ISLAND_EXIT();
    }

    // Mark this transaction as finished and hold on to the number of bytes
    // received
    state->rx_transaction_finished = true;
    state->rx_bytes_received = num_bytes;

    sns_osa_thread_sigs_set(state->signal_thread->thread, state->signal_flag);
  }
}

SNS_SECTION(".text.sns") static void
sns_async_uart_send_ack_event(sns_rc error,
    bool has_transaction_id,
    uint32_t transaction_id,
    sns_event_service *event_service,
    sns_sensor_instance* const this)
{
  sns_sensor_event *event_out = event_service->api->alloc_event(event_service,
      this, sizeof(sns_async_uart_ack_event));
  if ( NULL != event_out )
  {
    pb_ostream_t stream = pb_ostream_from_buffer((uint8_t *)&event_out->event, event_out->event_len);
    sns_async_uart_ack_event async_uart_event = sns_async_uart_ack_event_init_default;
    sns_memzero(&async_uart_event, sizeof(sns_async_uart_ack_event));
    async_uart_event.has_transaction_id = has_transaction_id;
    async_uart_event.transaction_id = transaction_id;
    async_uart_event.error_code = (sns_std_error)error;
    pb_encode(&stream, sns_async_uart_ack_event_fields, &async_uart_event);

    event_out->timestamp = sns_get_system_time();
    event_out->message_id = SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_ACK_EVENT;
    event_service->api->publish_event(event_service, this, event_out, NULL);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Failed to allocate output data event");
  }
}

SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_convert_port_id(uint32 in_port_id, uart_port_id* out_port_id)
{
  sns_rc ret_val = SNS_RC_SUCCESS;

  if (NULL == out_port_id)
  {
    return SNS_RC_INVALID_VALUE;
  }

  switch (in_port_id)
  {
    case 1:
    {
      *out_port_id = UART_INSTANCE_01;
      break;
    }
    case 2:
    {
      *out_port_id = UART_INSTANCE_02;
      break;
    }
    case 3:
    {
      *out_port_id = UART_INSTANCE_03;
      break;
    }
    case 4:
    {
      *out_port_id = UART_INSTANCE_04;
      break;
    }
    case 5:
    {
      *out_port_id = UART_INSTANCE_05;
      break;
    }
    case 6:
    {
      *out_port_id = UART_INSTANCE_06;
      break;
    }
    // PEND: Consider a more extensible way to handle the debug instance, maybe
    // if the in_port_id is 0, then we use the debug instance.
    case 7:
    {
      *out_port_id = UART_DEBUG_INSTANCE;
      break;
    }
    default:
    {
      ret_val = SNS_RC_INVALID_VALUE;
      break;
    }
  }

  return ret_val;
}

// This function aligns the buffer size to be a multiple of 4, as required by
// the UART driver.
SNS_SECTION(".text.sns") static uint32_t
sns_async_uart_inst_buf_size_align(uint32_t in_buf_size)
{
  uint32_t new_buf_size = in_buf_size;

  // Check to see if the input buffer size is not a multiple of 4
  if (new_buf_size % 4 != 0)
  {
    // Round up to the nearest multiple of 4
    new_buf_size = (new_buf_size & 0xFFFFFFC0) + 4;
  }

  return new_buf_size;
}

/* See sns_sensor_instance_api::event_notify */
SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // N/A
  return SNS_RC_SUCCESS;
}

bool
sns_async_uart_pb_decode_tx_rx_buffer_cb(pb_istream_t *stream,
    const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  bool rv;
  sns_async_uart_tx_rx_buffer tx_request = sns_async_uart_tx_rx_buffer_init_default;
  sns_async_uart_buffer_arg* parsed_buffer = (sns_async_uart_buffer_arg*) *arg;
  pb_buffer_arg buffer_arg = { .buf = NULL, .buf_len = 0 };

  // Decode the tx_rx_buffer.buffer field as a string (which is the same as bytes)
  tx_request.buffer.funcs.decode = pb_decode_string_cb;
  tx_request.buffer.arg = &buffer_arg;
  rv = pb_decode_noinit(stream, sns_async_uart_tx_rx_buffer_fields, &tx_request);

  // Copy the information from the decoded message into the arg so that the
  // caller can process it
  parsed_buffer->buffer = buffer_arg.buf;
  parsed_buffer->buffer_size = buffer_arg.buf_len;
  parsed_buffer->has_transaction_id = tx_request.has_transaction_id;
  parsed_buffer->transaction_id = tx_request.transaction_id;

  return rv;
}

/* See sns_sensor_instance_api::init */
SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_init(sns_sensor_instance* const this,
    sns_sensor_state const *sensor_state)
{
  //SNS_INST_PRINTF(HIGH, this, "sns_async_uart_inst_init");
  sns_async_uart_instance_state * state = (sns_async_uart_instance_state *)this->state->state;
  sns_async_uart_state *uart_sensor_state = (sns_async_uart_state *)sensor_state->state;
  sns_memzero(state, sizeof(sns_async_uart_instance_state));

  // Grab the signaling info
  state->signal_flag = uart_sensor_state->signal;
  state->signal_thread = uart_sensor_state->sig_thread;
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_deinit(sns_sensor_instance* const this)
{
  sns_async_uart_instance_state * state =
      (sns_async_uart_instance_state *) this->state->state;

  // Close the UART port, this should return ownership of the TX and RX buffers
  // back to the sensor instance.
  // PEND: Verify the UART port is open before attempting to close it
//  if (NULL != state->handle)
//  {
    uart_close(state->handle);
//  }

  // Free the TX and RX buffers
  if (NULL != state->rx_buffer)
  {
    sns_free(state->rx_buffer);
  }
  if (NULL != state->tx_buffer)
  {
    sns_free(state->tx_buffer);
    // Send an ACK event for the outstanding transmit request
    sns_service_manager *manager =
      this->cb->get_service_manager(this);
    sns_event_service *event_service =
      (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
    sns_async_uart_send_ack_event(SNS_RC_INVALID_VALUE,
        state->tx_transaction_id_is_valid, state->tx_transaction_id,
        event_service, this);
  }

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_set_client_config(sns_sensor_instance* const this,
                  sns_request const* client_request)
{
  sns_async_uart_instance_state * state = (sns_async_uart_instance_state *)this->state->state;
  sns_rc ret_val = SNS_RC_FAILED;
  uart_result uart_ret_val;

  SNS_INST_PRINTF(HIGH, this, "sns_async_uart_inst_set_client_config msg_id %d", client_request->message_id);

  if(SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_CONFIG == client_request->message_id)
  {
    // Config request
    SNS_INST_PRINTF(HIGH, this, "async UART config request received");
    uart_open_config uart_config;
    sns_async_uart_config async_uart_config = sns_async_uart_config_init_default;
    if(pb_decode_request(client_request, sns_async_uart_config_fields, NULL, &async_uart_config))
    {
      // Get the UART instance
      uart_port_id port_id;
      ret_val = sns_async_uart_inst_convert_port_id(async_uart_config.port_id, &port_id);
      if (SNS_RC_SUCCESS != ret_val)
      {
        SNS_INST_PRINTF(ERROR, this, "sns_async_uart_inst_convert_port_id failed");
      }
      else
      {
        // Set the UART config based on the config message
        uart_config.baud_rate        = async_uart_config.baud_rate; // This is an optional field, but can I count on it automatically setting the right defaults?
        uart_config.parity_mode      = SNS_ASYNC_UART_PARITY_MODE;
        uart_config.num_stop_bits    = SNS_ASYNC_UART_NUM_STOP_BITS;
        uart_config.bits_per_char    = SNS_ASYNC_UART_BITS_PER_CHAR;
        uart_config.enable_loopback  = SNS_ASYNC_UART_ENABLE_LOOPBACK;
        uart_config.enable_flow_ctrl = SNS_ASYNC_UART_ENABLE_FLOW_CTRL;
        uart_config.tx_cb_isr        = sns_async_uart_tx_callback;
        uart_config.rx_cb_isr        = sns_async_uart_rx_callback;

        // Get the RX buffer size
        state->rx_buffer_size =
            sns_async_uart_inst_buf_size_align(async_uart_config.rx_buffer_length);

        // Open the UART port
        SNS_ISLAND_EXIT();
        SNS_INST_PRINTF(HIGH, this, "Opening async UART port id %d baud_rate %d rx_buffer_size %d", port_id, uart_config.baud_rate, state->rx_buffer_size);
        uart_ret_val = uart_open(&state->handle, port_id, &uart_config);
        if (UART_SUCCESS == uart_ret_val)
        {
          // Allocate the RX buffer and queue it up so that the UART instance can
          // immediately begin receiving data.
          SNS_ISLAND_EXIT();
          state->rx_buffer = sns_malloc(SNS_HEAP_MAIN, state->rx_buffer_size);
          if (NULL == state->rx_buffer)
          {
            SNS_INST_PRINTF(ERROR, this, "unable to malloc the uart rx_buffer");
            // Close the UART port and return an error
            uart_close(state->handle);
            ret_val = SNS_RC_NOT_AVAILABLE; // PEND: Consider adding an SNS_RC code for "no memory"
          }
          else
          {
            sns_memzero(state->rx_buffer, state->rx_buffer_size);
            SNS_ISLAND_EXIT();
            uart_ret_val = uart_receive(state->handle, (uint8*) state->rx_buffer, state->rx_buffer_size, (void*) state);
            if (UART_SUCCESS != uart_ret_val)
            {
              // PEND: Consider tearing down the instance since the client will
              // not be able to receive any data.
              SNS_INST_PRINTF(ERROR, this, "uart_receive Failed");
            }
//            else
//            {
//              SNS_INST_PRINTF(HIGH, this, "uart completely set up");
//            }
          }
        }
        else
        {
          SNS_INST_PRINTF(ERROR, this, "uart_open Failed");
          // PEND: Convert the UART error code to an appropriate SNS_RC value. At
          // the time of this writing, there's only a generic UART error so we're
          // just setting the return value to FAILED.
          ret_val = SNS_RC_FAILED;
        }
      }
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to decode SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_CONFIG req");
      ret_val = SNS_RC_FAILED;
    }
    if (SNS_RC_SUCCESS != ret_val)
    {
      // Send an ACK event with the proper error code
      sns_service_manager *manager =
        this->cb->get_service_manager(this);
      sns_event_service *event_service =
        (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
      sns_async_uart_send_ack_event(ret_val,
          state->tx_transaction_id_is_valid, state->tx_transaction_id,
          event_service, this);
      return ret_val;
    }
  }
  else if(SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_TX_RX_BUFFER == client_request->message_id)
  {
    // Transmit request
//    SNS_INST_PRINTF(HIGH, this, "async UART transmit buffer request received");
    // Only one TX is allowed at a time (this is a UART driver limitation)
    // PEND: Internally queue up TX requests and handle them sequentially
    if(NULL == state->tx_buffer)
    {
      // Start a message decoder stream
      pb_istream_t stream = pb_istream_from_buffer(client_request->request, client_request->request_len);

      // Decode the sns_std_request wrapper
      sns_std_request temp;
      sns_memzero(&temp, sizeof(sns_std_request));

      sns_async_uart_buffer_arg tx_req_buffer;

      temp.payload.funcs.decode = sns_async_uart_pb_decode_tx_rx_buffer_cb;
      temp.payload.arg = &tx_req_buffer;
      pb_decode(&stream, sns_std_request_fields, &temp);

      // Make a deep copy of the TX request buffer and save it
      state->tx_transaction_id_is_valid = tx_req_buffer.has_transaction_id;
      state->tx_transaction_id = tx_req_buffer.transaction_id;
      SNS_ISLAND_EXIT();
      state->tx_buffer = sns_malloc(SNS_HEAP_MAIN, tx_req_buffer.buffer_size);
      if (NULL == state->tx_buffer)
      {
        // Return an error code
        ret_val = SNS_RC_NOT_AVAILABLE; // PEND: Consider adding an SNS_RC code for "no memory"
      }
      else
      {
        sns_memzero(state->tx_buffer, tx_req_buffer.buffer_size);
        sns_memscpy(state->tx_buffer, tx_req_buffer.buffer_size,
            tx_req_buffer.buffer, tx_req_buffer.buffer_size);
        state->tx_buffer_size = tx_req_buffer.buffer_size;

        // Begin the UART transmission
        SNS_ISLAND_EXIT();
        uart_ret_val = uart_transmit(state->handle, state->tx_buffer,
            tx_req_buffer.buffer_size, (void*) state);
        if (UART_SUCCESS != uart_ret_val)
        {
          SNS_INST_PRINTF(ERROR, this, "uart_transmit failed");
          ret_val = SNS_RC_FAILED;

          // Free the TX buffer and nullify the pointer
          sns_free(state->tx_buffer);
          state->tx_buffer = NULL;
        }
        else
        {
          SNS_INST_PRINTF(HIGH, this, "async UART transmit buffer size %u has_trans_id %u trans_id %u", state->tx_buffer_size, state->tx_transaction_id_is_valid, state->tx_transaction_id);
          ret_val = SNS_RC_SUCCESS;
        }
      }
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Cannot handle a new TX request, there is already one outstanding.");
      ret_val = SNS_RC_NOT_AVAILABLE;
    }

    if (SNS_RC_SUCCESS != ret_val)
    {
      sns_service_manager *manager =
        this->cb->get_service_manager(this);
      sns_event_service *event_service =
        (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

      sns_async_uart_send_ack_event(ret_val,
          state->tx_transaction_id_is_valid, state->tx_transaction_id,
          event_service, this);
      return ret_val;
    }
  }
  return ret_val;
}
/*===========================================================================
  Public Function Definition
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_rc
sns_async_uart_inst_generate_event(sns_sensor_instance *const this)
{
//  SNS_INST_PRINTF(HIGH, this, "sns_async_uart_inst_generate_event");

  sns_async_uart_instance_state * state =
    (sns_async_uart_instance_state *)this->state->state;
  uart_result uart_ret_val;
  sns_service_manager *manager =
    this->cb->get_service_manager(this);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

  // Handle RX events first since they are more time-sensitive than TX events.
  // This is because we need to immediately requeue another RX buffer with the
  // coreBSP UART driver, so we don't drop any received data.
  if (state->rx_transaction_finished)
  {
    SNS_INST_PRINTF(HIGH, this, "uart RX event");
    // Mark the RX transaction as completed
    state->rx_transaction_finished = false;

    // Grab the RX buffer information before the new RX buffer is requeued
    void* received_data = state->rx_buffer;
    uint32_t received_data_len = state->rx_bytes_received;

    // Requeue a new RX buffer
    // Allocate the RX buffer and queue it up ASAP so that the UART instance can
    // immediately begin receiving data.
    SNS_ISLAND_EXIT();
    state->rx_buffer = sns_malloc(SNS_HEAP_MAIN, state->rx_buffer_size);
    if (NULL == state->rx_buffer)
    {
      SNS_INST_PRINTF(ERROR, this, "unable to malloc the uart rx_buffer");

      // PEND: Consider adding an SNS_RC code for "no memory"
      sns_async_uart_send_ack_event(SNS_RC_NOT_AVAILABLE,
          state->tx_transaction_id_is_valid, state->tx_transaction_id,
          event_service, this);
      // PEND: Might need to close the UART port
//      uart_close(state->handle);
    }
    else
    {
      sns_memzero(state->rx_buffer, state->rx_buffer_size);
      SNS_ISLAND_EXIT();
      uart_ret_val = uart_receive(state->handle, (uint8*) state->rx_buffer, state->rx_buffer_size, (void*) state);
      if (UART_SUCCESS != uart_ret_val)
      {
        SNS_INST_PRINTF(ERROR, this, "uart_receive Failed");

        // Send an ACK event to let the client know that uart_receive failed
        sns_async_uart_send_ack_event(SNS_RC_FAILED, false, 0,
            event_service, this); // No special transaction ID information for RX events

        // PEND: Consider tearing down the instance since the client will
        // not be able to receive any data.
  //      uart_close(state->handle);
      }
    }

    // Create the RX buffer event
    sns_async_uart_tx_rx_buffer rx_buffer_event = sns_async_uart_tx_rx_buffer_init_zero;
    rx_buffer_event.has_transaction_id = false;
    rx_buffer_event.buffer.funcs.encode = pb_encode_string_cb;
    rx_buffer_event.buffer.arg = &((pb_buffer_arg)
        { .buf = received_data, .buf_len = received_data_len });

    // Encode and send the event
    pb_send_event(this, sns_async_uart_tx_rx_buffer_fields,
        &rx_buffer_event, sns_get_system_time(), SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_TX_RX_BUFFER,
        NULL);

    // Free the old RX buffer
    sns_free(received_data);
  }

  // Handle TX events
  if (state->tx_transaction_finished)
  {
    SNS_INST_PRINTF(HIGH, this, "uart TX event");
    state->tx_transaction_finished = false;

    // Free the TX buffer and nullify the pointer
    sns_free(state->tx_buffer);
    state->tx_buffer = NULL;

    // Send an ACK event with the proper error code
    sns_async_uart_send_ack_event(SNS_RC_SUCCESS,
        state->tx_transaction_id_is_valid, state->tx_transaction_id,
        event_service, this);
  }

  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_async_uart_instance_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_async_uart_inst_init,
  .deinit = &sns_async_uart_inst_deinit,
  .set_client_config = &sns_async_uart_inst_set_client_config,
  .notify_event = &sns_async_uart_inst_notify_event
};
