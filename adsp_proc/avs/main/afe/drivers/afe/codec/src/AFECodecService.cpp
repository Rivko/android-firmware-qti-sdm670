/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFECodecService.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "adsp_afe_service_commands.h"


/*==========================================================================
  Defines
========================================================================== */
#define AFE_CDC_RESP_Q_MASK 0x1
#define AFE_CDC_SLIMBUS_LA_OFFSET_DEFAULT 0x800

/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult cdc_device_cfg_init();
ADSPResult afe_svc_cdc_int_register_handler (elite_msg_any_t* pMsg);
ADSPResult afe_svc_cdc_int_deregister_handler (elite_msg_any_t* pMsg);
ADSPResult afe_svc_cdc_int_ack_handler(elite_msg_any_t* pMsg);
ADSPResult afe_svc_cdc_int_process_handler();
ADSPResult afe_svc_cdc_cmd_handler(void* dummy, elite_msg_any_t* pMsg);
/*==========================================================================
  Globals
========================================================================== */
static afe_cdc_handler_t cdc_handler_global;
afe_cdc_handler_t *cdc_handler_global_ptr = NULL;
qurt_elite_interrupt_ist_t codec_int_ist;

static   elite_svc_msg_handler_func afe_cdc_cmd_handler[1] =
{
    afe_svc_cdc_cmd_handler
};

/*==========================================================================
  Function implementations
========================================================================== */
ADSPResult afe_svc_set_cdc_dev_cfg(uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size)
{
  ADSPResult result = ADSP_EOK;
  uint16_t index;
  uint16_t bank_index;

  switch(param_id)
  {
    case AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG:
    {
      /* Choose the appropriate config version  */
      uint32_t slave_dev_minor_version =  *((uint32_t *)param_buffer_ptr);
      if(slave_dev_minor_version > AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG)
      {
        slave_dev_minor_version = AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG;
      }
      if(0x1 == slave_dev_minor_version)
      {
        if (param_size < sizeof(afe_cdc_slimbus_slave_cfg_param_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_cdc_slimbus_slave_cfg_param_v1_t *sb_cfg_param = (afe_cdc_slimbus_slave_cfg_param_v1_t *)param_buffer_ptr;
        cdc_handler_global_ptr->sb_cfg.slave_dev_minor_version = sb_cfg_param->slave_dev_minor_version;
        cdc_handler_global_ptr->sb_cfg.device_enum_addr_lsw = sb_cfg_param->device_enum_addr_lsw;
        cdc_handler_global_ptr->sb_cfg.device_enum_addr_msw = sb_cfg_param->device_enum_addr_msw;
        cdc_handler_global_ptr->sb_cfg.rx_slave_port_offset = sb_cfg_param->rx_slave_port_offset;
        cdc_handler_global_ptr->sb_cfg.tx_slave_port_offset = sb_cfg_param->tx_slave_port_offset;
      }
    }
    break;
    case AFE_PARAM_ID_CDC_REG_CFG:
    {
      /* Choose the appropriate config version  */
      uint32_t cdc_reg_cfg_minor_version =  *((uint32_t *)param_buffer_ptr);
      if(cdc_reg_cfg_minor_version > AFE_API_VERSION_CDC_REG_CFG)
      {
        cdc_reg_cfg_minor_version = AFE_API_VERSION_CDC_REG_CFG;
      }
      if(0x1 == cdc_reg_cfg_minor_version)
      {
        if (param_size < sizeof(afe_cdc_reg_cfg_param_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_cdc_reg_cfg_param_v1_t *cdc_reg_param = (afe_cdc_reg_cfg_param_v1_t *)param_buffer_ptr;
        int16_t reg_index = cdc_reg_param->reg_field_type;
        if(reg_index < AFE_CDC_MAX_DEV_CFG_REG_FIELDS)
        {
          /*
           * Legacy AFE_PARAM_ID_CDC_REG_CFG param assumes client to add codec register
           * address and slimbus logical address offset into a single vairabl. All targets
           * supported by AFE_PARAM_ID_CDC_REG_CFG has logical address offset of 0x800.
           *
           * Going forward, we expect client to provide codec register address via
           * AFE_PARAM_ID_CDC_REG_CFG_V2 and AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG separately.
           *
           * Codec handler is ought to work on actual codec register address directly and
           * apply slimbus logical address offset just before *slimbus* base access.
           * To keep implementation consistent with legacy, we are subtracting the known
           * slimbus_la_offset during Initialization. After this point,
           * reg_logical_addr is not be used and the subtracted value will get
           * added at *slimbus* base access.
           *
           * reg_locial_addr = 0x800 (AFE_CDC_SLIMBUS_LA_OFFSET_DEFAULT) + actual register address (reg_addr)
           */
          cdc_handler_global_ptr->reg_cfg[reg_index].reg_addr = cdc_reg_param->reg_logical_addr -
              AFE_CDC_SLIMBUS_LA_OFFSET_DEFAULT;
          cdc_handler_global_ptr->reg_cfg[reg_index].reg_offset_scale = cdc_reg_param->reg_offset_scale;
          cdc_handler_global_ptr->reg_cfg[reg_index].reg_bit_width = cdc_reg_param->reg_bit_width;
          cdc_handler_global_ptr->reg_cfg[reg_index].reg_field_bit_mask = cdc_reg_param->reg_field_bit_mask;
        }
        else
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "un-supported Reg field type: %d, Param Id: 0x%lx", reg_index, param_id);
          return ADSP_EBADPARAM;
        }
      }
    }
    break;
    case AFE_PARAM_ID_CDC_REG_CFG_V2:
    {
      /* Choose the appropriate config version  */
      uint32_t minor_version =  *((uint32_t *)param_buffer_ptr);
      if(minor_version > AFE_API_VERSION_CDC_REG_CFG_V2)
      {
        minor_version = AFE_API_VERSION_CDC_REG_CFG_V2;
      }
      if(0 == minor_version)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "0 minor version not supported ");
        return ADSP_EBADPARAM;
      }
      else if(0x1 == minor_version)
      {
        if (param_size < sizeof(afe_cdc_reg_cfg_param_v2_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Invalid payload size: %d, Param Id: 0x%lx",
                param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_cdc_reg_cfg_param_v2_t *cdc_reg_param =
            (afe_cdc_reg_cfg_param_v2_t *)param_buffer_ptr;
        int16_t reg_index = cdc_reg_param->reg_field_type;
        cdc_reg_dev_cfg_t *reg_cfg = cdc_handler_global_ptr->reg_cfg;
        if(reg_index < AFE_CDC_MAX_DEV_CFG_REG_FIELDS)
        {
          reg_cfg[reg_index].reg_addr = cdc_reg_param->reg_addr;
          reg_cfg[reg_index].reg_offset_scale = cdc_reg_param->reg_offset_scale;
          reg_cfg[reg_index].reg_bit_width = cdc_reg_param->reg_bit_width;
          reg_cfg[reg_index].reg_field_bit_mask = cdc_reg_param->reg_field_bit_mask;
        }
        else
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "un-supported Reg field type: %d, Param Id: 0x%lx",
                reg_index, param_id);
          return ADSP_EBADPARAM;
        }
      }
    }
    break;
    case AFE_PARAM_ID_CDC_REG_SLIMBUS_CFG:
    {
      /* Choose the appropriate config version  */
      uint32_t minor_version =  *((uint32_t *)param_buffer_ptr);
      if(minor_version > AFE_API_VERSION_CDC_REG_SLIMBUS_CFG)
      {
        minor_version = AFE_API_VERSION_CDC_REG_SLIMBUS_CFG;
      }
      if(0 == minor_version)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "0 minor version not supported ");
        return ADSP_EBADPARAM;
      }
      else if(0x1 == minor_version)
      {
        if (param_size < sizeof(afe_cdc_reg_slimbus_cfg_param_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Invalid payload size: %d, Param Id: 0x%lx",
                param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_cdc_reg_slimbus_cfg_param_v1_t *reg_slimbus_cfg_ptr =
            (afe_cdc_reg_slimbus_cfg_param_v1_t *)param_buffer_ptr;
        cdc_handler_global_ptr->sb_cfg.reg_prop.slimbus_la_offset =
            reg_slimbus_cfg_ptr->sb_la_offset;
      }
    }
    break;
    case AFE_PARAM_ID_CDC_AANC_VERSION:
    {
      /* Choose the appropriate config version  */
      uint32_t aanc_hw_ver_minor_version =  *((uint32_t *)param_buffer_ptr);
      if(aanc_hw_ver_minor_version > AFE_API_VERSION_CDC_AANC_VERSION)
      {
        aanc_hw_ver_minor_version = AFE_API_VERSION_CDC_AANC_VERSION;
      }
      if(0x1 == aanc_hw_ver_minor_version)
      {
        if (param_size < sizeof(afe_cdc_aanc_version_param_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_cdc_aanc_version_param_v1_t *aanc_hw_param = (afe_cdc_aanc_version_param_v1_t *)param_buffer_ptr;
        cdc_handler_global_ptr->aanc_hw_version = aanc_hw_param->aanc_hw_version;
        MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"AANC HW Version:;%lu",aanc_hw_param->aanc_hw_version);
      }
    }
    break;
    case AFE_PARAM_ID_CLIP_BANK_SEL_CFG:
    {
      /* Choose the appropriate config version  */
      uint32_t clip_bank_sel_cfg_minor_version =  *((uint32_t *)param_buffer_ptr);
      if(clip_bank_sel_cfg_minor_version > AFE_API_VERSION_CLIP_BANK_SEL_CFG)
      {
        clip_bank_sel_cfg_minor_version = AFE_API_VERSION_CLIP_BANK_SEL_CFG;
      }
      if(0x1 == clip_bank_sel_cfg_minor_version)
      {
        if (param_size < sizeof(afe_clip_bank_sel_cfg_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        afe_clip_bank_sel_cfg_v1_t *clip_bank_sel_cfg = (afe_clip_bank_sel_cfg_v1_t *)param_buffer_ptr;

        // Checking for incorrect num banks
        if ((2 != clip_bank_sel_cfg->num_banks) && (4 != clip_bank_sel_cfg->num_banks))
        {
          MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"wrong number of banks [%lu] for clip level reading",clip_bank_sel_cfg->num_banks);
          return ADSP_EUNSUPPORTED;
        }
        else
        {
          // Checking for any repetition in the bank mapping
          for(index = 0; index < clip_bank_sel_cfg->num_banks; index++)
          {
            for(bank_index = index+1; bank_index < clip_bank_sel_cfg->num_banks; bank_index++)
            {
              if(clip_bank_sel_cfg->bank_map[index] == clip_bank_sel_cfg->bank_map[bank_index])
              {
                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Repeated clip bank Map error");
                return ADSP_EUNSUPPORTED;
              }
            }
          }
          cdc_handler_global_ptr->clip_num_banks = clip_bank_sel_cfg->num_banks;
          MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Clip number of banks:%lu",cdc_handler_global_ptr->clip_num_banks);
        }


        for (index = 0; index < cdc_handler_global_ptr->clip_num_banks; index++)
        {
          // Initializing bank map to max banks
          //  in case of wrong mapping, clip interrupt handler will not access wrong bank
          cdc_handler_global_ptr->clip_bank_map[index] = AFE_CLIP_MAX_BANKS;
          if (clip_bank_sel_cfg->bank_map[index] > AFE_CLIP_MAX_BANKS)
          {
            MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"wrong Clip bank map[%d]:%lu",index, clip_bank_sel_cfg->bank_map[index]);
            return ADSP_EUNSUPPORTED;
          }
          cdc_handler_global_ptr->clip_bank_map[index] = clip_bank_sel_cfg->bank_map[index];
          MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Clip bank map[%d]:%lu",index, cdc_handler_global_ptr->clip_bank_map[index]);
        }
      }
    }
    break;
    case AFE_PARAM_ID_CDC_REG_CFG_INIT:
    {
      // call the device configuration init function
      result = cdc_device_cfg_init();
    }
    break;
    case AFE_PARAM_ID_CDC_REG_PAGE_CFG:
    {
      uint32_t minor_version =  *((uint32_t *)param_buffer_ptr);
      if(0 == minor_version)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported Minor version id %d for ::0x%lx", minor_version, param_id);
        return ADSP_EUNSUPPORTED;
      }
      /* Choose the appropriate config version  */
      if(minor_version > AFE_API_VERSION_CDC_REG_PAGE_CFG)
      {
        minor_version = AFE_API_VERSION_CDC_REG_PAGE_CFG;
      }
      if(0x1 == minor_version)
      {
        afe_cdc_reg_page_cfg_t* reg_page_cfg_ptr = NULL;
        if(param_size < sizeof(afe_cdc_reg_page_cfg_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        reg_page_cfg_ptr = (afe_cdc_reg_page_cfg_t*)param_buffer_ptr;

        /* Check supported values for enable */
        switch(reg_page_cfg_ptr->enable)
        {
          case TRUE:
          case FALSE:
            break;
          default:
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Unsupported arg: enable 0x%x,",
                  reg_page_cfg_ptr->enable);
            return ADSP_EBADPARAM;
        }

        /* Check supported values for proc_id */
        switch(reg_page_cfg_ptr->proc_id)
        {
          case AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_0:
          case AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_1:
          case AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_2:
          case AFE_CDC_REG_PAGE_ASSIGN_PROC_ID_3:
            break;
          default:
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Unsupported arg: proc_id 0x%x",
                  reg_page_cfg_ptr->proc_id);
            return ADSP_EBADPARAM;
        }
        cdc_handler_global_ptr->sb_cfg.reg_prop.proc_id = (uint8_t)reg_page_cfg_ptr->proc_id;
        cdc_handler_global_ptr->sb_cfg.reg_prop.uses_paging = (bool_t)reg_page_cfg_ptr->enable;
      }
    }
    break;
    case AFE_PARAM_ID_CDC_PWR_CNTL_CFG:
    {
      uint32_t minor_version =  *((uint32_t *)param_buffer_ptr);
      if(0 == minor_version)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported Minor version id %d for ::0x%lx", minor_version, param_id);
        return ADSP_EUNSUPPORTED;
      }
      /* Choose the appropriate config version  */
      if(minor_version > AFE_API_VERSION_CDC_PWR_CNTL_CFG)
      {
        minor_version = AFE_API_VERSION_CDC_PWR_CNTL_CFG;
      }
      if(0x1 == minor_version)
      {
        afe_cdc_pwr_cntl_cfg_v1_t* cdc_pwr_cntl_cfg_ptr = NULL;
        if(param_size < sizeof(afe_cdc_pwr_cntl_cfg_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
          return ADSP_EBADPARAM;
        }
        cdc_pwr_cntl_cfg_ptr = (afe_cdc_pwr_cntl_cfg_v1_t*)param_buffer_ptr;

        /* Check supported values for enable */
        if((TRUE != cdc_pwr_cntl_cfg_ptr->enable) && (FALSE != cdc_pwr_cntl_cfg_ptr->enable))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "Unsupported arg: enable 0x%x,",
                cdc_pwr_cntl_cfg_ptr->enable);
          return ADSP_EBADPARAM;
        }
        cdc_handler_global_ptr->cdc_pwr_cntl_enable = (bool_t)cdc_pwr_cntl_cfg_ptr->enable;
      }
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Unsupported Param id ::0x%lx", param_id);
      result = ADSP_EUNSUPPORTED;
      break;
    }
  }

  return result;
}

ADSPResult afe_svc_cdc_cmd_handler(void* dummy, elite_msg_any_t* pMsg)
{
  ADSPResult                      result = ADSP_EOK;
  uint32_t                        opcode = 0;

  opcode = ((elite_msg_custom_header_t *)pMsg->pPayload)->unSecOpCode;

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_svc_cdc_cmd_handler recvd cmd: 0x%lx", opcode);

  switch (opcode)
  {
    case ELITEMSG_CUSTOM_CDC_INT_REGISTER_REQ:
    {
      afe_svc_cdc_int_register_handler(pMsg);
    }
    break;
    case ELITEMSG_CUSTOM_CDC_INT_DEREGISTER_REQ:
    {
      afe_svc_cdc_int_deregister_handler(pMsg);
    }
    break;
    case ELITEMSG_CUSTOM_CDC_INT_CLIENT_ACK:
    {
      afe_svc_cdc_int_ack_handler(pMsg);

      /* Informing any pending interrupts during servicing the current or previous interrupts */
      // call interrupt processing handler
      afe_svc_cdc_int_process_handler();
    }
    break;
    case ELITEMSG_CUSTOM_TERMINATE_THREAD:
    {
      result = ADSP_ETERMINATED;
      elite_msg_return_payload_buffer(pMsg);
    }
    break;
    default:
    {
      result = ADSP_EBADPARAM;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Unsupported opcode to cdc cmd handler: 0x%lx", opcode);
      elite_msg_ack_msg(pMsg, ADSP_EBADPARAM);
    }
  }

  return result;
}

int afe_svc_cdc_handler_workloop(void* arg_ptr)
{
  ADSPResult result          = ADSP_EOK;
  uint32_t channel_status    = 0;
  elite_svc_handle_t  *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);

  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Enter: afe_svc_cdc_handler_workloop");

  for(;;)
  {
    channel_status = qurt_elite_channel_wait(&cdc_handler_global_ptr->channel, AFE_SVC_CDC_HANDLER_IST_MASK|AFE_SVC_CDC_HANDLER_CMD_MASK);

    if (channel_status & AFE_SVC_CDC_HANDLER_IST_MASK)
    {
      // call interrupt processing handler
      afe_svc_cdc_int_process_handler();

      // clear the ist signal
      qurt_elite_signal_clear(cdc_handler_global_ptr->ist_signal_ptr);

    }

    if (channel_status & AFE_SVC_CDC_HANDLER_CMD_MASK)
    {
      const uint32_t afe_svc_cdc_cmd_table_size = sizeof(afe_cdc_cmd_handler)/sizeof(afe_cdc_cmd_handler[0]);
      if (ADSP_ETERMINATED == (result = elite_svc_process_cmd_queue(NULL, cdc_svc_hptr, afe_cdc_cmd_handler, afe_svc_cdc_cmd_table_size)))
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Exiting from afe_svc_cdc_handler_workloop");
        break;
      }
    }
  }

  return ADSP_EOK;
}

ADSPResult afe_svc_cdc_handler_deinit(void)
{
  ADSPResult        result = ADSP_EOK;
  elite_msg_custom_afe_header_t   *pPayload = NULL;
  qurt_elite_bufmgr_node_t  bufMgrNode;
  elite_msg_any_t           cmd;
  int                       nActualSize;
  elite_svc_handle_t  *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);

  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Enter: afe_svc_cdc_handler_deinit");

#ifdef CDC_DEV_SLIMBUS
  if (NULL != cdc_handler_global_ptr->sb_cfg.p_sb_driver)
  {
    afe_slimbus_deinit_dev_handle(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
    cdc_handler_global_ptr->sb_cfg.p_sb_driver = NULL;
  }
#endif // CDC_DEV_SLIMBUS

  //Prepare & send termination commands and messages
  if (0 != cdc_svc_hptr->threadId)
  {
    // send terminate message to worker loop
    if (ADSP_SUCCEEDED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &bufMgrNode, &nActualSize)))
    {
      pPayload = (elite_msg_custom_afe_header_t*) bufMgrNode.pBuffer;
      pPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
      pPayload->afe_id = 0;
      pPayload->client_token = 0;
      pPayload->response_result = 0;
      pPayload->sec_op_code = ELITEMSG_CUSTOM_TERMINATE_THREAD;
      pPayload->responseq_ptr = NULL;
      cmd.unOpCode = ELITE_CUSTOM_MSG;
      cmd.pPayload = (void*) pPayload;
      if (ADSP_EOK != (result = qurt_elite_queue_push_back(cdc_svc_hptr->cmdQ, (uint64_t*) &cmd)))
      {
        elite_msg_return_payload_buffer(&cmd);
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to kill cdc handler thread");
      }
      else
      {
        // wait for the codec interrupt hander thread to join
        qurt_elite_thread_join(cdc_svc_hptr->threadId, &result);
        if (ADSP_FAILED(result))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed while joining CDC Handler thread: %d", result);
          result = ADSP_EFAILED;
        }
      }
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to kill cdc handler thread");
    }
  }

  // destroy the IST signal
  if (NULL != cdc_handler_global_ptr->ist_signal_ptr)
  {
    qurt_elite_signal_destroy(cdc_handler_global_ptr->ist_signal_ptr);
  }

  // destroy the cmdQ
  if (NULL != cdc_svc_hptr->cmdQ)
  {
    qurt_elite_queue_destroy(cdc_svc_hptr->cmdQ);
  }

  // destroy the channel
  qurt_elite_channel_destroy(&cdc_handler_global_ptr->channel);

  // destory the cdc mutex
  qurt_elite_mutex_destroy(&(cdc_handler_global_ptr->cdc_mutex));

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Exit: afe_svc_cdc_handler_deinit");

  return ADSP_EOK;
}

ADSPResult afe_svc_cdc_handler_init(void)
{
  ADSPResult result = ADSP_EOK;
  elite_svc_handle_t  *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);

  cdc_handler_global_ptr =  &cdc_handler_global;

  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Entered: afe_svc_cdc_handler_init");

  // initialize the global struct
  memset(cdc_handler_global_ptr, 0, sizeof(afe_cdc_handler_t));
  memset(cdc_svc_hptr, 0, sizeof(elite_svc_handle_t));

  //Initialise AANC HW version
  cdc_handler_global_ptr->aanc_hw_version =  AFE_AANC_HW_BLOCK_VERSION_1;

  cdc_handler_global_ptr->is_codec_int_registered = FALSE;

  //Initialize Codec Reg Slimbus Logical Address Offset
  cdc_handler_global_ptr->sb_cfg.reg_prop.slimbus_la_offset = AFE_CDC_SLIMBUS_LA_OFFSET_DEFAULT;

  // initialize the cdc mutex
  qurt_elite_mutex_init(&(cdc_handler_global_ptr->cdc_mutex));

  // initialize the channel
  qurt_elite_channel_init(&cdc_handler_global_ptr->channel);

  // create the cmd queue
  if (ADSP_FAILED(result = (qurt_elite_queue_create((char *)"AfeSvcCdcCmdQ", AFE_MAX_CMD_Q_ELEMENTS, &cdc_svc_hptr->cmdQ))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfeSvcCdcCmdQ Creation Failed");
    goto _init_bail_out;
  }

  // add the cmdQ to the channel
  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(cdc_handler_global_ptr->channel), cdc_svc_hptr->cmdQ, AFE_SVC_CDC_HANDLER_CMD_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add AfeSvcCdcCmdQ to channel: %d", result);
    goto _init_bail_out;
  }

  // create the signal for codec interrupt
  if (ADSP_FAILED(result = qurt_elite_signal_create(&cdc_handler_global_ptr->ist_signal_ptr)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create ist signal: %d", result);
    goto _init_bail_out;
  }

  if (ADSP_FAILED(result = qurt_elite_channel_add_signal(&cdc_handler_global_ptr->channel, cdc_handler_global_ptr->ist_signal_ptr, AFE_SVC_CDC_HANDLER_IST_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add ist signal to channel: %d", result);
    goto _init_bail_out;
  }

  // create codec handler sw thread
  if (ADSP_FAILED(result = qurt_elite_thread_launch(&(cdc_svc_hptr->threadId),
                                                    (char *)"CodecIntHldr", NULL,
                                                    AFE_SVC_CDC_HANDLER_STACK_SIZE,
                                                    elite_get_thrd_prio(ELITETHREAD_STAT_AFE_CDC_INT_SVC_PRIO_ID),
                                                    afe_svc_cdc_handler_workloop,
                                                    (void *)NULL,
                                                    QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to launch Codec handler thread %d", result);
    goto _init_bail_out;
  }

#ifdef CDC_DEV_SLIMBUS

  //TODO:: Need to remove this code once APPS sends the values using AFE_PARAM_ID_CDC_REG_CFG.

  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0].reg_bit_width = 0x8;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0].reg_field_bit_mask = 0xFF;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0].reg_addr = 0x100;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0].reg_offset_scale = 0x4;

  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1].reg_bit_width = 0x8;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1].reg_field_bit_mask = 0xFF;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1].reg_addr = 0x101;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1].reg_offset_scale = 0x4;

  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0].reg_bit_width = 0x8;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0].reg_field_bit_mask = 0xFF;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0].reg_addr = 0x180;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0].reg_offset_scale = 0x4;

  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1].reg_bit_width = 0x8;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1].reg_field_bit_mask = 0xFF;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1].reg_addr = 0x181;
  cdc_handler_global_ptr->reg_cfg[AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1].reg_offset_scale = 0x4;

#endif // CDC_DEV_SLIMBUS

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Exit: afe_svc_cdc_handler_init");

  return ADSP_EOK;

  _init_bail_out:
  afe_svc_cdc_handler_deinit();
  return ADSP_EFAILED;
}

/*
  This function is the register and deregister for cdc interrupt

  @param[in]     cdc_client_info_ptr: ptr to the codec int handler client info
  @param[in]     is_it_register: whether it is to register or deregister

  @return
  ADSPResult

  @dependencies
  None.
 */
ADSPResult afe_cdc_intr_reg_handler(cdc_client_info_t *cdc_client_info_ptr, bool_t is_it_register)
{
  ADSPResult result = ADSP_EOK;
  uint32_t payload_size;
  elite_msg_any_t msg, msg_ack;
  elite_msg_custom_afe_cdc_int_info_t *payload_ptr = NULL;
  elite_svc_handle_t *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);
  qurt_elite_queue_t *resp_q = NULL;
  qurt_elite_channel_t channel;

  memset(&channel, 0, sizeof(channel));

  payload_size = sizeof(elite_msg_custom_afe_cdc_int_info_t);

  qurt_elite_channel_init(&channel);

  // create the response queue
  if(ADSP_FAILED(
      result = (qurt_elite_queue_create((char * )"AfeCdcRespQ", AFE_MAX_CMD_Q_ELEMENTS, &resp_q))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfeCdcRespQ Creation Failed");
    goto __bail_out;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "resp_qptr 0x%lx", (uint32_t)resp_q);

  // add the resp Q to the channel
  if(ADSP_FAILED(result = qurt_elite_channel_addq(&channel, resp_q, AFE_CDC_RESP_Q_MASK)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add AfeCdcRespQ to channel: %d", result);
    goto __bail_out;
  }

  /* Get the message buffer from buffer manager */
  if(ADSP_EOK
      != (result = elite_msg_create_msg(&msg, &payload_size, ELITE_CUSTOM_MSG, resp_q, 0, ADSP_EOK)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to create EliteMsg for interrupt registration");
    goto __bail_out;
  }

  /* Populate the message payload */
  payload_ptr = (elite_msg_custom_afe_cdc_int_info_t *)msg.pPayload;

  /* Copy the cilent info into the payload */
  memscpy(&payload_ptr->client_info, sizeof(cdc_client_info_t), cdc_client_info_ptr,
          sizeof(cdc_client_info_t));

  /* Populate the secondary opcode */
  if(TRUE == is_it_register)
  {
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_CDC_INT_REGISTER_REQ;
  }
  else
  {
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_CDC_INT_DEREGISTER_REQ;
  }

  /* Push the msg payload into codec service command queue */
  if(ADSP_FAILED(result = qurt_elite_queue_push_back(cdc_svc_hptr->cmdQ, (uint64_t * )&msg)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to send interrupt registration msg, result: 0x%x", result);
    goto __bail_out_buffer;
  }

  /* Wait for ACK  from codec intr handler */
  if(ADSP_FAILED(result = elite_svc_wait_for_ack_and_get_msg(&msg, &msg_ack)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Wait for ack failed for codec intr registration");
    goto __bail_out_buffer;
  }

  payload_ptr = (elite_msg_custom_afe_cdc_int_info_t *)msg_ack.pPayload;

  if(TRUE == is_it_register)
  {
    if(0 != payload_ptr->client_info.client_handle)
    {
      /* Update the client_handle */
      cdc_client_info_ptr->client_handle = payload_ptr->client_info.client_handle;

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Successfully registered interrupt idx: 0x%x, client_handle: 0x%x",
            cdc_client_info_ptr->int_index, cdc_client_info_ptr->client_handle);
    }
    else
    {
      if (FALSE == cdc_handler_global_ptr->is_cdc_reg_cfg_init_received )
      {
        result = ADSP_EOK;
        MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "Not registering with codec interrupts, codec reg init not received: %d",\
              cdc_handler_global_ptr->is_cdc_reg_cfg_init_received );
      }
      else
      {
        result = ADSP_EFAILED;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register interrupt idx: 0x%x", cdc_client_info_ptr->int_index);
      }
    }
  }
  else /* Deregister */
  {
    if(0 == payload_ptr->client_info.client_handle)
    {
      /* Update the client_handle */
      cdc_client_info_ptr->client_handle = 0;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Successfully de-registered interrupt idx: 0x%x", cdc_client_info_ptr->int_index);
    }
    else
    {
      result = ADSP_EFAILED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-register interrupt idx: 0x%x", cdc_client_info_ptr->int_index);
    }
  }

  __bail_out_buffer:
  elite_msg_return_payload_buffer(&msg);

  __bail_out:
  // destroy the resp Q
  if(resp_q)
  {
    qurt_elite_queue_destroy(resp_q);
  }
  // destroy the channel
  qurt_elite_channel_destroy(&channel);

  return result;
}


ADSPResult afe_svc_check_codec_interrupts_supported(void)
{
  ADSPResult result= ADSP_EOK;

  //is_cdc_reg_cfg_init_received = FALSE only when  WCDxxxx codec is not connected to MSM.
  //If WCDxxxx is connected to MSM then only LA/WP drivers will send AFE_PARAM_ID_CDC_REG_CFG_INIT during boot up, otherwise they wont send
  //codec related register addresses. We are skipping the register/deregister of codec interrupts when WCD codec  is not present.

  if(FALSE == cdc_handler_global_ptr->is_cdc_reg_cfg_init_received)
  {
    MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "Codec interrupts not supported for this target" );
    result = ADSP_EUNSUPPORTED;
  }
  else
  {
    result = ADSP_EOK;
  }

  return result;

}
