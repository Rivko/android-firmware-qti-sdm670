/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

static unordered_map < string, string > paras;
static char module_name[64] = { 0 };

static char *p_module_name = NULL;
static mutex_locker g_mutex;

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static void light_on(bool enable) {
    if(enable) {
        if(!paras["type"].empty()) {
            if(!strncmp(paras["type"].c_str(), "flash_0", 7))
                write_file(get_value("flashlight_torch_0"), "200");
            else if(!strncmp(paras["type"].c_str(), "flash_1", 7))
                write_file(get_value("flashlight_torch_1"), "200");
            else
                MMI_ALOGE("Invalid params <type>, type=%s", paras["type"].c_str());
        } else {
            MMI_ALOGE("not set flashlight type in the parameter");
        }
        if(check_file_exist(get_value("leds_switch")))
            write_file(get_value("leds_switch"), "1");
        else
            write_file(get_value("leds_switch_0"), "1");
    } else {
        if(check_file_exist(get_value("flashlight_torch_0")))
            write_file(get_value("flashlight_torch_0"), "0");
        if(check_file_exist(get_value("flashlight_torch_1")))
            write_file(get_value("flashlight_torch_1"), "0");
        if(check_file_exist(get_value("leds_switch")))
            write_file(get_value("leds_switch"), "0");
        else
            write_file(get_value("leds_switch_0"), "0");
    }
}

static void *run_test(void *mod) {
    int delay = 0;

    if(mod == NULL) {
        MMI_ALOGE("NULL for cb function");
        return NULL;
    }
    delay = atoi(paras["delay"].c_str());

    signal(SIGUSR1, signal_handler);

    while(1) {
        light_on(true);
        // ((mmi_module_t *) mod)->cb_print(NULL, SUBCMD_MMI, "flashlight on", 13, PRINT);
        ((mmi_module_t *) mod)->cb_print(p_module_name, SUBCMD_MMI, "flashlight on", 13, PRINT);
        usleep(delay * 1000);
        light_on(false);
        // ((mmi_module_t *) mod)->cb_print(NULL, SUBCMD_MMI, "flashlight off", 14, PRINT);
        ((mmi_module_t *) mod)->cb_print(p_module_name, SUBCMD_MMI, "flashlight off", 14, PRINT);
        usleep(delay * 1000);
    }

    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run mmi start for module:[%s]", module->name);
    int ret = FAILED;

    paras = params;

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create pthread, error=%s\n", strerror(errno));
        return FAILED;
    } else {
        MMI_ALOGD("create thread(thread id=%d) for module:[%s]\n", module->run_pid, module->name);
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

    paras = params;
    light_on(true);
    // module->cb_print(NULL, SUBCMD_PCBA, "flashlight on", 13, PRINT);
    module->cb_print(p_module_name, SUBCMD_PCBA, "flashlight on", 13, PRINT);

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ERR_UNKNOW;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    MMI_ALOGI("module init finished for module:[%s]", module->name);
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
        MMI_ALOGE("NULL point received ");
        return FAILED;
    }
    mutex_locker::autolock _L(g_mutex);
    MMI_ALOGI("module stop start for module:[%s]", module->name);
    kill_thread(module->run_pid);
    light_on(false);

    MMI_ALOGI("thread(thread id=%lu) be killed for module:[%s]", module->run_pid, module->name);
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

    if(!params[KEY_MODULE_NAME].empty()) {
        snprintf(module_name, sizeof(module_name), "%s", params[KEY_MODULE_NAME].c_str());
        p_module_name = module_name;
    } else {
        p_module_name = NULL;
    }

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
    .name = "Flashlight",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
