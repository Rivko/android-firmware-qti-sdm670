/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/SolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/add_message_id.h>

typedef enum
{
  UIM_LPA_UNKOWN_RESP_ID,
  UIM_LPA_USER_RESP_ID
} uimLpaResponseType;

typedef struct
{
  uimLpaResponseType    resp_id;
  void                * data_ptr;
} qcril_uim_lpa_response_type;

class UimLpaRespMessage : public SolicitedMessage<void>,
                          public add_message_id<UimLpaRespMessage>
{
  private:
    uimLpaResponseType    mRespId;
    void                * mDataPtr;
    int                   mToken;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.lpa_reponse";

    UimLpaRespMessage() = delete;

    UimLpaRespMessage(uimLpaResponseType        resp_id,
                      void                    * data_ptr,
                      int                       token);

    int get_token()
    {
      return mToken;
    }

    uimLpaResponseType get_response_id()
    {
      return mRespId;
    }

    void * get_message_ptr()
    {
      return mDataPtr;
    }

    ~UimLpaRespMessage();

    string dump();
};
