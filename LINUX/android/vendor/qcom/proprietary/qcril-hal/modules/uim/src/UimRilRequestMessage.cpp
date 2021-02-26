/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include <telephony/ril.h>
#include <framework/Log.h>
#include "modules/uim/Uimi.h"
#include "modules/uim/UimRilRequestMessage.h"

#define TAG "UimRilRequestMessage"

#define UIM_MAX_CHAR_DATA  3

typedef union
{
  char                     * sim_data[UIM_MAX_CHAR_DATA];
  int                      * sim_int_ptr;
  RIL_SIM_IO_v6            * sim_io_ptr;
  RIL_SIM_APDU             * sim_apdu_ptr;
  RIL_SimAuthentication    * sim_auth_ptr;
  RIL_OpenChannelParams    * sim_open_ch_ptr;
} uim_ril_req_params;


bool UimRilRequestMessage::deep_copy
(
  int req_id,
  const void *payload,
  size_t payload_len,
  void **data
)
{
  uim_ril_req_params  *req_ptr = nullptr;

  (void)payload_len;

  QCRIL_LOG_INFO( "req_id : %d\n", req_id);

  if (data == nullptr)
  {
    return false;
  }

  if (payload != nullptr)
  {
    req_ptr = new uim_ril_req_params;
    if (req_ptr != nullptr)
    {
      mRilDataPtr = (void *)req_ptr;
      memset(req_ptr, 0x00, sizeof(uim_ril_req_params));
    }
    else
    {
      return false;
    }
  }

  switch (req_id)
  {
    case RIL_REQUEST_GET_SIM_STATUS:
      *data = req_ptr;
      break;

    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PIN2:
      if (req_ptr != nullptr)
      {
        char  ** in_ptr  = (char **)(payload);

        *data = (void *)req_ptr->sim_data;
        if (in_ptr[0] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[0] %s\n", in_ptr[0]);
          req_ptr->sim_data[0] = new char[strlen(in_ptr[0]) + 1]{0};

          if (req_ptr->sim_data[0] != nullptr)
          {
            memcpy(req_ptr->sim_data[0], in_ptr[0], strlen(in_ptr[0]));
          }
        }

        QCRIL_LOG_INFO( "in_ptr[1] %p\n", in_ptr[1]);
        if (in_ptr[1] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[1] %s\n", in_ptr[1]);
          req_ptr->sim_data[1] = new char[strlen(in_ptr[1]) + 1]{0};

          if (req_ptr->sim_data[1] != nullptr)
          {
            memcpy(req_ptr->sim_data[1], in_ptr[1], strlen(in_ptr[1]));
          }
        }
      }
      break;

    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PUK2:
    case RIL_REQUEST_CHANGE_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
      if (req_ptr != nullptr)
      {
        char  ** in_ptr  = (char **)(payload);

        *data = (void *)req_ptr->sim_data;
        if (in_ptr[0] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[0] %s\n", in_ptr[0]);
          req_ptr->sim_data[0] = new char[strlen(in_ptr[0]) + 1]{0};

          if (req_ptr->sim_data[0] != nullptr)
          {
            memcpy(req_ptr->sim_data[0], in_ptr[0], strlen(in_ptr[0]));
          }
        }

        if (in_ptr[1] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[1] %s\n", in_ptr[1]);
          req_ptr->sim_data[1] = new char[strlen(in_ptr[1]) + 1]{0};

          if (req_ptr->sim_data[1] != nullptr)
          {
            memcpy(req_ptr->sim_data[1], in_ptr[1], strlen(in_ptr[1]));
          }
        }

        if (in_ptr[2] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[2] %s\n", in_ptr[2]);
          req_ptr->sim_data[2] = new char[strlen(in_ptr[2]) + 1]{0};

          if (req_ptr->sim_data[2] != nullptr)
          {
            memcpy(req_ptr->sim_data[2], in_ptr[2], strlen(in_ptr[2]));
          }
        }
      }
      break;

    case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
    case RIL_REQUEST_GET_IMSI:
      if (req_ptr != nullptr)
      {
        char  ** in_ptr  = (char **)(payload);

        *data = (void *)req_ptr->sim_data;
        if (in_ptr[0] != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr[0] %s\n", in_ptr[0]);
          req_ptr->sim_data[0] = new char[strlen(in_ptr[0]) + 1]{0};

          if (req_ptr->sim_data[0] != nullptr)
          {
            memcpy(req_ptr->sim_data[0], in_ptr[0], strlen(in_ptr[0]));
          }
        }
      }
      break;

    case RIL_REQUEST_SIM_IO:
      if (payload != nullptr)
      {
        RIL_SIM_IO_v6 * io_ptr = (RIL_SIM_IO_v6 *)payload;

        req_ptr->sim_io_ptr = new RIL_SIM_IO_v6;

        if (req_ptr->sim_io_ptr == nullptr)
        {
          break;
        }

        memcpy(req_ptr->sim_io_ptr, io_ptr, sizeof(RIL_SIM_IO_v6));

        if (io_ptr->path != nullptr)
        {
          QCRIL_LOG_INFO( "path %s\n", io_ptr->path);

          req_ptr->sim_io_ptr->path = new char[strlen(io_ptr->path) + 1]{0};

          if (req_ptr->sim_io_ptr->path != nullptr)
          {
            memcpy(req_ptr->sim_io_ptr->path, io_ptr->path, strlen(io_ptr->path));
          }
        }
        if (io_ptr->data != nullptr)
        {
          QCRIL_LOG_INFO( "data %s\n", io_ptr->data);
          req_ptr->sim_io_ptr->data = new char[strlen(io_ptr->data) + 1]{0};

          if (req_ptr->sim_io_ptr->data != nullptr)
          {
            memcpy(req_ptr->sim_io_ptr->data, io_ptr->data, strlen(io_ptr->data));
          }
        }
        if (io_ptr->pin2 != nullptr)
        {
          QCRIL_LOG_INFO( "pin2 %s\n", io_ptr->pin2);
          req_ptr->sim_io_ptr->pin2 = new char[strlen(io_ptr->pin2) + 1]{0};

          if (req_ptr->sim_io_ptr->pin2 != nullptr)
          {
            memcpy(req_ptr->sim_io_ptr->pin2, io_ptr->pin2, strlen(io_ptr->pin2));
          }
        }
        if (io_ptr->aidPtr != nullptr)
        {
          QCRIL_LOG_INFO( "aidPtr %s\n", io_ptr->aidPtr);
          req_ptr->sim_io_ptr->aidPtr = new char[strlen(io_ptr->aidPtr) + 1]{0};

          if (req_ptr->sim_io_ptr->aidPtr != nullptr)
          {
            memcpy(req_ptr->sim_io_ptr->aidPtr, io_ptr->aidPtr, strlen(io_ptr->aidPtr));
          }
        }
        *data = (void *)req_ptr->sim_io_ptr;
      }
      break;

    case RIL_REQUEST_ISIM_AUTHENTICATION:
      if (req_ptr != nullptr)
      {
        char  * in_ptr  = (char *)(payload);

        if (in_ptr != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr %s\n", in_ptr);
          req_ptr->sim_data[0] = new char[strlen(in_ptr) + 1]{0};

          if (req_ptr->sim_data[0] != nullptr)
          {
            memcpy(req_ptr->sim_data[0], in_ptr, strlen(in_ptr));
          }
        }
        *data = (void *)req_ptr->sim_data[0];
      }
      break;

    case RIL_REQUEST_SIM_CLOSE_CHANNEL:
    case RIL_REQUEST_SIM_GET_ATR:
    case RIL_REQUEST_SET_SIM_CARD_POWER:
      if (req_ptr != nullptr)
      {
        int  * in_ptr  = (int *)(payload);

        if (in_ptr != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr %d\n", *in_ptr);
          req_ptr->sim_int_ptr = new int{0};

          if (req_ptr->sim_int_ptr != nullptr)
          {
            *req_ptr->sim_int_ptr = *in_ptr;
          }
        }
        *data = (void *)req_ptr->sim_int_ptr;
      }
      break;

    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
      if (req_ptr != nullptr)
      {
        RIL_SIM_APDU  * in_ptr  = (RIL_SIM_APDU *)(payload);

        if (in_ptr != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr %d\n", *in_ptr);
          req_ptr->sim_apdu_ptr = new RIL_SIM_APDU;

          if (req_ptr->sim_apdu_ptr == nullptr)
          {
            break;
          }

          memcpy(req_ptr->sim_apdu_ptr, in_ptr, sizeof(RIL_SIM_APDU));

          if (in_ptr->data != nullptr)
          {
            req_ptr->sim_apdu_ptr->data = new char[strlen(in_ptr->data) + 1]{0};

            if (req_ptr->sim_apdu_ptr->data != nullptr)
            {
              memcpy(req_ptr->sim_apdu_ptr->data, in_ptr->data, strlen(in_ptr->data));
            }
          }
        }
        *data = (void *)req_ptr->sim_apdu_ptr;
      }
      break;

    case RIL_REQUEST_SIM_AUTHENTICATION:
      if (req_ptr != nullptr)
      {
        RIL_SimAuthentication  * in_ptr  = (RIL_SimAuthentication *)(payload);

        if (in_ptr != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr %d\n", *in_ptr);
          req_ptr->sim_auth_ptr = new RIL_SimAuthentication;

          if (req_ptr->sim_auth_ptr == nullptr)
          {
            break;
          }

          memcpy(req_ptr->sim_auth_ptr, in_ptr, sizeof(RIL_SimAuthentication));

          if (in_ptr->authData != nullptr)
          {
            req_ptr->sim_auth_ptr->authData = new char[strlen(in_ptr->authData) + 1]{0};

            if (req_ptr->sim_auth_ptr->authData != nullptr)
            {
              memcpy(req_ptr->sim_auth_ptr->authData, in_ptr->authData, strlen(in_ptr->authData));
            }
          }

          if (in_ptr->aid != nullptr)
          {
            req_ptr->sim_auth_ptr->aid = new char[strlen(in_ptr->aid) + 1]{0};

            if (req_ptr->sim_auth_ptr->aid != nullptr)
            {
              memcpy(req_ptr->sim_auth_ptr->aid, in_ptr->aid, strlen(in_ptr->aid));
            }
          }
        }
        *data = (void *)req_ptr->sim_auth_ptr;
      }
      break;

    case RIL_REQUEST_SIM_OPEN_CHANNEL:
      if (req_ptr != nullptr)
      {
        RIL_OpenChannelParams  * in_ptr  = (RIL_OpenChannelParams *)(payload);

        if (in_ptr != nullptr)
        {
          QCRIL_LOG_INFO( "in_ptr %d\n", *in_ptr);
          req_ptr->sim_open_ch_ptr = new RIL_OpenChannelParams;

          if (req_ptr->sim_open_ch_ptr == nullptr)
          {
            break;
          }

          memcpy(req_ptr->sim_open_ch_ptr, in_ptr, sizeof(RIL_OpenChannelParams));

          if (in_ptr->aidPtr != nullptr)
          {
            req_ptr->sim_open_ch_ptr->aidPtr = new char[strlen(in_ptr->aidPtr) + 1]{0};

            if (req_ptr->sim_open_ch_ptr->aidPtr != nullptr)
            {
              memcpy(req_ptr->sim_open_ch_ptr->aidPtr, in_ptr->aidPtr, strlen(in_ptr->aidPtr));
            }
          }
        }
        *data = (void *)req_ptr->sim_open_ch_ptr;
      }
      break;

    default:
      break;
  }
  return true;
} /* deep_copy */


void UimRilRequestMessage::deep_clean
(
  int msg_id
)
{
  uim_ril_req_params *ril_data_ptr = (uim_ril_req_params *)mRilDataPtr;

  if (ril_data_ptr == nullptr)
  {
    return;
  }

  switch(msg_id)
  {
    case RIL_REQUEST_GET_SIM_STATUS:
      break;

    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PUK2:
    case RIL_REQUEST_CHANGE_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
    case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
    case RIL_REQUEST_GET_IMSI:
    case RIL_REQUEST_ISIM_AUTHENTICATION:
      if (ril_data_ptr->sim_data[0] != nullptr)
      {
        delete[] ril_data_ptr->sim_data[0];
      }
      if (ril_data_ptr->sim_data[1] != nullptr)
      {
        delete[] ril_data_ptr->sim_data[1];
      }
      if (ril_data_ptr->sim_data[2] != nullptr)
      {
        delete[] ril_data_ptr->sim_data[2];
      }
      break;

    case RIL_REQUEST_SIM_IO:
      if (ril_data_ptr->sim_io_ptr != nullptr)
      {
        if (ril_data_ptr->sim_io_ptr->path != nullptr)
        {
          delete ril_data_ptr->sim_io_ptr->path;
        }
        if (ril_data_ptr->sim_io_ptr->data != nullptr)
        {
          delete ril_data_ptr->sim_io_ptr->data;
        }
        if (ril_data_ptr->sim_io_ptr->pin2 != nullptr)
        {
          delete ril_data_ptr->sim_io_ptr->pin2;
        }
        if (ril_data_ptr->sim_io_ptr->aidPtr != nullptr)
        {
          delete ril_data_ptr->sim_io_ptr->aidPtr;
        }
        delete ril_data_ptr->sim_io_ptr;
      }
      break;

    case RIL_REQUEST_SIM_CLOSE_CHANNEL:
    case RIL_REQUEST_SIM_GET_ATR:
    case RIL_REQUEST_SET_SIM_CARD_POWER:
      if (ril_data_ptr->sim_int_ptr != nullptr)
      {
        delete ril_data_ptr->sim_int_ptr;
      }
      break;

    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
      if (ril_data_ptr->sim_apdu_ptr != nullptr)
      {
        if (ril_data_ptr->sim_apdu_ptr->data != nullptr)
        {
          delete ril_data_ptr->sim_apdu_ptr->data;
        }
        delete ril_data_ptr->sim_apdu_ptr;
      }
      break;

    case RIL_REQUEST_SIM_AUTHENTICATION:
      if (ril_data_ptr->sim_auth_ptr != nullptr)
      {
        if (ril_data_ptr->sim_auth_ptr->authData != nullptr)
        {
          delete ril_data_ptr->sim_auth_ptr->authData;
        }
        if (ril_data_ptr->sim_auth_ptr->aid != nullptr)
        {
          delete ril_data_ptr->sim_auth_ptr->aid;
        }
        delete ril_data_ptr->sim_auth_ptr;
      }
      break;

    case RIL_REQUEST_SIM_OPEN_CHANNEL:
      if (ril_data_ptr->sim_open_ch_ptr != nullptr)
      {
        if (ril_data_ptr->sim_open_ch_ptr->aidPtr != nullptr)
        {
          delete ril_data_ptr->sim_open_ch_ptr->aidPtr;
        }
        delete ril_data_ptr->sim_open_ch_ptr;
      }
      break;

    default:
      break;
  }

  delete (ril_data_ptr);
} /* deep clean */


/*---------------------------------------------------------------------------
  Class functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  UimRilRequestMessage - UimRilRequestMessage destructor
===========================================================================*/
UimRilRequestMessage::~UimRilRequestMessage()
{
  deep_clean(mMsgId);
} /* UimRilRequestMessage::~UimRilRequestMessage */


/*===========================================================================
  FUNCTION  UimRilRequestMessage - UimRilRequestMessage constructor
===========================================================================*/
UimRilRequestMessage::UimRilRequestMessage
(
  const ril_request_type                    *req_ptr
) : SolicitedMessage<generic_callback_payload>(get_class_message_id(), MESSAGE_EXPIRY_TIMEOUT)
{
  mName = MESSAGE_NAME;
  if (req_ptr != nullptr)
  {
    mRilDataPtr = nullptr;

    memset(&mParams, 0x00, sizeof(mParams));

    mParams.instance_id = req_ptr->instance_id;
    mParams.modem_id = QCRIL_DEFAULT_MODEM_ID;
    mParams.event_id_android = req_ptr->req_id_android;
    mParams.event_id = req_ptr->req_id;
    mParams.t = req_ptr->t;
    mToken    = req_ptr->t;
    mMsgId    = req_ptr->req_id_android;
    is_valid = deep_copy(req_ptr->req_id_android, req_ptr->payload, req_ptr->payload_len, &mParams.data);
    mParams.datalen = req_ptr->payload_len;

    QCRIL_LOG_INFO( "Request %d created : %d, payload_len : %d, data: %p\n",
                               mParams.event_id_android, is_valid,
                               req_ptr->payload_len, mParams.data); 
  }
} /* UimRilRequestMessage::UimRilRequestMessage */


string UimRilRequestMessage::dump()
{
  return mName + "{ msg_id: " + std::to_string(mParams.event_id_android) + " }";
}
