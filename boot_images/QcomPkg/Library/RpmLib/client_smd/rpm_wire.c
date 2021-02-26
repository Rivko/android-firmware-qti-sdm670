/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpm_wire.h"
#include "rpmclient.h"
#include "ULogFront.h"

extern ULogHandle     rpm_log;

static const unsigned REQ_KEY  = 0x00716572;

#define MSG__KEY 0x2367736d
#define ERR_KEY  0x00727265

static unsigned _rpm_wire_send(unsigned msg_id, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *resource_data)
{
    static kvp_t    *keys_buffer = 0;
    RPMMessageHeader header;
    smdl_iovec_type  header_vec, data_vec;
    unsigned         full_invalidate;

    if(!keys_buffer)
        keys_buffer = kvp_create(128);

    kvp_clear(keys_buffer);

    full_invalidate = rpm_resource_compose_message(resource_data, set, keys_buffer);

    if(!full_invalidate && kvp_eof(keys_buffer))
        return 0; // nothing to send!

    header.msg_id = msg_id;
    header.set = set;
    header.resource_type = resource;
    header.resource_id = id;

    if(full_invalidate)
    {
        header.data_len = 0;
        header_vec.next = 0;
    }
    else
    {
        header.data_len = kvp_get_raw_size(keys_buffer);

        data_vec.next   = 0;
        data_vec.length = header.data_len;
        data_vec.buffer = (char *)kvp_get_raw_bytes(keys_buffer);

        header_vec.next = &data_vec;
    }

    header_vec.length = sizeof(header);
    header_vec.buffer = &header;

    if(header.set == RPM_NEXT_ACTIVE_SET)
    {
        rpmclient_put_nas((rpm_service_type)REQ_KEY, &header_vec, &data_vec, header);
    }
    else
    {
    rpmclient_put((rpm_service_type)REQ_KEY, &header_vec);
    }

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

