/*
* Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/

 /*
  * Copyright (C) 2008 The Android Open Source Project
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
#include <hardware/hardware.h>
#include <utils/Timers.h>
#include "mmi_module.h"
#include <hardware/sensors.h>
#include "sensor_interface.h"

/**
* Defined case run in mmi mode,this mode support UI.
*
*/
#define CALIBRATION_NOT_SUPPORT 3
#define SENSOR_TYPE_HALL_EFFECT 33171016

static const char *subcmd_calibration = "calibration";

static const char *extra_cmd_list[] = {
    subcmd_calibration,
};

static struct sensors_poll_device_1 *device = NULL;
static struct sensor_t const *sensor_list = NULL;
static int dev_count = 0;
static int cur_sensor_type = 0;
static char cur_module_name[64];
static int calibration_result = FAILED;
static mutex_locker g_mutex;
static bool initialized = false;
static struct sensor_cal_module_t *sensor_cal_module = NULL;

static char const *get_sensor_name(int type) {
    switch (type) {
    case SENSOR_TYPE_ACCELEROMETER:
        return "Acc";
    case SENSOR_TYPE_MAGNETIC_FIELD:
        return "Mag";
    case SENSOR_TYPE_ORIENTATION:
        return "Ori";
    case SENSOR_TYPE_GYROSCOPE:
        return "Gyr";
    case SENSOR_TYPE_LIGHT:
        return "Lux";
    case SENSOR_TYPE_PRESSURE:
        return "Bar";
    case SENSOR_TYPE_TEMPERATURE:
        return "Tmp";
    case SENSOR_TYPE_PROXIMITY:
        return "Prx";
    case SENSOR_TYPE_GRAVITY:
        return "Grv";
    case SENSOR_TYPE_LINEAR_ACCELERATION:
        return "Lac";
    case SENSOR_TYPE_ROTATION_VECTOR:
        return "Rot";
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
        return "Hum";
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        return "Tam";
    case SENSOR_TYPE_HALL_EFFECT:
        return "Hal";
    }
    return "ukn";
}

static int get_sensor_type(const char *name) {
    int sensor_type = 0;

    if(!strncmp(name, "acc", 3))
        sensor_type = SENSOR_TYPE_ACCELEROMETER;
    else if(!strncmp(name, "mag", 3))
        sensor_type = SENSOR_TYPE_MAGNETIC_FIELD;
    else if(!strncmp(name, "ori", 3))
        sensor_type = SENSOR_TYPE_ORIENTATION;
    else if(!strncmp(name, "gyr", 3))
        sensor_type = SENSOR_TYPE_GYROSCOPE;
    else if(!strncmp(name, "lig", 3))
        sensor_type = SENSOR_TYPE_LIGHT;
    else if(!strncmp(name, "bar", 3))
        sensor_type = SENSOR_TYPE_PRESSURE;
    else if(!strncmp(name, "tmp", 3))
        sensor_type = SENSOR_TYPE_TEMPERATURE;
    else if(!strncmp(name, "pro", 3))
        sensor_type = SENSOR_TYPE_PROXIMITY;
    else if(!strncmp(name, "grv", 3))
        sensor_type = SENSOR_TYPE_GRAVITY;
    else if(!strncmp(name, "lac", 3))
        sensor_type = SENSOR_TYPE_LINEAR_ACCELERATION;
    else if(!strncmp(name, "rot", 3))
        sensor_type = SENSOR_TYPE_ROTATION_VECTOR;
    else if(!strncmp(name, "hum", 3))
        sensor_type = SENSOR_TYPE_RELATIVE_HUMIDITY;
    else if(!strncmp(name, "tam", 3))
        sensor_type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    else if(!strncmp(name, "hal", 3))
        sensor_type = SENSOR_TYPE_HALL_EFFECT;
    else
        MMI_ALOGE("invalid device name: %s", name);

    return sensor_type;
}

static int sensor_enable(int sensor_type, int delay, bool enable) {
    int err = FAILED;
    int found = false;

    MMI_ALOGI("start sensor(%s) %s", get_sensor_name(sensor_type), enable ? "enable" : "disable");
    if(sensor_list == NULL || sensor_type <= 0) {
        MMI_ALOGE("Empty sensor list or invalid sensor type: %d", sensor_type);
        return FAILED;
    }

    for(int i = 0; i < dev_count; i++) {
        if(sensor_list[i].type == sensor_type) {
            found = true;

            if(enable)
                device->setDelay((sensors_poll_device_t *) device, sensor_list[i].handle, ms2ns(delay));

            err = device->activate((sensors_poll_device_t *) device, sensor_list[i].handle, enable);
            if(err != SUCCESS) {
                MMI_ALOGE("sensor(%s) %s failed, error=%s\n", sensor_list[i].name, enable ? "enable" : "disable",
                          strerror(-err));
            } else {
                MMI_ALOGI("sensor(%s) %s success\n", sensor_list[i].name, enable ? "enable" : "disable");
            }
            break;
        }
    }

    if(!found) {
        MMI_ALOGE("sensor(%s) no find in the sensor list\n", get_sensor_name(sensor_type));
    }

    return err;
}

static int do_calibration(int sensor_type, unordered_map < string, string > &params) {
    int i = 0;
    int ret = FAILED;
    bool found = false;
    struct sensor_cal_cmd_t cmd;

    MMI_ALOGI("start sensor(%s) calibration", get_sensor_name(sensor_type));
    for(i = 0; i < dev_count; i++) {
        if(sensor_list[i].type == sensor_type) {
            switch (sensor_list[i].type) {
            case SENSOR_TYPE_ACCELEROMETER:
                found = true;
                break;
            case SENSOR_TYPE_LIGHT:
                found = true;
                break;
            case SENSOR_TYPE_PROXIMITY:
                found = true;
                break;
            default:
                break;
            }
            break;
        }
    }
    cmd.data_type = atoi(params["data_type"].c_str());
    cmd.test_type = atoi(params["test_type"].c_str());
    MMI_ALOGD("sensor(%s) data type: %d, test type: %d", get_sensor_name(sensor_type), cmd.data_type, cmd.test_type);

    if(found && !sensor_enable(sensor_type, 0, false)) {
        usleep(500 * 1000);
        if((sensor_cal_module != NULL) && (sensor_cal_module->methods != NULL))
            ret = sensor_cal_module->methods->calibrate((struct sensor_t *) &sensor_list[i], &cmd);
        MMI_ALOGI("sensor(%s) calibrate %s", get_sensor_name(sensor_type), MMI_RESULT(ret));
    } else {
        if(!found) {
            MMI_ALOGI("sensor(%s) no support calibrate", get_sensor_name(sensor_type));
        }
    }

    return ret;
}

static int test_event(sensors_event_t * event, unordered_map < string, string > &params) {
    int err = FAILED;
    double x_min, x_max, y_min, y_max, z_min, z_max, v_min, v_max;

    if(event != NULL) {
        switch (event->type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_ORIENTATION:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_GRAVITY:
        case SENSOR_TYPE_LINEAR_ACCELERATION:
        case SENSOR_TYPE_ROTATION_VECTOR:

            x_min = atof(params["x_min_limit"].c_str());
            x_max = atof(params["x_max_limit"].c_str());
            y_min = atof(params["y_min_limit"].c_str());
            y_max = atof(params["y_max_limit"].c_str());
            z_min = atof(params["z_min_limit"].c_str());
            z_max = atof(params["z_max_limit"].c_str());

            MMI_ALOGI("limitation for sensor(%s): x(min=%5.5f,max=%5.5f), y(min=%5.5f,max=%5.5f), \
						z(min=%5.5f,max=%5.5f), data(x=%5.5f,y=%5.5f,z=%5.5f)", get_sensor_name(event->type), x_min, x_max, y_min, y_max, z_min, z_max, event->data[0], event->data[1], event->data[2]);
            if(inside_float(event->data[0], x_min, x_max) && inside_float(event->data[1], y_min, y_max)
               && inside_float(event->data[2], z_min, z_max))
                err = SUCCESS;

            MMI_ALOGI("sensor(%s) data(x=%5.5f,y=%5.5f,z=%5.5f) test %s",
                      get_sensor_name(event->type), event->data[0], event->data[1], event->data[2], MMI_RESULT(err));

            break;

        case SENSOR_TYPE_LIGHT:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_TEMPERATURE:
        case SENSOR_TYPE_PROXIMITY:
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
            v_min = atof(params["min_limit"].c_str());
            v_max = atof(params["max_limit"].c_str());

            MMI_ALOGI("limitation for sensor(%s): value(min=%5.5f,max=%5.5f), data(value=%5.5f)",
                      get_sensor_name(event->type), v_min, v_max, event->data[0]);
            if(inside_float(event->data[0], v_min, v_max))
                err = SUCCESS;

            MMI_ALOGI("sensor(%s) data(value=%5.5f) test %s",
                      get_sensor_name(event->type), event->data[0], MMI_RESULT(err));

            break;
        default:
            MMI_ALOGE("FFBM SENSOR: received unknown sensor type: %d", event->type);
            break;
        }
    }

    return err;
}


static void print_event(sensors_event_t * event, const mmi_module_t * mod, bool is_pcba) {
    int err = FAILED;
    char print_buf[256] = {0};

    if(event == NULL || mod == NULL) {
        MMI_ALOGI("parameter error");
        return;
    }

    switch (event->type) {
    case SENSOR_TYPE_ACCELEROMETER:
    case SENSOR_TYPE_MAGNETIC_FIELD:
    case SENSOR_TYPE_ORIENTATION:
    case SENSOR_TYPE_GYROSCOPE:
    case SENSOR_TYPE_GRAVITY:
    case SENSOR_TYPE_LINEAR_ACCELERATION:
    case SENSOR_TYPE_ROTATION_VECTOR:

        snprintf(print_buf,
                 sizeof(print_buf),
                 "%s\nx = %5.5f\ny = %5.5f\nz = %5.5f\n", (char *) get_sensor_name(event->type),
                 event->data[0], event->data[1], event->data[2]);
        MMI_ALOGI("FFBM SENSOR: from sensor(%s) received data: x=%5.5f, y=%5.5f, z=%5.5f",
                  get_sensor_name(event->type), event->data[0], event->data[1], event->data[2]);
        break;
    case SENSOR_TYPE_LIGHT:
    case SENSOR_TYPE_PRESSURE:
    case SENSOR_TYPE_TEMPERATURE:
    case SENSOR_TYPE_PROXIMITY:
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
    case SENSOR_TYPE_HALL_EFFECT:
        snprintf(print_buf, sizeof(print_buf), "%s\nvalue = %5.5f\n",
                 (char *) get_sensor_name(event->type), event->data[0]);
        MMI_ALOGI("FFBM SENSOR: from sensor(%s) received data: value=%5.5f",
                  get_sensor_name(event->type), event->data[0]);
        break;
    default:
        MMI_ALOGE("FFBM SENSOR: received unknown sensor type: %d", event->type);
        break;
    }

    if(event->type == SENSOR_TYPE_ACCELEROMETER || event->type == SENSOR_TYPE_PROXIMITY
       || event->type == SENSOR_TYPE_LIGHT || event->type == SENSOR_TYPE_GYROSCOPE) {
        strlcat(print_buf, "calibration result = ", sizeof(print_buf));
        if(calibration_result == SUCCESS)
            strlcat(print_buf, "success \n", sizeof(print_buf));
        else if(calibration_result == CALIBRATION_NOT_SUPPORT)
            strlcat(print_buf, "not support \n", sizeof(print_buf));
        else
            strlcat(print_buf, "failed \n", sizeof(print_buf));
    }

    if(!is_pcba)
        mod->cb_print(cur_module_name, SUBCMD_MMI, print_buf, strlen(print_buf), PRINT_DATA);
    else
        mod->cb_print(cur_module_name, SUBCMD_PCBA, print_buf, strlen(print_buf), PRINT_DATA);

}

static void get_sensor_data(int sensor_type, sensors_event_t * event) {
    int n = 0;

    if(device == NULL || event == NULL) {
        MMI_ALOGE("invalid device addr value or parameter");
        return;
    }

    while(1) {
        n = device->poll((sensors_poll_device_t *) device, event, 1);
        if(n > 0 && event->type == sensor_type)
            break;
    }
}

static void *run_test(void *mod) {
    signal(SIGUSR1, signal_handler);
    sensors_event_t event;
    mmi_module_t *module = (mmi_module_t *) mod;

    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return NULL;
    }

    while(1) {
        get_sensor_data(cur_sensor_type, &event);
        print_event(&event, module, false);
    }

    return NULL;
}

static int32_t module_run_calibration(const mmi_module_t * module, unordered_map < string, string > &params) {
    int ret = FAILED;

    cur_sensor_type = get_sensor_type(params["type"].c_str());
    calibration_result = do_calibration(cur_sensor_type, params);
    usleep(1000 * 1000);

    sensor_enable(cur_sensor_type, atoi(params["delay"].c_str()), true);

    return calibration_result;
}

static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run mmi start for module:[%s]", module->name);
    int ret = FAILED;

    cur_sensor_type = get_sensor_type(params["type"].c_str());
    calibration_result = do_calibration(cur_sensor_type, params);
    usleep(1000 * 1000);
    strlcpy(cur_module_name, params[KEY_MODULE_NAME].c_str(), sizeof(cur_module_name));

    ret = sensor_enable(cur_sensor_type, atoi(params["delay"].c_str()), true);
    if(ret != SUCCESS) {
        MMI_ALOGE("FFBM SENSOR: sensor(%s) enable fail", get_sensor_name(cur_sensor_type));
        return ret;
    }

    ret = pthread_create((pthread_t *) & module->run_pid, NULL, run_test, (void *) module);
    if(ret < 0) {
        MMI_ALOGE("Can't create pthread, error=%s\n", strerror(errno));
        return FAILED;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s]", module->run_pid, module->name);

    MMI_ALOGI("run mmi finished for module:[%s]", module->name);
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    MMI_ALOGI("run pcba start for module:[%s]", module->name);

    int ret = FAILED, tried = 3;
    sensors_event_t event;

    cur_sensor_type = get_sensor_type(params["type"].c_str());
    strlcpy(cur_module_name, params[KEY_MODULE_NAME].c_str(), sizeof(cur_module_name));

    /**open sensor*/
    ret = sensor_enable(cur_sensor_type, atoi(params["delay"].c_str()), true);
    if(ret != SUCCESS) {
        MMI_ALOGE("FFBM SENSOR: sensor(%s) enable fail", get_sensor_name(cur_sensor_type));
        return FAILED;
    }

    /**Test several data, if any of them correct will returen succesful*/
    while(tried-- > 0) {
        get_sensor_data(cur_sensor_type, &event);
        ret = test_event(&event, params);
        if(ret == SUCCESS)
            break;
        else
            MMI_ALOGE("FFBM SENSOR: sensor(%s) data test fail, start retry", get_sensor_name(cur_sensor_type));
    }

    /**Close sensor*/
    sensor_enable(cur_sensor_type, 0, false);
    print_event(&event, module, true);

    MMI_ALOGI("run pcba finished for module:[%s]", module->name);
    return ret;
}

static void sensor_init() {
    MMI_ALOGI("start sensor initial");
    struct sensors_module_t *hal_mod = NULL;
    void *libsnscal = NULL;
    int i = 0;
    int err = FAILED;

    if(initialized) {
        MMI_ALOGI("Sensor already initialized success, device count=%d", dev_count);
        return;
    }

    err = hw_get_module(SENSORS_HARDWARE_MODULE_ID, (hw_module_t const **) &hal_mod);
    if(err != 0) {
        MMI_ALOGE("FFBM SENSOR: hw_get_module() call failed, error=%s\n", strerror(-err));
        return;
    }

    err = sensors_open(&hal_mod->common, (struct sensors_poll_device_t **) &device);
    if(err != 0) {
        MMI_ALOGE("FFBM SENSOR: sensors_open() call failed, error=%s\n", strerror(-err));
        return;
    }

    dev_count = hal_mod->get_sensors_list(hal_mod, &sensor_list);
    MMI_ALOGD("FFBM SENSOR: start activate all sensor(sensor count=%d)", dev_count);
    for(i = 0; i < dev_count; i++) {
        err = device->activate((sensors_poll_device_t *) device, sensor_list[i].handle, 0);
        if(err != SUCCESS)
            MMI_ALOGE("FFBM SENSOR: sensor(%s) activate failed, error=%s\n", sensor_list[i].name, strerror(-err));
        else
            MMI_ALOGI("FFBM SENSOR: sensor(%s), type(%d) activate success\n", sensor_list[i].name, sensor_list[i].type);
    }

    initialized = true;
    libsnscal = dlopen("sensor_calibrate.so", RTLD_NOW);
    if(!libsnscal) {
        MMI_ALOGE("FFBM SENSOR: 'sensor_calibrate.so' dlopen fail");
        return;
    }
    sensor_cal_module = (struct sensor_cal_module_t *) dlsym(libsnscal, SENSOR_CAL_MODULE_INFO_STR);
    if(!sensor_cal_module) {
        MMI_ALOGE("FFBM SENSOR: symbol(%s) loading fail\n", SENSOR_CAL_MODULE_INFO_STR);
        dlclose(libsnscal);
        return;
    }
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
    if(module == NULL) {
        MMI_ALOGE("NULL point received");
        return FAILED;
    }
    MMI_ALOGI("module deinit start for module:[%s]", module->name);

    initialized = false;

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

    sensor_enable(cur_sensor_type, 0, false);

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

    mutex_locker::autolock _L(g_mutex);
    sensor_init();

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = module_run_pcba(module, params);
    else if(!strcmp(cmd, subcmd_calibration))
        ret = module_run_calibration(module, params);
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
    .name = "Sensor",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = extra_cmd_list,
    .supported_cmd_list_size = sizeof(extra_cmd_list) / sizeof(char *),
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
