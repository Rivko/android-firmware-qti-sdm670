#ifndef ELITE_FWK_EXTNS_FEEDBACK_H
#define ELITE_FWK_EXTNS_FEEDBACK_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_feedback.h

@brief Frame work extensions for feedback header file

  This file defines a framework extensions and corresponding private propeties
  needed for feedback communication between modules.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   12/12/14    sw      (Tech Pubs) Edited Doxygen markup/comments for 2.6.
   10/28/14   roverma      Initial Version.
   ========================================================================= */

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_feedback_intro

The #FWK_EXTN_FEEDBACK framework extension is used to enable a custom framework
for data transfer from one module (source module) to another module
(destination module).

@latexonly \subsection{Process flow}

Figure \ref{fig:FrmwkExtFeedback} illustrates the process flow for the source
and destination modules communicating through the Feedback framework extension.

@endlatexonly

@fig_in_hdr{0.95,frmwk_ext_feedback_process_flow,Process flow for the Feedback framework extension,FrmwkExtFeedback}

 -# The Elite framework creates the necessary resources (queues, nodes in the
    feedback path client list, etc.) for the feedback path.
 -# The Elite framework sends the feedback path information to the source
    module of the feedback path using #CAPI_V2_PARAM_ID_FB_PATH_INFO.
 -# The Elite framework sends the feedback path client list information to the
    destination module of that feedback path using
    #CAPI_V2_PARAM_ID_CLIENT_INFO.
 -# The source module allocates the custom data buffers used to communicate
    with the destination module.
 -# The source module uses #CAPI_V2_PARAM_ID_FB_SRC_PUSH_BUF to push these
    allocated buffers to the buffer queue.
 -# During processing, the source module uses #CAPI_V2_PARAM_ID_FB_SRC_POP_BUF
    to get these buffers from the buffer queue and updates them.
 -# The destination module uses #CAPI_V2_PARAM_ID_FB_SRC_PUSH_BUF to push the
    buffers to the data queue.
 -# The destination module browses the feedback path client list and uses
    #CAPI_V2_PARAM_ID_FB_DST_GET_BUF_FROM_SRC to get the available buffer from
    each client.
 -# After consuming or processing these buffers, the destination module uses
    #CAPI_V2_PARAM_ID_FB_DST_RTN_BUF_TO_SRC to return the buffers to their
    feedback clients (source module).

These steps continue throughout the processing. When processing is complete,
the Elite framework deallocates the resources that were allocated for the
feedback path, including the custom data buffers allocated by the source
module.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "Elite_CAPI_V2_types.h"

/** @addtogroup capiv2_ext_feedback
@{ */

/** Unique identifier used to represent a custom framework extension for
  feedback between modules.

  This extension supports the following parameters:
  - #CAPI_V2_PARAM_ID_CLIENT_INFO
  - #CAPI_V2_PARAM_ID_FB_PATH_INFO
  - #CAPI_V2_PARAM_ID_FB_SRC_PUSH_BUF
  - #CAPI_V2_PARAM_ID_FB_SRC_POP_BUF
  - #CAPI_V2_PARAM_ID_FB_DST_RTN_BUF_TO_SRC
  - #CAPI_V2_PARAM_ID_FB_DST_GET_BUF_FROM_SRC @newpage
 */
#define FWK_EXTN_FEEDBACK 0x00010273

/** ID of the parameter used by the Elite framework to send custom feedback
  path client list information to a destination module. The destination module
  must browse this list to get the handles of the feedback path clients
  (source modules).

  The client list contains all of the source modules that send data to
  the destination module. This information is passed to the destination module
  from the service layer.

  @msgpayload{feedback_client_list_t}
  @table{weak__feedback__client__list__t}

  @par Example
  Caller:
  @code
  feedback_client_list_t *cust_client_list_ptr;
  capi_v2_buf_t client_buf;
  client_buf.data_ptr = (int8_t*)cust_client_list_ptr;
  client_buf.actual_data_len = sizeof(feedback_client_list_t*);
  client_buf.max_data_len = sizeof(feedback_client_list_t*);
  capi_ptr->vtbl_ptr->set_param(so.capi_ptr, CAPI_V2_PARAM_ID_CLIENT_INFO, &port_info, &client_buf);

  @endcode
  @par
  Callee:
  @code
  // Read data_ptr and max_data_len
  // Read and Cache the cust_client_list_ptr for future use
  @endcode
 */
#define CAPI_V2_PARAM_ID_CLIENT_INFO 0x00010274


/** @weakgroup weak_feedback_client_list_t
@{ */
/* Feedback client list: It represents the feedback path client list in
    which each node is feedback path client.
 */
typedef struct feedback_client_list_t
{
   void *element;
   /**< Pointer to a node in this feedback client list. */

   struct feedback_client_list_t *next;
   /**< Pointer to the next node in this feedback client list. */

}/** @cond */feedback_client_list_t/** @endcond */;
/** @} *//* end_weakgroup weak_feedback_client_list_t */


/** ID of the parameter used by the Elite framework to send feedback path
  information (such as buffer queue and data queue information) to the source
  module to use when it communicates with another module.

  @msgpayload{feedback_info_t}
  @table{weak__feedback__info__t}

  @msgpayload{feedback_config_t}
  @table{weak__feedback__config__t}

  @par Example
  Caller:
  @code
  capi_v2_buf_t params_buf;
  feedback_info_t feedback_path_info;

  params_buf.data_ptr = (int8_t*)&feedback_path_info;
  params_buf.actual_data_len = sizeof(feedback_info_t);
  params_buf.max_data_len = sizeof(feedback_info_t);
  capi_ptr->vtbl_ptr->set_param (
  so.capi_ptr,
  CAPI_V2_PARAM_ID_FB_PATH_INFO,
  &port_info,
  &params_buf);

  @endcode
  @par
  Callee:
  @code
  // Read data_ptr and max_data_len
  while (payload_size)
    {
        // Cache the feedback path information for future use
    };
  @endcode
 */
#define CAPI_V2_PARAM_ID_FB_PATH_INFO 0x00010275

/** Maximum number of channels supported on the feedback path. @newpage */
#define FEEDBACK_MAX_CHANNELS 4


/** @weakgroup weak_feedback_config_t
@{ */
/* Feedback channel info contains information about the channels and their mapping */
typedef struct feedback_config_t
{
   int32_t    num_channels;
   /**< Number of channels in the feedback path. Each Vsens and Isens is
        considered an individual channel.

        @values 1 to #FEEDBACK_MAX_CHANNELS */

   int32_t    chan_info[FEEDBACK_MAX_CHANNELS];
   /**< Channel mapping array that provides information on the order of
        the channels. */

}/** @cond */feedback_config_t/** @endcond */;
/** @} *//* end_weakgroup weak_feedback_config_t */


/** @weakgroup weak_feedback_info_t
@{ */
/* Feedback information: It is used for communication between two modules.
 * This info is sent from the Service to the Source module using CAPI_V2_PARAM_ID_FB_PATH_INFO.
 */
typedef struct feedback_info_t
{
   feedback_config_t feedback_cfg;
   /**< Feedback path configuration. */

   void  *bufQ_ptr;
   /**< Handle to the buffer queue of the service.

        The buffer queue is used at the source module for holding the buffers.
        - The source module gets the buffers from this queue.
        - The destination module returns the processed buffers to this queue.
          @tablebulletend */

   void  *dataQ_ptr;
   /**< Handle to the data queue of the service.

        The data queue is used at the destination module for holding the
        buffers.
        - The source module sends buffers to the destination module by pushing
          the buffers to this queue.
        - The destination module gets the same buffers by popping them from
          the data queue. @tablebulletend */

   uint32_t          *num_buffers_ptr;
   /**< Number of buffers in the buffer queue. */

}/** @cond */feedback_info_t/** @endcond */;
/** @} *//* end_weakgroup weak_feedback_info_t */


/** ID of the parameter used by the source module to push buffers from the
  source of the feedback path queues (buffer queue or data queue). To push the
  buffers, the source module raises #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE with
  this parameter as the payload.

  @msgpayload{capi_v2_fb_src_push_buf_t}
  @table{weak__capi__v2__fb__src__push__buf__t}

  @par Example
  Caller:
  @code
  capi_v2_event_info_t      event_info;
  capi_v2_fb_src_push_buf_t    push_buf;
  capi_v2_event_data_to_dsp_service_t data_event;

  push_buf.queue_ptr = (void*)queue_ptr;
  push_buf.buf_ptr = cx_buff_ptr;

  data_event.param_id = CAPI_V2_PARAM_ID_SRC_PUSH_BUF;
  data_event.payload.actual_data_len = sizeof(capi_v2_fb_src_push_buf_t);
  data_event.payload.max_data_len = sizeof(capi_v2_fb_src_push_buf_t);
  data_event.payload.data_ptr = (int8_t *)&push_buf; 

  event_info.payload.actual_data_len = sizeof(data_event);
  event_info.payload.max_data_len = sizeof(data_event);
  event_info.payload.data_ptr = (int8_t *)&data_event; 

  (*event_callback_info.event_cb)(event_callback_info.event_context,
                 CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, &event_info);
  @endcode @newpage
 */
#define CAPI_V2_PARAM_ID_FB_SRC_PUSH_BUF 0x00010291


/** @weakgroup weak_capi_v2_fb_src_push_buf_t
@{ */
typedef struct capi_v2_fb_src_push_buf_t
{
  void              *queue_ptr;
  /**< Pointer to the queue to which a buffer is to be pushed. */

  void              *buf_ptr;
  /**< Pointer to the buffer that is to be pushed to the queue. */

}/** @cond */capi_v2_fb_src_push_buf_t/** @endcond */;
/** @} *//* end_weakgroup weak_capi_v2_fb_src_push_buf_t */


/** ID of the parameter used by the source module to pop buffers from the
  source of the feedback path buffer queue. To pop the buffers, the source
  module raises #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE with this parameter as the
  payload.

  @msgpayload{capi_v2_fb_src_pop_buf_t}
  @table{weak__capi__v2__fb__src__pop__buf__t}

  @par Example
  Caller:
  @code
  capi_v2_event_info_t      event_info;
  capi_v2_fb_src_pop_buf_t     pop_buf;
  capi_v2_event_data_to_dsp_service_t data_event;

  pop_buf.buf_ptr_ptr = &buff_ptr;
  pop_buf.queue_ptr = queue_ptr;

  data_event.param_id = CAPI_V2_PARAM_ID_FB_SRC_POP_BUF;
  data_event.payload.actual_data_len = sizeof(capi_v2_fb_src_pop_buf_t);
  data_event.payload.max_data_len = sizeof(capi_v2_fb_src_pop_buf_t);
  data_event.payload.data_ptr = (int8_t *)&pop_buf; 

  event_info.payload.actual_data_len = sizeof(data_event);
  event_info.payload.max_data_len = sizeof(data_event);
  event_info.payload.data_ptr = (int8_t *)&data_event; 

  (*event_callback_info.event_cb)(event_callback_info.event_context,
                 CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, &event_info);
  @endcode @newpage
 */
#define CAPI_V2_PARAM_ID_FB_SRC_POP_BUF 0x00010292


/** @weakgroup weak_capi_v2_fb_src_pop_buf_t
@{ */
typedef struct capi_v2_fb_src_pop_buf_t
{
  void              *queue_ptr;
  /**< Pointer to the queue from which a buffer is to be popped. */

  void              **buf_ptr_ptr;
  /**< Double pointer to the buffer that is to be popped from the queue. */

}/** @cond */capi_v2_fb_src_pop_buf_t/** @endcond */;
/** @} *//* end_weakgroup weak_capi_v2_fb_src_pop_buf_t */


/** ID of the parameter used by the destination module to return a buffer to
  the source module by pushing that buffer to the buffer queue. The destination
  module raises #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE with this parameter as the
  payload. The module must provide the handle of the feedback path client in
  this payload.

  @msgpayload{capi_v2_fb_dst_rtn_buf_to_src_t}
  @table{weak__capi__v2__fb__dst__rtn__buf__to__src__t}

  @par Example
  Caller:
  @code
  capi_v2_event_info_t      event_info;
  capi_v2_fb_dst_return_buf_to_client_t     return_buf;
  capi_v2_event_data_to_dsp_service_t data_event;

  return_buf.buf_ptr = buff_ptr;
  return_buf.queue_ptr = client_ptr;

  data_event.param_id = CAPI_V2_PARAM_ID_FB_DST_RETURN_BUF_TO_CLIENT;
  data_event.payload.actual_data_len = sizeof(capi_v2_fb_dst_rtn_buf_to_src_t);
  data_event.payload.max_data_len = sizeof(capi_v2_fb_dst_rtn_buf_to_src_t);
  data_event.payload.data_ptr = (int8_t *)&return_buf; 

  event_info.payload.actual_data_len = sizeof(data_event);
  event_info.payload.max_data_len = sizeof(data_event);
  event_info.payload.data_ptr = (int8_t *)&data_event; 

  (*event_callback_info.event_cb)(event_callback_info.event_context,
                 CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, &event_info);
  @endcode @newpage
 */
#define CAPI_V2_PARAM_ID_FB_DST_RTN_BUF_TO_SRC 0x00010293


/** @weakgroup weak_capi_v2_fb_dst_rtn_buf_to_src_t
@{ */
typedef struct capi_v2_fb_dst_rtn_buf_to_src_t
{
  void              *client_ptr;
  /**< Pointer to the feedback client to which a buffer is to be returned. */

  void              *buf_ptr;
  /**< Pointer to the buffer that is to be returned to the feedback client. */

}/** @cond */capi_v2_fb_dst_rtn_buf_to_src_t/** @endcond */;
/** @} *//* end_weakgroup weak_capi_v2_fb_dst_rtn_buf_to_src_t */


/** ID of the parameter used by the destination module to get and return a
  buffer to the source module by popping that buffer from the data queue.
  The destination module raises #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE with this
  parameter as the payload. The module must provide the handle of the feedback
  path client in this payload.

  @msgpayload{capi_v2_fb_dst_get_buf_from_src_t}
  @table{weak__capi__v2__fb__dst__get__buf__from__src__t}

  @par Example
  Caller:
  @code
  capi_v2_event_info_t      event_info;
  capi_v2_fb_dst_get_buf_from_client_t     return_buf;
  capi_v2_event_data_to_dsp_service_t data_event;

  return_buf.buf_ptr_ptr = &buff_ptr;
  return_buf.queue_ptr = client_ptr;

  data_event.param_id = CAPI_V2_PARAM_ID_FB_DST_GET_BUF_FROM_CLIENT;
  data_event.payload.actual_data_len = sizeof(capi_v2_fb_dst_get_buf_from_src_t);
  data_event.payload.max_data_len = sizeof(capi_v2_fb_dst_get_buf_from_src_t);
  data_event.payload.data_ptr = (int8_t *)&return_buf; 

  event_info.payload.actual_data_len = sizeof(data_event);
  event_info.payload.max_data_len = sizeof(data_event);
  event_info.payload.data_ptr = (int8_t *)&data_event; 

  (*event_callback_info.event_cb)(event_callback_info.event_context,
                 CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, &event_info);
  @endcode @newpage
 */
#define CAPI_V2_PARAM_ID_FB_DST_GET_BUF_FROM_SRC 0x00010294


/** @weakgroup weak_capi_v2_fb_dst_get_buf_from_src_t
@{ */
typedef struct capi_v2_fb_dst_get_buf_from_src_t
{
  void              *client_ptr;
  /**< Pointer to the feedback client from which a buffer is to be received. */

  void              **buf_ptr_ptr;
  /**< Double pointer to the buffer that is to be received from the feedback
       client. */

}/** @cond */capi_v2_fb_dst_get_buf_from_src_t/** @endcond */;
/** @} *//* end_weakgroup weak_capi_v2_fb_dst_get_buf_from_src_t */

/** @} *//* end_addtogroup capiv2_ext_feedback */

/** @endcond *//* OEM_only */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_FEEDBACK_H */
