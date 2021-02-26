/*===========================================================================

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/16   ks      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_uim_util.h"
#include "qcril_uim_http.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client.h"
#include "user_identity_module_http_v01.h"


/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/* QMI http related defines */
#define QCRIL_UIM_HTTP_QMI_SVC_TIMEOUT               5000

#define QCRIL_QMI_UIM_HTTP_FREE_PTR(ptr)                                  \
  if (ptr != NULL)                                                          \
  {                                                                         \
    qcril_free(ptr);                                                        \
    ptr = NULL;                                                             \
  }                                                                         \


/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_HTTP_IND_PARAMS_TYPE

   DESCRIPTION:
     Structure used to copy indications received from the modem
-------------------------------------------------------------------------------*/
typedef struct
{
  qcril_instance_id_e_type         instance_id;
  qcril_modem_id_e_type            modem_id;
  qmi_client_type                  handle;
  unsigned int                     msg_id;
  void                           * msg_ptr;
} qcril_uim_http_ind_params_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_HTTP_CB_PARAMS_TYPE

   DESCRIPTION:
     Structure used to copy response received from the modem
-------------------------------------------------------------------------------*/
typedef struct
{
  unsigned int                         msg_id;
  void                               * msg_ptr;
  qmi_client_error_type                transp_err;
  qcril_uim_original_request_type    * orig_req_ptr;
} qcril_uim_http_cb_params_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_UIM_HTTP_RESPONSE_TO_TRANSACTION_TYPE

   DESCRIPTION:
     Structure used to copy response received from the client
-------------------------------------------------------------------------------*/
typedef struct
{
  uim_http_result_type_enum_v01 result;
  int32_t  token_id;
  uint32_t num_chunks;
  uint32_t current_chunk;
  uint32_t total_size;
  uint8_t *http_request_payload_ptr;
} qcril_uim_http_response_to_transaction_type;

/* Static Global QMI HTTP handle */
static qmi_client_type                                           qmi_http_handle             = NULL;
/* Globals to keep track of HTTP request/response payload chunks */
static uint8_t                                                 * http_trasaction_payload_ptr  = NULL;
static qcril_uim_http_response_to_transaction_type               response_to_transaction_data = {0, 0, 0, 0, 0, NULL};
static qcril_uim_http_transaction_ind_type                       http_transaction_ind;
static qmi_client_type                                           qmi_http_notifier            = NULL;
static qmi_client_os_params                                      qmi_http_notify_os_params;
static qmi_client_os_params                                      qmi_http_client_init_os_params;


/*=========================================================================

  FUNCTION:  qcril_uim_http_copy_indication

  Description : Makes a copy of the indication received from QMI UIM HTTP.

===========================================================================*/
static qcril_uim_http_ind_params_type  * qcril_uim_http_copy_indication
(
  qmi_client_type                user_handle_ptr,
  unsigned int                   msg_id,
  unsigned char                * qmi_http_ind_ptr,
  unsigned int                   qmi_http_ind_len,
  uint32_t                     * out_len_ptr
)
{
  void                             * decoded_payload_ptr  = NULL;
  uint32_t                           decoded_payload_len  = 0;
  qcril_uim_http_ind_params_type   * out_ptr              = NULL;
  qmi_client_error_type              qmi_err              = QMI_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if ((user_handle_ptr == NULL) ||
      (qmi_http_ind_ptr == NULL) ||
      (out_len_ptr     == NULL))
  {
    return NULL;
  }

  /* First decode the message payload from QCCI */
  qmi_idl_get_message_c_struct_len(uim_http_get_service_object_v01(),
                                   QMI_IDL_INDICATION,
                                   msg_id,
                                   &decoded_payload_len);

  if (decoded_payload_len == 0)
  {
    QCRIL_LOG_ERROR("Failed to find decoded_payload_len");
    return NULL;
  }

  /* Allocate decoded payload buffer */
  decoded_payload_ptr = qcril_malloc(decoded_payload_len);
  if (decoded_payload_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Failed to allocate payload ptr, payload len: 0x%x\n",
                    decoded_payload_len);
    return NULL;
  }

  /* Decode the Indication payload */
  qmi_err = qmi_client_message_decode(user_handle_ptr,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      qmi_http_ind_ptr,
                                      qmi_http_ind_len,
                                      decoded_payload_ptr,
                                      decoded_payload_len);
  if (qmi_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Failed to decode Indication: 0x%x, qmi_err: 0x%x", msg_id, qmi_err);
    QCRIL_QMI_UIM_HTTP_FREE_PTR(decoded_payload_ptr);
    return NULL;
  }

  /* Note: out_ptr and decoded_payload_ptr will be freed after
     processing the event in qcril_uim_http_process_qmi_indication */
  out_ptr = qcril_malloc(sizeof(qcril_uim_http_ind_params_type));
  if (out_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "out_ptr alloc failed");
    QCRIL_QMI_UIM_HTTP_FREE_PTR(decoded_payload_ptr);
    return NULL;
  }

  /* Initialize the payload data & assign the message data pointer */
  *out_len_ptr = sizeof(qcril_uim_http_ind_params_type);
  memset(out_ptr, 0, sizeof(qcril_uim_http_ind_params_type));
  out_ptr->handle      = user_handle_ptr;
  out_ptr->msg_id      = msg_id;
  out_ptr->msg_ptr     = decoded_payload_ptr;
  out_ptr->modem_id    = QCRIL_DEFAULT_MODEM_ID;
  out_ptr->instance_id = qmi_ril_get_process_instance_id();

  return out_ptr;
} /* qcril_uim_http_copy_indication */


/*=========================================================================

  FUNCTION:  qcril_uim_http_copy_callback

  Description : Makes a copy of the response received from QMI UIM HTTP.

===========================================================================*/
static qcril_uim_http_cb_params_type * qcril_uim_http_copy_callback
(
  unsigned int                         msg_id,
  void                               * resp_data_ptr,
  void                               * resp_cb_data_ptr,
  qmi_client_error_type                transp_err,
  uint32_t                           * out_len_ptr
)
{
  qcril_uim_http_cb_params_type   * out_ptr  = NULL;

  /* Sanity check */
  if ((resp_data_ptr    == NULL) ||
      (resp_cb_data_ptr == NULL) ||
      (out_len_ptr      == NULL))
  {
    return NULL;
  }

  /* Update size & allocate buffer */
  *out_len_ptr = sizeof(qcril_uim_http_cb_params_type);
  out_ptr = (qcril_uim_http_cb_params_type*)qcril_malloc(*out_len_ptr);
  if (out_ptr == NULL)
  {
    return NULL;
  }

  memset(out_ptr, 0, *out_len_ptr);

  /* Copy the response parameters */
  out_ptr->msg_id        = msg_id;
  out_ptr->msg_ptr       = resp_data_ptr;
  out_ptr->transp_err    = transp_err;
  out_ptr->orig_req_ptr  = (qcril_uim_original_request_type*)resp_cb_data_ptr;

  return out_ptr;
} /* qcril_uim_http_copy_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_http_transaction_resp

  Description : This function sends response to the client (APPS) for the request it sent

===========================================================================*/
static void qcril_uim_http_transaction_resp
(
  qcril_uim_original_request_type      * original_req_ptr,
  uim_http_transaction_resp_msg_v01    * resp_msg_ptr
)
{
  void                        * token_ptr;
  qcril_uim_http_result_type    result;

  if ((original_req_ptr == NULL) || (resp_msg_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "NULL original_req_ptr or resp_msg_ptr");
    return;
  }

  /* Retreive token */
  token_ptr = original_req_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_http_transaction_resp: result=0x%x, error=0x%x",
                    resp_msg_ptr->resp.result,
                    resp_msg_ptr->resp.error);

  /* Update the response error code */
  result = (resp_msg_ptr->resp.result == QMI_RESULT_SUCCESS_V01) ?
                            QCRIL_UIM_HTTP_RESULT_SUCCESS :
                            QCRIL_UIM_HTTP_RESULT_FAILURE;

  /* Generate response */
  qcril_uim_lpa_http_txn_completed_response(token_ptr, result);
} /* qcril_uim_http_transaction_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_http_generic_response_callback

  Description : Callback for two things
                1. the error transaction request send to MODEM
                2. reset request send to MODEM

===========================================================================*/
static void qcril_uim_http_generic_response_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_http_rsp_ptr,
  unsigned int                   qmi_http_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  QCRIL_NOTUSED(user_handle);
  QCRIL_NOTUSED(qmi_http_rsp_len);
  QCRIL_NOTUSED(resp_cb_data_ptr);

  QCRIL_LOG_INFO( "qcril_uim_http_generic_response_callback: msg_id = 0x%x, with transp err 0x%x ", msg_id, transp_err);

  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
} /* qcril_uim_http_generic_response_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_http_send_error_response_to_current_transaction

  Description : This function sends response to the Modem for any failure
                 in the transaction indication

===========================================================================*/
static void qcril_uim_http_send_error_response_to_current_transaction
(
  void
)
{
  int                                                  qmi_err_code     = 0;
  uim_http_transaction_req_msg_v01                   * qmi_http_req_ptr = NULL;
  uim_http_transaction_resp_msg_v01                  * qmi_http_rsp_ptr = NULL;
  qmi_txn_handle                                       txn_handle;

  if(http_transaction_ind.tokenId == 0)
  {
    /* do nothing if the token ID is invalid */
    return;
  }

  qmi_http_req_ptr = (uim_http_transaction_req_msg_v01 *) qcril_malloc(sizeof(uim_http_transaction_req_msg_v01));
  qmi_http_rsp_ptr = (uim_http_transaction_resp_msg_v01 *) qcril_malloc(sizeof(uim_http_transaction_resp_msg_v01));

  if ((qmi_http_rsp_ptr == NULL) || (qmi_http_req_ptr == NULL))
  {
    QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
    QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
    QCRIL_LOG_ERROR("%s", "error allocating memory for req or rsp pointers");
    return;
  }

  /* Fill the qmi_http_req_ptr with token ID and result as error */
  memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));
  qmi_http_req_ptr->token_id = http_transaction_ind.tokenId;
  qmi_http_req_ptr->result   = UIM_HTTP_UNKNOWN_ERROR_V01;

  qmi_err_code = qmi_client_send_msg_async(
                 qmi_http_handle,
                 QMI_UIM_HTTP_TRANSACTION_REQ_V01,
                 (void *) qmi_http_req_ptr,
                 sizeof(uim_http_transaction_req_msg_v01),
                 (void *) qmi_http_rsp_ptr,
                 sizeof(uim_http_transaction_resp_msg_v01),
                 qcril_uim_http_generic_response_callback,
                 NULL,
                &txn_handle);

  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("%s", "Failure in sending the error notification to modem");
    QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
  }
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
} /* qcril_uim_http_send_error_response_to_current_transaction */


/*===========================================================================

  FUNCTION:  qcril_uim_http_handle_transaction_ind

  Description : Handles the HTTP indication

===========================================================================*/
static void qcril_uim_http_handle_transaction_ind
(
  uim_http_transaction_ind_msg_v01        * ind_msg_ptr
)
{
  uint8                                 index   = 0;
  qcril_uim_http_transaction_ind_type   txn_ind;
  qcril_uim_http_custom_header_type   * headers = NULL;

  if (ind_msg_ptr == NULL)
  {
    return;
  }

  QCRIL_LOG_INFO( "%s token_id:0x%x", __FUNCTION__, ind_msg_ptr->token_id);

  memset(&txn_ind, 0x00, sizeof(txn_ind));

  /* Token ID should be same for all the chunks of a http command payload */
  if (http_trasaction_payload_ptr != NULL && (uint32_t)http_transaction_ind.tokenId != ind_msg_ptr->token_id)
  {
    QCRIL_LOG_INFO("Token ID received is different than expected, send error to previous token and process the new indication");
    goto report_error_to_modem;
  }

  /* http_transaction_payload_ptr will be NULL for the first chunk
        of a http request payload from the MODEM */
  if(http_trasaction_payload_ptr == NULL)
  {
    http_trasaction_payload_ptr = qcril_malloc(ind_msg_ptr->segment_info.total_size);
    if (http_trasaction_payload_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for http_trasaction_payload_ptr");
      goto report_error_to_modem;
    }
    memset(&http_transaction_ind, 0, sizeof(http_transaction_ind));
    http_transaction_ind.tokenId = ind_msg_ptr->token_id;
  }

  /* Copying URL */
  if (ind_msg_ptr->url_valid)
  {
    http_transaction_ind.url = qcril_malloc(strlen(ind_msg_ptr->url) + 1);
    if (http_transaction_ind.url == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for http_trasaction_ind url");
      goto report_error_to_modem;
    }
    strlcpy(http_transaction_ind.url,
            ind_msg_ptr->url,
            strlen(ind_msg_ptr->url) + 1);
  }

  /* Copying Content type */
  if (ind_msg_ptr->headers_valid)
  {
    http_transaction_ind.contentType = qcril_malloc(strlen(ind_msg_ptr->headers.content_type) + 1);
    if (http_transaction_ind.contentType == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for http_trasaction_ind contentType");
      goto report_error_to_modem;
    }
    strlcpy(http_transaction_ind.contentType,
            ind_msg_ptr->headers.content_type,
            strlen(ind_msg_ptr->headers.content_type) + 1);

    headers = (qcril_uim_http_custom_header_type *)qcril_malloc(
      ind_msg_ptr->headers.custom_header_len * sizeof(qcril_uim_http_custom_header_type));
    if (headers == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for headers");
      goto report_error_to_modem;
    }

    for(index = 0;
        index < ind_msg_ptr->headers.custom_header_len && index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
        index++)
    {
      headers[index].headerName = 
        (char *)qcril_malloc(strlen(ind_msg_ptr->headers.custom_header[index].name) + 1);
      if (headers[index].headerName == NULL)
      {
        QCRIL_LOG_ERROR("%s", "error allocating memory for header name");
        goto report_error_to_modem;
      }
      strlcpy(headers[index].headerName,
              ind_msg_ptr->headers.custom_header[index].name,
              strlen(ind_msg_ptr->headers.custom_header[index].name) + 1);

      headers[index].headerValue =
        (char *)qcril_malloc(strlen(ind_msg_ptr->headers.custom_header[index].value) + 1);
      if (headers[index].headerValue == NULL)
      {
        QCRIL_LOG_ERROR("%s", "error allocating memory for header value");
        goto report_error_to_modem;
      }
      strlcpy(headers[index].headerValue,
              ind_msg_ptr->headers.custom_header[index].value,
              strlen(ind_msg_ptr->headers.custom_header[index].value) + 1);
    }
    http_transaction_ind.customHeaders = headers;
    http_transaction_ind.no_of_headers = ind_msg_ptr->headers.custom_header_len;
  }

  /* Copying Payload */
  if ( (ind_msg_ptr->payload_body_valid != 0) && (ind_msg_ptr->payload_body_len > 0) )
  {
    if(ind_msg_ptr->segment_info.segment_offset + ind_msg_ptr->payload_body_len <= ind_msg_ptr->segment_info.total_size)
    {
      memcpy(http_trasaction_payload_ptr + ind_msg_ptr->segment_info.segment_offset,
             ind_msg_ptr->payload_body,
             ind_msg_ptr->payload_body_len);
    }
    else
    {
      goto report_error_to_modem;
    }
  }

  QCRIL_LOG_INFO( "Received the http transaction request payload. Received bytes 0x%x, total payload size 0x%x",
                                       ind_msg_ptr->payload_body_len + ind_msg_ptr->segment_info.segment_offset,
                                       ind_msg_ptr->segment_info.total_size);

  /* Send the entire http payload to the client after receiving the total payload from multiple chunk indications */
  if(ind_msg_ptr->payload_body_len + ind_msg_ptr->segment_info.segment_offset
     == ind_msg_ptr->segment_info.total_size)
  {
    http_transaction_ind.payload_len  = ind_msg_ptr->segment_info.total_size;
    http_transaction_ind.payload = http_trasaction_payload_ptr;

    qcril_uim_lpa_http_txn_indication(&http_transaction_ind);
    goto cleanup_memory;
  }

  return;

report_error_to_modem:
  qcril_uim_http_send_error_response_to_current_transaction();

cleanup_memory:
  QCRIL_QMI_UIM_HTTP_FREE_PTR(http_transaction_ind.url);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(http_transaction_ind.contentType);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(http_trasaction_payload_ptr);
  if(headers != NULL)
  {
    for(index = 0;
        index < ind_msg_ptr->headers.custom_header_len && index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
        index++)
    {
      QCRIL_QMI_UIM_HTTP_FREE_PTR(headers[index].headerName);
      QCRIL_QMI_UIM_HTTP_FREE_PTR(headers[index].headerValue);
    }
    QCRIL_QMI_UIM_HTTP_FREE_PTR(headers);
  }
} /* qcril_uim_http_handle_transaction_ind */


/*=========================================================================

  FUNCTION:  qcril_uim_http_process_qmi_indication

===========================================================================*/
void qcril_uim_http_process_qmi_indication
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_http_ind_params_type  * ind_params_ptr = NULL;

  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    return;
  }

  ind_params_ptr = (qcril_uim_http_ind_params_type*)params_ptr->data;
  if (ind_params_ptr == NULL)
  {
    return;
  }

  if (ind_params_ptr->msg_ptr == NULL)
  {
    goto clean_up;
  }

  if ((ind_params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (ind_params_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    goto clean_up;
  }

  QCRIL_LOG_INFO("qcril_uim_http_process_qmi_indication, msg_id: 0x%x", ind_params_ptr->msg_id);
  /* Process respective indications */
  switch(ind_params_ptr->msg_id)
  {
    case QMI_UIM_HTTP_TRANSACTION_IND_V01:
      qcril_uim_http_handle_transaction_ind(ind_params_ptr->msg_ptr);
      break;

    default:
      /* This shouldn't happen since we never post for these msg ids */
      QCRIL_LOG_ERROR("Unsupported QMI UIM LPA HTTP indication: 0x%x", ind_params_ptr->msg_id);
      break;
  }

clean_up:
  /* Free memory allocated previously. */
  QCRIL_QMI_UIM_HTTP_FREE_PTR(ind_params_ptr->msg_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(ind_params_ptr);
} /* qcril_uim_http_process_qmi_indication */


/*=========================================================================

  FUNCTION:  qcril_uim_http_ind_callback

  Description :
  Callback implementation for the QMI UIM HTTP indications. This will be
  called by QMI FW when the QMI UIM HTTP in the modem sends indications.

===========================================================================*/
static void qcril_uim_http_ind_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  unsigned char                * qmi_http_ind_ptr,
  unsigned int                   qmi_http_ind_len,
  void                         * ind_cb_data_ptr
)
{
  uint32_t                            ind_params_len = 0;
  qcril_uim_http_ind_params_type     *ind_params_ptr = NULL;
  IxErrnoType                         result         = E_FAILURE;

  QCRIL_NOTUSED(ind_cb_data_ptr);

  QCRIL_LOG_INFO("qcril_uim_http_ind_callback, msg_id: 0x%x", msg_id);

  if ((qmi_http_ind_ptr == NULL) || (qmi_http_ind_len == 0))
  {
    return;
  }

  /* Process only the supported IND messages */
  switch (msg_id)
  {
    case QMI_UIM_HTTP_TRANSACTION_IND_V01:
      ind_params_ptr = qcril_uim_http_copy_indication(user_handle,
                                                      msg_id,
                                                      qmi_http_ind_ptr,
                                                      qmi_http_ind_len,
                                                      &ind_params_len);
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported QMI UIM HTTP indication: 0x%x", msg_id);
      break;
  }

  if ((ind_params_ptr == NULL) || (ind_params_len == 0))
  {
    QCRIL_LOG_ERROR("Error copying the indication msg_id: 0x%x", msg_id);
    return;
  }

  /* Post the event to process the indication callback.
     Note: msg_ptr will be freed in the indication handler */
  QCRIL_LOG_INFO( "%s qcril_event_queue ", __FUNCTION__);
  result = qcril_event_queue( ind_params_ptr->instance_id,
                              ind_params_ptr->modem_id,
                              QCRIL_DATA_NOT_ON_STACK,
                              QCRIL_EVT_UIM_HTTP_QMI_INDICATION_CALLBACK,
                              (void *)ind_params_ptr,
                              ind_params_len,
                              NULL);
  if (result != E_SUCCESS)
  {
    QCRIL_LOG_ERROR( " qcril_event_queue failed, result: 0x%x\n", result);

    /* Free allocated memory in case event queueing fails */
    QCRIL_QMI_UIM_HTTP_FREE_PTR(ind_params_ptr->msg_ptr);
    QCRIL_QMI_UIM_HTTP_FREE_PTR(ind_params_ptr);
  }
} /* qcril_uim_http_ind_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_http_transaction_completed_callback

  Description : Callback implementation for the QMI UIM HTTP commands.

===========================================================================*/
static void qcril_uim_http_transaction_completed_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_http_rsp_ptr,
  unsigned int                   qmi_http_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  uint32_t                          params_len = 0;
  qcril_uim_http_cb_params_type   * params_ptr = NULL;
  IxErrnoType                       result     = E_FAILURE;

  QCRIL_NOTUSED(user_handle);
  QCRIL_NOTUSED(qmi_http_rsp_len);

  QCRIL_LOG_INFO( "qcril_uim_http_transaction_completed_callback: msg_id = 0x%x ", msg_id);

  if ((qmi_http_rsp_ptr == NULL) || (resp_cb_data_ptr == NULL))
  {
    goto clean_up;
  }

  /* Process only the supported RESP messages */
  switch (msg_id)
  {
    case QMI_UIM_HTTP_TRANSACTION_RESP_V01:
      params_ptr = qcril_uim_http_copy_callback(msg_id,
                                                qmi_http_rsp_ptr,
                                                resp_cb_data_ptr,
                                                transp_err,
                                                &params_len);
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported QMI UIM HTTP response: 0x%x", msg_id);
      break;
  }

  if ((params_ptr == NULL) || (params_len == 0))
  {
    QCRIL_LOG_ERROR("Error copying the response msg_id: 0x%x", msg_id);
    goto clean_up;
  }

  /* Post the event to process the response callback.
     Note: Upon successful posting of the event, necessary pointers will
     be freed in the response handler */
  QCRIL_LOG_INFO( "%s qcril_event_queue ", __FUNCTION__);
  result = qcril_event_queue( params_ptr->orig_req_ptr->instance_id,
                              params_ptr->orig_req_ptr->modem_id,
                              QCRIL_DATA_NOT_ON_STACK,
                              QCRIL_EVT_UIM_HTTP_QMI_TRANSACTION_COMPLETED_CALLBACK,
                              (void *)params_ptr,
                              params_len,
                              NULL);
  if (result == E_SUCCESS)
  {
    return;
  }

  QCRIL_LOG_ERROR( " qcril_event_queue failed, result: 0x%x\n", result);

clean_up:
  /* Free allocated pointers only if event queueing fails */
  QCRIL_QMI_UIM_HTTP_FREE_PTR(resp_cb_data_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(params_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(response_to_transaction_data.http_request_payload_ptr);
  memset(&response_to_transaction_data, 0, sizeof(qcril_uim_http_response_to_transaction_type));
} /* qcril_uim_http_transaction_completed_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_http_process_transaction_completed_qmi_callback

  Description : This function sends the http response payload to the MODEM in chunks

===========================================================================*/
void qcril_uim_http_process_transaction_completed_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qmi_txn_handle                           txn_handle;
  int                                      qmi_err_code        = 0;
  qcril_uim_http_cb_params_type          * callback_params_ptr = NULL;
  uim_http_transaction_req_msg_v01       * qmi_http_req_ptr    = NULL;
  uim_http_transaction_resp_msg_v01      * qmi_http_rsp_ptr    = NULL;
  qcril_uim_original_request_type        * orig_req_ptr        = NULL;

  /* Sanity checks */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    return;
  }

  callback_params_ptr = (qcril_uim_http_cb_params_type*)params_ptr->data;
  if (callback_params_ptr == NULL)
  {
    return;
  }

  if (callback_params_ptr->msg_ptr == NULL)
  {
    goto clean_up;
  }

  if (callback_params_ptr->orig_req_ptr == NULL)
  {
    goto clean_up;
  }

  if ((callback_params_ptr->orig_req_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (callback_params_ptr->orig_req_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, modem_id: 0x%x",
                    callback_params_ptr->orig_req_ptr->instance_id,
                    callback_params_ptr->orig_req_ptr->modem_id);
    goto clean_up;
  }

  QCRIL_LOG_DEBUG("%s: Response for msg_id: 0x%x, token=%d, transp_err: 0x%x",
                  __FUNCTION__,
                  callback_params_ptr->msg_id,
                  qcril_log_get_token_id((RIL_Token)callback_params_ptr->orig_req_ptr->token),
                  callback_params_ptr->transp_err);

  /* The first chunk is always sent from the qcril_uim_http_client_transaction_request() called by the client.
     This function does response processing for one of the response payload chunk sent from APPs to MODEM.
     If total response payload is less than MAX_PAYLOAD (2KB), num_chunks value will be 1. */
  response_to_transaction_data.current_chunk++;

  /* Check to see if there are still chunks left that needs to be sent to MODEM */
  if(response_to_transaction_data.current_chunk < response_to_transaction_data.num_chunks)
  {
    /* Allocate memory for the new QMI HTTP transaction request where the next payload chunk will be sent */
    qmi_http_req_ptr = (uim_http_transaction_req_msg_v01 *) qcril_malloc(sizeof(uim_http_transaction_req_msg_v01));
    if (qmi_http_req_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_req_ptr");
      goto clean_up;
    }

    memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));
    qmi_http_req_ptr->token_id = response_to_transaction_data.token_id;
    qmi_http_req_ptr->result   = response_to_transaction_data.result;
    qmi_http_req_ptr->segment_info_valid = 0x01;
    qmi_http_req_ptr->payload_body_valid = 0x01;
    qmi_http_req_ptr->segment_info.total_size = response_to_transaction_data.total_size;
    qmi_http_req_ptr->segment_info.segment_offset = (response_to_transaction_data.current_chunk
                                                      * UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01);

    if ((qmi_http_req_ptr->segment_info.total_size -
         qmi_http_req_ptr->segment_info.segment_offset)
         > UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 )
    {
      qmi_http_req_ptr->payload_body_len = UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;
    }
    else
    {
      qmi_http_req_ptr->payload_body_len =
       (qmi_http_req_ptr->segment_info.total_size - qmi_http_req_ptr->segment_info.segment_offset);
    }
    memcpy(qmi_http_req_ptr->payload_body,
           response_to_transaction_data.http_request_payload_ptr + qmi_http_req_ptr->segment_info.segment_offset,
           qmi_http_req_ptr->payload_body_len);

    /* Allocate original request */
    orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                   QCRIL_MAX_MODEM_ID - 1,
                                                   (RIL_Token)params_ptr->t,
                                                   params_ptr->event_id,
                                                   0);

    if (orig_req_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
      goto clean_up;
    }
    /* Allocate response pointer since it is an async command */
    qmi_http_rsp_ptr = (uim_http_transaction_resp_msg_v01 *)
                        qcril_malloc(sizeof(uim_http_transaction_resp_msg_v01));
    if (qmi_http_rsp_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_rsp_ptr");
      goto clean_up;
    }
    memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_transaction_resp_msg_v01));
    /* Proceed with transaction request */
    QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_http_service", "qcril_uim_http_process_transaction_completed_qmi_callback" );
    qmi_err_code = qmi_client_send_msg_async(
                     qmi_http_handle,
                     QMI_UIM_HTTP_TRANSACTION_REQ_V01,
                     (void *) qmi_http_req_ptr,
                     sizeof(uim_http_transaction_req_msg_v01),
                     (void *) qmi_http_rsp_ptr,
                     sizeof(uim_http_transaction_resp_msg_v01),
                     qcril_uim_http_transaction_completed_callback,
                     orig_req_ptr,
                     &txn_handle);

    /* On successful API call, free only the request */
    if (qmi_err_code != 0)
    {
      goto clean_up;
    }

    QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
    return;
  }

  /* When the entire response payload is sent, the client is notified using qcril_uim_http_transaction_resp */
  switch(callback_params_ptr->msg_id)
  {
   case QMI_UIM_HTTP_TRANSACTION_RESP_V01:
       qcril_uim_http_transaction_resp(callback_params_ptr->orig_req_ptr,
                                  callback_params_ptr->msg_ptr);
       break;

   default:
       /* This shouldn't happen since we never post for these msg ids */
       QCRIL_LOG_ERROR("Unsupported QMI UIM HTTP response: 0x%x",
                       callback_params_ptr->msg_id);
       break;
   }

clean_up:
  /* Free memory allocated previously.*/
  QCRIL_QMI_UIM_HTTP_FREE_PTR(callback_params_ptr->orig_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(callback_params_ptr->msg_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(callback_params_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(response_to_transaction_data.http_request_payload_ptr);
  memset(&response_to_transaction_data, 0, sizeof(qcril_uim_http_response_to_transaction_type));
} /* qcril_uim_http_process_transaction_completed_qmi_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_http_client_transaction_request

  Description : Handles UIM_HTTP_TRANSACTION request from the client.

===========================================================================*/
void qcril_uim_http_client_transaction_request
(
  void                                 * token_ptr,
  qcril_uim_http_transaction_req_type  * http_req_ptr
)
{
  qmi_txn_handle                          txn_handle;
  uint8                                   header_index     = 0;
  int                                     qmi_err_code     = 0;
  uim_http_transaction_req_msg_v01      * qmi_http_req_ptr = NULL;
  uim_http_transaction_resp_msg_v01     * qmi_http_rsp_ptr = NULL;
  qcril_uim_original_request_type       * orig_req_ptr     = NULL;
  qcril_uim_http_transaction_req_type   * in_ptr           = http_req_ptr;
  qcril_uim_http_custom_header_req_type * custom_hdr_ptr   = NULL;

  /* Initialize the global used to store the response of the http request */
  QCRIL_QMI_UIM_HTTP_FREE_PTR(response_to_transaction_data.http_request_payload_ptr);
  memset(&response_to_transaction_data, 0, sizeof(qcril_uim_http_response_to_transaction_type));

  if(in_ptr == NULL)
  {
    goto report_error_to_modem;
  }

  QCRIL_LOG_INFO("qcril_uim_http_client_http_request");

  /* Allocate QMI HTTP transaction request for the first response payload chunk to be sent */
  qmi_http_req_ptr = (uim_http_transaction_req_msg_v01 *) qcril_malloc(sizeof(uim_http_transaction_req_msg_v01));
  if (qmi_http_req_ptr == NULL)
  {
    goto report_error_to_modem;
  }

  if(in_ptr->payload == NULL && in_ptr->payload_len != 0)
  {
     goto report_error_to_modem;
  }

  /* Update the global response_to_transaction_data. This global will be used for the sending the subsequent chunks */
  if(in_ptr->payload_len == 0)
  {
    response_to_transaction_data.num_chunks = 1;
  }
  else
  {
    response_to_transaction_data.num_chunks = ((in_ptr->payload_len - 1) / UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) + 1;
    response_to_transaction_data.http_request_payload_ptr = qcril_malloc(in_ptr->payload_len);
    if (response_to_transaction_data.http_request_payload_ptr  == NULL)
    {
       goto report_error_to_modem;
    }
  }

  response_to_transaction_data.current_chunk   = 0;
  response_to_transaction_data.result          = (in_ptr->result ==
                                                  QCRIL_UIM_HTTP_RESULT_SUCCESS) ?
                                                  UIM_HTTP_TRANSACTION_SUCCESSFUL_V01 : UIM_HTTP_UNKNOWN_ERROR_V01;

  response_to_transaction_data.token_id        = in_ptr->tokenId;
  response_to_transaction_data.total_size      = in_ptr->payload_len;

  if(in_ptr->payload_len > 0)
  {
    memcpy(response_to_transaction_data.http_request_payload_ptr,
           in_ptr->payload,
           in_ptr->payload_len);
  }

  QCRIL_LOG_INFO("Sending payload of length 0x%x with total chunks 0x%x", in_ptr->payload_len, response_to_transaction_data.num_chunks);

  /* Filling the qmi_http_req_ptr */
  memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));
  qmi_http_req_ptr->token_id = response_to_transaction_data.token_id;
  qmi_http_req_ptr->result   = response_to_transaction_data.result;

  if(in_ptr->payload_len > 0)
  {
    qmi_http_req_ptr->segment_info_valid = 0x01;
    qmi_http_req_ptr->payload_body_valid = 0x01;
    qmi_http_req_ptr->segment_info.total_size = in_ptr->payload_len;
    /* Offset will be ZERO as this is the first chunk. Current_chunk value will be 0 */
    qmi_http_req_ptr->segment_info.segment_offset = (response_to_transaction_data.current_chunk
                                                     * UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01);

    if ((in_ptr->payload_len -
         qmi_http_req_ptr->segment_info.segment_offset)
         > UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 )
    {
      qmi_http_req_ptr->payload_body_len = UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;
    }
    else
    {
      qmi_http_req_ptr->payload_body_len =
        (in_ptr->payload_len - qmi_http_req_ptr->segment_info.segment_offset);
    }
    memcpy(qmi_http_req_ptr->payload_body,
           in_ptr->payload + qmi_http_req_ptr->segment_info.segment_offset,
           qmi_http_req_ptr->payload_body_len);

  }

  custom_hdr_ptr = in_ptr->customHeaders;
  if(custom_hdr_ptr != NULL)
  {
    for(header_index = 0; header_index < in_ptr->no_of_headers; header_index++)
    {
      qmi_http_req_ptr->headers_valid = TRUE;
      qmi_http_req_ptr->headers_len++;

      (void)strlcpy((qmi_http_req_ptr->headers[header_index].name),
                     custom_hdr_ptr[header_index].headerName,
                     UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1);

      (void)strlcpy((qmi_http_req_ptr->headers[header_index].value),
                     custom_hdr_ptr[header_index].headerValue,
                     UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1);
    }
  }
    /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(qmi_ril_get_process_instance_id(),
                                                 QCRIL_MAX_MODEM_ID - 1,
                                                 token_ptr,
                                                 0,
                                                 0);

  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    goto report_error_to_modem;
  }
  /* Allocate response pointer since it is an async command */
  qmi_http_rsp_ptr = (uim_http_transaction_resp_msg_v01 *)
                      qcril_malloc(sizeof(uim_http_transaction_resp_msg_v01));
  if (qmi_http_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_rsp_ptr");
    goto report_error_to_modem;
  }

  memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_transaction_resp_msg_v01));
  /* Proceed with Transaction request */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_http_service", "http_transaction_req" );

  qmi_err_code = qmi_client_send_msg_async(
                 qmi_http_handle,
                 QMI_UIM_HTTP_TRANSACTION_REQ_V01,
                 (void *) qmi_http_req_ptr,
                 sizeof(uim_http_transaction_req_msg_v01),
                 (void *) qmi_http_rsp_ptr,
                 sizeof(uim_http_transaction_resp_msg_v01),
                 qcril_uim_http_transaction_completed_callback,
                 orig_req_ptr,
                 &txn_handle);

  /* On successful API call, free only the request */
  if (qmi_err_code != 0)
  {
    goto report_error;
  }

  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  return;

report_error_to_modem:
  qcril_uim_http_send_error_response_to_current_transaction();

report_error:
  /* In any case of error, check & free all the allocated pointers */
  QCRIL_LOG_ERROR("Error in qcril_uim_http_client_transaction_request, qmi_err_code: 0x%x", qmi_err_code);
  qcril_uim_lpa_http_txn_completed_response(token_ptr, QCRIL_UIM_HTTP_RESULT_FAILURE);
   /* Clean up any original request if allocated */
  QCRIL_QMI_UIM_HTTP_FREE_PTR(orig_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(response_to_transaction_data.http_request_payload_ptr);
} /* qcril_uim_http_client_transaction_request */


/*===========================================================================

  FUNCTION:  qcril_uim_http_client_int

===========================================================================*/
void qcril_uim_http_client_int
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qmi_client_error_type           qmi_err_code       = 0;
  qmi_idl_service_object_type     client_service_obj = NULL;
  uint32_t                        num_services       = 0;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  (void)params_ptr;
  (void)ret_ptr;

  /* Get QMI UIM HTTP service object */
  client_service_obj = uim_http_get_service_object_v01();

  if (client_service_obj == NULL)
  {
    QCRIL_LOG_ERROR(" NULL client_service_obj ");
    return;
  }

    /* Check Service is present in the service up list */
  qmi_err_code = qmi_client_get_service_list(client_service_obj,
                                             NULL,
                                             NULL,
                                             &num_services);
  QCRIL_LOG_INFO("Service up, present in service list: %d", qmi_err_code);
  if (qmi_err_code != QMI_NO_ERR)
  {
    return;
  }

  /* Release qmi notifier handle */
  if (qmi_http_notifier != NULL)
  {
    QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_http_service notifier", "release" );
    qmi_err_code = qmi_client_release(qmi_http_notifier);
    qmi_http_notifier = NULL;
  }

  memset(&qmi_http_client_init_os_params, 0x00, sizeof(qmi_client_os_params));

  /* Call common client layer initialization function */
  qmi_err_code = qmi_client_init_instance(client_service_obj,
                                          QMI_CLIENT_INSTANCE_ANY,
                                          (qmi_client_ind_cb)qcril_uim_http_ind_callback,
                                          NULL,
                                          &qmi_http_client_init_os_params,
                                          QCRIL_UIM_HTTP_QMI_SVC_TIMEOUT,
                                          &qmi_http_handle);
  if (qmi_http_handle == NULL ||
      qmi_err_code != QMI_NO_ERR)
  {
    QCRIL_LOG_INFO("QMI HTTP client init failed: %d\n", qmi_err_code);
  }
  else
  {
#ifndef QMI_RIL_UTF
    uim_http_reset_req_msg_v01      qmi_http_req;
    uim_http_reset_resp_msg_v01   * qmi_http_rsp_ptr = NULL;
    qmi_txn_handle                  txn_handle;

    /* UTF is not supported for HTTP, don't send event during initalization which will fail
       bootup test cases */
    QCRIL_LOG_INFO("%s","QMI UIM HTTP Client init complete, send reset request to MODEM");

    qmi_http_rsp_ptr = (uim_http_reset_resp_msg_v01 *) qcril_malloc(sizeof(uim_http_reset_resp_msg_v01));

    if (qmi_http_rsp_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for rsp pointers");
      return;
    }

    memset(&qmi_http_req, 0x00, sizeof(uim_http_reset_req_msg_v01));
    memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_reset_resp_msg_v01));

    qmi_err_code = qmi_client_send_msg_async(
                   qmi_http_handle,
                   QMI_UIM_HTTP_RESET_REQ_V01,
                   (void *) &qmi_http_req,
                   sizeof(uim_http_reset_req_msg_v01),
                   (void *) qmi_http_rsp_ptr,
                   sizeof(uim_http_reset_resp_msg_v01),
                   qcril_uim_http_generic_response_callback,
                   NULL,
                   &txn_handle);

    if (qmi_err_code != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("%s", "error in sending http reset req");
      QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
    }
#endif /* QMI_RIL_UTF */
  }
} /* qcril_uim_http_client_int */


/*===========================================================================

  FUNCTION:  qcril_uim_http_service_up_event

===========================================================================*/
void qcril_uim_http_service_up_event
(
  qmi_client_type               user_handle,
  qmi_idl_service_object_type   svc_obj,
  qmi_client_notify_event_type  service_event,
  void                         *cb_data
)
{
  (void)user_handle;
  (void)svc_obj;
  (void)cb_data;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      /* Post event to qcril event thread to do the initalization */
      QCRIL_LOG_INFO( "%s qcril_event_queue ", __FUNCTION__);
      (void) qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                QCRIL_DEFAULT_MODEM_ID,
                                QCRIL_DATA_NOT_ON_STACK,
                                QCRIL_EVT_UIM_HTTP_QMI_HTTP_SRVC_UP,
                                NULL,
                                0,
                                NULL);
      break;
    default:
      QCRIL_LOG_INFO("Service event not handled :%d\n", service_event);
      break;
  }
} /* qcril_uim_http_service_up_event */


/*===========================================================================

  FUNCTION:  qcril_uim_http_init

===========================================================================*/
void qcril_uim_http_init
(
  void
)
{
  qmi_client_error_type           qmi_err_code       = QMI_NO_ERR;
  qmi_idl_service_object_type     client_service_obj = NULL;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (QCRIL_DEFAULT_INSTANCE_ID != qmi_ril_get_process_instance_id() ||
      qmi_http_notifier != NULL)
  {
    return;
  }

  qcril_uim_lpa_register_service(qmi_ril_get_process_instance_id());

  memset(&qmi_http_notify_os_params, 0x00, sizeof(qmi_client_os_params));

  /* Get QMI UIM HTTP service object */
  client_service_obj = uim_http_get_service_object_v01();

  if (client_service_obj == NULL)
  {
    return;
  }

  qmi_err_code = qmi_client_notifier_init(client_service_obj,
                                          &qmi_http_notify_os_params,
                                          &qmi_http_notifier);
  QCRIL_LOG_INFO("qmi_client_notifier_init return %d", (int) qmi_err_code);

  if (qmi_err_code == QMI_NO_ERR)
  {
    qmi_err_code = qmi_client_register_notify_cb(qmi_http_notifier,
                                                 qcril_uim_http_service_up_event,
                                                 NULL);
    QCRIL_LOG_INFO("qmi_client_register_notify_cb %d", (int) qmi_err_code);
  }
} /* qcril_uim_http_init */


/*===========================================================================

  FUNCTION:  qcril_uim_http_release

===========================================================================*/
void qcril_uim_http_release
(
  void
)
{
  int qmi_err_code = -1;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Deinitialize QMI notifier */
  if (qmi_http_notifier != NULL)
  {
    QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_http_service notifier", "release" );
    qmi_err_code = qmi_client_release(qmi_http_notifier);
    qmi_http_notifier = NULL;
  }

  /* Deinitialize QMI interface */
  if (qmi_http_handle != NULL)
  {
    QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_http_service", "release" );
    qmi_err_code = qmi_client_release(qmi_http_handle);
    qmi_http_handle = NULL;
  }

  /* Free the payload ptr if not done already */
  QCRIL_QMI_UIM_HTTP_FREE_PTR(http_trasaction_payload_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(response_to_transaction_data.http_request_payload_ptr);
  memset(&response_to_transaction_data, 0, sizeof(qcril_uim_http_response_to_transaction_type));
} /* qcril_uim_http_release */

