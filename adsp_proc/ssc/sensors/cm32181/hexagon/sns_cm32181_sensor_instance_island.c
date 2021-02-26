 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   3. Neither the name of Vishay Capella Microsystems nor the
*     names of its contributors may be used to endorse or promote products
*     derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_cm32181_hal.h"
#include "sns_cm32181_sensor.h"
#include "sns_cm32181_sensor_instance.h"

#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_printf.h"

/** See sns_sensor_instance_api::notify_event */
static sns_rc cm32181_inst_notify_event(sns_sensor_instance *const this)
{
	cm32181_instance_state *state = (cm32181_instance_state*)this->state->state;
	sns_sensor_event *event;

	SNS_INST_PRINTF(MED, this, "cm32181_inst_notify_event: Enter");

	// Turn COM port ON
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);

	// Handle Timer events
	if (NULL != state->timer_data_stream)
	{
		event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
		while (NULL != event)
		{
			pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
			sns_timer_sensor_event timer_event;
			if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
			{
				if (event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
				{
					if (state->ambient_light_info.sampling_intvl > 0)
					{
						cm32181_handle_ambient_light_sample(this);
					}
				}
			}
			else
			{

			}
			event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
		}
	}

	// Turn COM port OFF
	state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

	SNS_INST_PRINTF(MED, this, "cm32181_inst_notify_event: Exit");

	return SNS_RC_SUCCESS;
}

sns_sensor_instance_api cm32181_sensor_instance_api =
{
	.struct_len = sizeof(sns_sensor_instance_api),
	.init = &cm32181_inst_init,
	.deinit = &cm32181_inst_deinit,
	.set_client_config = &cm32181_inst_set_client_config,
	.notify_event = &cm32181_inst_notify_event
};
