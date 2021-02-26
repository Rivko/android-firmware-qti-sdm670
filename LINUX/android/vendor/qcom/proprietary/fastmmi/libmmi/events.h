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

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <sys/types.h>
typedef struct {
    int width;
    int height;
    int row_bytes;
    int pixel_bytes;
    unsigned char *data;
} GRSurface;

struct uevent {
    const char *action;
    const char *path;
    const char *subsystem;
    const char *firmware;
    const char *partition_name;
    const char *device_name;
    int partition_num;
    int major;
    int minor;
};

struct input_event;

typedef int (*ev_callback) (int fd, uint32_t epevents, void *data);
typedef int (*ev_set_key_callback) (int code, int value, void *data);
typedef int (*input_dev_callback) (input_absinfo * abs_x, input_absinfo * abs_y, void *data);

int ev_init(ev_callback input_cb, void *data);
void ev_exit(void);
int ev_add_fd(int fd, ev_callback cb, void *data);
int ev_sync_key_state(ev_set_key_callback set_key_cb, void *data);

/* timeout has the same semantics as for poll
 *    0 : don't block
 *  < 0 : block forever
 *  > 0 : block for 'timeout' milliseconds
 */
int ev_wait(int timeout);
int ev_get_input(int fd, uint32_t epevents, struct input_event *ev);
void ev_dispatch(void);
int ev_get_epollfd(void);
void monitor_uevent(input_dev_callback input_dev_cb);

#endif
