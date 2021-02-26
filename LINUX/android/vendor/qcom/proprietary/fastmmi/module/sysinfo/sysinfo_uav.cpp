/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/**
* Defined supported command list here.And put into extra_cmd_list,
* so server could get it.
*/
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MODULE_NAME "SystemInfo"
#define MODULE_AUTHOR "Qualcomm Technologies, Inc."
#define RUN_PID -1
#define HW_FILE "/tmp/hwversion"
#define SW_FILE "/tmp/swversion"
#define BUILD_FILE "/system/build.prop"
#define SN_FILE "/mnt/persist/SerialNumber"

static char g_sn[1024] = { 0 };
static char g_hwversion[1024] = { 0 };
static char g_swversion[1024] = { 0 };
static char g_buildversion[1024] = { 0 };

static const char *extra_cmd_list[] = {
};

static int32_t run_hwversion(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_swversion(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_buildversion(const mmi_module_t * module, unordered_map < string, string > &params);

static int write_sn(char *sn) {
    if(NULL == sn) {
        ALOGE("%s get sn error!\n", __FUNCTION__);
        return FAILED;
    }
    int ret = FAILED;
    int fd = -1;

    ret = check_file_exist(SN_FILE);

    if(SUCCESS == ret) {
        remove(SN_FILE);
    }
    creat(SN_FILE, 0766);
    fd = open(SN_FILE, O_RDWR | O_SYNC);
    if(fd < 0) {
        ALOGE("%s can not create the sn file ", __FUNCTION__);
        return ret;
    }
    write(fd, sn, strlen(sn));
    close(fd);
    ret = SUCCESS;
    return ret;
}

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
* Defined OEM specified run cases.
*
*/
static int32_t run_hwversion(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    int ret = -1;

    ret = check_file_exist("/etc/system.prop");
    if(FAILED == ret) {
        ALOGE("[HWversion] %s can not find /etc/system.prop file", __FUNCTION__);
        return ret;
    }
    ALOGI("[HWVERSION] : %s start divide the file\n", __func__);
    system("echo " " > /tmp/hwversion");
    system("sed -n 3p /etc/system.prop >> /tmp/hwversion");
    system("sed -n 4p /etc/system.prop >> /tmp/hwversion");
    system("sed -n 5p /etc/system.prop >> /tmp/hwversion");
    ret = read_file(HW_FILE, g_hwversion, sizeof(g_hwversion));
    if(ret != 0) {
        ALOGE("%s read HW Version error", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t run_swversion(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    int ret = -1;

    ret = check_file_exist("/etc/system.prop");
    if(FAILED == ret) {
        ALOGE("%s can not find /etc/system.prop file", __FUNCTION__);
        return ret;
    }
    system("echo " " > /tmp/swversion");
    system("sed -n 2p /etc/system.prop >> /tmp/swversion");
    system("sed -n 4p /etc/system.prop >> /tmp/swversion");
    system("sed -n 5p /etc/system.prop >> /tmp/swversion");
    ret = read_file(SW_FILE, g_swversion, sizeof(g_swversion));
    if(ret != 0) {
        ALOGE("%s read SW Version error", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t run_buildversion(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    int ret = -1;

    ret = read_file(BUILD_FILE, g_buildversion, sizeof(g_buildversion));
    if(ret != 0) {
        ALOGE("%s read Build Version error", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t run_sn(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    int ret = SUCCESS;

    strlcpy(g_sn, params["SN"].c_str(), sizeof(g_sn));
    ret = write_sn(g_sn);
    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("[hwversion] %s start", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    /* bzero global variable */
    memset(g_sn, 0, sizeof(g_sn));
    memset(g_hwversion, 0, sizeof(g_hwversion));
    memset(g_swversion, 0, sizeof(g_swversion));
    memset(g_buildversion, 0, sizeof(g_buildversion));
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    delete_file(SW_FILE, 0);
    delete_file(HW_FILE, 0);
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    kill_thread(module->run_pid);

    delete_file(SW_FILE, 0);
    delete_file(HW_FILE, 0);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    ALOGI("[hwversion] %s start.", __FUNCTION__);
    int ret = -1;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    char buf[1024] = { 0 };
    char format_buf[256] = { 0 };
    int run_flag = -1;

    /**MMI version info*/
    snprintf(format_buf, sizeof(format_buf), "MMI = %s\n", MMI_VERSION);
    strlcat(buf, format_buf, sizeof(buf));
    /* judge the board eagle or traibalazer */
    run_flag = check_file_exist("/etc/system.prop");
    if(run_flag == SUCCESS) {
        ret = run_hwversion(module, params);
        if(FAILED == ret) {
            strlcat(buf, "no hardware version\n", sizeof(buf));
        } else {
            strlcat(buf, g_hwversion, sizeof(buf));
        }
        ret = run_swversion(module, params);
        if(FAILED == ret) {
            strlcat(buf, "no software version\n", sizeof(buf));
        } else {
            strlcat(buf, g_swversion, sizeof(buf));
        }
    } else {
        run_flag = check_file_exist(BUILD_FILE);
        if(run_flag != SUCCESS) {
            system("mkdir -p /system");
            system("mount /dev/mmcblk0p12 /system");
        }
        run_flag = check_file_exist(BUILD_FILE);
        if(run_flag != SUCCESS) {
            return FAILED;
        }
        ret = run_buildversion(module, params);
        if(FAILED == ret) {
            strlcat(buf, "no build version\n", sizeof(buf));
        } else {
            strlcat(buf, g_buildversion, sizeof(buf));
        }
    }
    /* write sn to persist */
    ret = run_sn(module, params);
    if(ret == FAILED) {
        strlcat(buf, "Can not write sn to the board\n", sizeof(buf));
    } else {
        strlcat(buf, "SN : ", sizeof(buf));
        strlcat(buf, g_sn, sizeof(buf));
        strlcat(buf, "\n", sizeof(buf));
    }
    ALOGI("buf : %s \n", buf);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    }
    /** Default RUN mmi*/
    return ret;
}

/**
* Methods must be implemented by module.
*/
static struct mmi_module_methods_t module_methods = {
    .module_init = module_init,
    .module_deinit = module_deinit,
    .module_run = module_run,
    .module_stop = module_stop,
};

/**
* Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
* and the fields of this data structure must be initialize in strictly sequence as definition,
* please don't change the sequence as g++ not supported in CPP file.
*/
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .name = MODULE_NAME,
    .author = MODULE_AUTHOR,
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = RUN_PID,
};
