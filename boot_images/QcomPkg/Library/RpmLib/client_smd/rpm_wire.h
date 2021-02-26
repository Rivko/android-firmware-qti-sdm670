/*===========================================================================

  rpm_events.h - runtime-replacable wrapper the RPM wire protocol

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_WIRE_H
#define RPM_WIRE_H

#include "rpm.h"
#include "rpm_mq.h"
#include "rpm_resource.h"
#include "kvp.h"
#include "smd_lite.h"

typedef struct
{
    unsigned msg_id;
    unsigned set;
    unsigned resource_type;
    unsigned resource_id;
    unsigned data_len;
} RPMMessageHeader;

/* Returns 0 if the message sent nothing; otherwise nonzero. */
extern unsigned (*rpm_wire_send)(unsigned msg_id, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *keys);

void rpm_wire_recv(kvp_t *message, rpm_mq_t *mq);


/*
 * Top-level RPM service API
 *
 * This API allows you to send messages to and receive messages as a top level
 * service.  Most drivers do not want to use this API; instead they use the RPM
 * resource request API shown above.  That API will wrap your request in the
 * appropriate header to ensure its delivery to the request handling service.
 *
 * Since the main RPM service (request handling) is already implemented, the
 * non-internal use of these functions is generally limited to system tests or
 * target-specific hacks.
 */

typedef void (*rpmclient_service_cb)(kvp_t *message, void *context);
void rpmclient_register_handler(rpm_service_type service, rpmclient_service_cb cb, void *context);

void rpmclient_put(rpm_service_type service, smdl_iovec_type *data);
void rpmclient_put_nas(rpm_service_type service, smdl_iovec_type *data, smdl_iovec_type *kvps, RPMMessageHeader msg_header);

#endif // RPM_WIRE_H

