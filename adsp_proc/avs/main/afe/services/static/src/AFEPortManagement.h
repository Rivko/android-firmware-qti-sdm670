/*========================================================================
  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History
  when       who     what, where, why
  --------   ---     -------------------------------------------------------

  ========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_Data.h"
#include "adsp_error_codes.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"
#include "adsp_afe_service_commands.h"
#include "AFEDevService.h"
#include "AFEPortManager.h"
#include "AFEInternal.h"
#include "AFELoggingUtils.h"
#include "AFESvcAprIf.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEDeviceDriver.h"
#include "AFEGroupDeviceDriver.h"
#include "limiter24_api.h"
#include "AFECodecService.h"
#include "AFEHwMadDriver.h"
#include "AFESwMad.h"
#include "VfrDriverInternal.h"
#include "adsp_avs_common_api.h"
#include "adsp_prv_avs_common_api.h"

#ifndef AFEPORTMANAGEMENT_H
#define AFEPORTMANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Define constants
========================================================================== */
#define AFE_DEV_DATQ_MAX   			16
#define AFE_DEV_BUFQ_MAX   			32
#define AFE_UNDERRUN_TIME_THRESH 	10000 	/* In micro-sec: 10 ms 							*/
#define CLIENT_BUF_SIZE_FACTOR		1  	   /* Mult factor for samples per interrupt 	*/

#define AFE_PRV_MODULE_AVSYNC_TEST		0x0001021A
#define AFE_PRV_PARAM_ID_AVSYNC_STATS	0x0001021B
typedef struct afe_prv_avsync_stats_param
{
	uint32_t 		non_zero_ts_lsw;
	uint32_t 		non_zero_ts_msw;
	uint16_t       is_valid;
	uint16_t       reserved;
}afe_prv_avsync_stats_param_t;

typedef struct afe_avsync_stats
{
	uint64_t nonzero_timestamp;
}afe_avsync_stats_t;

/**
 * number of buffers in loopback clients (flag is used by both tx and rx):
 * 1 held by rx, 1 being filled by tx, 1 being used by rx.
 * 2 for drift (Need two buffers to collect in rate matching algorithm as we can enter the scenario where
 * two Rx intr can occur in between two Tx intr, thereby rate match algo can underrun.)
 * In order to resolve this, we will collect one more buffer and in effect it will introduce 1 frame delay.
 */
#define AFE_LOOPBACK_NUM_CLIENT_BUFS   5

/**
 * enable rate matching in loopback or not
 */
#define ENABLE_LOOPBACK_RATE_MATCHING  TRUE

#define BYTE_UP_CONV							1
#define BYTE_DOWN_CONV						2

#define BYTE_DOWN_CONV_SHIFT_FACT_DEFAULT 	(PCM_32BIT_Q_FORMAT - PCM_16BIT_Q_FORMAT) // (Q31 - Q15) for 32/24bit to 16bit conversion
#define BYTE_DOWN_CONV_SHIFT_FACT_24_BIT     (PCM_32BIT_Q_FORMAT - ELITE_32BIT_PCM_Q_FORMAT) // (Q31 - Q27) for 32bit to 24bit conversion

#define BYTE_DOWN_CONV_RND_FACT_DEFAULT      1 << (PCM_32BIT_Q_FORMAT - PCM_16BIT_Q_FORMAT - 1) //0x8000 for 32bit/24bit to 16bit conversion
#define BYTE_DOWN_CONV_RND_FACT_24_BIT      1 << (PCM_32BIT_Q_FORMAT - ELITE_32BIT_PCM_Q_FORMAT - 1) //0x8 for 32bit to 24bit conversion

#define AFE_ALIGNMENT_MASK_8_BYTE     0x7
#define AFE_ALIGNMENT_MASK_4_BYTE     0x3

// define the time stamp flag 
#define AFE_PORT_CLIENT_BUF_VALID_TIME_STAMP_FLAG  (1<<31)

/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult afe_port_client_disable_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg);
ADSPResult afe_port_client_enable_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg);
ADSPResult afe_port_client_dc_reset_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg);
ADSPResult afe_port_client_criteria_register_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg);
void afe_port_populate_tx_client_buf(afe_client_info_t *pClientInfo, afe_dev_port_t* pDevPort,
		samp_rate_conv_t *paResampler, int8_t * ptr_src_port_buf,
		int8_t *ptr_scratch_buf);
ADSPResult afe_port_add_to_rx_client_list(afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg, bufQList **list_node,
		int16_t *is_queue_init);
void       afe_port_tx_client_disconnect_handler (afe_dev_port_t* pDevPort, \
		bufQList* list_node, elite_msg_any_t *pMsg, int16_t is_deinit_queue, uint32_t num_buffers);
void       afe_port_rx_client_disconnect_handler (afe_dev_port_t* pDevPort,\
		bufQList* list_node, elite_msg_any_t *pMsg, int16_t is_deinit_queue);
bufQList*  afe_port_search_unlink_client(bufQList **ppQNode,elite_svc_handle_t  *svc_handle_ptr);
void       afe_port_add_to_client_list(afe_client_list **pClientList, bufQList *pQNode);
ADSPResult afe_port_get_new_buffer( afe_client_info_t *psClientInfo, qurt_elite_queue_t* bufQ);
ADSPResult  afe_port_create_buf(afe_dev_port_t *pDevPort, afe_client_info_t *psNewClient,
		int16_t buf_size_scale, int16_t buf_size_additional, int16_t *is_queue_init, \
		uint32_t *num_buffers_created );
void afe_port_data_buf_mask_init(afe_dev_port_t* pDevPort);
ADSPResult afe_port_send_media_type_to_tx_clients(afe_dev_port_t *pDevPort);
ADSPResult afe_port_client_reinit_at_port_start(afe_dev_port_t* pDevPort);
ADSPResult afe_port_compressed_client_voting_at_port_start(afe_dev_port_t* pDevPort);
ADSPResult afe_port_send_last_buf_and_eos(afe_client_info_t *client_info,afe_dev_port_t* dev_port_ptr);
ADSPResult afe_port_send_eos_to_tx_clients(afe_dev_port_t* pDevPort);
ADSPResult afe_port_generate_tx_eos(afe_client_info_t *client_info);
ADSPResult afe_port_rx_client_resample_init(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo);
ADSPResult afe_port_tx_client_resample_init(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo);
ADSPResult afe_port_render_pcm_samples(afe_dev_port_t *pDevPort);
ADSPResult afe_port_forward_pcm_samples(afe_dev_port_t *pDevPort);
ADSPResult afe_port_data_cmd(afe_client_info_t *pClientInfo, afe_dev_port_t* pDevPort, elite_msg_any_t* pMsg, bool cache_media_type);
void afe_port_interleave_samples(int8_t* src_addr, int8_t* dst_addr, int16_t num_channels, 
		uint32_t num_samples_per_ch, int16_t bytes_per_channel);
void afe_port_de_interleave_samples(int8_t* src_addr, int8_t* dst_addr, int16_t num_channels, 
		uint32_t num_samples_per_ch, int16_t bytes_per_channel);
void afe_port_stereo2mono_by_avg(int8_t *left, int8_t *right, int8_t* mono, uint32_t numSamples, uint16_t bytes_per_channel);
void afe_port_sample_up_down_convertor(afe_dev_port_t *pDevPort, int8_t * ptr_src_buf, int8_t * ptr_out_buf, uint32_t num_samp_per_ch,
		int16_t num_channels, uint32_t chan_spacing_in, uint32_t chan_spacing_out, uint32_t client_bit_width,
		uint16_t conv_mode);
ADSPResult afe_port_client_loopback_connect_handler(afe_dev_port_t* pDevPort, elite_msg_any_t* pMsg);
ADSPResult afe_port_enable_loopback(afe_dev_port_t* pDevPort, void *loopback_cfg_param);
ADSPResult afe_port_disable_loopback(afe_dev_port_t* pDevPort, void *loopback_cfg_param);
ADSPResult afe_port_reconfig_loopback(afe_dev_port_t *pDevPort);
ADSPResult afe_port_get_loopback_module_params(afe_dev_port_t *pDevPort, uint32_t param_id,
		int8_t* param_buffer_ptr, int32_t param_buf_len,
		uint16_t* params_buffer_len_req_ptr);
ADSPResult afe_port_set_loopback_module_params(afe_dev_port_t *pDevPort, uint32_t param_id,
		int8_t* params_buffer_ptr, uint16_t param_size);
void afe_port_apply_capture_time_stamp(afe_dev_port_t *pDevPort, afe_client_info_t *client);
void afe_port_calc_cmn_group_delay(afe_dev_port_t *pDevPort);
void afe_port_calc_client_gp_delay_us(afe_client_info_t *client);
ADSPResult afe_port_set_group_delay(afe_dev_port_t *pDevPort);


ADSPResult afe_svc_custom_msg_handler(void* pAudStatAfeSvc, elite_msg_any_t* pMsg);
ADSPResult afe_svc_apr_msg_handler (void* pAudStatAfeSvc, elite_msg_any_t* pMsg);

ADSPResult afe_port_get_avsync_test_prv_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
		int8_t* param_buffer_ptr, int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr);
ADSPResult afe_port_set_avsync_test_prv_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
		int8_t *param_buffer_ptr, uint16_t param_size);
void afe_port_collect_avsync_stats(afe_dev_port_t *pDevPort, int8_t *buffer);
void afe_port_update_timestamp(afe_dev_port_t *dev_port_ptr);
void afe_log_buf_latency(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo);
void afe_port_get_render_time_stamp(afe_dev_port_t *pDevPort, afe_client_info_t *client, uint64_t *time_stamp);
ADSPResult afe_cont_buf_latency_init(afe_dev_port *pDevPort);
ADSPResult afe_cont_buf_latency_deinit(afe_dev_port *pDevPort);
ADSPResult afe_cont_buf_latency_log(afe_dev_port *pDevPort, afe_client_info_t *pClientInfo);

ADSPResult afe_port_update_client_media_fmt(afe_dev_port_t *p_dev_port, elite_msg_any_t *pMsg);

ADSPResult afe_port_data_cmd_eos(afe_client_info_t *pClientInfo,
		afe_dev_port_t *pDevPort,
		elite_msg_any_t *pMsg);

ADSPResult afe_port_send_client_media_fmt_update(afe_dev_port_t *pDevPort,
		afe_client_info_t *pClientInfo);
/**
  Validate the port data type given the client data type.

  @param[in] pDevPort - the afe port to be validated.
  @param[in] pClient  - the afe client to be validated.

  @return
  ADSP_EOK if the port data type matches the client data type.
  ADSP_EBADPARAM for all failures.

  @dependencies
  None.
 */
ADSPResult afe_port_client_data_type_validation(afe_dev_port_t *pDevPort, afe_client_t *pClient);

/**
 * AVT drift update related functions
 */
void afe_port_update_avt_drift(afe_dev_port_t *dev_port_ptr);
void afe_port_send_avt_drift_update(afe_dev_port_t *dev_port_ptr, afe_client_info_t *client_info_ptr);
ADSPResult afe_port_get_drift_params(void *dev_port, uint32_t param_id, int8_t* param_buffer_ptr,
		int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr);
ADSPResult afe_port_reset_client_state(afe_dev_port_t *p_dev_port);

/**
  This function set the params for the module: AFE_MODULE_PORT.

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration data to fetch.
  @param[in] param_size  Actual Param size

  @return
  ADSP_EOK/ error code depends on success/failure.

  @dependencies
  None.
 */
ADSPResult afe_port_set_port_module_params(afe_dev_port_t *pDevPort,uint32_t param_id, int8_t *param_buffer_ptr, uint32_t param_size);

typedef enum
{
	PORT_MODULE_MEDIA_TYPE_V1 = 0x1
}afe_port_module_media_type_minor_version;

/** A utility function to reset the port's media type */
void afe_port_reset_media_type(afe_dev_port_t* pDevPort);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AFEPORTMANAGEMENT_H
