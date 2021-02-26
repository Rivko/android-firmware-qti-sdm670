/*========================================================================
  This file contains AFE Port processing related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEPortManager.h#1 $
 ====================================================================== */
#ifndef _AFE_PORTMANAGER_H_
#define _AFE_PORTMANAGER_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInterface.h"
#include "AFESampRateConv.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */
/** Maximum number of the resamplers equals to maximum number of the
	 channels supported.
 */
#define MAX_NUM_OF_RESAMPLERS  (MAX_AUDIO_CHANNELS)

/**
 * Maximum number of PCM buffers held. should be power of 2.
 */
#define AFE_MAX_PCM_BUFS   4

typedef enum afe_client_state
{
	AFE_CLIENT_ENABLE = 0,
	AFE_CLIENT_DISABLE,
	AFE_CLIENT_RESET,
}afe_client_state_e;

/**
 * Encapsulation of AVtimer drift based rate matching mainly for AFE loopback
 */
typedef struct afe_rate_match_info
{
	volatile uint64_t                 *client_avt_drift_info;
	/**< drift ptr. When rate matching is enabled,
	 * this drift ptr is compared with, this port's drift ptr
	 * and rate matching is done.  */
	afe_client_avt_drift_info_t       client_prev_drift;
	/**< this client's previous value of the avt drift */
	afe_client_avt_drift_info_t       port_prev_drift;
	/**< this port's previous value of the avt drift wrt this client*/
	int32_t                           avt_drift_mem_us;
	/**< drift memory between the (rx) port and the client (tx port) (in a loopback)*/
	uint32_t                          us_per_sample;
	/**< us per sample of the client */
	int8_t                            *data_buf_ptr;
	/**< buffer to copy data from client when rate matching */
	uint32_t                          data_buf_size;
	/**< Rate matching buffer size */
	uint16_t                          num_buffers_released; //these fields get reset when client media format changes, when rx port is restarted, when tx port is restarted(since reconnect happens)
	/**< number of buffers released before starting to collect tx port buffers = 1/2 number of buffer tx port creates. */
	uint16_t                          num_buffers_collected;
	/**< number of buffers collected on rx side = 1/2 the number of buffers created at tx side */
	elite_msg_data_buffer_t           *pcm_bufs[AFE_MAX_PCM_BUFS];
	/**< circular buffer of PCM buffer ptrs from tx port.*/
	uint8_t                           curr_pcm_buf_rd_ind;
	/**< read index of above array. this will lag behind the below wr_ind */
	uint8_t                           curr_pcm_buf_wr_ind;
	/**< write index of above array: both differ by 1/2 the number of tx port buffers */
	void                              *ss_cfg_ptr[MAX_AUDIO_CHANNELS];
	/**< sample slipping configuration structure */
	void                              *ss_data_ptr[MAX_AUDIO_CHANNELS];
	/**< sample slipping static data structure */
	uint16_t                          num_multi_frame;
	/**< for multi-frame sample slip, corrections should be done only once a AFE_SAMPLE_SLIP_MULTI_FRAMES frame). this counter tracks it.*/
	uint8_t                           num_buff_to_collect;
	/**< Number of buffers to collect before sample slip, if fractional resampling is
	 *   needed then two buffers need to be collected ow one */
} afe_rate_match_info_t;

/** @brief Used in the AFE to store information about the clients that
  connect to the AFE.
 */
typedef struct afe_client_info
{
	uint32_t                          buf_size;
	/**< Buffer size of each buffer in
                                          samples/channel; must be an integer
                                          multiple of the number of samples
                                          per interrupt. */
	uint32_t                          int_samples_per_period;
	/**< Number of the samples per interrupt;
                                          equals unSampleRate *
                                          blockTransferTime. */
	uint32_t                          remaining_samples;
	/**< Tx path: remaining samples to be
                                          filled in the PCM buffer;
                                          Rx path: remaining samples to be
                                          rendered in the PCM buffer. */
	int8_t                            *curr_buff_ptr;
	/**< Pointer to the current sample being
                                          processed. */
	elite_msg_data_buffer_t           *ps_curr_pcm_buff;
	/**< Pointer to the current PCM buffer
                                          being processed. */
	qurt_elite_queue_t                *bufQ;
	/**< Tx path: buffer queue over which more
                                          buffers are extracted;
                                          Rx path: not applicable. */
	qurt_elite_channel_t              qurt_elite_channel;
	/**< Channel for registering buffer queues
                                          or data queues.*/
	elite_svc_handle_t                this_client_manager;
	/**< Contains the AFE's handle provided
                                          to the client. */
	afe_client_t                      afe_client;
	/**< Client information from the client.*/

	samp_rate_conv_t				       samp_rate_conv;
	/**< sampling rate converter Information */

	uint16_t						     		 unNumResamplers;
	/**< Number of resamplers initialized for
                                          this client. */
	afe_client_state_e                state;
	/**< Bit signal if the client is
                                          disabled (1) or not (0). */
	uint32_t                          client_id;
	/**< Unique Client ID -> the address of this client */
	uint64_t                          prev_time;
	/**< Previous time read from the DMA. */
	uint64_t 						       client_gp_delay_us;
	/**< client specific group delay */
	uint32_t                          under_run_counter;
	/**< Underrun counter. */
	int32_t                           actual_buf_size;
	/**< size of the actual buffer allocated (tx) */
	afe_rate_match_info_t             *rate_match_info_ptr;
	/**< rate matching related info */
	bool_t                            client_disconnect_marker_flag;
	/**< client disconnect indicator flag */
	void                              *pop_suppressor_module_ptr;
	/**< instance pointer for pop suppressor module */
	uint32_t                          client_kpps;    /**This variable is used to store individual client kpps during client connect.
                                  Later during client disconnect, this value is deducted from aggregated client kpps.
                                  client_kpps includes client framework kpps (and client re-sampler kpps,if resampler is needed)
                                  client_kpps unit is kpps*/
	uint32_t                          client_bw;      /**This variable is used to store individual client bandwidth during client connect.
                                  Later during client disconnect, this value is deducted from aggregated client bandwidth.
                                  client_bw includes client framework bw (and client re-sampler bw, if resampler is needed)
                                  client_bw unit is Bytes per second*/
} afe_client_info_t;

/**
 * Function that renders the compressed data through the given audio interface  in thread safe manner.
 *
 * @param[in] vpDevPort, is the pointer to the port instanc of type afe_dev_port_t
 *
 * @return ADSP status result
 */
ADSPResult afe_port_render_compressed_data_thread_safe(void *vpDevPort);

/**
 * Function that forwards the compressed data through the given 
 * audio interface  in thread safe manner. 
 *
 * @param[in] vpDevPort, is the pointer to the port instanc of type afe_dev_port_t
 *
 * @return ADSP status result
 */
ADSPResult afe_port_forward_compressed_data_thread_safe(void *vpDevPort);


/**
 * Function that renders PCM samples in RX path and also forwards the EC reference signals
 * in a threadsafe manner.
 *
 * @param[in] vpDevPort, is the pointer to the port instanc of type afe_dev_port_t
 *
 * @return ADSP status result
 */

ADSPResult afe_port_render_pcm_samples_thread_safe(void *vpDevPort);

/**
 * Multilpexing the sub-device data and render it to the device 
 * in a threadsafe manner. 
 *
 * @param[in] p_group_dev_state, pointer to group device object 
 *
 * @return ADSP status result
 */

ADSPResult afe_group_device_render_pcm_samples_thread_safe(void *p_group_dev_state);

/**
 * Multilpexing the sub-device data and render it to the device 
 * in a threadsafe manner. 
 *
 * @param[in] p_group_dev_state, pointer to group device object 
 *
 * @return ADSP status result
 */
ADSPResult afe_group_device_forward_pcm_samples_thread_safe(void *p_group_dev_state);

/**
 * handle slot mapping for render path
 *
 * @param[in] p_dev_port, pointer to AFE port object 
 * @param[in] p_header_filled, pointer to bool object for SAD header related 
 *
 * @return ADSP status result
 */

ADSPResult afe_slot_mapping_rx(void *p_dev_port, bool_t *p_header_filled);

/**
 * handle slot mapping for forward path
 *
 * @param[in] p_dev_port, pointer to AFE port object 
 *
 * @return ADSP status result
 */
ADSPResult afe_slot_mapping_tx(void *p_dev_port);


/**
 * Function that forwards the port PCM samples upstream
 * in a threadsafe manner.
 *
 * @param[in] vpDevPort, is the pointer to the port instance of type afe_dev_port_t
 *
 * @return ADSP status result
 */
ADSPResult afe_port_forward_pcm_samples_thread_safe(void *vpDevPort);

/**
 * function that handles custom messages to a port.
 * @param[in] vpDevPort, is the pointer to the port instance of type afe_dev_port_t
 * @param[in] pMsg, message to be handled
 *
 * @return ADSP status result
 */
ADSPResult afe_port_custom_msg_handler(void* vpDevPort, elite_msg_any_t* pMsg);

/**
 * function that handles APR messages to a port.
 * @param[in] vpDevPort, is the pointer to the port instance of type afe_dev_port_t
 * @param[in] pMsg, message to be handled
 *
 * @return ADSP status result
 */
ADSPResult afe_port_apr_msg_handler(void* pDevPort, elite_msg_any_t* pMsg);

ADSPResult afe_port_client_connect_handler(void* pDevPort, elite_msg_any_t* pMsg);

ADSPResult afe_port_client_disconnect_handler (void* pDevPort, elite_msg_any_t *pMsg);

ADSPResult afe_port_register_client_disconnect_marker_handler (void* pDevPort, elite_msg_any_t *pMsg);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AFE_PORTMANAGER_H_
