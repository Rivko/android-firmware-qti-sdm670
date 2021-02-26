/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <errno.h>
#include <hardware.h>
#include <math.h>
#include <sensors.h>
#include <sys/types.h>
#include <stdint.h>
#include "sensors_hal.h"

struct sensors_hal_device_t
{
  /* must be first member of this struct */
  sensors_poll_device_1_t device;
  bool is_open;
};

static int
sensors_close(struct hw_device_t *dev)
{
    sensors_hal_device_t *hal_device = (sensors_hal_device_t *)dev;
    hal_device->is_open = false;
    return 0;
}

static int
sensors_activate(struct sensors_poll_device_t *dev, int handle, int en)
{
    sensors_hal& hal = sensors_hal::get_instance();
    return hal.activate(handle, en);
}

static int
sensors_batch(sensors_poll_device_1_t *dev, int handle,
              int flags, int64_t sampling_period_ns,
              int64_t max_report_latency_ns)
{
    sensors_hal& hal = sensors_hal::get_instance();
    return hal.batch(handle, flags, sampling_period_ns,
                     max_report_latency_ns);
}

static int
sensors_set_delay(struct sensors_poll_device_t *dev, int handle, int64_t ns)
{
    return sensors_batch((sensors_poll_device_1_t *)dev, handle, 0, ns, 0);
}

static int
sensors_poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count)
{
    sensors_hal& hal = sensors_hal::get_instance();
    return hal.poll(data, count);
}

static int
sensors_flush(sensors_poll_device_1_t *dev, int handle)
{
    sensors_hal& hal = sensors_hal::get_instance();
    return hal.flush(handle);
}

static int
sensors_inject_sensor_data(sensors_poll_device_1_t * dev,
    const sensors_event_t *data)
{
    (void)data;
    int ret = -EPERM;

    return ret;
}

#ifdef SNS_DIRECT_REPORT_SUPPORT
static int
sensors_register_direct_channel(sensors_poll_device_1_t *dev,
    const struct sensors_direct_mem_t* mem, int channel_handle)
{
    sensors_hal& hal = sensors_hal::get_instance();
    int ret = 0;

    if (NULL == mem) {
        hal.unregister_direct_channel(channel_handle);
    } else {
        ret = hal.register_direct_channel(mem);
    }

    return ret;
}

static int
sensors_config_direct_report(sensors_poll_device_1_t *dev,
    int sensor_handle, int channel_handle, const struct sensors_direct_cfg_t * config)
{
    sensors_hal& hal = sensors_hal::get_instance();

    return hal.config_direct_report(sensor_handle, channel_handle, config);
}
#endif

static int
sensors_open(const struct hw_module_t* module, const char* id,
    struct hw_device_t** device)
{
    static sensors_hal_device_t sensors_hal_device;
    sensors_poll_device_1_t* dev = &sensors_hal_device.device;
    memset(dev, 0x00, sizeof(sensors_poll_device_1_t));
    dev->common.tag       = HARDWARE_DEVICE_TAG;
    dev->common.version   = SENSORS_DEVICE_API_VERSION_1_3;
    dev->common.module    = const_cast<hw_module_t*>(module);
    dev->common.close     = sensors_close;
    dev->activate         = sensors_activate;
    dev->setDelay         = sensors_set_delay;
    dev->poll             = sensors_poll;
    dev->batch            = sensors_batch;
    dev->flush            = sensors_flush;
    dev->inject_sensor_data = sensors_inject_sensor_data;
#ifdef SNS_DIRECT_REPORT_SUPPORT
    dev->register_direct_channel = sensors_register_direct_channel;
    dev->config_direct_report = sensors_config_direct_report;
#else
    dev->register_direct_channel = nullptr;
    dev->config_direct_report = nullptr;
#endif
    sensors_hal_device.is_open = true;
    *device = &dev->common;
    return 0;
}

static int get_sensors_list(struct sensors_module_t* module,
    struct sensor_t const** list)
{
    sensors_hal& hal = sensors_hal::get_instance();
    return hal.get_sensors_list(list);
}

static int sensors_set_operation_mode(unsigned int mode)
{
    return (mode ? -EINVAL : 0);
}

static struct hw_module_methods_t sensors_module_methods = {
    .open = sensors_open
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = (uint16_t)SENSORS_DEVICE_API_VERSION_1_4,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "QTI Sensors HAL Module",
        .author = "Qualcomm Technologies, Inc.",
        .methods = &sensors_module_methods,
        .dso = NULL,
        .reserved = {0},
    },
    .get_sensors_list = get_sensors_list,
    .set_operation_mode = sensors_set_operation_mode,
};
