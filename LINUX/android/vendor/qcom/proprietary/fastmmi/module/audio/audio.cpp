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
#define PCBA_REPLY_STR "Test finished, please check the result\n"
#define PCBA_EXTERNAL_LOOPBACK_PASS "External Loopback Test Pass"
#define PCBA_EXTERNAL_LOOPBACK_FAIL "External Loopback Test Fail"
#define PCBA_EXTERNAL_LOOPBACK_POWER "Power"

static int g_pid = -1;

static void get_ftm_test_config(char *config, int size) {
    char soundCardInfo[256] = { 0 };
    char sound_card[256] = { 0 };
    FILE *fp = NULL;
    char config_path[256] = { 0 };
    if((fp = fopen("/proc/asound/cards", "r")) != NULL) {
        while(fgets(soundCardInfo, sizeof(soundCardInfo), fp) != NULL)
            sscanf(soundCardInfo, "%*s%*s%*s%*s%s", sound_card);
        fclose(fp);

        snprintf(config_path, sizeof(config_path), "%s_%s", get_value("ftm_test_config"), sound_card);
    } else {
        MMI_ALOGE("file(/proc/asound/cards) open fail, error=%s", strerror(errno));
    }

    if(check_file_exist(config_path)) {
        strlcpy(config, config_path, size);
    } else
        strlcpy(config, get_value("ftm_test_config"), size);

    MMI_ALOGI("ftm test config file: '%s'", config);
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run mmi start for module: [%s]", module->name);
    const char *args[16];
    int i = 0;
    int ret = FAILED;
    char result[SIZE_2K] = { 0 };
    char ftm_config_file[256] = { 0 };
    exec_cmd_t execmd;

    get_ftm_test_config(ftm_config_file, sizeof(ftm_config_file));
    args[i++] = get_value("mm_audio_ftm");
    args[i++] = "-c";
    args[i++] = ftm_config_file;
    args[i++] = "-tc";
    args[i++] = params["tc"].c_str();
    args[i++] = "-d";
    args[i++] = params["duration"].c_str();
    args[i++] = "-v";
    args[i++] = params["volume"].c_str();

    if(!params["file"].empty()) {
        args[i++] = "-file";
        args[i++] = params["file"].c_str();
    }

    if(!params["fl"].empty()) {
        args[i++] = "-fl";
        args[i++] = params["fl"].c_str();
    }
    if(!params["fh"].empty()) {
        args[i++] = "-fh";
        args[i++] = params["fh"].c_str();
    }
    args[i] = NULL;

    memset(&execmd, 0, sizeof(exec_cmd_t));
    execmd.cmd = get_value("mm_audio_ftm");
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = result;
    execmd.size = sizeof(result);
    execmd.params = (char **) args;
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; j < i && NULL != args[j]; j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(NULL, &execmd);
    if(ret != SUCCESS) {
        MMI_ALOGE("command(%s) exec fail", execmd.cmd);
    }

    sleep(1);

    const char *args_play[12] = { get_value("mm_audio_ftm"), "-c", ftm_config_file,
        "-tc", params["tc_play"].c_str(), "-d", params["duration"].c_str(), "-v", params["volume"].c_str(), "-file",
        params["file"].c_str(), NULL
    };

    if(!strcmp(params["type"].c_str(), "mic")) {
        MMI_ALOGI("start to play the record file");
        memset(&execmd, 0, sizeof(exec_cmd_t));
        execmd.cmd = get_value("mm_audio_ftm");
        execmd.pid = &g_pid;
        execmd.exit_str = NULL;
        execmd.result = result;
        execmd.size = sizeof(result);
        execmd.params = (char **) args_play;
        {
            char temp_agrs[512] = { 0 };
            for(int j = 1; j < sizeof(args_play) / sizeof(char *) && NULL != args_play[j]; j++)
                snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args_play[j]);
            MMI_ALOGI("exec command:'%s', args:%s", execmd.cmd, temp_agrs);
        }
        ret = exe_cmd(NULL, &execmd);
        if(ret != SUCCESS) {
            MMI_ALOGE("command(%s) exec fail", execmd.cmd);
        }
    }

    MMI_ALOGI("run mmi finished for module: [%s]", module->name);
    return ret;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run pcba start for module: %s", module->name);
    char result[SIZE_2K] = { 0 };
    char buf[SIZE_1K] = { 0 };
    char tmp[256] = { 0 };
    const char *args[24];
    int i = 0;
    char ftm_config_file[256] = { 0 };
    int ret = FAILED;
    bool power_pass = false;
    exec_cmd_t execmd;

    get_ftm_test_config(ftm_config_file, sizeof(ftm_config_file));

   /**Before start kill the previous*/
    if(g_pid > 0) {
        kill(g_pid, SIGTERM);
        usleep(100);
        kill_proc(g_pid);
    }
    args[i++] = get_value("mm_audio_ftm");
    args[i++] = "-c";
    args[i++] = ftm_config_file;
    args[i++] = "-tc";
    args[i++] = params["tc"].c_str();

    if(!params["duration"].empty()) {
        args[i++] = "-d";
        args[i++] = params["duration"].c_str();
    }
    if(!params["volume"].empty()) {
        args[i++] = "-v";
        args[i++] = params["volume"].c_str();
    }
    if(!params["txvolume"].empty()) {
        args[i++] = "-tv";
        args[i++] = params["txvolume"].c_str();
    }
    if(!params["rxvolume"].empty()) {
        args[i++] = "-rv";
        args[i++] = params["rxvolume"].c_str();
    }
    if(!params["file"].empty()) {
        args[i++] = "-file";
        args[i++] = params["file"].c_str();
    }
    if(!params["fl"].empty()) {
        args[i++] = "-fl";
        args[i++] = params["fl"].c_str();
    }
    if(!params["fh"].empty()) {
        args[i++] = "-fh";
        args[i++] = params["fh"].c_str();
    }
    if(!params["analyze"].empty()) {
        args[i++] = "-analyze";
        args[i++] = params["analyze"].c_str();
    }
    if(!params["fdiff"].empty()) {
        args[i++] = "-fdiff";
        args[i++] = params["fdiff"].c_str();
    }
    args[i] = NULL;

    memset(&execmd, 0, sizeof(exec_cmd_t));
    execmd.cmd = get_value("mm_audio_ftm");
    execmd.pid = &g_pid;
    execmd.exit_str = NULL;
    execmd.result = result;
    execmd.size = sizeof(result);
    execmd.params = (char **) args;
    {
        char temp_agrs[512] = { 0 };
        for(int j = 1; j < i && NULL != args[j]; j++)
            snprintf(temp_agrs + strlen(temp_agrs), sizeof(temp_agrs) - strlen(temp_agrs), "%s ", args[j]);
        MMI_ALOGI("exe command:'%s', args:%s", execmd.cmd, temp_agrs);
    }
    ret = exe_cmd(NULL, &execmd);
    if(ret != SUCCESS) {
        MMI_ALOGE("command(%s) exec fail", execmd.cmd);
        return FAILED;
    }

    char *p = result;
    char *ptr;
    char *tokptr = NULL;

    while(*p != '\0') {         /*print every line of scan result information */
        ptr = tmp;
        while(*p != '\n' && *p != '\0') {
            *ptr++ = *p++;
        }

        p++;
        *ptr = '\0';

        MMI_ALOGI("command:'%s' exec result:%s", execmd.cmd, tmp);
        ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_POWER);
        if(ptr != NULL) {
            strlcat(buf, tmp, strlen(tmp));
            ptr = strtok_r(tmp, ":d", &tokptr);
            if(ptr != NULL)
                ptr = strtok_r(NULL, ":d", &tokptr);
            if(!params["power"].empty() && (NULL != ptr) && (atof(ptr) > atof(params["power"].c_str())))
                power_pass = true;
        }
        ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_PASS);
        if(ptr != NULL) {
            strlcat(buf, PCBA_EXTERNAL_LOOPBACK_PASS, strlen(PCBA_EXTERNAL_LOOPBACK_PASS));
            module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
            if(!params["power"].empty() && !power_pass) {
                MMI_ALOGE("module:[%s] test fail", module->name);
                return FAILED;
            } else {
                MMI_ALOGI("module:[%s] test pass", module->name);;
                return SUCCESS;
            }
        }
        ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_FAIL);
        if(ptr != NULL) {
            strlcat(buf, PCBA_EXTERNAL_LOOPBACK_FAIL, strlen(PCBA_EXTERNAL_LOOPBACK_FAIL));
            module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
            MMI_ALOGE("module:[%s] test fail", module->name);
            return FAILED;
        }
    }
    module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, PCBA_REPLY_STR, strlen(PCBA_REPLY_STR), PRINT);

    MMI_ALOGE("module:[%s] test fail", module->name);
    MMI_ALOGI("run pcba finished for module: %s", module->name);
    return ERR_UNKNOW;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;

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
    MMI_ALOGI("module deinit start for moudle:[%s]", module->name);

    MMI_ALOGI("module deinit finished for moudle:[%s]", module->name);
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
    .name = "Audio",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
