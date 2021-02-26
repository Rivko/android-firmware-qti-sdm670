/** @file topo_utils.h

 @brief This file contains Topo Utils APIs published by AFE

*/

/*========================================================================
  $Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/inc/topo_utils.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 4/8/2014   mk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _TOPO_UTILS_H_
#define _TOPO_UTILS_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite_types.h"
#include "Elite_CAPI_V2.h"
#include "adsp_afe_service_commands.h"
#include "bgt_handler_fwk.h"
#include "AFECodecService.h"
#include "Elite_fwk_extns_codec_interrupt.h"
#include "EliteCmnTopology_db_if.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus


#define TOPO_UTILS_VERSION    0x00010000

typedef struct topo_t topo_t;
/**
 * Structure for client to codec service for interrupt handling
 */ 
typedef struct topo_cdc_client
{
  bool_t is_pending_ack;
  /**< ack pending to codec service */
  cdc_client_info_t client_info;
  /**< Client info for codec interrupt handling */

} topo_cdc_client_t; 

typedef struct topo_cdc_int_info_t
{
  capi_v2_num_needed_cdc_int_t num_cdc_int;
  /**< AFE port interface which contains the back ground thread handler */
  capi_v2_cdc_int_t *cdc_int_ptr;
  /**< pointer to background thread handler */

}topo_cdc_int_info_t; 

// Define the only media format type supported.
typedef struct topo_media_fmt_t
{
	capi_v2_data_format_header_t h;
	capi_v2_standard_data_format_t f;
}topo_media_fmt_t;

typedef struct topo_module_amdb_hdl
{
   enum topo_module_amdb_hdl_type
   {
      NULL_HANDLE,
      CAPI_V2_HANDLE
   } hdl_type;

   void* handle;
}topo_module_amdb_hdl;

typedef struct topo_module_event_handle_t
{
   topo_t *topo_ptr;
   uint32_t module_index;
}topo_module_event_handle_t;

/**
  @brief Output Media Type information to be passed to the Module.
 */
typedef struct topo_send_output_media_fmt_t
{
  uint32_t           sampling_rate;
  /**< Output sampling rate in Hz for the Module. */
  uint32_t           num_channels;
  /**< Number of output channels for the Module. */
    uint8_t            channel_mapping[AFE_PORT_MAX_CHANNEL_CNT];
  /**< Chanel mapping of the Module. */
  uint32_t           bits_per_sample;
  /**< Number bits per sample for the Module.*/
}topo_send_output_media_fmt_t;

/* Topo Handle used to access topo APIs */
typedef void* topo_handle_t;

/** @brief Structure to represent the node of the
 *  	   topology. */
typedef struct topo_node_t
{
  uint32_t 						module_id;
  /**< Module id */

  uint16_t 						instance_id;
  /**< Instance id of the Module id */

  bool_t 							is_in_place;
  /**< Is this module inplace */

  bool_t          		requires_data_buffering;
  /**< Is the algo requires buffering */

  bool_t          is_media_fmt_conv_capability;
  /**< Module has the capability to change media format*/

  capi_v2_num_needed_framework_extensions_t	num_extensions;
  /**< Number of Custom frameworks supported by the module */

  capi_v2_framework_extension_id_t	*fwk_extn_ptr;
  /**< Custom frameworks supported by the module */

  uint32_t						kpps;
  /**< kpps for the module */

  uint32_t						bps;
  /**< Bps (Bytes per Sec) for the module */

  uint32_t 						delay_in_us;
  /**< algorithm delay of the module*/

  uint32_t        		in_buf_index;
  /**< Index of input buffer */

  uint32_t        		out_buf_index;
  /**< Index of output buffer */

  capi_v2_t						*module_ptr;
  /**< Pointer to the module */

  uint32_t          feedback_handle;
  /**< Feedback path handle corresponding to this module*/

  module_bgt_info_t *bgt_info_ptr;
  /**< Background thread info for this module */

  QURT_ELITE_HEAP_ID heap_id;
  /**< Heap ID for module memory allocations */

  topo_cdc_int_info_t cdc_int_info;
  /**< Codec interrupts need to monitored for the module */

  topo_cdc_client_t *cdc_client_ptr;
  /**< Client info for codec interrupt handling */

  topo_module_event_handle_t event_handle;
  /**< Module Handle for events */

  topo_media_fmt_t output_media_fmt;
  /**< Output Media Format Supported by Module */

  bool_t is_output_media_event_raised;
  /**< Indicates if module raised output media format event*/

  uint32_t module_input_buf_size;
  /**< Modules input buffer size requirement*/

  uint32_t module_ouput_buf_size;
  /**< Modules output buffer size requirement*/

  bool_t is_enabled;
  /**< Indicates if module is enabled*/
}topo_node_t;
/** @brief Structure for storing a handle from AMDB. */ 


typedef enum
{
   TOPO_STATE_STOP = 0,   /**< Topo Stop state. */
   TOPO_STATE_START /**< Topo Start state. */
} topo_state_t;


/** @brief Structure to represent the topology. */
struct topo_t
{
  topo_node_t 		*module;
  /**< Array of modules in the topology */

  uint32_t 			  num_modules;
  /**< Number of modules in the topology */

  uint32_t			  kpps;
  /**< aggregated kpps for the topology */

  uint32_t			  bps;
  /**< aggregated Bps (Bytes per Sec) for the topology */

  uint32_t 			  delay_in_us;
  /**< delay in us for the topology */

  uint32_t			  bit_width;
  /**< bit width for the topology */

  uint32_t     		sampling_rate;
  /**< Sampling rate for the topology */

  uint32_t     		num_channels;
  /**< number of channels for the topology */

  uint32_t     		bytes_per_channel;
  /**< bytes per channel for the topology */

  uint32_t			  topo_input_buf_size;
  /**< input buffer size for the topology */

  uint32_t 			  stack_size;
  /**< Maximum stack size needed by topology */

  void				    *session_ptr;

  capi_v2_buf_t   buffer1[AFE_PORT_MAX_CHANNEL_CNT];
  /**< Intermediate buffer1 */

  capi_v2_stream_data_t buflist1;
  /**< Intermediate buffer list1 */

  capi_v2_buf_t   buffer2[AFE_PORT_MAX_CHANNEL_CNT];
  /**< Intermediate buffer2 */

  capi_v2_stream_data_t buflist2;
  /**< Intermediate buffer list 2 */

  topo_state_t    topo_state;
  /**< Topology State */

  uint32_t internal_buf_size;
  /**< Topology temp buffer size*/

  uint8_t *internal_buf_ptr;
  /**< Topology temp buffer pointer*/

};

typedef enum
{
  TOPO_CB_TYPE_INVALID = 0, /**< Invalid TOPO call back Type*/
  TOPO_CB_TYPE_PRE,         /**< TOPO call back Type - PRE  regular function */
  TOPO_CB_TYPE_POST,        /**< TOPO call back Type - POST regular function */
  TOPO_CB_TYPE_MAX = 0x7FFFFFFF
} topo_cb_type;


typedef ADSPResult (*topo_cb_f)(void *context_ptr,
                                void *cb_arg_ptr,
                                topo_cb_type cb_type);

typedef struct topo_callback_info_t topo_callback_info_t;
struct topo_callback_info_t
{
   topo_cb_f 		 cb_f;    /* Callback function to be used in topology */
   void				   *cb_arg; /* call back function argument */
};

typedef ADSPResult (*topo_ack_cdc_int_cb_f)(void *client_info_ptr);


/* Topo Definition used to create a Topo instance */
typedef afe_topology_definition_t topology_definition_t;

/*
  This function constructs the topology for the given topology definition

  @param[out]    handle: Returned for the topology constructed
  @param[in/out] topo_def_ptr: pointer to topo definition structure
  @param[in]     event_cb_f: CAPI_V2 Event callback function
  @param[in]     session_ptr: Session pointer of the client
  @param[in]     cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_create(topo_handle_t *handle,
                       topology_definition_t *topo_def_ptr,
                       capi_v2_event_cb_f event_cb_f,
                       void *session_ptr,
                       topo_callback_info_t *cb_info_ptr);

/*
  This function constructs the topology for the given common db
  topology definition
 
  @param[out]    handle: Returned for the topology constructed
  @param[in/out] topo_def_ptr: pointer to topo definition structure
  @param[in]     event_cb_f: CAPI_V2 Event callback function
  @param[in]     session_ptr: Session pointer of the client
  @param[in]     cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_create_common_db(topo_handle_t *handle,
                       elite_cmn_topo_db_entry_t *topology_ptr,
                       capi_v2_event_cb_f event_cb_f,
                       void *session_ptr,
                       topo_callback_info_t *cb_info_ptr);

/*
  This function destroys the topology 

  @param[in/out]  handle: Handle returned during topology creation
  @param[in]      cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_destroy(topo_handle_t handle,
                        topo_callback_info_t *cb_info_ptr);


/*
  This function sets paramers for a given module in the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  module_id: module id
  @param[in]  instance_id: instance id
  @param[in]  param_id: Param id
  @param[in]  param_buf_len: Size of the parameter
  @param[in]  param_buf_ptr: pointer to calibration data payload
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_set_param(topo_handle_t handle,
                          uint32_t module_id,
                          uint16_t instance_id,
                          uint32_t param_id,
                          uint32_t param_buf_len,
                          int8_t *param_buf_ptr,
                          topo_callback_info_t *cb_info_ptr);

/*
  This function gets paramers for a given module in the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  module_id: module id
  @param[in]  instance_id: instance id
  @param[in]  param_id: Param id
  @param[in]  param_buf_len: Size of the parameter buffer
  @param[in]  param_buf_ptr: pointer to calibration data payload
  @param[out] actual_param_buf_len: ptr to the parameter buffer length
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_get_param(topo_handle_t handle,
                          uint32_t module_id,
                          uint16_t instance_id,
                          uint32_t param_id,
                          int32_t param_buf_len,
                          int8_t *param_buf_ptr,
                          uint32_t *actual_param_buf_len,
                          topo_callback_info_t *cb_info_ptr);

/*
  This function starts the topology and sends the necessary initial information for the topology 

  @param[in]  handle: Handle returned during topology creation
  @param[in]  media_fmt_ptr: Pointer to the media fmt describing data stream
  @param[in]  frame_size_in_sample: Frame size in samples
  @param[in]  cb_info_ptr: Pointer to a list of call back function informations
  @param[in]  num_cb_fn: Number fo call back function in the above list
  @param[in]  cb_info_ptr: Pointer to callback function
  
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_start(topo_handle_t handle,
                      topo_media_fmt_t *media_fmt_ptr,
                      uint32_t frame_size_in_samples,
                      topo_callback_info_t *cb_info_ptr);

/*
  This function stops the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  cb_info_ptr: Pointer to callback function
  
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_stop(topo_handle_t handle,
                     topo_callback_info_t *cb_info_ptr);

/*
  This function processes the topology

  @param[in]  handle: Handle returned during topology creation
  @param[in]  input_buf_ptr: pointer to the input buffer
  @param[in]  output_buf_ptr: pointer to the output buffer
  @param[in]  cb_info_ptr: Pointer to callback function
  @param[in]  timestamp: timestamp that need to be propagated to Modules
  @param[in]  is_timestamp_valid: Indicates if timestamp is valid or not

  @return
  Indication of success or failure.

  @dependencies
  None.
*/

ADSPResult topo_process(topo_handle_t handle,
                        int8_t *input_buf_ptr,
                        int8_t *output_buf_ptr,
                        topo_callback_info_t *cb_info_ptr,
                        uint64_t timestamp,
                        bool_t is_timestamp_valid);

/*
  This function set media type of input data

  @param[in]  handle: Handle returned during topology creation
  @param[in]  media_type_ptr: pointer to data media type
  @param[in]  cb_info_ptr: Pointer to callback function

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult topo_set_input_media_type(topo_handle_t handle,
                                     topo_media_fmt_t *media_fmt_ptr,
                                     topo_callback_info_t *cb_info_ptr);

/*
  This function return module inded

  @param[in]     cb_handle: Handle returned during Event call back

  @return
  Module index

  @dependencies
  None.
*/
uint32_t topo_event_get_module_index(void *handle);

/*
  This function return Topo handle

  @param[in]     cb_handle: Handle returned during Event call back

  @return
  Handle returned during topology creation (in true type)

  @dependencies
  None.
*/
/*TODO: Avoid returning entire topo structure, return structure containing
        required data - as in subset like mcps, kpps, algo_delay, etc*/
topo_t* topo_event_get_struct_ptr(void *handle);

/*
  This function sets codec interrupt and relavant information to the modules which are
  registered for particular interrupts in the topology

  @param[in]  handle: pointer to the topology constructed
  @param[in]  int_id : Interrupt id
  @param[out]  param_size : Size of the codec interrupt related information
  @param[out]  data_buf_ptr : pointer to buffer 

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_cdc_int_set_param(topo_handle_t handle, uint32_t int_id, uint32_t param_size, int8_t *data_buf_ptr);

/*
  This function acks codec interrupt of the modules which are
  registered for particular interrupts in the topology

  @param[in]  handle: pointer to the topology constructed
  @param[in]  int_index : Interrupt id
  @param[in]  cb_f : call back function to do client specific action

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_ack_cdc_int_client_info(topo_handle_t handle, uint16_t int_index, topo_ack_cdc_int_cb_f cb_f);

/*
  This function Sends output media format to modules which support media format covertion

  @param[in]  handle: pointer to the topology constructed
  @param[in]  pParams : output media format parameters

  @return
  ADSPResult.

  @dependencies
  None.
*/
ADSPResult topo_send_capiv2_output_media_format (topo_handle_t handle, const topo_send_output_media_fmt_t *pParams);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_TOPO_UTILS_H_



