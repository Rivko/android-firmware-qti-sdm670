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
static char loc_latitude[] = "LAT";
static char loc_longitude[] = "LON";

#define JUSTIFY_KEY "Stop test"
#define EXIT_STR "Exiting GARDEn"

static const mmi_module_t *g_module = NULL;
static int g_pid = -1;

static pthread_mutex_t g_mutex;

static void cb_function(char *str, int size) {
    if(g_module != NULL) {
        if(size > SIZE_1K) {
            str = str + (size - SIZE_1K);
            g_module->cb_print(NULL, SUBCMD_MMI, str, size, PRINT);
        }
    }
}

static int start_test(char *buf, uint32_t size, unordered_map < string, string > &params) {

    char result[SIZE_1K * 10] = { 0 };
    char tmp[256] = { 0 };
    int ret = FAILED;
    bool found = false;
    exec_cmd_t execmd;

    char *args[12] = { (char *) get_value("garden_app"), (char *) "-z", (char *) "0", (char *) "-u", (char *) "0",
        (char *) "-q", (char *) "0", (char *) "-A", (char *) params["SVs"].c_str(), (char *) "-B",
        (char *) params["SNR"].c_str(), NULL
    };
    execmd.cmd = get_value("garden_app");
    execmd.params = args;
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = result;
    execmd.size = sizeof(result);
    execmd.exit_str = EXIT_STR;
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; j < sizeof(args) / sizeof(char *) && NULL != args[j]; j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(cb_function, &execmd);
    if(ret != SUCCESS) {
        MMI_ALOGE("command:'%s' exec fail", execmd.cmd);
        return FAILED;
    }
    MMI_ALOGI("command:'%s' exec result=%s", execmd.cmd, MMI_STR(execmd.result));

    char *p = result;
    char *ptr;

    while(*p != '\0') {         /*print every line of scan result information */
        ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

        ptr = strstr(tmp, JUSTIFY_KEY);
        if(ptr != NULL) {
            snprintf(buf, size, "GPS test pass\n%s", tmp);
            found = true;
        }
    }

    if(!found) {
        snprintf(tmp, sizeof(tmp), "GPS test fail\n");
        strlcat(buf, tmp, size);
    }

    MMI_ALOGI("GPS test %s", found ? "pass" : "fail");
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
    MMI_ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));

    g_module = module;
    kill_proc(g_pid);

    pthread_mutex_lock(&g_mutex);
    ret = start_test(buf, sizeof(buf), params);
    pthread_mutex_unlock(&g_mutex);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
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
    .name = "GPS",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
