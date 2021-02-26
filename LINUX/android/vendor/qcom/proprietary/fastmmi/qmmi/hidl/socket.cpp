/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "cutils/properties.h"
#include "socket.h"

static int g_sock = -1;
static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;
static sem_t g_cmd_complete_sem;
static bool is_socket_ready = false;
msg_t g_msg;
using namespace std;

void enqueue_msg(msg_queue_t * queue, msg_t * msg) {

    ALOGI("send msg: moduld=[%s], cmd=%d, subcmd=%s, msg_id=%d, msg=%s, msg_size=%d, result=%d",
          msg->module, msg->cmd, msg->subcmd, msg->msg_id, msg->msg, msg->length, msg->result);
    pthread_mutex_lock(&queue->lock);
    queue->queue.push_back(msg);
    pthread_mutex_unlock(&queue->lock);
}

void dequeue_msg(msg_queue_t * queue, msg_t ** msg) {
    pthread_mutex_lock(&queue->lock);
    if(!queue->queue.empty()) {
        *msg = queue->queue.front();
        queue->queue.pop_front();
    }
    pthread_mutex_unlock(&queue->lock);
}

int send_msg(int fd, msg_t * msg) {
    int ret = 0;

    if(fd > 0) {
        ALOGI("send msg: moduld=[%s], cmd=%d, subcmd=%s, msg_id=%d, msg=%s, msg_size=%d, result=%d",
              msg->module, msg->cmd, msg->subcmd, msg->msg_id, msg->msg, msg->length, msg->result);

        ret = write(fd, msg, sizeof(msg_t));
        if(ret < 0) {
            ALOGE("write msg fail, error=%s", strerror(errno));
        } else {
        }
    } else {
        ALOGE("Invalid socket fd, socket fd=%d", fd);
    }
    return ret;
}

/**Static basical function for running specified cmd*/
int send_cmd(int sock, const char *module_name, int cmd, const char *subcmd, const char *params, int params_size) {

    msg_t msg;
    int ret = -1;

    if(sock < 0 || module_name == NULL) {
        ALOGE("Invalid socket id(%d) or module name(%s)", sock, !module_name ? "null" : module_name);
        return -1;
    }

    memset(&msg, 0, sizeof(msg_t));
    strlcpy(msg.module, module_name, sizeof(msg.module));
    msg.cmd = cmd;

    if(subcmd != NULL)
        strlcpy(msg.subcmd, subcmd, sizeof(msg.subcmd));

    if(params != NULL) {
        strlcpy(msg.msg, params, sizeof(msg.msg));
        msg.length = params_size;
    }

    ret = send_msg(sock, &msg);
    if(ret <= 0) {
        ALOGE("send msg fail, error=%s", strerror(errno));
        return -1;
    }

    return 0;
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
void send_cmd_sync(int cmd, const char *subcmd, const char *params, uint32_t size) {
    send_cmd(g_sock, CLIENT_QMMI_NAME, cmd, subcmd, params, size);
    sem_wait(&g_cmd_complete_sem);
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
void send_cmd(int cmd, const char *subcmd, const char *params, uint32_t size) {
    send_cmd(g_sock, CLIENT_QMMI_NAME, cmd, subcmd, params, size);
}

/**
 * Say Hello to server
 * @return 0 = success, !0 = failure.
 */
int say_hello() {
    return send_cmd(g_sock, CLIENT_QMMI_NAME, CMD_HELLO, NULL, NULL, 0);
}

/**
 * run app
 * @return 0 = success, !0 = failure.
 */
msg_t *run_app(const char *name, const char *params, uint32_t size) {
    send_cmd(CMD_RUN_APP, name, params, size);
    return &g_msg;
}

/**
 * enter_ship_mode
 * @return 0 = success, !0 = failure.
 */
msg_t *enter_ship_mode() {
    send_cmd_sync(CMD_SHIPMODE_CTRL, OPS_ON, NULL, 0);
    return &g_msg;
}

/**
 * charger_enable
 * @return 0 = success, !0 = failure.
 */
msg_t *charger_enable(bool enable) {
    if(enable)
        send_cmd_sync(CMD_CHARGER_CTRL, OPS_ON, NULL, 0);
    else
        send_cmd_sync(CMD_CHARGER_CTRL, OPS_OFF, NULL, 0);

    return &g_msg;
}

/**
 * wifi_enable
 * @return 0 = success, !0 = failure.
 */
msg_t *wifi_enable(bool enable) {
    if(enable)
        send_cmd_sync(CMD_WIFI_CTRL, OPS_ON, NULL, 0);
    else
        send_cmd_sync(CMD_WIFI_CTRL, OPS_OFF, NULL, 0);

    return &g_msg;
}

/**
 * charger_enable
 * @return 0 = success, !0 = failure.
 */
msg_t *smb_status() {
    send_cmd_sync(CMD_SMB_CTRL, OPS_ON, NULL, 0);
    return &g_msg;
}

/**
 * stop app
 * @return 0 = success, !0 = failure.
 */
msg_t *stop_app(const char *name) {
    send_cmd(CMD_STOP_APP, name, NULL, 0);
    return &g_msg;
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
static int msg_handle(msg_t * msg) {

    int ret = -1;

    if(msg == NULL) {
        ALOGE("parameter error.");
        return -1;
    }

    ALOGI("handle msg: moduld=[%s], cmd=%d, subcmd=%s, msg=%s, msg_size=%d",
          msg->module, msg->cmd, msg->subcmd, msg->msg, msg->length);
    switch (msg->cmd) {

    case CMD_RUN_APP:
    case CMD_STOP_APP:
    case CMD_CHARGER_CTRL:
    case CMD_SHIPMODE_CTRL:
    case CMD_SMB_CTRL:
    case CMD_WIFI_CTRL:
        memcpy(&g_msg, msg, sizeof(msg_t));
        sem_post(&g_cmd_complete_sem);
        ret = 0;
        break;

    case CMD_RESULT:
        break;
    default:
        ALOGE("Received invalid command: %d", msg->cmd);
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

    ALOGI("thread(msg_handle_thread) start for factory HIDL");
    while(1) {

        sem_wait(&g_msg_sem);
        dequeue_msg(&g_msg_queue, &msg);

        if(msg != NULL) {
            ALOGI("handle msg: moduld=[%s], cmd=%d, subcmd=%s, msg=%s, msg_size=%d",
                  msg->module, msg->cmd, msg->subcmd, msg->msg, msg->length);

            /**Skip not diag module command*/
            if(strcmp(msg->module, CLIENT_QMMI_NAME)) {
                ALOGI("handle msg: moduld=[%s], cmd=%d, subcmd=%s, msg=%s, msg_size=%d",
                      msg->module, msg->cmd, msg->subcmd, msg->msg, msg->length);
                free(msg);
                continue;
            }
            ret = msg_handle(msg);
            if(ret < 0) {
                ALOGE("handle msg: moduld=[%s], cmd=%d, subcmd=%s, msg=%s, msg_size=%d",
                      msg->module, msg->cmd, msg->subcmd, msg->msg, msg->length);
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
        ALOGE("Invalid socket id received");
        return NULL;
    }

    sock = *(int *) s;

    ALOGI("thread(msg_waiting_thread) start for factory HIDL");
    while(1) {

        /** Receive a reply from the MMI server */
        msg = (msg_t *) malloc(sizeof(msg_t));
        if(msg == NULL) {
            ALOGE("out of memory, abort the current request, error=%s\n", strerror(errno));
            break;
        }

        if((recv_size = recv(sock, msg, sizeof(msg_t), MSG_WAITALL)) < 0) {
            ALOGE("msg recv fail, error=%s\n", strerror(errno));
            break;
        }

        ALOGI("handle msg: moduld=[%s], cmd=%d, subcmd=%s, msg=%s, msg_size=%d",
              msg->module, msg->cmd, msg->subcmd, msg->msg, msg->length);
        /** Enquenue the request which handled in a single thread*/
        enqueue_msg(&g_msg_queue, msg);

        /**Notify the handle thread*/
        sem_post(&g_msg_sem);
    }

    return NULL;
}

/**
 * Connect to server, Never return if connect fail
 * @return Sock fd.
 */
int connect_server(const char *path) {

    struct sockaddr_un server;
    socklen_t alen = 0;
    int sock, ret = 0;

    if(path == NULL) {
        ALOGE("Invalid parameter\n");
        return -1;
    }

    sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sock < 0) {
        ALOGE("Failed to open socket '%s', error=%s\n", path, strerror(errno));
        return -1;
    }
    /** Initialize address structure */
    memset(&server, 0, sizeof(struct sockaddr_un));

    /** Set address family to unix domain sockets */
    server.sun_family = AF_UNIX;

    /** Set address to the requested pathname */
    snprintf(server.sun_path, sizeof(server.sun_path), "%s", path);

    /** Get length of pathname */
    alen = strlen(server.sun_path) + sizeof(server.sun_family);

    while(1) {
        ret = connect(sock, (struct sockaddr *) &server, alen);
        if(ret == 0)
            break;

        ALOGI("Connect to server '%s' failed, socket id=%d, err = %d, %s", path, sock, ret, strerror(errno));
        sleep(1);
    }
    ALOGI("Connected to server '%s' success, socket id=%d", path, sock);
    is_socket_ready = true;
    return sock;
}

bool check_socket_ready() {
    return is_socket_ready;
}

static void *init_socket(void *s) {

    int ret = 0;

    pthread_t pid_wait, pid_handle;

    sem_init(&g_msg_sem, 0, 0);
    sem_init(&g_cmd_complete_sem, 0, 0);

    /** Connect to MMI server via socket*/
    g_sock = connect_server(MMI_SOCKET);
    if(g_sock < 0) {
        ALOGE("connect to server(%s) fail", MMI_SOCKET);
        goto out;
    }

     /**Ready, say hello to server*/
    ret = say_hello();
    if(ret) {
        ALOGE("send 'Say Hello' to server Fail");
        goto out;
    }

    ret = pthread_create(&pid_wait, NULL, msg_waiting_thread, &g_sock);
    if(ret < 0) {
        ALOGE("Can't create msg waiting pthread, error=%s\n", strerror(errno));
        goto out;
    }
    ALOGI("create msg waiting thread(thread id=%lu) for factory hidl\n", pid_wait);

    ret = pthread_create(&pid_handle, NULL, msg_handle_thread, NULL);
    if(ret < 0) {
        ALOGE("Can't create msg handle pthread, error=%s\n", strerror(errno));
        goto out;
    }
    ALOGI("create msg handle thread(thread id=%lu) for factory hidl\n", pid_handle);

    pthread_join(pid_wait, NULL);
    pthread_join(pid_handle, NULL);
    return NULL;

  out:
    ALOGE("mmi_diag process start fail");
    sem_close(&g_msg_sem);
    sem_close(&g_cmd_complete_sem);
    return NULL;
}

bool start_mmid() {
    char svc_mmid_status[PROPERTY_VALUE_MAX] = { 0 };
    int retry = 10;
    bool started = false;

    while(retry > 0) {
        property_get("init.svc.mmid", svc_mmid_status, "stopped");

        if(!strcmp(svc_mmid_status, "running")) {
            started = true;
            break;
        } else {
            //service not run, start it
            ALOGE("Start mmid time:%d", retry);
            property_set("ctl.start", "mmid");
            sleep(1);
            retry--;
        }
    }

    return started;
}

void create_socket_thread() {
    pthread_t pid;

    if(!start_mmid()) {
        ALOGE("Start MMID failed");
        return;
    }

    int ret = pthread_create(&pid, NULL, init_socket, NULL);

    if(ret < 0) {
        ALOGE("Can't create msg waiting pthread, error=%s\n", strerror(errno));
    }
}
