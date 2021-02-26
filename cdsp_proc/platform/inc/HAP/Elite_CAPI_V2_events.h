#ifndef ELITE_CAPI_V2_EVENTS_H
#define ELITE_CAPI_V2_EVENTS_H

/* ========================================================================*/
/**
 @file Elite_CAPI_V2_events.h
 Common Audio Processing Interface v2 header file

 This file defines the events that can be raised by a module using
 the CAPI_V2 interface.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

#include "Elite_CAPI_V2_types.h"

/*
 * Events are used to send asynchronous notifications to the framework. During initialization,
 * the caller provides a callback function and a context pointer. The module can call this
 * function any time to raise an event. The appropriate payload must be sent based on the
 * event id. The callback is not thread-safe, so it must be called from the same thread context
 * as the interface functions. The payload data is copied before the function returns.
 *
 * Example:
 * Raising the KPPS change event:
 * capi_v2_event_KPPS_t payload;
 * payload.KPPS = 10000;
 *
 * capi_v2_event_info_t payload_buffer;
 * payload_buffer.port_info.is_valid = FALSE;
 * payload_buffer.payload.data_ptr = (int8_t*)(&payload);
 * payload_buffer.payload.actual_data_len = payload_buffer.payload.max_data_len = sizeof(payload);
 *
 * capi_v2_err_t result = event_cb_ptr(context_ptr, CAPI_V2_EVENT_KPPS, &payload_buffer);
 *
 */
// IDs for the events that can be raised by a module
enum capi_v2_event_id_t
{
   CAPI_V2_EVENT_KPPS, /** Used to indicate the KPPS (Kilo packets per second) requirement
    of the module. This value should be calculated assuming zero cache
    miss. This event must be raised when the module gets
    enabled and whenever the KPPS requirement changes.
    Payload Structure: capi_v2_event_KPPS_t */
   CAPI_V2_EVENT_BANDWIDTH, /** Used to indicate the bandwidth requirement (in bytes per second)
    of the module. This event must be raised when the module gets
    enabled and whenever the bandwidth requirement changes. The bandwidth
    must be specified separately for code and data. The bandwidth must be
    calculated assuming no cache.
    Payload Structure: capi_v2_event_bandwidth_t */
   CAPI_V2_EVENT_DATA_TO_DSP_CLIENT, /** Used to send data to the DSP's client apps processor. The
    module must specify a param id to indicate the type of data that
    is present in the payload. It can also specify an optional token
    to indicate additional information such as an instance identifier.
    It must then provide the payload in a buffer. The buffer can be
    safely destroyed or reused once the callback returns.
    Payload Structure: capi_v2_event_data_to_dsp_client_t */
   CAPI_V2_EVENT_DATA_TO_OTHER_MODULE, /** This event is not supported. */
   CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED, /** This indicates that the output media format from the
    module has changed. If this event is raised during a
    call to the process() function, it is assumed that
    any data output from this process() function will be
    using the old media type. Subsequent calls to process()
    will output data of the new media type. If the module
    wants to immediately output data of the new media type,
    it must exit process() with zero output and wait for
    process() to be called again.
    Payload Structure: capi_v2_set_get_media_format_t */
   CAPI_V2_EVENT_PROCESS_STATE, /** This event is sent to the client to indicate if the module is enabled
    or disabled. The module is considered enabled by default. It is
    recommended that the module disable itself if it is not doing any
    processing. The process() function of the module will not be called
    until it raises this event again to enable itself.
    Payload Structure: capi_v2_event_process_state_t */
   CAPI_V2_EVENT_ALGORITHMIC_DELAY, /** This event is used to indicate the algorithmic delay caused by the
    module. This must include any buffering delay. This event must be
    raised whenever the delay changes. The delay is provided in
    microseconds.
    Payload Structure: capi_v2_event_algorithmic_delay_t */
   CAPI_V2_EVENT_HEADROOM, /** Used to indicate the head room requirement of the module. The head room
    requirement is zero by default. The module can provide the requirement
    in millibels.
    Payload Structure: capi_v2_event_headroom_t */
   CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE, /** Used to indicate to the client that the threshold for a port has
    changed. One event has to be raised for each port whose threshold
    has changed. For more information on port thresholds, refer to
    the comments for the CAPI_V2_REQUIRES_DATA_BUFFERING property.

    Payload Structure: capi_v2_port_data_threshold_change_t
    */
   CAPI_V2_EVENT_METADATA_AVAILABLE, /** Used to indicate to the client that metadata is available for an output port.
    One event has to be raised for each port whose threshold has changed.
    For more information, refer to the comments for CAPI_V2_METADATA property.

    Payload Structure: none
    */
   CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, /** Used to send data to the DSP's service. The
       module must specify a param id to indicate the type of data that
       is present in the payload. It can also specify an optional token
       to indicate additional information such as an instance identifier.
       It must then provide the payload in a buffer. The buffer can be
       safely destroyed or reused once the callback returns.
       Payload Structure: capi_v2_event_data_to_dsp_serivce_t */
   CAPI_V2_MAX_EVENT = 0x7FFFFFFF
};

typedef enum capi_v2_event_id_t capi_v2_event_id_t;

typedef struct capi_v2_event_info_t capi_v2_event_info_t;
struct capi_v2_event_info_t
{
   capi_v2_port_info_t port_info;
   capi_v2_buf_t  payload;
};

/* The signature of the callback function that is used to raise an event to the client.
 * The pointer to this function and the context_ptr are provided by the client in the
 * call to the init() function.
 */
typedef capi_v2_err_t (*capi_v2_event_cb_f)(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);

// Payloads for the events
typedef struct capi_v2_event_KPPS_t capi_v2_event_KPPS_t;
struct capi_v2_event_KPPS_t
{
   uint32_t KPPS;
};

typedef struct capi_v2_event_bandwidth_t capi_v2_event_bandwidth_t;
struct capi_v2_event_bandwidth_t
{
   uint32_t code_bandwidth;
   uint32_t data_bandwidth;
};

typedef struct capi_v2_event_data_to_dsp_client_t capi_v2_event_data_to_dsp_client_t;
struct capi_v2_event_data_to_dsp_client_t
{
   uint32_t param_id;
   uint32_t token;
   capi_v2_buf_t payload;
};

typedef struct capi_v2_event_data_to_dsp_service_t capi_v2_event_data_to_dsp_service_t;
struct capi_v2_event_data_to_dsp_service_t
{
   uint32_t param_id;
   uint32_t token;
   capi_v2_buf_t payload;
};

typedef struct capi_v2_event_process_state_t capi_v2_event_process_state_t;
struct capi_v2_event_process_state_t
{
   bool_t is_enabled;
};

typedef struct capi_v2_event_algorithmic_delay_t capi_v2_event_algorithmic_delay_t;
struct capi_v2_event_algorithmic_delay_t
{
   uint32_t delay_in_us;
};

typedef struct capi_v2_event_headroom_t capi_v2_event_headroom_t;
struct capi_v2_event_headroom_t
{
   uint32_t headroom_in_millibels;
};

typedef struct capi_v2_port_data_threshold_change_t capi_v2_port_data_threshold_change_t;
struct capi_v2_port_data_threshold_change_t
{
   uint32_t new_threshold_in_bytes;
};


#endif /* #ifndef ELITE_CAPI_V2_EVENTS_H */
