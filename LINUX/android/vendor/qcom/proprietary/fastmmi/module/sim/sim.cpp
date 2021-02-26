/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

#define MAX_SIM_NUM 3
#define JUSTIFY_KEY "card state Present"

#define SIM_INPUT_FILE "sim_input.txt"
#define SIM_OUTPUT_FILE "sim_output.txt"

static char input[PATH_MAX];
static char input_file[PATH_MAX];
static char output[PATH_MAX];
static char output_file[PATH_MAX];

static int g_pid = -1;
static pthread_mutex_t g_mutex;
static int sim_state[MAX_SIM_NUM];

static int prepare_params() {
    FILE *fp = NULL;
    char str[256] = "card status";

    fp = fopen(input_file, "w");
    if(fp == NULL) {
        MMI_ALOGE("file(%s) open fail, error=%s", input_file, strerror(errno));
        return -1;
    }

    fwrite(str, 1, strlen(str), fp);
    fclose(fp);
    /*output file empty */
    fp = fopen(output_file, "w");
    if(fp == NULL) {
        MMI_ALOGE("file(%s) open fail, error=%s", output_file, strerror(errno));
        return -1;
    }
    fclose(fp);
    return 0;
}

static void parse_sim_state() {
    char line[1024] = { 0, };
    bool found = false;
    bool simExist = false;
    int index = 0;

    MMI_ALOGI("start parse simcard state");
    FILE *file = fopen(output_file, "r");

    if(file == NULL) {
        MMI_ALOGE("file(%s) open fail, error=%s", output_file, strerror(errno));
        return;
    }

    memset(sim_state, 0, sizeof(sim_state));

    while(fgets(line, sizeof(line), file) != NULL) {

        if(line[0] == '#') {
            continue;
        }

        if(found) {
            found = false;
            if(strstr(line, JUSTIFY_KEY) != NULL) {
                simExist = true;
                if(index >= 0 && index < MAX_SIM_NUM) {
                    sim_state[index] = 1;
                    MMI_ALOGI("FFBM SIM: found simcard%d\n", index);
                } else {
                    MMI_ALOGE("Sim card not found, index error: ", index);
                }
            }
        }

        if(!strncmp(line, "CARD", 4)) {
            get_device_index(line, "CARD", &index);
            found = true;
            continue;
        }
    }

    if(!simExist) {
        MMI_ALOGE("FFBM SIM: no found simcard\n");
    }
    fclose(file);
}

static void *thread_test(void *) {
    signal(SIGUSR1, signal_handler);

    int ret = FAILED;
    char buf[SIZE_1K] = { 0 };
    exec_cmd_t execmd;

    if(!check_file_exist(get_value("qmi_simple_ril_test"))) {
        MMI_ALOGE("file(%s) not exist\n", get_value("qmi_simple_ril_test"));
        return NULL;
    }
    char *args[4] = { (char *) get_value("qmi_simple_ril_test"), input, output, NULL };
    execmd.cmd = get_value("qmi_simple_ril_test");
    execmd.params = args;
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = buf;
    execmd.size = sizeof(buf);
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; j < sizeof(args) / sizeof(char *) && NULL != args[j]; j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    exe_cmd(NULL, &execmd);
    MMI_ALOGI("'%s' exec result: %s", execmd.cmd, MMI_STR(execmd.result));

    return NULL;
}

static void start_test(const mmi_module_t * module) {
    MMI_ALOGI("start test for module:[%s]\n", module->name);
    int ret = -1;

    prepare_params();
    ret = pthread_create((pthread_t *) & module->run_pid, NULL, thread_test, NULL);
    if(ret < 0) {
        MMI_ALOGE("Thread create fail, error=%s\n", strerror(errno));
        return;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);

    /**waiting sometime and then kill the process*/
    usleep(1000 * 3000);
    kill_proc(g_pid);

    pthread_join(module->run_pid, NULL);
    /**parse output file*/
    parse_sim_state();
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    pthread_mutex_init(&g_mutex, NULL);
    snprintf(input_file, sizeof(input_file), "%s%s", get_value(KEY_FTM_AP_DIR), SIM_INPUT_FILE);
    snprintf(output_file, sizeof(output_file), "%s%s", get_value(KEY_FTM_AP_DIR), SIM_OUTPUT_FILE);
    snprintf(input, sizeof(input), "input=%s", input_file);
    snprintf(output, sizeof(output), "output=%s", output_file);
    MMI_ALOGI("input file: '%s' for module:[%s]", input_file, module->name);
    MMI_ALOGI("output file: '%s' for module:[%s]", output_file, module->name);

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

    MMI_ALOGI("test thread(thread id=%lu) be killed for module:[%s]", module->run_pid, module->name);
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

    pthread_mutex_lock(&g_mutex);
    start_test(module);
    pthread_mutex_unlock(&g_mutex);

    for(int i = 0; i < MAX_SIM_NUM; i++)
        MMI_ALOGI("simCard%d status: %s", i, sim_state[i] ? "present" : "absent");

    if(!strcmp(params["sub"].c_str(), "0")) {
        if(sim_state[0])
            ret = SUCCESS;
        snprintf(buf, sizeof(buf), "status = %s \n", sim_state[0] ? "detected" : "not detected");
    } else if(!strcmp(params["sub"].c_str(), "1")) {
        if(sim_state[1])
            ret = SUCCESS;
        snprintf(buf, sizeof(buf), "status = %s  \n", sim_state[1] ? "detected" : "not detected");
    } else {
        MMI_ALOGE("invalid sub parameter, sub=%s", MMI_STR(params["sub"].c_str()));
    }

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
    .name = "Simcard",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
