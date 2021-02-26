/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <hardware/vibrator.h>
#include <hardware/hardware.h>
#include "mmi_module.h"

static vibrator_device_t *dev = NULL;
static const char str_vibrator_on[] = "Vibrator on\n";
static const char str_vibrator_off[] = "Vibrator off\n";

static int32_t module_stop(const mmi_module_t * module);

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static int hal_init(unordered_map < string, string > &params) {
    int ret = FAILED;
    hw_module_t *hw_module = NULL;

    ret = hw_get_module(VIBRATOR_HARDWARE_MODULE_ID, (hw_module_t const **) &hw_module);
    if(ret == SUCCESS) {
        ret = vibrator_open(hw_module, &dev);
        if(ret != SUCCESS) {
            MMI_ALOGE("Vibrator FFBM: vibrator open failed\n");
        }
    } else {
        MMI_ALOGE("Vibrator FFBM: hardware module:%s loading failed\n", VIBRATOR_HARDWARE_MODULE_ID);
    }

    return ret;
}

static void hal_deinit(void) {
    if(dev != NULL) {
        dev->common.close((hw_device_t *) dev);
        dev = NULL;
    }
}

static void *run_test(void *mod) {
    signal(SIGUSR1, signal_handler);
    if(dev == NULL) {
        MMI_ALOGE("Invalid dev addr");
        return NULL;
    }

    while(1) {
        MMI_ALOGI("vibrator on 200 ms");

        dev->vibrator_on(dev, 200);
        usleep(800 * 1000);
    }
    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run mmi start for module:[%s]", module->name);

    int ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);

    if(ret < 0) {
        MMI_ALOGE("Can't create thread, error=%s\n", strerror(errno));
        return FAILED;
    } else {
        MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);
        pthread_join(module->run_pid, NULL);
    }

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return ret;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    MMI_ALOGI("vibrator on 30 seconds");
    dev->vibrator_on(dev, (30 * 1000));
    module->cb_print(NULL, SUBCMD_PCBA, str_vibrator_on, strlen(str_vibrator_on), PRINT);

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ERR_UNKNOW;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    ret = hal_init(params);
    if(SUCCESS != ret) {
        MMI_ALOGE("hal init failed");
    }

    MMI_ALOGI("module init finished for module:[%s]", module->name);
    return ret;
}

static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module deinit start for module:[%s]", module->name);

    hal_deinit();
    MMI_ALOGI("module deinit finished for module:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    if(dev != NULL) {
        MMI_ALOGI("vibrator off");
        dev->vibrator_off(dev);
    }

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
    int ret = FAILED;

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = module_run_pcba(module, params);
    else {
        MMI_ALOGE("Received invalid command: %s", MMI_STR(cmd));
        ret = FAILED;
    }

    MMI_ALOGI("module run finished for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
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
    .version_major = 1,
    .version_minor = 0,
    .name = "Vibrator",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
