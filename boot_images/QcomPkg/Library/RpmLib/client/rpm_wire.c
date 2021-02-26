/*===========================================================================

  Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  01/12/15     rli     Glink update
=============================================================================*/

#include "rpm_wire.h"
#include "rpmclient.h"
#include "ULogFront.h"

extern ULogHandle     rpm_log;

static const unsigned REQ_KEY  = 0x00716572;

#define MSG__KEY 0x2367736d
#define ERR_KEY  0x00727265

static unsigned _rpm_wire_send(unsigned msg_id, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *resource_data)
{
    RPMMessagePacket  msg;
    static kvp_t    *keys_buffer = 0;
    unsigned         full_invalidate;

    if(!keys_buffer)
        keys_buffer = kvp_create(128);

	if (!keys_buffer)
	{
		return 0;
	}

    kvp_clear(keys_buffer);

    full_invalidate = rpm_resource_compose_message(resource_data, set, keys_buffer);

    if(!full_invalidate && kvp_eof(keys_buffer))
        return 0; // nothing to send!

    msg.msg_id = msg_id;
    msg.set = set;
    msg.resource_type = resource;
    msg.resource_id = id;

    if(full_invalidate)
    {
        msg.data_len = 0;
        msg.kvp_buffer = 0;
    }
    else
    {
        msg.data_len = kvp_get_raw_size(keys_buffer);
        msg.kvp_buffer = (char *)kvp_get_raw_bytes(keys_buffer);
    }

    msg.service = (rpm_service_type)REQ_KEY;    
    rpmclient_put(&msg);

    return 1;
}

unsigned (*rpm_wire_send)(unsigned msg_id, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *keys) = _rpm_wire_send;

void rpm_wire_recv(kvp_t *message, rpm_mq_t *mq)
{
    unsigned key, length, *value, *message_id = 0;
    const char *error = 0;

    while(!kvp_eof(message))
    {
        kvp_get(message, &key, &length, (const char **)&value);
        switch(key)
        {
            case MSG__KEY:
                message_id = value;
                break;

            case ERR_KEY:
                error = (const char *)value;
                break;
        }
    }

    if(!message_id)
    {
        ULOG_RT_PRINTF_0(rpm_log, "rpm_wire_error (error: \"invalid packet\")");
        return; // invalid packet; no message identifier
    }

    if(error)
    {
        ULOG_RT_PRINTF_2(rpm_log, "rpm_wire_error (msg_id: 0x%08x) (error: \"%s\")", *message_id, error);
    }

    rpm_mq_completed(mq, *message_id);
    rpm_mq_flush_limit(mq);
}

