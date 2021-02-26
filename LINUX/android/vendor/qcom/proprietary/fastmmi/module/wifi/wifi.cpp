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
static int g_pid = -1;

static pthread_mutex_t g_mutex;

#define KEY_WORD_CHANNEL "Channel:"
#define KEY_WORD_ESSID "ESSID:"
#define KEY_WORD_SIGNAL_LEVEL "Signal level"
#define KEY_WORD_SSID "SSID"
#define KEY_WORD_SIGNAL "signal"
#define KEY_WORD_FREQ "freq"

enum {
    METHOD_NORMAL = 0,
    METHOD_2_4G = 1,
    METHOD_5G = 2,
};

typedef struct {
    char channel[64];
    char essid[64];
    char signal_level[128];
} wifi_info;

static void cb_function(char *str, int size) {
    if(g_module != NULL)
        g_module->cb_print(NULL, SUBCMD_MMI, str, size, PRINT);
}

static void load_driver() {
    char temp[256] = { 0 };
    snprintf(temp, sizeof(temp), "insmod %s", get_value("wifi_driver"));
    system(temp);

    MMI_ALOGI("exec '%s' to load wifi driver", temp);
}

static void unload_driver() {
    system("rmmod wlan");

    MMI_ALOGI("exec 'rmmod wlan' to unload wifi driver");
}

static void config_wlan() {
    char temp[256] = { 0 };
    snprintf(temp, sizeof(temp), "%s wlan0 up", get_value("wifi_ifconfig"));
    system(temp);

    MMI_ALOGI("exec '%s' to config wifi", temp);
}

static int exec_wifi_cmd(char *result, int result_size) {
    int ret = FAILED;
    char *args[4];
    int i = 0;
    exec_cmd_t execmd;

    if(check_file_exist(get_value("wifi_iw"))) {
        args[i++] = (char *) get_value("wifi_iw");
        args[i++] = (char *) "wlan0";
        args[i++] = (char *) "scan";
        args[i] = (char *) NULL;
        execmd.cmd = (char *) get_value("wifi_iw");
    } else if(check_file_exist(get_value("wifi_iwlist"))) {
        args[i++] = (char *) get_value("wifi_iwlist");
        args[i++] = (char *) "wlan0";
        args[i++] = (char *) "scanning";
        args[i] = (char *) NULL;
        execmd.cmd = (char *) get_value("wifi_iwlist");
    } else {
        MMI_ALOGE("Cannot find WIFI tool to scan device");
        return ret;
    }
    execmd.params = args;
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = result;
    execmd.size = result_size;
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; (j < sizeof(args) / sizeof(char *)) && (NULL != args[j]); j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);

        MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(cb_function, &execmd);
    return ret;
}

static bool parse_exec_result(int method, char *result, char *buf, uint32_t size) {
    wifi_info wifi_result;
    char tmp[256] = { 0 };
    char *p = result;
    char *ptr;
    char *ptr_end;
    bool found = false;
    bool found_2_4g = false;
    bool found_5g = false;
    int freq_temp = 0;

    //initial the local variable
    memset(&wifi_result, 0, sizeof(wifi_info));

    while(*p != '\0') {         /*print every line of scan result information */
        ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

        if(check_file_exist(get_value("wifi_iw"))) {
            if(strstr(tmp, KEY_WORD_FREQ) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                freq_temp = atoi(ptr);
                if(atoi(ptr) > 5000)
                    found_5g = true;
                else if(atoi(ptr) < 2500)
                    found_2_4g = true;
                memset(wifi_result.channel, 0, sizeof(wifi_result.channel));
                snprintf(wifi_result.channel, sizeof(wifi_result.channel), "Channel:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_SIGNAL) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.signal_level, 0, sizeof(wifi_result.signal_level));
                snprintf(wifi_result.signal_level, sizeof(wifi_result.signal_level), "Signal level:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_SSID) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.essid, 0, sizeof(wifi_result.essid));
                snprintf(wifi_result.essid, sizeof(wifi_result.essid), "AP=ESSID:%s;", ptr);
                found = true;
                if(((method == METHOD_2_4G) && (freq_temp < 2500))
                  || ((method == METHOD_5G) && (freq_temp > 5000))
                  || (method == METHOD_NORMAL)) {
                    strlcat(buf, wifi_result.essid, size);
                    strlcat(buf, wifi_result.channel, size);
                    strlcat(buf, wifi_result.signal_level, size);
                    strlcat(buf, "\n", size);
                }
            }
        } else if(check_file_exist(get_value("wifi_iwlist"))) {
            if(strstr(tmp, KEY_WORD_CHANNEL) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                freq_temp = atoi(ptr);
                if(atoi(ptr) > 35)
                    found_5g = true;
                else if(atoi(ptr) < 15)
                    found_2_4g = true;
                memset(wifi_result.channel, 0, sizeof(wifi_result.channel));
                snprintf(wifi_result.channel, sizeof(wifi_result.channel), "Channel:%s;", ptr);
            } else if(strstr(tmp, KEY_WORD_ESSID) != NULL) {
                ptr = strstr(tmp, ":");
                ptr++;
                memset(wifi_result.essid, 0, sizeof(wifi_result.essid));
                snprintf(wifi_result.essid, sizeof(wifi_result.essid), "AP=ESSID:%s;", ptr);
                found = true;
            } else if(strstr(tmp, KEY_WORD_SIGNAL_LEVEL) != NULL) {
                ptr = strstr(tmp, KEY_WORD_SIGNAL_LEVEL);
                ptr_end = strstr(tmp, "Noise level");
                memset(wifi_result.signal_level, 0, sizeof(wifi_result.signal_level));
                snprintf(wifi_result.signal_level, (ptr_end - ptr), "Signal level:%s;", (ptr + 13));
                if(((method == METHOD_2_4G) && (freq_temp < 15))
                  || ((method == METHOD_5G) && (freq_temp > 35))
                  || (method == METHOD_NORMAL)) {
                    strlcat(buf, wifi_result.essid, size);
                    strlcat(buf, wifi_result.channel, size);
                    strlcat(buf, wifi_result.signal_level, size);
                    strlcat(buf, "\n", size);
                }
            }
        }
    }

    if((method == METHOD_NORMAL) && found) {
        MMI_ALOGI("wifi test pass, get wifi info: %s", buf);
        return found;
    } else if((method == METHOD_2_4G) && found_2_4g && found) {
        MMI_ALOGI("wifi 2.4G test pass, get wifi info: %s", buf);
        return found;
    } else if((method == METHOD_5G) && found_5g && found) {
        MMI_ALOGI("wifi 5G test pass, get wifi info: %s", buf);
        return found;
    } else {
        MMI_ALOGE("wifi test fail");
        return FAILED;
    }
}

static int start_test(int method, char *buf, uint32_t size) {
    char result[800 * SIZE_1K] = { 0 };
    char tmp[256] = { 0 };
    char *args[4];
    int ret = FAILED;
    bool found = false;
    int i = 0;

    if(!check_file_exist(get_value("wifi_driver")) || !check_file_exist(get_value("wifi_ifconfig"))
       || (!check_file_exist(get_value("wifi_iwlist")) && !check_file_exist(get_value("wifi_iw"))))
        return FAILED;

    load_driver();
    config_wlan();
    ret = exec_wifi_cmd(result, sizeof(result));
    unload_driver();

    if(ret != SUCCESS) {
        MMI_ALOGE("exec wifi command failed");
        return FAILED;
    }

    found = parse_exec_result(method, result, buf, size);

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
    int method = METHOD_NORMAL;
    char buf[SIZE_1K] = { 0 };

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
    g_module = module;

    pthread_mutex_lock(&g_mutex);
    if(!strncmp(params["method"].c_str(), "normal", 6))
        method = METHOD_NORMAL;
    else if(!strncmp(params["method"].c_str(), "2.4G", 4))
        method = METHOD_2_4G;
    else if(!strncmp(params["method"].c_str(), "5G", 2))
        method = METHOD_5G;

    ret = start_test(method, buf, sizeof(buf));

    MMI_ALOGI("module:[%s] test %s", module->name, MMI_TEST_RESULT(ret));
    pthread_mutex_unlock(&g_mutex);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    } else {
        MMI_ALOGE("Received invalid command: %s", MMI_STR(cmd));
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
    .name = "Wifi",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
