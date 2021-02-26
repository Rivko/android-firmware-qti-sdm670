/*========================================================================
  This file contains functions/structures to interface with the decoder.

  Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/algos/enc_dec/dec_fwk/inc/AFEDecoder_If.h#1 $

  Edit History
  when           who  what, where, why
  --------           ---      -------------------------------------------------------
  08/19/16   KR     Created file.
  ========================================================================== */
#ifndef AFEDECODER_IF_H
#define AFEDECODER_IF_H

#include "AFEInternal.h"
#include "EliteLinkedList.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define AFE_TO_DEC_NUM_Q_ELEM        (32)
#define DEC_TO_AFE_NUM_Q_ELEM        (32)

typedef struct afe_port_decoder_params_t afe_port_decoder_params_t;
struct afe_port_decoder_params_t
{
	uint32_t                               dec_fmt_id;
	/**< Decoder format ID */
	uint32_t                               dec_depacketizer_id;
	/**< De-packetizer ID within the decoder */
	elite_svc_handle_t             *dec_handle_ptr;
	/**< Pointer to the handle of the decoder */
	qurt_elite_thread_t            dec_thread_id;
	/**< Decoder thread ID, used to wait on the thread join */
	qurt_elite_channel_t          afe_dec_channel;
	/**< Channel used for communication to and from the decoder */
	bool_t                                   is_afe_dec_channel_created;
	/**< Flag to keep track of whether the channel used for communication to and from the decoder was created or not */
	qurt_elite_queue_t             *afe_dev_to_dec_q;
	/**< Pointer to the AFE->decoder queue */
	elite_svc_handle_t             dec_downstream_handle;
	/**< Decoder's downstream handle */
	uint32_t                               dec_ip_data_q_num_bufs;
	/**< Number of buffers between AFE and decoder */
	elite_msg_any_t                 my_data_q_msg;
	/**< Holds the current data Q msg being processed */
	elite_msg_data_buffer_t   *buf_ptr;
	/**< Pointer to the current buffer being processed */
	int8_t                                   *current_read_ptr;
	/**< Current read pointer */
	int8_t                                   *current_write_ptr;
	/**< Current write pointer */
	uint32_t                               num_rem_bytes_to_read_src;
	/**< Current #bytes to read from src */
	uint32_t                               num_bytes_channel_spacing_src;
	/**< The channel spacing (in bytes) at the source */
	uint32_t                               num_rem_bytes_to_write_dest;
	/**< Current #bytes to write to dest */
	uint32_t                               num_bytes_channel_spacing_dest;
	/**< The channel spacing (in bytes) at the dest */
	int8_t                 		              *working_buf_int_dev;
	/**< Intermediate device working buffer dev to copy from/to the DMA */
	int8_t                 		              *working_buf_dev;
	/**< Working device buffer to copy from/to the DMA */
	uint32_t                               working_buf_size_dec_in;
	/**< Device's working buffer size. applicable only for decoder's input queued buffers */
	uint32_t                               working_buf_size_int_dev;
	/**< Intermediate device's working buffer size. applicable only for working_buf_int_dev */
	uint32_t                               working_buf_size_dev;
	/**< Fevice's working buffer size. applicable only for working_buf_dev */
	bool_t                                   is_drop_data_due_to_incorrect_media_type;
	/**< Flag to indicate that the decoder's output will be dropped because the media type is incorrect */
	volatile uint32_t               decoder_delay;
	/**< Delay variable shared with decoder svc */
	uint32_t                               prev_delay;
	/**< Previous value of delay read by AFE */
	bool_t                                   data_flow_begun;
	/**< Flag to indicate that the data has started flowing */
	elite_linked_list_node_t                 *cached_params_list;
	/**< Cache of set param calls with custom parameters that have not yet been sent downstream. */
};

/*=============================================================================
 * FUNCTION         afe_create_dec_svc
 *
 * DESCRIPTION    function to create the decoder svc and all the associated queues,
 *  buffers and establish connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult.
 ===============================================================================*/
ADSPResult afe_create_dec_svc(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_destroy_dec_svc
 *
 * DESCRIPTION    function to destroy the decoder svc and all the associated queues,
 *  buffers and remove connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_destroy_dec_svc(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_set_dec_params
 *
 * DESCRIPTION    function to set parameters on the decoder svc
 *
 * @param[in]       pDevPort device port instance.
 * @param[in]       param_id unique paramID for this parameter.
 * @param[in]       param_buffer_ptr Pointer to the start of the parameter buffer.
 * @param[in]       param_size Size of the parameter.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult.
 ===============================================================================*/
ADSPResult afe_set_dec_params(afe_dev_port_t *pDevPort, uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size);

/*=============================================================================
 * FUNCTION         afe_send_and_get_data_decoder
 *
 * DESCRIPTION    function to send and receive data from the decoder from the steady state rendering routine in AFE
 *
 * @param[in]       pDevPort device port instance.
 * @param[in]       src_ptr   Pointer to the start of the data from which data is copied to the decoder
 * @param[in]       dest_ptr Pointer to the start of the data into which data is copied from the decoder
 * @param[in]       bytes_needed Size of the data being copied
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_send_and_get_data_decoder(afe_dev_port_t  *pDevPort, int8_t *src_ptr, int8_t *dest_ptr, uint32_t bytes_needed);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AFEDECODER_IF_H
