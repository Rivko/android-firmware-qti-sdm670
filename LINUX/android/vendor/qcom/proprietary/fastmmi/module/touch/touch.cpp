/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <linux/input.h>
#include "mmi_module.h"

#define TOUCHSCREEN_INFO_PATH "/sys/kernel/ts_info/ts_info"
#define TOUCH_TRACK_FILENAME  "touch_track.txt"

static FILE *touch_track_fp = NULL;
static char touch_track_file[PATH_MAX];

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int touch_input_callback(int fd, uint32_t revents, void *data) {
    struct input_event ev;
    static int x_last = -1;
    static int y_last = -1;
    static point_t last_point(-1, -1);
    point_t cur_point(-1, -1);
    int retval = 0;
    char str[64] = { 0 };

    retval = ev_get_input(fd, revents, &ev);
    if(retval < 0) {
        MMI_ALOGE("get input event fail");
        return -1;
    }

    __u16 type = ev.type;
    __u16 code = ev.code;
    __u32 value = ev.value;

    int x = x_last;
    int y = y_last;

    if(type == EV_ABS) {
        if(code == ABS_MT_POSITION_X) {
            x_last = x = value;
        } else if(code == ABS_MT_POSITION_Y) {
            y_last = y = value;
        }
    } else if(type == EV_SYN) {

        point_t cur_point(x, y);

        if(!(last_point == cur_point)) {
            last_point = cur_point;

            snprintf(str, sizeof(str), "(%d,%d)\n", cur_point.x, cur_point.y);
            if(touch_track_fp != NULL) {
                fwrite(str, sizeof(char), strlen(str), touch_track_fp);
                fflush(touch_track_fp);
            }
        }
    }

    return 0;
}

/**
* Run touch pcba test by reading event id method
* @return, 0 -success; -1
*/
static int32_t pcba_event_id_test(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("start touch pcba test, use method: %s", TOUCH_METHOD_READ_EVENT_ID);
    DIR *dir;
    struct dirent *de;
    int fd;
    int i = 0, j = 0;
    bool ret = FAILED;
    int max_event_index = 0;
    int index = 0;
    char buffer[NAME_MAX];
    char format_buf[NAME_MAX];
    char filepath[PATH_MAX] = { 0 };
    unsigned long keyBitmask[BITS_TO_LONGS(KEY_MAX)];
    unsigned long absBitmask[BITS_TO_LONGS(ABS_MAX)];
    struct input_id id;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }

    dir = opendir("/dev/input");
    if(dir == 0) {
        MMI_ALOGE("directory(%s) open fail, error=%s", "/dev/input", strerror(errno));
        return -1;
    }

    while((de = readdir(dir))) {
        if(strncmp(de->d_name, "event", 5))
            continue;
        get_device_index(de->d_name, "event", &index);
        if(index > max_event_index)
            max_event_index = index;
    }

    for(i = 0; i < max_event_index + 1; i++) {
        unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];

        snprintf(filepath, sizeof(filepath), "/dev/input/event%d", i);
        fd = open(filepath, O_RDONLY);
        if(fd < 0) {
            MMI_ALOGE("file(%s) open fail, error=%s", filepath, strerror(errno));
            continue;
        }
        /* read the evbits of the input device */
        if(ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0) {
            MMI_ALOGE("could not get ev_bits from '%s', error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }

        /* TODO: add ability to specify event masks. For now, just assume
         * that only EV_KEY and EV_REL event types are ever needed. */
        if(!test_bit(EV_KEY, ev_bits) && !test_bit(EV_REL, ev_bits)) {
            MMI_ALOGE("could not get EV_KEY and EV_REL from '%s', error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }

        /* read the evbits of the input device */
        if(ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBitmask)), keyBitmask) < 0) {
            MMI_ALOGE("could not get keyBitmask from '%s', error=%s", filepath, strerror(errno));
            close(fd);
            continue;
        }

        if(ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask) < 0) {
            MMI_ALOGE("could not get absBitmask from '%s', error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }
        /*See if this is a touch pad. Is this a new modern multi-touch driver */
        if(test_bit(ABS_MT_POSITION_X, absBitmask)
           && test_bit(ABS_MT_POSITION_Y, absBitmask)) {

            ret = SUCCESS;
            if(ioctl(fd, EVIOCGNAME(sizeof(format_buf) - 1), &format_buf) < 1) {
                MMI_ALOGE("could not get device name from '%s', error=%s\n", filepath, strerror(errno));
                close(fd);
                continue;
            }
            snprintf(buffer, sizeof(buffer), "name = %s\n", format_buf);

            if(ioctl(fd, EVIOCGID, &id)) {
                MMI_ALOGE("could not get device id from '%s', error=%s\n", filepath, strerror(errno));
                close(fd);
                continue;
            }

            snprintf(format_buf, sizeof(format_buf), "bus = %04x\n"
                     "vendor = %04x\n"
                     "product = %04x\n" "version = %04x\n", id.bustype, id.vendor, id.product, id.version);
            strlcat(buffer, format_buf, sizeof(buffer));
            module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buffer, strlen(buffer), PRINT_DATA);

            MMI_ALOGI("touch device info: %s\n", buffer);
        } else {
            MMI_ALOGW("device(%s) is not touch device\n", filepath);
        }
    }
    closedir(dir);

    if(SUCCESS != ret) {
        MMI_ALOGE("pcba test(METHOD=%s) %s for module:[%s]\n",
                  TOUCH_METHOD_READ_EVENT_ID, MMI_TEST_RESULT(ret), module->name);
    } else {
        MMI_ALOGI("pcba test(METHOD=%s) %s for module:[%s]\n",
                  TOUCH_METHOD_READ_EVENT_ID, MMI_TEST_RESULT(ret), module->name);
    }
    return ret;
}

void *event_waiting_thread(void *) {
    signal(SIGUSR1, signal_handler);

    while(1) {
        if(!ev_wait(-1))
            ev_dispatch();
    }
    return NULL;
}

/**
* Run touch pcba test by reading touchscreen id method
* @return, 0 -success; -1
*/
static int32_t pcba_touchscreen_touch_track(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("start touch pcba test, use method: %s", TOUCH_METHOD_TOUCH_TRACK);

    int ret = FAILED;
    struct stat st;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }

   /**Close file*/
    if(touch_track_fp != NULL) {
        fclose(touch_track_fp);
        touch_track_fp = NULL;
    }

    touch_track_fp = fopen(touch_track_file, "w+");
    if(touch_track_fp == NULL) {
        MMI_ALOGE("file(%s) open failed, error=%s\n", touch_track_file, strerror(errno));
        return FAILED;
    }

    /**Kill previous created thread before start again*/
    kill_thread(module->run_pid);

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, event_waiting_thread, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create thread, error=%s\n", strerror(errno));
        return FAILED;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);

    return SUCCESS;
}

/**
* Run touch pcba test by reading touchscreen id method
* @return, 0 -success; -1
*/
static int32_t pcba_touchscreen_id_test(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("start touch pcba test, use method: %s", TOUCH_METHOD_READ_TOUCH_ID);
    char buffer[SIZE_512] = { 0 };

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }

    FILE *fp = fopen(TOUCHSCREEN_INFO_PATH, "r");

    if(fp == NULL) {
        MMI_ALOGE("file(%s) open failed, error=%s\n", TOUCHSCREEN_INFO_PATH, strerror(errno));
        return FAILED;
    }

    fread(buffer, 1, SIZE_512, fp);
    module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buffer, strlen(buffer), PRINT_DATA);
    fclose(fp);

    MMI_ALOGI("pcba test(METHOD=%s) %s for module:[%s], touch device info: %s\n",
              TOUCH_METHOD_READ_TOUCH_ID, MMI_TEST_RESULT(0), module->name, buffer);
    return SUCCESS;
}

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
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
    int32_t ret = FAILED;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    if(!strcmp(params[METHOD].c_str(), TOUCH_METHOD_READ_EVENT_ID))
        ret = pcba_event_id_test(module, params);
    else if(!strcmp(params[METHOD].c_str(), TOUCH_METHOD_READ_TOUCH_ID))
        ret = pcba_touchscreen_id_test(module, params);
    else if(!strcmp(params[METHOD].c_str(), TOUCH_METHOD_TOUCH_TRACK))
        ret = pcba_touchscreen_touch_track(module, params);
    else
        ret = pcba_event_id_test(module, params);

    if(strcmp(params[METHOD].c_str(), TOUCH_METHOD_TOUCH_TRACK)) {
        MMI_ALOGI("pcba test(METHOD=%s) %s for module:[%s]",
                  params[METHOD].c_str(), MMI_TEST_RESULT(ret), module->name);
    }

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module init start for module:[%s]", module->name);

    ev_init(touch_input_callback, NULL);
    snprintf(touch_track_file, sizeof(touch_track_file), "%s%s", get_value(KEY_FTM_AP_DIR), TOUCH_TRACK_FILENAME);
    MMI_ALOGI("touch track file: '%s' for module:[%s]", touch_track_file, module->name);

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

    kill_thread(module->run_pid);

    if(touch_track_fp != NULL)
        fclose(touch_track_fp);

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
    MMI_ALOGI("module run start for module:[%s], subcmd=%s", module->name, MMI_STR(cmd));

    if(!strcmp(cmd, SUBCMD_PCBA))
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
    .name = "Touch",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
