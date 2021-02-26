/*============================================================================
  FILE:          vptx_sensors.cpp

  OVERVIEW:      This file has the function definitions for sensors interface for fpro v2 algorithm.

  DEPENDENCIES:  sensors code should be present

Copyright (c) 2014-2015, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/voice_proc_tx_sensors/src/vptx_sensors.cpp#2 $

  when       who        what, where, why
  ---------- -------    ---------------------------------------------------------
  2014-07-14 rishabht   Initial revision.
  2018-07-24 manigupt   Sensors API update.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "vptx_sensors.h"
#include "qmi_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * Utility structure used to encode a string or already encoded message
 * into another Protocol Buffer message.
 */
typedef struct vptx_sns_buffer_arg
{
   /* Buffer to be written, or reference to read buffer */
   void const *buf;
   /* Length of buf */
   size_t buf_len;
} vptx_sns_buffer_arg;

/**
 * Utility structure used to decode received message
 * into another Protocol Buffer message.
 */
typedef struct vptx_decode_arg
{
   /* Decoded payload*/
   void const *arg;

   /* Msg_id of the event */
   uint32_t msg_id;

   /* Pointer to vptx instance, to write sensor data into*/
   qmi_obj_t *qmi_obj;
} vptx_decode_arg;

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid vptx_sns_suid_lookup = sns_suid_sensor_init_default;


/*=============================================================================
  Function Declarations
  ===========================================================================*/
ADSPResult vptx_sns_qmi_init(void *pQmi_object, qurt_elite_channel_t *pChannel,
                             uint32_t qmi_reg_mask, uint32_t qmi_ind_mask,
                             uint32_t session_num);

// Register and send request
ADSPResult vptx_sns_qmi_register(void *pQmi_object);

static bool vptx_sns_send_suid_req(qmi_client_type qmi_handle,
                                       char const *data_type,
                                       qmi_obj_t *pQmi_obj);

static size_t vptx_sns_get_encoded_suid_req(char const *data_type,
                                                  void **encoded_req);

static bool vptx_sns_send_basic_req(qmi_client_type qmi_handle,
                                    vptx_sns_buffer_arg const *payload,
                                    sns_std_suid suid, uint32_t msg_id,
                                    qmi_obj_t *pQmi_obj);

static bool vptx_sns_send_qmi_req(qmi_client_type qmi_handle,
                                  sns_client_req_msg_v01 const *req_msg,
                                  qmi_obj_t *pQmi_obj);

static void vptx_sns_registration_cb(elite_qmi_client_type user_handle,
                                     unsigned int msg_id, void *ind_buf,
                                     unsigned int ind_buf_len,
                                     void *ind_cb_data,
                                     elite_qmi_client_error_type err);

static void vptx_sns_indication_cb(elite_qmi_client_type user_handle,
                                   unsigned int msg_id, void *ind_buf,
                                   unsigned int ind_buf_len, void *ind_cb_data);

static void vptx_sns_handle_event_msg(sns_std_suid const *suid,
                                      void const *event_msg,
                                      size_t event_msg_len,
                                      qmi_obj_t *pQmi_obj);

static bool vptx_sns_decode_events(pb_istream_t *stream,
                                   const pb_field_t *field, void **arg);

static bool vptx_sns_decode_suid_event(pb_istream_t *stream,
                                       const pb_field_t *field, void **arg);

static bool vptx_sns_decode_suid(pb_istream_t *stream, const pb_field_t *field,
                                 void **arg);

static bool vptx_sns_send_enable_req(sns_std_suid *suid, qmi_obj_t *pQmi_obj);

static bool vptx_sns_decode_fcng_sns_event(pb_istream_t *stream,
                                           const pb_field_t *field, void **arg);

static bool vptx_sns_decode_fcng_sns(pb_istream_t *stream,
                                     const pb_field_t *field,
                                     qmi_obj_t *pQmi_obj);

static uint32_t vptx_sns_get_msg_id(pb_istream_t *stream);

static bool vptx_sns_encode_payload(pb_ostream_t *stream,
                                    const pb_field_t *field, void *const *arg);

int32_t vptx_sns_get_holding_pattern(void *pQmi_object);

void vptx_sns_clear_qmi_signal(void *pQmi_object, reg_ind_signal_t sig_flag);

void vptx_sns_qmi_deregister(void *pQmi_object);



/*=============================================================================
  Function Definitions
  ===========================================================================*/

/*
 * The below function gets invoked in vptx init  to get the size of
 * qmi structure
*/
uint32_t vptx_sns_qmi_get_mem_req(void)
{
   return sizeof(qmi_obj_t); 
}

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
static bool vptx_sns_encode_payload(pb_ostream_t *stream,
                                    const pb_field_t *field, void *const *arg)
{
   vptx_sns_buffer_arg *info = (vptx_sns_buffer_arg *)*arg;

   if (!pb_encode_tag_for_field(stream, field)) 
   {
      return false;
   }

   if (NULL == info)
   {
      pb_encode_string(stream, NULL, 0);
   }
   else
   {
      if (!pb_encode_string(stream, (pb_byte_t *)info->buf, info->buf_len))
      {
         return false;
      }
   }

   return true;
}



/*
 * The below call back function gets invoked in QMI thread context as a response
 * to the request
 * placed.
 * Arguements :
 * user_handle : unique user handle for a client
 * msg_id      : msg id corresponding to registration call back by sensors
 * ind_buf     : buffer containing response from sensors for the registration
 * request
 * ind_buf_len : length of buffer containing response from sensors
 * err         : error flag to indicate if there are any qmi transport errors
 *
 * Vptx thread places an asynchronous request to Sensors to start receiving Indications
 * conveying changes in holding pattern. At the time this request is placed, a pointer to
 * Vptx instance is passed as Callback data.
 * The below function gets invoked in QMI thread context as a response to the request
 * placed. This function:
 *   1. Uses the Vptx instance structure to compare the user handle received now
 *      with what was set at the time the request was placed.
 *   2. Extract session number, for debug purposes - in a read-only operation.
 *   3. Set signal (write operation), so Vptx is notified of this response.
 *   4. Request response status or Error code (write operation).
 */
static void vptx_sns_registration_cb(elite_qmi_client_type       user_handle,
                                     unsigned int                msg_id,
                                     void *                      ind_buf,
                                     unsigned int                ind_buf_len,
                                     void *                      ind_cb_data,
                                     elite_qmi_client_error_type err)
{

   qmi_obj_t *pQmi_obj = (qmi_obj_t *)ind_cb_data;

   MSG_3(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: vptx_sns_registration_cb : Received response - error %d, msg_id %d, size %d",
         err,
         msg_id,
         ind_buf_len);

   if (QMI_NO_ERR == err)
   {
      sns_client_resp_msg_v01 *resp = (sns_client_resp_msg_v01 *)ind_buf;

      MSG_2(MSG_SSID_QDSP6,
            DBG_HIGH_PRIO,
            "VCP: vptx_sns_registration_cb : Received client ID %d result %i",
            resp->client_id,
            resp->result);
   }


   qurt_elite_signal_send(pQmi_obj->reg_sig);
   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx notified of registration response from Sensors.",
         pQmi_obj->session_num);

   qurt_elite_memory_free(ind_buf);
}

/**
 * Decode the sensor event (aka a sensor sample).
 */
static bool vptx_sns_decode_fcng_sns(pb_istream_t *stream, const pb_field_t *field, qmi_obj_t *pQmi_obj)
{
   sns_facing_event_state state = sns_facing_event_state_default;

   if (!pb_decode(stream, sns_facing_event_fields, &state))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors Error decoding facing sensor event: %s",
            pQmi_obj->session_num,
            PB_GET_ERROR(stream));
      return false;
   }

   pQmi_obj->holding_pattern = state;
   MSG_2(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors received facing sensor event: %d",
         pQmi_obj->session_num,
         state);

   return true;
}

/**
 * Decode an event message received from the facing Sensor.
 */
static bool vptx_sns_decode_fcng_sns_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
   vptx_decode_arg *obj_arg  = (vptx_decode_arg *)*arg;
   uint32_t         msg_id   = (uint32_t)(obj_arg->msg_id);
   qmi_obj_t *      pQmi_obj = obj_arg->qmi_obj;


   if (SNS_FACING_MSGID_SNS_FACING_EVENT == msg_id)
   {
      return vptx_sns_decode_fcng_sns(stream, field, pQmi_obj);
   }
   else if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == msg_id)
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors received error event",
            pQmi_obj->session_num);
      return false;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_HIGH_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors received unknown event %d",
            pQmi_obj->session_num,
            msg_id);
      return false;
   }

   return true;
}

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 *
 * At this time we will send an attributes request to the SUID.
 */
static bool vptx_sns_decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
   sns_std_suid uid;
   vptx_decode_arg *obj_arg = (vptx_decode_arg *)*arg;

   if (!pb_decode(stream, sns_std_suid_fields, &uid))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors Error decoding SUID Event: %s",
            obj_arg->qmi_obj->session_num,
            PB_GET_ERROR(stream));
      return false;
   }

   MSG_3(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors received SUID Event with SUID %lld %lld",
         obj_arg->qmi_obj->session_num,
         uid.suid_low,
         uid.suid_high);

   // use decoded facing_suid obtained from lookup sensor indication
   vptx_sns_send_enable_req(&uid, (qmi_obj_t *)obj_arg->qmi_obj);

   return true;
}

/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
static bool vptx_sns_decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
   sns_suid_event suid_event = sns_suid_event_init_default;
   vptx_decode_arg *obj_arg = (vptx_decode_arg *)*arg;

   suid_event.suid.funcs.decode      = &vptx_sns_decode_suid;
   suid_event.suid.arg               = obj_arg;

   if (!pb_decode(stream, sns_suid_event_fields, &suid_event))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors error decoding SUID Event: %s",
            obj_arg->qmi_obj->session_num,
            PB_GET_ERROR(stream));
      return false;
   }

   return true;
}

/**
 * Get the message ID of the encoded event residing on stream.
 *
 * @return Message ID
 */
static uint32_t vptx_sns_get_msg_id(pb_istream_t *stream)
{
   sns_client_event_msg_sns_client_event event = sns_client_event_msg_sns_client_event_init_default;

   if (!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx_sensors error decoding SUID Event: %s",
            PB_GET_ERROR(stream));
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_HIGH_PRIO,
            "VCP: Vptx_sensors decoding event with message ID %ld, timestamp %lld",
            event.msg_id,
            event.timestamp);
      return event.msg_id;
   }

   return 0;
}

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * @param[i] arg Sensor-specific decode function
 */
static bool vptx_sns_decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{

   bool                                  rv         = true;
   vptx_decode_arg *                     obj_arg    = (vptx_decode_arg *)*arg;
   sns_client_event_msg_sns_client_event event      = sns_client_event_msg_sns_client_event_init_default;
   pb_istream_t                          stream_cpy = *stream;

   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors begin decoding event",
         obj_arg->qmi_obj->session_num);
   obj_arg->msg_id = vptx_sns_get_msg_id(&stream_cpy);

   event.payload.funcs.decode = (bool (*)(pb_istream_t *, const pb_field_t *, void **))obj_arg->arg;
   event.payload.arg          = obj_arg;

   if (!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors error decoding Event: %s",
            obj_arg->qmi_obj->session_num,
            PB_GET_ERROR(stream));
      rv = false;
   }

   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors event decoding complete",
         obj_arg->qmi_obj->session_num);
   return rv;
}

/**
 * Handle a received event message from the Sensor Service.
 *
 * @param[i] suid Source Sensor of this event
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 * @param[i] pQmi_obj context object of type pQmi_obj
 */
static void vptx_sns_handle_event_msg(sns_std_suid const *suid, void const *event_msg, size_t event_msg_len, qmi_obj_t *pQmi_obj)
{
   pb_istream_t         stream;
   sns_client_event_msg event = sns_client_event_msg_init_default;

   vptx_decode_arg *decode_arg =
      (vptx_decode_arg *)qurt_elite_memory_malloc(sizeof(vptx_decode_arg), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == decode_arg)
   {
      MSG(MSG_SSID_QDSP6,
          DBG_HIGH_PRIO,
          "VCP: Vptx session # (%lx): Vptx_sensors failed to allocate memory for decode_arg");
      return;		 
   }
   decode_arg->msg_id  = 0;
   decode_arg->qmi_obj = pQmi_obj;

   MSG_3(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors processing events from SUID %lld %lld",
         pQmi_obj->session_num,
         suid->suid_low,
         suid->suid_high);

   if ((suid->suid_high == vptx_sns_suid_lookup.suid_high) && (suid->suid_low == vptx_sns_suid_lookup.suid_low))
   {
      decode_arg->arg = (void *)&vptx_sns_decode_suid_event;

      event.events.funcs.decode = &vptx_sns_decode_events;
      event.events.arg          = (void *)decode_arg;
   }
   else
   {
      decode_arg->arg = (void *)&vptx_sns_decode_fcng_sns_event;

      event.events.funcs.decode = &vptx_sns_decode_events;
      event.events.arg          = (void *)decode_arg;
   }

   stream = pb_istream_from_buffer((pb_byte_t *)event_msg, event_msg_len);
   if (!pb_decode(&stream, sns_client_event_msg_fields, &event))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors error decoding event list: %s",
            pQmi_obj->session_num,
            PB_GET_ERROR(&stream));
   }

   qurt_elite_memory_free(decode_arg);
}

/*
 * This call back function gets invoked in the QMI thread context as a callback
 * routine with
 * each indication from Sensors.
 * Arguements :
 * user_handle : unique user handle for a client
 * msg_id      : msg id corresponding to holding pattern change indication by
 * sensors
 * ind_buf     : buffer containing response from sensors for the holding pattern
 * change
 * ind_buf_len : length of buffer containing response from sensors
 *
 * This function gets invoked in the QMI thread context as a callback routine with
 * each indication from Sensors.
 */
static void vptx_sns_indication_cb(elite_qmi_client_type user_handle,
                                   unsigned int          msg_id,
                                   void *                ind_buf,
                                   unsigned int          ind_buf_len,
                                   void *                ind_cb_data)
{
   // Read qmi_obj handle.
   // If execution has reached here, it means that Vptx state is still present.
   // Had Vptx issued a qmi_client_release or a qmi_client_delete_async_txn (basically
   // retracting its requests), then QMI retracts all pending transactions before releasing.
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)ind_cb_data;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx session # (%lx): Received SNS Indications, message ID %d,\
   forwarding to Vptx",
        pQmi_obj->session_num,
        msg_id);

   // Check user handle against cached Vptx handle.
   if (user_handle != pQmi_obj->user_handle)
   {
      // No course of action available.
      MSG_3(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_indication_cb : Vptx session # (%lx): SNS indications sent to wrong handle %lu, %lu.",
            pQmi_obj->session_num,
            user_handle,
            pQmi_obj->user_handle);
      return;
   }

   size_t                      ind_len = sizeof(sns_client_report_ind_msg_v01);
   elite_qmi_client_error_type err;
   qmi_idl_service_object_type svc_obj = SNS_CLIENT_SVC_get_service_object_v01();

   sns_client_report_ind_msg_v01 *ind_msg =
      (sns_client_report_ind_msg_v01 *)qurt_elite_memory_malloc(ind_len, QURT_ELITE_HEAP_DEFAULT);

   err = qmi_idl_message_decode(svc_obj, QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind_msg, ind_len);

   // Check for QMI Decode errors.
   if (ELITE_QMI_NO_ERR != err)
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_indication_cb : Vptx session # (%lx): QMI Decode error 0x%x",
            pQmi_obj->session_num,
            err);
      qurt_elite_memory_free(ind_msg);
      return;
   }

   sns_client_event_msg event = sns_client_event_msg_init_default;

   // Decode just the sns_client_event_msg in order to get the SUID
   pb_istream_t stream = pb_istream_from_buffer(ind_msg->payload, ind_msg->payload_len);
   if (pb_decode(&stream, sns_client_event_msg_fields, &event))
   {
      vptx_sns_handle_event_msg(&event.suid, ind_msg->payload, ind_msg->payload_len, pQmi_obj);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_indication_cb : Vptx session # (%lx): Error decoding Event Message: %s",
            pQmi_obj->session_num,
            PB_GET_ERROR(&stream));
   }

   qurt_elite_memory_free(ind_msg);

   // Notify Vptx thread about indication callback.
   qurt_elite_signal_send(pQmi_obj->ind_sig);
   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Received SNS Indications, signalled Vptx",
         pQmi_obj->session_num);
}

/**
 * Send a Facing enable request for the specified SUID.
 */
static bool vptx_sns_send_enable_req(sns_std_suid *suid, qmi_obj_t *pQmi_obj)
{
   bool rv = false;

   rv = vptx_sns_send_basic_req(pQmi_obj->user_handle, NULL, *suid, SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG, pQmi_obj);

   return rv;
}

/**
 * Send a QMI request message.
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] req_msg Completed request message to be sent
 *
 * @return true on success, false otherwise
 */
static bool vptx_sns_send_qmi_req(qmi_client_type qmi_handle, sns_client_req_msg_v01 const *req_msg, qmi_obj_t *pQmi_obj)
{
   bool   rv       = false;
   size_t resp_len = sizeof(sns_client_resp_msg_v01);
   void * resp     = qurt_elite_memory_malloc(resp_len, QURT_ELITE_HEAP_DEFAULT);

   MSG_3(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors sending request payload len %i, handle %d",
         pQmi_obj->session_num,
         req_msg->payload_len,
         (uint32_t)qmi_handle);

   if (NULL == resp)
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors unable to allocate sns_client_resp_msg_v01",
            pQmi_obj->session_num);
   }
   else
   {

      qmi_txn_handle        txn_handle;
      qmi_client_error_type qmi_err = qmi_client_send_msg_async(qmi_handle,
                                                                SNS_CLIENT_REQ_V01,
                                                                (void *)req_msg,
                                                                sizeof(*req_msg),
                                                                resp,
                                                                resp_len,
                                                                vptx_sns_registration_cb,
                                                                (void *)pQmi_obj,
                                                                &txn_handle);

      if (QMI_NO_ERR != qmi_err)
      {
         MSG_2(MSG_SSID_QDSP6,
               DBG_ERROR_PRIO,
               "VCP: Vptx session # (%lx): Vptx_sensors qmi_client_send_msg_async error %i",
               pQmi_obj->session_num,
               qmi_err);
         qurt_elite_memory_free(resp);
      }
      else
      {
         rv = true;
      }
   }

   return rv;
}

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 */
static bool vptx_sns_send_basic_req(qmi_client_type       qmi_handle,
                           vptx_sns_buffer_arg const *payload,
                           sns_std_suid          suid,
                           uint32_t              msg_id,
                           qmi_obj_t *           pQmi_obj)
{

   sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
   sns_client_req_msg_v01 req_msg;
   memset(&req_msg, 0, sizeof(sns_client_req_msg_v01));
   pb_ostream_t stream = pb_ostream_from_buffer(req_msg.payload, SNS_CLIENT_REQ_LEN_MAX_V01);

   pb_req_msg.suid                         = suid;
   pb_req_msg.msg_id                       = msg_id;
   pb_req_msg.request.has_batching         = false;
   pb_req_msg.request.payload.funcs.encode = &vptx_sns_encode_payload;
   pb_req_msg.request.payload.arg          = (void *)payload;

   MSG_2(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Vptx_sensors sending msg_id %ld",
         pQmi_obj->session_num,
         msg_id);

   if (!pb_encode(&stream, sns_client_request_msg_fields, &pb_req_msg))
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Vptx_sensors error encoding request: %s",
            pQmi_obj->session_num,
            PB_GET_ERROR(&stream));
      return false;
   }
   else
   {
      req_msg.payload_len = stream.bytes_written;
      vptx_sns_send_qmi_req(qmi_handle, &req_msg, pQmi_obj);
      return true;
   }
}

/*
 * The below function gets invoked in vptx init to allocate memory and  initialize :
 *   1. Registration signal pointer in vptx qmi structure(signal which will be set when call back is recieved
 *      from sensors after successful registration)
 *   2. Indication signal pointer in vptx qmi structure (signal which will be set when call back is recieved
 *      from sensors after holding pattern change)
*/
ADSPResult vptx_sns_qmi_init(void *                pQmi_object,
                             qurt_elite_channel_t *pChannel,
                             uint32_t              qmi_reg_mask,
                             uint32_t              qmi_ind_mask,
                             uint32_t              session_num)
{
   ADSPResult nResult = ADSP_EOK;
   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_qmi_init : NULL qmi object pointer !!");
      return ADSP_ENOMEMORY;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;
   // First default init for sensors structure
   pQmi_obj->user_handle     = NULL;
   pQmi_obj->holding_pattern = SNS_FACING_EVENT_STATE_UP; // initialize holding pattern with face up position
   pQmi_obj->instance_id     = 0;
   pQmi_obj->session_num     = session_num;

   pQmi_obj->reg_sig =
      (qurt_elite_signal_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pQmi_obj->ind_sig =
      (qurt_elite_signal_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pQmi_obj->reg_sig || NULL == pQmi_obj->ind_sig)
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_qmi_init : Failed to allocate memory for VpTx sensors Queues/Signals session(%#lx)",
            pQmi_obj->session_num);
      vptx_sns_qmi_destroy(pQmi_obj);
      return ADSP_ENOMEMORY;
   }

   if (ADSP_FAILED(nResult = qurt_elite_signal_init(pQmi_obj->reg_sig)) ||
       ADSP_FAILED(nResult = qurt_elite_signal_init(pQmi_obj->ind_sig)) ||
       ADSP_FAILED(nResult = qurt_elite_channel_add_signal(pChannel, (pQmi_obj->reg_sig), qmi_reg_mask)) ||
       ADSP_FAILED(nResult = qurt_elite_channel_add_signal(pChannel, (pQmi_obj->ind_sig), qmi_ind_mask))

          )
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_qmi_init : Failed create Vptx sensors MsgQs !! session(%lx)",
            pQmi_obj->session_num);
      vptx_sns_qmi_destroy(pQmi_obj);
      return nResult;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx QMI signals memalloc/Initialization Done !!");

   return nResult;
}

/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] data_type Sensor type to search for
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t vptx_sns_get_encoded_suid_req(char const *data_type, void **encoded_req)
{
   size_t       encoded_req_size;
   sns_suid_req suid_req = sns_suid_req_init_default;

   *encoded_req = NULL;

   vptx_sns_buffer_arg *buffer_arg =
      (vptx_sns_buffer_arg *)qurt_elite_memory_malloc(sizeof(vptx_sns_buffer_arg), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == buffer_arg)
   {
      MSG(MSG_SSID_QDSP6,
          DBG_HIGH_PRIO,
          "VCP: Vptx session # (%lx): Vptx_sensors failed to allocate memory for buffer_arg");
      return 0;		 
   }
   buffer_arg->buf     = data_type;
   buffer_arg->buf_len = strlen(data_type);

   suid_req.data_type.funcs.encode = &vptx_sns_encode_payload;
   suid_req.data_type.arg          = buffer_arg;
   suid_req.has_register_updates   = true;
   suid_req.register_updates       = true;

   if (!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vptx_sensors pb_get_encoded_size error");
   }
   else
   {
      void *       encoded_buffer = qurt_elite_memory_malloc(encoded_req_size, QURT_ELITE_HEAP_DEFAULT);
      pb_ostream_t stream         = pb_ostream_from_buffer((pb_byte_t *)encoded_buffer, encoded_req_size);

      if (!pb_encode(&stream, sns_suid_req_fields, &suid_req))
      {
         MSG_1(MSG_SSID_QDSP6,
               DBG_ERROR_PRIO,
               "VCP: Vptx_sensors error decoding facing sensor event: %s",
               PB_GET_ERROR(&stream));
      }
      else
      {
         *encoded_req = encoded_buffer;
      }
   }

   qurt_elite_memory_free(buffer_arg);

   return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send a request to the SUID Lookup Sensor for the complete list of SUIDs
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] data_type Sensor type to search for
 *
 * @return true if request was sent successfully, false otherwise
 */
static bool vptx_sns_send_suid_req(qmi_client_type qmi_handle, char const *data_type, qmi_obj_t *pQmi_obj)
{
   bool   rv          = false;
   void * encoded_req = NULL;

   size_t encoded_req_len = vptx_sns_get_encoded_suid_req(data_type, &encoded_req);
   if (NULL != encoded_req)
   {
      vptx_sns_buffer_arg payload = (vptx_sns_buffer_arg){.buf = encoded_req, .buf_len = encoded_req_len };
      rv = vptx_sns_send_basic_req(qmi_handle, &payload, vptx_sns_suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ, pQmi_obj);
      qurt_elite_memory_free(encoded_req);
   }

   return rv;
}

/*
 * The below function gets invoked in vptx run cmnd to place request to QMI for registration
 * this function will:
 *   1. Get the service object to communicate with sensors code
 *   2. Check if qmi service is up, and get the list of services
 *   3. qmi client initialization
 *   4. place asynchronous message to request for indications from Facing Service
*/
ADSPResult vptx_sns_qmi_register(void *pQmi_object)
{

#if defined(__qdsp6__) && !defined(SIM)

   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_qmi_register : NULL qmi object pointer !!");
      return ADSP_ENOMEMORY;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;
   /*------------------------------------------------------------*/
   /* QMI interfacing */
   /*------------------------------------------------------------*/
   elite_qmi_client_error_type retval;
   elite_qmi_service_info      info;
   elite_qmi_service_info      svc_under_use_info;
   unsigned int                num_services = 0;
   unsigned int                num_entries  = 1; // Accepting only 1 service.

   /*-----------------------------------------------------------------------
   Get Sensor service object
    -----------------------------------------------------------------------*/
   elite_qmi_idl_service_object_type sensor_svc_obj = SNS_CLIENT_SVC_get_service_object_v01();
   if (!sensor_svc_obj)
   {
      MSG_1(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: Vptx session # (%lx): Unable to get sensor service object",
            pQmi_obj->session_num);
      vptx_sns_qmi_deregister(pQmi_obj);
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Found Sensor service object",
         pQmi_obj->session_num);

   /*------------------------------------------------------------------
   Check if the Sensor  service is up, if not return. Else,
   Get addressing information for accessing wds service
   Currently getting entire service list, but this can be replaced to
   just talk to the first responder (quickest sensor service to respond).
   ------------------------------------------------------------------*/
   retval = elite_qmi_client_get_service_list(sensor_svc_obj, &info, &num_entries, &num_services);

   if (ELITE_QMI_NO_ERR != retval || 1 != num_services) // TODO : check if this is valid or it should be > 0
   {
      MSG_3(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_qmi_register : Vptx session # (%lx): Get Sensor service list error %d, num_services =%u",
            pQmi_obj->session_num,
            retval,
            num_entries);
      vptx_sns_qmi_deregister(pQmi_obj);
      return ADSP_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Sensor service UP num_entries = %u, num_services = %u",
         pQmi_obj->session_num,
         num_entries,
         num_services);

   svc_under_use_info = info;

   retval = elite_qmi_client_init(&svc_under_use_info,
                                  sensor_svc_obj,
                                  &vptx_sns_indication_cb,
                                  (void *)pQmi_obj,
                                  NULL,
                                  &pQmi_obj->user_handle);
   if (ELITE_QMI_NO_ERR != retval)
   {
      MSG_2(MSG_SSID_QDSP6,
            DBG_ERROR_PRIO,
            "VCP: vptx_sns_qmi_register : Vptx session # (%lx): Sensor client init failed with error %d",
            pQmi_obj->session_num,
            retval);
      vptx_sns_qmi_deregister(pQmi_obj);
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: Vptx session # (%lx): Sensor client init succeeded...",
         pQmi_obj->session_num);

   /* Prepare and send the request */
   vptx_sns_send_suid_req(pQmi_obj->user_handle, "facing", pQmi_obj);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx QMI sensors registration request Done !!");
#endif
   return ADSP_EOK;
}

/*
 * The below function gets invoked in vptx end to free memory allocated for :
 *   1. Registration signal pointer in vptx qmi structure(signal set when call back is recieved
 *      from sensors after successful registration)
 *   2. Indication signal pointer in vptx qmi structure (signal set when call back is recieved
 *      from sensors after holding pattern change)
*/
void vptx_sns_qmi_destroy(void *pQmi_object)
{
   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_qmi_destroy : NULL qmi object pointer !!");
      return;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;

   // It can now be assumed that there are no more QMI callback
   // activities pending that will write to Vptx memory. We can
   // clean up all associated memory.
   // call utility function to deinitialize and destroy all signals
   if (NULL != pQmi_obj->reg_sig)
   {
      qurt_elite_signal_deinit(pQmi_obj->reg_sig);
      qurt_elite_memory_free(pQmi_obj->reg_sig);
      pQmi_obj->reg_sig = NULL;
   }
   if (NULL != pQmi_obj->ind_sig)
   {
      qurt_elite_signal_deinit(pQmi_obj->ind_sig);
      qurt_elite_memory_free(pQmi_obj->ind_sig);
      pQmi_obj->ind_sig = NULL;
   }

   return;
}

/*
 * The below function gets invoked in vptx stop cmnd to deregister from  QMI
 * this function will:
 *   1. set registration flag to FALSE
 *   2. release the client (deregister from qmi)
 *   3. free memory allocated for response and request queues
*/
void vptx_sns_qmi_deregister(void *pQmi_object)
{
#if defined(__qdsp6__) && !defined(SIM)

   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_qmi_deregister : NULL qmi object pointer !!");
      return;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;

   // First place Blocking call that will delete all the currently
   // placed requests from Vptx client.

   MSG_1(MSG_SSID_QDSP6,
         DBG_HIGH_PRIO,
         "VCP: vptx deregistering from QMI : user handle (%lu) !!",
         (uint32_t)pQmi_obj->user_handle);
   if (NULL != pQmi_obj->user_handle)
   {
      elite_qmi_client_release(pQmi_obj->user_handle);
      pQmi_obj->user_handle = NULL;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx deregistration done from QMI !!");

#endif
   return;
}

/*
 * The below function gets invoked whenever holding pattern
 * information is required by framework.
 * It will return the current holding pattern value.
*/
int32_t vptx_sns_get_holding_pattern(void *pQmi_object)
{
   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_get_holding_pattern : NULL qmi object pointer !!");
      return 1;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;

   return (int32_t)pQmi_obj->holding_pattern;
}

/*
 * The below function will clear the registration and indication signals
*/
void vptx_sns_clear_qmi_signal(void *pQmi_object, reg_ind_signal_t sig_flag)
{
   if (NULL == pQmi_object)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vptx_sns_clear_qmi_signal : NULL qmi object pointer !!");
      return;
   }
   qmi_obj_t *pQmi_obj = (qmi_obj_t *)pQmi_object;

   if (REG_SIG == sig_flag)
   {
      qurt_elite_signal_clear(pQmi_obj->reg_sig);
   }
   else
   {
      qurt_elite_signal_clear(pQmi_obj->ind_sig);
   }
   return;
}
