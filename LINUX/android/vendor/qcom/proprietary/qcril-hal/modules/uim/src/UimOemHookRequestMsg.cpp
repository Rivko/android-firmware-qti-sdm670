/******************************************************************************
#  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/UimOemHook.h"
#include "UimOemHookRequestMsg.h"

#define TAG "UimOemHookRequestMsg"

typedef union
{
  int                            slot_id;
  RIL_SlotCardPower              slot_card_power;
  RIL_UiccPower                  uicc_power;
  RIL_PersonalizationReq         perso_req;
  qcril_enter_deperso_req_msg    deperso_req;
  RIL_SlotStreamApduReq          apdu_data;
  RIL_PhySlotMap                 slot_map;
  RIL_SetAPDUBehavior            apdu_behavior;
  RIL_TerminalCapabilityLPATLV   set_tc;
} uim_oem_hook_params;

void UimOemHookRequestMsg::deep_copy
(
  int        request_type,
  void      *in_ptr,
  uint32_t   in_len,
  void     **out_ptr
)
{
  uim_oem_hook_params     * hook_params = nullptr;
  uint8_t                   i           = 0;

  if (in_ptr == nullptr || out_ptr == nullptr)
  {
    return;
  }

  hook_params = new uim_oem_hook_params{};
  *out_ptr    = hook_params;

  if (hook_params == nullptr)
  {
    return;
  }

  switch (request_type)
  {
    case UIM_OEM_HOOK_UICC_VOLTAGE_STATUS_REQ:
    case UIM_OEM_HOOK_PERSONALIZATION_STATUS_REQ:
    case UIM_OEM_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ:
    case UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
    case UIM_OEM_HOOK_GET_SLOTS_STATUS:
      break;
    case UIM_OEM_HOOK_UICC_POWER_REQ:
      hook_params->uicc_power = *((RIL_UiccPower *)in_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_CARD_POWER_REQ:
      hook_params->slot_card_power = *((RIL_SlotCardPower *)in_ptr);
      break;
    case UIM_OEM_HOOK_PERSONALIZATION_REACTIVATE_REQ:
      {
        uint32_t ctrl_key_len = strlen((char *)in_ptr) + 1;

        hook_params->perso_req.controlKey = new char[ctrl_key_len]{0};
        if (hook_params->perso_req.controlKey != nullptr)
        {
          strlcpy(hook_params->perso_req.controlKey, (char *)in_ptr, ctrl_key_len);
        }

        if (((uint8_t *)in_ptr + ctrl_key_len) != NULL)
        {
          hook_params->perso_req.persoType =
               (RIL_PersoSubstate)*((uint8_t *)in_ptr + ctrl_key_len);
        }
      }
      break;
    case UIM_OEM_HOOK_ENTER_DEPERSONALIZATION_REQ:
      if (in_ptr != nullptr && in_len > 2)
      {
        uint32_t code_len   = 0;

        hook_params->deperso_req.deperso_type = new char[strlen((char *)in_ptr) + 1]{};

        if (hook_params->deperso_req.deperso_type == nullptr)
        {
          break;
        }
        strlcpy(hook_params->deperso_req.deperso_type, (char *)in_ptr, strlen((char *)in_ptr) + 1);

        QCRIL_LOG_INFO( "deperso_type: (%s)\n", hook_params->deperso_req.deperso_type);

        /* Check length. Note that deperso code at the very least has to be empty string */
        if (in_len <= (strlen(hook_params->deperso_req.deperso_type)+ 1))
        {
          QCRIL_LOG_ERROR("datalen mismatch, total datalen: %d", in_len);
          break;
        }

        /* Update offset to point to CK. Note that empty Deperso code is supported */
        code_len = in_len - (strlen(hook_params->deperso_req.deperso_type) + 1);
        hook_params->deperso_req.deperso_code = new char[code_len + 1]{};

        if (hook_params->deperso_req.deperso_code != nullptr)
        {
          strlcpy(hook_params->deperso_req.deperso_code,
                  (char *)in_ptr + (strlen(hook_params->deperso_req.deperso_type) + 1),
                  code_len + 1);
          QCRIL_LOG_INFO( "deperso_code: (%s)\n", hook_params->deperso_req.deperso_code );
        }
      }
      break;
    case UIM_OEM_HOOK_SLOT_GET_CARD_STATE_REQ:
    case UIM_OEM_HOOK_SLOT_GET_ATR_REQ:
      hook_params->slot_id = *((int *)in_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_SEND_APDU_REQ:
      hook_params->apdu_data = *((RIL_SlotStreamApduReq *)in_ptr);
      if (((RIL_SlotStreamApduReq *)in_ptr)->apdu_data.data != nullptr)
      {
        hook_params->apdu_data.apdu_data.data =
           new char[strlen(((RIL_SlotStreamApduReq *)in_ptr)->apdu_data.data) + 1]{0};
        if (hook_params->apdu_data.apdu_data.data != nullptr)
        {
          memcpy(hook_params->apdu_data.apdu_data.data,
                 ((RIL_SlotStreamApduReq *)in_ptr)->apdu_data.data,
                 strlen(((RIL_SlotStreamApduReq *)in_ptr)->apdu_data.data));
        }
      }
      break;
    case UIM_OEM_HOOK_SWITCH_SLOT_REQ:
      for (i = 0; i < ((RIL_PhySlotMap *)in_ptr)->array_len && i < QMI_UIM_MAX_CARD_COUNT; i++)
      {
        hook_params->slot_map.slot_map[i] = ((RIL_PhySlotMap *)in_ptr)->slot_map[i];
      }
      hook_params->slot_map.array_len = ((RIL_PhySlotMap *)in_ptr)->array_len;
      break;
    case UIM_OEM_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ:
      hook_params->apdu_behavior = *((RIL_SetAPDUBehavior *)in_ptr);
      break;
    case UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      hook_params->set_tc.tlv_present = ((RIL_TerminalCapabilityLPATLV *)in_ptr)->tlv_present;
      hook_params->set_tc.value = ((RIL_TerminalCapabilityLPATLV *)in_ptr)->value;
      break;
    default:
      break;
  }
} /* deep_copy */


void UimOemHookRequestMsg::deep_clean
(
  int    request_type,
  void  *data_ptr
)
{
  uim_oem_hook_params     * hook_params = nullptr;

  if (data_ptr == nullptr)
  {
    return;
  }

  hook_params = (uim_oem_hook_params *) data_ptr;

  switch (request_type)
  {
    case UIM_OEM_HOOK_UICC_VOLTAGE_STATUS_REQ:
    case UIM_OEM_HOOK_PERSONALIZATION_STATUS_REQ:
    case UIM_OEM_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ:
    case UIM_OEM_HOOK_UICC_POWER_REQ:
    case UIM_OEM_HOOK_SLOT_CARD_POWER_REQ:
    case UIM_OEM_HOOK_SLOT_GET_CARD_STATE_REQ:
    case UIM_OEM_HOOK_SLOT_GET_ATR_REQ:
    case UIM_OEM_HOOK_SWITCH_SLOT_REQ:
    case UIM_OEM_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ:
    case UIM_OEM_HOOK_GET_SLOTS_STATUS:
    case UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
    case UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      break;
    case UIM_OEM_HOOK_PERSONALIZATION_REACTIVATE_REQ:
      if (hook_params->perso_req.controlKey != nullptr)
      {
        delete[] hook_params->perso_req.controlKey;
      }
      break;
    case UIM_OEM_HOOK_ENTER_DEPERSONALIZATION_REQ:
      if (hook_params->deperso_req.deperso_type != nullptr)
      {
        delete[] hook_params->deperso_req.deperso_type;
      }
      if (hook_params->deperso_req.deperso_code != nullptr)
      {
        delete[] hook_params->deperso_req.deperso_code;
      }
      break;
    case UIM_OEM_HOOK_SLOT_SEND_APDU_REQ:
      if (hook_params->apdu_data.apdu_data.data != nullptr)
      {
        delete[] hook_params->apdu_data.apdu_data.data;
      }
      break;
    default:
      break;
  }

  delete hook_params;
} /* deep clean */


/*===========================================================================
  FUNCTION  UimOemHookRequestMsg - UimOemHookRequestMsg destructor
===========================================================================*/
UimOemHookRequestMsg::~UimOemHookRequestMsg()
{
  deep_clean(mMsgId, mParams.data);
  setCallback(nullptr);
} /* UimOemHookRequestMsg::~UimOemHookRequestMsg() */


/*===========================================================================
  FUNCTION  UimOemHookRequestMsg - UimOemHookRequestMsg constructor
===========================================================================*/
UimOemHookRequestMsg::UimOemHookRequestMsg
(
  int msg_id,
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type * const ret_ptr,
  GenericCallback<UimOemHookRspData> *callback
) : SolicitedMessage<UimOemHookRspData>(get_class_message_id())
{
  mMsgId  = msg_id;
  mRetPtr = ret_ptr;
  memset(&mParams, 0x00, sizeof(mParams));

  if (params_ptr != nullptr)
  {
    memcpy(&mParams, params_ptr, sizeof(mParams));
    mParams.event_id_android = msg_id;
    mParams.datalen = params_ptr->datalen;
    if (params_ptr->datalen != 0 && params_ptr->data != nullptr)
    {
      deep_copy(msg_id, params_ptr->data, params_ptr->datalen, &mParams.data);
    }
    else
    {
      mParams.data = nullptr;
    }
  }
  setCallback(callback);
} /* UimOemHookRequestMsg::UimOemHookRequestMsg */
