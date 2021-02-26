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
#include "worker.h"

using namespace std;

static const char *SSC_DATATYPE_MAG = "mag";
static const char *SSC_DATATYPE_MAGCAL = "mag_cal";
static const uint32_t MAG_RESERVED_FIFO_COUNT = 600;

/**
 * @brief class implementing both calibrated and uncalibrated
 *        magnetometer sensors
 *
 */
class magnetometer : public ssc_sensor
{
public:

    magnetometer(sensor_uid mag_suid, sensor_wakeup_type wakeup,
                 sensor_cal_type calibrated);
    virtual bool is_calibrated() override;
private:

    virtual void activate() override;

    virtual void deactivate() override;

    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;

    void start_magcal();

    sensor_cal_type _cal;

    static int active_count;
    static bool magcal_available;
    static ssc_connection* magcal_conn;
    static sensor_uid magcal_suid;
    static bool magcal_initialized;
    static std::mutex cal_mutex;
    static float bias[3];
    /* status based on calibration accuracy */
    static int8_t sample_status;

    static void send_cal_req();
    static void magcal_conn_event_cb(const uint8_t *data, size_t size);
    static worker *_magcal_worker;

};

/* static member variables */
int magnetometer::active_count = 0;
ssc_connection* magnetometer::magcal_conn;
sensor_uid magnetometer::magcal_suid;
bool magnetometer::magcal_initialized = false;
std::mutex magnetometer::cal_mutex;
float magnetometer::bias[3] = {0};
int8_t magnetometer::sample_status = SENSOR_STATUS_UNRELIABLE;
bool magnetometer::magcal_available = false;
worker *magnetometer::_magcal_worker;

magnetometer::magnetometer(sensor_uid suid, sensor_wakeup_type wakeup,
                     sensor_cal_type calibrated):
    ssc_sensor(suid, wakeup),
    _cal(calibrated)
{
    if (_cal == SENSOR_CALIBRATED) {
        set_type(SENSOR_TYPE_MAGNETIC_FIELD);
        set_string_type(SENSOR_STRING_TYPE_MAGNETIC_FIELD);
        set_sensor_typename("Magnetometer");
    } else {
        set_type(SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
        set_string_type(SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
        set_sensor_typename("Magnetometer-Uncalibrated");
    }
#ifdef SNS_DIRECT_REPORT_SUPPORT
    if (check_direct_channel_support()) {
        set_direct_channel_flag(true);
    }
#endif

    if (!magcal_initialized) {
        const auto& magcal_suids =
            sensor_factory::instance().get_suids(SSC_DATATYPE_MAGCAL);
        if (magcal_suids.size() == 0) {
            sns_loge("magcal is not available");
        } else {
            /* get first magcal suid (only one expected) */
            magcal_suid = magcal_suids[0];
            magcal_available = true;
        }
        magcal_initialized = true;
    }

    set_fifo_reserved_count(MAG_RESERVED_FIFO_COUNT);
    set_resampling(true);
    set_nowk_msgid(SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT);
    set_nowk_msgid(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT);
    _magcal_worker = nullptr;
}

void magnetometer::send_cal_req()
{
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    if (magcal_conn == nullptr) {
        sns_logd("magcal_conn is closed, not sending request");
        return;
    }

    sns_logd("SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG");
    pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
    pb_req_msg.mutable_request()->clear_payload();
    pb_req_msg.mutable_suid()->set_suid_high(magcal_suid.high);
    pb_req_msg.mutable_suid()->set_suid_low(magcal_suid.low);

    /* If current calibration status is "UNRELIABLE", set wakeup flag to
     * ensure next valid calibration parameters are sent immediately. */
    pb_req_msg.mutable_request()->mutable_batching()->
        set_batch_period(SENSOR_STATUS_UNRELIABLE != magnetometer::sample_status
          ? UINT32_MAX : 0);
    pb_req_msg.mutable_request()->mutable_batching()->
        set_flush_period(UINT32_MAX);
    pb_req_msg.mutable_susp_config()->set_delivery_type(
        SENSOR_STATUS_UNRELIABLE != magnetometer::sample_status
          ? SNS_CLIENT_DELIVERY_NO_WAKEUP : SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->
        set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    magcal_conn->send_request(pb_req_msg_encoded);
}

void magnetometer::start_magcal()
{
    /* create a new ssc connection for magcal */
    std::lock_guard<mutex> lock(cal_mutex);

    magcal_conn = new ssc_connection(magnetometer::magcal_conn_event_cb);
    if (magcal_conn)
        magcal_conn->use_report_indication();
    else
        sns_loge("ssc_connection failed for mag cal");

    _magcal_worker =new worker;
     if (_magcal_worker)
        _magcal_worker->setname("magcal");
     else
        sns_loge("worker thread creation failed for mag cal");

    send_cal_req();
}

void magnetometer::activate()
{
    if (!is_active()) {
        if (active_count == 0 && magcal_available) {
            sns_logd("start magcal");
            start_magcal();
        }
        active_count++;
        ssc_sensor::activate();
    }
}

void magnetometer::deactivate()
{
    if (is_active()) {
        ssc_sensor::deactivate();
        active_count--;
        if (active_count == 0 && magcal_available) {
            /* close calibration connection */
            sns_logd("stop magcal");
            /* set the magcal_conn variable to null while protected by the
               mutex, this will make sure that any connection callbacks
               accessing this variable will not use it to send new requests */
            ssc_connection* temp;
            cal_mutex.lock();
            temp = magcal_conn;
            magcal_conn = nullptr;
            if (nullptr != _magcal_worker) {
              delete _magcal_worker;
              _magcal_worker = nullptr;
            }
            cal_mutex.unlock();
            delete temp;
        }
    }

}

void magnetometer::magcal_conn_event_cb(const uint8_t *data, size_t size)
{
    int8_t status;
    sns_client_event_msg pb_event_msg;
    pb_event_msg.ParseFromArray(data, size);

    lock_guard<mutex> lock(cal_mutex);
    status = magnetometer::sample_status;

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

            sns_logd("magcal bias=(%f, %f, %f) status=%d",
                     bias[0], bias[1], bias[2], sample_status);
        }
    }

    if((SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == status &&
        SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != magnetometer::sample_status) ||
       (SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE != status &&
        SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == magnetometer::sample_status)) {
      if(_magcal_worker != nullptr){
        _magcal_worker->add_task(NULL,[] {magnetometer::send_cal_req();});
      } else {
        magnetometer::send_cal_req();
      }
    }
}

void magnetometer::handle_sns_std_sensor_event(
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_sensor_event pb_sensor_event;
    pb_sensor_event.ParseFromString(pb_event.payload());

    if(pb_sensor_event.data_size()) {
        sensors_event_t hal_event = create_sensor_hal_event(pb_event.timestamp());

        if (_cal == SENSOR_CALIBRATED) {
            lock_guard<mutex> lock(cal_mutex);
            hal_event.magnetic.x = pb_sensor_event.data(0) - bias[0];
            hal_event.magnetic.y = pb_sensor_event.data(1) - bias[1];
            hal_event.magnetic.z = pb_sensor_event.data(2) - bias[2];
            hal_event.magnetic.status = sample_status;
            sns_logd("mag_sample_cal: ts=%lld ns; value = [%f, %f, %f], status=%d",
                     (long long) hal_event.timestamp, hal_event.magnetic.x,
                     hal_event.magnetic.y, hal_event.magnetic.z,
                     hal_event.magnetic.status);
        } else {
            lock_guard<mutex> lock(cal_mutex);
            hal_event.uncalibrated_magnetic.x_uncalib = pb_sensor_event.data(0);
            hal_event.uncalibrated_magnetic.y_uncalib = pb_sensor_event.data(1);
            hal_event.uncalibrated_magnetic.z_uncalib = pb_sensor_event.data(2);
            hal_event.uncalibrated_magnetic.x_bias = bias[0];
            hal_event.uncalibrated_magnetic.y_bias = bias[1];
            hal_event.uncalibrated_magnetic.z_bias = bias[2];

            sns_logd("mag_sample_uncal: ts=%lld ns; value = [%f, %f, %f],"
                     " bias = [%f, %f, %f]", (long long) hal_event.timestamp,
                     hal_event.uncalibrated_magnetic.x_uncalib,
                     hal_event.uncalibrated_magnetic.y_uncalib,
                     hal_event.uncalibrated_magnetic.z_uncalib,
                     hal_event.uncalibrated_magnetic.x_bias,
                     hal_event.uncalibrated_magnetic.y_bias,
                     hal_event.uncalibrated_magnetic.z_bias);
        }
        submit_sensors_hal_event(hal_event);
    }else {
        sns_loge("empty data returned for mag");
    }
}

bool magnetometer::is_calibrated()
{
  return (_cal == SENSOR_CALIBRATED) ? true : false;
}

/* create all variants for calibrated mag sensor */
static vector<unique_ptr<sensor>> get_available_magnetometers_calibrated()
{
    const vector<sensor_uid>& mag_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_MAG);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : mag_suids) {
        if (!(sensor_factory::instance().get_settings()
                                        & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<magnetometer>(suid, SENSOR_WAKEUP,
                                                            SENSOR_CALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup, %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<magnetometer>(suid, SENSOR_NO_WAKEUP,
                                                        SENSOR_CALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

/* create all variants for uncalibrated mag sensor */
static vector<unique_ptr<sensor>> get_available_magnetometers_uncalibrated()
{
    const vector<sensor_uid>& mag_suids =
        sensor_factory::instance().get_suids(SSC_DATATYPE_MAG);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : mag_suids) {
        if (!(sensor_factory::instance().get_settings()
                                        & DISABLE_WAKEUP_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<magnetometer>(suid, SENSOR_WAKEUP,
                                                            SENSOR_UNCALIBRATED));
            } catch (const exception& e) {
                sns_loge("failed for wakeup,  %s", e.what());
            }
        }
        try {
            sensors.push_back(make_unique<magnetometer>(suid, SENSOR_NO_WAKEUP,
                                                        SENSOR_UNCALIBRATED));
        } catch (const exception& e) {
            sns_loge("failed for nowakeup, %s", e.what());
        }
    }
    return sensors;
}

static bool magnetometer_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_MAGNETIC_FIELD,
                                    get_available_magnetometers_calibrated);
    sensor_factory::register_sensor(SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
                                    get_available_magnetometers_uncalibrated);
    sensor_factory::request_datatype(SSC_DATATYPE_MAG);
    sensor_factory::request_datatype(SSC_DATATYPE_MAGCAL);
    return true;
}

SENSOR_MODULE_INIT(magnetometer_module_init);
