/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/lpa/LpaUimHttpRequestMsg.h"
#include "modules/lpa/lpa_service_types.h"

LpaUimHttpRequestMsg::~LpaUimHttpRequestMsg()
{
  switch(mMsgId)
  {
    case LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED:
      {
        uim_http_transaction_msg * http_ptr = (uim_http_transaction_msg *)mMsgPtr;

        if (http_ptr != nullptr && http_ptr->payload_ptr != nullptr)
        {
          delete[] http_ptr->payload_ptr;
          http_ptr->payload_ptr = nullptr;
        }

        if (http_ptr != nullptr)
        {
          delete http_ptr;
          http_ptr = nullptr;
        }
      }
      break;
    default:
      break;
  }

  mMsgPtr = nullptr;
  mTokenId = 0;
  setCallback(nullptr);
}


LpaUimHttpRequestMsg::LpaUimHttpRequestMsg(void *dataPtr, int token,
                                           lpaQmiUimHttpRequestType req_id,
                                           GenericCallback<lpa_qmi_uim_http_rsp_data_type> *callback) :
                                           SolicitedMessage<lpa_qmi_uim_http_rsp_data_type>(get_class_message_id())
{
  mMsgPtr = nullptr;
  mMsgId  = req_id;

  if (dataPtr != nullptr)
  {
    switch(req_id)
    {
      case LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED:
        {
          uim_http_transaction_msg                * qmi_http_req_ptr  = nullptr;
          lpa_service_http_transaction_req_type   * http_req_ptr =
                               (lpa_service_http_transaction_req_type *)dataPtr;

          qmi_http_req_ptr = new uim_http_transaction_msg;

          if (qmi_http_req_ptr == nullptr)
          {
            return;
          }

          memset(qmi_http_req_ptr, 0x00, sizeof(uim_http_transaction_msg));

          if (http_req_ptr->result == LPA_SERVICE_RESULT_SUCCESS)
          {
            qmi_http_req_ptr->result = UIM_HTTP_TRANSACTION_SUCCESSFUL;
          }
          else
          {
            qmi_http_req_ptr->result = UIM_HTTP_UNKNOWN_ERROR;
          }

          qmi_http_req_ptr->token_id = (uint32_t)http_req_ptr->tokenId;

          if (http_req_ptr->no_of_headers > 0)
          {
            uint32_t     i = 0;

            qmi_http_req_ptr->headers_valid = true;
            qmi_http_req_ptr->headers_len = (http_req_ptr->no_of_headers < UIM_HTTP_CUST_HEADER_MAX_COUNT) ?
                                              http_req_ptr->no_of_headers : UIM_HTTP_CUST_HEADER_MAX_COUNT;

            for (i = 0; i < http_req_ptr->no_of_headers && i < UIM_HTTP_CUST_HEADER_MAX_COUNT; i++)
            {
              if (http_req_ptr->customHeaders[i].headerName != nullptr)
              {
                strlcpy(qmi_http_req_ptr->headers[i].name,
                        http_req_ptr->customHeaders[i].headerName,
                        UIM_HTTP_HEADER_NAME_MAX_LEN + 1);
              }

              if (http_req_ptr->customHeaders[i].headerValue != nullptr)
              {
                strlcpy(qmi_http_req_ptr->headers[i].value,
                        http_req_ptr->customHeaders[i].headerValue,
                        UIM_HTTP_HEADER_NAME_MAX_LEN + 1);
              }
            }
          }

          if (http_req_ptr->payload != nullptr &&
              http_req_ptr->payload_len != 0)
          {
            qmi_http_req_ptr->payload_ptr = new uint8_t[http_req_ptr->payload_len] {0};
            if (qmi_http_req_ptr->payload_ptr != nullptr)
            {
              memcpy(qmi_http_req_ptr->payload_ptr, http_req_ptr->payload, http_req_ptr->payload_len);
              qmi_http_req_ptr->payload_len = http_req_ptr->payload_len;
            }
          }

          mMsgPtr = (void *)qmi_http_req_ptr;
        }
        break;
      default:
        break;
    }
  }
  mName = MESSAGE_NAME;
  mTokenId = token;
  setCallback(callback);
}


string LpaUimHttpRequestMsg::dump()
{
  return mName + "{ "
    + "req_id = " + std::to_string(mMsgId)
    + " }";
} /* LpaUserReqMessage::dump */
