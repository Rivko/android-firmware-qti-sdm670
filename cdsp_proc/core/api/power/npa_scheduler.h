/*============================================================================
@file npa_scheduler.h

NPA Scheduled Request Function/Structure Declarations

This file should not be included by NPA clients unless scheduled requests are
supported on the platform.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/power/npa_scheduler.h#1 $
============================================================================*/
#ifndef NPA_SCHEDULER_H
#define NPA_SCHEDULER_H

#include "npa.h"

/*----------------------------------------------------------------------------
 * Enumerations/Constants
 * -------------------------------------------------------------------------*/

/* Scheduled Request Notification State */
typedef enum {
  NPA_SCHEDULED_NOTIFICATION_LATE, /* Request executed but was late */
  NPA_SCHEDULED_NOTIFICATION_ON_TIME, /* Request executed by deadline */
  NPA_SCHEDULED_NOTIFICATION_UNSCHEDULED, /* Successfully unscheduled */
  NPA_SCHEDULED_NOTIFICATION_NO_ACTION, /* nothing was done */
  NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED, 
                                   /* implied complete occurred but
                                    * desired issue has not occurred yet */
  NPA_SCHEDULED_NOTIFICATION_UNDEFINED /* undefined value */
} npa_scheduled_notification_state;



/**
 * <!-- npa_scheduler_callback -->
 *
 * @brief Callback when request done
 *
 * Call back function that will be invoked by the scheduler to the client
 * upon completion of the scheduled request.
 *
 * The notification state will give the status of the request handling.
 * The scheduler will make every effort to see this
 * flag is not set to true, but given variation in execution and
 * concurrency, it is not possible to guarantee that requests will 
 * never complete late. 
 *
 * A client that has a scheduled request cannot be safely used for
 * another request operation before the registered
 * callback has been invoked or npa_unschedule_request() on that
 * client is called.  
 * Any attempts to use the client before will result in undefined behavior.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 * @param state : Returns how the request was handled on time, late, cancelled.
 * @param context : pointer given when scheduled
 */
typedef void (*npa_scheduler_callback) ( npa_client_handle client, 
                                         npa_scheduled_notification_state state,
                                         void * context );



 /*----------------------------------------------------------------------------
 * NPA Scheduled Resource API
 * -------------------------------------------------------------------------*/

/**
 * <!-- npa_create_schedulable_client_ex -->
 *
 * @brief Creates a synchronous, schedulable client of a resource
 *
 * Create a schedulable client of the given resource with the given
 * work model.  The new client will not be created with an
 * initial request. Work requests made via these clients will be
 * fully processed before returning unless an npa_schedule_client()
 * call is performed first.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 * @param resource_name : name of the resource that is to be used
 * @param client_name : name of the client that's being created
 * @param client_type : the work model that this client will use for 
 * work requests
 * @param client_value : resource-specific value argument
 * @param client_ref : resource-specific pointer argument
 *
 *  @return Handle to new client, NULL if client could not be created
 */
npa_client_handle 
    npa_create_schedulable_client_ex(const char *resource_name,
                                     const char *client_name,
                                     npa_client_type type,
                                     unsigned int      client_value,
                                     void             *client_ref );


/**
 * <!-- npa_create_schedulable_client -->
 *
 * @brief Creates a synchronous, schedulable client of a resource
 * 
 * Create a schedulable client of the given resource with the given
 * work model.  The new client will not be created with an
 * initial request. Work requests made via these clients will be
 * fully processed before returning unless an npa_schedule_client()
 * call is performed first.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *
 * A client must never attempt to use a NULL handle to update a resource.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to the
 * framework must be immutable. 
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created
 * @param client_type : Work model that this client will use for work requests
 *
 * @return Handle to new client, NULL if client could not be created
 */
#define npa_create_schedulable_client( resource_name, client_name, client_type ) \
  npa_create_schedulable_client_ex( resource_name, client_name, client_type, 0, NULL )


/**
 * <!-- npa_schedule_client_qclk -->
 *
 * @brief Places client into scheduled request mode.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time in QTimer ticks.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and will be invoked
 * when the request is complete. Note that the callback will be ran in the
 * Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is 
 * invoked or npa_unschedule_request is invoked and the client is successfully
 * moved out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request.  Client handles are
 * the same as the address of the npa_client structure.
 * @param request_time : Target absolute time for the request to be finished.
 * Units are T_TICKS for QTimer.
 * @param late_probability : Percentage of time (in 2^-31) that a late 
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when
 * request is completed. This may occur in a different thread.
 * @param context : data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
npa_client_handle npa_schedule_client_qclk( npa_client_handle client, 
                                            uint64 request_time, 
                                            uint32 late_probability,
                                            npa_scheduler_callback notification,
                                            void * context );


/**
 * <!-- npa_sclk_to_qclk -->
 *
 * @brief Converts time from sclks to qtimer ticks
 */
uint64 npa_sclk_to_qclk( npa_resource_time time );


/**
 * <!-- npa_schedule_client_sclk --> 
 *
 * @brief Places client into scheduled request mode.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time in slow clock ticks.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and
 * will be invoked when the request is complete. Note that the callback
 * will be ran in the Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is invoked
 * or npa_unschedule_request is invoked and the client is successfully moved
 * out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 * @param request_time : Target absolute time for the request to be finished.
 * Units are T_TICKS for slow clock.
 * @param late_probablity : percentage of time (in 2^-31) that a late
 * completion can be tolerated. 0 (0%) lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when
 * the request is completed. This may occur in a different thread.
 * @param context : Data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
#define npa_schedule_client_sclk(                                              \
          client, request_time, late_probability, notification, context )      \
        npa_schedule_client_qclk(                                              \
          client, npa_sclk_to_qclk( request_time ),               \
          late_probability, notification, context )


/**
 * <!-- npa_schedule_client -->
 * 
 * @brief Places client into scheduled request mode.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time in slow clock ticks.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and
 * will be invoked when the request is complete. Note that the callback
 * will be ran in the Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is invoked
 * or npa_unschedule_request is invoked and the client is successfully moved
 * out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 *
 * @param client : Handle of the client for the request. Client handles are the
 * same as the address of the npa_client structure.
 * @param request_time : Target absolute time for the request to be finished.
 * Units are T_TICKS for slow clock.
 * @param late_probablity : percentage of time (in 2^-31) that a late
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when the
 * request is completed. This may occur in a different thread.
 * @param context : Data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
#define npa_schedule_client( client, request_time, late_probability, notification, context ) \
npa_schedule_client_sclk( client, request_time, late_probability, notification, context )


/**
 * <!-- npa_release_and_schedule_client -->
 *
 * @brief Places client into scheduled request mode and completes any current
 * request on the client.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time.  It will also complete
 * any current requests in the most determined optimal method.  This can be
 * either issuing the "npa_complete()" immediately to just waiting for the
 * system to go to sleep and letting the Next Active Set and Sleep set handle
 * the cancel.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and will be invoked
 * when the request is complete. Note that the callback will be run in the
 * Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is invoked
 * or npa_unschedule_request is invoked and the client is successfully moved
 * out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 * @param deadline_time : Target absolute time for the request to be finished.
 * Units are T_TICKS for slow clock. Delta from "NOW" must be less than
 * 0x80000000.
 * @param late_probablity : percentage of time (in 2^-31) that a late
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when the
 * request is completed. This may occur in a different thread.
 * @param context : data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
#define npa_release_and_schedule_client(                                      \
          client, request_time, late_probability, notification, context )     \
        npa_release_and_schedule_client_qclk(                                 \
          client, npa_sclk_to_qclk( request_time ),                           \
          late_probability, notification, context )


/**
 * <!-- npa_release_and_schedule_client_qclk -->
 *
 * @brief Places client into scheduled request mode and completes any current
 * request on the client.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time.  It will also complete
 * an current requests in the most determined optimal method.  This can be
 * either issuing the "npa_complete()" immediately to just waiting for the
 * system to go to sleep and letting the Next Active Set and Sleep set handle
 * the cancel.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and will be invoked
 * when the request is complete. Note that the callback will be ran in the
 * Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is
 * invoked or npa_unschedule_request is invoked and the client is successfully
 * moved out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 * @param deadline_time : Target absolute time for the request to be finished.
 * Units are T_TICKS for qclock.
 * @param late_probability : percentage of time (in 2^-31) that a late 
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when
 * request is completed. This may occur in a different thread.
 * @param context : data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
npa_client_handle 
  npa_release_and_schedule_client_qclk( npa_client_handle client, 
                                        uint64 request_time, 
                                        uint32 late_probability,
                                        npa_scheduler_callback notification,
                                        void * context );

/**
 * <!-- npa_unschedule_request -->
 *
 * @brief Remove client from scheduled request mode.
 *
 * Takes a client and if it is in Scheduled Resource mode, moves it back
 * into normal operation mode (i.e. no scheduled).
 *
 * The notification callback setup by npa_schedule_client may still be invoked
 * by the scheduler thread if the request is currently being processed.
 *
 * This function is safe to call even if the client does not currently have an
 * active request scheduled against it but will error if it is not a 
 * schedulable client.
 *
 * @note If the scheduled request was started with a call to
 * npa_release_and_schedule_client(), the request has not been handled
 * by the NPA Scheduler and the implied complete has  not happened,
 * the implied complete will be issued in the caller's thread before
 * returning from this function.  This is to force the client request
 * into a known state - either the complete or the request has been
 * handled. 
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 *
 * @return Return value is the notification state of this request.
 */
npa_scheduled_notification_state 
    npa_unschedule_request( npa_client_handle client);


/**
 * <!-- npa_sync_scheduled_request -->
 *
 * @brief Force immediate execution of any pending scheduled request on a client
 *
 * Takes a client and if it is in Scheduled Resource mode with a pending
 * issued request, forces it to execute immediately, else it will remove any
 * client waiting for the issue. 
 *
 * The notification callback setup by npa_schedule_client will be invoked
 * if the scheduled issued request is pending or currently being processed.
 *
 * This function is safe to call even if the client does not currently have an
 * active request scheduled against it but will error if it is not a 
 * schedulable client.
 *
 * @param client : Handle of the client with the scheduled request.
 *
 * @return Return value is the notification state of this request.  Return
 * NPA_SCHEDULED_NOTIFICAITON_NO_ACTION if there was no issued request pending.
 * Else will return NPA_SCHEDULED_NOTIFICATION_LATE
 */
npa_scheduled_notification_state 
  npa_sync_scheduled_request(npa_client_handle client);

/**
 * <!-- npa_update_scheduled_request_time -->
 *
 * @brief Update the request_time or deadline of a previously scheduled
 * request
 *
 * @param client : Handle of the client with the scheduled request
 * @param new_request_time : The new request_time or deadline
 *
 * @return Returns 0 if the deadline was successfully updated, -1 if not.
 * The call may fail if the client does not have a scheduled request set up
 * or if the scheduled request has already been picked up for processing.
 */
int npa_update_scheduled_request_time(npa_client_handle client,
                                      uint64 new_request_time);
                                            
/**
 * <!-- npa_notify_next_awake -->
 *
 * @brief Register a callback with the scheduler that will be invoked after
 * wakeup, before scheduled client notifications. This interface is intended
 * for resource authors who special-process requests with the NEXT_AWAKE
 * attribute and require notifications at wakeup to complete said processing.
 * The callbacks will be invoked in LIFO order (reflecting assumed resource
 * dependencies) and there will be one callback for each invocation of the
 * below API.
 */
void npa_notify_next_awake( npa_simple_callback func, void *data );

/**
 * <!-- npa_is_next_wakeup_after_nas -->
 *
 * @brief Returns TRUE if npa_scheduler.sleep.enter == 0. This global is set to 1
 * before the scheduler issues NAS requests and reset to 0 at the end of the
 * LPR exit routine on next wakeup. Since the scheduler is the only entity
 * that issues NAS requests, resource authors may be able to use this API
 * to distingush whether a non-NAS (or non-NEXT_AWAKE) request received
 * after a NAS request (but before NEXT_AWAKE callbacks) is after next
 * wakeup (legal) or not.
 */
unsigned int npa_is_next_wakeup_after_nas( void );

#endif /* NPA_SCHEDULER_H */
