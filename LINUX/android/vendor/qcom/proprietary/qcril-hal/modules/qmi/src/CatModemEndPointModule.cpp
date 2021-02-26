/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>

#include "common_v01.h"
#include "card_application_toolkit_v02.h"
#include "framework/legacy.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceDownIndMessage.h"
#include "modules/qmi/QmiCatSetupRequest.h"
#include "modules/qmi/QmiCatSetupRequestSync.h"
#include "modules/uim/GstkQmiCatRequestMsg.h"
#include "modules/uim/GstkQmiCatRequestSyncMsg.h"
#include "modules/uim/GstkQmiCatIndicationMsg.h"
#include "CatModemEndPointModule.h"

#ifdef QMI_RIL_UTF
extern "C"{
#include "ril_utf_qmi_sim.h"
}
#endif

#ifndef TAG
#define TAG "CatModemEndPointModule"
#endif /* TAG */

/* Synchronous message default timeout (in milli-seconds) */
#define QMI_CAT_DEFAULT_TIMEOUT                     5000

#define QCRIL_GSTK_QMI_FREE_PTR(ptr)                                        \
  if (ptr != nullptr)                                                       \
  {                                                                         \
    delete (uint8_t *)ptr;                                                  \
    ptr = nullptr;                                                          \
  }                                                                         \

typedef struct
{
  std::shared_ptr<GstkQmiCatRequestMsg>   reqPtr;
  CatModemEndPointModule                * me;
} GstkUserData;


/*---------------------------------------------------------------------------
  Internal functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qmi_cat_notify_cb - QMI Notify callback function
===========================================================================*/
static void qmi_cat_notify_cb
(
  qmi_client_type user_handle,
  qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event,
  void *notify_cb_data
)
{
  /* Wrapper for member method*/
  CatModemEndPointModule *me = (CatModemEndPointModule *)notify_cb_data;
  me->onQmiNotifyCb(user_handle, service_obj, service_event);
} /* qmi_cat_notify_cb */


/*===========================================================================
  FUNCTION  qmi_cat_copy_indication - Copy the qmi indication
===========================================================================*/
static uint8_t * qmi_cat_copy_indication
(
  qmi_client_type                      user_handle_ptr,
  unsigned long                        msg_id,
  const unsigned char                * ind_data_ptr,
  int                                  ind_data_len,
  uint32_t                           * ind_len,
  qmi_cat_indication_id_type         * ind_id
)
{
  uint8_t                 * decoded_payload_ptr  = nullptr;
  uint32_t                  decoded_payload_len  = 0;
  qmi_client_error_type     qmi_err              = QMI_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if ((user_handle_ptr == nullptr) ||
      (ind_data_ptr    == nullptr) ||
      (ind_len         == nullptr) ||
      (ind_id          == nullptr))
  {
    QCRIL_LOG_ERROR( "%s\n", "Invalid input, cannot proceed");
    return NULL;
  }

  /* First decode the message payload from QCCI */
  qmi_idl_get_message_c_struct_len(cat_get_service_object_v02(),
                                   QMI_IDL_INDICATION,
                                   msg_id,
                                   &decoded_payload_len);
  if (decoded_payload_len == 0)
  {
    QCRIL_LOG_ERROR("Failed to find decoded_payload_len");
    return nullptr;
  }

  switch(msg_id)
  {
    case QMI_CAT_SEND_TR_IND_V02:
      *ind_id = QMI_CAT_SEND_TR_IND;
      break;
    case QMI_CAT_SEND_EVENLOPE_CMD_IND_V02:
      *ind_id = QMI_CAT_SEND_EVENLOPE_CMD_IND;
      break;
    case QMI_CAT_EVENT_REPORT_IND_V02:
      *ind_id = QMI_CAT_EVENT_REPORT_IND;
      break;
    case QMI_CAT_SCWS_OPEN_CHANNEL_IND_V02:
      *ind_id = QMI_CAT_SCWS_OPEN_CHANNEL_IND;
      break;
    case QMI_CAT_SCWS_CLOSE_CHANNEL_IND_V02:
      *ind_id = QMI_CAT_SCWS_CLOSE_CHANNEL_IND;
      break;
    case QMI_CAT_SCWS_SEND_DATA_IND_V02:
      *ind_id = QMI_CAT_SCWS_SEND_DATA_IND;
      break;
    default:
      return nullptr;
  }

  /* Allocate decoded payload buffer */
  decoded_payload_ptr = new uint8_t[decoded_payload_len]{};
  if (decoded_payload_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Failed to allocate payload ptr, payload len: 0x%x\n",
                    decoded_payload_len);
    return nullptr;
  }

  /* Decode the Indication payload */
  qmi_err = qmi_client_message_decode(user_handle_ptr,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_data_ptr,
                                      ind_data_len,
                                      decoded_payload_ptr,
                                      decoded_payload_len);
  if (qmi_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Failed to decode Indication: %ld, qmi_err: 0x%x",
                    msg_id, qmi_err);
    delete[] decoded_payload_ptr;
    return nullptr;
  }

  *ind_len = decoded_payload_len;

  return decoded_payload_ptr;
} /* qmi_cat_copy_indication */



/*===========================================================================
  FUNCTION  qmi_cat_client_indication_cb - QMI Notify callback function
===========================================================================*/
static void qmi_cat_client_indication_cb
(
  qmi_client_type                 user_handle,
  unsigned int                    msg_id,
  void                          * ind_buf_ptr,
  unsigned int                    ind_buf_len,
  void                          * ind_cb_data_ptr
)
{
  qmi_cat_indication_id_type     ind_id         = QMI_CAT_EVENT_REPORT_IND;
  uint32_t                       ind_len        = 0;
  uint8_t                      * ind_params_ptr = 0;

  QCRIL_LOG_INFO("qmi_cat_client_indication_cb, msg_id: 0x%x", msg_id);

  if ((ind_buf_ptr     == nullptr) ||
      (ind_buf_len     == 0) ||
      (ind_cb_data_ptr == nullptr))
  {
    QCRIL_LOG_ERROR("%s","NULL ind_buf_ptr !");
    return;
  }

  /* Process valid IND messages */
  switch (msg_id)
  {
    case QMI_CAT_SEND_TR_IND_V02:
    case QMI_CAT_SEND_EVENLOPE_CMD_IND_V02:
    case QMI_CAT_EVENT_REPORT_IND_V02:
    case QMI_CAT_SCWS_OPEN_CHANNEL_IND_V02:
    case QMI_CAT_SCWS_CLOSE_CHANNEL_IND_V02:
    case QMI_CAT_SCWS_SEND_DATA_IND_V02:
      ind_params_ptr = qmi_cat_copy_indication(user_handle,
                                               msg_id,
                                               (unsigned char *)ind_buf_ptr,
                                               ind_buf_len,
                                               &ind_len,
                                               &ind_id);
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported QMI CAT indication: 0x%x", msg_id);
      return;
  }

  if (ind_params_ptr != nullptr &&
      ind_len        != 0)
  {
    CatModemEndPointModule *me = (CatModemEndPointModule *)ind_cb_data_ptr;
    me->onQmiUnsolIndCb(ind_id, ind_len, ind_params_ptr);

    /* Free outptr */
    delete[] ind_params_ptr;
    ind_params_ptr = nullptr;
  }
} /* qmi_cat_client_indication_cb */


/*===========================================================================
  FUNCTION  qmi_cat_async_cb - QMI callback function
===========================================================================*/
static void qmi_cat_async_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                         * resp_cb_data,
  qmi_client_error_type          transp_err
)
{
  GstkUserData     * cb_data        = (GstkUserData *)resp_cb_data;

  QCRIL_LOG_INFO("qmi_cat_async_cb, msg_id: 0x%x", msg_id);

  (void)user_handle;
  (void)resp_c_struct_len;

  /* Sanity check */
  if (cb_data == nullptr || cb_data->me == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL resp_c_struct");
    return;
  }

  cb_data->me->onQmiAsyncCb(cb_data->reqPtr, msg_id, resp_c_struct, transp_err);

  delete cb_data;
} /* qmi_cat_async_cb */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_send_set_event_report - set event report
===========================================================================*/
static int qcril_qmi_cat_send_set_event_report
(
  qmi_client_type                                  mQmiSvcClient,
  const qcril_gstk_request_set_evt_rpt_type      * evt_rpt_ptr
)
{
  cat_set_event_report_req_msg_v02   event_report_req{};
  cat_set_event_report_resp_msg_v02  event_resport_resp{};

  if (evt_rpt_ptr == nullptr)
  {
    return QMI_INTERNAL_ERR;
  }

  /* First update the slot mask */
  event_report_req.slot_mask_valid = TRUE;

  switch (evt_rpt_ptr->slot)
  {
    case 0:
      event_report_req.slot_mask = CAT_SET_EVENT_REPORT_SLOT_1_V02;
      break;
    case 1:
      event_report_req.slot_mask = CAT_SET_EVENT_REPORT_SLOT_2_V02;
      break;
    case 2:
      event_report_req.slot_mask = CAT_SET_EVENT_REPORT_SLOT_3_V02;
      break;
    default:
      QCRIL_LOG_ERROR("Invalid slot id: 0x%x \n", evt_rpt_ptr->slot);
      break;
  }

  /* Updated the necessary masks */
  if (evt_rpt_ptr->dec_evt_mask != 0)
  {
    /* For SCWS updated only SCWS decoded event report mask */
    event_report_req.pc_dec_evt_report_req_mask_valid = TRUE;
    event_report_req.pc_dec_evt_report_req_mask = evt_rpt_ptr->dec_evt_mask;
  }

  if (evt_rpt_ptr->evt_mask != 0)
  {
    event_report_req.pc_evt_report_req_mask_valid = TRUE;
    event_report_req.pc_evt_report_req_mask = evt_rpt_ptr->evt_mask;
  }

  QCRIL_LOG_INFO("dec_evt_mask: 0x%x raw mask: 0x%x, slot_mask : 0x%x",
                  event_report_req.pc_dec_evt_report_req_mask,
                  event_report_req.pc_evt_report_req_mask,
                  event_report_req.slot_mask);

  return qmi_client_send_msg_sync(mQmiSvcClient,
                                  QMI_CAT_SET_EVENT_REPORT_REQ_V02,
                                  (void *) &event_report_req,
                                  sizeof(cat_set_event_report_req_msg_v02),
                                  (void *) &event_resport_resp,
                                  sizeof(cat_set_event_report_resp_msg_v02),
                                  QMI_CAT_DEFAULT_TIMEOUT);
} /* qcril_qmi_cat_send_set_event_report */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_scws_open_channel - send open channel to qmi cat
===========================================================================*/
static int qcril_qmi_cat_scws_open_channel
(
  qmi_client_type                            mQmiSvcClient,
  const qmi_cat_scws_channel_info_msg      * param_data_ptr
)
{
  int                                   qmi_err_code = QMI_INTERNAL_ERR;
  cat_scws_open_channel_req_msg_v02     open_channel_req{};
  cat_scws_open_channel_resp_msg_v02    open_channel_resp{};

  /* Sanity check */
  if (param_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: nullptr param_data_ptr");
    return qmi_err_code;
  }

  /* Init open channel params */
  open_channel_req.channel_status.ch_id  = param_data_ptr->ch_id;
  open_channel_req.channel_status.state  = (cat_scws_channel_state_enum_v02)param_data_ptr->channel_status;
  open_channel_req.slot.slot             = (cat_slot_enum_v02)param_data_ptr->slot;
  open_channel_req.slot_valid            = TRUE;

  /* Send the result to the card */
  QCRIL_LOG_INFO( "%s, <to Card> for channel_id: 0x%X, channel_state: 0x%X \n",
                  __FUNCTION__, open_channel_req.channel_status.ch_id,
                  open_channel_req.channel_status.state);

  qmi_err_code = qmi_client_send_msg_sync(mQmiSvcClient,
                                          QMI_CAT_SCWS_OPEN_CHANNEL_REQ_V02,
                                          (void *) &open_channel_req,
                                          sizeof(cat_scws_open_channel_req_msg_v02),
                                          (void *) &open_channel_resp,
                                          sizeof(cat_scws_open_channel_resp_msg_v02),
                                          QMI_CAT_DEFAULT_TIMEOUT);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("Error for OPEN_CHANNEL_REQ, client_err: 0x%x, error_code: 0x%x\n",
                     qmi_err_code, open_channel_resp.resp.error);
  }

  return qmi_err_code;
} /* qcril_qmi_cat_scws_open_channel */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_scws_close_channel - send close channel to qmi cat
===========================================================================*/
static int qcril_qmi_cat_scws_close_channel
(
  qmi_client_type                            mQmiSvcClient,
  const qmi_cat_scws_channel_info_msg      * param_data_ptr
)
{
  int                                   qmi_err_code = QMI_INTERNAL_ERR;
  cat_scws_close_channel_req_msg_v02    close_channel_req{};
  cat_scws_close_channel_resp_msg_v02   close_channel_resp{};

  /* Sanity check */
  if (param_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: nullptr param_data_ptr");
    return qmi_err_code;
  }

  /* Init close channel params */
  close_channel_req.channel_status.ch_id = param_data_ptr->ch_id;
  close_channel_req.channel_status.state = (cat_scws_channel_state_enum_v02)param_data_ptr->channel_status;
  close_channel_req.slot.slot            = (cat_slot_enum_v02)param_data_ptr->slot;
  close_channel_req.slot_valid           = TRUE;

  /* Send the result to the card */
  QCRIL_LOG_INFO( "%s, <to card> for channel_id: 0x%X, channel_state: 0x%X \n",
                  __FUNCTION__, close_channel_req.channel_status.ch_id,
                  close_channel_req.channel_status.state);

  qmi_err_code = qmi_client_send_msg_sync(mQmiSvcClient,
                                          QMI_CAT_SCWS_CLOSE_CHANNEL_REQ_V02,
                                          (void *) &close_channel_req,
                                          sizeof(cat_scws_close_channel_req_msg_v02),
                                          (void *) &close_channel_resp,
                                          sizeof(cat_scws_close_channel_resp_msg_v02),
                                          QMI_CAT_DEFAULT_TIMEOUT);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("Error for CLOSE_CHANNEL_REQ, client_err: 0x%x, error_code: 0x%x\n",
                     qmi_err_code, close_channel_resp.resp.error);
  }
  return qmi_err_code;
} /* qcril_qmi_cat_scws_close_channel */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_scws_send_data - send data to qmi cat
===========================================================================*/
static int qcril_qmi_cat_scws_send_data
(
  qmi_client_type                            mQmiSvcClient,
  const qmi_cat_scws_send_data_req_msg     * param_data_ptr
)
{
  int                                   qmi_err_code = QMI_INTERNAL_ERR;
  cat_scws_send_data_req_msg_v02        send_data_req{};
  cat_scws_send_data_resp_msg_v02       send_data_resp{};

  /* Sanity check */
  if (param_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: nullptr param_data_ptr");
    return qmi_err_code;
  }

  /* Init send data params */
  send_data_req.result.ch_id  = param_data_ptr->ch_id;
  send_data_req.result.result = param_data_ptr->result;
  send_data_req.slot_valid    = TRUE;
  send_data_req.slot.slot     = (cat_slot_enum_v02)param_data_ptr->slot;

  /* Send the packet to the Agent */
  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X \n", __FUNCTION__,
                  param_data_ptr->ch_id );

  qmi_err_code = qmi_client_send_msg_sync(mQmiSvcClient,
                                          QMI_CAT_SCWS_SEND_DATA_REQ_V02,
                                          (void *) &send_data_req,
                                          sizeof(cat_scws_send_data_req_msg_v02),
                                          (void *) &send_data_resp,
                                          sizeof(cat_scws_send_data_resp_msg_v02),
                                          QMI_CAT_DEFAULT_TIMEOUT);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("Error for SEND_DATA_REQ, client_err: 0x%x, error_code: 0x%x\n",
                     qmi_err_code, send_data_resp.resp.error);
  }

  return qmi_err_code;
} /* qcril_qmi_cat_scws_send_data */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_scws_data_available - send data available
===========================================================================*/
static int qcril_qmi_cat_scws_data_available
(
  qmi_client_type                              mQmiSvcClient,
  const qmi_cat_scws_data_available_type *     data_avail_ptr,
  qmi_client_recv_msg_async_cb                 callback_function_ptr,
  void                                       * user_data
)
{
  int                                     qmi_err_code            = QMI_INTERNAL_ERR;
  cat_scws_data_available_req_msg_v02   * data_available_req_ptr  = nullptr;
  cat_scws_data_available_resp_msg_v02  * data_available_resp_ptr = nullptr;
  qmi_txn_handle                          txn_handle;

  /* Sanity check */
  if (data_avail_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: nullptr data_avail_ptr");
    return qmi_err_code;
  }

  /* Allocate request pointer on the heap */
  data_available_req_ptr = new cat_scws_data_available_req_msg_v02 ();
  if (data_available_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for data_available_req_ptr!");
    return qmi_err_code;
  }

  /* Init data available params */
  data_available_req_ptr->result.data_len = data_avail_ptr->data_len;
  if (data_available_req_ptr->result.data_len > QMI_CAT_SCWS_DATA_MAX_LENGTH_V02)
  {
    QCRIL_LOG_ERROR("Data length exceeds limt, data_len: 0x%x!",
                    data_available_req_ptr->result.data_len);
    /* Free allocated request pointer */
    QCRIL_GSTK_QMI_FREE_PTR(data_available_req_ptr);
    return qmi_err_code;
  }

  data_available_req_ptr->remaining_data_len = data_avail_ptr->remaining_data_len;
  data_available_req_ptr->result.ch_id       = data_avail_ptr->ch_id;
  data_available_req_ptr->slot_valid         = TRUE;
  data_available_req_ptr->slot.slot          = (cat_slot_enum_v02)data_avail_ptr->slot;

  memcpy(data_available_req_ptr->result.data,
         data_avail_ptr->data,
         data_available_req_ptr->result.data_len);

  /* Allocate response pointer since the command is an async one */
  data_available_resp_ptr = new cat_scws_data_available_resp_msg_v02{};
  if (data_available_resp_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for data_available_resp_ptr!");
    QCRIL_GSTK_QMI_FREE_PTR(data_available_req_ptr);
    return qmi_err_code;
  }

  qmi_err_code = qmi_client_send_msg_async(
                   mQmiSvcClient,
                   QMI_CAT_SCWS_DATA_AVAILABLE_REQ_V02,
                   (void *) data_available_req_ptr,
                   sizeof(cat_scws_data_available_req_msg_v02),
                   (void *) data_available_resp_ptr,
                   sizeof(cat_scws_data_available_resp_msg_v02),
                   callback_function_ptr,
                   user_data,
                   &txn_handle);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("Error for DATA_AVAILABLE_REQ, client_err: 0x%x, error_code: 0x%x\n",
                    qmi_err_code, data_available_resp_ptr->resp.error);
    QCRIL_GSTK_QMI_FREE_PTR(data_available_resp_ptr);
  }
  /* Free allocated request pointer */
  QCRIL_GSTK_QMI_FREE_PTR(data_available_req_ptr);
  return qmi_err_code;
} /* qcril_qmi_cat_scws_data_available */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_scws_channel_status - send channel status
===========================================================================*/
static int qcril_qmi_cat_scws_channel_status
(
  qmi_client_type                            mQmiSvcClient,
  const qmi_cat_scws_channel_info_msg      * param_data_ptr
)
{
  int                                   qmi_err_code = QMI_INTERNAL_ERR;
  cat_scws_channel_status_req_msg_v02   channel_status_req{};
  cat_scws_channel_status_resp_msg_v02  channel_status_resp{};

  if (param_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: nullptr param_data_ptr");
    return qmi_err_code;
  }

  /* Init channel status params */
  channel_status_req.slot_valid           = TRUE;
  channel_status_req.slot.slot            = (cat_slot_enum_v02)param_data_ptr->slot;
  channel_status_req.channel_status.ch_id = param_data_ptr->ch_id;
  channel_status_req.channel_status.state = (cat_scws_channel_state_enum_v02)param_data_ptr->channel_status;

  qmi_err_code = qmi_client_send_msg_sync(
                   mQmiSvcClient,
                   QMI_CAT_SCWS_CHANNEL_STATUS_REQ_V02,
                   (void *) &channel_status_req,
                   sizeof(cat_scws_channel_status_req_msg_v02),
                   (void *) &channel_status_resp,
                   sizeof(cat_scws_channel_status_resp_msg_v02),
                   QMI_CAT_DEFAULT_TIMEOUT);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("Error for SCWS_CHANNEL_STATUS_REQ, client_err: 0x%x, error_code: 0x%x\n",
                    qmi_err_code, channel_status_resp.resp.error);
  }
  return qmi_err_code;
} /* qcril_qmi_cat_scws_channel_status */


/*===========================================================================
  FUNCTION  qcril_qmi_cat_send_envelope_cmd - send envelope cmd to qmi cat
===========================================================================*/
static int qcril_qmi_cat_send_envelope_cmd
(
  qmi_client_type                              mQmiSvcClient,
  const qcril_gstk_request_envelope_cmd_type * env_data_ptr,
  qmi_client_recv_msg_async_cb                 callback_function_ptr,
  void                                       * user_data
)
{
  int                                  qmi_err_code      = QMI_INTERNAL_ERR;
  cat_send_envelope_cmd_req_msg_v02  * env_cmd_req_ptr   = nullptr;
  cat_send_envelope_cmd_resp_msg_v02 * env_cmd_resp_ptr  = nullptr;
  qmi_txn_handle                       txn_handle;

  if (env_data_ptr == nullptr ||
      env_data_ptr->envelope_data_len > QMI_CAT_ENVELOPE_DATA_MAX_LENGTH)
  {
    return QMI_INTERNAL_ERR;
  }

  /* Allocate request pointer on the heap */
  env_cmd_req_ptr = new cat_send_envelope_cmd_req_msg_v02{};
  if (env_cmd_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for env_cmd_req_ptr!");
    return QMI_INTERNAL_ERR;
  }

  env_cmd_req_ptr->envelope_cmd.env_cmd_type  = CAT_ENVELOPE_CMD_TYPE_UNKNOWN_V02;

  env_cmd_req_ptr->slot_valid = TRUE;
  env_cmd_req_ptr->slot.slot  = (cat_slot_enum_v02)env_data_ptr->slot_id;

  env_cmd_req_ptr->envelope_cmd.envelope_data_len = env_data_ptr->envelope_data_len;
  memcpy(env_cmd_req_ptr->envelope_cmd.envelope_data,
         env_data_ptr->envelope_data,
         QMI_CAT_ENVELOPE_DATA_MAX_LENGTH);

  env_cmd_req_ptr->indication_token_valid = TRUE;
  env_cmd_req_ptr->indication_token       = env_data_ptr->token;

  /* Allocate envelope command response */
  env_cmd_resp_ptr = new cat_send_envelope_cmd_resp_msg_v02{};
  if (env_cmd_resp_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for envelope command response!");
    QCRIL_GSTK_QMI_FREE_PTR(env_cmd_req_ptr);
    return QMI_INTERNAL_ERR;
  }

  qmi_err_code = qmi_client_send_msg_async(mQmiSvcClient,
                                           QMI_CAT_SEND_ENVELOPE_CMD_REQ_V02,
                                           (void *) env_cmd_req_ptr,
                                           sizeof(cat_send_envelope_cmd_req_msg_v02),
                                           (void *) env_cmd_resp_ptr,
                                           sizeof(cat_send_envelope_cmd_resp_msg_v02),
                                           callback_function_ptr,
                                           user_data,
                                           &txn_handle);
  if (qmi_err_code == 0)
  {
    /* Free allocated request pointer */
    QCRIL_GSTK_QMI_FREE_PTR(env_cmd_req_ptr);
    return qmi_err_code;
  }

  /* Clean up command response pointer */
  QCRIL_GSTK_QMI_FREE_PTR(env_cmd_resp_ptr);

  /* Free allocated request pointer */
  QCRIL_GSTK_QMI_FREE_PTR(env_cmd_req_ptr);

  return qmi_err_code;
} /* qcril_qmi_cat_send_envelope_cmd */


/*==============================================================================
  FUNCTION  qcril_qmi_cat_send_terminal_rsp - send terminal response to qmi cat
===============================================================================*/
static int qcril_qmi_cat_send_terminal_rsp
(
  qmi_client_type                         mQmiSvcClient,
  const qmi_cat_terminal_response_type  * tr_data_ptr,
  qmi_client_recv_msg_async_cb            callback_function_ptr,
  void                                  * user_data
)
{
  int                                  qmi_err_code      = QMI_INTERNAL_ERR;
  cat_send_tr_req_msg_v02            * send_tr_req_ptr   = nullptr;
  cat_send_tr_resp_msg_v02           * send_tr_resp_ptr  = nullptr;
  qmi_txn_handle                       txn_handle;

  if (tr_data_ptr == nullptr ||
      tr_data_ptr->terminal_response_len > QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH)
  {
    return QMI_INTERNAL_ERR;
  }

  /* Allocate request pointer on the heap */
  send_tr_req_ptr = new cat_send_tr_req_msg_v02{};
  if (send_tr_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for send_tr_req_ptr!");
    return QMI_INTERNAL_ERR;
  }

  send_tr_req_ptr->terminal_response.uim_ref_id = tr_data_ptr->uim_ref_id;

  send_tr_req_ptr->indication_token_valid = TRUE;
  send_tr_req_ptr->indication_token       = tr_data_ptr->token;

  send_tr_req_ptr->slot_valid = TRUE;
  send_tr_req_ptr->slot.slot  = (cat_slot_enum_v02)tr_data_ptr->slot_id;

  send_tr_req_ptr->terminal_response.terminal_response_len = tr_data_ptr->terminal_response_len;

  memcpy(send_tr_req_ptr->terminal_response.terminal_response,
         tr_data_ptr->terminal_response,
         QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH);

  /* Allocate terminal response data */
  send_tr_resp_ptr = new cat_send_tr_resp_msg_v02{};
  if (send_tr_resp_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for terminal response!");
    QCRIL_GSTK_QMI_FREE_PTR(send_tr_req_ptr);
    return QMI_INTERNAL_ERR;
  }

  qmi_err_code = qmi_client_send_msg_async(mQmiSvcClient,
                                           QMI_CAT_SEND_TR_REQ_V02,
                                           (void *) send_tr_req_ptr,
                                           sizeof(cat_send_tr_req_msg_v02),
                                           (void *) send_tr_resp_ptr,
                                           sizeof(cat_send_tr_resp_msg_v02),
                                           callback_function_ptr,
                                           user_data,
                                           &txn_handle);
  if (qmi_err_code == 0)
  {
    /* Free allocated request pointer */
    QCRIL_GSTK_QMI_FREE_PTR(send_tr_req_ptr);
    return qmi_err_code;
  }

  /* Clean up command response pointer */
  QCRIL_GSTK_QMI_FREE_PTR(send_tr_resp_ptr);

  /* Free allocated request pointer */
  QCRIL_GSTK_QMI_FREE_PTR(send_tr_req_ptr);

  return qmi_err_code;
} /* qcril_qmi_cat_send_terminal_rsp */


/*==============================================================================
  FUNCTION  qcril_qmi_cat_send_event_cnf - send event cnf to qmi cat
===============================================================================*/
static int qcril_qmi_cat_send_event_cnf
(
  qmi_client_type                            mQmiSvcClient,
  const qmi_cat_event_confirmation_req_msg * param_data_ptr
)
{
  int                                 qmi_err_code  = QMI_INTERNAL_ERR;
  cat_event_confirmation_req_msg_v02  event_conf_req{};
  cat_event_confirmation_resp_msg_v02 event_conf_resp{};

  QCRIL_LOG_INFO("%s", "qcril_qmi_cat_send_event_cnf\n");

  if (param_data_ptr == nullptr)
  {
    return QMI_INTERNAL_ERR;
  }

  /* Limitation in UI: UI has no knowledge if icon is being displayed
     successfully. Assume icon displayed and send icon conf = TRUE */
  event_conf_req.display_valid   = TRUE;
  event_conf_req.display.display = 0x01;
  if (param_data_ptr->evt_data == QMI_CAT_CONFIRM)
  {
    QCRIL_LOG_INFO("User %s call\n",(param_data_ptr->cnf_data ? "ACCEPTED" : "REJECTED"));
    event_conf_req.confirm_valid   = TRUE;
    event_conf_req.confirm.confirm = (param_data_ptr->cnf_data ? 0x01 : 0x00);
  }
  event_conf_req.slot_valid      = TRUE;
  event_conf_req.slot.slot       = (cat_slot_enum_v02)param_data_ptr->slot;

  qmi_err_code = qmi_client_send_msg_sync(
                   mQmiSvcClient,
                   QMI_CAT_EVENT_CONFIRMATION_REQ_V02,
                   (void *) &event_conf_req,
                   sizeof(cat_event_confirmation_req_msg_v02),
                   (void *) &event_conf_resp,
                   sizeof(cat_event_confirmation_resp_msg_v02),
                   QMI_CAT_DEFAULT_TIMEOUT);

  return qmi_err_code;
} /* qcril_qmi_cat_send_event_cnf */


/*==============================================================================
  FUNCTION  qcril_qmi_cat_perform_config_check - qmi cat config check
===============================================================================*/
static int qcril_qmi_cat_perform_config_check
(
  qmi_client_type                            mQmiSvcClient,
  qmi_cat_sync_rsp_data_type               * rsp_data_ptr
)
{
  qmi_client_error_type                qmi_err_code      = QMI_INTERNAL_ERR;
  cat_config_mode_enum_v02             config_mode       = CAT_CONFIG_MODE_DISABLED_V02;
  cat_get_configuration_resp_msg_v02 * get_conf_resp_ptr = nullptr;
  cat_set_configuration_req_msg_v02  * set_conf_req_ptr  = nullptr;
  cat_set_configuration_resp_msg_v02   set_conf_resp{};

  get_conf_resp_ptr = new cat_get_configuration_resp_msg_v02{};
  if(get_conf_resp_ptr == nullptr)
  {
    return QMI_SERVICE_ERR;
  }

  qmi_err_code = qmi_client_send_msg_sync( mQmiSvcClient,
                                           QMI_CAT_GET_CONFIGURATION_REQ_V02,
                                           nullptr,
                                           0,
                                           (void *) get_conf_resp_ptr,
                                           sizeof(cat_get_configuration_resp_msg_v02),
                                           QMI_CAT_DEFAULT_TIMEOUT);

  if ((qmi_err_code                             != QMI_NO_ERR)             ||
      (get_conf_resp_ptr->resp.result           != QMI_RESULT_SUCCESS_V01) ||
      (get_conf_resp_ptr->resp.error            != QMI_ERR_NONE_V01)       ||
      (get_conf_resp_ptr->cat_config_mode_valid == 0))
  {
    /* Nothing we can do about an error, just return */
    QCRIL_LOG_ERROR("Error for get config, qmi_err_code: 0x%x, result: 0x%x, error: 0x%x",
                    qmi_err_code,
                    get_conf_resp_ptr->resp.result,
                    get_conf_resp_ptr->resp.error);
    QCRIL_GSTK_QMI_FREE_PTR(get_conf_resp_ptr);
    return qmi_err_code;
  }

  config_mode = get_conf_resp_ptr->cat_config_mode;
  QCRIL_GSTK_QMI_FREE_PTR(get_conf_resp_ptr);

  QCRIL_LOG_INFO("cat_config_mode: 0x%x", config_mode);

  if ((config_mode == CAT_CONFIG_MODE_ANDROID_V02) ||
      (config_mode == CAT_CONFIG_MODE_CUSTOM_RAW_V02))
  {
    /* Nothing else to do */
    return qmi_err_code;
  }

  /* Set it to Android mode if we found some other value */
  set_conf_req_ptr = new cat_set_configuration_req_msg_v02{};
  if(set_conf_req_ptr == nullptr)
  {
    return QMI_SERVICE_ERR;
  }

  set_conf_req_ptr->cat_config_mode = CAT_CONFIG_MODE_ANDROID_V02;

  qmi_err_code = qmi_client_send_msg_sync( mQmiSvcClient,
                                           QMI_CAT_SET_CONFIGURATION_REQ_V02,
                                           (void *) set_conf_req_ptr,
                                           sizeof(cat_set_configuration_req_msg_v02),
                                           (void *) &set_conf_resp,
                                           sizeof(cat_set_configuration_resp_msg_v02),
                                           QMI_CAT_DEFAULT_TIMEOUT);
  if ((qmi_err_code              != QMI_NO_ERR)             ||
      (set_conf_resp.resp.result != QMI_RESULT_SUCCESS_V01) ||
      (set_conf_resp.resp.error  != QMI_ERR_NONE_V01))
  {
    /* Nothing we can do about an error, just return */
    QCRIL_LOG_ERROR("Error for set config, qmi_err_code: 0x%x, result: 0x%x, error: 0x%x",
                    qmi_err_code,
                    set_conf_resp.resp.result,
                    set_conf_resp.resp.error);
  }

  QCRIL_GSTK_QMI_FREE_PTR(set_conf_req_ptr);

  if (rsp_data_ptr != nullptr)
  {
    rsp_data_ptr->qmi_err_code = set_conf_resp.resp.error;
  }

  return qmi_err_code;
} /* qcril_qmi_cat_perform_config_check */


/*==============================================================================
  FUNCTION  qcril_qmi_cat_get_cached_cmd - qmi cat get cached command
===============================================================================*/
static int qcril_qmi_cat_get_cached_cmd
(
  qmi_client_type                            mQmiSvcClient,
  const qcril_gstk_request_cached_cmd_type * param_data_ptr,
  qmi_cat_sync_rsp_data_type               * rsp_data_ptr
)
{
  cat_get_cached_proactive_cmd_req_msg_v02    get_cache_req;
  cat_get_cached_proactive_cmd_resp_msg_v02  *get_cache_resp_ptr = NULL;
  qmi_client_error_type                       qmi_err_code       = QMI_INTERNAL_ERR;

  QCRIL_LOG_INFO("%s", "qcril_qmi_cat_get_cached_cmd\n");

  if (param_data_ptr == nullptr || rsp_data_ptr == nullptr)
  {
    return QMI_INTERNAL_ERR;
  }

  get_cache_resp_ptr = new cat_get_cached_proactive_cmd_resp_msg_v02{};
  if(NULL == get_cache_resp_ptr)
  {
    QCRIL_LOG_ERROR("qcril_malloc fail get_cache_resp_ptr\n");
    return QMI_INTERNAL_ERR;
  }

  get_cache_req.slot_valid = TRUE;
  get_cache_req.slot.slot  = (cat_slot_enum_v02)param_data_ptr->slot;

  QCRIL_LOG_INFO("Requesting qmi_req_cmd_id=0x%x", param_data_ptr->cmd_id);

  get_cache_req.command_id = (cat_cached_command_id_enum_v02)param_data_ptr->cmd_id;

  qmi_err_code = qmi_client_send_msg_sync(
                        mQmiSvcClient,
                        QMI_CAT_GET_CACHED_PROACTIVE_CMD_REQ_V02,
                        (void *)&get_cache_req,
                        sizeof(get_cache_req),
                        (void *)get_cache_resp_ptr,
                        sizeof(*get_cache_resp_ptr),
                        QMI_CAT_DEFAULT_TIMEOUT);

  rsp_data_ptr->qmi_err_code = get_cache_resp_ptr->resp.error;

  if (qmi_err_code                    != QMI_NO_ERR ||
      get_cache_resp_ptr->resp.result != QMI_RESULT_SUCCESS_V01 ||
      get_cache_resp_ptr->resp.error  != QMI_ERR_NONE_V01)
  {
    QCRIL_LOG_ERROR("Error GET_CACHED_PROACTIVE_CMD_REQ, qmi_err_code: 0x%x, result: 0x%x, error: 0x%x\n",
                    qmi_err_code, get_cache_resp_ptr->resp.result, get_cache_resp_ptr->resp.error);
  }
  else
  {
    uint8_t      * pc_cmd_ptr = nullptr;

    switch (get_cache_req.command_id)
    {
      case CAT_CACHED_COMMAND_ID_SETUP_MENU_V02:
        if (get_cache_resp_ptr->setup_menu_valid)
        {
          rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len = get_cache_resp_ptr->setup_menu.pc_setup_menu_len;
          pc_cmd_ptr = get_cache_resp_ptr->setup_menu.pc_setup_menu;
        }
        break;
      case CAT_CACHED_COMMAND_ID_SETUP_EVENT_LIST_V02:
        if (get_cache_resp_ptr->setup_event_list_raw_valid)
        {
          rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len = get_cache_resp_ptr->setup_event_list_raw.pc_setup_event_list_len;
          pc_cmd_ptr = get_cache_resp_ptr->setup_event_list_raw.pc_setup_event_list;
        }
        break;
      case CAT_CACHED_COMMAND_ID_SETUP_IDLE_TEXT_V02:
        if(get_cache_resp_ptr->idle_mode_text_valid)
        {
          rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len = get_cache_resp_ptr->idle_mode_text.pc_setup_idle_mode_text_len;
          pc_cmd_ptr = get_cache_resp_ptr->idle_mode_text.pc_setup_idle_mode_text;
        }
        break;
      default:
        break;
    }

    if (pc_cmd_ptr != nullptr &&
        rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len <= QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH)
    {
      memcpy(rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data,
             pc_cmd_ptr,
             rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len);
    }
    else
    {
      rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len = 0;
      qmi_err_code = QMI_SERVICE_ERR;
    }
  }

  QCRIL_GSTK_QMI_FREE_PTR(get_cache_resp_ptr);
  return qmi_err_code;
} /* qcril_qmi_cat_get_cached_cmd */


/*===========================================================================
  FUNCTION  qmi_cat_process_qcril_gstk_request - Process QCRIL GSTK requests
===========================================================================*/
static int qmi_cat_process_qcril_gstk_request
(
  qmi_client_type                      mQmiSvcClient,
  qcril_gstk_request_type              request_type,
  const qcril_gstk_request_data_type * param_data_ptr,
  qmi_client_recv_msg_async_cb         callback_function_ptr,
  void                               * user_data,
  qmi_cat_sync_rsp_data_type         * rsp_data_ptr
)
{
  int ret = QMI_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s, request: %d", __FUNCTION__, request_type);

  switch(request_type)
  {
    case QCRIL_GSTK_REQUEST_SEND_ENVELOPE_COMMAND:
      ret = qcril_qmi_cat_send_envelope_cmd(mQmiSvcClient,
                                            (const qcril_gstk_request_envelope_cmd_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data);
      break;

    case QCRIL_GSTK_REQUEST_SEND_TERMINAL_RESPONSE:
      ret = qcril_qmi_cat_send_terminal_rsp(mQmiSvcClient,
                                            (const qmi_cat_terminal_response_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data);
      break;

    case QCRIL_GSTK_REQUEST_SEND_EVENT_CONFIRMATION:
      ret = qcril_qmi_cat_send_event_cnf(mQmiSvcClient,
                                         (const qmi_cat_event_confirmation_req_msg*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUEST_SCWS_OPEN_CHANNEL:
      ret = qcril_qmi_cat_scws_open_channel(mQmiSvcClient,
                                            (const qmi_cat_scws_channel_info_msg*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUEST_SCWS_CLOSE_CHANNEL:
      ret = qcril_qmi_cat_scws_close_channel(mQmiSvcClient,
                                             (const qmi_cat_scws_channel_info_msg*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUEST_SCWS_SEND_DATA:
      ret = qcril_qmi_cat_scws_send_data(mQmiSvcClient,
                                         (const qmi_cat_scws_send_data_req_msg*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUEST_SCWS_DATA_AVAILABLE:
      ret = qcril_qmi_cat_scws_data_available(mQmiSvcClient,
                                            (const qmi_cat_scws_data_available_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data);
      break;

    case QCRIL_GSTK_REQUEST_SCWS_CHANNEL_STATUS:
      ret = qcril_qmi_cat_scws_channel_status(mQmiSvcClient,
                                              (const qmi_cat_scws_channel_info_msg*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUSET_SET_EVENT_REPORT:
      ret = qcril_qmi_cat_send_set_event_report(mQmiSvcClient,
                                                (const qcril_gstk_request_set_evt_rpt_type*)param_data_ptr);
      break;

    case QCRIL_GSTK_REQUSET_CHECK_CONFIGURATION:
      ret = qcril_qmi_cat_perform_config_check(mQmiSvcClient,
                                               rsp_data_ptr);
      break;

    case QCRIL_GSTK_REQUEST_GET_CACHED_CMD:
      ret = qcril_qmi_cat_get_cached_cmd(mQmiSvcClient,
                                         (const qcril_gstk_request_cached_cmd_type*)param_data_ptr,
                                         rsp_data_ptr);
      break;

    default:
      ret = QMI_INTERNAL_ERR;
      break;
  }
  return ret;
} /* qmi_cat_process_qcril_gstk_request */


/*===========================================================================
  FUNCTION  qmi_cat_convert_slot_type_to_slot_index
===========================================================================*/
static uint8_t qmi_cat_convert_slot_type_to_slot_index
(
  cat_slot_enum_v02      slot_type
)
{
  uint8 slot_index = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;

  /* Convert instance id to slot index */
  switch (slot_type)
  {
    case CAT_SLOT1_V02:
      slot_index = 0;
      break;
    case CAT_SLOT2_V02:
      slot_index = 1;
      break;
    case CAT_SLOT3_V02:
      slot_index = 2;
      break;
    default:
      QCRIL_LOG_ERROR("Invalid slot type for conversion: 0x%x \n", slot_type);
      break;
  }

  QCRIL_LOG_DEBUG( "Slot index found: 0x%x\n", slot_index );
  return slot_index;
} /* qmi_cat_convert_slot_type_to_slot_index*/


/*===========================================================================
  FUNCTION  qmi_cat_process_event_report_ind
===========================================================================*/
static void qmi_cat_process_event_report_ind
(
  const cat_event_report_ind_msg_v02               * ind_msg_ptr,
  std::shared_ptr<qcril_gstk_qmi_ind_params_type>    ind_ptr
)
{
  const uint8_t * pc_data_ptr   = nullptr;
  uint32_t        unique_ref_id = 0;
  uint32_t        pc_data_len   = 0;
  uint8_t         cmd_type      = 0;

  /* Sanity check */
  if (ind_msg_ptr == nullptr || ind_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_data_ptr cannot process event report IND");
    return;
  }

  if (ind_msg_ptr->slot_valid == TRUE)
  {
    ind_ptr->slot = qmi_cat_convert_slot_type_to_slot_index(ind_msg_ptr->slot.slot);
  }

  /* Since we expect only one of the following TLVs, */
  if (ind_msg_ptr->display_text_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Display Text TLV");
    pc_data_len   = ind_msg_ptr->display_text.pc_display_text_len;
    pc_data_ptr   = ind_msg_ptr->display_text.pc_display_text;
    unique_ref_id = ind_msg_ptr->display_text.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_DISPLAY_TEXT;
  }
  else if (ind_msg_ptr->get_inkey_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Get Inkey TLV");
    pc_data_len   = ind_msg_ptr->get_inkey.pc_get_inkey_len;
    pc_data_ptr   = ind_msg_ptr->get_inkey.pc_get_inkey;
    unique_ref_id = ind_msg_ptr->get_inkey.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_GET_INKEY;
  }
  else if (ind_msg_ptr->get_input_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Get Input TLV");
    pc_data_len   = ind_msg_ptr->get_input.pc_get_input_len;
    pc_data_ptr   = ind_msg_ptr->get_input.pc_get_input;
    unique_ref_id = ind_msg_ptr->get_input.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_GET_INPUT;
  }
  else if (ind_msg_ptr->setup_menu_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Setup Menu TLV");
    pc_data_len   = ind_msg_ptr->setup_menu.pc_setup_menu_len;
    pc_data_ptr   = ind_msg_ptr->setup_menu.pc_setup_menu;
    unique_ref_id = ind_msg_ptr->setup_menu.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU;
  }
  else if (ind_msg_ptr->select_item_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Select Item TLV");
    pc_data_len   = ind_msg_ptr->select_item.pc_select_item_len;
    pc_data_ptr   = ind_msg_ptr->select_item.pc_select_item;
    unique_ref_id = ind_msg_ptr->select_item.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SELECT_ITEM;
  }
  else if (ind_msg_ptr->idle_mode_text_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Setup Idle Mode Text TLV");
    pc_data_len   = ind_msg_ptr->idle_mode_text.pc_setup_idle_mode_text_len;
    pc_data_ptr   = ind_msg_ptr->idle_mode_text.pc_setup_idle_mode_text;
    unique_ref_id = ind_msg_ptr->idle_mode_text.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT;
   }
  else if (ind_msg_ptr->lang_notification_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Language Notification TLV");
    pc_data_len   = ind_msg_ptr->lang_notification.pc_lang_notification_len;
    pc_data_ptr   = ind_msg_ptr->lang_notification.pc_lang_notification;
    unique_ref_id = ind_msg_ptr->lang_notification.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_LANG_NOTIFICATION;
  }
  else if (ind_msg_ptr->play_tone_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Play Tone TLV");
    pc_data_len   = ind_msg_ptr->play_tone.pc_play_tone_len;
    pc_data_ptr   = ind_msg_ptr->play_tone.pc_play_tone;
    unique_ref_id = ind_msg_ptr->play_tone.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_PLAY_TONE;
  }
  else if (ind_msg_ptr->setup_call_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Setup Call TLV");
    pc_data_len   = ind_msg_ptr->setup_call.pc_setup_call_len;
    pc_data_ptr   = ind_msg_ptr->setup_call.pc_setup_call;
    unique_ref_id = ind_msg_ptr->setup_call.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SET_UP_CALL;
  }
  else if (ind_msg_ptr->send_sms_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Send SMS TLV");
    pc_data_len   = ind_msg_ptr->send_sms.pc_send_sms_len;
    pc_data_ptr   = ind_msg_ptr->send_sms.pc_send_sms;
    unique_ref_id = ind_msg_ptr->send_sms.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SEND_SHORT_MESSAGE;
  }
  else if (ind_msg_ptr->send_dtmf_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Send DTMF TLV");
    pc_data_len   = ind_msg_ptr->send_dtmf.pc_send_dtmf_len;
    pc_data_ptr   = ind_msg_ptr->send_dtmf.pc_send_dtmf;
    unique_ref_id = ind_msg_ptr->send_dtmf.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SEND_DTMF;
  }
  else if (ind_msg_ptr->launch_browser_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Launch Browser TLV");
    pc_data_len   = ind_msg_ptr->launch_browser.pc_launch_browser_len;
    pc_data_ptr   = ind_msg_ptr->launch_browser.pc_launch_browser;
    unique_ref_id = ind_msg_ptr->launch_browser.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_LAUNCH_BROWSER;
  }
  else if (ind_msg_ptr->send_ss_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Send SS TLV");
    pc_data_len   = ind_msg_ptr->send_ss.pc_send_ss_len;
    pc_data_ptr   = ind_msg_ptr->send_ss.pc_send_ss;
    unique_ref_id = ind_msg_ptr->send_ss.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SEND_SS;
  }
  else if (ind_msg_ptr->send_ussd_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Send USSD TLV");
    pc_data_len   = ind_msg_ptr->send_ussd.pc_send_ussd_len;
    pc_data_ptr   = ind_msg_ptr->send_ussd.pc_send_ussd;
    unique_ref_id = ind_msg_ptr->send_ussd.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_USSD;
  }
  else if (ind_msg_ptr->provide_local_info_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Provide Local Information TLV");
    pc_data_len   = ind_msg_ptr->provide_local_info.pc_provide_local_info_len;
    pc_data_ptr   = ind_msg_ptr->provide_local_info.pc_provide_local_info;
    unique_ref_id = ind_msg_ptr->provide_local_info.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_PROVIDE_LOCAL_INFO;
  }
  else if (ind_msg_ptr->setup_event_list_raw_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Setup Event List Raw TLV");
    pc_data_len   = ind_msg_ptr->setup_event_list_raw.pc_setup_event_list_len;
    pc_data_ptr   = ind_msg_ptr->setup_event_list_raw.pc_setup_event_list;
    unique_ref_id = ind_msg_ptr->setup_event_list_raw.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST;
  }
  else if (ind_msg_ptr->open_channel_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Open Channel TLV");
    pc_data_len   = ind_msg_ptr->open_channel.pc_open_channel_len;
    pc_data_ptr   = ind_msg_ptr->open_channel.pc_open_channel;
    unique_ref_id = ind_msg_ptr->open_channel.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_OPEN_CHANNEL;
  }
  else if (ind_msg_ptr->close_channel_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Close Channel TLV");
    pc_data_len   = ind_msg_ptr->close_channel.pc_close_channel_len;
    pc_data_ptr   = ind_msg_ptr->close_channel.pc_close_channel;
    unique_ref_id = ind_msg_ptr->close_channel.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_CLOSE_CHANNEL;
  }
  else if (ind_msg_ptr->send_data_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Send Data TLV");
    pc_data_len   = ind_msg_ptr->send_data.pc_send_data_len;
    pc_data_ptr   = ind_msg_ptr->send_data.pc_send_data;
    unique_ref_id = ind_msg_ptr->send_data.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_SEND_DATA;
  }
  else if (ind_msg_ptr->receive_data_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Receive Data TLV");
    pc_data_len   = ind_msg_ptr->receive_data.pc_receive_data_len;
    pc_data_ptr   = ind_msg_ptr->receive_data.pc_receive_data;
    unique_ref_id = ind_msg_ptr->receive_data.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_RECEIVE_DATA;
  }
  else if (ind_msg_ptr->proactive_session_end_type_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: End Proactive Session TLV");
    ind_ptr->ind_data.evt_rpt.is_end_session = TRUE;
    ind_ptr->ind_data.evt_rpt.cmd_type = QCRIL_GSTK_QMI_CMD_STK_END_OF_PROACTIVE_SES;
  }
  else if (ind_msg_ptr->refresh_alpha_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Refresh Alpha TLV");
    pc_data_len   = ind_msg_ptr->refresh_alpha.pc_refresh_alpha_len;
    pc_data_ptr   = ind_msg_ptr->refresh_alpha.pc_refresh_alpha;
    unique_ref_id = ind_msg_ptr->refresh_alpha.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_REFRESH;
  }
  else if (ind_msg_ptr->activate_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Activate TLV");
    pc_data_len   = ind_msg_ptr->activate.pc_activate_len;
    pc_data_ptr   = ind_msg_ptr->activate.pc_activate;
    unique_ref_id = ind_msg_ptr->activate.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_ACTIVATE;
  }
#ifdef FEATURE_QCRIL_GSTK_CONTACTLESS
  else if (ind_msg_ptr->contactless_state_changed_valid)
  {
    QCRIL_LOG_INFO("%s", "Command will be handled by Android: Contactless State Changed TLV");
    pc_data_len   = ind_msg_ptr->contactless_state_changed.pc_contactless_state_changed_len;
    pc_data_ptr   = ind_msg_ptr->contactless_state_changed.pc_contactless_state_changed;
    unique_ref_id = ind_msg_ptr->contactless_state_changed.uim_ref_id;
    cmd_type      = QCRIL_GSTK_QMI_CMD_STK_CONTACTLESS_STATE;
  }
#endif /* FEATURE_QCRIL_GSTK_CONTACTLESS */
  else
  {
    QCRIL_LOG_ERROR( "%s", "Unhandled TLV");
    return;
  }

  if (pc_data_len < QCRIL_GSTK_QMI_COMMAND_MIN_SIZE ||
      pc_data_len > QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH)
  {
    QCRIL_LOG_ERROR("Length out of range: 0x%x, discarding TLV", pc_data_len);
    return;
  }

  if (pc_data_ptr != nullptr)
  {
    memcpy(ind_ptr->ind_data.evt_rpt.cmd_data, pc_data_ptr, pc_data_len);
    ind_ptr->ind_data.evt_rpt.cmd_len       = pc_data_len;
    ind_ptr->ind_data.evt_rpt.unique_ref_id = unique_ref_id;
    ind_ptr->ind_data.evt_rpt.cmd_type      = cmd_type;
  }
} /* qmi_cat_process_event_report_ind */


/*===========================================================================
  FUNCTION  qmi_cat_process_scws_open_channel_ind
===========================================================================*/
static void qmi_cat_process_scws_open_channel_ind
(
  cat_scws_open_channel_ind_msg_v02                * open_channel_ind_ptr,
  std::shared_ptr<qcril_gstk_qmi_ind_params_type>    ind_ptr
)
{
  /* Sanity check */
  if (open_channel_ind_ptr == nullptr ||
      ind_ptr              == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL open_channel_ind_ptr");
    return;
  }

  /* If Slot TLV is present, use it. Otherwise default to slot 1 */
  if (open_channel_ind_ptr->slot_valid)
  {
    ind_ptr->slot = qmi_cat_convert_slot_type_to_slot_index(open_channel_ind_ptr->slot.slot);
  }

  if (!open_channel_ind_ptr->open_channel_info_valid)
  {
    QCRIL_LOG_ERROR("%s", "Invalid open channel info TLV, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X, cat_slot_type: %d, port: %d \n",
                  __FUNCTION__, open_channel_ind_ptr->open_channel_info.ch_id,
                  open_channel_ind_ptr->slot.slot, open_channel_ind_ptr->open_channel_info.port);

  ind_ptr->ind_data.scws_open_ch.ch_id        = open_channel_ind_ptr->open_channel_info.ch_id;
  ind_ptr->ind_data.scws_open_ch.port         = open_channel_ind_ptr->open_channel_info.port;
  ind_ptr->ind_data.scws_open_ch.buffer_size  = open_channel_ind_ptr->open_channel_info.buffer_size;

  if (open_channel_ind_ptr->alpha_valid &&
      open_channel_ind_ptr->alpha.text_len > 0 &&
      open_channel_ind_ptr->alpha.text_len <= QMI_CAT_DCS_ENCODED_STRING_MAX_LENGTH)
  {
    ind_ptr->ind_data.scws_open_ch.dcs = (qmi_cat_alpha_dcs_type)open_channel_ind_ptr->alpha.dcs;
    memcpy(ind_ptr->ind_data.scws_open_ch.text,
           open_channel_ind_ptr->alpha.text,
           open_channel_ind_ptr->alpha.text_len);
    ind_ptr->ind_data.scws_open_ch.text_len = open_channel_ind_ptr->alpha.text_len;
  }
} /* qmi_cat_process_scws_open_channel_ind */


/*===========================================================================
  FUNCTION  qmi_cat_process_scws_close_channel_ind
===========================================================================*/
static void qmi_cat_process_scws_close_channel_ind
(
  cat_scws_close_channel_ind_msg_v02               * close_channel_ind_ptr,
  std::shared_ptr<qcril_gstk_qmi_ind_params_type>    ind_ptr
)
{
  /* Sanity check */
  if (close_channel_ind_ptr == nullptr ||
      ind_ptr               == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL close_channel_ind_ptr");
    return;
  }

  /* If Slot TLV is present, use it. Otherwise default to slot 1 */
  if (close_channel_ind_ptr->slot_valid)
  {
    ind_ptr->slot = qmi_cat_convert_slot_type_to_slot_index(close_channel_ind_ptr->slot.slot);
  }

  if (!close_channel_ind_ptr->close_channel_info_valid)
  {
    QCRIL_LOG_ERROR("%s", "Invalid close channel info TLV, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X, state: %d \n",
                  __FUNCTION__, close_channel_ind_ptr->close_channel_info.ch_id,
                  close_channel_ind_ptr->close_channel_info.state );

  ind_ptr->ind_data.scws_close_ch.ch_id = close_channel_ind_ptr->close_channel_info.ch_id;
  ind_ptr->ind_data.scws_close_ch.state =
         (qmi_cat_scws_channel_state_type)close_channel_ind_ptr->close_channel_info.state;
} /* qmi_cat_process_scws_close_channel_ind */


/*===========================================================================
  FUNCTION  qmi_cat_process_scws_send_data_ind
===========================================================================*/
static void qmi_cat_process_scws_send_data_ind
(
  cat_scws_send_data_ind_msg_v02                   * send_data_ind_ptr,
  std::shared_ptr<qcril_gstk_qmi_ind_params_type>    ind_ptr
)
{
  /* Sanity check */
  if (send_data_ind_ptr == NULL || ind_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL send_data_ind_ptr");
    return;
  }

  /* If Slot TLV is present, use it. Otherwise default to slot 1 */
  if (send_data_ind_ptr->slot_valid)
  {
    ind_ptr->slot = qmi_cat_convert_slot_type_to_slot_index(send_data_ind_ptr->slot.slot);
  }

  if (!send_data_ind_ptr->send_data_info_valid)
  {
    QCRIL_LOG_ERROR("%s", "Invalid send data info TLV, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO( "%s, channel_id: 0x%X, cat_slot_type: %d, total_packets: %d, current_packet: %d, data_len: 0x%X \n",
                  __FUNCTION__, send_data_ind_ptr->send_data_info.ch_id, send_data_ind_ptr->slot.slot,
                  send_data_ind_ptr->send_data_info.total_packets, send_data_ind_ptr->send_data_info.current_packet,
                  send_data_ind_ptr->send_data_info.data_len );

  ind_ptr->ind_data.scws_send_data.ch_id          = send_data_ind_ptr->send_data_info.ch_id;
  ind_ptr->ind_data.scws_send_data.total_packets  = send_data_ind_ptr->send_data_info.total_packets;
  ind_ptr->ind_data.scws_send_data.current_packet = send_data_ind_ptr->send_data_info.current_packet;

  if (send_data_ind_ptr->send_data_info.data_len > 0 &&
      send_data_ind_ptr->send_data_info.data_len <= QMI_CAT_SCWS_DATA_MAX_LENGTH)
  {
    memcpy(ind_ptr->ind_data.scws_send_data.data,
           send_data_ind_ptr->send_data_info.data,
           send_data_ind_ptr->send_data_info.data_len);
    ind_ptr->ind_data.scws_send_data.data_len = send_data_ind_ptr->send_data_info.data_len;
  }
} /* qmi_cat_process_scws_send_data_ind */


/*---------------------------------------------------------------------------
  CatModemEndPointModule class functions
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  CatModemEndPointModule - Constructor
===========================================================================*/
CatModemEndPointModule::CatModemEndPointModule
(
  string name,
  ModemEndPoint *owner
)
{
  mName = name;
  mOwner = owner;
  mServiceObject = nullptr;
  mQmiSvcClient = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;

  mMessageHandler = {
       HANDLER(QmiCatSetupRequest, CatModemEndPointModule::handleQmiClientSetup),
       HANDLER(QmiCatSetupRequestSync, CatModemEndPointModule::handleQmiClientSetup),
       HANDLER(GstkQmiCatRequestMsg, CatModemEndPointModule::handleQcrilGstkRequest),
       HANDLER(GstkQmiCatRequestSyncMsg, CatModemEndPointModule::handleQcrilGstkSyncRequest),
       HANDLER(QmiServiceUpIndMessage, CatModemEndPointModule::handleQmiServiceUp),
       HANDLER(QmiServiceDownIndMessage, CatModemEndPointModule::handleQmiServiceDown),
  };
} /* CatModemEndPointModule::CatModemEndPointModule */


/*===========================================================================
  FUNCTION  CatModemEndPointModule - init
===========================================================================*/

void CatModemEndPointModule::init()
{
  QCRIL_LOG_INFO( "%s init\n", __FUNCTION__);

  /* Call base init before doing any other stuff.*/
  Module::init();
}


/*===========================================================================
  FUNCTION  CatModemEndPointModule - Destructor
===========================================================================*/
CatModemEndPointModule::~CatModemEndPointModule()
{
  QCRIL_LOG_INFO( "%s", __FUNCTION__);
  if (mNotifyHandle != nullptr)
  {
    (void)qmi_client_release(mNotifyHandle);
    mNotifyHandle = nullptr;
  }
  if (mQmiSvcClient != nullptr)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }
  mLooper = nullptr;
} /* CatModemEndPointModule::~CatModemEndPointModule */


/*===========================================================================
  FUNCTION  onQmiNotifyCb - CatModemEndPointModule QMI client notfier
                                    callback handler
===========================================================================*/
void CatModemEndPointModule::onQmiNotifyCb
(
  qmi_client_type user_handle, qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event
)
{
  (void)user_handle;
  (void)service_obj;
  QCRIL_LOG_DEBUG("[CatModemEndPointModule]: onQmiNotifyCb() %d", service_event);

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceUpIndMessage(nullptr)));
      break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceDownIndMessage(QMI_SERVICE_ERR)));
      break;
    default:
      break;
  }
  return;
} /* CatModemEndPointModule::onQmiNotifyCb */


/*===========================================================================
  FUNCTION  CatModemEndPointModule::onQmiAsyncCb - QMI callback function
===========================================================================*/
void CatModemEndPointModule::onQmiAsyncCb
(
  std::shared_ptr<GstkQmiCatRequestMsg>  req_msg_ptr,
  unsigned int                           msg_id,
  void                                 * resp_c_struct,
  int                                    transp_err
)
{
  std::shared_ptr<qmi_cat_rsp_data_type>    resp_data_ptr  = nullptr;
  qmi_cat_rsp_data_type                   * rsp_params_ptr = nullptr;

  QCRIL_LOG_INFO("qmi_cat_async_cb, msg_id: 0x%x", msg_id);

  /* Sanity check */
  if (resp_c_struct == nullptr || req_msg_ptr == nullptr )
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL resp_c_struct");
    return;
  }

  resp_data_ptr  = std::shared_ptr<qmi_cat_rsp_data_type>(new qmi_cat_rsp_data_type{});

  if (resp_data_ptr == nullptr)
  {
    req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::FAILURE, resp_data_ptr);
    return;
  }

  rsp_params_ptr = resp_data_ptr.get();
  if(rsp_params_ptr == nullptr) {
    req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::FAILURE, resp_data_ptr);
    return;
  }
  rsp_params_ptr->transp_err = transp_err;
  rsp_params_ptr->orig_ptr = req_msg_ptr->get_orig_ptr();

  /* Process valid RESP messages */
  switch (msg_id)
  {
    case QMI_CAT_SCWS_DATA_AVAILABLEA_RESP_V02:
      rsp_params_ptr->rsp_id = QMI_CAT_SRVC_SCWS_DATA_AVAILABLE_RSP_MSG;
      if (rsp_params_ptr->transp_err == QMI_NO_ERR)
      {
        cat_scws_data_available_resp_msg_v02 * data_avail_resp_ptr =
                               (cat_scws_data_available_resp_msg_v02 *)resp_c_struct;

        rsp_params_ptr->qmi_err = data_avail_resp_ptr->resp.error;
      }
      break;

    case QMI_CAT_SEND_EVENLOPE_CMD_RESP_V02:
      rsp_params_ptr->rsp_id = QMI_CAT_SRVC_SEND_ENVELOPE_COMMAND_RSP_MSG;
      QCRIL_LOG_INFO("Envelope command response Transport err: 0x%x", rsp_params_ptr->transp_err);

      if (rsp_params_ptr->transp_err == QMI_NO_ERR)
      {
        cat_send_envelope_cmd_resp_msg_v02 * env_resp_ptr =
                               (cat_send_envelope_cmd_resp_msg_v02 *)resp_c_struct;

      QCRIL_LOG_INFO("Result: 0x%x, qmi_err: 0x%x, indication : %d",
                      env_resp_ptr->resp.result,
                      env_resp_ptr->resp.error,
                      env_resp_ptr->indication_token_valid);

        /* Response comes as indication */
        if (env_resp_ptr->indication_token_valid)
        {
          /* Cache the request, to check when indication comes */
          mEnvList.push_back(req_msg_ptr);
          QCRIL_GSTK_QMI_FREE_PTR(resp_c_struct);
          return;
        }

        rsp_params_ptr->qmi_err = env_resp_ptr->resp.error;

        if (env_resp_ptr->resp.result == QMI_RESULT_SUCCESS_V01)
        {
          if (env_resp_ptr->env_resp_data_valid)
          {
            if ((env_resp_ptr->env_resp_data.env_resp_data_len > 0) &&
                (env_resp_ptr->env_resp_data.env_resp_data_len <= QMI_CAT_RAW_ENV_RSP_DATA_MAX_LENGTH))
            {
              memcpy(rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data,
                     env_resp_ptr->env_resp_data.env_resp_data,
                     env_resp_ptr->env_resp_data.env_resp_data_len);

              rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data_len =
                         env_resp_ptr->env_resp_data.env_resp_data_len;
            }

            rsp_params_ptr->rsp_data.env_rsp_data.sw1 = env_resp_ptr->env_resp_data.sw1;
            rsp_params_ptr->rsp_data.env_rsp_data.sw2 = env_resp_ptr->env_resp_data.sw2;
          }
        }
      }
      break;

    case QMI_CAT_SEND_TR_RESP_V02:
      rsp_params_ptr->rsp_id = QMI_CAT_SRVC_SEND_TERMINAL_RESPONSE_RSP_MSG;
      QCRIL_LOG_INFO("TR response Transport err: 0x%x", rsp_params_ptr->transp_err);

      if (rsp_params_ptr->transp_err == QMI_NO_ERR)
      {
        cat_send_tr_resp_msg_v02 * tr_resp_ptr = (cat_send_tr_resp_msg_v02 *)resp_c_struct;

        QCRIL_LOG_INFO("Result: 0x%x, qmi_err: 0x%x, indication : %d", 
                        tr_resp_ptr->resp.result,
                        tr_resp_ptr->resp.error,
                        tr_resp_ptr->indication_token_valid);

        /* Response comes as indication */
        if (tr_resp_ptr->indication_token_valid)
        {
          /* Cache the TR request to check when indication comes */
          mTrMsgPtr = req_msg_ptr;
          QCRIL_GSTK_QMI_FREE_PTR(resp_c_struct);
          return;
        }

        rsp_params_ptr->qmi_err = tr_resp_ptr->resp.error;

        if (tr_resp_ptr->resp.result == QMI_RESULT_SUCCESS_V01)
        {
          if (tr_resp_ptr->tr_response_data_valid &&
              (tr_resp_ptr->tr_response_data.tr_response_len > 0) &&
              (tr_resp_ptr->tr_response_data.tr_response_len <= QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH))
          {

            memcpy(rsp_params_ptr->rsp_data.tr_rsp_data.tr_response,
                   tr_resp_ptr->tr_response_data.tr_response,
                   tr_resp_ptr->tr_response_data.tr_response_len);

            rsp_params_ptr->rsp_data.tr_rsp_data.tr_response_len =
                       tr_resp_ptr->tr_response_data.tr_response_len;

            rsp_params_ptr->rsp_data.tr_rsp_data.sw1 = tr_resp_ptr->tr_response_data.sw1;
            rsp_params_ptr->rsp_data.tr_rsp_data.sw2 = tr_resp_ptr->tr_response_data.sw2;
          }
        }
      }
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported QMI CAT response: 0x%x", msg_id);
      break;
  }

  /* Send response back to GSTK module */
  req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);

  QCRIL_GSTK_QMI_FREE_PTR(resp_c_struct);
} /* onQmiAsyncCb */


/*===========================================================================
  FUNCTION  onQmiUnsolIndCb - CatModemEndPointModule QMI CAT unsol
                                    message handler
===========================================================================*/
void CatModemEndPointModule::onQmiUnsolIndCb
(
  qmi_cat_indication_id_type   ind_id,
  uint32_t                     ind_len,
  uint8_t                    * ind_data_ptr
)
{
  auto ind_ptr     = std::shared_ptr<qcril_gstk_qmi_ind_params_type>(new qcril_gstk_qmi_ind_params_type{});;
  auto ind_msg_ptr = std::make_shared<GstkQmiCatIndicationMsg>(ind_id, ind_ptr);

  if (ind_data_ptr == nullptr || ind_len == 0 ||
      ind_msg_ptr == nullptr || ind_ptr == nullptr)
  {
    return;
  }

  switch(ind_id)
  {
    case QMI_CAT_SEND_TR_IND:
      if (mTrMsgPtr != nullptr)
      {
        cat_send_tr_ind_msg_v02        * tr_ind_ptr = (cat_send_tr_ind_msg_v02 *)ind_data_ptr;
        qmi_cat_terminal_response_type * tr_req_ptr = (qmi_cat_terminal_response_type *)mTrMsgPtr->get_message();

        if (tr_req_ptr != nullptr &&
            tr_ind_ptr->indication_token == tr_req_ptr->token)
        {
          auto resp_data_ptr  = std::shared_ptr<qmi_cat_rsp_data_type>(new qmi_cat_rsp_data_type{});

          if (resp_data_ptr != nullptr)
          {
            qmi_cat_rsp_data_type  * rsp_params_ptr = resp_data_ptr.get();

            rsp_params_ptr->rsp_id     = QMI_CAT_SRVC_SEND_TERMINAL_RESPONSE_RSP_MSG;
            rsp_params_ptr->transp_err = QMI_NO_ERR;
            rsp_params_ptr->orig_ptr   = mTrMsgPtr->get_orig_ptr();
            rsp_params_ptr->qmi_err    = tr_ind_ptr->resp.error;

            if (tr_ind_ptr->resp.result == QMI_RESULT_SUCCESS_V01)
            {
              if (tr_ind_ptr->tr_response_data_valid &&
                  (tr_ind_ptr->tr_response_data.tr_response_len > 0) &&
                  (tr_ind_ptr->tr_response_data.tr_response_len <= QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH))
              {
                memcpy(rsp_params_ptr->rsp_data.tr_rsp_data.tr_response,
                       tr_ind_ptr->tr_response_data.tr_response,
                       tr_ind_ptr->tr_response_data.tr_response_len);

                rsp_params_ptr->rsp_data.tr_rsp_data.tr_response_len =
                           tr_ind_ptr->tr_response_data.tr_response_len;

                rsp_params_ptr->rsp_data.tr_rsp_data.sw1 = tr_ind_ptr->tr_response_data.sw1;
                rsp_params_ptr->rsp_data.tr_rsp_data.sw2 = tr_ind_ptr->tr_response_data.sw2;
              }
            }
          }
          mTrMsgPtr->sendResponse(mTrMsgPtr, Message::Callback::Status::SUCCESS, resp_data_ptr);
          mTrMsgPtr = nullptr;
        }
      }
      else
      {
        QCRIL_LOG_ERROR("No TR response pending");
      }
      return;
    case QMI_CAT_SEND_EVENLOPE_CMD_IND:
      for (auto msg_ptr : mEnvList)
      {
        static uint8_t                                pos    = 0;
        cat_send_envelope_cmd_ind_msg_v02    * env_ind_ptr   = (cat_send_envelope_cmd_ind_msg_v02 *)ind_data_ptr;
        qcril_gstk_request_envelope_cmd_type * env_req_ptr   = (qcril_gstk_request_envelope_cmd_type *)msg_ptr->get_message();
        auto                                   resp_data_ptr = std::shared_ptr<qmi_cat_rsp_data_type>(new qmi_cat_rsp_data_type{});

        if (env_req_ptr == nullptr)
        {
          break;
        }

        if (env_ind_ptr->indication_token != env_req_ptr->token)
        {
          pos++;
          continue;
        }

        QCRIL_LOG_ERROR("Found envlope command at %d", pos);
        if (resp_data_ptr != nullptr)
        {
          qmi_cat_rsp_data_type  * rsp_params_ptr = resp_data_ptr.get();

          rsp_params_ptr->rsp_id = QMI_CAT_SRVC_SEND_ENVELOPE_COMMAND_RSP_MSG;
          rsp_params_ptr->transp_err = QMI_NO_ERR;
          rsp_params_ptr->orig_ptr   = msg_ptr->get_orig_ptr();
          rsp_params_ptr->qmi_err    = env_ind_ptr->resp.error;

          if (env_ind_ptr->resp.result == QMI_RESULT_SUCCESS_V01)
          {
            if (env_ind_ptr->env_resp_data_valid)
            {
              if ((env_ind_ptr->env_resp_data.env_resp_data_len > 0) &&
                  (env_ind_ptr->env_resp_data.env_resp_data_len <= QMI_CAT_RAW_ENV_RSP_DATA_MAX_LENGTH))
              {
                memcpy(rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data,
                       env_ind_ptr->env_resp_data.env_resp_data,
                       env_ind_ptr->env_resp_data.env_resp_data_len);

                rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data_len =
                           env_ind_ptr->env_resp_data.env_resp_data_len;
              }

              rsp_params_ptr->rsp_data.env_rsp_data.sw1 = env_ind_ptr->env_resp_data.sw1;
              rsp_params_ptr->rsp_data.env_rsp_data.sw2 = env_ind_ptr->env_resp_data.sw2;
            }
          }
        }
        if(msg_ptr) {
          msg_ptr->sendResponse(msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
          mEnvList.erase(mEnvList.begin()+pos);
        }
        return;
      }
      QCRIL_LOG_ERROR("No matching envelope command pending");
      return;
    case QMI_CAT_EVENT_REPORT_IND:
      /* Check any pending proactive command is present or not */
      if (mTrMsgPtr != nullptr)
      {
        mTrMsgPtr->sendResponse(mTrMsgPtr, Message::Callback::Status::FAILURE, nullptr);
        mTrMsgPtr = nullptr;
      }
      qmi_cat_process_event_report_ind(
         (cat_event_report_ind_msg_v02 *)ind_data_ptr, ind_ptr);
      break;
    case QMI_CAT_SCWS_OPEN_CHANNEL_IND:
      qmi_cat_process_scws_open_channel_ind(
         (cat_scws_open_channel_ind_msg_v02*)ind_data_ptr, ind_ptr);
      break;
    case QMI_CAT_SCWS_CLOSE_CHANNEL_IND:
      qmi_cat_process_scws_close_channel_ind(
         (cat_scws_close_channel_ind_msg_v02*)ind_data_ptr, ind_ptr);
      break;
    case QMI_CAT_SCWS_SEND_DATA_IND:
      qmi_cat_process_scws_send_data_ind(
         (cat_scws_send_data_ind_msg_v02*)ind_data_ptr, ind_ptr);
      break;
    default:
      return;
  }

  ind_msg_ptr->broadcast();
} /* CatModemEndPointModule::onQmiUnsolIndCb */


/*===========================================================================
  FUNCTION  handleQmiClientSetup -  CatModemEndPointModule QMI Client setup
                                    requet handler
===========================================================================*/
void CatModemEndPointModule::handleQmiClientSetup
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;

  QCRIL_LOG_INFO( "handleQmiClientSetup : %d", mOwner->getState());

  if (mServiceObject == nullptr)
  {
    mServiceObject = cat_get_service_object_v02();
    if (mServiceObject == nullptr)
    {
      QCRIL_LOG_ERROR("[CatModemEndPointModule]:Did not get cat_get_service_object");
      return;
    }

    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifyParams, &mNotifyHandle);

    if (rc == QMI_NO_ERR)
    {
      mMessageList.push_back(msg);
      rc = qmi_client_register_notify_cb(mNotifyHandle, qmi_cat_notify_cb, this);
      if (rc != QMI_NO_ERR)
      {
        QCRIL_LOG_ERROR("qmi_client_register_notify_cb failed : %d", rc);
      }
    }
    else
    {
      QCRIL_LOG_ERROR("qmi_client_notifier_init failed : %d", rc);
    }
    return;
  }
  else
  {
    mMessageList.push_back(msg);
    return;
  }
} /* CatModemEndPointModule::handleQmiClientSetup */


/*===========================================================================
  FUNCTION  handleQmiServiceUp -    CatModemEndPointModule QMI service up
                                    indication handler
===========================================================================*/
void CatModemEndPointModule::handleQmiServiceUp
(
  std::shared_ptr<Message> msg
)
{
  qmi_client_error_type client_err     = QMI_NO_ERR;
  qmi_client_type       qmi_svc_client = nullptr;
  qmi_service_info      info;

  (void)msg;
  QCRIL_LOG_DEBUG("[CatModemEndPointModule]: handleQmiServiceUp()");

  if (mQmiSvcClient != nullptr)
  {
    QCRIL_LOG_INFO( "Already Registered to service");
    return;
  }

  memset(&mOsParams, 0x00, sizeof(mOsParams));

  client_err = qmi_client_get_any_service(mServiceObject, &info);
  Log::getInstance().d("[CatModemEndPointModule]: client_info: " + std::to_string(client_err));

  client_err = qmi_client_init(&info,
                               mServiceObject,
                               (qmi_client_ind_cb)qmi_cat_client_indication_cb,
                               this,
                               &mOsParams,
                               &qmi_svc_client);

  QCRIL_LOG_DEBUG("[CatModemEndPointModule]: client_err = %d", client_err);
  if (client_err == QMI_NO_ERR)
  {
    QCRIL_LOG_INFO( "Qmi service client : %p", qmi_svc_client);

    mQmiSvcClient = qmi_svc_client;

    mOwner->setState(ModemEndPoint::State::OPERATIONAL);

    /* Notify clients of this end-point that  we have qmi client handle. */
    for (auto msg : mMessageList)
    {
      QCRIL_LOG_DEBUG("Notify client = %s, handleQmiServiceUp : %d", msg->dump().c_str(), mOwner->getState());
      std::shared_ptr<QmiCatSetupRequest> shared_msg =
          std::static_pointer_cast<QmiCatSetupRequest>(msg);
      if(shared_msg)
      {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
                                 nullptr);
      }
    }
    mMessageList.clear();

    /* send service up indication */
    std::shared_ptr<GstkQmiCatIndicationMsg> ind_msg_ptr =
         std::make_shared<GstkQmiCatIndicationMsg>(QMI_CAT_SRVC_UP_IND_MSG,
                                                   nullptr);
    if (ind_msg_ptr != nullptr)
    {
      ind_msg_ptr->broadcast();
    }
  }
} /* CatModemEndPointModule::handleQmiServiceUp */


/*===========================================================================
  FUNCTION  handleQmiServiceDown -  CatModemEndPointModule QMI service down
                                    indication handler
===========================================================================*/
void CatModemEndPointModule::handleQmiServiceDown
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;

  if (mQmiSvcClient != nullptr)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }

  QCRIL_LOG_INFO( "handleQmiServiceDown : %d", mOwner->getState());

  mOwner->setState(ModemEndPoint::State::NON_OPERATIONAL);

  /* Cleanup pending TR and Envelope commands */
  if (mTrMsgPtr != nullptr)
  {
    mTrMsgPtr->sendResponse(mTrMsgPtr, Message::Callback::Status::FAILURE, nullptr);
    mTrMsgPtr = nullptr;
  }

  for (auto msg_ptr : mEnvList)
  {
    if(msg_ptr)
    {
      msg_ptr->sendResponse(msg_ptr, Message::Callback::Status::FAILURE, nullptr);
    }
  }
  mEnvList.clear();

  std::shared_ptr<GstkQmiCatIndicationMsg> ind_msg_ptr =
       std::make_shared<GstkQmiCatIndicationMsg>(QMI_CAT_SRVC_DOWN_IND_MSG, nullptr);
  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* CatModemEndPointModule::handleQmiServiceDown */


/*===========================================================================
  FUNCTION  handleQcrilGstkRequest - CatModemEndPointModule QCRIL request
                                    handler
===========================================================================*/
void CatModemEndPointModule::handleQcrilGstkRequest
(
  std::shared_ptr<GstkQmiCatRequestMsg> msg_ptr
)
{
  int                           ret       = QMI_INTERNAL_ERR;
  GstkUserData                * user_data = nullptr;

  QCRIL_LOG_INFO("%s, State: %d", __FUNCTION__, mOwner->getState());

  user_data = new GstkUserData{msg_ptr, this};

  if (user_data != nullptr &&
      mOwner->getState() == ModemEndPoint::State::OPERATIONAL)
  {
    ret = qmi_cat_process_qcril_gstk_request(mQmiSvcClient,
                                            msg_ptr->get_msg_id(),
                                            msg_ptr->get_message(),
                                            qmi_cat_async_cb,
                                            (void *)user_data,
                                            nullptr);
  }

  QCRIL_LOG_INFO(" QMI error code : 0x%x", ret);

  if (ret != QMI_NO_ERR)
  {
    if (user_data != nullptr)
    {
      delete user_data;
      user_data = nullptr;
    }
    msg_ptr->sendResponse(msg_ptr, Message::Callback::Status::FAILURE,
                           nullptr);
  }
} /* CatModemEndPointModule::handleQcrilGstkRequest */


/*===========================================================================
  FUNCTION  handleQcrilGstkSyncRequest - CatModemEndPointModule QCRIL sync
                                        request handler
===========================================================================*/
void CatModemEndPointModule::handleQcrilGstkSyncRequest
(
  std::shared_ptr<GstkQmiCatRequestSyncMsg> msg_ptr
)
{
  std::shared_ptr<int>    respPtr   = nullptr;

  QCRIL_LOG_INFO("%s, State: %d", __FUNCTION__, mOwner->getState());

  respPtr = std::make_shared<int>(QMI_INTERNAL_ERR);

  if (mOwner->getState() != ModemEndPoint::State::OPERATIONAL ||
      respPtr == nullptr ||
      msg_ptr == nullptr)
  {
    QCRIL_LOG_INFO(" Failed to handle request req: %p, resp: %p", respPtr.get(), msg_ptr.get());

    respPtr = nullptr;
    msg_ptr->sendResponse(msg_ptr,
                          Message::Callback::Status::FAILURE,
                          respPtr);
    return;
  }

  *respPtr = qmi_cat_process_qcril_gstk_request(mQmiSvcClient,
                                                msg_ptr->get_msg_id(),
                                                msg_ptr->get_message(),
                                                nullptr,
                                                nullptr,
                                                msg_ptr->get_rsp_data());

 QCRIL_LOG_INFO(" QMI error code : 0x%x", *respPtr);

  msg_ptr->sendResponse(msg_ptr,
                        Message::Callback::Status::SUCCESS,
                        respPtr);
} /* CatModemEndPointModule::handleQcrilGstkSyncRequest */

#ifdef QMI_RIL_UTF
void CatModemEndPointModule::cleanUpQmiSvcClient()
{
    if(nullptr != mQmiSvcClient)
    {
        qmi_client_release(mQmiSvcClient);
        mQmiSvcClient = nullptr;
        mServiceObject = nullptr;
    }
}
#endif
