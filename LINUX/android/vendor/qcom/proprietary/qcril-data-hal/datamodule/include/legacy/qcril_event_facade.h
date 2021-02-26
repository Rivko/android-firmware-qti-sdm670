/**
 Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "framework/legacy.h"
#include "framework/qcril_event.h"

#include "DataCommon.h"
#include "data_system_determination_v01.h"
#include "wireless_data_service_v01.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                        EVENTS

===========================================================================*/

QCRIL_DECLARE_LEGACY_EVENT(DATA_COMMAND_CALLBACK, qcril_data_command_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_EVENT_CALLBACK, qcril_data_event_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_ASYNC_CALLBACK, qcril_data_async_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_SET_RAT_PREF_CALLBACK, qcril_data_set_rat_pref_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_LQE_EVENT_CALLBACK, qcril_data_lqe_event_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_WDS_EVENT_CALLBACK, qcril_data_wds_event_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_DSD_EVENT_CALLBACK, qcril_data_dsd_event_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_WDS_SUSPEND_CALLBACK, qcril_data_qmi_wds_suspend_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_WDS_RESUME_CALLBACK, qcril_data_qmi_wds_resume_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_DSD_SUSPEND_CALLBACK, qcril_data_qmi_dsd_suspend_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_DSD_RESUME_CALLBACK, qcril_data_qmi_dsd_resume_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(DATA_KEEPALIVE_EVENT_CALLBACK, qcril_data_keepalive_event_hdlr, &QCRIL_GRP_DATA, QMI_RIL_ALLOWED_IN_ALL_STATES);

//QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_SETUP_DATA_CALL, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_DEACTIVATE_DATA_CALL, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_DATA_CALL_LIST, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_GET_DATA_CALL_PROFILE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_SET_DATA_PROFILE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_START_LCE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_STOP_LCE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_PULL_LCEDATA, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_START_KEEPALIVE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(RIL_REQUEST_STOP_KEEPALIVE, NULL, &QCRIL_GRP_ANDROID_RIL_REQ, QMI_RIL_ALLOWED_IN_ALL_STATES);


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================================
  FUNCTION:  qcril_data_set_pref_data_tech

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_pref_data_tech
(
  qcril_instance_id_e_type        instance_id,
  qcril_data_pref_data_tech_e_type pref_data_tech
);

/*=========================================================================
  FUNCTION:  qcril_data_set_dsd_sys_status

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup. This is
    called upon receipt of
    QMI_DSD_REPORT_SYSTEM_STATUS_IND_V01/QMI_DSD_GET_SYSTEM_STATUS_RESP_V01.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_dsd_sys_status
(
  dsd_system_status_ind_msg_v01 *dsd_system_status
);

/*=========================================================================
  FUNCTION:  qcril_data_set_data_sys_status

===========================================================================*/
/*!
    @brief
    Set the preferred data technology used for data call setup. This is
    newer version of qcril_arb_set_pref_data_tech, we pass over all info
    returned from modem to upper layer

    @return
    None
*/
/*=========================================================================*/
void qcril_data_set_data_sys_status
(
  qcril_instance_id_e_type          instance_id,
  wds_data_system_status_type_v01 * data_sys_status
);

/*===========================================================================

  FUNCTION:  qcril_data_client_notify

===========================================================================*/
/*!
    @brief
    Notify data call status

    @return
    None
*/
/*=========================================================================*/
void qcril_data_client_notify
(
  qcril_data_net_evt_t      evt
);

/*===========================================================================

  FUNCTION:  qcril_data_new_dds_info

===========================================================================*/
/*!
    @brief
    Notify new DDS information

    @return
    None
*/
/*=========================================================================*/
void qcril_data_new_dds_info
(
  DDSSubIdInfo sub_info
);

/*===========================================================================

  FUNCTION:  qcril_data_dds_status_followup

===========================================================================*/
/*!
    @brief
    Notify DDS switch followup status

    @return
    None
*/
/*=========================================================================*/
void qcril_data_dds_status_followup
(
  DDSStatusInfo * status
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_enable_data_con

===========================================================================*/
/*!
    @brief
    Notify embms enable data response

    @return
    None
*/
/*=========================================================================*/
void qcril_data_embms_enable_data_con
(
  qcril_embms_enable_response_payload_type *payload
);

/*===========================================================================

  FUNCTION:  qcril_data_embms_disable_data_ind

===========================================================================*/
/*!
    @brief
    Notify embms disable indication

    @return
    None
*/
/*=========================================================================*/
void qcril_data_embms_disable_data_ind
(
  qcril_embms_disable_indication_payload_type *payload
);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------*/
#ifdef QCRIL_DATA_MODULE_DEV

QCRIL_DECLARE_LEGACY_EVENT(HOOK_GET_DATA_CALL_PROFILE, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_DATA_GO_DORMANT, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_ACTIVATE_TMGI, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_DEACTIVATE_TMGI, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_ACTIVE_TMGI, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_SAI_LIST, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_OSS_WARNING, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_AVAILABLE_TMGI, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_EMBMS_UNSOL_CONTENT_DESC_CONTROL, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_DECLARE_LEGACY_EVENT(HOOK_UNSOL_OPERATOR_RESERVED_PCO, NULL, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);

#endif /* QCRIL_DATA_MODULE_DEV */


