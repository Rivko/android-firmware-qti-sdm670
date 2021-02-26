/******************************************************************************
#  Copyright (c) 2014, 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_ims_msg_meta.c
  @brief   qcril qmi - ims message meta info

  DESCRIPTION
    Handles ims message meta info related functions.

******************************************************************************/

#include<vector>
#include "qcril_qmi_ims_msg_meta.h"

typedef struct
{
    ims_MsgId msg_id;
    ims_MsgType msg_type;
    const pb_field_t* msg_pb_fields;
    size_t msg_req_size;
    qcril_evt_e_type msg_event;
    const char* msg_log_str;
} qcril_qmi_ims_msg_meta_type;

static std::vector<qcril_qmi_ims_msg_meta_type> &get_meta()
{
    static std::vector<qcril_qmi_ims_msg_meta_type> the_meta;

    return the_meta;
}

//===========================================================================
// qcril_qmi_ims_init_msg_meta
//===========================================================================
void qcril_qmi_ims_init_msg_meta()
{
    std::vector<qcril_qmi_ims_msg_meta_type> &qcril_qmi_ims_msg_meta_data = get_meta();
    // Requests and Response
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, ims_MsgType_REQUEST,  NULL,                    0,                        QCRIL_EVT_IMS_SOCKET_IMS_REGISTRATION_STATE, "IMS_REGISTRATION_STATE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_IMS_REGISTRATION_STATE, ims_MsgType_RESPONSE, ims_Registration_fields, sizeof(ims_Registration), QCRIL_EVT_NONE,                              "IMS_REGISTRATION_STATE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DIAL,                   ims_MsgType_REQUEST,  ims_Dial_fields,         sizeof(ims_Dial),         QCRIL_EVT_IMS_SOCKET_REQ_DIAL,               "IMS_DIAL REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DIAL,                   ims_MsgType_RESPONSE, NULL,                    0,                        QCRIL_EVT_NONE,                              "IMS_DIAL RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_ANSWER, ims_MsgType_REQUEST,  ims_Answer_fields, sizeof(ims_Answer), QCRIL_EVT_IMS_SOCKET_REQ_ANSWER, "IMS_ANSWER REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_ANSWER, ims_MsgType_RESPONSE, NULL,              0,                  QCRIL_EVT_NONE,                  "IMS_ANSWER RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP, ims_MsgType_REQUEST,  ims_Hangup_fields, sizeof(ims_Hangup), QCRIL_EVT_IMS_SOCKET_REQ_HANGUP, "IMS_HANGUP REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP, ims_MsgType_RESPONSE, NULL,              0,                  QCRIL_EVT_NONE,                  "IMS_HANGUP RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_LAST_CALL_FAIL_CAUSE, ims_MsgType_REQUEST,  NULL,                             0,                                 QCRIL_EVT_IMS_SOCKET_REQ_LAST_CALL_FAIL_CAUSE, "IMS_LAST_CALL_FAIL_CAUSE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_LAST_CALL_FAIL_CAUSE, ims_MsgType_RESPONSE, ims_CallFailCauseResponse_fields, sizeof(ims_CallFailCauseResponse), QCRIL_EVT_NONE,                                "IMS_LAST_CALL_FAIL_CAUSE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_CURRENT_CALLS,    ims_MsgType_REQUEST,  NULL,                             0,                                 QCRIL_EVT_IMS_SOCKET_REQ_GET_CURRENT_CALLS,    "IMS_GET_CURRENT_CALLS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_CURRENT_CALLS,    ims_MsgType_RESPONSE, ims_CallList_fields,              sizeof(ims_CallList),              QCRIL_EVT_NONE,                                "IMS_GET_CURRENT_CALLS RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP_WAITING_OR_BACKGROUND,        ims_MsgType_REQUEST,  NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_WAITING_OR_BACKGROUND,        "IMS_HANGUP_WAITING_OR_BACKGROUND REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP_WAITING_OR_BACKGROUND,        ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE,                                               "IMS_HANGUP_WAITING_OR_BACKGROUND RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND, ims_MsgType_REQUEST,  NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_FOREGROUND_RESUME_BACKGROUND, "IMS_HANGUP_FOREGROUND_RESUME_BACKGROUND REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND, ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE,                                               "IMS_HANGUP_FOREGROUND_RESUME_BACKGROUND RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, ims_MsgType_REQUEST,  ims_SwitchWaitingOrHoldingAndActive_fields, sizeof(ims_SwitchWaitingOrHoldingAndActive), QCRIL_EVT_IMS_SOCKET_REQ_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, "IMS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, ims_MsgType_RESPONSE, NULL,                                       0,                                                  QCRIL_EVT_NONE,                                                "IMS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_CONFERENCE,                           ims_MsgType_REQUEST,  NULL,                                       0,                                                  QCRIL_EVT_IMS_SOCKET_REQ_CONFERENCE,                           "IMS_CONFERENCE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_CONFERENCE,                           ims_MsgType_RESPONSE, ims_SipErrorInfo_fields,                    sizeof(ims_SipErrorInfo),                           QCRIL_EVT_NONE,                                                "IMS_CONFERENCE RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, ims_MsgType_REQUEST,  NULL,            0,                QCRIL_EVT_IMS_SOCKET_REQ_EXIT_ECBM, "IMS_EXIT_ECBM REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, ims_MsgType_RESPONSE, NULL,            0,                QCRIL_EVT_NONE,                     "IMS_EXIT_ECBM RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER, ims_MsgType_REQUEST, ims_ExplicitCallTransfer_fields, sizeof(ims_ExplicitCallTransfer), QCRIL_EVT_IMS_SOCKET_REQ_EXPLICIT_CALL_TRANSFER, "IMS_EXPLICIT_CALL_TRANSFER REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER, ims_MsgType_RESPONSE, NULL,            0,                QCRIL_EVT_NONE,                     "IMS_EXPLICIT_CALL_TRANSFER RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF,                         ims_MsgType_REQUEST,  ims_Dtmf_fields, sizeof(ims_Dtmf), QCRIL_EVT_IMS_SOCKET_REQ_DTMF,      "IMS_DTMF REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF,                         ims_MsgType_RESPONSE, NULL,            0,                QCRIL_EVT_NONE,                     "IMS_DTMF RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF_START, ims_MsgType_REQUEST,  ims_Dtmf_fields, sizeof(ims_Dtmf), QCRIL_EVT_IMS_SOCKET_REQ_DTMF_START, "IMS_DTMF_START REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF_START, ims_MsgType_RESPONSE, NULL,            0,                QCRIL_EVT_NONE,                      "IMS_DTMF_START RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF_STOP,  ims_MsgType_REQUEST,  NULL,            0,                QCRIL_EVT_IMS_SOCKET_REQ_DTMF_STOP,  "IMS_DTMF_STOP REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DTMF_STOP,  ims_MsgType_RESPONSE, NULL,            0,                QCRIL_EVT_NONE,                      "IMS_DTMF_STOP RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_MODIFY_CALL_INITIATE, ims_MsgType_REQUEST,  ims_CallModify_fields, sizeof(ims_CallModify), QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_INITIATE, "IMS_MODIFY_CALL_INITIATE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_MODIFY_CALL_INITIATE, ims_MsgType_RESPONSE, NULL,                  0,                      QCRIL_EVT_NONE,                                "IMS_MODIFY_CALL_INITIATE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM,  ims_MsgType_REQUEST,  ims_CallModify_fields, sizeof(ims_CallModify), QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_CONFIRM,  "IMS_MODIFY_CALL_CONFIRM REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM,  ims_MsgType_RESPONSE, NULL,                  0,                      QCRIL_EVT_NONE,                                "IMS_MODIFY_CALL_CONFIRM RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CLIP, ims_MsgType_REQUEST,  NULL,                           0,                               QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CLIP, "IMS_QUERY_CLIP REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CLIP, ims_MsgType_RESPONSE, ims_ClipProvisionStatus_fields, sizeof(ims_ClipProvisionStatus), QCRIL_EVT_NONE,                      "IMS_QUERY_CLIP RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_CLIR,   ims_MsgType_REQUEST,  NULL,                           0,                               QCRIL_EVT_IMS_SOCKET_REQ_GET_CLIR,   "IMS_GET_CLIR REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_CLIR,   ims_MsgType_RESPONSE, ims_Clir_fields,                sizeof(ims_Clir),                QCRIL_EVT_NONE,                      "IMS_GET_CLIR RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CLIR,                  ims_MsgType_REQUEST,  ims_Clir_fields,                sizeof(ims_Clir),                       QCRIL_EVT_IMS_SOCKET_REQ_SET_CLIR,                  "IMS_SET_CLIR REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CLIR,                  ims_MsgType_RESPONSE, NULL,                           0,                                      QCRIL_EVT_NONE,                                     "IMS_SET_CLIR RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS, ims_MsgType_REQUEST,  ims_CallForwardInfoList_fields, sizeof(ims_CallForwardInfoList), QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_FORWARD_STATUS, "IMS_QUERY_CALL_FORWARD_STATUS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS, ims_MsgType_RESPONSE, ims_CallForwardInfoList_fields, sizeof(ims_CallForwardInfoList), QCRIL_EVT_NONE,                                     "IMS_QUERY_CALL_FORWARD_STATUS RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS, ims_MsgType_REQUEST,  ims_CallForwardInfoList_fields, sizeof(ims_CallForwardInfoList), QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_FORWARD_STATUS, "IMS_SET_CALL_FORWARD_STATUS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS, ims_MsgType_RESPONSE, ims_SuppSvcResponse_fields,     sizeof(ims_SuppSvcResponse),     QCRIL_EVT_NONE,                                   "IMS_SET_CALL_FORWARD_STATUS RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CALL_WAITING,      ims_MsgType_REQUEST,  ims_ServiceClass_fields,        sizeof(ims_ServiceClass),        QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_WAITING,      "IMS_QUERY_CALL_WAITING REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_CALL_WAITING,      ims_MsgType_RESPONSE, ims_CallWaitingInfo_fields,     sizeof(ims_CallWaitingInfo),     QCRIL_EVT_NONE,                                   "IMS_QUERY_CALL_WAITING RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CALL_WAITING,     ims_MsgType_REQUEST,  ims_CallWaitingInfo_fields, sizeof(ims_CallWaitingInfo), QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_WAITING,     "IMS_SET_CALL_WAITING REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_CALL_WAITING,     ims_MsgType_RESPONSE, ims_SuppSvcResponse_fields, sizeof(ims_SuppSvcResponse), QCRIL_EVT_NONE,                                "IMS_SET_CALL_WAITING RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE, ims_MsgType_REQUEST,  ims_Registration_fields,    sizeof(ims_Registration),    QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE, "IMS_REG_STATE_CHANGE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE, ims_MsgType_RESPONSE, NULL,                       0,                           QCRIL_EVT_NONE,                                "IMS_REG_STATE_CHANGE RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION, ims_MsgType_REQUEST,  ims_SuppSvcStatus_fields, sizeof(ims_SuppSvcStatus), QCRIL_EVT_IMS_SOCKET_REQ_SET_SUPP_SVC_NOTIFICATION, "IMS_SET_SUPP_SVC_NOTIFICATION REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION, ims_MsgType_RESPONSE, ims_SuppSvcStatus_fields, sizeof(ims_SuppSvcStatus), QCRIL_EVT_NONE,                                     "IMS_SET_SUPP_SVC_NOTIFICATION RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_ADD_PARTICIPANT,           ims_MsgType_REQUEST,  ims_Dial_fields,          sizeof(ims_Dial),          QCRIL_EVT_IMS_SOCKET_REQ_ADD_PARTICIPANT,           "IMS_ADD_PARTICIPANT REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_ADD_PARTICIPANT,           ims_MsgType_RESPONSE, ims_SipErrorInfo_fields,  sizeof(ims_SipErrorInfo),  QCRIL_EVT_NONE,                                     "IMS_ADD_PARTICIPANT RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_SERVICE_STATUS, ims_MsgType_REQUEST,  NULL,                     0,                         QCRIL_EVT_IMS_SOCKET_REQ_QUERY_SERVICE_STATUS, "IMS_QUERY_SERVICE_STATUSE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_SERVICE_STATUS, ims_MsgType_RESPONSE, ims_SrvStatusList_fields, sizeof(ims_SrvStatusList), QCRIL_EVT_NONE,                                "IMS_QUERY_SERVICE_STATUSE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_SERVICE_STATUS,   ims_MsgType_REQUEST,  ims_Info_fields,          sizeof(ims_Info),          QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS,   "IMS_SET_SERVICE_STATUS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_SERVICE_STATUS,   ims_MsgType_RESPONSE, NULL,                     0,                         QCRIL_EVT_NONE,                                "IMS_SET_SERVICE_STATUS RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SUPP_SVC_STATUS, ims_MsgType_REQUEST,  ims_SuppSvcRequest_fields,  sizeof(ims_SuppSvcRequest),  QCRIL_EVT_IMS_SOCKET_REQ_SUPP_SVC_STATUS, "IMS_SUPP_SVC_STATUSE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SUPP_SVC_STATUS, ims_MsgType_RESPONSE, ims_SuppSvcResponse_fields, sizeof(ims_SuppSvcResponse), QCRIL_EVT_NONE,                           "IMS_SUPP_SVC_STATUSE RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DEFLECT_CALL,    ims_MsgType_REQUEST,  ims_DeflectCall_fields,     sizeof(ims_DeflectCall),     QCRIL_EVT_IMS_SOCKET_REQ_CALL_DEFLECTION, "IMS_CALL_DEFLECTION REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_DEFLECT_CALL,    ims_MsgType_RESPONSE, NULL,                       0,                           QCRIL_EVT_NONE,                           "IMS_CALL_DEFLECTION RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_COLR, ims_MsgType_REQUEST,  NULL,                     0,                        QCRIL_EVT_IMS_SOCKET_REQ_GET_COLR, "IMS_GET_COLR REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_COLR, ims_MsgType_RESPONSE, ims_Colr_fields,          sizeof(ims_Colr),         QCRIL_EVT_NONE,                    "IMS_GET_COLR RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_COLR, ims_MsgType_REQUEST,  ims_Colr_fields,          sizeof(ims_Colr),         QCRIL_EVT_IMS_SOCKET_REQ_SET_COLR, "IMS_SET_COLR REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_COLR, ims_MsgType_RESPONSE, ims_SipErrorInfo_fields,  sizeof(ims_SipErrorInfo), QCRIL_EVT_NONE,                    "IMS_SET_COLR RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_VT_CALL_QUALITY, ims_MsgType_REQUEST,  NULL,                        0,                                   QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY, "IMS_QUERY_VT_CALL_QUALITY REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_QUERY_VT_CALL_QUALITY, ims_MsgType_RESPONSE, ims_VideoCallQuality_fields, sizeof(ims_VideoCallQuality), QCRIL_EVT_NONE,                                 "IMS_QUERY_VT_CALL_QUALITY RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_VT_CALL_QUALITY,   ims_MsgType_REQUEST,  ims_VideoCallQuality_fields, sizeof(ims_VideoCallQuality), QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY,   "IMS_SET_VT_CALL_QUALITY REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_VT_CALL_QUALITY,   ims_MsgType_RESPONSE, NULL,                        0,                                   QCRIL_EVT_NONE,                                 "IMS_SET_VT_CALL_QUALITY RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HOLD,   ims_MsgType_REQUEST,  ims_Hold_fields,          sizeof(ims_Hold),         QCRIL_EVT_IMS_SOCKET_REQ_HOLD,   "IMS_HOLD REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_HOLD,   ims_MsgType_RESPONSE, ims_SipErrorInfo_fields,  sizeof(ims_SipErrorInfo), QCRIL_EVT_NONE,                  "IMS_HOLD RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_RESUME, ims_MsgType_REQUEST,  ims_Resume_fields,        sizeof(ims_Resume),       QCRIL_EVT_IMS_SOCKET_REQ_RESUME, "IMS_RESUME REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_RESUME, ims_MsgType_RESPONSE, ims_SipErrorInfo_fields,  sizeof(ims_SipErrorInfo), QCRIL_EVT_NONE,                  "IMS_RESUME RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_UI_TTY_MODE, ims_MsgType_REQUEST,  ims_TtyNotify_fields, sizeof(ims_TtyNotify), QCRIL_EVT_IMS_SOCKET_REQ_SET_TTY_MODE, "IMS_SEND_UI_TTY_MODE REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_UI_TTY_MODE, ims_MsgType_RESPONSE, NULL,                 0,                     QCRIL_EVT_NONE,                        "IMS_SEND_UI_TTY_MODE RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_RTP_STATISTICS,       ims_MsgType_REQUEST,  NULL,                         0,                             QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_STATISTICS,       "IMS_GET_RTP_STATISTICS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_RTP_STATISTICS,       ims_MsgType_RESPONSE, ims_RtpStatisticsData_fields, sizeof(ims_RtpStatisticsData), QCRIL_EVT_NONE,                                    "IMS_GET_RTP_STATISTICS RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS, ims_MsgType_REQUEST,  NULL,                         0,                             QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_ERROR_STATISTICS, "IMS_GET_RTP_ERROR_STATISTICS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS, ims_MsgType_RESPONSE, ims_RtpStatisticsData_fields, sizeof(ims_RtpStatisticsData), QCRIL_EVT_NONE,                                    "IMS_GET_RTP_ERROR_STATISTICS RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_WIFI_CALLING_STATUS, ims_MsgType_REQUEST,  NULL,                       0,                           QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS, "IMS_GET_WIFI_CALLING_STATUS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_WIFI_CALLING_STATUS, ims_MsgType_RESPONSE, ims_WifiCallingInfo_fields, sizeof(ims_WifiCallingInfo), QCRIL_EVT_NONE,                                   "IMS_GET_WIFI_CALLING_STATUS RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_WIFI_CALLING_STATUS, ims_MsgType_REQUEST,  ims_WifiCallingInfo_fields, sizeof(ims_WifiCallingInfo), QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS, "IMS_SET_WIFI_CALLING_STATUS REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_WIFI_CALLING_STATUS, ims_MsgType_RESPONSE, NULL,                       0,                           QCRIL_EVT_NONE,                                   "IMS_SET_WIFI_CALLING_STATUS RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_IMS_CONFIG, ims_MsgType_REQUEST,  ims_ConfigMsg_fields, sizeof(ims_ConfigMsg), QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG, "IMS_SET_IMS_CONFIG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_IMS_CONFIG, ims_MsgType_RESPONSE, ims_ConfigMsg_fields, sizeof(ims_ConfigMsg), QCRIL_EVT_NONE,                          "IMS_SET_IMS_CONFIG RESP"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_IMS_CONFIG, ims_MsgType_REQUEST,  ims_ConfigMsg_fields, sizeof(ims_ConfigMsg), QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG, "IMS_GET_IMS_CONFIG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_IMS_CONFIG, ims_MsgType_RESPONSE, ims_ConfigMsg_fields, sizeof(ims_ConfigMsg), QCRIL_EVT_NONE,                          "IMS_GET_IMS_CONFIG RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO, ims_MsgType_REQUEST, ims_GeoLocationInfo_fields, sizeof(ims_GeoLocationInfo), QCRIL_EVT_IMS_SOCKET_REQ_SET_GEO_LOC,                          "IMS_SEND_GEOLOCATION_INFO REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO, ims_MsgType_RESPONSE, NULL,                      0,                           QCRIL_EVT_NONE,                                                "IMS_SEND_GEOLOCATION_INFO RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOPS_INFO,    ims_MsgType_REQUEST,  NULL,                   0,                    QCRIL_EVT_IMS_SOCKET_REQ_GET_VOPS_INFO,   "IMS_GET_IMS_VOPS_INFO_REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOPS_INFO,    ims_MsgType_RESPONSE, ims_VopsInfo_fields,    sizeof(ims_VopsInfo), QCRIL_EVT_NONE,                           "IMS_GET_IMS_VOPS_INFO_RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_SSAC_INFO,    ims_MsgType_REQUEST,  NULL,                   0,                    QCRIL_EVT_IMS_SOCKET_REQ_GET_SSAC_INFO,   "IMS_GET_IMS_SSAC_INFO_REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_SSAC_INFO,    ims_MsgType_RESPONSE, ims_SsacInfo_fields,    sizeof(ims_SsacInfo), QCRIL_EVT_NONE,                           "IMS_GET_IMS_SSAC_INFO_RESP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_REQUEST_GEOLOCATION, ims_MsgType_UNSOL_RESPONSE, ims_GeoLocationInfo_fields, sizeof(ims_GeoLocationInfo), QCRIL_EVT_NONE,                          "IMS_UNSOL_GEOLOCATION_INFO UNSOL"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOWIFI_CALL_QUALITY, ims_MsgType_REQUEST, NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_GET_VOWIFI_CALL_QUALITY, "IMS_GET_VOWIFI_CALL_QUALITY REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOWIFI_CALL_QUALITY, ims_MsgType_RESPONSE, ims_VoWiFiCallQuality_fields, sizeof(ims_VoWiFiCallQuality), QCRIL_EVT_NONE, "IMS_GET_VOWIFI_CALL_QUALITY RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_VOLTE_PREF, ims_MsgType_REQUEST, ims_VolteUserPref_fields, sizeof(ims_VolteUserPref), QCRIL_EVT_IMS_SOCKET_REQ_SET_VOLTE_PREF, "IMS_SET_VOLTE_PREF REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_VOLTE_PREF, ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE, "IMS_SET_VOLTE_PREF RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOLTE_PREF, ims_MsgType_REQUEST, NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_GET_VOLTE_PREF, "IMS_GET_VOLTE_PREF REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_VOLTE_PREF, ims_MsgType_RESPONSE, ims_VolteUserPref_fields, sizeof(ims_VolteUserPref), QCRIL_EVT_NONE, "IMS_GET_VOLTE_PREF RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_HANDOVER_CONFIG, ims_MsgType_REQUEST, NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_GET_HANDOVER_CONFIG, "IMS_GET_HANDOVER_CONFIG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_HANDOVER_CONFIG, ims_MsgType_RESPONSE, ims_HandoverConfigMsg_fields, sizeof(ims_HandoverConfigMsg), QCRIL_EVT_NONE, "IMS_GET_HANDOVER_CONFIG RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_HANDOVER_CONFIG, ims_MsgType_REQUEST, ims_HandoverConfigMsg_fields, sizeof(ims_HandoverConfigMsg), QCRIL_EVT_IMS_SOCKET_REQ_SET_HANDOVER_CONFIG, "IMS_SET_HANDOVER_CONFIG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SET_HANDOVER_CONFIG, ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE, "IMS_SET_HANDOVER_CONFIG RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ims_MsgType_REQUEST, NULL, 0, QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_SUB_CONFIG, "IMS_GET_IMS_SUB_CONFIG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_RTT_MSG, ims_MsgType_REQUEST, ims_RttMessage_fields, sizeof(ims_RttMessage), QCRIL_EVT_IMS_SOCKET_REQ_SEND_RTT_MSG, "IMS_SEND_RTT_MSG REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_SEND_RTT_MSG, ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE , "IMS_SEND_RTT_MSG RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG, ims_MsgType_RESPONSE, ims_ImsSubConfig_fields, sizeof(ims_ImsSubConfig), QCRIL_EVT_NONE, "IMS_GET_IMS_SUB_CONFIG RSP"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_CANCEL_MODIFY_CALL, ims_MsgType_REQUEST, ims_CancelModifyCall_fields, sizeof(ims_CancelModifyCall), QCRIL_EVT_IMS_SOCKET_REQ_CANCEL_MODIFY_CALL, "IMS_CANCEL_MODIFY_CALL REQ"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_REQUEST_CANCEL_MODIFY_CALL, ims_MsgType_RESPONSE, NULL, 0, QCRIL_EVT_NONE , "IMS_CANCEL_MODIFY_CALL RSP"});

    // Unsol Indications
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RESPONSE_CALL_STATE_CHANGED,        ims_MsgType_UNSOL_RESPONSE, ims_CallList_fields,     sizeof(ims_CallList),     QCRIL_EVT_NONE, "IMS_UNSOL_CALL_STATE_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_CALL_RING,                          ims_MsgType_UNSOL_RESPONSE, NULL,                    0,                        QCRIL_EVT_NONE, "IMS_UNSOL_CALL_RING"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RINGBACK_TONE,                      ims_MsgType_UNSOL_RESPONSE, ims_RingBackTone_fields, sizeof(ims_RingBackTone), QCRIL_EVT_NONE, "IMS_UNSOL_RINGBACK_TONE"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, ims_MsgType_UNSOL_RESPONSE, ims_Registration_fields, sizeof(ims_Registration), QCRIL_EVT_NONE, "IMS_UNSOL_NETWORK_STATE_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS, ims_MsgType_UNSOL_RESPONSE, ims_RegistrationBlockStatus_fields, sizeof(ims_RegistrationBlockStatus_fields), QCRIL_EVT_NONE, "IMS_UNSOL_REGISTRATION_BLOCK_STATUS"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE, ims_MsgType_UNSOL_RESPONSE, NULL,                  0 ,                        QCRIL_EVT_NONE, "IMS_UNSOL_ENTER_ECBM"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,  ims_MsgType_UNSOL_RESPONSE, NULL,                  0,                         QCRIL_EVT_NONE, "IMS_UNSOL_EXIT_ECBM"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_MODIFY_CALL,                   ims_MsgType_UNSOL_RESPONSE, ims_CallModify_fields, sizeof(ims_CallModify),    QCRIL_EVT_NONE, "IMS_UNSOL_MODIFY_CALL"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RESPONSE_HANDOVER,             ims_MsgType_UNSOL_RESPONSE, ims_Handover_fields,   sizeof(ims_Handover),      QCRIL_EVT_NONE, "IMS_UNSOL_HANDOVER"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_REFRESH_CONF_INFO,     ims_MsgType_UNSOL_RESPONSE, ims_ConfInfo_fields,            sizeof(ims_ConfInfo),            QCRIL_EVT_NONE, "IMS_UNSOL_REFRESH_CONF_INFO"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_SRV_STATUS_UPDATE,     ims_MsgType_UNSOL_RESPONSE, ims_SrvStatusList_fields,       sizeof(ims_SrvStatusList),       QCRIL_EVT_NONE, "IMS_UNSOL_SRV_STATUS_UPDATE"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_SUPP_SVC_NOTIFICATION, ims_MsgType_UNSOL_RESPONSE, ims_SuppSvcNotification_fields, sizeof(ims_SuppSvcNotification), QCRIL_EVT_NONE, "IMS_UNSOL_SUPP_SVC_NOTIFICATION"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_TTY_NOTIFICATION,      ims_MsgType_UNSOL_RESPONSE, ims_TtyNotify_fields,           sizeof(ims_TtyNotify),           QCRIL_EVT_NONE, "IMS_UNSOL_TTY_NOTIFICATION"});

    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RADIO_STATE_CHANGED, ims_MsgType_UNSOL_RESPONSE, ims_RadioStateChanged_fields, sizeof(ims_RadioStateChanged), QCRIL_EVT_NONE, "IMS_UNSOL_RADIO_STATE_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_MWI,                 ims_MsgType_UNSOL_RESPONSE, ims_Mwi_fields,               sizeof(ims_Mwi),               QCRIL_EVT_NONE, "IMS_UNSOL_MWI"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_REFRESH_VICE_INFO,   ims_MsgType_UNSOL_RESPONSE, ims_ViceInfo_fields,          sizeof(ims_ViceInfo),          QCRIL_EVT_NONE, "IMS_UNSOL_REFRESH_VICE_INFO"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_VOWIFI_CALL_QUALITY, ims_MsgType_UNSOL_RESPONSE, ims_VoWiFiCallQuality_fields, sizeof(ims_VoWiFiCallQuality), QCRIL_EVT_NONE, "IMS_UNSOL_VOWIFI_CALL_QUALITY"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_VOPS_CHANGED,        ims_MsgType_UNSOL_RESPONSE, ims_VopsInfo_fields,          sizeof(ims_VopsInfo),          QCRIL_EVT_NONE, "IMS_UNSOL_VOPS_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_SSAC_CHANGED,        ims_MsgType_UNSOL_RESPONSE, ims_SsacInfo_fields,          sizeof(ims_SsacInfo),          QCRIL_EVT_NONE, "IMS_UNSOL_SSAC_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_PARTICIPANT_STATUS_INFO, ims_MsgType_UNSOL_RESPONSE, ims_ParticipantStatusInfo_fields, sizeof(ims_ParticipantStatusInfo), QCRIL_EVT_NONE, "IMS_UNSOL_PARTICIPANT_STATUS_INFO"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_RESPONSE_RTT_MSG_RECEIVED, ims_MsgType_UNSOL_RESPONSE, ims_RttMessage_fields, sizeof(ims_RttMessage), QCRIL_EVT_NONE, "IMS_UNSOL_RTT_MSG_RECEIVED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_IMS_SUB_CONFIG_CHANGED, ims_MsgType_UNSOL_RESPONSE, ims_ImsSubConfig_fields, sizeof(ims_ImsSubConfig), QCRIL_EVT_NONE, "IMS_UNSOL_IMS_SUB_CONFIG_CHANGED"});
    qcril_qmi_ims_msg_meta_data.push_back({ims_MsgId_UNSOL_ON_SS, ims_MsgType_UNSOL_RESPONSE, NULL, 0, QCRIL_EVT_NONE, "IMS_UNSOL_IMS_ON_SS"});
}
//===========================================================================
// qcril_qmi_ims_get_msg_size
//===========================================================================
size_t qcril_qmi_ims_get_msg_size(ims_MsgId msg_id, ims_MsgType msg_type)
{
    std::vector<qcril_qmi_ims_msg_meta_type> &qcril_qmi_ims_msg_meta_data = get_meta();
    size_t i;
    for (i=0; i<qcril_qmi_ims_msg_meta_data.size(); i++)
    {
        if ( qcril_qmi_ims_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_ims_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_qmi_ims_msg_meta_data[i].msg_req_size;
        }
    }
    return 0;
} // qcril_qmi_ims_get_msg_size

//===========================================================================
// qcril_qmi_ims_get_msg_pb_fields
//===========================================================================
const pb_field_t* qcril_qmi_ims_get_msg_pb_fields(ims_MsgId msg_id, ims_MsgType msg_type)
{
    size_t i;
    std::vector<qcril_qmi_ims_msg_meta_type> &qcril_qmi_ims_msg_meta_data = get_meta();
    for (i=0; i<qcril_qmi_ims_msg_meta_data.size(); i++)
    {
        if ( qcril_qmi_ims_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_ims_msg_meta_data[i].msg_type == msg_type )
        {
            return qcril_qmi_ims_msg_meta_data[i].msg_pb_fields;
        }
    }
    return NULL;
} // qcril_qmi_ims_get_msg_pb_fields

//===========================================================================
// qcril_qmi_ims_get_msg_log_str
//===========================================================================
const char* qcril_qmi_ims_get_msg_log_str(ims_MsgId msg_id, ims_MsgType msg_type)
{
    size_t i;
    std::vector<qcril_qmi_ims_msg_meta_type> &qcril_qmi_ims_msg_meta_data = get_meta();
    for (i=0; i<qcril_qmi_ims_msg_meta_data.size(); i++)
    {
        if ( qcril_qmi_ims_msg_meta_data[i].msg_id == msg_id &&
             qcril_qmi_ims_msg_meta_data[i].msg_type == msg_type )
        {
            if (qcril_qmi_ims_msg_meta_data[i].msg_log_str)
            {
                return qcril_qmi_ims_msg_meta_data[i].msg_log_str;
            }
            else
            {
                break;
            }
        }
    }
    return "Unknown msg";
} // qcril_qmi_ims_get_msg_log_str

//===========================================================================
// qcril_qmi_ims_get_msg_event
//===========================================================================
qcril_evt_e_type qcril_qmi_ims_get_msg_event(ims_MsgId msg_id, ims_MsgType msg_type)
{
    QCRIL_NOTUSED(msg_type);
    size_t i;
    std::vector<qcril_qmi_ims_msg_meta_type> &qcril_qmi_ims_msg_meta_data = get_meta();
    for (i=0; i<qcril_qmi_ims_msg_meta_data.size(); i++)
    {
        if (qcril_qmi_ims_msg_meta_data[i].msg_id == msg_id)
        {
            return qcril_qmi_ims_msg_meta_data[i].msg_event;
        }
    }
    return QCRIL_EVT_NONE;
} // qcril_qmi_ims_get_msg_event
