/*========================================================================
   This file contains functions to set the parameters for different modules.

Copyright (c) 2009-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/inc/AFECodecService.h#1 $
 ====================================================================== */
#ifndef _AFE_CDC_SVC_HANDLER_H_
#define _AFE_CDC_SVC_HANDLER_H_

#include "Elite.h"
/*==========================================================================
  Include Files
========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// codec interrupt indices
#define AFE_CDC_INT_INDEX_START   AFE_CDC_INT_MAD_AUDIO
#define AFE_CDC_INT_MAD_AUDIO    0x0
#define AFE_CDC_INT_MAD_BEACON    0x1
#define AFE_CDC_INT_MAD_ULT      0x2
#define AFE_CDC_INT_AUDIO_CLIP   0x3
#define AFE_CDC_INT_SPKR_AUDIO_CLIP   AFE_CDC_INT_AUDIO_CLIP
#define AFE_CDC_INT_VBAT_ATTACK  0x4
#define AFE_CDC_INT_VBAT_RELEASE 0x5
#define AFE_CDC_INT_SPKR2_AUDIO_CLIP 0x6
#define AFE_CDC_INT_INDEX_END   AFE_CDC_INT_SPKR2_AUDIO_CLIP
#define AFE_CDC_INT_INDEX_SIZE   (AFE_CDC_INT_INDEX_END - AFE_CDC_INT_INDEX_START + 1)

// macro check for the valid codec interrupt index
#define IS_VALID_CDC_INT_INDEX(int_index) (int_index <= AFE_CDC_INT_INDEX_END)

#define CDC_CLIP_MAX_REG_PER_BANK     8   /* Max number of registers present in a codec pcm level bank*/
#define CDC_CLIP_PCM_BUF_LEN          16

// codec int handler client info
typedef struct cdc_int_client_info
{
  uint16_t client_id;
  uint16_t client_handle;
  uint16_t int_index;
  uint16_t enable_int_ack_ctrl;
  qurt_elite_queue_t *client_q_ptr;
  qurt_elite_signal_t *client_signal_ptr;
} cdc_client_info_t;

// codec reg device config
typedef struct cdc_reg_dev_cfg
{
  uint32_t reg_addr;
  /* codec register address */
  uint32_t reg_field_bit_mask;
  /* accessible bit field mask */
  uint16_t reg_bit_width;
  /* bit width of register */
  uint16_t reg_offset_scale;
  /* Offset scale factor. eg, 0x400+1*n, where 1 is reg_offset_scale. */
} cdc_reg_dev_cfg_t;


#define AFE_CRC_GET_ABSOLUTE_ADDRESS(reg_addr,offset,port_num)             (reg_addr + (offset*port_num))
/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult afe_svc_set_cdc_dev_cfg(uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size);
ADSPResult afe_svc_cdc_handler_init(void);
ADSPResult afe_svc_cdc_handler_deinit(void);
ADSPResult afe_cdc_update_register(cdc_reg_dev_cfg_t *reg_cfg_ptr, uint8_t *value_ptr, uint16_t offset_multiplier);
cdc_reg_dev_cfg_t* afe_cdc_get_dev_cfg_ptr(uint32_t reg_field_type);

ADSPResult afe_port_get_pcmlevels_from_codec(uint8_t* cdc_clip_pcm_level);
ADSPResult afe_cdc_get_and_extract_pcm_levels_from_cdc(uint32_t clip_intr_id, int16_t* clip_pcm_level_buf_ptr, uint32_t *clip_pcm_buf_len_ptr);
ADSPResult afe_port_custom_cdc_int_handling(elite_msg_any_t* pMsg);
ADSPResult afe_cdc_intr_reg_handler(cdc_client_info_t *cdc_client_info_ptr, bool_t is_it_register);

ADSPResult afe_cdc_update_cdc_pwr_state_register(uint8_t *value_ptr);

/* This function to check whether codec interrupt handler supported or not.
   This function is prerequisite to call before registering for any codec interrupts
 
   @param[in]  none

   @result
   returns ADSP_EOK, if codec interrupts supported
   returns ADSP_EUNSUPPORTED, if codec interrupts not supported

  @dependencies
  None.
*/
ADSPResult afe_svc_check_codec_interrupts_supported(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AFE_CDC_SVC_HANDLER_H_

