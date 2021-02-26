/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFECodecHandler.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "topo_utils.h"
#include "AFESvc_topo.h"

/*==========================================================================
  Functions
========================================================================== */
ADSPResult afe_cdc_dev_cfg_read_int_status(uint16 int_idx, uint16_t *int_status)
{
  ADSPResult result = ADSP_EOK;
  cdc_reg_dev_cfg_t *reg_cfg_ptr = cdc_handler_global_ptr->client_info_list[int_idx].int_status_reg_cfg_ptr;
  uint8_t dest_value = 0x0;

  *int_status = FALSE;


  if((NULL != reg_cfg_ptr) && (0 != reg_cfg_ptr->reg_addr))
  {

#ifdef CDC_DEV_SLIMBUS
    result = afe_sb_read_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                  &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                  reg_cfg_ptr->reg_addr,
                                  &dest_value,
                                  reg_cfg_ptr->reg_field_bit_mask,
                                  reg_cfg_ptr->reg_bit_width,
                                  TRUE);
#endif // CDC_DEV_SLIMBUS
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_cdc_dev_cfg_read_int_status: reg_addr: 0x%lx, value: %d", reg_cfg_ptr->reg_addr, dest_value);
  }

  if(ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to read int status register: idx: 0x%x", int_idx);
    return result;
  }
  if(dest_value)
  {
    *int_status = TRUE;
  }

  return result; 
}

ADSPResult afe_cdc_dev_cfg_write_int_mask(uint16_t int_idx, uint16_t is_it_masking)
{
  ADSPResult result = ADSP_EOK;
  cdc_reg_dev_cfg_t *reg_cfg_ptr = cdc_handler_global_ptr->client_info_list[int_idx].int_mask_reg_cfg_ptr;;
  uint8_t dest_value = 0x0;

  // 0x0 for unmasking and 0x1 for masking
  if (TRUE == is_it_masking)
  {
    dest_value = 0x1;
  }
  if (ADSP_EOK != (result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to write int mask register: idx: 0x%x", int_idx);
    return result;
  }

  return result;
}

ADSPResult afe_cdc_dev_cfg_write_int_clear(uint16_t int_idx)
{
  ADSPResult result = ADSP_EOK;
  uint8_t dest_value = 0x1;
  cdc_reg_dev_cfg_t *reg_cfg_ptr = cdc_handler_global_ptr->client_info_list[int_idx].int_clear_reg_cfg_ptr;

  if (ADSP_EOK != (result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to write int clear register: idx: 0x%x", int_idx);
    return result;
  }

  return result;
}

ADSPResult afe_cdc_update_register(cdc_reg_dev_cfg_t *reg_cfg_ptr, uint8_t *value_ptr, uint16_t offset_multiplier)
{
  ADSPResult result = ADSP_EOK;
  if (NULL == reg_cfg_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "NULL pointer: reg_addr: 0x%p", reg_cfg_ptr);
    return ADSP_EFAILED;
  }

  if (0 != reg_cfg_ptr->reg_addr)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_cdc_update_register: reg_addr: 0x%lx, value: %d", reg_cfg_ptr->reg_addr, *value_ptr);
#ifdef CDC_DEV_SLIMBUS
    result = afe_sb_update_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                    &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                    (reg_cfg_ptr->reg_addr + reg_cfg_ptr->reg_offset_scale*offset_multiplier),
                                    value_ptr,
                                    reg_cfg_ptr->reg_field_bit_mask,
                                    reg_cfg_ptr->reg_bit_width,
                                    TRUE);
#endif // CDC_DEV_SLIMBUS
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "NULL Logical address: reg_addr: 0x%lx", reg_cfg_ptr->reg_addr);
    return ADSP_EFAILED;
  }

  return result;
}

ADSPResult afe_port_get_pcmlevels_from_codec(uint8_t* cdc_clip_pcm_level)
{
  uint32_t         bank_index;
  uint8_t         bank_sel = 0x0;
  ADSPResult result = ADSP_EOK;

#ifdef CDC_DEV_SLIMBUS 
  cdc_reg_dev_cfg_t     *reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CDC_SPKR_CLIPDET_VAL0];
  cdc_reg_dev_cfg_t     *bank_sel_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CLIP_PIPE_BANK_SEL];
  uint32_t               reg_field_shift;

  reg_field_shift = Q6_R_ct0_R(bank_sel_reg_cfg_ptr->reg_field_bit_mask);
#endif
  for(bank_index = 0; bank_index < cdc_handler_global_ptr->clip_num_banks; bank_index++)
  {
    bank_sel = cdc_handler_global_ptr->clip_bank_map[bank_index];
    // getting pcm levels only when there is a valid bank mapping present
    if (bank_sel < AFE_CLIP_MAX_BANKS)
    {

#ifdef CDC_DEV_SLIMBUS 
      uint8_t* cdc_clip_pcm_level_ptr = cdc_clip_pcm_level + (bank_sel * AFE_CLIP_MAX_REG_PER_BANK);

      if(0 != bank_sel_reg_cfg_ptr->reg_addr)
      {
        // Select the bank

        // Merging the bank selection bits with the back up of the bank selection register taken during port start
        //align the value to be written to the correct position
        bank_sel = (bank_sel << reg_field_shift) & bank_sel_reg_cfg_ptr->reg_field_bit_mask;

        // Always a non-blocking call
        result = afe_sb_update_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                        &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                        bank_sel_reg_cfg_ptr->reg_addr,
                                        &bank_sel,
                                        bank_sel_reg_cfg_ptr->reg_field_bit_mask,
                                        bank_sel_reg_cfg_ptr->reg_bit_width,
                                        FALSE);

        if (0 != reg_cfg_ptr->reg_addr)
        {
          // Read from the registers, since the bank registers are present at contiguous locations,
          // using one read for reading all the registers in the bank
          result = afe_sb_read_multi_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                              &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                              reg_cfg_ptr->reg_addr,
                                              cdc_clip_pcm_level_ptr,
                                              reg_cfg_ptr->reg_field_bit_mask,
                                              reg_cfg_ptr->reg_bit_width,// multiplying with number of registers in bank to read all the registers in one shot
                                              AFE_CLIP_MAX_REG_PER_BANK, TRUE);
        }
        else
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FBSP: Device configuration for clip level read is not done ");
        }

      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FBSP: Device configuration for selecting the bank for clip level read is not done ");
      }
#endif // CDC_DEV_SLIMBUS
    }
  }
  return result;
}

ADSPResult afe_cdc_update_cdc_pwr_state_register(uint8_t *value_ptr)
{
  ADSPResult result = ADSP_EOK;
  cdc_reg_dev_cfg_t *reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CDC_SYS_PWR_STATE_CTRL];
  if (0 != reg_cfg_ptr->reg_addr)
  {
    result = afe_cdc_update_register(reg_cfg_ptr, value_ptr, 0);
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_cdc_pwr_state: reg_addr: 0x%lx, value: %d", reg_cfg_ptr->reg_addr, *value_ptr);
  }

  return result;
}

ADSPResult afe_cdc_get_and_extract_pcm_levels_from_cdc(uint32_t clip_intr_id, int16_t *clip_pcm_level_buf_ptr, uint32_t *clip_pcm_buf_len_ptr)
{
  uint32_t bank_index;
  uint8_t bank_sel = 0x0;
  ADSPResult result = ADSP_EOK;
  uint8_t *cdc_clip_pcm_level_ptr = NULL;
  int16_t * clip_pcm_buf,*clip_pcm_buf_1,*clip_pcm_buf_2;
  uint32_t bank_map;
  int8_t temp_pcm = 0;

  uint8_t cdc_clip_pcm_level[CDC_CLIP_MAX_REG_PER_BANK][AFE_CLIP_MAX_BANKS];

#ifdef CDC_DEV_SLIMBUS
  uint32_t clip_det_val0_reg_addr, clip_pipe_bank_sel;
  uint32_t reg_field_shift;

  if(AFE_CDC_INT_SPKR_AUDIO_CLIP == clip_intr_id)
  {
    clip_det_val0_reg_addr = AFE_CRC_CDC_SPKR_CLIPDET_VAL0;
    clip_pipe_bank_sel = AFE_CRC_SPKR_CLIP_PIPE_BANK_SEL;
  }
  else
  {
    clip_det_val0_reg_addr = AFE_CRC_CDC_SPKR2_CLIPDET_VAL0;
    clip_pipe_bank_sel = AFE_CRC_SPKR2_CLIP_PIPE_BANK_SEL;
  }
  cdc_reg_dev_cfg_t *reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[clip_det_val0_reg_addr];
  cdc_reg_dev_cfg_t *bank_sel_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[clip_pipe_bank_sel];
  reg_field_shift = Q6_R_ct0_R(bank_sel_reg_cfg_ptr->reg_field_bit_mask);
#endif

  for(bank_index = 0; bank_index < cdc_handler_global_ptr->clip_num_banks; bank_index++)
  {
    bank_sel = cdc_handler_global_ptr->clip_bank_map[bank_index];
    // getting pcm levels only when there is a valid bank mapping present
    if(bank_sel < AFE_CLIP_MAX_BANKS)
    {
#ifdef CDC_DEV_SLIMBUS
      cdc_clip_pcm_level_ptr = &cdc_clip_pcm_level[0][bank_sel];

      if(0 != bank_sel_reg_cfg_ptr->reg_addr)
      {
        // Select the bank
        // Merging the bank selection bits with the back up of the bank selection register taken during port start
        //align the value to be written to the correct position
        bank_sel = (bank_sel << reg_field_shift) & bank_sel_reg_cfg_ptr->reg_field_bit_mask;

        // Always a non-blocking call
        result = afe_sb_update_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                        &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                        bank_sel_reg_cfg_ptr->reg_addr,
                                        &bank_sel,
                                        bank_sel_reg_cfg_ptr->reg_field_bit_mask,
                                        bank_sel_reg_cfg_ptr->reg_bit_width,
                                        FALSE);

        if(0 != reg_cfg_ptr->reg_addr)
        {
          // Read from the registers, since the bank registers are present at contiguous locations,
          // using one read for reading all the registers in the bank
          result = afe_sb_read_multi_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                              &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                              reg_cfg_ptr->reg_addr,
                                              cdc_clip_pcm_level_ptr,
                                              reg_cfg_ptr->reg_field_bit_mask,
                                              reg_cfg_ptr->reg_bit_width, // multiplying with number of registers in bank to read all the registers in one shot
                                              CDC_CLIP_MAX_REG_PER_BANK, TRUE);
        }
        else
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Speaker Protection: Device configuration for clip level read is not done ");
        }
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Speaker Protection: Device configuration for selecting the bank for clip level read is not done ");
      }
#endif // CDC_DEV_SLIMBUS
    }
  }

  clip_pcm_buf = clip_pcm_level_buf_ptr;

  // Since two registers from two different banks construct one pcm level, numbanks will be divided by 2 to get pcm buf len
  *clip_pcm_buf_len_ptr = (cdc_handler_global_ptr->clip_num_banks >> 1) * AFE_CLIP_MAX_REG_PER_BANK;

  if(2 == cdc_handler_global_ptr->clip_num_banks)
  {
    clip_pcm_buf_2 = clip_pcm_buf_1 = &clip_pcm_buf[0];
  }
  else // number of banks 4
  {
    clip_pcm_buf_1 = &clip_pcm_buf[0];
    clip_pcm_buf_2 = &clip_pcm_buf[8];
  }

  for(bank_index = 0; bank_index < cdc_handler_global_ptr->clip_num_banks; bank_index++)
  {
    bank_map = cdc_handler_global_ptr->clip_bank_map[bank_index];
    if(bank_map < AFE_CLIP_MAX_BANKS)
    { 
      cdc_clip_pcm_level_ptr = &cdc_clip_pcm_level[0][bank_map];

      switch(bank_map)
      {
        // The registers in bank 0 and 1 will contain 8msbs of actual pcm level
        case 0:
        {
          for(int index = 0; index < AFE_CLIP_MAX_REG_PER_BANK; index++)
          {
            // Sign extend shift
            temp_pcm = (int8_t)(cdc_clip_pcm_level_ptr[index]);
            clip_pcm_buf_1[index] = ((int16_t)temp_pcm << 2);
          }
          break;
        }
        case 1:
        {
          for(int index = 0; index < AFE_CLIP_MAX_REG_PER_BANK; index++)
          {
            // Sign extend shift
            temp_pcm = (int8_t)(cdc_clip_pcm_level_ptr[index]);
            clip_pcm_buf_2[index] = ((int16_t)temp_pcm << 2);
          }
          break;
        }
        // The registers in bank 2 and 3 will contain 2lsbs of actual pcm level in 2lsbs
        case 2:
        {
          for(int index = 0; index < AFE_CLIP_MAX_REG_PER_BANK; index++)
          {
            clip_pcm_buf_1[index] |= (int16_t)(cdc_clip_pcm_level_ptr[index] & 0x03);
          }
          break;
        }
        case 3:
        {
          for(int index = 0; index < AFE_CLIP_MAX_REG_PER_BANK; index++)
          {
            clip_pcm_buf_2[index] |= (int16_t)(cdc_clip_pcm_level_ptr[index] & 0x03);
          }
          break;
        }
      }
    }
  }

  return result;
}


ADSPResult afe_port_custom_cdc_int_handling(elite_msg_any_t* pMsg)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  elite_msg_custom_afe_cdc_int_info_t *pCustomAfeMsgPayload = NULL;
  uint32_t int_index;

  uint32_t int_id;


  pCustomAfeMsgPayload = (elite_msg_custom_afe_cdc_int_info_t *)pMsg->pPayload;

  /* Get port info */
  if(ADSP_EOK != (result = afe_svc_get_port(pCustomAfeMsgPayload->client_info.client_id, &pDevPort)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: Can't find port_id: 0x%x for the custom cmd, result: 0x%x",
          pCustomAfeMsgPayload->client_info.client_id, result);
    return ADSP_EFAILED;
  }

  int_index = (uint32_t)pCustomAfeMsgPayload->client_info.int_index;

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "interrupt %ld is raised", int_index);

  if(ADSP_EOK != afe_topo_cdc_get_int_id(int_index, &int_id))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid interrupt index: %lu", int_index);
    return ADSP_EUNSUPPORTED;
  }

  /* Any interrupt need to be acked properly, in the case of errors also.
        In unavoidable cases such as not having details of codec client structure to ack, this can be relaxed */
  switch(int_index)
  {
     case AFE_CDC_INT_MAD_AUDIO: /* HW MAD interrupts */
     case AFE_CDC_INT_MAD_BEACON:
     case AFE_CDC_INT_MAD_ULT:
     {
       afe_port_mutex_lock(pDevPort);
       afe_port_hw_mad_interrupt_handler((void *)pDevPort, &pCustomAfeMsgPayload->client_info);
       afe_port_mutex_unlock(pDevPort);
       break;
     }
       /* Interrupts for FBSP Rx processing */
     case AFE_CDC_INT_SPKR_AUDIO_CLIP:
     case AFE_CDC_INT_SPKR2_AUDIO_CLIP:
     {
       // get pcm clip levels from codec
       int16_t cdc_clip_pcm_level[CDC_CLIP_PCM_BUF_LEN];
       int16_t *cdc_clip_pcm_level_ptr = NULL;
       uint32_t clip_pcm_buf_len;

       // Checking whether DSP received clip bank information
       if(0 != cdc_handler_global_ptr->clip_num_banks)
       {
         cdc_clip_pcm_level_ptr = &cdc_clip_pcm_level[0];

         // Getting the pcm levels from codec
         if(ADSP_EOK
            != afe_cdc_get_and_extract_pcm_levels_from_cdc(int_index, cdc_clip_pcm_level_ptr,
                                                           &clip_pcm_buf_len))
         {
           MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FBSP: Getting pcm levels from codec is failed ");
         }
         else
         {
           afe_port_mutex_lock(pDevPort);

           capi_v2_cdc_clip_pcm_info_t clip_pcm_info;

           clip_pcm_info.cdc_clip_pcm_level_buf.actual_data_len = sizeof(uint16_t) * clip_pcm_buf_len;
           clip_pcm_info.cdc_clip_pcm_level_buf.max_data_len = sizeof(uint16_t) * clip_pcm_buf_len;
           clip_pcm_info.cdc_clip_pcm_level_buf.data_ptr = (int8_t *)cdc_clip_pcm_level_ptr;

           // do set param of interrupt and clip pcm levels
           if(ADSP_EOK
              != (result = topo_cdc_int_set_param(pDevPort->port_topo_ptr,
                                                  int_id,
                                                  sizeof(capi_v2_cdc_clip_pcm_info_t),
                                                  (int8_t *)&clip_pcm_info)))
           {
             MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: sending clip interrupt and  pcm levels  failed with result:"
                   "0x%x for port id 0x%x", result, pDevPort->intf);
           }
           afe_port_mutex_unlock(pDevPort);
         }
       }

       if(ADSP_EOK != (result = topo_ack_cdc_int_client_info(pDevPort->port_topo_ptr, int_index, afe_topo_ack_cdc_intr)))
       {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: acking clip interrupt failed with result:"
               "0x%x for port id 0x%x", result, pDevPort->intf);
       }

     }
       break;

     case AFE_CDC_INT_VBAT_ATTACK:
     case AFE_CDC_INT_VBAT_RELEASE:
     {
       afe_port_mutex_lock(pDevPort);
       if(ADSP_EOK
          != (result = topo_cdc_int_set_param(pDevPort->port_topo_ptr, int_id, 0, NULL)))
       {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: sending vbat interrupt to topology failed with result: 0x%x for port id 0x%x",
               result, pDevPort->intf);
       }
       afe_port_mutex_unlock(pDevPort);

       if(ADSP_EOK != (result = topo_ack_cdc_int_client_info(pDevPort->port_topo_ptr, int_index, afe_topo_ack_cdc_intr)))
       {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: acking vbat interrupt failed with result:"
               "0x%x for port id 0x%x", result, pDevPort->intf);
       }
     }
       break;

     default:
       break;
  }
  return result; 
}
