#ifndef QCRIL_UIM_CARD_H
#define QCRIL_UIM_CARD_H
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "modules/uim/qcril_uim.h"
#include "modules/uim/UimCardStatusIndMsg.h"

/*===========================================================================

  FUNCTION:  qcril_uim_init_card_status

===========================================================================*/
/*!
    @brief
    Initializes the global card status structure.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_init_card_status
(
  qmi_uim_card_status_type  * card_status_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_get_sim_status

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_GET_SIM_STATUS request from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_sim_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_process_status_change_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI card status changed indication. Based
    on the indication received, if needed, it updates the global card status,
    ret_ptr and sends card events internal to QCRIL (CM & PBM).

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_status_change_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);


/*=========================================================================

  FUNCTION:  qcril_uim_process_power_down

===========================================================================*/
/*!
    @brief
    Process the power down request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_process_power_down
(
  qcril_instance_id_e_type          instance_id,
  qcril_modem_id_e_type             modem_id,
  int                               slot
);


/*=========================================================================

  FUNCTION:  qcril_uim_process_power_down

===========================================================================*/
/*!
    @brief
    Process the power up request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_process_power_up
(
  qcril_instance_id_e_type          instance_id,
  qcril_modem_id_e_type             modem_id,
  int                               slot
);


/*===========================================================================

  FUNCTION:  qcril_uim_update_cm_card_status

===========================================================================*/
/*!
    @brief
    Update QCRIL(CM) card status per UIM card state.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_update_cm_card_status
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type    modem_id,
  int                      slot,
  qcril_card_status_e_type new_card_status
);


/*===========================================================================

  FUNCTION:  qcril_uim_update_gstk_card_event

===========================================================================*/
/*!
    @brief
    Update QCRIL GSTK with card error/absent status.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_update_gstk_card_event
(
  int                      slot,
  boolean                  is_refresh
);


/*=========================================================================

  FUNCTION:  qcril_uim_process_change_subscription

===========================================================================*/
/*!
    @brief
    Processes the subscription activation/deactivation requests from QCRIL
    NAS.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_process_change_subscription
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_update_prov_session_type

===========================================================================*/
/*!
    @brief
    Updates the global provisioning session status type based on the passed
    session type.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_update_prov_session_type
(
  qmi_uim_session_type                  session_type,
  qcril_uim_prov_session_state_type     session_state
);

/*=========================================================================

  FUNCTION:  qcril_uim_get_atr_resp

===========================================================================*/
/*!
    @brief
    Processes the response for get ATR command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_atr_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_process_recovery_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI recovery complete indication.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_recovery_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_voltage_supply_resp

===========================================================================*/
/*!
    @brief
    Processes the response for voltage supply command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_voltage_supply_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_process_supply_voltage_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI supply voltage indication.
    Note that the data type for QCRIL_EVT_HOOK_UNSOL_UICC_VOLTAGE_STATUS is
    RIL_UiccVoltageStatus

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_supply_voltage_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_direct_get_card_status

===========================================================================*/
/*!
    @brief
    Returns card status from UIM RIL cache

    @return
    None
*/
/*=========================================================================*/
RIL_Errno qcril_uim_direct_get_card_status
(
  uint8               slot,
  RIL_CardStatus_v6 * ril_card_status
);

/*=========================================================================

  FUNCTION:  qcril_uim_power_resp

===========================================================================*/
/*!
    @brief
    Processes the response for power up/down command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_power_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_free_aid_and_label_info

===========================================================================*/
/*!
    @brief
    Frees the memory allocated for aid and label data that was allocated as
    part of qcril_uim_add_aid_info and qcril_uim_add_label_info respectively.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_free_aid_and_label_info
(
  RIL_CardStatus_v6      * ril_card_status_ptr
);

/*=========================================================================

  FUNCTION:  qcril_uim_change_prov_session_callback

===========================================================================*/
/*!
    @brief
    Callback for the change provisioning session. It sends an event to
    QCRIL(CM) regarding the status of the activate/deactivate command along
    with the session type.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_change_prov_session_callback
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
);

/*=========================================================================

  FUNCTION:  qcril_uim_cancel_card_state_check_timer

===========================================================================*/
/*!
    @brief
    Cancels the card state check timer

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cancel_card_state_check_timer
(
  void
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_voltage_status

===========================================================================*/
/*!
    @brief
     Handles QCRIL_EVT_HOOK_UICC_VOLTAGE_STATUS_REQ request from QCRIL.
     Note that both the request & response have no data payload.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_voltage_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_uicc_power

===========================================================================*/
void qcril_uim_request_uicc_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_process_get_atr

===========================================================================*/
/*!
    @brief
     Handles RIL_REQUEST_SIM_GET_ATR and QCRIL_EVT_HOOK_SLOT_GET_ATR_REQ
     request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_atr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

#endif /* QCRIL_UIM_CARD_H */

