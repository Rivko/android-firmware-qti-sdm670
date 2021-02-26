/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __NL_WRAPPER__
#define __NL_WRAPPER__

#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/object-api.h>
#include <linux/pkt_sched.h>

#include "nl80211_copy.h"
#include "common.h"

#define SOCKET_BUFFER_SIZE      (32768U)
#define RECV_BUF_SIZE           (4096)
#define DEFAULT_EVENT_CB_SIZE   (64)
#define DEFAULT_CMD_SIZE        (64)

struct cb_info {
    int nl_cmd;
    uint32_t vendor_id;
    int vendor_subcmd;
    nl_recvmsg_msg_cb_t cb_func;
    void *cb_arg;
};

struct nlIfaceInfo {
    struct nl_sock *cmd_sock;                       // command socket object
    struct nl_sock *event_sock;                     // event socket object
    struct cb_info *event_cb;                              // event callbacks
    int num_event_cb;                               // number of event callbacks
    int alloc_event_cb;                             // number of allocated callback objects
    pthread_mutex_t cb_lock;                        // mutex for the event_cb access
    int nl80211_family_id;                          // family id for 80211 driver
    u8 clean_up;
    u8 in_event_loop;
};

struct nlattr * attr_start(struct nl_msg *nlmsg, int attribute);
void attr_end(struct nl_msg *nlmsg, struct nlattr *attr);
static void parseNested(struct cmd_params *response, struct nlattr *tb_vendor,
                 int index);
#endif
