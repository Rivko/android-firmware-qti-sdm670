/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MMID_DAEMON_H
#define __MMID_DAEMON_H
#include "socket.h"

#define APP_MM_AUDIO_FTM "mm-audio-ftm"
#define APP_MM_AUDIO_FTM_BIN "/vendor/bin/mm-audio-ftm"
#define APP_WIFI_IFCONFIG_BIN "/vendor/bin/ifconfig"

#define MODULE_WIFI_KO "/vendor/bin/mm-audio-ftm"

#define APP_FTMDAEMON "ftmdaemon"
#define APP_FTMDAEMON_BIN "/vendor/bin/ftmdaemon"
#define MAX_PARAM_NUM 32

#define OPS_ON "on"
#define OPS_OFF "off"

using namespace std;

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
    CMD_RESULT = 0x101,
};

/**************************/
class mutex_locker {
    pthread_mutex_t m_mutex;
  public:
      class autolock {
        mutex_locker & locker;
      public:
        inline autolock(mutex_locker & locker):locker(locker) {
            locker.lock();
        }
        inline ~ autolock() {
            locker.unlock();
        }
    };
    inline mutex_locker() {
        pthread_mutex_init(&m_mutex, 0);
    }
    inline ~ mutex_locker() {
        pthread_mutex_destroy(&m_mutex);
    }
    inline void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    inline void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
};

class module_info {
  public:
    char module[64];
    int socket_fd;
    pid_t pid;

      module_info(char *mod) {
        if(mod != NULL)
            strlcpy(module, mod, sizeof(module));

        pid = -1;
        socket_fd = -1;
    }
};

typedef struct {
    char app_name[64];
    int32_t proc_id;
    bool activated;
    mutex_locker lock;
} app_status_t;

void handle_hello(msg_t * msg, module_info * mod);
void handle_run_app(msg_t * msg, module_info * mod);
void handle_stop_app(msg_t * msg, module_info * mod);
void handle_charger_ctrl(msg_t * msg, module_info * mod);
void handle_shipmode_ctrl(msg_t * msg, module_info * mod);
void handle_smb_ctrl(msg_t * msg, module_info * mod);
void handle_wifi_ctrl(msg_t * msg, module_info * mod);

bool is_app_supported(char *name);
app_status_t *get_app_status(string name);
#endif                          /* __MMI_H */
