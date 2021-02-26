/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QMMI_DAEMON_SOCKET_H
#define __QMMI_DAEMON_SOCKET_H

#include "utils.h"

#define CLIENT_QMMI_NAME "QMMI"
#define MMI_SOCKET "/dev/socket/mmi"
#define SIZE_1K 1024

using namespace std;
typedef void (*callback) (char *, int);

enum {
    FAILED = -1,
    SUCCESS = 0,
    ERR_UNKNOW = INT_MAX
};

/*
 * communication message between mmi core and client
 * */
typedef struct {
    char module[64];
    int msg_id;
    int cmd;
    char subcmd[32];
    int length;
    int result;
    char msg[SIZE_1K];          //para:xxx;  capture:normal:para
} __attribute__ ((__packed__)) msg_t;

/**Queue List*/
typedef struct {
    list < msg_t * >queue;
    pthread_mutex_t lock;
} msg_queue_t;


int create_socket(const char *name);
int connect_server(const char *path);
void enqueue_msg(msg_queue_t * queue, msg_t * msg);
void dequeue_msg(msg_queue_t * queue, msg_t ** msg);
int send_msg(int fd, msg_t * msg);

#endif
