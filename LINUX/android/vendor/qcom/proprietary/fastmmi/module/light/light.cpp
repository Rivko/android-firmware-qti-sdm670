/*
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/

 /*
  * Copyright (C) 2009 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#ifdef ANDROID
#include <hardware/lights.h>
#include <hardware/hardware.h>
#endif
#include "mmi_module.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
static const char str_led_on[] = "Do you see LED light on?";
static const char str_led_off[] = "Do you see LED light off?";
static const char str_btn_on[] = "Do you see button light on?";
static const char str_btn_off[] = "Do you see button light off?";

#ifdef ANDROID
static const char str_lcd[] = "Do you see LCD backlight brightness change?";
#endif

/**
 * The parameters that can be set for a given light.
 *
 * Not all lights must support all parameters.  If you
 * can do something backward-compatible, you should.
 */

enum {
    LIGHT_INDEX_BACKLIGHT = 0,
    LIGHT_INDEX_KEYBOARD = 1,
    LIGHT_INDEX_BUTTONS = 2,
    LIGHT_INDEX_BATTERY = 3,
    LIGHT_INDEX_NOTIFICATIONS = 4,
    LIGHT_INDEX_ATTENTION = 5,
    LIGHT_INDEX_BLUETOOTH = 6,
    LIGHT_INDEX_WIFI = 7,
    LIGHT_COUNT,
};

enum {
    LED_COLOR_RED = 0,
    LED_COLOR_GREEN = 1,
    LED_COLOR_BLUE = 2,
    LED_COLOR_COUNT,
};

#ifdef ANDROID
struct Devices {
    light_device_t *lights[LIGHT_COUNT];
};

static Devices *devices = NULL;
#endif

static int led_color = LED_COLOR_RED;
static int light_device = 0;
static int light_delay = 0;
static unordered_map < string, string > *paras;
static pthread_mutex_t g_mutex;

static int32_t module_stop(const mmi_module_t * module);

#ifdef ANDROID
static light_device_t *get_device(hw_module_t * module, char const *name) {
    int err;
    hw_device_t *device;

    MMI_ALOGI("open device: [%s]", name);
    err = module->methods->open(module, name, &device);
    if(err == 0) {
        return (light_device_t *) device;
    } else {
        MMI_ALOGE("device(%s) open fail, error=%s", name, strerror(errno));
        return NULL;
    }
}

static int hal_init(void) {
    int ret = FAILED;
    hw_module_t *hw_module = NULL;

    devices = (struct Devices *) malloc(sizeof(struct Devices));
    if(devices == NULL) {
        MMI_ALOGE("Light FFBM: fail to malloc for devices, error=%s", strerror(errno));
        return ret;
    }

    ret = hw_get_module(LIGHTS_HARDWARE_MODULE_ID, (hw_module_t const **) &hw_module);

    if(ret == SUCCESS) {
        MMI_ALOGI("Light FFBM: hardware module(%s) loading successed\n", LIGHTS_HARDWARE_MODULE_ID);
        devices->lights[LIGHT_INDEX_BACKLIGHT]
            = get_device(hw_module, LIGHT_ID_BACKLIGHT);
        devices->lights[LIGHT_INDEX_KEYBOARD]
            = get_device(hw_module, LIGHT_ID_KEYBOARD);
        devices->lights[LIGHT_INDEX_BUTTONS]
            = get_device(hw_module, LIGHT_ID_BUTTONS);
        devices->lights[LIGHT_INDEX_BATTERY]
            = get_device(hw_module, LIGHT_ID_BATTERY);
        devices->lights[LIGHT_INDEX_NOTIFICATIONS]
            = get_device(hw_module, LIGHT_ID_NOTIFICATIONS);
        devices->lights[LIGHT_INDEX_ATTENTION]
            = get_device(hw_module, LIGHT_ID_ATTENTION);
        devices->lights[LIGHT_INDEX_BLUETOOTH]
            = get_device(hw_module, LIGHT_ID_BLUETOOTH);
        devices->lights[LIGHT_INDEX_WIFI]
            = get_device(hw_module, LIGHT_ID_WIFI);
    } else {
        MMI_ALOGI("Light FFBM: hardware module(%s) loading fail\n", LIGHTS_HARDWARE_MODULE_ID);
        free(devices);
        devices = NULL;
    }

    return ret;
}

static void enable(const mmi_module_t * module, bool on) {
    light_state_t state;

    MMI_ALOGI("light device %s for module:[%s]\n", (true == on) ? "on" : "off", module->name);

    memset(&state, 0, sizeof(light_state_t));
    /*Tell user we are waiting for valid data */

    if(light_device == LIGHT_INDEX_BUTTONS) {   //Button light test
        state.flashMode = LIGHT_FLASH_NONE;
        state.flashOnMS = 0;
        state.flashOffMS = 0;
        state.brightnessMode = BRIGHTNESS_MODE_USER;
        state.color = 0xFF020202;

        if(!on)
            state.color = 0xFF000000;
        else
            state.color = 0xFF020202;
    } else if(light_device == LIGHT_INDEX_NOTIFICATIONS) {  // LED light test
        state.flashMode = LIGHT_FLASH_NONE;
        state.flashOnMS = 0;
        state.flashOffMS = 0;
        state.brightnessMode = BRIGHTNESS_MODE_USER;

        if(!on)
            state.color = 0x000000;
        else {
            if(led_color == LED_COLOR_RED)
                state.color = 0xFF0000;
            else if(led_color == LED_COLOR_BLUE)
                state.color = 0x0000FF;
            else if(led_color == LED_COLOR_GREEN)
                state.color = 0x00FF00;
            else
                MMI_ALOGE("FFBM LIGHT: Unknow LED color");
        }
    } else if(light_device == LIGHT_INDEX_BACKLIGHT) {
        if(!on)
            state.color = 0xFF0000;
        else
            state.color = 0xFFFFFF;
    }

    devices->lights[light_device]->set_light(devices->lights[light_device], &state);

    if(light_device == LIGHT_INDEX_BUTTONS) {
        if(on)
            module->cb_print(NULL, SUBCMD_MMI, str_btn_on, strlen(str_btn_on), PRINT);
        else
            module->cb_print(NULL, SUBCMD_MMI, str_btn_off, strlen(str_btn_off), PRINT);
    } else if(light_device == LIGHT_INDEX_NOTIFICATIONS) {
        if(on)
            module->cb_print(NULL, SUBCMD_MMI, str_led_on, strlen(str_led_on), PRINT);
        else
            module->cb_print(NULL, SUBCMD_MMI, str_led_off, strlen(str_led_off), PRINT);
    } else if(light_device == LIGHT_INDEX_BACKLIGHT)
        module->cb_print(NULL, SUBCMD_MMI, str_lcd, strlen(str_lcd), PRINT);
    else
        MMI_ALOGE("FFBM LIGHT: Unknow light device");
}
#else
static void enable(const mmi_module_t * module, bool on) {
    MMI_ALOGI("light device %s for module:[%s]\n", (true == on) ? "on" : "off", module->name);
    if(!on) {
        if(led_color == LED_COLOR_RED) {
            system("echo 0 > /sys/kernel/j19_red/leds/brightness");
            system("echo 0 > /sys/kernel/j20_red/leds/brightness");
            system("echo 0 > /sys/class/leds/red/brightness");
        } else if(led_color == LED_COLOR_GREEN) {
            system("echo 0 > /sys/kernel/j19_green/leds/brightness");
            system("echo 0 > /sys/kernel/j20_green/leds/brightness");
            system("echo 0 > /sys/class/leds/green/brightness");
        } else if(led_color == LED_COLOR_BLUE) {
            system("echo 0 > /sys/class/leds/blue/brightness");
        } else
            MMI_ALOGE("FFBM LIGHT: Unknow LED color");
    } else {
        if(led_color == LED_COLOR_RED) {
            system("echo 1 > /sys/kernel/j19_red/leds/brightness");
            system("echo 1 > /sys/kernel/j20_red/leds/brightness");
            system("echo 255 > /sys/class/leds/red/brightness");
        } else if(led_color == LED_COLOR_GREEN) {
            system("echo 1 > /sys/kernel/j19_green/leds/brightness");
            system("echo 1 > /sys/kernel/j20_green/leds/brightness");
            system("echo 255 > /sys/class/leds/green/brightness");
        } else if(led_color == LED_COLOR_BLUE) {
            system("echo 255 > /sys/class/leds/blue/brightness");
        } else
            MMI_ALOGE("FFBM LIGHT: Unknow LED color");
    }
}
#endif

static int init(unordered_map < string, string > &params) {
    light_delay = atoi(params["delay"].c_str());

    MMI_ALOGI("light device type=%s", params["type"].c_str());

    if(!strncmp(params["type"].c_str(), "led", 3)) {
        light_device = LIGHT_INDEX_NOTIFICATIONS;
        if(!strncmp(params["color"].c_str(), "red", 3))
            led_color = LED_COLOR_RED;
        else if(!strncmp(params["color"].c_str(), "gre", 3))
            led_color = LED_COLOR_GREEN;
        else if(!strncmp(params["color"].c_str(), "blu", 3))
            led_color = LED_COLOR_BLUE;
        else
            MMI_ALOGE("FFBM LIGHT: Unknow LED color");

        MMI_ALOGI("led color=%s", params["color"].c_str());
    } else if(!strncmp(params["type"].c_str(), "but", 3))
        light_device = LIGHT_INDEX_BUTTONS;
    else if(!strncmp(params["type"].c_str(), "lcd", 3))
        light_device = LIGHT_INDEX_BACKLIGHT;
    else {
        light_device = LIGHT_COUNT;
        return FAILED;
    }
    return SUCCESS;
}

static void *run_test(void *mod) {
    mmi_module_t *module = (mmi_module_t *) mod;

    signal(SIGUSR1, signal_handler);
    while(1) {
        enable((const mmi_module_t *) mod, true);
        usleep(1000 * light_delay);
        enable((const mmi_module_t *) mod, false);
        usleep(1000 * light_delay);
    }

    return NULL;
}

static void deinit(const mmi_module_t * mod) {
    enable(mod, false);
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;

    MMI_ALOGI("run mmi start for module:[%s]", module->name);
    pthread_mutex_lock(&g_mutex);
    ret = init(params);
    if(ret != SUCCESS) {
        MMI_ALOGE("FFBM Light: fail to initialize");
        return ret;
    }

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create pthread, error=%s\n", strerror(errno));
        ret = FAILED;
    } else {
        MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);
        pthread_join(module->run_pid, NULL);
    }
    pthread_mutex_unlock(&g_mutex);

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return ret;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run pcba start for module:[%s]", module->name);
    int ret = FAILED;
    char buf[64] = { 0 };

    ret = init(params);
    enable(module, true);
    snprintf(buf, sizeof(buf), "%s on\n", params["type"].c_str());
    module->cb_print(NULL, SUBCMD_PCBA, buf, strlen(buf), PRINT);

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ret;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;

    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
        return FAILED;
    }
    MMI_ALOGI("module init start for module=[%s]", module->name);

    pthread_mutex_init(&g_mutex, NULL);
#ifdef ANDROID
    ret = hal_init();
    if(ret != SUCCESS) {
        MMI_ALOGE("FFBM Light: fail to initialize light HAL");
        return ret;
    }
#endif

    MMI_ALOGI("module init finished for module=[%s]", module->name);
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
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

    MMI_ALOGI("thread(thread id=%lu) be killed", module->run_pid);
    deinit(module);

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
        MMI_ALOGE("parameter error");
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
    .name = "Light",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
