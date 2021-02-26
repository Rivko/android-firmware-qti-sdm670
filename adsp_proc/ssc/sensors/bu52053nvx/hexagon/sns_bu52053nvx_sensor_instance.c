//==============================================================================
// MODULE      : sns_bu52053nvx_sensor_instance.c
// FUNCTION    : BU52053NVX Hall virtual Sensor Instance implementation.
// AUTHOR      : Masafumi Seike
// PROGRAMMED  : Sensor Application Group, ROHM CO.,LTD.
// MAKING DATE : May/12/2017
// MODIFY DATE : Jun/28/2017
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
#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"
#include "sns_hall.pb.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_diag.pb.h"
#include "sns_bu52053nvx_sensor.h"
#include "sns_bu52053nvx_sensor_instance.h"
#include "sns_printf.h"
#include "sns_gpio_service.h"
#include "sns_sensor_util.h"

//==============================================================================
//   function prototype
//==============================================================================
// sensor api funciton
static sns_rc bu52053nvx_inst_init(sns_sensor_instance *const this,
                                   sns_sensor_state const *sstate);
static sns_rc bu52053nvx_inst_deinit(sns_sensor_instance *const this);
static sns_rc bu52053nvx_inst_set_client_config(sns_sensor_instance *const this,
                                                sns_request const *client_request);
static sns_rc bu52053nvx_inst_notify_event(sns_sensor_instance *const this);

// internal use
static void inst_cleanup(sns_sensor_instance *const this);
static void bu52053nvx_send_data(sns_sensor_instance *const instance, sns_time ts);
static sns_rc bu52053nvx_send_config_event(sns_sensor_instance *const this);
static sns_rc bu52053nvx_read_gpio(sns_sensor_instance *instance, uint32_t gpio,
                                   bool is_chip_pin, sns_gpio_state* val);


//==============================================================================
// Global Data Definitions
//==============================================================================
// See sns_sensor_instance_api::set_client_config
sns_sensor_instance_api bu52053nvx_sensor_instance_api = {
    .struct_len        = sizeof(sns_sensor_instance_api),
    .init              = &bu52053nvx_inst_init,
    .deinit            = &bu52053nvx_inst_deinit,
    .set_client_config = &bu52053nvx_inst_set_client_config,
    .notify_event      = &bu52053nvx_inst_notify_event
};


//==============================================================================
// NAME      : bu52053nvx_inst_init
// FUNCTION  : Initializes sensor instance function
// param[in] : this : Sensor instance reference
// REMARKS   :
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate)
{
    sns_rc                    result;
    BU52053NVX_INSTANCE_STATE *state        = (BU52053NVX_INSTANCE_STATE *)this->state->state;
    BU52053NVX_STATE          *sensor_state = (BU52053NVX_STATE *)sstate->state;
    sns_service_manager       *service_mgr  = this->cb->get_service_manager(this);
    sns_stream_service        *stream_mgr   =
       (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    state->diag_service =
       (sns_diag_service *) service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

    SNS_INST_PRINTF(LOW, this, "bu52053nvx_inst_init");

    //---------Setup stream connections with dependent Sensors---------
    stream_mgr->api->create_sensor_instance_stream(stream_mgr, this,
                               sensor_state->irq_suid, &state->interrupt_data_stream);
    if (0 == state->interrupt_data_stream) {
        inst_cleanup(this);
        SNS_INST_PRINTF(ERROR, this, "Can't create sensor instance.");

        result = SNS_RC_FAILED;
    } else {
        //----------- Copy all Sensor UIDs in instance state -------------
        sns_memscpy(&state->hall_info.suid, sizeof(state->hall_info.suid),
                    &((sns_sensor_uid)HALL_SUID), sizeof(state->hall_info.suid));

        // Initialize IRQ info to be used by the Instance
        state->irq_info = sensor_state->irq_config;
        state->irq_is_reqistered = false;
        result = SNS_RC_SUCCESS;
    }

    return (result);
}

//==============================================================================
// NAME      : bu52053nvx_inst_deinit
// FUNCTION  : Removed sensor_state parameter
// param[in] : this : Sensor instance reference
// REMARKS   :
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_inst_deinit(sns_sensor_instance *const this)
{
    inst_cleanup(this);
    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : bu52053nvx_inst_set_client_config
// FUNCTION  : Set parameter of system and IC
// param[in] : client_request : Sensor instance reference
// REMARKS   : In this case it is no process
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
static sns_rc bu52053nvx_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request)
{
    BU52053NVX_INSTANCE_STATE *state = (BU52053NVX_INSTANCE_STATE*)this->state->state;
    SNS_INST_PRINTF(HIGH, this, "call client config.");

    if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
       if(!state->irq_is_reqistered)
       {
           uint8_t         buffer[20];
           sns_data_stream *data_stream = state->interrupt_data_stream;
           sns_request     irq_req      =
           { .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ, .request = buffer};

           irq_req.request_len =
              pb_encode_request(buffer, sizeof(buffer), &state->irq_info, sns_interrupt_req_fields, 0);
           if (irq_req.request_len > 0) {
               data_stream->api->send_request(data_stream, &irq_req);
               state->irq_is_reqistered = true;
           }
       }
       //raising physical config event
       if (SNS_RC_SUCCESS != bu52053nvx_send_config_event(this)) {
           SNS_INST_PRINTF(ERROR, this, "Physical config event failed for HALL sensor ");
       }
	   
	   if(!state->first_event_sent){
		   bu52053nvx_send_data(this, sns_get_system_time());
	   }
	   else{
		   sns_hall_event hall_event        = sns_hall_event_init_default;
		   hall_event.status          = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
		   hall_event.hall_event_type = state->prev_event.hall_event_type;
		   pb_send_event(this, sns_hall_event_fields, &hall_event,
                              sns_get_system_time(), SNS_HALL_MSGID_SNS_HALL_EVENT, &state->hall_info.suid);
	   }
    }

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : bu52053nvx_inst_notify_event
// FUNCTION  : update sensor data to application
// param[in] : this : Sensor instance reference
// REMARKS   :
// RETURN    : SNS_DDF_SUCCESS
//==============================================================================
// See sns_sensor_instance_api::notify_event
static sns_rc bu52053nvx_inst_notify_event(sns_sensor_instance *const this)
{
    sns_sensor_event          *event;
    BU52053NVX_INSTANCE_STATE *state = (BU52053NVX_INSTANCE_STATE *)this->state->state;

    // Handle interrupts
    if (0 != state->interrupt_data_stream) {
        event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
        while (0 != event) {
            if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT) {
                // non process
                SNS_INST_PRINTF(LOW, this, "SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT");
            } else if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT) {
                SNS_INST_PRINTF(LOW, this, "SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT");
                bu52053nvx_send_data(this, event->timestamp);
            } else {
                SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d", event->message_id);
            }
            event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
        }
    }

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : inst_cleanup
// FUNCTION  : clear instance state
// REMARKS   :
// RETURN    :
//==============================================================================
static void inst_cleanup(sns_sensor_instance *const this)
{
    BU52053NVX_INSTANCE_STATE *state = (BU52053NVX_INSTANCE_STATE*)this->state->state;
    sns_sensor_util_remove_sensor_instance_stream(this,
                                                  &state->interrupt_data_stream);
    return;
}

//==============================================================================
// NAME      : bu52053nvx_send_data
// FUNCTION  : send sensor data to application
// REMARKS   :
// RETURN    :
//==============================================================================
static void bu52053nvx_send_data(sns_sensor_instance *const this, sns_time ts)
{
    sns_rc         result;
    sns_gpio_state data;

    sns_hall_event hall_event        = sns_hall_event_init_default;
    BU52053NVX_INSTANCE_STATE *state = (BU52053NVX_INSTANCE_STATE*)this->state->state;


    result = bu52053nvx_read_gpio(this, state->irq_info.interrupt_num, state->irq_info.is_chip_pin, &data);
    if(result == SNS_RC_SUCCESS) {

        hall_event.status          = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

        if(data == SNS_GPIO_STATE_LOW) {
           hall_event.hall_event_type = SNS_HALL_EVENT_TYPE_NEAR;
        } else
        {
           hall_event.hall_event_type = SNS_HALL_EVENT_TYPE_FAR;
        }

        if(!state->first_event_sent
           || hall_event.hall_event_type != state->prev_event.hall_event_type)
        {
           if (!pb_send_event(this, sns_hall_event_fields, &hall_event,
                              ts, SNS_HALL_MSGID_SNS_HALL_EVENT, &state->hall_info.suid))
           {
              SNS_INST_PRINTF(ERROR, this, "Failed to send HALL sensor event");
           } else
           {
              SNS_INST_PRINTF(HIGH, this, "DBG: HALL sensor instance event sent %d", hall_event.hall_event_type);
              state->prev_event.hall_event_type = hall_event.hall_event_type;
              if(!state->first_event_sent)
              {
                 state->first_event_sent = true;
              }
           }
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "Error: Can't get the data from GPIO.");
    }

    return;
}

//==============================================================================
// NAME      : bu52053nvx_send_config_event
// FUNCTION  : send config event to SMGR
// param[in] : this : Sensor instance reference
// REMARKS   :
// RETURN    : returned value of pb_send_event function
//==============================================================================
static sns_rc bu52053nvx_send_config_event(sns_sensor_instance *const this)
{
    sns_rc                               result = SNS_RC_SUCCESS;
    BU52053NVX_INSTANCE_STATE            *state = (BU52053NVX_INSTANCE_STATE*)this->state->state;
    sns_std_sensor_physical_config_event phy_sensor_config = sns_std_sensor_physical_config_event_init_default;

    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current     = DEVICE_ACTIVE_CURRENT;
    phy_sensor_config.range[0]           = 0;
    phy_sensor_config.range[1]           = 1;

    if(!pb_send_event(this, sns_std_sensor_physical_config_event_fields, &phy_sensor_config,
                        sns_get_system_time(), SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                        &state->hall_info.suid))
    {
       result = SNS_RC_FAILED;
    }

    return (result);
}

//==============================================================================
// * NAME      : bu52053nvx_read_gpio
// * FUNCTION  : read gpio pin
// * param[in] : instance    : Sensor reference
// * param[in] : gpio        :
// * param[in] : is_chip_pin : true if gpio is chip level TLMM pin else false
// * REMARKS   :
// *           :
// * RETURN    :
//==============================================================================
static sns_rc bu52053nvx_read_gpio(sns_sensor_instance *instance, uint32_t gpio, bool is_chip_pin, sns_gpio_state* val)
{
    sns_service_manager *smgr     = instance->cb->get_service_manager(instance);
    sns_gpio_service    *gpio_svc = (sns_gpio_service*)smgr->get_service(smgr, SNS_GPIO_SERVICE);
    sns_rc result                 = SNS_RC_SUCCESS;

    result = gpio_svc->api->read_gpio(gpio, is_chip_pin, val);

    SNS_INST_PRINTF(LOW, instance, "gpio_val = %d  result = %d", *val, result);

    return (result);
}

