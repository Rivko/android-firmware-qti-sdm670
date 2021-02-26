/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __UTILS_H
#define __UTILS_H

#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/sockets.h>
#include <cutils/klog.h>
#include <errno.h>
#include <unordered_map>
#define __STDC_FORMAT_MACROS

#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>

#define SIZE_1K 1024
#define SIZE_2K (SIZE_1K*2)

#define FTM_TEST_CONFIG_DEFAULT "/vendor/etc/ftm_test_config"
#define SYS_SHIP_MODE "/sys/module/qpnp_power_on/parameters/ship_mode_en"
#define SYS_INPUT_SUSPEND "/sys/class/power_supply/battery/input_suspend"
#define SYS_POWER_SUPPLY_PARALLEL "/sys/class/power_supply/parallel/"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG   "mmid"
#endif

using namespace std;
typedef void (*callback) (char *, int);

typedef struct exec_cmd_t {
    const char *cmd;
    char **params;
    char *result;
    char *exit_str;
    int *pid;
    int size;
} exec_cmd_t;

void signal_handler(int signal);
int fork_daemon(const char *filepath, char **args, int *cid);
void kill_proc(int pid);
bool is_proc_exist(int pid);
bool check_file_exist(const char *path);
int exe_cmd(callback cb, exec_cmd_t * exec_cmd);
int read_file(const char *filepath, char *buf, int size);
int write_file(const char *path, const char *content);
bool enter_ship_mode();
void charger_enable(bool enable);
bool smb_ready();
#endif
