#ifndef ELITE_LIB_CAPI_V2_PROCESS_THREAD_H
#define ELITE_LIB_CAPI_V2_PROCESS_THREAD_H

/**
  @file Elite_lib_capi_v2_process_thread.h

  @brief A CAPIv2 supporting library for launching the core process in a
  separate thread.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.   All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  06/05/15    sw      (Tech Pubs) Edited Doxygen markup/comments for 8952 (2.6)
  2/28/15    bvodapal   Created file.
=============================================================================*/

/** @weakgroup weakf_capiv2_lib_core_proc_thread_intro

The Core Process library is used to create a thread and launch the core process
(CAPIv2 process) in a separate thread.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"


/** @addtogroup capiv2_lib_core_thread
@{ */

/** Unique identifier that represents this library interface. */
#define ELITE_LIB_CAPI_V2_PROCESS_THREAD 0x00010915

/*------------------------------------------------------------------------------
 * Interface definition
 *----------------------------------------------------------------------------*/

/** Function pointer that invokes the process call of the utilityn thread
  process.

  @datatypes
  capi_v2_t \n
  capi_v2_stream_data_t

  @param[in,out] _pif   Pointer to the module object.
  @param[in,out] input  Array of pointers to the input data for each input
                        port. \n
                        The length of the array is the number of input ports.
                        The client sets the number of input ports using the
                        #CAPI_V2_PORT_NUM_INFO property. \n
                        The function must modify the actual_data_len field
                        to indicate how many bytes were consumed.
   @param[out] output   Array of pointers to the output data for each output
                        port. \n
                        The client sets the number of output ports using the
                        #CAPI_V2_PORT_NUM_INFO property. \n
                        The function sets the actual_data_len field to
                        indicate how many bytes were generated.

  @return
  None.

  @dependencies
  This separate thread process performs only core processing.
  All initializations must have been performed by the parent module that
  launches this thread.
 */
typedef void (*capi_v2_pu_process_fn_t)(capi_v2_t *_pif,
        const capi_v2_stream_data_t* input,
        capi_v2_stream_data_t* output);

/** Virtual function table of the library interface.

  To use this library:
   -# Get an object of type elite_lib_capi_v2_process_thread_util_t using
      #CAPI_V2_EVENT_GET_LIBRARY_INSTANCE. \n @vertspace{2}
      The object is an instance of library, along with the virtual function
      table for the library interface.
   -# Call capi_v2_pu_launch_thread() to launch a thread with a defined stack
      size.
   -# Call capi_v2_pu_set_process() to set the core library process function
      to be launched in a separate thread. \n @vertspace{2}
      The core process function must take in frame size samples and provide
      frame size samples in every call.
   -# Call capi_v2_pu_set_media_type() to set the number of channels and
      channel map. \n @vertspace{2}
      The library creates data buffers.
   -# Call capi_v2_pu_process() to instantiate library processing.
   -# Call capi_v2_pu_wait_for_processing_done() to notify the calling thread
      to wait before changing any configuration.
   -# Protect the media format changes and set or get parameters to ensure that
      there are no race conditions. \n @vertspace{2}
      One method is to wait for the thread to be idle.
   -# Call capi_v2_pu_reset() to clear the stale data in the internal buffers
      of the thread utility library.
 */
struct elite_lib_capi_v2_process_thread_util_vtable
{
    capi_v2_library_base_t b;
    capi_v2_err_t (*capi_v2_pu_process )(void *obj_ptr, const capi_v2_stream_data_t* input, capi_v2_stream_data_t* output);
    capi_v2_err_t (*capi_v2_pu_launch_thread)(uint32_t stack_size, void *obj_ptr, char *thread_name);
    capi_v2_err_t (*capi_v2_pu_set_media_type) (void *obj_ptr, uint32_t frame_size_in_bytes, uint32_t num_input_buffers,
            const uint16_t input_channel_map[],uint32_t num_output_buffers, const uint16_t output_channel_map[]);
    void   (*capi_v2_pu_set_process_function)(void *obj_ptr, capi_v2_t *module_ptr, capi_v2_pu_process_fn_t fn);
    void   (*capi_v2_pu_reset)(void *obj_ptr);
    void   (*capi_v2_pu_wait_for_processing_done)(void *obj_ptr);
};

/** Contains the pointer to the virtual table of the thread
  process utility defined in elite_lib_capi_v2_process_thread_util_vtable.
 */
struct elite_lib_capi_v2_process_thread_util_t
{
   const elite_lib_capi_v2_process_thread_util_vtable *vtable;
   /**< Pointer to the virtual function table. */
};

/** @} *//* end_addtogroup capiv2_lib_core_thread */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_LIB_CAPI_V2_PROCESS_THREAD_H */
