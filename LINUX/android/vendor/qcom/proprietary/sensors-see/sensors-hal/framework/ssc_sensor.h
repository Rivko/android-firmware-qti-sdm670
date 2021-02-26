/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once
#include <vector>
#include <mutex>
#include "sensor.h"
#include "ssc_utils.h"
#include "ssc_connection.h"
#include "sns_client.pb.h"
#include "sensors_log.h"
#include "sensor_attributes.h"
#include "sensor_factory.h"
#include "sensors_hal_common.h"
#include "sensor_diag_log.h"
/**
 * @brief Abstract class of all sensors which are implemented on
 *        SSC.
 *
 *  This class provides common implementation for activating,
 *  deactivating and configuring sensors. This also contains
 *  logic for handling wakeup/nowakeup and batching
 *  functionality.
 */
class ssc_sensor : public sensor
{
public:
    /**
     * @brief creates an ssc_sensor object for given SUID and wakeup
     *        mode
     *
     * @param suid suid of the ssc sensor
     * @param wakeup wakeup mode
     */
    ssc_sensor(sensor_uid suid, sensor_wakeup_type wakeup);

    virtual ~ssc_sensor() = default;

    /* see sensor::activate */
    virtual void activate() override;

    /* see sensor::deactivate */
    virtual void deactivate() override;

    /* see sensor::update_config */
    virtual void update_config() override;

    /* see sensor::flush */
    virtual void flush() override;

    /* see sensor::is_active */
    virtual bool is_active() override { return (_ssc_conn != nullptr); }

    /**
     * @brief get calibration mode
     * @return bool true if calibration mode is SENSOR_CALIBRATED
     *                false otherwrise.
     */
    virtual bool is_calibrated() override { return false; }

    /**
     * @brief get SUID of this sensor
     * @return const sensor_uid
     */
    virtual const sensor_uid get_sensor_suid() override { return _suid; }
    /**
     * @brief generic function template for creating ssc_sensors of
     *        a given typename S
     */
    template<typename S>
    static std::vector<std::unique_ptr<sensor>> get_available_sensors()
    {
        const char *datatype = S::ssc_datatype();
        const std::vector<sensor_uid>& suids =
             sensor_factory::instance().get_suids(datatype);
        std::vector<std::unique_ptr<sensor>> sensors;
        for (const auto& suid : suids) {
            if (!(sensor_factory::instance().get_settings()
                                    & DISABLE_WAKEUP_SENSORS_FLAG)) {
              try {
                  sensors.push_back(std::make_unique<S>(suid, SENSOR_WAKEUP));
              } catch (const std::exception& e) {
                  sns_loge("%s", e.what());
                  sns_loge("failed to create sensor %s, wakeup=true", datatype);
              }
            }
            try {
                sensors.push_back(std::make_unique<S>(suid, SENSOR_NO_WAKEUP));
            } catch (const std::exception& e) {
                sns_loge("%s", e.what());
                sns_loge("failed to create sensor %s, wakeup=false", datatype);
            }
        }
        return sensors;
    }

    /**
     * @brief generic function template for creating ssc_sensors of
     *        a given typename S
     */
    template<typename S>
    static std::vector<std::unique_ptr<sensor>> get_available_wakeup_sensors()
    {
        const char *datatype = S::ssc_datatype();
        const std::vector<sensor_uid>& suids =
             sensor_factory::instance().get_suids(datatype);
        std::vector<std::unique_ptr<sensor>> sensors;
        for (const auto& suid : suids) {
            try {
                sensors.push_back(std::make_unique<S>(suid, SENSOR_WAKEUP));
            } catch (const std::exception& e) {
                sns_loge("%s", e.what());
                sns_loge("failed to create sensor %s, wakeup=true", datatype);
            }
        }
        return sensors;
    }

protected:

    /**
     * @brief create a config request message for this sensor
     * @return sns_client_request_msg
     */
    virtual sns_client_request_msg create_sensor_config_request();

    /**
     * @brief handle all sensor-related events from ssc, default
     *        implementation is for std_sensor.proto
     *
     * @param pb_event
     */
    virtual void handle_sns_client_event(
        const sns_client_event_msg_sns_client_event& pb_event);

    /**
     * @brief handle the std_sensor event, default implementation
     *        copies the data from sensor event and submits event to
     *        sensor_hal
     * @param pb_event
     */
    virtual void handle_sns_std_sensor_event(
        const sns_client_event_msg_sns_client_event& pb_event);

    /**
     * @brief handler function that will be called after a
     *        METADATA_FLUSH_COMPLETE event is sent. Derived sensor
     *        classes can use this as a notification for flush
     *        completion
     */
    virtual void on_flush_complete() { }

    /**
     * @brief returns SUID of this sensor
     * @return const sensor_uid&
     */
    const sensor_uid& suid() const { return _suid; }

    /**
     * @brief returns the datatype of this sensor
     * @return const std::string&
     */
    const std::string& datatype() const { return _datatype; }

    /**
     * @brief returns an object storing attributes of this sensor
     * @return const sensor_attributes&
     */
    const sensor_attributes& attributes() const { return _attributes; }

    /**
     * @brief creates a blank sensor_event_t for given ssc qtimer
     *        timestamp
     * @param ssc_timestamp timestamp in qtimer ticks
     * @return sensors_event_t
     */
    sensors_event_t create_sensor_hal_event(uint64_t ssc_timestamp);

    sns_client_delivery get_delivery_type() const
    {
        if ((get_sensor_info().flags & SENSOR_FLAG_WAKE_UP) != 0) {
            return SNS_CLIENT_DELIVERY_WAKEUP;
        } else {
            return SNS_CLIENT_DELIVERY_NO_WAKEUP;
        }
    }

    static int8_t sensors_hal_sample_status(sns_std_sensor_sample_status std_status);

    /**
     * @brief set if resampler is used for this sensor
     * @param val
     */
    void set_resampling(bool val);

    /**
     * @brief set the sensor name with optional type parameter
     * @param type_name
     */
    void set_sensor_typename(const string& type_name = "");

    /**
     * @brief Adds a message id into the no wakeup message id list
     * maintained for the sensor.
     * @param msg_id
     */
    void set_nowk_msgid(const int msg_id);

private:

    /* set sensor information based on attributes and config */
    void set_sensor_info();

    /* callback function for all ssc_connection events */
    void ssc_conn_event_cb(const uint8_t *data, size_t size);

    void ssc_conn_error_cb(ssc_error_type e);
    void ssc_conn_resp_cb(uint32_t resp_value);
    /* event handler for ssc flush event */
    void handle_sns_std_flush_event(
        const sns_client_event_msg_sns_client_event& pb_event);

    /* Update the list of nowk message ids in the request message */
    void add_nowk_msgid_list(sns_client_request_msg &req_msg);

    sns_client_request_msg create_resampler_config_request();

    /* create and send config request to ssc */
    void send_sensor_config_request();

    /* get suid of resampler sensor */
    sensor_uid get_resampler_suid();

    /* lookup attributes for this sensor */
    const sensor_attributes& lookup_attributes() const
    {
        return sensor_factory::instance().get_attributes(_suid);
    }

    /* suid of this sensor */
    sensor_uid _suid;

    /* datatype of this sensor */
    std::string _datatype;

    /* attributes of this sensor */
    const sensor_attributes& _attributes;

    /* suid of resampler sensor */
    sensor_uid _resampler_suid;

    /* flag for resampler usage */
    bool _resampling = false;

    /* max delay supported by sensor */
    int32_t _sensor_max_delay;

    /* max delay supported by resampler */
    const uint32_t RESAMPLER_MAX_DELAY = USEC_PER_SEC;

    /* connection to SSC */
    std::unique_ptr<ssc_connection> _ssc_conn;

    /* number of flush requests for which a flush event is not generated */
    std::atomic<uint32_t> _pending_flush_requests;

    sensor_wakeup_type _wakeup_type;

    /* interface to diag logging library */
    sensors_diag_log _diag;

    /* mutex for serializing api calls */
    std::mutex _mutex;

    /*used to restart sensors after SSR*/
    static const uint8_t RETRY_CNT = 60;

    /*used to update the wakeup sensor string*/
    std::string _sensor_name;

    /* Maintains the list of no wakeup message ids */
    std::vector<int> _nowk_msg_id_list;
};
