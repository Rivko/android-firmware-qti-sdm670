/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
#define KEY_WORD_RSSI "rssi"

struct audio_params {
    char *sz_testcase;
    char *sz_duration;
    char *sz_volume;
};

static struct audio_params g_audio_params;
static const char next_tune[] = "next_tune";

static const char *extra_cmd_list[] = {
    next_tune
};

static int g_pid = -1;

static void local_exe_cmd(const char *cmd, char *result, int size) {
    char line[SIZE_1K];
    char ps[SIZE_1K] = { 0 };
    char name[SIZE_1K] = { 0 };
    char value[SIZE_1K] = { 0 };
    char indicator = ':';
    FILE *pp;

    if(cmd != NULL)
        strlcpy(ps, cmd, sizeof(ps));

    MMI_ALOGI("start exec cmd: '%s'\n", ps);
    if((pp = popen(ps, "r")) != NULL) {
        while(fgets(line, sizeof(line), pp) != NULL) {
            char *p = &line[0];

            parse_nv_by_indicator(p, indicator, name, sizeof(name), value, sizeof(value));
            char *pname = trim(name);
            char *pvalue = trim(value);

            MMI_ALOGD("pname=%s, pvalue=%s\n", MMI_STR(pname), MMI_STR(pvalue));
            if(*pname != '\0' && *pvalue != '\0') {
                if(!strncmp(pname, "result", 6)) {
                    strlcpy(result, pvalue, size);
                    break;
                }
            }
        }
        pclose(pp);
        MMI_ALOGI("cmd('%s') exit\n", ps);
        pp = NULL;
    } else {
        MMI_ALOGE("file(%s) popen fail, error=%s\n", ps, strerror(errno));
    }
}

static void get_ftm_test_config(char *config, int size) {
    char soundCardInfo[256] = { 0 };
    char sound_card[256] = { 0 };
    FILE *fp = NULL;
    char config_path[256] = { 0 };
    if((fp = fopen("/proc/asound/cards", "r")) != NULL) {
        while(fgets(soundCardInfo, sizeof(soundCardInfo), fp) != NULL)
            sscanf(soundCardInfo, "%*s%*s%*s%*s%s", sound_card);
        fclose(fp);

        snprintf(config_path, sizeof(config_path), "/etc/ftm_test_config_%s", sound_card);

        MMI_ALOGI("ftm config file: '%s'\n", config_path);
    } else {
        MMI_ALOGE("file(%s) open fail, error=%s\n", "/proc/asound/cards", strerror(errno));
    }

    if(check_file_exist(config_path))
        strlcpy(config, config_path, size);
    else
        strlcpy(config, get_value("ftm_test_config"), size);
}

static void fm_disable(void) {
    char fm_test_cmd[SIZE_512] = { 0 };
    char result[256] = { 0 };

    snprintf(fm_test_cmd, sizeof(fm_test_cmd), "%s %s", get_value("fmfactorytest"), " disable");
    MMI_ALOGI("exec '%s' for fm disable\n", fm_test_cmd);
    local_exe_cmd(fm_test_cmd, result, sizeof(result));

    MMI_ALOGI("fm disable result: %s\n", !result[0] ? "\"\"" : result);
}

static int fm_tune(char *fm_rssi, unordered_map < string, string > &params) {
    char fm_test_cmd[SIZE_512] = { 0 };
    char result[SIZE_2K] = { 0 };
    char tmp[128] = { 0 };
    char *ptr = NULL;
    int ret = 0;

    snprintf(fm_test_cmd, sizeof(fm_test_cmd), "%s %s %s", get_value("fmfactorytest"), " tune ",
             (char *) params["tune"].c_str());
    MMI_ALOGI("exec '%s' for fm tune\n", fm_test_cmd);
    local_exe_cmd(fm_test_cmd, result, sizeof(result));
    MMI_ALOGI("fm tune result: %s\n", !result[0] ? "\"\"" : result);
    if((strstr(result, KEY_WORD_RSSI) != NULL) && (fm_rssi != NULL)) {
        ptr = strstr(result, ":");
        ptr += 2;
        snprintf(tmp, sizeof(tmp), "RSSI:%s;", ptr);
        strlcpy(fm_rssi, tmp, sizeof(tmp));
        MMI_ALOGI("Received Signal Strength Indicator(RSSI): %s\n", ptr);
    }
    ret = atoi(result);
    return ret;
}

static int fm_seeknext(const mmi_module_t * mod) {
    char fm_test_cmd[SIZE_512] = { 0 };
    char result[SIZE_2K] = { 0 };
    char fm_rssi[128] = { 0 };
    char *ptr = NULL;
    int tune = 0;

    snprintf(fm_test_cmd, sizeof(fm_test_cmd), "%s %s", get_value("fmfactorytest"), " seeknext");
    MMI_ALOGI("exec '%s' for fm seeknext\n", fm_test_cmd);
    mod->cb_print(NULL, SUBCMD_MMI, "search_tune", 11, PRINT);
    local_exe_cmd(fm_test_cmd, result, sizeof(result));
    MMI_ALOGI("fm seeknext result: %s\n", !result[0] ? "\"\"" : result);
    if(strstr(result, KEY_WORD_RSSI) != NULL) {
        ptr = strstr(result, ":");
        ptr += 2;
        snprintf(fm_rssi, sizeof(fm_rssi), "RSSI:%s;", ptr);
    }
    tune = atoi(result);
    snprintf(result, sizeof(result), "%s:%d.%dMHz;%s", "playing_tune", tune / 1000, (tune % 1000) / 100, fm_rssi);
    mod->cb_print(NULL, SUBCMD_MMI, result, strlen(result), PRINT_DATA);
    sleep(2);
    return 0;
}

static int fm_enable(bool enable, const mmi_module_t * mod) {
    char fm_test_cmd[SIZE_512] = { 0 };
    char result[256] = { 0 };
    int ret = FAILED;

    snprintf(fm_test_cmd, sizeof(fm_test_cmd), "%s %s", get_value("fmfactorytest"), " enable");
    MMI_ALOGI("exec '%s' for fm enable\n", fm_test_cmd);
    if(enable)
        mod->cb_print(NULL, SUBCMD_MMI, "FM enable", 9, PRINT);

    local_exe_cmd(fm_test_cmd, result, sizeof(result));
    ret = atoi(result);

    MMI_ALOGI("fm enalbe result: %s\n", !result[0] ? "\"\"" : result);
    return ret;
}

static void set_audio_path(void) {
    char ftm_config_file[256] = { 0 };
    int pid = fork();

    if(pid == 0) {
        get_ftm_test_config(ftm_config_file, sizeof(ftm_config_file));
        MMI_ALOGI("ftm config file: '%s'\n", ftm_config_file);
        const char *args[10] = { get_value("mm_audio_ftm"), "-tc", (const char *) g_audio_params.sz_testcase,
            "-c", ftm_config_file, "-d", (const char *) g_audio_params.sz_duration,
            "-v", (const char *) g_audio_params.sz_volume, NULL
        };

        {
            char log_args[256] = { 0 };
            for(int i = 1; (i < sizeof(args) / sizeof(char *)) && (NULL != args[i]); i++) {
                snprintf(log_args + strlen(log_args), sizeof(log_args) - strlen(log_args), "%s ", args[i]);
            }
            MMI_ALOGI("start execv ftm cmd: '%s', args: %s\n", get_value("mm_audio_ftm"), log_args);
        }
        int ret = execv((char *) get_value("mm_audio_ftm"), (char **) args);

        if(ret == -1) {
            MMI_ALOGE("'%s' execv fail, error=%s\n", get_value("mm_audio_ftm"), strerror(errno));
            _exit(100);
        }
    } else if(pid > 0) {
        g_pid = pid;
        MMI_ALOGI("wait for '%s'(pid=%ld) exit\n", get_value("mm_audio_ftm"), pid);
        waitpid(g_pid, NULL, 0);
        g_pid = -1;
        MMI_ALOGI("'%s'(pid=%ld) exit\n", get_value("mm_audio_ftm"), pid);
    } else if(pid < 0) {
        MMI_ALOGE("fork fail, strerror=%s\n", strerror(errno));
    }
}

static void *run_test(void *mod) {
    mmi_module_t *module = (mmi_module_t *) mod;

    if(module->cb_print == NULL) {
        MMI_ALOGE("NULL for cb function");
        return NULL;
    }

    signal(SIGUSR1, signal_handler);
    set_audio_path();

    return NULL;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run mmi start for module:[%s]", module->name);
    int ret = FAILED;
    char result[SIZE_2K] = { 0 };
    char fm_rssi[128] = { 0 };

    ret = fm_enable(true, module);
    if(ret == SUCCESS) {
        ret = fm_tune(fm_rssi, params);
        if(ret == SUCCESS) {
            snprintf(result, sizeof(result), "%s:%sMHz;%s", "playing_tune",
                (char *) params["tune"].c_str(), fm_rssi);
            module->cb_print(NULL, SUBCMD_MMI, result, strlen(result), PRINT_DATA);
        } else
            MMI_ALOGE("fm tune fail");
    } else
        MMI_ALOGE("fm enable fail");

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0)
        MMI_ALOGE("Can't create pthread, error=%s\n", strerror(errno));
    else {
        MMI_ALOGD("create thread(thread id=%d) for module:[%s]", module->run_pid, module->name);
        pthread_join(module->run_pid, NULL);
        MMI_ALOGI("thread(thread id=%d) exit for module:[%s]", module->run_pid, module->name);
    }

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run pcba start for module:[%s]", module->name);
    int ret = FAILED;
    char fm_rssi[128] = { 0 };

    sleep(4);
    ret = fm_enable(true, module);
    if(ret == SUCCESS)
        ret = fm_tune(fm_rssi, params);

    fm_disable();

    if(ret == SUCCESS) {
        module->cb_print(NULL, SUBCMD_PCBA, "FM test pass\n", 13, PRINT);
        return SUCCESS;
    } else {
        module->cb_print(NULL, SUBCMD_PCBA, "FM test fail\n", 13, PRINT);
        return FAILED;
    }
    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    MMI_ALOGI("received parameter tc=%s, duration=%s, volume=%s",
              params["tc"].c_str(), params["duration"].c_str(), params["volume"].c_str());
    memset(&g_audio_params, 0, sizeof(struct audio_params));
    g_audio_params.sz_testcase = (char *) params["tc"].c_str();
    g_audio_params.sz_duration = (char *) params["duration"].c_str();
    g_audio_params.sz_volume = (char *) params["volume"].c_str();

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
    MMI_ALOGI("module stop start for module:[%s]", module->name);

    fm_disable();

    if(g_pid > 0) {
        MMI_ALOGI("'%s'(pid=%d) be killed", get_value("mm_audio_ftm"), g_pid);
        kill(g_pid, SIGTERM);
        g_pid = -1;
    }

    MMI_ALOGI("thread(thread id=%d) be killed", module->run_pid);
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
    else if(!strcmp(cmd, next_tune))
        ret = fm_seeknext(module);
    else {
        MMI_ALOGE("received invalid command: %s", MMI_STR(cmd));
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
    .name = "FM",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
