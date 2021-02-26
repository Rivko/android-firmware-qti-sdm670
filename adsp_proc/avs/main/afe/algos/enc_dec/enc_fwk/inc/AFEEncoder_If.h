/*========================================================================
  This file contains functions/structures to interface with the encoder.

  Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/algos/enc_dec/enc_fwk/inc/AFEEncoder_If.h#1 $

  Edit History
  when           who  what, where, why
  --------           ---      -------------------------------------------------------
  08/09/16   KR     Created file.
  ========================================================================== */
#ifndef AFEENCODER_IF_H
#define AFEENCODER_IF_H

#include "AFEInternal.h"
#include "EliteLinkedList.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define AFE_TO_ENC_NUM_Q_ELEM        (32)
#define ENC_TO_AFE_NUM_Q_ELEM        (32)

typedef struct afe_port_encoder_params_t afe_port_encoder_params_t;
struct afe_port_encoder_params_t
{
	uint32_t                               enc_fmt_id;
	/**< Encoder format ID */
	uint32_t                               enc_cfg_blk_size;
	/**< Encoder config block size */
	void                                     *enc_cfg_blk_ptr;
    /**< Pointer to the payload of the encoder config block */
	uint32_t                               enc_packetizer_id;
	/**< Packetizer ID following the encoder */
	elite_svc_handle_t             *enc_handle_ptr;
	/**< Pointer to the handle of the encoder */
	qurt_elite_thread_t            enc_thread_id;
	/**< Encoder thread ID, used to wait on the thread join */
	qurt_elite_channel_t          afe_enc_channel;
	/**< Channel used for communication to and from the encoder */
	bool_t                                   is_afe_enc_channel_created;
	/**< Flag to keep track of whether the channel used for communication to and from the encoder was created or not */
	qurt_elite_queue_t             *afe_port_to_enc_q;
	/**< Pointer to the AFE->encoder queue */
	elite_svc_handle_t             enc_downstream_handle;
	/**< Encoder's downstream handle */
	uint32_t                               enc_ip_data_q_num_bufs;
	/**< Number of buffers between AFE and encoder */
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
	uint32_t                               num_rem_bytes_to_write_dest;
	/**< Current #bytes to write to dest */
	int8_t                 		              *working_buf_int_dev;
	/**< Intermediate device working buffer dev to copy from/to the DMA. Only applicable for I2S */
	int8_t                 		              *working_buf_dev;
	/**< Working device buffer to copy from/to the DMA */
	uint32_t                               working_buf_size_enc_in;
	/**< Port's working buffer size. applicable only for encoder's input queued buffers */
	uint32_t                               working_buf_size_int_dev;
	/**< Intermediate device's working buffer size. applicable only for working_buf_int_dev */
	uint32_t                               working_buf_size_dev;
	/**< Device's working buffer size. applicable only for working_buf_dev */
	volatile uint32_t               encoder_delay;
	/**< Delay caused by the encoder in microseconds. Variable is updated by encoder svc only based on initial zeros it sends */
	bool_t                                   data_flow_begun;
	/**< Flag to indicate that the data has started flowing */
	elite_linked_list_node_t                 *cached_params_list;
	/**< Cache of set param calls with custom parameters that have not yet been sent downstream. */
};

/*=============================================================================
 * FUNCTION         afe_create_enc_svc
 *
 * DESCRIPTION    function to create the encoder svc and all the associated queues,
 *  buffers and establish connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult.
 ===============================================================================*/
ADSPResult afe_create_enc_svc(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_destroy_enc_svc
 *
 * DESCRIPTION    function to destroy the encoder svc and all the associated queues,
 *  buffers and remove connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_destroy_enc_svc(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_set_enc_params
 *
 * DESCRIPTION    function to set parameters on the encoder svc
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
ADSPResult afe_set_enc_params(afe_dev_port_t *pDevPort, uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size);

/*=============================================================================
 * FUNCTION         afe_send_and_get_data_encoder
 *
 * DESCRIPTION    function to send and receive data from the encoder from the steady state rendering routine in AFE
 *
 * @param[in]       pDevPort device port instance.
 * @param[in]       src_ptr   Pointer to the start of the data from which data is copied to the encoder
 * @param[in]       dest_ptr Pointer to the start of the data into which data is copied from the encoder
 * @param[in]       bytes_needed Size of the data being copied
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_send_and_get_data_encoder(afe_dev_port_t  *pDevPort, int8_t *src_ptr, int8_t *dest_ptr, uint32_t bytes_needed);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AFEENCODER_IF_H
