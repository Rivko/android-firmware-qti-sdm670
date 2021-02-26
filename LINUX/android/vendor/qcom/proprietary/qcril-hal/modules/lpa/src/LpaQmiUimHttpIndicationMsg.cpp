/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/lpa/LpaQmiUimHttpIndicationMsg.h"


/*---------------------------------------------------------------------------
  Class functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  LpaQmiUimHttpIndicationMsg - LpaQmiUimHttpIndicationMsg Destructor
===========================================================================*/
LpaQmiUimHttpIndicationMsg::~LpaQmiUimHttpIndicationMsg()
{
  switch(mIndId)
  {
    case QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG:
      if (mIndPtr != nullptr)
      {
        uim_http_transaction_ind_msg *ind_ptr = (uim_http_transaction_ind_msg *)mIndPtr;

        if (ind_ptr->payload_ptr != nullptr)
        {
          delete[] ind_ptr->payload_ptr;
          ind_ptr->payload_ptr = nullptr;
        }

        delete ind_ptr;
        mIndPtr = nullptr;
      }
      break;

    default:
      break;
  }
} /* LpaQmiUimHttpIndicationMsg::~LpaQmiUimHttpIndicationMsg */


/*===========================================================================
  FUNCTION  LpaQmiUimHttpIndicationMsg - LpaQmiUimHttpIndicationMsg Constructor
===========================================================================*/
LpaQmiUimHttpIndicationMsg::LpaQmiUimHttpIndicationMsg
(
  lpa_uim_http_ind_type          ind_id,
  void                         * ind_data_ptr
) : UnSolicitedMessage(get_class_message_id())
{
  mIndPtr   = nullptr;
  mName     = MESSAGE_NAME;
  mIndId    = ind_id;

  switch(ind_id)
  {
    case QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG:
      if (ind_data_ptr != nullptr)
      {
        uim_http_transaction_ind_msg  *http_ind_ptr = (uim_http_transaction_ind_msg *)ind_data_ptr;
        uim_http_transaction_ind_msg  *out_ptr      = nullptr;

        out_ptr = new uim_http_transaction_ind_msg;
        if (out_ptr != nullptr)
        {
          memcpy(out_ptr, http_ind_ptr, sizeof(uim_http_transaction_ind_msg));
          
          if (http_ind_ptr->payload_len != 0 &&
              http_ind_ptr->payload_ptr != NULL)
          {
            out_ptr->payload_ptr = new uint8_t[http_ind_ptr->payload_len];

            if (out_ptr->payload_ptr != nullptr)
            {
              memcpy(out_ptr->payload_ptr, http_ind_ptr->payload_ptr, http_ind_ptr->payload_len);
              out_ptr->payload_len = http_ind_ptr->payload_len;
            }
            else
            {
              out_ptr->payload_len = 0;
            }
          }
        }
        mIndPtr = (void *)out_ptr;
      }
      break;
    default:
      break;
  }
} /* LpaQmiUimHttpIndicationMsg::LpaQmiUimHttpIndicationMsg */


/*===========================================================================
  FUNCTION  LpaQmiUimHttpIndicationMsg - clone function
===========================================================================*/
std::shared_ptr<UnSolicitedMessage> LpaQmiUimHttpIndicationMsg::clone()
{
  return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<LpaQmiUimHttpIndicationMsg>(mIndId, (void *)mIndPtr));
} /* LpaQmiUimHttpIndicationMsg::clone() */
