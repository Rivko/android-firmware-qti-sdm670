/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "diag_ext.h"

static int g_sock = -1;
static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;
static sem_t g_cmd_complete_sem;
msg_t g_msg;

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
void send_cmd(const char *subcmd, const char *params, uint32_t size) {
    send_cmd(g_sock, CLIENT_DIAG_NAME, CMD_CTRL, subcmd, params, size);
    sem_wait(&g_cmd_complete_sem);
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
void send_cmd(const char *subcmd) {
    send_cmd(subcmd, NULL, 0);
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
static int msg_handle(msg_t * msg) {

    int ret = -1;

    if(msg == NULL) {
        MMI_ALOGE("parameter error.");
        return -1;
    }

    MMI_ALOGI("handle msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d",
              msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);
    switch (msg->cmd) {

    case CMD_CTRL:
        memcpy(&g_msg, msg, sizeof(msg_t));
        sem_post(&g_cmd_complete_sem);
        ret = 0;
        break;
    case CMD_RESULT:
        log_msg(msg->msg);
        break;
    default:
        MMI_ALOGE("Received invalid command: %d", msg->cmd);
        break;
    }

    return ret;
}

/**
 * Handle message thread to read message from pending message queue
 * @Never return.
 */
static void *msg_handle_thread(void *) {

    int ret = -1;
    msg_t *msg = NULL;

    MMI_ALOGI("thread(msg_handle_thread) start for diag");
    while(1) {

        sem_wait(&g_msg_sem);
        dequeue_msg(&g_msg_queue, &msg);

        if(msg != NULL) {
            MMI_ALOGI("dequeue msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d\n",
                     msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);

            /**Skip not diag module command*/
            if(strcmp(msg->module, CLIENT_DIAG_NAME)) {
                MMI_ALOGW("not diag msg: moduld=[%s], cmd=%s, subcmd=%s",
                          msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
                free(msg);
                continue;
            }
            ret = msg_handle(msg);
            if(ret < 0) {
                MMI_ALOGE("Can't handle msg: moduld=[%s], cmd=%s, subcmd=%s",
                          msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
            }
            free(msg);
        }
    }

    return NULL;
}

/**
 * Receive thread handle function
 * @Never return.
 */
static void *msg_waiting_thread(void *s) {

    int recv_size = -1, sock = -1;
    msg_t *msg = NULL;

    if(s == NULL) {
        MMI_ALOGE("Invalid socket id received");
        return NULL;
    }

    sock = *(int *) s;

    MMI_ALOGI("thread(msg_waiting_thread) start for diag");
    while(1) {

        /** Receive a reply from the MMI server */
        msg = (msg_t *) zmalloc(sizeof(msg_t));
        if(msg == NULL) {
            MMI_ALOGE("out of memory, abort the current request, error=%s\n", strerror(errno));
            break;
        }

        if((recv_size = recv(sock, msg, sizeof(msg_t), MSG_WAITALL)) < 0) {
            MMI_ALOGE("msg recv fail, error=%s\n", strerror(errno));
            break;
        }

        MMI_ALOGI("from mmi recv msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d",
                  msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);
        /** Enquenue the request which handled in a single thread*/
        enqueue_msg(&g_msg_queue, msg);

        /**Notify the handle thread*/
        sem_post(&g_msg_sem);
    }

    return NULL;
}

static void usage() {
    const char *usage = "mmi_diag";

    MMI_ALOGI("%s\n", usage);
}

int main(int argc, char *argv[]) {

    int ret = 0;

    pthread_t pid_wait, pid_handle;

    MMI_PROC_TYPE(MMI_PROC_TYPE_MMI_DIAG);
    MMI_ALOGI("start mmi_diag process");

    sem_init(&g_msg_sem, 0, 0);
    sem_init(&g_cmd_complete_sem, 0, 0);

    /** Initial path config from xml file*/
    parse_strings(MMI_PATH_CONFIG);

    ret = diag_init();
    if(ret < 0) {
        MMI_ALOGE("diag init fail");
        goto out;
    }

    MMI_ALOGI("start connect to server");
    /** Connect to MMI server via socket*/
	/*
    g_sock = connect_server(get_value(KEY_MMI_SOCKET));
    if(g_sock < 0) {
        MMI_ALOGE("connect to server(%s) fail", get_value(KEY_MMI_SOCKET));
        goto out;
    }
    */

     /**Ready, say hello to server*/
    ret = say_hello(g_sock, CLIENT_DIAG_NAME);
    if(ret) {
        MMI_ALOGE("send 'Say Hello' to server Fail");
    }

    ret = pthread_create(&pid_wait, NULL, msg_waiting_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg waiting pthread, error=%s\n", strerror(errno));
        goto out;
    }
    MMI_ALOGD("create msg waiting thread(thread id=%lu) for diag\n", pid_wait);

    ret = pthread_create(&pid_handle, NULL, msg_handle_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg handle pthread, error=%s\n", strerror(errno));
        goto out;
    }
    MMI_ALOGD("create msg handle thread(thread id=%lu) for diag\n", pid_handle);

    pthread_join(pid_wait, NULL);
    pthread_join(pid_handle, NULL);

  out:
    MMI_ALOGE("mmi_diag process start fail");
    diag_deinit();
    sem_close(&g_msg_sem);
    sem_close(&g_cmd_complete_sem);
    exit(1);
}
