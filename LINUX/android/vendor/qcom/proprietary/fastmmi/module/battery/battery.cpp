/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

#define KEY_CHARGING "Charging"
#define KEY_CHARGED "Charged"
#define KEY_FULL "Full"

enum {
    PMIC_CHARGE = 0,
    SMB_CHARGE = 1,
};

static sem_t g_complete_sem;
static int charge_method = PMIC_CHARGE;

static int get_battery_info(char *buf, int size) {
    int ret = FAILED;
    char *p = NULL;
    char tmp[NAME_MAX] = { 0 };

    if(buf == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }

    p = buf;
    if(!read_file(get_value("battery_health"), tmp, sizeof(tmp))) {
        snprintf(p, size, "battery health = %s\n", tmp);
        size -= strlen(buf);
        p = buf + strlen(buf);
    }
    MMI_ALOGI("battery health: %s", tmp);

    if(!read_file(get_value("battery_status"), tmp, sizeof(tmp))) {
        if(!strcasecmp(tmp, KEY_CHARGING) || !strcasecmp(tmp, KEY_CHARGED) || !strcasecmp(tmp, KEY_FULL))
            ret = SUCCESS;

        snprintf(p, size, "PMIC charging status = %s\n", tmp);
        size -= strlen(buf);
        p = buf + strlen(buf);
    }
    MMI_ALOGI("PMIC charging status: %s", tmp);

    if(!read_file(get_value("battery_voltage_now"), tmp, sizeof(tmp))) {
        snprintf(p, size, "battery voltage = %s\n", tmp);
        size -= strlen(buf);
        p = buf + strlen(buf);
    }
    MMI_ALOGI("battery voltage: %s", tmp);

    return ret;
}

static int smb_detect(char *buf, int size) {
    int ret = FAILED;
    char *p = NULL;
    char tmp[NAME_MAX] = { 0 };

    if(buf == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }

    p = buf;

    if(!read_file(get_value("battery_voltage_now"), tmp, sizeof(tmp))) {
        if(!strcasecmp(tmp, KEY_CHARGING) && !strcasecmp(tmp, KEY_CHARGED) && !strcasecmp(tmp, KEY_FULL))
            ret = SUCCESS;

        snprintf(p, size, "SMB charging status = %s\n", tmp);
        size -= strlen(buf);
        p = buf + strlen(buf);
    }
    MMI_ALOGI("SMB charging status: %s", tmp);

    if(!read_file(get_value("battery_voltage_now"), tmp, sizeof(tmp))) {
        snprintf(p, size, "battery voltage = %s\n", tmp);
        size -= strlen(buf);
        p = buf + strlen(buf);
    }
    MMI_ALOGI("battery voltage: %s", tmp);

    return ret;
}

static void *run_test(void *mod) {

    int ret = FAILED;
    char buf[256] = { 0 };
    mmi_module_t *module = (mmi_module_t *) mod;

    if(module == NULL || module->cb_print == NULL) {
        MMI_ALOGE("NULL for cb function");
        return NULL;
    }
    signal(SIGUSR1, signal_handler);
    while(1) {
        if(charge_method == PMIC_CHARGE)
            ret = get_battery_info(buf, sizeof(buf));
        else if(charge_method == SMB_CHARGE)
            ret = smb_detect(buf, sizeof(buf));
        module->cb_print(NULL, SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
        if(ret == SUCCESS) {
            MMI_ALOGI("Usb cable inserted");
            sem_post(&g_complete_sem);
        }
        sleep(1);
    }

    return NULL;
}

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run mmi start for module:[%s]", module->name);

    sem_init(&g_complete_sem, 0, 0);

    if(!strncmp(params["method"].c_str(), "pmic", 4))
        charge_method = PMIC_CHARGE;
    else if(!strncmp(params["method"].c_str(), "smb", 3))
        charge_method = SMB_CHARGE;
    else
        MMI_ALOGE("Invalid params <method>, method=%s\n", MMI_STR(params["method"].c_str()));

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create thread, error=%s\n", strerror(errno));
        return FAILED;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);

    sem_wait(&g_complete_sem);
    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = -1;
    char buf[256] = { 0 };

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    ret = get_battery_info(buf, sizeof(buf));
    module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);

    memset(buf, 0, sizeof(buf));
    if(!read_file(get_value("battery_status"), buf, sizeof(buf))) {
        if(!strcasecmp(buf, KEY_CHARGING) || !strcasecmp(buf, KEY_CHARGED) || !strcasecmp(buf, KEY_FULL))
            ret = SUCCESS;
        else
            ret = FAILED;
    }
    MMI_ALOGI("battery status: %s", buf);

    if(SUCCESS == ret) {
        MMI_ALOGI("module:[%s] test pass", module->name);
    } else {
        MMI_ALOGE("module:[%s] test fail", module->name);
    }

    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for moudle:[%s]", module->name);

    MMI_ALOGI("module init finished for moudle:[%s]", module->name);
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("NULL point  received");
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
    .name = "Battery",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
