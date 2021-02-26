/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/lpa/LpaUimHttpSrvc.h"

typedef struct
{
  uim_http_result_type_enum result;
  uint32_t                  token_id;
  bool                      headers_valid;
  uint32_t                  headers_len;
  uim_http_header_type      headers[UIM_HTTP_CUST_HEADER_MAX_COUNT];
  uint32_t                  payload_len;
  uint8_t                  *payload_ptr;
  uint32_t                  offset;
} uim_http_transaction_msg;

typedef enum
{
  LPA_QMI_UIM_HTTP_REQUEST_INVALID,
  LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED
} lpaQmiUimHttpRequestType;

class LpaUimHttpRequestMsg : public SolicitedMessage<lpa_qmi_uim_http_rsp_data_type>,
                             public add_message_id<LpaUimHttpRequestMsg>
{
  private:
    int                        mTokenId;
    lpaQmiUimHttpRequestType   mMsgId;
    void                     * mMsgPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.qmi_uim_http_request";
    LpaUimHttpRequestMsg() = delete;
    ~LpaUimHttpRequestMsg();

    LpaUimHttpRequestMsg(void *dataPtr, int clientToken,
                         lpaQmiUimHttpRequestType req_id,
                         GenericCallback<lpa_qmi_uim_http_rsp_data_type> *callback);

    inline int get_token(void)
    {
      return mTokenId;
    }

    inline void * get_message()
    {
      return mMsgPtr;
    }

    inline uint32_t get_msg_id(void)
    {
      return mMsgId;
    }

    string dump();
};
