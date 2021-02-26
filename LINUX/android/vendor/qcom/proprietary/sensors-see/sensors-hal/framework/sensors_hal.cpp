/*
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <functional>
#include <cutils/properties.h>
#include <cassert>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include "sensors_hal.h"
#include "sensor_factory.h"
#include "ssc_utils.h"
#include <sensors.h>
#include "sensors_timeutil.h"

#ifdef SNS_DIRECT_REPORT_SUPPORT
#include"sns_low_lat_stream.h"
#include"sns_low_lat.h"


/*  These macros define the equivalent sample period (in us) for the various
    SENSOR_DIRECT_RATE levels.
    NOTE: Although the HAL will request at these sampling periods, it's possible
    for the sensor to stream at another nearby rate based on ODR supported by driver. */
static const int SNS_DIRECT_RATE_NORMAL_SAMPLE_US    = 20000U;
static const int SNS_DIRECT_RATE_FAST_SAMPLE_US      = 5000U;
static const int SNS_DIRECT_RATE_VERY_FAST_SAMPLE_US = 1250U;
#endif


/* Maximum virtual memory consumption allowed for this process */
#define SNS_HAL_VMEM_MAX 1000000000    // 1000MB

using namespace std;

const char SENSORS_HAL_PROP_DEBUG[] = "persist.debug.sensors.hal";

/* map debug property value to log_level */
static const unordered_map<char, sensors_log::log_level> log_level_map = {
    { '0', sensors_log::SILENT },
    { '1', sensors_log::INFO },
    { 'e', sensors_log::ERROR },
    { 'E', sensors_log::ERROR },
    { 'i', sensors_log::INFO },
    { 'I', sensors_log::INFO },
    { 'd', sensors_log::DEBUG },
    { 'D', sensors_log::DEBUG },
    { 'v', sensors_log::VERBOSE },
    { 'V', sensors_log::VERBOSE },
};

void sensors_hal::get_system_config()
{
    char debug_prop[PROPERTY_VALUE_MAX];
    int len;
    len = property_get(SENSORS_HAL_PROP_DEBUG, debug_prop, "i");
    if (len > 0) {
        if (log_level_map.find(debug_prop[0]) != log_level_map.end()) {
            _sysconfig.log_level = log_level_map.at(debug_prop[0]);
        }
    }
    sns_logi("log_level: %d", _sysconfig.log_level);
}

#if !defined (__has_feature) || !__has_feature(address_sanitizer)
/**
 * Set maximum memory consumption of this (Sensors HAL) process.
 */
static void
set_memory_limit(void) {
    struct rlimit rlim;

    errno = 0;
    if(0 != getrlimit(RLIMIT_AS, &rlim))
        sns_loge("getrlimit failed (%i) %s", errno, strerror(errno));
    else
        sns_loge("getrlimit %lu %lu", rlim.rlim_cur, rlim.rlim_max);

    rlim.rlim_cur = SNS_HAL_VMEM_MAX;
    errno = 0;
    if(0 != setrlimit(RLIMIT_AS, &rlim))
        sns_loge("setrlimit failed (%i) %s", errno, strerror(errno));
    else
        sns_loge("setrlimit succeeded %lu %lu", rlim.rlim_cur, rlim.rlim_max);
}
#endif

sensors_hal::sensors_hal()
{
    using namespace std::chrono;
    auto tp_start = steady_clock::now();
    sensors_log::set_tag("sensors-hal");
    get_system_config();
    sensors_log::set_level(_sysconfig.log_level);
    sns_logi("initializing sensors_hal");
#if !defined (__has_feature) || !__has_feature(address_sanitizer)
    set_memory_limit();
#endif
    try {
        init_sensors();
#ifdef SNS_DIRECT_REPORT_SUPPORT
        if (check_direct_channel_support()) {
            pthread_mutex_init(&open_direct_channels_mutex, NULL);
            open_direct_channels = std::list<direct_channel *>();
        }
#endif
    } catch (const runtime_error& e) {
        sns_loge("FATAL: failed to initialize sensors_hal: %s", e.what());
        return;
    }
    auto tp_end = steady_clock::now();
    sns_logi("sensors_hal initialized, init_time = %fs",
             duration_cast<duration<double>>(tp_end - tp_start).count());
}

int sensors_hal::activate(int handle, int enable)
{
    auto it = _sensors.find(handle);
    if (it == _sensors.end()) {
        sns_loge("handle %d not supported", handle);
        return -1;
    }
    auto& sensor = it->second;
    try {
        sns_logi("%s/%d en=%d", sensor->get_sensor_info().stringType,
                 handle, enable);
        if (enable) {
            sensor->activate();
        } else {
            sensor->deactivate();
        }
    } catch (const exception& e) {
        sns_loge("failed: %s", e.what());
        return -1;
    }
    sns_logi("%s/%d en=%d completed", sensor->get_sensor_info().stringType,
             handle, enable);
    return 0;
}

int sensors_hal::batch(int handle, int flags, int64_t sampling_period_ns,
                       int64_t max_report_latency_ns)
{
    auto it = _sensors.find(handle);
    if (it == _sensors.end()) {
        sns_loge("handle %d not supported", handle);
        return -1;
    }

    auto& sensor = it->second;
    sensor_params params;
    params.max_latency_ns = max_report_latency_ns;
    params.sample_period_ns = sampling_period_ns;

    try {
        sns_logi("%s/%d, period=%lld, max_latency=%lld",
                 sensor->get_sensor_info().stringType, handle,
                 (long long) sampling_period_ns,
                 (long long) max_report_latency_ns);
        sensor->set_config(params);
    } catch (const exception& e) {
        sns_loge("failed: %s", e.what());
        return -1;
    }
    sns_logi("%s/%d, period=%lld, max_latency=%lld request completed",
             sensor->get_sensor_info().stringType, handle,
             (long long) sampling_period_ns,
             (long long) max_report_latency_ns);
    return 0;
}

int sensors_hal::flush(int handle)
{
    auto it = _sensors.find(handle);
    if (it == _sensors.end()) {
        sns_loge("handle %d not supported", handle);
        return -1;
    }
    auto& sensor = it->second;
    const sensor_t& sensor_info = sensor->get_sensor_info();
    bool is_oneshot = (sensor->get_reporting_mode() == SENSOR_FLAG_ONE_SHOT_MODE);
    if (!sensor->is_active() || is_oneshot) {
        sns_loge("invalid flush request, active=%d, oneshot=%d",
                  sensor->is_active(), is_oneshot);
        return -EINVAL;
    }
    try {
        sns_logi("%s/%d", sensor_info.stringType, handle);
        sensor->flush();
    } catch (const exception& e) {
        sns_loge("failed, %s", e.what());
        return -1;
    }
    sns_logi("%s/%d completed", sensor_info.stringType, handle);

    return 0;
}

int sensors_hal::poll(sensors_event_t* data, int count)
{
    sns_logv("");
    int num_events = 0;
    /* get up to count number of events, block if no events available */
    for (int i = 0; i < count; i++) {
        if (num_events == 0 || _event_queue.size() > 0) {
            data[num_events++] = _event_queue.pop();
        }
    }
    sns_logv("num_events = %d", num_events);
     /*queue can have wakeup and nonwakeup so releasing in for loop
        once the wakeup events are "0" can make system  go to suspend
        moving down can decrease the probability of premature suspend*/
    _event_queue.release_wakelock();
    return num_events;
}

void sensors_hal::init_sensors()
{
    auto sensors = sensor_factory::instance().get_all_available_sensors();
    auto cb  = [this](const auto& event, auto wakeup) { _event_queue.push(event, wakeup); };

    for (unique_ptr<sensor>& s : sensors) {
        assert(s != nullptr);
        s->register_callback(cb);
        const sensor_t& sensor_info = s->get_sensor_info();
        sns_logd("%s: %s/%d wakeup=%d", sensor_info.name,
                 sensor_info.stringType, sensor_info.handle,
                 (sensor_info.flags & SENSOR_FLAG_WAKE_UP) != 0);
        _hal_sensors.push_back(sensor_info);
        _sensors[sensor_info.handle] = std::move(s);
    }
#ifdef SNS_DIRECT_REPORT_SUPPORT
    if (check_direct_channel_support()) {
        gralloc_device = nullptr;
    }
    /* write sensor list to file*/
#endif
}

int sensors_hal::get_sensors_list(const sensor_t **s_list)
{
    int num_sensors = (int)_hal_sensors.size();
    sns_logi("num_sensors=%d", num_sensors);
    *s_list = &_hal_sensors[0];
    return num_sensors;
}

#ifdef SNS_DIRECT_REPORT_SUPPORT
int sensors_hal::register_direct_channel(const struct sensors_direct_mem_t* mem)
{
    int ret = 0;
    struct native_handle *mem_handle = NULL;
    int sns_low_lat_handle = -1;

    sns_logi("%s : enter", __FUNCTION__);
    /* Validate input parameters */
    if (mem->type != SENSOR_DIRECT_MEM_TYPE_GRALLOC ||
        mem->format != SENSOR_DIRECT_FMT_SENSORS_EVENT ||
        mem->size == 0) {
        sns_loge("%s : invalid input mem type=%d, format=%d, size=%zu",
            __FUNCTION__, mem->type, mem->format, mem->size);
        return -EINVAL;
    }

    /* Make sure the input fd isn't already in use */
    if (mem->handle == NULL ||
        this->is_used_buffer_fd(mem->handle->data[0])) {
        sns_loge("%s : invalid FD", __FUNCTION__);
        return -EINVAL;
    }
    sns_logd("%s : handle=%p, size=%zu", __FUNCTION__,
        mem->handle, mem->size);

    /* Make sure memory not in use */
    if (this->is_used_memory(mem->handle)) {
        sns_loge("%s : memory is in use %p", __FUNCTION__, mem->handle);
        return -EINVAL; //debug only
    }

    /* Construct a new direct channel object and initialize channel */
    direct_channel* channel = new direct_channel(mem->handle, mem->size);
    sns_low_lat_handle = channel->get_low_lat_handle();
    if (sns_low_lat_handle == -1) {
        sns_loge("%s : direct channel init failed, ret=%d", __FUNCTION__, ret);
        return -EINVAL;
    }

    /* Add the new direct channel object to the list of direct channels */
    ret = pthread_mutex_lock(&open_direct_channels_mutex);
    if (ret == 0) {
        open_direct_channels.insert(open_direct_channels.cend(), channel);

        ret = pthread_mutex_unlock(&open_direct_channels_mutex);
        if (ret != 0) {
            sns_loge("%s : error unlocking mutex ret %d", __FUNCTION__, ret);
        }
    } else {
        sns_loge("%s : error locking mutex ret %d", __FUNCTION__, ret);
    }

    /* If all goes well, return the android handle */
    if (ret == 0) {
        sns_logv("%s : assigned channel handle %d", __FUNCTION__,
                    channel->get_client_channel_handle());
        return channel->get_client_channel_handle();
    } else {
        return -1;
    }
}

void sensors_hal::unregister_direct_channel(int channel_handle)
{
    int ret;
    std::list<direct_channel*>::const_iterator iter;
    sns_logv("%s : channel handle %d", __FUNCTION__, channel_handle);

    /* Search for the corresponding DirectChannel object */
    ret = pthread_mutex_lock(&open_direct_channels_mutex);
    if (ret == 0) {
        iter = find_direct_channel(channel_handle);
        /* If the channel is found, stop it, remove it and delete it */
        if (iter != open_direct_channels.cend()) {
            /* The stream_stop call below isn't strictly necessary. When the
               DirectChannel object is deconstructed, it calls stream_deinit()
               which inherently calls stream_stop(). But it was added here for
               completeness-sake. */
            ret = sns_low_lat_stream_stop((*iter)->get_low_lat_handle());
            delete *iter;
            open_direct_channels.erase(iter);
        } else {
            sns_loge("%s : unable to find channel handle %d", __FUNCTION__, channel_handle);
        }

        ret = pthread_mutex_unlock(&open_direct_channels_mutex);
        if (ret != 0) {
            sns_loge("%s : error unlocking mutex %d", __FUNCTION__, ret);
        }
    } else {
        sns_loge("%s : error locking mutex ret %d", __FUNCTION__, ret);
    }
}

int sensors_hal::config_direct_report(int sensor_handle, int channel_handle,
    const struct sensors_direct_cfg_t *config)
{
    int ret = -EINVAL;
    int type;
    sns_std_suid_t request_suid = {0, 0};
    sensor_uid suid;
    int64 timestamp_offset;
    unsigned int flags = SNS_LLCM_FLAG_ANDROID_STYLE_OUTPUT;
    int sns_low_lat_handle = -1;
    sensor_factory& sensor_fac = sensor_factory::instance();
    std::list<direct_channel*>::const_iterator iter;

    sns_logi("%s : enter", __FUNCTION__);

    ret = pthread_mutex_lock(&open_direct_channels_mutex);
    if (ret == 0) {
        /* Find the corresponding direct channel */
        iter = find_direct_channel(channel_handle);
        if (iter != open_direct_channels.cend()) {
            sns_low_lat_handle = (*iter)->get_low_lat_handle();
        } else {
            sns_loge("%s : The channel %d is not available!", __FUNCTION__, channel_handle);
            ret = pthread_mutex_unlock(&open_direct_channels_mutex);
            if (ret != 0) {
                sns_loge("%s : error unlocking mutex %d", __FUNCTION__, ret);
            }
            return -EINVAL;
        }
    } else {
        sns_loge("%s : error locking mutex ret %d", __FUNCTION__, ret);
    }
    ret = pthread_mutex_unlock(&open_direct_channels_mutex);
    if (ret != 0) {
        sns_loge("%s : error unlocking mutex %d", __FUNCTION__, ret);
    }

    /* If sensor_handle is -1 and the rate is STOP, then stop all streams within
       the channel */
    if (sensor_handle == -1) {
        if (config->rate_level == SENSOR_DIRECT_RATE_STOP) {
            /* Send special SUID to stop all sensors in the channel
              without tearing down the channel */
            ret = sns_low_lat_stream_config(sns_low_lat_handle,
                0, &request_suid, 0, flags, sensor_handle);
        } else {
            sns_loge("%s : unexpected inputs, sensor handle %d rete_level %d", __FUNCTION__,
                        sensor_handle, config->rate_level);
            ret = -EINVAL;
        }
    } else {
        auto it = _sensors.find(sensor_handle);

        if (it == _sensors.end()) {
            sns_loge("handle %d not supported", sensor_handle);
            return -EINVAL;
        }
        /* Obtain the sensor SUID */
        auto& sensor = it->second;

        suid = sensor->get_sensor_suid();

        /* Check whether calibration is required */
        if(sensor->is_calibrated())
        {
          flags |= SNS_LLCM_FLAG_CALIBRATED_STREAM;
        }
        request_suid.suid_low = suid.low;
        request_suid.suid_high = suid.high;

        if (config->rate_level == SENSOR_DIRECT_RATE_STOP) {
            /* Stop single sensor by set sampling period to 0 */
            ret = sns_low_lat_stream_config(sns_low_lat_handle,
                0, &request_suid, 0, flags, sensor_handle);
            if (ret) {
                sns_loge("%s : Deactivate the handle %d is not successful",
                             __FUNCTION__, sensor_handle);
            }
        } /* Check to make sure the sensor supports the desired rate */
        else if ((int)((sensor->get_sensor_info().flags & SENSOR_FLAG_MASK_DIRECT_REPORT) >>
                    SENSOR_FLAG_SHIFT_DIRECT_REPORT) >= config->rate_level) {
            unsigned int sample_period_us = direct_rate_level_to_sample_period(config->rate_level);
            if (sample_period_us == 0) {
                sns_loge("%s : Unexpected direct report rate %d for handle %d", __FUNCTION__,
                            config->rate_level, sensor_handle);
                return -EINVAL;
            }
            /* Get timestamp offset */
            timestamp_offset = sensors_timeutil::get_instance().getElapsedRealtimeNanoOffset();
            sns_logi("%s : config direct report, TS_offset=%lld",
                             __FUNCTION__, timestamp_offset);
            /* Start sensor with non-zero sampling rate */
            ret = sns_low_lat_stream_config(sns_low_lat_handle,
                timestamp_offset, &request_suid, sample_period_us, flags, sensor_handle);
            if (ret) {
                sns_loge("%s : activate the handle %d is not successful", __FUNCTION__,
                           sensor_handle);
            } else {
                /* Return the sensor handle if everything is successful */
                ret = sensor_handle;
            }
        } else {
            sns_loge("%s : Unsupported direct report rate %d for handle %d", __FUNCTION__,
                        config->rate_level, sensor_handle);
        }
    }
    return ret;
}

/*
  * Find channel which match match particular handle,
  * this function should be used under protect of mutex
  */
std::list<direct_channel*>::const_iterator sensors_hal::find_direct_channel(int channel_handle)
{
    for (auto iter = open_direct_channels.cbegin();
        iter != open_direct_channels.cend(); ++iter) {
        if ((*iter)->get_client_channel_handle() == channel_handle) {
            return iter;
        }
    }
    return open_direct_channels.cend();
}

bool sensors_hal::is_used_buffer_fd(int buffer_fd)
{
    int ret = -EINVAL;

    ret = pthread_mutex_lock(&open_direct_channels_mutex);
    if (ret != 0) {
        sns_loge("%s : error locking mutex ret %d", __FUNCTION__, ret);
        /* return true to block use of fd */
        return true;
    }

    for (auto iter = open_direct_channels.cbegin();
        iter != open_direct_channels.cend(); ++iter) {
        if ((*iter)->get_buffer_fd() == buffer_fd) {
            ret = pthread_mutex_unlock(&open_direct_channels_mutex);
            return true;
        }
    }

    ret = pthread_mutex_unlock(&open_direct_channels_mutex);
    if (ret != 0) {
        sns_loge("%s : error unlocking mutex %d", __FUNCTION__, ret);
    }
    return false;
}

bool sensors_hal::is_used_memory(const struct native_handle *mem_handle)
{
    int ret = -EINVAL;
    bool found = false;

    ret = pthread_mutex_lock(&open_direct_channels_mutex);
    if (ret != 0) {
        sns_loge("%s : error locking mutex ret %d", __FUNCTION__, ret);
        /* return true to block use of fd */
        return true;
    }

    for (auto iter = open_direct_channels.cbegin();
        iter != open_direct_channels.cend(); ++iter) {
        if ((*iter)->is_same_memory(mem_handle)) {
            found = true;
            break;
        }
    }

    ret = pthread_mutex_unlock(&open_direct_channels_mutex);
    if (ret != 0) {
        sns_loge("%s : error unlocking mutex %d", __FUNCTION__, ret);
    }
    return found;
}

unsigned int sensors_hal::direct_rate_level_to_sample_period(int rate_level)
{
    unsigned int sample_period_us = 0;
    switch(rate_level) {
        case SENSOR_DIRECT_RATE_NORMAL:
            sample_period_us = SNS_DIRECT_RATE_NORMAL_SAMPLE_US;
            break;
        case SENSOR_DIRECT_RATE_FAST:
            sample_period_us = SNS_DIRECT_RATE_FAST_SAMPLE_US;
            break;
        case SENSOR_DIRECT_RATE_VERY_FAST:
            sample_period_us = SNS_DIRECT_RATE_VERY_FAST_SAMPLE_US;
            break;
       default:
            sns_loge("%s : Unsupported direct report rate %d", __FUNCTION__, rate_level);
            break;
    }
    return sample_period_us;
}
#endif
