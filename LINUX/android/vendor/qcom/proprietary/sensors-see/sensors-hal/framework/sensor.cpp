/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <algorithm>
#include "sensors_log.h"
#include "sensor.h"
#include "sensors_hal_common.h"

using namespace std;

int sensor::_next_handle = SENSORS_HANDLE_BASE + 1;

static const uint32_t SNS_DIRECT_MIN_VERY_FAST_RATE_HZ = 440U;
static const uint32_t SNS_DIRECT_MIN_FAST_RATE_HZ = 110U;
static const uint32_t SNS_DIRECT_MIN_NORMAL_RATE_HZ = 28U;

sensor::sensor(sensor_wakeup_type wakeup_type):_bwakeup(false)
{
    memset(&_sensor_info, 0x00, sizeof(sensor_t));
    _sensor_info.handle = _next_handle++;
    set_required_permission("");
    if (wakeup_type == SENSOR_WAKEUP)
      _bwakeup = true;
}

void sensor::set_config(sensor_params params)
{
    /* clamp the sample period according to the min and max delays supported by
       sensor.*/
    uint64_t min_delay_ns = _sensor_info.minDelay * NSEC_PER_USEC;
    uint64_t max_delay_ns = _sensor_info.maxDelay * NSEC_PER_USEC;
    sns_logd("sp_requested = %llu ns",
             (unsigned long long) params.sample_period_ns);
    sns_logd("min_delay=%lluns, max_delay=%lluns",
             (unsigned long long) min_delay_ns,
             (unsigned long long) max_delay_ns);

    if (params.sample_period_ns < min_delay_ns) {
        params.sample_period_ns = max(min_delay_ns, (uint64_t) NSEC_PER_MSEC);
    } else if (params.sample_period_ns > max_delay_ns) {
        params.sample_period_ns = max_delay_ns;
    }

    sns_logd("sp_set = %llu ns, active = %d",
             (unsigned long long) params.sample_period_ns, is_active());

    /* if new params are same as existing params, no update required. */
    if (!(params == _params)) {
        _params = params;
        /* if sensor is active, update configuration */
        if (is_active()) {
            update_config();
        }
    }
}

/* set direct channel flags */
void sensor::set_direct_channel_flag(bool en)
{
    if (en && _min_low_lat_delay != 0) {
        _sensor_info.flags |= SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC;
    } else {
        _sensor_info.flags &=
            ~(SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC | SENSOR_DIRECT_MEM_TYPE_ASHMEM);
        return;
    }


    /* Set rate flag according sensor minimum delay */
    if ( (unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_VERY_FAST_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_VERY_FAST << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else if ((unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_FAST_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_FAST << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else if ((unsigned long long)_min_low_lat_delay <=
            USEC_PER_SEC / SNS_DIRECT_MIN_NORMAL_RATE_HZ ) {
        _sensor_info.flags |=
            SENSOR_DIRECT_RATE_NORMAL << SENSOR_FLAG_SHIFT_DIRECT_REPORT;
    } else {
        _sensor_info.flags &=
            ~(SENSOR_FLAG_DIRECT_CHANNEL_GRALLOC | SENSOR_DIRECT_MEM_TYPE_ASHMEM);
        sns_loge("_min_low_lat_delay=(%d) not meet requirement, direct channel disabled.",
            _min_low_lat_delay);
    }
}