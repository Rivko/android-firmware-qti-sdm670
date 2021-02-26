/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static const mmi_module_t *g_module = NULL;
static int g_pid;
static pthread_mutex_t g_mutex;

#define KEY_WORD_DEVICE "Device name is :"
#define KEY_WORD_RSSI "Device RSSI VALUE is"
#define KEY_WORD_ADDRESS "Device address is :"

typedef struct {
    char ap_device[64];
    char rssi[64];
} bt_info;

static void cb_function(char *str, int size) {
    if(g_module != NULL)
        g_module->cb_print(NULL, SUBCMD_MMI, str, size, PRINT);
}

static int start_test(char *buf, uint32_t size, unordered_map < string, string > &params) {
    char result[SIZE_1K * 4];
    char tmp[256];
    int ret = FAILED;
    bool found = false;
    bt_info bt_result;
    const char *args[3];
    int i = 0;
    exec_cmd_t execmd;

    //initial the local variable
    memset(&bt_result, 0, sizeof(bt_info));

    if(buf == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }

    if(!check_file_exist(get_value("bdt"))) {
        MMI_ALOGE("file(%s) not exist", get_value("bdt"));
        return FAILED;
    }

    args[i++] = (char *) get_value("bdt");
    if(!params["method"].empty()) {
        args[i++] = params["method"].c_str();
        args[i] = NULL;
    } else {
        args[i++] = "get_a_device";
        args[i] = NULL;
    }

    execmd.cmd = (char *) get_value("bdt");
    execmd.params = (char **) args;
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = result;
    execmd.size = sizeof(result);
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; j < i; j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(cb_function, &execmd);
    if(ret != SUCCESS) {
        MMI_ALOGE("'%s' exec fail", execmd.cmd);
        return FAILED;
    }
    MMI_ALOGI("'%s' exec result: %s", execmd.cmd, result);

    char *p = result;
    char *ptr;

    while(*p != '\0') {         /*print every line of scan result information */
        ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

        ptr = strstr(tmp, KEY_WORD_ADDRESS);
        if(strstr(tmp, KEY_WORD_ADDRESS) != NULL) {
            found = true;
            ptr = strstr(tmp, ":");
            ptr++;
            memset(bt_result.ap_device, 0, sizeof(bt_result.ap_device));
            snprintf(bt_result.ap_device, sizeof(bt_result.ap_device), "AP=Device name:%s;", ptr);

            MMI_ALOGI("AP device name: %s", ptr);
        } else if(strstr(tmp, KEY_WORD_DEVICE) != NULL) {
            ptr = strstr(tmp, ":");
            ptr++;
            memset(bt_result.ap_device, 0, sizeof(bt_result.ap_device));
            snprintf(bt_result.ap_device, sizeof(bt_result.ap_device), "AP=Device name:%s;", ptr);

            MMI_ALOGI("AP device name: %s", ptr);
        } else if(strstr(tmp, KEY_WORD_RSSI) != NULL) {
            found = true;
            if(strstr(bt_result.ap_device, "AP=Device name") == NULL)
                continue;
            ptr = strstr(tmp, ":");
            ptr++;
            memset(bt_result.rssi, 0, sizeof(bt_result.rssi));
            snprintf(bt_result.rssi, sizeof(bt_result.rssi), "RSSI:%s;", ptr);
            strlcat(buf, bt_result.ap_device, size);
            strlcat(buf, bt_result.rssi, size);
            strlcat(buf, "\n", size);

            MMI_ALOGI("device RSSI: %s", ptr);
        }
    }

    return found ? SUCCESS : FAILED;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    g_module = module;
    pthread_mutex_init(&g_mutex, NULL);

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
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    kill_proc(g_pid);
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
    char buf[SIZE_1K] = { 0 };

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s], subcmd:%s", module->name, MMI_STR(cmd));
    g_module = module;

    pthread_mutex_lock(&g_mutex);
    ret = start_test(buf, sizeof(buf), params);
    if(SUCCESS == ret) {
        MMI_ALOGI("module:[%s] test %s", module->name, MMI_TEST_RESULT(ret));
    } else {
        MMI_ALOGE("module:[%s] test %s", module->name, MMI_TEST_RESULT(ret));
    }
    pthread_mutex_unlock(&g_mutex);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    } else {
        MMI_ALOGE("Received error subcmd: %s", MMI_STR(cmd));
    }

    MMI_ALOGI("module run finished for module:[%s], subcmd:%s", module->name, MMI_STR(cmd));
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
    .name = "Bluetooth",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
