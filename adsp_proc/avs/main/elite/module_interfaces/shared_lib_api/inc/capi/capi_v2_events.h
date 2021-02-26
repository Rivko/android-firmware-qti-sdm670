#ifndef CAPI_V2_EVENTS_H
#define CAPI_V2_EVENTS_H

/* ========================================================================*/
/**
 @file capi_v2_events.h
 Common Audio Processing Interface v2 header file

 This file defines the events that can be raised by a module using
 the CAPI_V2 interface.
 */
/* =========================================================================
 Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 Export of this technology or software is regulated by the U.S. Government,
 Diversion contrary to U.S. law prohibited.
  ========================================================================== */
/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 08/28/17    sw     (Tech Pubs) Edited Doxygen comments for AVS.2.8 (SDM845).
 05/14/15    sw     (Tech Pubs) Merged Doxygen comments from 8994 (2.6);
                    edited Doxygen comments/markup for 8952 (2.6).
 ========================================================================= */

#include "capi_v2_types.h"

/** @weakgroup weakf_capiv2_event_info
  Modules use events to send asynchronous notifications to the framework.
  During initialization, the framework provides a callback function and a
  context pointer. The module can call this function any time to raise an
  event. The appropriate payload must be sent based on the event ID.

  The callback is not thread safe, so it must be called from the same thread
  context as the interface functions unless mentioned otherwise in the event
  description. The payload data is copied before the function returns.

  For example, raising the kilo packets per second (KPPS) change event:
  @code
   capi_v2_event_KPPS_t payload;
   payload.KPPS = 10000;

   capi_v2_event_info_t payload_buffer;
   payload_buffer.port_info.is_valid = FALSE;
   payload_buffer.payload.data_ptr = (int8_t*)(&payload);
   payload_buffer.payload.actual_data_len = payload_buffer.payload.max_data_len = sizeof(payload);

   capi_v2_err_t result = event_cb_ptr(context_ptr, CAPI_V2_EVENT_KPPS, &payload_buffer);

  @endcode
 */

/** @ingroup capiv2_events
  Valid IDs for the events that can be raised by a module.
*/
enum capi_v2_event_id_t
{
   CAPI_V2_EVENT_KPPS,
   /**< Indicates the KPPS requirement of the module.
        This value must be calculated assuming zero cache miss. @vertspace{4}

        Payload structure: capi_v2_event_KPPS_t @vertspace{4}

        This event must be raised when the module is enabled and when the KPPS
        requirement changes. @vertspace{6} */

   CAPI_V2_EVENT_BANDWIDTH,
    /**< Indicates the bandwidth requirement of the module (in bytes per
         second). @vertspace{4}

         Payload structure: capi_v2_event_bandwidth_t @vertspace{4}

         The bandwidth must be specified separately for code and data. The
         bandwidth must be calculated assuming no cache. @vertspace{4}

         This event must be raised when the module is enabled and when the
         bandwidth requirement changes. @vertspace{6} */

   CAPI_V2_EVENT_DATA_TO_DSP_CLIENT,
    /**< Sends data to the client aDSP. @vertspace{4}

         Payload structure: capi_v2_event_data_to_dsp_client_t @vertspace{6}

         The module must specify a parameter ID to indicate the type of data
         that is present in the payload. The module can also specify an
         optional token to indicate additional information such as an instance
         identifier. @vertspace{4}

         The module must provide the payload in a buffer. The buffer can be
         safely destroyed or reused once the callback returns. @vertspace{6} */

   CAPI_V2_EVENT_DATA_TO_OTHER_MODULE,
    /**< Currently not supported. @vertspace{6} @newpage*/

   CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED,
    /**< Indicates that the output media format from the module has changed.
         @vertspace{4}

         Payload structure: capi_v2_set_get_media_format_t @vertspace{4}

         If this event is raised during a call to capi_v2_vtbl_t::process(),
         any data output from this process function is assumed to use
         the old media type. @vertspace{4}

         Subsequent calls to the process function output the data of the new
         media type. If the module is to immediately output the data of the
         new media type, it must exit the process function with zero output
         and wait for the process function to be called again. @vertspace{6} */

   CAPI_V2_EVENT_PROCESS_STATE,
    /**< Sent to the client to specify whether the module is enabled or
         disabled. The module is considered enabled by default. @vertspace{4}

         Payload structure: capi_v2_event_process_state_t @vertspace{4}

         We recommend that the module disable itself if it is not doing any
         processing. The capi_v2_vtbl_t::process() function of the module is
         not called until it raises this event again to enable itself.
         @vertspace{6} */

   CAPI_V2_EVENT_ALGORITHMIC_DELAY,
    /**< Indicates the algorithmic delay (in microseconds) caused by the
         module. This value must include any buffering delay. @vertspace{4}

         Payload structure: capi_v2_event_algorithmic_delay_t @vertspace{4}

         This event must be raised when the delay changes. @vertspace{6} */

   CAPI_V2_EVENT_HEADROOM,
    /**< Indicates the head room requirement (in millibels) of the module (the
         default is zero). @vertspace{4}

         Payload structure: capi_v2_event_headroom_t @vertspace{6} */

   CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE,
    /**< Indicates to the client that the threshold for a port has changed.
         @vertspace{4}

         Payload structure: capi_v2_port_data_threshold_change_t @vertspace{4}

         One event must be raised for each port whose threshold has changed.
         For more information on port thresholds, see
         #CAPI_V2_REQUIRES_DATA_BUFFERING. @vertspace{6} */

   CAPI_V2_EVENT_METADATA_AVAILABLE,
    /**< Indicates to the client that metadata is available for an output port.
         @vertspace{4}

         Payload structure: None @vertspace{4}

         One event must be raised for each port whose threshold has changed.
         For more information, see #CAPI_V2_METADATA. @vertspace{6} */

   CAPI_V2_EVENT_DATA_TO_DSP_SERVICE,
    /**< Sends data to the aDSP service. @vertspace{4}

         Payload structure: capi_v2_event_data_to_dsp_service_t @vertspace{4}

         The module must specify a parameter ID to indicate the type of data
         that is present in the payload. It can also specify an optional token
         to indicate additional information such as an instance identifier.
         It must then provide the payload in a buffer. The buffer can be
         safely destroyed or reused once the callback returns. @vertspace{6} */

   CAPI_V2_EVENT_GET_LIBRARY_INSTANCE,
    /**< Queries for an instance of a supporting library. @vertspace{4}

         Payload structure: capi_v2_event_get_library_instance_t @vertspace{4}

         The module must provide the ID of the library. The service returns a
         pointer to an instance of the library. The instance contains a pointer
         to the virtual function table of the library interface as the first
         element. Every library virtual function table has
         capi_v2_library_base_t as its first element. @vertspace{4}

         For example, the following definitions are in the library header file:
         @vertspace{4}
   @code
   #define LIB_GUID 0xABCD

   struct lib_obj;

   struct lib_vtable
   {
      capi_v2_library_base_t b; // Should be the first element of every library vtable
      capi_v2_err_t lib_func1(lib_obj *obj, uint32_t param);
      capi_v2_err_t lib_func2(lib_obj *obj, uint32_t param);
      capi_v2_err_t lib_func3(lib_obj *obj, uint32_t param);
   };

   struct lib_obj
   {
      lib_vtable *vtble;
   };

   @endcode
   @vertspace{12}

   The following code is in the module: @vertspace{4}
   @code
    capi_v2_event_get_library_instance_t payload;
    payload.id = LIB_GUID;
    payload.ptr = NULL;

    capi_v2_event_info_t payload_buffer;
    payload_buffer.port_info.is_valid = FALSE; // May be a valid value based on the use case.
    payload_buffer.payload.data_ptr = (int8_t*)(&payload);
    payload_buffer.payload.actual_data_len = payload_buffer.payload.max_data_len = sizeof(payload);

    capi_v2_err_t result = event_cb_ptr(context_ptr, CAPI_V2_EVENT_GET_LIBRARY_INSTANCE, &payload_buffer);

    lib_obj *lib_ptr = (lib_obj*)payload.ptr;

    lib_ptr->vtbl.lib_func1(lib_ptr, param);
    ...
    lib_ptr->vtbl.lib_func2(lib_ptr, param);
    ...
    lib_ptr->vtbl.lib_func3(lib_ptr, param);
    ...

    lib_ptr->vtbl.b.end(lib_ptr); // The pointer is freed here.
    lib_ptr = NULL;

    //The GUID can be queried from the lib object itself. This allows the code to determine the type of the object if it is not known:
    void *unknown_lib_ptr = get_lib(); // Some function that returns a stored lib pointer whose type is unknown.
    uint32_t interface_id = ((capi_v2_library_base_t**)unknown_lib_ptr)->get_interface_id();
    switch(interface_id)
    {
    case LIB_GUID:
       lib_obj *lib_ptr = (lib_obj*)unknown_lib_ptr;
       ...
    ...
    }

    @endcode @vertspace{6} @newpage */

   CAPI_V2_EVENT_GET_DLINFO,
   /**< Queries for dynamic load information if a module is available as an SO
        file. @vertspace{4}

        Payload structure: capi_v2_event_dlinfo_t @vertspace{6} */

   CAPI_V2_MAX_EVENT = 0x7FFFFFFF
   /**< Maximum value that an event ID can take. */

};

typedef enum capi_v2_event_id_t capi_v2_event_id_t;

typedef struct capi_v2_event_info_t capi_v2_event_info_t;

/** @ingroup capiv2_data_types
  Contains information about an event.
 */
struct capi_v2_event_info_t
{
   capi_v2_port_info_t port_info;
   /**< Port for which this event is raised.

        Set this field to an invalid value for events that are not specific
        to any port. */

   capi_v2_buf_t  payload;
   /**< Buffer that holds the payload for the event. */

};

/** @ingroup capiv2_func_event_callback
  Signature of the callback function that is used to raise an event to the
  client.
  The pointer to this function and the context_ptr are provided by the client
  in the call to capi_v2_init_f().

  @datatypes
  #capi_v2_event_id_t \n
  capi_v2_event_info_t

  @param [in] context_ptr    Context pointer value provided by the framework
                             when the callback function was passed.
  @param [in] id             ID for the event being raised.
  @param [in] event_info_ptr Information about the event being raised.

  @returns
  CAPI_V2_EOK -- The event was processed successfully.
  @par
  Error code -- Otherwise.

  @dependencies
  This function must be called from the same thread context as the interface
  functions.
 */
typedef capi_v2_err_t (*capi_v2_event_cb_f)(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);

// Payloads for the events
typedef struct capi_v2_event_KPPS_t capi_v2_event_KPPS_t;

/** @addtogroup capiv2_payload_structs
@{ */
/** Payload of the #CAPI_V2_EVENT_KPPS event, which indicates the KPPS
  requirement of a module.
 */
struct capi_v2_event_KPPS_t
{
   uint32_t KPPS;  /**< Kilo packets per second requirement of the module. */
};

typedef struct capi_v2_event_bandwidth_t capi_v2_event_bandwidth_t;

/** Payload of the #CAPI_V2_EVENT_BANDWIDTH event, which indicates the
  bandwidth requirement of a module.
 */
struct capi_v2_event_bandwidth_t
{
   uint32_t code_bandwidth;
   /**< Code bandwidth of the module (in bytes per second). */

   uint32_t data_bandwidth;
   /**< Data bandwidth of the module (in bytes per second). */
};

typedef struct capi_v2_event_data_to_dsp_client_t capi_v2_event_data_to_dsp_client_t;

/** Payload of the #CAPI_V2_EVENT_DATA_TO_DSP_CLIENT event.
  (Currently not supported).
 */
struct capi_v2_event_data_to_dsp_client_t
{
   uint32_t param_id;
   /**< Indicates the type of data that is present in the payload. */

   uint32_t token;
   /**< Optional token that indicates additional information, such as an
        instance identifier. */

   capi_v2_buf_t payload;
   /**< Buffer containing the payload.

        This buffer can be safely destroyed or reused once the callback
        returns. */
};

typedef struct capi_v2_event_data_to_dsp_service_t capi_v2_event_data_to_dsp_service_t;

/** Payload of the #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE event, which sends data
  to the aDSP service.
 */
struct capi_v2_event_data_to_dsp_service_t
{
   uint32_t param_id;
   /**< Indicates the type of data that is present in the payload. */

   uint32_t token;
   /**< Optional token that indicates additional information, such as an
        instance identifier. */

   capi_v2_buf_t payload;
   /**< Buffer containing the payload.

        This buffer can be safely destroyed or reused once the callback
        returns. */
};

typedef struct capi_v2_event_process_state_t capi_v2_event_process_state_t;

/** Payload of the #CAPI_V2_EVENT_PROCESS_STATE event, which specifies whether
  a module is enabled or disabled.
 */
struct capi_v2_event_process_state_t
{
   bool_t is_enabled;
   /**< Specifies whether the module is enabled. If a module is disabled, its
        capi_v2_vtbl_t::process() function is not called.

        @values
        - 0 -- Disabled
        - 1 -- Enabled (Default) @tablebulletend */
};

typedef struct capi_v2_event_algorithmic_delay_t capi_v2_event_algorithmic_delay_t;

/** Payload of the #CAPI_V2_EVENT_ALGORITHMIC_DELAY event, which indicates the
  algorithmic delay caused by a module.
 */
struct capi_v2_event_algorithmic_delay_t
{
   uint32_t delay_in_us;
   /**< Algorithmic delay in microseconds caused by the module.
        This value must include any buffering delay. */
};

typedef struct capi_v2_event_headroom_t capi_v2_event_headroom_t;

/** Payload of the #CAPI_V2_EVENT_HEADROOM event, which indicates the head room
  requirement of a module.
 */
struct capi_v2_event_headroom_t
{
   uint32_t headroom_in_millibels;
   /**< Headroom requirement of the module. The default is zero. */
};

typedef struct capi_v2_port_data_threshold_change_t capi_v2_port_data_threshold_change_t;

/** Payload of the #CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE event, which
  indicates that the threshold for a port has changed.
 */
struct capi_v2_port_data_threshold_change_t
{
   uint32_t new_threshold_in_bytes;  /**< Value of the threshold for a port. 
                                          @newpagetable */
};
/** @} *//* end_addtogroup capiv2_payload_structs */

typedef struct capi_v2_library_base_t capi_v2_library_base_t;

/** @ingroup capiv2_data_types
  First element of every library virtual function table.
*/
struct capi_v2_library_base_t
{
   uint32_t (*get_interface_id)(void* obj_ptr);
   /**< Returns the ID associated with the interface that this object
        implements. */

   void (*end)(void* obj_ptr);
   /**< De-initializes the object and frees the memory associated with it. The
        object pointer is not valid after this call. */
};

typedef struct capi_v2_event_get_library_instance_t capi_v2_event_get_library_instance_t;

/** @ingroup capiv2_payload_structs
  Payload of the #CAPI_V2_EVENT_GET_LIBRARY_INSTANCE event, which queries
  for an instance of a supporting library.
*/
struct capi_v2_event_get_library_instance_t
{
   uint32_t id;   /**< ID of the library. */
   void *ptr;     /**< Pointer to the instance of the library. */
};

/** @ingroup capiv2_payload_structs
  Payload of the #CAPI_V2_EVENT_GET_DLINFO event, which queries for
  dynamic load information of an SO file.
*/
struct capi_v2_event_dlinfo_t
{
   uint32_t is_dl;
   /**< Indicates whether the SO file is dynamically loaded.

        @values
        - TRUE -- File is dynamically loaded
        - FALSE -- Otherwise

        The rest of this payload is applicable only if the SO file is loaded. */

   uint32_t load_addr_lsw;
   /**< Lower 32 bits of the physical address where the SO file is loaded. */

   uint32_t load_addr_msw;
   /**< Upper 32 bits of the physical address where the SO file is loaded.

        The 64-bit number formed by load_addr_lsw and load_addr_msw must
        be 32-byte aligned and must have been previously mapped. */

   uint32_t load_size;
   /**< Size (in bytes) of the loaded SO file. */

};

#endif /* #ifndef CAPI_V2_EVENTS_H */
