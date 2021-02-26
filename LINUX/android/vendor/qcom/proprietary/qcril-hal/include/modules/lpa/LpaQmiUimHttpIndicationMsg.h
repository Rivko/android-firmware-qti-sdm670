/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/lpa/LpaUimHttpSrvc.h"


typedef struct
{
  uint32_t                  token_id;
  bool                      url_valid;
  char                      url[UIM_HTTP_URL_MAX_LEN + 1];
  bool                      headers_valid;
  uim_http_req_info_type    headers;
  uint32_t                  payload_len;
  uint8_t                  *payload_ptr;
} uim_http_transaction_ind_msg;

typedef enum
{
  QMI_UIM_HTTP_SRVC_INVALID_IND_MSG,
  QMI_UIM_HTTP_SRVC_UP_IND_MSG,
  QMI_UIM_HTTP_SRVC_DOWN_IND_MSG,
  QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG
} lpa_uim_http_ind_type;

class LpaQmiUimHttpIndicationMsg : public UnSolicitedMessage,
                                   public add_message_id<LpaQmiUimHttpIndicationMsg>
{
  private:
    void                    *mIndPtr;
    lpa_uim_http_ind_type    mIndId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.qmi_uim_http_indication";
    LpaQmiUimHttpIndicationMsg() = delete;
    ~LpaQmiUimHttpIndicationMsg();

    LpaQmiUimHttpIndicationMsg(lpa_uim_http_ind_type   ind_id,
                               void                  * ind_data_ptr);

    std::shared_ptr<UnSolicitedMessage> clone();

    inline string dump()
    {
      return mName + "{}";
    }

    inline void* get_message()
    {
      return mIndPtr;
    }

    inline uint32_t get_ind_id(void)
    {
      return mIndId;
    }
};
