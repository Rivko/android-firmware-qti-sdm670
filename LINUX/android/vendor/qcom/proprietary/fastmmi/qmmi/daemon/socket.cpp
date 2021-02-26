/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "socket.h"

int create_socket(const char *name) {
    struct sockaddr_un addr;
    int sockfd, ret;

    if(name == NULL) {
        ALOGE("NULL params\n");
        return -1;
    }
    sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sockfd < 0) {
        ALOGE("Failed to open socket '%s', error=%s\n", name, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", name);

    ret = unlink(addr.sun_path);
    if(ret != 0 && errno != ENOENT) {
        ALOGE("Failed to unlink old socket '%s', error=%s\n", name, strerror(errno));
        close(sockfd);
        return -1;
    }

    ret =::bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if(ret) {
        ALOGE("Failed to bind socket '%s', error=%s\n", name, strerror(errno));
        unlink(addr.sun_path);
        close(sockfd);
        return -1;
    }

    chmod(addr.sun_path, (mode_t) 0666);
    ALOGD("Created socket success with '%s', sockfd=%d\n", addr.sun_path, sockfd);

    return sockfd;
}

int send_msg(int fd, msg_t * msg) {
    int ret = 0;

    if(fd > 0) {
        ALOGI("send msg: moduld=[%s], cmd=%d, subcmd=%s, msg_id=%d, msg=%s, msg_size=%d, result=%d via fd=%d",
              msg->module, msg->cmd, msg->subcmd, msg->msg_id, msg->msg, msg->length, msg->result, fd);

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
