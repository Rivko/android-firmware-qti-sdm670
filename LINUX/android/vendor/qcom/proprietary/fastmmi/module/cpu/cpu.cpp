/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

int get_core_count(void) {
    char buf[256] = { 0 };

    if(read_file(get_value("cpu_present"), buf, sizeof(buf))) {
        MMI_ALOGE("FFBM CPU: Cannot read the core count of CPU");
        return -1;
    }

    string input(buf);
    int splitIndex = input.find_first_of('-');

    if((splitIndex > 0) && (splitIndex < input.length())) {
        return (buf[splitIndex + 1] - '0' + 1);
    }

    return -1;
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

    int ret = FAILED;
    char buf[NAME_MAX] = { 0 };
    char path[256] = { 0 };
    uint64_t max_freq = 0, min_limit = 0, max_limit = 0;
    uint64_t temp_freq = 0;
    int i = 0;
    int core_count = 0;

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));

    core_count = get_core_count();
    if(core_count < 0) {
        MMI_ALOGE("FFBM CPU: The core count is wrong");
        return FAILED;
    }
    MMI_ALOGD("cpu core count: %d", core_count);

    for(i = 0; i < core_count; i++) {
        snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);
        if(!read_file(path, buf, sizeof(buf)))
            temp_freq = string_to_long(buf);
        if((temp_freq != 0) && (temp_freq > max_freq))
            max_freq = temp_freq;
    }
    MMI_ALOGD("Get CPU max frequency: %ld Hz", max_freq);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        snprintf(buf, sizeof(buf), "CPUFREQ MAX = %" PRIu64 " Hz \n", max_freq);
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
        ret = SUCCESS;
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        min_limit = string_to_long(params[KEY_MIN_LIMINT]);
        max_limit = string_to_long(params[KEY_MAX_LIMINT]);
        if(max_limit != 0) {
            if(max_freq > min_limit && max_freq <= max_limit)
                ret = SUCCESS;
        }

        MMI_ALOGD("CPU frequency min limit: %llu kB, max limit: %llu kB", min_limit, max_limit);
        snprintf(buf, sizeof(buf), "CPUFREQ MAX = %" PRIu64 " Hz \n", max_freq);
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    }

    MMI_ALOGI("module run finished for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));
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
    .name = "CPU",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
