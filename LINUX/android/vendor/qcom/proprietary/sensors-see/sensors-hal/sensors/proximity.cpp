/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_factory.h"
#include "ssc_sensor.h"
#include "sns_proximity.pb.h"

using namespace std;

static const char *SSC_DATATYPE_PROXIMITY = "proximity";
static const uint32_t PROX_RESERVED_FIFO_COUNT = 300;

using namespace std;

class proximity : public ssc_sensor
{
public:
    proximity(sensor_uid suid, sensor_wakeup_type wakeup);
    static const char* ssc_datatype() { return SSC_DATATYPE_PROXIMITY; }

private:
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event) override;
};

proximity::proximity(sensor_uid suid, sensor_wakeup_type wakeup):
    ssc_sensor(suid, wakeup)
{
    set_type(SENSOR_TYPE_PROXIMITY);
    set_string_type(SENSOR_STRING_TYPE_PROXIMITY);
    set_fifo_reserved_count(PROX_RESERVED_FIFO_COUNT);
    set_sensor_typename("Proximity Sensor");
}

void proximity::handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event)
{
    if (pb_event.msg_id() == SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT) {
        sns_proximity_event pb_prox_event;
        pb_prox_event.ParseFromString(pb_event.payload());

        sensors_event_t hal_event =
             create_sensor_hal_event(pb_event.timestamp());

        if (pb_prox_event.proximity_event_type() ==
                SNS_PROXIMITY_EVENT_TYPE_NEAR) {
            hal_event.distance = 0.0f;
        } else {
            hal_event.distance = get_sensor_info().maxRange;
        }
        submit_sensors_hal_event(hal_event);

        sns_logv("prox_event: ts=%llu, raw_adc=%lu, near_far=%d, distance=%f",
                 (unsigned long long)hal_event.timestamp,
                 (unsigned long)pb_prox_event.raw_adc(),
                 pb_prox_event.proximity_event_type(),
                 hal_event.distance);
    }
}

/*Proximity is default wakeup sensor , so create it*/
static vector<unique_ptr<sensor>> get_available_prox_sensors()
{
    const vector<sensor_uid>& prox_suids =
         sensor_factory::instance().get_suids(SSC_DATATYPE_PROXIMITY);
    vector<unique_ptr<sensor>> sensors;
    for (const auto& suid : prox_suids) {
        const sensor_attributes& attr =
            sensor_factory::instance().get_attributes(suid);
        /* publish only on-change proximity sensor */
        if (attr.get_stream_type() != SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) {
            continue;
        }
        try {
            sensors.push_back(make_unique<proximity>(suid, SENSOR_WAKEUP));
        } catch (const exception& e) {
            sns_loge("failed for wakeup, %s", e.what());
        }
        if (!(sensor_factory::instance().get_settings()
                        & DISABLE_PROXIMITY_SENSORS_FLAG)) {
            try {
                sensors.push_back(make_unique<proximity>(suid, SENSOR_NO_WAKEUP));
            } catch (const exception& e) {
                sns_loge("failed for nowakeup, %s", e.what());
            }
        }
    }

    return sensors;
}
static bool proximity_module_init()
{
    /* register supported sensor types with factory */
    sensor_factory::register_sensor(SENSOR_TYPE_PROXIMITY,
                                    get_available_prox_sensors);
    sensor_factory::request_datatype(SSC_DATATYPE_PROXIMITY);
    return true;
}

SENSOR_MODULE_INIT(proximity_module_init);
