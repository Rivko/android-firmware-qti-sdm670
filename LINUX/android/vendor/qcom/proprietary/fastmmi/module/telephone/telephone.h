/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __TELEPHONE_H_
#define __TELEPHONE_H_

/* sub command */
#define CMD_MMI                   0x00
#define CMD_PCBA                  0x01

/* parameter key */
#define KEY_CALL_NUMBER           "number"
#define KEY_HOLD_TIME             "timeout"

/* ansy message timeout time */
#define QMI_SYNC_REQ_DEF_TIMEOUT  (500)

#define CALL_ID_TYPE_ANSWER       1
#define CALL_ID_TYPE_DIALING      2

/* test thread parameter */
typedef struct test_param {
    int sub_cmd;
    int timeout;                /* alerting or conversation time: millisecond */
    char call_number[32];       /* call number */
} param_t;

typedef struct requester_private_info {
    qmi_idl_service_object_type service_object;
    qmi_client_os_params os_param;
    qmi_client_type qmi_svc_client;
} qmi_requester_private_info;

typedef struct request_param {
    int message_id;
    int timeout;
    qmi_client_type qmi_svc_client;
    qmi_idl_service_object_type service_object;
    int message_specific_payload_len;
    void *message_specific_payload;
} qmi_request_param;

typedef struct qmi_call_id {
    int ringing_call_id;
    int existing_call_id;
} qmi_call_id;

typedef struct qmi_call_satus {
    int call_status;
    char *call_status_str;
} qmi_call_status_log;

#endif    /* __TELEPHONE_H_ */
