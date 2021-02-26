/*========================================================================
   This file contains functions to set the parameters for different modules.

Copyright (c) 2009-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/inc/AFECodecHandler.h#1 $
 ====================================================================== */
#ifndef _AFE_CDC_HANDLER_I_H_
#define _AFE_CDC_HANDLER_I_H_

/*==========================================================================
  Include Files
========================================================================== */
#include "AFEInternal.h"
#include "AFECodecService.h"
#include "AFEHwMadDriver.h"
#include "EliteMsg_AfeCustom.h"
#include "AFESlimbusDriver.h"
#include "AFEDeviceDriver.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef SIM
#undef CDC_DEV_SLIMBUS
#define CDC_DEV_SLIMBUS
#endif // SIM

/*==========================================================================
  Macro definitions
========================================================================== */
#define AFE_SVC_CDC_HANDLER_CMD_MASK    (1<<31)
#define AFE_SVC_CDC_HANDLER_IST_MASK    (1<<30)
#define AFE_SVC_CDC_HANDLER_STACK_SIZE  4096

// maximum number of codec reg fields
#define AFE_CDC_MAX_DEV_CFG_REG_FIELDS   100

// maximum number of supported clients for the given interrupt
#define AFE_CDC_INT_MAX_CLIENTS       2

// offset multiplier for client handle
#define AFE_CDC_CLIENT_HANDLE_OFFSET_MULT   0x100

// converting client handle from relative to absolute index
#define CLIENT_HANDLE_CONVERT_REL_TO_ABS(client_handle, int_idx) (client_handle + AFE_CDC_CLIENT_HANDLE_OFFSET_MULT * int_idx + 1)

// converting client handle from absolute index to relative
#define CLIENT_HANDLE_CONVERT_ABS_TO_REL(client_handle, int_idx) (client_handle - AFE_CDC_CLIENT_HANDLE_OFFSET_MULT * int_idx - 1)

// slimbus slave device config
typedef struct cdc_sb_slave_dev_info
{
  uint16_t tx_slave_port_offset;
  uint16_t rx_slave_port_offset;
  void *p_sb_driver;
  uint32_t slave_dev_minor_version;
  uint32_t device_enum_addr_lsw;
  uint32_t device_enum_addr_msw;
  afe_reg_prop reg_prop;
} cdc_sb_slave_dev_info_t;

// codec interrupt handle info
typedef struct client_info_list
{
  uint16_t num_clients;
  uint16_t postpone_int_unmask;
  uint16_t client_signaled[AFE_CDC_INT_MAX_CLIENTS];
  uint16_t client_is_used[AFE_CDC_INT_MAX_CLIENTS];
  cdc_client_info_t client_info[AFE_CDC_INT_MAX_CLIENTS];
  cdc_reg_dev_cfg_t *int_routing_reg_cfg_ptr;
  cdc_reg_dev_cfg_t *int_mask_reg_cfg_ptr;
  cdc_reg_dev_cfg_t *int_status_reg_cfg_ptr;
  cdc_reg_dev_cfg_t *int_clear_reg_cfg_ptr;
} client_info_list_t;

// codec handler global struct def
typedef struct afe_cdc_handler
{
  bool_t is_cdc_reg_cfg_init_received;
  bool_t is_codec_int_registered;
  uint16_t intr_id;
  qurt_elite_signal_t *ist_signal_ptr;
  qurt_elite_channel_t channel;
  qurt_elite_mutex_t cdc_mutex;
  uint32_t sw_int_mask;
  client_info_list_t client_info_list[AFE_CDC_INT_INDEX_SIZE];
  cdc_reg_dev_cfg_t reg_cfg[AFE_CDC_MAX_DEV_CFG_REG_FIELDS];
  cdc_sb_slave_dev_info_t sb_cfg;
  bool_t cdc_pwr_cntl_enable;
  gpio_int_properties_t cdc_gpio_int_prop;
  uint32_t aanc_hw_version;
  uint32_t clip_num_banks;
  uint32_t clip_bank_map[AFE_CLIP_MAX_BANKS];
} afe_cdc_handler_t;

/*==========================================================================
  Globals
========================================================================== */
extern afe_cdc_handler_t *cdc_handler_global_ptr;
extern aud_stat_afe_svc_t *afe_svc_global_ptr;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AFE_CDC_HANDLER_I_H_
