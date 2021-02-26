/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPMCLIENT_H
#define RPMCLIENT_H

#include "rpm.h"
#include "kvp.h"
#include "rpm_comdef.h"

/// Call before using any other rpm API.
void rpmclient_init(void);

/*
 * RPM Request Service API
 * 
 * This is the main RPM interface; it's used by NPA remoting and some drivers
 * to actually make requests of the RPM.  Internally the driver keeps a copy of
 * each request being made to the RPM and attempts to synchronize the desired
 * request state with the RPM's stored request state in the most efficient
 * manner possible.
 *
 * It's important to note that rpm_post_request returns immediately after
 * storing the request state *locally* -- before attempting to make use of the
 * resource you must call rpm_barrier on the returned message ID.  If you need
 * to immediately make use of the resource, the convenience macro
 * rpm_sync_request(set, resource, id, size, data) is available to perform both
 * steps in a single line of code.
 */

/// Post a request to the *local* request state.  This will eventually synchronize with the RPM copy.
// @param  set        The RPM "set" which receives this request, affecting what scenarios it applies to.
// @param  resource   The class of resource being requested (different resource classes accept different data).
// @param  id         The particular resource being addressed (i.e., if 'resource' tells us we're talking to a clock, this identifies *which* clock).
// @param  kvps       The data being sent in this request; the data is overlaid on previous kvps, so incremental updates are acceptable.
// @return message_id Unique identifier by which this request can be identified later.
unsigned rpm_post_request(rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps);

/// Ensure that a posted request has been completely synchronized on the RPM side.
// @param message_id The return value from the rpm_post_request call that needs to be guaranteed.
void rpm_barrier(unsigned message_id);

/// Intended for use only from sleep; runs the RPM message queue from contexts that interrupts don't work.
// @param message_id The return value from the rpm_force_sync that must be completed.
void rpm_churn_queue(unsigned message_id);

// This used to determine if any RPM messages are in progress at any stage
// @return TRUE if the queue is empty (and thus nothing needs to be done). FALSE if its non-empty
bool rpm_is_mq_empty(void);

/// Convenience macro for when a request will be immediately relied upon.
#define rpm_sync_request(set, resource, id, kvps) \
    do { rpm_barrier(rpm_post_request(set, resource, id, kvps)); } while(0)

/// Forces all contents of a set to be flushed to the RPM.
// This is generally most useful for sets which are not normally synchronized
// (for example, the sleep set is merely cached until sleep calls this function
// just before an RPM handshake).
// @return The message ID for the final post from the set.
unsigned rpm_force_sync(rpm_set_type set);

/// Signal to the RPM that a sleep attempt (may have) been aborted.
// This function is used to notify the RPM to go ahead and parse any queued
// requests to the next active set, since this processor will not be halting in
// the near future.  If there are no next active set messages queued to the RPM
// or if they were all successfully processed while this processor is asleep,
// this action is harmless.  This must be asserted before waiting for next
// active message responses if the SPM handshake for your processor can fail
// (due to WFI fallthrough with an already-pending interrupt, etc.).  It should
// be deasserted as soon as all NAS acknowledgments are received, or at a
// minimum before any new NAS requests are sent.
void rpm_signal_aborted_sleep(unsigned asserted);

/// Preallocate all of the storage for a {resource,key} in advance so sleep can avoid malloc calls.
void rpm_preallocate_resource_kvp(rpm_resource_type resource, unsigned id, unsigned key, unsigned expected_length);

// @return whether or not the RPM is up and running 
bool rpm_is_up(void);

/// Set lockless mode
void rpm_set_lockless_mode(void);

/// Unset lockless mode
void rpm_unset_lockless_mode(void);

/// RPM deinitialization
void rpmclient_deinit(void);

//New Features: rpm resource request size API
unsigned rpm_get_request_size(rpm_set_type set, rpm_resource_type resource, unsigned id);
#endif // RPMCLIENT_H

