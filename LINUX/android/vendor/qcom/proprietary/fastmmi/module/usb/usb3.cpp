/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

/**
* Defined supported command list here.And put into extra_cmd_list,
* so server could get it.
*/
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MODULE_NAME "USB3"
#define MODULE_AUTHOR "Qualcomm Technologies, Inc."
#define RUN_PID -1

static char us_buf[50] = { 0 };

static int us_read(void) {
    if(NULL == us_buf) {
        MMI_ALOGE("get sn error!\n");
        return FAILED;
    }

    if(!check_file_exist(get_value("current_speed"))) {
        MMI_ALOGE("the file(%s) is not exist!\n", get_value("current_speed"));
        return FAILED;
    }

    read_file(get_value("current_speed"), us_buf, sizeof(us_buf));

    return SUCCESS;
}

/**
 * Defined case run in mmi mode,this mode support UI.
 * @return, 0 -success; -1
 */
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received ");
        return FAILED;
    }
    MMI_ALOGI("run mmi start for module:[%s]", module->name);

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    char buf[50] = { 0 };
    int ret = FAILED;

    ret = us_read();
    if(ret != SUCCESS) {
        MMI_ALOGE("read usb speed mode failed");
        return ret;
    }

    strlcat(buf, "Speed Mode : ", sizeof(buf));
    strlcat(buf, us_buf, sizeof(buf));
    strlcat(buf, "\n", sizeof(buf));

    MMI_ALOGI("usb speed mode info: %s", us_buf);
    if(!strncmp(us_buf, "super", 5)) {
        ret = SUCCESS;
        MMI_ALOGI("usb pcba test pass for module:[%s]", module->name);
    } else {
        ret = FAILED;
        MMI_ALOGE("usb pcba test fail for module:[%s]", module->name);
    }

    module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    memset(us_buf, 0, sizeof(us_buf));

    MMI_ALOGI("module init finished for module:[%s]", module->name);
    /* init semaphore set the value 1 */
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module deinit start for module:[%s]", module->name);

    MMI_ALOGI("module deinit finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    kill_thread(module->run_pid);

    MMI_ALOGI("module stop finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    int ret = -1;
    int i = -1;

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s]", module->name);

    ret = module_run_pcba(module, params);

    MMI_ALOGI("usb test %s for module:[%s]", MMI_TEST_RESULT(ret), module->name);
    MMI_ALOGI("module run finished for module:[%s]", module->name);
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
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = RUN_PID,
};
