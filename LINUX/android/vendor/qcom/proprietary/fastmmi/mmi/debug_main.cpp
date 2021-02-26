/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "common.h"
#include "utils.h"
#include "xmlparse.h"

static int g_sock = -1;
static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;

#ifdef MMI_ALOGI
#undef MMI_ALOGI
#define MMI_ALOGI printf
#endif

#ifdef MMI_ALOGE
#undef MMI_ALOGE
#define MMI_ALOGE printf
#endif

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
void send_cmd(const char *subcmd, const char *params, uint32_t size) {
    send_cmd(g_sock, CLIENT_DEBUG_NAME, CMD_CTRL, subcmd, params, size);
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
static void msg_handle(msg_t * msg) {

    if(msg == NULL)
        return;

    switch (msg->cmd) {

    case CMD_RESULT:
        MMI_ALOGI("%s", msg->msg);

        break;
    default:

        MMI_ALOGE("Invalid command received(%d) is invalid)", msg->cmd);
        break;
    }
}

/**
 * Handle message thread to read message from pending message queue
 * @Never return.
 */
static void *console_thread(void *) {

    char user_input[32];
    char case_name[32];

    while(1) {

        fgets(user_input, sizeof(user_input), stdin);
        MMI_ALOGI("Get command from user: %s\n", user_input);
        if(!strcmp(user_input, SUBCMD_STAT)) {
            send_cmd(SUBCMD_STAT);
        } else if(!strncmp(user_input, "run", 3)) {
            snprintf(case_name, sizeof(case_name), "%s:%s", KEY_CASE_NAME, &user_input[3]);
            MMI_ALOGI("run case:%s(size:%lu)\n", case_name, strlen(case_name));
            send_cmd(SUBCMD_RUNCASE, case_name, strlen(case_name));
        } else if(!strcmp(user_input, "runall")) {
            send_cmd(SUBCMD_RUNALL);
        }
    }

    return NULL;
}

/**
 * Handle message thread to read message from pending message queue
 * @Never return.
 */
static void *msg_handle_thread(void *) {

    msg_t *msg = NULL;

    while(1) {

        sem_wait(&g_msg_sem);
        dequeue_msg(&g_msg_queue, &msg);

        if(msg != NULL) {
            MMI_ALOGI("dequeue msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d\n",
                    msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);

            /**Skip not diag module command*/
            if(strcmp(msg->module, CLIENT_DEBUG_NAME)) {
                MMI_ALOGI("not debug msg: moduld=[%s], cmd=%s, subcmd=%s",
                          msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
                free(msg);
                continue;
            }
            msg_handle(msg);
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
        MMI_ALOGE("No sock id received");
        return NULL;
    }

    sock = *(int *) s;

    while(1) {

        /** Receive a reply from the MMI server */
        msg = (msg_t *) zmalloc(sizeof(msg_t));
        if(msg == NULL) {
            MMI_ALOGE("out of memory, abort the current request:(%s)\n", strerror(errno));
            break;
        }

        if((recv_size = recv(sock, msg, sizeof(msg_t), MSG_WAITALL)) < 0) {
            MMI_ALOGE("Recv fail: %s, received size:%d\n", strerror(errno), recv_size);
            break;
        }

        /** Enquenue the request which handled in a single thread*/
        enqueue_msg(&g_msg_queue, msg);

        /**Notify the handle thread*/
        sem_post(&g_msg_sem);
    }

    return NULL;
}

static void usage() {
    const char *usage = "mmi_debug";

    MMI_ALOGI("%s\n", usage);
}

int main(int argc, char *argv[]) {

    int ret = 0;

    pthread_t pid_wait, pid_handle, pid_console;

    MMI_PROC_TYPE(MMI_PROC_TYPE_MMI_DEBUG);

    /** Initial path config from xml file*/
    parse_strings(MMI_PATH_CONFIG);

    sem_init(&g_msg_sem, 0, 0);
    MMI_ALOGI("mmi_debug start\n");

    /** Connect to MMI server via socket*/
    g_sock = connect_server(get_value(KEY_MMI_SOCKET));
    if(g_sock < 0)
        goto out;

     /**Ready, say hello to server*/
    say_hello(g_sock, CLIENT_DEBUG_NAME);
    MMI_ALOGI("mmi_debug connected to server , sock: %d \n", g_sock);

    ret = pthread_create(&pid_console, NULL, console_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg waiting pthread: %s\n", strerror(errno));
        goto out;
    }

    ret = pthread_create(&pid_wait, NULL, msg_waiting_thread, &g_sock);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg waiting pthread: %s\n", strerror(errno));
        goto out;
    }

    ret = pthread_create(&pid_handle, NULL, msg_handle_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg handle pthread: %s\n", strerror(errno));
        goto out;
    }

    pthread_join(pid_wait, NULL);
    pthread_join(pid_handle, NULL);

  out:
    MMI_ALOGI("mmi diag exit to clear resource");
    sem_close(&g_msg_sem);
    exit(1);
}
