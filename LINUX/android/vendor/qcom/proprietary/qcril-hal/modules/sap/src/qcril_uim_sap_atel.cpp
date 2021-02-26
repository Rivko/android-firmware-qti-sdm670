/*===========================================================================

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "ril.h"
#include "modules/android/version.h"
#include "framework/Log.h"
#include "modules/uim/qcril_uim_sap.h"
#include "pb_decode.h"
#include "pb_encode.h"

#ifndef QMI_RIL_UTF
#include <hardware/ril/librilutils/proto/sap-api.pb.h>
#else
#include "sap-api.pb.h"
#endif

#define TAG "ATEL_SAP"

/* Forward declarations */
static const char *   qcril_uim_sap_get_version(void);
static void           qcril_uim_sap_on_cancel(RIL_Token token);
static int            qcril_uim_sap_on_supports(int request_id);
static RIL_RadioState qcril_uim_sap_on_state_request(void);
static void           qcril_uim_sap_on_request_atel(int       request,
                                                    void    * data,
                                                    size_t    datalen,
                                                    RIL_Token t);
/* Global variables */
static qmi_uim_sap_status_type     qcril_sap_state        = QMI_UIM_SAP_STATE_NOT_ENABLED;
static RIL_Env                   * qcril_sap_response_api;
static const RIL_RadioFunctions    qcril_sap_request_api  =
{ QCRIL_RIL_VERSION,
  qcril_uim_sap_on_request_atel,
  qcril_uim_sap_on_state_request,
  qcril_uim_sap_on_supports,
  qcril_uim_sap_on_cancel,
  qcril_uim_sap_get_version
};

typedef struct
{
  int       request_id;
  uint8   * data_ptr;
  size_t    data_len;
} qcril_uim_sap_atel_request;


/*===========================================================================

INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

FUNCTION:  qcril_uim_sap_check_supported_messages

===========================================================================*/
/*!
    @brief
    Checks if the particualr MsgId is supported by the RIL implementation.

    @return
    1 if supported, 0 otherwise.
*/
/*=========================================================================*/
static int qcril_uim_sap_check_supported_messages
(
  int     request_id
)
{
  int result = 0;

  switch ((MsgId)request_id)
  {
    /* Supported */
    case MsgId_RIL_SIM_SAP_CONNECT:
    case MsgId_RIL_SIM_SAP_DISCONNECT:
    case MsgId_RIL_SIM_SAP_APDU:
    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
    case MsgId_RIL_SIM_SAP_POWER:
    case MsgId_RIL_SIM_SAP_RESET_SIM:
    case MsgId_RIL_SIM_SAP_STATUS:
    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
    case MsgId_RIL_SIM_SAP_ERROR_RESP:
      result = 1;
      break;

    /* Unsupported */
    case MsgId_UNKNOWN_REQ:
    case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:
      default:
      break;
  }
  return result;
} /* qcril_uim_sap_check_supported_messages */


/*=========================================================================

FUNCTION:  qcril_uim_sap_convert_err_to_proto_err

===========================================================================*/
/*!
    @brief
    Converts qcril uim sap error to proto error.

    @return
    int proto error.
*/
/*=========================================================================*/
int qcril_uim_sap_convert_err_to_proto_err(int msg_id, int err_code)
{
  int err = 0;

  switch (msg_id)
  {
    case MsgId_RIL_SIM_SAP_APDU:
      switch(err_code)
      {
        case QCRIL_UIM_SAP_SUCCESS:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SUCCESS;
          break;
        case QCRIL_UIM_SAP_GENERIC_FAILURE:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_GENERIC_FAILURE;
          break;
        case QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SIM_NOT_READY;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SIM_ALREADY_POWERED_OFF;
          break;
        case QCRIL_UIM_SAP_CARD_REMOVED:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SIM_ABSENT;
          break;
        default:
          err = RIL_SIM_SAP_APDU_RSP_Response_RIL_E_GENERIC_FAILURE;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
      switch(err_code)
      {
        case QCRIL_UIM_SAP_SUCCESS:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SUCCESS;
          break;
        case QCRIL_UIM_SAP_GENERIC_FAILURE:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_GENERIC_FAILURE;
          break;
        case QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SIM_DATA_NOT_AVAILABLE;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SIM_ALREADY_POWERED_OFF;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_ON:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SIM_ALREADY_POWERED_ON;
          break;
        case QCRIL_UIM_SAP_CARD_REMOVED:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SIM_ABSENT;
          break;
        default:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_GENERIC_FAILURE;
      }
      break;

    case MsgId_RIL_SIM_SAP_POWER:
      switch(err_code)
      {
        case QCRIL_UIM_SAP_SUCCESS:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SUCCESS;
          break;
        case QCRIL_UIM_SAP_GENERIC_FAILURE:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_GENERIC_FAILURE;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SIM_ALREADY_POWERED_OFF;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_ON:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SIM_ALREADY_POWERED_ON;
          break;
        case QCRIL_UIM_SAP_CARD_REMOVED:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SIM_ABSENT;
          break;
        default:
          err = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_GENERIC_FAILURE;
      }
      break;

    case MsgId_RIL_SIM_SAP_RESET_SIM:
      switch(err_code)
      {
        case QCRIL_UIM_SAP_SUCCESS:
          err = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_SUCCESS;
          break;
        case QCRIL_UIM_SAP_GENERIC_FAILURE:
          err = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_GENERIC_FAILURE;
          break;
        case QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
          err = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_SIM_NOT_READY;
          break;
        case QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
          err = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_SIM_ALREADY_POWERED_OFF;
          break;
        case QCRIL_UIM_SAP_CARD_REMOVED:
          err = RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SIM_ABSENT;
          break;
        default:
          err = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_GENERIC_FAILURE;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      switch(err_code)
      {
        case QCRIL_UIM_SAP_SUCCESS:
          err = RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response_RIL_E_SUCCESS;
          break;
        case QCRIL_UIM_SAP_GENERIC_FAILURE:
          err = RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response_RIL_E_GENERIC_FAILURE;
          break;
        case QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
        case QCRIL_UIM_SAP_CARD_REMOVED:
          err = RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response_RIL_E_SIM_DATA_NOT_AVAILABLE;
          break;
        default:
          err = RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response_RIL_E_GENERIC_FAILURE;
      }
      break;
    default:
      break;
  }
  return err;
} /* qcril_uim_sap_convert_err_to_proto_err */


/*=========================================================================

FUNCTION:  qcril_uim_sap_create_payload

===========================================================================*/
/*!
    @brief
    Allocated the buffer for the variable payload dynamically and copies the
    data that needs to sent in the response.
    Note that the memory allocated here needs to be freed by the caller.

    @return
    Pointer to valid buffer if successful, NULL otherwise.
*/
/*=========================================================================*/
static pb_bytes_array_t  * qcril_uim_sap_create_payload
(
  uint8   * data_ptr,
  uint16    data_len
)
{
  pb_bytes_array_t   * payload_data_ptr = NULL;
  uint16               payload_data_len = 0;

  /* Sanity check */
  if ((data_ptr == NULL) || (data_len == 0))
  {
    return NULL;
  }

  payload_data_len = sizeof(pb_bytes_array_t) + data_len;
  payload_data_ptr = (pb_bytes_array_t *)qcril_malloc(payload_data_len);
  if (payload_data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "payload_data_ptr alloc failed");
    return NULL;
  }

  /* Copy the data */
  memset(payload_data_ptr, 0, payload_data_len);
  payload_data_ptr->size = (size_t)data_len;
  memcpy(payload_data_ptr->bytes, data_ptr, data_len);

  return payload_data_ptr;
} /* qcril_uim_sap_create_payload */


/*=========================================================================

FUNCTION:  qcril_uim_sap_unpack_message

===========================================================================*/
/*!
    @brief
    Unpacks specific messages by passing the incoming raw byte stream through
    the nanopb's decode API. Also note that the memory allocated for the
    request pointer must be freed by the caller.

    @return
    Valid pointer if supported, NULL otherwise.
*/
/*=========================================================================*/
static void * qcril_uim_sap_unpack_message
(
  MsgId                         message_id,
  uint8                       * payload_ptr,
  size_t                        payload_len
)
{
  void          * request_ptr   = NULL;
  pb_istream_t    stream;

  /* Create the stream */
  stream = pb_istream_from_buffer((uint8_t *)payload_ptr, payload_len);

  /* Decode based on the message id */
  switch (message_id)
  {
    case MsgId_RIL_SIM_SAP_CONNECT:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_CONNECT_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_CONNECT_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_DISCONNECT:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_DISCONNECT_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_DISCONNECT_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_APDU:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_APDU_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_APDU_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_TRANSFER_ATR_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_TRANSFER_ATR_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_POWER:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_POWER_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_POWER_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_RESET_SIM:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_RESET_SIM_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_RESET_SIM_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(request_ptr, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ));
      if (!pb_decode(&stream, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ_fields, request_ptr))
      {
        QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");
        return NULL;
      }
      break;

    /* Unsupported */
    case MsgId_UNKNOWN_REQ:
    case MsgId_RIL_SIM_SAP_ERROR_RESP:
    case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:
      default:
      break;
  }
  return request_ptr;
} /* qcril_uim_sap_unpack_message */


/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_atel_sap_ind

===========================================================================*/
/*!
    @brief
    Handles the SAP indication callback. Based on the indication received
    from the modem, respective packed unsol response types are constructed
    and the OnUnsolicitedResponse is called.

    Note that the qmi_indication_ptr pointer is freed in the caller - the
    main indication callback.

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_sap_qmi_handle_atel_sap_ind
(
  qmi_uim_sap_status_type  sap_state
)
{
  MsgId                           message_id       = (MsgId)0;
  void                          * response_ptr     = NULL;
  uint16                          response_len     = 0;
  pb_ostream_t                    stream;


  QCRIL_LOG_DEBUG("qcril_uim_sap_qmi_handle_atel_sap_ind, sap_state: 0x%x ", sap_state);

  /* Only 2 unsol responses are send from the QMI IND & it depends on the sap_status:
  1. RIL_SIM_SAP_STATUS_IND, upon status for connection establishment/disconnection
  2. RIL_SIM_SAP_DISCONNECT_IND, upon disconnection request only */
  qcril_sap_state = sap_state;
  switch (qcril_sap_state)
  {
    case QMI_UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY:
      {
        RIL_SIM_SAP_DISCONNECT_IND   disconnect_ind;

        response_ptr = (uint8_t *)qcril_malloc(sizeof(RIL_SIM_SAP_DISCONNECT_IND));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(&disconnect_ind, 0, sizeof(RIL_SIM_SAP_DISCONNECT_IND));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_DISCONNECT_IND));

        message_id = MsgId_RIL_SIM_SAP_DISCONNECT;
        disconnect_ind.disconnectType =
        RIL_SIM_SAP_DISCONNECT_IND_DisconnectType_RIL_S_DISCONNECT_TYPE_GRACEFUL;

        /* Create an output stream & encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_DISCONNECT_IND));

        if (!pb_encode(&stream, RIL_SIM_SAP_DISCONNECT_IND_fields, &disconnect_ind))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_DISCONNECT_IND");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case QMI_UIM_SAP_STATE_CONNECTION_ERROR:
    case QMI_UIM_SAP_STATE_NOT_ENABLED:
      {
        RIL_SIM_SAP_STATUS_IND   msg_status_ind;

        response_ptr = (uint8_t *)qcril_malloc(sizeof(RIL_SIM_SAP_STATUS_IND));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(&msg_status_ind, 0, sizeof(RIL_SIM_SAP_STATUS_IND));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_STATUS_IND));

        message_id = MsgId_RIL_SIM_SAP_STATUS;

        msg_status_ind.statusChange =
        RIL_SIM_SAP_STATUS_IND_Status_RIL_SIM_STATUS_CARD_NOT_ACCESSIBLE;

        /* Create an output stream & encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_STATUS_IND));

        if (!pb_encode(&stream, RIL_SIM_SAP_STATUS_IND_fields, &msg_status_ind))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_STATUS_IND");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case QMI_UIM_SAP_STATE_CONNECTED_SUCCESSFULLY:
    case QMI_UIM_SAP_STATE_CONNECTING:
    case QMI_UIM_SAP_STATE_DISCONNECTING:
    default:
      /* Note that for a succesfully connected case, if the connection response
      isnt sent yet, the STATUS_IND is sent after response callback comes */
      QCRIL_LOG_DEBUG("Skipping SAP UNSOL response for sap_state: 0x%x", qcril_sap_state);
      return;
  }

  /* Send the UNSOL Response for valid cases */
  if ((response_ptr != NULL) &&
      (response_len != 0))
  {
    /* Send the data using OnUnsolicitedResponse */
    qcril_sap_response_api->OnUnsolicitedResponse(message_id,
                                                  response_ptr,
                                                  response_len);
  }

  /* Clear allocated response pointer */
  QCRIL_UIM_SAP_FREE_PTR(response_ptr);
} /* qcril_uim_sap_qmi_handle_atel_sap_ind */


/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_atel_card_status

===========================================================================*/
/*!
    @brief
    Handles the card status. Based on the card status received
    from the modem, respective packed unsol response types are constructed
    and the OnUnsolicitedResponse is called.

    Note that the qmi_indication_ptr pointer is freed in the caller - the
    main indication callback.

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_sap_qmi_handle_atel_card_status
(
  qcril_card_status_e_type  card_state
)
{
  uint8_t                       * response_ptr     = NULL;
  uint16                          response_len     = 0;
  pb_ostream_t                    stream;
  RIL_SIM_SAP_STATUS_IND          msg_status_ind;
  boolean                         send_unsol       = FALSE;


  QCRIL_LOG_DEBUG("qcril_uim_sap_qmi_handle_atel_card_status, card_state: 0x%x ", card_state);

  memset(&msg_status_ind, 0, sizeof(RIL_SIM_SAP_STATUS_IND));

  switch (card_state)
  {
    case QCRIL_CARD_STATUS_ABSENT:
      send_unsol = TRUE;
      msg_status_ind.statusChange =
        RIL_SIM_SAP_STATUS_IND_Status_RIL_SIM_STATUS_CARD_REMOVED;
      break;

    case QCRIL_CARD_STATUS_UP:
      /* Send card insert event only if SAP is already connected */
      if (qcril_sap_state == QMI_UIM_SAP_STATE_CONNECTED_SUCCESSFULLY)
      {
        send_unsol = TRUE;
        msg_status_ind.statusChange =
          RIL_SIM_SAP_STATUS_IND_Status_RIL_SIM_STATUS_CARD_INSERTED;
      }
      break;

    default:
      break;
  }

  if (send_unsol)
  {
    response_ptr = (uint8_t *)qcril_malloc(sizeof(RIL_SIM_SAP_STATUS_IND));
    if (response_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
      return;
    }

    memset(response_ptr, 0, sizeof(RIL_SIM_SAP_STATUS_IND));

    /* Create an output stream & encode the outgoing message */
    stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_STATUS_IND));

    if (!pb_encode(&stream, RIL_SIM_SAP_STATUS_IND_fields, &msg_status_ind))
    {
      QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_STATUS_IND");
      QCRIL_UIM_SAP_FREE_PTR(response_ptr);
      return;
    }
    response_len = stream.bytes_written;
    /* Send the data using OnUnsolicitedResponse */
    qcril_sap_response_api->OnUnsolicitedResponse(MsgId_RIL_SIM_SAP_STATUS,
                                                  response_ptr,
                                                  response_len);
    /* Clear allocated response pointer */
    QCRIL_UIM_SAP_FREE_PTR(response_ptr);
  }
} /* qcril_uim_sap_qmi_handle_atel_card_status */


/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_atel_sap_request_resp

===========================================================================*/
/*!
    @brief
    Handles the SAP request callback. Based on the response
    received from the modem, respective packed response types are constructed
    and the onRequestComplete is called. This completes the original request
    called on the RIL SAP socket.

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_sap_qmi_handle_atel_sap_request_resp
(
  int             request_id,
  int             err_code,
  int             qmi_err_code,
  void          * token,
  uint8_t       * data_ptr,
  uint32_t        data_len
)
{
  MsgId                                message_id         = (MsgId)request_id;
  RIL_Errno                            ril_err            = RIL_E_SUCCESS;
  void                               * response_ptr       = NULL;
  uint16                               response_len       = 0;
  pb_bytes_array_t                   * payload_data_ptr   = NULL;
  size_t                               encoded_size       = 0;
  void                               * unsol_response_ptr = NULL;
  uint16                               unsol_response_len = 0;
  pb_ostream_t                         stream;


  /* Sanity check */
  if(qcril_sap_response_api == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL qcril_sap_response_api");
    return;
  }

  ril_err = qcril_uim_convert_err_value(qmi_err_code);

  QCRIL_LOG_DEBUG("handle_sap_request_resp, token: %d, MsgId: 0x%x, qmi_err_code: 0x%x, err_code: %d",
                  qcril_log_get_uim_token_id((RIL_Token)token),
                  message_id,
                  qmi_err_code,
                  err_code);

  /* Update the parameters for respective responses */
  switch (message_id)
  {
    case MsgId_RIL_SIM_SAP_CONNECT:
      {
        RIL_SIM_SAP_CONNECT_RSP resp;

        response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_CONNECT_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        /* Encode the outgoing message */
        memset(&resp, 0, sizeof(RIL_SIM_SAP_CONNECT_RSP));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_CONNECT_RSP));

        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_CONNECT_RSP));

        resp.response = (qmi_err_code == 0) ?
          RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SUCCESS :
          RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SAP_CONNECT_FAILURE;

        if (!pb_encode(&stream, RIL_SIM_SAP_CONNECT_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_CONNECT_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;

        if (qmi_err_code != 0)
        {
          if (qcril_uim_sap_get_active_client() == QCRIL_UIM_SAP_CLIENT_ID_ATEL)
          {
            qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_MAX_CLIENTS);
          }
        }

        /* Now send UNSOL response if needed */
        if ((qmi_err_code == 0) &&
            (qcril_sap_state == QMI_UIM_SAP_STATE_CONNECTED_SUCCESSFULLY))
        {
          RIL_SIM_SAP_STATUS_IND   msg_status_ind;

          unsol_response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_STATUS_IND));
          if (unsol_response_ptr == NULL)
          {
            QCRIL_LOG_ERROR("%s","Error allocating unsol_response_ptr");
            return;
          }

          memset(&msg_status_ind, 0, sizeof(RIL_SIM_SAP_STATUS_IND));
          memset(unsol_response_ptr, 0, sizeof(RIL_SIM_SAP_STATUS_IND));

          message_id = MsgId_RIL_SIM_SAP_STATUS;

          msg_status_ind.statusChange = RIL_SIM_SAP_STATUS_IND_Status_RIL_SIM_STATUS_CARD_RESET;

          /* Create an output stream & encode the outgoing message */
          stream = pb_ostream_from_buffer((uint8_t *)unsol_response_ptr, sizeof(RIL_SIM_SAP_STATUS_IND));

          if (!pb_encode(&stream, RIL_SIM_SAP_STATUS_IND_fields, &msg_status_ind))
          {
            QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_STATUS_IND");
            QCRIL_UIM_SAP_FREE_PTR(unsol_response_ptr);
            return;
          }
          unsol_response_len = stream.bytes_written;
        }
      }
      break;

    case MsgId_RIL_SIM_SAP_DISCONNECT:
      {
        RIL_SIM_SAP_DISCONNECT_RSP resp;

        /* Nothing to update in the response struct */
        response_ptr = (uint8_t *)qcril_malloc(sizeof(RIL_SIM_SAP_DISCONNECT_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr, cannot proceed");
          return;
        }

        /* Encode the outgoing message */
        memset(&resp, 0, sizeof(RIL_SIM_SAP_DISCONNECT_RSP));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_DISCONNECT_RSP));

        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_DISCONNECT_RSP));

        /* Encode the message */
        if (!pb_encode(&stream, RIL_SIM_SAP_DISCONNECT_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_CONNECT_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;

        /* Also reset the active client ID upon a successful disconnection */
        qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_MAX_CLIENTS);
      }
      break;

    case MsgId_RIL_SIM_SAP_APDU:
      {
        RIL_SIM_SAP_APDU_RSP   resp;

        /* Update the outgoing message */
        memset(&resp, 0, sizeof(RIL_SIM_SAP_APDU_RSP));

        resp.type         = RIL_SIM_SAP_APDU_RSP_Type_RIL_TYPE_APDU;
        resp.response     = (RIL_SIM_SAP_APDU_RSP_Response)qcril_uim_sap_convert_err_to_proto_err(message_id, err_code);
        if (data_len == 0 ||
            data_ptr == NULL ||
            resp.response != RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SUCCESS)
        {
          payload_data_ptr = NULL;
        }
        else
        {
          payload_data_ptr = qcril_uim_sap_create_payload(data_ptr, data_len);
        }
        resp.apduResponse = (pb_bytes_array_t*)payload_data_ptr;

        /* Encode the message. For variable pb byte array, we need to
        first find the encoded size */
        if (!pb_get_encoded_size(&encoded_size,
                                 RIL_SIM_SAP_APDU_RSP_fields,
                                 &resp))
        {
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_LOG_ERROR("%s","Error in pb_get_encoded_size");
          return;
        }

        response_ptr = qcril_malloc(encoded_size);
        if (response_ptr == NULL)
        {
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(response_ptr, 0, encoded_size);
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, encoded_size);
        if (!pb_encode(&stream, RIL_SIM_SAP_APDU_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_APDU_RSP");
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
      {
        RIL_SIM_SAP_TRANSFER_ATR_RSP resp;

        if (data_len == 0 ||
            data_ptr == NULL)
        {
          payload_data_ptr = NULL;
        }
        else
        {
          payload_data_ptr = qcril_uim_sap_create_payload(data_ptr, data_len);
        }

        /* Update the outgoing message */
        memset(&resp, 0, sizeof(RIL_SIM_SAP_TRANSFER_ATR_RSP));
        resp.atr = (pb_bytes_array_t*)payload_data_ptr;
        resp.response = (RIL_SIM_SAP_TRANSFER_ATR_RSP_Response)qcril_uim_sap_convert_err_to_proto_err(message_id, err_code);

        /* Encode the message. For variable pb byte array, we need to
        first find the encoded size */
        if (!pb_get_encoded_size(&encoded_size,
                                 RIL_SIM_SAP_TRANSFER_ATR_RSP_fields,
                                 &resp))
        {
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_LOG_ERROR("%s","Error in pb_get_encoded_size");
          return;
        }

        response_ptr = qcril_malloc(encoded_size);
        if (response_ptr == NULL)
        {
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(response_ptr, 0, encoded_size);
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, encoded_size);
        if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_ATR_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_TRANSFER_ATR_RSP");
          QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case MsgId_RIL_SIM_SAP_POWER:
      {
        RIL_SIM_SAP_POWER_RSP resp;

        response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_POWER_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(&resp, 0, sizeof(RIL_SIM_SAP_POWER_RSP));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_POWER_RSP));

        resp.response = (RIL_SIM_SAP_POWER_RSP_Response)qcril_uim_sap_convert_err_to_proto_err(message_id, err_code);

        /* Encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_POWER_RSP));
        if (!pb_encode(&stream, RIL_SIM_SAP_POWER_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_POWER_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case MsgId_RIL_SIM_SAP_RESET_SIM:
      {
        RIL_SIM_SAP_RESET_SIM_RSP resp;

        response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_RESET_SIM_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(&resp, 0, sizeof(RIL_SIM_SAP_RESET_SIM_RSP));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_RESET_SIM_RSP));

        resp.response = (RIL_SIM_SAP_RESET_SIM_RSP_Response)qcril_uim_sap_convert_err_to_proto_err(message_id, err_code);

        /* Encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_RESET_SIM_RSP));
        if (!pb_encode(&stream, RIL_SIM_SAP_RESET_SIM_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_RESET_SIM_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      {
        RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP resp;

        response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP));
        memset(&resp, 0, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP));
        resp.response = (RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response)qcril_uim_sap_convert_err_to_proto_err(message_id, err_code);

        if ((data_ptr != NULL) &&
            (data_len > 0) &&
            (data_len <= sizeof(int32_t)))
        {
          resp.has_CardReaderStatus = TRUE;
          memcpy(&resp.CardReaderStatus, data_ptr, data_len);
        }

        /* Encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP));
        if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;

    case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:
      {
        RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP resp;

        response_ptr = qcril_malloc(sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP));
        if (response_ptr == NULL)
        {
          QCRIL_LOG_ERROR("%s","Error allocating response_ptr");
          return;
        }

        memset(&resp, 0, sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP));
        memset(response_ptr, 0, sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP));

        resp.response = RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_Response_RIL_E_GENERIC_FAILURE;

        /* Encode the outgoing message */
        stream = pb_ostream_from_buffer((uint8_t *)response_ptr, sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP));
        if (!pb_encode(&stream, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_fields, &resp))
        {
          QCRIL_LOG_ERROR("%s","Error encoding RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP");
          QCRIL_UIM_SAP_FREE_PTR(response_ptr);
          return;
        }
        response_len = stream.bytes_written;
      }
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported MsgId: 0x%x", message_id);
      return;
  }

  qcril_sap_response_api->OnRequestComplete((RIL_Token)token,
                                            ril_err,
                                            response_ptr,
                                            response_len);

  /* Send the UNSOL Response if needed */
  if ((unsol_response_ptr != NULL) && (unsol_response_len != 0))
  {
    /* Send the data using OnUnsolicitedResponse */
    qcril_sap_response_api->OnUnsolicitedResponse(message_id,
                                                  unsol_response_ptr,
                                                  unsol_response_len);
  }

  /* Free up any memory allocated */
  QCRIL_UIM_SAP_FREE_PTR(payload_data_ptr);
  QCRIL_UIM_SAP_FREE_PTR(response_ptr);
  QCRIL_UIM_SAP_FREE_PTR(unsol_response_ptr);
} /* qcril_uim_sap_qmi_handle_atel_sap_request_resp */


/*=========================================================================

FUNCTION:  qcril_uim_sap_handle_qmi_sap_request

===========================================================================*/
/*!
    @brief
    Handler function to send QMI_UIM_SAP_REQUEST to the modem.

    @return
    Boolean indicating the outcome of the request.
*/
/*=========================================================================*/
static boolean qcril_uim_sap_handle_qmi_sap_request
(
  RIL_Token                    token,
  MsgId                        message_id,
  uint8                      * payload_ptr,
  size_t                       payload_len,
  int                        * error_code
)
{
  int                                 qmi_result       = 0;
  qmi_uim_slot_type                   qmi_slot         = QMI_UIM_SLOT_1;
  void                              * message_ptr      = NULL;
  qmi_uim_sap_request_params_type     sap_request_params;

  if (error_code == NULL ||
      !qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot))
  {
    return FALSE;
  }

  /* Unpack incoming data based on the message id */
  message_ptr = qcril_uim_sap_unpack_message(message_id, payload_ptr, payload_len);
  if (message_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Failed to unpack payload, message_id: 0x%x", message_id);
    return FALSE;
  }

  memset(&sap_request_params, 0, sizeof(qmi_uim_sap_request_params_type));

  /* Update QMI parameters from protobuf request & dispatch it to modem */
  sap_request_params.slot = qmi_slot;

  switch (qcril_uim_sap_get_sim_state())
  {
    case QCRIL_CARD_STATUS_UNKNOWN:
    case QCRIL_CARD_STATUS_NOT_APPLICABLE:
    case QCRIL_CARD_STATUS_NOT_ACCESSIBLE:
      *error_code = QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE;
      QCRIL_UIM_SAP_FREE_PTR(message_ptr);
      return FALSE;
    case QCRIL_CARD_STATUS_ABSENT:
      *error_code = QCRIL_UIM_SAP_CARD_REMOVED;
      QCRIL_UIM_SAP_FREE_PTR(message_ptr);
      return FALSE;
    case QCRIL_CARD_STATUS_DOWN:
      /* Only POWER UP request works in power down state */
      if (message_id == MsgId_RIL_SIM_SAP_POWER &&
          ((RIL_SIM_SAP_POWER_REQ *)message_ptr)->state)
      {
        break;
      }
      *error_code = QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF;
      QCRIL_UIM_SAP_FREE_PTR(message_ptr);
      return FALSE;
    case QCRIL_CARD_STATUS_UP:
      /* Only POWER UP request fails in power up state */
      if (message_id == MsgId_RIL_SIM_SAP_POWER &&
          ((RIL_SIM_SAP_POWER_REQ *)message_ptr)->state)
      {
        *error_code = QCRIL_UIM_SAP_CARD_ALREADY_POWERED_ON;
        QCRIL_UIM_SAP_FREE_PTR(message_ptr);
        return FALSE;
      }
      break;
    default:
      break;
  }


  switch (message_id)
  {
    case MsgId_RIL_SIM_SAP_APDU:
      if (((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command == NULL)
      {
        QCRIL_LOG_ERROR( "%s", "Null apdu ptr, cannot proceed");
        goto send_error;
      }

      if ((((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command->size <= 0) ||
          (((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command->size > QMI_UIM_MAX_APDU_DATA))
      {
        QCRIL_LOG_ERROR("invalid command->size: 0x%x, cannot proceed",
                        ((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command->size);
        goto send_error;
      }

      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_SEND_APDU;
      sap_request_params.apdu.data_len = ((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command->size;
      sap_request_params.apdu.data_ptr = ((RIL_SIM_SAP_APDU_REQ *)message_ptr)->command->bytes;
      break;

    case MsgId_RIL_SIM_SAP_POWER:
      if (((RIL_SIM_SAP_POWER_REQ *)message_ptr)->state)
      {
        sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_POWER_SIM_ON;
        qcril_uim_sap_set_sim_state(QCRIL_CARD_STATUS_UP);
      }
      else
      {
        sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_POWER_SIM_OFF;
        qcril_uim_sap_set_sim_state(QCRIL_CARD_STATUS_DOWN);
      }
      break;

    case MsgId_RIL_SIM_SAP_RESET_SIM:
      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_RESET_SIM;
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_GET_ATR;
      break;

    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_READER_STATUS;
      break;

    default:
      QCRIL_LOG_ERROR( "Unspported message_id: 0x%x\n", message_id);
      goto send_error;
  }

  qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_ATEL,
                                          QCRIL_UIM_SAP_REQUEST_REQ,
                                          (int)message_id,
                                          token,
                                          &sap_request_params);

  if (qmi_result >= 0)
  {
    QCRIL_UIM_SAP_FREE_PTR(message_ptr);
    return TRUE;
  }

  QCRIL_LOG_ERROR("SAP request failed, qmi_result: 0x%x", qmi_result);

  send_error:
    /* Clear allocated message pointer */
    QCRIL_UIM_SAP_FREE_PTR(message_ptr);
    return FALSE;
} /* qcril_uim_sap_handle_qmi_sap_request */


/*=========================================================================

FUNCTION:  qcril_uim_sap_handle_qmi_sap_connection

===========================================================================*/
/*!
    @brief
    Handler function to send QMI_UIM_SAP_CONNECTION to the modem.

    @return
    Boolean indicating the outcome of the request.
*/
/*=========================================================================*/
static boolean qcril_uim_sap_handle_qmi_sap_connection
(
  RIL_Token                    token,
  MsgId                        message_id,
  uint8                      * payload_ptr,
  size_t                       payload_len
)
{
  int                                   qmi_result       = 0;
  qmi_uim_slot_type                     qmi_slot         = QMI_UIM_SLOT_1;
  void                                * message_ptr      = NULL;
  qmi_uim_sap_connection_params_type    sap_request_params;
  uint8                                 active_client_id = qcril_uim_sap_get_active_client();

  /* Reject SAP connect request if there is already an active client */
  if ((message_id       == MsgId_RIL_SIM_SAP_CONNECT) &&
      (active_client_id != QCRIL_UIM_SAP_MAX_CLIENTS))
  {
    QCRIL_LOG_ERROR("Rejecting SAP connect req, active_client_id: 0x%x", active_client_id);
    return FALSE;
  }

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot))
  {
    return FALSE;
  }

  /* Unpack incoming data based on the message id */
  message_ptr = qcril_uim_sap_unpack_message(message_id, payload_ptr, payload_len);
  if (message_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Failed to unpack payload, message_id: 0x%x", message_id);
    return FALSE;
  }

  memset(&sap_request_params, 0, sizeof(qmi_uim_sap_connection_params_type));

  /* Update QMI parameters from protobuf request & dispatch it to modem */
  sap_request_params.slot           = qmi_slot;
  sap_request_params.conn_condition = QMI_UIM_SAP_CONN_COND_BLOCK_VOICE;

  if (message_id == MsgId_RIL_SIM_SAP_CONNECT)
  {
    sap_request_params.operation_type = QMI_UIM_SAP_CONNECTION_CONNECT;
  }
  else if (message_id == MsgId_RIL_SIM_SAP_DISCONNECT)
  {
    /* Note - for disconnect req, there is no mode passed from client */
    sap_request_params.operation_type  = QMI_UIM_SAP_CONNECTION_DISCONNECT;
    sap_request_params.disconnect_mode = QMI_UIM_SAP_DISCONNECT_MODE_GRACEFUL;
  }
  else
  {
    QCRIL_LOG_ERROR( "Unspported message_id: 0x%x\n", message_id);
    QCRIL_UIM_SAP_FREE_PTR(message_ptr);
    return FALSE;
  }

  QCRIL_UIM_SAP_FREE_PTR(message_ptr);

  qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_ATEL,
                                          QCRIL_UIM_SAP_CONNECTION_REQ,
                                          (int)message_id,
                                          token,
                                          &sap_request_params);

  if (qmi_result < 0)
  {
    QCRIL_LOG_ERROR("SAP connection request failed, qmi_result: 0x%x", qmi_result);
    return FALSE;
  }
  qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_CLIENT_ID_ATEL);

  return TRUE;
} /* qcril_uim_sap_handle_qmi_sap_connection */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_process_request

===========================================================================*/
/*!
    @brief
    This is the SAP request handling in RIL event thread context upon
    receiving QCRIL_EVT_INTERNAL_UIM_SAP_REQ.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_process_request
(
  qcril_uim_sap_atel_request * req_ptr,
  RIL_Token                    token
)
{
  int                          err_code = QCRIL_UIM_SAP_GENERIC_FAILURE;
  boolean                      result   = FALSE;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Invalid input, NULL request_ptr");
    return;
  }

  switch (req_ptr->request_id)
  {
    case MsgId_RIL_SIM_SAP_CONNECT:
    case MsgId_RIL_SIM_SAP_DISCONNECT:
      err_code = QCRIL_UIM_SAP_CONNECT_FAILURE;
      result = qcril_uim_sap_handle_qmi_sap_connection(token,
                                                       (MsgId)req_ptr->request_id,
                                                       req_ptr->data_ptr,
                                                       req_ptr->data_len);
      break;

    case MsgId_RIL_SIM_SAP_APDU:
    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
    case MsgId_RIL_SIM_SAP_POWER:
    case MsgId_RIL_SIM_SAP_RESET_SIM:
    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      result = qcril_uim_sap_handle_qmi_sap_request(token,
                                                    (MsgId)req_ptr->request_id,
                                                    req_ptr->data_ptr,
                                                    req_ptr->data_len,
                                                    &err_code);
      break;

    default:
      err_code = QCRIL_UIM_SAP_NOT_SUPPORTED;
      QCRIL_LOG_ERROR("Unhandled message, id: 0x%x", req_ptr->request_id);
      break;
  }

  if (result == FALSE)
  {
    qcril_uim_sap_qmi_handle_atel_sap_request_resp(req_ptr->request_id,
                                                   err_code,
                                                   -1,
                                                   token,
                                                   NULL,
                                                   0);
  }
  QCRIL_UIM_SAP_FREE_PTR(req_ptr->data_ptr);
  QCRIL_UIM_SAP_FREE_PTR(req_ptr);
} /* qcril_uim_sap_process_request */


/*===========================================================================

                      QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION:  qcril_uim_sap_on_request_atel

===========================================================================*/
/*!
    @brief
    This is the QCRIL UIM SAP request callback implementation that was
    reported earlier via the RIL_UIM_SAP_Init API. It corresponds to onRequest
    field in RIL_RadioFunctions. It is called from RILD to make various SAP
    related requests that come as part of the SAP specific socket.
    RIL_onRequestComplete() may be called from any thread, before or after
    this function returns. Returning from this routine implies QCRIL UIM SAP
    is ready to process another command asynchronously.

    @return
    None.

*/
/*=========================================================================*/
static void qcril_uim_sap_on_request_atel
(
  int        request,
  void     * data_ptr,
  size_t     data_len,
  RIL_Token  token
)
{
  boolean                     result      = FALSE;
  qcril_instance_id_e_type    instance_id = QCRIL_MAX_INSTANCE_ID;
  MsgId                       message_id  = (MsgId)request;
  int                         err_code    = QCRIL_UIM_SAP_GENERIC_FAILURE;
  qcril_uim_sap_atel_request *req_ptr     = NULL;

  /* Get the RILD instance ID */
  instance_id = qmi_ril_get_process_instance_id();
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    QCRIL_LOG_ERROR("Invalid value, instance_id: 0x%x", instance_id);
    return;
  }

  QCRIL_LOG_INFO("Request details: instance_id: 0x%x, MsgId: 0x%x, packed payload length: 0x%x",
                  instance_id,
                  message_id,
                  data_len);

  /* Pass it on the main request handling. Upon successful call to the modem,
  we expect the callback to respond back with the result */
  switch (message_id)
  {
    case MsgId_RIL_SIM_SAP_CONNECT:
    case MsgId_RIL_SIM_SAP_DISCONNECT:
    case MsgId_RIL_SIM_SAP_APDU:
    case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
    case MsgId_RIL_SIM_SAP_POWER:
    case MsgId_RIL_SIM_SAP_RESET_SIM:
    case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
      req_ptr = (qcril_uim_sap_atel_request *)
                 qcril_malloc(sizeof(qcril_uim_sap_atel_request));

      if (req_ptr != NULL)
      {
        req_ptr->request_id = message_id;
        if (data_ptr != NULL && data_len != 0)
        {
          req_ptr->data_ptr = (uint8 *)qcril_malloc(data_len * sizeof(uint8));
          if (req_ptr->data_ptr != NULL)
          {
            memcpy(req_ptr->data_ptr, data_ptr, data_len);
            req_ptr->data_len = data_len;
          }
        }
        qcril_uim_sap_process_request(req_ptr, token);
        result = TRUE;
      }
      break;

    case MsgId_RIL_SIM_SAP_ERROR_RESP:
    case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:
    case MsgId_RIL_SIM_SAP_STATUS:
    default:
      err_code = QCRIL_UIM_SAP_NOT_SUPPORTED;
      break;
  }

  if (result == FALSE)
  {
    qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_ATEL,
                                    (int)message_id,
                                    token,
                                    err_code);
  }
} /* qcril_uim_sap_on_request_atel */


/*===========================================================================

FUNCTION:  qcril_uim_sap_get_version

===========================================================================*/
/*!
    @brief
    This is the QCRIL UIM SAP API that corresponds to getVersion field in
    RIL_RadioFunctions. It is called from RILD to get a version string for
    the vendor RIL implementation .

    @return
    Null terminated string for the RIL implementation.

*/
/*=========================================================================*/
static const char * qcril_uim_sap_get_version
(
  void
)
{
  static char version[] = "Qualcomm Technologies Inc. RIL UIM SAP 1.0";

  QCRIL_LOG_DEBUG( "RIL UIM SAP version %s", version );

  return version;
} /* qcril_uim_sap_get_version */


/*===========================================================================

FUNCTION:  qcril_uim_sap_on_cancel

===========================================================================*/
/*!
    @brief
    This is the QCRIL UIM SAP API that corresponds to onCancel field in
    RIL_RadioFunctions. It is called from RILD to cancel a specific request
    corresponding to the supplied token. The RIL is supposed to make a best
    effort to cancel the outstanding request, the current design is to take
    no effort & let the current command finish.

    @return
    None.

*/
/*=========================================================================*/
static void qcril_uim_sap_on_cancel
(
  RIL_Token token
)
{
  QCRIL_NOTUSED(token);

  QCRIL_LOG_DEBUG( "qcril_uim_sap_on_cancel, token: 0x%x", token);
} /* qcril_uim_sap_on_cancel */


/*===========================================================================

FUNCTION:  qcril_uim_sap_on_supports

===========================================================================*/
/*!
    @brief
    This is the QCRIL UIM SAP API that corresponds to onSupports field in
    RIL_RadioFunctions. It is called from RILD to get the status if the
    corresponding API is supported on the SAP specific socket.

    Note: request_id here corresponds to MsgId of SAP protobuf interface.

    @return
    1 if specified request code is supported or 0 if it is not.

*/
/*=========================================================================*/
static int qcril_uim_sap_on_supports
(
  int request_id
)
{
  QCRIL_LOG_DEBUG( "qcril_uim_sap_on_supports, request_id: 0x%x", request_id);
  return qcril_uim_sap_check_supported_messages(request_id);
} /* qcril_uim_sap_on_supports */


/*===========================================================================

FUNCTION:  qcril_uim_sap_on_state_request

===========================================================================*/
/*!
    @brief
    This is the QCRIL UIM SAP API that corresponds to onStateRequest
    field in RIL_RadioFunctions. It is called from RILD to get the current
    radio state on the SAP specific socket.

    @return
    RIL_RadioState.

*/
/*=========================================================================*/
static RIL_RadioState qcril_uim_sap_on_state_request
(
  void
)
{
  /* This API is not currently called by RILD for the SAP socket */
  return RADIO_STATE_ON;
} /* qcril_uim_sap_on_state_request */


/*=========================================================================

FUNCTION:  RIL_SAP_Init

===========================================================================*/
/*!
    @brief
    Initializes QMI_UIM service's SAP interface for ATEL client. It is called
    whenever RILD starts or modem restarts. The RIL_Env is stored based on an
    available entry in the client table.

    @return
    Pointer to RIL_RadioFunctions.
*/
/*=========================================================================*/
const RIL_RadioFunctions     * RIL_SAP_Init
(
  const struct RIL_Env  * env,
  int                     argc,
  char                 ** argv
)
{
  qcril_instance_id_e_type ril_instance_id = qmi_ril_get_process_instance_id();

  QCRIL_NOTUSED(argc);
  QCRIL_NOTUSED(argv);
  QCRIL_LOG_DEBUG( "RILD %d, running RIL_SAP_Init()", ril_instance_id);
  if (ril_instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    return NULL;
  }

  /* Store the pointer based on the instance_id */
  qcril_sap_response_api = (struct RIL_Env *) env;

  qcril_uim_sap_register_client(QCRIL_UIM_SAP_CLIENT_ID_ATEL,
                                qcril_uim_sap_qmi_handle_atel_sap_ind,
                                qcril_uim_sap_qmi_handle_atel_sap_request_resp,
                                qcril_uim_sap_qmi_handle_atel_card_status);

  return &qcril_sap_request_api;
} /* RIL_SAP_Init */
