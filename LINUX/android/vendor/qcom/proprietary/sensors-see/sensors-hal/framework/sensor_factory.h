/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <vector>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "ssc_utils.h"
#include "sns_client.pb.h"
#include "sensor_attributes.h"
#include "sensor.h"
#include "sensors_log.h"
#include <sstream>
#include <fstream>

using get_available_sensors_func =
    std::function<std::vector<std::unique_ptr<sensor>>()>;

/**
 * @brief class to provide hashing operation on sensor_uid to
 *        use sensor_uid as a key in unordered_map
 *
 */
struct sensor_uid_hash
{
    std::size_t operator()(const sensor_uid& suid) const
    {
        std::string data(reinterpret_cast<const char*>(&suid), sizeof(sensor_uid));
        return std::hash<std::string>()(data);
    }
};

/**
 * @brief singleton class for creating sensor objects of
 *        multiple types, querying information such as SUIDs,
 *        attributes. Classes that implement sensors are
 *        registered here.
 */
class sensor_factory
{
public:

    /**
     * @brief get handle to sensor_factory instance
     * @return sensor_factory&
     */
    static sensor_factory& instance()
    {
        static sensor_factory factory;
        return factory;
    }

    /**
     * @brief register a new sensor type to be supported
     *
     * @param type sensor type as defined in sensors.h
     * @param func factory function for creating sensors of this
     *             type
     *
     */
    static void register_sensor(int type, get_available_sensors_func func)
    {
        try {
            callbacks().emplace(type, func);
        } catch (const std::exception& e) {
            sns_loge("failed to register type %d", type);
        }
    }

    /**
     * @brief get list of suids for given datatype
     * @param datatype
     * @return const std::vector<sensor_uid>& list of suids
     */
    const std::vector<sensor_uid>& get_suids(const std::string& datatype) const;


    /**
     * @brief get mapped/secondary sensor suid for a specified sensor type and hardware ID
     * @param suid  and datatype
     * @return const sensor_suid
     */
     int get_pairedsuid( const std::string& datatype,
                                      const sensor_uid &paired_suid,
                                      sensor_uid &mapped_suid);

    /**
     * @brief get sensor attributes for given suid
     * @param suid
     * @return const sensor_attributes&
     */
    const sensor_attributes& get_attributes(const sensor_uid& suid) const
    {
        auto it = _attributes.find(suid);
        if (it == _attributes.end()) {
            throw std::runtime_error("get_attributes() unrecognized suid");
        }
        return it->second;
    }

    /**
     * @brief request a datatype to be queried when factory is
     *        initialized
     *
     * @param datatype
     */
    static void request_datatype(const char *datatype)
    {
        try {
            datatypes().insert(std::string(datatype));
        } catch (const std::exception& e) {
            sns_loge("failed to insert %s", datatype);
        }
    }

    std::vector<std::unique_ptr<sensor>> get_all_available_sensors() const;

    uint32_t get_settings() {
      return _settings;
    }
private:

    /* singleton operation */
    sensor_factory();
    sensor_factory (const sensor_factory&) = delete;
    sensor_factory& operator= (const sensor_factory&) = delete;

    /* discover available suids from static list of datatypes */
    void discover_sensors();

    /* query and save attributes for all available suids */
    void retrieve_attributes();

    sns_client_request_msg create_attr_request(sensor_uid suid);

    void handle_attribute_event(
        sensor_uid suid, const sns_client_event_msg_sns_client_event& pb_event);

    /* map of sensor suids for each datatype */
    std::unordered_map<std::string, std::vector<sensor_uid>> _suid_map;

    /* map of sensor attributes for each suid */
    std::unordered_map<sensor_uid, sensor_attributes, sensor_uid_hash> _attributes;

    /* mutex and condition variables to synchronize attribute query */
    std::mutex _attr_mutex;

    /* pending attribute requests */
    uint32_t _pending_attributes;

    std::chrono::steady_clock::time_point _tp_last_suid;

    /* set of all ssc sensor datatypes */
    static std::unordered_set<std::string>& datatypes()
    {
        static std::unordered_set<std::string> _datatypes;
        return _datatypes;
    }

    /* map of factory callback functions for each sensor type */
    static std::unordered_map<int, get_available_sensors_func>& callbacks()
    {
        static std::unordered_map<int, get_available_sensors_func> _callbacks;
        return _callbacks;
    }

    uint32_t get_discovery_timeout_ms();

    /* waits for a longer amount of time for some critical sensor types to
       become available */
    void wait_for_mandatory_sensors(suid_lookup& lookup);

    /* callback function to handle suid events */
    void suid_lookup_callback(const std::string& datatype,
                              const std::vector<sensor_uid>& suids);

    uint32_t _settings;
    /*_laterboot is used to convery whether current bootup is first bootup or not*/
    bool _laterboot;

    /*_write & _read is used to update/read the sensors_list during bootup*/
    std::ofstream _write;
    std::ifstream _read;

    /*Functions to initlation /deinit the mandatory lit from sensors_list.txt */
    void init_mandatory_list_db();
    void deinit_mandatory_list_db();

    /**
     * @brief is used to update the mandatory list if new sensor found
     *
     * @param datatype: sensor name
     */
    void update_mandatory_list_database(std::string datatype);

    /* waits for a longer amount of time for in first boot if sensor list is 0
           if suid sensor is not available then none of the sensors are
           available so wait more time for first boot*/
    void wait_for_sensors(suid_lookup& lookup);
};
