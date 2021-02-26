/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __FACTORY_HIDL_SOCKET_H__
#define __FACTORY_HIDL_SOCKET_H__

#include <list>
#include <cutils/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "vendor.qti.hardware.factory@1.0-impl"
#endif

#define SIZE_1K 1024
using namespace std;

#define MMI_SOCKET "/dev/socket/mmi"
#define OPS_ON "on"
#define OPS_OFF "off"

enum {
    // client to server
    CMD_HELLO = 0x1,
    CMD_RUN_APP = 0x2,
    CMD_STOP_APP = 0x3,
		CMD_CHARGER_CTRL = 0x4,
		CMD_SHIPMODE_CTRL = 0x5,
		CMD_SMB_CTRL = 0x6,
		CMD_WIFI_CTRL = 0x7,

    // server to client
    CMD_RESULT = 0x101,         // for display/log message 
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

#define CLIENT_QMMI_NAME "QMMI"
void create_socket_thread();
bool check_socket_ready();
msg_t *run_app(const char *name, const char *params, uint32_t size);
msg_t *stop_app(const char *name);
msg_t *charger_enable(bool enable);
msg_t *wifi_enable(bool enable);
msg_t *enter_ship_mode();
msg_t *smb_status();
#endif
