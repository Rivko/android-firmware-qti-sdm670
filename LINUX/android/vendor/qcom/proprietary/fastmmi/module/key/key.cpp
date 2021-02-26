/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <linux/input.h>
#include "mmi_module.h"

static key_map_t key_map[] = {
    {KEYPAD_STR_HOME, NULL, KEY_HOME, false, false},
    {KEYPAD_STR_MENU, NULL, KEY_MENU, false, false},
    {KEYPAD_STR_BACK, NULL, KEY_BACK, false, false},
    {KEYPAD_STR_VOLUMEDOWN, NULL, KEY_VOLUMEDOWN, false, false},
    {KEYPAD_STR_VOLUMEUP, NULL, KEY_VOLUMEUP, false, false},
    {KEYPAD_STR_POWER, NULL, KEY_POWER, false, false},
    {KEYPAD_STR_SNAPSHOT, NULL, KEY_CAMERA_SNAPSHOT, false, false},
    {KEYPAD_STR_0, NULL, KEY_0, false, false},
    {KEYPAD_STR_1, NULL, KEY_1, false, false},
    {KEYPAD_STR_2, NULL, KEY_2, false, false},
    {KEYPAD_STR_3, NULL, KEY_3, false, false},
    {KEYPAD_STR_4, NULL, KEY_4, false, false},
    {KEYPAD_STR_5, NULL, KEY_5, false, false},
    {KEYPAD_STR_6, NULL, KEY_6, false, false},
    {KEYPAD_STR_7, NULL, KEY_7, false, false},
    {KEYPAD_STR_8, NULL, KEY_8, false, false},
    {KEYPAD_STR_9, NULL, KEY_9, false, false},
    {KEYPAD_STR_NUMERIC_POUND, NULL, KEY_NUMERIC_POUND, false, false},
    {KEYPAD_STR_STAR, NULL, KEY_KPASTERISK, false, false},
    {KEYPAD_STR_STAR, NULL, KEY_NUMERIC_STAR, false, false},
    {KEYPAD_STR_LEFTSHIFT, NULL, KEY_LEFTSHIFT, false, false},
    {KEYPAD_STR_RIGHTSHIFT, NULL, KEY_RIGHTSHIFT, false, false},
    {KEYPAD_STR_LEFT, NULL, KEY_LEFT, false, false},
    {KEYPAD_STR_RIGHT, NULL, KEY_RIGHT, false, false},
    {KEYPAD_STR_OK, NULL, KEY_OK, false, false},

};

static mmi_module_t *g_module = NULL;
static char module_name[32] = { 0 };

static bool pcba_success = false;
static sem_t semaphore_key_success;

static int32_t module_stop(const mmi_module_t * module);

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int key_input_callback(int fd, uint32_t revents, void *data) {
    struct input_event ev;
    char tmp[SIZE_512] = { 0 };
    char buf[SIZE_512] = { 0 };
    int retval = 0;
    unsigned int i = 0;

    retval = ev_get_input(fd, revents, &ev);
    if(retval < 0) {
        MMI_ALOGE("get input event fail");
        return -1;
    }

    if(ev.type == EV_KEY) {
        for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
            if((ev.code == key_map[i].key_code) && (ev.value == 0)) {
                key_map[i].tested = true;
            }
        }
    }
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if((key_map[i].exist) && (key_map[i].tested))
            pcba_success = true;
        else if((key_map[i].exist) && !(key_map[i].tested)) {
            pcba_success = false;
            break;
        }
    }

    if(pcba_success) {
        for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
            if(key_map[i].exist && key_map[i].tested) {
                MMI_ALOGI("key(%s) test pass", key_map[i].key_name);
                snprintf(buf, sizeof(buf), "%s = detected\n", key_map[i].key_name);
                strlcat(tmp, buf, sizeof(tmp));
            }
        }
        MMI_ALOGI("PCBA key test pass");
        snprintf(buf, sizeof(buf), "Key PCBA test Pass\n%s", tmp);
        g_module->cb_print(module_name, SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
        sem_post(&semaphore_key_success);
    } else if((ev.type == EV_KEY) && (ev.value == 0)) {
        for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
            if(key_map[i].exist && key_map[i].tested) {
                MMI_ALOGI("key(%s) test pass", key_map[i].key_name);
                snprintf(buf, sizeof(buf), "%s = detected\n", key_map[i].key_name);
                strlcat(tmp, buf, sizeof(tmp));
            } else if(key_map[i].exist && !key_map[i].tested) {
                MMI_ALOGI("key(%s) not detected", key_map[i].key_name);
                snprintf(buf, sizeof(buf), "%s = not detected\n", key_map[i].key_name);
                strlcat(tmp, buf, sizeof(tmp));
            }
        }
        MMI_ALOGI("PCBA key test fail");
        snprintf(buf, sizeof(buf), "Key PCBA test Fail\n%s", tmp);
        g_module->cb_print(module_name, SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    }

    return 0;
}

static void *run_test(void *mod) {
    signal(SIGUSR1, signal_handler);
    if(mod == NULL) {
        MMI_ALOGE("parameter error");
        return NULL;
    }

    while(!pcba_success) {
        if(!ev_wait(-1))
            ev_dispatch();
    }

    return NULL;
}

static int init(unordered_map < string, string > &params) {
    unsigned int i = 0;
    char tmp[SIZE_512] = { 0 };
    char buf[SIZE_512] = { 0 };

    pcba_success = false;

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        key_map[i].tested = false;
        if(strstr(params["keys"].c_str(), key_map[i].key_name) != NULL) {
            key_map[i].exist = true;
            MMI_ALOGI("key(%s) need to test\n", key_map[i].key_name);
        } else {
            MMI_ALOGI("key(%s) not need to test\n", key_map[i].key_name);
        }
    }

    if(sem_init(&semaphore_key_success, 0, 0) != 0) {
        MMI_ALOGE("semaphore(semaphore_key_success) init failed, error=%s\n", strerror(errno));
        return FAILED;
    }

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist) {
            snprintf(buf, sizeof(buf), "%s = not detected\n", key_map[i].key_name);
            strlcat(tmp, buf, sizeof(tmp));
        }
    }
    snprintf(buf, sizeof(buf), "Key PCBA test Fail\n%s", tmp);
    g_module->cb_print(module_name, SUBCMD_PCBA, buf, strlen(buf), DATA);

    return SUCCESS;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run mmi start for module:[%s]", module->name);

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return ERR_UNKNOW;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;
    char tmp[SIZE_512] = { 0 };
    char buf[SIZE_512] = { 0 };
    unsigned int i = 0;
    struct timespec time_sec;
    int sem_status;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    ret = init(params);
    if(ret != SUCCESS) {
        MMI_ALOGE("init failed on key test");
        return FAILED;
    }

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create pthread, error=%s\n", strerror(errno));
        return FAILED;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s]\n", module->run_pid, module->name);

    if(clock_gettime(CLOCK_REALTIME, &time_sec) == -1) {
        MMI_ALOGE("get system time fail, error=%s", strerror(errno));
    }
    time_sec.tv_sec += atoi(params["timeout"].c_str());;

    sem_status = sem_timedwait(&semaphore_key_success, &time_sec);

    kill_thread(module->run_pid);

    MMI_ALOGI("thread(thread id=%lu) be killed for module:[%s]\n", module->run_pid, module->name);
    if(sem_status == -1) {
        MMI_ALOGE("%s seconds timeout, key test fail", params["timeout"].c_str());
        return FAILED;
    } else {
        MMI_ALOGI("run pcba finished for module:[%s]", module->name);
        return SUCCESS;
    }

    return ERR_UNKNOW;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    g_module = (mmi_module_t *) module;
    strlcpy(module_name, params[KEY_MODULE_NAME].c_str(), sizeof(module_name));
    ev_init(key_input_callback, NULL);

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

    kill_thread(module->run_pid);
    MMI_ALOGI("thread(thread id=%lu) be killed for module:[%s]\n", module->run_pid, module->name);
    MMI_ALOGI("module stop finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
* Attention: the UI mode running in MMI application, no need implementation in module.
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    int ret = -1;

    if(!module || !cmd) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module run start for module:[%s]", module->name);

    if(!strncmp(cmd, SUBCMD_PCBA, strlen(cmd)))
        ret = module_run_pcba(module, params);
    else {
        MMI_ALOGE("Received invalid command: %s", MMI_STR(cmd));
        ret = FAILED;
    }

    MMI_ALOGI("module run finished for module:[%s]", module->name);
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
    .name = "Key",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
