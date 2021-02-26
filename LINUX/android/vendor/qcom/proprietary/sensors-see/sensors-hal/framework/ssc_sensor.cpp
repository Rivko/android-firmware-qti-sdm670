/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <thread>
#include <chrono>

#include "ssc_utils.h"
#include "ssc_sensor.h"
#include "sensors_log.h"
#include "sensors_timeutil.h"
#include "sns_resampler.pb.h"
#include <mutex>

using namespace std;
static const auto MSEC_PER_SEC = 1000;
static const auto WAIT_TIME_MS = 250;

static const char *SSC_DATATYPE_RESAMPLER = "resampler";
static char DIAG_LOG_MODULE_NAME[] = "sensors-hal";

ssc_sensor::ssc_sensor(sensor_uid suid, sensor_wakeup_type wakeup):
    sensor(wakeup),
    _suid(suid),
    _attributes(lookup_attributes()),
    _resampler_suid(get_resampler_suid()),
    _pending_flush_requests(0),
    _diag(uint64_t(this))
{
    if (wakeup == SENSOR_WAKEUP) {
        set_wakeup_flag(true);
        _wakeup_type = wakeup;
    }
    set_sensor_info();
}

void ssc_sensor::set_sensor_typename(const string& type_name)
{
    string name_attr = attributes().get_string(SNS_STD_SENSOR_ATTRID_NAME);
    name_attr.pop_back(); /* remove NUL terminator */

    _sensor_name = name_attr + " " + type_name +
        ((_wakeup_type == SENSOR_WAKEUP) ? " Wakeup" : " Non-wakeup");
    set_name(_sensor_name.c_str());
}
void ssc_sensor::set_nowk_msgid(const int msg_id)
{
  _nowk_msg_id_list.push_back(msg_id);
}
void ssc_sensor::set_sensor_info()
{
    const sensor_attributes& attr = attributes();

    set_vendor(attr.get_string(SNS_STD_SENSOR_ATTRID_VENDOR).c_str());
    _datatype = attr.get_string(SNS_STD_SENSOR_ATTRID_TYPE);

    set_sensor_typename();

    auto version = attr.get_ints(SNS_STD_SENSOR_ATTRID_VERSION);
    if (version.size() > 0 && version[0] > 0) {
        set_version(version[0]);
    } else {
        set_version(1);
    }

    auto stream_type = attr.get_stream_type();
    switch (stream_type) {
      case SNS_STD_SENSOR_STREAM_TYPE_STREAMING: {
          set_reporting_mode(SENSOR_FLAG_CONTINUOUS_MODE);
          vector<float> rates = attr.get_floats(SNS_STD_SENSOR_ATTRID_RATES);
          /* streaming sensors must have rates attribute */
          if (rates.size() == 0) {
              throw runtime_error(
                  "rates attribute unavailable for a streaming sensor");
          }

          /* ceil() is used to make sure that delay (period) is not
             truncated, as it will cause the calculated rates to be
             more than what is supported by the sensor */
          _sensor_max_delay = int32_t(ceil(USEC_PER_SEC / rates[0]));
          set_max_delay(_sensor_max_delay);
          int32_t min_delay = int32_t(ceil(USEC_PER_SEC / rates[rates.size()-1]));
          set_min_delay(min_delay);

          vector<float> low_lat_rates = attr.get_floats(SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES);
          if (low_lat_rates.size() != 0) {
            min_delay = int32_t(ceil(USEC_PER_SEC / low_lat_rates[low_lat_rates.size()-1]));
          }
          set_min_low_lat_delay(min_delay);

          /* if streaming sensor is physical, use resampler */
          auto phys_sensor_attr = attr.get_bools(
              SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR);
          if (phys_sensor_attr.size() > 0 && phys_sensor_attr[0] == true) {
              set_resampling(true);
          }
          break;
      }
      case SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE:
      case SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT:
          set_reporting_mode(SENSOR_FLAG_ON_CHANGE_MODE);
          break;
      default:
          throw runtime_error("invalid stream_type " + to_string(stream_type));
    }

    auto resolution = attr.get_floats(SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION);
    if (resolution.size() > 0) {
        set_resolution(resolution[0]);
    } else {
        /* set default value 0.1 */
        set_resolution(0.1);
        sns_logd("dt=%s, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION not set",
                 _datatype.c_str());
    }


    auto ranges = attr.get_ranges(SNS_STD_SENSOR_ATTRID_SELECTED_RANGE);
    if (ranges.size() > 0) {
        set_max_range(ranges[0].second);
    } else {
        /* set default value 1.0 */
        set_max_range(1.0f);
        sns_logd("dt=%s, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE not set",
                 _datatype.c_str());
    }

    auto currents = attr.get_ints(SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT);
    if (currents.size() > 0) {
        /* use max current and convert from uA to mA */
        auto max_iter = std::max_element(currents.begin(), currents.end());
        set_power(float(*max_iter) / 1000.0);
    }

    // TODO: calculate following based on system capacity
    set_fifo_max_count(10000);
}

void ssc_sensor::activate()
{
    std::lock_guard<mutex> lk(_mutex);
    if (!is_active()) {
        /* establish a new connection to ssc */
        _ssc_conn = make_unique<ssc_connection>(
        [this](const uint8_t *data, size_t size)
        {
            ssc_conn_event_cb(data, size);
        });
        if ( _wakeup_type == SENSOR_WAKEUP)
            _ssc_conn->set_unsuspendable_channel();
        _ssc_conn->use_report_indication();
        _ssc_conn->register_error_cb([this](auto e){ ssc_conn_error_cb(e); });
        _ssc_conn->register_resp_cb([this](uint32_t resp_value){ ssc_conn_resp_cb(resp_value); });
        send_sensor_config_request();
    }
}

void ssc_sensor::deactivate()
{
    std::lock_guard<mutex> lk(_mutex);
    if (is_active()) {
        _ssc_conn.reset();
    }
}
void ssc_sensor::add_nowk_msgid_list(sns_client_request_msg &req_msg)
{
  std::vector<int>::iterator it = _nowk_msg_id_list.begin();
  while(it != _nowk_msg_id_list.end())
  {
    req_msg.mutable_susp_config()->add_nowakeup_msg_ids(*it);
    it++;
  }
}

sns_client_request_msg ssc_sensor::create_resampler_config_request()
{
    sns_client_request_msg pb_req_msg;
    sns_resampler_config pb_resampler_config;
    string pb_resampler_config_encoded;
    float sample_rate = float(NSEC_PER_SEC) / get_params().sample_period_ns;
    sns_logd("sr=%f", sample_rate);
    pb_resampler_config.mutable_sensor_uid()->set_suid_high(_suid.high);
    pb_resampler_config.mutable_sensor_uid()->set_suid_low(_suid.low);
    pb_resampler_config.set_resampled_rate(sample_rate);
    pb_resampler_config.set_rate_type(SNS_RESAMPLER_RATE_FIXED);
    pb_resampler_config.set_filter(true); // TODO: check if this is correct

    pb_resampler_config.SerializeToString(&pb_resampler_config_encoded);
    sns_logd("SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG");
    add_nowk_msgid_list(pb_req_msg);
    pb_req_msg.set_msg_id(SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG);
    pb_req_msg.mutable_request()->set_payload(pb_resampler_config_encoded);

    return pb_req_msg;
}

void ssc_sensor::send_sensor_config_request()
{
    string pb_req_msg_encoded;

    sns_client_request_msg pb_req_msg;

    if (_resampling == true) {
        pb_req_msg = create_resampler_config_request();
        pb_req_msg.mutable_suid()->set_suid_high(_resampler_suid.high);
        pb_req_msg.mutable_suid()->set_suid_low(_resampler_suid.low);
    } else {
        pb_req_msg = create_sensor_config_request();
        pb_req_msg.mutable_suid()->set_suid_high(_suid.high);
        pb_req_msg.mutable_suid()->set_suid_low(_suid.low);
    }

    uint32_t batch_period_us = get_params().max_latency_ns / NSEC_PER_USEC;
    uint32_t flush_period_us = 0;

    /* To support Android Hi-Fi requirements for non-wakeup sensors, if
       batch period is less than the time required to buffer
       fifoReservedEventCount number of events, we need to set flush period
       according to the fifoReservedEventCount. For streaming sensors,
       this will be derived from the sample_period and for non-streaming
       sensors, flush period will be set to a max value */
    if (_wakeup_type == SENSOR_NO_WAKEUP) {
        if (get_reporting_mode() == SENSOR_FLAG_CONTINUOUS_MODE)
        {
            flush_period_us = (get_params().sample_period_ns / NSEC_PER_USEC) *
                    get_sensor_info().fifoReservedEventCount;
        } else {
            if (get_sensor_info().fifoReservedEventCount > 0) {
                flush_period_us = UINT32_MAX;
            }
        }
        if (flush_period_us > batch_period_us) {
            pb_req_msg.mutable_request()->mutable_batching()->
                set_flush_period(flush_period_us);
        } else {
            flush_period_us = 0;
        }
    } else {
        /*after fifoMaxEventCount need to wake up Apps and send the samples
        set batch period corresponding to that as you need to wakeup the App*/
        if (get_reporting_mode() == SENSOR_FLAG_CONTINUOUS_MODE) {
            uint32_t batch_period_max_us =
            (get_params().sample_period_ns / NSEC_PER_USEC) *
            get_sensor_info().fifoMaxEventCount;
            if(batch_period_us > batch_period_max_us) {
                sns_logi("dt=%s, asked batch_period_us : %u adjusted_us : %u",
                    _datatype.c_str(),
                    (unsigned int)batch_period_us,
                    (unsigned int)batch_period_max_us);
                batch_period_us = batch_period_max_us;
            }
         }
    }

    pb_req_msg.mutable_request()->mutable_batching()->
        set_batch_period(batch_period_us);

    pb_req_msg.mutable_susp_config()->set_delivery_type(get_delivery_type());
    pb_req_msg.mutable_susp_config()->
        set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);

    sns_logd("dt=%s, bp=%u, fp=%u, resampling=%d", _datatype.c_str(),
             (unsigned int) batch_period_us,
             (unsigned int) flush_period_us,
             _resampling);
    _diag.log_request_msg(pb_req_msg_encoded, _datatype, DIAG_LOG_MODULE_NAME);
    _ssc_conn->send_request(pb_req_msg_encoded);
}

void ssc_sensor::update_config()
{
    lock_guard<mutex> lk(_mutex);
    send_sensor_config_request();
}

void ssc_sensor::flush()
{
    std::lock_guard<mutex> lk(_mutex);
    if (!is_active()) {
        return;
    }
    sns_client_request_msg pb_req_msg;
    string pb_req_msg_encoded;

    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_FLUSH_REQ);
    pb_req_msg.mutable_request()->clear_payload();
    if (_resampling == true) {
        pb_req_msg.mutable_suid()->set_suid_high(_resampler_suid.high);
        pb_req_msg.mutable_suid()->set_suid_low(_resampler_suid.low);
    } else {
        pb_req_msg.mutable_suid()->set_suid_high(_suid.high);
        pb_req_msg.mutable_suid()->set_suid_low(_suid.low);
    }

    pb_req_msg.mutable_susp_config()->set_delivery_type(get_delivery_type());
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    pb_req_msg.SerializeToString(&pb_req_msg_encoded);

    _pending_flush_requests++;
    uint32_t pending = _pending_flush_requests;
    sns_logd("sending SNS_STD_MSGID_SNS_STD_FLUSH_REQ, pending=%u",
             (unsigned int) pending);

    _diag.log_request_msg(pb_req_msg_encoded, _datatype, DIAG_LOG_MODULE_NAME);

    _ssc_conn->send_request(pb_req_msg_encoded);
}

void ssc_sensor::handle_sns_std_flush_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    uint32_t pending = _pending_flush_requests;
    if (pending > 0) {
        sensors_event_t hal_event;
        memset(&hal_event, 0x00, sizeof(sensors_event_t));
        hal_event.version = META_DATA_VERSION;
        hal_event.meta_data.what = META_DATA_FLUSH_COMPLETE;
        hal_event.meta_data.sensor = get_sensor_info().handle;
        sns_logd("dt=%s, META_DATA_FLUSH_COMPLETE, pending=%u",
                 _datatype.c_str(), (unsigned int) pending);
        submit_sensors_hal_event(hal_event);
        _pending_flush_requests--;
        on_flush_complete();
    }
}

void ssc_sensor::ssc_conn_event_cb(const uint8_t *data, size_t size)
{
    string pb_event_msg_encoded((char *)data, size);
    _diag.log_event_msg(pb_event_msg_encoded, _datatype, DIAG_LOG_MODULE_NAME);

    sns_client_event_msg pb_event_msg;
    pb_event_msg.ParseFromArray(data, size);
    for (int i=0; i < pb_event_msg.events_size(); i++) {
        auto&& pb_event = pb_event_msg.events(i);
        sns_logv("event[%d] msg_id=%d, ts=%llu", i, pb_event.msg_id(),
                 (unsigned long long) pb_event.timestamp());

        if (pb_event.msg_id() == SNS_STD_MSGID_SNS_STD_FLUSH_EVENT) {
            handle_sns_std_flush_event(pb_event);
        } else {
            handle_sns_client_event(pb_event);
        }
    }
}
void ssc_sensor::ssc_conn_resp_cb(uint32_t resp_value)
{
  _diag.log_response_msg(resp_value,_datatype, DIAG_LOG_MODULE_NAME);
}
void ssc_sensor::ssc_conn_error_cb(ssc_error_type e)
{
    sns_loge("error = %d", e);
    /* re-send config request when connection resets */
    if (e == SSC_CONNECTION_RESET) {
        bool got_sensorback = false;
        uint8_t retry_cnt = RETRY_CNT;
        sns_loge("connection reset, resend config request");
        //check for suid and then send config request
        while (retry_cnt && is_active()) {
            using namespace std::chrono;
            suid_lookup lookup(
                [this, &got_sensorback](const string& datatype, const auto& suids)
                {
                    sns_logd("got the suid call back");
                    int i;
                    for (i = 0; i < (int)suids.size(); i++) {
                      if ((suids[i].high == _suid.high) &&
                          (suids[i].low == _suid.low))
                          got_sensorback = true;
                    }
                });
            if (got_sensorback == true) {
                sns_logi("after ssr, discovery of %s took %f sec",
                    _datatype.c_str(),
                    (float)((RETRY_CNT-retry_cnt)*((float)WAIT_TIME_MS/MSEC_PER_SEC)));
                break;
            }
            lookup.request_suid(_datatype);
            retry_cnt--;
            sns_logd("requesting suid (cnt) %d", retry_cnt);
            this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
        }
        if (is_active()) {
            if (got_sensorback)
                send_sensor_config_request();
            else
                sns_loge("could not restart %s after ssr", _datatype.c_str());
        } else {
            sns_logi("%s deactivated during ssr", _datatype.c_str());
        }
    }
}

void ssc_sensor::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    switch(pb_event.msg_id()) {
      case SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT:
          handle_sns_std_sensor_event(pb_event);
          break;
      default:
          break;
    }
}

sns_client_request_msg ssc_sensor::create_sensor_config_request()
{
    sns_client_request_msg pb_req_msg;
    auto stream_type = attributes().get_stream_type();
    add_nowk_msgid_list(pb_req_msg);
    /* populate request message based on stream type */
    if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING) {
        float sample_rate = float(NSEC_PER_SEC) / get_params().sample_period_ns;
        sns_logd("sr=%f", sample_rate);
        sns_std_sensor_config pb_stream_cfg;
        string pb_stream_cfg_encoded;
        sns_logd("sending SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG");
        pb_stream_cfg.set_sample_rate(sample_rate);
        pb_stream_cfg.SerializeToString(&pb_stream_cfg_encoded);
        pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
        pb_req_msg.mutable_request()->set_payload(pb_stream_cfg_encoded);
    } else {
        sns_logd("sending SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG");
        pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
        pb_req_msg.mutable_request()->clear_payload();
    }
    return pb_req_msg;
}

void ssc_sensor::handle_sns_std_sensor_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_stream_event;
    pb_stream_event.ParseFromString(pb_event.payload());

    sensors_event_t hal_event = create_sensor_hal_event(pb_event.timestamp());

    int num_items = pb_stream_event.data_size();
    if (num_items > SENSORS_HAL_MAX_DATA_LEN) {
        sns_loge("num_items=%d exceeds SENSORS_HAL_MAX_DATA_LEN=%d",
                 num_items, SENSORS_HAL_MAX_DATA_LEN);
        num_items = SENSORS_HAL_MAX_DATA_LEN;
    }

    for (int i = 0; i < num_items; i++) {
        hal_event.data[i] = pb_stream_event.data(i);
    }

    if (sensors_log::get_level() >= sensors_log::DEBUG) {
        /* debug: print the event data */
        string s = "[";
        for (int i = 0; i < pb_stream_event.data_size(); i++) {
            s += to_string(pb_stream_event.data(i));
            if (i < pb_stream_event.data_size() - 1) {
                 s += ", ";
            } else {
                s += "]";
            }
        }
        sns_logd("%s_sample: ts = %llu ns; value = %s", _datatype.c_str(),
                 (unsigned long long) hal_event.timestamp, s.c_str());
    }

    submit_sensors_hal_event(hal_event);
}

sensors_event_t ssc_sensor::create_sensor_hal_event(uint64_t ssc_timestamp)
{
    sensors_event_t hal_event;
    //Fix for static analysis error - uninitialized variable
    memset(&hal_event, 0x00, sizeof(sensors_event_t));
    hal_event.version = sizeof(sensors_event_t);
    hal_event.sensor = get_sensor_info().handle;
    hal_event.type = get_sensor_info().type;
    hal_event.timestamp = sensors_timeutil::get_instance().
        qtimer_ticks_to_elapsedRealtimeNano(ssc_timestamp);

    sns_logv("ssc_ts = %llu tk, hal_ts = %lld ns, %lf",
             (unsigned long long)ssc_timestamp,
             (long long)hal_event.timestamp, hal_event.timestamp / 1000000000.0 );
    return hal_event;
}

sensor_uid ssc_sensor::get_resampler_suid()
{
    const auto& suids =
        sensor_factory::instance().get_suids(SSC_DATATYPE_RESAMPLER);
    if (suids.size() == 0) {
        throw runtime_error("resampler suid not found");
    }
    return suids[0];
}

void ssc_sensor::set_resampling(bool val)
{
    _resampling = val;
    /* change max delay (min rate) based on resampler usage */
    set_max_delay(val ? RESAMPLER_MAX_DELAY : _sensor_max_delay);
}

int8_t ssc_sensor::sensors_hal_sample_status(
    sns_std_sensor_sample_status std_status)
{
    switch (std_status) {
      case SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE:
          return SENSOR_STATUS_UNRELIABLE;
      case SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW:
          return SENSOR_STATUS_ACCURACY_LOW;
      case SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_MEDIUM:
          return SENSOR_STATUS_ACCURACY_MEDIUM;
      case SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH:
          return SENSOR_STATUS_ACCURACY_HIGH;
      default:
          sns_loge("invalid std_status = %d", std_status);
    }
    return SENSOR_STATUS_UNRELIABLE;
}

static bool ssc_sensor_module_init()
{
    sensor_factory::request_datatype(SSC_DATATYPE_RESAMPLER);
    return true;
}

SENSOR_MODULE_INIT(ssc_sensor_module_init);
