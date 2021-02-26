/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <thread>
#include <chrono>
#include "sensors_log.h"
#include "sensor_factory.h"
#include "sensors_hal_common.h"
#include <cutils/properties.h>
#include <time.h>
#include <sys/stat.h>

using namespace std;

static const auto MSEC_PER_SEC = 1000;

/* timeout duration for discovery of ssc sensors for first bootup */
static const auto SENSOR_DISCOVERY_TIMEOUT_FIRST = 15;
/* timeout duration for discovery of ssc sensors for all bootups */
static const auto SENSOR_DISCOVERY_TIMEOUT_REST = 3;

/* additional wait time for discovery of critical sensors */
static const int MANDATORY_SENSOR_WAIT_TIME_SEC = 15;

static vector<string> mandatory_sensor_datatypes;

static std::mutex mandatory_sensors_mutex;
static map<string, bool> mandatory_sensors_map;
static size_t num_mandatory_sensors_found = 0;

static atomic<bool> all_mandatory_sensors_found { false };

/* timeout duration for receiving attributes for a sensor */
static const uint64_t SENSOR_GET_ATTR_TIMEOUT_NS = 3000000000;
static const int GET_ATTR_NUM_ITERS = 100;

#define HAL_PROP_SIMULATE_SSCTRIGGER "vendor.slpi.ssrsimulate"

const char MANDATORY_SENSORS_LIST_FILE[] = "/persist/sensors/sensors_list.txt";

/*following properties are accessble from HAL service only right now*/
bool ssr_simulate() {
    char ssr_trigger[PROPERTY_VALUE_MAX] = "false";
    property_get( HAL_PROP_SIMULATE_SSCTRIGGER, ssr_trigger, "false" );
    if (!strncmp(ssr_trigger, "true", 4)) {
        /* reset to false so that simulation happens only once */
        property_set( HAL_PROP_SIMULATE_SSCTRIGGER, "false");
        return true;
    }
    return false;
}

uint32_t sensor_factory::get_discovery_timeout_ms()
{
    if (_laterboot) {
        return SENSOR_DISCOVERY_TIMEOUT_REST * MSEC_PER_SEC;
    } else {
        return SENSOR_DISCOVERY_TIMEOUT_FIRST * MSEC_PER_SEC;
    }
}

void sensor_factory::init_mandatory_list_db()
{
    struct stat buf;
    _laterboot = (stat(MANDATORY_SENSORS_LIST_FILE, &buf) == 0);

   if (!_laterboot) {
       sns_logi("first boot after flash");
   }
   if(_laterboot){
       _read.open(MANDATORY_SENSORS_LIST_FILE, std::ofstream::in);
        if (_read.fail()) {
           sns_loge("open fail for sensors_list reading");
           return;
       }

       std::string str;
       while (std::getline(_read, str)){
           mandatory_sensor_datatypes.push_back(str);
       }
       _write.open(MANDATORY_SENSORS_LIST_FILE, std::ofstream::app);
        if (_write.fail()) {
           sns_loge("open fail for sensors_list");
           return;
    }
    } else {
        _write.open(MANDATORY_SENSORS_LIST_FILE, std::ofstream::out);
         if (_write.fail()) {
            sns_loge("open fail for sensors_list");
            return;
        }
    }
}

void sensor_factory::deinit_mandatory_list_db()
{
    if(_laterboot){
        _write.close();
        _read.close();
    }
    else{
        _write.close();
    }
}

void sensor_factory::update_mandatory_list_database(std::string datatype)
{
    if(_laterboot){
        if(!(std::find(mandatory_sensor_datatypes.begin() , mandatory_sensor_datatypes.end() , datatype) !=
         mandatory_sensor_datatypes.end())){
            _write << datatype + "\n";
           sns_logi("laterboot datatype %s write", datatype.c_str());
        }
    } else {
        if(!(std::find(mandatory_sensor_datatypes.begin() , mandatory_sensor_datatypes.end() , datatype) !=
         mandatory_sensor_datatypes.end())){
            _write << datatype + "\n";
            num_mandatory_sensors_found++;
            mandatory_sensor_datatypes.push_back(datatype);
        }
    }
}
sensor_factory::sensor_factory()
{
    _settings = get_sns_settings();
    _pending_attributes = 0;
    init_mandatory_list_db();

    if(_laterboot){
        for (const string& s : mandatory_sensor_datatypes) {
            mandatory_sensors_map.emplace(s, 0);
        }
    }

    if (!(_settings & DISABLE_SENSORS_FLAG)) {

        /* find available sensors on ssc */
        discover_sensors();

        if (_suid_map.size() > 0) {
            retrieve_attributes();
        }
        /*it enables to check for ssr triggering from HAL*/
        if (ssr_simulate()) {
            sns_logd("simulated slpi ssr  ");
            auto no_sensors = _suid_map.size();
            if (!trigger_ssr()) {
                discover_sensors();
                if (_suid_map.size() > 0) {
                    retrieve_attributes();
                }
                sns_loge("number of sensors before ssr: %lu",
                                (unsigned long)no_sensors);
                sns_loge("number of sensors after  ssr: %lu",
                                (unsigned long)_suid_map.size());
            } else {
                sns_loge("ssr trigger failed");
            }
        }
    }else {
     sns_logi("disabled sensors from sns_settings");
    }
    deinit_mandatory_list_db();
}

vector<unique_ptr<sensor>> sensor_factory::get_all_available_sensors() const
{
    vector<unique_ptr<sensor>> all_sensors;
    for (const auto& item : callbacks()) {
        const auto& get_sensors = item.second;
        vector<unique_ptr<sensor>> sensors = get_sensors();
        sns_logd("type=%d, num_sensors=%u", item.first, (unsigned int)sensors.size());
        for (auto&& s : sensors) {
            all_sensors.push_back(std::move(s));
        }
    }
    return all_sensors;
}

void sensor_factory::suid_lookup_callback(const string& datatype,
                                          const vector<sensor_uid>& suids)
{
    using namespace std::chrono;
    if (suids.size() > 0) {
        lock_guard<mutex> lock(mandatory_sensors_mutex);
        _suid_map[datatype] = suids;
        update_mandatory_list_database(datatype);
        if(_laterboot)
        {
            auto it = mandatory_sensors_map.find(datatype);
            if (it != mandatory_sensors_map.end() && it->second == false) {
                it->second = true;
                num_mandatory_sensors_found++;
                if (num_mandatory_sensors_found ==
                    mandatory_sensor_datatypes.size()) {
                    all_mandatory_sensors_found.store(true);
                }
            }
        }
        _tp_last_suid = steady_clock::now();
        sns_logv("received suid for dt=%s, t=%fs", datatype.c_str(),
                 duration_cast<duration<double>>(
                     _tp_last_suid.time_since_epoch()).count());
    }
}


void sensor_factory::wait_for_sensors(suid_lookup& lookup)
{
    int count = MANDATORY_SENSOR_WAIT_TIME_SEC/2;
    while (count) {
        sns_logi("SEE not ready yet wait count %d", count);
        {
            lock_guard<mutex> lock(mandatory_sensors_mutex);
            for (auto& dt : datatypes()) {
                if (_suid_map.find(dt) == _suid_map.end()) {
                    sns_logd("re-sending request for %s", dt.c_str());
                    lookup.request_suid(dt);
                }
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(2*MSEC_PER_SEC));
        count--;
        {
            lock_guard<mutex> lock(mandatory_sensors_mutex);
            if (num_mandatory_sensors_found) {
                sns_logi("sensors found after re discover %d", count);
                break;
            }
            if ((count == 0) && (num_mandatory_sensors_found == 0)) {
                sns_loge("sensors list is 0 after first boot ");
                break;
            }
        }
    }
}

void sensor_factory::wait_for_mandatory_sensors(suid_lookup& lookup)
{
    int count = MANDATORY_SENSOR_WAIT_TIME_SEC;
    while (all_mandatory_sensors_found.load() == false) {
        sns_logi("some mandatory sensors not available yet, "
                 "will wait for %d seconds...", count);
        {
            lock_guard<mutex> lock(mandatory_sensors_mutex);
            for (auto p : mandatory_sensors_map) {
                if (p.second == false) {
                    sns_logd("non available ones %s", p.first.c_str());
                }
            }
            for (auto& dt : datatypes()) {
                if (_suid_map.find(dt) == _suid_map.end()) {
                    sns_logd("re-sending request for %s", dt.c_str());
                    lookup.request_suid(dt);
                }
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(MSEC_PER_SEC));
        count--;

        {
            lock_guard<mutex> lock(mandatory_sensors_mutex);
            if (count == 0 && all_mandatory_sensors_found.load() == false) {
                sns_loge("some mandatory sensors not available even after %d "
                         "seconds, giving up.", MANDATORY_SENSOR_WAIT_TIME_SEC);
                sns_loge("%lu missing sensor(s)", (unsigned long)
                         mandatory_sensor_datatypes.size() - num_mandatory_sensors_found);
                for (auto p : mandatory_sensors_map) {
                    if (p.second == false) {
                        sns_loge("    %s", p.first.c_str());
                    }
                }
                break;
            }
        }
    }
}

void sensor_factory::discover_sensors()
{
    using namespace std::chrono;

    sns_logd("discovery start t=%fs", duration_cast<duration<double>>(
             steady_clock::now().time_since_epoch()).count());

    suid_lookup lookup(
        [this](const string& datatype, const auto& suids)
        {
            suid_lookup_callback(datatype, suids);
        });

    sns_logd("discovering available sensors...");

    for (const string& dt : datatypes()) {
        sns_logd("requesting %s", dt.c_str());
        lookup.request_suid(dt);
    }
    auto tp_wait_start = steady_clock::now();

    /* wait for some time for discovery of available sensors */
    auto delay = get_discovery_timeout_ms();

    sns_logv("before sleep, now=%f", duration_cast<duration<double>>(
             steady_clock::now().time_since_epoch()).count());

    sns_logi("waiting for suids for %us ...", (delay/MSEC_PER_SEC));

    this_thread::sleep_for(std::chrono::milliseconds(delay));

    sns_logv("after sleep, now=%f", duration_cast<duration<double>>(
             steady_clock::now().time_since_epoch()).count());

    /* additional wait for discovery of critical sensors */
    if (_laterboot) {
        wait_for_mandatory_sensors(lookup);
    } else if (0 == num_mandatory_sensors_found) {
        sns_loge("first boot generated 0 sensors so re-discover");
        wait_for_sensors(lookup);
    }
    sns_logd("available sensors on ssc");
    for (const auto& item : _suid_map) {
        sns_logd("%-20s%4u", item.first.c_str(), (unsigned int)item.second.size());
    }

    sns_logi("suid discovery time = %fs",
             duration_cast<duration<double>>(_tp_last_suid - tp_wait_start)
             .count());
}

sns_client_request_msg sensor_factory::create_attr_request(sensor_uid suid)
{
    sns_client_request_msg pb_req_msg;
    sns_std_attr_req pb_attr_req;
    string pb_attr_req_encoded;
    /* populate the pb_attr_req message */
    pb_attr_req.set_register_updates(false);
    pb_attr_req.SerializeToString(&pb_attr_req_encoded);
    /* populate the client request message */
    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    pb_req_msg.mutable_request()->set_payload(pb_attr_req_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(suid.high);
    pb_req_msg.mutable_suid()->set_suid_low(suid.low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    return pb_req_msg;
}

void sensor_factory::retrieve_attributes()
{
    ssc_connection ssc_conn([this](const uint8_t* data, size_t size)
    {
        sns_client_event_msg pb_event_msg;
        pb_event_msg.ParseFromArray(data, size);
        for (int i=0; i < pb_event_msg.events_size(); i++) {
            auto&& pb_event = pb_event_msg.events(i);
            sns_logv("event[%d] msg_id=%d", i, pb_event.msg_id());
            if (pb_event.msg_id() ==  SNS_STD_MSGID_SNS_STD_ATTR_EVENT) {
                sensor_uid suid (pb_event_msg.suid().suid_low(),
                                 pb_event_msg.suid().suid_high());
                handle_attribute_event(suid, pb_event);
            }
        }
    });

    /* send attribute request for all suids */
    for (const auto& item : _suid_map) {
        const auto& datatype = item.first;
        const auto& suids = item.second;
        int count=0;
        for (const sensor_uid& suid : suids) {
            _attr_mutex.lock();
            _pending_attributes++;
            _attr_mutex.unlock();
            string pb_attr_req_encoded;
            sns_logd("requesting attributes for %s-%d", datatype.c_str(),
                     count++);
            create_attr_request(suid).SerializeToString(&pb_attr_req_encoded);
            ssc_conn.send_request(pb_attr_req_encoded);
        }
    }

    sns_logd("waiting for attributes...");

    /* wait until attributes are received */
    int count = GET_ATTR_NUM_ITERS;
    uint64_t delay = SENSOR_GET_ATTR_TIMEOUT_NS / GET_ATTR_NUM_ITERS;
    struct timespec ts;
    int err;
    ts.tv_sec = delay / NSEC_PER_SEC;
    ts.tv_nsec = delay % NSEC_PER_SEC;

    unique_lock<mutex> lk(_attr_mutex);
    while (count > 0) {
        if (_pending_attributes == 0) {
            break;
        }
        /* unlock the mutex while sleeping */
        lk.unlock();
        do {
            err = clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
        } while (err == EINTR);
        lk.lock();
        if (err != 0) {
            sns_loge("clock_nanosleep() failed, err=%d, count=%d", err, count);
        }
        count--;
    }

    if (_pending_attributes > 0) {
        sns_loge("timeout while waiting for attributes, pending = %d, err = %d",
              _pending_attributes, err);
        return;
    }

    sns_logd("attributes received");
}

void sensor_factory::handle_attribute_event(sensor_uid suid,
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_attr_event pb_attr_event;
    pb_attr_event.ParseFromString(pb_event.payload());

    sensor_attributes attr;

    for (int i=0; i<pb_attr_event.attributes_size(); i++) {
        attr.set(pb_attr_event.attributes(i).attr_id(),
                       pb_attr_event.attributes(i).value());
    }
    _attributes[suid] = attr;

    /* notify the thread waiting for attributes */
    unique_lock<mutex> lock(_attr_mutex);
    if (_pending_attributes > 0) {
        _pending_attributes--;
    }
}

int sensor_factory::get_pairedsuid(
                               const std::string& datatype,
                               const sensor_uid &master_suid,
                               sensor_uid &paired_suid)
{
    /*get the list of suids */
    std::vector<sensor_uid>  suids = get_suids(datatype);
    const string master_suid_vendor = _attributes[master_suid].get_string(SNS_STD_SENSOR_ATTRID_VENDOR);
    const string master_suid_name   = _attributes[master_suid].get_string(SNS_STD_SENSOR_ATTRID_NAME);
    const string master_type = _attributes[master_suid].get_string(SNS_STD_SENSOR_ATTRID_TYPE);
    int master_suid_hwid = -1;
    bool match = false;
    if (_attributes[master_suid].is_present(SNS_STD_SENSOR_ATTRID_HW_ID)) {
        master_suid_hwid = _attributes[master_suid].get_ints(SNS_STD_SENSOR_ATTRID_HW_ID)[0];
    }

    sns_logd("to be paired::datatype: %s, vendor: %s , name:%s, hwid %d",
                datatype.c_str(), master_suid_vendor.c_str(),
                master_suid_name.c_str(), master_suid_hwid);

    /*get attributes of specified data type suid & return if matches*/
    for (std::vector<sensor_uid>::iterator it = suids.begin(); it != suids.end(); ++it) {
        sns_logv("for pairing.., parsed vendor:%s, name:%s ",
                    _attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_VENDOR).c_str(),
                    _attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_NAME).c_str());

        if ((_attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_VENDOR)
                 == master_suid_vendor ) &&
                 (_attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_NAME)
                 ==(master_suid_name))) {
            match = true;
            /*HW_ID is optional field so check if it is present only*/
            if ( master_suid_hwid != -1 && _attributes[*it].is_present(SNS_STD_SENSOR_ATTRID_HW_ID)){
                if ( master_suid_hwid != _attributes[*it].get_ints(SNS_STD_SENSOR_ATTRID_HW_ID)[0]) {
                    match = false;
                    continue;
                }
            }
            if( match == true) {
                paired_suid = *it;
                sns_logd("datatype: %s, paired with:type:%s,name:%s,vendor:%s",
                            datatype.c_str(), master_type.c_str(),
                            _attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_NAME).c_str(),
                            _attributes[*it].get_string(SNS_STD_SENSOR_ATTRID_VENDOR).c_str());
                break;
            }
        }
    }

    if(match == false) {
       sns_loge("no data type mapping b/w master/paired %s/%s",
                                        master_type.c_str(), datatype.c_str());
       return -1;
    }

    return 0;
}

const std::vector<sensor_uid>& sensor_factory::get_suids(const std::string& datatype) const
{
    auto it = _suid_map.find(datatype);
    if (it != _suid_map.end()) {
        return it->second;
    } else {
        static vector<sensor_uid> empty;
        return empty;
    }
}
