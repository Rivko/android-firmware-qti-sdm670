/********************************************************************
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>

#define LOG_BUF_SIZE 1024
#define QMP_SOCK_NAME "/dev/socket/seempdw"
#ifndef __unused
#define __unused  __attribute__((__unused__))
#endif

static int __seemp_write_to_log_init(struct iovec *vec, size_t nr);
static int (*__seemp_write_to_log)(struct iovec *vec, size_t nr) = __seemp_write_to_log_init;
static int seemp_logd_fd = -1;

// give up, resources too limited
static int __seemp_write_to_log_null(struct iovec *vec __unused,
                               size_t nr __unused)
{
    return -1;
}

// log_init_lock assumed
static int __seemp_write_to_log_initialize()
{
    int i, ret = 0;
    if (seemp_logd_fd >= 0) {
        i = seemp_logd_fd;
        seemp_logd_fd = -1;
        close(i);
    }

    i = socket(PF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (i < 0) {
        ret = -errno;
        __seemp_write_to_log = __seemp_write_to_log_null;
    } else if (fcntl(i, F_SETFL, O_NONBLOCK) < 0) {
        ret = -errno;
        close(i);
        i = -1;
        __seemp_write_to_log = __seemp_write_to_log_null;
    } else {
        struct sockaddr_un un;
        memset(&un, 0, sizeof(struct sockaddr_un));
        un.sun_family = AF_UNIX;
        memcpy(un.sun_path, QMP_SOCK_NAME, strlen(QMP_SOCK_NAME)+1);
        if (connect(i, (struct sockaddr *)&un, sizeof(struct sockaddr_un)) < 0) {
            ret = -errno;
            close(i);
            i = -1;
        }
    }
    seemp_logd_fd = i;
    return ret;

}

static int __seemp_write_to_log_socket(struct iovec *vec, size_t nr)
{
    ssize_t ret;
    if (seemp_logd_fd < 0) {
        return -EBADF;
    }

    //
    // The write below could be lost, but will never block.
    //
    // ENOTCONN occurs if logd dies.
    // EAGAIN occurs if logd is overloaded.
    //

    ret = writev(seemp_logd_fd, vec, nr);
    if (ret < 0) {
        ret = -errno;
        if (ret == -ENOTCONN) {
            ret = __seemp_write_to_log_initialize();
            if (ret < 0) {
                return ret;
            }

            ret = writev(seemp_logd_fd, vec, nr);
            if (ret < 0) {
                ret = -errno;
            }
        }
    }

    return ret;
}

static int __seemp_write_to_log_init(struct iovec *vec, size_t nr)
{
    if (__seemp_write_to_log == __seemp_write_to_log_init) {
        int ret;

        ret = __seemp_write_to_log_initialize();
        if (ret < 0) {
            return ret;
        }

        __seemp_write_to_log = __seemp_write_to_log_socket;
    }
    return __seemp_write_to_log(vec, nr);
}

static int __android_qmp_seemp_socket_write(int len, const char *msg)
{
     struct iovec vec;
     vec.iov_base = (void *) msg;
     vec.iov_len  = len;

     return __seemp_write_to_log(&vec, 1);
}

void seemp_log_record(int apiID, const char* msgStr)
{
    unsigned int apiIDLen = sizeof(apiID);
    unsigned int msgLen = strlen(msgStr);
    unsigned int len = apiIDLen + 1 + msgLen + 1;
    char *msg = (char*)malloc(len);
    if ( NULL == msg )
    {
        return;
    }
    char *params = msg + apiIDLen + 1; // app_id + encoding byte + params
    *((int*)msg) = apiID;

    memcpy(params, msgStr, msgLen + 1);
    msg[len - 1] = 0;

    __android_qmp_seemp_socket_write(len, msg);
    free(msg);
    return;
}
