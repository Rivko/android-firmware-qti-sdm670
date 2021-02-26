/*===========================================================================

  rpm_resource.h - container for client-side kvps

  Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_RESOURCE_H
#define RPM_RESOURCE_H

#include "rpm_comdef.h"
#include "rpm.h"
#include "kvp.h"
#include "bytearray.h"

typedef struct
{
    bytearray_t *data;
    bool         valid;
} rpm_request_t;

typedef struct
{
    unsigned key;

    // The representaiton of the active set.
    rpm_request_t active_at_rpm; // Cache of last request sent.
    rpm_request_t active_local;  // Current desired request.

    // The representation of the sleep set.
    rpm_request_t sleep_at_rpm; // Cache of the last request sent.
    rpm_request_t sleep_local;  // Current desired request.

    // The representation of the NAS.  NAS exists in a special world, existing
    // only transiently during the sleep enter/exit process.  It is never
    // cached, since if your processor is awake any NAS requests have been
    // applied to the active set direclty.
    rpm_request_t nas_local;

    bool dirty[RPM_NUM_SETS];
} rpm_key_data_t;

typedef struct
{
    uint64_t resource_identifier; // concatenation of {type, id}

    unsigned        num_keys;
    rpm_key_data_t *keys;

    bool dirty[RPM_NUM_SETS];
} rpm_resource_t;

// Initialize an rpm_resource_t to a "blank" setting for a given resource {type,id}.
void rpm_resource_init(rpm_resource_t *self, rpm_resource_type type, unsigned id);

// Formally log a request to the resource.  May affect dirtiness.
void rpm_resource_update(rpm_resource_t *self, rpm_set_type set, kvp_t *request_data);

unsigned rpm_resource_get_request_size(rpm_set_type set, rpm_resource_t *self);

// Get the actual dirty data that needs to be sent to the RPM, collected into the out parameter message_data.
// Note: it is assumed that any composed message will be sent immediately; all previously "dirty" buffers will be marked clean.
// Return value: true if the entire resource should be invalidated, else false.
bool rpm_resource_compose_message(rpm_resource_t *self, rpm_set_type set, kvp_t *message_data);

// Allocate space in {active,sleep,nas} for a key 'key', allowing for at least 'expected_length' bytes of data.
rpm_key_data_t *rpm_resource_preallocate(rpm_resource_t *self, unsigned key, unsigned expected_length);

#endif // RPM_RESOURCE_H

