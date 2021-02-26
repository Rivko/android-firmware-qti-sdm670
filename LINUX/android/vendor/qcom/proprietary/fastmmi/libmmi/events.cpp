/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

 /*
  * Copyright (C) 2007 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include <cutils/uevent.h>
#include <poll.h>

#include "common.h"
#include "events.h"
#include "utils.h"

#define MAX_DEVICES 16
#define MAX_MISC_FDS 16
#define UEVENT_MSG_LEN  2048
#define MAX_TRY_COUNT 5

struct fd_info {
    int fd;
    ev_callback cb;
    void *data;
};

static int epollfd;
static struct epoll_event polledevents[MAX_DEVICES + MAX_MISC_FDS];
static int npolledevents;

static struct fd_info ev_fdinfo[MAX_DEVICES + MAX_MISC_FDS];
static ev_callback g_input_cb;
static input_dev_callback g_input_dev_cb;

static unsigned ev_count = 0;
static unsigned ev_dev_count = 0;
static unsigned ev_misc_count = 0;
static int device_fd = -1;

int ev_init(ev_callback input_cb, void *data) {
    DIR *dir;
    struct dirent *de;
    int fd;
    struct epoll_event ev;
    bool epollctlfail = false;

    epollfd = epoll_create(MAX_DEVICES + MAX_MISC_FDS);
    if(epollfd == -1)
        return -1;

    dir = opendir("/dev/input");
    if(dir != 0) {
        while((de = readdir(dir))) {
            unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];

            if(strncmp(de->d_name, "event", 5))
                continue;
            fd = openat(dirfd(dir), de->d_name, O_RDONLY);
            if(fd < 0)
                continue;

            /* read the evbits of the input device */
            if(ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0) {
                close(fd);
                continue;
            }

            /* TODO: add ability to specify event masks. For now, just assume
             * that only EV_KEY and EV_REL event types are ever needed. */
            if(!test_bit(EV_KEY, ev_bits) && !test_bit(EV_REL, ev_bits) && !test_bit(EV_SW, ev_bits)) {
                close(fd);
                continue;
            } else if(test_bit(EV_ABS, ev_bits) && test_bit(EV_REL, ev_bits)) {
                close(fd);
                continue;
            }

            ev.events = EPOLLIN | EPOLLWAKEUP;
            ev.data.ptr = (void *) &ev_fdinfo[ev_count];
            if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev)) {
                close(fd);
                epollctlfail = true;
                continue;
            }

            ev_fdinfo[ev_count].fd = fd;
            ev_fdinfo[ev_count].cb = input_cb;
            ev_fdinfo[ev_count].data = data;
            ev_count++;
            ev_dev_count++;
            if(ev_dev_count == MAX_DEVICES)
                break;
        }
    }

    if(epollctlfail && !ev_count) {
        close(epollfd);
        epollfd = -1;
        return -1;
    }

    g_input_cb = input_cb;
    return 0;
}

int ev_add_fd(int fd, ev_callback cb, void *data) {
    struct epoll_event ev;
    int ret;

    if(ev_misc_count == MAX_MISC_FDS || cb == NULL)
        return -1;

    ev.events = EPOLLIN | EPOLLWAKEUP;
    ev.data.ptr = (void *) &ev_fdinfo[ev_count];
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    if(!ret) {
        ev_fdinfo[ev_count].fd = fd;
        ev_fdinfo[ev_count].cb = cb;
        ev_fdinfo[ev_count].data = data;
        ev_count++;
        ev_misc_count++;
    }

    return ret;
}

int ev_get_epollfd(void) {
    return epollfd;
}

void ev_exit(void) {
    while(ev_count > 0) {
        close(ev_fdinfo[--ev_count].fd);
    }
    ev_misc_count = 0;
    ev_dev_count = 0;
    close(epollfd);
}

int ev_wait(int timeout) {
    npolledevents = epoll_wait(epollfd, polledevents, ev_count, timeout);
    if(npolledevents <= 0)
        return -1;
    return 0;
}

void ev_dispatch(void) {
    int n;

    for(n = 0; n < npolledevents; n++) {
        struct fd_info *fdi = (fd_info *) (polledevents[n].data.ptr);
        ev_callback cb = fdi->cb;

        if(cb)
            cb(fdi->fd, polledevents[n].events, fdi->data);
    }
}

int ev_get_input(int fd, uint32_t epevents, struct input_event *ev) {
    int r;

    if(epevents & EPOLLIN) {
        r = read(fd, ev, sizeof(*ev));
        if(r == sizeof(*ev))
            return 0;
    }
    return -1;
}

int ev_sync_key_state(ev_set_key_callback set_key_cb, void *data) {
    unsigned long key_bits[BITS_TO_LONGS(KEY_MAX)];
    unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];
    unsigned i;
    int ret;

    for(i = 0; i < ev_dev_count; i++) {
        int code;

        memset(key_bits, 0, sizeof(key_bits));
        memset(ev_bits, 0, sizeof(ev_bits));

        ret = ioctl(ev_fdinfo[i].fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits);
        if(ret < 0 || !test_bit(EV_KEY, ev_bits))
            continue;

        ret = ioctl(ev_fdinfo[i].fd, EVIOCGKEY(sizeof(key_bits)), key_bits);
        if(ret < 0)
            continue;

        for(code = 0; code <= KEY_MAX; code++) {
            if(test_bit(code, key_bits))
                set_key_cb(code, 1, data);
        }
    }

    return 0;
}

static void parse_event(const char *msg, struct uevent *uevent) {
    uevent->action = "";
    uevent->path = "";
    uevent->subsystem = "";
    uevent->firmware = "";
    uevent->major = -1;
    uevent->minor = -1;
    uevent->partition_name = NULL;
    uevent->partition_num = -1;
    uevent->device_name = NULL;

    /* currently ignoring SEQNUM */
    while(*msg) {
        if(!strncmp(msg, "ACTION=", 7)) {
            msg += 7;
            uevent->action = msg;
        } else if(!strncmp(msg, "DEVPATH=", 8)) {
            msg += 8;
            uevent->path = msg;
        } else if(!strncmp(msg, "SUBSYSTEM=", 10)) {
            msg += 10;
            uevent->subsystem = msg;
        } else if(!strncmp(msg, "FIRMWARE=", 9)) {
            msg += 9;
            uevent->firmware = msg;
        } else if(!strncmp(msg, "MAJOR=", 6)) {
            msg += 6;
            uevent->major = atoi(msg);
        } else if(!strncmp(msg, "MINOR=", 6)) {
            msg += 6;
            uevent->minor = atoi(msg);
        } else if(!strncmp(msg, "PARTN=", 6)) {
            msg += 6;
            uevent->partition_num = atoi(msg);
        } else if(!strncmp(msg, "PARTNAME=", 9)) {
            msg += 9;
            uevent->partition_name = msg;
        } else if(!strncmp(msg, "DEVNAME=", 8)) {
            msg += 8;
            uevent->device_name = msg;
        }

        /* advance to after the next \0 */
        while(*msg++);
    }
    MMI_ALOGI("parse uevent { '%s', '%s', '%s', '%s', %d, %d }\n", uevent->action, uevent->path, uevent->subsystem,
              uevent->firmware, uevent->major, uevent->minor);
}

static void handle_device_event(struct uevent *uevent) {
    int fd = 0;
    unsigned long absBitmask[BITS_TO_LONGS(ABS_MAX)];

    if(strstr(uevent->path, "/event") != NULL && !strncmp(uevent->subsystem, "input", 5)) {
        MMI_ALOGI("handle uevent path: %s \n", uevent->path);
        unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];
        char event_fd[PATH_MAX] = { 0 };
        string file_path = string(uevent->path);
        size_t last_index = file_path.find_last_of("/");

        if(last_index != string::npos) {
            file_path = file_path.substr(last_index + 1, file_path.length());
        }
        snprintf(event_fd, sizeof(event_fd), "/dev/input/%s", file_path.c_str());
        MMI_ALOGD("handle event full path: %s \n", event_fd);
        for(int i = 0; i < MAX_TRY_COUNT; i++) {
            fd = open(event_fd, O_RDONLY);
            if(fd < 0)
                usleep(500 * 1000);
            else
                break;
        }
        if(fd < 0) {
            MMI_ALOGD("open file failed: %s %s\n", event_fd, strerror(errno));
            return;
        }

        /* read the evbits of the input device */
        if(ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0) {
            close(fd);
            return;
        }

        /* TODO: add ability to specify event masks. For now, just assume
         * that only EV_KEY and EV_REL event types are ever needed. */
        if(!test_bit(EV_KEY, ev_bits) && !test_bit(EV_REL, ev_bits) && !test_bit(EV_SW, ev_bits)) {
            close(fd);
            return;
        } else if(test_bit(EV_ABS, ev_bits) && test_bit(EV_REL, ev_bits)) {
            close(fd);
            return;
        }

        /**check if it is touch*/
        if(ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask) >= 0) {
            /*See if this is a touch pad. Is this a new modern multi-touch driver */
            if(test_bit(ABS_MT_POSITION_X, absBitmask)
               && test_bit(ABS_MT_POSITION_Y, absBitmask)) {
                MMI_ALOGI("it is touch device, update the ts info");
                input_absinfo abs_x = { 0, 0, 0 };
                input_absinfo abs_y = { 0, 0, 0 };

                if(ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &abs_x) >=0
                  && ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &abs_y)>= 0) {
                    MMI_ALOGI("update touch info \n");
                    if(g_input_dev_cb != NULL)
                        g_input_dev_cb(&abs_x, &abs_y, NULL);
                }
            }
        }

        ev_add_fd(fd, g_input_cb, NULL);
    }
}

void handle_device_fd() {
    MMI_ALOGI("uevent handle_device_fd\n");

    char msg[UEVENT_MSG_LEN + 2];
    int n;

    while((n = uevent_kernel_multicast_recv(device_fd, msg, UEVENT_MSG_LEN)) > 0) {
        if(n >= UEVENT_MSG_LEN) /* overflow -- discard */
            continue;

        msg[n] = '\0';
        msg[n + 1] = '\0';

        struct uevent uevent;

        parse_event(msg, &uevent);
        handle_device_event(&uevent);
    }
}

void *wait_uevent_thread(void *dev_cb) {
    signal(SIGUSR1, signal_handler);
    MMI_ALOGI("thread(wait_uevent_thread) started\n");

    device_fd = uevent_open_socket(256 * 1024, true);
    if(device_fd == -1) {
        MMI_ALOGW("uevent socket fd null");
        return NULL;
    }
    fcntl(device_fd, F_SETFL, O_NONBLOCK);
    pollfd ufd;

    ufd.events = POLLIN;
    ufd.fd = device_fd;
    while(true) {
        ufd.revents = 0;
        int nr = poll(&ufd, 1, -1);

        if(nr <= 0) {
            continue;
        }
        if(ufd.revents & POLLIN) {
            handle_device_fd();
        }
    }

    return NULL;
}

void monitor_uevent(input_dev_callback input_dev_cb) {
    pthread_t wait_uevent_tid;

    g_input_dev_cb = input_dev_cb;

    int retval = pthread_create(&wait_uevent_tid, NULL, wait_uevent_thread, NULL);

    if(retval < 0) {
        MMI_ALOGE("create uevent thread fail, error=%s", strerror(errno));
        return;
    }
    MMI_ALOGD("create wait uevent thread(thread id=%lu) for wait uevent\n", wait_uevent_tid);
}
