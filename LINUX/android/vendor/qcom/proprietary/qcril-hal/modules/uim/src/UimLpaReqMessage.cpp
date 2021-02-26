/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/uim/UimLpaReqMessage.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/lpa/lpa_service_types.h"

UimLpaReqMessage::UimLpaReqMessage
(
  uimLpaRequestType                                  req_id,
  GenericCallback<qcril_uim_lpa_response_type>     * callback,
  void                                             * data_ptr,
  int                                                token
) : SolicitedMessage<qcril_uim_lpa_response_type>(get_class_message_id())
{
  mName    = MESSAGE_NAME;
  mReqId   = req_id;
  mToken   = token;
  mDataPtr = nullptr;

  switch (req_id)
  {
    case UIM_LPA_USER_REQUEST_ID:
      if (data_ptr != nullptr)
      {
        lpa_service_user_req_type   * usr_req_ptr = (lpa_service_user_req_type *)data_ptr;
        qcril_uim_lpa_user_req_type * out_ptr     = nullptr;

        out_ptr = new qcril_uim_lpa_user_req_type;
        if (out_ptr != nullptr)
        {
          memset(out_ptr, 0x00, sizeof(qcril_uim_lpa_user_req_type));

          out_ptr->event = (qcril_uim_lpa_user_req_event_type)usr_req_ptr->event;

          if (usr_req_ptr->activationCode != nullptr)
          {
            uint32_t len = strlen(usr_req_ptr->activationCode);

            out_ptr->activationCode = new char[len + 1]{0};
            if (out_ptr->activationCode != nullptr)
            {
              strlcpy(out_ptr->activationCode, usr_req_ptr->activationCode, len + 1);
            }
          }

          if (usr_req_ptr->confirmationCode != nullptr)
          {
            uint32_t len = strlen(usr_req_ptr->confirmationCode);

            out_ptr->confirmationCode = new char[len + 1]{0};
            if (out_ptr->confirmationCode != nullptr)
            {
              strlcpy(out_ptr->confirmationCode, usr_req_ptr->confirmationCode, len + 1);
            }
          }

          if (usr_req_ptr->nickname != nullptr)
          {
            uint32_t len = strlen(usr_req_ptr->nickname);

            out_ptr->nickname = new char[len + 1]{0};
            if (out_ptr->nickname != nullptr)
            {
              strlcpy(out_ptr->nickname, usr_req_ptr->nickname, len + 1);
            }
          }

          out_ptr->iccid_len = 0;
          if (usr_req_ptr->iccid != nullptr && usr_req_ptr->iccid_len != 0)
          {
            out_ptr->iccid = new uint8_t[usr_req_ptr->iccid_len]{0};
            if (out_ptr->iccid != nullptr)
            {
              memcpy(out_ptr->iccid, usr_req_ptr->iccid, usr_req_ptr->iccid_len);
              out_ptr->iccid_len = usr_req_ptr->iccid_len;
            }
          }

          out_ptr->resetMask = usr_req_ptr->resetMask;
          out_ptr->userOk = usr_req_ptr->userOk;
          out_ptr->srvOpReq.opCode = (qcril_uim_lpa_srv_addr_op_type)usr_req_ptr->srvOpReq.opCode;

          if (usr_req_ptr->srvOpReq.smdpAddress != nullptr)
          {
            uint32_t len = strlen(usr_req_ptr->srvOpReq.smdpAddress);

            out_ptr->srvOpReq.smdpAddress = new char[len + 1]{0};
            if (out_ptr->srvOpReq.smdpAddress != nullptr)
            {
              strlcpy(out_ptr->srvOpReq.smdpAddress, usr_req_ptr->srvOpReq.smdpAddress, len + 1);
            }
          }
        }
        mDataPtr = (void *)out_ptr;
      }
      break;

    default:
      break;
  }
  setCallback(callback);
} /* UimLpaReqMessage::UimLpaReqMessage */


UimLpaReqMessage::~UimLpaReqMessage()
{
  switch (mReqId)
  {
    case UIM_LPA_USER_REQUEST_ID:
      if (mDataPtr != nullptr)
      {
        qcril_uim_lpa_user_req_type * out_ptr = (qcril_uim_lpa_user_req_type *)mDataPtr;

        if (out_ptr->activationCode != nullptr)
        {
          delete[] out_ptr->activationCode;
          out_ptr->activationCode = nullptr;
        }

        if (out_ptr->confirmationCode != nullptr)
        {
          delete[] out_ptr->confirmationCode;
          out_ptr->confirmationCode = nullptr;
        }

        if (out_ptr->nickname != nullptr)
        {
          delete[] out_ptr->nickname;
          out_ptr->nickname = nullptr;
        }

        if (out_ptr->iccid != nullptr)
        {
          delete[] out_ptr->iccid;
          out_ptr->iccid = nullptr;
        }

        if (out_ptr->srvOpReq.smdpAddress != nullptr)
        {
          delete[] out_ptr->srvOpReq.smdpAddress;
          out_ptr->srvOpReq.smdpAddress = nullptr;
        }

        delete out_ptr;
        out_ptr = nullptr;
      }
      break;
    default:
      break;
  }
  mDataPtr = nullptr;
  setCallback(nullptr);
} /* UimLpaReqMessage::~UimLpaReqMessage */


string UimLpaReqMessage::dump()
{
  return mName + "{ "
    + "req_id = " + std::to_string(mReqId)
    + "token = " + std::to_string(mToken)
    + " }";
} /* LpaUserReqMessage::dump */
