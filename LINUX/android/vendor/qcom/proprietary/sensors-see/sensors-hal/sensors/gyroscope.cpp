/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <mutex>
#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_resampler.pb.h"
#include "sns_cal.pb.h"
#include "additionalinfo_sensor.h"
#include "sensors_timeutil.h"
#include "worker.h"

using namespace std;

using namespace std::placeholders;
static const char *SSC_DATATYPE_GYRO = "gyro";
static const char *SSC_DATATYPE_GYROCAL = "gyro_cal";
static const char *SSC_DATATYPE_TEMP    = "sensor_temperature";

static const float RAD_PER_DEG = 3.1415 / 180.0;

/**
 * @brief class implementing both calibrated and uncalibrated
 *        gyroscope sensors
 *
 */
class gyroscope : public ssc_sensor
{
public:
    gyroscope(sensor_uid gyro_suid, sensor_wakeup_type wakeup,
              sensor_cal_type calibrated);
    virtual bool is_calibrated() override;
private:
    /* see sensor::activate */
    virtual void activate() override;

    /* see sensor::deactivate */
    virtual void deactivate() override;

    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;

    virtual void on_flush_complete() override;

    /* send gyro_temp additional info to hal */
    void send_additional_info(int64_t timestamp);

    void start_gyrocal();

    sensor_uid _gyrotemp_suid;
    sensor_cal_type _cal;
    bool _gyro_temp_available = false;
    bool _first_additional_info_sent = false;
    float _temp;
    std::unique_ptr<additionalinfo_sensor> _gyrotemp;
    void gyro_temp_event_cb(sensors_event_t hal_event);

    static int active_count;
    static ssc_connection* gyrocal_conn;
    static sensor_uid gyrocal_suid;
    static bool gyrocal_initialized;
    static std::mutex cal_mutex;
    static float bias[3];
    /* status based on calibration accuracy */
    static int8_t sample_status;

    static void send_cal_req();
    static void gyrocal_conn_event_cb(const uint8_t *data, size_t size);
    /*create a seperate worker*/
    static worker *_gyrocal_worker;
};

/* static member variables */
int gyroscope::active_count = 0;
ssc_connection* gyroscope::gyrocal_conn;
sensor_uid gyroscope::gyrocal_suid;
bool gyroscope::gyrocal_initialized = false;
std::mutex gyroscope::cal_mutex;
float gyroscope::bias[3] = {0};
int8_t gyroscope::sample_status = SENSOR_STATUS_UNRELIABLE;
worker *gyroscope::_gyrocal_worker;

gyroscope::gyroscope(sensor_uid gyro_suid, sensor_wakeup_type wakeup,
                     sensor_cal_type calibrated):
    ssc_sensor(gyro_suid, wakeup),
    _cal(calibrated),
    _gyro_temp_available(false)
{
    if (_cal == SENSOR_CALIBRATED) {
        set_type(SENSOR_TYPE_GYROSCOPE);
        set_string_type(SENSOR_STRING_TYPE_GYROSCOPE);
        set_sensor_typename("Gyroscope");
    } else {
        set_type(SENSOR_TYPE_GYROSCOPE_UNCALIBRATED);
        set_string_type(SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED);
        set_sensor_typename("Gyroscope-Uncalibrated");
    }
#ifdef SNS_DIRECT_REPORT_SUPPORT
    if (check_direct_channel_support()) {
        set_direct_channel_flag(true);
    }
#endif

    if (gyrocal_initialized == false) {
        const auto& gyrocal_suids =
            sensor_factory::instance().get_suids(SSC_DATATYPE_GYROCAL);
        if (gyrocal_suids.size() == 0) {
            throw runtime_error("gyrocal suid not found");
        }
        /* get first gyrocal suid (only one expected) */
        gyrocal_suid = gyrocal_suids[0];
        gyrocal_initialized = true;
    }

    if(!sensor_factory::instance().get_pairedsuid(
                                    SSC_DATATYPE_TEMP,
                                    gyro_suid,
                                    _gyrotemp_suid))
    {
         _gyro_temp_available = true;
         set_additional_info_flag(true);
         sns_logd("gyro temperature is available..");
    }
    set_resampling(true);
    set_nowk_msgid(SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT);

    /* convert range from dps to rps */
    set_max_range(get_sensor_info().maxRange * RAD_PER_DEG);
    /* convert resolution for dps to rps */
    set_resolution(get_sensor_info().resolution * RAD_PER_DEG);
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    _gyrocal_worker = nullptr;
}

void gyroscope::gyro_temp_event_cb(sensors_event_t hal_event)
{
    _temp = hal_event.additional_info.data_float[0];
    send_additional_info(hal_event.timestamp);
}

void gyroscope::send_cal_req()
{
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    if (gyrocal_conn == nullptr) {
        sns_logd("cal connection closed, not sending request");
        return;
    }

    sns_logd("SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG");
    pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
    pb_req_msg.mutable_request()->clear_payload();
    pb_req_msg.mutable_suid()->set_suid_high(gyrocal_suid.high);
    pb_req_msg.mutable_suid()->set_suid_low(gyrocal_suid.low);

    /* If current calibration status is "UNRELIABLE", set wakeup flag to
     * ensure next valid calibration parameters are sent immediately. */
    pb_req_msg.mutable_request()->mutable_batching()->
        set_batch_period(SENSOR_STATUS_UNRELIABLE != gyroscope::sample_status
          ? UINT32_MAX : 0);
    pb_req_msg.mutable_request()->mutable_batching()->
        set_flush_period(UINT32_MAX);
    pb_req_msg.mutable_susp_config()->set_delivery_type(
        SENSOR_STATUS_UNRELIABLE != gyroscope::sample_status
          ? SNS_CLIENT_DELIVERY_NO_WAKEUP : SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->
        set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    gyrocal_conn->send_request(pb_req_msg_encoded);
}

void gyroscope::start_gyrocal()
{
    std::lock_guard<mutex> lock(cal_mutex);

    gyrocal_conn = new ssc_connection(gyroscope::gyrocal_conn_event_cb);
    if (gyrocal_conn)
        gyrocal_conn->use_report_indication();
    else
        sns_loge("ssc_connection failed for gyro cal");

    _gyrocal_worker =new worker;
     if (_gyrocal_worker)
        _gyrocal_worker->setname("gyrocal");
     else
         sns_loge("worker thread creation failed for gyro cal");

    send_cal_req();
}

void gyroscope::activate()
{
    if (!is_active()) {

        if (active_count == 0) {
            sns_logd("start gyrocal");
            start_gyrocal();
        }

        active_count++;

        if(_gyro_temp_available) {
            _gyrotemp = make_unique<additionalinfo_sensor>(_gyrotemp_suid,
                              SENSOR_NO_WAKEUP,[this] (sensors_event_t hal_event)
                {
                    gyro_temp_event_cb(hal_event);
                });
            sensor_params config_params = get_params();
            /* recommended rate is 1/1000 of master */
            config_params.sample_period_ns = get_params().sample_period_ns * 1000;
            _gyrotemp->set_config(config_params);
            sns_logd("activate the gyro temp sensor with rate: %lu latency %lu",
                    (unsigned long)config_params.sample_period_ns,
                    (unsigned long)config_params.max_latency_ns);
            _gyrotemp->activate();
        }
        ssc_sensor::activate();
    }
}

void gyroscope::deactivate()
{
    if (is_active()) {
        ssc_sensor::deactivate();
        active_count--;

        /* disable gyrotemp */
        if(_gyro_temp_available && _gyrotemp) {
            _gyrotemp->deactivate();
            _gyrotemp.reset();
        }

        if (active_count == 0) {
            /* close calibration connection */
            sns_logd("stop gyrocal");
            /* set the gyrocal_conn variable to null while protected by the
               mutex, this will make sure that any connection callbacks
               accessing this variable will not use it to send new requests */
            ssc_connection* temp;
            cal_mutex.lock();
            temp = gyrocal_conn;
            gyrocal_conn = nullptr;
            if (_gyrocal_worker) {
              delete _gyrocal_worker;
              _gyrocal_worker = nullptr;
            }
            cal_mutex.unlock();
            delete temp;
        }
    }
}

void gyroscope::send_additional_info(int64_t timestamp)
{
    sensors_event_t hal_event = create_sensor_hal_event(0);
    hal_event.type = SENSOR_TYPE_ADDITIONAL_INFO;
    sns_logd("gyro_temp_additional_info temp=%f, ts=%lld", _temp,
             (long long) timestamp);

    /* additional_info frame begin */
    hal_event.timestamp = timestamp;
    hal_event.additional_info.type = AINFO_BEGIN;
    submit_sensors_hal_event(hal_event);

    hal_event.timestamp++;
    hal_event.additional_info.type = AINFO_INTERNAL_TEMPERATURE;
    hal_event.additional_info.data_float[0] = _temp;
    submit_sensors_hal_event(hal_event);

    /* additional_info frame end */
    hal_event.timestamp++;
    hal_event.additional_info.type = AINFO_END;
    submit_sensors_hal_event(hal_event);
}

void gyroscope::on_flush_complete()
{
    if (_gyro_temp_available) {
        send_additional_info(android::elapsedRealtimeNano());
    }
}

void gyroscope::gyrocal_conn_event_cb(const uint8_t *data, size_t size)
{
    int8_t status;
    sns_client_event_msg pb_event_msg;
    pb_event_msg.ParseFromArray(data, size);

    lock_guard<mutex> lock(cal_mutex);
    status = gyroscope::sample_status;

    for (int i=0; i < pb_event_msg.events_size(); i++) {
        auto&& pb_event = pb_event_msg.events(i);
        sns_logv("event[%d] msg_id=%d", i, pb_event.msg_id());

        if (pb_event.msg_id() == SNS_CAL_MSGID_SNS_CAL_EVENT) {
            sns_cal_event pb_cal_event;

            pb_cal_event.ParseFromString(pb_event.payload());
            bias[0] = pb_cal_event.bias(0);
            bias[1] = pb_cal_event.bias(1);
            bias[2] = pb_cal_event.bias(2);
            sample_status =
                ssc_sensor::sensors_hal_sample_status(pb_cal_event.status());
            sns_logd("gyrocal bias=(%f, %f, %f) status=%d",
                     bias[0], bias[1], bias[2], sample_status);
        }
    }

    if((SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == status &&
        SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != gyroscope::sample_status) ||
       (SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != status &&
        SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == gyroscope::sample_status)) {
        if(_gyrocal_worker != nullptr) {
          _gyrocal_worker->add_task(NULL,[] {gyroscope::send_cal_req();});
        } else {
          gyroscope::send_cal_req();
        }
    }
}

void gyroscope::handle_sns_std_sensor_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_sensor_event;
    pb_sensor_event.ParseFromString(pb_event.payload());

    if(pb_sensor_event.data_size()) {
        sensors_event_t hal_event = create_sensor_hal_event(pb_event.timestamp());

        if (_cal == SENSOR_CALIBRATED) {
            lock_guard<mutex> lock(cal_mutex);
            hal_event.gyro.x = pb_sensor_event.data(0) - bias[0];
            hal_event.gyro.y = pb_sensor_event.data(1) - bias[1];
            hal_event.gyro.z = pb_sensor_event.data(2) - bias[2];
            hal_event.gyro.status = sample_status;
            sns_logd("gyro_sample_cal: ts=%lld ns; value = [%f, %f, %f], status = %d",
                     (long long) hal_event.timestamp,
                     hal_event.gyro.x, hal_event.gyro.y, hal_event.gyro.z,
                     hal_event.gyro.status);
        } else {
            lock_guard<mutex> lock(cal_mutex);
            hal_event.uncalibrated_gyro.x_uncalib = pb_sensor_event.data(0);
            hal_event.uncalibrated_gyro.y_uncalib = pb_sensor_event.data(1);
            hal_event.uncalibrated_gyro.z_uncalib = pb_sensor_event.data(2);
            hal_event.uncalibrated_gyro.x_bias = bias[0];
            hal_event.uncalibrated_gyro.y_bias = bias[1];
            hal_event.uncalibrated_gyro.z_bias = bias[2];

            sns_logd("gyro_sample_uncal: ts=%lld ns; value = [%f, %f, %f],"
                     " bias = [%f, %f, %f]", (long long) hal_event.timestamp,
                     hal_event.uncalibrated_gyro.x_uncalib,
                     hal_event.uncalibrated_gyro.y_uncalib,
                     hal_event.uncalibrated_gyro.z_uncalib,
                     hal_event.uncalibrated_gyro.x_bias,
                     hal_event.uncalibrated_gyro.y_bias,
                     hal_event.uncalibrated_gyro.z_bias);
        }
        submit_sensors_hal_event(hal_event);

        if (_gyro_temp_available && !_first_additional_info_sent) {
            send_additional_info(hal_event.timestamp);
            _first_additional_info_sent = true;
        }
    } else {
        sns_loge("empty data returned for gyro");
    }


}

bool gyroscope::is_calibrated()
{
  return (_cal == SENSOR_CALIBRATED) ? true : false;
}

/* create sensor variants supported by this class and register the function
   to sensor_factory */
static vector<unique_ptr<sensor>> get_available_gyroscopes_calibrated()
{
    const vector<sensor_uid>& gyro_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_GYRO);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : gyro_suids) {
        if (!(sensor_factory::instance().get_settings()
                                & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<gyroscope>(suid, SENSOR_WAKEUP,
                                                         SENSOR_CALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup, %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<gyroscope>(suid, SENSOR_NO_WAKEUP,
                                                     SENSOR_CALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

static vector<unique_ptr<sensor>> get_available_gyroscopes_uncalibrated()
{
    const vector<sensor_uid>& gyro_suids =
        sensor_factory::instance().get_suids(SSC_DATATYPE_GYRO);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : gyro_suids) {
        if (!(sensor_factory::instance().get_settings()
                                  & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<gyroscope>(suid, SENSOR_WAKEUP,
                                                         SENSOR_UNCALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup,  %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<gyroscope>(suid, SENSOR_NO_WAKEUP,
                                                     SENSOR_UNCALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

static bool gyroscope_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_GYROSCOPE,
                                    get_available_gyroscopes_calibrated);
    sensor_factory::register_sensor(SENSOR_TYPE_GYROSCOPE_UNCALIBRATED,
                                    get_available_gyroscopes_uncalibrated);
    sensor_factory::request_datatype(SSC_DATATYPE_GYRO);
    sensor_factory::request_datatype(SSC_DATATYPE_GYROCAL);
    sensor_factory::request_datatype(SSC_DATATYPE_TEMP);
    return true;
}

SENSOR_MODULE_INIT(gyroscope_module_init);
