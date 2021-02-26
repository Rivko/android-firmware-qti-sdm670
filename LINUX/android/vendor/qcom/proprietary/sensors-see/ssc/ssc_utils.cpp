/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string>
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "sns_client.pb.h"
#include "ssc_connection.h"
#include "sensors_log.h"
#include "ssc_utils.h"
#include <cinttypes>
#include <cmath>
#include <chrono>
#include <cutils/properties.h>
#include <fstream>
#include <string>

using namespace std;
using namespace google::protobuf::io;

using namespace std::chrono;

static const char *SLPI_SSR_SYSFS_PATH
            = "sys/kernel/boot_slpi/ssr";
static const char * SNS_SETTINGS_FILE
            = "/persist/sensors/sensors_settings";

const char SENSORS_HAL_PROP_DIRECT_CHANNEL[] = "persist.vendor.sensors.direct_channel";

suid_lookup::suid_lookup(suid_event_function cb):
    _cb(cb),
    _ssc_conn(get_ssc_event_cb())
{
}

void suid_lookup::request_suid(std::string datatype)
{
    sns_client_request_msg pb_req_msg;
    sns_suid_req pb_suid_req;
    string pb_suid_req_encoded;
    // TODO: add this as an optional parameter to the function
    bool default_only = false;

    const sensor_uid LOOKUP_SUID = {
        12370169555311111083ull,
        12370169555311111083ull
    };

    sns_logv("requesting suid for %s, ts = %fs", datatype.c_str(),
             duration_cast<duration<float>>(high_resolution_clock::now().
                                            time_since_epoch()).count());

    /* populate SUID request */
    pb_suid_req.set_data_type(datatype);
    pb_suid_req.set_register_updates(true);
    if(true == support_default_sensor()) {
      pb_suid_req.set_default_only(default_only);
    }
    pb_suid_req.SerializeToString(&pb_suid_req_encoded);

    /* populate the client request message */
    pb_req_msg.set_msg_id(SNS_SUID_MSGID_SNS_SUID_REQ);
    pb_req_msg.mutable_request()->set_payload(pb_suid_req_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(LOOKUP_SUID.high);
    pb_req_msg.mutable_suid()->set_suid_low(LOOKUP_SUID.low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    string pb_req_msg_encoded;
    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    _ssc_conn.send_request(pb_req_msg_encoded);
}

void suid_lookup::handle_ssc_event(const uint8_t *data, size_t size)
{
    /* parse the pb encoded event */
    sns_client_event_msg pb_event_msg;
    pb_event_msg.ParseFromArray(data, size);
    /* iterate over all events in the message */
    for (int i = 0; i < pb_event_msg.events_size(); i++) {
        auto& pb_event = pb_event_msg.events(i);
        if (pb_event.msg_id() != SNS_SUID_MSGID_SNS_SUID_EVENT) {
            sns_loge("invalid event msg_id=%d", pb_event.msg_id());
            continue;
        }
        sns_suid_event pb_suid_event;
        pb_suid_event.ParseFromString(pb_event.payload());
        const string& datatype =  pb_suid_event.data_type();

        sns_logv("suid_event for %s, num_suids=%d, ts=%fs", datatype.c_str(),
                 pb_suid_event.suid_size(),
                 duration_cast<duration<float>>(high_resolution_clock::now().
                                                time_since_epoch()).count());

        /* create a list of  all suids found for this datatype */
        vector<sensor_uid> suids(pb_suid_event.suid_size());
        for (int j=0; j < pb_suid_event.suid_size(); j++) {
            suids[j] = sensor_uid(pb_suid_event.suid(j).suid_low(),
                                  pb_suid_event.suid(j).suid_high());
        }
        /* send callback for this datatype */
        _cb(datatype, suids);
    }
}

const char SENSORS_HAL_SSR_SUPPORT[] = "persist.vendor.sensors.hal_trigger_ssr";
bool support_ssr_trigger()
{
    char ssr_prop[PROPERTY_VALUE_MAX];
    property_get(SENSORS_HAL_SSR_SUPPORT, ssr_prop, "false");
    sns_logd("ssr_prop: %s",ssr_prop);
    if (!strncmp("true", ssr_prop, 4)) {
        sns_logi("support_ssr_trigger: %s",ssr_prop);
        return true;
    }
    return false;
}

/* utility function to trigger ssr*/
/*  all deamons/system_app do not have permissions to open
     sys/kernel/boot_slpi/ssr , right now only hal_sensors can do it*/
int trigger_ssr() {
    if(support_ssr_trigger() == true) {
        int _fd = open(SLPI_SSR_SYSFS_PATH, O_WRONLY);
        if (_fd<0) {
            sns_logd("failed to open sys/kernel/boot_slpi/ssr");
            return -1;
        }
        if (write(_fd, "1", 1) > 0) {
           sns_loge("ssr triggered successfully");
           /*allow atleast some time before connecting after ssr*/
           sleep(2);
           return 0;
        } else {
            sns_loge("failed to write sys/kernel/boot_slpi/ssr");
            perror("Error: ");
                return -1;
        }
    } else {
        sns_logd("trigger_ssr not supported");
        return -1;
    }
}

/*utility to read the settings related to wakeup in
/perist/registry/registry/sensor_settings*/
uint32_t get_sns_settings() {
    uint32_t settings = 0x0;
    std::ifstream file(SNS_SETTINGS_FILE);
    std::string str;
    /*right now first line but make it generic to add or remove any algo*/
    while (std::getline(file, str))
    {
        sns_logd("sensors_settings line: %s", str.c_str());
        if ( !(settings & DISABLE_SENSORS_FLAG) &&
                    !(str.compare(DISABLE_SENSORS_STRING)))
            settings |= DISABLE_SENSORS_FLAG;

        if ( !(settings & DISABLE_WAKEUP_SENSORS_FLAG) &&
                    !(str.compare(DISABLE_WAKEUP_SENSORS_STRING)))
            settings |= DISABLE_WAKEUP_SENSORS_FLAG;

        if ( !(settings & DISABLE_PROXIMITY_SENSORS_FLAG) &&
                    !(str.compare(DISABLE_PROXIMITY_SENSORS_STRING)))
            settings |= DISABLE_PROXIMITY_SENSORS_FLAG;

        if ( !(settings & SNS_DIAG_CIRC_BUFF_MODE_FLAG) &&
                    !(str.compare(SNS_DAIG_CIRC_BUFF_MODE_STRING)))
            settings |= SNS_DIAG_CIRC_BUFF_MODE_FLAG;

    }
    sns_logi("settings %d", settings);
    return settings;
}

bool check_direct_channel_support()
{
    char prop_val[PROPERTY_VALUE_MAX] = {0};
#ifdef ADSPRPC
    property_get(SENSORS_HAL_PROP_DIRECT_CHANNEL, prop_val, "false");
#else
    property_get(SENSORS_HAL_PROP_DIRECT_CHANNEL, prop_val, "true");
#endif
    if (!strncmp("true", prop_val, 4)) {
        sns_logi("direct channel enabled");
        return true;
    }
    else
        return false;
}
const char SENSORS_DEFAULT_SENSOR_SUPPORT[] = 
"persist.vendor.sensors.default_sensor";
bool support_default_sensor()
{
    char prop_val[PROPERTY_VALUE_MAX] = {0};

    property_get(SENSORS_DEFAULT_SENSOR_SUPPORT, prop_val, "true");
    if (!strncmp("true", prop_val, 4)) {
        sns_logi("default sensor support mode is enabled");
        return true;
    }
    else
        return false;
}

