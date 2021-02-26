//==============================================================================
// MODULE      : sns_bu52053nvx_sensor.c
// FUNCTION    : sensor function file
// AUTHOR      : Masafumi Seike
// PROGRAMMED  : Sensor Application Group, ROHM CO.,LTD.
// MAKING DATE : May/12/2017
// MODIFY DATE : AUG/10/2017
// REMARKS     :
// COPYRIGHT   : Copyright (C) 2018 - ROHM CO.,LTD.
//             : Redistribution and use in source and binary forms, with or
//             : without modification, are permitted provided that the
//             : following conditions are met:
//             : 1. Redistributions of source code must retain the above
//             : copyright notice, this list of conditions and the following
//             : disclaimer.
//             : 2. Redistributions in binary form must reproduce the above
//             : copyright notice, this list of conditions and the following
//             : disclaimer in the documentation and/or other materials
//             : provided with the distribution.
//             : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//             : contributors may be used to endorse or promote products
//             : derived from this software without specific prior written
//             : permission.
//             : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//             : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//             : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//             : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//             : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//             : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//             : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//             : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//             : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//             : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//             : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//             : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//             : SUCH DAMAGE.
//==============================================================================


//==============================================================================
//                              INCLUDE FILES
//==============================================================================
#include <string.h>
#include "sns_bu52053nvx_sensor.h"
#include "sns_mem_util.h"
#include "sns_types.h"
#include "sns_service_manager.h"
#include "sns_attribute_util.h"
#include "sns_pb_util.h"
#include "sns_sensor_util.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_stream_service.h"
#include "sns_printf.h"
#include "sns_hall.pb.h"
#include "pb_encode.h"
#include "sns_bu52053nvx_ver.h"
#include "sns_sensor_util.h"


//==============================================================================
//   function prototype
//==============================================================================
// sensor api funciton
static sns_rc               bu52053nvx_hall_init(sns_sensor *const this);
static sns_rc               bu52053nvx_hall_deinit(sns_sensor *const this);
static sns_sensor_uid const *bu52053nvx_hall_get_sensor_uid(sns_sensor const *const this);
static sns_sensor_instance  *bu52053nvx_set_client_request(sns_sensor *const this,
                                    struct sns_request const *exist_request,
                                    struct sns_request const *new_request, bool except);
static sns_rc               bu52053nvx_sensor_notify_event(sns_sensor *const this);

// internal use
static void bu52053nvx_publish_attributes(sns_sensor *const this);
static void bu52053nvx_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len);
static void bu52053nvx_process_suid_events(sns_sensor *const this);
static void bu52053nvx_publish_available(sns_sensor *const this);
static void bu52053nvx_instance_config(sns_sensor *this, sns_sensor_instance *instance);
static void bu52053nvx_power_rail_timer(sns_sensor *const this, sns_time timeout_ticks,
                                        BU52053NVX_POWER_RAIL_PENDING_STATE pwr_rail_pend_state);
static void bu52053nvx_turn_rails_on(sns_sensor *this, sns_time *rail_on_timestamp,
                                     BU52053NVX_POWER_RAIL_PENDING_STATE power_state);
static void bu52053nvx_turn_rails_off(sns_sensor *this, sns_time *rail_on_timestamp);
static void bu52053nvx_turn_rails_on_off(sns_sensor *this, sns_power_rail_state setting,
                                         sns_time *rail_on_timestamp);


//==============================================================================
// Global Data Definitions
//==============================================================================
sns_sensor_api bu52053nvx_sensor_api = {
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &bu52053nvx_hall_init,
    .deinit             = &bu52053nvx_hall_deinit,
    .get_sensor_uid     = &bu52053nvx_hall_get_sensor_uid,
    .set_client_request = &bu52053nvx_set_client_request,
    .notify_event       = &bu52053nvx_sensor_notify_event,
};


//==============================================================================
// NAME      : bu52053nvx_hall_init
// FUNCTION  : Initializes sensor function
// param[in] : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_hall_init(sns_sensor *const this)
{
    BU52053NVX_STATE    *state = (BU52053NVX_STATE *)this->state->state;
    sns_service_manager *smgr  = this->cb->get_service_manager(this);
    size_t temp;

    state->sensor                = BU52053NVX_HALL;
    state->sensor_client_present = false;
    state->diag_service          = (sns_diag_service *)smgr->get_service(smgr, SNS_DIAG_SERVICE);
    temp                         = 0;

    temp = sns_memscpy(&state->my_suid, sizeof(state->my_suid),
                       &((sns_sensor_uid)HALL_SUID), sizeof(sns_sensor_uid));
    if (temp == sizeof(sns_sensor_uid)) {
        bu52053nvx_publish_attributes(this);

        bu52053nvx_send_suid_req(this, "interrupt", sizeof("interrupt"));
        bu52053nvx_send_suid_req(this, "timer", sizeof("timer"));
    }

    SNS_PRINTF(HIGH, this, "DBG: HALL sensor initalized");

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : bu52053nvx_hall_deinit
// FUNCTION  : close sensor function
// param[in] : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_hall_deinit(sns_sensor *const this)
{
    BU52053NVX_STATE *state = (BU52053NVX_STATE*)this->state->state;

    sns_sensor_util_remove_sensor_stream(this, &state->fw_stream);
    sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : bu52053nvx_hall_get_sensor_uid
// FUNCTION  : return sensor uuid for hall
// param[in] : this : Sensor reference
// REMARKS   :
// RETURN    : pointer of sensor uid
//==============================================================================
static sns_sensor_uid const *bu52053nvx_hall_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = HALL_SUID;

    return &sensor_uid;
}

//==============================================================================
// NAME      : bu52053nvx_set_client_request
// FUNCTION  : ensor API common between all BU52053NVX
// param[in] : this          : Sensor reference
// param[in] : exist_request : existing request
// param[in] : new_request   : new request
// param[in] : except        : true to remove request
// REMARKS   :
// RETURN    : pointer of sns_sensor_instance
//==============================================================================
sns_sensor_instance *bu52053nvx_set_client_request(sns_sensor *const this,
            struct sns_request const *exist_request, struct sns_request const *new_request,
            bool except)
{
    sns_sensor_instance *instance    = sns_sensor_util_get_shared_instance(this);
    bool                reval_config = false;

    SNS_PRINTF(LOW, this, "set_client_req 0x%X  0x%X  %d", exist_request, new_request, except);

    if (except) {
        if (0 != instance) {
            instance->cb->remove_client_request(instance, exist_request);
            SNS_PRINTF(LOW, this, "Removed 0x%X", exist_request);
            // clear for sensor instance
            bu52053nvx_instance_config(this, instance);
        }
    } else {
        if (0 == instance) {
            instance = this->cb->create_instance(this, sizeof(BU52053NVX_INSTANCE_STATE));
            bu52053nvx_turn_rails_on(this, 0, POWER_RAIL_PENDING_SET_CLIENT_REQ);
        } else {
           if(NULL != exist_request && NULL != new_request &&
              new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ) {
              BU52053NVX_STATE *state = (BU52053NVX_STATE *)this->state->state;
              sns_sensor_util_send_flush_event(&state->my_suid, instance);
              return instance;
           } else {
            if ((0 != exist_request) && (0 != new_request)) {
                instance->cb->remove_client_request(instance, exist_request);
              }
           }
        }
        // Add the new request to list of client_requests.
        if (0 != instance) {
            BU52053NVX_INSTANCE_STATE *inst_state =
               (BU52053NVX_INSTANCE_STATE *)instance->state->state;
            if (0 != new_request) {
				reval_config = true;
                instance->cb->add_client_request(instance, new_request);
            }
            if ((reval_config) && (inst_state->instance_is_ready_to_configure)) {
                // clear for sensor instance
                bu52053nvx_instance_config(this, instance);
            }
        }
    }

    if ((0 != instance) &&
        (0 == instance->cb->get_client_request(instance, &(sns_sensor_uid)HALL_SUID, true))) {
        this->cb->remove_instance(instance);
        bu52053nvx_turn_rails_off(this, 0);
    }

    return (instance);
}


//==============================================================================
// NAME      : bu52053nvx_sensor_notify_event
// FUNCTION  : Sensor API common between all BU52053NVX Sensors.
// param[in] : this          : Sensor reference
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_sensor_notify_event(sns_sensor *const this)
{
    sns_sensor_event    *event       = 0;
    int                 temp         = 0;
    BU52053NVX_STATE    *state       = (BU52053NVX_STATE *)this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);

    if (state->fw_stream) {
        if ((0 == sns_memcmp(&state->irq_suid, &((sns_sensor_uid){{0}}), sizeof(state->irq_suid)))
            || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)))
            ) {
            bu52053nvx_process_suid_events(this);
        }
        else
        {
           sns_sensor_util_remove_sensor_stream(this, &state->fw_stream);
        }
    }

    //----------------------Handle a Timer Sensor event.-------------------
    if (0 != state->timer_stream) {
        event = state->timer_stream->api->peek_input(state->timer_stream);
        while (0 != event) {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event, event->event_len);
            sns_timer_sensor_event timer_event;
            if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) {
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                    if (state->power_rail_pend_state == POWER_RAIL_PENDING_INIT) {
                        SNS_PRINTF(MED, this, "POWER_RAIL_PENDING_INIT");
                        state->hw_is_present = true;

                        //----------------------Turn Power Rail OFF--------------------------
                        bu52053nvx_turn_rails_off(this, 0);
                        bu52053nvx_publish_available(this);

                        state->power_rail_pend_state = POWER_RAIL_PENDING_NONE;
                    } else if (state->power_rail_pend_state == POWER_RAIL_PENDING_SET_CLIENT_REQ) {
                        SNS_PRINTF(MED, this, "POWER_RAIL_PENDING_SET_CLIENT_REQ");
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
                        if (0 != instance) {
                            BU52053NVX_INSTANCE_STATE *inst_state =
                               (BU52053NVX_INSTANCE_STATE *) instance->state->state;
                            inst_state->instance_is_ready_to_configure = true;
                            bu52053nvx_instance_config(this, instance);
                        }
                        state->power_rail_pend_state = POWER_RAIL_PENDING_NONE;
                    } else {
                        // no process
                    }
                } else {
                    SNS_PRINTF(ERROR, this, "pb_decode error");
                }
            }
            event = state->timer_stream->api->get_next_input(state->timer_stream);
        }
    }

    //---------------------Register Power Rails --------------------------
    temp = sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid));
    if ((0 != temp) && (0 == state->pwr_rail_service)) {
        state->rail_config.rail_vote    = SNS_RAIL_OFF;
        state->rail_config.num_of_rails = NUM_OF_RAILS;
        strlcpy(state->rail_config.rails[0].name, RAIL_1,
                sizeof(state->rail_config.rails[0].name));
        state->pwr_rail_service =
           (sns_pwr_rail_service *)service_mgr->get_service(service_mgr, SNS_POWER_RAIL_SERVICE);
        state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service, &state->rail_config);

        // set interrupt
        state->irq_config.interrupt_num            = HALL_IRQ_NUM;
        state->irq_config.interrupt_trigger_type   = SNS_INTERRUPT_TRIGGER_TYPE_DUAL_EDGE;
        state->irq_config.interrupt_drive_strength = SNS_INTERRUPT_DRIVE_STRENGTH_2_MILLI_AMP;
        state->irq_config.interrupt_pull_type      = SNS_INTERRUPT_PULL_TYPE_NO_PULL;
        state->irq_config.is_chip_pin              = true;

        //---------------------Turn Power Rails ON----------------------------
        bu52053nvx_turn_rails_on(this, 0, POWER_RAIL_PENDING_INIT);
    }

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : bu52053nvx_publish_attributes
// FUNCTION  : Publish all Sensor attributes
// param[in] : this : Sensor reference
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_publish_attributes(sns_sensor *const this)
{
    BU52053NVX_STATE *state = (BU52053NVX_STATE *)this->state->state;
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};

        sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
        range1[0].has_flt                     = true;
        range1[0].flt                         = 0;
        range1[1].has_flt                     = true;
        range1[1].flt                         = 1;
        values[0].has_subtype                 = true;
        values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
        values[0].subtype.values.arg          = &((pb_buffer_arg)
                                                  { .buf = range1, .buf_len = ARR_SIZE(range1) });

        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES, values, ARR_SIZE(&values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, values, ARR_SIZE(&values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint     = DEVICE_ACTIVE_CURRENT;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT, values, ARR_SIZE(&values), false);
    }
    {
        sns_std_attr_value_data values[]   = {SNS_ATTR};
        char const              op_mode1[] = BU52053NVX_HIGH_PERF;

        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg          = &((pb_buffer_arg) { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
        char const              proto[] = "sns_hall.proto";

        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg          = &((pb_buffer_arg)
                                       { .buf = proto, .buf_len = sizeof(proto) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API, values, ARR_SIZE(&values), false);
    }
    {
        char const              name[] = "bu52053nvx";
        sns_std_attr_value_data value  = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg)
                                   { .buf = name, .buf_len = sizeof(name) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
    }
    {
        char const              type[] = "hall";
        sns_std_attr_value_data value  = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg)
                                   { .buf = type, .buf_len = sizeof(type) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
    }
    {
        char const              vendor[] = "ROHM";
        sns_std_attr_value_data value    = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg)
                                   { .buf = vendor, .buf_len = sizeof(vendor) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = false;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = false;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = true;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = false;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
    }

    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = BU52053NVX_DRIVER_VER;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = 0;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = 0;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = DEVICE_ACTIVE_CURRENT; //uA
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = true;
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, false);
    }
    {
        sns_std_attr_value_data value        = sns_std_attr_value_data_init_default;
        sns_hall_event          stream_event = sns_hall_event_init_default;
        pb_get_encoded_size(&state->encoded_event_len, sns_hall_event_fields, &stream_event);
        value.has_sint = true;
        value.sint     = state->encoded_event_len;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, true);
    }

    return;
}

//==============================================================================
// NAME      : bu52053nvx_send_suid_req
// FUNCTION  : Sends a request to the SUID Sensor to get SUID of a dependent Sensor.
// param[in] : this          : Sensor reference
// param[in] : data_type     : data_type of dependent Sensor
// param[in] : data_type_len : Length of the data_type string
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len)
{
    uint8_t             buffer[50];
    size_t              encoded_len;
    BU52053NVX_STATE    *state          = (BU52053NVX_STATE *)this->state->state;
    sns_service_manager *manager        = this->cb->get_service_manager(this);
    sns_stream_service  *stream_service = (sns_stream_service *)manager->get_service(manager, SNS_STREAM_SERVICE);
    pb_buffer_arg       data            = (pb_buffer_arg){ .buf = data_type, .buf_len = data_type_len };

    sns_memset(buffer, 0, sizeof(buffer));
    sns_suid_req suid_req = sns_suid_req_init_default;
    suid_req.has_register_updates   = true;
    suid_req.register_updates       = true;
    suid_req.data_type.funcs.encode = &pb_encode_string_cb;
    suid_req.data_type.arg          = &data;

    if (state->fw_stream == 0) {
        stream_service->api->create_sensor_stream(stream_service, this, sns_get_suid_lookup(), &state->fw_stream);
    }

    encoded_len = pb_encode_request(buffer, sizeof(buffer), &suid_req, sns_suid_req_fields, 0);
    if (0 < encoded_len) {
        sns_request request = (sns_request){.request_len = encoded_len, .request = buffer, .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
        state->fw_stream->api->send_request(state->fw_stream, &request);
    }

    return;
}

//==============================================================================
// NAME      : bu52053nvx_process_suid_events
// FUNCTION  : Processes events from SUID Sensor.
// param[in] : this : Sensor reference
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_process_suid_events(sns_sensor *const this)
{
    BU52053NVX_STATE *state = (BU52053NVX_STATE *)this->state->state;

    for (; 0 != state->fw_stream->api->get_input_cnt(state->fw_stream); state->fw_stream->api->get_next_input(state->fw_stream)) {
        sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);

        if (SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id) {
            sns_sensor_uid  uid_list;
            sns_suid_search suid_search;
            pb_istream_t    stream        = pb_istream_from_buffer((void *)event->event, event->event_len);
            sns_suid_event  suid_event    = sns_suid_event_init_default;
            pb_buffer_arg   data_type_arg = { .buf = 0, .buf_len = 0 };
            suid_search.suid         = &uid_list;
            suid_search.num_of_suids = 0;

            suid_event.data_type.funcs.decode = &pb_decode_string_cb;
            suid_event.data_type.arg          = &data_type_arg;
            suid_event.suid.funcs.decode      = &pb_decode_suid_event;
            suid_event.suid.arg               = &suid_search;

            if (!pb_decode(&stream, sns_suid_event_fields, &suid_event)) {
                SNS_PRINTF(ERROR, this, "SUID Decode failed");
                continue;
            }

            // if no suids found, ignore the event
            if (suid_search.num_of_suids == 0) {
                continue;
            }

            // save suid based on incoming data type name
            if (0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len)) {
                state->irq_suid = uid_list;
            } else if (0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len)) {
                state->timer_suid = uid_list;
            } else {
                // if system execute this process and you need to do process, you should write program here.
            }
        }
    }

    return;
}

//==============================================================================
// NAME      : bu52053nvx_publish_available
// FUNCTION  : This function parses the client_request list per Sensor and
//           : determines final config for the Sensor Instance.
// param[in] : this        : Sensor reference
// param[in] : instance    : Sensor Instance to config
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_publish_available(sns_sensor *const this)
{

    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean     = true;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);

    return;
}

//==============================================================================
// NAME      : bu52053nvx_instance_config
// FUNCTION  : call set_client_config
// param[in] : this        : Sensor reference
// param[in] : instance    : Sensor Instance to config
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_instance_config(sns_sensor *this, sns_sensor_instance *instance)
{
    sns_request               config;

    SNS_PRINTF(LOW, this, "bu52053nvx_instance_config");

    // set instants contig
    config.message_id  = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
    config.request_len = 0;
    this->instance_api->set_client_config(instance, &config);

    return;
}

//==============================================================================
// NAME      : bu52053nvx_power_rail_timer
// FUNCTION  : power on system block
// param[in] : this                : Sensor reference
// param[in] : timeout_ticks       : system time on crrent
// REMARKS   : pwr_rail_pend_state :
// RETURN    :
//==============================================================================
static void bu52053nvx_power_rail_timer(sns_sensor *const this, sns_time timeout_ticks,
                                        BU52053NVX_POWER_RAIL_PENDING_STATE pwr_rail_pend_state)
{
    size_t                  req_len;
    uint8_t                 buffer[20];
    BU52053NVX_STATE        *state      = (BU52053NVX_STATE *)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;

    sns_memset(buffer, 0, sizeof(buffer));
    req_len                    = 0;
    req_payload.is_periodic    = false;
    req_payload.start_time     = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload, sns_timer_sensor_config_fields, 0);
    if (req_len > 0) {
        sns_request timer_req = {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG, .request = buffer, .request_len = req_len  };
        state->timer_stream->api->send_request(state->timer_stream, &timer_req);
        state->power_rail_pend_state = pwr_rail_pend_state;
    } else {
    }

    return;
}

//==============================================================================
// NAME      : bu52053nvx_turn_rails_on
// FUNCTION  : power on system block
// param[in] : this : Sensor reference
// param[in] : time : system time on crrent
// param[in] : state : power rail state
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_turn_rails_on(sns_sensor *this, sns_time *rail_on_timestamp,
                                     BU52053NVX_POWER_RAIL_PENDING_STATE power_state)
{
    BU52053NVX_STATE *state = (BU52053NVX_STATE*)this->state->state;

    SNS_PRINTF(LOW, this, "bu52053nvx_turn_rails_on  %d", power_state);

    bu52053nvx_turn_rails_on_off(this, SNS_RAIL_ON_NPM, rail_on_timestamp);

    //-------------Create a Timer stream for Power Rail ON timeout.---------
    if (0 == state->timer_stream) {
        sns_service_manager *smgr = this->cb->get_service_manager(this);
        sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
        stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid, &state->timer_stream);
    }
    if (0 != state->timer_stream) {
        sns_time            temp2        = 0;
        temp2 = sns_convert_ns_to_ticks(BU52053NVX_OFF_TO_IDLE_MS * 1000 * 1000);
        bu52053nvx_power_rail_timer(this, temp2, power_state);
        state->power_rail_pend_state = power_state;
    }
    return;
}

//==============================================================================
// NAME      : bu52053nvx_turn_rails_off
// FUNCTION  : power off system block
// param[in] : this : Sensor reference
// param[in] : time : system time on crrent
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_turn_rails_off(sns_sensor *this, sns_time *rail_on_timestamp)
{
    bu52053nvx_turn_rails_on_off(this, SNS_RAIL_OFF, rail_on_timestamp);

    return;
}

//==============================================================================
// * NAME      : bu52053nvx_turn_rails_on_off
// * FUNCTION  : power off/on system block
// * param[in] : this                 : Sensor reference
// * param[in] : sns_power_rail_state : power rail state
// * param[in] : time                 : system time on crrent
// * REMARKS   : This function should only call bu52053nvx_turn_rails_on and
// *           : bu52053nvx_turn_rails_off
// * RETURN    :
//==============================================================================
static void bu52053nvx_turn_rails_on_off(sns_sensor *this,
                      sns_power_rail_state setting, sns_time *rail_on_timestamp)
{
    BU52053NVX_STATE *state = (BU52053NVX_STATE *)this->state->state;

    if (state->rail_config.rail_vote != setting) {
        state->rail_config.rail_vote = setting;
        state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                           this, &state->rail_config, rail_on_timestamp);
    }

    return;
}


