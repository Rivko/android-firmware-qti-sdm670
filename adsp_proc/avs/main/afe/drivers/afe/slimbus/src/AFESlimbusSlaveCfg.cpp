/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusSlaveCfg.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusSlaveCfg.cpp

DESCRIPTION: AFE Slimbus Slave device port configuration related routines

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusSlaveCfg.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
03-20-2011  mspk  Initial Draft


============================================================================*/
#include "AFECodecHandler.h"
#include "AFESlimbusSlaveCfg_i.h"
#include "ddislimbus.h"

/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_slimbus_slave_port_config(afe_slimbus_state_struct_t *p_dev_state, int16_t multi_ch_grouping)
{
  ADSPResult           result = ADSP_EOK;
  DalDeviceHandle       *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
  uint16_t        slave_dev_dir, slave_port_id, slave_port_idx = 0, offset = 0;
  uint8_t         payload;
  SlimBusResourceHandle   h_slave_port = 0;
  afe_sb_cfg_info_t *cfg_ptr = &p_dev_state->afe_sb_cfg;
  cdc_reg_dev_cfg_t *slave_port_watermark_reg_cfg = NULL;
        cdc_reg_dev_cfg_t *slave_port_enable_reg_cfg  = NULL;
  cdc_reg_dev_cfg_t *multi_chan_0_reg_cfg  = NULL;
  cdc_reg_dev_cfg_t *multi_chan_1_reg_cfg  = NULL;
  
  uint16_t       max_num_of_slave_ports;
  uint32_t             multi_chan_cfg_reg_addr, slave_port_watermark_reg_cfg_addr, slave_port_enable_reg_addr;
  uint16_t   payload_tx_0 = 0, payload_tx_1 = 0, payload_rx_0 = 0,payload_rx_1 = 0;
  bool_t       ve_write_mode = FALSE;

  if(0 == cfg_ptr->num_slave_ports )
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Num stream is 0, nothing to config");
    return ADSP_EOK;
  }

  if (SLIMBUS_PORT_SOURCE_FLOW == p_dev_state->direction)
  {  
    slave_dev_dir = SLIMBUS_PORT_SINK_FLOW;
    cfg_ptr->slave_port_watermark_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_PORT_RX_WATERMARK_n);
    cfg_ptr->slave_port_enable_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_PORT_RX_ENABLE_n);
    cfg_ptr->slave_port_multi_chan_0_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0);
    cfg_ptr->slave_port_multi_chan_1_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1);
    offset = cdc_handler_global_ptr->sb_cfg.rx_slave_port_offset;
    max_num_of_slave_ports = SB_PGD_MAX_NUMBER_OF_RX_SLAVE_DEV_PORTS;
  }
  else
  {
    slave_dev_dir = SLIMBUS_PORT_SOURCE_FLOW;
    cfg_ptr->slave_port_watermark_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_PORT_TX_WATERMARK_n);
    cfg_ptr->slave_port_enable_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_PORT_TX_ENABLE_n);
    cfg_ptr->slave_port_multi_chan_0_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0);
    cfg_ptr->slave_port_multi_chan_1_reg_cfg = afe_cdc_get_dev_cfg_ptr(AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1);
    offset = cdc_handler_global_ptr->sb_cfg.tx_slave_port_offset;
    max_num_of_slave_ports = SB_PGD_MAX_NUMBER_OF_TX_SLAVE_DEV_PORTS;
  }
  
  slave_port_watermark_reg_cfg = cfg_ptr->slave_port_watermark_reg_cfg;
  slave_port_enable_reg_cfg    = cfg_ptr->slave_port_enable_reg_cfg ;
  multi_chan_0_reg_cfg         =  cfg_ptr->slave_port_multi_chan_0_reg_cfg;
  multi_chan_1_reg_cfg         =  cfg_ptr->slave_port_multi_chan_1_reg_cfg;
  cfg_ptr->slave_dev.slimbus_la_offset = cdc_handler_global_ptr->sb_cfg.reg_prop.slimbus_la_offset;


  for (slave_port_idx=0; slave_port_idx < cfg_ptr->num_slave_ports; slave_port_idx++)
  {
    slave_port_id = cfg_ptr->slave_port_mapping[slave_port_idx];
    if (slave_port_id > (max_num_of_slave_ports-1))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SLIMbus: invalid salve port id: %d", slave_port_id);
      return ADSP_EFAILED;
    }
    // getting absolute slave_port_id value
    slave_port_id += offset;

    // look for the valid port range and chose the payload accordingly
    if (slave_port_id <= SB_PGD_TX_PORT_MULTI_CHANNEL_0_END_PORT_ID)
    {
      payload_tx_0 = payload_tx_0 | (1 << slave_port_id);
    }
    else if (slave_port_id <= SB_PGD_TX_PORT_MULTI_CHANNEL_1_END_PORT_ID)
    {
      payload_tx_1 = payload_tx_1 | (1 << (slave_port_id - SB_PGD_TX_PORT_MULTI_CHANNEL_1_START_PORT_ID));
    }
    else if (slave_port_id <= SB_PGD_RX_PORT_MULTI_CHANNEL_0_END_PORT_ID)
    {
      payload_rx_0 = payload_rx_0  | (1 << (slave_port_id - SB_PGD_RX_PORT_MULTI_CHANNEL_0_START_PORT_ID));
    }
    else if (slave_port_id <= SB_PGD_RX_PORT_MULTI_CHANNEL_1_END_PORT_ID)
    {
      payload_rx_1 = payload_rx_1  | (1 << (slave_port_id - SB_PGD_RX_PORT_MULTI_CHANNEL_1_START_PORT_ID));
    }
    else
    {
      return ADSP_EFAILED;
    }
  }

  for (slave_port_idx = 0; slave_port_idx < cfg_ptr->num_slave_ports; slave_port_idx++)
  {
    // get the slave port to be configured
    slave_port_id = cfg_ptr->slave_port_mapping[slave_port_idx];

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "slave_port_id: 0x%x", slave_port_id);
  
    // get the slave port handle
    if (DAL_SUCCESS != (result = DalSlimBus_GetSlavePortHandle(p_core_driver, cfg_ptr->slave_dev_pgd_la, slave_port_id+offset, &h_slave_port)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get slave port handle, 0x%x", result);
      return ADSP_EFAILED;
    }

    if (SLIMBUS_PORT_SOURCE_FLOW == slave_dev_dir)
    {
      multi_chan_cfg_reg_addr = AFE_CRC_GET_ABSOLUTE_ADDRESS(multi_chan_0_reg_cfg->reg_addr,\
                                   multi_chan_0_reg_cfg->reg_offset_scale,\
                                   slave_port_id);
      payload = payload_tx_0;
      // configure the slave port with multi channel grouping configuration
      result = afe_sb_write_register((void *)p_dev_state->p_sb_driver,
        &cfg_ptr->slave_dev,
        multi_chan_cfg_reg_addr,
        &payload,
        0xFF,
        multi_chan_0_reg_cfg->reg_bit_width,
        ve_write_mode);

      multi_chan_cfg_reg_addr = AFE_CRC_GET_ABSOLUTE_ADDRESS(multi_chan_1_reg_cfg->reg_addr,\
                                   multi_chan_1_reg_cfg->reg_offset_scale,\
                                   slave_port_id);
      payload = payload_tx_1;
      result = afe_sb_write_register((void *)p_dev_state->p_sb_driver,
        &cfg_ptr->slave_dev,
        multi_chan_cfg_reg_addr,
        &payload,
        0xFF,
        multi_chan_1_reg_cfg->reg_bit_width,
        ve_write_mode);
    }
    else
    {
      multi_chan_cfg_reg_addr = AFE_CRC_GET_ABSOLUTE_ADDRESS(multi_chan_0_reg_cfg->reg_addr,\
                                   multi_chan_0_reg_cfg->reg_offset_scale,\
                                     slave_port_id);

      payload = payload_rx_0;
      result = afe_sb_write_register((void *)p_dev_state->p_sb_driver,
          &cfg_ptr->slave_dev,
          multi_chan_cfg_reg_addr,
          &payload,
          0xFF,
          multi_chan_0_reg_cfg->reg_bit_width,
          ve_write_mode);

      multi_chan_cfg_reg_addr = AFE_CRC_GET_ABSOLUTE_ADDRESS(multi_chan_1_reg_cfg->reg_addr,\
                                   multi_chan_1_reg_cfg->reg_offset_scale,\
                                   slave_port_id);
      payload = payload_rx_1;
      // configure the slave port with multi channel grouping configuration
      result = afe_sb_write_register((void *)p_dev_state->p_sb_driver,
          &cfg_ptr->slave_dev,
          multi_chan_cfg_reg_addr,
          &payload,
          0xFF,
          multi_chan_1_reg_cfg->reg_bit_width,
          ve_write_mode);

    }

    slave_port_watermark_reg_cfg_addr = (slave_port_watermark_reg_cfg->reg_addr + slave_port_watermark_reg_cfg->reg_offset_scale*slave_port_id);
    slave_port_enable_reg_addr = (slave_port_enable_reg_cfg->reg_addr + slave_port_enable_reg_cfg->reg_offset_scale*slave_port_id);

    // set the water mark for slave_port (this port id is relative and not absolute number which can include offset)
    payload = SLAVE_PORT_WATER_MARK_VALUE;
    result = afe_sb_update_register((void *)p_dev_state->p_sb_driver,
        &cfg_ptr->slave_dev,
        slave_port_watermark_reg_cfg_addr,
        &payload,
        slave_port_watermark_reg_cfg->reg_field_bit_mask,
        slave_port_enable_reg_cfg->reg_bit_width,
        ve_write_mode);

    // all writes are NON blocking except for last channel, last Value element write
    if (slave_port_idx == (cfg_ptr->num_slave_ports-1))
    {
      ve_write_mode = TRUE;
    }

    // set the water mark for slave_port (this port id is relative and not absolute number which can include offset)
    payload = SLAVE_PORT_ENABLE;
    result = afe_sb_update_register((void *)p_dev_state->p_sb_driver,
          &cfg_ptr->slave_dev,
          slave_port_enable_reg_addr,
          &payload,
          slave_port_enable_reg_cfg->reg_field_bit_mask,
          slave_port_enable_reg_cfg->reg_bit_width,
          ve_write_mode);

    if (DAL_SUCCESS != (result = DalSlimBus_ConnectPortToChannel(p_core_driver, p_dev_state->h_data_channel[slave_port_idx], (SlimBusPortFlowType)slave_dev_dir, h_slave_port)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to connect slave port to the channel, 0x%x", result);
      return ADSP_EFAILED;
    }
  }

  return ADSP_EOK;
}
