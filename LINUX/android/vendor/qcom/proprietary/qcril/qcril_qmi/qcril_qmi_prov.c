/******************************************************************************
#  @file    qcril_qmi_prov.c
#  @brief   Manual Provisioning and MSIM feature
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "ril.h"
#include "IxErrno.h"
#include "comdef.h"
#include "qcrili.h"
#include "qcril_db.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_prov.h"
#include "qcril_uim_card.h"
#include "qcril_qmi_client.h"
#include "qmi_ril_platform_dep.h"

qcril_qmi_prov_common_type prov_common_cache;

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
void qcril_qmi_prov_send_unsol_sub_pref_change()
{
    RIL_SubProvStatus resp_payload;

    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_prov_fill_prov_preference_info(&resp_payload);

    QCRIL_LOG_INFO( "User pref %d", resp_payload.user_preference);
    QCRIL_LOG_INFO( "Current sub pref %d", resp_payload.current_sub_preference);
    qcril_hook_unsol_response( QCRIL_DEFAULT_INSTANCE_ID,
                               QCRIL_EVT_HOOK_UNSOL_SUB_PROVISION_STATUS,
                               (char *)&resp_payload,
                               sizeof(resp_payload));

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    char                               iccid[QMI_DMS_UIM_ID_MAX_V01 + 1];
    RIL_Errno                          ril_req_res = RIL_E_SUCCESS;
    qcril_request_resp_params_type     resp;
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    memset(iccid, 0, sizeof(iccid));
    qcril_qmi_prov_get_iccid_from_cache(iccid);

    QCRIL_LOG_INFO("iccid - %s",iccid);
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       ril_req_res,
                                       &resp );

    resp.resp_pkt = iccid;
    resp.resp_len = strlen(iccid);
    qcril_send_request_response( &resp );

    QCRIL_LOG_FUNC_RETURN();
}


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
)
{
    RIL_Errno                             ril_req_res = RIL_E_SUCCESS;
    RIL_SubProvStatus                     resp_payload;
    qcril_request_resp_params_type        resp;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    memset(&resp_payload, 0, sizeof(resp_payload));

    qcril_qmi_prov_fill_prov_preference_info(&resp_payload);

    QCRIL_LOG_INFO( "User pref %d", resp_payload.user_preference);
    QCRIL_LOG_INFO( "Current sub pref %d", resp_payload.current_sub_preference);

    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       ril_req_res,
                                       &resp );
    resp.resp_pkt = &resp_payload;
    resp.resp_len = sizeof(resp_payload);
    qcril_send_request_response( &resp );

    QCRIL_LOG_FUNC_RETURN();
}

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
void qcril_qmi_prov_handle_prov_state_change(qcril_qmi_prov_state_type state)
{
    QCRIL_LOG_FUNC_ENTRY();

    qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                      QCRIL_DEFAULT_MODEM_ID,
                      QCRIL_DATA_ON_STACK,
                      QCRIL_EVT_RIL_REQUEST_MANUAL_PROVISIONING,
                      &state,
                      sizeof(state),
                      (RIL_Token) QCRIL_SUB_PROVISION_INTERNAL_TOKEN);

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    int                                   pending_event;
    RIL_Errno                             ril_req_res = RIL_E_SUCCESS;
    qcril_qmi_prov_state_type             prov_state;
    RIL_SetSubProvPreference              *prov_pref;
    qcril_reqlist_public_type             qcril_req_info;
    qcril_request_resp_params_type        resp;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    if(NULL != params_ptr->data && params_ptr->datalen > QMI_RIL_ZERO )
    {
        do
        {
            memset(&resp, 0, sizeof(resp));
            memset(&qcril_req_info, 0, sizeof(qcril_req_info));
            prov_pref = (RIL_SetSubProvPreference *)params_ptr->data;
            QCRIL_LOG_INFO("USER %s", prov_pref->act_status ? "ACTIVATE":"DEACTIVATE");

            /* Set state to USER_ACTIVATE/USER_DEACTIVATE and wait for event
            ** QCRIL_EVT_QMI_PROV_ACTIVATE_SUB_STATUS/QCRIL_EVT_QMI_PROV_DEACTIVATE_SUB_STATUS.
            ** As part of above events handling, update user preference in database and send
            ** response to telephony.
            */
            if ( prov_pref->act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE )
            {
                pending_event = QCRIL_EVT_QMI_PROV_ACTIVATE_SUB_STATUS;
                prov_state = QCRIL_QMI_PROV_STATE_USER_ACTIVATE;
            }
            else
            {
                pending_event = QCRIL_EVT_QMI_PROV_DEACTIVATE_SUB_STATUS;
                prov_state = QCRIL_QMI_PROV_STATE_USER_DEACTIVATE;
            }

            qcril_reqlist_default_entry( params_ptr->t,
                                         params_ptr->event_id,
                                         QCRIL_DEFAULT_MODEM_ID,
                                         QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                                         pending_event,
                                         NULL,
                                         &qcril_req_info );

            if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
            {
                qcril_qmi_prov_handle_prov_state_change(prov_state);
            }
            else
            {
                QCRIL_LOG_ERROR("Failed to add entry to reqlist..");
                ril_req_res = RIL_E_GENERIC_FAILURE;
                break;
            }

        } while(FALSE);
    }

    if ((ril_req_res != RIL_E_SUCCESS))
    {
        QCRIL_LOG_ESSENTIAL("res %d", ril_req_res);
        qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                           params_ptr->t,
                                           params_ptr->event_id,
                                           ril_req_res,
                                           &resp );
        qcril_send_request_response( &resp );
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    int                              res = RIL_E_GENERIC_FAILURE;
    int                              found_req_res = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type        req_info;
    qcril_request_resp_params_type   resp;
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    memset(&resp, 0, sizeof(resp));
    memset(&req_info, 0, sizeof(req_info));

    if ( params_ptr->data != NULL && params_ptr->datalen > 0 )
    {
        res = *(int*) params_ptr->data;

        found_req_res = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                      QCRIL_DEFAULT_MODEM_ID,
                                                      QCRIL_EVT_QMI_PROV_ACTIVATE_SUB_STATUS,
                                                      &req_info );

        QCRIL_LOG_INFO("found req - %d, res - %d", found_req_res, res );

        if ( found_req_res == RIL_E_SUCCESS )
        {
            if ( res == RIL_E_SUCCESS )
            {
                /* Request is from user and response is success,
                ** so update data base with user preference.
                */
                qcril_qmi_prov_update_db_with_user_pref(RIL_UICC_SUBSCRIPTION_ACTIVATE);
            }

            qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                               req_info.t,
                                               req_info.request,
                                               res,
                                               &resp );
            qcril_send_request_response( &resp );
            qcril_qmi_prov_send_unsol_sub_pref_change();
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    int                              res = RIL_E_GENERIC_FAILURE;
    int                              found_req_res = RIL_E_GENERIC_FAILURE;
    qcril_reqlist_public_type        req_info;
    qcril_request_resp_params_type   resp;
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();


    memset(&resp, 0, sizeof(resp));
    memset(&req_info, 0, sizeof(req_info));

    if ( params_ptr->data != NULL && params_ptr->datalen > 0 )
    {
        res = *(int*) params_ptr->data;

        found_req_res = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                                                      QCRIL_DEFAULT_MODEM_ID,
                                                      QCRIL_EVT_QMI_PROV_DEACTIVATE_SUB_STATUS,
                                                      &req_info );

        QCRIL_LOG_INFO("found req - %d, res - %d", found_req_res, res );
        if ( found_req_res == RIL_E_SUCCESS )
        {
            if ( res == RIL_E_SUCCESS )
            {
                /* Request is from user and response is success,
                ** so update data base with user preference.
                */
                qcril_qmi_prov_update_db_with_user_pref(RIL_UICC_SUBSCRIPTION_DEACTIVATE);
            }
            qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                               req_info.t,
                                               req_info.request,
                                               res,
                                               &resp );
            qcril_send_request_response( &resp );
            qcril_qmi_prov_send_unsol_sub_pref_change();
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    int                              send_resp = TRUE;
    int                              apps_prov = FALSE;
    int                              apps_deprov = FALSE;
    qcril_qmi_prov_action_type       prov_action = QCRIL_QMI_PROV_ACTION_NONE;
    int                              app_index = QCRIL_INVALID_APP_INDEX;
    RIL_Errno                        res = RIL_E_SUCCESS;
    RIL_UiccSubActStatus             act_status;
    qcril_qmi_prov_state_type        prop_state = QCRIL_QMI_PROV_STATE_NONE;
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    if ( params_ptr->datalen > 0 && params_ptr->data != NULL )
    {
        do
        {
            prop_state = *(qcril_qmi_prov_state_type*)params_ptr->data;

            QCRIL_LOG_INFO("proposed provisioning state - %d", prop_state);
            /* Update provisioning state machine with proposed state */
            res = qcril_qmi_prov_set_prov_state(prop_state, &prov_action);
            if ( res != RIL_E_SUCCESS || prop_state == QCRIL_QMI_PROV_STATE_NONE )
            {
                /* If defer flag is set, don't send response. */
                if ( prov_action == QCRIL_QMI_PROV_ACTION_DEFER )
                {
                    QCRIL_LOG_INFO("Differ this request as already manual provisioning in-progress");
                    send_resp = FALSE;
                }
                else if ( prop_state == QCRIL_QMI_PROV_STATE_NONE )
                {
                    QCRIL_LOG_INFO("request is to just to reset the status...");
                    send_resp = FALSE;
                }
                break;
            }

            /* Based on current state, mark applications as pending prov/deprov */
            if ( RIL_E_SUCCESS != qcril_qmi_prov_evaluate_and_mark_apps() )
            {
                res = RIL_E_GENERIC_FAILURE;
                break;
            }

            act_status = qcril_qmi_prov_get_act_status_from_state(prop_state);

            /* First send request for one app. Once activation response from UIM
            ** is received, send another request to UIM for second app if present. */
            if ( act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE )
             {
                 app_index = qcril_qmi_prov_get_app_index_for_activation();
             }
             else
             {
                 app_index = qcril_qmi_prov_get_app_index_for_deactivation();
             }

             if ( app_index != QCRIL_INVALID_APP_INDEX )
             {
                 res = qcril_qmi_prov_activate_deactivate_app( params_ptr->t,
                                                               params_ptr->event_id,
                                                               act_status );
                 if ( res == RIL_E_SUCCESS )
                 {
                     send_resp = FALSE;
                 }
             }
             else
             {
                 QCRIL_LOG_INFO("Checking if all apps are activated/deactivated.");
                 if ( act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE )
                 {
                     apps_prov = qcril_qmi_prov_all_apps_provisioned();
                 }
                 else
                 {
                     apps_deprov = qcril_qmi_prov_all_apps_deprovisioned();
                 }

                 /* If all apps are activate/deactivated send success response */
                 if ( apps_prov || apps_deprov )
                 {
                     qcril_qmi_prov_set_curr_sub_prov_status(act_status);
                 }
                 else
                 {
                     res = RIL_E_GENERIC_FAILURE;
                     QCRIL_LOG_INFO("No app available on SIM card or SIM card is absent");
                 }
             }

        } while ( FALSE );
    }

    /* If activation/deactivation request is not sent to UIM, send response here */
    if ( send_resp == TRUE )
    {
        qcril_qmi_prov_check_and_send_prov_resp(res, prop_state);
    }
}

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
    int event_id,
    RIL_UiccSubActStatus act_status
)
{
    RIL_Errno                        ril_req_res = RIL_E_SUCCESS;
    IxErrnoType                      err_no = E_FAILURE;
    qcril_evt_e_type                 pending_event_id;
    RIL_SelectUiccSub                select_uicc_sub;
    qcril_subs_mode_pref             mode_pref;
    qcril_reqlist_u_type             u_info;
    qcril_modem_id_e_type            modem_id;
    qmi_client_error_type            qmi_client_error;
    qcril_req_state_e_type           pending_state;
    qcril_reqlist_nas_type           *req_info_ptr = NULL;
    qcril_instance_id_e_type         instance_id;
    qcril_reqlist_public_type        reqlist_entry;
    qcril_uicc_subs_info_type        subs_info;
    nas_get_mode_pref_resp_msg_v01   qmi_mode_pref_resp;

    memset( &u_info, NIL, sizeof(u_info));
    memset( &subs_info, NIL, sizeof(subs_info));
    memset( &qmi_mode_pref_resp, NIL, sizeof(qmi_mode_pref_resp) );
    memset( &select_uicc_sub, NIL, sizeof(select_uicc_sub) );
    instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    modem_id    = QCRIL_DEFAULT_MODEM_ID;

    /* fill data required for UIM to activate/deactivate. */
    select_uicc_sub.slot       = qmi_ril_get_sim_slot();
    select_uicc_sub.sub_type   = qcril_qmi_nas_get_modem_stack_id();
    select_uicc_sub.act_status = act_status;

    if ( act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE )
    {
        pending_state             = QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS;
        pending_event_id          = QCRIL_EVT_CM_ACTIVATE_PROVISION_STATUS;
        select_uicc_sub.app_index = qcril_qmi_prov_get_app_index_for_activation();

        QCRIL_LOG_DEBUG( "RID %d Activate sub: slot %d app_index %d",
                                                        instance_id,
                                                        select_uicc_sub.slot,
                                                        select_uicc_sub.app_index );
    }
    else
    {
        pending_state             = QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS;
        pending_event_id          = QCRIL_EVT_CM_DEACTIVATE_PROVISION_STATUS;
        select_uicc_sub.app_index = qcril_qmi_prov_get_app_index_for_deactivation();

        QCRIL_LOG_DEBUG( "RID %d Deactivate sub: slot %d app_index %d",
                                                        instance_id,
                                                        select_uicc_sub.slot,
                                                        select_uicc_sub.app_index );
    }

    qmi_client_error =
        qmi_client_send_msg_sync_with_shm( qcril_qmi_client_get_user_handle ( QCRIL_QMI_CLIENT_NAS ),
                                           QMI_NAS_GET_MODE_PREF_REQ_MSG_V01,
                                           NULL,
                                           NIL,  // empty request payload
                                           (void*) &qmi_mode_pref_resp,
                                           sizeof( qmi_mode_pref_resp ),
                                           QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT );

    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_client_error,
                                                                           &qmi_mode_pref_resp.resp );

    QCRIL_LOG_INFO( ".. mode pref r %d, 0mpv %d, 0mp %d, 1mpv %d, 1mp %d, 2mpv %d, 2mp %d",
                    (int) ril_req_res,
                    (int) qmi_mode_pref_resp.idx0_mode_pref_valid,
                    (int) qmi_mode_pref_resp.idx0_mode_pref,
                    (int) qmi_mode_pref_resp.idx1_mode_pref_valid,
                    (int) qmi_mode_pref_resp.idx1_mode_pref,
                    (int) qmi_mode_pref_resp.idx2_mode_pref_valid,
                    (int) qmi_mode_pref_resp.idx2_mode_pref );

    if ( RIL_E_SUCCESS == ril_req_res )
    {
        if ( RIL_SUBSCRIPTION_1 == select_uicc_sub.sub_type )
        {
            mode_pref =
                qcril_qmi_nas_conv_nas_mode_pref_to_qcril( qmi_mode_pref_resp.idx0_mode_pref_valid,
                                                           qmi_mode_pref_resp.idx0_mode_pref,
                                                           &select_uicc_sub );
        }
        else if ( RIL_SUBSCRIPTION_2 == select_uicc_sub.sub_type )
        {
            mode_pref =
                qcril_qmi_nas_conv_nas_mode_pref_to_qcril( qmi_mode_pref_resp.idx1_mode_pref_valid,
                                                           qmi_mode_pref_resp.idx1_mode_pref,
                                                           &select_uicc_sub );
        }
        else
        {
             mode_pref =
                qcril_qmi_nas_conv_nas_mode_pref_to_qcril( qmi_mode_pref_resp.idx2_mode_pref_valid,
                                                           qmi_mode_pref_resp.idx2_mode_pref,
                                                           &select_uicc_sub );
        }
        QCRIL_LOG_INFO( ".. mode pref-cnv param %d, cnved %d", (int) select_uicc_sub.sub_type, (int) mode_pref );

        req_info_ptr = &u_info.nas;
        memcpy( &req_info_ptr->select_uicc_sub, &select_uicc_sub, sizeof( RIL_SelectUiccSub ) );

        if( E_SUCCESS == qcril_reqlist_query_by_request( instance_id,
                                                event_id, &reqlist_entry ) )
        {
            QCRIL_LOG_INFO( ".. reqlist updating existing entry, instance id %d", (int) instance_id);
            err_no = qcril_reqlist_update_pending_event_id( instance_id,
                                          modem_id, t, pending_event_id );
        }
        else
        {
            QCRIL_LOG_INFO( ".. Create new entry, instance id %d", (int) instance_id);
            qcril_reqlist_default_entry( t,
                                         event_id,
                                         modem_id,
                                         pending_state,
                                         pending_event_id,
                                         &u_info,
                                         &reqlist_entry );
            err_no = qcril_reqlist_new( instance_id, &reqlist_entry );
        }

        if ( err_no == E_SUCCESS )
        {
            QCRIL_LOG_INFO( ".. reqlist outstanding new ok, instance id %d", (int) instance_id);

            subs_info.subs_mode_pref = mode_pref;
            memcpy( &subs_info.uicc_subs_info, &select_uicc_sub, sizeof( RIL_SelectUiccSub ) );

            if ( select_uicc_sub.act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE )
            {
                QCRIL_LOG_DEBUG( "Request QCRIL(UIM) to activate slot %d aid %d",
                                                                select_uicc_sub.slot,
                                                                select_uicc_sub.app_index );
                err_no = qcril_process_event( instance_id,
                                              modem_id,
                                              QCRIL_EVT_INTERNAL_MMGSDI_ACTIVATE_SUBS,
                                              (void *) &subs_info,
                                              sizeof( qcril_uicc_subs_info_type ),
                                              (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
                QCRIL_LOG_INFO( ".. QCRIL_EVT_INTERNAL_MMGSDI_ACTIVATE_SUBS res %d", (int) err_no );

            }
            else
            {
                QCRIL_LOG_DEBUG( "Request QCRIL(UIM) to deactivate slot %d aid %d",
                                                                select_uicc_sub.slot,
                                                                select_uicc_sub.app_index );
                err_no = qcril_process_event( instance_id,
                                              modem_id,
                                              QCRIL_EVT_INTERNAL_MMGSDI_DEACTIVATE_SUBS,
                                              (void *) &subs_info,
                                              sizeof( qcril_uicc_subs_info_type ),
                                              (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
                QCRIL_LOG_INFO( ".. QCRIL_EVT_INTERNAL_MMGSDI_DEACTIVATE_SUBS res %d", (int) err_no );
            }

            if ( err_no != E_SUCCESS )
            {
                QCRIL_LOG_INFO("Failed to send request to QCRIL UIM - %d", err_no);
                ril_req_res = RIL_E_GENERIC_FAILURE;
            }
        }
        else
        {
            QCRIL_LOG_INFO( ".. reqlist oustanding new fail");
            ril_req_res = RIL_E_GENERIC_FAILURE;
        }
    }

    /* if success, update app status from
    ** pending provisioning/pending deprovisioning -> provisioning/deprovisioning
    ** if failure, update app status to not_provisioned
    */
    qcril_qmi_prov_update_app_provision_status_using_app_index(
                            select_uicc_sub.app_index,
                            select_uicc_sub.act_status,
                            ril_req_res);

    QCRIL_LOG_INFO("completed with %d", (int) ril_req_res);
    return ril_req_res;
}

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
int qcril_qmi_prov_all_apps_deprovisioned(void)
{
  int res                                              = FALSE;
  qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
  qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

  QCRIL_LOG_FUNC_ENTRY();

  gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
  cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

  QCRIL_LOG_INFO("gw_provision_state: %d cdma_provision_state: %d",
                                                gw_provision_state,
                                                cdma_provision_state);

  if ( ( (gw_provision_state   == QCRIL_APP_STATUS_DEPROVISIONED)  &&
         (cdma_provision_state == QCRIL_APP_STATUS_DEPROVISIONED) ) ||
       ( (gw_provision_state   == QCRIL_APP_STATUS_DEPROVISIONED) &&
         (cdma_provision_state == QCRIL_APP_STATUS_NOT_PROVISIONED ) ) ||
       ( (cdma_provision_state == QCRIL_APP_STATUS_DEPROVISIONED) &&
         (gw_provision_state   == QCRIL_APP_STATUS_NOT_PROVISIONED ) ) )
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

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

int qcril_qmi_prov_all_apps_provisioned(void)
{
  int res                                              = FALSE;
  qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
  qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

  QCRIL_LOG_FUNC_ENTRY();

  gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
  cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

  QCRIL_LOG_INFO("gw_provision_state: %d cdma_provision_state: %d",
                                                gw_provision_state,
                                                cdma_provision_state);

  /* In case of failure as well app provision state will be not provisioned */
  if ( ( (gw_provision_state   == QCRIL_APP_STATUS_PROVISIONED) &&
         (cdma_provision_state == QCRIL_APP_STATUS_PROVISIONED)) ||
       ( (gw_provision_state   == QCRIL_APP_STATUS_PROVISIONED) &&
         (cdma_provision_state == QCRIL_APP_STATUS_NOT_PROVISIONED ) ) ||
       ( (cdma_provision_state == QCRIL_APP_STATUS_PROVISIONED) &&
         (gw_provision_state   == QCRIL_APP_STATUS_NOT_PROVISIONED ) ) )
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

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
int qcril_qmi_prov_is_any_app_being_provisioned(void)
{
  int res                                              = FALSE;
  qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
  qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

  QCRIL_LOG_FUNC_ENTRY();

  gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
  cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

  QCRIL_LOG_INFO("gw_provision_state: %d cdma_provision_state: %d",
                                                gw_provision_state,
                                                cdma_provision_state);

  if ((gw_provision_state   == QCRIL_APP_STATUS_PROVISIONING) ||
      (cdma_provision_state == QCRIL_APP_STATUS_PROVISIONING) ||
      (gw_provision_state   == QCRIL_APP_STATUS_PENDING_PROVISIONING) ||
      (cdma_provision_state == QCRIL_APP_STATUS_PENDING_PROVISIONING) )
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

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
int qcril_qmi_prov_is_any_app_being_deprovisioned(void)
{
  int res                                              = FALSE;
  qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
  qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

  QCRIL_LOG_FUNC_ENTRY();

  gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
  cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

  QCRIL_LOG_INFO("gw_provision_state: %d cdma_provision_state: %d",
                gw_provision_state,
                cdma_provision_state);
  if ((gw_provision_state   == QCRIL_APP_STATUS_DEPROVISIONING) ||
      (cdma_provision_state == QCRIL_APP_STATUS_DEPROVISIONING) ||
      (gw_provision_state   == QCRIL_APP_STATUS_PENDING_DEPROVISIONING) ||
      (cdma_provision_state == QCRIL_APP_STATUS_PENDING_DEPROVISIONING) )
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

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
int qcril_qmi_prov_is_any_app_provisioned()
{
  int res                                              = FALSE;
  qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
  qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

  QCRIL_LOG_FUNC_ENTRY();

  gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
  cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

  QCRIL_LOG_INFO("gw_provision_state: %d cdma_provision_state: %d",
                                                 gw_provision_state,
                                                 cdma_provision_state);

  if ( (gw_provision_state   == QCRIL_APP_STATUS_PROVISIONED) ||
       (cdma_provision_state == QCRIL_APP_STATUS_PROVISIONED) )
  {
    res = TRUE;
  }
  else
  {
    res = FALSE;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

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
int qcril_qmi_prov_get_app_index_for_activation(void)
{
    int app_index                                        = QCRIL_INVALID_APP_INDEX;
    qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
    qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

    QCRIL_LOG_FUNC_ENTRY();

    gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
    cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

    if ( gw_provision_state == QCRIL_APP_STATUS_PENDING_PROVISIONING )
    {
        app_index = qcril_qmi_prov_get_gw_app_index();
    }
    else if ( cdma_provision_state == QCRIL_APP_STATUS_PENDING_PROVISIONING )
    {
        app_index = qcril_qmi_prov_get_cdma_app_index();
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(app_index);
    return app_index;
}

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
int qcril_qmi_prov_get_app_index_for_deactivation(void)
{
    int app_index                                        = QCRIL_INVALID_APP_INDEX;
    qcril_card_app_provision_status gw_provision_state   = QCRIL_APP_STATUS_NOT_PROVISIONED;
    qcril_card_app_provision_status cdma_provision_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

    QCRIL_LOG_FUNC_ENTRY();

    gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
    cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

    if ( gw_provision_state == QCRIL_APP_STATUS_PENDING_DEPROVISIONING )
    {
        app_index = qcril_qmi_prov_get_gw_app_index();
    }
    else if ( cdma_provision_state == QCRIL_APP_STATUS_PENDING_DEPROVISIONING )
    {
        app_index = qcril_qmi_prov_get_cdma_app_index();
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(app_index);
    return app_index;
}

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
void qcril_qmi_prov_check_and_send_prov_resp(RIL_Errno res, qcril_qmi_prov_state_type state)
{
    int                            event = 0;
    int                            send_event = TRUE;
    qcril_qmi_prov_state_type      curr_state = QCRIL_QMI_PROV_STATE_NONE;
    qcril_qmi_prov_status_type     prov_status;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&prov_status, NIL, sizeof(prov_status));
    curr_state = qcril_qmi_prov_get_prov_state();
    switch ( state )
    {
        case QCRIL_QMI_PROV_STATE_FM_START:
            event = QCRIL_EVT_QMI_PROV_FM_START_STATUS;
            break;

        case QCRIL_QMI_PROV_STATE_FM_APPLY:
            event = QCRIL_EVT_QMI_PROV_FM_APPLY_STATUS;
            break;

        case QCRIL_QMI_PROV_STATE_USER_ACTIVATE:
            event = QCRIL_EVT_QMI_PROV_ACTIVATE_SUB_STATUS;
            break;

        case QCRIL_QMI_PROV_STATE_USER_DEACTIVATE:
            event = QCRIL_EVT_QMI_PROV_DEACTIVATE_SUB_STATUS;
            break;

        case QCRIL_QMI_PROV_STATE_CARD_UP:
            event = QCRIL_EVT_QMI_PROV_STATUS_CHANGED;
            break;

        default:
            send_event = FALSE;
            break;
    }

    /* If state is FM_START is success, wait for FM_APPLY before changing state to NONE.
    ** In all other cases move to NONE. */
    if ( (state == curr_state) &&
        !( (state == QCRIL_QMI_PROV_STATE_FM_START) && (res == RIL_E_SUCCESS) ) )
    {
        qcril_qmi_prov_set_prov_state(QCRIL_QMI_PROV_STATE_NONE, NULL);
    }

    QCRIL_LOG_INFO("state %d, event to post %d, should send_event %d", state, event, send_event);
    if ( send_event == TRUE )
    {
        qcril_event_queue ( QCRIL_DEFAULT_INSTANCE_ID,
                            QCRIL_DEFAULT_MODEM_ID,
                            QCRIL_DATA_ON_STACK,
                            event,
                            &res,
                            sizeof(res),
                            (RIL_Token)QCRIL_TOKEN_ID_INTERNAL );
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps_to_deactivate(void)
{
    int                       app_index;
    RIL_Errno                 res = RIL_E_SUCCESS;
    RIL_CardStatus_v6         ril_card_status;
    qcril_instance_id_e_type  instance_id  = qmi_ril_get_process_instance_id();

    /* 3gpp sim app state */
    qcril_card_app_provision_status gw_provision_state;
    /* 3gpp2 sim app state */
    qcril_card_app_provision_status cdma_provision_state;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&ril_card_status, 0, sizeof(ril_card_status));
    gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
    cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

    /* Retrieve card status info */
    if (qcril_uim_direct_get_card_status(instance_id,
                                  &ril_card_status) == RIL_E_SUCCESS)
    {
        /* Mark 3gpp sim app */
        app_index = qcril_qm_prov_retrieve_gw_app_index(&ril_card_status);
        if (app_index != QCRIL_INVALID_APP_INDEX)
        {

            QCRIL_LOG_DEBUG("3gpp app index: %d, 3gpp app state %d",
                             app_index,
                             ril_card_status.applications[app_index].app_state);

            /* Deactivate apps irrespective of current app state.
            UIM should return success immediately if app is already deactivated */
            if ( ril_card_status.applications[app_index].app_state !=
                                                RIL_APPSTATE_UNKNOWN )
            {
                qcril_qmi_prov_set_gw_app_index(app_index);
                qcril_qmi_prov_set_gw_provision_state(QCRIL_APP_STATUS_PENDING_DEPROVISIONING);
            }
        }

        /* Mark 3gpp2 sim apps */
        app_index = qcril_qmi_prov_retrieve_cdma_app_index(&ril_card_status);
        if (app_index != QCRIL_INVALID_APP_INDEX)
        {
            QCRIL_LOG_DEBUG("3gpp2 app index: %d, 3gpp2 app state %d",
                            app_index,
                            ril_card_status.applications[app_index].app_state);

            /* Deactivate apps irrespective of current app state.
            UIM should return success immediately if app is already deactivated */
            if ( ril_card_status.applications[app_index].app_state !=
                                                    RIL_APPSTATE_UNKNOWN )
            {
                qcril_qmi_prov_set_cdma_app_index(app_index);
                qcril_qmi_prov_set_cdma_provision_state(QCRIL_APP_STATUS_PENDING_DEPROVISIONING);
            }
        }
    }
    else
    {
        res = RIL_E_GENERIC_FAILURE;
        QCRIL_LOG_DEBUG("Card status failed to retrieve");
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps_to_activate(void)
{
    qcril_instance_id_e_type  instance_id  = qmi_ril_get_process_instance_id();
    RIL_CardStatus_v6         ril_card_status;
    int                       app_index;
    RIL_Errno                 res = RIL_E_SUCCESS;

    /* 3gpp sim app state */
    qcril_card_app_provision_status gw_provision_state;
    /* 3gpp2 sim app state */
    qcril_card_app_provision_status cdma_provision_state;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&ril_card_status, 0, sizeof( ril_card_status ));
    gw_provision_state   = qcril_qmi_prov_get_gw_provision_state();
    cdma_provision_state = qcril_qmi_prov_get_cdma_provision_state();

    /* retrieve card status info */
    if (qcril_uim_direct_get_card_status(instance_id,
                                  &ril_card_status) == RIL_E_SUCCESS)
    {
        /* Mark 3gpp sim app */
        if ((gw_provision_state == QCRIL_APP_STATUS_DEPROVISIONED) ||
            (gw_provision_state == QCRIL_APP_STATUS_NOT_PROVISIONED))
        {
            app_index = qcril_qm_prov_retrieve_gw_app_index(&ril_card_status);
            if (app_index != QCRIL_INVALID_APP_INDEX)
            {
                QCRIL_LOG_DEBUG("3gpp app index: %d, 3gpp app state %d",
                                 app_index,
                                 ril_card_status.applications[app_index].app_state);

                /* Activate apps irrespective of current app state.
                UIM should return success immediately if app is already activated */
                if ( ril_card_status.applications[app_index].app_state !=
                                                    RIL_APPSTATE_UNKNOWN )
                {
                    qcril_qmi_prov_set_gw_app_index(app_index);
                    qcril_qmi_prov_set_gw_provision_state(QCRIL_APP_STATUS_PENDING_PROVISIONING);

                }
            }
        }
        else
        {
            QCRIL_LOG_DEBUG("3gpp app already or being provisioned");
        }

        /* Mark 3gpp2 sim app */
        if ((cdma_provision_state == QCRIL_APP_STATUS_DEPROVISIONED) ||
            (cdma_provision_state == QCRIL_APP_STATUS_NOT_PROVISIONED))
        {
            app_index = qcril_qmi_prov_retrieve_cdma_app_index(&ril_card_status);
            if (app_index != QCRIL_INVALID_APP_INDEX)
            {
                QCRIL_LOG_DEBUG("3gpp2 app index: %d, 3gpp2 app state %d",
                                 app_index,
                                 ril_card_status.applications[app_index].app_state);

                /* Activate apps irrespective of current app state.
                UIM should return success immediately if app is already activated */
                if ( ril_card_status.applications[app_index].app_state !=
                                                    RIL_APPSTATE_UNKNOWN )
                {
                    qcril_qmi_prov_set_cdma_app_index(app_index);
                    qcril_qmi_prov_set_cdma_provision_state(QCRIL_APP_STATUS_PENDING_PROVISIONING);
                }
            }
        }
        else
        {
            QCRIL_LOG_DEBUG("3gpp2 app already or being provisioned");
        }
    }
    else
    {
        res = RIL_E_GENERIC_FAILURE;
        QCRIL_LOG_DEBUG("Card status failed to retrieve");
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    int index = 0;
    int usim_index = QCRIL_INVALID_APP_INDEX;
    int sim_index = QCRIL_INVALID_APP_INDEX;
    RIL_AppStatus *app_status = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    if (ril_card_status)
    {
        for (index = 0; index < RIL_CARD_MAX_APPS; index++)
        {
            app_status = &ril_card_status->applications[index];

            if(app_status->app_type == RIL_APPTYPE_USIM)
            {
                usim_index = index;
                QCRIL_LOG_INFO( "USIM APP present index - %d", usim_index );
                break;
            }

            if( (sim_index == -1) && (app_status->app_type == RIL_APPTYPE_SIM) )
            {
                sim_index = index;
                QCRIL_LOG_INFO( "SIM APP present index - %d", sim_index );
            }
        }
    }

    if(usim_index != QCRIL_INVALID_APP_INDEX)
    {
        index = usim_index;
    }
    else if(sim_index != QCRIL_INVALID_APP_INDEX)
    {
        index = sim_index;
    }
    else
    {
        index = QCRIL_INVALID_APP_INDEX;
    }

    QCRIL_LOG_INFO( "index - %d", index );
    return index;
}


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
)
{
    int index = 0;
    int csim_index = QCRIL_INVALID_APP_INDEX;
    int ruim_index = QCRIL_INVALID_APP_INDEX;
    RIL_AppStatus *app_status = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    if (ril_card_status)
    {
        for (index = 0; index < RIL_CARD_MAX_APPS; index++)
        {
            app_status = &ril_card_status->applications[index];

            if(app_status->app_type == RIL_APPTYPE_CSIM)
            {
                csim_index = index;
                QCRIL_LOG_INFO( "CSIM APP present index - %d", csim_index );
                break;
            }

            if( (ruim_index == -1) && (app_status->app_type == RIL_APPTYPE_RUIM) )
            {
                ruim_index = index;
                QCRIL_LOG_INFO( "RUIM APP present index - %d", ruim_index );
            }
        }
    }

    if(csim_index != QCRIL_INVALID_APP_INDEX)
    {
        index = csim_index;
    }
    else if(ruim_index != QCRIL_INVALID_APP_INDEX)
    {
        index = ruim_index;
    }
    else
    {
        index = QCRIL_INVALID_APP_INDEX;
    }

    QCRIL_LOG_INFO( "index - %d", index );
    return index;
}

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
)
{
    RIL_Errno                                 res = RIL_E_SUCCESS;
    IxErrnoType                               err_no = E_SUCCESS;
    RIL_SelectUiccSub                         *sub_ptr;
    RIL_SubscriptionType                      sub_num;
    qcril_modem_id_e_type                     modem_id;
    qcril_instance_id_e_type                  instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    qcril_provision_info_type                 *provision_info_ptr;
    qcril_qmi_prov_state_type                 prov_state = QCRIL_QMI_PROV_STATE_NONE;
    qcril_reqlist_public_type                 req_info;
    qcril_request_resp_params_type            resp;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED(ret_ptr);

    modem_id = params_ptr->modem_id;
    memset(&resp, NIL, sizeof(resp));
    memset(&req_info, NIL, sizeof(req_info));
    provision_info_ptr = ( qcril_provision_info_type *) params_ptr->data;

    if( provision_info_ptr != NULL )
    {
        if ( qcril_reqlist_query_by_event( instance_id, modem_id, QCRIL_EVT_CM_ACTIVATE_PROVISION_STATUS, &req_info ) == E_SUCCESS )
        {
          sub_ptr = &req_info.sub.nas.select_uicc_sub;
          if ( ( req_info.request == QCRIL_EVT_RIL_REQUEST_MANUAL_PROVISIONING ) &&
               ( sub_ptr->act_status == RIL_UICC_SUBSCRIPTION_ACTIVATE ) )
          {
              if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_FAILURE )
              {
                  QCRIL_LOG_DEBUG( "RID %d, activate sub failure, slot %d, app_index %d, error_code =%d",
                                                                               instance_id,
                                                                               sub_ptr->slot,
                                                                               sub_ptr->app_index,
                                                                               provision_info_ptr->err_code);

                  if ( provision_info_ptr->err_code == RIL_E_SUBSCRIPTION_NOT_SUPPORTED )
                  {
                      QCRIL_LOG_ERROR( "Incompatible mode preference selected via QMI_NAS_GET_MODE_PREF_REQ " );
                  }
              }
              else if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_IN_PROGRESS )
              {
                  QCRIL_LOG_DEBUG( "RID %d, activate sub in progress, slot %d, app_index %d, session_type %d",
                                                                           instance_id,
                                                                           sub_ptr->slot,
                                                                           sub_ptr->app_index,
                                                                           provision_info_ptr->session_type );
                  // update session type
                  err_no = qcril_reqlist_update_sub_id( instance_id,
                                                        req_info.t,
                                                        provision_info_ptr->session_type );

                  QCRIL_LOG_INFO( ".. qcril_reqlist_update_sub_id res %d", (int) err_no );
              }
              else
              {
                  QCRIL_LOG_DEBUG( "RID %d, UIM activate sub success, slot %d, app_index %d, session_type %d",
                                                                           instance_id,
                                                                           sub_ptr->slot,
                                                                           sub_ptr->app_index,
                                                                           provision_info_ptr->session_type);
                  switch ( provision_info_ptr->session_type )
                  {
                      case QMI_UIM_SESSION_TYPE_TER_GW_PROV:
                      case QMI_UIM_SESSION_TYPE_TER_1X_PROV:
                      case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3:
                      case QMI_UIM_SESSION_TYPE_CARD_SLOT_3:
                          sub_num = RIL_SUBSCRIPTION_3;
                          break;

                      case QMI_UIM_SESSION_TYPE_SEC_GW_PROV:
                      case QMI_UIM_SESSION_TYPE_SEC_1X_PROV:
                      case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2:
                      case QMI_UIM_SESSION_TYPE_CARD_SLOT_2:
                          sub_num = RIL_SUBSCRIPTION_2;
                          break;

                      case QMI_UIM_SESSION_TYPE_PRI_GW_PROV:
                      case QMI_UIM_SESSION_TYPE_PRI_1X_PROV:
                      case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1:
                      case QMI_UIM_SESSION_TYPE_CARD_SLOT_1:
                      default:
                          sub_num = RIL_SUBSCRIPTION_1;
                          break;
                  }
              }

              /* In case of prov fail or success check if we have
              ** any other apps to activate.*/
              if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_FAILURE ||
                   provision_info_ptr->status == QCRIL_PROVISION_STATUS_SUCCESS )
              {
                  qcril_qmi_prov_update_app_provision_status_using_app_index(sub_ptr->app_index,
                                                                             sub_ptr->act_status,
                                                                             provision_info_ptr->err_code);

                  /* Free existing reqlist so that we can send req to UIM
                  ** for another app */
                  qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                                     (void *)(intptr_t)QCRIL_SUB_PROVISION_INTERNAL_TOKEN);

                  if ( qcril_qmi_prov_is_any_app_being_provisioned() )
                  {
                      // this function might return failure due to get mode pref failure.
                      // However, app status will be updated when this function returns with failure.
                      res = qcril_qmi_prov_activate_deactivate_app( req_info.t,
                                                                    req_info.request,
                                                                    RIL_UICC_SUBSCRIPTION_ACTIVATE );
                  }

                  // If second app activation fails or no other app to activate send response.
                  if ( res == RIL_E_GENERIC_FAILURE ||
                       qcril_qmi_prov_is_any_app_being_provisioned() == FALSE )
                  {
                      prov_state = qcril_qmi_prov_get_prov_state();

                      /* No pending apps, check if any app provisioned. If so, return SUCCESS.*/
                      if ( qcril_qmi_prov_is_any_app_provisioned() )
                      {
                          qcril_qmi_prov_check_and_send_prov_resp(RIL_E_SUCCESS, prov_state);
                      }
                      else
                      {
                          /* No pending apps, none of the apps are provisioned.Send failure */
                          qcril_qmi_prov_update_app_provision_status_using_app_index( sub_ptr->app_index,
                                                                                      sub_ptr->act_status,
                                                                                      RIL_E_GENERIC_FAILURE);
                          qcril_qmi_prov_check_and_send_prov_resp(RIL_E_GENERIC_FAILURE, prov_state);
                      }
                  }
              }
          }

        }
    }
    else
    {
        QCRIL_LOG_FATAL("CHECK FAILED");
    }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_prov_subs_activate_followup

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
)
{
    RIL_Errno                            res = RIL_E_SUCCESS;
    RIL_SelectUiccSub                    *sub_ptr;
    RIL_SubscriptionType                 sub_num;
    qcril_modem_id_e_type                modem_id;
    qcril_instance_id_e_type             instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    qcril_provision_info_type            *provision_info_ptr;
    qcril_qmi_prov_state_type            prov_state = QCRIL_QMI_PROV_STATE_NONE;
    qcril_reqlist_public_type            req_info;
    qcril_request_resp_params_type       resp;

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED(ret_ptr);

    modem_id = params_ptr->modem_id;
    memset(&resp, NIL, sizeof(resp));
    memset(&req_info, NIL, sizeof(req_info));
    provision_info_ptr = ( qcril_provision_info_type *) params_ptr->data;

    if( provision_info_ptr != NULL )
    {
        if ( qcril_reqlist_query_by_event( instance_id, modem_id, QCRIL_EVT_CM_DEACTIVATE_PROVISION_STATUS, &req_info ) == E_SUCCESS )
        {
            sub_ptr = &req_info.sub.nas.select_uicc_sub;
            if ( ( req_info.request == QCRIL_EVT_RIL_REQUEST_MANUAL_PROVISIONING ) &&
                 ( sub_ptr->act_status == RIL_UICC_SUBSCRIPTION_DEACTIVATE ) )
            {
              if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_FAILURE )
              {

                QCRIL_LOG_DEBUG( "RID %d, UIM deactivate sub failure, slot %d, app_index %d",
                                                                         instance_id,
                                                                         sub_ptr->slot,
                                                                         sub_ptr->app_index);
              }
              else if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_SUCCESS )
              {
                QCRIL_LOG_DEBUG( "RID %d, UIM deactivate sub success, slot %d, app_index %d, session_type %d",
                                                                             instance_id,
                                                                             sub_ptr->slot,
                                                                             sub_ptr->app_index,
                                                                             provision_info_ptr->session_type);

                switch ( provision_info_ptr->session_type )
                {
                    case QMI_UIM_SESSION_TYPE_TER_GW_PROV:
                    case QMI_UIM_SESSION_TYPE_TER_1X_PROV:
                    case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3:
                    case QMI_UIM_SESSION_TYPE_CARD_SLOT_3:
                        sub_num = RIL_SUBSCRIPTION_3;
                        break;

                    case QMI_UIM_SESSION_TYPE_SEC_GW_PROV:
                    case QMI_UIM_SESSION_TYPE_SEC_1X_PROV:
                    case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2:
                    case QMI_UIM_SESSION_TYPE_CARD_SLOT_2:
                        sub_num = RIL_SUBSCRIPTION_2;
                        break;

                    case QMI_UIM_SESSION_TYPE_PRI_GW_PROV:
                    case QMI_UIM_SESSION_TYPE_PRI_1X_PROV:
                    case QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1:
                    case QMI_UIM_SESSION_TYPE_CARD_SLOT_1:
                    default:
                        sub_num = RIL_SUBSCRIPTION_1;
                        break;
                }

              }

              if ( provision_info_ptr->status == QCRIL_PROVISION_STATUS_FAILURE ||
                   provision_info_ptr->status == QCRIL_PROVISION_STATUS_SUCCESS )
              {

                  qcril_qmi_prov_update_app_provision_status_using_app_index( sub_ptr->app_index,
                                                                              sub_ptr->act_status,
                                                                              provision_info_ptr->err_code);
                  /* Free existing reqlist so that we can send req to UIM
                  ** for another app */
                  qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                                     (void *)(intptr_t)QCRIL_SUB_PROVISION_INTERNAL_TOKEN);

                  /* In case of deprov fail or success check if we have
                  ** any other apps to deactivate.*/
                  if ( qcril_qmi_prov_is_any_app_being_deprovisioned() )
                  {
                      res = qcril_qmi_prov_activate_deactivate_app( req_info.t,
                                                                    req_info.request,
                                                                    RIL_UICC_SUBSCRIPTION_DEACTIVATE );
                  }

                  // if second app deactivation fails or no other app to deactivate send response.
                  if ( res == RIL_E_GENERIC_FAILURE ||
                       qcril_qmi_prov_is_any_app_being_deprovisioned() == FALSE )
                  {
                    prov_state = qcril_qmi_prov_get_prov_state();

                    /* send sucess if all apps are deprovisioned, failure otherwise.*/
                    if ( qcril_qmi_prov_all_apps_deprovisioned() )
                    {
                        qcril_qmi_prov_check_and_send_prov_resp(RIL_E_SUCCESS, prov_state);
                    }
                    else
                    {
                        qcril_qmi_prov_check_and_send_prov_resp(RIL_E_GENERIC_FAILURE, prov_state);
                    }
                  }
              }
            }
        }
        else
        {
            QCRIL_LOG_DEBUG( "Late Deactivate provision status event" );
        }
    }
    QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_prov_subs_deactivate_followup

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
void qcril_qmi_prov_reset_prov_pref_info(int reset_state)
{
    int slot = qmi_ril_get_sim_slot();

    QCRIL_LOG_FUNC_ENTRY();

    prov_common_cache.prov_info[slot].gw_app_index          = QCRIL_INVALID_APP_INDEX;
    prov_common_cache.prov_info[slot].cdma_app_index        = QCRIL_INVALID_APP_INDEX;
    prov_common_cache.prov_info[slot].user_prov_pref        = QCRIL_INVALID_PROV_PREF;
    prov_common_cache.prov_info[slot].curr_prov_status      = QCRIL_INVALID_PROV_PREF;
    prov_common_cache.prov_info[slot].gw_provision_state    =
                                                     QCRIL_APP_STATUS_NOT_PROVISIONED;
    prov_common_cache.prov_info[slot].cdma_provision_state  =
                                                    QCRIL_APP_STATUS_NOT_PROVISIONED;

    memset(prov_common_cache.prov_info[slot].iccid, 0, sizeof(prov_common_cache.prov_info[slot].iccid));

    if ( reset_state )
    {
        prov_common_cache.prov_info[slot].state             = QCRIL_QMI_PROV_STATE_NONE;

        // Only during init or resuming from SSR need to reset state, clear any pending manual prov requests.
        qcril_reqlist_free(QCRIL_DEFAULT_INSTANCE_ID,
                           (void *)(intptr_t)QCRIL_SUB_PROVISION_INTERNAL_TOKEN);
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
)
{
    int      slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].curr_prov_status = curr_prov_status;

}

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
int qcril_qmi_prov_get_curr_sub_prov_status(void)
{
    int       slot = qmi_ril_get_sim_slot();
    int       curr_prov_status = QCRIL_INVALID_PROV_PREF;

    curr_prov_status = prov_common_cache.prov_info[slot].curr_prov_status;

    return curr_prov_status;
}

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
int qcril_qmi_prov_get_user_pref_from_cache(void)
{
    int        slot = qmi_ril_get_sim_slot();
    int        user_prov_pref = QCRIL_INVALID_PROV_PREF;

    user_prov_pref = prov_common_cache.prov_info[slot].user_prov_pref;

    return user_prov_pref;
}

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
void qcril_qmi_prov_update_user_pref_cache(RIL_UiccSubActStatus new_user_pref)
{
    int      slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].user_prov_pref = new_user_pref;

}

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
void qcril_qmi_prov_set_gw_provision_state(qcril_card_app_provision_status prov_state)
{
    int       slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].gw_provision_state = prov_state;
}

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
void qcril_qmi_prov_set_cdma_provision_state(qcril_card_app_provision_status prov_state)
{
    int       slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].cdma_provision_state = prov_state;
}

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
qcril_card_app_provision_status qcril_qmi_prov_get_gw_provision_state()
{
    int       slot = qmi_ril_get_sim_slot();
    int       prov_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

    prov_state = prov_common_cache.prov_info[slot].gw_provision_state;

    return prov_state;
}


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
qcril_card_app_provision_status qcril_qmi_prov_get_cdma_provision_state()
{
    int       slot = qmi_ril_get_sim_slot();
    int       prov_state = QCRIL_APP_STATUS_NOT_PROVISIONED;

    prov_state = prov_common_cache.prov_info[slot].cdma_provision_state;

    return prov_state;
}

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
int qcril_qmi_prov_get_gw_app_index()
{
    int       slot = qmi_ril_get_sim_slot();
    int       app_index = QCRIL_INVALID_APP_INDEX;

    app_index = prov_common_cache.prov_info[slot].gw_app_index;

    return app_index;
}

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
int qcril_qmi_prov_get_cdma_app_index()
{
    int      slot = qmi_ril_get_sim_slot();
    int      app_index = QCRIL_INVALID_APP_INDEX;

    app_index = prov_common_cache.prov_info[slot].cdma_app_index;

    return app_index;
}

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
void qcril_qmi_prov_set_gw_app_index(int app_index)
{
    int        slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].gw_app_index = app_index;
}

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
int qcril_qmi_prov_set_cdma_app_index(int app_index)
{
    int        slot = qmi_ril_get_sim_slot();
    prov_common_cache.prov_info[slot].cdma_app_index = app_index;
    return app_index;
}

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
)
{
    int                             slot = qmi_ril_get_sim_slot();
    int                             gw_app_index = QCRIL_INVALID_APP_INDEX;
    int                             cdma_app_index = QCRIL_INVALID_APP_INDEX;
    int                             *app_index_ptr = NULL;
    qcril_card_app_provision_status *provision_status_ptr = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    gw_app_index   = qcril_qmi_prov_get_gw_app_index();
    cdma_app_index = qcril_qmi_prov_get_cdma_app_index();

    QCRIL_LOG_DEBUG("provision_action: %d, index: %d, result: %d",
                                         provision_act, index, res);

    if ((gw_app_index != -1) && (gw_app_index == index))
    {
        provision_status_ptr =
            &prov_common_cache.prov_info[slot].gw_provision_state;
        app_index_ptr = &prov_common_cache.prov_info[slot].gw_app_index;
        QCRIL_LOG_DEBUG("retrieve GW privision status %d",
                        *provision_status_ptr);
    }
    else if ((cdma_app_index != -1) && (cdma_app_index == index))
    {
        provision_status_ptr =
            &prov_common_cache.prov_info[slot].cdma_provision_state;
        app_index_ptr = &prov_common_cache.prov_info[slot].cdma_app_index;
        QCRIL_LOG_DEBUG("retrieve CDMA privision status %d",
            *provision_status_ptr);
    }

    if (provision_status_ptr && app_index_ptr)
    {
        if (provision_act == RIL_UICC_SUBSCRIPTION_ACTIVATE)
        {
            if (res != RIL_E_SUCCESS)
            {
                /* Important Note::Failure to activate will be marked as not_provisioned */
                *app_index_ptr = -1;
                *provision_status_ptr = QCRIL_APP_STATUS_NOT_PROVISIONED;
            }
            else
            {
                if (*provision_status_ptr == QCRIL_APP_STATUS_PENDING_PROVISIONING)
                {
                    *provision_status_ptr = QCRIL_APP_STATUS_PROVISIONING;
                }
                else if (*provision_status_ptr == QCRIL_APP_STATUS_PROVISIONING)
                {
                    *provision_status_ptr = QCRIL_APP_STATUS_PROVISIONED;
                }
            }
        }
        else if (provision_act == RIL_UICC_SUBSCRIPTION_DEACTIVATE)
        {
            if (res != RIL_E_SUCCESS)
            {
                 /* Important Note::Failure to deactivate will be marked as provisioned
                 ** as we try to deactivate only if app is provisioned earlier. */
                *provision_status_ptr = QCRIL_APP_STATUS_PROVISIONED;
            }
            else
            {
                if (*provision_status_ptr == QCRIL_APP_STATUS_PENDING_DEPROVISIONING)
                {
                    *provision_status_ptr = QCRIL_APP_STATUS_DEPROVISIONING;
                }
                else if (*provision_status_ptr == QCRIL_APP_STATUS_DEPROVISIONING)
                {
                    *provision_status_ptr = QCRIL_APP_STATUS_DEPROVISIONED;
                }

            }
        }

        /* if any of the app is provisioned mark curr_prov_status as Activated
        ** else curr_sub_prov_status will remain as invalid.*/
        if ( ( prov_common_cache.prov_info[slot].gw_provision_state ==
                                            QCRIL_APP_STATUS_PROVISIONED ) ||
             ( prov_common_cache.prov_info[slot].cdma_provision_state ==
                                                         QCRIL_APP_STATUS_PROVISIONED ) )
        {
            qcril_qmi_prov_set_curr_sub_prov_status(RIL_UICC_SUBSCRIPTION_ACTIVATE);
        }
        /* We will hit this if any app is not provisioned, mark status as
        ** deactivated even if one app is depropvisioned */
        else if ( ( prov_common_cache.prov_info[slot].gw_provision_state ==
                                            QCRIL_APP_STATUS_DEPROVISIONED ) ||
                  ( prov_common_cache.prov_info[slot].cdma_provision_state ==
                                                              QCRIL_APP_STATUS_DEPROVISIONED ) )
        {
            qcril_qmi_prov_set_curr_sub_prov_status(RIL_UICC_SUBSCRIPTION_DEACTIVATE);
        }

        QCRIL_LOG_DEBUG("provision status %d", *provision_status_ptr);
    }

}

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
qcril_qmi_prov_state_type qcril_qmi_prov_get_prov_state(void)
{
    int        slot = qmi_ril_get_sim_slot();
    int        state = QCRIL_QMI_PROV_STATE_NONE;

    state = prov_common_cache.prov_info[slot].state;

    return state;
}

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
RIL_UiccSubActStatus qcril_qmi_prov_get_act_status_from_state(int state)
{
    int          user_prov_pref = QCRIL_INVALID_PROV_PREF;
    int          act_status = RIL_UICC_SUBSCRIPTION_ACTIVATE;

    user_prov_pref = qcril_qmi_prov_get_user_pref_from_cache();

    switch(state)
    {
        case QCRIL_QMI_PROV_STATE_USER_ACTIVATE:
            act_status = RIL_UICC_SUBSCRIPTION_ACTIVATE;
            break;

        case QCRIL_QMI_PROV_STATE_FM_START:
        case QCRIL_QMI_PROV_STATE_USER_DEACTIVATE:
            act_status = RIL_UICC_SUBSCRIPTION_DEACTIVATE;
            break;

        /* FM apply/card status changed, decide activate/deactivate
        ** based on user preference.*/
        case QCRIL_QMI_PROV_STATE_FM_APPLY:
        case QCRIL_QMI_PROV_STATE_CARD_UP:
            act_status = user_prov_pref;
            break;
    }

    return act_status;
}

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
)
{
    int                              slot = qmi_ril_get_sim_slot();
    RIL_Errno                        res = RIL_E_GENERIC_FAILURE;
    qcril_qmi_prov_state_type        curr_state = QCRIL_QMI_PROV_STATE_NONE;

    QCRIL_LOG_FUNC_ENTRY();

    curr_state = qcril_qmi_prov_get_prov_state();

    QCRIL_LOG_INFO("Current state - %d, req state - %d", curr_state, state);

    do
    {

        switch( curr_state )
        {
            case QCRIL_QMI_PROV_STATE_NONE:
                prov_common_cache.prov_info[slot].state = state;
                res = RIL_E_SUCCESS;
                break;

            /* Only FM_APPLY/FM_START/NONE are valid transitions from FM_START
               FM_START might come when already in FM_START state if flex map
               failed on other RILD.*/
            case QCRIL_QMI_PROV_STATE_FM_START:
                if ( state == QCRIL_QMI_PROV_STATE_FM_APPLY ||
                     state == QCRIL_QMI_PROV_STATE_NONE     ||
                     state == QCRIL_QMI_PROV_STATE_FM_START )
                {
                    prov_common_cache.prov_info[slot].state = state;
                    res = RIL_E_SUCCESS;
                }
                break;

            /* Only Valid transition from curr_state is to NONE */
            case QCRIL_QMI_PROV_STATE_FM_APPLY:
            case QCRIL_QMI_PROV_STATE_USER_ACTIVATE:
            case QCRIL_QMI_PROV_STATE_USER_DEACTIVATE:
                if ( state == QCRIL_QMI_PROV_STATE_NONE )
                {
                    prov_common_cache.prov_info[slot].state = state;
                    res = RIL_E_SUCCESS;
                }
                break;

            /* If provisioning currently on going due card UP, defer FM_START request. */
            case QCRIL_QMI_PROV_STATE_CARD_UP:
                if ( state == QCRIL_QMI_PROV_STATE_NONE )
                {
                    prov_common_cache.prov_info[slot].state = state;
                    res = RIL_E_SUCCESS;
                }
                else if ( ( state == QCRIL_QMI_PROV_STATE_FM_START ) &&  prov_action )
                {
                    *prov_action = QCRIL_QMI_PROV_ACTION_DEFER;
                }
                break;

             default:
                QCRIL_LOG_INFO("should not be here curr_state - %d", curr_state);
                break;

        }


    } while(FALSE);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
RIL_Errno qcril_qmi_prov_evaluate_and_mark_apps(void)
{
    int                              user_prov_pref = QCRIL_INVALID_PROV_PREF;
    RIL_Errno                        res = RIL_E_SUCCESS;
    qcril_qmi_prov_state_type        curr_state;

    QCRIL_LOG_FUNC_ENTRY();

    curr_state = qcril_qmi_prov_get_prov_state();

    QCRIL_LOG_INFO("Current state - %d", curr_state);

    do
    {
        user_prov_pref = qcril_qmi_prov_get_user_pref_from_cache();

        switch(curr_state)
        {
            case QCRIL_QMI_PROV_STATE_USER_ACTIVATE:
                res = qcril_qmi_prov_evaluate_and_mark_apps_to_activate();
                break;

            case QCRIL_QMI_PROV_STATE_FM_START:
            case QCRIL_QMI_PROV_STATE_USER_DEACTIVATE:
                res = qcril_qmi_prov_evaluate_and_mark_apps_to_deactivate();
                break;

            /* FM apply/card status changed, decide activate/deactivate
            ** based on user preference.*/
            case QCRIL_QMI_PROV_STATE_FM_APPLY:
            case QCRIL_QMI_PROV_STATE_CARD_UP:
                if ( user_prov_pref == RIL_UICC_SUBSCRIPTION_ACTIVATE )
                {
                    res = qcril_qmi_prov_evaluate_and_mark_apps_to_activate();
                }
                else if ( user_prov_pref == RIL_UICC_SUBSCRIPTION_DEACTIVATE )
                {
                    res = qcril_qmi_prov_evaluate_and_mark_apps_to_deactivate();
                }
                else
                {
                    QCRIL_LOG_INFO("Invalid user provisioning preference - %d", user_prov_pref);
                }
                break;
            default:
                break;
        }

    } while(FALSE);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char *iccid_ptr;
    int slot = qmi_ril_get_sim_slot();
    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    iccid_ptr = params_ptr->data;

    if ( params_ptr->data != NULL && params_ptr->datalen > 0 )
    {
        strlcpy( prov_common_cache.prov_info[slot].iccid,
                 iccid_ptr,
                 QMI_DMS_UIM_ID_MAX_V01 + 1 );

        QCRIL_LOG_INFO("updated iccid - %s", prov_common_cache.prov_info[slot].iccid);
        qcril_qmi_nas_fetch_user_prov_pref_from_database();
    }

    QCRIL_LOG_FUNC_RETURN();
}

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
void qcril_qmi_prov_get_iccid_from_cache(char *iccid)
{
    int slot = qmi_ril_get_sim_slot();
    if(NULL != iccid)
    {
        strlcpy(iccid,
                prov_common_cache.prov_info[slot].iccid,
                QMI_DMS_UIM_ID_MAX_V01 + 1 );
    }
}

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
void qcril_qmi_prov_fill_prov_preference_info(RIL_SubProvStatus *payload)
{
    if(NULL != payload)
    {
        memset(payload, 0, sizeof(*payload));
        payload->user_preference = qcril_qmi_prov_get_user_pref_from_cache();
        payload->current_sub_preference = qcril_qmi_prov_get_curr_sub_prov_status();
    }
}

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
RIL_Errno qcril_qmi_prov_update_db_with_user_pref(int user_prov_pref)
{
    int                  prev_userpref = QCRIL_INVALID_PROV_PREF;
    char                 iccid[ QMI_DMS_UIM_ID_MAX_V01 + 1 ];
    RIL_Errno            res = RIL_E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();

    qcril_qmi_prov_get_iccid_from_cache(iccid);
    res = qcril_db_query_user_pref_from_prov_table(iccid,&prev_userpref);

    do
    {
        if ( res != RIL_E_SUCCESS )
        {
            QCRIL_LOG_INFO("error while querying user preference");
            break;
        }

        if ( prev_userpref == QCRIL_INVALID_PROV_PREF )
        {
            QCRIL_LOG_INFO("iccid %s not present", iccid);
            qcril_db_insert_new_iccid_into_prov_table(iccid,user_prov_pref);
        }
        else if ( prev_userpref != user_prov_pref )
        {
            res = qcril_db_update_user_pref_prov_table(iccid,user_prov_pref);
            if ( res == RIL_E_SUCCESS )
            {
                /* update cache as well.*/
                qcril_qmi_prov_update_user_pref_cache(user_prov_pref);
                QCRIL_LOG_INFO(" iccid - %s prev user pref - %d, new user pref - %d", iccid, prev_userpref, user_prov_pref);
            }
        }

    }while(0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}


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
int qcril_qmi_nas_fetch_user_prov_pref_from_database(void)
{

    int                pref = QCRIL_INVALID_PROV_PREF;
    char               iccid[QMI_DMS_UIM_ID_MAX_V01 + 1];
    RIL_Errno          res = RIL_E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();

    memset(iccid, NIL,sizeof(iccid));
    qcril_qmi_prov_get_iccid_from_cache(iccid);

    res = qcril_db_query_user_pref_from_prov_table(iccid,&pref);

    QCRIL_LOG_INFO("user preference from db - %d", pref);

    if ( pref == QCRIL_INVALID_PROV_PREF )
    {
        /* for new card, set user preference by default as activate.*/
        QCRIL_LOG_INFO("New card inserted... set user preference to activate");
        pref = RIL_UICC_SUBSCRIPTION_ACTIVATE;
        qcril_qmi_prov_update_db_with_user_pref(RIL_UICC_SUBSCRIPTION_ACTIVATE);
    }

    qcril_qmi_prov_update_user_pref_cache(pref);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(pref);
    return pref;
}


