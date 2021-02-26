/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

 Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFEDeviceConfig.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "AFEHwMadDriver_i.h"
#include "AFECdcPwrCtl.h"

ADSPResult cdc_init_interrupt_info()
{
  ADSPResult result = ADSP_EOK;
  client_info_list_t *client_list_info = NULL;
  uint16_t int_idx = 0;

  // store the reg indices for all the supported interrupts
  for(int_idx = AFE_CDC_INT_INDEX_START; int_idx <= AFE_CDC_INT_INDEX_END; int_idx++)
  {
    client_list_info = &cdc_handler_global_ptr->client_info_list[int_idx];
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Interrupt Index: %d", int_idx);
    switch(int_idx)
    {
      case AFE_CDC_INT_MAD_AUDIO:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_AUDIO_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_AUDIO_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_AUDIO_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_AUDIO_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_MAD_BEACON:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_BEACON_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_BEACON_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_BEACON_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_BEACON_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_MAD_ULT:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_ULT_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_ULT_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_ULT_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_MAD_ULT_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_AUDIO_CLIP:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CLIP_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CLIP_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CLIP_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_CLIP_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_VBAT_ATTACK:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_ATTACK_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_ATTACK_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_ATTACK_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_ATTACK_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_VBAT_RELEASE:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_RELEASE_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_RELEASE_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_RELEASE_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_VBAT_RELEASE_INT_CLEAR_REG];
      }
      break;
      case AFE_CDC_INT_SPKR2_AUDIO_CLIP:
      {
        client_list_info->int_routing_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_SPKR2_CLIP_INT_DEST_SELECT_REG];
        client_list_info->int_mask_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_SPKR2_CLIP_INT_MASK_REG];
        client_list_info->int_status_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_SPKR2_CLIP_INT_STATUS_REG];
        client_list_info->int_clear_reg_cfg_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_SPKR2_CLIP_INT_CLEAR_REG];
      }
      break;
      default:
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Un-Supported Interrupt Index: %d", int_idx);
        result = ADSP_EUNSUPPORTED;
        break;
    }
  }

  return result;
}

ADSPResult cdc_init_sb_slave_dev_info()
{
  ADSPResult result = ADSP_EOK;

  // initialize the slave la to zero
  cdc_handler_global_ptr->sb_cfg.reg_prop.slave_dev_la = 0;

#ifdef CDC_DEV_SLIMBUS

  // get logical addr of the codec interface device
  uint64_t device_enum_addr = ((uint64_t)cdc_handler_global_ptr->sb_cfg.device_enum_addr_msw << 32) | (cdc_handler_global_ptr->sb_cfg.device_enum_addr_lsw);

  if (ADSP_EOK != (result = afe_sb_get_logical_addr(cdc_handler_global_ptr->sb_cfg.p_sb_driver, device_enum_addr, &(cdc_handler_global_ptr->sb_cfg.reg_prop.slave_dev_la))))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Unable to get logical address for EA for Codec Device Cfg");
    return result;
  }
  MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Codec Logical addr :: %x",cdc_handler_global_ptr->sb_cfg.reg_prop.slave_dev_la);
#endif // CDC_DEV_SLIMBUS

  return result;
}


ADSPResult cdc_cfg_int_paging()
{
  ADSPResult result = ADSP_EOK;

#ifdef CDC_DEV_SLIMBUS
  if(cdc_handler_global_ptr->sb_cfg.reg_prop.uses_paging)
  {
    result = afe_sb_reg_page_init(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                  &cdc_handler_global_ptr->sb_cfg.reg_prop);
  }
#endif // CDC_DEV_SLIMBUS
  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Failed to initialize codec page init 0x%x",
          result);
  }

  return result;
}

ADSPResult cdc_cfg_pwr_state_cntl()
{
  ADSPResult result = ADSP_EOK;

#ifdef CDC_DEV_SLIMBUS
  if(TRUE == cdc_handler_global_ptr->cdc_pwr_cntl_enable)
  {
    result = afe_cfg_cdc_pwr_state_cntl(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
  }
#endif // CDC_DEV_SLIMBUS
  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Failed to configure power state control for codec 0x%x",
          result);
  }

  return result;
}

ADSPResult cdc_cfg_int_routing()
{
  ADSPResult result = ADSP_EOK;
  client_info_list_t  *client_list_info = NULL;
  cdc_reg_dev_cfg_t *reg_cfg_ptr = NULL;
  uint16_t int_idx = 0;
  uint8_t dest_value = 0x1;

  // go through the interrupts that are supported by ADSP
  for (int_idx = AFE_CDC_INT_INDEX_START; int_idx <= AFE_CDC_INT_INDEX_END; int_idx++)
  {
    // get handle to interrupt specific node
    client_list_info = &cdc_handler_global_ptr->client_info_list[int_idx];

    // get reg cfg of desti nation selection register for configuring the interrupt routing
    reg_cfg_ptr = client_list_info->int_routing_reg_cfg_ptr;

    if (0 != reg_cfg_ptr->reg_addr)
    {
#ifdef CDC_DEV_SLIMBUS
      result = afe_sb_update_register(cdc_handler_global_ptr->sb_cfg.p_sb_driver,
                                      &cdc_handler_global_ptr->sb_cfg.reg_prop,
                                      reg_cfg_ptr->reg_addr,
                                      &dest_value,
                                      reg_cfg_ptr->reg_field_bit_mask,
                                      reg_cfg_ptr->reg_bit_width,
                                      TRUE);
#endif // CDC_DEV_SLIMBUS
      if (ADSP_FAILED(result))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Faile to update codec register: idx: 0x%x, reg_addr: 0x%lx, dest_value::%u", int_idx, reg_cfg_ptr->reg_addr,dest_value);
        return result;
      }
    }
  }

  return result;
}

ADSPResult cdc_device_cfg_init()
{
  ADSPResult result = ADSP_EOK;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entered cdc_device_cfg_init");

  qurt_elite_mutex_lock(&(cdc_handler_global_ptr->cdc_mutex));

  /* Before performing any codec operations, this function will execute for commands like
   AFE_PARAM_ID_CDC_REG_CFG_INIT which is must for  codec register operations.
   Doing the slimbus init handle here for one time rather than at bootup.
   If we are doing in bootup, even for internal codec targets Deviceattach is getting called
   and updating few PAD registers by slimbus driver. Due to this it is affecting internal codec which is
   generating noise. Dont move this init handle to bootup code */

#ifdef  CDC_DEV_SLIMBUS
  /* No fallback  if its failed. Must to debug why there is failure. */
  if (NULL == (cdc_handler_global_ptr->sb_cfg.p_sb_driver = afe_slimbus_init_dev_handle(AFE_SLIMBUS_DEVICE_1)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to register with SLIMBUS driver: %d", result);
    goto __bail_out;
  }  
#endif

  if (ADSP_EOK != (result = cdc_init_interrupt_info()))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize interrupt info: 0x%x", result);
    goto __bail_out;
  }

  if (ADSP_EOK != (result = cdc_init_sb_slave_dev_info()))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize slimbus slave info: 0x%x", result);
    goto __bail_out;
  }

  if (ADSP_EOK != (result = cdc_cfg_int_paging()))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize paging: 0x%x", result);
    goto __bail_out;
  }

  if (ADSP_EOK != (result = cdc_cfg_int_routing()))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize interrupt routing: 0x%x", result);
    goto __bail_out;
  }

  if (ADSP_EOK != (result = cdc_cfg_pwr_state_cntl()))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure codec power state control: 0x%x", result);
    goto __bail_out;
  }

  cdc_handler_global_ptr->is_cdc_reg_cfg_init_received = TRUE;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Exit cdc_device_cfg_init");

  __bail_out:

  qurt_elite_mutex_unlock(&(cdc_handler_global_ptr->cdc_mutex));

  return result;
}

ADSPResult afe_cdc_hw_mad_reg_cfg_init(void *ptr)
{
  hw_mad_data_t *hw_mad_ptr = (hw_mad_data_t *) ptr;
  hw_mad_mod_t *mad_mod_ptr = NULL;

  // MAD Audio initialization
  mad_mod_ptr = &hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO];
  mad_mod_ptr->int_idx = AFE_CDC_INT_MAD_AUDIO;
  mad_mod_ptr->reg_cfg_mad_enable_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_AUDIO_ENABLE];
  mad_mod_ptr->reg_cfg_sleep_time_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_AUDIO_SLEEP_TIME];
  mad_mod_ptr->reg_cfg_tx_switch_off_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_TX_AUDIO_SWITCH_OFF];

  // MAD Beacon initialization
  mad_mod_ptr = &hw_mad_ptr->mad_data[AFE_HW_MAD_BEACON];
  mad_mod_ptr->int_idx = AFE_CDC_INT_MAD_BEACON;
  mad_mod_ptr->reg_cfg_mad_enable_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_BEACON_ENABLE];
  mad_mod_ptr->reg_cfg_sleep_time_ptr  = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_BEACON_SLEEP_TIME];
  mad_mod_ptr->reg_cfg_tx_switch_off_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_TX_BEACON_SWITCH_OFF];

  // MAD Beacon initialization
  mad_mod_ptr = &hw_mad_ptr->mad_data[AFE_HW_MAD_ULTRA_SOUND];
  mad_mod_ptr->int_idx = AFE_CDC_INT_MAD_ULT;
  mad_mod_ptr->reg_cfg_mad_enable_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_ULTR_ENABLE];
  mad_mod_ptr->reg_cfg_sleep_time_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_ULTR_SLEEP_TIME];
  mad_mod_ptr->reg_cfg_tx_switch_off_ptr = &cdc_handler_global_ptr->reg_cfg[AFE_CRC_HW_MAD_TX_ULTR_SWITCH_OFF];

  return ADSP_EOK;
}

uint16_t afe_cdc_hwmad_sleep_time_conversion(uint16_t sleep_time_ms)
{
  if (sleep_time_ms < 19)
  {
    return 0;
  }
  else if (sleep_time_ms < 30)
  {
    return 0x10;
  }
  else if (sleep_time_ms < 86)
  {
    return 0x11;
  }
  else if (sleep_time_ms < 142)
  {
    return 0x12;
  }
  else if (sleep_time_ms < 198)
  {
    return 0x13;
  }
  else if (sleep_time_ms < 200)
  {
    return 0x14;
  }
  else if (sleep_time_ms < 254)
  {
    return 0x1;
  }
  else if (sleep_time_ms < 400)
  {
    return 0x15;
  }
  else if (sleep_time_ms < 600)
  {
    return 0x2;
  }
  else if (sleep_time_ms < 800)
  {
    return 0x3;
  }
  else if (sleep_time_ms < 1000)
  {
    return 0x4;
  }
  else if (sleep_time_ms < 2000)
  {
    return 0x5;
  }
  else if (sleep_time_ms < 3000)
  {
    return 0x6;
  }
  else if (sleep_time_ms < 4000)
  {
    return 0x7;
  }
  else if (sleep_time_ms < 5000)
  {
    return 0x8;
  }
  else if (sleep_time_ms < 10000)
  {
    return 0x9;
  }
  else if (sleep_time_ms < 20000)
  {
    return 0xA;
  }
  else if (sleep_time_ms < 30000)
  {
    return 0xB;
  }
  else if (sleep_time_ms < 40000)
  {
    return 0xC;
  }
  else if (sleep_time_ms < 50000)
  {
    return 0xD;
  }
  else if (sleep_time_ms < 60000)
  {
    return 0xE;
  }
  else
  {
    return 0xF;
  }
  return 0x0;
}

cdc_reg_dev_cfg_t* afe_cdc_get_dev_cfg_ptr(uint32_t reg_field_type)
{
  return &cdc_handler_global_ptr->reg_cfg[reg_field_type];
}

