/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <pthread.h>
#include <signal.h>
#include "nv.h"
#include "log.h"
#include "qmi_client.h"
#include "device_management_service_v01.h"
#include "voice_service_v02.h"
#include "network_access_service_v01.h"
#include "wireless_messaging_service_v01.h"
#include "qmi.h"
#include "telephone.h"
#include "mmi_module.h"

static char module_name[64] = { 0 };    // module name
static cb_print_t msg_print = NULL;     // call back function for send msg to mmi process
static mutex_locker g_mutex;
/* modem operate mode */
dms_operating_mode_enum_v01 g_modem_mode = DMS_OPERATING_MODE_ENUM_MIN_ENUM_VAL_V01;
/* request info for dialing */
static qmi_requester_private_info voice_requester_info;
/* request info for modem mode */
static qmi_requester_private_info dms_requester_info;

/* call status */
static const qmi_call_status_log call_status_tbl[] = {
    {CALL_STATE_ORIGINATING_V02, "ORIGINATING"},
    {CALL_STATE_INCOMING_V02, "INCOMING"},
    {CALL_STATE_CONVERSATION_V02, "CONVERSATION"},
    {CALL_STATE_CC_IN_PROGRESS_V02, "CC_IN_PROGRESS"},
    {CALL_STATE_ALERTING_V02, "ALERTING"},
    {CALL_STATE_HOLD_V02, "HOLD"},
    {CALL_STATE_WAITING_V02, "WAITING"},
    {CALL_STATE_DISCONNECTING_V02, "DISCONNECTING"},
    {CALL_STATE_END_V02, "END"},
    {CALL_STATE_SETUP_V02, "SETUP"},
    {-1, NULL}
};

static int32_t end_call_handler();
static int32_t end_call_handler(int);

static char *get_call_status_str(int call_status) {
    const qmi_call_status_log *p = call_status_tbl;

    while(-1 != p->call_status && NULL != p->call_status_str) {
        if(call_status == p->call_status) {
            return p->call_status_str;
        }
        p++;
    }
    return "Unknow call status";
}

static void qmi_unsolicited_voice_ind_handler(qmi_client_type user_handle,
                                              unsigned int msg_id,
                                              void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data) {
    MMI_ALOGI("qmi_unsolicited_voice_ind_handler by called, msg_id=%d, ind_buf_len=%d", msg_id, ind_buf_len);
}

static void qmi_unsolicited_dms_ind_handler(qmi_client_type user_handle,
                                            unsigned int msg_id,
                                            void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data) {
    MMI_ALOGI("qmi_unsolicited_dms_ind_handler by called, msg_id=%d, ind_buf_len=%d", msg_id, ind_buf_len);
}

/* sync send message */
static qmi_client_error_type send_msg_to_qmi_svc_sync(qmi_request_param * req_info, void *response, int rsp_buf_size) {
    uint32_t response_payload_len = 0;

    /* check response */
    qmi_idl_get_message_c_struct_len(req_info->service_object,
                                     QMI_IDL_RESPONSE, req_info->message_id, &response_payload_len);
    if(rsp_buf_size != response_payload_len) {
        MMI_ALOGE("Input response buffer size is invalid, rsp_buf_size=%d", rsp_buf_size);
        return QMI_IDL_PARAMETER_ERROR;
    }
    MMI_ALOGE("Message(id=%d) response size=%d", req_info->message_id, response_payload_len);

    qmi_client_error_type rc = qmi_client_send_msg_sync(req_info->qmi_svc_client,
                                                        req_info->message_id,
                                                        (void *) req_info->message_specific_payload,
                                                        req_info->message_specific_payload_len,
                                                        (void *) response,
                                                        rsp_buf_size,
                                                        req_info->timeout);

    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("sync send message to qmi service failed, error code=%d", rc);
    }
    return rc;
}

static int32_t set_modem_report(qmi_requester_private_info * param, int is_report) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    dms_set_event_report_req_msg_v01 report_req;
    dms_set_event_report_resp_msg_v01 report_rsp;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&report_req, 0x00, sizeof(report_req));
    memset(&report_rsp, 0x00, sizeof(report_rsp));

    report_req.report_oprt_mode_state = (uint8_t) is_report;
    report_req.report_oprt_mode_state_valid = TRUE;

    req_info.message_id = QMI_DMS_SET_EVENT_REPORT_REQ_V01;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = param->qmi_svc_client;
    req_info.service_object = param->service_object;
    req_info.message_specific_payload = &report_req;
    req_info.message_specific_payload_len = sizeof(report_req);

    MMI_ALOGI("Send dms set event report req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &report_rsp, sizeof(report_rsp));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send dms set event report req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != report_rsp.resp.result) {
        MMI_ALOGE("set event report req failed, error code=%d", report_rsp.resp.result);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t set_modem_mode(qmi_requester_private_info * param, dms_operating_mode_enum_v01 mode) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    dms_set_operating_mode_req_msg_v01 set_mode_request_msg;
    dms_set_operating_mode_resp_msg_v01 set_mode_rsp_msg;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&set_mode_request_msg, 0x00, sizeof(set_mode_request_msg));
    memset(&set_mode_rsp_msg, 0x00, sizeof(set_mode_rsp_msg));

    set_mode_request_msg.operating_mode = mode;
    req_info.message_id = QMI_DMS_SET_OPERATING_MODE_REQ_V01;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = param->qmi_svc_client;
    req_info.service_object = param->service_object;
    req_info.message_specific_payload = &set_mode_request_msg;
    req_info.message_specific_payload_len = sizeof(set_mode_request_msg);

    MMI_ALOGI("Send DMS set operating mode req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &set_mode_rsp_msg, sizeof(set_mode_rsp_msg));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send dms set operating mode req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != set_mode_rsp_msg.resp.result) {
        MMI_ALOGE("Get operating mode req failed, error code=%d", set_mode_rsp_msg.resp.result);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t get_modem_mode(qmi_requester_private_info * param, dms_operating_mode_enum_v01 * mode) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    dms_get_operating_mode_req_msg_v01 get_mode_request_msg;
    dms_get_operating_mode_resp_msg_v01 get_mode_rsp_msg;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&get_mode_request_msg, 0x00, sizeof(get_mode_request_msg));
    memset(&get_mode_rsp_msg, 0x00, sizeof(get_mode_rsp_msg));

    req_info.message_id = QMI_DMS_GET_OPERATING_MODE_REQ_V01;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = param->qmi_svc_client;
    req_info.service_object = param->service_object;
    req_info.message_specific_payload = &get_mode_request_msg;
    req_info.message_specific_payload_len = sizeof(get_mode_request_msg);

    rc = send_msg_to_qmi_svc_sync(&req_info, &get_mode_rsp_msg, sizeof(get_mode_rsp_msg));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send dms get operating mode req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != get_mode_rsp_msg.resp.result) {
        MMI_ALOGE("Get operating mode req failed, error code=%d", get_mode_rsp_msg.resp.result);
        return FAILED;
    }
    *mode = get_mode_rsp_msg.operating_mode;
    return SUCCESS;
}

static int32_t get_call_id(qmi_call_id * call_id, int call_id_type) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    voice_get_all_call_info_resp_msg_v02 call_info_msg;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&call_info_msg, 0x00, sizeof(call_info_msg));

    req_info.message_id = QMI_VOICE_GET_ALL_CALL_INFO_REQ_V02;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = voice_requester_info.qmi_svc_client;
    req_info.service_object = voice_requester_info.service_object;
    req_info.message_specific_payload = NULL;
    req_info.message_specific_payload_len = 0;

    MMI_ALOGI("Send get all call info req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &call_info_msg, sizeof(call_info_msg));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send get all call info req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != call_info_msg.resp.result) {
        MMI_ALOGE("Get all call info req failed, error code=%d", call_info_msg.resp.result);
        return FAILED;
    }

    call_id->ringing_call_id = -1;
    call_id->existing_call_id = -1;
    for(int i = 0; i < call_info_msg.call_info_len; i++) {
        if(CALL_STATE_INCOMING_V02 == call_info_msg.call_info[i].call_state) {
            call_id->ringing_call_id = (int) call_info_msg.call_info[i].call_id;
        }
        call_id->existing_call_id = (int) call_info_msg.call_info[i].call_id;
    }

    if(CALL_ID_TYPE_ANSWER == call_id_type && -1 == call_id->ringing_call_id) {
        MMI_ALOGE("Not found call id for answer");
        return FAILED;
    } else if(CALL_ID_TYPE_DIALING == call_id_type && -1 == call_id->existing_call_id) {
        MMI_ALOGE("Not found call id for dialing");
        return FAILED;
    }
    return SUCCESS;
}

static int32_t get_call_status(int call_id, int *call_status) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    voice_get_call_info_req_msg_v02 call_info_req;
    voice_get_call_info_resp_msg_v02 call_info_resp;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&call_info_req, 0x00, sizeof(call_info_req));
    memset(&call_info_resp, 0x00, sizeof(call_info_resp));

    call_info_req.call_id = (uint8_t) call_id;
    req_info.message_id = QMI_VOICE_GET_CALL_INFO_REQ_V02;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = voice_requester_info.qmi_svc_client;
    req_info.service_object = voice_requester_info.service_object;
    req_info.message_specific_payload = (void *) &call_info_req;
    req_info.message_specific_payload_len = sizeof(call_info_req);

    MMI_ALOGI("Send get call info req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &call_info_resp, sizeof(call_info_resp));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send get call info req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != call_info_resp.resp.result) {
        MMI_ALOGE("Get call info req failed, error code=%d", call_info_resp.resp.result);
        return FAILED;
    }

    *call_status = (int) (call_info_resp.call_info.call_state);
    MMI_ALOGI("Get call status=%d for call id=%d", *call_status, call_id);

    return SUCCESS;
}

static int32_t dialing_handler(char *dialing_number) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    voice_dial_call_req_msg_v02 dial_call_req_msg;
    voice_dial_call_resp_msg_v02 dial_call_rsp_msg;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&dial_call_req_msg, 0x00, sizeof(dial_call_req_msg));
    memset(&dial_call_rsp_msg, 0x00, sizeof(dial_call_rsp_msg));

    snprintf(dial_call_req_msg.calling_number, sizeof(dial_call_req_msg.calling_number), "%s", dialing_number);
    req_info.message_id = QMI_VOICE_DIAL_CALL_REQ_V02;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = voice_requester_info.qmi_svc_client;
    req_info.service_object = voice_requester_info.service_object;
    req_info.message_specific_payload = &dial_call_req_msg;
    req_info.message_specific_payload_len = sizeof(dial_call_req_msg);

    MMI_ALOGI("Send voice dial call req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &dial_call_rsp_msg, sizeof(dial_call_rsp_msg));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send voice dial call req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != dial_call_rsp_msg.resp.result) {
        MMI_ALOGE("Voice dial call req failed, error code=%d", dial_call_rsp_msg.resp.result);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t end_call_handler(int call_id) {
    qmi_client_error_type rc;
    qmi_request_param req_info;
    voice_end_call_req_msg_v02 end_call_req;
    voice_end_call_resp_msg_v02 end_call_rsp;

    memset(&req_info, 0x00, sizeof(req_info));
    memset(&end_call_req, 0x00, sizeof(end_call_req));
    memset(&end_call_rsp, 0x00, sizeof(end_call_rsp));

    end_call_req.call_id = call_id;
    req_info.message_id = QMI_VOICE_END_CALL_REQ_V02;
    req_info.timeout = QMI_SYNC_REQ_DEF_TIMEOUT;
    req_info.qmi_svc_client = voice_requester_info.qmi_svc_client;
    req_info.service_object = voice_requester_info.service_object;
    req_info.message_specific_payload = &end_call_req;
    req_info.message_specific_payload_len = sizeof(end_call_req);

    MMI_ALOGI("Send end call req to qmi service");
    rc = send_msg_to_qmi_svc_sync(&req_info, &end_call_rsp, sizeof(end_call_rsp));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Send end call req failed, error code=%d", rc);
        return FAILED;
    }
    if(QMI_RESULT_SUCCESS_V01 != end_call_rsp.resp.result) {
        MMI_ALOGE("End call req failed, error code=%d", end_call_rsp.resp.result);
        return FAILED;
    }
    MMI_ALOGI("Call(call id=%d) be end", end_call_rsp.call_id);

    return SUCCESS;
}

static int32_t end_call_handler() {
    int ret;
    qmi_client_error_type rc;
    qmi_call_id call_id;
    qmi_request_param req_info;
    voice_end_call_req_msg_v02 end_call_req;
    voice_end_call_resp_msg_v02 end_call_rsp;

    memset(&call_id, 0x00, sizeof(call_id));
    memset(&req_info, 0x00, sizeof(req_info));
    memset(&end_call_req, 0x00, sizeof(end_call_req));
    memset(&end_call_rsp, 0x00, sizeof(end_call_rsp));

    ret = get_call_id(&call_id, CALL_ID_TYPE_DIALING);
    if(SUCCESS != ret) {
        MMI_ALOGE("Get call info failed..");
        return FAILED;
    }
    MMI_ALOGI("Get call id=%d", call_id.existing_call_id);

    ret = end_call_handler(call_id.existing_call_id);
    if(SUCCESS != ret) {
        MMI_ALOGE("End call req failed");
        return FAILED;
    }

    return SUCCESS;
}

static int32_t test_dialing_phone(int sub_cmd, char *number, int timeout) {
    int ret = 0;
    int sleep_cnt = 0;
    int call_status = 0;
    int status_transition = 0;
    qmi_call_id call_id = { 0 };
    char buf[SIZE_1K] = { 0 };

    MMI_ALOGI("Start test call phone, call number=%s", number);

    /* dialing */
    ret = dialing_handler(number);
    if(SUCCESS != ret) {
        MMI_ALOGE("Dialing handle failed");
        return FAILED;
    }

    /* get call id */
    ret = get_call_id(&call_id, CALL_ID_TYPE_DIALING);
    if(SUCCESS != ret) {
        MMI_ALOGE("Get call id failed");
        return FAILED;
    }
    MMI_ALOGI("Get call id=%d", call_id.existing_call_id);

    /* query call status */
    while(1) {
        /* get call status */
        ret = get_call_status(call_id.existing_call_id, &call_status);
        if(SUCCESS != ret) {
            MMI_ALOGE("Get call status failed");
            return FAILED;
        }

        /* print call status */
        if(call_status != status_transition) {
            snprintf(buf, sizeof(buf), "Calling number: %s\nCalling status: %s\n", number,
                     get_call_status_str(call_status));
            msg_print(module_name, (sub_cmd == CMD_MMI) ? SUBCMD_MMI : SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
        }

        /* dialing and wait for alerting */
        if(CALL_STATE_ORIGINATING_V02 == call_status) {
            /* get call status per 50 ms */
            int temp_timeout = timeout + (50 - (timeout % 50));

            sleep_cnt = temp_timeout / 50;
        } else if(CALL_STATE_DISCONNECTING_V02 == call_status || CALL_STATE_END_V02 == call_status) {   /* connect is disconnected or end by peer */
            MMI_ALOGI("Connection is disconnected or end by peer");
            end_call_handler(call_id.existing_call_id);

            if(CALL_STATE_ALERTING_V02 == status_transition || CALL_STATE_CONVERSATION_V02 == status_transition
               || CALL_STATE_WAITING_V02 == status_transition || CALL_STATE_HOLD_V02 == status_transition) {
                return SUCCESS;
            } else {
                return FAILED;
            }
        } else if(sub_cmd == CMD_PCBA
                  && (CALL_STATE_ALERTING_V02 == call_status || CALL_STATE_CONVERSATION_V02 == call_status
                      || CALL_STATE_HOLD_V02 == call_status || CALL_STATE_WAITING_V02 == call_status)) {
            end_call_handler(call_id.existing_call_id);
            return SUCCESS;
        } else {
            if(sleep_cnt--) {
                /* 50 ms */
                usleep(50 * 1000);
            } else {            /* timeout */
                end_call_handler(call_id.existing_call_id);
                if(sub_cmd == CMD_MMI
                   && (CALL_STATE_ALERTING_V02 == call_status || CALL_STATE_CONVERSATION_V02 == call_status
                       || CALL_STATE_HOLD_V02 == call_status || CALL_STATE_WAITING_V02 == call_status)) {
                    return SUCCESS;
                } else {
                    return FAILED;
                }
            }
        }
        status_transition = call_status;
    }
    return FAILED;
}

static int test_telephone(param_t *param) {
    int ret = FAILED;

    /* set modem mode to online */
    ret = set_modem_mode(&dms_requester_info, DMS_OP_MODE_ONLINE_V01);
    if(SUCCESS != ret) {
        MMI_ALOGE("Set Modem mode failed");
        return FAILED;
    }

    /* test dialing phone */
    ret = test_dialing_phone(param->sub_cmd, param->call_number, param->timeout);
    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    int32_t ret;
    qmi_client_error_type rc;

    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    /* set send message function pointer */
    msg_print = module->cb_print;
    snprintf(module_name, sizeof(module_name), "%s", params[KEY_MODULE_NAME].c_str());

    /* Get the voice service object */
    voice_requester_info.service_object = voice_get_service_object_v02();
    if(NULL == voice_requester_info.service_object) {
        MMI_ALOGE("Get the VOICE service objetc failed for module:[%s]", module->name);
        return FAILED;
    }
    /* Get the DMS service object */
    dms_requester_info.service_object = dms_get_service_object_v01();
    if(NULL == dms_requester_info.service_object) {
        MMI_ALOGE("Get the DMS service objetc failed for module:[%s]", module->name);
        return FAILED;
    }

    /* Initialize client object for voice */
    rc = qmi_client_init_instance(voice_requester_info.service_object, QMI_CLIENT_INSTANCE_ANY, qmi_unsolicited_voice_ind_handler, NULL, &(voice_requester_info.os_param), 4,   // 4 millisecond
                                  &(voice_requester_info.qmi_svc_client));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Initialize VOICE client object failed for module[%s], error code=%d", module->name, rc);
        return FAILED;
    }
    /* Initialize client object for dms */
    rc = qmi_client_init_instance(dms_requester_info.service_object, QMI_CLIENT_INSTANCE_ANY, qmi_unsolicited_dms_ind_handler, NULL, &(dms_requester_info.os_param), 4, // 100 millisecond
                                  &(dms_requester_info.qmi_svc_client));
    if(QMI_NO_ERR != rc) {
        MMI_ALOGE("Initialize DMS client object failed for module:[%s], error code=%d", module->name, rc);
        return FAILED;
    }

    ret = set_modem_report(&dms_requester_info, TRUE);
    if(SUCCESS != ret) {
        MMI_ALOGE("Set Modem report mode failed for module:[%s]", module->name);
        return FAILED;
    }

    /* get modem mode */
    ret = get_modem_mode(&dms_requester_info, &g_modem_mode);
    if(SUCCESS != ret) {
        MMI_ALOGE("Get Modem mode failed for module:[%s]", module->name);
        return FAILED;
    }

    MMI_ALOGI("module init finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }
    MMI_ALOGI("module deinit start for module:[%s]", module->name);
    MMI_ALOGI("module deinit finished for module:[%s]", module->name);

    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    /* end the call */
    end_call_handler();
    /* recover modem mode */
    set_modem_mode(&dms_requester_info, g_modem_mode);

    MMI_ALOGI("module stop finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    int ret = -1;
    param_t parameter;
    char buf[SIZE_1K] = { 0 };

    if(module == NULL || cmd == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }
    MMI_ALOGI("module run start for module: [%s]", module->name);

    /* subcmd */
    if(!strcmp(cmd, SUBCMD_MMI)) {
        parameter.sub_cmd = CMD_MMI;
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        parameter.sub_cmd = CMD_PCBA;
    } else {
        MMI_ALOGE("Received Invalid subcmd:%s", cmd);
        return FAILED;
    }

    /* timeout */
    if(params[KEY_HOLD_TIME].empty()) {
        MMI_ALOGW("Not set the call timeout and using default timeout:5s");
        parameter.timeout = 5000;   /* default timeout is 5s */
    } else {
        parameter.timeout = (int) strtol(params[KEY_HOLD_TIME].c_str(), NULL, 10);
    }

    /* call number */
    if(params[KEY_CALL_NUMBER].empty()) {
        MMI_ALOGW("Not set dialing number and using default number:112");
        snprintf(parameter.call_number, sizeof(parameter.call_number), "%s", "112");
    } else {
        snprintf(parameter.call_number, sizeof(parameter.call_number), "%s", params[KEY_CALL_NUMBER].c_str());
    }

    mutex_locker::autolock _L(g_mutex);
    /* start telephone test */
    ret = test_telephone(&parameter);
    if(SUCCESS == ret) {
        snprintf(buf, sizeof(buf), "Telephone Test Result= %s\n", "PASS");
    } else {
        snprintf(buf, sizeof(buf), "Telephone Test Result= %s\n", "FAILED");
    }

    if(CMD_MMI == parameter.sub_cmd) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
        /* 1 Second */
        usleep(1000 * 1000);
    } else if(CMD_PCBA == parameter.sub_cmd) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    }

    MMI_ALOGI("module run finished for module: [%s]", module->name);
    return ret;
}

/**
* Methods must be implemented by module.
*/
static struct mmi_module_methods_t module_methods = {
    .module_init = module_init,
    .module_deinit = module_deinit,
    .module_run = module_run,
    .module_stop = module_stop,
};

/**
* Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
* and the fields of this data structure must be initialize in strictly sequence as definition,
* please don't change the sequence as g++ not supported in CPP file.
*/
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = 1,
    .version_minor = 0,
    .name = "Telephone",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL,           /* it is initialized by mmi agent */
    .run_pid = -1,
};
