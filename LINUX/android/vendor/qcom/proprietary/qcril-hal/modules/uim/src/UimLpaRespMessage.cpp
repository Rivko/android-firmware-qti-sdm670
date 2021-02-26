/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/uim/UimLpaRespMessage.h"
#include "modules/uim/qcril_uim_lpa.h"

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_iccid_to_byte

===========================================================================*/
static uint8_t qcril_uim_lpa_iccid_to_byte
(
  uint8_t       * iccid_byte_ptr,
  uint8_t         iccid_byte_len,
  const uint8_t * iccid_ptr,
  uint8_t         len
)
{
  uint8_t  i = 0;
  uint8_t  j = 0;
  uint8_t ch = 0;

  if (iccid_ptr == NULL ||
      iccid_byte_ptr == NULL ||
      len == 0 ||
      len > QCRIL_UIM_ICCID_LEN ||
      iccid_byte_len == 0 ||
      len > 2 * QCRIL_UIM_ICCID_LEN)
  {
      return 0;
  }

  for ( i = 0; i < len && j < iccid_byte_len; i++ )
  {
    /* copy first digit */
    ch = iccid_ptr[i] & 0x0F;
    /* iccid bcd octets can have padded 0xf, need to ignore the same */
    if (0x0F == ch)
    {
      continue;
    }

    if (ch > 9)
    {
      return 0;
    }
    iccid_byte_ptr[j] = ch + '0';
    j++;

    if (j < iccid_byte_len)
    {
      /* copy second digit */
      ch = iccid_ptr[i] >> 4;
      /* iccid bcd octets can have padded 0xf, need to ignore the same */
      if (0x0F == ch)
      {
        continue;
      }

      if (ch > 9)
      {
          return 0;
      }
      iccid_byte_ptr[j] = ch + '0';
      j++;
    }
  }
  return j;
} /* qcril_uim_lpa_iccid_to_byte */


UimLpaRespMessage::UimLpaRespMessage
(
  uimLpaResponseType        resp_id,
  void                    * data_ptr,
  int                       token
) : SolicitedMessage<void>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mRespId = resp_id;
  mToken  = token;

  switch (resp_id)
  {
    case UIM_LPA_USER_RESP_ID:
      if (data_ptr != nullptr)
      {
        qcril_uim_lpa_user_resp_type * usr_resp_ptr = (qcril_uim_lpa_user_resp_type *)data_ptr;
        qcril_uim_lpa_user_resp_type * out_ptr      = nullptr;

        out_ptr = new qcril_uim_lpa_user_resp_type;
        if (out_ptr != nullptr)
        {
          memcpy(out_ptr, usr_resp_ptr, sizeof(qcril_uim_lpa_user_resp_type));

          out_ptr->eid_len = 0;
          if (usr_resp_ptr->eid != nullptr && usr_resp_ptr->eid_len != 0)
          {
            out_ptr->eid = new uint8_t[usr_resp_ptr->eid_len];

            if (out_ptr->eid != nullptr)
            {
              memcpy(out_ptr->eid, usr_resp_ptr->eid, usr_resp_ptr->eid_len);
              out_ptr->eid_len = usr_resp_ptr->eid_len;
            }
          }

          if (usr_resp_ptr->srvAddr.smdpAddress != nullptr)
          {
            uint32_t len = strlen(usr_resp_ptr->srvAddr.smdpAddress);

            out_ptr->srvAddr.smdpAddress = new char[len + 1]{0};
            if (out_ptr->srvAddr.smdpAddress != nullptr)
            {
              strlcpy(out_ptr->srvAddr.smdpAddress, usr_resp_ptr->srvAddr.smdpAddress, len + 1);
            }
          }

          if (usr_resp_ptr->srvAddr.smdsAddress != nullptr)
          {
            uint32_t len = strlen(usr_resp_ptr->srvAddr.smdsAddress);

            out_ptr->srvAddr.smdsAddress = new char[len + 1]{0};
            if (out_ptr->srvAddr.smdsAddress != nullptr)
            {
              strlcpy(out_ptr->srvAddr.smdsAddress, usr_resp_ptr->srvAddr.smdsAddress, len + 1);
            }
          }

          out_ptr->no_of_profiles = 0;
          if (usr_resp_ptr->no_of_profiles != 0 && usr_resp_ptr->profiles != nullptr)
          {
            out_ptr->profiles = new qcril_uim_lpa_profile_info_type[usr_resp_ptr->no_of_profiles];

            if (out_ptr->profiles != nullptr)
            {
              uint32_t i = 0;

              out_ptr->no_of_profiles = usr_resp_ptr->no_of_profiles;
              for (i = 0; i < usr_resp_ptr->no_of_profiles; i++)
              {
                memcpy(&out_ptr->profiles[i], &usr_resp_ptr->profiles[i], sizeof(qcril_uim_lpa_profile_info_type));

                out_ptr->profiles[i].iccid_len = 0;
                if (usr_resp_ptr->profiles[i].iccid != nullptr && usr_resp_ptr->profiles[i].iccid_len != 0)
                {
                  out_ptr->profiles[i].iccid = new uint8_t[usr_resp_ptr->profiles[i].iccid_len * 2];

                  if (out_ptr->profiles[i].iccid != nullptr)
                  {
                    out_ptr->profiles[i].iccid_len = qcril_uim_lpa_iccid_to_byte(out_ptr->profiles[i].iccid,
                                                                                 usr_resp_ptr->profiles[i].iccid_len * 2,
                                                                                 usr_resp_ptr->profiles[i].iccid,
                                                                                 usr_resp_ptr->profiles[i].iccid_len);
                  }
                }

                out_ptr->profiles[i].icon_len = 0;
                if (usr_resp_ptr->profiles[i].icon != nullptr && usr_resp_ptr->profiles[i].icon_len != 0)
                {
                  out_ptr->profiles[i].icon = new uint8_t[usr_resp_ptr->profiles[i].icon_len];

                  if (out_ptr->profiles[i].icon != nullptr)
                  {
                    memcpy(out_ptr->profiles[i].icon, usr_resp_ptr->profiles[i].icon, usr_resp_ptr->profiles[i].icon_len);
                    out_ptr->profiles[i].icon_len = usr_resp_ptr->profiles[i].icon_len;
                  }
                }

                if (usr_resp_ptr->profiles[i].profileName != nullptr)
                {
                  uint32_t len = strlen(usr_resp_ptr->profiles[i].profileName);

                  out_ptr->profiles[i].profileName = new char[len + 1]{0};
                  if (out_ptr->profiles[i].profileName != nullptr)
                  {
                    strlcpy(out_ptr->profiles[i].profileName, usr_resp_ptr->profiles[i].profileName, len + 1);
                  }
                }

                if (usr_resp_ptr->profiles[i].nickName != nullptr)
                {
                  uint32_t len = strlen(usr_resp_ptr->profiles[i].nickName);

                  out_ptr->profiles[i].nickName = new char[len + 1]{0};
                  if (out_ptr->profiles[i].nickName != nullptr)
                  {
                    strlcpy(out_ptr->profiles[i].nickName, usr_resp_ptr->profiles[i].nickName, len + 1);
                  }
                }

                if (usr_resp_ptr->profiles[i].spName != nullptr)
                {
                  uint32_t len = strlen(usr_resp_ptr->profiles[i].spName);

                  out_ptr->profiles[i].spName = new char[len + 1]{0};
                  if (out_ptr->profiles[i].spName != nullptr)
                  {
                    strlcpy(out_ptr->profiles[i].spName, usr_resp_ptr->profiles[i].spName, len + 1);
                  }
                }
              }
            }
          }
        }
        mDataPtr = (void *)out_ptr;
      }
      break;

    default:
      break;
  }
} /* UimLpaRespMessage::UimLpaRespMessage */


UimLpaRespMessage::~UimLpaRespMessage()
{
  switch (mRespId)
  {
    case UIM_LPA_USER_RESP_ID:
      if (mDataPtr != nullptr)
      {
        qcril_uim_lpa_user_resp_type * out_ptr = (qcril_uim_lpa_user_resp_type *)mDataPtr;

        if (out_ptr->eid != nullptr)
        {
          delete[] out_ptr->eid;
          out_ptr->eid = nullptr;
        }

        if (out_ptr->srvAddr.smdpAddress != nullptr)
        {
          delete[] out_ptr->srvAddr.smdpAddress;
          out_ptr->srvAddr.smdpAddress = nullptr;
        }

        if (out_ptr->srvAddr.smdsAddress != nullptr)
        {
          delete[] out_ptr->srvAddr.smdsAddress;
          out_ptr->srvAddr.smdsAddress = nullptr;
        }

        if (out_ptr->profiles != nullptr)
        {
          uint32_t i = out_ptr->no_of_profiles;

          for (i = 0; i < out_ptr->no_of_profiles; i++)
          {
            if (out_ptr->profiles[i].iccid != nullptr)
            {
              delete[] out_ptr->profiles[i].iccid;
              out_ptr->profiles[i].iccid = nullptr;
            }

            if (out_ptr->profiles[i].profileName != nullptr)
            {
              delete[] out_ptr->profiles[i].profileName;
              out_ptr->profiles[i].profileName = nullptr;
            }

            if (out_ptr->profiles[i].nickName != nullptr)
            {
              delete[] out_ptr->profiles[i].nickName;
              out_ptr->profiles[i].nickName = nullptr;
            }

            if (out_ptr->profiles[i].spName != nullptr)
            {
              delete[] out_ptr->profiles[i].spName;
              out_ptr->profiles[i].spName = nullptr;
            }

            if (out_ptr->profiles[i].icon != nullptr)
            {
              delete[] out_ptr->profiles[i].icon;
              out_ptr->profiles[i].icon = nullptr;
            }
          }
          delete[] out_ptr->profiles;
          out_ptr->profiles = nullptr;
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
} /* UimLpaRespMessage::~UimLpaRespMessage */


string UimLpaRespMessage::dump()
{
  return mName + "{ "
    + "resp_id = " + std::to_string(mRespId)
    + " }";
} /* LpaUserReqMessage::dump */
