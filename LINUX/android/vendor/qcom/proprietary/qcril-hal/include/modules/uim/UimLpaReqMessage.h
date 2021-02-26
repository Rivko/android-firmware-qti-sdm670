/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once


#include <framework/SolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/add_message_id.h>
#include "modules/uim/UimLpaRespMessage.h"

typedef enum
{
  UIM_LPA_UNKOWN_REQUEST_ID,
  UIM_LPA_USER_REQUEST_ID
} uimLpaRequestType;

class UimLpaReqMessage : public SolicitedMessage<qcril_uim_lpa_response_type>,
                         public add_message_id<UimLpaReqMessage>
{
  private:
    uimLpaRequestType     mReqId;
    void                * mDataPtr;
    int                   mToken;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.lpa_request";

    UimLpaReqMessage() = delete;

    UimLpaReqMessage(uimLpaRequestType                              mReqId,
                     GenericCallback<qcril_uim_lpa_response_type> * callback,
                     void                                         * data_ptr,
                     int                                            token);

    inline uimLpaRequestType get_req_id()
    {
      return mReqId;
    }

    inline void * get_data_ptr()
    {
      return mDataPtr;
    }

    inline int get_token()
    {
      return mToken;
    }

    ~UimLpaReqMessage();

    string dump();
};
