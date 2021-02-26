/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
#  @file    qcril_qmi_prov.h
#  @brief   Manual Provisioning and MSIM feature
#
#******************************************************************************/
#ifndef QCRIL_QMI_PROV_H
#define QCRIL_QMI_PROV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "qcrili.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "network_access_service_v01.h"
#include "device_management_service_v01.h"

#ifdef __cplusplus
} // end of extern "C"
#endif

#include "qcril_qmi_nas.h"

#define QCRIL_INVALID_PROV_PREF (-1)

typedef enum {
    QCRIL_QMI_PROV_STATE_NONE = 0,
    QCRIL_QMI_PROV_STATE_FM_START,
    QCRIL_QMI_PROV_STATE_FM_APPLY,
    QCRIL_QMI_PROV_STATE_USER_ACTIVATE,
    QCRIL_QMI_PROV_STATE_USER_DEACTIVATE,
    QCRIL_QMI_PROV_STATE_CARD_UP,
} qcril_qmi_prov_state_type;

typedef enum {
    QCRIL_QMI_PROV_ACTION_NONE = 0,
    QCRIL_QMI_PROV_ACTION_DEFER,
    QCRIL_QMI_PROV_ACTION_IGNORE
} qcril_qmi_prov_action_type;

typedef struct
{
    int act_status;
    int gw_app_index;
    int cdma_app_index;
    int gw_app_prov_state;
    int cdma_app_prov_state;
    int sub_type;
}qcril_qmi_prov_status_type;

typedef struct
{
    int                             user_prov_pref;
    int                             curr_prov_status;
    int                             gw_app_index;
    int                             cdma_app_index;
    char                            iccid[ QMI_DMS_UIM_ID_MAX_V01 + 1];
    qcril_qmi_prov_state_type       state;
    qcril_card_app_provision_status gw_provision_state;
    qcril_card_app_provision_status cdma_provision_state;
} qcril_qmi_prov_info_type;

typedef struct
{
    qcril_qmi_prov_info_type prov_info[QMI_UIM_MAX_CARD_COUNT];
}qcril_qmi_prov_common_type;

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_send_unsol_sub_pref_change

===========================================================================*/
/*!
    @brief
    Sends QCRIL_EVT_HOOK_UNSOL_SUB_PROVISION_STATUS to telephony.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_send_unsol_sub_pref_change();

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_get_sim_iccid_req_handler

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_GET_SIM_ICCID

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_get_sim_iccid_req_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_get_sub_prov_pref_req_handler

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_get_sub_prov_pref_req_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_handle_prov_state_change
===========================================================================*/
/*!
    @brief
    Post internal event to handle different states in provisioning module.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_handle_prov_state_change(qcril_qmi_prov_state_type state);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_set_sub_prov_pref_req_handler

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_set_sub_prov_pref_req_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_prov_activate_sub_status_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_PROV_ACTIVATE_SUB_STATUS

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_nas_prov_activate_sub_status_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_prov_deactivate_sub_status_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_PROV_DEACTIVATE_SUB_STATUS

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_nas_prov_deactivate_sub_status_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_update_provisioning_state

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_RIL_REQUEST_UPDATE_PROV_STATE.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_update_provisioning_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_manual_provisioning_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_RIL_REQUEST_MANUAL_PROVISIONING.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_manual_provisioning_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_activate_deactivate_app

===========================================================================*/
/*!
    @brief
    Core function of provisioning module.
    Sends request to UIM for activation/deactivation.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_activate_deactivate_app
(
    RIL_Token t,
    qcril_evt_e_type event_id,
    RIL_UiccSubActStatus act_status
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_all_apps_deprovisioned

===========================================================================*/
/*!
    @brief
    Check if all SIM applications are deprovisioned.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_prov_all_apps_deprovisioned(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_all_apps_provisioned

===========================================================================*/
/*!
    @brief
    Check if all SIM applications are provisioned.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_prov_all_apps_provisioned(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_is_any_app_being_provisioned

===========================================================================*/
/*!
    @brief
    Check if any SIM application is in process of provisioning.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_prov_is_any_app_being_provisioned(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_is_any_app_being_deprovisioned

===========================================================================*/
/*!
    @brief
    Check if any SIM application is in process of deprovisioning.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_prov_is_any_app_being_deprovisioned(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_is_any_app_provisioned

===========================================================================*/
/*!
    @brief
    Check if any SIM application provisioned.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_prov_is_any_app_provisioned(void);

/*===========================================================================

    qcril_qmi_prov_get_app_index_for_activation

============================================================================*/
/*!
    @brief
    Get next applications app_index for activation.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_app_index_for_activation(void);

/*===========================================================================

    qcril_qmi_prov_get_app_index_for_deactivation

============================================================================*/
/*!
    @brief
    Get next applications app_index for deactivation.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_app_index_for_deactivation(void);

/*===========================================================================

    qcril_qmi_prov_check_and_send_prov_resp

============================================================================*/
/*!
    @brief
    Send appropriate response based on current prov state.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_check_and_send_prov_resp(RIL_Errno res, qcril_qmi_prov_state_type prov_state);

/*===========================================================================

    qcril_qmi_prov_evaluate_and_mark_apps_to_deactivate

============================================================================*/
/*!
    @brief
    Mark applications to deactivate if they are in ready state.

    @return
    None
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps_to_deactivate(void);

/*===========================================================================

    qcril_qmi_prov_evaluate_and_mark_apps_to_activate

============================================================================*/
/*!
    @brief
    Mark applications to activate if they are in ready state.

    @return
    None
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps_to_activate(void);

/*===========================================================================

    qcril_qm_prov_retrieve_gw_app_index

============================================================================*/
/*!
    @brief
    Retrieve GW app index. If there are multiple gw apps preference
    will be give to USIM.

    @return
    None
*/
/*=========================================================================*/
int qcril_qm_prov_retrieve_gw_app_index
(
    RIL_CardStatus_v6 *ril_card_status
);

/*===========================================================================

    qcril_qmi_prov_retrieve_cdma_app_index

============================================================================*/
/*!
    @brief
    Retrieve CDMA app index. If multiple CDMA apps are available, preference
    would be given to CSIM.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_retrieve_cdma_app_index
(
    RIL_CardStatus_v6 *ril_card_status
);

/*===========================================================================

    qcril_qmi_prov_subs_activate_followup

============================================================================*/
/*!
    @brief
    Handler for QCRIL_EVT_QMI_PROV_ACTIVATE_FOLLOW_UP from UIM..

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_subs_activate_followup
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

/*===========================================================================

    qcril_qmi_prov_subs_deactivate_followup

============================================================================*/
/*!
    @brief
    Handler for QCRIL_EVT_QMI_PROV_DEACTIVATE_FOLLOW_UP from UIM..

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_subs_deactivate_followup
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*===========================================================================

    qcril_qmi_prov_reset_prov_pref_info

============================================================================*/
/*!
    @brief
    Reset provisioning module internal cache.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_reset_prov_pref_info(int reset_state);

/*===========================================================================

    qcril_qmi_prov_set_curr_sub_prov_status

============================================================================*/
/*!
    @brief
    Update curr_sub_prov_status in cache.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_set_curr_sub_prov_status
(
    int curr_prov_status
);

/*===========================================================================

    qcril_qmi_prov_get_curr_sub_prov_status

============================================================================*/
/*!
    @brief
    Retrieve curr_sub_prov_status from cache.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_curr_sub_prov_status(void);

/*===========================================================================

    qcril_qmi_prov_get_user_pref_from_cache

============================================================================*/
/*!
    @brief
    Retrieve user_pref from cache.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_user_pref_from_cache(void);

/*===========================================================================

    qcril_qmi_prov_update_user_pref_cache

============================================================================*/
/*!
    @brief
    Update user_pref in cache.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_update_user_pref_cache(RIL_UiccSubActStatus new_user_pref);

/*===========================================================================

    qcril_qmi_prov_set_gw_provision_state

============================================================================*/
/*!
    @brief
    Update gw app provisioning state.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_set_gw_provision_state(qcril_card_app_provision_status prov_state);

/*===========================================================================

    qcril_qmi_prov_set_cdma_provision_state

============================================================================*/
/*!
    @brief
    Update cdma app provisioning state.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_set_cdma_provision_state(qcril_card_app_provision_status prov_state);

/*===========================================================================

    qcril_qmi_prov_get_gw_provision_state

============================================================================*/
/*!
    @brief
    retrieve gw app provisioning state from cache.

    @return
    None
*/
/*=========================================================================*/
qcril_card_app_provision_status qcril_qmi_prov_get_gw_provision_state(void);

/*===========================================================================

    qcril_qmi_prov_get_cdma_provision_state

============================================================================*/
/*!
    @brief
    retrieve cdma app provisioning state from cache.

    @return
    None
*/
/*=========================================================================*/
qcril_card_app_provision_status qcril_qmi_prov_get_cdma_provision_state(void);

/*===========================================================================

    qcril_qmi_prov_get_gw_app_index

============================================================================*/
/*!
    @brief
    retrieve gw app index from cache.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_gw_app_index(void);

/*===========================================================================

    qcril_qmi_prov_get_cdma_app_index

============================================================================*/
/*!
    @brief
    retrieve cdma app index from cache.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_get_cdma_app_index(void);

/*===========================================================================

    qcril_qmi_prov_set_gw_app_index

============================================================================*/
/*!
    @brief
    update gw app index in cache.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_set_gw_app_index(int app_index);

/*===========================================================================

    qcril_qmi_prov_set_cdma_app_index

============================================================================*/
/*!
    @brief
    update cdma app index in cache.

    @return
    None
*/
/*=========================================================================*/
int qcril_qmi_prov_set_cdma_app_index(int app_index);

/*===========================================================================

    qcril_qmi_prov_update_app_provision_status_using_app_index

============================================================================*/
/*!
    @brief
    Update app provision status.

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_prov_update_app_provision_status_using_app_index
(
    int                  index,
    RIL_UiccSubActStatus provision_act,
    RIL_Errno            res
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_get_prov_state
===========================================================================*/
/*!
    @brief
    Get prov state.

    @return
    None.
*/
/*=========================================================================*/
qcril_qmi_prov_state_type qcril_qmi_prov_get_prov_state(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_get_act_status_from_state
===========================================================================*/
/*!
    @brief
    Based on state, user preference return appropriate act_status.

    @return
    None.
*/
/*=========================================================================*/
RIL_UiccSubActStatus qcril_qmi_prov_get_act_status_from_state(int state);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_set_prov_state
===========================================================================*/
/*!
    @brief
    Handle prov state machine.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_set_prov_state
(
    qcril_qmi_prov_state_type   state,
    qcril_qmi_prov_action_type  *prov_action
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_evaluate_and_mark_apps
===========================================================================*/
/*!
    @brief
    Evaluate application to activate/deactivate.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_update_iccid_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_update_iccid_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_get_iccid_from_cache

===========================================================================*/
/*!
    @brief
    Get iccid from internal cache. iccid will be null terminated string.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_get_iccid_from_cache(char *iccid);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_fill_prov_preference_info

===========================================================================*/
/*!
    @brief
    Fetach current subscription status, user preference from internal cache.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_prov_fill_prov_preference_info(RIL_SubProvStatus *payload);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_update_db_with_user_pref

===========================================================================*/
/*!
    @brief
    Update database with current user preference for subscription.

    @return
    None.
*/
/*=========================================================================*/
RIL_Errno qcril_qmi_prov_update_db_with_user_pref(RIL_UiccSubActStatus user_prov_pref);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_fetch_user_prov_pref_from_database

===========================================================================*/
/*!
    @brief
    Fetch user subscription preference from database.

    @return
    None.
*/
/*=========================================================================*/
int qcril_qmi_nas_fetch_user_prov_pref_from_database(void);

#endif
