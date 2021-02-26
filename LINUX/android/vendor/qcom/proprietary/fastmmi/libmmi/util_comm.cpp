/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "utils.h"

int create_socket(const char *name) {
    struct sockaddr_un addr;
    int sockfd, ret;

    if(name == NULL) {
        MMI_ALOGE("NULL params\n");
        return -1;
    }
    sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sockfd < 0) {
        MMI_ALOGE("Failed to open socket '%s', error=%s\n", name, strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", name);

    ret = unlink(addr.sun_path);
    if(ret != 0 && errno != ENOENT) {
        MMI_ALOGE("Failed to unlink old socket '%s', error=%s\n", name, strerror(errno));
        close(sockfd);
        return -1;
    }

    ret =::bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if(ret) {
        MMI_ALOGE("Failed to bind socket '%s', error=%s\n", name, strerror(errno));
        unlink(addr.sun_path);
        close(sockfd);
        return -1;
    }

    chmod(addr.sun_path, (mode_t) 0660);
    MMI_ALOGD("Created socket success with '%s', sockfd=%d\n", addr.sun_path, sockfd);

    return sockfd;
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
        MMI_ALOGE("Invalid parameter\n");
        return -1;
    }

    sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sock < 0) {
        MMI_ALOGE("Failed to open socket '%s', error=%s\n", path, strerror(errno));
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

        sleep(1);
    }
    MMI_ALOGI("Connected to server '%s' success, socket id=%d", path, sock);

    return sock;
}

/**
 * Say Hello to server
 * @return 0 = success, !0 = failure.
 */
int say_hello(int sock, const char *module_name) {
    return send_cmd(sock, module_name, CMD_HELLO, NULL, NULL, 0);
}

/**Static basical function for running specified cmd*/
int send_cmd(int sock, const char *module_name, int cmd, const char *subcmd, const char *params, int params_size) {

    msg_t msg;
    int ret = -1;

    if(sock < 0 || module_name == NULL) {
        MMI_ALOGE("Invalid socket id(%d) or module name(%s)", sock, !module_name ? "null" : module_name);
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
        MMI_ALOGE("send msg fail, error=%s", strerror(errno));
        return -1;
    }

    return 0;
}

void enqueue_msg(msg_queue_t * queue, msg_t * msg) {

    MMI_ALOGI("enqueue msg: moduld=[%s], cmd=%s, subcmd=%s, msg_id=%s, msg=%s, msg_size=%d, result=%s",
              msg->module,
              MMI_CMD_NAME(msg->cmd),
              MMI_STR(msg->subcmd), MMI_PRI_TYPE(msg->msg_id), MMI_STR(msg->msg), msg->length, MMI_RESULT(msg->result));
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
        MMI_ALOGI("send msg: moduld=[%s], cmd=%s, subcmd=%s, msg_id=%s, msg=%s, msg_size=%d, result=%s",
                  msg->module,
                  MMI_CMD_NAME(msg->cmd),
                  MMI_STR(msg->subcmd),
                  MMI_PRI_TYPE(msg->msg_id), MMI_STR(msg->msg), msg->length, MMI_RESULT(msg->result));

        ret = write(fd, msg, sizeof(msg_t));
        if(ret < 0) {
            MMI_ALOGE("write msg fail, error=%s", strerror(errno));
        } else {
        }
    } else {
        MMI_ALOGE("Invalid socket fd, socket fd=%d", fd);
    }
    return ret;
}

void signal_handler(int signal) {
    MMI_ALOGI("thread(thread id=%lu) received singal(%d) exit\n", pthread_self(), signal);
    pthread_exit(NULL);
}

bool is_proc_exist(int pid) {
    return kill(pid, 0) == 0;
}

void kill_thread(pthread_t tid) {

    if(tid > 0) {
        int ret = pthread_kill(tid, 0);

        if(ret == ESRCH)
            MMI_ALOGI("No thread with the ID: ", (unsigned int) tid);
        else if(ret == EINVAL)
            MMI_ALOGI("An invalid signal was specified ");
        else {
            pthread_kill(tid, SIGUSR1);
            pthread_join(tid, NULL);
            MMI_ALOGI("thread(thread id=%lu) be killed\n", tid);
        }
    }
}

void kill_proc(int pid) {
    int stat;

    if(pid > 0 && is_proc_exist(pid)) {
        kill(pid, SIGKILL);
        waitpid(pid, &stat, 0);
        if(WIFSIGNALED(stat))
            MMI_ALOGI("Child process(pid=%d) received signal(%d) and exit\n", pid, WTERMSIG(stat));

    }
}
