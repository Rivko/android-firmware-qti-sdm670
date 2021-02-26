/*============================================================================
@file awake_until.h

Awake Until Resource Declarations

This file should not be included by NPA clients unless the Awake Until
feature is expected to be defined and supported. This feature is only
supported alongwith the NPA Scheduled Request feature.

To use the Awake Until feature, create a client upon the
AWAKE_UNTIL_RESOURCE_NAME Resource and issue sync requests to set values.

Note that a state value of zero should not be used as this is used internally
to cancel/complete a request.  So the time should always be non-zero.  This
can be accomplished by either or'ing in a 1 or grabbing the max of the time
and 1. If the value zero is passed in, the resource will interpret this as
cancelling the request.

npa_client_handle awake_until_client;
awake_until_client = npa_create_sync_client( AWAKE_UNTIL_RESOURCE_NAME,
                                             "awake_until client",
                                             NPA_CLIENT_REQUIRED );
npa_issue_required_request( awake_until_client, time | 0x1 );

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/power/awake_until.h#1 $
============================================================================*/
#ifndef AWAKE_UNTIL_H
#define AWAKE_UNTIL_H

#include "npa.h"

#define AWAKE_UNTIL_RESOURCE_NAME "/core/cpu/awake_until"

#endif /* AWAKE_UNTIL_H */

