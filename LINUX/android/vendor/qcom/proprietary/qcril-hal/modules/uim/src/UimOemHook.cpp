/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/legacy.h"
#include "modules/uim/UimOemHook.h"
#include "modules/uim/qcril_uim.h"
#include "UimOemHookRequestMsg.h"
#include "UimOemHookRequestCallback.h"
#include "qcril_qmi_oem_events.h"

/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_voltage_status

===========================================================================*/
void qcril_uim_request_oem_hook_voltage_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_UICC_VOLTAGE_STATUS_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_voltage_status */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_uicc_power

===========================================================================*/
void qcril_uim_request_oem_hook_uicc_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_UICC_POWER_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_uicc_power */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_slot_card_power

===========================================================================*/
void qcril_uim_request_oem_hook_slot_card_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SLOT_CARD_POWER_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_slot_card_power */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_perso_reactivate

===========================================================================*/
void qcril_uim_request_oem_hook_perso_reactivate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_PERSONALIZATION_REACTIVATE_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_perso_reactivate */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_perso_status

===========================================================================*/
void qcril_uim_request_oem_hook_perso_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_PERSONALIZATION_STATUS_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_perso_status */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_enter_perso_key

===========================================================================*/
void qcril_uim_request_oem_hook_enter_perso_key
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_ENTER_DEPERSONALIZATION_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_enter_perso_key */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_sim_status

===========================================================================*/
void qcril_uim_request_oem_hook_get_sim_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SLOT_GET_CARD_STATE_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_get_sim_status */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_atr

===========================================================================*/
void qcril_uim_request_oem_hook_get_atr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SLOT_GET_ATR_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_get_atr */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_send_apdu

===========================================================================*/
void qcril_uim_request_oem_hook_send_apdu
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SLOT_SEND_APDU_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_send_apdu */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_switch_slot

===========================================================================*/
void qcril_uim_request_oem_hook_switch_slot
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SWITCH_SLOT_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_switch_slot */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_physical_slot_mapping

===========================================================================*/
void qcril_uim_request_oem_hook_get_physical_slot_mapping
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ,
                                                       params_ptr,
                                                       ret_ptr,
                                                       &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_get_physical_slot_mapping */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_set_apdu_behavior

===========================================================================*/
void qcril_uim_request_oem_hook_set_apdu_behavior
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ,
                                                        params_ptr,
                                                        ret_ptr,
                                                        &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_set_apdu_behavior */


/*=========================================================================

  FUNCTION: qcril_uim_request_oem_hook_get_slots_status

===========================================================================*/
void qcril_uim_request_oem_hook_get_slots_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_GET_SLOTS_STATUS,
                                                        params_ptr,
                                                        ret_ptr,
                                                        &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_get_terminal_capability */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_get_terminal_capability

===========================================================================*/
void qcril_uim_request_oem_hook_get_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ,
                                                        params_ptr,
                                                        ret_ptr,
                                                        &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_get_terminal_capability */


/*=========================================================================

  FUNCTION:  qcril_uim_request_oem_hook_set_terminal_capability

===========================================================================*/
void qcril_uim_request_oem_hook_set_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  UimOemHookRequestCallback  oem_hook_cb("OEM HOOK REQUEST");
  auto req_ptr = std::make_shared<UimOemHookRequestMsg>(UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ,
                                                        params_ptr,
                                                        ret_ptr,
                                                        &oem_hook_cb);

  if (req_ptr != nullptr)
  {
    req_ptr->dispatch();
  }
} /* qcril_uim_request_oem_hook_set_terminal_capability */

